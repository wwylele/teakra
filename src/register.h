#pragma once

#include <algorithm>
#include <array>
#include <memory>
#include <vector>
#include "common_types.h"
#include "oprand.h"


struct RegisterState {
    void Reset() {
        pc = 0;

    }

    u32 pc = 0; // 18-bit, program counter

    u16 GetPcL() const {
        return pc & 0xFFFF;
    }
    u16 GetPcH() const {
        return (pc >> 16) & 0xFFFF;
    }
    void SetPC(u16 low, u16 high) {
        pc = (u32)low | ((u32)high << 16);
    }

    u16 dvm = 0; // data value match for breakpoints and trap
    u16 repc = 0; // rep loop counter
    bool rep = false; // true when in rep loop
    u16 mixp = 0;
    u16 sv = 0; // 16-bit two's complement shift value
    u16 sp = 0; // stack pointer

    std::array<u16, 8> r{}; // general registers

    // shadows for bank exchange;
    u16 r0b = 0, r1b = 0, r4b = 0, r7b = 0;

    // accumulators
    struct Accumulator {
        // 40-bit 2's comp on real TeakLite.
        // Use 64-bit 2's comp here. The upper 24 bits are always sign extension
        u64 value = 0;
    };
    std::array<Accumulator, 2> a;
    std::array<Accumulator, 2> b;

    // multiplication unit
    struct Product {
        u32 value = 0;
    };
    std::array<u16, 2> x{}; // factor
    std::array<u16, 2> y{}; // factor
    std::array<Product, 2> p; // product

    // step/modulo
    u16 stepi = 0, stepj = 0; // 7 bit 2's comp
    u16 modi = 0, modj = 0; // 9 bit
    u16 stepi0 = 0, stepj0 = 0; // alternative step

    // Shadows for bank exchange
    u16 stepib = 0, stepjb = 0;
    u16 modib = 0, modjb = 0;
    u16 stepi0b = 0, stepj0b = 0;

    // 1-bit flags
    u16 fz = 0, fm = 0, fn = 0, fv = 0, fc = 0, fe = 0;
    u16 fls = 0; // set on saturation
    u16 flv = 0; // latching fv
    u16 fr = 0;
    u16 fc1 = 0; // used by max||max||vtrshr?
    std::array<u16, 2> vtr{}; // fc/fc1 latching

    // interrupt pending bit
    std::array<u16, 3> ip{};
    u16 vip = 0;

    // interrupt enable bit
    std::array<u16, 3> im{};
    u16 vim = 0;

    // interrupt context switching bit
    std::array<u16, 3> ic{};
    u16 vic = 0;
    u16 nimc = 0;

    // interrupt enable master bit
    u16 ie = 0;

    // vectored(?) interrupt handler address;
    u32 viaddr = 0;

    u16 movpd = 0; // 2-bit, higher part of program address for movp/movd
    u16 bcn = 0; // 3-bit, nest loop counter
    u16 lp = 0; // 1-bit, set when in a loop
    std::array<u16, 2> sar{}; // sar[0]=1 disable saturation when read from acc; sar[1]=1 disable saturation when write to acc?
    u16 ym = 0; // 2-bit, modify y on multiplication
    u16 mod0_unk_const = 1; // 3-bit
    std::array<u16, 2> ps{}; // 2-bit, product shift mode
    // sign bit of p0/1. For signed multiplication and mov, this is set to equal bit 31
    // product shift and extension is signed according to this.
    std::array<u16, 2> psign{};
    u16 s = 0; // 1-bit, shift mode. 0 - arithmetic, 1 - logic
    std::array<u16, 2> ou{}; // user output pins
    std::array<u16, 2> iu{}; // user input pins
    u16 page = 0; // 8-bit, Higher part of MemImm8 address
    u16 bankstep = 0; // 1 bit. If set, stepi/j0 will be exchanged along with cfgi/j in banke
    u16 mod1_unk = 0; // 3-bit

    // m=0, ms=0: use stepi/j (no modulo)
    // m=1, ms=0: use stepi/j with modulo
    // m=0, ms=1: use stepi0/j0 (no modulo)
    // m=1, ms=1: use stepi/j  (no modulo)??
    std::array<u16, 8> m{};
    std::array<u16, 8> ms{};

    struct BlockRepeatFrame {
        u32 start = 0;
        u32 end = 0;
        u16 lc = 0;
    };

    std::array<BlockRepeatFrame, 4> bkrep_stack;
    u16& Lc() {
        if (lp)
            return bkrep_stack[bcn - 1].lc;
        return bkrep_stack[0].lc;
    }

    // 3 bits each
    // 0: +0
    // 1: +1
    // 2: -1
    // 3: +s
    // 4: +2
    // 5: -2
    // 6: +2 ?
    // 7: -2 ?
    std::array<u16, 4> arstep{}, arpstepi{}, arpstepj{};

    // 2 bits each
    // 0: +0
    // 1: +1
    // 2: -1
    // 3: -1 ?
    std::array<u16, 4> aroffset{}, arpoffseti{}, arpoffsetj{};

    // 3 bits each, represent r0~r7
    std::array<u16, 4> arrn{};

    // 2 bits each. for i represent r0~r4, for j represents r5~r7
    std::array<u16, 4> arprni{}, arprnj{};

    class ShadowRegister {
    public:
        ShadowRegister(u16& origin): origin(origin) {}
        void Store() {
            shadow = origin;
        }
        void Restore() {
            origin = shadow;
        }
    private:
        u16& origin;
        u16 shadow = 0;
    };

    class ShadowSwapRegister {
    public:
        ShadowSwapRegister(u16& origin): origin(origin) {}
        void Swap() {
            std::swap(origin, shadow);
        }
    private:
        u16& origin;
        u16 shadow = 0;
    };

    std::vector<ShadowRegister> shadow_registers {
        ShadowRegister(fls),
        ShadowRegister(flv),
        ShadowRegister(fe),
        ShadowRegister(fc),
        ShadowRegister(fv),
        ShadowRegister(fn),
        ShadowRegister(fm),
        ShadowRegister(fz),
        ShadowRegister(fc1),
        ShadowRegister(fr),
    };

    std::vector<ShadowSwapRegister> shadow_swap_registers {
        ShadowSwapRegister(movpd),
        ShadowSwapRegister(sar[0]),
        ShadowSwapRegister(sar[1]),
        ShadowSwapRegister(ym),
        ShadowSwapRegister(s),
        ShadowSwapRegister(ps[0]),
        ShadowSwapRegister(ps[1]),
        ShadowSwapRegister(page),
        ShadowSwapRegister(bankstep),
        ShadowSwapRegister(mod1_unk),
        ShadowSwapRegister(m[0]),
        ShadowSwapRegister(m[1]),
        ShadowSwapRegister(m[2]),
        ShadowSwapRegister(m[3]),
        ShadowSwapRegister(m[4]),
        ShadowSwapRegister(m[5]),
        ShadowSwapRegister(m[6]),
        ShadowSwapRegister(m[7]),
        ShadowSwapRegister(ms[0]),
        ShadowSwapRegister(ms[1]),
        ShadowSwapRegister(ms[2]),
        ShadowSwapRegister(ms[3]),
        ShadowSwapRegister(ms[4]),
        ShadowSwapRegister(ms[5]),
        ShadowSwapRegister(ms[6]),
        ShadowSwapRegister(ms[7]),
        ShadowSwapRegister(im[0]), // ?
        ShadowSwapRegister(im[1]), // ?
        ShadowSwapRegister(im[2]), // ?
        ShadowSwapRegister(vim), // ?
        ShadowSwapRegister(arstep[0]),
        ShadowSwapRegister(arstep[1]),
        ShadowSwapRegister(arstep[2]),
        ShadowSwapRegister(arstep[3]),
        ShadowSwapRegister(arpstepi[0]),
        ShadowSwapRegister(arpstepi[1]),
        ShadowSwapRegister(arpstepi[2]),
        ShadowSwapRegister(arpstepi[3]),
        ShadowSwapRegister(arpstepj[0]),
        ShadowSwapRegister(arpstepj[1]),
        ShadowSwapRegister(arpstepj[2]),
        ShadowSwapRegister(arpstepj[3]),
        ShadowSwapRegister(aroffset[0]),
        ShadowSwapRegister(aroffset[1]),
        ShadowSwapRegister(aroffset[2]),
        ShadowSwapRegister(aroffset[3]),
        ShadowSwapRegister(arpoffseti[0]),
        ShadowSwapRegister(arpoffseti[1]),
        ShadowSwapRegister(arpoffseti[2]),
        ShadowSwapRegister(arpoffseti[3]),
        ShadowSwapRegister(arpoffsetj[0]),
        ShadowSwapRegister(arpoffsetj[1]),
        ShadowSwapRegister(arpoffsetj[2]),
        ShadowSwapRegister(arpoffsetj[3]),
        ShadowSwapRegister(arrn[0]),
        ShadowSwapRegister(arrn[1]),
        ShadowSwapRegister(arrn[2]),
        ShadowSwapRegister(arrn[3]),
        ShadowSwapRegister(arprni[0]),
        ShadowSwapRegister(arprni[1]),
        ShadowSwapRegister(arprni[2]),
        ShadowSwapRegister(arprni[3]),
        ShadowSwapRegister(arprnj[0]),
        ShadowSwapRegister(arprnj[1]),
        ShadowSwapRegister(arprnj[2]),
        ShadowSwapRegister(arprnj[3]),
    };

    bool ConditionPass(Cond cond) const {
        switch(cond.GetName()) {
        case CondValue::True: return true;
        case CondValue::Eq: return fz == 1;
        case CondValue::Neq: return fz == 0;
        case CondValue::Gt: return fz == 0 && fm == 0;
        case CondValue::Ge: return fm == 0;
        case CondValue::Lt: return fm == 1;
        case CondValue::Le: return fm == 1 || fz == 1;
        case CondValue::Nn: return fn == 0;
        case CondValue::C: return fc == 1; // ?
        case CondValue::V: return fv == 1;
        case CondValue::E: return fe == 1;
        case CondValue::L: return fls == 1 || flv == 1; // ??
        case CondValue::Nr: return fr == 0;
        case CondValue::Niu0: return iu[0] == 0;
        case CondValue::Iu0: return iu[0] == 1;
        case CondValue::Iu1: return iu[1] == 1;
        default: throw "";
        }
    }

    template<typename PseudoRegisterT>
    u16 Get() const {
        return PseudoRegisterT::Get(this);
    }

    template<typename PseudoRegisterT>
    void Set(u16 value) {
        PseudoRegisterT::Set(this, value);
    }

};

template<u16 RegisterState::* target>
struct Redirector {
    static u16 Get(const RegisterState* self) {
        return self->*target;
    }
    static void Set(RegisterState* self, u16 value) {
        self->*target = value;
    }
};

template<std::size_t size, std::array<u16, size> RegisterState::* target, std::size_t index>
struct ArrayRedirector {
    static u16 Get(const RegisterState* self) {
        return (self->*target)[index];
    }
    static void Set(RegisterState* self, u16 value) {
        (self->*target)[index] = value;
    }
};

template<u16 RegisterState::* target>
struct UnkRedirector {
    static u16 Get(const RegisterState* self) {
        return self->*target;
    }
    static void Set(RegisterState* self, u16 value) {
        if (value != Get(self))
            printf("warning: Setting = 0x%X\n", value);
        self->*target = value;
    }
};

template<u16 RegisterState::* target0, u16 RegisterState::* target1>
struct DoubleRedirector {
    static u16 Get(const RegisterState* self) {
        return self->*target0 | self->*target1;
    }
    static void Set(RegisterState* self, u16 value){
        self->*target0 = self->*target1 = value;
    }
};

template<u16 RegisterState::* target>
struct RORedirector {
    static u16 Get(const RegisterState* self) {
        return self->*target;
    }
    static void Set(RegisterState*, u16) {
        // no
    }
};

template<std::size_t size, std::array<u16, size> RegisterState::* target, std::size_t index>
struct ArrayRORedirector {
    static u16 Get(const RegisterState* self) {
        return (self->*target)[index];
    }
    static void Set(RegisterState*, u16) {
        // no
    }
};

template<unsigned index>
struct AccEProxy {
    static u16 Get(const RegisterState* self) {
        return (u16)((self->a[index].value >> 32) & 0xF);
    }
    static void Set(RegisterState* self, u16 value) {
        u32 value32 = SignExtend<4>((u32)value);
        self->a[index].value &= 0xFFFFFFFF;
        self->a[index].value |= (u64)value32 << 32;
    }
};

template<typename Proxy, unsigned position, unsigned length>
struct ProxySlot {
    using proxy = Proxy;
    static constexpr unsigned pos = position;
    static constexpr unsigned len = length;
    static_assert(length < 16, "Error");
    static_assert(position + length <= 16, "Error");
    static constexpr u16 mask = ((1 << length) - 1) << position;
};

template<typename... ProxySlots>
struct PseudoRegister {
    static_assert((ProxySlots::mask | ...) == (ProxySlots::mask + ...), "Error");
    static u16 Get(const RegisterState* self) {
        return ((ProxySlots::proxy::Get(self) << ProxySlots::pos) | ...);
    }
    static void Set(RegisterState* self, u16 value) {
        (ProxySlots::proxy::Set(self,
            (value >> ProxySlots::pos) & ((1 << ProxySlots::len) - 1)) , ...);
    }
};

using cfgi = PseudoRegister<
    ProxySlot<Redirector<&RegisterState::stepi>, 0, 7>,
    ProxySlot<Redirector<&RegisterState::modi>, 7, 9>
>;

using cfgj = PseudoRegister<
    ProxySlot<Redirector<&RegisterState::stepj>, 0, 7>,
    ProxySlot<Redirector<&RegisterState::modj>, 7, 9>
>;

using stt0 = PseudoRegister<
    ProxySlot<Redirector<&RegisterState::fls>, 0, 1>,
    ProxySlot<Redirector<&RegisterState::flv>, 1, 1>,
    ProxySlot<Redirector<&RegisterState::fe>, 2, 1>,
    ProxySlot<Redirector<&RegisterState::fc>, 3, 1>,
    ProxySlot<Redirector<&RegisterState::fv>, 4, 1>,
    ProxySlot<Redirector<&RegisterState::fn>, 5, 1>,
    ProxySlot<Redirector<&RegisterState::fm>, 6, 1>,
    ProxySlot<Redirector<&RegisterState::fz>, 7, 1>,
    ProxySlot<Redirector<&RegisterState::fc1>, 11, 1>
>;
using stt1 = PseudoRegister<
    ProxySlot<Redirector<&RegisterState::fr>, 4, 1>,

    ProxySlot<ArrayRedirector<2, &RegisterState::psign, 0>, 14, 1>,
    ProxySlot<ArrayRedirector<2, &RegisterState::psign, 1>, 15, 1>
>;
using stt2 = PseudoRegister<
    ProxySlot<ArrayRORedirector<3, &RegisterState::ip, 0>, 0, 1>,
    ProxySlot<ArrayRORedirector<3, &RegisterState::ip, 1>, 1, 1>,
    ProxySlot<ArrayRORedirector<3, &RegisterState::ip, 2>, 2, 1>,
    ProxySlot<RORedirector<&RegisterState::vip>, 3, 1>,

    ProxySlot<Redirector<&RegisterState::movpd>, 6, 2>,

    ProxySlot<RORedirector<&RegisterState::bcn>, 12, 3>,
    ProxySlot<RORedirector<&RegisterState::lp>, 15, 1>
>;
using mod0 = PseudoRegister<
    ProxySlot<ArrayRedirector<2, &RegisterState::sar, 0>, 0, 1>,
    ProxySlot<ArrayRedirector<2, &RegisterState::sar, 1>, 1, 1>,
    ProxySlot<RORedirector<&RegisterState::mod0_unk_const>, 2, 3>,
    ProxySlot<Redirector<&RegisterState::ym>, 5, 2>,
    ProxySlot<Redirector<&RegisterState::s>, 7, 1>,
    ProxySlot<ArrayRedirector<2, &RegisterState::ou, 0>, 8, 1>,
    ProxySlot<ArrayRedirector<2, &RegisterState::ou, 1>, 9, 1>,
    ProxySlot<ArrayRedirector<2, &RegisterState::ps, 0>, 10, 2>,

    ProxySlot<ArrayRedirector<2, &RegisterState::ps, 1>, 13, 2>
>;
using mod1 = PseudoRegister<
    ProxySlot<Redirector<&RegisterState::page>, 0, 8>,

    ProxySlot<Redirector<&RegisterState::bankstep>, 12, 1>,
    ProxySlot<UnkRedirector<&RegisterState::mod1_unk>, 13, 3>
>;
using mod2 = PseudoRegister<
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 0>, 0, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 1>, 1, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 2>, 2, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 3>, 3, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 4>, 4, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 5>, 5, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 6>, 6, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 7>, 7, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::ms, 0>, 8, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::ms, 1>, 9, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::ms, 2>, 10, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::ms, 3>, 11, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::ms, 4>, 12, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::ms, 5>, 13, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::ms, 6>, 14, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::ms, 7>, 15, 1>
>;
using mod3 = PseudoRegister<
    ProxySlot<Redirector<&RegisterState::nimc>, 0, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::ic, 0>, 1, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::ic, 1>, 2, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::ic, 2>, 3, 1>,
    ProxySlot<Redirector<&RegisterState::vic>, 4, 1>, // ?

    ProxySlot<Redirector<&RegisterState::ie>, 7, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::im, 0>, 8, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::im, 1>, 9, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::im, 2>, 10, 1>,
    ProxySlot<Redirector<&RegisterState::vim>, 11, 1> // ?
>;

using st0 = PseudoRegister<
    ProxySlot<ArrayRedirector<2, &RegisterState::sar, 0>, 0, 1>,
    ProxySlot<Redirector<&RegisterState::ie>, 1, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::im, 0>, 2, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::im, 1>, 3, 1>,
    ProxySlot<Redirector<&RegisterState::fr>, 4, 1>,
    ProxySlot<DoubleRedirector<&RegisterState::fls, &RegisterState::flv>, 5, 1>,
    ProxySlot<Redirector<&RegisterState::fe>, 6, 1>,
    ProxySlot<Redirector<&RegisterState::fc>, 7, 1>,
    ProxySlot<Redirector<&RegisterState::fv>, 8, 1>,
    ProxySlot<Redirector<&RegisterState::fn>, 9, 1>,
    ProxySlot<Redirector<&RegisterState::fm>, 10, 1>,
    ProxySlot<Redirector<&RegisterState::fz>, 11, 1>,
    ProxySlot<AccEProxy<0>, 12, 4>
>;
using st1 = PseudoRegister<
    ProxySlot<Redirector<&RegisterState::page>, 0, 8>,
    // 8, 9: reserved
    ProxySlot<ArrayRedirector<2, &RegisterState::ps, 0>, 10, 2>,
    ProxySlot<AccEProxy<1>, 12, 4>
>;
using st2 = PseudoRegister<
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 0>, 0, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 1>, 1, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 2>, 2, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 3>, 3, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 4>, 4, 1>,
    ProxySlot<ArrayRedirector<8, &RegisterState::m, 5>, 5, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::im, 2>, 6, 1>,
    ProxySlot<Redirector<&RegisterState::s>, 7, 1>,
    ProxySlot<ArrayRedirector<2, &RegisterState::ou, 0>, 8, 1>,
    ProxySlot<ArrayRedirector<2, &RegisterState::ou, 1>, 9, 1>,
    ProxySlot<ArrayRORedirector<2, &RegisterState::iu, 0>, 10, 1>,
    ProxySlot<ArrayRORedirector<2, &RegisterState::iu, 1>, 11, 1>,
    // 12: reserved
    ProxySlot<ArrayRORedirector<3, &RegisterState::ip, 2>, 13, 1>, // Note the index order!
    ProxySlot<ArrayRORedirector<3, &RegisterState::ip, 0>, 14, 1>,
    ProxySlot<ArrayRORedirector<3, &RegisterState::ip, 1>, 15, 1>
>;
using icr = PseudoRegister<
    ProxySlot<Redirector<&RegisterState::nimc>, 0, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::ic, 0>, 1, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::ic, 1>, 2, 1>,
    ProxySlot<ArrayRedirector<3, &RegisterState::ic, 2>, 3, 1>,
    ProxySlot<RORedirector<&RegisterState::lp>, 4, 1>,
    ProxySlot<RORedirector<&RegisterState::bcn>, 5, 3>
>;

template<unsigned index>
using ar = PseudoRegister<
    ProxySlot<ArrayRedirector<4, &RegisterState::arstep, index * 2 + 1>, 0, 3>,
    ProxySlot<ArrayRedirector<4, &RegisterState::aroffset, index * 2 + 1>, 3, 2>,
    ProxySlot<ArrayRedirector<4, &RegisterState::arstep, index * 2>, 5, 3>,
    ProxySlot<ArrayRedirector<4, &RegisterState::aroffset, index * 2>, 8, 2>,
    ProxySlot<ArrayRedirector<4, &RegisterState::arrn, index * 2 + 1>, 10, 3>,
    ProxySlot<ArrayRedirector<4, &RegisterState::arrn, index * 2>, 13, 3>
>;

using ar0 = ar<0>;
using ar1 = ar<1>;

template<unsigned index>
using arp = PseudoRegister<
    ProxySlot<ArrayRedirector<4, &RegisterState::arpstepi, index>, 0, 3>,
    ProxySlot<ArrayRedirector<4, &RegisterState::arpoffseti, index>, 3, 2>,
    ProxySlot<ArrayRedirector<4, &RegisterState::arpstepj, index>, 5, 3>,
    ProxySlot<ArrayRedirector<4, &RegisterState::arpoffsetj, index>, 8, 2>,
    ProxySlot<ArrayRedirector<4, &RegisterState::arprni, index>, 10, 2>,
    // bit 12 reserved
    ProxySlot<ArrayRedirector<4, &RegisterState::arprnj, index>, 13, 2>
    // bit 15 reserved
>;

using arp0 = arp<0>;
using arp1 = arp<1>;
using arp2 = arp<2>;
using arp3 = arp<3>;
