#include <string>
#include "btdmp.h"

namespace Teakra {

Btdmp::Btdmp(const char* debug_string) : debug_string(debug_string) {}

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
                    std::printf("BTDMP%s: transmit buffer underrun", debug_string);
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
