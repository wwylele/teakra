#pragma once
#include "decoder.h"
#include "oprand.h"
#include "register.h"
#include "memory.h"


class Interpreter {
public:

    Interpreter (RegisterState& regs, MemoryInterface& mem) : regs(regs), mem(mem) {}

    void Run(unsigned cycles) {
        for (unsigned i  = 0; i < cycles; ++i) {
            u16 opcode = mem.PRead(regs.pc++);
            auto decoder = Decode<Interpreter>(opcode);
            if (!decoder)
                throw "unknown code!";
            u16 expand_value = 0;
            if (decoder->NeedExpansion()) {
                expand_value = mem.PRead(regs.pc++);
            }

            // TODO: bkrep and rep check

            decoder->call(*this, opcode, expand_value);
        }
    }

    using instruction_return_type = void;

    void nop(Dummy) {
        // literally nothing
    }

    void norm(Ax a, Rn b, StepZIDS bs) {
        throw "unimplemented";
    }
    //INST(swap, 0x4980, At<SwapTypes, 0>),
    void trap(Dummy) {
        throw "unimplemented";
    }

    void alm(Alm op, MemImm8 a, Ax b) {
        throw "unimplemented";
    }
    void alm(Alm op, Rn a, StepZIDS as, Ax b) {
        throw "unimplemented";
    }
    void alm(Alm op, Register a, Ax b) {
        throw "unimplemented";
    }
    void alm_r6(Alm op, Ax b) {
        throw "unimplemented";
    }

    void AluGeneric(Alu op, u16 a, Ax b) {
        switch(op.GetName()) {
        case AluOp::Or: {
            u64 value = GetAcc(b.GetName());
            value |= a;
            SetAcc_NoSaturation(b.GetName(), value);
            break;
        }
        case AluOp::And: {
            u64 value = GetAcc(b.GetName());
            value &= a;
            SetAcc_NoSaturation(b.GetName(), value);
            break;
        }
        case AluOp::Xor: {
            u64 value = GetAcc(b.GetName());
            value ^= a;
            SetAcc_NoSaturation(b.GetName(), value);
            break;
        }
        case AluOp::Cmp:
        case AluOp::Sub:
        case AluOp::Add: {
            u64 value = GetAcc(b.GetName()) & 0xFF'FFFF'FFFF;
            u64 a_extend = SignExtend<16, u64>(a) & 0xFF'FFFF'FFFF;
            u64 result;
            if (op.GetName() == AluOp::Add)
                result = value + a_extend;
            else
                result = value - a_extend;
            regs.fc = (result >> 40) & 1;
            regs.fv = ((~(value ^ a_extend) & (value ^ result)) >> 39) & 1;
            if (regs.fv) {
                regs.flv = 1;
            }
            result = SignExtend<40>(result);
            if (op.GetName() == AluOp::Cmp) {
                SetAccFlag(result);
            } else {
                SetAcc(b.GetName(), result);
            }
            break;
        }
        default:throw "???";
        }
    }

    void alu(Alu op, MemImm16 a, Ax b) {
        u16 value = LoadFromMemory(a);
        AluGeneric(op, value, b);
    }
    void alu(Alu op, MemR7Imm16 a, Ax b) {
        u16 value = LoadFromMemory(a);
        AluGeneric(op, value, b);
    }
    void alu(Alu op, Imm16 a, Ax b) {
        u16 value = a.storage;
        AluGeneric(op, value, b);
    }
    void alu(Alu op, Imm8 a, Ax b) {
        u16 value = a.storage;
        if (op.GetName() == AluOp::And) {
            value &= 0xFF00; // for And operation, value is 1-extended to 16-bit.
        }
        AluGeneric(op, value, b);
    }
    void alu(Alu op, MemR7Imm7s a, Ax b) {
        u16 value = LoadFromMemory(a);
        AluGeneric(op, value, b);
    }

    void or_(Ab a, Ax b, Ax c) {
        throw "unimplemented";
    }
    void or_(Ax a, Bx b, Ax c) {
        throw "unimplemented";
    }
    void or_(Bx a, Bx b, Ax c) {
        throw "unimplemented";
    }

    u16 GenericAlb(Alb op, u16 a, u16 b) {
        u16 result;
        switch (op.GetName()) {
        case AlbOp::Set: {
            result = a | b;
            break;
        }
        case AlbOp::Rst: {
            result = ~a & b;
            break;
        }
        case AlbOp::Chng: {
            result = a ^ b;
            break;
        }
        case AlbOp::Addv: {
            u32 r = a + b;
            regs.fc = (r >> 16) != 0;
            result = r & 0xFFFF;
            break;
        }
        case AlbOp::Tst0: {
            result = (a & b) != 0;
            break;
        }
        case AlbOp::Tst1: {
            result = (a & ~b) != 0;
            break;
        }
        case AlbOp::Cmpv:
        case AlbOp::Subv: {
            u32 r = b - a;
            regs.fc = (r >> 16) != 0;
            result = r & 0xFFFF;
            break;
        }
        default: throw "???";
        }
        regs.fm = result >> 15;
        regs.fz = result == 0;
        return result;
    }

    bool IsAlbModifying(Alb op) {
        switch (op.GetName()) {
        case AlbOp::Set:
        case AlbOp::Rst:
        case AlbOp::Chng:
        case AlbOp::Addv:
        case AlbOp::Subv:
            return true;
        case AlbOp::Tst0:
        case AlbOp::Tst1:
        case AlbOp::Cmpv:
            return false;
        default: throw "???";
        }
    }

    void alb(Alb op, Imm16 a, MemImm8 b) {
        u16 bv = LoadFromMemory(b);
        u16 result = GenericAlb(op, a.storage, bv);
        if (IsAlbModifying(op))
            StoreToMemory(b, result);
    }
    void alb(Alb op, Imm16 a, Rn b, StepZIDS bs) {
        u16 address = RnAddressAndModify(GetRnUnit(b.GetName()), bs.GetName());
        u16 bv = mem.DRead(address);
        u16 result = GenericAlb(op, a.storage, bv);
        if (IsAlbModifying(op))
            mem.DWrite(address, result);
    }
    void alb(Alb op, Imm16 a, Register b) {
        u16 bv;
        if (b.GetName() == RegName::p) {
            bv = ProductToBus40(RegName::p0) >> 16;
        } else if (b.GetName() == RegName::a0 || b.GetName() == RegName::a1) {
            throw "weird effect";
        } else if (b.GetName() == RegName::a0l || b.GetName() == RegName::a1l
            || b.GetName() == RegName::b0l || b.GetName() == RegName::b1l) {
            bv = GetAcc(b.GetName()) & 0xFFFF;
        } else if (b.GetName() == RegName::a0l || b.GetName() == RegName::a1l
            || b.GetName() == RegName::b0l || b.GetName() == RegName::b1l) {
            bv = (GetAcc(b.GetName()) >> 16) & 0xFFFF;
        } else {
            bv = RegToBus16(b.GetName());
        }
        u16 result = GenericAlb(op, a.storage, bv);
        if (IsAlbModifying(op)) {
            switch (b.GetName()) {
            case RegName::a0: case RegName::a1:
                throw "weird effect";
            // operation on accumulators doesn't go through regular bus with flag and saturation
            case RegName::a0l: regs.a[0].value = (regs.a[0].value & 0xFF'FFFF'0000) | result; break;
            case RegName::a1l: regs.a[1].value = (regs.a[1].value & 0xFF'FFFF'0000) | result; break;
            case RegName::b0l: regs.b[0].value = (regs.b[0].value & 0xFF'FFFF'0000) | result; break;
            case RegName::b1l: regs.b[1].value = (regs.b[1].value & 0xFF'FFFF'0000) | result; break;
            case RegName::a0h: regs.a[0].value = (regs.a[0].value & 0xFF'0000'FFFF) | ((u64)result << 16); break;
            case RegName::a1h: regs.a[1].value = (regs.a[1].value & 0xFF'0000'FFFF) | ((u64)result << 16); break;
            case RegName::b0h: regs.b[0].value = (regs.b[0].value & 0xFF'0000'FFFF) | ((u64)result << 16); break;
            case RegName::b1h: regs.b[1].value = (regs.b[1].value & 0xFF'0000'FFFF) | ((u64)result << 16); break;
            default:
                RegFromBus16(b.GetName(), result); // including RegName:p (p0h)
            }
        }
    }
    void alb_r6(Alb op, Imm16 a) {
        u16 bv = regs.r[6];
        u16 result = GenericAlb(op, a.storage, bv);
        if (IsAlbModifying(op))
            regs.r[6] = result;
    }
    void alb(Alb op, Imm16 a, SttMod b) {
        u16 bv = RegToBus16(b.GetName());
        u16 result = GenericAlb(op, a.storage, bv);
        if (IsAlbModifying(op))
            RegFromBus16(b.GetName(), result);
    }

    void Moda(ModaOp op, RegName a, Cond cond) {
        if (regs.ConditionPass(cond)) {
            switch (op) {
            case ModaOp::Shr: {
                u64 result = ShiftBus40(GetAcc(a), 0xFFFF);
                SetAcc(a, result, /*No saturation if logic shift*/regs.s == 1);
                break;
            }
            case ModaOp::Shr4: {
                u64 result = ShiftBus40(GetAcc(a), 0xFFFC);
                SetAcc(a, result, /*No saturation if logic shift*/regs.s == 1);
                break;
            }
            case ModaOp::Shl: {
                u64 result = ShiftBus40(GetAcc(a), 1);
                SetAcc(a, result, /*No saturation if logic shift*/regs.s == 1);
                break;
            }
            case ModaOp::Shl4: {
                u64 result = ShiftBus40(GetAcc(a), 4);
                SetAcc(a, result, /*No saturation if logic shift*/regs.s == 1);
                break;
            }
            case ModaOp::Ror: {
                u64 value = GetAcc(a) & 0xFF'FFFF'FFFF;
                u16 old_fc = regs.fc;
                regs.fc = value & 1;
                value >>= 1;
                value |= (u64)old_fc << 39;
                value = SignExtend<40>(value);
                SetAcc_NoSaturation(a, value);
                break;
            }
            case ModaOp::Rol: {
                u64 value = GetAcc(a);
                u16 old_fc = regs.fc;
                regs.fc = (value >> 39) & 1;
                value <<= 1;
                value |= old_fc;
                value = SignExtend<40>(value);
                SetAcc_NoSaturation(a, value);
                break;
            }
            case ModaOp::Clr: {
                SetAcc(a, 0);
                break;
            }
            case ModaOp::Not: {
                u64 result = ~GetAcc(a);
                SetAcc_NoSaturation(a, result);
                break;
            }
            case ModaOp::Neg: {
                u64 value = GetAcc(a);
                regs.fc = value != 0; // ?
                regs.fv = value == 0xFFFF'FF80'0000'0000; // ?
                if (regs.fv)
                    regs.flv = 1;
                u64 result = SignExtend<40, u64>(~GetAcc(a) + 1);
                SetAcc(a, result);
                break;
            }
            case ModaOp::Rnd: {
                // TODO: after impl add
                break;
            }
            case ModaOp::Pacr: {
                // TODO: after impl add
                break;
            }
            case ModaOp::Clrr: {
                SetAcc(a, 0x8000);
                break;
            }
            case ModaOp::Inc: {
                // TODO: after impl add
                break;
            }
            case ModaOp::Dec: {
                // TODO: after impl add
                break;
            }
            case ModaOp::Copy: {
                // note: bX doesn't support
                u64 value = GetAcc(a == RegName::a0 ? RegName::a1 : RegName::a0);
                SetAcc(a, value);
            }
            default:
                throw "??";
            }
        }
    }

    void moda4(Moda4 op, Ax a, Cond cond) {
        Moda(op.GetName(), a.GetName(), cond);
    }

    void moda3(Moda3 op, Bx a, Cond cond) {
        Moda(op.GetName(), a.GetName(), cond);
    }

    void bkrep(Imm8 a, Address16 addr) {
        throw "unimplemented";
    }
    void bkrep(Register a, Address18_16 addr_low, Address18_2 addr_high) {
        throw "unimplemented";
    }
    void bkrep_r6(Address18_16 addr_low, Address18_2 addr_high) {
        throw "unimplemented";
    }
    void bkreprst(ArRn4 a) {
        throw "unimplemented";
    }
    void bkreprst_memsp(Dummy) {
        throw "unimplemented";
    }
    void bkrepsto(ArRn4 a) {
        throw "unimplemented";
    }
    void bkrepsto_memsp(Dummy) {
        throw "unimplemented";
    }

    void banke(BankFlags flags) {
        throw "unimplemented";
    }
    void bankr(Dummy) {
        throw "unimplemented";
    }
    void bankr(Ar a) {
        throw "unimplemented";
    }
    void bankr(Ar a, Arp b) {
        throw "unimplemented";
    }
    void bankr(Arp a) {
        throw "unimplemented";
    }

    void BitReverse(u32 unit) {
        u16 value = regs.r[unit];
        u16 result = 0;
        for (u32 i = 0; i < 16; ++i) {
            result |= ((value >> i) & 1) << (15 - i);
        }
        regs.r[unit] = result;
    }
    void bitrev(Rn a) {
        u32 unit = GetRnUnit(a.GetName());
        BitReverse(unit);
    }
    void bitrev_dbrv(Rn a) {
        u32 unit = GetRnUnit(a.GetName());
        BitReverse(unit);
        regs.ms[unit] = 0; // what does this do with bitrev, though?
    }
    void bitrev_ebrv(Rn a) {
        u32 unit = GetRnUnit(a.GetName());
        BitReverse(unit);
        regs.ms[unit] = 1;
    }

    void br(Address18_16 addr_low, Address18_2 addr_high, Cond cond) {
        if (regs.ConditionPass(cond)) {
            regs.SetPC(addr_low.storage, addr_high.storage);
        }
    }

    void brr(RelAddr7 addr, Cond cond) {
        if (regs.ConditionPass(cond)) {
            regs.pc += addr.storage; // note: pc is the address of the NEXT instruction
        }
    }

    void break_(Dummy) {
        throw "unimplemented";
    }

    void call(Address18_16 addr_low, Address18_2 addr_high, Cond cond) {
        if (regs.ConditionPass(cond)) {
            u16 l = regs.GetPcL();
            u16 h = regs.GetPcH();
            mem.DWrite(--regs.sp, l);
            mem.DWrite(--regs.sp, h);
            regs.SetPC(addr_low.storage, addr_high.storage);
        }
    }
    void calla(Axl a) {
        u16 l = regs.GetPcL();
        u16 h = regs.GetPcH();
        mem.DWrite(--regs.sp, l);
        mem.DWrite(--regs.sp, h);
        regs.pc = RegToBus16(a.GetName()); // use movpd?
    }
    void calla(Ax a) {
        u16 l = regs.GetPcL();
        u16 h = regs.GetPcH();
        mem.DWrite(--regs.sp, l);
        mem.DWrite(--regs.sp, h);
        regs.pc = GetAcc(a.GetName()) & 0x3FFFF; // no saturation ?
    }
    void callr(RelAddr7 addr, Cond cond) {
        if (regs.ConditionPass(cond)) {
            u16 l = regs.GetPcL();
            u16 h = regs.GetPcH();
            mem.DWrite(--regs.sp, l);
            mem.DWrite(--regs.sp, h);
            regs.pc += addr.storage;
        }
    }

    void cntx_s(Dummy) {
        throw "unimplemented";
    }
    void cntx_r(Dummy) {
        throw "unimplemented";
    }

    void ret(Cond c) {
        if (regs.ConditionPass(c)) {
            u16 h = mem.DRead(regs.sp++);
            u16 l = mem.DRead(regs.sp++);
            regs.SetPC(l, h);
        }
    }
    void retd(Dummy) {
        throw "unimplemented";
    }
    void reti(Cond c) {
        if (regs.ConditionPass(c)) {
            u16 h = mem.DRead(regs.sp++);
            u16 l = mem.DRead(regs.sp++);
            regs.SetPC(l, h);
            regs.ie = 1;
        }
    }
    void retic(Cond c) {
        throw "unimplemented";
    }
    void retid(Dummy) {
        throw "unimplemented";
    }
    void retidc(Dummy) {
        throw "unimplemented";
    }
    void rets(Imm8 a) {
        u16 h = mem.DRead(regs.sp++);
        u16 l = mem.DRead(regs.sp++);
        regs.SetPC(l, h);
        regs.sp += a.storage;
    }

    void load_ps(Imm2 a) {
        regs.ps[0] = a.storage;
    }
    void load_stepi(Imm7s a) {
        regs.stepi = a.storage;
    }
    void load_stepj(Imm7s a) {
        regs.stepj = a.storage;
    }
    void load_page(Imm8 a) {
        regs.page = a.storage;
    }
    void load_modi(Imm9 a) {
        regs.modi = a.storage;
    }
    void load_modj(Imm9 a) {
        regs.modj = a.storage;
    }
    void load_movpd(Imm2 a) {
        regs.movpd = a.storage;
    }
    void load_ps01(Imm4 a) {
        regs.ps[0] = a.storage & 3;
        regs.ps[0] = a.storage >> 2;
    }

    void push(Imm16 a) {
        mem.DWrite(--regs.sp, a.storage);
    }
    void push(Register a) {
        // need test: p0, aX
        u16 value = RegToBus16(a.GetName());
        mem.DWrite(--regs.sp, value);
    }
    void push(Abe a) {
        u16 value = (SaturateAcc(GetAcc(a.GetName()), false) >> 32) & 0xFFFF;
        mem.DWrite(--regs.sp, value);
    }
    void push(ArArpSttMod a) {
        u16 value = RegToBus16(a.GetName());
        mem.DWrite(--regs.sp, value);
    }
    void push_prpage(Dummy) {
        throw "unimplemented";
    }
    void push(Px a) {
        u32 value = (u32)ProductToBus40(a.GetName());
        u16 h = value >> 16;
        u16 l = value & 0xFFFF;
        mem.DWrite(--regs.sp, l);
        mem.DWrite(--regs.sp, h);
    }
    void push_r6(Dummy) {
        u16 value = regs.r[6];
        mem.DWrite(--regs.sp, value);
    }
    void push_repc(Dummy) {
        u16 value = regs.repc;
        mem.DWrite(--regs.sp, value);
    }
    void push_x0(Dummy) {
        u16 value = regs.x[0];
        mem.DWrite(--regs.sp, value);
    }
    void push_x1(Dummy) {
        u16 value = regs.x[1];
        mem.DWrite(--regs.sp, value);
    }
    void push_y1(Dummy) {
        u16 value = regs.y[1];
        mem.DWrite(--regs.sp, value);
    }
    void pusha(Ax a) {
        u32 value = SaturateAcc(GetAcc(a.GetName()), false) & 0xFFFF'FFFF;
        u16 h = value >> 16;
        u16 l = value & 0xFFFF;
        mem.DWrite(--regs.sp, l);
        mem.DWrite(--regs.sp, h);
    }
    void pusha(Bx a) {
        u32 value = SaturateAcc(GetAcc(a.GetName()), false) & 0xFFFF'FFFF;
        u16 h = value >> 16;
        u16 l = value & 0xFFFF;
        mem.DWrite(--regs.sp, l);
        mem.DWrite(--regs.sp, h);
    }

    void pop(Register a) {
        // need test: p0
        u16 value = mem.DRead(regs.sp++);
        RegFromBus16(a.GetName(), value);
    }
    void pop(Abe a) {
        u32 value32 = SignExtend<8, u32>(mem.DRead(regs.sp++) & 0xFF);
        RegisterState::Accumulator* target;
        switch(a.GetName()) {
        case RegName::a0e: target = &regs.a[0]; break;
        case RegName::a1e: target = &regs.a[1]; break;
        case RegName::b0e: target = &regs.b[0]; break;
        case RegName::b1e: target = &regs.b[1]; break;
        default: throw "???";
        }
        SetAcc(a.GetName(), (target->value & 0xFFFFFFFF) | (u64)value32 << 32);
    }
    void pop(ArArpSttMod a) {
        u16 value = mem.DRead(regs.sp++);
        RegFromBus16(a.GetName(), value);
    }
    void pop(Bx a) {
        u16 value = mem.DRead(regs.sp++);
        RegFromBus16(a.GetName(), value);
    }
    void pop_prpage(Dummy) {
        throw "unimplemented";
    }
    void pop(Px a) {
        u16 h = mem.DRead(regs.sp++);
        u16 l = mem.DRead(regs.sp++);
        u32 value = ((u32)h << 16) | l;
        ProductFromBus32(a.GetName(), value);
    }
    void pop_r6(Dummy) {
        u16 value = mem.DRead(regs.sp++);
        regs.r[6] = value;
    }
    void pop_repc(Dummy) {
        u16 value = mem.DRead(regs.sp++);
        regs.repc = value;
    }
    void pop_x0(Dummy) {
        u16 value = mem.DRead(regs.sp++);
        regs.x[0] = value;
    }
    void pop_x1(Dummy) {
        u16 value = mem.DRead(regs.sp++);
        regs.x[1] = value;
    }
    void pop_y1(Dummy) {
        u16 value = mem.DRead(regs.sp++);
        regs.y[1] = value;
    }
    void popa(Ab a) {
        u16 h = mem.DRead(regs.sp++);
        u16 l = mem.DRead(regs.sp++);
        u64 value = SignExtend<32, u64>((h << 16) | l);
        SetAcc(a.GetName(), value);
    }

    void rep(Imm8 a) {
        throw "unimplemented";
    }
    void rep(Register a) {
        throw "unimplemented";
    }
    void rep_r6(Dummy) {
        throw "unimplemented";
    }

    void shfc(Ab a, Ab b, Cond cond) {
        if (regs.ConditionPass(cond)) {
            u64 value = GetAcc(a.GetName());
            u16 sv = regs.sv;
            SetAcc(b.GetName(), ShiftBus40(value, sv), /*No saturation if logic shift*/regs.s == 1);
        }
    }
    void shfi(Ab a, Ab b, Imm6s s) {
        u64 value = GetAcc(a.GetName());
        u16 sv = SignExtend<6, u16>(s.storage);
        SetAcc(b.GetName(), ShiftBus40(value, sv), /*No saturation if logic shift*/regs.s == 1);
    }

    void tst4b(ArRn4 b, ArStep4 bs) {
        throw "unimplemented";
    }
    void tst4b(ArRn4 b, ArStep4 bs, Ax c) {
        throw "unimplemented";
    }
    void tstb(MemImm8 a, Imm4 b) {
        throw "unimplemented";
    }
    void tstb(Rn a, StepZIDS as, Imm4 b) {
        throw "unimplemented";
    }
    void tstb(Register a, Imm4 b) {
        throw "unimplemented";
    }
    void tstb_r6(Imm4 b) {
        throw "unimplemented";
    }
    void tstb(SttMod a, Imm16 b) {
        throw "unimplemented";
    }

    void and_(Ab a, Ab b, Ax c) {
        throw "unimplemented";
    }

    void dint(Dummy) {
        regs.ie = 0;
    }
    void eint(Dummy) {
        regs.ie = 1;
    }

    void movd(R0123 a, StepZIDS as, R45 b, StepZIDS bs) {
        u16 address_s = RnAddressAndModify(GetRnUnit(a.GetName()), as.GetName());
        u32 address_d = RnAddressAndModify(GetRnUnit(b.GetName()), bs.GetName());
        address_d |= (u32)regs.movpd << 16;
        mem.PWrite(address_d, mem.DRead(address_s));
    }
    void movp(Axl a, Register b) {
        u32 address = RegToBus16(a.GetName());
        address |= (u32)regs.movpd << 16;
        u16 value = mem.PRead(address);
        RegFromBus16(b.GetName(), value);
    }
    void movp(Ax a, Register b) {
        u32 address = GetAcc(a.GetName()) & 0x3FFFF; // no saturation
        u16 value = mem.PRead(address);
        RegFromBus16(b.GetName(), value);
    }
    void movp(Rn a, StepZIDS as, R0123 b, StepZIDS bs) {
        u32 address_s = RnAddressAndModify(GetRnUnit(a.GetName()), as.GetName());
        u16 address_d = RnAddressAndModify(GetRnUnit(b.GetName()), bs.GetName());
        address_s |= (u32)regs.movpd << 16;
        mem.DWrite(address_d, mem.PRead(address_s));
    }
    void movpdw(Ax a) {
        u32 address = GetAcc(a.GetName()) & 0x3FFFF; // no saturation
        u16 h = mem.PRead(address);
        u16 l = mem.PRead(address);
        regs.SetPC(l, h);
    }

    void mov(Ab a, Ab b) {
        u64 value = GetAcc(a.GetName());
        SetAcc(b.GetName(), value);
    }
    void mov_dvm(Abl a) {
        throw "unimplemented";
    }
    void mov_x0(Abl a) {
        u16 value16 = RegToBus16(a.GetName());
        regs.x[0] = value16;
    }
    void mov_x1(Abl a) {
        u16 value16 = RegToBus16(a.GetName());
        regs.x[1] = value16;
    }
    void mov_y1(Abl a) {
        u16 value16 = RegToBus16(a.GetName());
        regs.y[1] = value16;
    }

    void StoreToMemory(MemImm8 addr, u16 value) {
        mem.DWrite(addr.storage + (regs.page << 8), value);
    }
    void StoreToMemory(MemImm16 addr, u16 value) {
        mem.DWrite(addr.storage, value);
    }
    void StoreToMemory(MemR7Imm16 addr, u16 value) {
        mem.DWrite(addr.storage + regs.r[7], value);
    }
    void StoreToMemory(MemR7Imm7s addr, u16 value) {
        mem.DWrite(addr.storage + regs.r[7], value);
    }

    void mov(Ablh a, MemImm8 b) {
        u16 value16 = RegToBus16(a.GetName());
        StoreToMemory(b, value16);
    }
    void mov(Axl a, MemImm16 b) {
        u16 value16 = RegToBus16(a.GetName());
        StoreToMemory(b, value16);
    }
    void mov(Axl a, MemR7Imm16 b) {
        u16 value16 = RegToBus16(a.GetName());
        StoreToMemory(b, value16);
    }
    void mov(Axl a, MemR7Imm7s b) {
        u16 value16 = RegToBus16(a.GetName());
        StoreToMemory(b, value16);
    }

    u16 LoadFromMemory(MemImm8 addr) {
        return mem.DRead(addr.storage + (regs.page << 8));
    }
    u16 LoadFromMemory(MemImm16 addr) {
        return mem.DRead(addr.storage);
    }
    u16 LoadFromMemory(MemR7Imm16 addr) {
        return mem.DRead(addr.storage + regs.r[7]);
    }
    u16 LoadFromMemory(MemR7Imm7s addr) {
        return mem.DRead(addr.storage + regs.r[7]);
    }

    void mov(MemImm16 a, Ax b) {
        u16 value = LoadFromMemory(a);
        RegFromBus16(b.GetName(), value);
    }
    void mov(MemImm8 a, Ab b) {
        u16 value = LoadFromMemory(a);
        RegFromBus16(b.GetName(), value);
    }
    void mov(MemImm8 a, Ablh b) {
        u16 value = LoadFromMemory(a);
        RegFromBus16(b.GetName(), value);
    }
    void mov_eu(MemImm8 a, Axh b) {
        throw "unimplemented";
    }
    void mov(MemImm8 a, RnOld b) {
        u16 value = LoadFromMemory(a);
        RegFromBus16(b.GetName(), value);
    }
    void mov_sv(MemImm8 a) {
        u16 value = LoadFromMemory(a);
        regs.sv = value;
    }
    void mov_dvm_to(Ab b) {
        throw "unimplemented";
    }
    void mov_icr_to(Ab b) {
        u16 value = regs.icr.Get();
        RegFromBus16(b.GetName(), value);
    }
    void mov(Imm16 a, Bx b) {
        u16 value = a.storage;
        RegFromBus16(b.GetName(), value);
    }
    void mov(Imm16 a, Register b) {
        u16 value = a.storage;
        RegFromBus16(b.GetName(), value);
    }
    void mov_icr(Imm5 a) {
        throw "unimplemented";
    }
    void mov(Imm8s a, Axh b) {
        u16 value = (u16)(s16)a.Value();
        RegFromBus16(b.GetName(), value);
    }
    void mov(Imm8s a, RnOld b) {
        u16 value = (u16)(s16)a.Value();
        RegFromBus16(b.GetName(), value);
    }
    void mov_sv(Imm8s a) {
        u16 value = (u16)(s16)a.Value();
        regs.sv = value;
    }
    void mov(Imm8 a, Axl b) {
        u16 value = a.Value();
        RegFromBus16(b.GetName(), value);
    }
    void mov(MemR7Imm16 a, Ax b) {
        u16 value = LoadFromMemory(a);
        RegFromBus16(b.GetName(), value);
    }
    void mov(MemR7Imm7s a, Ax b) {
        u16 value = LoadFromMemory(a);
        RegFromBus16(b.GetName(), value);
    }
    void mov(Rn a, StepZIDS as, Bx b) {
        u16 address = RnAddressAndModify(GetRnUnit(a.GetName()), as.GetName());
        u16 value = mem.DRead(address);
        RegFromBus16(b.GetName(), value);
    }
    void mov(Rn a, StepZIDS as, Register b) {
        u16 address = RnAddressAndModify(GetRnUnit(a.GetName()), as.GetName());
        u16 value = mem.DRead(address);
        RegFromBus16(b.GetName(), value);
    }
    void mov_memsp_to(Register b) {
        u16 value = mem.DRead(regs.sp);
        RegFromBus16(b.GetName(), value);
    }
    void mov_mixp_to(Register b) {
        u16 value = regs.mixp;
        RegFromBus16(b.GetName(), value);
    }
    void mov(RnOld a, MemImm8 b) {
        u16 value = RegToBus16(a.GetName());
        StoreToMemory(b, value);
    }
    void mov_icr(Register a) {
        u16 value = RegToBus16(a.GetName());
        regs.icr.Set(value);
    }
    void mov_mixp(Register a) {
        u16 value = RegToBus16(a.GetName());
        regs.mixp = value;
    }
    void mov(Register a, Rn b, StepZIDS bs) {
        // a = a0 or a1 is overrided
        // a = p0 untested
        u16 value = RegToBus16(a.GetName());
        u16 address = RnAddressAndModify(GetRnUnit(b.GetName()), bs.GetName());
        mem.DWrite(address, value);
    }
    void mov(Register a, Bx b) {
        if (a.GetName() == RegName::p) {
            u64 value = ProductToBus40(RegName::p0);
            SetAcc(b.GetName(), value);
        } else if (a.GetName() == RegName::a0 || a.GetName() == RegName::a1) {
            // Is there any difference from the mov(Ab, Ab) instruction?
            u64 value = GetAcc(a.GetName());
            SetAcc(b.GetName(), value);
        } else {
            u16 value = RegToBus16(a.GetName());
            RegFromBus16(b.GetName(), value);
        }
    }
    void mov(Register a, Register b) {
        // a = a0 or a1 is overrided
        if (a.GetName() == RegName::p) {
            // b loses its typical meaning in this case
            RegName b_name = (b.storage & 1) ? RegName::a0 : RegName::a1;
            u64 value = ProductToBus40(RegName::p0);
            SetAcc(b_name, value);
        } else {
            u16 value = RegToBus16(a.GetName());
            RegFromBus16(b.GetName(), value);
        }
    }
    void mov_repc_to(Ab b) {
        u16 value = regs.repc;
        RegFromBus16(b.GetName(), value);
    }
    void mov_sv_to(MemImm8 b) {
        u16 value = regs.sv;
        StoreToMemory(b, value);
    }
    void mov_x0_to(Ab b) {
        u16 value = regs.x[0];
        RegFromBus16(b.GetName(), value);
    }
    void mov_x1_to(Ab b) {
        u16 value = regs.x[1];
        RegFromBus16(b.GetName(), value);
    }
    void mov_y1_to(Ab b) {
        u16 value = regs.y[1];
        RegFromBus16(b.GetName(), value);
    }
    void mov(Imm16 a, ArArp b) {
        u16 value = a.storage;
        RegFromBus16(b.GetName(), value);
    }
    void mov_r6(Imm16 a) {
        u16 value = a.storage;
        regs.r[6] = value;
    }
    void mov_repc(Imm16 a) {
        u16 value = a.storage;
        regs.repc = value;
    }
    void mov_stepi0(Imm16 a) {
        u16 value = a.storage;
        regs.stepi0 = value;
    }
    void mov_stepj0(Imm16 a) {
        u16 value = a.storage;
        regs.stepj0 = value;
    }
    void mov(Imm16 a, SttMod b) {
        u16 value = a.storage;
        RegFromBus16(b.GetName(), value);
    }
    void mov_prpage(Imm4 a) {
        throw "unimplemented";
    }

    void mov_a0h_stepi0(Dummy) {
        u16 value = RegToBus16(RegName::a0h);
        regs.stepi0 = value;
    }
    void mov_a0h_stepj0(Dummy) {
        u16 value = RegToBus16(RegName::a0h);
        regs.stepj0 = value;
    }
    void mov_stepi0_a0h(Dummy) {
        u16 value = regs.stepi0;
        RegFromBus16(RegName::a0h, value);
    }
    void mov_stepj0_a0h(Dummy) {
        u16 value = regs.stepj0;
        RegFromBus16(RegName::a0h, value);
    }

    void mov_prpage(Abl a) {
        throw "unimplemented";
    }
    void mov_repc(Abl a) {
        u16 value = RegToBus16(a.GetName());
        regs.repc = value;
    }
    void mov(Abl a, ArArp b) {
        u16 value = RegToBus16(a.GetName());
        RegFromBus16(b.GetName(), value);
    }
    void mov(Abl a, SttMod b) {
        u16 value = RegToBus16(a.GetName());
        RegFromBus16(b.GetName(), value);
    }

    void mov_prpage_to(Abl b) {
        throw "unimplemented";
    }
    void mov_repc_to(Abl b) {
        u16 value = regs.repc;
        RegFromBus16(b.GetName(), value);
    }
    void mov(ArArp a, Abl b) {
        u16 value = RegToBus16(a.GetName());
        RegFromBus16(b.GetName(), value);
    }
    void mov(SttMod a, Abl b) {
        u16 value = RegToBus16(a.GetName());
        RegFromBus16(b.GetName(), value);
    }

    void mov_repc_to(ArRn2 b, ArStep2 bs) {
        u16 address = RnAddressAndModify(GetArRnUnit(b.storage), GetArStep(bs.storage));
        u16 value = regs.repc;
        mem.DWrite(address, value);
    }
    void mov(ArArp a, ArRn2 b, ArStep2 bs) {
        u16 address = RnAddressAndModify(GetArRnUnit(b.storage), GetArStep(bs.storage));
        u16 value = RegToBus16(a.GetName());
        mem.DWrite(address, value);
    }
    void mov(SttMod a, ArRn2 b, ArStep2 bs) {
        u16 address = RnAddressAndModify(GetArRnUnit(b.storage), GetArStep(bs.storage));
        u16 value = RegToBus16(a.GetName());
        mem.DWrite(address, value);
    }

    void mov_repc(ArRn2 a, ArStep2 as) {
        u16 address = RnAddressAndModify(GetArRnUnit(a.storage), GetArStep(as.storage));
        u16 value = mem.DRead(address);
        regs.repc = value;
    }
    void mov(ArRn2 a, ArStep2 as, ArArp b) {
        // are you sure it is ok to both use and modify ar registers?
        u16 address = RnAddressAndModify(GetArRnUnit(a.storage), GetArStep(as.storage));
        u16 value = mem.DRead(address);
        RegFromBus16(b.GetName(), value);
    }
    void mov(ArRn2 a, ArStep2 as, SttMod b) {
        u16 address = RnAddressAndModify(GetArRnUnit(a.storage), GetArStep(as.storage));
        u16 value = mem.DRead(address);
        RegFromBus16(b.GetName(), value);
    }

    void mov_repc_to(MemR7Imm16 b) {
        u16 value = regs.repc;
        StoreToMemory(b, value);
    }
    void mov(ArArpSttMod a, MemR7Imm16 b) {
        u16 value = RegToBus16(a.GetName());
        StoreToMemory(b, value);
    }

    void mov_repc(MemR7Imm16 a) {
        u16 value = LoadFromMemory(a);
        regs.repc = value;
    }
    void mov(MemR7Imm16 a, ArArpSttMod b) {
        u16 value = LoadFromMemory(a);
        RegFromBus16(b.GetName(), value);
    }

    void mov_pc(Ax a) {
        throw "unimplemented";
    }
    void mov_pc(Bx a) {
        throw "unimplemented";
    }

    void mov_mixp_to(Bx b) {
        u16 value = regs.mixp;
        RegFromBus16(b.GetName(), value);
    }
    void mov_mixp_r6(Dummy) {
        u16 value = regs.mixp;
        regs.r[6] = value;
    }
    void mov_p0h_to(Bx b) {
        u16 value = (ProductToBus40(RegName::p0) >> 16) & 0xFFFF;
        RegFromBus16(b.GetName(), value);
    }
    void mov_p0h_r6(Dummy) {
        u16 value = (ProductToBus40(RegName::p0) >> 16) & 0xFFFF;
        regs.r[6] = value;
    }
    void mov_p0h_to(Register b) {
        u16 value = (ProductToBus40(RegName::p0) >> 16) & 0xFFFF;
        RegFromBus16(b.GetName(), value);
    }
    void mov_p0(Ab a) {
        u32 value = SaturateAcc(GetAcc(a.GetName()), false) & 0xFFFFFFFF;
        ProductFromBus32(RegName::p0, value);
    }
    void mov_p1_to(Ab b) {
        u64 value = ProductToBus40(RegName::p0);
        SetAcc(b.GetName(), value);
    }

    void mov2(Px a, ArRn4 b, ArStep4 bs) {
        u32 value = ProductToBus32_NoShift(a.GetName());
        u16 l = value & 0xFFFF;
        u16 h = (value >> 16) & 0xFFFF;
        u16 address = RnAddressAndModify(GetArRnUnit(b.storage), GetArStep(bs.storage));
        u16 address2 = address + GetArOffset(bs.storage);
        mem.DWrite(address, l);
        mem.DWrite(address2, h);
    }
    void mov2s(Px a, ArRn4 b, ArStep4 bs) {
        u64 value = ProductToBus40(a.GetName());
        u16 l = value & 0xFFFF;
        u16 h = (value >> 16) & 0xFFFF;
        u16 address = RnAddressAndModify(GetArRnUnit(b.storage), GetArStep(bs.storage));
        u16 address2 = address + GetArOffset(bs.storage);
        mem.DWrite(address, l);
        mem.DWrite(address2, h);
    }
    void mov2(ArRn4 a, ArStep4 as, Px b) {
        u16 address = RnAddressAndModify(GetArRnUnit(a.storage), GetArStep(as.storage));
        u16 address2 = address + GetArOffset(as.storage);
        u16 l = mem.DRead(address);
        u16 h = mem.DRead(address2);
        u64 value = SignExtend<32, u64>((h << 16) | l);
        ProductFromBus32(b.GetName(), value);
    }
    void mova(Ab a, ArRn4 b, ArStep4 bs) {
        u64 value = SaturateAcc(GetAcc(a.GetName()), false);
        u16 l = value & 0xFFFF;
        u16 h = (value >> 16) & 0xFFFF;
        u16 address = RnAddressAndModify(GetArRnUnit(b.storage), GetArStep(bs.storage));
        u16 address2 = address + GetArOffset(bs.storage);
        mem.DWrite(address, l);
        mem.DWrite(address2, h);
    }
    void mova(ArRn4 a, ArStep4 as, Ab b) {
        u16 address = RnAddressAndModify(GetArRnUnit(a.storage), GetArStep(as.storage));
        u16 address2 = address + GetArOffset(as.storage);
        u16 l = mem.DRead(address);
        u16 h = mem.DRead(address2);
        u64 value = SignExtend<32, u64>((h << 16) | l);
        SetAcc(b.GetName(), value);
    }

    void mov_r6_to(Bx b) {
        u16 value = regs.r[6];
        RegFromBus16(b.GetName(), value);
    }
    void mov_r6_mixp(Dummy) {
        u16 value = regs.r[6];
        regs.mixp = value;
    }
    void mov_r6_to(Register b) {
        u16 value = regs.r[6];
        RegFromBus16(b.GetName(), value);
    }
    void mov_r6(Register a) {
        u16 value = RegToBus16(a.GetName());
        regs.r[6] = value;
    }
    void mov_memsp_r6(Dummy) {
        u16 value = mem.DRead(regs.sp);
        regs.r[6] = value;
    }
    void mov_r6_to(Rn b, StepZIDS bs) {
        u16 value = regs.r[6];
        u16 address = RnAddressAndModify(GetRnUnit(b.GetName()), bs.GetName());
        mem.DWrite(address, value);
    }
    void mov_r6(Rn a, StepZIDS as) {
        u16 address = RnAddressAndModify(GetRnUnit(a.GetName()), as.GetName());
        u16 value = mem.DRead(address);
        regs.r[6] = value;
    }

    u64 ShiftBus40(u64 value, u16 sv) {
        value &= 0xFF'FFFF'FFFF;
        if (sv < 0x8000) {
            if (sv > 40) {
                if (regs.s == 0) {
                    regs.fv = 1;
                    regs.flv = 1;
                }
                value = 0;
                regs.fc = 0;
            } else {
                if (regs.s == 0) {
                    regs.fv = SignExtend<40>(value) != SignExtend(value, 40 - sv);
                    if (regs.fv) {
                        regs.flv = 1;
                    }
                }
                value <<= sv;
                regs.fc = (value & ((u64)1 << 40)) != 0;
            }
        } else {
            u16 nsv = ~sv + 1;
            if (nsv > 40) {
                if (regs.s == 0) {
                    value = 0xFF'FFFF'FFFF;
                    regs.fc = 1;
                } else {
                    value = 0;
                    regs.fc = 0;
                }
            } else {
                regs.fc = (value & ((u64)1 << (nsv - 1))) != 0;
                value >>= nsv;
                if (regs.s == 0) {
                    value = SignExtend(value, 40 - nsv);
                }
            }

            if (regs.s == 0) {
                regs.fv = 0;
            }
        }

        return SignExtend<40>(value);
    }

    void movs(MemImm8 a, Ab b) {
        u16 value = LoadFromMemory(a);
        u16 sv = regs.sv;
        SetAcc(b.GetName(), ShiftBus40(value, sv), /*No saturation if logic shift*/regs.s == 1);
    }
    void movs(Rn a, StepZIDS as, Ab b) {
        u16 address = RnAddressAndModify(GetRnUnit(a.GetName()), as.GetName());
        u16 value = mem.DRead(address);
        u16 sv = regs.sv;
        SetAcc(b.GetName(), ShiftBus40(value, sv), /*No saturation if logic shift*/regs.s == 1);
    }
    void movs(Register a, Ab b) {
        u64 value = SignExtend<16, u64>(RegToBus16(a.GetName()));
        u16 sv = regs.sv;
        SetAcc(b.GetName(), ShiftBus40(value, sv), /*No saturation if logic shift*/regs.s == 1);
    }
    void movs_r6_to(Ax b) {
        u16 value = regs.r[6];
        u16 sv = regs.sv;
        SetAcc(b.GetName(), ShiftBus40(value, sv), /*No saturation if logic shift*/regs.s == 1);
    }
    void movsi(RnOld a, Ab b, Imm5s s) {
        u64 value = SignExtend<16, u64>(RegToBus16(a.GetName()));
        u16 sv = SignExtend<5, u16>(s.storage);
        SetAcc(b.GetName(), ShiftBus40(value, sv), /*No saturation if logic shift*/regs.s == 1);
    }
private:
    RegisterState& regs;
    MemoryInterface& mem;

    u64 GetAcc(RegName name) const {
        switch(name) {
        case RegName::a0: case RegName::a0h: case RegName::a0l: case RegName::a0e: return regs.a[0].value;
        case RegName::a1: case RegName::a1h: case RegName::a1l: case RegName::a1e: return regs.a[1].value;
        case RegName::b0: case RegName::b0h: case RegName::b0l: case RegName::b0e: return regs.b[0].value;
        case RegName::b1: case RegName::b1h: case RegName::b1l: case RegName::b1e: return regs.b[1].value;
        default: throw "nope";
        }
    }

    u64 SaturateAcc(u64 value, bool storing) {
        if (!regs.sar[storing]) {
            if (value != SignExtend<32>(value)) {
                regs.fls = 1;
                if ((value >> 39) != 0)
                    return 0xFFFF'FFFF'8000'0000;
                else
                    return 0x0000'0000'7FFF'FFFF;
            }
            // note: fls doesn't change value otherwise
        }
        return value;
    }

    u16 RegToBus16(RegName reg) {
        switch(reg) {
        case RegName::a0: case RegName::a1: case RegName::b0: case RegName::b1:
            // get aXl, but unlike using RegName::aXl, this does never saturate.
            // This only happen to insturctions using "Register" oprand,
            // and doesn't apply to all instructions. Need test and special check.
            return GetAcc(reg) & 0xFFFF;
        case RegName::a0l: case RegName::a1l: case RegName::b0l: case RegName::b1l:
            return SaturateAcc(GetAcc(reg), false) & 0xFFFF;
        case RegName::a0h: case RegName::a1h: case RegName::b0h: case RegName::b1h:
            return (SaturateAcc(GetAcc(reg), false) >> 16) & 0xFFFF;
        case RegName::a0e: case RegName::a1e: case RegName::b0e: case RegName::b1e:
            throw "?";

        case RegName::r0: return regs.r[0];
        case RegName::r1: return regs.r[1];
        case RegName::r2: return regs.r[2];
        case RegName::r3: return regs.r[3];
        case RegName::r4: return regs.r[4];
        case RegName::r5: return regs.r[5];
        case RegName::r6: return regs.r[6];
        case RegName::r7: return regs.r[7];

        case RegName::x0: return regs.x[0];
        case RegName::x1: return regs.x[1];
        case RegName::y0: return regs.y[0];
        case RegName::y1: return regs.y[1];
        case RegName::p0:
        case RegName::p1:throw "?";


        case RegName::p:
            // This only happen to insturctions using "Register" oprand,
            // and doesn't apply to all instructions. Need test and special check.
            return (ProductToBus40(RegName::p0) >> 16) & 0xFFFF;

        case RegName::pc: throw "?";
        case RegName::sp: return regs.sp;
        case RegName::sv: return regs.sv;
        case RegName::lc: return regs.lc;

        case RegName::ar0: return regs.ar0.Get();
        case RegName::ar1: return regs.ar1.Get();

        case RegName::arp0: return regs.arp0.Get();
        case RegName::arp1: return regs.arp1.Get();
        case RegName::arp2: return regs.arp2.Get();
        case RegName::arp3: return regs.arp3.Get();

        case RegName::ext0:
        case RegName::ext1:
        case RegName::ext2:
        case RegName::ext3: throw "?";

        case RegName::stt0: return regs.stt0.Get();
        case RegName::stt1: return regs.stt1.Get();
        case RegName::stt2: return regs.stt2.Get();

        case RegName::st0:
        case RegName::st1:
        case RegName::st2: throw "?";

        case RegName::cfgi: return regs.cfgi.Get();
        case RegName::cfgj: return regs.cfgj.Get();

        case RegName::mod0: return regs.mod0.Get();
        case RegName::mod1: return regs.mod1.Get();
        case RegName::mod2: return regs.mod2.Get();
        case RegName::mod3: return regs.mod3.Get();
        default: throw "?";
        }
    }

    void SetAccFlag(u64 value) {
        if (value != SignExtend<40>(value))
            throw "remove this check later";
        regs.fz = value == 0;
        regs.fm = (value >> 39) != 0;
        regs.fe = value != SignExtend<32>(value);
        u64 bit31 = (value >> 31) & 1;
        u64 bit30 = (value >> 30) & 1;
        regs.fn = regs.fz || (!regs.fe && (bit31 ^ bit30) != 0);
    }

    void SetAcc(RegName name, u64 value, bool no_saturation = false) {
        SetAccFlag(value);

        if (!no_saturation)
            value = SaturateAcc(value, true);

        switch(name) {
        case RegName::a0: case RegName::a0h: case RegName::a0l: case RegName::a0e: regs.a[0].value = value; break;
        case RegName::a1: case RegName::a1h: case RegName::a1l: case RegName::a1e: regs.a[1].value = value; break;
        case RegName::b0: case RegName::b0h: case RegName::b0l: case RegName::b0e: regs.b[0].value = value; break;
        case RegName::b1: case RegName::b1h: case RegName::b1l: case RegName::b1e: regs.b[1].value = value; break;
        default: throw "nope";
        }
    }

    void SetAcc_NoSaturation(RegName name, u64 value) {
        SetAcc(name, value, true);
    }


    void RegFromBus16(RegName reg, u16 value) {
        switch(reg) {
        case RegName::a0: case RegName::a1: case RegName::b0: case RegName::b1:
            SetAcc(reg, SignExtend<16, u64>(value));
            break;
        case RegName::a0l: case RegName::a1l: case RegName::b0l: case RegName::b1l:
            SetAcc(reg, (u64)value);
            break;
        case RegName::a0h: case RegName::a1h: case RegName::b0h: case RegName::b1h:
            SetAcc(reg, SignExtend<32, u64>(value << 16));
            break;
        case RegName::a0e: case RegName::a1e: case RegName::b0e: case RegName::b1e:
            throw "?";

        case RegName::r0: regs.r[0] = value; break;
        case RegName::r1: regs.r[1] = value; break;
        case RegName::r2: regs.r[2] = value; break;
        case RegName::r3: regs.r[3] = value; break;
        case RegName::r4: regs.r[4] = value; break;
        case RegName::r5: regs.r[5] = value; break;
        case RegName::r6: regs.r[6] = value; break;
        case RegName::r7: regs.r[7] = value; break;

        case RegName::x0: regs.x[0] = value; break;
        case RegName::x1: regs.x[1] = value; break;
        case RegName::y0: regs.y[0] = value; break;
        case RegName::y1: regs.y[1] = value; break;
        case RegName::p0:
        case RegName::p1: throw "?";
        case RegName::p: // p0h
            regs.psm[0] = value > 0x7FFF; // ?
            regs.p[0].value = (regs.p[0].value & 0xFFFF) | (value << 16);
            break;

        case RegName::pc: throw "?";
        case RegName::sp: regs.sp = value; break;
        case RegName::sv: regs.sv = value; break;
        case RegName::lc: regs.lc = value; break;

        case RegName::ar0: regs.ar0.Set(value); break;
        case RegName::ar1: regs.ar1.Set(value); break;

        case RegName::arp0: regs.arp0.Set(value); break;
        case RegName::arp1: regs.arp1.Set(value); break;
        case RegName::arp2: regs.arp2.Set(value); break;
        case RegName::arp3: regs.arp3.Set(value); break;

        case RegName::ext0:
        case RegName::ext1:
        case RegName::ext2:
        case RegName::ext3: throw "?";

        case RegName::stt0: regs.stt0.Set(value); break;
        case RegName::stt1: regs.stt1.Set(value); break;
        case RegName::stt2: regs.stt2.Set(value); break;

        case RegName::st0: regs.st0.Set(value); break;
        case RegName::st1: regs.st1.Set(value); break;
        case RegName::st2: regs.st2.Set(value); break;

        case RegName::cfgi: regs.cfgi.Set(value); break;
        case RegName::cfgj: regs.cfgj.Set(value); break;

        case RegName::mod0: regs.mod0.Set(value); break;
        case RegName::mod1: regs.mod1.Set(value); break;
        case RegName::mod2: regs.mod2.Set(value); break;
        case RegName::mod3: regs.mod3.Set(value); break;
        default: throw "?";
        }
    }

    static u32 GetRnUnit(RegName reg) {
        switch(reg) {
        case RegName::r0: return 0; break;
        case RegName::r1: return 1; break;
        case RegName::r2: return 2; break;
        case RegName::r3: return 3; break;
        case RegName::r4: return 4; break;
        case RegName::r5: return 5; break;
        case RegName::r6: return 6; break;
        case RegName::r7: return 7; break;
        default: throw "?";
        }
    }

    u32 GetArRnUnit(u16 arrn) const {
        return regs.arrn[arrn];
    }

    StepValue GetArStep(u16 arstep) const {
        switch(regs.arstep[arstep]) {
        case 0: return StepValue::Zero;
        case 1: return StepValue::Increase;
        case 2: return StepValue::Decrease;
        case 3: return StepValue::PlusStep;
        case 4: case 6: return StepValue::Increase2;
        case 5: case 7: return StepValue::Decrease2;
        default: throw "???";
        }
    }

    u16 GetArOffset(u16 arstep) const {
        switch(regs.aroffset[arstep]) {
        case 0: return 0;
        case 1: return 1;
        case 2: case 3: return 0xFFFF;
        default: throw "???";
        }
    }

    u16 RnAddressAndModify(unsigned unit, StepValue step) {
        u16 ret = regs.r[unit];

        u16 s;
        switch(step) {
        case StepValue::Zero: s = 0; break;
        case StepValue::Increase: s = 1; break;
        case StepValue::Decrease: s = 0xFFFF; break;
        case StepValue::Increase2: s = 2; break;
        case StepValue::Decrease2: s = 0xFFFE; break;
        case StepValue::PlusStep:
            if (regs.ms[unit] && !regs.m[unit]) {
                s = unit < 4 ? regs.stepi0 : regs.stepj0;
            } else {
                s = unit < 4 ? regs.stepi : regs.stepj;
                s = SignExtend<7>(s);
            }
            break;
        default: throw "?";
        }

        if (s == 0)
            return ret;

        if (!regs.ms[unit] && regs.m[unit]) {
            // Do modular arithmatic
            // this part is tested but really weird
            u16 mod = unit < 4 ? regs.modi : regs.modj;

            u16 mask = 0;
            for (unsigned i = 0; i < 9; ++i) {
                mask |= mod >> i;
            }

            u16 next;
            if (s < 0x8000) {
                next = (ret + s) & mask;
                if (next == ((mod + 1) & mask)) {
                    next = 0;
                }
            } else {
                next = ret & mask;
                if (next == 0) {
                    next = mod + 1;
                }
                next += s;
                next &= mask;
            }
            regs.r[unit] &= ~mask;
            regs.r[unit] |= next;
        } else {
            regs.r[unit] += s;
        }
        return ret;
    }

    u32 ProductToBus32_NoShift(RegName reg) const {
        u32 unit;
        switch(reg) {
        case RegName::p0:
            unit = 0; break;
        case RegName::p1:
            unit = 1; break;
        default:
            throw "???";
        }
        return regs.p[unit].value;
    }

    u64 ProductToBus40(RegName reg) const {
        u32 unit;
        switch(reg) {
        case RegName::p0:
            unit = 0; break;
        case RegName::p1:
            unit = 1; break;
        default:
            throw "???";
        }
        u64 value = regs.p[unit].value;
        switch (regs.ps[unit]) {
        case 0:
            if (regs.psm[unit])
                value = SignExtend<32>(value);
            break;
        case 1:
            value >>= 1;
            if (regs.psm[unit])
                value = SignExtend<31>(value);
            break;
        case 2:
            value <<= 1;
            if (regs.psm[unit])
                value = SignExtend<33>(value);
            break;
        case 3:
            value <<= 2;
            if (regs.psm[unit])
                value = SignExtend<34>(value);
            break;
        }
        return value;
    }

    void ProductFromBus32(RegName reg, u32 value) {
        u32 unit;
        switch(reg) {
        case RegName::p0:
            unit = 0; break;
        case RegName::p1:
            unit = 1; break;
        default:
            throw "???";
        }
        regs.p[unit].value = value;
        regs.psm[unit] = value >> 31;
    }
};
