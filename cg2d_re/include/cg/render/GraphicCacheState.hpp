#pragma once

#include <array>
#include <cstdint>

namespace cg::render {

// 原函数：0x00456D50。
std::int32_t ClearTextQueueTail_456D50(std::int8_t mode); // 仅清指定层尾令牌，返回符号扩展mode。

// 原456D30：重置图形缓存。
std::uint32_t ResetGraphicCache_456D30();

} // namespace cg::render
