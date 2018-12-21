#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <memory>

namespace Teakra {

struct AHBMCallback {
    std::function<std::uint8_t(std::uint32_t address)> read8;
    std::function<void(std::uint32_t address, std::uint8_t value)> write8;
};

class Teakra {
public:
    Teakra();
    ~Teakra();

    void Reset();

    std::array<std::uint8_t, 0x80000>& GetDspMemory();
    const std::array<std::uint8_t, 0x80000>& GetDspMemory() const;

    // APBP Data
    bool SendDataIsEmpty(std::uint8_t index) const;
    void SendData(std::uint8_t index, std::uint16_t value);
    bool RecvDataIsReady(std::uint8_t index) const;
    std::uint16_t RecvData(std::uint8_t index);
    void SetRecvDataHandler(std::uint8_t index, std::function<void()> handler);

    // APBP Semaphore
    void SetSemaphore(std::uint16_t value);
    void ClearSemaphore(std::uint16_t value);
    void MaskSemaphore(std::uint16_t value);
    void SetSemaphoreHandler(std::function<void()> handler);
    std::uint16_t GetSemaphore() const;

    // core
    void Run(unsigned cycle);

    void SetAHBMCallback(const AHBMCallback& callback);

    void SetAudioCallback(std::function<void(std::array<std::int16_t, 2>)> callback);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
} // namespace Teakra
