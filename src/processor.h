#pragma once

#include <memory>
#include "common_types.h"

namespace Teakra {

class MemoryInterface;

class Processor {
public:
    Processor(MemoryInterface& memory_interface);
    ~Processor();
    void Reset();
    void Run(unsigned cycles);
    void SignalInterrupt(u32 i);
    void SignalVectoredInterrupt(u32 address);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Teakra
