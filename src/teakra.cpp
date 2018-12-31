#include <array>
#include <atomic>
#include "ahbm.h"
#include "apbp.h"
#include "btdmp.h"
#include "core_timing.h"
#include "dma.h"
#include "icu.h"
#include "memory_interface.h"
#include "mmio.h"
#include "processor.h"
#include "shared_memory.h"
#include "teakra/teakra.h"
#include "timer.h"

namespace Teakra {

struct Teakra::Impl {
    CoreTiming core_timing;
    SharedMemory shared_memory;
    MemoryInterfaceUnit miu;
    ICU icu;
    Apbp apbp_from_cpu, apbp_from_dsp;
    std::array<Timer, 2> timer{{{core_timing}, {core_timing}}};
    Ahbm ahbm;
    Dma dma{shared_memory, ahbm};
    std::array<Btdmp, 2> btdmp{{{core_timing}, {core_timing}}};
    MMIORegion mmio{miu, icu, apbp_from_cpu, apbp_from_dsp, timer, dma, ahbm, btdmp};
    MemoryInterface memory_interface{shared_memory, miu};
    Processor processor{core_timing, memory_interface};

    Impl() {
        memory_interface.SetMMIO(mmio);
        using namespace std::placeholders;
        icu.SetInterruptHandler(std::bind(&Processor::SignalInterrupt, &processor, _1),
                                std::bind(&Processor::SignalVectoredInterrupt, &processor, _1, _2));

        timer[0].SetInterruptHandler([this]() { icu.TriggerSingle(0xA); });
        timer[1].SetInterruptHandler([this]() { icu.TriggerSingle(0x9); });

        apbp_from_cpu.SetDataHandler(0, [this]() { icu.TriggerSingle(0xE); });
        apbp_from_cpu.SetDataHandler(1, [this]() { icu.TriggerSingle(0xE); });
        apbp_from_cpu.SetDataHandler(2, [this]() { icu.TriggerSingle(0xE); });
        apbp_from_cpu.SetSemaphoreHandler([this]() { icu.TriggerSingle(0xE); });

        btdmp[0].SetInterruptHandler([this]() { icu.TriggerSingle(0xB); });
        btdmp[1].SetInterruptHandler([this]() { icu.TriggerSingle(0xB); });

        dma.SetInterruptHandler([this]() { icu.TriggerSingle(0xF); });
    }

    void Reset() {
        shared_memory.raw.fill(0);
        miu.Reset();
        apbp_from_cpu.Reset();
        apbp_from_dsp.Reset();
        timer[0].Reset();
        timer[1].Reset();
        ahbm.Reset();
        dma.Reset();
        btdmp[0].Reset();
        btdmp[1].Reset();
        processor.Reset();
    }
};

Teakra::Teakra() : impl(new Impl) {}
Teakra::~Teakra() = default;

void Teakra::Reset() {
    impl->Reset();
}

std::array<std::uint8_t, 0x80000>& Teakra::GetDspMemory() {
    return impl->shared_memory.raw;
}

const std::array<std::uint8_t, 0x80000>& Teakra::GetDspMemory() const {
    return impl->shared_memory.raw;
}

void Teakra::Run(unsigned cycle) {
    impl->processor.Run(cycle);
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
    impl->apbp_from_dsp.SetDataHandler(index, std::move(handler));
}

void Teakra::SetSemaphore(std::uint16_t value) {
    impl->apbp_from_cpu.SetSemaphore(value);
}
void Teakra::SetSemaphoreHandler(std::function<void()> handler) {
    impl->apbp_from_dsp.SetSemaphoreHandler(std::move(handler));
}
std::uint16_t Teakra::GetSemaphore() const {
    return impl->apbp_from_dsp.GetSemaphore();
}
void Teakra::ClearSemaphore(std::uint16_t value) {
    impl->apbp_from_dsp.ClearSemaphore(value);
}
void Teakra::MaskSemaphore(std::uint16_t value) {
    impl->apbp_from_dsp.MaskSemaphore(value);
}
void Teakra::SetAHBMCallback(const AHBMCallback& callback) {
    impl->ahbm.SetExternalMemoryCallback(callback.read8, callback.write8);
}

void Teakra::SetAudioCallback(std::function<void(std::array<s16, 2>)> callback) {
    impl->btdmp[0].SetAudioCallback(std::move(callback));
}

} // namespace Teakra
