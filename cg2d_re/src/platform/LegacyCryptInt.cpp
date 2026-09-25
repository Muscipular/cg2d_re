#include "cg/platform/LegacyCryptInt.hpp"

#include "MainLoopDependencies.hpp"

#include <bit>

namespace cg::platform::legacy_crypt_int {
namespace {

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

LegacyCryptInt* ConstructZero_401C70(LegacyCryptInt* storage)
{
    storage->legacyVftToken = kLegacyCryptIntVftToken;
    storage->mask = NextMask();
    storage->readCount = 0;
    SetNorVal_401F50(storage, 0);
    return storage;
}

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

void Destroy_401CF0(LegacyCryptInt* value)
{
    value->legacyVftToken = kLegacyCryptIntVftToken;
}

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

std::int32_t Add_401E10(LegacyCryptInt* value, const std::int32_t amount)
{
    const std::uint32_t result =
        ToBits(Read_401EC0(value)) + ToBits(amount);
    return FromBits(result);
}

std::int32_t Subtract_401E60(
    LegacyCryptInt* value, const std::int32_t amount)
{
    const std::uint32_t result =
        ToBits(Read_401EC0(value)) - ToBits(amount);
    return FromBits(result);
}

std::int32_t Multiply_401E90(
    LegacyCryptInt* value, const std::int32_t amount)
{
    const std::uint32_t result =
        ToBits(Read_401EC0(value)) * ToBits(amount);
    return FromBits(result);
}

bool Equal_401D00(LegacyCryptInt* value, std::int32_t expected)
{
    return Read_401EC0(value) == expected;
}

bool Greater_401D40(LegacyCryptInt* value, std::int32_t threshold)
{
    return Read_401EC0(value) > threshold;
}

bool GreaterEqual_401D70(LegacyCryptInt* value, std::int32_t minimum)
{
    return Read_401EC0(value) >= minimum;
}

bool LessEqual_401DA0(LegacyCryptInt* value, std::int32_t maximum)
{
    return Read_401EC0(value) <= maximum;
}

bool Less_401DB0(LegacyCryptInt* value, std::int32_t maximum)
{
    return Read_401EC0(value) < maximum;
}

bool IntLessEqualCrypt_401FC0(std::int32_t lhs, LegacyCryptInt* rhs)
{
    return lhs <= Read_401EC0(rhs);
}

std::int32_t IntMinusCrypt_401FE0(
    const std::int32_t lhs, LegacyCryptInt* const rhs)
{
    return FromBits(ToBits(lhs) - ToBits(Read_401EC0(rhs)));
}

} // namespace cg::platform::legacy_crypt_int
