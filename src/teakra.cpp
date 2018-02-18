#include "teakra/teakra.h"
#include "apbp.h"
#include "shared_memory.h"
#include "memory_interface.h"
#include "mmio.h"
#include "icu.h"
#include "core.h"
#include <thread>
#include <atomic>

namespace Teakra {

struct Teakra::Impl {
    SharedMemory shared_memory;
    DataMemoryController data_memory_controller;
    ICU icu;
    Apbp apbp_from_cpu{"cpu->dsp"}, apbp_from_dsp{"dsp->cpu"};
    MMIORegion mmio{data_memory_controller, icu, apbp_from_cpu, apbp_from_dsp};
    MemoryInterface memory_interface{shared_memory, data_memory_controller, mmio};
    Core core{memory_interface};

    Impl() {
        using namespace std::placeholders;
        icu.OnInterrupt = std::bind(&Core::SignalInterrupt, &core, _1);
        icu.OnVectoredInterrupt = std::bind(&Core::SignalVectoredInterrupt, &core, _1);

        apbp_from_cpu.SetDataHandler(0, [this](){
            //mmio.apbp_flags |= 1 << 8;
            icu.TriggerSingle(0xE);
        });

        apbp_from_cpu.SetDataHandler(1, [this](){
            //mmio.apbp_flags |= 1 << 12;
            icu.TriggerSingle(0xE);
        });

        apbp_from_cpu.SetDataHandler(2, [this](){
            //mmio.apbp_flags |= 1 << 13;
            icu.TriggerSingle(0xE);
        });

        for (unsigned channel = 0; channel < 16; ++channel) {
            apbp_from_cpu.SetSemaphoreHandler(channel, [this](){
                mmio.apbp_flags |= 1 << 9;
                icu.TriggerSingle(0xE);
            });
        }

    }

    std::unique_ptr<std::thread> core_thread = nullptr;
    std::atomic_flag running_flag = ATOMIC_FLAG_INIT;
    void CoreThread() {
        core.Reset();
        while(running_flag.test_and_set()) {
            core.Run(1);
        }
        running_flag.clear();
    }
};

Teakra::Teakra() : impl(new Impl) {}
Teakra::~Teakra() {
    if (impl->core_thread)
        Stop();
}

std::array<std::uint8_t, 0x80000>& Teakra::GetDspMemory() {
    return impl->shared_memory.raw;
}

void Teakra::Start() {
    if (impl->core_thread)
        Stop();
    impl->core.Reset();
    impl->running_flag.test_and_set();
    impl->core_thread = std::make_unique<std::thread>(&Impl::CoreThread, impl.get());
}

void Teakra::Stop() {
    if (!impl->core_thread)
        return;
    impl->running_flag.clear();
    impl->core_thread->join();
    impl->core_thread = nullptr;
}

bool Teakra::SendDataIsEmpty(std::uint8_t index) const {
    return !impl->apbp_from_cpu.IsDataReady(index);
}
void Teakra::SendData(std::uint8_t index, std::uint16_t value) {
    impl->apbp_from_cpu.SendData(index, value);
}
bool Teakra::RecvDataIsReady(std::uint8_t index) const {
    return impl->apbp_from_dsp.IsDataReady(index);
}
std::uint16_t Teakra::RecvData(std::uint8_t index) {
    return impl->apbp_from_dsp.RecvData(index);
}
void Teakra::SetRecvDataHandler(std::uint8_t index, std::function<void()> handler) {
    impl->apbp_from_dsp.SetDataHandler(index, handler);
}

void Teakra::SetSemaphore(std::uint16_t value) {
    impl->apbp_from_cpu.SetSemaphore(value);
}
void Teakra::SetSemaphoreHandler(std::uint8_t index, std::function<void()> handler) {
    impl->apbp_from_dsp.SetSemaphoreHandler(index, handler);
}

} // namespace Teakra
