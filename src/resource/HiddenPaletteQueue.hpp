#pragma once

#include <optional>

namespace cg::resource {

// 来自 IDA 0x58B7E8..0x58D348 的旧客户端隐藏调色盘静态 queue1。
// 值是 hidden palette Graphic 的 mapNo，不是 Graphic seq/globalIndex。
[[nodiscard]] int StaticHiddenPaletteQueueCount() noexcept;
[[nodiscard]] std::optional<int> FindStaticHiddenPaletteMapNoByPalIndex(int palIndex) noexcept;

} // namespace cg::resource
