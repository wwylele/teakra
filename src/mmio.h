#pragma once
#include "common_types.h"
#include <memory>
#include <array>
#include "icu.h"

namespace Teakra {

class MemoryInterfaceUnit;
class Apbp;
class Timer;

class MMIORegion {
public:
    MMIORegion(
        MemoryInterfaceUnit& miu,
        ICU& icu,
        Apbp& apbp_from_cpu,
        Apbp& apbp_from_dsp,
        std::array<Timer, 2>& timer
    );
    ~MMIORegion();
    u16 Read(u16 addr); // not const because it can be a FIFO register
    void Write(u16 addr, u16 value);
private:
    class Impl;
    std::unique_ptr<Impl> impl;

    MemoryInterfaceUnit& miu;
    ICU& icu;
    Apbp& apbp_from_cpu;
    Apbp& apbp_from_dsp;
    std::array<Timer, 2>& timer;
};

} // namespace Teakra
