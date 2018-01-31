#pragma once
#include <vector>
#include "common_types.h"

class Dsp1 {
public:
    explicit Dsp1(const std::vector<u8>& raw);

    struct Header {
        u8 signature[0x100];
        u8 magic[4];
        u32 binary_size;
        u16 memory_layout;
        u16 padding;
        u8 unknown;
        u8 filter_segment_type;
        u8 num_segments;
        u8 flags;
        u32 filter_segment_address;
        u32 filter_segment_size;
        u64 zero;
        struct Segment {
            u32 offset;
            u32 address;
            u32 size;
            u8 pa, pb, pc;
            u8 memory_type;
            u8 sha256[0x20];
        } segments[10];
    };

    static_assert(sizeof(Header) == 0x300, "!");

    struct Segment {
        std::vector<u8> data;
        u8 memory_type;
        u32 target;
    };

    std::vector<Segment> segments;
};
