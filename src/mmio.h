#pragma once
#include "common_types.h"
#include <memory>
#include <array>
#include "icu.h"

namespace Teakra {

class DataMemoryController;

class MMIORegion {
public:
    MMIORegion(DataMemoryController& miu, ICU& icu);
    ~MMIORegion();
    u16 Read(u16 addr); // not const because it can be a FIFO register
    void Write(u16 addr, u16 value);
private:
    class Impl;
    std::unique_ptr<Impl> impl;

    DataMemoryController& miu;
    ICU& icu;
};

} // namespace Teakra
