#include "mmio.h"
#include <functional>

auto NoSet = [](u16) {printf("Warning: NoSet\n");};
auto NoGet = []()->u16 {printf("Warning: NoGet\n"); return 0;};

struct Cell {
    std::function<void(u16)> set;
    std::function<u16(void)> get;
    Cell() {
        std::shared_ptr<u16> storage = std::make_shared<u16>(0);
        set = [storage](u16 value) {*storage = value;};
        get = [storage]() ->u16 {return *storage;};
    }
    Cell(u16 constant) {
        set = NoSet;
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

MMIORegion::MMIORegion(MIU& miu, ICU& icu) : impl(new Impl), miu(miu), icu(icu) {
    using namespace std::placeholders;

    impl->cells[0x01A] = Cell(0xC902); // chip detect

    // memory bank setter has a delay of about 1 cycle. Game usually has a nop after the write instruction
    impl->cells[0x10E].set = std::bind(&MIU::SetMemoryBank, &miu, 0, _1);
    impl->cells[0x10E].get = std::bind(&MIU::GetMemoryBank, &miu, 0);
    impl->cells[0x110].set = std::bind(&MIU::SetMemoryBank, &miu, 1, _1);
    impl->cells[0x110].get = std::bind(&MIU::GetMemoryBank, &miu, 1);
    impl->cells[0x114].set(0x1E20); // low = X space size, high = Y space size (both in 0x400)
    impl->cells[0x11A].set(0x0014); // bit 6 enables memory bank exchanging?
    impl->cells[0x11E].set = std::bind(&MIU::SetMMIOLocation, &miu, _1);
    impl->cells[0x11E].get = std::bind(&MIU::GetMMIOLocation, &miu);

    impl->cells[0x200].set = NoSet;
    impl->cells[0x200].get = std::bind(&ICU::GetRequest, &icu);
    impl->cells[0x202].set = std::bind(&ICU::Acknowledge, &icu, _1);
    impl->cells[0x202].get = NoGet;
    impl->cells[0x204].set = std::bind(&ICU::Trigger, &icu, _1);
    impl->cells[0x204].get = NoGet;
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
}

MMIORegion::~MMIORegion() = default;


u16 MMIORegion::Read(u16 addr) {
    u16 value = impl->cells[addr].get();
    printf(">>>>>>>>> MMIO Read  @%04X -> %04X\n", addr, value);
    return value;
}
void MMIORegion::Write(u16 addr, u16 value) {
    printf(">>>>>>>>> MMIO Write @%04X <- %04X\n", addr, value);
    impl->cells[addr].set(value);
}
