#include <cstdio>
#include <string>
#include <vector>

#include "decoder.h"
#include "disassembler.h"
#include "dsp1.h"
#include "interpreter.h"
#include "oprand.h"
#include "register.h"

int main() {
    FILE* file = fopen("/media/wwylele/学习_娱乐/3DS/PokemonY.romfs/sound/dspaudio.cdc", "rb");
    std::vector<u8> raw(217976);
    fread(raw.data(), raw.size(), 1, file);
    fclose(file);
    Dsp1 dsp(raw);

    Disassembler dsm;

    for (u32 opcode = 0; opcode < 0x10000; ++opcode) {
        Decode<Disassembler>((u16)opcode);
    }

    file = fopen("/home/wwylele/teakra/teakra.out", "wt");

    for (const auto& segment : dsp.segments) {
        if (segment.memory_type == 0 || segment.memory_type == 1) {
            fprintf(file, "\n>>>>>>>> Segment <<<<<<<<\n\n");
            for (unsigned pos = 0; pos < segment.data.size(); pos += 2) {
                u16 opcode = segment.data[pos] | (segment.data[pos + 1] << 8);
                fprintf(file, "%08X  %04X         ", segment.target + pos / 2, opcode);
                auto decoder = Decode<Disassembler>(opcode);
                bool expand = false;
                u16 expand_value = 0;
                std::string result = "";
                if (decoder) {
                    if (decoder->NeedExpansion()) {
                        expand = true;
                        pos += 2;
                        expand_value = segment.data[pos] | (segment.data[pos + 1] << 8);
                    }
                    result = decoder->call(dsm, opcode, expand_value);
                }
                if (result.empty()) {
                    result = "[Unknown]";
                }
                fprintf(file, "%s\n", result.c_str());
                if (expand) {
                    fprintf(file, "%08X  %04X ^^^\n", segment.target + pos / 2, expand_value);
                }
            }
        }

        if (segment.memory_type == 2) {
            fprintf(file, "\n>>>>>>>> Data Segment <<<<<<<<\n\n");
            for (unsigned pos = 0; pos < segment.data.size(); pos += 2) {
                u16 opcode = segment.data[pos] | (segment.data[pos + 1] << 8);
                fprintf(file, "%08X  %04X\n", segment.target + pos / 2, opcode);
            }
        }
    }

    fclose(file);

    printf("Start interpreter...\n");

    RegisterState r;
    DspMemorySharedWithCitra m;
    Interpreter interpreter(r, m);
    r.Reset();
    while(1) {
        interpreter.Run(1);
    }
}
