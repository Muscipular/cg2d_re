#include "cg/platform/LegacyStringCompare.hpp"

#include "LegacyStringCompareState.hpp"
#include "LegacyMultibyte.hpp"

#include <bit>
#include <malloc.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::platform {

LegacyTextLocaleStorage gLegacyTextLocale1086160 {};
std::uint32_t& gConversionCodePage_1086170 =
    gLegacyTextLocale1086160.conversionCodePage;
char gNlsProbeD68A7C = 0;

// 原 58FC38 的 EOF 槽及 ASCII 分类；其余 128 项原字节均为零。
std::array<std::uint16_t, 257> gCtypeTable58FC38 {
    0x0000, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0028, 0x0028, 0x0028, 0x0028, 0x0028, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0048, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0084, 0x0084, 0x0084, 0x0084, 0x0084, 0x0084, 0x0084,
    0x0084, 0x0084, 0x0084, 0x0010, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081, 0x0081,
    0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
    0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
    0x0001, 0x0001, 0x0001, 0x0001, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0010, 0x0010, 0x0082, 0x0082, 0x0082, 0x0082, 0x0082, 0x0082,
    0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
    0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
    0x0002, 0x0002, 0x0002, 0x0002, 0x0010, 0x0010, 0x0010, 0x0010,
    0x0020
};
std::uint16_t* gCtypePointer58FC30 = gCtypeTable58FC38.data() + 1;
static_assert(sizeof(gCtypePointer58FC30) == 8);

// 原[543D0C,543D37)：包含数量耗尽/零数量时的末端读取，不是宿主strnlen。
std::uint32_t CountLegacyBytes_543D0C(const char* const source, const std::uint32_t limit)
{
    const volatile char* cursor = source;
    std::uint32_t remaining = limit;
    if (remaining != 0) {
        do {
            if (detail::Cg2dReadLegacyCountByte(cursor) == 0) {
                break;
            }
            ++cursor;
            --remaining;
        } while (remaining != 0);
    }
    return detail::Cg2dReadLegacyCountByte(cursor) == 0 ? static_cast<std::uint32_t>(cursor - source) : limit;
}

// 53DC64/543A1A调用域：LOWERCASE/UPPERCASE、目的长3、代码页参数0、strict=1。
std::int32_t MapLegacyCase_543AE8(const std::uint32_t locale, const std::uint32_t flags,
    const char* source, const int sourceSize, char* destination)
{
    return MapLegacyString_543AE8(
        locale, flags, source, sourceSize, destination, 3, 0, 1);
}

namespace {

// 本比较器输入仅 0..65535；小于 256 时原 isctype 同样直接读取此表。
std::uint32_t LowerCompareValue_53DC64(const std::uint32_t value)
{
    if (gLegacyTextLocale1086160.ctypeLocale == 0) {
        return value >= 'A' && value <= 'Z' ? value + 32u : value;
    }
    if (value < 256 && (gCtypePointer58FC30[value] & 1u) == 0) {
        return value;
    }
    const auto highByte = static_cast<std::uint8_t>(value >> 8);
    const auto lowByte = static_cast<std::uint8_t>(value);
    char source[3];
    int sourceSize;
    if ((gCtypePointer58FC30[highByte] & 0x8000u) != 0) {
        source[0] = std::bit_cast<char>(highByte);
        source[1] = std::bit_cast<char>(lowByte);
        source[2] = '\0';
        sourceSize = 2;
    } else {
        source[0] = std::bit_cast<char>(lowByte);
        source[1] = '\0';
        sourceSize = 1;
    }
    char destination[4];
    const int result = MapLegacyCase_543AE8(
        gLegacyTextLocale1086160.ctypeLocale, LCMAP_LOWERCASE,
        source, sourceSize, destination);
    if (result == 0) {
        return value;
    }
    const std::uint32_t first = static_cast<std::uint8_t>(destination[0]);
    return result == 1 ? first
        : first | (static_cast<std::uint32_t>(
            static_cast<std::uint8_t>(destination[1])) << 8);
}

} // namespace

std::uint16_t ClassifyLegacyByte(const std::uint8_t value, const std::uint16_t mask)
{
    return gCtypePointer58FC30[value] & mask;
}

std::int32_t CompareInsensitive_53CF50(const char* left, const char* right)
{
    if (gLegacyTextLocale1086160.ctypeLocale == 0) {
        for (;;) {
            auto rhs = static_cast<std::uint8_t>(*right++);
            auto lhs = static_cast<std::uint8_t>(*left++);
            if (lhs != rhs) {
                if (rhs >= 'A' && rhs <= 'Z') {
                    rhs += 32;
                }
                if (lhs >= 'A' && lhs <= 'Z') {
                    lhs += 32;
                }
                if (lhs != rhs) {
                    return lhs < rhs ? -1 : 1;
                }
            }
            if (rhs == 0) {
                return 0;
            }
        }
    }

    std::uint32_t rhs = 0xFF;
    std::uint32_t lhs = 0;
    while (static_cast<std::uint8_t>(rhs) != 0) {
        rhs = (rhs & 0xFFFFFF00u) | static_cast<std::uint8_t>(*right++);
        lhs = (lhs & 0xFFFFFF00u) | static_cast<std::uint8_t>(*left++);
        if (static_cast<std::uint8_t>(rhs) == static_cast<std::uint8_t>(lhs)) {
            continue;
        }
        lhs = LowerCompareValue_53DC64(lhs);
        rhs = LowerCompareValue_53DC64(rhs);
        if (static_cast<std::uint8_t>(lhs) != static_cast<std::uint8_t>(rhs)) {
            return static_cast<std::uint8_t>(lhs) < static_cast<std::uint8_t>(rhs) ? -1 : 1;
        }
    }
    return static_cast<std::int32_t>(rhs);
}

std::int32_t CompareInsensitiveN_54AB30(
    const char* left, const char* right, std::uint32_t count)
{
    if (count == 0) {
        return 0;
    }
    if (gLegacyTextLocale1086160.ctypeLocale == 0) {
        for (;;) {
            auto lhs = static_cast<std::uint8_t>(*left);
            auto rhs = static_cast<std::uint8_t>(*right);
            if (lhs == 0 || rhs == 0) {
                return lhs == rhs ? 0 : (lhs < rhs ? -1 : 1);
            }
            ++left;
            ++right;
            if (lhs >= 'A' && lhs <= 'Z') {
                lhs += 32;
            }
            if (rhs >= 'A' && rhs <= 'Z') {
                rhs += 32;
            }
            if (lhs != rhs) {
                return lhs < rhs ? -1 : 1;
            }
            if (--count == 0) {
                return 0;
            }
        }
    }

    // 原每轮只写AL/BL，高位来自上轮转换；终止和比较均使用完整U32。
    std::uint32_t lhs = 0;
    std::uint32_t rhs = 0;
    for (;;) {
        lhs = (lhs & 0xFFFFFF00u) | static_cast<std::uint8_t>(*left);
        rhs = (rhs & 0xFFFFFF00u) | static_cast<std::uint8_t>(*right);
        if (lhs == 0 || rhs == 0) {
            return lhs == rhs ? 0 : (lhs < rhs ? -1 : 1);
        }
        ++left;
        ++right;
        rhs = LowerCompareValue_53DC64(rhs);
        lhs = LowerCompareValue_53DC64(lhs);
        if (lhs != rhs) {
            return lhs < rhs ? -1 : 1;
        }
        if (--count == 0) {
            return 0;
        }
    }
}

} // namespace cg::platform
