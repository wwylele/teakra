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

    void SendSample(u16 value);

    void Tick() {
        if (transmit_enable) {
            ++transmit_timer;
            if (transmit_timer >= transmit_period) {
                transmit_timer = 0;
                for (int i = 0; i < 2; ++i) {
                    if (transmit_queue.empty()) {
                        std::printf("BTDMP%s: transmit buffer underrun", debug_string);
                        SendSample(0);
                    } else {
                        SendSample(transmit_queue.front());
                        transmit_queue.pop();
                        transmit_empty = transmit_queue.empty();
                        transmit_full = false;
                        if (transmit_empty) {
                            handler();
                        }
                    }
                }
            }
        }
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

    std::FILE* file;

    struct WAVFILEHEADER {
        u32 _RIFF;
        u32 dataSize;
        u32 _WAVE;
        u32 _fmt_;

        u32 _10H;
        u16 format;
        u16 channel;
        u32 sampleRate;
        u32 dataRate;

        u16 blockSize;
        u16 bitSize;
        u32 _data;
        u32 dataSize2;

    } wavfileheader{
        0x46464952, 36 /*sampleCount*2+36*/,
        0x45564157, 0x20746D66,

        0x00000010, 1,
        2,          32728,
        32728 * 4,

        4,          16,
        0x61746164, 0 /*sampleCount*2*/
    };
};

} // namespace Teakra
