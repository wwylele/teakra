#pragma once

#include <cstdint>
#include <functional>
#include <array>
#include <memory>

namespace Teakra {
class Teakra {
public:
    Teakra();
    ~Teakra();

    std::array<std::uint8_t, 0x80000>& GetDspMemory();

    // APBP Data
    bool SendDataIsEmpty(std::uint8_t index) const;
    void SendData(std::uint8_t index, std::uint16_t value);
    bool RecvDataIsReady(std::uint8_t index) const;
    std::uint16_t RecvData(std::uint8_t index);
    void SetRecvDataHandler(std::uint8_t index, std::function<void()> handler);

    // APBP Semaphore
    void SetSemaphore(std::uint16_t value);
    void SetSemaphoreHandler(std::function<void()> handler);
    std::uint16_t GetSemaphore();

    // core
    void Run(unsigned cycle);
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
} // namespace Teakra
