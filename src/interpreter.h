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
        if (regs.ConditionPass(cond)) {
            // HACK: hard code for the dsptester
            regs.b[0].value = 0;
        }
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
        regs.page = a.storage;
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
        u32 value32 = SignExtend<8>((u32)(mem.DRead(regs.sp++) & 0xFF));
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
        u64 value = SignExtend<32>((u64)((h << 16) | l));
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
        throw "unimplemented";
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
        u16 address = RnAddressAndModify(a.GetName(), as);
        u16 value = mem.DRead(address);
        RegFromBus16(b.GetName(), value);
    }
    void mov(Rn a, StepZIDS as, Register b) {
        u16 address = RnAddressAndModify(a.GetName(), as);
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
        u16 address = RnAddressAndModify(b.GetName(), bs);
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
        u16 value = regs.repc;
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
        if (a.GetName() == RegName::p0) {
            u64 value = ProductToBus40(RegName::p0);
            regs.r[6] = (value >> 16) & 0xFFFF;
        } if (a.GetName() == RegName::a0 || a.GetName() == RegName::a1) {
            // get aXl, but unlike using RegName::aXl, this does never saturates
            u64 value = GetAcc(RegName::p0);
            regs.r[6] = value & 0xFFFF;
        }
        u16 value = RegToBus16(a.GetName());
        regs.r[6] = value;
    }
    void mov_memsp_r6(Dummy) {
        u16 value = mem.DRead(regs.sp);
        regs.r[6] = value;
    }
    void mov_r6_to(Rn b, StepZIDS bs) {
        u16 value = regs.r[6];
        u16 address = RnAddressAndModify(b.GetName(), bs);
        mem.DWrite(address, value);
    }
    void mov_r6(Rn a, StepZIDS as) {
        u16 address = RnAddressAndModify(a.GetName(), as);
        u16 value = mem.DRead(address);
        regs.r[6] = value;
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

    u64 GetAcc(RegName name) {
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
                regs.fl[0] = 1;
                if ((value >> 39) != 0)
                    return 0xFFFF'FFFF'8000'0000;
                else
                    return 0x0000'0000'7FFF'FFFF;
            }
            // note: fl[0] doesn't change value otherwise
        }
        return value;
    }

    u16 RegToBus16(RegName reg) {
        switch(reg) {
        case RegName::a0: case RegName::a1: case RegName::b0: case RegName::b1:
            throw "undefined???";
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
        case RegName::p:throw "?";

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

    void SetAcc(RegName name, u64 value, bool no_saturation = false) {
        regs.fz = value == 0;
        regs.fm = (value >> 39) != 0;
        regs.fe = value != SignExtend<32>(value);
        u64 bit31 = (value >> 31) & 1;
        u64 bit30 = (value >> 30) & 1;
        regs.fn = regs.fz || (!regs.fe && (bit31 ^ bit30) != 0);

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


    void RegFromBus16(RegName reg, u16 value) {
        switch(reg) {
        case RegName::a0: case RegName::a1: case RegName::b0: case RegName::b1:
            SetAcc(reg, SignExtend<16>((u64)value));
            break;
        case RegName::a0l: case RegName::a1l: case RegName::b0l: case RegName::b1l:
            SetAcc(reg, (u64)value);
            break;
        case RegName::a0h: case RegName::a1h: case RegName::b0h: case RegName::b1h:
            SetAcc(reg, SignExtend<32>((u64)(value << 16)));
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

    u16 RnAddressAndModify(RegName reg, StepZIDS step) {
        // TODO: Verify this function is correct!!!
        unsigned unit;
        switch(reg) {
        case RegName::r0: unit = 0; break;
        case RegName::r1: unit = 1; break;
        case RegName::r2: unit = 2; break;
        case RegName::r3: unit = 3; break;
        case RegName::r4: unit = 4; break;
        case RegName::r5: unit = 5; break;
        case RegName::r6: unit = 6; break;
        case RegName::r7: unit = 7; break;
        default: throw "?";
        }
        u16 ret = regs.r[unit];

        u16 s;
        switch(step.GetName()) {
        case StepZIDSValue::Zero: s = 0; break;
        case StepZIDSValue::Increase: s = 1; break;
        case StepZIDSValue::Decrease: s = 0xFFFF; break;
        case StepZIDSValue::PlusStep:
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

        u16 mod;
        if (!regs.ms[unit] && regs.m[unit]) {
            mod = unit < 4 ? regs.modi : regs.modj;
        } else {
            mod = 0;
        }

        if (!mod) {
            regs.r[unit] += s;
            return ret;
        }

        u16 mask = 0;
        for (unsigned i = 0; i < 9; ++i) {
            mask |= mod >> i;
        }

        u16 l = regs.r[unit] & mask;
        regs.r[unit] &= ~mask;
        if (l == mod && s < 0x8000) {
            l = 0;
        } else if (l == 0 && s >= 0x8000) {
            l = mod;
        } else {
            l += s;
            l &= mask;
        }
        regs.r[unit] |= l;
        return ret;
    }

    u64 ProductToBus40(RegName reg) {
        unsigned unit;
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
        unsigned unit;
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
