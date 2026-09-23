#pragma once

#include <cstdint>

namespace cg::audio {

struct SpatialAudioRecord;

extern std::uint32_t gValue103DE80; // 原 0x103DE80，虚拟零填充 W32；52CD30 写入，52CCE0 读取。

SpatialAudioRecord* __cdecl Sub_52CD40(); // 原 [0x52CD40,0x52CD5C)，重置 50 项并返回一过尾指针。
std::uint32_t Sub_52B990() noexcept;

} // namespace cg::audio
