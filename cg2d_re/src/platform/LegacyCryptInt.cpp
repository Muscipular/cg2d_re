#include "cg/platform/LegacyCryptInt.hpp"

#include "MainLoopDependencies.hpp"

#include <bit>
#include <cstdlib>

namespace cg::platform::legacy_crypt_int {
namespace {

// 原CRT首项的两个独立对象；不与后续保护状态共享。
LegacyCryptInt gStartupValue5939C8{}; // 原全局：0x005939C8。
LegacyCryptInt gStartupValue5939D8{}; // 原全局：0x005939D8。

std::uint32_t ToBits(std::int32_t value)
{
    return std::bit_cast<std::uint32_t>(value);
}

std::int32_t FromBits(std::uint32_t value)
{
    return std::bit_cast<std::int32_t>(value);
}

std::uint32_t NextMask()
{
    return ToBits(static_cast<std::int32_t>(NextRandom_53B4C9()));
}

} // namespace

// 原函数：0x00401C10。
std::int32_t InitializeLegacyStartupValues_401C10()
{
    ConstructStartupValues_401C20();
    return std::atexit(DestroyStartupValues_401C50);
}

// 原函数：0x00401C20。
void ConstructStartupValues_401C20()
{
    ConstructZero_401C70(&gStartupValue5939C8);
    ConstructZero_401C70(&gStartupValue5939D8);
}

// 原函数：0x00401C50。
void DestroyStartupValues_401C50()
{
    Destroy_401CF0(&gStartupValue5939D8);
    Destroy_401CF0(&gStartupValue5939C8);
}

// 原函数：0x00401C70。
LegacyCryptInt* ConstructZero_401C70(LegacyCryptInt* storage)
{
    storage->legacyVftToken = kLegacyCryptIntVftToken;
    storage->mask = NextMask();
    storage->readCount = 0;
    SetNorVal_401F50(storage, 0);
    return storage;
}

// 原函数：0x00401CC0。
LegacyCryptInt* ConstructFromInt_401CC0(
    LegacyCryptInt* storage,
    std::int32_t value)
{
    storage->legacyVftToken = kLegacyCryptIntVftToken;
    storage->mask = NextMask();
    storage->readCount = 0;
    SetNorVal_401F50(storage, value);
    return storage;
}

// 原函数：0x00401CF0。
void Destroy_401CF0(LegacyCryptInt* value)
{
    value->legacyVftToken = kLegacyCryptIntVftToken;
}

// 原函数：0x00401F50。
void SetNorVal_401F50(LegacyCryptInt* target, std::int32_t value)
{
    target->encoded = ToBits(value) ^ target->mask;
}

void CopyPayload(LegacyCryptInt* target, const LegacyCryptInt* source)
{
    target->encoded = source->encoded;
    target->mask = source->mask;
    target->readCount = source->readCount;
}

void AssignFromInt(LegacyCryptInt* target, std::int32_t value)
{
    LegacyCryptInt temporary;
    ConstructFromInt_401CC0(&temporary, value);
    CopyPayload(target, &temporary);
    Destroy_401CF0(&temporary);
}

// 原函数：0x00401EC0。
// 关联原入口：0x00401EA0（跳转共用本函数读取正文）。
std::int32_t Read_401EC0(LegacyCryptInt* value)
{
    const std::uint8_t nextCount =
        static_cast<std::uint8_t>(value->readCount + std::uint8_t{1});
    value->readCount = nextCount;

    if (std::bit_cast<std::int8_t>(nextCount) > 20) {
        const std::int32_t plain = FromBits(value->encoded ^ value->mask);
        value->readCount = 0;
        value->mask = NextMask();
        SetNorVal_401F50(value, plain);
        return plain;
    }

    return FromBits(value->encoded ^ value->mask);
}

// 原函数：0x00401DE0。
std::int32_t Increment_401DE0(LegacyCryptInt* value, const std::int32_t unused)
{
    static_cast<void>(unused);
    const std::int32_t result = Read_401EC0(value) + 1;
    SetNorVal_401F50(value, result);
    return result;
}

// 原函数：0x00401E10。
std::int32_t Add_401E10(LegacyCryptInt* value, const std::int32_t amount)
{
    return Read_401EC0(value) + amount;
}

// 原函数：0x00401E20。
void AddAssign_401E20(LegacyCryptInt* const value, const std::int32_t amount)
{
    const std::int32_t result = Add_401E10(value, amount);
    SetNorVal_401F50(value, result);
}

// 原函数：0x00401E40。
void SubtractAssign_401E40(LegacyCryptInt* const value, const std::int32_t amount)
{
    const std::int32_t result = Subtract_401E60(value, amount);
    SetNorVal_401F50(value, result);
}

// 原函数：0x00401E60。
std::int32_t Subtract_401E60(
    LegacyCryptInt* value, const std::int32_t amount)
{
    return Read_401EC0(value) - amount;
}

// 原函数：0x00401E70。
std::int32_t Difference_401E70(LegacyCryptInt* lhs, LegacyCryptInt* rhs)
{
    const std::int32_t right = Read_401EC0(rhs);
    return Read_401EC0(lhs) - right;
}

// 原函数：0x00401E90。
std::int32_t Multiply_401E90(
    LegacyCryptInt* value, const std::int32_t amount)
{
    return Read_401EC0(value) * amount;
}

// 原函数：0x00401D00。
bool Equal_401D00(LegacyCryptInt* value, std::int32_t expected)
{
    return Read_401EC0(value) == expected;
}

// 原函数：0x00401D20。
bool NotEqual_401D20(LegacyCryptInt* lhs, LegacyCryptInt* rhs)
{
    const std::int32_t right = Read_401EC0(rhs);
    return Read_401EC0(lhs) != right;
}

// 原函数：0x00401D40。
bool Greater_401D40(LegacyCryptInt* value, std::int32_t threshold)
{
    return Read_401EC0(value) > threshold;
}

// 原函数：0x00401D50。
bool GreaterCrypt_401D50(LegacyCryptInt* const lhs, LegacyCryptInt* const rhs)
{
    const std::int32_t right = Read_401EC0(rhs);
    return Read_401EC0(lhs) > right;
}

// 原函数：0x00401D70。
bool GreaterEqual_401D70(LegacyCryptInt* value, std::int32_t minimum)
{
    return Read_401EC0(value) >= minimum;
}

// 原函数：0x00401D80。
bool GreaterEqualCrypt_401D80(LegacyCryptInt* const lhs, LegacyCryptInt* const rhs)
{
    const std::int32_t right = Read_401EC0(rhs);
    return Read_401EC0(lhs) >= right;
}

// 原函数：0x00401DA0。
bool LessEqual_401DA0(LegacyCryptInt* value, std::int32_t maximum)
{
    return Read_401EC0(value) <= maximum;
}

// 原函数：0x00401DB0。
bool Less_401DB0(LegacyCryptInt* value, std::int32_t maximum)
{
    return Read_401EC0(value) < maximum;
}

// 原函数：0x00401DC0。
bool LessCrypt_401DC0(LegacyCryptInt* const lhs, LegacyCryptInt* const rhs)
{
    const std::int32_t right = Read_401EC0(rhs);
    return Read_401EC0(lhs) < right;
}

// 原函数：0x00401F80。
bool IntGreaterCrypt_401F80(const std::int32_t lhs, LegacyCryptInt* const rhs)
{
    return lhs > Read_401EC0(rhs);
}

// 原函数：0x00401FA0。
std::uint32_t IntLessCryptRaw_401FA0(
    const std::int32_t lhs, LegacyCryptInt* const rhs)
{
    const std::int32_t right = Read_401EC0(rhs);
    return (ToBits(right) & 0xFFFFFF00U)
        | static_cast<std::uint32_t>(lhs < right);
}

// RHL01：原401FC0单读右对象，再按有符号S32比较lhs<=rhs；已证调用仅消费AL。
bool IntLessEqualCrypt_401FC0(std::int32_t lhs, LegacyCryptInt* rhs)
{
    return lhs <= Read_401EC0(rhs);
}

// 原函数：0x00401FE0。
std::int32_t IntMinusCrypt_401FE0(
    const std::int32_t lhs, LegacyCryptInt* const rhs)
{
    return lhs - Read_401EC0(rhs);
}

// 原函数：0x00401CA0。
LegacyCryptInt* DestroyAndFree_401CA0(LegacyCryptInt* value, std::uint32_t flags)
{
    Destroy_401CF0(value);
    if ((flags & 1u) != 0) {
        std::free(value);
    }
    return value;
}

} // namespace cg::platform::legacy_crypt_int
