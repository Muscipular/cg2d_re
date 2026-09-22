#pragma once

#include <cstdint>

namespace cg::audio {

extern std::uint32_t gValue103DE80; // 原 0x103DE80，虚拟零填充 W32；52CD30 写入，52CCE0 读取。

std::uint32_t Sub_52B990() noexcept;

} // namespace cg::audio
