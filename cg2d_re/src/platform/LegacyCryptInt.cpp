#include "cg/platform/LegacyCryptInt.hpp"

#include "MainLoopDependencies.hpp"

#include <array>
#include <bit>
#include <cstdlib>
#include <new>
#include <optional>

namespace cg::platform {
namespace {

// 原CRT首项的两个独立对象：0x005939C8、0x005939D8；不与后续保护状态共享。
std::array<std::optional<LegacyCryptInt>, 2>& BorrowStartupValuesStorage()
{
    static std::array<std::optional<LegacyCryptInt>, 2> storage;
    return storage;
}

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

// 原0x00401C10、0x00401C20；按5939C8→5939D8构造，中央启动顺序保持。
LegacyCryptInt::StartupValues::StartupValues()
{
    BorrowStartupValuesStorage()[0].emplace();
    BorrowStartupValuesStorage()[1].emplace();
}

// 原0x00401C50：原组逆序销毁，退出登记由MSVC承接。
LegacyCryptInt::StartupValues::~StartupValues()
{
    BorrowStartupValuesStorage()[1].reset();
    BorrowStartupValuesStorage()[0].reset();
}

// 原0x00401C70 sub_401C70@[0x401C70,0x401C92)：零值默认构造复用有值构造，RNG一次。
LegacyCryptInt::LegacyCryptInt()
    : LegacyCryptInt(std::int32_t{0})
{
}

// 原0x00401CC0 CryptInt_Init@[0x401CC0,0x401CE7)。reserved沿原构造不写。
LegacyCryptInt::LegacyCryptInt(const std::int32_t value)
{
    legacyVftToken = kLegacyCryptIntVftToken;
    mask = NextMask();
    readCount = 0;
    SetNorVal_401F50(value);
}

// 原0x00401CF0 CryptInt_delete@[0x401CF0,0x401CF7)：析构只恢复原token。
LegacyCryptInt::~LegacyCryptInt()
{
    legacyVftToken = kLegacyCryptIntVftToken;
}

LegacyCryptInt::LegacyCryptInt(const RawPayload& raw) noexcept
{
    RestoreRaw(raw);
}

void LegacyCryptInt::RestoreRaw(const RawPayload& raw) noexcept
{
    legacyVftToken = raw.legacyVftToken;
    encoded = raw.encoded;
    mask = raw.mask;
    readCount = raw.readCount;
    for (std::size_t index = 0; index < 3; ++index) {
        reserved[index] = raw.reserved[index];
    }
}

void* LegacyCryptInt::operator new(const std::size_t size)
{
    if (void* storage = std::malloc(size)) {
        return storage;
    }
    throw std::bad_alloc();
}

// 原0x00401CA0：delete先析构再free；栈／成员由原生析构结束寿命。
void LegacyCryptInt::operator delete(void* const storage) noexcept
{
    std::free(storage);
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

LegacyCryptInt::LegacyCryptInt(const std::uint32_t value)
    : LegacyCryptInt(std::bit_cast<std::int32_t>(value))
{
}

LegacyCryptInt& LegacyCryptInt::operator=(const std::int32_t value)
{
    LegacyCryptInt temporary(value);
    CopyPayload(&temporary);
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
