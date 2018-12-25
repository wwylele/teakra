# Registers

Related code: [register.h](register.h)

## Basic Registers

The `RegisterState` class includes registers that program can directly access individually, such as `r0`~`r7` and accumulators, and registers that are not directly exposed to the program but affect execution. These registers have various bit length in hardware, but they are all represented as `u16` for consistency if possible(which makes it easier to define pseudo-registers, explained below). All extra bits are zero-padded. Exceptions are
 - program counter `pc`, as well as other program address registers, are 18-bit on hardware. they are zero-padded to `u32` here.
 - accumulators `a0`, `a1`, `b0` and `b1` are 40-bit on hardware. They are sign-extended to `u64` here.
 - Multiplication results `p0` and `p1` are 33-bit on hardware. They are stored in separate `u32` fields `p[*]` for lower bits and zero-padded one-bit `u16` fields `pe[*]` for the highest bit.

Note that many registers have signed-integer semantics with two's complement representation, but we still store them as unsigned integer. This is to cooperate with the interpreter policy where signed integer arithmetic is avoided in order to avoid undefined or implementation-defined behaviours.


## Shadow Registers

Many registers have "shadow" conterpart that are not directly accessible, but can be swapped with the main registers during context change. These shadow registers are classified into 4 groups:
 - bank exchange registers
 - Batch one-side shadow registers
 - Batch two-side shadow registers
 - shadow ar/arp

Bank exchange registers swap with there counterparts by the `banke` opcode. In the `banke` opcode, program can specify which registers to swap. These bank exchange registers are suffixed with `b` in `RegisterState`.

Batch shadow registers (both one-side and two-side) swap with coutnerparts by the `cntx r/s` opcode. They are always swapped together, so there is no need to provide ability to swap then individually. The difference of one-side and two-side is that for one-side registers, only main -> shadow transfer happens in `cntx s` and shadow -> main in `cntx r`, while the full swap between main <-> shadow happens in both `cntx s` and `cntx r` for two-side registers. In `RegisterState`, batch shadow registers are created using template `Shadow(Swap)(Array)Register`, where `Swap` templates are for two-side registers and `Array` templates are for register arrays. They are then added to the master templates `Shadow(Swap)RegisterList`, which includes all shadow register states by inheritance and handles value swapping together.

Shadow ar/arp registers behaves much like Batch two-side shadow registers for ar/arp registers, but they can be also swapped with smaller batch pieces by opcode `bankr`, so they are defined separately from `ShadowSwapRegisterList` as `ShadowSwapAr(p)`.

## Block Repeat Stack

Teak supports block repeat nested in 4 levels. It has a small 4 frame stack for this, with each frame storing the loop counter, the start and the end address. The `lc` (loop counter) register visible to the program is always the one in the stack top. It is implemented as a function `Lc()` instead of a normal `u16 lc` registers in order to emulate this behaviour.

## Pseudo-registers

These registers works like bit fields and are combination of basic registers. Program can read and write them in 16-bit words, while each bit of them affects the corresponding basic register. However, they are not implemented as bit fields here for the following reasons:
 - There are two sets of control/status pseudo-registers, many fields of which maps to the same basic registers but with different bit layout. One set is TeakLite-compatible and the other includes all Teak-exclusive registers.
 - Some bits have special behaviour other than simply store the states.


These pseudo-registers are defined as some C++ types with the same names, such as `cfgi`, `stt0` and `arp2`, and `RegisterState` provides two template functions `u16 Get<T>()` and `void Set<T>(u16)`, where `T` should be one of the predefined type names, for read from and write to pseudo-registers. Here is an example
```
u16 stt1_value = registers.Get<stt1>();
```
Internally, these predefined pseudo-register-types, using template, store pointers to basic registers and their bit position and length.

## Details of all registers
