#pragma once
#include <array>
#include <cstdio>
#include <functional>
#include <queue>
#include "common_types.h"

namespace Teakra {

class Ahbm {
public:
    enum class UnitSize : u16 {
        U8 = 0,
        U16 = 1,
        U32 = 2,
    };

    enum class BurstSize : u16 {
        X1 = 0,
        X4 = 1,
        X8 = 2,
    };

    enum class Direction : u16 {
        Read = 0,
        Write = 1,
    };

    void Reset();

    u16 GetBusyFlag() {
        // std::printf("AHBM: GetBusyFlag\n");
        return busy_flag;
    }
    void SetUnitSize(u16 i, u16 value) {
        // std::printf("AHBM: SetUnitSize[%u] : %04X\n", i, value);
        channels[i].unit_size = static_cast<UnitSize>(value);
    }
    u16 GetUnitSize(u16 i) {
        // std::printf("AHBM: GetUnitSize[%u]\n", i);
        return static_cast<u16>(channels[i].unit_size);
    }
    void SetBurstSize(u16 i, u16 value) {
        // std::printf("AHBM: SetBurstSize[%u] : %04X\n", i, value);
        channels[i].burst_size = static_cast<BurstSize>(value);
    }
    u16 GetBurstSize(u16 i) {
        // std::printf("AHBM: GetBurstSize[%u]\n", i);
        return static_cast<u16>(channels[i].burst_size);
    }
    void SetDirection(u16 i, u16 value) {
        // std::printf("AHBM: SetDirection[%u] : %04X\n", i, value);
        channels[i].direction = static_cast<Direction>(value);
    }
    u16 GetDirection(u16 i) {
        // std::printf("AHBM: GetDirection[%u]\n", i);
        return static_cast<u16>(channels[i].direction);
    }
    void SetDmaChannel(u16 i, u16 value) {
        // std::printf("AHBM: SetDmaChannelB[%u] : %04X\n", i, value);
        channels[i].dma_channel = value;
    }
    u16 GetDmaChannel(u16 i) {
        // std::printf("AHBM: GetDmaChannel[%u]\n", i);
        return channels[i].dma_channel;
    }

    std::function<u8(u32)> read_external;
    std::function<void(u32, u8)> write_external;

    u16 Read16(u16 channel, u32 address);
    u32 Read32(u16 channel, u32 address);
    void Write16(u16 channel, u32 address, u16 value);
    void Write32(u16 channel, u32 address, u32 value);

    u16 GetChannelForDma(u16 dma_channel);

private:
    u16 busy_flag = 0;
    struct Channel {
        UnitSize unit_size = UnitSize::U8;
        BurstSize burst_size = BurstSize::X1;
        Direction direction = Direction::Read;
        u16 dma_channel = 0;

        std::queue<u32> burst_queue;
        u32 write_burst_start = 0;
        unsigned GetBurstSize();
    };
    std::array<Channel, 3> channels;

    void WriteInternal(u16 channel, u32 address, u32 value);
};

} // namespace Teakra
