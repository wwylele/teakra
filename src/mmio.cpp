#include "mmio.h"
#include "apbp.h"
#include "memory_interface.h"
#include <functional>
#include <string>

namespace Teakra {

auto NoSet(const std::string& debug_string) {
    return [debug_string](u16) {printf("Warning: NoSet on %s\n", debug_string.data());};
}
auto NoGet(const std::string& debug_string) {
    return [debug_string]()->u16 {printf("Warning: NoGet on %s\n", debug_string.data()); return 0;};
}

struct Cell {
    std::function<void(u16)> set;
    std::function<u16(void)> get;
    Cell() {
        std::shared_ptr<u16> storage = std::make_shared<u16>(0);
        set = [storage](u16 value) {*storage = value;};
        get = [storage]() ->u16 {return *storage;};
    }
    Cell(u16 constant) {
        set = NoSet("");
        get = [constant]()->u16 {return constant;};
    }
    Cell(Cell* mirror) {
        set = [mirror](u16 value){mirror->set(value);};
        get = [mirror]()->u16 {return mirror->get();};
    }
};

class MMIORegion::Impl {
public:
    std::array<Cell, 0x800> cells{};
};

MMIORegion::MMIORegion(
    DataMemoryController& miu,
    ICU& icu,
    Apbp& apbp_from_cpu,
    Apbp& apbp_from_dsp
):
    impl(new Impl),
    miu(miu),
    icu(icu),
    apbp_from_cpu(apbp_from_cpu),
    apbp_from_dsp(apbp_from_dsp)
{
    using namespace std::placeholders;

    impl->cells[0x01A] = Cell(0xC902); // chip detect

    // APBP
    for (unsigned i = 0; i < 3; ++i) {
        impl->cells[0x0C0 + i * 4].set = std::bind(&Apbp::SendData, &apbp_from_dsp, i, _1);
        impl->cells[0x0C0 + i * 4].get = NoGet("Apbp::SendData" + std::to_string(i));
        impl->cells[0x0C2 + i * 4].set = NoSet("Apbp::RecvData" + std::to_string(i));
        impl->cells[0x0C2 + i * 4].get = std::bind(&Apbp::RecvData, &apbp_from_cpu, i);
    }
    impl->cells[0x0CC].set = std::bind(&Apbp::SetSemaphore, &apbp_from_dsp, _1);
    impl->cells[0x0CC].get = NoGet("Apbp::SetSemaphore");
    impl->cells[0x0CE].set = NoSet("Apbp::MaskSemaphore?");
    impl->cells[0x0CE].get = NoGet("Apbp::MaskSemaphore?");
    impl->cells[0x0D0].set = std::bind(&Apbp::ClearSemaphore, &apbp_from_cpu, _1);
    impl->cells[0x0D0].get = NoGet("Apbp::ClearSemaphore");
    impl->cells[0x0D2].set = NoSet("Apbp::GetSemaphore?");
    impl->cells[0x0D2].get = std::bind(&Apbp::GetSemaphore, &apbp_from_cpu);
    impl->cells[0x0D6].set = [this](u16 value){ apbp_flags = value; };
    impl->cells[0x0D6].get = [this](){
        u16 value = apbp_flags;
        if (this->apbp_from_cpu.IsDataReady(0))
            value |= 1 << 8;
        if (this->apbp_from_cpu.IsDataReady(1))
            value |= 1 << 12;
        if (this->apbp_from_cpu.IsDataReady(2))
            value |= 1 << 13;
        return value;
    };

    // MIU
    // memory bank setter has a delay of about 1 cycle. Game usually has a nop after the write instruction
    impl->cells[0x10E].set = std::bind(&DataMemoryController::SetMemoryBank, &miu, 0, _1);
    impl->cells[0x10E].get = std::bind(&DataMemoryController::GetMemoryBank, &miu, 0);
    impl->cells[0x110].set = std::bind(&DataMemoryController::SetMemoryBank, &miu, 1, _1);
    impl->cells[0x110].get = std::bind(&DataMemoryController::GetMemoryBank, &miu, 1);
    impl->cells[0x114].set(0x1E20); // low = X space size, high = Y space size (both in 0x400)
    impl->cells[0x11A].set(0x0014); // bit 6 enables memory bank exchanging?
    impl->cells[0x11E].set = std::bind(&DataMemoryController::SetMMIOLocation, &miu, _1);
    impl->cells[0x11E].get = std::bind(&DataMemoryController::GetMMIOLocation, &miu);

    // ICU
    impl->cells[0x200].set = NoSet("ICU::GetRequest");
    impl->cells[0x200].get = std::bind(&ICU::GetRequest, &icu);
    impl->cells[0x202].set = std::bind(&ICU::Acknowledge, &icu, _1);
    impl->cells[0x202].get = NoGet("ICU::Acknowledge");
    impl->cells[0x204].set = std::bind(&ICU::Trigger, &icu, _1);
    impl->cells[0x204].get = NoGet("ICU::Trigger");
    impl->cells[0x206].set = std::bind(&ICU::SetEnable, &icu, 0, _1);
    impl->cells[0x206].get = std::bind(&ICU::GetEnable, &icu, 0);
    impl->cells[0x208].set = std::bind(&ICU::SetEnable, &icu, 1, _1);
    impl->cells[0x208].get = std::bind(&ICU::GetEnable, &icu, 1);
    impl->cells[0x20A].set = std::bind(&ICU::SetEnable, &icu, 2, _1);
    impl->cells[0x20A].get = std::bind(&ICU::GetEnable, &icu, 2);
    impl->cells[0x20C].set = std::bind(&ICU::SetEnableVectored, &icu, _1);
    impl->cells[0x20C].get = std::bind(&ICU::GetEnableVectored, &icu);
    // 20E: some type bit for each
    // 210: some type bit for each
    for (unsigned i = 0; i < 16; ++i) {
        // 0x212 + i * 4 : ?
        impl->cells[0x214 + i * 4].set = std::bind(&ICU::SetVector, &icu, i, _1);
        impl->cells[0x214 + i * 4].get = std::bind(&ICU::GetVector, &icu, i);
    }

    // Audio
    for (unsigned i = 0; i < 2; ++i) {
        impl->cells[0x2CA + i * 0x80].get = []()->u16{return 0x0002;}; // hack
    }
}

MMIORegion::~MMIORegion() = default;


u16 MMIORegion::Read(u16 addr) {
    u16 value = impl->cells[addr].get();
    //if (addr != 0x02CA)
        printf(">>>>>>>>> MMIO Read  @%04X -> %04X\n", addr, value);
    return value;
}
void MMIORegion::Write(u16 addr, u16 value) {
    printf(">>>>>>>>> MMIO Write @%04X <- %04X\n", addr, value);
    impl->cells[addr].set(value);
}
} // namespace Teakra
