#pragma once

#include <string>
#include <cstdint>

namespace Teakra::Disassembler {

bool NeedExpansion(std::uint16_t opcode);
std::string Do(std::uint16_t opcode, std::uint16_t expansion = 0);

} // namespace Teakra::dissasembler