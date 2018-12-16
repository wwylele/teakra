#include <string>
#include "btdmp.h"
#include "crash.h"

namespace Teakra {

class Btdmp::BtdmpTimingCallbacks : public CoreTiming::Callbacks {
public:
    BtdmpTimingCallbacks(Btdmp& parent) : parent(parent) {}
    void Tick() override {
        parent.Tick();
    }

    u64 GetMaxSkip() const override {
        return parent.GetMaxSkip();
    }

    void Skip(u64 ticks) override {
        parent.Skip(ticks);
    }

private:
    Btdmp& parent;
};

Btdmp::Btdmp(CoreTiming& core_timing) {
    core_timing.RegisterCallbacks(std::make_unique<BtdmpTimingCallbacks>(*this));
}

Btdmp::~Btdmp() = default;

void Btdmp::Reset() {
    transmit_period = 0;
    transmit_timer = 0;
    transmit_enable = 0;
    transmit_empty = true;
    transmit_full = false;
    transmit_queue = {};
}

void Btdmp::Tick() {
    if (transmit_enable) {
        ++transmit_timer;
        if (transmit_timer >= transmit_period) {
            transmit_timer = 0;
            std::array<std::int16_t, 2> sample;
            for (int i = 0; i < 2; ++i) {
                if (transmit_queue.empty()) {
                    std::printf("BTDMP: transmit buffer underrun\n");
                    sample[i] = 0;
                } else {
                    sample[i] = static_cast<s16>(transmit_queue.front());
                    transmit_queue.pop();
                    transmit_empty = transmit_queue.empty();
                    transmit_full = false;
                    if (transmit_empty) {
                        interrupt_handler();
                    }
                }
            }
            if (audio_callback) {
                audio_callback(sample);
            }
        }
    }
}

u64 Btdmp::GetMaxSkip() const {
    if (!transmit_enable || transmit_queue.empty()) {
        return CoreTiming::Callbacks::Infinity;
    }

    u64 ticks = 0;
    if (transmit_timer < transmit_period) {
        // number of ticks before the tick of the next transmit
        ticks += transmit_period - transmit_timer - 1;
    }

    // number of ticks from the next transmit to the one just before the transmit that empties
    // the buffer
    ticks += ((transmit_queue.size() + 1) / 2 - 1) * transmit_period;

    return ticks;
}

void Btdmp::Skip(u64 ticks) {
    if (!transmit_enable)
        return;

    if (transmit_timer >= transmit_period)
        transmit_timer = 0;

    u64 future_timer = transmit_timer + ticks;
    u64 cycles = future_timer / transmit_period;
    transmit_timer = (u16)(future_timer % transmit_period);

    for (u64 c = 0; c < cycles; ++c) {
        std::array<std::int16_t, 2> sample;
        for (int i = 0; i < 2; ++i) {
            if (transmit_queue.empty()) {
                sample[i] = 0;
            } else {
                sample[i] = static_cast<s16>(transmit_queue.front());
                transmit_queue.pop();
                ASSERT(!transmit_queue.empty());
                transmit_full = false;
            }
        }
        if (audio_callback) {
            audio_callback(sample);
        }
    }
}

} // namespace Teakra
