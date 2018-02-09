#include "core.h"
#include "interpreter.h"
#include "register.h"

namespace Teakra {

struct Core::Impl {
    Impl(MemoryInterface& memory_interface) : interpreter(regs, memory_interface) {}
    RegisterState regs;
    Interpreter interpreter;
};

Core::Core(MemoryInterface& memory_interface) : impl(new Impl(memory_interface)) {}
Core::~Core() = default;

void Core::Reset() {
    impl->regs = RegisterState();
}

void Core::Run(unsigned cycles) {
    impl->interpreter.Run(cycles);
}

void Core::SignalInterrupt(u32 i) {
    impl->interpreter.SignalInterrupt(i);
}
void Core::SignalVectoredInterrupt(u32 address) {
    impl->interpreter.SignalVectoredInterrupt(address);
}

} // namespace Teakra
