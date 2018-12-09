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

    u64 GetMaxSkip() override {
        if (!parent.transmit_enable || parent.transmit_queue.empty()) {
            return Infinity;
        }

        u64 ticks = 0;
        if (parent.transmit_timer < parent.transmit_period) {
            // number of ticks before the tick of the next transmit
            ticks += parent.transmit_period - parent.transmit_timer - 1;
        }

        // number of ticks from the next transmit to the one just before the transmit that empties
        // the buffer
        ticks += ((parent.transmit_queue.size() + 1) / 2 - 1) * parent.transmit_period;

        return ticks;
    }

    void Skip(u64 ticks) override {
        if (!parent.transmit_enable)
            return;

        if (parent.transmit_timer >= parent.transmit_period)
            parent.transmit_timer = 0;

        u64 future_timer = parent.transmit_timer + ticks;
        u64 cycles = future_timer / parent.transmit_period;
        parent.transmit_timer = (u16)(future_timer % parent.transmit_period);

        for (u64 c = 0; c < cycles; ++c) {
            std::array<std::int16_t, 2> sample;
            for (int i = 0; i < 2; ++i) {
                if (parent.transmit_queue.empty()) {
                    sample[i] = 0;
                } else {
                    sample[i] = static_cast<s16>(parent.transmit_queue.front());
                    parent.transmit_queue.pop();
                    ASSERT(!parent.transmit_queue.empty());
                    parent.transmit_full = false;
                }
            }
            if (parent.audio_callback) {
                parent.audio_callback(sample);
            }
        }
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
                        handler();
                    }
                }
            }
            if (audio_callback) {
                audio_callback(sample);
            }
        }
    }
}

} // namespace Teakra
