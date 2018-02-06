#pragma once
#include "common_types.h"
#include <memory>
#include <array>
#include "icu.h"

class MIU {
public:
    u16 GetMMIOLocation() const {
        return mmio_location;
    }
    void SetMMIOLocation(u16 value) {
        mmio_location = value;
    }
    bool InMMIO(u16 addr) const {
        return addr >= mmio_location && addr < mmio_location + 0x800;
    }
    u16 ToMMIO(u16 addr)  const {
        return addr - mmio_location;
    }
    void SetMemoryBank(u8 region, u16 bank) {
        memory_bank[region] = bank;
    }
    u16 GetMemoryBank(u8 region) {
        return memory_bank[region];
    }
    u32 ConvertAddressByBank(u16 address) {
        return address + memory_bank[address / 0x8000] * 0x10000;
    }
private:
    u16 mmio_location = 0x8000;
    std::array<u8, 2> memory_bank{};
};

class MMIORegion {
public:
    MMIORegion(MIU& miu, ICU& icu);
    ~MMIORegion();
    u16 Read(u16 addr); // not const because it can be a FIFO register
    void Write(u16 addr, u16 value);
private:
    class Impl;
    std::unique_ptr<Impl> impl;

    MIU& miu;
    ICU& icu;
};
