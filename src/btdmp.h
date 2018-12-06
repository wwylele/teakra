#pragma once
#include <array>
#include <cstdio>
#include <functional>
#include <queue>
#include "common_types.h"

namespace Teakra {

class Btdmp {
public:
    Btdmp(const char* debug_string);
    ~Btdmp();

    void Reset();

    void SetTransmitPeriod(u16 value) {
        // std::printf("BTDMP%s: SetTransmitPeriod %04X\n", debug_string, value);
        transmit_period = value;
    }

    u16 GetTransmitPeriod() {
        // std::printf("BTDMP%s: GetTransmitPeriod\n", debug_string);
        return transmit_period;
    }

    void SetTransmitEnable(u16 value) {
        // std::printf("BTDMP%s: SetTransmitEnable %04X\n", debug_string, value);
        transmit_enable = value;
    }

    u16 GetTransmitEnable() {
        // std::printf("BTDMP%s: GetTransmitEnable\n", debug_string);
        return transmit_enable;
    }

    u16 GetTransmitEmpty() {
        // std::printf("BTDMP%s: GetTransmitEmpty\n", debug_string);
        return transmit_empty;
    }

    u16 GetTransmitFull() {
        // std::printf("BTDMP%s: GetTransmitFull\n", debug_string);
        return transmit_full;
    }

    void Send(u16 value) {
        // std::printf("BTDMP%s: Send %04X\n", debug_string, value);
        if (transmit_queue.size() == 16) {
            std::printf("BTDMP%s: transmit buufer overrun", debug_string);
        } else {
            transmit_queue.push(value);
            transmit_empty = false;
            transmit_full = transmit_queue.size() == 16;
        }
    }

    void SetTransmitFlush(u16 value) {
        // std::printf("BTDMP%s: SetTransmitFlush %04X\n", debug_string, value);
        transmit_queue = {};
        transmit_empty = true;
        transmit_full = false;
    }

    u16 GetTransmitFlush() {
        // std::printf("BTDMP%s: GetTransmitFlush\n", debug_string);
        return 0;
    }

    void Tick();

    void SetAudioCallback(std::function<void(std::array<std::int16_t, 2>)> callback) {
        audio_callback = callback;
    }

    std::function<void()> handler;

private:
    const char* debug_string;
    u16 transmit_period = 0;
    u16 transmit_timer = 0;
    u16 transmit_enable = 0;
    bool transmit_empty = true;
    bool transmit_full = false;
    std::queue<u16> transmit_queue;
    std::function<void(std::array<std::int16_t, 2>)> audio_callback;
};

} // namespace Teakra
