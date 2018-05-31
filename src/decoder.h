#pragma once
#include "matcher.h"
#include "oprand.h"
#include <type_traits>
#include <vector>

template <typename V, typename F, u16 expected, typename ... OprandAtT>
struct MatcherCreator {

    static Matcher<V> Create(const char* name, F func) {
        // Oprands shouldn't overlap each other, nor overlap with the expected ones
        static_assert((OprandAtT::Mask + ... + expected) == (OprandAtT::Mask | ... | expected), "Error");

        auto proxy = [func](V& visitor, u16 opcode, u16 expansion) {
            return (visitor.*func)(OprandAtT::Filter(opcode, expansion) ...);
        };

        u16 mask = (~OprandAtT::Mask & ...);
        bool expanded = (OprandAtT::NeedExpansion || ...);
        return Matcher<V>(name, mask, expected, expanded, proxy);
    }
};

template <typename ... OprandAtConstT>
struct RejectorCreator {
    static constexpr Rejector rejector{(OprandAtConstT::Mask | ...), (OprandAtConstT::Pad | ...)};
};

template<typename V, typename ... OprandAtT>
struct VisitorFunction{
    using type = typename V::instruction_return_type (V::*)(typename OprandAtT::OprandType...);
};

template <typename V>
std::vector<Matcher<V>> GetDecodeTable() {
    return {

#define INST(name, expected, ...) MatcherCreator<V, typename VisitorFunction<V, __VA_ARGS__>::type, expected, __VA_ARGS__>::Create(#name, &V::name)
#define EXCEPT(...) Except(RejectorCreator<__VA_ARGS__>::rejector)

    // <<< Misc >>>
    INST(nop, 0x0000, DummyMatch),
    INST(norm, 0x94C0, At<Ax, 8>, At<Rn, 0>, At<StepZIDS, 3>),
    //INST(swap, 0x4980, At<SwapTypes, 0>),
    INST(trap, 0x0020, DummyMatch),

    // <<< ALM normal >>>
    INST(alm, 0xA000, At<Alm, 9>, At<MemImm8, 0>, At<Ax, 8>),
    INST(alm, 0x8080, At<Alm, 9>, At<Rn, 0>, At<StepZIDS, 3>, At<Ax, 8>),
    INST(alm, 0x80A0, At<Alm, 9>, At<Register, 0>, At<Ax, 8>),

    // <<< ALM r6 >>>
    INST(alm_r6, 0xD388, Const<Alm, 0>, At<Ax, 4>),
    INST(alm_r6, 0xD389, Const<Alm, 1>, At<Ax, 4>),
    INST(alm_r6, 0xD38A, Const<Alm, 2>, At<Ax, 4>),
    INST(alm_r6, 0xD38B, Const<Alm, 3>, At<Ax, 4>),
    INST(alm_r6, 0xD38C, Const<Alm, 4>, At<Ax, 4>),
    INST(alm_r6, 0xD38D, Const<Alm, 5>, At<Ax, 4>),
    INST(alm_r6, 0xD38E, Const<Alm, 6>, At<Ax, 4>),
    INST(alm_r6, 0xD38F, Const<Alm, 7>, At<Ax, 4>),
    INST(alm_r6, 0x9462, Const<Alm, 8>, At<Ax, 0>),
    INST(alm_r6, 0x9464, Const<Alm, 9>, At<Ax, 0>),
    INST(alm_r6, 0x9466, Const<Alm, 10>, At<Ax, 0>),
    INST(alm_r6, 0x5E23, Const<Alm, 11>, At<Ax, 8>),
    INST(alm_r6, 0x5E22, Const<Alm, 12>, At<Ax, 8>),
    INST(alm_r6, 0x5F41, Const<Alm, 13>, Const<Ax, 0>),
    INST(alm_r6, 0x9062, Const<Alm, 14>, At<Ax, 8>), // unused@0
    INST(alm_r6, 0x8A63, Const<Alm, 15>, At<Ax, 3>),

    // <<< ALU normal >>>
    INST(alu, 0xD4F8, At<Alu, 0>, At<MemImm16, 16>, At<Ax, 8>)
        .EXCEPT(AtConst<Alu, 0, 4>).EXCEPT(AtConst<Alu, 0, 5>),
    INST(alu, 0xD4D8, At<Alu, 0>, At<MemR7Imm16, 16>, At<Ax, 8>)
        .EXCEPT(AtConst<Alu, 0, 4>).EXCEPT(AtConst<Alu, 0, 5>),
    INST(alu, 0x80C0, At<Alu, 9>, At<Imm16, 16>, At<Ax, 8>)
        .EXCEPT(AtConst<Alu, 9, 4>).EXCEPT(AtConst<Alu, 9, 5>),
    INST(alu, 0xC000, At<Alu, 9>, At<Imm8, 0>, At<Ax, 8>)
        .EXCEPT(AtConst<Alu, 9, 4>).EXCEPT(AtConst<Alu, 9, 5>),
    INST(alu, 0x4000, At<Alu, 9>, At<MemR7Imm7s, 0>, At<Ax, 8>)
        .EXCEPT(AtConst<Alu, 9, 4>).EXCEPT(AtConst<Alu, 9, 5>),

    // <<< OR Extra >>>
    INST(or_, 0xD291, At<Ab, 10>, At<Ax, 6>, At<Ax, 5>),
    INST(or_, 0xD4A4, At<Ax, 8>, At<Bx, 1>, At<Ax, 0>),
    INST(or_, 0xD3C4, At<Bx, 10>, At<Bx, 1>, At<Ax, 0>),

    // <<< ALB normal >>>
    INST(alb, 0xE100, At<Alb, 9>, At<Imm16, 16>, At<MemImm8, 0>),
    INST(alb, 0x80E0, At<Alb, 9>, At<Imm16, 16>, At<Rn, 0>, At<StepZIDS, 3>),
    INST(alb, 0x81E0, At<Alb, 9>, At<Imm16, 16>, At<Register, 0>),
    INST(alb_r6, 0x47B8, At<Alb, 0>, At<Imm16, 16>),

    // <<< ALB SttMod >>>
    INST(alb, 0x43C8, Const<Alb, 0>, At<Imm16, 16>, At<SttMod, 0>),
    INST(alb, 0x4388, Const<Alb, 1>, At<Imm16, 16>, At<SttMod, 0>),
    INST(alb, 0x0038, Const<Alb, 2>, At<Imm16, 16>, At<SttMod, 0>),
    //INST(alb, 0x????, Const<Alb, 3>, At<Imm,16, 16>, At<SttMod, 0>),
    INST(alb, 0x9470, Const<Alb, 4>, At<Imm16, 16>, At<SttMod, 0>),
    INST(alb, 0x9478, Const<Alb, 5>, At<Imm16, 16>, At<SttMod, 0>),
    //INST(alb, 0x????, Const<Alb, 6>, At<Imm,16, 16>, At<SttMod, 0>),
    //INST(alb, 0x????, Const<Alb, 7>, At<Imm,16, 16>, At<SttMod, 0>),

    // <<< Add extra >>>
    INST(add, 0xD2DA, At<Ab, 10>, At<Bx, 0>),
    INST(add, 0x5DF0, At<Bx, 1>, At<Ax, 0>),
    INST(add_p1, 0xD782, At<Ax, 0>),
    INST(add, 0x5DF8, At<Px, 1>, At<Bx, 0>),
    INST(add_p0_p1, 0x5DC0, At<Ab, 2>),
    INST(add_p0_p1a, 0x5DC1, At<Ab, 2>),
    INST(add3_p0_p1, 0x4590, At<Ab, 2>),
    INST(add3_p0_p1a, 0x4592, At<Ab, 2>),
    INST(add3_p0a_p1a, 0x4593, At<Ab, 2>),

    // <<< Sub extra >>>
    INST(sub, 0x8A61, At<Ab, 3>, At<Bx, 8>),
    INST(sub, 0x8861, At<Bx, 4>, At<Ax, 3>),
    INST(sub_p1, 0xD4B9, At<Ax, 8>),
    INST(sub, 0x8FD0, At<Px, 1>, At<Bx, 0>),
    INST(sub_p0_p1, 0x5DC2, At<Ab, 2>),
    INST(sub_p0_p1a, 0x5DC3, At<Ab, 2>),
    INST(sub3_p0_p1, 0x80C6, At<Ab, 10>),
    INST(sub3_p0_p1a, 0x82C6, At<Ab, 10>),
    INST(sub3_p0a_p1a, 0x83C6, At<Ab, 10>),

    // <<< Mul >>>
    INST(mul, 0x8000, At<Mul3, 8>, At<Rn, 0>, At<StepZIDS, 3>, At<Imm16, 16>, At<Ax, 11>),
    INST(mul_y0, 0x8020, At<Mul3, 8>, At<Rn, 0>, At<StepZIDS, 3>, At<Ax, 11>),
    INST(mul_y0, 0x8040, At<Mul3, 8>, At<Register, 0>, At<Ax, 11>),
    INST(mul, 0xD000, At<Mul3, 8>, At<R45, 2>, At<StepZIDS, 5>, At<R0123, 0>, At<StepZIDS, 3>, At<Ax, 11>),
    INST(mul_y0_r6, 0x5EA0, At<Mul3, 1>, At<Ax, 0>),
    INST(mul_y0, 0xE000, At<Mul2, 9>, At<MemImm8, 0>, At<Ax, 11>),

    // <<< Mul Extra >>>
    INST(mpyi, 0x0800, At<Imm8s, 0>),
    // TODO: more

    // <<< MODA >>>
    INST(moda4, 0x6700, At<Moda4, 4>, At<Ax, 12>, At<Cond, 0>)
        .EXCEPT(AtConst<Moda4, 4, 7>),
    INST(moda3, 0x6F00, At<Moda3, 4>, At<Bx, 12>, At<Cond, 0>),

    // <<< Block repeat >>>
    INST(bkrep, 0x5C00, At<Imm8, 0>, At<Address16, 16>),
    INST(bkrep, 0x5D00, At<Register, 0>, At<Address18_16, 16>, At<Address18_2, 5>),
    INST(bkrep_r6, 0x8FDC, At<Address18_16, 16>, At<Address18_2, 0>),
    INST(bkreprst, 0xDA9C, At<ArRn2, 0>), // MemR0425
    INST(bkreprst_memsp, 0x5F48, DummyMatch), // Unused2@0
    INST(bkrepsto, 0xDADC, At<ArRn2, 0>), // MemR0425, Unused1@10
    INST(bkrepsto_memsp, 0x9468, DummyMatch), // Unused3@0

    // <<< Bank >>>
    INST(banke, 0x4B80, At<BankFlags, 0>),
    INST(bankr, 0x8CDF, DummyMatch),
    INST(bankr, 0x8CDC, At<Ar, 0>),
    INST(bankr, 0x8CD0, At<Ar, 2>, At<Arp, 0>),
    INST(bankr, 0x8CD8, At<Arp, 0>),

    // <<< Bitrev >>>
    INST(bitrev, 0x5EB8, At<Rn, 0>),
    INST(bitrev_dbrv, 0xD7B8, At<Rn, 0>),
    INST(bitrev_ebrv, 0xD7E0, At<Rn, 0>),

    // <<< Branching >>>
    INST(br, 0x4180, At<Address18_16, 16>, At<Address18_2, 4>, At<Cond, 0>),
    INST(brr, 0x5000, At<RelAddr7, 4>, At<Cond, 0>),

    // <<< Break >>>
    INST(break_, 0xD3C0, DummyMatch),

    // <<< Call >>>
    INST(call, 0x41C0, At<Address18_16, 16>, At<Address18_2, 4>, At<Cond, 0>),
    INST(calla, 0xD480, At<Axl, 8>),
    INST(calla, 0xD381, At<Ax, 4>),
    INST(callr, 0x1000, At<RelAddr7, 4>, At<Cond, 0>),

    // <<< Context >>>
    INST(cntx_s, 0xD380, DummyMatch),
    INST(cntx_r, 0xD390, DummyMatch),

    // <<< Return >>>
    INST(ret, 0x4580, At<Cond, 0>),
    INST(retd, 0xD780, DummyMatch),
    INST(reti, 0x45C0, At<Cond, 0>),
    INST(retic, 0x45D0, At<Cond, 0>),
    INST(retid, 0xD7C0, DummyMatch),
    INST(retidc, 0xD3C3, DummyMatch),
    INST(rets, 0x0900, At<Imm8, 0>),

    // <<< Load >>>
    INST(load_ps, 0x4D80, At<Imm2, 0>),
    INST(load_stepi, 0xDB80, At<Imm7s, 0>),
    INST(load_stepj, 0xDF80, At<Imm7s, 0>),
    INST(load_page, 0x0400, At<Imm8, 0>),
    INST(load_modi, 0x0200, At<Imm9, 0>),
    INST(load_modj, 0x0A00, At<Imm9, 0>),
    INST(load_movpd, 0xD7D8, At<Imm2, 1>), // unused1@0
    INST(load_ps01, 0x0010, At<Imm4, 0>),

    // <<< Push >>>
    INST(push, 0x5F40, At<Imm16, 16>),
    INST(push, 0x5E40, At<Register, 0>),
    INST(push, 0xD7C8, At<Abe, 1>), // unused1@0
    INST(push, 0xD3D0, At<ArArpSttMod, 0>),
    INST(push_prpage, 0xD7FC, DummyMatch), // unused2@0
    INST(push, 0xD78C, At<Px, 1>), // unused1@0
    INST(push_r6, 0xD4D7, DummyMatch), // unused1@5
    INST(push_repc, 0xD7F8, DummyMatch), // unused2@0
    INST(push_x0, 0xD4D4, DummyMatch), // unused1@5
    INST(push_x1, 0xD4D5, DummyMatch), // unused1@5
    INST(push_y1, 0xD4D6, DummyMatch), // unused1@5
    INST(pusha, 0x4384, At<Ax, 6>), // unused2@0
    INST(pusha, 0xD788, At<Bx, 1>), // unused1@0

    // <<< Pop >>>
    INST(pop, 0x5E60, At<Register, 0>),
    INST(pop, 0x47B4, At<Abe, 0>),
    INST(pop, 0x80C7, At<ArArpSttMod, 8>),
    INST(pop, 0x0006, At<Bx, 5>), // unused1@0
    INST(pop_prpage, 0xD7F4, DummyMatch), // unused2@0
    INST(pop, 0xD496, At<Px, 0>),
    INST(pop_r6, 0x0024, DummyMatch), // unused1@0
    INST(pop_repc, 0xD7F0, DummyMatch), // unued2@0
    INST(pop_x0, 0xD494, DummyMatch),
    INST(pop_x1, 0xD495, DummyMatch),
    INST(pop_y1, 0x0004, DummyMatch), // unused1@0
    INST(popa, 0x47B0, At<Ab, 0>),

    // <<< Repeat >>>
    INST(rep, 0x0C00, At<Imm8, 0>),
    INST(rep, 0x0D00, At<Register, 0>),
    INST(rep_r6, 0x0002, DummyMatch), // unused1@0

    // <<< Shift >>>
    INST(shfc, 0xD280, At<Ab, 10>, At<Ab, 5>, At<Cond, 0>),
    INST(shfi, 0x9240, At<Ab, 10>, At<Ab, 7>, At<Imm6s, 0>),

    // <<< TSTB >>>
    INST(tst4b, 0x80C1, At<ArRn2, 10>, At<ArStep2, 8>),
    INST(tst4b, 0x4780, At<ArRn2, 2>, At<ArStep2, 0>, At<Ax, 4>),
    INST(tstb, 0xF000, At<MemImm8, 0>, At<Imm4, 8>),
    INST(tstb, 0x9020, At<Rn, 0>, At<StepZIDS, 3>, At<Imm4, 8>),
    INST(tstb, 0x9000, At<Register, 0>, At<Imm4, 8>)
        .EXCEPT(AtConst<Register, 0, 24>), // override by tstb_r6
    INST(tstb_r6, 0x9018, At<Imm4, 8>),
    INST(tstb, 0x0028, At<SttMod, 0>, At<Imm16, 16>), // unused12@20

    // <<< AND Extra >>>
    INST(and_, 0x6770, At<Ab, 2>, At<Ab, 0>, At<Ax, 12>),

    // <<< Interrupt >>>
    INST(dint, 0x43C0, DummyMatch),
    INST(eint, 0x4380, DummyMatch),

    // <<< MODR >>>
    INST(modr, 0x0080, At<Rn, 0>, At<StepZIDS, 3>),
    INST(modr_dmod, 0x00A0, At<Rn, 0>, At<StepZIDS, 3>),
    INST(modr_i2, 0x4990, At<Rn, 0>),
    INST(modr_i2_dmod, 0x4998, At<Rn, 0>),
    INST(modr_d2, 0x5DA0, At<Rn, 0>),
    INST(modr_d2_dmod, 0x5DA8, At<Rn, 0>),
    INST(modr_eemod, 0xD294, At<ArpRn2, 10>, At<ArpStep2, 0>, At<ArpStep2, 5>),
    INST(modr_edmod, 0x0D80, At<ArpRn2, 5>, At<ArpStep2, 1>, At<ArpStep2, 3>),
    INST(modr_demod, 0x8464, At<ArpRn2, 8>, At<ArpStep2, 0>, At<ArpStep2, 3>),
    INST(modr_ddmod, 0x0D81, At<ArpRn2, 5>, At<ArpStep2, 1>, At<ArpStep2, 3>),

    // <<< MOV >>>
    INST(mov, 0xD290, At<Ab, 10>, At<Ab, 5>),
    INST(mov_dvm, 0xD298, At<Abl, 10>),
    INST(mov_x0, 0xD2D8, At<Abl, 10>),
    INST(mov_x1, 0xD394, At<Abl, 0>),
    INST(mov_y1, 0xD384, At<Abl, 0>),

    INST(mov, 0x3000, At<Ablh, 9>, At<MemImm8, 0>),
    INST(mov, 0xD4BC, At<Axl, 8>, At<MemImm16, 16>),
    INST(mov, 0xD49C, At<Axl, 8>, At<MemR7Imm16, 16>),
    INST(mov, 0xDC80, At<Axl, 8>, At<MemR7Imm7s, 0>),

    INST(mov, 0xD4B8, At<MemImm16, 16>, At<Ax, 8>),
    INST(mov, 0x6100, At<MemImm8, 0>, At<Ab, 11>),
    INST(mov, 0x6200, At<MemImm8, 0>, At<Ablh, 10>),
    INST(mov_eu, 0x6500, At<MemImm8, 0>, At<Axh, 12>),
    INST(mov, 0x6000, At<MemImm8, 0>, At<RnOld, 10>),
    INST(mov_sv, 0x6D00, At<MemImm8, 0>),

    INST(mov_dvm_to, 0xD491, At<Ab, 5>),
    INST(mov_icr_to, 0xD492, At<Ab, 5>),

    INST(mov, 0x5E20, At<Imm16, 16>, At<Bx, 8>),
    INST(mov, 0x5E00, At<Imm16, 16>, At<Register, 0>),
    INST(mov_icr, 0x4F80, At<Imm5, 0>),
    INST(mov, 0x2500, At<Imm8s, 0>, At<Axh, 12>),
    // ext move skippeda
    INST(mov, 0x2300, At<Imm8s, 0>, At<RnOld, 10>),
    INST(mov_sv, 0x0500, At<Imm8s, 0>),
    INST(mov, 0x2100, At<Imm8, 0>, At<Axl, 12>),

    INST(mov, 0xD498, At<MemR7Imm16, 16>, At<Ax, 8>),
    INST(mov, 0xD880, At<MemR7Imm7s, 0>, At<Ax, 8>),
    INST(mov, 0x98C0, At<Rn, 0>, At<StepZIDS, 3>, At<Bx, 8>),
    INST(mov, 0x1C00, At<Rn, 0>, At<StepZIDS, 3>, At<Register, 5>),

    INST(mov_memsp_to, 0x47E0, At<Register, 0>),
    INST(mov_mixp_to, 0x47C0, At<Register, 0>),
    INST(mov, 0x2000, At<RnOld, 9>, At<MemImm8, 0>),
    INST(mov_icr, 0x4FC0, At<Register, 0>),
    INST(mov_mixp, 0x5E80, At<Register, 0>),
    INST(mov, 0x1800, At<Register, 5>, At<Rn, 0>, At<StepZIDS, 3>)
        .EXCEPT(AtConst<Register, 5, 24>).EXCEPT(AtConst<Register, 5, 25>), // override by mov_r6(_to)
    INST(mov, 0x5EC0, At<Register, 0>, At<Bx, 5>),
    INST(mov, 0x5800, At<Register, 0>, At<Register, 5>),
    INST(mov_repc_to, 0xD490, At<Ab, 5>),
    INST(mov_sv_to, 0x7D00, At<MemImm8, 0>),
    INST(mov_x0_to, 0xD493, At<Ab, 5>),
    INST(mov_x1_to, 0x49C1, At<Ab, 4>),
    INST(mov_y1_to, 0xD299, At<Ab, 10>),

    // <<< MOV load >>>
    INST(mov, 0x0008, At<Imm16, 16>, At<ArArp, 0>),
    INST(mov_r6, 0x0023, At<Imm16, 16>),
    INST(mov_repc, 0x0001, At<Imm16, 16>),
    INST(mov_stepi0, 0x8971, At<Imm16, 16>),
    INST(mov_stepj0, 0x8979, At<Imm16, 16>),
    INST(mov, 0x0030, At<Imm16, 16>, At<SttMod, 0>),
    INST(mov_prpage, 0x5DD0, At<Imm4, 0>),

    // <<< <<< MOV p/d >>>
    INST(movd, 0x5F80, At<R0123, 0>, At<StepZIDS, 3>, At<R45, 2>, At<StepZIDS, 5>),
    INST(movp, 0x0040, At<Axl, 5>, At<Register, 0>),
    INST(movp, 0x0D40, At<Ax, 5>, At<Register, 0>),
    INST(movp, 0x0600, At<Rn, 0>, At<StepZIDS, 3>, At<R0123, 5>, At<StepZIDS, 7>),
    INST(movpdw, 0xD499, At<Ax, 8>),

    // <<< MOV 2 >>>
    INST(mov_a0h_stepi0, 0xD49B, DummyMatch),
    INST(mov_a0h_stepj0, 0xD59B, DummyMatch),
    INST(mov_stepi0_a0h, 0xD482, DummyMatch),
    INST(mov_stepj0_a0h, 0xD582, DummyMatch),

    INST(mov_prpage, 0x9164, At<Abl, 0>),
    INST(mov_repc, 0x9064, At<Abl, 0>),
    INST(mov, 0x9540, At<Abl, 3>, At<ArArp, 0>),
    INST(mov, 0x9C60, At<Abl, 3>, At<SttMod, 0>),

    INST(mov_prpage_to, 0x5EB0, At<Abl, 0>),
    INST(mov_repc_to, 0xD2D9, At<Abl, 10>),
    INST(mov, 0x9560, At<ArArp, 0>, At<Abl, 3>),
    INST(mov, 0xD2F8, At<SttMod, 0>, At<Abl, 10>),

    INST(mov_repc_to, 0xD7D0, At<ArRn1, 1>, At<ArStep1, 0>),
    INST(mov, 0xD488, At<ArArp, 0>, At<ArRn1, 8>, At<ArStep1, 5>),
    INST(mov, 0x49A0, At<SttMod, 0>, At<ArRn1, 4>, At<ArStep1, 3>),

    INST(mov_repc, 0xD7D4, At<ArRn1, 1>, At<ArStep1, 0>),
    INST(mov, 0x8062, At<ArRn1, 4>, At<ArStep1, 3>, At<ArArp, 8>),
    INST(mov, 0x8063, At<ArRn1, 4>, At<ArStep1, 3>, At<SttMod, 8>),

    INST(mov_repc_to, 0xD3C8, At<MemR7Imm16, 16>), // unused3@0
    INST(mov, 0x5F50, At<ArArpSttMod, 0>, At<MemR7Imm16, 16>),

    INST(mov_repc, 0xD2DC, At<MemR7Imm16, 16>), // unused2@0, unused1@10
    INST(mov, 0x4D90, At<MemR7Imm16, 16>, At<ArArpSttMod, 0>),

    INST(mov_pc, 0x886B, At<Ax, 8>),
    INST(mov_pc, 0x8863, At<Bx, 8>),

    INST(mov_mixp_to, 0x8A73, At<Bx, 3>),
    INST(mov_mixp_r6, 0x4381, DummyMatch),
    INST(mov_p0h_to, 0x4382, At<Bx, 0>),
    INST(mov_p0h_r6, 0xD3C2, DummyMatch),
    INST(mov_p0h_to, 0x4B60, At<Register, 0>),
    INST(mov_p0, 0x8FD4, At<Ab, 0>),
    INST(mov_p1_to, 0x8FD8, At<Ab, 0>),

    INST(mov2, 0x88D0, At<Px, 1>, At<ArRn2, 8>, At<ArStep2, 2>),
    INST(mov2s, 0x88D1, At<Px, 1>, At<ArRn2, 8>, At<ArStep2, 2>),
    INST(mov2, 0xD292, At<ArRn2, 10>, At<ArStep2, 5>, At<Px, 0>),
    INST(mova, 0x4DC0, At<Ab, 4>, At<ArRn2, 2>, At<ArStep2, 0>),
    INST(mova, 0x4BC0, At<ArRn2, 2>, At<ArStep2, 0>, At<Ab, 4>),

    INST(mov_r6_to, 0xD481, At<Bx, 8>),
    INST(mov_r6_mixp, 0x43C1, DummyMatch),
    INST(mov_r6_to, 0x5F00, At<Register, 0>),
    INST(mov_r6, 0x5F60, At<Register, 0>),
    INST(mov_memsp_r6, 0xD29C, DummyMatch), // Unused2@0, Unused1@10
    INST(mov_r6_to, 0x1B00, At<Rn, 0>, At<StepZIDS, 3>),
    INST(mov_r6, 0x1B20, At<Rn, 0>, At<StepZIDS, 3>),

    INST(movs, 0x6300, At<MemImm8, 0>, At<Ab, 11>),
    INST(movs, 0x0180, At<Rn, 0>, At<StepZIDS, 3>, At<Ab, 5>),
    INST(movs, 0x0100, At<Register, 0>, At<Ab, 5>),
    INST(movs_r6_to, 0x5F42, At<Ax, 0>),
    INST(movsi, 0x4080, At<RnOld, 9>, At<Ab, 5>, At<Imm5s, 0>),
    };
}

template<typename V>
Matcher<V> Decode(u16 instruction) {
    static const auto table = GetDecodeTable<V>();

    const auto matches_instruction = [instruction](const auto& matcher) { return matcher.Matches(instruction); };

    auto iter = std::find_if(table.begin(), table.end(), matches_instruction);
    if (iter == table.end()) {
        return Matcher<V>::AllMatcher([](V& v, u16 opcode, u16){
            return v.undefined(opcode);
        });
    } else {
        auto other = std::find_if(iter + 1, table.end(), matches_instruction);
        if (other != table.end()) {
            printf("!!!Decode Clash For %04X!!!\n", instruction);
            printf("First = %s\n", iter->GetName());
            printf("Second = %s\n", other->GetName());
            throw "wtf";
        }
        return *iter;
    }
}
