#include "interpreter.h"
#include "processor.h"
#include "register.h"

namespace Teakra {

struct Processor::Impl {
    Impl(MemoryInterface& memory_interface) : interpreter(regs, memory_interface) {}
    RegisterState regs;
    Interpreter interpreter;
};

Processor::Processor(MemoryInterface& memory_interface) : impl(new Impl(memory_interface)) {}
Processor::~Processor() = default;

void Processor::Reset() {
    impl->regs = RegisterState();
}

void Processor::Run(unsigned cycles) {
    impl->interpreter.Run(cycles);
}

void Processor::SignalInterrupt(u32 i) {
    impl->interpreter.SignalInterrupt(i);
}
void Processor::SignalVectoredInterrupt(u32 address) {
    impl->interpreter.SignalVectoredInterrupt(address);
}

} // namespace Teakra
