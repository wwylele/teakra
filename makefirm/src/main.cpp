#include <iostream>
#include <fstream>
#include <string>
#include <openssl/sha.h>
#include "common_types.h"

template <typename T>
std::vector<u8> Sha256(const std::vector<T>& data) {
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.data(), data.size() * sizeof(T));
    std::vector<u8> result(0x20);
    SHA256_Final(result.data(), &ctx);
    return result;
}

struct Segment {
    std::vector<u16> data;
    u8 memory_type;
    u32 target;
};

int main(int argc, char** argv) {
    if (argc < 3)
        throw "nope";

    std::ifstream in(argv[1]);
    if (!in.is_open())
        throw "fail";

    std::string line;
    std::vector<Segment> segments;
    u32 addr;
    while(std::getline(in, line)) {
        if (line.size() < 6)
            continue;
        if (line[0] == '.') {
            Segment s;
            s.memory_type = line[1] == 'p' ? 0 : 2;
            addr = s.target = std::stoi(std::string(line.begin() + 2, line.begin() + 6), 0, 16);
            segments.push_back(s);
            printf("New segment(%d) @ %04X\n", s.memory_type, s.target);
        } else {
            std::string sa, sv;
            sa = std::string(line.begin(), line.begin() + 4);
            sv = std::string(line.begin() + 5, line.begin() + 9);
            if (sa != "    ") {
                if ((u16)std::stoi(sa, 0, 16) != addr) {
                    throw "wtf";
                }
            }
            u16 v = (u16)std::stoi(sv, 0, 16);
            segments.back().data.push_back(v);
            printf("%04X:%04X\n", addr, v);
            ++addr;
        }
    }
    in.close();

    FILE* out = fopen(argv[2], "wb");
    if (!out)
        throw "fail!!!";

    u32 data_ptr = 0x300;

    for (unsigned i = 0; i < segments.size(); ++i) {
        fseek(out, 0x120 + i * 0x30, SEEK_SET);
        fwrite(&data_ptr, 4, 1, out);
        fwrite(&segments[i].target, 4, 1, out);
        u32 size = segments[i].data.size() * 2;
        fwrite(&size, 4, 1, out);
        u32 memory_type = segments[i].memory_type << 24;
        fwrite(&memory_type, 4, 1, out);
        auto sha = Sha256(segments[i].data);
        fwrite(sha.data(), 0x20, 1, out);

        fseek(out, data_ptr, SEEK_SET);
        fwrite(segments[i].data.data(), size, 1, out);
        data_ptr += size;
    }

    fseek(out, 0x100, SEEK_SET);
    fwrite("DSP1", 4, 1, out);
    fwrite(&data_ptr, 4, 1, out);
    u32 memory_layout = 0x0000FFFF;
    fwrite(&memory_layout, 4, 1, out);
    u32 misc = segments.size() << 16;
    fwrite(&misc, 4, 1, out);
    u32 zero = 0;
    fwrite(&zero, 4, 1, out);
    fwrite(&zero, 4, 1, out);
    fwrite(&zero, 4, 1, out);
    fwrite(&zero, 4, 1, out);

    fclose(out);

}
