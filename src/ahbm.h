#pragma once
#include <array>
#include <cstdio>
#include "common_types.h"

namespace Teakra {

class Ahbm {
public:
    u16 GetBusyFlag() {
        std::printf("AHBM: GetBusyFlag\n");
        return busy_flag;
    }
    void SetA(u16 i, u16 value) {
        std::printf("AHBM: SetA[%u] : %04X\n", i, value);
        channels[i].a = value;
    }
    u16 GetA(u16 i) {
        std::printf("AHBM: GetA[%u]\n", i);
        return channels[i].a;
    }
    void SetB(u16 i, u16 value) {
        std::printf("AHBM: SetB[%u] : %04X\n", i, value);
        channels[i].b = value;
    }
    u16 GetB(u16 i) {
        std::printf("AHBM: GetB[%u]\n", i);
        return channels[i].b;
    }
    void SetDmaChannel(u16 i, u16 value) {
        std::printf("AHBM: SetDmaChannelB[%u] : %04X\n", i, value);
        channels[i].dma_channel = value;
    }
    u16 GetDmaChannel(u16 i) {
        std::printf("AHBM: GetDmaChannel[%u]\n", i);
        return channels[i].dma_channel;
    }
private:
    u16 busy_flag = 0;
    struct Channel {
        u16 a = 0;
        u16 b = 0;
        u16 dma_channel = 0;
    };
    std::array<Channel, 7> channels;
};

} // namespace Teakra
