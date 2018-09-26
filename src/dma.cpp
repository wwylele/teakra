#include <cstring>
#include "dma.h"
#include "shared_memory.h"

namespace Teakra {

void Dma::DoDma() {
    u32 src =
        channels[active_channel].addr_src_low | ((u32)channels[active_channel].addr_src_high << 16);
    u32 dst =
        channels[active_channel].addr_dst_low | ((u32)channels[active_channel].addr_dst_high << 16);
    auto data = read_callback(src, channels[active_channel].length * 2 *
                                       (channels[active_channel].f[0] + 1));
    std::memcpy(shared_memory.raw.data() + dst * 2, data.data(), data.size());
    handler();
}

} // namespace Teakra
