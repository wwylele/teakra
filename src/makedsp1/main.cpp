#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include "../common_types.h"
#include "../parser.h"
#include "sha256.h"

template <typename T>
std::vector<u8> Sha256(const std::vector<T>& data) {
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE*)data.data(), data.size() * sizeof(T));
    std::vector<u8> result(0x20);
    sha256_final(&ctx, result.data());
    return result;
}

struct Segment {
    std::vector<u16> data;
    u8 memory_type;
    u32 target;
};

std::vector<std::string> StringToTokens(const std::string& in) {
    std::vector<std::string> out;
    bool need_new = true;
    for (char c : in) {
        if (c == ' ' || c == '\t') {
            need_new = true;
        } else {
            if (need_new) {
                need_new = false;
                out.push_back("");
            }
            out.back() += c;
        }
    }
    return out;
}

int main(int argc, char** argv) {
    auto parser = Teakra::GenerateParser();

    if (argc < 3)
        throw "nope";

    std::ifstream in(argv[1]);
    if (!in.is_open())
        throw "fail";

    std::string line;
    std::vector<Segment> segments;
    u32 addr;
    int line_number = 0;
    bool next_must_be_data = false;
    while (std::getline(in, line)) {
        ++line_number;
        auto comment_pos = line.find("//");
        if (comment_pos != std::string::npos) {
            line.erase(comment_pos);
        }

        auto tokens = StringToTokens(line);
        if (tokens.size() == 0)
            continue;

        if (tokens[0] == "segment") {
            if (tokens.size() != 3) {
                printf("%d: Wrong parameter count for 'segment'\n", line_number);
                return -1;
            }
            Segment s;
            if (tokens[1] == "p") {
                s.memory_type = 0;
            } else if (tokens[1] == "d") {
                s.memory_type = 2;
            } else {
                printf("%d: Unknown segment type %s\n", line_number, tokens[1].c_str());
                return -1;
            }

            addr = s.target = std::stoi(tokens[2], 0, 16);
            segments.push_back(s);
            printf("New segment(%d) @ %04X\n", s.memory_type, s.target);
        } else {
            u16 v;
            if (tokens[0] == "data") {
                next_must_be_data = false;
                if (tokens.size() != 2) {
                    printf("%d: Wrong parameter count for 'data'\n", line_number);
                    return -1;
                }
                v = (u16)std::stoi(tokens[1], 0, 16);
            } else {
                if (next_must_be_data) {
                    printf("%d: must be data for expansion\n", line_number);
                    return -1;
                }
                auto maybe_v = parser->Parse(tokens);
                if (maybe_v.status == Teakra::Parser::Opcode::Invalid) {
                    printf("%d: could not parse\n", line_number);
                    return -1;
                }
                next_must_be_data = maybe_v.status == Teakra::Parser::Opcode::ValidWithExpansion;
                v = maybe_v.opcode;
            }

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
