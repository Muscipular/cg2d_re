#pragma once

#include <cstdint>

namespace cg::network {

// 原 sub_516DA0@[0x516DA0,0x517038)：分配并清零九项协议缓冲。
std::int32_t AllocateProtocolBuffers_516DA0(std::uint32_t bufferLength);

} // namespace cg::network
