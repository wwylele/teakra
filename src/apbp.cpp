#include "apbp.h"
#include <array>
#include <atomic>
#include <mutex>
#include <utility>

namespace Teakra{
class DataChannel {
public:
    void Send(u16 data) {
        {
            std::lock_guard lock(mutex);
            ready = true;
            this->data = data;
        }
        if (handler)
            handler();
    }
    u16 Recv() {
        std::lock_guard lock(mutex);
        ready = false;
        return data;
    }
    bool IsReady() const {
        std::lock_guard lock(mutex);
        return ready;
    }
    std::function<void()> handler;
private:
    bool ready = false;
    u16 data = 0;
    mutable std::mutex mutex;
};

class SemaphoreChannel {
public:
    bool Set() {
        signal = true;
        if (!mask) {
            if (handler) {
                handler();
            }
            return true;
        }
        return false;
    }
    void Clear() {
        signal = false;
    }
    bool Get() const {
        return signal;
    }
    std::function<void()> handler;
    std::atomic<bool> mask{false};
private:
    std::atomic<bool> signal{false};
};

class Apbp::Impl {
public:
    std::array<DataChannel, 3> data_channels;
    std::array<SemaphoreChannel, 16> semaphore_channels;
    std::atomic<bool> semaphore_master_signal{false};
};

Apbp::Apbp(const char* debug_string): impl(new Impl), debug_string(debug_string) {}
Apbp::~Apbp() = default;

void Apbp::SendData(unsigned channel, u16 data) {
    printf("SendData %s %u %04X\n", debug_string, channel, data);
    impl->data_channels[channel].Send(data);
}

u16 Apbp::RecvData(unsigned channel) {
    printf("RecvData %s %u\n", debug_string, channel);
    return impl->data_channels[channel].Recv();
}

bool Apbp::IsDataReady(unsigned channel) const {
    return impl->data_channels[channel].IsReady();
}

void Apbp::SetDataHandler(unsigned channel, std::function<void()> handler) {
    impl->data_channels[channel].handler = std::move(handler);
}

void Apbp::SetSemaphore(u16 bits) {
    for (unsigned channel = 0; channel < 16; ++channel) {
        if ((bits >> channel) & 1) {
            printf("SetSemaphore %s %u\n", debug_string, channel);
            bool signaled = impl->semaphore_channels[channel].Set();
            impl->semaphore_master_signal = impl->semaphore_master_signal & signaled;
        }
    }
}

void Apbp::ClearSemaphore(u16 bits) {
    for (unsigned channel = 0; channel < 16; ++channel) {
        if ((bits >> channel) & 1) {
            printf("ClearSemaphore %s %u\n", debug_string, channel);
            impl->semaphore_channels[channel].Clear();
        }
    }
    impl->semaphore_master_signal = false; // ?
}

u16 Apbp::GetSemaphore() const {
    u16 result = 0;
    for (unsigned channel = 0; channel < 16; ++channel) {
        result |= impl->semaphore_channels[channel].Get() << channel;
    }
    return result;
}

void Apbp::MaskSemaphore(u16 bits) {
    for (unsigned channel = 0; channel < 16; ++channel) {
        if ((bits >> channel) & 1) {
            impl->semaphore_channels[channel].mask = true;
        }
    }
}

u16 Apbp::GetSemaphoreMask() const {
    u16 result = 0;
    for (unsigned channel = 0; channel < 16; ++channel) {
        result |= impl->semaphore_channels[channel].mask << channel;
    }
    return result;
}

void Apbp::SetSemaphoreHandler(unsigned channel, std::function<void()> handler) {
    impl->semaphore_channels[channel].handler = std::move(handler);
}

bool Apbp::IsSemaphoreSignaled() {
    return impl->semaphore_master_signal;
}
} // namespace Teakra
