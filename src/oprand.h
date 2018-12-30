#pragma once
#include "common_types.h"

template <typename T, T... values>
inline constexpr bool NoOverlap = (values + ...) == (values | ...);

template <unsigned bits>
struct Oprand {
    static_assert(bits > 0 && bits <= 16);
    static constexpr unsigned Bits = bits;

protected:
    u16 storage;

    template <typename OprandT, unsigned pos>
    friend struct At;

    template <typename OprandT, u16 value>
    friend struct Const;
};

template <typename OprandT, unsigned pos>
struct At {
    static constexpr unsigned Bits = OprandT::Bits;
    static_assert((Bits < 16 && pos < 16 && Bits + pos <= 16) || (Bits == 16 && pos == 16));
    static constexpr u16 Mask = (((1 << Bits) - 1) << pos) & 0xFFFF;
    static constexpr bool NeedExpansion = pos == 16;
    static constexpr bool PassAsParameter = true;
    using FilterResult = OprandT;
    static constexpr OprandT Extract(u16 opcode, u16 expansion) {
        OprandT oprand{};
        if (NeedExpansion)
            oprand.storage = expansion;
        else
            oprand.storage = (u16)((opcode & Mask) >> pos);
        return oprand;
    }
};

template <typename OprandT, unsigned pos>
struct AtNamed {
    using BaseType = At<OprandT, pos>;
    static constexpr unsigned Bits = BaseType::Bits;
    static constexpr u16 Mask = BaseType::Mask;
    static constexpr bool NeedExpansion = BaseType::NeedExpansion;
    static constexpr bool PassAsParameter = BaseType::PassAsParameter;
    using FilterResult = typename BaseType::FilterResult::NameType;
    static constexpr auto Extract(u16 opcode, u16 expansion) {
        return BaseType::Extract(opcode, expansion).GetName();
    }
};

template <unsigned pos>
struct Unused {
    static_assert(pos < 16);
    static constexpr u16 Mask = 1 << pos;
    static constexpr bool NeedExpansion = false;
    static constexpr bool PassAsParameter = false;
};

template <typename OprandT, u16 value>
struct Const {
    static constexpr u16 Mask = 0;
    static constexpr bool NeedExpansion = false;
    static constexpr bool PassAsParameter = true;
    using FilterResult = OprandT;
    static constexpr OprandT Extract(u16, u16) {
        OprandT oprand{};
        oprand.storage = value;
        return oprand;
    }
};

enum class SumBase {
    Zero,
    Acc,
    Sv,
    SvRnd,
};

template <typename T, T value>
struct Cn {
    static constexpr u16 Mask = 0;
    static constexpr bool NeedExpansion = false;
    static constexpr bool PassAsParameter = true;
    using FilterResult = T;
    static constexpr T Extract(u16, u16) {
        return value;
    }
};

using SX = Cn<bool, true>;
using UX = Cn<bool, false>;
using SY = Cn<bool, true>;
using UY = Cn<bool, false>;
using BZr = Cn<SumBase, SumBase::Zero>;
using BAc = Cn<SumBase, SumBase::Acc>;
using BSv = Cn<SumBase, SumBase::Sv>;
using BSr = Cn<SumBase, SumBase::SvRnd>;
using PA = Cn<bool, true>;
using PP = Cn<bool, false>;
using Sub = Cn<bool, true>;
using Add = Cn<bool, false>;
using EMod = Cn<bool, false>;
using DMod = Cn<bool, true>;

struct NoParam {
    static constexpr u16 Mask = 0;
    static constexpr bool NeedExpansion = false;
    static constexpr bool PassAsParameter = false;
};

template <typename OprandT, unsigned pos, u16 value>
struct AtConst {
    using Base = At<OprandT, pos>;
    static_assert(Base::NeedExpansion == false, "");
    static constexpr u16 Mask = Base::Mask;
    static constexpr u16 Pad = value << pos;
};

//////////////////////////////////////////////////////////////////////////////

constexpr unsigned intlog2(unsigned n) {
    if (n % 2 != 0)
        throw "wtf";
    return (n == 2) ? 1 : 1 + intlog2(n / 2);
}

template <typename EnumT, EnumT... names>
struct EnumOprand : Oprand<intlog2(sizeof...(names))> {
    using NameType = EnumT;
    static constexpr EnumT values[] = {names...};
    constexpr EnumT GetName() const {
        return values[this->storage];
    }
};

template <typename EnumT>
struct EnumAllOprand : Oprand<intlog2((unsigned)EnumT::EnumEnd)> {
    using NameType = EnumT;
    constexpr EnumT GetName() const {
        return (EnumT)this->storage;
    }
};

// clang-format off

enum class RegName {
    a0, a0l, a0h, a0e,
    a1, a1l, a1h, a1e,
    b0, b0l, b0h, b0e,
    b1, b1l, b1h, b1e,

    r0, r1, r2, r3, r4, r5, r6, r7,

    y0, p,

    pc, sp, sv, lc,

    ar0, ar1,
    arp0, arp1, arp2, arp3,

    ext0, ext1, ext2, ext3,

    stt0, stt1, stt2,
    st0, st1, st2,
    cfgi, cfgj,
    mod0, mod1, mod2, mod3,

    undefine,
};

template <RegName ... reg_names>
using RegOprand = EnumOprand <RegName, reg_names...>;

struct Register : RegOprand<
    RegName::r0,
    RegName::r1,
    RegName::r2,
    RegName::r3,
    RegName::r4,
    RegName::r5,
    RegName::r7,
    RegName::y0,
    RegName::st0,
    RegName::st1,
    RegName::st2,
    RegName::p, // take special care of this as src oprand
    RegName::pc,
    RegName::sp,
    RegName::cfgi,
    RegName::cfgj,
    RegName::b0h,
    RegName::b1h,
    RegName::b0l,
    RegName::b1l,
    RegName::ext0,
    RegName::ext1,
    RegName::ext2,
    RegName::ext3,
    RegName::a0, // take special care of this as src oprand
    RegName::a1, // take special care of this as src oprand
    RegName::a0l,
    RegName::a1l,
    RegName::a0h,
    RegName::a1h,
    RegName::lc,
    RegName::sv
> {
    // only used in mov(Register, Register)
    constexpr RegName GetNameForMovFromP() {
        return (this->storage & 1) ? RegName::a1 : RegName::a0;
    }
};
struct Ax : RegOprand<
    RegName::a0,
    RegName::a1
> {};
struct Axl : RegOprand<
    RegName::a0l,
    RegName::a1l
> {};
struct Axh : RegOprand<
    RegName::a0h,
    RegName::a1h
> {};
struct Bx : RegOprand<
    RegName::b0,
    RegName::b1
> {};
struct Bxl : RegOprand<
    RegName::b0l,
    RegName::b1l
> {};
struct Bxh : RegOprand<
    RegName::b0h,
    RegName::b1h
> {};
struct Px : Oprand<1> {
    Px() = default;
    Px(u16 index) {
        this->storage = index;
    }
    constexpr u16 Index() const {
        return this->storage;
    }
};
struct Ab : RegOprand<
    RegName::b0,
    RegName::b1,
    RegName::a0,
    RegName::a1
> {};
struct Abl : RegOprand<
    RegName::b0l,
    RegName::b1l,
    RegName::a0l,
    RegName::a1l
> {};
struct Abh : RegOprand<
    RegName::b0h,
    RegName::b1h,
    RegName::a0h,
    RegName::a1h
> {};
struct Abe : RegOprand<
    RegName::b0e,
    RegName::b1e,
    RegName::a0e,
    RegName::a1e
> {};
struct Ablh : RegOprand<
    RegName::b0l,
    RegName::b0h,
    RegName::b1l,
    RegName::b1h,
    RegName::a0l,
    RegName::a0h,
    RegName::a1l,
    RegName::a1h
> {};
struct RnOld : RegOprand<
    RegName::r0,
    RegName::r1,
    RegName::r2,
    RegName::r3,
    RegName::r4,
    RegName::r5,
    RegName::r7,
    RegName::y0
> {};
struct Rn : RegOprand<
    RegName::r0,
    RegName::r1,
    RegName::r2,
    RegName::r3,
    RegName::r4,
    RegName::r5,
    RegName::r6,
    RegName::r7
> {
    Rn() = default;
    Rn(u16 index) {
        this->storage = index;
    }
    constexpr u16 Index() const {
        return this->storage;
    }
};

struct R45 : RegOprand<
    RegName::r4,
    RegName::r5
> {
    constexpr u16 Index() const {
        return this->storage + 4;
    }
};

struct R0123 : RegOprand<
    RegName::r0,
    RegName::r1,
    RegName::r2,
    RegName::r3
> {
    constexpr u16 Index() const {
        return this->storage;
    }
};

struct ArArpSttMod : RegOprand<
    RegName::ar0,
    RegName::ar1,
    RegName::arp0,
    RegName::arp1,
    RegName::arp2,
    RegName::arp3,
    RegName::undefine,
    RegName::undefine,
    RegName::stt0,
    RegName::stt1,
    RegName::stt2,
    RegName::undefine,
    RegName::mod0,
    RegName::mod1,
    RegName::mod2,
    RegName::mod3
> {};
struct ArArp : RegOprand<
    RegName::ar0,
    RegName::ar1,
    RegName::arp0,
    RegName::arp1,
    RegName::arp2,
    RegName::arp3,
    RegName::undefine,
    RegName::undefine
> {};
struct SttMod : RegOprand<
    RegName::stt0,
    RegName::stt1,
    RegName::stt2,
    RegName::undefine,
    RegName::mod0,
    RegName::mod1,
    RegName::mod2,
    RegName::mod3
> {};

struct Ar : RegOprand<
    RegName::ar0,
    RegName::ar1
> {
    constexpr u16 Index() const {
        return this->storage;
    }
};

struct Arp : RegOprand<
    RegName::arp0,
    RegName::arp1,
    RegName::arp2,
    RegName::arp3
> {
    constexpr u16 Index() const {
        return this->storage;
    }
};

enum SwapTypeValue {
    a0b0,
    a0b1,
    a1b0,
    a1b1,
    a0b0a1b1,
    a0b1a1b0,
    a0b0a1,
    a0b1a1,
    a1b0a0,
    a1b1a0,
    b0a0b1,
    b0a1b1,
    b1a0b0,
    b1a1b0,
    reserved0,
    reserved1,

    EnumEnd,
};

using SwapType = EnumAllOprand<SwapTypeValue>;

enum class StepValue {
    Zero,
    Increase,
    Decrease,
    PlusStep,
    Increase2Mode1,
    Decrease2Mode1,
    Increase2Mode2,
    Decrease2Mode2,
};

using StepZIDS = EnumOprand<StepValue,
    StepValue::Zero,
    StepValue::Increase,
    StepValue::Decrease,
    StepValue::PlusStep
>;

template<unsigned bits, u16 offset = 0>
struct ArIndex : Oprand<bits>{
    constexpr u16 Index() const {
        return this->storage + offset;
    }
};

struct ArRn1 : ArIndex<1> {};
struct ArRn2 : ArIndex<2> {};
struct ArStep1 : ArIndex<1> {};
struct ArStep1Alt : ArIndex<1, 2> {};
struct ArStep2 : ArIndex<2> {};
struct ArpRn1 : ArIndex<1> {};
struct ArpRn2 : ArIndex<2> {};
struct ArpStep1 : ArIndex<1> {};
struct ArpStep2 : ArIndex<2> {};

struct Address18_2 : Oprand<2> {
    constexpr u32 Address32() const {
        return (u32)(this->storage) << 16;
    }
};
struct Address18_16 : Oprand<16> {
    constexpr u32 Address32() const {
        return this->storage;
    }
};

constexpr u32 Address32(Address18_16 low, Address18_2 high) {
    return low.Address32() | high.Address32();
}

struct Address16 : Oprand<16> {
    constexpr u32 Address32() {
        return this->storage;
    }
};

struct RelAddr7 : Oprand<7> {
    constexpr u32 Relative32() {
        return SignExtend<7, u32>(this->storage);
    }
};

template <unsigned bits>
struct Imm : Oprand<bits> {
    constexpr u16 Unsigned16() const {
        return this->storage;
    }
};

template <unsigned bits>
struct Imms : Oprand<bits> {
    constexpr u16 Signed16() const {
        return SignExtend<bits, u16>(this->storage);
    }
};

struct Imm2 : Imm<2> {};
struct Imm4 : Imm<4> {};
struct Imm5 : Imm<5> {};
struct Imm5s : Imms<5> {};
struct Imm6s : Imms<6> {};
struct Imm7s : Imms<7> {};
struct Imm8 : Imm<8> {};
struct Imm8s : Imms<8> {};
struct Imm9 : Imm<9> {};
struct Imm16 : Imm<16> {};

struct MemImm8 : Imm8 {};
struct MemImm16 : Imm16 {};
struct MemR7Imm7s : Imm7s {};
struct MemR7Imm16 : Imm16 {};


enum class AlmOp {
    Or,
    And,
    Xor,
    Add,
    Tst0,
    Tst1,
    Cmp,
    Sub,
    Msu,
    Addh,
    Addl,
    Subh,
    Subl,
    Sqr,
    Sqra,
    Cmpu,

    Reserved
};

using Alm = EnumOprand<AlmOp,
    AlmOp::Or,
    AlmOp::And,
    AlmOp::Xor,
    AlmOp::Add,
    AlmOp::Tst0,
    AlmOp::Tst1,
    AlmOp::Cmp,
    AlmOp::Sub,
    AlmOp::Msu,
    AlmOp::Addh,
    AlmOp::Addl,
    AlmOp::Subh,
    AlmOp::Subl,
    AlmOp::Sqr,
    AlmOp::Sqra,
    AlmOp::Cmpu
>;

using Alu = EnumOprand<AlmOp,
    AlmOp::Or,
    AlmOp::And,
    AlmOp::Xor,
    AlmOp::Add,
    AlmOp::Reserved,
    AlmOp::Reserved,
    AlmOp::Cmp,
    AlmOp::Sub
>;

enum class AlbOp {
    Set,
    Rst,
    Chng,
    Addv,
    Tst0,
    Tst1,
    Cmpv,
    Subv,

    EnumEnd
};

using Alb = EnumAllOprand<AlbOp>;

enum class MulOp {
    Mpy,
    Mpysu,
    Mac,
    Macus,
    Maa,
    Macuu,
    Macsu,
    Maasu,
};

using Mul3 = EnumOprand<MulOp,
    MulOp::Mpy,
    MulOp::Mpysu,
    MulOp::Mac,
    MulOp::Macus,
    MulOp::Maa,
    MulOp::Macuu,
    MulOp::Macsu,
    MulOp::Maasu
>;

using Mul2 = EnumOprand<MulOp,
    MulOp::Mpy,
    MulOp::Mac,
    MulOp::Maa,
    MulOp::Macsu
>;

enum class ModaOp {
    Shr,
    Shr4,
    Shl,
    Shl4,
    Ror,
    Rol,
    Clr,
    Reserved,
    Not,
    Neg,
    Rnd,
    Pacr,
    Clrr,
    Inc,
    Dec,
    Copy,

};

using Moda4 = EnumOprand<ModaOp,
    ModaOp::Shr,
    ModaOp::Shr4,
    ModaOp::Shl,
    ModaOp::Shl4,
    ModaOp::Ror,
    ModaOp::Rol,
    ModaOp::Clr,
    ModaOp::Reserved,
    ModaOp::Not,
    ModaOp::Neg,
    ModaOp::Rnd,
    ModaOp::Pacr,
    ModaOp::Clrr,
    ModaOp::Inc,
    ModaOp::Dec,
    ModaOp::Copy
>;

using Moda3 = EnumOprand<ModaOp,
    ModaOp::Shr,
    ModaOp::Shr4,
    ModaOp::Shl,
    ModaOp::Shl4,
    ModaOp::Ror,
    ModaOp::Rol,
    ModaOp::Clr,
    ModaOp::Clrr
>;

enum class CondValue {
    True,
    Eq,
    Neq,
    Gt,
    Ge,
    Lt,
    Le,
    Nn,
    C,
    V,
    E,
    L,
    Nr,
    Niu0,
    Iu0,
    Iu1,

    EnumEnd
};

using Cond = EnumAllOprand<CondValue>;

struct BankFlags : Oprand<6>{
    constexpr bool Cfgi() const {
        return (this->storage & 1) != 0;
    }
    constexpr bool R4() const {
        return (this->storage & 2) != 0;
    }
    constexpr bool R1() const {
        return (this->storage & 4) != 0;
    }
    constexpr bool R0() const {
        return (this->storage & 8) != 0;
    }
    constexpr bool R7() const {
        return (this->storage & 16) != 0;
    }
    constexpr bool Cfgj() const {
        return (this->storage & 32) != 0;
    }
};

enum class CbsCondValue {
    Ge,
    Gt,

    EnumEnd
};

using CbsCond = EnumAllOprand<CbsCondValue>;

// clang-format on
