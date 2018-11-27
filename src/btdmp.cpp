#include "btdmp.h"
#include <string>

namespace Teakra {

Btdmp::Btdmp(const char* debug_string) : debug_string(debug_string) {
    file = std::fopen((std::string("dspout.wav.") + debug_string).c_str(), "wb");
    std::fseek(file, sizeof(wavfileheader), SEEK_SET);
}
Btdmp::~Btdmp() {
    std::fseek(file, 0, SEEK_SET);
    std::fwrite(&wavfileheader, 1, sizeof(wavfileheader), file);
    std::fclose(file);
}

void Btdmp::SendSample(u16 value) {
    wavfileheader.dataSize += 2;
    wavfileheader.dataSize2 += 2;
    std::fwrite(&value, 1, 2, file);
}

} // namespace Teakra
