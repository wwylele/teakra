#pragma once
#include <array>
#include <cstdio>
#include <functional>
#include "common_types.h"
#include "teakra/teakra.h"

namespace Teakra {

struct SharedMemory;
class Ahbm;

class Dma {
public:
    Dma(SharedMemory& shared_memory, Ahbm& ahbm) : shared_memory(shared_memory), ahbm(ahbm) {}

    void Reset();

    void EnableChannel(u16 value) {
        // std::printf("DMA: enable channel %04X\n", value);
        enable_channel = value;
    }
    u16 GetChannelEnabled() {
        // std::printf("DMA: GetChannelEnabled\n");
        return enable_channel;
    }

    void ActivateChannel(u16 value) {
        // std::printf("DMA: ActivateChannel %04X\n", value);
        active_channel = value;
    }
    u16 GetActiveChannel() {
        // std::printf("DMA: GetActiveChannel\n");
        return active_channel;
    }

    void SetAddrSrcLow(u16 value) {
        // std::printf("DMA: SetAddrSrcLow %04X\n", value);
        channels[active_channel].addr_src_low = value;
    }
    u16 GetAddrSrcLow() {
        // std::printf("DMA: GetAddrSrcLow\n");
        return channels[active_channel].addr_src_low;
    }

    void SetAddrSrcHigh(u16 value) {
        // std::printf("DMA: SetAddrSrcHigh %04X\n", value);
        channels[active_channel].addr_src_high = value;
    }
    u16 GetAddrSrcHigh() {
        // std::printf("DMA: GetAddrSrcHigh\n");
        return channels[active_channel].addr_src_high;
    }

    void SetAddrDstLow(u16 value) {
        // std::printf("DMA: SetAddrDstLow %04X\n", value);
        channels[active_channel].addr_dst_low = value;
    }
    u16 GetAddrDstLow() {
        // std::printf("DMA: GetAddrDstLow\n");
        return channels[active_channel].addr_dst_low;
    }

    void SetAddrDstHigh(u16 value) {
        // std::printf("DMA: SetAddrDstHigh %04X\n", value);
        channels[active_channel].addr_dst_high = value;
    }
    u16 GetAddrDstHigh() {
        // std::printf("DMA: GetAddrDstHigh\n");
        return channels[active_channel].addr_dst_high;
    }

    void SetSize0(u16 value) {
        // std::printf("DMA: SetSize0 %04X\n", value);
        channels[active_channel].size0 = value;
    }
    u16 GetSize0() {
        // std::printf("DMA: GetSize0\n");
        return channels[active_channel].size0;
    }

    void SetSize1(u16 value) {
        // std::printf("DMA: SetSize1 %04X\n", value);
        channels[active_channel].size1 = value;
    }
    u16 GetSize1() {
        // std::printf("DMA: GetSize1\n");
        return channels[active_channel].size1;
    }

    void SetSize2(u16 value) {
        // std::printf("DMA: SetSize2 %04X\n", value);
        channels[active_channel].size2 = value;
    }
    u16 GetSize2() {
        // std::printf("DMA: GetSize2\n");
        return channels[active_channel].size2;
    }

    void SetSrcStep0(u16 value) {
        // std::printf("DMA: SetSrcStep0 %04X\n", value);
        channels[active_channel].src_step0 = value;
    }
    u16 GetSrcStep0() {
        // std::printf("DMA: GetSrcStep0\n");
        return channels[active_channel].src_step0;
    }

    void SetDstStep0(u16 value) {
        // std::printf("DMA: SetDstStep0 %04X\n", value);
        channels[active_channel].dst_step0 = value;
    }
    u16 GetDstStep0() {
        // std::printf("DMA: GetDstStep0\n");
        return channels[active_channel].dst_step0;
    }

    void SetSrcStep1(u16 value) {
        // std::printf("DMA: SetSrcStep1 %04X\n", value);
        channels[active_channel].src_step1 = value;
    }
    u16 GetSrcStep1() {
        // std::printf("DMA: GetSrcStep1\n");
        return channels[active_channel].src_step1;
    }

    void SetDstStep1(u16 value) {
        // std::printf("DMA: SetDstStep1 %04X\n", value);
        channels[active_channel].dst_step1 = value;
    }
    u16 GetDstStep1() {
        // std::printf("DMA: GetDstStep1\n");
        return channels[active_channel].dst_step1;
    }

    void SetSrcStep2(u16 value) {
        // std::printf("DMA: SetSrcStep2 %04X\n", value);
        channels[active_channel].src_step2 = value;
    }
    u16 GetSrcStep2() {
        // std::printf("DMA: GetSrcStep2\n");
        return channels[active_channel].src_step2;
    }

    void SetDstStep2(u16 value) {
        // std::printf("DMA: SetDstStep2 %04X\n", value);
        channels[active_channel].dst_step2 = value;
    }
    u16 GetDstStep2() {
        // std::printf("DMA: GetDstStep2\n");
        return channels[active_channel].dst_step2;
    }

    void SetSrcSpace(u16 value) {
        // std::printf("DMA: SetSrcSpace %04X\n", value);
        channels[active_channel].src_space = value;
    }
    u16 GetSrcSpace() {
        // std::printf("DMA: GetSrcSpace\n");
        return channels[active_channel].src_space;
    }

    void SetDstSpace(u16 value) {
        // std::printf("DMA: SetDstSpace %04X\n", value);
        channels[active_channel].dst_space = value;
    }
    u16 GetDstSpace() {
        // std::printf("DMA: GetDstSpace\n");
        return channels[active_channel].dst_space;
    }

    void SetDwordMode(u16 value) {
        // std::printf("DMA: SetDwordMode %04X\n", value);
        channels[active_channel].dword_mode = value;
    }
    u16 GetDwordMode() {
        // std::printf("DMA: GetDwordMode\n");
        return channels[active_channel].dword_mode;
    }

    void SetY(u16 value) {
        // std::printf("DMA: SetY %04X\n", value);
        channels[active_channel].y = value;
    }
    u16 GetY() {
        // std::printf("DMA: GetY\n");
        return channels[active_channel].y;
    }

    void SetZ(u16 value) {
        // std::printf("DMA: SetZ %04X\n", value);
        channels[active_channel].z = value;

        if (value == 0x40C0) {
            DoDma(active_channel);
        }
    }
    u16 GetZ() {
        // std::printf("DMA: GetZ\n");
        return channels[active_channel].z;
    }

    void DoDma(u16 channel);

    std::function<void()> handler;

private:
    u16 enable_channel = 0;
    u16 active_channel = 0;

    struct Channel {
        u16 addr_src_low = 0, addr_src_high = 0;
        u16 addr_dst_low = 0, addr_dst_high = 0;
        u16 size0 = 0, size1 = 0, size2 = 0;
        u16 src_step0 = 0, dst_step0 = 0;
        u16 src_step1 = 0, dst_step1 = 0;
        u16 src_step2 = 0, dst_step2 = 0;
        u16 src_space = 0, dst_space = 0;
        u16 dword_mode;
        u16 y = 0;
        u16 z = 0;

        u32 current_src = 0, current_dst = 0;
        u16 counter0 = 0, counter1 = 0, counter2 = 0;
        u16 running = 0;
        u16 ahbm_channel = 0;

        void Start();
        void Tick(Dma& parent);
    };

    std::array<Channel, 8> channels;

    SharedMemory& shared_memory;
    Ahbm& ahbm;
};

} // namespace Teakra
