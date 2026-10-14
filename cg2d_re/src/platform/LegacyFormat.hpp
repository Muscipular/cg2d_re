#pragma once

#include "LegacyStdio.hpp"

#include <cstdio> // 原0x0053EB28／0x0053DEF4：MSVC std::vfprintf／std::vfscanf提供，当前无CPP消费者。
#include <cstdint>

namespace cg::platform {

// 原4D8DA0@4D8F5B单W32读取；只借对应命令元素地址，经CPP intrinsic取样后给%d。
// 命令仍仅按原分支写入；CPP该处不直接求值未生产整数，不补值/模拟原栈或外推原值等价。
extern "C" std::int32_t Cg2dReadLegacyShopCommandW32(
    const std::int32_t* command) noexcept;

// 原53CB10；MainLoop真实计时除法，U64参数/返回和现调用保持。
std::uint64_t DivideLegacyUnsigned64_53CB10(
    std::uint64_t dividend, std::uint64_t divisor);

} // namespace cg::platform
