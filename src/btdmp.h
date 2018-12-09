#pragma once
#include <array>
#include <cstdio>
#include <functional>
#include <queue>
#include "common_types.h"
#include "core_timing.h"

namespace Teakra {

class Btdmp {
public:
    Btdmp(CoreTiming& core_timing);
    ~Btdmp();

    void Reset();

    void SetTransmitPeriod(u16 value) {
        transmit_period = value;
    }

    u16 GetTransmitPeriod() {
        return transmit_period;
    }

    void SetTransmitEnable(u16 value) {
        transmit_enable = value;
    }

    u16 GetTransmitEnable() {
        return transmit_enable;
    }

    u16 GetTransmitEmpty() {
        return transmit_empty;
    }

    u16 GetTransmitFull() {
        return transmit_full;
    }

    void Send(u16 value) {
        if (transmit_queue.size() == 16) {
            std::printf("BTDMP: transmit buffer overrun\n");
        } else {
            transmit_queue.push(value);
            transmit_empty = false;
            transmit_full = transmit_queue.size() == 16;
        }
    }

    void SetTransmitFlush(u16 value) {
        transmit_queue = {};
        transmit_empty = true;
        transmit_full = false;
    }

    u16 GetTransmitFlush() {
        return 0;
    }

    void Tick();

    void SetAudioCallback(std::function<void(std::array<std::int16_t, 2>)> callback) {
        audio_callback = callback;
    }

    std::function<void()> handler;

private:
    u16 transmit_period = 0;
    u16 transmit_timer = 0;
    u16 transmit_enable = 0;
    bool transmit_empty = true;
    bool transmit_full = false;
    std::queue<u16> transmit_queue;
    std::function<void(std::array<std::int16_t, 2>)> audio_callback;

    class BtdmpTimingCallbacks;
};

} // namespace Teakra
