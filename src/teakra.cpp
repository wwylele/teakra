#include <array>
#include <atomic>
#include "ahbm.h"
#include "apbp.h"
#include "btdmp.h"
#include "core.h"
#include "dma.h"
#include "icu.h"
#include "memory_interface.h"
#include "mmio.h"
#include "shared_memory.h"
#include "teakra/teakra.h"
#include "timer.h"

namespace Teakra {

struct Teakra::Impl {
    SharedMemory shared_memory;
    MemoryInterfaceUnit miu;
    ICU icu;
    Apbp apbp_from_cpu{"cpu->dsp"}, apbp_from_dsp{"dsp->cpu"};
    std::array<Timer, 2> timer;
    Dma dma{shared_memory};
    Ahbm ahbm;
    std::array<Btdmp, 2> btdmp{{{"0"}, {"1"}}};
    MMIORegion mmio{miu, icu, apbp_from_cpu, apbp_from_dsp, timer, dma, ahbm, btdmp};
    MemoryInterface memory_interface{shared_memory, miu, mmio};
    Core core{memory_interface};

    Impl() {
        using namespace std::placeholders;
        icu.OnInterrupt = std::bind(&Core::SignalInterrupt, &core, _1);
        icu.OnVectoredInterrupt = std::bind(&Core::SignalVectoredInterrupt, &core, _1);

        timer[0].handler = [this]() { icu.TriggerSingle(0xA); };

        timer[1].handler = [this]() { icu.TriggerSingle(0x9); };

        apbp_from_cpu.SetDataHandler(0, [this]() { icu.TriggerSingle(0xE); });

        apbp_from_cpu.SetDataHandler(1, [this]() { icu.TriggerSingle(0xE); });

        apbp_from_cpu.SetDataHandler(2, [this]() { icu.TriggerSingle(0xE); });

        apbp_from_cpu.SetSemaphoreHandler([this]() { icu.TriggerSingle(0xE); });

        btdmp[0].handler = [this]() { icu.TriggerSingle(0xB); };
        btdmp[1].handler = [this]() { icu.TriggerSingle(0xB); };

        dma.handler = [this]() { icu.TriggerSingle(0xF); };
    }
};

Teakra::Teakra() : impl(new Impl) {}
Teakra::~Teakra() = default;
std::array<std::uint8_t, 0x80000>& Teakra::GetDspMemory() {
    return impl->shared_memory.raw;
}

void Teakra::Run(unsigned cycle) {
    for (unsigned i = 0; i < cycle; ++i) {
        impl->core.Run(1);
        impl->timer[0].Tick();
        impl->timer[1].Tick();
        impl->btdmp[0].Tick();
        impl->btdmp[1].Tick();
    }
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
void Teakra::SetSemaphoreHandler(std::function<void()> handler) {
    impl->apbp_from_dsp.SetSemaphoreHandler(handler);
}
std::uint16_t Teakra::GetSemaphore() {
    return impl->apbp_from_dsp.GetSemaphore();
}

void Teakra::SetDmaReadCallback(
    std::function<std::vector<uint8_t>(std::uint32_t address, std::uint32_t size)> callback) {
    impl->dma.SetReadCallback(std::move(callback));
}

} // namespace Teakra
