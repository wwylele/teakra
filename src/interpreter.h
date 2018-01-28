#pragma once
#include "decoder.h"
#include "oprand.h"
#include "register.h"
#include "memory.h"

class Interpreter {
public:

    Interpreter (RegisterState& regs, MemoryInterface& mem) : regs(regs), mem(mem) {}
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

    void alu(Alu op, MemImm16 a, Ax b) {
        throw "unimplemented";
    }
    void alu(Alu op, MemR7Imm16 a, Ax b) {
        throw "unimplemented";
    }
    void alu(Alu op, Imm16 a, Ax b) {
        throw "unimplemented";
    }
    void alu(Alu op, Imm8 a, Ax b) {
        throw "unimplemented";
    }
    void alu(Alu op, MemR7Imm7s a, Ax b) {
        throw "unimplemented";
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

    void alb(Alb op, Imm16 a, MemImm8 b) {
        throw "unimplemented";
    }
    void alb(Alb op, Imm16 a, Rn b, StepZIDS bs) {
        throw "unimplemented";
    }
    void alb(Alb op, Imm16 a, Register b) {
        throw "unimplemented";
    }
    void alb_r6(Alb op, Imm16 a) {
        throw "unimplemented";
    }
    void alb(Alb op, Imm16 a, SttMod b) {
        throw "unimplemented";
    }

    void moda4(Moda4 op, Ax a, Cond cond) {
        throw "unimplemented";
    }

    void moda3(Moda3 op, Bx a, Cond cond) {
        throw "unimplemented";
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
    void bkreprst(R0425 a) {
        throw "unimplemented";
    }
    void bkreprst_memsp(Dummy) {
        throw "unimplemented";
    }
    void bkrepsto(R0425 a) {
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

    void bitrev(Rn a) {
        throw "unimplemented";
    }
    void bitrev_dbrv(Rn a) {
        throw "unimplemented";
    }
    void bitrev_ebrv(Rn a) {
        throw "unimplemented";
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
        throw "unimplemented";
    }
    void calla(Ax a) {
        throw "unimplemented";
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
        throw "unimplemented";
    }
    void load_stepi(Imm7s a) {
        throw "unimplemented";
    }
    void load_stepj(Imm7s a) {
        throw "unimplemented";
    }
    void load_page(Imm8 a) {
        throw "unimplemented";
    }
    void load_modi(Imm9 a) {
        throw "unimplemented";
    }
    void load_modj(Imm9 a) {
        throw "unimplemented";
    }
    void load_movpd(Imm2 a) {
        throw "unimplemented";
    }
    void load_ps01(Imm4 a) {
        throw "unimplemented";
    }

    void push(Imm16 a) {
        throw "unimplemented";
    }
    void push(Register a) {
        throw "unimplemented";
    }
    void push(Abe a) {
        throw "unimplemented";
    }
    void push(ArArpSttMod a) {
        throw "unimplemented";
    }
    void push_prpage(Dummy) {
        throw "unimplemented";
    }
    void push(Px a) {
        throw "unimplemented";
    }
    void push_r6(Dummy) {
        throw "unimplemented";
    }
    void push_repc(Dummy) {
        throw "unimplemented";
    }
    void push_x0(Dummy) {
        throw "unimplemented";
    }
    void push_x1(Dummy) {
        throw "unimplemented";
    }
    void push_y1(Dummy) {
        throw "unimplemented";
    }
    void pusha(Ax a) {
        throw "unimplemented";
    }
    void pusha(Bx a) {
        throw "unimplemented";
    }

    void pop(Register a) {
        throw "unimplemented";
    }
    void pop(Abe a) {
        throw "unimplemented";
    }
    void pop(ArArpSttMod a) {
        throw "unimplemented";
    }
    void pop(Bx a) {
        throw "unimplemented";
    }
    void pop_prpage(Dummy) {
        throw "unimplemented";
    }
    void pop(Px a) {
        throw "unimplemented";
    }
    void pop_r6(Dummy) {
        throw "unimplemented";
    }
    void pop_repc(Dummy) {
        throw "unimplemented";
    }
    void pop_x0(Dummy) {
        throw "unimplemented";
    }
    void pop_x1(Dummy) {
        throw "unimplemented";
    }
    void pop_y1(Dummy) {
        throw "unimplemented";
    }
    void popa(Ab a) {
        throw "unimplemented";
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
        throw "unimplemented";
    }
    void shfi(Ab a, Ab b, Imm6s s) {
        throw "unimplemented";
    }

    void tst4b(R0425 b, StepII2D2S bs) {
        throw "unimplemented";
    }
    void tst4b(R0425 b, StepII2D2S bs, Ax c) {
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
        throw "unimplemented";
    }
    void eint(Dummy) {
        throw "unimplemented";
    }

    void movd(R0123 a, StepZIDS as, R45 b, StepZIDS bs) {
        throw "unimplemented";
    }
    void movp(Axl a, Register b) {
        throw "unimplemented";
    }
    void movp(Ax a, Register b) {
        throw "unimplemented";
    }
    void movp(Rn a, StepZIDS as, R0123 b, StepZIDS bs) {
        throw "unimplemented";
    }
    void movpdw(Ax a) {
        throw "unimplemented";
    }

    s64 LoadFromAcc(RegName name) {
        switch(name) {
        case RegName::a0: case RegName::a0h: case RegName::a0l: return regs.a[0].value;
        case RegName::a1: case RegName::a1h: case RegName::a1l: return regs.a[1].value;
        case RegName::b0: case RegName::b0h: case RegName::b0l: return regs.b[0].value;
        case RegName::b1: case RegName::b1h: case RegName::b1l: return regs.b[1].value;
        default: throw "nope";
        }
    }

    void StoreToAcc_UpdateFlag(s64 value) {
        regs.fz = value == 0;
        regs.fm = value < 0;
        regs.fe = value > 0x7FFFFFFF || value < -0x80000000;
        u64 unsigned_value = (u64)value;
        u64 bit31 = (unsigned_value >> 31) & 1;
        u64 bit30 = (unsigned_value >> 30) & 1;
        regs.fn = regs.fz || (!regs.fe && (bit31 ^ bit30) != 0);
    }

    void LoadFromAcc_Saturation(s64& value) {
        if (regs.sar[0]) {
            if (value > 0x7FFFFFFF) {
                value = 0x7FFFFFFF;
                regs.fl[0] = 1;
            } else if (value < -0x80000000) {
                value = -0x80000000;
                regs.fl[0] = 1;
            }
            // note: fl[0 or 1] doesn't change value otherwise
        }
    }

    u16 LoadFromAcc_GetPart(RegName name, s64 value) {
        switch(name) {
        case RegName::a0h: case RegName::a1h: case RegName::b0h: case RegName::b1h:
            return ((u64)value >> 16) & 0xFFFF;
        case RegName::a0l: case RegName::a1l: case RegName::b0l: case RegName::b1l:
            return (u64)value & 0xFFFF;
        default: throw "nope";
        }
    }

    void StoreToAcc_Saturation(s64& value) {
        if (regs.sar[1]) {
            if (value > 0x7FFFFFFF) {
                value = 0x7FFFFFFF;
                regs.fl[0] = 1;
            } else if (value < -0x80000000) {
                value = -0x80000000;
                regs.fl[0] = 1;
            }
            // note: fl[0 or 1] doesn't change value otherwise
        }
    }

    void StoreToAcc(RegName name, s64 value) {
        switch(name) {
        case RegName::a0: case RegName::a0h: case RegName::a0l: regs.a[0].value = value; break;
        case RegName::a1: case RegName::a1h: case RegName::a1l: regs.a[1].value = value; break;
        case RegName::b0: case RegName::b0h: case RegName::b0l: regs.b[0].value = value; break;
        case RegName::b1: case RegName::b1h: case RegName::b1l: regs.b[1].value = value; break;
        default: throw "nope";
        }
    }

    void mov(Ab a, Ab b) {
        s64 value;
        value = LoadFromAcc(a.GetName());
        StoreToAcc_UpdateFlag(value);
        StoreToAcc_Saturation(value);
        StoreToAcc(b.GetName(), value);
    }
    void mov_dvm(Abl a) {
        throw "unimplemented";
    }
    void mov_x0(Abl a) {
        s64 value;
        value = LoadFromAcc(a.GetName());
        LoadFromAcc_Saturation(value);
        u16 value16 = LoadFromAcc_GetPart(a.GetName(), value);
        regs.x[0] = value16;
    }
    void mov_x1(Abl a) {
        s64 value;
        value = LoadFromAcc(a.GetName());
        LoadFromAcc_Saturation(value);
        u16 value16 = LoadFromAcc_GetPart(a.GetName(), value);
        regs.x[1] = value16;
    }
    void mov_y1(Abl a) {
        s64 value;
        value = LoadFromAcc(a.GetName());
        LoadFromAcc_Saturation(value);
        u16 value16 = LoadFromAcc_GetPart(a.GetName(), value);
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
        s64 value;
        value = LoadFromAcc(a.GetName());
        LoadFromAcc_Saturation(value);
        u16 value16 = LoadFromAcc_GetPart(a.GetName(), value);
        StoreToMemory(b, value16);
    }
    void mov(Axl a, MemImm16 b) {
        s64 value;
        value = LoadFromAcc(a.GetName());
        LoadFromAcc_Saturation(value);
        u16 value16 = LoadFromAcc_GetPart(a.GetName(), value);
        StoreToMemory(b, value16);
    }
    void mov(Axl a, MemR7Imm16 b) {
        s64 value;
        value = LoadFromAcc(a.GetName());
        LoadFromAcc_Saturation(value);
        u16 value16 = LoadFromAcc_GetPart(a.GetName(), value);
        StoreToMemory(b, value16);
    }
    void mov(Axl a, MemR7Imm7s b) {
        s64 value;
        value = LoadFromAcc(a.GetName());
        LoadFromAcc_Saturation(value);
        u16 value16 = LoadFromAcc_GetPart(a.GetName(), value);
        StoreToMemory(b, value16);
    }

    s64 StoreToAcc_ExtendS16(u16 value) {
        return (s16)value;
    }

    s64 StoreToAcc_ExtendU16(RegName r, u16 value) {
        switch(name) {
        case RegName::a0h: case RegName::a1h: case RegName::b0h: case RegName::b1h:
            return value;
        case RegName::a0l: case RegName::a1l: case RegName::b0l: case RegName::b1l:
            return (s32)(u32)(value << 16);
        default: throw "nope";
        }
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
        s64 value40 = StoreToAcc_ExtendS16(value);
        StoreToAcc_UpdateFlag(value40);
        // StoreToAcc_Saturation(value); no effect
        StoreToAcc(b.GetName(), value40);
    }
    void mov(MemImm8 a, Ab b) {
        u16 value = LoadFromMemory(a);
        s64 value40 = StoreToAcc_ExtendS16(value);
        StoreToAcc_UpdateFlag(value40);
        // StoreToAcc_Saturation(value); no effect
        StoreToAcc(b.GetName(), value40);
    }
    void mov(MemImm8 a, Ablh b) {
        u16 value = LoadFromMemory(a);
        s64 value40 = StoreToAcc_ExtendU16(b.GetName(), value);
        StoreToAcc_UpdateFlag(value40);
        // StoreToAcc_Saturation(value); no effect
        StoreToAcc(b.GetName(), value40);
    }
    void mov_eu(MemImm8 a, Axh b) {
        throw "unimplemented";
    }
    void mov(MemImm8 a, RnOld b) {
        u16 value = LoadFromMemory(a);

        switch(b.GetName()) {
        case RegName::r0: regs.r[0] = value; break;
        case RegName::r1: regs.r[1] = value; break;
        case RegName::r2: regs.r[2] = value; break;
        case RegName::r3: regs.r[3] = value; break;
        case RegName::r4: regs.r[4] = value; break;
        case RegName::r5: regs.r[5] = value; break;
        case RegName::r7: regs.r[7] = value; break;
        case RegName::y0: regs.y[0] = value; break;
        }
    }
    void mov_sv(MemImm8 a) {
        u16 value = LoadFromMemory(a);
        regs.sv = value;
    }
    void mov_dvm_to(Ab b) {
        throw "unimplemented";
    }
    void mov_icr_to(Ab b) {
        throw "unimplemented";
    }
    void mov(Imm16 a, Bx b) {
        u16 value = a.storage;
        s64 value40 = StoreToAcc_ExtendS16(value);
        StoreToAcc_UpdateFlag(value40);
        // StoreToAcc_Saturation(value); no effect
        StoreToAcc(b.GetName(), value40);
    }
    void mov(Imm16 a, Register b) {
        throw "unimplemented";
    }
    void mov_icr(Imm5 a) {
        throw "unimplemented";
    }
    void mov(Imm8s a, Axh b) {
        throw "unimplemented";
    }
    void mov(Imm8s a, RnOld b) {
        throw "unimplemented";
    }
    void mov_sv(Imm8s a) {
        throw "unimplemented";
    }
    void mov(Imm8 a, Axl b) {
        throw "unimplemented";
    }
    void mov(MemR7Imm16 a, Ax b) {
        throw "unimplemented";
    }
    void mov(MemR7Imm7s a, Ax b) {
        throw "unimplemented";
    }
    void mov(Rn a, StepZIDS as, Bx b) {
        throw "unimplemented";
    }
    void mov(Rn a, StepZIDS as, Register b) {
        throw "unimplemented";
    }
    void mov_memsp_to(Register b) {
        throw "unimplemented";
    }
    void mov_mixp_to(Register b) {
        throw "unimplemented";
    }
    void mov(RnOld a, MemImm8 b) {
        throw "unimplemented";
    }
    void mov_icr(Register a) {
        throw "unimplemented";
    }
    void mov_mixp(Register a) {
        throw "unimplemented";
    }
    void mov(Register a, Rn b, StepZIDS bs) {
        throw "unimplemented";
    }
    void mov(Register a, Bx b) {
        throw "unimplemented";
    }
    void mov(Register a, Register b) {
        throw "unimplemented";
    }
    void mov_repc_to(Ab b) {
        throw "unimplemented";
    }
    void mov_sv_to(MemImm8 b) {
        throw "unimplemented";
    }
    void mov_x0_to(Ab b) {
        throw "unimplemented";
    }
    void mov_x1_to(Ab b) {
        throw "unimplemented";
    }
    void mov_y1_to(Ab b) {
        throw "unimplemented";
    }
    void mov(Imm16 a, ArArp b) {
        throw "unimplemented";
    }
    void mov_r6(Imm16 a) {
        throw "unimplemented";
    }
    void mov_repc(Imm16 a) {
        throw "unimplemented";
    }
    void mov_stepi0(Imm16 a) {
        throw "unimplemented";
    }
    void mov_stepj0(Imm16 a) {
        throw "unimplemented";
    }
    void mov(Imm16 a, SttMod b) {
        throw "unimplemented";
    }
    void mov_prpage(Imm4 a) {
        throw "unimplemented";
    }

    void mov_a0h_stepi0(Dummy) {
        throw "unimplemented";
    }
    void mov_a0h_stepj0(Dummy) {
        throw "unimplemented";
    }
    void mov_stepi0_a0h(Dummy) {
        throw "unimplemented";
    }
    void mov_stepj0_a0h(Dummy) {
        throw "unimplemented";
    }

    void mov_prpage(Abl a) {
        throw "unimplemented";
    }
    void mov_repc(Abl a) {
        throw "unimplemented";
    }
    void mov(Abl a, ArArp b) {
        throw "unimplemented";
    }
    void mov(Abl a, SttMod b) {
        throw "unimplemented";
    }

    void mov_prpage_to(Abl b) {
        throw "unimplemented";
    }
    void mov_repc_to(Abl b) {
        throw "unimplemented";
    }
    void mov(ArArp a, Abl b) {
        throw "unimplemented";
    }
    void mov(SttMod a, Abl b) {
        throw "unimplemented";
    }

    void mov_repc_to(R04 b, StepII2 bs) {
        throw "unimplemented";
    }
    void mov(ArArp a, R04 b, StepII2 bs) {
        throw "unimplemented";
    }
    void mov(SttMod a, R04 b, StepII2 bs) {
        throw "unimplemented";
    }

    void mov_repc(R04 a, StepII2 as) {
        throw "unimplemented";
    }
    void mov(R04 a, StepII2 as, ArArp b) {
        throw "unimplemented";
    }
    void mov(R04 a, StepII2 as, SttMod b) {
        throw "unimplemented";
    }

    void mov_repc_to(MemR7Imm16 b) {
        throw "unimplemented";
    }
    void mov(ArArpSttMod a, MemR7Imm16 b) {
        throw "unimplemented";
    }

    void mov_repc(MemR7Imm16 a) {
        throw "unimplemented";
    }
    void mov(MemR7Imm16 a, ArArpSttMod b) {
        throw "unimplemented";
    }

    void mov_pc(Ax a) {
        throw "unimplemented";
    }
    void mov_pc(Bx a) {
        throw "unimplemented";
    }

    void mov_mixp_to(Bx b) {
        throw "unimplemented";
    }
    void mov_mixp_r6(Dummy) {
        throw "unimplemented";
    }
    void mov_p0h_to(Bx b) {
        throw "unimplemented";
    }
    void mov_p0h_r6(Dummy) {
        throw "unimplemented";
    }
    void mov_p0h_to(Register b) {
        throw "unimplemented";
    }
    void mov_p0(Ab a) {
        throw "unimplemented";
    }
    void mov_p1_to(Ab b) {
        throw "unimplemented";
    }

    void mov2(Px a, R0425 b, StepII2D2S bs) {
        throw "unimplemented";
    }
    void mov2s(Px a, R0425 b, StepII2D2S bs) {
        throw "unimplemented";
    }
    void mov2(R0425 a, StepII2D2S as, Px b) {
        throw "unimplemented";
    }
    void mova(Ab a, R0425 b, StepII2D2S bs) {
        throw "unimplemented";
    }
    void mova(R0425 a, StepII2D2S as, Ab b) {
        throw "unimplemented";
    }

    void mov_r6_to(Bx b) {
        throw "unimplemented";
    }
    void mov_r6_mixp(Dummy) {
        throw "unimplemented";
    }
    void mov_r6_to(Register b) {
        throw "unimplemented";
    }
    void mov_r6(Register a) {
        throw "unimplemented";
    }
    void mov_memsp_r6(Dummy) {
        throw "unimplemented";
    }
    void mov_r6_to(Rn b, StepZIDS bs) {
        throw "unimplemented";
    }
    void mov_r6(Rn a, StepZIDS as) {
        throw "unimplemented";
    }

    void movs(MemImm8 a, Ab b) {
        throw "unimplemented";
    }
    void movs(Rn a, StepZIDS as, Ab b) {
        throw "unimplemented";
    }
    void movs(Register a, Ab b) {
        throw "unimplemented";
    }
    void movs_r6_to(Ax b) {
        throw "unimplemented";
    }
    void movsi(RnOld a, Ab b, Imm5s s) {
        throw "unimplemented";
    }
private:
    RegisterState& regs;
    MemoryInterface& mem;
};
