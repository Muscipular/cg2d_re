#include "LegacyFloatNumeric.hpp"

#include <bit>
#include <cstring>

namespace cg::platform {

namespace {

// 原5925D8/592738；每组八进制位对应7项，最后一项为4096次幂。
constexpr LegacyFloat96 kPositivePowers5925D8[29] = {
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x02, 0x40}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x05, 0x40}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x08, 0x40}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x0C, 0x40}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xC3, 0x0F, 0x40}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0xF4, 0x12, 0x40}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x96, 0x98, 0x16, 0x40}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xBC, 0xBE, 0x19, 0x40}},
    {{0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0xBF, 0xC9, 0x1B, 0x8E, 0x34, 0x40}},
    {{0x00, 0x00, 0x00, 0xA1, 0xED, 0xCC, 0xCE, 0x1B, 0xC2, 0xD3, 0x4E, 0x40}},
    {{0x20, 0xF0, 0x9E, 0xB5, 0x70, 0x2B, 0xA8, 0xAD, 0xC5, 0x9D, 0x69, 0x40}},
    {{0xD0, 0x5D, 0xFD, 0x25, 0xE5, 0x1A, 0x8E, 0x4F, 0x19, 0xEB, 0x83, 0x40}},
    {{0x71, 0x96, 0xD7, 0x95, 0x43, 0x0E, 0x05, 0x8D, 0x29, 0xAF, 0x9E, 0x40}},
    {{0xF9, 0xBF, 0xA0, 0x44, 0xED, 0x81, 0x12, 0x8F, 0x81, 0x82, 0xB9, 0x40}},
    {{0xBF, 0x3C, 0xD5, 0xA6, 0xCF, 0xFF, 0x49, 0x1F, 0x78, 0xC2, 0xD3, 0x40}},
    {{0x6F, 0xC6, 0xE0, 0x8C, 0xE9, 0x80, 0xC9, 0x47, 0xBA, 0x93, 0xA8, 0x41}},
    {{0xBC, 0x85, 0x6B, 0x55, 0x27, 0x39, 0x8D, 0xF7, 0x70, 0xE0, 0x7C, 0x42}},
    {{0xBC, 0xDD, 0x8E, 0xDE, 0xF9, 0x9D, 0xFB, 0xEB, 0x7E, 0xAA, 0x51, 0x43}},
    {{0xA1, 0xE6, 0x76, 0xE3, 0xCC, 0xF2, 0x29, 0x2F, 0x84, 0x81, 0x26, 0x44}},
    {{0x28, 0x10, 0x17, 0xAA, 0xF8, 0xAE, 0x10, 0xE3, 0xC5, 0xC4, 0xFA, 0x44}},
    {{0xEB, 0xA7, 0xD4, 0xF3, 0xF7, 0xEB, 0xE1, 0x4A, 0x7A, 0x95, 0xCF, 0x45}},
    {{0x65, 0xCC, 0xC7, 0x91, 0x0E, 0xA6, 0xAE, 0xA0, 0x19, 0xE3, 0xA3, 0x46}},
    {{0x0D, 0x65, 0x17, 0x0C, 0x75, 0x81, 0x86, 0x75, 0x76, 0xC9, 0x48, 0x4D}},
    {{0x58, 0x42, 0xE4, 0xA7, 0x93, 0x39, 0x3B, 0x35, 0xB8, 0xB2, 0xED, 0x53}},
    {{0x4D, 0xA7, 0xE5, 0x5D, 0x3D, 0xC5, 0x5D, 0x3B, 0x8B, 0x9E, 0x92, 0x5A}},
    {{0xFF, 0x5D, 0xA6, 0xF0, 0xA1, 0x20, 0xC0, 0x54, 0xA5, 0x8C, 0x37, 0x61}},
    {{0xD1, 0xFD, 0x8B, 0x5A, 0x8B, 0xD8, 0x25, 0x5D, 0x89, 0xF9, 0xDB, 0x67}},
    {{0xAA, 0x95, 0xF8, 0xF3, 0x27, 0xBF, 0xA2, 0xC8, 0x5D, 0xDD, 0x80, 0x6E}},
    {{0x4C, 0xC9, 0x9B, 0x97, 0x20, 0x8A, 0x02, 0x52, 0x60, 0xC4, 0x25, 0x75}}
};
constexpr LegacyFloat96 kNegativePowers592738[29] = {
    {{0xCD, 0xCC, 0xCD, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFB, 0x3F}},
    {{0x71, 0x3D, 0x0A, 0xD7, 0xA3, 0x70, 0x3D, 0x0A, 0xD7, 0xA3, 0xF8, 0x3F}},
    {{0x5A, 0x64, 0x3B, 0xDF, 0x4F, 0x8D, 0x97, 0x6E, 0x12, 0x83, 0xF5, 0x3F}},
    {{0xC3, 0xD3, 0x2C, 0x65, 0x19, 0xE2, 0x58, 0x17, 0xB7, 0xD1, 0xF1, 0x3F}},
    {{0xD0, 0x0F, 0x23, 0x84, 0x47, 0x1B, 0x47, 0xAC, 0xC5, 0xA7, 0xEE, 0x3F}},
    {{0x40, 0xA6, 0xB6, 0x69, 0x6C, 0xAF, 0x05, 0xBD, 0x37, 0x86, 0xEB, 0x3F}},
    {{0x33, 0x3D, 0xBC, 0x42, 0x7A, 0xE5, 0xD5, 0x94, 0xBF, 0xD6, 0xE7, 0x3F}},
    {{0xC2, 0xFD, 0xFD, 0xCE, 0x61, 0x84, 0x11, 0x77, 0xCC, 0xAB, 0xE4, 0x3F}},
    {{0x2F, 0x4C, 0x5B, 0xE1, 0x4D, 0xC4, 0xBE, 0x94, 0x95, 0xE6, 0xC9, 0x3F}},
    {{0x92, 0xC4, 0x53, 0x3B, 0x75, 0x44, 0xCD, 0x14, 0xBE, 0x9A, 0xAF, 0x3F}},
    {{0xDE, 0x67, 0xBA, 0x94, 0x39, 0x45, 0xAD, 0x1E, 0xB1, 0xCF, 0x94, 0x3F}},
    {{0x24, 0x23, 0xC6, 0xE2, 0xBC, 0xBA, 0x3B, 0x31, 0x61, 0x8B, 0x7A, 0x3F}},
    {{0x61, 0x55, 0x59, 0xC1, 0x7E, 0xB1, 0x53, 0x7C, 0x12, 0xBB, 0x5F, 0x3F}},
    {{0xD7, 0xEE, 0x2F, 0x8D, 0x06, 0xBE, 0x92, 0x85, 0x15, 0xFB, 0x44, 0x3F}},
    {{0x24, 0x3F, 0xA5, 0xE9, 0x39, 0xA5, 0x27, 0xEA, 0x7F, 0xA8, 0x2A, 0x3F}},
    {{0x7D, 0xAC, 0xA1, 0xE4, 0xBC, 0x64, 0x7C, 0x46, 0xD0, 0xDD, 0x55, 0x3E}},
    {{0x63, 0x7B, 0x06, 0xCC, 0x23, 0x54, 0x77, 0x83, 0xFF, 0x91, 0x81, 0x3D}},
    {{0x91, 0xFA, 0x3A, 0x19, 0x7A, 0x63, 0x25, 0x43, 0x31, 0xC0, 0xAC, 0x3C}},
    {{0x21, 0x89, 0xD1, 0x38, 0x82, 0x47, 0x97, 0xB8, 0x00, 0xFD, 0xD7, 0x3B}},
    {{0xDC, 0x88, 0x58, 0x08, 0x1B, 0xB1, 0xE8, 0xE3, 0x86, 0xA6, 0x03, 0x3B}},
    {{0xC6, 0x84, 0x45, 0x42, 0x07, 0xB6, 0x99, 0x75, 0x37, 0xDB, 0x2E, 0x3A}},
    {{0x33, 0x71, 0x1C, 0xD2, 0x23, 0xDB, 0x32, 0xEE, 0x49, 0x90, 0x5A, 0x39}},
    {{0xA6, 0x87, 0xBE, 0xC0, 0x57, 0xDA, 0xA5, 0x82, 0xA6, 0xA2, 0xB5, 0x32}},
    {{0xE2, 0x68, 0xB2, 0x11, 0xA7, 0x52, 0x9F, 0x44, 0x59, 0xB7, 0x10, 0x2C}},
    {{0x25, 0x49, 0xE4, 0x2D, 0x36, 0x34, 0x4F, 0x53, 0xAE, 0xCE, 0x6B, 0x25}},
    {{0x8F, 0x59, 0x04, 0xA4, 0xC0, 0xDE, 0xC2, 0x7D, 0xFB, 0xE8, 0xC6, 0x1E}},
    {{0x9E, 0xE7, 0x88, 0x5A, 0x57, 0x91, 0x3C, 0xBF, 0x50, 0x83, 0x22, 0x18}},
    {{0x4E, 0x4B, 0x65, 0x62, 0xFD, 0x83, 0x8F, 0xAF, 0x06, 0x94, 0x7D, 0x11}},
    {{0xE4, 0x2D, 0xDE, 0x9F, 0xCE, 0xD2, 0xC8, 0x04, 0xDD, 0xA6, 0xD8, 0x0A}}
};

std::uint16_t LoadFloat16(const void* source)
{
    std::uint16_t value;
    std::memcpy(&value, source, sizeof(value));
    return value;
}

std::uint32_t LoadFloat32(const void* source)
{
    std::uint32_t value;
    std::memcpy(&value, source, sizeof(value));
    return value;
}

void StoreFloat16(void* destination, const std::uint16_t value)
{
    std::memcpy(destination, &value, sizeof(value));
}

void StoreFloat32(void* destination, const std::uint32_t value)
{
    std::memcpy(destination, &value, sizeof(value));
}

} // namespace

std::uint32_t AddLegacyFloatWord_546D95(const std::uint32_t first,
    const std::uint32_t second, std::uint8_t* const output)
{
    const std::uint32_t sum = first + second;
    const std::uint32_t carry = sum < first || sum < second ? 1u : 0u;
    StoreFloat32(output, sum);
    return carry;
}

std::uint32_t AddLegacyFloat96_546DB6(
    LegacyFloat96* const destination, const LegacyFloat96* const source)
{
    auto* const output = destination->bytes;
    const auto* const input = source->bytes;
    if (AddLegacyFloatWord_546D95(LoadFloat32(output), LoadFloat32(input), output) != 0) {
        if (AddLegacyFloatWord_546D95(LoadFloat32(output + 4), 1, output + 4) != 0) {
            StoreFloat32(output + 8, LoadFloat32(output + 8) + 1u);
        }
    }
    // 源与目标可同址；中/高字必须在前一阶段写入后重新读取。
    if (AddLegacyFloatWord_546D95(
            LoadFloat32(output + 4), LoadFloat32(input + 4), output + 4) != 0) {
        StoreFloat32(output + 8, LoadFloat32(output + 8) + 1u);
    }
    return AddLegacyFloatWord_546D95(
        LoadFloat32(output + 8), LoadFloat32(input + 8), output + 8);
}

LegacyFloat96* ShiftLegacyFloat96Left_546E14(LegacyFloat96* const value)
{
    auto* const bytes = value->bytes;
    const std::uint32_t low = LoadFloat32(bytes);
    const std::uint32_t middle = LoadFloat32(bytes + 4);
    StoreFloat32(bytes, low << 1);
    const std::uint32_t high = LoadFloat32(bytes + 8);
    StoreFloat32(bytes + 4, (middle << 1) | (low >> 31));
    StoreFloat32(bytes + 8, (high << 1) | (middle >> 31));
    return value;
}

LegacyFloat96* ShiftLegacyFloat96Right_546E42(LegacyFloat96* const value)
{
    auto* const bytes = value->bytes;
    const std::uint32_t high = LoadFloat32(bytes + 8);
    const std::uint32_t middle = LoadFloat32(bytes + 4);
    StoreFloat32(bytes + 4, (middle >> 1) | (high << 31));
    const std::uint32_t low = LoadFloat32(bytes);
    StoreFloat32(bytes + 8, high >> 1);
    StoreFloat32(bytes, (low >> 1) | (middle << 31));
    return value;
}

void MultiplyLegacyFloat96_547971(
    LegacyFloat96* const destination, const LegacyFloat96* const source)
{
    auto* const left = destination->bytes;
    const auto* const right = source->bytes;
    const std::uint16_t rightSignExponent = LoadFloat16(right + 10);
    LegacyFloat96 product {};
    const std::uint16_t leftSignExponent = LoadFloat16(left + 10);
    const std::uint32_t sign = (leftSignExponent ^ rightSignExponent) & 0x8000u;
    const std::uint32_t leftExponent = leftSignExponent & 0x7FFFu;
    const std::uint32_t rightExponent = rightSignExponent & 0x7FFFu;
    std::uint32_t exponent = leftExponent + rightExponent;
    if (leftExponent >= 0x7FFF || rightExponent >= 0x7FFF
        || static_cast<std::uint16_t>(exponent) > 0xBFFD) {
        StoreFloat32(left + 4, 0);
        StoreFloat32(left, 0);
        StoreFloat32(left + 8, (sign != 0 ? 0x80000000u : 0u) + 0x7FFF8000u);
        return;
    }
    if (static_cast<std::uint16_t>(exponent) <= 0x3FBF) {
        StoreFloat32(left + 8, 0);
        StoreFloat32(left + 4, 0);
        StoreFloat32(left, 0);
        return;
    }
    if (leftExponent == 0) {
        ++exponent;
        if ((LoadFloat32(left + 8) & 0x7FFFFFFFu) == 0
            && LoadFloat32(left + 4) == 0 && LoadFloat32(left) == 0) {
            StoreFloat16(left + 10, 0);
            return;
        }
    }
    if (rightExponent == 0) {
        ++exponent;
        if ((LoadFloat32(right + 8) & 0x7FFFFFFFu) == 0
            && LoadFloat32(right + 4) == 0 && LoadFloat32(right) == 0) {
            StoreFloat32(left + 8, 0);
            StoreFloat32(left + 4, 0);
            StoreFloat32(left, 0);
            return;
        }
    }
    for (std::uint32_t row = 0; row < 5; ++row) {
        auto* const sum = product.bytes + row * 2;
        for (std::uint32_t column = 0; column < 5 - row; ++column) {
            const std::uint32_t term = static_cast<std::uint32_t>(
                LoadFloat16(left + (row + column) * 2)) * LoadFloat16(right + 8 - column * 2);
            if (AddLegacyFloatWord_546D95(LoadFloat32(sum), term, sum) != 0) {
                StoreFloat16(sum + 4, static_cast<std::uint16_t>(LoadFloat16(sum + 4) + 1u));
            }
        }
    }
    exponent += 0xC002u;
    while (std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(exponent)) > 0
        && (product.bytes[11] & 0x80u) == 0) {
        ShiftLegacyFloat96Left_546E14(&product);
        exponent += 0xFFFFu;
    }
    if (std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(exponent)) <= 0) {
        exponent += 0xFFFFu;
        const auto signedExponent =
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(exponent));
        if (signedExponent < 0) {
            std::int32_t shifts = -static_cast<std::int32_t>(signedExponent);
            exponent += static_cast<std::uint32_t>(shifts);
            std::uint32_t discarded = 0;
            do {
                if ((product.bytes[0] & 1u) != 0) {
                    ++discarded;
                }
                ShiftLegacyFloat96Right_546E42(&product);
            } while (--shifts != 0);
            if (discarded != 0) {
                product.bytes[0] |= 1u;
            }
        }
    }
    if (LoadFloat16(product.bytes) > 0x8000u
        || (LoadFloat32(product.bytes) & 0x1FFFFu) == 0x18000u) {
        if (LoadFloat32(product.bytes + 2) != 0xFFFFFFFFu) {
            StoreFloat32(product.bytes + 2, LoadFloat32(product.bytes + 2) + 1u);
        } else {
            StoreFloat32(product.bytes + 2, 0);
            if (LoadFloat32(product.bytes + 6) != 0xFFFFFFFFu) {
                StoreFloat32(product.bytes + 6, LoadFloat32(product.bytes + 6) + 1u);
            } else {
                StoreFloat32(product.bytes + 6, 0);
                if (LoadFloat16(product.bytes + 10) != 0xFFFFu) {
                    StoreFloat16(product.bytes + 10,
                        static_cast<std::uint16_t>(LoadFloat16(product.bytes + 10) + 1u));
                } else {
                    ++exponent;
                    StoreFloat16(product.bytes + 10, 0x8000);
                }
            }
        }
    }
    if (static_cast<std::uint16_t>(exponent) >= 0x7FFFu) {
        StoreFloat32(left + 4, 0);
        StoreFloat32(left, 0);
        StoreFloat32(left + 8, (sign != 0 ? 0x80000000u : 0u) + 0x7FFF8000u);
        return;
    }
    StoreFloat16(left, LoadFloat16(product.bytes + 2));
    StoreFloat32(left + 2, LoadFloat32(product.bytes + 4));
    StoreFloat32(left + 6, LoadFloat32(product.bytes + 8));
    StoreFloat16(left + 10, static_cast<std::uint16_t>(exponent | sign));
}

void ScaleLegacyFloat96_547B91(LegacyFloat96* const value,
    std::int32_t power, const std::int32_t preserveExtra)
{
    if (power == 0) {
        return;
    }
    const LegacyFloat96* table = kPositivePowers5925D8;
    if (power < 0) {
        table = kNegativePowers592738;
        power = -power;
    }
    if (preserveExtra == 0) {
        StoreFloat16(value->bytes, 0);
    }
    std::uint32_t group = 0;
    do {
        const std::uint32_t digit = static_cast<std::uint32_t>(power) & 7u;
        power >>= 3;
        if (digit != 0) {
            const LegacyFloat96* factor = &table[group * 7 + digit - 1];
            LegacyFloat96 adjusted;
            if (LoadFloat16(factor->bytes) >= 0x8000u) {
                adjusted = *factor;
                StoreFloat32(adjusted.bytes + 2, LoadFloat32(adjusted.bytes + 2) - 1u);
                factor = &adjusted;
            }
            MultiplyLegacyFloat96_547971(value, factor);
        }
        ++group;
    } while (power != 0);
}

void ConvertLegacyDoubleTo80_544F7B(
    LegacyFloat80* const output, const double* const source)
{
    const auto* const bytes = reinterpret_cast<const std::uint8_t*>(source);
    const std::uint16_t highWord = LoadFloat16(bytes + 6);
    const std::uint32_t sign = highWord & 0x8000u;
    const std::uint32_t originalExponent = (highWord >> 4) & 0x7FFu;
    std::uint32_t high = LoadFloat32(bytes + 4) & 0xFFFFFu;
    std::uint32_t low = LoadFloat32(bytes);
    std::uint32_t exponent;
    std::uint32_t integerBit = 0x80000000u;
    if (originalExponent == 0) {
        if (high == 0 && low == 0) {
            StoreFloat32(output->bytes + 4, 0);
            StoreFloat32(output->bytes, 0);
            StoreFloat16(output->bytes + 8, 0);
            return;
        }
        exponent = originalExponent + 0x3C01u;
        integerBit = 0;
    } else {
        exponent = originalExponent == 0x7FFu ? 0x7FFFu : originalExponent + 0x3C00u;
    }
    high = (low >> 21) | (high << 11) | integerBit;
    low <<= 11;
    StoreFloat32(output->bytes + 4, high);
    StoreFloat32(output->bytes, low);
    while ((high & 0x80000000u) == 0) {
        low = LoadFloat32(output->bytes);
        high = (high << 1) | (low >> 31);
        StoreFloat32(output->bytes, low << 1);
        StoreFloat32(output->bytes + 4, high);
        exponent += 0xFFFFu;
    }
    StoreFloat16(output->bytes + 8, static_cast<std::uint16_t>(exponent | sign));
}

std::int32_t GenerateLegacyFloatDecimal_546F36(const LegacyFloat80 value,
    std::int32_t precision, const std::uint32_t flags, LegacyFloatDecimalText* const output)
{
    const std::uint16_t signExponent = LoadFloat16(value.bytes + 8);
    const std::uint32_t exponent = signExponent & 0x7FFFu;
    const bool negative = (signExponent & 0x8000u) != 0;
    output->sign = negative ? '-' : ' ';
    const std::uint32_t high = LoadFloat32(value.bytes + 4);
    const std::uint32_t low = LoadFloat32(value.bytes);
    const auto writeZero = [output]() -> std::int32_t {
        output->decimal = 0;
        output->sign = ' ';
        output->count = 1;
        output->digits[0] = '0';
        output->digits[1] = '\0';
        return 1;
    };
    if (exponent == 0 && high == 0 && low == 0) {
        return writeZero();
    }
    if (exponent == 0x7FFFu) {
        output->decimal = 1;
        const char* text;
        std::uint8_t count;
        if ((high != 0x80000000u || low != 0) && (high & 0x40000000u) == 0) {
            text = "1#SNAN";
            count = 6;
        } else if (negative && high == 0xC0000000u && low == 0) {
            text = "1#IND";
            count = 5;
        } else if (high == 0x80000000u && low == 0) {
            text = "1#INF";
            count = 5;
        } else {
            text = "1#QNAN";
            count = 6;
        }
        std::strcpy(output->digits, text);
        output->count = count;
        return 0;
    }

    const std::uint32_t estimateBits = exponent * 0x4D10u
        + ((exponent >> 8) + (high >> 24) * 2u) * 0x4Du - 0x134312F4u;
    std::int32_t decimal = std::bit_cast<std::int32_t>(estimateBits) >> 16;
    LegacyFloat96 scaled;
    StoreFloat16(scaled.bytes, 0);
    StoreFloat16(scaled.bytes + 10, static_cast<std::uint16_t>(exponent));
    StoreFloat32(scaled.bytes + 6, high);
    StoreFloat32(scaled.bytes + 2, low);
    ScaleLegacyFloat96_547B91(&scaled,
        -static_cast<std::int32_t>(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(decimal))), 1);
    if (LoadFloat16(scaled.bytes + 10) >= 0x3FFFu) {
        const LegacyFloat96 tenth {{
            0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
            0xCC, 0xCC, 0xCC, 0xCC, 0xFB, 0x3F
        }};
        ++decimal;
        MultiplyLegacyFloat96_547971(&scaled, &tenth);
    }
    output->decimal = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(decimal));
    if ((flags & 1u) != 0) {
        precision += static_cast<std::int32_t>(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(decimal)));
        if (precision <= 0) {
            return writeZero();
        }
    }
    if (precision > 21) {
        precision = 21;
    }
    std::int32_t shifts = static_cast<std::int32_t>(LoadFloat16(scaled.bytes + 10)) - 0x3FFE;
    StoreFloat16(scaled.bytes + 10, 0);
    for (std::int32_t index = 0; index < 8; ++index) {
        ShiftLegacyFloat96Left_546E14(&scaled);
    }
    if (shifts < 0) {
        shifts = (-shifts) & 0xFF;
        while (shifts > 0) {
            ShiftLegacyFloat96Right_546E42(&scaled);
            --shifts;
        }
    }

    auto* const bytes = reinterpret_cast<std::uint8_t*>(output);
    std::int32_t cursor = 4;
    for (std::int32_t remaining = precision + 1; remaining > 0; --remaining) {
        // 原参数槽在此复用为12B副本，只用于本位的乘十运算。
        const LegacyFloat96 saved = scaled;
        ShiftLegacyFloat96Left_546E14(&scaled);
        ShiftLegacyFloat96Left_546E14(&scaled);
        static_cast<void>(AddLegacyFloat96_546DB6(&scaled, &saved));
        ShiftLegacyFloat96Left_546E14(&scaled);
        const std::uint8_t digit = scaled.bytes[11];
        scaled.bytes[11] = 0;
        bytes[cursor++] = static_cast<std::uint8_t>(digit + '0');
    }
    const auto rounding = std::bit_cast<std::int8_t>(bytes[cursor - 1]);
    cursor -= 2;
    if (rounding >= '5') {
        while (cursor >= 4 && bytes[cursor] == '9') {
            bytes[cursor--] = '0';
        }
        if (cursor < 4) {
            ++cursor;
            StoreFloat16(bytes, static_cast<std::uint16_t>(LoadFloat16(bytes) + 1u));
        }
        ++bytes[cursor];
    } else {
        while (cursor >= 4 && bytes[cursor] == '0') {
            --cursor;
        }
        if (cursor < 4) {
            return writeZero();
        }
    }
    // 原AL只保留相对地址差低U8，再按S8扩展定位终止字节。
    output->count = static_cast<std::uint8_t>(cursor - 3);
    bytes[4 + std::bit_cast<std::int8_t>(output->count)] = 0;
    return 1;
}

} // namespace cg::platform
