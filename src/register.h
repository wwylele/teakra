#pragma once
#include <vector>
#include <memory>
#include "common_types.h"
#include "oprand.h"

struct RegisterState {
    void Reset() {
        pc = 0;

    }

    u32 pc;

    u16 GetPcL() {
        return pc & 0xFFFF;
    }
    u16 GetPcH() {
        return pc >> 16;
    }
    void SetPC(u16 low, u16 high) {
        pc = low | ((u32)high << 16);
    }

    u16 dvm;
    u16 repc;
    u16 lc;
    u16 mixp;
    u16 sv;
    u16 sp;

    u16 r[8];

    struct Accumulator {
        u64 value;
    };
    Accumulator a[2];
    Accumulator b[2];

    struct Product {
        u32 value;
    };
    u16 x[2];
    u16 y[2];
    Product p[2];

    u16 ar[2]; // ?
    u16 arp[4]; // ?
    u16 stepi0, stepj0; // ?
    u16 vtr[2]; // ?

    class RegisterProxy {
    public:
        virtual ~RegisterProxy() = default;
        virtual u16 Get() = 0;
        virtual void Set(u16 value) = 0;
    };

    class Redirector : public RegisterProxy {
    public:
        Redirector(u16& target) : target(target) {}
        u16 Get() override {
            return target;
        }
        void Set(u16 value) override {
            target = value;
        }
    private:
        u16& target;
    };

    class RORedirector : public Redirector {
        using Redirector::Redirector;
        void Set(u16 value) override {}
    };

    struct ProxySlot {
        std::shared_ptr<RegisterProxy> proxy;
        unsigned position;
        unsigned length;
    };

    struct PseudoRegister {
        std::vector<ProxySlot> slots;

        u16 Get() {
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

    u16 stepi, stepj;
    u16 modi, modj;

    PseudoRegister cfgi {{
        {std::make_shared<Redirector>(stepi), 0, 7},
        {std::make_shared<Redirector>(modi), 7, 9},
    }};
    PseudoRegister cfgj {{
        {std::make_shared<Redirector>(stepj), 0, 7},
        {std::make_shared<Redirector>(modj), 7, 9},
    }};

    u16 fz, fm, fn, fv, fc, fe, fl[2], fr;
    u16 nimc;
    u16 ip[3], vip;
    u16 im[3], vim;
    u16 ic[3], vic;
    u16 ie;
    u16 movpd;
    u16 bcn;
    u16 lp;
    u16 sar; // 0 - enable saturation; 1 - disable saturation
    u16 ps[3];
    u16 s;
    u16 ou[2];
    u16 iu[2];
    u16 page;
    u16 m[8];

    PseudoRegister stt0 {{
        {std::make_shared<Redirector>(fl[0]), 0, 1},
        {std::make_shared<Redirector>(fl[1]), 1, 1},
        {std::make_shared<Redirector>(fe), 2, 1},
        {std::make_shared<Redirector>(fc), 3, 1},
        {std::make_shared<Redirector>(fv), 4, 1},
        {std::make_shared<Redirector>(fn), 5, 1},
        {std::make_shared<Redirector>(fm), 6, 1},
        {std::make_shared<Redirector>(fz), 7, 1},
    }};
    PseudoRegister stt1 {{
        {std::make_shared<Redirector>(fr), 4, 1},
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
        {std::make_shared<Redirector>(sar), 0, 1},

        {std::make_shared<Redirector>(ps[0]), 2, 2},

        {std::make_shared<Redirector>(s), 7, 1},
        {std::make_shared<Redirector>(ou[0]), 8, 1},
        {std::make_shared<Redirector>(ou[0]), 9, 1},
        {std::make_shared<Redirector>(ps[1]), 10, 2},

        {std::make_shared<Redirector>(ps[2]), 13, 2},
    }};
    PseudoRegister mod1 {{
        {std::make_shared<Redirector>(page), 0, 8},
    }};
    PseudoRegister mod2 {{
        {std::make_shared<Redirector>(m[0]), 0, 1},
        {std::make_shared<Redirector>(m[1]), 1, 1},
        {std::make_shared<Redirector>(m[2]), 2, 1},
        {std::make_shared<Redirector>(m[3]), 3, 1},
        {std::make_shared<Redirector>(m[4]), 4, 1},
        {std::make_shared<Redirector>(m[5]), 5, 1},
        {std::make_shared<Redirector>(m[6]), 6, 1}, // ?
        {std::make_shared<Redirector>(m[7]), 7, 1}, // ?
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

    bool ConditionPass(Cond cond) {
        switch(cond.GetName()) {
        case CondValue::True: return true;
        case CondValue::Eq: return fz == 1;
        case CondValue::Neq: return fz == 0;
        case CondValue::Gt: return fz == 0 && fm == 0;
        case CondValue::Ge: return fm == 0;
        case CondValue::Lt: return fm == 1;
        case CondValue::Le: return fm == 1 || fz == 1;
        case CondValue::Nn: return fn == 0;
        case CondValue::C: return fc == 1;
        case CondValue::V: return fv == 1;
        case CondValue::E: return fe == 1;
        case CondValue::L: return fl[0] == 1; // ??
        case CondValue::Nr: return fr == 0;
        case CondValue::Niu0: return iu[0] == 0;
        case CondValue::Iu0: return iu[0] == 1;
        case CondValue::Iu1: return iu[1] == 1;
        default: throw "";
        }
    }

};
