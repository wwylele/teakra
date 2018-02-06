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

    class RegisterProxy {
    public:
        virtual ~RegisterProxy() = default;
        virtual u16 Get() const = 0;
        virtual void Set(u16 value) = 0;
    };

    class Redirector : public RegisterProxy {
    public:
        explicit Redirector(u16& target) : target(target) {}
        u16 Get() const override {
            return target;
        }
        void Set(u16 value) override {
            target = value;
        }
    private:
        u16& target;
    };

    class UnkRedirector : public Redirector {
    public:
        UnkRedirector(u16& target, const char* name) : Redirector(target), name(name) {}
        void Set(u16 value) override {
            if (value != Get())
                printf("warning: Setting %s = 0x%X\n", name, value);
            Redirector::Set(value);
        }
    private:
        const char* name;
    };

    class DoubleRedirector : public RegisterProxy {
    public:
        DoubleRedirector(u16& target0, u16& target1) : target0(target0), target1(target1) {}
        u16 Get() const override {
            return target0 | target1;
        }
        void Set(u16 value) override {
            target0 = target1 = value;
        }
    private:
        u16& target0, target1;
    };

    class RORedirector : public Redirector {
        using Redirector::Redirector;
        void Set(u16 value) override {}
    };

    class AccEProxy : public RegisterProxy {
    public:
        explicit AccEProxy(Accumulator& target) : target(target) {}
        u16 Get() const override {
            return (u16)((target.value >> 32) & 0xF);
        }
        void Set(u16 value) override {
            u32 value32 = SignExtend<4>((u32)value);
            target.value &= 0xFFFFFFFF;
            target.value |= (u64)value32 << 32;
        }
    private:
        Accumulator& target;
    };

    struct ProxySlot {
        std::shared_ptr<RegisterProxy> proxy;
        unsigned position;
        unsigned length;
    };

    struct PseudoRegister {
        std::vector<ProxySlot> slots;

        u16 Get() const {
            u16 result = 0;
            for (const auto& slot : slots) {
                result |= slot.proxy->Get() << slot.position;
            }
            return result;
        }
        void Set(u16 value) {
            for (const auto& slot : slots) {
                slot.proxy->Set((value >> slot.position) & ((1 << slot.length) - 1));
            }
        }
    };

    // step/modulo
    u16 stepi = 0, stepj = 0; // 7 bit 2's comp
    u16 modi = 0, modj = 0; // 9 bit
    u16 stepi0 = 0, stepj0 = 0; // alternative step

    // Shadows for bank exchange
    u16 stepib = 0, stepjb = 0;
    u16 modib = 0, modjb = 0;
    u16 stepi0b = 0, stepj0b = 0;

    PseudoRegister cfgi {{
        {std::make_shared<Redirector>(stepi), 0, 7},
        {std::make_shared<Redirector>(modi), 7, 9},
    }};
    PseudoRegister cfgj {{
        {std::make_shared<Redirector>(stepj), 0, 7},
        {std::make_shared<Redirector>(modj), 7, 9},
    }};

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

    PseudoRegister stt0 {{
        {std::make_shared<Redirector>(fls), 0, 1},
        {std::make_shared<Redirector>(flv), 1, 1},
        {std::make_shared<Redirector>(fe), 2, 1},
        {std::make_shared<Redirector>(fc), 3, 1},
        {std::make_shared<Redirector>(fv), 4, 1},
        {std::make_shared<Redirector>(fn), 5, 1},
        {std::make_shared<Redirector>(fm), 6, 1},
        {std::make_shared<Redirector>(fz), 7, 1},
        {std::make_shared<Redirector>(fc1), 11, 1},
    }};
    PseudoRegister stt1 {{
        {std::make_shared<Redirector>(fr), 4, 1},

        {std::make_shared<Redirector>(psign[0]), 14, 1},
        {std::make_shared<Redirector>(psign[1]), 15, 1},
    }};
    PseudoRegister stt2 {{
        {std::make_shared<RORedirector>(ip[0]), 0, 1},
        {std::make_shared<RORedirector>(ip[1]), 1, 1},
        {std::make_shared<RORedirector>(ip[2]), 2, 1},
        {std::make_shared<RORedirector>(vip), 3, 1},

        {std::make_shared<Redirector>(movpd), 6, 2},

        {std::make_shared<RORedirector>(bcn), 12, 3},
        {std::make_shared<RORedirector>(lp), 15, 1},
    }};
    PseudoRegister mod0 {{
        {std::make_shared<Redirector>(sar[0]), 0, 1},
        {std::make_shared<Redirector>(sar[1]), 1, 1},
        {std::make_shared<RORedirector>(mod0_unk_const), 2, 3},
        {std::make_shared<Redirector>(ym), 5, 2},
        {std::make_shared<Redirector>(s), 7, 1},
        {std::make_shared<Redirector>(ou[0]), 8, 1},
        {std::make_shared<Redirector>(ou[1]), 9, 1},
        {std::make_shared<Redirector>(ps[0]), 10, 2},

        {std::make_shared<Redirector>(ps[1]), 13, 2},
    }};
    PseudoRegister mod1 {{
        {std::make_shared<Redirector>(page), 0, 8},

        {std::make_shared<Redirector>(bankstep), 12, 1},
        {std::make_shared<UnkRedirector>(mod1_unk, "mod1.13"), 13, 3},
    }};
    PseudoRegister mod2 {{
        {std::make_shared<Redirector>(m[0]), 0, 1},
        {std::make_shared<Redirector>(m[1]), 1, 1},
        {std::make_shared<Redirector>(m[2]), 2, 1},
        {std::make_shared<Redirector>(m[3]), 3, 1},
        {std::make_shared<Redirector>(m[4]), 4, 1},
        {std::make_shared<Redirector>(m[5]), 5, 1},
        {std::make_shared<Redirector>(m[6]), 6, 1},
        {std::make_shared<Redirector>(m[7]), 7, 1},
        {std::make_shared<Redirector>(ms[0]), 8, 1},
        {std::make_shared<Redirector>(ms[1]), 9, 1},
        {std::make_shared<Redirector>(ms[2]), 10, 1},
        {std::make_shared<Redirector>(ms[3]), 11, 1},
        {std::make_shared<Redirector>(ms[4]), 12, 1},
        {std::make_shared<Redirector>(ms[5]), 13, 1},
        {std::make_shared<Redirector>(ms[6]), 14, 1},
        {std::make_shared<Redirector>(ms[7]), 15, 1},
    }};
    PseudoRegister mod3 {{
        {std::make_shared<Redirector>(nimc), 0, 1},
        {std::make_shared<Redirector>(ic[0]), 1, 1},
        {std::make_shared<Redirector>(ic[1]), 2, 1},
        {std::make_shared<Redirector>(ic[2]), 3, 1},
        {std::make_shared<Redirector>(vic), 4, 1}, // ?

        {std::make_shared<Redirector>(ie), 7, 1},
        {std::make_shared<Redirector>(im[0]), 8, 1},
        {std::make_shared<Redirector>(im[1]), 9, 1},
        {std::make_shared<Redirector>(im[2]), 10, 1},
        {std::make_shared<Redirector>(vim), 11, 1}, // ?
    }};

    PseudoRegister st0 {{
        {std::make_shared<Redirector>(sar[0]), 0, 1},
        {std::make_shared<Redirector>(ie), 1, 1},
        {std::make_shared<Redirector>(im[0]), 2, 1},
        {std::make_shared<Redirector>(im[1]), 3, 1},
        {std::make_shared<Redirector>(fr), 4, 1},
        {std::make_shared<DoubleRedirector>(fls, flv), 5, 1},
        {std::make_shared<Redirector>(fe), 6, 1},
        {std::make_shared<Redirector>(fc), 7, 1},
        {std::make_shared<Redirector>(fv), 8, 1},
        {std::make_shared<Redirector>(fn), 9, 1},
        {std::make_shared<Redirector>(fm), 10, 1},
        {std::make_shared<Redirector>(fz), 11, 1},
        {std::make_shared<AccEProxy>(a[01]), 12, 4},
    }};
    PseudoRegister st1 {{
        {std::make_shared<Redirector>(page), 0, 8},
        // 8, 9: reserved
        {std::make_shared<Redirector>(ps[0]), 10, 2},
        {std::make_shared<AccEProxy>(a[1]), 12, 4},
    }};
    PseudoRegister st2 {{
        {std::make_shared<Redirector>(m[0]), 0, 1},
        {std::make_shared<Redirector>(m[1]), 1, 1},
        {std::make_shared<Redirector>(m[2]), 2, 1},
        {std::make_shared<Redirector>(m[3]), 3, 1},
        {std::make_shared<Redirector>(m[4]), 4, 1},
        {std::make_shared<Redirector>(m[5]), 5, 1},
        {std::make_shared<Redirector>(im[2]), 6, 1},
        {std::make_shared<Redirector>(s), 7, 1},
        {std::make_shared<Redirector>(ou[0]), 8, 1},
        {std::make_shared<Redirector>(ou[1]), 9, 1},
        {std::make_shared<RORedirector>(iu[0]), 10, 1},
        {std::make_shared<RORedirector>(iu[1]), 11, 1},
        // 12: reserved
        {std::make_shared<RORedirector>(ip[2]), 13, 1},
        {std::make_shared<RORedirector>(ip[0]), 14, 1},
        {std::make_shared<RORedirector>(ip[1]), 15, 1},
    }};
    PseudoRegister icr {{
        {std::make_shared<Redirector>(nimc), 0, 1},
        {std::make_shared<Redirector>(ic[0]), 1, 1},
        {std::make_shared<Redirector>(ic[1]), 2, 1},
        {std::make_shared<Redirector>(ic[2]), 3, 1},
        {std::make_shared<RORedirector>(lp), 4, 1},
        {std::make_shared<RORedirector>(bcn), 5, 3},
        // reserved
    }};

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

    PseudoRegister ar0 {{
        {std::make_shared<Redirector>(arstep[1]), 0, 3},
        {std::make_shared<Redirector>(aroffset[1]), 3, 2},
        {std::make_shared<Redirector>(arstep[0]), 5, 3},
        {std::make_shared<Redirector>(aroffset[0]), 8, 2},
        {std::make_shared<Redirector>(arrn[1]), 10, 3},
        {std::make_shared<Redirector>(arrn[0]), 13, 3},
    }};

    PseudoRegister ar1 {{
        {std::make_shared<Redirector>(arstep[3]), 0, 3},
        {std::make_shared<Redirector>(aroffset[3]), 3, 2},
        {std::make_shared<Redirector>(arstep[2]), 5, 3},
        {std::make_shared<Redirector>(aroffset[2]), 8, 2},
        {std::make_shared<Redirector>(arrn[3]), 10, 3},
        {std::make_shared<Redirector>(arrn[2]), 13, 3},
    }};

    PseudoRegister arp0 {{
        {std::make_shared<Redirector>(arpstepi[0]), 0, 3},
        {std::make_shared<Redirector>(arpoffseti[0]), 3, 2},
        {std::make_shared<Redirector>(arpstepj[0]), 5, 3},
        {std::make_shared<Redirector>(arpoffsetj[0]), 8, 2},
        {std::make_shared<Redirector>(arprni[0]), 10, 2},
        // bit 12 reserved
        {std::make_shared<Redirector>(arprnj[0]), 13, 2},
        // bit 15 reserved
    }};

    PseudoRegister arp1 {{
        {std::make_shared<Redirector>(arpstepi[1]), 0, 3},
        {std::make_shared<Redirector>(arpoffseti[1]), 3, 2},
        {std::make_shared<Redirector>(arpstepj[1]), 5, 3},
        {std::make_shared<Redirector>(arpoffsetj[1]), 8, 2},
        {std::make_shared<Redirector>(arprni[1]), 10, 2},
        // bit 12 reserved
        {std::make_shared<Redirector>(arprnj[1]), 13, 2},
        // bit 15 reserved
    }};

    PseudoRegister arp2 {{
        {std::make_shared<Redirector>(arpstepi[2]), 0, 3},
        {std::make_shared<Redirector>(arpoffseti[2]), 3, 2},
        {std::make_shared<Redirector>(arpstepj[2]), 5, 3},
        {std::make_shared<Redirector>(arpoffsetj[2]), 8, 2},
        {std::make_shared<Redirector>(arprni[2]), 10, 2},
        // bit 12 reserved
        {std::make_shared<Redirector>(arprnj[2]), 13, 2},
        // bit 15 reserved
    }};

    PseudoRegister arp3 {{
        {std::make_shared<Redirector>(arpstepi[3]), 0, 3},
        {std::make_shared<Redirector>(arpoffseti[3]), 3, 2},
        {std::make_shared<Redirector>(arpstepj[3]), 5, 3},
        {std::make_shared<Redirector>(arpoffsetj[3]), 8, 2},
        {std::make_shared<Redirector>(arprni[3]), 10, 2},
        // bit 12 reserved
        {std::make_shared<Redirector>(arprnj[3]), 13, 2},
        // bit 15 reserved
    }};

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

};
