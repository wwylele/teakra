#pragma once
#include "common_types.h"
#include <functional>
#include <memory>
#include <array>

namespace Teakra{
class Apbp {
public:
    Apbp(const char* debug_string);
    ~Apbp();

    void SendData(unsigned channel, u16 data);
    u16 RecvData(unsigned channel);
    bool IsDataReady(unsigned channel) const;
    void SetDataHandler(unsigned channel, std::function<void()> handler);

    void SetSemaphore(u16 bits);
    void ClearSemaphore(u16 bits);
    u16 GetSemaphore() const;
    void MaskSemaphore(u16 bits);
    u16 GetSemaphoreMask() const;
    void SetSemaphoreHandler(std::function<void()> handler);

    bool IsSemaphoreSignaled();
private:
    class Impl;
    std::unique_ptr<Impl> impl;
    const char* debug_string;


};
} // namespace Teakra
