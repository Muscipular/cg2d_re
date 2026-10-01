#pragma once

#include <cstdint>
#include <cstdio>

namespace cg::platform {

// 原0x1086128的九个S32值槽；多个调用方借用同一可变对象，不是现代CRT tm。
struct LegacyTm53B8C6 {
    std::int32_t seconds;       // +00 秒
    std::int32_t minutes;       // +04 分
    std::int32_t hours;         // +08 时
    std::int32_t day;           // +0C 月内日
    std::int32_t month0based;   // +10 从零起的月份
    std::int32_t yearMinus1900; // +14 距1900年
    std::int32_t weekDay;       // +18 星期
    std::int32_t yearDay0based; // +1C 从零起的年内日
    std::int32_t dst;           // +20 夏令时标记
};

// 原 _time@[0x53BA26,0x53BB02)，保留 32 位结果及原时区采样、缓存、转换调用。
// output 可以为 nullptr；其他值须指向可写 int32_t。未以本机 time64/time32 代替原函数。
std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output);

// 原 53CCD0 在 auto.dat 两个 caller 中的时间合同；不代替通用 stat。
// file 必须仍打开、output 可写；保留修改/访问/创建时间转换顺序，返回 0/-1。
std::int32_t ReadLegacyFileCreationTime32(std::FILE* file, std::int32_t* output);

} // namespace cg::platform
