#pragma once

#include <iomanip>
#include <sstream>
#include <string>

#include "common_types.h"
#include "oprand.h"

template<typename T>
std::string ToHex(T i)
{
  bool neg;
  u64 v;
  if (i < 0) {
      v = -i;
      neg = true;
  } else {
      v = i;
      neg = false;
  }
  std::stringstream stream;
  if (neg) stream << "-";
  stream << "0x"
         << std::setfill ('0') << std::setw(sizeof(T) * 2)
         << std::hex << v;
  return stream.str();
}

std::string Dsm(MemImm8 a) {
    return "[page:" + ToHex((u8)a.Value()) + "]";
}

std::string Dsm(MemImm16 a) {
    return "[" + ToHex(a.Value()) + "]";
}

std::string Dsm(MemR7Imm16 a) {
    return "[r7+" + ToHex(a.Value()) + "]";
}
std::string Dsm(MemR7Imm7s a) {
    return "[r7+" + ToHex(a.Value()) + "]";
}

template <typename ImmT>
std::string DsmImm(ImmT a) {
    return ToHex(a.Value());
}

template <typename ArRn>
std::string DsmArRn(ArRn a) {
    return "arrn" + ToHex(a.storage);
}

template <typename ArStep>
std::string DsmArStep(ArStep a) {
    return "+ars" + ToHex(a.storage);
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

std::string Dsm(StepZIDS step) {
    switch (step.GetName()) {
    case StepValue::Zero: return "";
    case StepValue::Increase: return "++";
    case StepValue::Decrease: return "--";
    case StepValue::PlusStep: return "++s";
    default: throw "what";
    }
}

class Disassembler {
public:
    using instruction_return_type = std::string;

    std::string nop(Dummy) {
        return "nop";
    }

    std::string norm(Ax a, Rn b, StepZIDS bs) {
        return "norm " + DsmReg(a) + " " + DsmReg(b) + Dsm(bs);
    }
    //INST(swap, 0x4980, At<SwapTypes, 0>),
    std::string trap(Dummy) {
        return "trap";
    }

    std::string alm(Alm op, MemImm8 a, Ax b) {
        return Dsm(op) + " " + Dsm(a) + " " + DsmReg(b);
    }
    std::string alm(Alm op, Rn a, StepZIDS as, Ax b) {
        return Dsm(op) + " [" + DsmReg(a) + Dsm(as) + "] " + DsmReg(b);
    }
    std::string alm(Alm op, Register a, Ax b) {
        return Dsm(op) + " " + DsmReg(a) + " " + DsmReg(b);
    }
    std::string alm_r6(Alm op, Ax b) {
        return Dsm(op) + " r6 " + DsmReg(b);
    }

    std::string alu(Alu op, MemImm16 a, Ax b) {
        return Dsm(op) + " " + Dsm(a) + " " + DsmReg(b);
    }
    std::string alu(Alu op, MemR7Imm16 a, Ax b) {
        return Dsm(op) + " " + Dsm(a) + " " + DsmReg(b);
    }
    std::string alu(Alu op, Imm16 a, Ax b) {
        return Dsm(op) + " " + DsmImm(a) + " " + DsmReg(b);
    }
    std::string alu(Alu op, Imm8 a, Ax b) {
        return Dsm(op) + " " + DsmImm(a) + " " + DsmReg(b);
    }
    std::string alu(Alu op, MemR7Imm7s a, Ax b) {
        return Dsm(op) + " " + Dsm(a) + " " + DsmReg(b);
    }

    std::string or_(Ab a, Ax b, Ax c) {
        return "or " + DsmReg(a) + " " + DsmReg(b) + " " + DsmReg(c);
    }
    std::string or_(Ax a, Bx b, Ax c) {
        return "or " + DsmReg(a) + " " + DsmReg(b) + " " + DsmReg(c);
    }
    std::string or_(Bx a, Bx b, Ax c){
        return "or " + DsmReg(a) + " " + DsmReg(b) + " " + DsmReg(c);
    }

    std::string alb(Alb op, Imm16 a, MemImm8 b) {
        return Dsm(op) + " " + DsmImm(a) + " " + Dsm(b);
    }
    std::string alb(Alb op, Imm16 a, Rn b, StepZIDS bs) {
        return Dsm(op) + " " + DsmImm(a) + " [" + DsmReg(b) + Dsm(bs) + "]";
    }
    std::string alb(Alb op, Imm16 a, Register b) {
        return Dsm(op) + " " + DsmImm(a) + " " + DsmReg(b);
    }
    std::string alb_r6(Alb op, Imm16 a) {
        return Dsm(op) + " " + DsmImm(a) + " r6";
    }
    std::string alb(Alb op, Imm16 a, SttMod b) {
        return Dsm(op) + " " + DsmImm(a) + " " + DsmReg(b);
    }

    std::string add(Ab a, Bx b) {
        return "";
    }
    std::string add(Bx a, Ax b) {
        return "";
    }
    std::string add_p1(Ax b) {
        return "";
    }
    std::string add(Px a, Bx b) {
        return "";
    }
    std::string add_p0_p1(Ab c) {
        return "";
    }
    std::string add_p0_p1a(Ab c) {
        return "";
    }
    std::string add3_p0_p1(Ab c) {
        return "";
    }
    std::string add3_p0_p1a(Ab c) {
        return "";
    }
    std::string add3_p0a_p1a(Ab c) {
        return "";
    }

    std::string sub(Ab a, Bx b) {
        return "";
    }
    std::string sub(Bx a, Ax b) {
        return "";
    }
    std::string sub_p1(Ax b) {
        return "";
    }
    std::string sub(Px a, Bx b) {
        return "";
    }
    std::string sub_p0_p1(Ab c) {
        return "";
    }
    std::string sub_p0_p1a(Ab c) {
        return "";
    }
    std::string sub3_p0_p1(Ab c) {
        return "";
    }
    std::string sub3_p0_p1a(Ab c) {
        return "";
    }
    std::string sub3_p0a_p1a(Ab c) {
        return "";
    }

    std::string moda4(Moda4 op, Ax a, Cond cond) {
        return Dsm(op) + " " + DsmReg(a) + " " + Dsm(cond);
    }

    std::string moda3(Moda3 op, Bx a, Cond cond) {
        return Dsm(op) + " " + DsmReg(a) + " " + Dsm(cond);
    }

    std::string bkrep(Imm8 a, Address16 addr) {
        return "bkrep " + DsmImm(a) + " " + Dsm(addr);
    }
    std::string bkrep(Register a, Address18_16 addr_low, Address18_2 addr_high) {
        return "bkrep " + DsmReg(a) + " " + Dsm(addr_low, addr_high);
    }
    std::string bkrep_r6(Address18_16 addr_low, Address18_2 addr_high) {
        return "bkrep r6 " + Dsm(addr_low, addr_high);
    }
    std::string bkreprst(ArRn2 a) {
        return "bkreprst [" + DsmArRn(a) + "]";
    }
    std::string bkreprst_memsp(Dummy) {
        return "bkreprst [sp]";
    }
    std::string bkrepsto(ArRn2 a) {
        return "bkrepsto [" + DsmArRn(a) + "]";
    }
    std::string bkrepsto_memsp(Dummy) {
        return "bkrepsto [sp]";
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
        return "bankr " + DsmReg(a);
    }
    std::string bankr(Ar a, Arp b) {
        return "bankr " + DsmReg(a) + " " + DsmReg(b);
    }
    std::string bankr(Arp a) {
        return "bankr " + DsmReg(a);
    }

    std::string bitrev(Rn a) {
        return "bitrev " + DsmReg(a);
    }
    std::string bitrev_dbrv(Rn a) {
        return "bitrev " + DsmReg(a) + " dbrv";
    }
    std::string bitrev_ebrv(Rn a) {
        return "bitrev " + DsmReg(a) + " ebrv";
    }

    std::string br(Address18_16 addr_low, Address18_2 addr_high, Cond cond) {
        return "br " + Dsm(addr_low, addr_high) + " " + Dsm(cond);
    }

    std::string brr(RelAddr7 addr, Cond cond) {
        return "brr " + ToHex(addr.storage) + " " + Dsm(cond);
    }

    std::string break_(Dummy) {
        return "break";
    }

    std::string call(Address18_16 addr_low, Address18_2 addr_high, Cond cond) {
        return "call " + Dsm(addr_low, addr_high) + " " + Dsm(cond);
    }
    std::string calla(Axl a) {
        return "calla " + DsmReg(a);
    }
    std::string calla(Ax a) {
        return "calla " + DsmReg(a);
    }
    std::string callr(RelAddr7 addr, Cond cond) {
        return "callr " + ToHex(addr.storage) + " " + Dsm(cond);
    }

    std::string cntx_s(Dummy) {
        return "cntx s";
    }
    std::string cntx_r(Dummy) {
        return "cntx r";
    }

    std::string ret(Cond c) {
        return "ret " + Dsm(c);
    }
    std::string retd(Dummy) {
        return "retd";
    }
    std::string reti(Cond c) {
        return "reti " + Dsm(c);
    }
    std::string retic(Cond c) {
        return "retic " + Dsm(c);
    }
    std::string retid(Dummy) {
        return "retid";
    }
    std::string retidc(Dummy) {
        return "retidc";
    }
    std::string rets(Imm8 a) {
        return "rets " + DsmImm(a);
    }

    std::string load_ps(Imm2 a) {
        return "load " + DsmImm(a) + " ps";
    }
    std::string load_stepi(Imm7s a) {
        return "load " + DsmImm(a) + " stepi";
    }
    std::string load_stepj(Imm7s a) {
        return "load " + DsmImm(a) + " stepj";
    }
    std::string load_page(Imm8 a) {
        return "load " + DsmImm(a) + " page";
    }
    std::string load_modi(Imm9 a) {
        return "load " + DsmImm(a) + " modi";
    }
    std::string load_modj(Imm9 a) {
        return "load " + DsmImm(a) + " modj";
    }
    std::string load_movpd(Imm2 a) {
        return "load " + DsmImm(a) + " movpd";
    }
    std::string load_ps01(Imm4 a) {
        return "load " + DsmImm(a) + " ps01";
    }

    std::string push(Imm16 a) {
        return "push " + DsmImm(a);
    }
    std::string push(Register a) {
        return "push " + DsmReg(a);
    }
    std::string push(Abe a) {
        return "push " + DsmReg(a);
    }
    std::string push(ArArpSttMod a) {
        return "push " + DsmReg(a);
    }
    std::string push_prpage(Dummy) {
        return "push prpage";
    }
    std::string push(Px a) {
        return "push " + DsmReg(a);
    }
    std::string push_r6(Dummy) {
        return "push r6";
    }
    std::string push_repc(Dummy) {
        return "push repc";
    }
    std::string push_x0(Dummy) {
        return "push x0";
    }
    std::string push_x1(Dummy) {
        return "push x1";
    }
    std::string push_y1(Dummy) {
        return "push y1";
    }
    std::string pusha(Ax a) {
        return "pusha " + DsmReg(a);
    }
    std::string pusha(Bx a) {
        return "pusha " + DsmReg(a);
    }

    std::string pop(Register a) {
        return "pop " + DsmReg(a);
    }
    std::string pop(Abe a) {
        return "pop " + DsmReg(a);
    }
    std::string pop(ArArpSttMod a) {
        return "pop " + DsmReg(a);
    }
    std::string pop(Bx a) {
        return "pop " + DsmReg(a);
    }
    std::string pop_prpage(Dummy) {
        return "pop prpage";
    }
    std::string pop(Px a) {
        return "pop " + DsmReg(a);
    }
    std::string pop_r6(Dummy) {
        return "pop r6";
    }
    std::string pop_repc(Dummy) {
        return "pop repc";
    }
    std::string pop_x0(Dummy) {
        return "pop x0";
    }
    std::string pop_x1(Dummy) {
        return "pop x1";
    }
    std::string pop_y1(Dummy) {
        return "pop y1";
    }
    std::string popa(Ab a) {
        return "popa " + DsmReg(a);
    }

    std::string rep(Imm8 a) {
        return "rep " + DsmImm(a);
    }
    std::string rep(Register a) {
        return "rep " + DsmReg(a);
    }
    std::string rep_r6(Dummy) {
        return "rep r6";
    }

    std::string shfc(Ab a, Ab b, Cond cond) {
        return "shfc " + DsmReg(a) + " " + DsmReg(b) + " " + Dsm(cond);
    }
    std::string shfi(Ab a, Ab b, Imm6s s) {
        return "shfi " + DsmReg(a) + " " + DsmReg(b) + " " + DsmImm(s);
    }

    std::string tst4b(ArRn2 b, ArStep2 bs) {
        return "tst4b a0l [" + DsmArRn(b) + DsmArStep(bs) + "]";
    }
    std::string tst4b(ArRn2 b, ArStep2 bs, Ax c) {
        return "tst4b a0l [" + DsmArRn(b) + DsmArStep(bs) + "] " + DsmReg(c);
    }
    std::string tstb(MemImm8 a, Imm4 b) {
        return "tstb " + Dsm(a) + " " + DsmImm(b);
    }
    std::string tstb(Rn a, StepZIDS as, Imm4 b) {
        return "tstb [" + DsmReg(a) + Dsm(as) + "] " + DsmImm(b);
    }
    std::string tstb(Register a, Imm4 b) {
        return "tstb " + DsmReg(a) + " " + DsmImm(b);
    }
    std::string tstb_r6(Imm4 b) {
        return "tstb r6 " + DsmImm(b);
    }
    std::string tstb(SttMod a, Imm16 b) {
        return "tstb " + DsmReg(a) + " " + DsmImm(b);
    }

    std::string and_(Ab a, Ab b, Ax c) {
        return "and " + DsmReg(a) + " " + DsmReg(b) + " " + DsmReg(c);
    }

    std::string dint(Dummy) {
        return "dint";
    }
    std::string eint(Dummy) {
        return "eint";
    }

    std::string mul(Mul3 op, Rn y, StepZIDS ys, Imm16 x, Ax a) {
        return "";
    }
    std::string mul_y0(Mul3 op, Rn x, StepZIDS xs, Ax a) {
        return "";
    }
    std::string mul_y0(Mul3 op, Register x, Ax a) {
        return "";
    }
    std::string mul(Mul3 op, R45 y, StepZIDS ys, R0123 x, StepZIDS xs, Ax a) {
        return "";
    }
    std::string mul_y0_r6(Mul3 op, Ax a) {
        return "";
    }
    std::string mul_y0(Mul2 op, MemImm8 x, Ax a) {
        return "";
    }

    std::string mpyi(Imm8s x) {
        return "";
    }

    std::string modr(Rn a, StepZIDS as) {
        return "";
    }
    std::string modr_dmod(Rn a, StepZIDS as) {
        return "";
    }
    std::string modr_i2(Rn a) {
        return "";
    }
    std::string modr_i2_dmod(Rn a)  {
        return "";
    }
    std::string modr_d2(Rn a)  {
        return "";
    }
    std::string modr_d2_dmod(Rn a)  {
        return "";
    }
    std::string modr_eemod(ArpRn2 a, ArpStep2 asi, ArpStep2 asj) {
        return "";
    }
    std::string modr_edmod(ArpRn2 a, ArpStep2 asi, ArpStep2 asj) {
        return "";
    }
    std::string modr_demod(ArpRn2 a, ArpStep2 asi, ArpStep2 asj) {
        return "";
    }
    std::string modr_ddmod(ArpRn2 a, ArpStep2 asi, ArpStep2 asj) {
        return "";
    }

    std::string movd(R0123 a, StepZIDS as, R45 b, StepZIDS bs) {
        return "movd [" + DsmReg(a) + Dsm(as) + "] [Prog:" + DsmReg(b) + Dsm(bs) + "]";
    }
    std::string movp(Axl a, Register b) {
        return "movp [Prog:" + DsmReg(a) + "] " + DsmReg(b);
    }
    std::string movp(Ax a, Register b) {
        return "movp [Prog:" + DsmReg(a) + "] " + DsmReg(b);
    }
    std::string movp(Rn a, StepZIDS as, R0123 b, StepZIDS bs) {
        return "movd [Prog:" + DsmReg(a) + Dsm(as) + "] [" + DsmReg(b) + Dsm(bs) + "]";
    }
    std::string movpdw(Ax a) {
        return "movpdw [Prog:" + DsmReg(a) +",+1] pc";
    }

    std::string mov(Ab a, Ab b) {
        return "mov " + DsmReg(a) + " " + DsmReg(b);
    }
    std::string mov_dvm(Abl a) {
        return "mov " + DsmReg(a) + " dvm";
    }
    std::string mov_x0(Abl a) {
        return "mov " + DsmReg(a) + " x0";
    }
    std::string mov_x1(Abl a) {
        return "mov " + DsmReg(a) + " x1" ;
    }
    std::string mov_y1(Abl a) {
        return "mov " + DsmReg(a) + " y1" ;
    }
    std::string mov(Ablh a, MemImm8 b) {
        return "mov " + DsmReg(a) + " " +  Dsm(b);
    }
    std::string mov(Axl a, MemImm16 b) {
        return "mov " + DsmReg(a) + " " +  Dsm(b);
    }
    std::string mov(Axl a, MemR7Imm16 b) {
        return "mov " + DsmReg(a) + " " +  Dsm(b);
    }
    std::string mov(Axl a, MemR7Imm7s b) {
        return "mov " + DsmReg(a) + " " +  Dsm(b);
    }
    std::string mov(MemImm16 a, Ax b) {
        return "mov " + Dsm(a) + " " +  DsmReg(b);
    }
    std::string mov(MemImm8 a, Ab b) {
        return "mov " + Dsm(a) + " " +  DsmReg(b);
    }
    std::string mov(MemImm8 a, Ablh b) {
        return "mov " + Dsm(a) + " " +  DsmReg(b);
    }
    std::string mov_eu(MemImm8 a, Axh b) {
        return "mov " + Dsm(a) + " " +  DsmReg(b) + ",eu";
    }
    std::string mov(MemImm8 a, RnOld b) {
        return "mov " + Dsm(a) + " " +  DsmReg(b);
    }
    std::string mov_sv(MemImm8 a) {
        return "mov " + Dsm(a) + " sv";
    }
    std::string mov_dvm_to(Ab b) {
        return "mov dvm " +  DsmReg(b);
    }
    std::string mov_icr_to(Ab b) {
        return "mov icr " +  DsmReg(b);
    }
    std::string mov(Imm16 a, Bx b) {
        return "mov " + DsmImm(a) + " " +  DsmReg(b);
    }
    std::string mov(Imm16 a, Register b) {
        return "mov " + DsmImm(a) + " " +  DsmReg(b);
    }
    std::string mov_icr(Imm5 a) {
        return "mov " + DsmImm(a) + " icr";
    }
    std::string mov(Imm8s a, Axh b) {
        return "mov " + DsmImm(a) + " " +  DsmReg(b);
    }
    std::string mov(Imm8s a, RnOld b) {
        return "mov " + DsmImm(a) + " " +  DsmReg(b);
    }
    std::string mov_sv(Imm8s a) {
        return "mov " + DsmImm(a) + " sv";
    }
    std::string mov(Imm8 a, Axl b) {
        return "mov " + DsmImm(a) + " " +  DsmReg(b);
    }
    std::string mov(MemR7Imm16 a, Ax b) {
        return "mov " + Dsm(a) + " " +  DsmReg(b);
    }
    std::string mov(MemR7Imm7s a, Ax b) {
        return "mov " + Dsm(a) + " " +  DsmReg(b);
    }
    std::string mov(Rn a, StepZIDS as, Bx b) {
        return "mov [" + DsmReg(a) + Dsm(as) + "] " + DsmReg(b);
    }
    std::string mov(Rn a, StepZIDS as, Register b) {
        return "mov [" + DsmReg(a) + Dsm(as) + "] " + DsmReg(b);
    }
    std::string mov_memsp_to(Register b) {
        return "mov [sp] " +  DsmReg(b);
    }
    std::string mov_mixp_to(Register b) {
        return "mov mixp " +  DsmReg(b);
    }
    std::string mov(RnOld a, MemImm8 b) {
        return "mov " + DsmReg(a) + " " +  Dsm(b);
    }
    std::string mov_icr(Register a) {
        return "mov " + DsmReg(a) + " icr";
    }
    std::string mov_mixp(Register a) {
        return "mov " + DsmReg(a) + " mixp";
    }
    std::string mov(Register a, Rn b, StepZIDS bs) {
        return "mov " + DsmReg(a) + " [" + DsmReg(b) + Dsm(bs) + "]";
    }
    std::string mov(Register a, Bx b) {
        return "mov " + DsmReg(a) + " " +  DsmReg(b);
    }
    std::string mov(Register a, Register b) {
        return "mov " + DsmReg(a) + " " +  DsmReg(b);
    }
    std::string mov_repc_to(Ab b) {
        return "mov repc " +  DsmReg(b);
    }
    std::string mov_sv_to(MemImm8 b) {
        return "mov sv " +  Dsm(b);
    }
    std::string mov_x0_to(Ab b) {
        return "mov x0 " +  DsmReg(b);
    }
    std::string mov_x1_to(Ab b) {
        return "mov x1 " +  DsmReg(b);
    }
    std::string mov_y1_to(Ab b) {
        return "mov y1 " +  DsmReg(b);
    }
    std::string mov(Imm16 a, ArArp b) {
        return "mov " + DsmImm(a) + " " + DsmReg(b);
    }
    std::string mov_r6(Imm16 a) {
        return "mov " + DsmImm(a) + " r6";
    }
    std::string mov_repc(Imm16 a) {
        return "mov " + DsmImm(a) + " repc";
    }
    std::string mov_stepi0(Imm16 a) {
        return "mov " + DsmImm(a) + " stepi0";
    }
    std::string mov_stepj0(Imm16 a) {
        return "mov " + DsmImm(a) + " stepj0";
    }
    std::string mov(Imm16 a, SttMod b) {
        return "mov " + DsmImm(a) + " " + DsmReg(b);
    }
    std::string mov_prpage(Imm4 a) {
        return "mov " + DsmImm(a) + " prpage";
    }

    std::string mov_a0h_stepi0(Dummy) {
        return "mov a0h stepi0";
    }
    std::string mov_a0h_stepj0(Dummy) {
        return "mov a0h stepj0";
    }
    std::string mov_stepi0_a0h(Dummy) {
        return "mov stepi0 a0h";
    }
    std::string mov_stepj0_a0h(Dummy) {
        return "mov stepj0 a0h";
    }

    std::string mov_prpage(Abl a) {
        return "mov " + DsmReg(a) + " prpage" ;
    }
    std::string mov_repc(Abl a) {
        return "mov " + DsmReg(a) + " repc" ;
    }
    std::string mov(Abl a, ArArp b) {
        return "mov " + DsmReg(a) + " " + DsmReg(b);
    }
    std::string mov(Abl a, SttMod b) {
        return "mov " + DsmReg(a) + " " + DsmReg(b);
    }

    std::string mov_prpage_to(Abl b) {
        return "mov prpage " + DsmReg(b);
    }
    std::string mov_repc_to(Abl b) {
        return "mov repc " + DsmReg(b);
    }
    std::string mov(ArArp a, Abl b) {
        return "mov " + DsmReg(a) + " " + DsmReg(b);
    }
    std::string mov(SttMod a, Abl b) {
        return "mov " + DsmReg(a) + " " + DsmReg(b);
    }

    std::string mov_repc_to(ArRn1 b, ArStep1 bs) {
        return "mov repc [" + DsmArRn(b) + DsmArStep(bs) + "]";
    }
    std::string mov(ArArp a, ArRn1 b, ArStep1 bs) {
        return "mov " + DsmReg(a) + " [" + DsmArRn(b) + DsmArStep(bs) + "]";
    }
    std::string mov(SttMod a, ArRn1 b, ArStep1 bs) {
        return "mov " + DsmReg(a) + " [" + DsmArRn(b) + DsmArStep(bs) + "]";
    }

    std::string mov_repc(ArRn1 a, ArStep1 as) {
        return "mov [" + DsmArRn(a) + DsmArStep(as) + "] repc";
    }
    std::string mov(ArRn1 a, ArStep1 as, ArArp b) {
        return "mov [" + DsmArRn(a) + DsmArStep(as) + "] " + DsmReg(b) ;
    }
    std::string mov(ArRn1 a, ArStep1 as, SttMod b) {
        return "mov [" + DsmArRn(a) + DsmArStep(as) + "] " + DsmReg(b) ;
    }

    std::string mov_repc_to(MemR7Imm16 b) {
        return "mov repc " + Dsm(b);
    }
    std::string mov(ArArpSttMod a, MemR7Imm16 b) {
        return "mov " + DsmReg(a) + " " + Dsm(b);
    }

    std::string mov_repc(MemR7Imm16 a) {
        return "mov " + Dsm(a) + " repc";
    }
    std::string mov(MemR7Imm16 a, ArArpSttMod b) {
        return "mov " + Dsm(a) + " " + DsmReg(b);
    }

    std::string mov_pc(Ax a) {
        return "mov " + DsmReg(a) + " pc";
    }
    std::string mov_pc(Bx a) {
        return "mov " + DsmReg(a) + " pc";
    }

    std::string mov_mixp_to(Bx b) {
        return "mov mixp " + DsmReg(b);
    }
    std::string mov_mixp_r6(Dummy) {
        return "mov mixp r6";
    }
    std::string mov_p0h_to(Bx b) {
        return "mov p0h " + DsmReg(b);
    }
    std::string mov_p0h_r6(Dummy) {
        return "mov p0h r6";
    }
    std::string mov_p0h_to(Register b) {
        return "mov p0h " + DsmReg(b);
    }
    std::string mov_p0(Ab a) {
        return "mov " + DsmReg(a) + " p0";
    }
    std::string mov_p1_to(Ab b) {
        return "mov p1 " + DsmReg(b);
    }

    std::string mov2(Px a, ArRn2 b, ArStep2 bs) {
        return "mov " + DsmReg(a) + " [" + DsmArRn(b) + DsmArStep(bs) + "]";
    }
    std::string mov2s(Px a, ArRn2 b, ArStep2 bs) {
        return "mov " + DsmReg(a) + " [" + DsmArRn(b) + DsmArStep(bs) + "] s";
    }
    std::string mov2(ArRn2 a, ArStep2 as, Px b) {
        return "mov [" + DsmArRn(a) + DsmArStep(as) + "] " + DsmReg(b);
    }
    std::string mova(Ab a, ArRn2 b, ArStep2 bs) {
        return "mova " + DsmReg(a) + " [" + DsmArRn(b) + DsmArStep(bs) + "]";
    }
    std::string mova(ArRn2 a, ArStep2 as, Ab b) {
        return "mov [" +DsmArRn(a) + DsmArStep(as) + "] " + DsmReg(b);
    }

    std::string mov_r6_to(Bx b) {
        return "mov r6 " + DsmReg(b);
    }
    std::string mov_r6_mixp(Dummy) {
        return "mov r6 mixp";
    }
    std::string mov_r6_to(Register b) {
        return "mov r6 " + DsmReg(b);
    }
    std::string mov_r6(Register a) {
        return "mov " + DsmReg(a) + " r6";
    }
    std::string mov_memsp_r6(Dummy) {
        return "mov [sp] r6";
    }
    std::string mov_r6_to(Rn b, StepZIDS bs) {
        return "mov r6 [" + DsmReg(b) + Dsm(bs) + "]";
    }
    std::string mov_r6(Rn a, StepZIDS as) {
        return "mov [" + DsmReg(a) + Dsm(as) + "] r6";
    }

    std::string movs(MemImm8 a, Ab b) {
        return "movs " + Dsm(a) + " " + DsmReg(b);
    }
    std::string movs(Rn a, StepZIDS as, Ab b) {
        return "movs [" + DsmReg(a) + Dsm(as) + "] " + DsmReg(b);
    }
    std::string movs(Register a, Ab b) {
        return "movs " + DsmReg(a) + " " + DsmReg(b);
    }
    std::string movs_r6_to(Ax b) {
        return "movs r6 " + DsmReg(b);
    }
    std::string movsi(RnOld a, Ab b, Imm5s s) {
        return "movs " + DsmReg(a) + " " + DsmReg(b) + " " + DsmImm(s);
    }
};
