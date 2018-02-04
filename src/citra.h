#pragma once
#include "memory.h"
#include "mmio.h"

class DspMemorySharedWithCitra : public MemoryInterface {
    u16* program;
    u16* data;

    MIU miu;
    MMIORegion mmio{miu};
public:
    DspMemorySharedWithCitra();
    u16 PRead(u32 addr) const override;
    void PWrite(u32 addr, u16 value) override;
    u16 DRead(u16 addr) override;
    void DWrite(u16 addr, u16 value) override;
};
