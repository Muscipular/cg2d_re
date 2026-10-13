#include "cg/platform/LegacyCryptInt.hpp"

#include "MainLoopDependencies.hpp"

#include <bit>
#include <cstdlib>

namespace cg::platform {
namespace {

// 原CRT首项的两个独立对象；不与后续保护状态共享。
LegacyCryptInt gStartupValue5939C8{}; // 原全局：0x005939C8。
LegacyCryptInt gStartupValue5939D8{}; // 原全局：0x005939D8。

} // namespace

std::uint32_t LegacyCryptInt::ToBits(std::int32_t value)
{
    return std::bit_cast<std::uint32_t>(value);
}

std::int32_t LegacyCryptInt::FromBits(std::uint32_t value)
{
    return std::bit_cast<std::int32_t>(value);
}

std::uint32_t LegacyCryptInt::NextMask()
{
    return ToBits(static_cast<std::int32_t>(NextRandom_53B4C9()));
}

// 原函数：0x00401C10；LCI01启动拥有者替代CRT构造／退出登记。
LegacyCryptInt::StartupValues::StartupValues()
{
    ConstructStartupValues_401C20();
}

// 对应同一启动组的原逆序清理；登记与异常规则由MSVC承接。
LegacyCryptInt::StartupValues::~StartupValues()
{
    DestroyStartupValues_401C50();
}

// 原函数：0x00401C20。
void LegacyCryptInt::ConstructStartupValues_401C20()
{
    gStartupValue5939C8.ConstructZero_401C70();
    gStartupValue5939D8.ConstructZero_401C70();
}

// 原函数：0x00401C50。
void LegacyCryptInt::DestroyStartupValues_401C50()
{
    gStartupValue5939D8.Destroy_401CF0();
    gStartupValue5939C8.Destroy_401CF0();
}

// 原函数：0x00401C70。
LegacyCryptInt* LegacyCryptInt::ConstructZero_401C70()
{
    this->legacyVftToken = kLegacyCryptIntVftToken;
    this->mask = NextMask();
    this->readCount = 0;
    SetNorVal_401F50(0);
    return this;
}

// 原函数：0x00401CC0。
LegacyCryptInt* LegacyCryptInt::ConstructFromInt_401CC0(std::int32_t value)
{
    this->legacyVftToken = kLegacyCryptIntVftToken;
    this->mask = NextMask();
    this->readCount = 0;
    SetNorVal_401F50(value);
    return this;
}

// 原函数：0x00401CF0。
void LegacyCryptInt::Destroy_401CF0()
{
    this->legacyVftToken = kLegacyCryptIntVftToken;
}

// 原函数：0x00401F50。
void LegacyCryptInt::SetNorVal_401F50(std::int32_t value)
{
    this->encoded = ToBits(value) ^ this->mask;
}

void LegacyCryptInt::CopyPayload(const LegacyCryptInt* source)
{
    this->encoded = source->encoded;
    this->mask = source->mask;
    this->readCount = source->readCount;
}

// 原函数：0x00401EC0。
// 关联原入口：0x00401EA0（跳转共用本函数读取正文）。
std::int32_t LegacyCryptInt::Read_401EC0()
{
    const std::uint8_t nextCount =
        static_cast<std::uint8_t>(this->readCount + std::uint8_t{1});
    this->readCount = nextCount;

    if (std::bit_cast<std::int8_t>(nextCount) > 20) {
        const std::int32_t plain = FromBits(this->encoded ^ this->mask);
        this->readCount = 0;
        this->mask = NextMask();
        SetNorVal_401F50(plain);
        return plain;
    }

    return FromBits(this->encoded ^ this->mask);
}

// 原函数：0x00401FA0。
std::uint32_t LegacyCryptInt::IntLessCryptRaw_401FA0(const std::int32_t lhs)
{
    const std::int32_t right = Read_401EC0();
    return (ToBits(right) & 0xFFFFFF00U)
        | static_cast<std::uint32_t>(lhs < right);
}

// 原函数：0x00401CA0。
LegacyCryptInt* LegacyCryptInt::DestroyAndFree_401CA0(std::uint32_t flags)
{
    Destroy_401CF0();
    if ((flags & 1u) != 0) {
        std::free(this);
    }
    return this;
}

// 原函数：0x00401CC0；整数构造沿既有有值构造正文。
LegacyCryptInt::LegacyCryptInt(const std::int32_t value)
{
    ConstructFromInt_401CC0(value);
}

LegacyCryptInt::LegacyCryptInt(const std::uint32_t value)
    : LegacyCryptInt(std::bit_cast<std::int32_t>(value))
{
}

LegacyCryptInt& LegacyCryptInt::operator=(const std::int32_t value)
{
    LegacyCryptInt temporary(value);
    CopyPayload(&temporary);
    temporary.Destroy_401CF0();
    return *this;
}

LegacyCryptInt& LegacyCryptInt::operator=(const std::uint32_t value)
{
    return *this = std::bit_cast<std::int32_t>(value);
}

// 原函数：0x00401EA0、0x00401EC0；两种转换各执行一次有副作用的读取。
// 原S32标量比较：0x00401D00、0x00401D40、0x00401D70、0x00401DA0、0x00401DB0、0x00401F80、0x00401FC0。
// 原S32标量算术：0x00401E10、0x00401E60、0x00401E90、0x00401FE0；通过转换与内建operator承接。
LegacyCryptInt::operator std::int32_t()
{
    return Read_401EC0();
}

LegacyCryptInt::operator std::uint32_t()
{
    return static_cast<std::uint32_t>(Read_401EC0());
}

// 原函数：0x00401E20。
LegacyCryptInt& LegacyCryptInt::operator+=(const std::int32_t amount)
{
    const std::int32_t result = Read_401EC0() + amount;
    SetNorVal_401F50(result);
    return *this;
}

LegacyCryptInt& LegacyCryptInt::operator+=(const std::uint32_t amount)
{
    const std::uint32_t result = static_cast<std::uint32_t>(*this) + amount;
    SetNorVal_401F50(std::bit_cast<std::int32_t>(result));
    return *this;
}

// 原函数：0x00401E40。
LegacyCryptInt& LegacyCryptInt::operator-=(const std::int32_t amount)
{
    const std::int32_t result = Read_401EC0() - amount;
    SetNorVal_401F50(result);
    return *this;
}

LegacyCryptInt& LegacyCryptInt::operator-=(const std::uint32_t amount)
{
    const std::uint32_t result = static_cast<std::uint32_t>(*this) - amount;
    SetNorVal_401F50(std::bit_cast<std::int32_t>(result));
    return *this;
}

// 原函数：0x00401DE0；单读递增正文移入前置operator，当前调用不消费旧整数返回。
LegacyCryptInt& LegacyCryptInt::operator++()
{
    const std::int32_t result = Read_401EC0() + 1;
    SetNorVal_401F50(result);
    return *this;
}

std::int32_t LegacyCryptInt::operator++(int)
{
    const std::int32_t previous = Read_401EC0();
    SetNorVal_401F50(previous + 1);
    return previous;
}

std::int32_t operator+(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    const std::int32_t right = rhs.Read_401EC0();
    return lhs.Read_401EC0() + right;
}

// 原函数：0x00401E70。
std::int32_t operator-(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    const std::int32_t right = rhs.Read_401EC0();
    return lhs.Read_401EC0() - right;
}

std::int32_t operator*(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    const std::int32_t right = rhs.Read_401EC0();
    return lhs.Read_401EC0() * right;
}

// 原函数：0x00401D20；相等与不等使用同一右→左读取顺序。
bool operator==(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    return !(lhs != rhs);
}

bool operator!=(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    const std::int32_t right = rhs.Read_401EC0();
    return lhs.Read_401EC0() != right;
}

// 原函数：0x00401D50。
bool operator>(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    const std::int32_t right = rhs.Read_401EC0();
    return lhs.Read_401EC0() > right;
}

// 原函数：0x00401D80。
bool operator>=(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    const std::int32_t right = rhs.Read_401EC0();
    return lhs.Read_401EC0() >= right;
}

// 原函数：0x00401DC0。
bool operator<(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    const std::int32_t right = rhs.Read_401EC0();
    return lhs.Read_401EC0() < right;
}

bool operator<=(LegacyCryptInt& lhs, LegacyCryptInt& rhs)
{
    return !(lhs > rhs);
}

} // namespace cg::platform
