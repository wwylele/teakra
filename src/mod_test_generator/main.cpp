#include "../test.h"
#include <cstdio>

int main(int argc, char** argv) {
    if (argc < 2)
        return -1;

    TestCase test_case{};
    test_case.before.r[0] = 0x4839;
    test_case.opcode = 0x0098; // modr r0+s
    test_case.expand = 0;
    test_case.before.mod2 = 1; // enable mod for r0;
    std::FILE* f = std::fopen(argv[1], "wb");
    for (u16 legacy = 0; legacy < 2; ++legacy) {
        test_case.before.mod1 = legacy << 13;
        for (u32 i = 0; i < 0x10000; ++i) {
            test_case.before.cfgi = (u16)i;
            std::fwrite(&test_case, sizeof(test_case), 1, f);
        }
    }
    std::fclose(f);
}
