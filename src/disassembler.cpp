#include <iomanip>
#include <sstream>

#include "common_types.h"
#include "oprand.h"
#include "decoder.h"
#include "teakra/disassembler.h"

namespace Teakra::Disassembler {

template<typename T>
std::string ToHex(T i)
{
  u64 v = i;
  std::stringstream stream;
  stream << "0x"
         << std::setfill ('0') << std::setw(sizeof(T) * 2)
         << std::hex << v;
  return stream.str();
}

template <unsigned bits>
std::string Dsm(Imm<bits> a) {
    return ToHex(a.storage);
}

template <unsigned bits>
std::string Dsm(Imms<bits> a) {
    u16 value = SignExtend<bits, u16>(a.storage);
    bool negative = (value >> 15) != 0;
    if (negative) {
        value = (~value) + 1;
    }
    return (negative? '-' : ' ') + ToHex(value);
}

std::string Dsm(MemImm8 a) {
    return "[page:" + Dsm((Imm8)a) + "]";
}

std::string Dsm(MemImm16 a) {
    return "[" + Dsm((Imm16)a) + "]";
}

std::string Dsm(MemR7Imm16 a) {
    return "[r7+" + Dsm((Imm16)a) + "]";
}
std::string Dsm(MemR7Imm7s a) {
    return "[r7+" + Dsm((Imm7s)a) + "]";
}

template <typename ArRn>
std::string DsmArRn(ArRn a) {
    return "arrn" + std::to_string(a.storage);
}

template <typename ArStep>
std::string DsmArStep(ArStep a) {
    return "+ars" + std::to_string(a.storage);
}

template <typename ArpRn>
std::string DsmArpRni(ArpRn a) {
    return "arprni" + std::to_string(a.storage);
}

template <typename ArpStep>
std::string DsmArpStepi(ArpStep a) {
    return "+arpsi" + std::to_string(a.storage);
}

template <typename ArpRn>
std::string DsmArpRnj(ArpRn a) {
    return "arprnj" + std::to_string(a.storage);
}

template <typename ArpStep>
std::string DsmArpStepj(ArpStep a) {
    return "+arpsj" + std::to_string(a.storage);
}

template <typename RegT>
std::string DsmReg(RegT a) {
    switch (a.GetName()) {
    case RegName::a0: return "a0";
    case RegName::a0l: return "a0l";
    case RegName::a0h: return "a0h";
    case RegName::a0e: return "a0e";
    case RegName::a1: return "a1";
    case RegName::a1l: return "a1l";
    case RegName::a1h: return "a1h";
    case RegName::a1e: return "a1e";
    case RegName::b0: return "b0";
    case RegName::b0l: return "b0l";
    case RegName::b0h: return "b0h";
    case RegName::b0e: return "b0e";
    case RegName::b1: return "b1";
    case RegName::b1l: return "b1l";
    case RegName::b1h: return "b1h";
    case RegName::b1e: return "b1e";

    case RegName::r0: return "r0";
    case RegName::r1: return "r1";
    case RegName::r2: return "r2";
    case RegName::r3: return "r3";
    case RegName::r4: return "r4";
    case RegName::r5: return "r5";
    case RegName::r6: return "r6";
    case RegName::r7: return "r7";

    case RegName::ar0: return "ar0";
    case RegName::ar1: return "ar1";
    case RegName::arp0: return "arp0";
    case RegName::arp1: return "arp1";
    case RegName::arp2: return "arp2";
    case RegName::arp3: return "arp3";
    case RegName::stt0: return "stt0";
    case RegName::stt1: return "stt1";
    case RegName::stt2: return "stt2";
    case RegName::mod0: return "mod0";
    case RegName::mod1: return "mod1";
    case RegName::mod2: return "mod2";
    case RegName::mod3: return "mod3";
    case RegName::cfgi: return "cfgi";
    case RegName::cfgj: return "cfgj";

    case RegName::x0: return "x0";
    case RegName::x1: return "x1";
    case RegName::y0: return "y0";
    case RegName::y1: return "y1";
    case RegName::p0: return "p0";
    case RegName::p1: return "p1";
    case RegName::p: return "p*";

    case RegName::pc: return "pc";
    case RegName::sp: return "sp";
    case RegName::sv: return "sv";
    case RegName::lc: return "lc";

    case RegName::st0: return "st0";
    case RegName::st1: return "st1";
    case RegName::st2: return "st2";
    default: return "???" + std::to_string((int)a.GetName());
    }
}

std::string Dsm(Alm alm) {
    switch (alm.GetName()) {
    case AlmOp::Or: return "or";
    case AlmOp::And: return "and";
    case AlmOp::Xor: return "xor";
    case AlmOp::Add: return "add";
    case AlmOp::Tst0: return "tst0";
    case AlmOp::Tst1: return "tst1";
    case AlmOp::Cmp: return "cmp";
    case AlmOp::Sub: return "sub";
    case AlmOp::Msu: return "msu";
    case AlmOp::Addh: return "addh";
    case AlmOp::Addl: return "addl";
    case AlmOp::Subh: return "subh";
    case AlmOp::Subl: return "subl";
    case AlmOp::Sqr: return "sqr";
    case AlmOp::Sqra: return "sqra";
    case AlmOp::Cmpu: return "cmpu";
    default: throw "what";
    }
}

std::string Dsm(Alu alu) {
    switch (alu.GetName()) {
    case AlmOp::Or: return "or";
    case AlmOp::And: return "and";
    case AlmOp::Xor: return "xor";
    case AlmOp::Add: return "add";
    case AlmOp::Cmp: return "cmp";
    case AlmOp::Sub: return "sub";
    default: throw "what";
    }
}

std::string Dsm(Alb alb) {
    switch (alb.GetName()) {
    case AlbOp::Set: return "set";
    case AlbOp::Rst: return "rst";
    case AlbOp::Chng: return "chng";
    case AlbOp::Addv: return "addv";
    case AlbOp::Tst0: return "tst0";
    case AlbOp::Tst1: return "tst1";
    case AlbOp::Cmpv: return "cmpv";
    case AlbOp::Subv: return "subv";
    default: throw "what";
    }
}

std::string Dsm(Moda4 moda4) {
    switch (moda4.GetName()) {
    case ModaOp::Shr: return "shr";
    case ModaOp::Shr4: return "shr4";
    case ModaOp::Shl: return "shl";
    case ModaOp::Shl4: return "shl4";
    case ModaOp::Ror: return "ror";
    case ModaOp::Rol: return "rol";
    case ModaOp::Clr: return "clr";
    case ModaOp::Not: return "not";
    case ModaOp::Neg: return "neg";
    case ModaOp::Rnd: return "rnd";
    case ModaOp::Pacr: return "pacr";
    case ModaOp::Clrr: return "clrr";
    case ModaOp::Inc: return "inc";
    case ModaOp::Dec: return "dec";
    case ModaOp::Copy: return "copy";
    default: return "[ERROR]";
    }
}

std::string Dsm(Moda3 moda3) {
    switch (moda3.GetName()) {
    case ModaOp::Shr: return "shr";
    case ModaOp::Shr4: return "shr4";
    case ModaOp::Shl: return "shl";
    case ModaOp::Shl4: return "shl4";
    case ModaOp::Ror: return "ror";
    case ModaOp::Rol: return "rol";
    case ModaOp::Clr: return "clr";
    case ModaOp::Clrr: return "clrr";
    default: return "[ERROR]";
    }
}

std::string Dsm(Mul3 mul) {
    switch(mul.GetName()) {
    case MulOp::Mpy: return "mpy";
    case MulOp::Mpysu: return "mpysu";
    case MulOp::Mac: return "mac";
    case MulOp::Macus: return "macus";
    case MulOp::Maa: return "maa";
    case MulOp::Macuu: return "macuu";
    case MulOp::Macsu: return "macsu";
    case MulOp::Maasu: return "maasu";
    default: return "[ERROR]";
    }
}

std::string Dsm(Mul2 mul) {
    switch(mul.GetName()) {
    case MulOp::Mpy: return "mpy";
    case MulOp::Mac: return "mac";
    case MulOp::Maa: return "maa";
    case MulOp::Macsu: return "macsu";
    default: return "[ERROR]";
    }
}


std::string Dsm(Cond cond) {
    switch (cond.GetName()) {
    case CondValue::True: return "always";
    case CondValue::Eq: return "eq";
    case CondValue::Neq: return "neq";
    case CondValue::Gt: return "gt";
    case CondValue::Ge: return "ge";
    case CondValue::Lt: return "lt";
    case CondValue::Le: return "le";
    case CondValue::Nn: return "mn";
    case CondValue::C: return "c";
    case CondValue::V: return "v";
    case CondValue::E: return "e";
    case CondValue::L: return "l";
    case CondValue::Nr: return "nr";
    case CondValue::Niu0: return "niu0";
    case CondValue::Iu0: return "iu0";
    case CondValue::Iu1: return "iu1";
    default: throw "what";
    }
}

std::string Dsm(Address16 addr) {
    return ToHex((u32)(addr.storage));
}

std::string Dsm(Address18_16 addr_low, Address18_2 addr_high) {
    return ToHex((u32)(addr_low.storage + (addr_high.storage << 16)));
}

struct A18 {
    A18(Address18_16 low, Address18_2 high) : low(low), high(high) {}
    Address18_16 low;
    Address18_2 high;
};

std::string Dsm(A18 addr) {
    return Dsm(addr.low, addr.high);
}

std::string Dsm(StepZIDS step) {
    switch (step.GetName()) {
    case StepValue::Zero: return "";
    case StepValue::Increase: return "++";
    case StepValue::Decrease: return "--";
    case StepValue::PlusStep: return "++s";
    default: throw "what";
    }
}

std::string Dsm(std::string t) {
    return t;
}

template <typename RegT>
struct R {
    R(RegT reg) : reg(reg) {}
    RegT reg;
};

template <typename RegT>
struct MemR {
    MemR(RegT reg, StepZIDS step) : reg(reg), step(step) {}
    RegT reg;
    StepZIDS step;
};

template <typename ArRn, typename ArStep>
struct MemARS {
    MemARS(ArRn reg, ArStep step) : reg(reg), step(step) {}
    ArRn reg;
    ArStep step;
};

template <typename ArpRn, typename ArpStep>
struct MemARPSI {
    MemARPSI(ArpRn reg, ArpStep step) : reg(reg), step(step) {}
    ArpRn reg;
    ArpStep step;
};

template <typename ArpRn, typename ArpStep>
struct MemARPSJ {
    MemARPSJ(ArpRn reg, ArpStep step) : reg(reg), step(step) {}
    ArpRn reg;
    ArpStep step;
};

template <typename ArRn>
struct MemAR {
    MemAR(ArRn reg) : reg(reg){}
    ArRn reg;
};

template <typename Reg>
struct MemG {
    MemG(Reg reg) : reg(reg){}
    Reg reg;
};

template <typename RegT>
std::string Dsm(R<RegT> t) {
    return DsmReg(t.reg);
}

template <typename RegT>
std::string Dsm(MemR<RegT> t) {
    return "[" + DsmReg(t.reg) + Dsm(t.step) + "]";
}

template <typename ArRn, typename ArStep>
std::string Dsm(MemARS<ArRn, ArStep> t) {
    return "[" + DsmArRn(t.reg) + DsmArStep(t.step) + "]";
}

template <typename ArpRn, typename ArpStep>
std::string Dsm(MemARPSI<ArpRn, ArpStep> t) {
    return "[" + DsmArpRni(t.reg) + DsmArpStepi(t.step) + "]";
}

template <typename ArpRn, typename ArpStep>
std::string Dsm(MemARPSJ<ArpRn, ArpStep> t) {
    return "[" + DsmArpRnj(t.reg) + DsmArpStepj(t.step) + "]";
}

template <typename ArRn>
std::string Dsm(MemAR<ArRn> t) {
    return "[" + DsmArRn(t.reg) + "]";
}

template <typename Reg>
std::string Dsm(MemG<Reg> t) {
    return "[" + DsmReg(t.reg) + "]";
}

template <typename ... T>
std::string D(T ... t) {
    return ((Dsm(t) + "    ") + ...);
}

class Disassembler {
public:
    using instruction_return_type = std::string;

    std::string undefined(u16 opcode) {
        return "[undefined]";
    }

    std::string nop(Dummy) {
        return "nop";
    }

    std::string norm(Ax a, Rn b, StepZIDS bs) {
        return D("norm", R(a), MemR(b, bs));
    }
    std::string swap(SwapType swap) {
        std::string desc;
        switch (swap.GetName()) {
        case SwapTypeValue::a0b0:
            desc = "a0<->b0";
            break;
        case SwapTypeValue::a0b1:
            desc = "a0<->b1";
            break;
        case SwapTypeValue::a1b0:
            desc = "a1<->b0";
            break;
        case SwapTypeValue::a1b1:
            desc = "a1<->b1";
            break;
        case SwapTypeValue::a0b0a1b1:
            desc = "a<->b";
            break;
        case SwapTypeValue::a0b1a1b0:
            desc = "a-x-b";
            break;
        case SwapTypeValue::a0b0a1:
            desc = "a0->b0->a1";
            break;
        case SwapTypeValue::a0b1a1:
            desc = "a0->b1->a1";
            break;
        case SwapTypeValue::a1b0a0:
            desc = "a1->b0->a0";
            break;
        case SwapTypeValue::a1b1a0:
            desc = "a1->b1->a0";
            break;
        case SwapTypeValue::b0a0b1:
            desc = "b0->a0->b1";
            break;
        case SwapTypeValue::b0a1b1:
            desc = "b0->a1->b1";
            break;
        case SwapTypeValue::b1a0b0:
            desc = "b1->a0->b0";
            break;
        case SwapTypeValue::b1a1b0:
            desc = "b1->a1->b0";
            break;
        default:
            desc = "?";
        }
        return D("swap", desc);
    }
    std::string trap(Dummy) {
        return "trap";
    }

    std::string alm(Alm op, MemImm8 a, Ax b) {
        return D(op, a, R(b));
    }
    std::string alm(Alm op, Rn a, StepZIDS as, Ax b) {
        return D(op, MemR(a, as), R(b));
    }
    std::string alm(Alm op, Register a, Ax b) {
        return D(op, R(a), R(b));
    }
    std::string alm_r6(Alm op, Ax b) {
        return D(op, "r6", R(b));
    }

    std::string alu(Alu op, MemImm16 a, Ax b) {
        return D(op, a, R(b));
    }
    std::string alu(Alu op, MemR7Imm16 a, Ax b) {
        return D(op, a, R(b));
    }
    std::string alu(Alu op, Imm16 a, Ax b) {
        return D(op, a, R(b));
    }
    std::string alu(Alu op, Imm8 a, Ax b) {
        return D(op, a, R(b));
    }
    std::string alu(Alu op, MemR7Imm7s a, Ax b) {
        return D(op, a, R(b));
    }

    std::string or_(Ab a, Ax b, Ax c) {
        return D("or", R(a), R(b), R(c));
    }
    std::string or_(Ax a, Bx b, Ax c) {
        return D("or", R(a), R(b), R(c));
    }
    std::string or_(Bx a, Bx b, Ax c){
        return D("or", R(a), R(b), R(c));
    }

    std::string alb(Alb op, Imm16 a, MemImm8 b) {
        return D(op, a, b);
    }
    std::string alb(Alb op, Imm16 a, Rn b, StepZIDS bs) {
        return D(op, a, MemR(b, bs));
    }
    std::string alb(Alb op, Imm16 a, Register b) {
        return D(op, a, R(b));
    }
    std::string alb_r6(Alb op, Imm16 a) {
        return D(op, a, "r6");
    }
    std::string alb(Alb op, Imm16 a, SttMod b) {
        return D(op, a, R(b));
    }

    std::string add(Ab a, Bx b) {
        return D("add", R(a), R(b));
    }
    std::string add(Bx a, Ax b) {
        return D("add", R(a), R(b));
    }
    std::string add_p1(Ax b) {
        return D("add", "p1", R(b));
    }
    std::string add(Px a, Bx b) {
        return D("add", R(a), R(b));
    }
    std::string add_p0_p1(Ab c) {
        return D("add", "p0", "p1", R(c));
    }
    std::string add_p0_p1a(Ab c) {
        return D("add", "p0", "p1a", R(c));
    }
    std::string add3_p0_p1(Ab c) {
        return D("add3", "p0", "p1", R(c));
    }
    std::string add3_p0_p1a(Ab c) {
        return D("add3", "p0", "p1a", R(c));
    }
    std::string add3_p0a_p1a(Ab c) {
        return D("add3", "p0a", "p1a", R(c));
    }

    std::string sub(Ab a, Bx b) {
        return D("sub", R(a), R(b));
    }
    std::string sub(Bx a, Ax b) {
        return D("sub", R(a), R(b));
    }
    std::string sub_p1(Ax b) {
        return D("sub", "p1", R(b));
    }
    std::string sub(Px a, Bx b) {
        return D("sub", R(a), R(b));
    }
    std::string sub_p0_p1(Ab c) {
        return D("sub", "p0", "p1", R(c));
    }
    std::string sub_p0_p1a(Ab c) {
        return D("sub", "p0", "p1a", R(c));
    }
    std::string sub3_p0_p1(Ab c) {
        return D("sub3", "p0", "p1", R(c));
    }
    std::string sub3_p0_p1a(Ab c) {
        return D("sub3", "p0", "p1a", R(c));
    }
    std::string sub3_p0a_p1a(Ab c) {
        return D("sub3", "p0a", "p1a", R(c));
    }

    std::string addsub_p0_p1(Ab c) {
        return D("addsub", "p0", "p1", R(c));
    }
    std::string addsub_p1_p0(Ab c) {
        return D("addsub", "p1", "p0", R(c));
    }
    std::string addsub_p0_p1a(Ab c) {
        return D("addsub", "p0", "p1a", R(c));
    }
    std::string addsub_p1a_p0(Ab c) {
        return D("addsub", "p1a", "p0", R(c));
    }

    std::string add_add(ArpRn1 a, ArpStep1 asi, ArpStep1 asj, Ab b) {
        return D("add||add", MemARPSJ(a, asj), MemARPSI(a, asi), R(b));
    }
    std::string add_sub(ArpRn1 a, ArpStep1 asi, ArpStep1 asj, Ab b) {
        return D("add||sub", MemARPSJ(a, asj), MemARPSI(a, asi), R(b));
    }
    std::string sub_add(ArpRn1 a, ArpStep1 asi, ArpStep1 asj, Ab b) {
        return D("sub||add", MemARPSJ(a, asj), MemARPSI(a, asi), R(b));
    }
    std::string sub_sub(ArpRn1 a, ArpStep1 asi, ArpStep1 asj, Ab b) {
        return D("sub||sub", MemARPSJ(a, asj), MemARPSI(a, asi), R(b));
    }

    std::string moda4(Moda4 op, Ax a, Cond cond) {
        return D(op, R(a), cond);
    }

    std::string moda3(Moda3 op, Bx a, Cond cond) {
        return D(op, R(a), cond);
    }

    std::string bkrep(Imm8 a, Address16 addr) {
        return D("bkrep", a, addr);
    }
    std::string bkrep(Register a, Address18_16 addr_low, Address18_2 addr_high) {
        return D("bkrep", R(a), A18(addr_low, addr_high));
    }
    std::string bkrep_r6(Address18_16 addr_low, Address18_2 addr_high) {
        return D("bkrep", "r6", A18(addr_low, addr_high));
    }
    std::string bkreprst(ArRn2 a) {
        return D("bkreprst", MemAR(a));
    }
    std::string bkreprst_memsp(Dummy) {
        return D("bkreprst", "[sp]");
    }
    std::string bkrepsto(ArRn2 a) {
        return D("bkrepsto", MemAR(a));
    }
    std::string bkrepsto_memsp(Dummy) {
        return D("bkrepsto", "[sp]");
    }

    std::string banke(BankFlags flags) {
        std::string s;
        if (flags.storage & 1) s += " r0";
        if (flags.storage & 2) s += " r1";
        if (flags.storage & 4) s += " r4";
        if (flags.storage & 8) s += " cfgi";
        if (flags.storage & 16) s += " r7";
        if (flags.storage & 32) s += " cfgj";
        return "banke" + s;
    }
    std::string bankr(Dummy) {
        return "bankr";
    }
    std::string bankr(Ar a) {
        return D("bankr", R(a));
    }
    std::string bankr(Ar a, Arp b) {
        return D("bankr", R(a), R(b));
    }
    std::string bankr(Arp a) {
        return D("bankr", R(a));
    }

    std::string bitrev(Rn a) {
        return D("bitrev", R(a));
    }
    std::string bitrev_dbrv(Rn a) {
        return D("bitrev", R(a), "dbrv");
    }
    std::string bitrev_ebrv(Rn a) {
        return D("bitrev", R(a), "ebrv");
    }

    std::string br(Address18_16 addr_low, Address18_2 addr_high, Cond cond) {
        return D("br", A18(addr_low, addr_high), cond);
    }

    std::string brr(RelAddr7 addr, Cond cond) {
        return D("brr", ToHex(addr.storage), cond);
    }

    std::string break_(Dummy) {
        return "break";
    }

    std::string call(Address18_16 addr_low, Address18_2 addr_high, Cond cond) {
        return D("call", A18(addr_low, addr_high), cond);
    }
    std::string calla(Axl a) {
        return D("calla", R(a));
    }
    std::string calla(Ax a) {
        return D("calla", R(a));
    }
    std::string callr(RelAddr7 addr, Cond cond) {
        return D("callr", ToHex(addr.storage), cond);
    }

    std::string cntx_s(Dummy) {
        return "cntx s";
    }
    std::string cntx_r(Dummy) {
        return "cntx r";
    }

    std::string ret(Cond c) {
        return D("ret", c);
    }
    std::string retd(Dummy) {
        return "retd";
    }
    std::string reti(Cond c) {
        return D("reti", c);
    }
    std::string retic(Cond c) {
        return D("retic", c);
    }
    std::string retid(Dummy) {
        return "retid";
    }
    std::string retidc(Dummy) {
        return "retidc";
    }
    std::string rets(Imm8 a) {
        return D("rets", a);
    }

    std::string load_ps(Imm2 a) {
        return D("load", a, "ps");
    }
    std::string load_stepi(Imm7s a) {
        return D("load", a, "stepi");
    }
    std::string load_stepj(Imm7s a) {
        return D("load", a, "stepj");
    }
    std::string load_page(Imm8 a) {
        return D("load", a, "page");
    }
    std::string load_modi(Imm9 a) {
        return D("load", a, "modi");
    }
    std::string load_modj(Imm9 a) {
        return D("load", a, "modj");
    }
    std::string load_movpd(Imm2 a) {
        return D("load", a, "movpd");
    }
    std::string load_ps01(Imm4 a) {
        return D("load", a, "ps01");
    }

    std::string push(Imm16 a) {
        return D("push", a);
    }
    std::string push(Register a) {
        return D("push", R(a));
    }
    std::string push(Abe a) {
        return D("push", R(a));
    }
    std::string push(ArArpSttMod a) {
        return D("push", R(a));
    }
    std::string push_prpage(Dummy) {
        return D("push", "prpage");
    }
    std::string push(Px a) {
        return D("push", R(a));
    }
    std::string push_r6(Dummy) {
        return D("push", "r6");
    }
    std::string push_repc(Dummy) {
        return D("push", "repc");
    }
    std::string push_x0(Dummy) {
        return D("push", "x0");
    }
    std::string push_x1(Dummy) {
        return D("push", "x1");
    }
    std::string push_y1(Dummy) {
        return D("push", "y1");
    }
    std::string pusha(Ax a) {
        return D("pusha", R(a));
    }
    std::string pusha(Bx a) {
        return D("pusha", R(a));
    }

    std::string pop(Register a) {
        return D("pop", R(a));
    }
    std::string pop(Abe a) {
        return  D("pop", R(a));
    }
    std::string pop(ArArpSttMod a) {
        return  D("pop", R(a));
    }
    std::string pop(Bx a) {
        return D("pop", R(a));
    }
    std::string pop_prpage(Dummy) {
        return  D("pop", "prpage");
    }
    std::string pop(Px a) {
        return  D("pop", R(a));
    }
    std::string pop_r6(Dummy) {
        return D("pop", "r6");
    }
    std::string pop_repc(Dummy) {
        return D("pop", "repc");
    }
    std::string pop_x0(Dummy) {
        return D("pop", "x0");
    }
    std::string pop_x1(Dummy) {
        return D("pop", "x1");
    }
    std::string pop_y1(Dummy) {
        return D("pop", "y1");
    }
    std::string popa(Ab a) {
        return D("popa", R(a));
    }

    std::string rep(Imm8 a) {
        return D("rep", a);
    }
    std::string rep(Register a) {
        return D("rep", R(a));
    }
    std::string rep_r6(Dummy) {
        return D("rep", "r6");
    }

    std::string shfc(Ab a, Ab b, Cond cond) {
        return D("shfc", R(a), R(b), cond);
    }
    std::string shfi(Ab a, Ab b, Imm6s s) {
        return D("shfi", R(a), R(b), s);
    }

    std::string tst4b(ArRn2 b, ArStep2 bs) {
        return D("tst4b", "a0l", MemARS(b, bs));
    }
    std::string tst4b(ArRn2 b, ArStep2 bs, Ax c) {
        return D("tst4b", "a0l", MemARS(b, bs), R(c));
    }
    std::string tstb(MemImm8 a, Imm4 b) {
        return D("tstb", a, b);
    }
    std::string tstb(Rn a, StepZIDS as, Imm4 b) {
        return D("tstb", MemR(a, as), b);
    }
    std::string tstb(Register a, Imm4 b) {
        return D("tstb", R(a), b);
    }
    std::string tstb_r6(Imm4 b) {
       return D("tstb", "r6", b);
    }
    std::string tstb(SttMod a, Imm16 b) {
        return D("tstb", R(a), b);
    }

    std::string and_(Ab a, Ab b, Ax c) {
        return D("and", R(a), R(b), R(c));
    }

    std::string dint(Dummy) {
        return "dint";
    }
    std::string eint(Dummy) {
        return "eint";
    }

    std::string mul(Mul3 op, Rn y, StepZIDS ys, Imm16 x, Ax a) {
        return D(op, MemR(y, ys), x, R(a));
    }
    std::string mul_y0(Mul3 op, Rn x, StepZIDS xs, Ax a) {
        return D(op, "y0", MemR(x, xs), R(a));
    }
    std::string mul_y0(Mul3 op, Register x, Ax a) {
        return D(op, "y0", R(x), R(a));
    }
    std::string mul(Mul3 op, R45 y, StepZIDS ys, R0123 x, StepZIDS xs, Ax a) {
        return D(op, MemR(y, ys), MemR(x, xs), R(a));
    }
    std::string mul_y0_r6(Mul3 op, Ax a) {
        return D(op, "y0", "r6", R(a));
    }
    std::string mul_y0(Mul2 op, MemImm8 x, Ax a) {
        return D(op, "y0", x, R(a));
    }

    std::string mpyi(Imm8s x) {
        return D("mpyi", "y0", x);
    }

    std::string modr(Rn a, StepZIDS as) {
        return D("modr", MemR(a, as));
    }
    std::string modr_dmod(Rn a, StepZIDS as) {
        return D("modr", MemR(a, as), "dmod");
    }
    std::string modr_i2(Rn a) {
        return D("modr", R(a), "+2");
    }
    std::string modr_i2_dmod(Rn a)  {
        return D("modr", R(a), "+2", "dmod");
    }
    std::string modr_d2(Rn a)  {
        return D("modr", R(a), "-2");
    }
    std::string modr_d2_dmod(Rn a)  {
        return D("modr", R(a), "-2", "dmod");
    }
    std::string modr_eemod(ArpRn2 a, ArpStep2 asi, ArpStep2 asj) {
        return D("modr", MemARPSI(a, asi), MemARPSI(a, asj), "eemod");
    }
    std::string modr_edmod(ArpRn2 a, ArpStep2 asi, ArpStep2 asj) {
        return D("modr", MemARPSI(a, asi), MemARPSI(a, asj), "edmod");
    }
    std::string modr_demod(ArpRn2 a, ArpStep2 asi, ArpStep2 asj) {
        return D("modr", MemARPSI(a, asi), MemARPSI(a, asj), "demod");
    }
    std::string modr_ddmod(ArpRn2 a, ArpStep2 asi, ArpStep2 asj) {
        return D("modr", MemARPSI(a, asi), MemARPSI(a, asj), "ddmod");
    }

    std::string movd(R0123 a, StepZIDS as, R45 b, StepZIDS bs) {
        return D("mov d->p", MemR(a, as), MemR(b, bs));
    }
    std::string movp(Axl a, Register b) {
        return D("mov p->r", MemG(a), R(b));
    }
    std::string movp(Ax a, Register b) {
        return D("mov p->r", MemG(a), R(b));
    }
    std::string movp(Rn a, StepZIDS as, R0123 b, StepZIDS bs) {
        return D("mov p->d", MemR(a, as), MemR(b, bs));
    }
    std::string movpdw(Ax a) {
        return D("mov p->pc", MemG(a));
    }

    std::string mov(Ab a, Ab b) {
        return D("mov", R(a), R(b));
    }
    std::string mov_dvm(Abl a) {
        return D("mov", R(a), "dvm");
    }
    std::string mov_x0(Abl a) {
        return D("mov", R(a), "x0");
    }
    std::string mov_x1(Abl a) {
         return D("mov", R(a), "x1");
    }
    std::string mov_y1(Abl a) {
        return D("mov", R(a), "y1");
    }
    std::string mov(Ablh a, MemImm8 b) {
        return D("mov", R(a), b);
    }
    std::string mov(Axl a, MemImm16 b) {
        return D("mov", R(a), b);
    }
    std::string mov(Axl a, MemR7Imm16 b) {
        return D("mov", R(a), b);
    }
    std::string mov(Axl a, MemR7Imm7s b) {
        return D("mov", R(a), b);
    }
    std::string mov(MemImm16 a, Ax b) {
        return D("mov", a, R(b));
    }
    std::string mov(MemImm8 a, Ab b) {
        return D("mov", a, R(b));
    }
    std::string mov(MemImm8 a, Ablh b) {
        return D("mov", a, R(b));
    }
    std::string mov_eu(MemImm8 a, Axh b) {
        return D("mov", a, R(b), "eu");
    }
    std::string mov(MemImm8 a, RnOld b) {
        return D("mov", a, R(b));
    }
    std::string mov_sv(MemImm8 a) {
       return D("mov", a, "sv");
    }
    std::string mov_dvm_to(Ab b) {
        return D("mov", "dvm", R(b));
    }
    std::string mov_icr_to(Ab b) {
        return D("mov", "icr", R(b));
    }
    std::string mov(Imm16 a, Bx b) {
        return D("mov", a, R(b));
    }
    std::string mov(Imm16 a, Register b) {
        return D("mov", a, R(b));
    }
    std::string mov_icr(Imm5 a) {
        return D("mov", a, "icr");
    }
    std::string mov(Imm8s a, Axh b) {
        return D("mov", a, R(b));
    }
    std::string mov(Imm8s a, RnOld b) {
        return D("mov", a, R(b));
    }
    std::string mov_sv(Imm8s a) {
        return D("mov", a, "sv");
    }
    std::string mov(Imm8 a, Axl b) {
        return D("mov", a, R(b));
    }
    std::string mov(MemR7Imm16 a, Ax b) {
        return D("mov", a, R(b));
    }
    std::string mov(MemR7Imm7s a, Ax b) {
        return D("mov", a, R(b));
    }
    std::string mov(Rn a, StepZIDS as, Bx b) {
        return D("mov", MemR(a, as), R(b));
    }
    std::string mov(Rn a, StepZIDS as, Register b) {
        return D("mov", MemR(a, as), R(b));
    }
    std::string mov_memsp_to(Register b) {
        return D("mov", "[sp]", R(b));
    }
    std::string mov_mixp_to(Register b) {
        return D("mov", "mixp", R(b));
    }
    std::string mov(RnOld a, MemImm8 b) {
        return D("mov", R(a), b);
    }
    std::string mov_icr(Register a) {
        return D("mov", R(a), "icr");
    }
    std::string mov_mixp(Register a) {
        return D("mov", R(a), "mixp");
    }
    std::string mov(Register a, Rn b, StepZIDS bs) {
        return D("mov", R(a), MemR(b, bs));
    }
    std::string mov(Register a, Bx b) {
        return D("mov", R(a), R(b));
    }
    std::string mov(Register a, Register b) {
        return D("mov", R(a), R(b));
    }
    std::string mov_repc_to(Ab b) {
        return D("mov", "repc", R(b));
    }
    std::string mov_sv_to(MemImm8 b) {
        return D("mov", "sv", b);
    }
    std::string mov_x0_to(Ab b) {
        return D("mov", "x0", R(b));
    }
    std::string mov_x1_to(Ab b) {
        return D("mov", "x1", R(b));
    }
    std::string mov_y1_to(Ab b) {
        return D("mov", "y1", R(b));
    }
    std::string mov(Imm16 a, ArArp b) {
        return D("mov", a, R(b));
    }
    std::string mov_r6(Imm16 a) {
        return D("mov", a, "r6");
    }
    std::string mov_repc(Imm16 a) {
        return D("mov", a, "repc");
    }
    std::string mov_stepi0(Imm16 a) {
        return D("mov", a, "stepi0");
    }
    std::string mov_stepj0(Imm16 a) {
        return D("mov", a, "stepj0");
    }
    std::string mov(Imm16 a, SttMod b) {
        return D("mov", a, R(b));
    }
    std::string mov_prpage(Imm4 a) {
        return D("mov", a, "prpage");
    }

    std::string mov_a0h_stepi0(Dummy) {
        return D("mov", "a0h", "stepi0");
    }
    std::string mov_a0h_stepj0(Dummy) {
        return D("mov", "a0h", "stepj0");
    }
    std::string mov_stepi0_a0h(Dummy) {
        return D("mov", "stepi0", "a0h");
    }
    std::string mov_stepj0_a0h(Dummy) {
        return D("mov", "stepi0", "a1h");
    }

    std::string mov_prpage(Abl a) {
        return D("mov", R(a), "prpage");
    }
    std::string mov_repc(Abl a) {
        return D("mov", R(a), "repc");
    }
    std::string mov(Abl a, ArArp b) {
        return D("mov", R(a), R(b));
    }
    std::string mov(Abl a, SttMod b) {
        return D("mov", R(a), R(b));
    }

    std::string mov_prpage_to(Abl b) {
        return D("mov", "prpage", R(b));
    }
    std::string mov_repc_to(Abl b) {
        return D("mov", "repc", R(b));
    }
    std::string mov(ArArp a, Abl b) {
        return D("mov", R(a), R(b));
    }
    std::string mov(SttMod a, Abl b) {
        return D("mov", R(a), R(b));
    }

    std::string mov_repc_to(ArRn1 b, ArStep1 bs) {
        return D("mov", "repc", MemARS(b, bs));
    }
    std::string mov(ArArp a, ArRn1 b, ArStep1 bs) {
        return D("mov", R(a), MemARS(b, bs));
    }
    std::string mov(SttMod a, ArRn1 b, ArStep1 bs) {
        return D("mov", R(a), MemARS(b, bs));
    }

    std::string mov_repc(ArRn1 a, ArStep1 as) {
        return D("mov", MemARS(a, as), "repc");
    }
    std::string mov(ArRn1 a, ArStep1 as, ArArp b) {
        return D("mov", MemARS(a, as), R(b));
    }
    std::string mov(ArRn1 a, ArStep1 as, SttMod b) {
        return D("mov", MemARS(a, as), R(b));
    }

    std::string mov_repc_to(MemR7Imm16 b) {
        return D("mov", "repc", b);
    }
    std::string mov(ArArpSttMod a, MemR7Imm16 b) {
        return D("mov", R(a), b);
    }

    std::string mov_repc(MemR7Imm16 a) {
        return D("mov", a, "repc");
    }
    std::string mov(MemR7Imm16 a, ArArpSttMod b) {
        return D("mov", a, R(b));
    }

    std::string mov_pc(Ax a) {
        return D("mov", R(a), "pc");
    }
    std::string mov_pc(Bx a) {
        return D("mov", R(a), "pc");
    }

    std::string mov_mixp_to(Bx b) {
        return D("mov", "mixp", R(b));
    }
    std::string mov_mixp_r6(Dummy) {
        return D("mov", "mixp", "r6");
    }
    std::string mov_p0h_to(Bx b) {
        return D("mov", "p0h", R(b));
    }
    std::string mov_p0h_r6(Dummy) {
        return D("mov", "p0h", "r6");
    }
    std::string mov_p0h_to(Register b) {
        return D("mov", "p0h", R(b));
    }
    std::string mov_p0(Ab a) {
        return D("mov", R(a), "p0");
    }
    std::string mov_p1_to(Ab b) {
        return D("mov", "p1", R(b));
    }

    std::string mov2(Px a, ArRn2 b, ArStep2 bs) {
        return D("mov", R(a), MemARS(b, bs));
    }
    std::string mov2s(Px a, ArRn2 b, ArStep2 bs) {
        return D("mov s", R(a), MemARS(b, bs));
    }
    std::string mov2(ArRn2 a, ArStep2 as, Px b) {
        return D("mov", MemARS(a, as), R(b));
    }
    std::string mova(Ab a, ArRn2 b, ArStep2 bs) {
        return D("mov", R(a), MemARS(b, bs));
    }
    std::string mova(ArRn2 a, ArStep2 as, Ab b) {
        return D("mov", MemARS(a, as), R(b));
    }

    std::string mov_r6_to(Bx b) {
        return D("mov", "r6", R(b));
    }
    std::string mov_r6_mixp(Dummy) {
        return D("mov", "r6", "mixp");
    }
    std::string mov_r6_to(Register b) {
        return D("mov", "r6", R(b));
    }
    std::string mov_r6(Register a) {
        return D("mov", R(a), "r6");
    }
    std::string mov_memsp_r6(Dummy) {
        return D("mov", "[sp]", "r6");
    }
    std::string mov_r6_to(Rn b, StepZIDS bs) {
        return D("mov", "r6", MemR(b, bs));
    }
    std::string mov_r6(Rn a, StepZIDS as) {
        return D("mov", MemR(a, as), "r6");
    }

    std::string movs(MemImm8 a, Ab b) {
        return D("movs", a, R(b));
    }
    std::string movs(Rn a, StepZIDS as, Ab b) {
        return D("movs", MemR(a, as), R(b));
    }
    std::string movs(Register a, Ab b) {
        return D("movs", R(a), R(b));
    }
    std::string movs_r6_to(Ax b) {
        return D("movs", "r6", R(b));
    }
    std::string movsi(RnOld a, Ab b, Imm5s s) {
        return D("movsi", R(a), R(b), s);
    }

    std::string exp(Bx a) {
        return D("exp", R(a));
    }
    std::string exp(Bx a, Ax b) {
        return D("exp", R(a), R(b));
    }
    std::string exp(Rn a, StepZIDS as) {
        return D("exp", MemR(a, as));
    }
    std::string exp(Rn a, StepZIDS as, Ax b) {
        return D("exp", MemR(a, as), R(b));
    }
    std::string exp(Register a) {
        return D("exp", R(a));
    }
    std::string exp(Register a, Ax b) {
        return D("exp", R(a), R(b));
    }
    std::string exp_r6(Dummy) {
        return D("exp", "r6");
    }
    std::string exp_r6(Ax b) {
        return D("exp", "r6", R(b));
    }

    std::string lim(Ax a, Ax b) {
        return D("lim", R(a), R(b));
    }

    std::string vtrclr0(Dummy) {
        return D("vtrclr0");
    }
    std::string vtrclr1(Dummy) {
        return D("vtrclr1");
    }
    std::string vtrclr(Dummy) {
        return D("vtrclr");
    }
    std::string vtrmov0(Axl a) {
        return D("vtrmov0", R(a));
    }
    std::string vtrmov1(Axl a) {
        return D("vtrmov1", R(a));
    }
    std::string vtrmov(Axl a) {
        return D("vtrmov", R(a));
    }
    std::string vtrshr(Dummy) {
        return D("vtrshr");
    }

    std::string clrp0(Dummy) {
        return D("clr0");
    }
    std::string clrp1(Dummy) {
        return D("clr1");
    }
    std::string clrp(Dummy) {
        return D("clr");
    }

};

bool NeedExpansion(std::uint16_t opcode) {
    auto decoder = Decode<Disassembler>(opcode);
    return decoder.NeedExpansion();
}

std::string Do(std::uint16_t opcode, std::uint16_t expansion) {
    Disassembler dsm;
    auto decoder = Decode<Disassembler>(opcode);
    return decoder.call(dsm, opcode, expansion);
}

} // namespace Teakra::dissasembler
