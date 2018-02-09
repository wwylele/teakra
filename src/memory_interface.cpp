#include "memory_interface.h"
#include "shared_memory.h"
#include "mmio.h"

namespace Teakra {
MemoryInterface::MemoryInterface(SharedMemory& shared_memory,
    DataMemoryController& data_memory_controller, MMIORegion& mmio) : shared_memory(shared_memory),
    data_memory_controller(data_memory_controller), mmio(mmio) {

}
u16 MemoryInterface::ProgramRead(u32 address) const {
    return shared_memory.ReadWord(address);
}
void MemoryInterface::ProgramWrite(u32 address, u16 value) {
    shared_memory.WriteWord(address, value);
}
u16 MemoryInterface::DataRead(u16 address) {
    if (data_memory_controller.InMMIO(address)) {
        return mmio.Read(data_memory_controller.ToMMIO(address));
    }
    u32 converted = data_memory_controller.ConvertAddressByBank(address);
    u16 value = shared_memory.ReadWord(converted);
    //printf("READ @ %04X -> %04X\n", address, value);
    return value;
}
void MemoryInterface::DataWrite(u16 address, u16 value) {
    if (data_memory_controller.InMMIO(address)) {
        return mmio.Write(data_memory_controller.ToMMIO(address), value);
    }
    u32 converted = data_memory_controller.ConvertAddressByBank(address);
    //printf("READ @ %04X <- %04X\n", address, value);
    shared_memory.WriteWord(converted, value);
}

} // namespace Teakra
