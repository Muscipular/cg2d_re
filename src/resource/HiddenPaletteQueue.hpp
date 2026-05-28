#pragma once

#include <array>

namespace cg::resource {

// 来自 IDA 0x58B7E8..0x58D348 的旧客户端隐藏调色盘静态 queue1。
// 值是 hidden palette Graphic 的 mapNo，不是 Graphic seq/globalIndex。
// 返回完整静态 queue1；值 <= 0 表示没有有效 hidden palette mapNo。
[[nodiscard]] std::array<int, 1752> BuildStaticHiddenPaletteQueue() noexcept;

} // namespace cg::resource
