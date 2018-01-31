#include "memory.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

DspMemorySharedWithCitra::DspMemorySharedWithCitra() {
    int fd = open("/home/wwylele/Desktop/dspmem", O_RDWR | O_CREAT, 00777);
    ftruncate(fd, 0x00080000);
    u16* ptr = (u16*)mmap(nullptr, 0x00080000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    program = ptr;
    data = ptr + 0x20000;
}
u16 DspMemorySharedWithCitra::PRead(u32 addr) const {
    return program[addr];
}
void DspMemorySharedWithCitra::PWrite(u32 addr, u16 value) {
    program[addr] = value;
}
u16 DspMemorySharedWithCitra::DRead(u16 addr) const {
    return data[addr];
}
void DspMemorySharedWithCitra::DWrite(u16 addr, u16 value) {
    data[addr] = value;
}
