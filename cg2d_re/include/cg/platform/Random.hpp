#pragma once

#include <cstdint>

namespace cg::platform {

// 原0x0053B4BF/0x0053B4C9：进程共用引擎，不依赖账号或会话。
std::uint32_t SeedRandom_53B4BF(std::uint32_t seed);
int Rand16();
// 原0x46FAF0；RRU01：合法闭区间均匀分布，upper == -1返回0，倒置区间沿原算法。
std::int32_t RandomRange_46FAF0(std::int32_t lower, std::int32_t upper);

} // namespace cg::platform
