#include "teakra/teakra.h"
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
    MMIORegion mmio{data_memory_controller, icu};
    MemoryInterface memory_interface{shared_memory, data_memory_controller, mmio};
    Core core{memory_interface};

    Impl() {
        using namespace std::placeholders;
        icu.OnInterrupt = std::bind(&Core::SignalInterrupt, &core, _1);
        icu.OnVectoredInterrupt = std::bind(&Core::SignalVectoredInterrupt, &core, _1);

    }

    std::unique_ptr<std::thread> core_thread = nullptr;
    std::atomic_flag running_flag = ATOMIC_FLAG_INIT;
    void CoreThread() {
        core.Reset();
        while(running_flag.test_and_set()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
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

} // namespace Teakra
