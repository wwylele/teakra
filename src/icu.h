#pragma once

#include "common_types.h"
#include <bitset>
#include <functional>
#include <array>

namespace Teakra {

class ICU {
public:
    using IrqBits = std::bitset<16>;
    u16 GetRequest() const {
        return (u16)request.to_ulong();
    }
    void Acknowledge(u16 irq_bits) {
        request &= IrqBits(irq_bits);
    }
    void Trigger(u16 irq_bits) {
        IrqBits bits(irq_bits);
        request |= bits;
        for (u32 irq = 0; irq < 16; ++irq) {
            if (bits[irq]) {
                for (u32 interrupt = 0; interrupt < enabled.size(); ++interrupt) {
                    if (enabled[interrupt][irq]) {
                        OnInterrupt(interrupt);
                    }
                }
                if (vectored_enabled[irq]) {
                    OnVectoredInterrupt(GetVector(irq));
                }
            }
        }
    }
    void TriggerSingle(u32 irq) {
        Trigger(1 << irq);
    }
    void SetEnable(u32 interrupt_index, u16 irq_bits) {
        enabled[interrupt_index] = IrqBits(irq_bits);
    }
    void SetEnableVectored(u16 irq_bits) {
        vectored_enabled = IrqBits(irq_bits);
    }
    u16 GetEnable(u32 interrupt_index) const {
        return (u16)enabled[interrupt_index].to_ulong();
    }
    u16 GetEnableVectored() const {
        return (u16)vectored_enabled.to_ulong();
    }

    u32 GetVector(u32 irq) const {
        return vector_low[irq] | ((u32)vector_high[irq] << 16);
    }

    std::function<void(u32)> OnInterrupt;
    std::function<void(u32)> OnVectoredInterrupt;

    std::array<u16, 16> vector_low, vector_high;
private:
    IrqBits request;
    std::array<IrqBits, 3> enabled;
    IrqBits vectored_enabled;
};

} // namespace Teakra
