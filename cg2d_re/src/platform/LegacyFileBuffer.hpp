#pragma once

#include <cstddef>
#include <cstdint>

#include "LegacyLocale.hpp"
#include "LegacyStdio.hpp"
#include "LegacyStreamBuffer.hpp"

namespace cg::platform {

// 原两个独立W32状态种子，目前仅真实声明；初值/实际数据绑定未补证，不补零/FF。
// volatile只保持原每入口两次独立读取，不增加锁或原子操作。
extern volatile std::uint32_t gLegacyNarrowFileState1087B00;
extern volatile std::uint32_t gLegacyWideFileState1087AF8;

// 原basic_filebuf已证+00..+53视图；真实指针拓宽，状态位型/标志宽度保持。
// 不自动初始化/构析、构造owner或接宿主streambuf/FILE。
template <typename Character>
struct LegacyFileBufferStorage final {
    LegacyStreamBufferStorage<Character> buffer;   // 原+00..+33；实际首成员。
    LegacyLocaleImplementation* baseLocale;       // 原+34；本组保留不构析。
    std::uint32_t state38Bits;                    // 原+38；本组清0。
    std::uint32_t state3CBits;                    // 原+3C；第二次种子读取。
    std::uint32_t state40Bits;                    // 原+40；第一次种子读取。
    std::uint32_t state44Bits;                    // 原+44；仅mode0清0。
    std::uint8_t modeOneFlag;                     // 原+48；mode==1的0/1 U8。
    LegacyLocaleImplementation* privateLocale;    // 原+4C；mode0显式重构。
    LegacyFile* file;                              // 原+50；实际原FILE。
};
using LegacyNarrowFileBuffer = LegacyFileBufferStorage<char>;
using LegacyWideFileBuffer = LegacyFileBufferStorage<std::uint16_t>;
static_assert(offsetof(LegacyNarrowFileBuffer, baseLocale) == 96);
static_assert(offsetof(LegacyNarrowFileBuffer, state38Bits) == 104);
static_assert(offsetof(LegacyNarrowFileBuffer, state3CBits) == 108);
static_assert(offsetof(LegacyNarrowFileBuffer, state40Bits) == 112);
static_assert(offsetof(LegacyNarrowFileBuffer, state44Bits) == 116);
static_assert(offsetof(LegacyNarrowFileBuffer, modeOneFlag) == 120);
static_assert(offsetof(LegacyNarrowFileBuffer, privateLocale) == 128);
static_assert(offsetof(LegacyNarrowFileBuffer, file) == 136);
static_assert(sizeof(LegacyNarrowFileBuffer) == 144);
static_assert(offsetof(LegacyWideFileBuffer, privateLocale) == 128);
static_assert(offsetof(LegacyWideFileBuffer, file) == 136);
static_assert(sizeof(LegacyWideFileBuffer) == 144);

// 原两_Init void方法；mode保持原W32/S32位型，仅比较0/1，不增范围限制。
void InitializeLegacyNarrowFileBuffer_548EFC(
    LegacyNarrowFileBuffer* storage, LegacyFile* file, std::int32_t mode);
void InitializeLegacyWideFileBuffer_549E88(
    LegacyWideFileBuffer* storage, LegacyFile* file, std::int32_t mode);
// 原两close；FILE成功关闭返回0才_Init(nullptr,2)/返this，其他返空。
LegacyNarrowFileBuffer* CloseLegacyNarrowFileBuffer_548ED5(LegacyNarrowFileBuffer* storage);
LegacyWideFileBuffer* CloseLegacyWideFileBuffer_549E61(LegacyWideFileBuffer* storage);

} // namespace cg::platform
