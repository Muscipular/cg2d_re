#pragma once

#include <array>
#include <cstdint>

namespace cg::render {

// 原 [0x62ECA0,0x62ECB8) 的六个连续 W32 槽；0 为空，文本尾令牌 i+1 指向第 i 项。
extern std::array<std::uint32_t, 6> gGraphicCache4Queue;
std::int32_t ClearTextQueueTail_456D50(std::int8_t mode); // 仅清指定层尾令牌，返回符号扩展mode。

} // namespace cg::render
