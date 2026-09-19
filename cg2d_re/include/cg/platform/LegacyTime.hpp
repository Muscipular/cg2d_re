#pragma once

#include <cstdint>

namespace cg::platform {

// 原 _time@[0x53BA26,0x53BB02)，保留 32 位结果及原时区采样、缓存、转换调用。
// output 可以为 nullptr；其他值须指向可写 int32_t。未以本机 time64/time32 代替原函数。
std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output);

} // namespace cg::platform
