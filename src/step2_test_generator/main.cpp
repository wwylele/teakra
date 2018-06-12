#include "../test.h"
#include <cstdio>

int main(int argc, char** argv) {
    if (argc < 2)
        return -1;

    TestCase test_case{};
    u16 r0base = 0x4839;
    test_case.opcode = 0xD294; // modr r0+arps0
    test_case.expand = 0;
    test_case.before.mod2 = 1; // enable mod for r0;
    std::FILE* f = std::fopen(argv[1], "wb");
    for (u16 legacy = 0; legacy < 2; ++legacy) {
        test_case.before.mod1 = legacy << 13;
        for (u16 step = 4; step < 8; ++step) {
            test_case.before.arp[0] = step;
            for (u16 mod = 0; mod < 0x200; ++mod) {
                test_case.before.cfgi = mod << 7;
                for (u16 r = 0; r < 0x20; ++r) {
                    test_case.before.r[0] = r + r0base;
                    std::fwrite(&test_case, sizeof(test_case), 1, f);
                }
            }
        }

    }
    std::fclose(f);
}
