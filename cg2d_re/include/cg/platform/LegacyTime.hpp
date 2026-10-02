#pragma once

#include <cstdint>
#include <cstdio>

namespace cg::platform {

// 原1085FE0共享S32错误槽；文件查询/目录创建或切换写入，截图读取17判断目录已存在。
extern std::int32_t gLegacyErrno_1085FE0;

// 原53DA45/53DA5C；S32解析，end借用原字节游标，失败/溢出沿旧共享错误槽。
std::int32_t Strtol_53DA45(const char* text, char** endPointer, std::int32_t radix);

// 原53C649；转发到既有Atol_53C5BE，返回原S32数值，不新增转换或校验。
std::int32_t Atoi_53C649(const char* text);

// 原54ABDD；按原W32/进制写数字文本并返回buffer，十进制负数才添加负号。
char* Itoa_54ABDD(std::int32_t value, char* buffer, std::int32_t radix);

// 原53D26E；原字节分词并就地截断，空text续用同一进程共享借用位置。
char* StrTok_53D26E(char* text, const char* delimiters);

// 原53C654；旧字节路径交CreateDirectoryA，非零错误经542F44写共享槽，返回0/-1。
std::int32_t CreateLegacyDirectory_53C654(const char* path);

// 原53DD2F；切目录后更新驱动器环境项，失败沿542F44，不回滚已发生的目录改变。
std::int32_t ChangeLegacyDirectory_53DD2F(const char* path);

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
