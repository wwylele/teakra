#pragma once

#include <array>
#include "common_types.h"

class MemoryInterface {
public:
    virtual ~MemoryInterface() = default;
    virtual u16 PRead(u32 addr) const = 0;
    virtual void PWrite(u32 addr, u16 value) = 0;
    virtual u16 DRead(u16 addr) = 0; // not const because it can be a FIFO register
    virtual void DWrite(u16 addr, u16 value) = 0;
};

class DspMemory : public MemoryInterface {
public:
    std::array<u16, 0x20000> program;
    std::array<u16, 0x20000> data;

    u16 PRead(u32 addr) const override {
        return program[addr];
    }
    void PWrite(u32 addr, u16 value) override {
        program[addr] = value;
    }
    u16 DRead(u16 addr) override {
        return data[addr];
    }
    void DWrite(u16 addr, u16 value) override {
        data[addr] = value;
    }
};
