#pragma once
#include <array>
#include <cstdio>
#include <functional>
#include "common_types.h"

namespace Teakra {

struct SharedMemory;

class Dma {
public:
    Dma(SharedMemory& shared_memory) : shared_memory(shared_memory) {}
    void EnableChannel(u16 value) {
        std::printf("DMA: enable channel %04X\n", value);
        enable_channel = value;
    }
    u16 GetChannelEnabled() {
        std::printf("DMA: GetChannelEnabled\n");
        return enable_channel;
    }

    void ActivateChannel(u16 value) {
        std::printf("DMA: ActivateChannel %04X\n", value);
        active_channel = value;
    }
    u16 GetActiveChannel() {
        std::printf("DMA: GetActiveChannel\n");
        return active_channel;
    }

    void SetAddrSrcLow(u16 value) {
        std::printf("DMA: SetAddrSrcLow %04X\n", value);
        channels[active_channel].addr_src_low = value;
    }
    u16 GetAddrSrcLow() {
        std::printf("DMA: GetAddrSrcLow\n");
        return channels[active_channel].addr_src_low;
    }

    void SetAddrSrcHigh(u16 value) {
        std::printf("DMA: SetAddrSrcHigh %04X\n", value);
        channels[active_channel].addr_src_high = value;
    }
    u16 GetAddrSrcHigh() {
        std::printf("DMA: GetAddrSrcHigh\n");
        return channels[active_channel].addr_src_high;
    }

    void SetAddrDstLow(u16 value) {
        std::printf("DMA: SetAddrDstLow %04X\n", value);
        channels[active_channel].addr_dst_low = value;
    }
    u16 GetAddrDstLow() {
        std::printf("DMA: GetAddrDstLow\n");
        return channels[active_channel].addr_dst_low;
    }

    void SetAddrDstHigh(u16 value) {
        std::printf("DMA: SetAddrDstHigh %04X\n", value);
        channels[active_channel].addr_dst_high = value;
    }
    u16 GetAddrDstHigh() {
        std::printf("DMA: GetAddrDstHigh\n");
        return channels[active_channel].addr_dst_high;
    }

    void SetLength(u16 value) {
        std::printf("DMA: SetLength %04X\n", value);
        channels[active_channel].length = value;
    }
    u16 GetLength() {
        std::printf("DMA: GetLength\n");
        return channels[active_channel].length;
    }

    void SetF(int i, u16 value) {
        std::printf("DMA: SetF[%d] %04X\n", i, value);
        channels[active_channel].f[i] = value;
    }
    u16 GetF(int i) {
        std::printf("DMA: GetF[%d]\n", i);
        return channels[active_channel].f[i];
    }

    void SetSrcA(u16 value) {
        std::printf("DMA: SetSrcA %04X\n", value);
        channels[active_channel].src_a = value;
    }
    u16 GetSrcA() {
        std::printf("DMA: GetSrcA\n");
        return channels[active_channel].src_a;
    }

    void SetDstA(u16 value) {
        std::printf("DMA: SetDstA %04X\n", value);
        channels[active_channel].dst_a = value;
    }
    u16 GetDstA() {
        std::printf("DMA: GetDstA\n");
        return channels[active_channel].dst_a;
    }

    void SetSrcB(u16 value) {
        std::printf("DMA: SetSrcB %04X\n", value);
        channels[active_channel].src_b = value;
    }
    u16 GetSrcB() {
        std::printf("DMA: GetSrcB\n");
        return channels[active_channel].src_b;
    }

    void SetDstB(u16 value) {
        std::printf("DMA: SetDstB %04X\n", value);
        channels[active_channel].dst_b = value;
    }
    u16 GetDstB() {
        std::printf("DMA: GetDstB\n");
        return channels[active_channel].dst_b;
    }

    void SetSrcC(u16 value) {
        std::printf("DMA: SetSrcC %04X\n", value);
        channels[active_channel].src_c = value;
    }
    u16 GetSrcC() {
        std::printf("DMA: GetSrcC\n");
        return channels[active_channel].src_c;
    }

    void SetDstC(u16 value) {
        std::printf("DMA: SetDstC %04X\n", value);
        channels[active_channel].dst_c = value;
    }
    u16 GetDstC() {
        std::printf("DMA: GetDstC\n");
        return channels[active_channel].dst_c;
    }

    void SetX(u16 value) {
        std::printf("DMA: SetX %04X\n", value);
        channels[active_channel].x = value;
    }
    u16 GetX() {
        std::printf("DMA: GetX\n");
        return channels[active_channel].x;
    }

    void SetY(u16 value) {
        std::printf("DMA: SetY %04X\n", value);
        channels[active_channel].y = value;
    }
    u16 GetY() {
        std::printf("DMA: GetY\n");
        return channels[active_channel].y;
    }

    void SetZ(u16 value) {
        std::printf("DMA: SetZ %04X\n", value);
        channels[active_channel].z = value;

        if (value == 0x40C0) {
            DoDma();
        }
    }
    u16 GetZ() {
        std::printf("DMA: GetZ\n");
        return channels[active_channel].z;
    }

    void SetReadCallback(std::function<std::vector<u8>(u32 address, u32 size)> callback) {
        read_callback = std::move(callback);
    }

    void DoDma();

    std::function<void()> handler;

private:
    u16 enable_channel = 0;
    u16 active_channel = 0;

    struct Channel {
        u16 addr_src_low = 0, addr_src_high = 0;
        u16 addr_dst_low = 0, addr_dst_high = 0;
        u16 length = 0;
        u16 f[2]{};
        u16 src_a = 0, dst_a = 0;
        u16 src_b = 0, dst_b = 0;
        u16 src_c = 0, dst_c = 0;
        u16 x = 0, y = 0, z = 0;
    };

    std::array<Channel, 8> channels;

    std::function<std::vector<u8>(u32 address, u32 size)> read_callback;
    SharedMemory& shared_memory;
};

} // namespace Teakra
