#include "mmio.h"
#include <functional>

struct Cell {
    std::function<void(u16)> set;
    std::function<u16(void)> get;
    Cell() {
        std::shared_ptr<u16> storage = std::make_shared<u16>(0);
        set = [storage](u16 value) {*storage = value;};
        get = [storage]() ->u16 {return *storage;};
    }
    Cell(u16 constant) {
        set = [](u16 value){};
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

MMIORegion::MMIORegion(MIU& miu) : impl(new Impl), miu(miu) {
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
