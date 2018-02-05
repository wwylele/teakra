#include <boost/optional.hpp>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "decoder.h"
#include "disassembler.h"
#include "dsp1.h"
#include "interpreter.h"
#include "oprand.h"
#include "register.h"
#include "citra.h"

using FILEPtr = std::unique_ptr<FILE, decltype(&std::fclose)>;

static boost::optional<Dsp1> LoadDSPFile() {
    auto file = FILEPtr{fopen("/media/wwylele/学习_娱乐/3DS/PokemonY.romfs/sound/dspaudio.cdc", "rb"), std::fclose};
    if (!file) {
        return boost::none;
    }

    std::vector<u8> raw(217976);
    fread(raw.data(), raw.size(), 1, file.get());
    return Dsp1{raw};
}

static bool DisassembleDSPFile(const Dsp1& dsp) {
    auto file = FILEPtr{fopen("/home/wwylele/teakra/teakra.out", "wt"), std::fclose};
    if (!file) {
        return false;
    }

    Disassembler dsm;

    for (u32 opcode = 0; opcode < 0x10000; ++opcode) {
        Decode<Disassembler>((u16)opcode);
    }

    for (const auto& segment : dsp.segments) {
        if (segment.memory_type == 0 || segment.memory_type == 1) {
            fprintf(file.get(), "\n>>>>>>>> Segment <<<<<<<<\n\n");
            for (unsigned pos = 0; pos < segment.data.size(); pos += 2) {
                u16 opcode = segment.data[pos] | (segment.data[pos + 1] << 8);
                fprintf(file.get(), "%08X  %04X         ", segment.target + pos / 2, opcode);
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
                fprintf(file.get(), "%s\n", result.c_str());
                if (expand) {
                    fprintf(file.get(), "%08X  %04X ^^^\n", segment.target + pos / 2, expand_value);
                }
            }
        }

        if (segment.memory_type == 2) {
            fprintf(file.get(), "\n>>>>>>>> Data Segment <<<<<<<<\n\n");
            for (unsigned pos = 0; pos < segment.data.size(); pos += 2) {
                u16 opcode = segment.data[pos] | (segment.data[pos + 1] << 8);
                fprintf(file.get(), "%08X  %04X\n", segment.target + pos / 2, opcode);
            }
        }
    }

    return true;
}

static void RunInterpreter() {
    RegisterState r;
    DspMemorySharedWithCitra m;
    Interpreter interpreter(r, m);
    r.Reset();

    while (true) {
        interpreter.Run(1);
    }
}

int main() {
    auto dsp = LoadDSPFile();
    if (!dsp) {
        printf("Unable to load DSP file.\n");
        return 1;
    }

    if (!DisassembleDSPFile(dsp.get())) {
        printf("Unable to create disassembler output file.\n");
        return 1;
    }

    printf("Starting interpreter...\n");
    RunInterpreter();
    return 0;
}
