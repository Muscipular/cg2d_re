#pragma once

#include <array>
#include <cstdint>

namespace cg::render {

// 原 [0x62ECA0,0x62ECB8) 的六个连续 W32 槽，映像初值均为 0。
extern std::array<std::uint32_t, 6> gGraphicCache4Queue;

} // namespace cg::render
