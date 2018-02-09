#pragma once

#include "common_types.h"
#include <memory>

namespace Teakra {

class MemoryInterface;

class Core {
public:
    Core(MemoryInterface& memory_interface);
    ~Core();
    void Reset();
    void Run(unsigned cycles);
    void SignalInterrupt(u32 i);
    void SignalVectoredInterrupt(u32 address);
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Teakra
