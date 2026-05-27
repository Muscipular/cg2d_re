#pragma once

#include <cstdint>

namespace cg::resource {

// DecodeMapCell 校对自 IDA 0x0046CF20 初始化表与 0x0046CF00 查询逻辑。
[[nodiscard]] std::uint32_t DecodeMapCell(std::uint32_t raw) noexcept;

} // namespace cg::resource
