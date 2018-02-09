#pragma once

#include "common_types.h"
#include <array>

namespace Teakra {

class DataMemoryController {
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
        return 0x20000 + address + memory_bank[address / 0x8000] * 0x10000;
    }
private:
    u16 mmio_location = 0x8000;
    std::array<u8, 2> memory_bank{};
};

struct SharedMemory;
class MMIORegion;

class MemoryInterface {
public:
    MemoryInterface(SharedMemory& shared_memory, DataMemoryController& data_memory_controller,
        MMIORegion& mmio);
    u16 ProgramRead(u32 address) const;
    void ProgramWrite(u32 address, u16 value);
    u16 DataRead(u16 address); // not const because it can be a FIFO register
    void DataWrite(u16 address, u16 value);

private:
    SharedMemory& shared_memory;
    DataMemoryController& data_memory_controller;
    MMIORegion& mmio;
};

} // namespace Teakra
