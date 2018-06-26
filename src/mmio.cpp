#include "mmio.h"
#include "apbp.h"
#include "memory_interface.h"
#include <functional>
#include <string>
#include <vector>

namespace Teakra {

auto NoSet(const std::string& debug_string) {
    return [debug_string](u16) {printf("Warning: NoSet on %s\n", debug_string.data());};
}
auto NoGet(const std::string& debug_string) {
    return [debug_string]()->u16 {printf("Warning: NoGet on %s\n", debug_string.data()); return 0;};
}

struct BitFieldSlot {
    unsigned pos;
    unsigned length;
    std::function<void(u16)> set;
    std::function<u16(void)> get;

    static BitFieldSlot RefSlot(unsigned pos, unsigned length, u16& var) {
        BitFieldSlot slot {pos, length, {}, {}};
        slot.set = [&var](u16 value) {var = value;};
        slot.get = [&var]()->u16 {return var;};
        return slot;
    }
};

struct Cell {
    std::function<void(u16)> set;
    std::function<u16(void)> get;

    Cell(std::function<void(u16)> set, std::function<u16(void)> get):
        set(std::move(set)), get(std::move(get)) {}
    Cell() {
        std::shared_ptr<u16> storage = std::make_shared<u16>(0);
        set = [storage](u16 value) {*storage = value;};
        get = [storage]() ->u16 {return *storage;};
    }
    static Cell ConstCell(u16 constant) {
        Cell cell({}, {});
        cell.set = NoSet("");
        cell.get = [constant]()->u16 {return constant;};
        return cell;
    }
    static Cell RefCell(u16& var) {
        Cell cell({}, {});
        cell.set = [&var](u16 value) {var = value;};
        cell.get = [&var]()->u16 {return var;};
        return cell;
    }

    static Cell MirrorCell(Cell* mirror) {
        Cell cell({}, {});
        cell.set = [mirror](u16 value){mirror->set(value);};
        cell.get = [mirror]()->u16 {return mirror->get();};
        return cell;
    }

    static Cell BitFieldCell(const std::vector<BitFieldSlot>& slots) {
        Cell cell({}, {});
        std::shared_ptr<u16> storage = std::make_shared<u16>(0);
        cell.set = [storage, slots](u16 value) {
            for (const auto& slot : slots) {
                if (slot.set) {
                    slot.set((value >> slot.pos) & ((1 << slot.length) - 1));
                }
            }
            *storage = value;
        };
        cell.get = [storage, slots]() -> u16 {
            u16 value = *storage;
            for (const auto& slot : slots) {
                if (slot.get) {
                    value &= ~(((1 << slot.length) - 1) << slot.pos);
                    value |= slot.get() << slot.pos;
                }
            }
            return value;
        };
        return cell;
    }
};

class MMIORegion::Impl {
public:
    std::array<Cell, 0x800> cells{};
};

MMIORegion::MMIORegion(
    MemoryInterfaceUnit& miu,
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

    impl->cells[0x01A] = Cell::ConstCell(0xC902); // chip detect

    // Timer
    for (unsigned i = 0; i < 1; ++i) {
        impl->cells[0x20 + i * 0x10] = Cell::BitFieldCell({ // TIMERx_CFG
            BitFieldSlot{0, 2, {}, {}}, // TS
            BitFieldSlot{2, 3, {}, {}}, // CM
            BitFieldSlot{6, 1, {}, {}}, // TP
            BitFieldSlot{7, 1, {}, {}}, // CT
            BitFieldSlot{8, 1, {}, {}}, // PC
            BitFieldSlot{9, 1, {}, {}}, // MU
            BitFieldSlot{10, 1, {}, {}}, // RES
            BitFieldSlot{11, 1, {}, {}}, // BP
            BitFieldSlot{12, 1, {}, {}}, // CS
            BitFieldSlot{13, 1, {}, {}}, // GP
            BitFieldSlot{14, 2, {}, {}}, // TM
        });

        impl->cells[0x22 + i * 0x10] = Cell(); // TIMERx_EW
        impl->cells[0x24 + i * 0x10] = Cell(); // TIMERx_SCL
        impl->cells[0x26 + i * 0x10] = Cell(); // TIMERx_SCH
        impl->cells[0x28 + i * 0x10] = Cell(); // TIMERx_CCL
        impl->cells[0x2A + i * 0x10] = Cell(); // TIMERx_CCH
        impl->cells[0x2C + i * 0x10] = Cell(); // TIMERx_SPWMCL
        impl->cells[0x2E + i * 0x10] = Cell(); // TIMERx_SPWMCH
    }

    // APBP
    for (unsigned i = 0; i < 3; ++i) {
        impl->cells[0x0C0 + i * 4].set = std::bind(&Apbp::SendData, &apbp_from_dsp, i, _1);
        impl->cells[0x0C0 + i * 4].get = NoGet("Apbp::SendData" + std::to_string(i));
        impl->cells[0x0C2 + i * 4].set = NoSet("Apbp::RecvData" + std::to_string(i));
        impl->cells[0x0C2 + i * 4].get = std::bind(&Apbp::RecvData, &apbp_from_cpu, i);
    }
    impl->cells[0x0CC].set = std::bind(&Apbp::SetSemaphore, &apbp_from_dsp, _1);
    impl->cells[0x0CC].get = std::bind(&Apbp::GetSemaphore, &apbp_from_dsp);
    impl->cells[0x0CE].set = std::bind(&Apbp::MaskSemaphore, &apbp_from_cpu, _1);
    impl->cells[0x0CE].get = std::bind(&Apbp::GetSemaphoreMask, &apbp_from_cpu);
    impl->cells[0x0D0].set = std::bind(&Apbp::ClearSemaphore, &apbp_from_cpu, _1);
    impl->cells[0x0D0].get = NoGet("Apbp::ClearSemaphore");
    impl->cells[0x0D2].set = NoSet("Apbp::GetSemaphore");
    impl->cells[0x0D2].get = std::bind(&Apbp::GetSemaphore, &apbp_from_cpu);
    // impl->cells[0x0D4]; // interrupt mask?
    impl->cells[0x0D6] = Cell::BitFieldCell({
        BitFieldSlot{5, 1, {}, [this]()->u16{
            return this->apbp_from_dsp.IsDataReady(0);
        }},
        BitFieldSlot{6, 1, {}, [this]()->u16{
            return this->apbp_from_dsp.IsDataReady(1);
        }},
        BitFieldSlot{7, 1, {}, [this]()->u16{
            return this->apbp_from_dsp.IsDataReady(2);
        }},
        BitFieldSlot{8, 1, {}, [this]()->u16{
            return this->apbp_from_cpu.IsDataReady(0);
        }},
        BitFieldSlot{9, 1, {}, [this]()->u16{
            return this->apbp_from_cpu.IsSemaphoreSignaled();
        }},
        BitFieldSlot{12, 1, {}, [this]()->u16{
            return this->apbp_from_cpu.IsDataReady(1);
        }},
        BitFieldSlot{13, 1, {}, [this]()->u16{
            return this->apbp_from_cpu.IsDataReady(2);
        }},
    });

    // MIU
    // impl->cells[0x100]; // MIU_WSCFG0
    // impl->cells[0x102]; // MIU_WSCFG1
    // impl->cells[0x104]; // MIU_Z0WSCFG
    // impl->cells[0x106]; // MIU_Z1WSCFG
    // impl->cells[0x108]; // MIU_Z2WSCFG
    // impl->cells[0x10C]; // MIU_Z3WSCFG
    impl->cells[0x10E] = Cell::RefCell(miu.x_page); // MIU_XPAGE
    impl->cells[0x110] = Cell::RefCell(miu.y_page); // MIU_YPAGE
    impl->cells[0x112] = Cell::RefCell(miu.z_page); // MIU_ZPAGE
    impl->cells[0x114] = Cell::BitFieldCell({ // MIU_PAGE0CFG
        BitFieldSlot::RefSlot(0, 6, miu.x_size[0]),
        BitFieldSlot::RefSlot(8, 6, miu.y_size[0]),
    });
    impl->cells[0x116] = Cell::BitFieldCell({ // MIU_PAGE1CFG
        BitFieldSlot::RefSlot(0, 6, miu.x_size[1]),
        BitFieldSlot::RefSlot(8, 6, miu.y_size[1]),
    });
    // impl->cells[0x118]; // MIU_OFFPAGECFG
    impl->cells[0x11A] = Cell::BitFieldCell({
        BitFieldSlot{0, 1, {}, {}}, // PP
        BitFieldSlot{1, 1, {}, {}}, // TESTP
        BitFieldSlot{2, 1, {}, {}}, // INTP
        BitFieldSlot{4, 1, {}, {}}, // ZSINGLEP
        BitFieldSlot::RefSlot(6, 1, miu.page_mode), // PAGEMODE
    });
    // impl->cells[0x11C]; // MIU_DLCFG
    impl->cells[0x11E] = Cell::RefCell(miu.mmio_base); // MIU_MMIOBASE
    // impl->cells[0x120]; // MIU_OBSCFG
    // impl->cells[0x122]; // MIU_POLARITY

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
    // impl->cells[0x20E]; // polarity for each interrupt?
    // impl->cells[0x210]; // source type for each interrupt?
    for (unsigned i = 0; i < 16; ++i) {
        impl->cells[0x212 + i * 4] = Cell::BitFieldCell({
            BitFieldSlot::RefSlot(0, 2, icu.vector_high[i]),
            BitFieldSlot{15, 1, {}, {}}, // ?
        });
        impl->cells[0x214 + i * 4] = Cell::RefCell(icu.vector_low[i]);
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
