#include "memory_interface.h"
#include "mmio.h"
#include "shared_memory.h"

namespace Teakra {
MemoryInterface::MemoryInterface(SharedMemory& shared_memory,
                                 MemoryInterfaceUnit& memory_interface_unit)
    : shared_memory(shared_memory), memory_interface_unit(memory_interface_unit) {}

void MemoryInterface::SetMMIO(MMIORegion& mmio) {
    this->mmio = &mmio;
}

u16 MemoryInterface::ProgramRead(u32 address) const {
    return shared_memory.ReadWord(address);
}
void MemoryInterface::ProgramWrite(u32 address, u16 value) {
    shared_memory.WriteWord(address, value);
}
u16 MemoryInterface::DataRead(u16 address) {
    if (memory_interface_unit.InMMIO(address)) {
        ASSERT(mmio != nullptr);
        return mmio->Read(memory_interface_unit.ToMMIO(address));
    }
    u32 converted = memory_interface_unit.ConvertDataAddress(address);
    u16 value = shared_memory.ReadWord(converted);
    return value;
}
void MemoryInterface::DataWrite(u16 address, u16 value) {
    if (memory_interface_unit.InMMIO(address)) {
        ASSERT(mmio != nullptr);
        return mmio->Write(memory_interface_unit.ToMMIO(address), value);
    }
    u32 converted = memory_interface_unit.ConvertDataAddress(address);
    shared_memory.WriteWord(converted, value);
}

} // namespace Teakra
