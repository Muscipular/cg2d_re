#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原窄字符串已消费的+00..+0F存储视图；原+00..+03不解释、不补初值。
// 地址字段拓宽；没有隐式构析，调用方沿原Tidy/共享标记生产和释放。
struct LegacyNarrowString final {
    std::uint8_t prefix00[4]; // 原未在本组解释的前缀字节。
    char* buffer;             // 原+04；NSH01宿主字节块+1，buffer[-1]仍是共享标记。
    std::uint32_t length;     // 原+08；旧字节数，非Unicode字符数。
    std::uint32_t capacity;   // 原+0C；不含标记和末NUL，保持W32。
};
static_assert(offsetof(LegacyNarrowString, buffer) == 8);
static_assert(offsetof(LegacyNarrowString, length) == 16);
static_assert(offsetof(LegacyNarrowString, capacity) == 20);
static_assert(sizeof(LegacyNarrowString) == 24);

extern std::uint32_t gLegacyNarrowStringNpos550040; // 原已加载FFFFFFFF；原比较时读取。
extern char gLegacyNarrowStringEmpty55013B[2]; // 原标记/首NUL；空串指针取+1。

// 原54A67A/54A899已接真实消息/对象与抛出依赖；前缀来源已证，实际SDK提供者待。
void ThrowLegacyStringLength_54A67A();
void ThrowLegacyStringRange_54A899();

// 原八叶；显式对象地址按x64传递，数值、共享U8及调用顺序见局部SPEC。
void TidyLegacyNarrowString_548E29(LegacyNarrowString* storage, bool release);
void FreezeLegacyNarrowString_548F74(LegacyNarrowString* storage);
bool GrowLegacyNarrowString_548FA0(
    LegacyNarrowString* storage, std::uint32_t requested, bool trim);
void SplitLegacyNarrowString_549025(LegacyNarrowString* storage);
void CopyLegacyNarrowString_54908A(LegacyNarrowString* storage, std::uint32_t requested);
LegacyNarrowString* AssignLegacyNarrowString_549055(
    LegacyNarrowString* storage, const char* source, std::uint32_t count);
LegacyNarrowString* AppendLegacyNarrowString_548D69(
    LegacyNarrowString* storage, std::uint32_t count, std::int8_t character);
LegacyNarrowString* EraseLegacyNarrowString_548DC2(
    LegacyNarrowString* storage, std::uint32_t position, std::uint32_t count);

// 原54A315；保留自身双erase/整串共享/分段复制，源对象是非拥有借用。
LegacyNarrowString* AssignLegacyNarrowStringFromString_54A315(
    LegacyNarrowString* storage, const LegacyNarrowString* source,
    std::uint32_t position, std::uint32_t count);

} // namespace cg::platform
