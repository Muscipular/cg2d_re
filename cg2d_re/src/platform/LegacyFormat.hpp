#pragma once

#include "LegacyStdio.hpp"

#include <cstdio> // 原0x0053EB28／0x0053DEF4：MSVC std::vfprintf／std::vfscanf提供，当前无CPP消费者。
#include <cstdint>

namespace cg::platform {

// 原53CB10；MainLoop真实计时除法，U64参数/返回和现调用保持。
std::uint64_t DivideLegacyUnsigned64_53CB10(
    std::uint64_t dividend, std::uint64_t divisor);

} // namespace cg::platform
