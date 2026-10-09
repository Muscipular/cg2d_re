#pragma once

#include "cg/platform/LegacyStdio.hpp"

#include <cstdint>

namespace cg::platform {


// MC01：查询当前MSVC线程的errno；目录创建与截图的EEXIST判断共用真实状态。
std::int32_t& LegacyErrno_1085FE0() noexcept;
// 当前MSVC线程的_doserrno，宽度仍为W32；不缓存线程局部地址。
unsigned long& LegacyDosError_1085FE4() noexcept;

// 原53DA45：直接std::strtol，返回S32；end借用输入，错误沿MSVC errno。
std::int32_t Strtol_53DA45(const char* text, char** endPointer, std::int32_t radix);

// 原53C649：直接std::atoi，返回S32。
std::int32_t Atoi_53C649(const char* text);

// 原54ABDD：直接MSVC _itoa，目标容量与进制由调用方提供。
char* Itoa_54ABDD(std::int32_t value, char* buffer, std::int32_t radix);

// 原53D26E：直接std::strtok，就地分词与续读状态由MSVC持有。
char* StrTok_53D26E(char* text, const char* delimiters);

// 原5450F0：直接std::strcmp，返回MSVC实际比较结果。
std::int32_t __cdecl StrCmp_5450F0(const char* left, const char* right);

// 原53BFC0：直接std::strncmp，数量保持U32。
std::int32_t __cdecl StrNCmp_53BFC0(const char* left, const char* right, std::uint32_t count);

// 原53F9E0：直接std::strlen，按调用方接口返回U32字节数。
std::uint32_t __cdecl StrLen_53F9E0(const char* text);

// 原544DB0：直接std::strcpy，目标容量由调用方保证。
char* __cdecl StrCpy_544DB0(char* target, const char* source);

// 原544DC0：直接std::strcat，目标容量由调用方保证。
char* __cdecl StrCat_544DC0(char* target, const char* source);

// 原53C790：直接std::strncat，源上限保持U32，返回target。
char* __cdecl StrNCat_53C790(char* target, const char* source, std::uint32_t count);

// 原53BEC0：直接std::strncpy，保持count个字节的复制/补零合同，满长不另补NUL。
char* __cdecl StrNCpy_53BEC0(char* target, const char* source, std::uint32_t count);

// 原53C2F0：直接std::strchr，返回借用首匹配或空。
char* __cdecl StrChr_53C2F0(const char* text, std::int32_t target);

// 原53BE40：直接std::strstr，返回借用首匹配或空。
char* __cdecl StrStr_53BE40(const char* text, const char* substring);

// 原53C654：直接MSVC _mkdir，窄字节路径和真实errno由CRT处理。
std::int32_t CreateLegacyDirectory_53C654(const char* path);

// 原53DD2F：直接MSVC _chdir，目录/驱动器环境与错误由CRT处理。
std::int32_t ChangeLegacyDirectory_53DD2F(const char* path);

// 九个业务S32日历字段；由_localtime32逐字段发布到当前线程的借用对象。
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

// 原53BA26：直接MSVC _time32，业务结果保持S32；output可空。
std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output);

// auto.dat时间：同一MSVC FILE经_fileno/_fstat32，成功才发布S32创建时间。
std::int32_t ReadLegacyFileCreationTime32(LegacyFile* file, std::int32_t* output);

} // namespace cg::platform
