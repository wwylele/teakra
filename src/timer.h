#pragma once

#include <functional>
#include <utility>
#include "common_types.h"
#include "core_timing.h"

namespace Teakra {

class Timer {
public:
    Timer(CoreTiming& core_timing);

    enum CountMode : u32 {
        Single = 0,
        AutoRestart = 1,
        FreeRunning = 2,
        EventCount = 3,
    };

    void Reset();

    void Restart();
    void Tick();
    void TickEvent();

    u16 update_mmio = 0;
    u16 pause = 0;
    u16 count_mode = 0;
    u16 scale = 0;

    u16 start_high = 0;
    u16 start_low = 0;
    u32 counter = 0;
    u16 counter_high = 0;
    u16 counter_low = 0;

    void SetInterruptHandler(std::function<void()> handler) {
        interrupt_handler = std::move(handler);
    }

private:
    std::function<void()> interrupt_handler;

    void UpdateMMIO();

    class TimerTimingCallbacks;
};

} // namespace Teakra
