#include "dsp1.h"
#include <cstdio>
#include <cstring>
#include <utility>

Dsp1::Dsp1(const std::vector<u8>& raw) {
    Header header;
    std::memcpy(&header, raw.data(), sizeof(header));

    printf("Memory layout = %04X\n", header.memory_layout);
    printf("Unk = %02X\n", header.unknown);
    printf("Filter segment type = %d\n", header.filter_segment_type);
    printf("Num segments = %d\n", header.num_segments);
    printf("Flags = %d\n", header.flags);
    printf("Filter address = 2 * %08X\n", header.filter_segment_address);
    printf("Filter size = %08X\n", header.filter_segment_size);

    for (u32 i = 0; i < header.num_segments; ++i) {
        Segment segment;
        segment.data = std::vector<u8>(raw.begin() + header.segments[i].offset,
            raw.begin() + header.segments[i].offset + header.segments[i].size);
        segment.memory_type = header.segments[i].memory_type;
        segment.target = header.segments[i].address;/*header.segments[i].address * 2 +
            (segment.memory_type == 2 ? 0x1FF40000 : 0x1FF00000);*/

        printf("[Segment %u]\n", i);
        printf("memory_type = %d\n", segment.memory_type);
        printf("target = %08X\n", segment.target);
        printf("size = %08X\n", static_cast<u32>(segment.data.size()));

        segments.push_back(std::move(segment));
    }
}
