#pragma once

#include <cstdint>
#include <cstdio>

namespace cg::platform {

// 原 _time@[0x53BA26,0x53BB02)，保留 32 位结果及原时区采样、缓存、转换调用。
// output 可以为 nullptr；其他值须指向可写 int32_t。未以本机 time64/time32 代替原函数。
std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output);

// 原 53CCD0 在 auto.dat 两个 caller 中的时间合同；不代替通用 stat。
// file 必须仍打开、output 可写；保留修改/访问/创建时间转换顺序，返回 0/-1。
std::int32_t ReadLegacyFileCreationTime32(std::FILE* file, std::int32_t* output);

} // namespace cg::platform
