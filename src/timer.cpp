#include "crash.h"
#include "timer.h"

namespace Teakra {

enum CountMode : u32 {
    Single = 0,
    AutoRestart = 1,
    FreeRunning = 2,
    EventCount = 3,
};

void Timer::Reset() {
    update_mmio = 0;
    pause = 0;
    count_mode = 0;
    scale = 0;

    start_high = 0;
    start_low = 0;
    counter = 0;
    counter_high = 0;
    counter_low = 0;
}

void Timer::Restart() {
    ASSERT(count_mode < 4);
    if (count_mode != CountMode::FreeRunning) {
        counter = ((u32)start_high << 16) | start_low;
        UpdateMMIO();
    }
}

void Timer::Tick() {
    ASSERT(count_mode < 4);
    ASSERT(scale == 0);
    if (pause)
        return;
    if (count_mode == CountMode::EventCount)
        return;
    if (counter == 0) {
        if (count_mode == CountMode::AutoRestart) {
            Restart();
        } else if (count_mode == CountMode::FreeRunning) {
            counter = 0xFFFFFFFF;
            UpdateMMIO();
        }
    } else {
        --counter;
        UpdateMMIO();
        if (counter == 0)
            handler();
    }
}

void Timer::TickEvent() {
    if (pause)
        return;
    if (count_mode != CountMode::EventCount)
        return;
    if (counter == 0)
        return;
    --counter;
    UpdateMMIO();
    if (counter == 0)
        handler();
}

void Timer::UpdateMMIO() {
    if (!update_mmio)
        return;
    counter_high = counter >> 16;
    counter_low = counter & 0xFFFF;
}

} // namespace Teakra
