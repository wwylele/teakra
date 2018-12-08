// This file is under the public domain.

#pragma once

#include <cstddef>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#include "common_types.h"

namespace Common {

#ifdef _MSC_VER
inline u8 MostSignificantSetBit(u8 val) {
    unsigned long index;
    _BitScanReverse(&index, val);
    return static_cast<u8>(index);
}
inline u8 MostSignificantSetBit(u16 val) {
    unsigned long index;
    _BitScanReverse(&index, val);
    return static_cast<u8>(index);
}
inline u8 MostSignificantSetBit(u32 val) {
    unsigned long index;
    _BitScanReverse(&index, val);
    return static_cast<u8>(index);
}
inline u8 MostSignificantSetBit(u64 val) {
    unsigned long index;
    _BitScanReverse64(&index, val);
    return static_cast<u8>(index);
}
#else
inline u8 MostSignificantSetBit(u8 val) {
    return static_cast<u8>(sizeof(unsigned int) * 8 - __builtin_clz(val) - 1);
}
inline u8 MostSignificantSetBit(u16 val) {
    return static_cast<u8>(sizeof(unsigned int) * 8 - __builtin_clz(val) - 1);
}
inline u8 MostSignificantSetBit(u32 val) {
    return static_cast<u8>(sizeof(unsigned int) * 8 - __builtin_clz(val) - 1);
}
inline u8 MostSignificantSetBit(u64 val) {
    return static_cast<u8>(sizeof(unsigned long long) * 8 - __builtin_clzll(val) - 1);
}
#endif
} // namespace Common
