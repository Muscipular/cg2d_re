#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyStringCompare.hpp"
#include "StartupNative.hpp"
#include "LegacyStringCompareState.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyLowIo.hpp"
#include "LegacyStdio.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace cg::platform {

// 原缓存分别位于 0x1085FD0、0x1085FC8；这里只还原各已访问对象，不推断中间填充用途。
SYSTEMTIME gUtcTimeCache_1085FD0 {};
std::int32_t gDaylightCache_1085FC8 = 0;

// 原 CRT 错误槽；文件时间、目录创建及截图共用，不绑定现代 CRT errno。
std::int32_t gLegacyErrno_1085FE0 = 0;
std::uint32_t gLegacyDosError_1085FE4 = 0;

static_assert(offsetof(LegacyTm53B8C6, seconds) == 0x00);
static_assert(offsetof(LegacyTm53B8C6, minutes) == 0x04);
static_assert(offsetof(LegacyTm53B8C6, hours) == 0x08);
static_assert(offsetof(LegacyTm53B8C6, day) == 0x0C);
static_assert(offsetof(LegacyTm53B8C6, month0based) == 0x10);
static_assert(offsetof(LegacyTm53B8C6, yearMinus1900) == 0x14);
static_assert(offsetof(LegacyTm53B8C6, weekDay) == 0x18);
static_assert(offsetof(LegacyTm53B8C6, yearDay0based) == 0x1C);
static_assert(offsetof(LegacyTm53B8C6, dst) == 0x20);
static_assert(sizeof(LegacyTm53B8C6) == 0x24);

// 按用户裁决使用标准库环境入口，返回由标准库持有的借用指针。
char* __cdecl GetEnv_54518B(const char* name)
{
    return std::getenv(name);
}

// Atol、Malloc、Free三个入口按用户裁决使用现代标准库。
std::int32_t __cdecl Atol_53C5BE(const char* text)
{
    return static_cast<std::int32_t>(std::atol(text));
}

// 原53DA5C；指针仅表达原字节游标，数值和状态标志保持W32。
static std::uint32_t StrToXl_53DA5C(const char* const text, char** const endPointer,
    std::int32_t radix, std::uint32_t flags)
{
    enum : std::uint32_t {
        unsignedResult = 1u,
        negative = 2u,
        overflow = 4u,
        digitsSeen = 8u
    };
    std::uint32_t value = 0;
    const char* cursor = text;
    auto current = static_cast<std::uint8_t>(*cursor++);
    // 此处参数始终U8，原MB_CUR_MAX两支最终都读取同一ctype槽。
    while (ClassifyLegacyByte(current, 8) != 0) {
        current = static_cast<std::uint8_t>(*cursor++);
    }
    if (current == '-') {
        flags |= negative;
        current = static_cast<std::uint8_t>(*cursor++);
    } else if (current == '+') {
        current = static_cast<std::uint8_t>(*cursor++);
    }
    if (radix < 0 || radix == 1 || radix > 36) {
        if (endPointer != nullptr) {
            *endPointer = const_cast<char*>(text);
        }
        return 0;
    }
    if (radix == 0) {
        if (current != '0') {
            radix = 10;
        } else {
            const char nextByte = *cursor;
            radix = nextByte == 'x' || nextByte == 'X' ? 16 : 8;
        }
    }
    if (radix == 16 && current == '0') {
        const char nextByte = *cursor;
        if (nextByte == 'x' || nextByte == 'X') {
            current = static_cast<std::uint8_t>(cursor[1]);
            cursor += 2;
        }
    }
    const auto base = static_cast<std::uint32_t>(radix);
    const std::uint32_t cutoff = 0xFFFFFFFFu / base;
    for (;;) {
        std::uint32_t digit;
        if (ClassifyLegacyByte(current, 4) != 0) {
            digit = static_cast<std::uint32_t>(
                static_cast<std::int32_t>(std::bit_cast<std::int8_t>(current)) - '0');
        } else if (ClassifyLegacyByte(current, 0x103) != 0) {
            digit = static_cast<std::uint32_t>(
                UpperLegacyCharacter_543A1A(std::bit_cast<std::int8_t>(current)) - '7');
        } else {
            break;
        }
        if (digit >= base) {
            break;
        }
        flags |= digitsSeen;
        if (value < cutoff || (value == cutoff && digit <= 0xFFFFFFFFu % base)) {
            value = value * base + digit;
        } else {
            flags |= overflow;
        }
        current = static_cast<std::uint8_t>(*cursor++);
    }
    const char* end = cursor - 1;
    if ((flags & digitsSeen) == 0) {
        if (endPointer != nullptr) {
            end = text;
        }
        value = 0;
    } else if ((flags & overflow) != 0
        || ((flags & unsignedResult) == 0
            && value > ((flags & negative) != 0 ? 0x80000000u : 0x7FFFFFFFu))) {
        gLegacyErrno_1085FE0 = 34;
        value = (flags & unsignedResult) != 0 ? 0xFFFFFFFFu
            : ((flags & negative) != 0 ? 0x80000000u : 0x7FFFFFFFu);
    }
    if (endPointer != nullptr) {
        *endPointer = const_cast<char*>(end);
    }
    return (flags & negative) != 0 ? 0u - value : value;
}

// 原53DA45只把flags=0交53DA5C；返回有符号32位位型。
std::int32_t Strtol_53DA45(
    const char* const text, char** const endPointer, const std::int32_t radix)
{
    return std::bit_cast<std::int32_t>(StrToXl_53DA5C(text, endPointer, radix, 0));
}

// 原[53C649,53C654)：只转调53C5BE，沿用该入口已批准的std替代边界。
std::int32_t Atoi_53C649(const char* text)
{
    return Atol_53C5BE(text);
}

void* __cdecl Malloc_53C3AC(std::uint32_t size)
{
    return std::malloc(static_cast<std::size_t>(size));
}

void __cdecl Free_53C0A9(void* memory)
{
    std::free(memory);
}

// 原[54AC0A,54AC66)：U32除法产生低U8数字，先写NUL再在目标内反转数字区。
static void Xtoa_54AC0A(
    std::uint32_t value,
    char* const buffer,
    const std::uint32_t radix,
    const std::int32_t negative)
{
    char* cursor = buffer;
    if (negative != 0) {
        *cursor++ = '-';
        value = 0u - value;
    }

    char* const digits = cursor;
    do {
        const std::uint32_t digit = value % radix;
        value /= radix;
        const std::uint32_t byte = digit > 9u ? digit + 0x57u : digit + 0x30u;
        *cursor++ = std::bit_cast<char>(static_cast<std::uint8_t>(byte));
    } while (value != 0);
    *cursor = '\0';

    std::int32_t first = 0;
    std::int32_t last = static_cast<std::int32_t>(cursor - digits) - 1;
    do {
        const char firstByte = digits[first];
        const char lastByte = digits[last];
        digits[last] = firstByte;
        digits[first] = lastByte;
        --last;
        ++first;
    } while (first < last);
}

// 原54ABDD：只有十进制负S32添加负号，其他进制按原W32位模式转换。
char* Itoa_54ABDD(const std::int32_t value, char* const buffer, const std::int32_t radix)
{
    Xtoa_54AC0A(
        std::bit_cast<std::uint32_t>(value),
        buffer,
        std::bit_cast<std::uint32_t>(radix),
        radix == 10 && value < 0 ? 1 : 0);
    return buffer;
}

// 原108602C：StrTok_53D26E独占的进程共享续读位置，不拥有输入文本。
static char* gLegacyTokenCursor_108602C = nullptr;

// 原[53D26E,53D30A)：先建完整U8分隔位图，再就地截断并发布借用续读指针。
char* StrTok_53D26E(char* text, const char* delimiters)
{
    std::uint8_t delimiterBits[32] {};
    std::uint8_t value;
    do {
        value = static_cast<std::uint8_t>(*delimiters++);
        delimiterBits[value >> 3] |= static_cast<std::uint8_t>(1u << (value & 7u));
    } while (value != 0);

    char* cursor = text != nullptr ? text : gLegacyTokenCursor_108602C;
    for (;;) {
        value = static_cast<std::uint8_t>(*cursor);
        if ((delimiterBits[value >> 3] & (1u << (value & 7u))) == 0 || value == 0) {
            break;
        }
        ++cursor;
    }

    char* const token = cursor;
    for (;;) {
        value = static_cast<std::uint8_t>(*cursor);
        if (value == 0) {
            break;
        }
        if ((delimiterBits[value >> 3] & (1u << (value & 7u))) != 0) {
            *cursor++ = '\0';
            break;
        }
        ++cursor;
    }

    char* const result = token == cursor ? nullptr : token;
    gLegacyTokenCursor_108602C = cursor;
    return result;
}

// 原 [0x5450F0,0x545174)：左侧按 1/2/4 字节对齐读取，右侧逐字节比较。
std::int32_t __cdecl StrCmp_5450F0(const char* left, const char* right)
{
    const auto* lhs = reinterpret_cast<const unsigned char*>(left);
    const auto* rhs = reinterpret_cast<const unsigned char*>(right);
    for (;;) {
        const std::uintptr_t alignment = reinterpret_cast<std::uintptr_t>(lhs) & 3u;
        const std::uint32_t width = (alignment & 1u) != 0 ? 1u
            : alignment != 0 ? 2u : 4u;
        std::uint32_t block = 0;
        std::memcpy(&block, lhs, width);
        for (std::uint32_t index = 0; index < width; ++index) {
            const unsigned char value =
                static_cast<unsigned char>((block >> (index * 8)) & 0xFFu);
            const unsigned char other = rhs[index];
            if (value != other) {
                return value < other ? -1 : 1;
            }
            if (value == 0) {
                return 0;
            }
        }
        lhs += width;
        rhs += width;
    }
}

// 原 [0x53F9E0,0x53FA5B)：候选块复读后才定位首个 NUL。
std::uint32_t __cdecl StrLen_53F9E0(const char* text)
{
    const auto* start = reinterpret_cast<const unsigned char*>(text);
    const auto* cursor = start;
    while ((reinterpret_cast<std::uintptr_t>(cursor) & 3u) != 0) {
        const unsigned char value = *cursor++;
        if (value == 0) {
            return static_cast<std::uint32_t>(cursor - start - 1);
        }
    }

    for (;;) {
        std::uint32_t probe;
        std::memcpy(&probe, cursor, sizeof(probe));
        cursor += 4;
        if (((probe + 0x7EFEFEFFu) ^ ~probe) & 0x81010100u) {
            std::uint32_t block;
            std::memcpy(&block, cursor - 4, sizeof(block));
            for (std::uint32_t index = 0; index < 4; ++index) {
                if (((block >> (index * 8)) & 0xFFu) == 0) {
                    return static_cast<std::uint32_t>(cursor - start - 4 + index);
                }
            }
        }
    }
}

// 原 0x544DB0 直接进入共享复制主体，不执行 0x544DC0 的入口。
char* __cdecl StrCpy_544DB0(char* target, const char* source)
{
    auto* destination = reinterpret_cast<unsigned char*>(target);
    const auto* cursor = reinterpret_cast<const unsigned char*>(source);
    while ((reinterpret_cast<std::uintptr_t>(cursor) & 3u) != 0) {
        const unsigned char value = *cursor;
        if (value == 0) {
            *destination = 0;
            return target;
        }
        *destination++ = value;
        ++cursor;
    }

    for (;;) {
        std::uint32_t probe;
        std::memcpy(&probe, cursor, sizeof(probe));
        std::uint32_t block;
        std::memcpy(&block, cursor, sizeof(block));
        cursor += 4;
        if (((probe + 0x7EFEFEFFu) ^ ~probe) & 0x81010100u) {
            for (std::uint32_t index = 0; index < 4; ++index) {
                if (((block >> (index * 8)) & 0xFFu) != 0) {
                    continue;
                }
                if (index == 0) {
                    *destination = 0;
                } else if (index == 1) {
                    const std::uint16_t prefix = static_cast<std::uint16_t>(block);
                    std::memcpy(destination, &prefix, sizeof(prefix));
                } else if (index == 2) {
                    const std::uint16_t prefix = static_cast<std::uint16_t>(block);
                    std::memcpy(destination, &prefix, sizeof(prefix));
                    char* result = target;
                    destination[2] = 0;
                    return result;
                } else {
                    std::memcpy(destination, &block, sizeof(block));
                }
                return target;
            }
        }
        std::memcpy(destination, &block, sizeof(block));
        destination += 4;
    }
}

// 原544DC0：先找目的末NUL，再共用544E21复制尾；返回原target。
char* __cdecl StrCat_544DC0(char* target, const char* source)
{
    auto* cursor = reinterpret_cast<unsigned char*>(target);
    while ((reinterpret_cast<std::uintptr_t>(cursor) & 3u) != 0) {
        const unsigned char value = *cursor++;
        if (value == 0) {
            StrCpy_544DB0(reinterpret_cast<char*>(cursor - 1), source);
            return target;
        }
    }

    for (;;) {
        std::uint32_t probe;
        std::memcpy(&probe, cursor, sizeof(probe));
        cursor += 4;
        if (((probe + 0x7EFEFEFFu) ^ ~probe) & 0x81010100u) {
            std::uint32_t block;
            std::memcpy(&block, cursor - 4, sizeof(block));
            for (std::uint32_t index = 0; index < 4; ++index) {
                if (((block >> (index * 8)) & 0xFFu) == 0) {
                    StrCpy_544DB0(
                        reinterpret_cast<char*>(cursor - 4 + index), source);
                    return target;
                }
            }
        }
    }
}

// 原53C790：U32限长追加，保留目的候选复读、源分块读写及原target返回。
char* __cdecl StrNCat_53C790(char* target, const char* source, std::uint32_t count)
{
    if (count == 0) {
        return target;
    }

    auto* destination = reinterpret_cast<unsigned char*>(target);
    bool foundEnd = false;
    while ((reinterpret_cast<std::uintptr_t>(destination) & 3u) != 0) {
        const unsigned char value = *destination++;
        if (value == 0) {
            --destination;
            foundEnd = true;
            break;
        }
    }
    while (!foundEnd) {
        std::uint32_t probe;
        std::memcpy(&probe, destination, sizeof(probe));
        destination += 4;
        if (((probe + 0x7EFEFEFFu) ^ ~probe) & 0x81010100u) {
            std::uint32_t block;
            std::memcpy(&block, destination - 4, sizeof(block));
            for (std::uint32_t index = 0; index < 4; ++index) {
                if (((block >> (index * 8)) & 0xFFu) == 0) {
                    destination = destination - 4 + index;
                    foundEnd = true;
                    break;
                }
            }
        }
    }

    const auto* cursor = reinterpret_cast<const unsigned char*>(source);
    while ((reinterpret_cast<std::uintptr_t>(cursor) & 3u) != 0) {
        const unsigned char value = *cursor++;
        if (value == 0) {
            *destination = 0;
            return target;
        }
        *destination++ = value;
        if (--count == 0) {
            *destination = 0;
            return target;
        }
    }

    std::uint32_t blockCount = count >> 2;
    std::uint32_t tailCount = count & 3u;
    while (blockCount != 0) {
        std::uint32_t probe;
        std::memcpy(&probe, cursor, sizeof(probe));
        std::uint32_t block;
        std::memcpy(&block, cursor, sizeof(block));
        cursor += 4;
        if (((probe + 0x7EFEFEFFu) ^ ~probe) & 0x81010100u) {
            for (std::uint32_t index = 0; index < 4; ++index) {
                if (((block >> (index * 8)) & 0xFFu) != 0) {
                    continue;
                }
                if (index == 0) {
                    *destination = 0;
                } else if (index == 1) {
                    const std::uint16_t prefix = static_cast<std::uint16_t>(block);
                    std::memcpy(destination, &prefix, sizeof(prefix));
                } else if (index == 2) {
                    const std::uint16_t prefix = static_cast<std::uint16_t>(block);
                    std::memcpy(destination, &prefix, sizeof(prefix));
                    destination[2] = 0;
                } else {
                    std::memcpy(destination, &block, sizeof(block));
                }
                return target;
            }
        }
        std::memcpy(destination, &block, sizeof(block));
        destination += 4;
        --blockCount;
    }

    while (tailCount != 0) {
        const unsigned char value = *cursor++;
        *destination++ = value;
        if (value == 0) {
            return target;
        }
        --tailCount;
    }
    *destination = 0;
    return target;
}

// 原 [0x53BEC0,0x53BFBE)：保留源对齐块读取与遇 NUL 后的分段补零。
char* __cdecl StrNCpy_53BEC0(char* target, const char* source, std::uint32_t count)
{
    if (count == 0) {
        return target;
    }

    auto* destination = reinterpret_cast<unsigned char*>(target);
    const auto* cursor = reinterpret_cast<const unsigned char*>(source);
    std::uint32_t remaining = count;
    const std::uint32_t zero = 0;
    while ((reinterpret_cast<std::uintptr_t>(cursor) & 3u) != 0) {
        const unsigned char value = *cursor++;
        *destination++ = value;
        if (--remaining == 0) {
            return target;
        }
        if (value == 0) {
            while (remaining != 0
                   && (reinterpret_cast<std::uintptr_t>(destination) & 3u) != 0) {
                *destination++ = 0;
                --remaining;
            }
            while (remaining >= 4) {
                std::memcpy(destination, &zero, sizeof(zero));
                destination += 4;
                remaining -= 4;
            }
            while (remaining != 0) {
                *destination++ = 0;
                --remaining;
            }
            return target;
        }
    }

    while (remaining >= 4) {
        std::uint32_t probe;
        std::memcpy(&probe, cursor, sizeof(probe));
        std::uint32_t block;
        std::memcpy(&block, cursor, sizeof(block));
        cursor += 4;
        if (((probe + 0x7EFEFEFFu) ^ ~probe) & 0x81010100u) {
            for (std::uint32_t index = 0; index < 4; ++index) {
                if (((block >> (index * 8)) & 0xFFu) != 0) {
                    continue;
                }
                if (index == 0) {
                    block = 0;
                } else if (index == 1) {
                    block &= 0xFFu;
                } else if (index == 2) {
                    block &= 0xFFFFu;
                }
                std::memcpy(destination, &block, sizeof(block));
                destination += 4;
                remaining -= 4;
                while (remaining >= 4) {
                    std::memcpy(destination, &zero, sizeof(zero));
                    destination += 4;
                    remaining -= 4;
                }
                while (remaining != 0) {
                    *destination++ = 0;
                    --remaining;
                }
                return target;
            }
        }
        std::memcpy(destination, &block, sizeof(block));
        destination += 4;
        remaining -= 4;
    }

    while (remaining != 0) {
        const unsigned char value = *cursor++;
        *destination++ = value;
        if (value == 0) {
            --remaining;
            while (remaining != 0) {
                *destination++ = 0;
                --remaining;
            }
            return target;
        }
        --remaining;
    }
    return target;
}

// 原53C2F0及共享53C2E0返回块：查找低W8目标，返回原文本内的借用指针。
char* __cdecl StrChr_53C2F0(const char* text, const std::int32_t target)
{
    const auto needle = static_cast<unsigned char>(target);
    const auto* cursor = reinterpret_cast<const unsigned char*>(text);
    while ((reinterpret_cast<std::uintptr_t>(cursor) & 3u) != 0) {
        const unsigned char value = *cursor++;
        if (value == needle) {
            return const_cast<char*>(reinterpret_cast<const char*>(cursor - 1));
        }
        if (value == 0) {
            return nullptr;
        }
    }

    const std::uint32_t pattern = static_cast<std::uint32_t>(needle) * 0x01010101u;
    for (;;) {
        std::uint32_t probe;
        std::memcpy(&probe, cursor, sizeof(probe));
        const std::uint32_t zeroSum = probe + 0x7EFEFEFFu;
        const std::uint32_t compared = probe ^ pattern;
        const std::uint32_t matchFlags =
            ((compared + 0x7EFEFEFFu) ^ ~compared) & 0x81010100u;
        const std::uint32_t zeroFlags = (zeroSum ^ ~probe) & 0x81010100u;
        cursor += 4;
        if (matchFlags != 0) {
            // 原53C368仅在目标候选出现后重读，逐字节先匹配再判NUL。
            std::uint32_t block;
            std::memcpy(&block, cursor - 4, sizeof(block));
            for (std::uint32_t index = 0; index < 4; ++index) {
                const auto value = static_cast<unsigned char>(block >> (index * 8u));
                if (value == needle) {
                    return const_cast<char*>(reinterpret_cast<const char*>(cursor - 4 + index));
                }
                if (value == 0) {
                    return nullptr;
                }
            }
            continue;
        }
        if (zeroFlags == 0) {
            continue;
        }
        if ((zeroFlags & 0x01010100u) != 0) {
            return nullptr;
        }
        if ((zeroSum & 0x80000000u) != 0) {
            continue;
        }
        return nullptr;
    }
}

// 原53BE40：空串/单字节共用查找，多字节保留候选复用及两字节比对顺序。
char* __cdecl StrStr_53BE40(const char* text, const char* substring)
{
    const auto* const needle = reinterpret_cast<const unsigned char*>(substring);
    const unsigned char first = needle[0];
    if (first == 0) {
        return const_cast<char*>(text);
    }
    const unsigned char second = needle[1];
    if (second == 0) {
        return StrChr_53C2F0(text, first);
    }

    const auto* cursor = reinterpret_cast<const unsigned char*>(text);
    unsigned char value = *cursor++;
    for (;;) {
        while (value != first) {
            if (value == 0) {
                return nullptr;
            }
            value = *cursor++;
        }
        value = *cursor++;
        if (value != second) {
            // 原53BE7F回到53BE6C，复用该字节，不直接跳过它。
            continue;
        }

        const auto* const candidate = cursor - 2;
        const auto* needleCursor = needle;
        for (;;) {
            const unsigned char expected = needleCursor[2];
            if (expected == 0) {
                return const_cast<char*>(reinterpret_cast<const char*>(candidate));
            }
            const unsigned char actual = *cursor;
            cursor += 2;
            if (actual != expected) {
                break;
            }
            const unsigned char tail = needleCursor[3];
            if (tail == 0) {
                return const_cast<char*>(reinterpret_cast<const char*>(candidate));
            }
            const unsigned char actualTail = cursor[-1];
            needleCursor += 2;
            if (tail != actualTail) {
                break;
            }
        }
        // 原53BE58从第二个已匹配源字节重新候选，needle也重置至首部。
        cursor = candidate + 1;
        value = *cursor++;
    }
}

namespace {

struct LegacyFileErrorEntry {
    std::uint32_t nativeCode;
    std::int32_t error;
};

// 原 [591E80,591FE8)；重复键按首次匹配，不排序或合并。
constexpr LegacyFileErrorEntry kLegacyFileErrors591E80[] {
    {1, 22}, {2, 2}, {3, 2}, {4, 24}, {5, 13},
    {6, 9}, {7, 12}, {8, 12}, {9, 12}, {10, 7},
    {11, 8}, {12, 22}, {13, 22}, {15, 2}, {16, 13},
    {17, 18}, {18, 2}, {33, 13}, {53, 2}, {65, 13},
    {67, 2}, {80, 17}, {82, 13}, {83, 13}, {87, 22},
    {89, 11}, {108, 13}, {109, 32}, {112, 28}, {114, 9},
    {6, 22}, {128, 10}, {129, 10}, {130, 9}, {131, 22},
    {132, 13}, {145, 41}, {158, 13}, {161, 2}, {164, 11},
    {167, 13}, {183, 17}, {206, 2}, {215, 11}, {1816, 12}
};
static_assert(sizeof(kLegacyFileErrors591E80) == 0x168);

std::uintptr_t MapLegacyFileError_542F44(const std::uint32_t code)
{
    gLegacyDosError_1085FE4 = code;
    for (const LegacyFileErrorEntry& entry : kLegacyFileErrors591E80) {
        if (entry.nativeCode == code) {
            gLegacyErrno_1085FE0 = entry.error;
            return static_cast<std::uintptr_t>(entry.error);
        }
    }
    if (code >= 19 && code <= 36) {
        gLegacyErrno_1085FE0 = 13;
    } else if (code >= 188) {
        gLegacyErrno_1085FE0 = 8;
        if (code > 202) {
            gLegacyErrno_1085FE0 = 22;
        }
    } else {
        gLegacyErrno_1085FE0 = 22;
    }
    return reinterpret_cast<std::uintptr_t>(kLegacyFileErrors591E80 + 45);
}

// GMT@0x541158 恒返回这一份共享对象；原运行初值不由未加载区字节推断。
LegacyTm53B8C6 gLegacyTm_1086128;

// 原 0x58FB78/7C/80 的映像初值；刷新入口后续可改写，不能视作常量。
std::int32_t gTimezone_58FB78 = 28800;
std::int32_t gDaylight_58FB7C = 1;
std::int32_t gDstBias_58FB80 = -3600;

char gStandardName_58FB84[64] = "PST";
char gDaylightName_58FBC4[64] = "PDT";
char* gStandardNamePointer_58FC04 = gStandardName_58FB84;
char* gDaylightNamePointer_58FC08 = gDaylightName_58FBC4;

struct LegacyDstDate {
    std::int32_t year;
    std::int32_t yearDay;
    std::int32_t milliseconds;
};

LegacyDstDate gDstStartDate_58FC10 {-1, 0, 0};
LegacyDstDate gDstEndDate_58FC20 {-1, 0, 0};
std::int32_t gSystemTzValid_1086068 = 0;
TIME_ZONE_INFORMATION gTimeZoneInformation_1086070 {};
char* gTzCache_108611C = nullptr;
std::int32_t gTzSetCount_1086120 = 0;

// 原 0x5921F0 与 0x592224（末项 0x592254），供同文件转换入口复用。
constexpr std::int32_t kLeapDays_5921F0[13] = {
    -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};
constexpr std::int32_t kPlainDays_592224[13] = {
    -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364
};

// 原 [0x540F27,0x541067)：计算 DST 边界，分别发布开始或结束缓存。
std::int32_t __cdecl CvtDate_540F27(
    std::int32_t start, std::int32_t format, std::int32_t year,
    std::int32_t month, std::int32_t week, std::int32_t weekDay,
    std::int32_t day, std::int32_t hour, std::int32_t minute,
    std::int32_t second, std::int32_t millisecond)
{
    const std::int32_t* days = (year & 3) == 0 ? kLeapDays_5921F0 : kPlainDays_592224;
    std::int32_t yearDay;
    if (format == 1) {
        const std::int32_t firstDay = days[month - 1] + 1;
        const std::int32_t firstWeekDay =
            (365 * year + ((year - 1) >> 2) + firstDay - 25563) % 7;
        yearDay = firstDay + 7 * week - firstWeekDay + weekDay;
        if (firstWeekDay <= weekDay) {
            yearDay -= 7;
        }
        if (week == 5 && yearDay > days[month]) {
            yearDay -= 7;
        }
    } else {
        yearDay = days[month - 1] + day;
    }

    if (start == 1) {
        const std::int32_t startHour = hour;
        gDstStartDate_58FC10.yearDay = yearDay;
        const std::int32_t hourMinute = startHour * 60 + minute;
        gDstStartDate_58FC10.year = year;
        const std::int32_t milliseconds =
            ((hourMinute * 60 + second) * 1000) + millisecond;
        gDstStartDate_58FC10.milliseconds = milliseconds;
        return milliseconds;
    }

    const std::int32_t endHour = hour;
    gDstEndDate_58FC20.yearDay = yearDay;
    const std::int32_t hourSeconds = (endHour * 60 + minute) * 60;
    std::int32_t milliseconds =
        ((hourSeconds + gDstBias_58FB80 + second) * 1000) + millisecond;
    gDstEndDate_58FC20.milliseconds = milliseconds;
    if (milliseconds < 0) {
        milliseconds += 86400000;
        --yearDay;
        gDstEndDate_58FC20.milliseconds = milliseconds;
        gDstEndDate_58FC20.yearDay = yearDay;
    } else if (milliseconds >= 86400000) {
        milliseconds -= 86400000;
        ++yearDay;
        gDstEndDate_58FC20.milliseconds = milliseconds;
        gDstEndDate_58FC20.yearDay = yearDay;
    }
    gDstEndDate_58FC20.year = year;
    return milliseconds;
}

// 原 [0x540B1D,0x540D7B)：共享槽按原分支与调用次序逐次发布。
void RefreshTz_540B1D()
{
    gSystemTzValid_1086068 = 0;
    gDstEndDate_58FC20.year = -1;
    gDstStartDate_58FC10.year = -1;

    char* tz = GetEnv_54518B("TZ");
    if (tz == nullptr) {
        if (GetTimeZoneInformation(&gTimeZoneInformation_1086070) == TIME_ZONE_ID_INVALID) {
            return;
        }

        const std::int32_t bias = gTimeZoneInformation_1086070.Bias;
        const std::int32_t standardBias = gTimeZoneInformation_1086070.StandardBias;
        const WORD standardMonth = gTimeZoneInformation_1086070.StandardDate.wMonth;
        gTimezone_58FB78 = bias * 60;
        gSystemTzValid_1086068 = 1;
        if (standardMonth != 0) {
            gTimezone_58FB78 = gTimezone_58FB78 + standardBias * 60;
        }

        if (gTimeZoneInformation_1086070.DaylightDate.wMonth != 0
            && gTimeZoneInformation_1086070.DaylightBias != 0) {
            gDaylight_58FB7C = 1;
            gDstBias_58FB80 =
                (gTimeZoneInformation_1086070.DaylightBias - standardBias) * 60;
        } else {
            gDaylight_58FB7C = 0;
            gDstBias_58FB80 = 0;
        }

        BOOL usedDefault = FALSE;
        char* standardName = gStandardNamePointer_58FC04;
        const UINT standardCodePage = gConversionCodePage_1086170;
        const int standardLength = WideCharToMultiByte(
            standardCodePage, 0x220, gTimeZoneInformation_1086070.StandardName, -1,
            standardName, 63, nullptr, &usedDefault);
        if (standardLength == 0 || usedDefault != FALSE) {
            gStandardNamePointer_58FC04[0] = '\0';
        } else {
            gStandardNamePointer_58FC04[63] = '\0';
        }

        char* daylightName = gDaylightNamePointer_58FC08;
        const UINT daylightCodePage = gConversionCodePage_1086170;
        const int daylightLength = WideCharToMultiByte(
            daylightCodePage, 0x220, gTimeZoneInformation_1086070.DaylightName, -1,
            daylightName, 63, nullptr, &usedDefault);
        if (daylightLength == 0 || usedDefault != FALSE) {
            gDaylightNamePointer_58FC08[0] = '\0';
        } else {
            gDaylightNamePointer_58FC08[63] = '\0';
        }
        return;
    }

    if (*tz == '\0') {
        return;
    }
    if (gTzCache_108611C != nullptr && StrCmp_5450F0(gTzCache_108611C, tz) == 0) {
        return;
    }

    Free_53C0A9(gTzCache_108611C);
    const std::uint32_t length = StrLen_53F9E0(tz);
    gTzCache_108611C = static_cast<char*>(Malloc_53C3AC(length + 1));
    if (gTzCache_108611C == nullptr) {
        return;
    }
    StrCpy_544DB0(gTzCache_108611C, tz);

    StrNCpy_53BEC0(gStandardNamePointer_58FC04, tz, 3);
    gStandardNamePointer_58FC04[3] = '\0';
    const char* cursor = tz + 3;
    bool negative = false;
    if (*cursor == '-') {
        negative = true;
        ++cursor;
    }

    std::int32_t offset = Atol_53C5BE(cursor) * 3600;
    gTimezone_58FB78 = offset;
    while (*cursor == '+' || (*cursor >= '0' && *cursor <= '9')) {
        ++cursor;
    }
    if (*cursor == ':') {
        ++cursor;
        const std::int32_t minutes = Atol_53C5BE(cursor) * 60;
        offset = gTimezone_58FB78 + minutes;
        gTimezone_58FB78 = offset;
        while (*cursor >= '0' && *cursor <= '9') {
            ++cursor;
        }
        if (*cursor == ':') {
            ++cursor;
            const std::int32_t seconds = Atol_53C5BE(cursor);
            offset = gTimezone_58FB78 + seconds;
            gTimezone_58FB78 = offset;
            while (*cursor >= '0' && *cursor <= '9') {
                ++cursor;
            }
        }
    }
    if (negative) {
        offset = -offset;
        gTimezone_58FB78 = offset;
    }

    const std::int32_t daylight = static_cast<std::int8_t>(*cursor);
    gDaylight_58FB7C = daylight;
    if (daylight != 0) {
        StrNCpy_53BEC0(gDaylightNamePointer_58FC08, cursor, 3);
        gDaylightNamePointer_58FC08[3] = '\0';
    } else {
        gDaylightNamePointer_58FC08[0] = '\0';
    }
}

// 原 [0x541067,0x541160)：负值不写共享对象；正值按原九次写入顺序覆盖。
LegacyTm53B8C6* GmTime_541067(const std::int32_t* timeValue)
{
    const std::int32_t time = *timeValue;
    if (time < 0) {
        return nullptr;
    }

    const std::int32_t cycle = time / 126230400;
    std::int32_t remaining = time - cycle * 126230400;
    std::int32_t year = 70 + 4 * cycle;
    const std::int32_t* days = kPlainDays_592224;
    if (remaining >= 31536000) {
        remaining -= 31536000;
        ++year;
        if (remaining >= 31536000) {
            remaining -= 31536000;
            ++year;
            if (remaining < 31622400) {
                days = kLeapDays_5921F0;
            } else {
                remaining -= 31622400;
                ++year;
            }
        }
    }

    const std::int32_t yearDay = remaining / 86400;
    std::int32_t index = 1;
    while (days[index] < yearDay) {
        ++index;
    }

    gLegacyTm_1086128.yearMinus1900 = year;             // 0x5410BE
    gLegacyTm_1086128.yearDay0based = yearDay;           // 0x5410D1
    gLegacyTm_1086128.month0based = index - 1;           // 0x5410FF
    gLegacyTm_1086128.day = yearDay - days[index - 1];   // 0x54110B
    // 0x541110 再读原指针；它可以别名于刚被覆写的共享对象。
    gLegacyTm_1086128.weekDay = (*timeValue / 86400 + 4) % 7; // 0x541123
    const std::int32_t daySeconds = remaining - yearDay * 86400;
    gLegacyTm_1086128.hours = daySeconds / 3600;        // 0x54112F
    const std::int32_t hourSeconds = daySeconds % 3600;
    gLegacyTm_1086128.minutes = hourSeconds / 60;       // 0x541141
    gLegacyTm_1086128.dst = 0;                          // 0x54114B
    gLegacyTm_1086128.seconds = hourSeconds % 60;       // 0x541152
    return &gLegacyTm_1086128;
}

} // namespace

// 原 0x540B08 惰性时区入口；刷新早退也只增加一次共享计数。
void __cdecl TzSet_540B08()
{
    if (gTzSetCount_1086120 != 0) {
        return;
    }
    RefreshTz_540B1D();
    ++gTzSetCount_1086120;
}

// 原 [0x540D7B,0x540F27)：双年份缓存命中时直接判定，否则重算两端边界。
std::int32_t __cdecl IsDst_540D7B(LegacyTm53B8C6* time)
{
    if (gDaylight_58FB7C == 0) {
        return 0;
    }

    const std::int32_t year = time->yearMinus1900;
    if (gDstStartDate_58FC10.year != year || gDstEndDate_58FC20.year != year) {
        if (gSystemTzValid_1086068 != 0) {
            const std::int32_t startMillisecond =
                gTimeZoneInformation_1086070.DaylightDate.wMilliseconds;
            const bool startRelative = gTimeZoneInformation_1086070.DaylightDate.wYear == 0;
            const std::int32_t startSecond =
                gTimeZoneInformation_1086070.DaylightDate.wSecond;
            const std::int32_t startMinute =
                gTimeZoneInformation_1086070.DaylightDate.wMinute;
            const std::int32_t startHour =
                gTimeZoneInformation_1086070.DaylightDate.wHour;
            if (startRelative) {
                const std::int32_t startWeekDay =
                    gTimeZoneInformation_1086070.DaylightDate.wDayOfWeek;
                const std::int32_t startWeek =
                    gTimeZoneInformation_1086070.DaylightDate.wDay;
                const std::int32_t startMonth =
                    gTimeZoneInformation_1086070.DaylightDate.wMonth;
                CvtDate_540F27(
                    1, 1, year, startMonth, startWeek, startWeekDay, 0,
                    startHour, startMinute, startSecond, startMillisecond);
            } else {
                const std::int32_t startDay =
                    gTimeZoneInformation_1086070.DaylightDate.wDay;
                const std::int32_t startMonth =
                    gTimeZoneInformation_1086070.DaylightDate.wMonth;
                CvtDate_540F27(
                    1, 0, year, startMonth, 0, 0, startDay,
                    startHour, startMinute, startSecond, startMillisecond);
            }

            const std::int32_t endMillisecond =
                gTimeZoneInformation_1086070.StandardDate.wMilliseconds;
            const bool endRelative = gTimeZoneInformation_1086070.StandardDate.wYear == 0;
            const std::int32_t endSecond =
                gTimeZoneInformation_1086070.StandardDate.wSecond;
            const std::int32_t endMinute =
                gTimeZoneInformation_1086070.StandardDate.wMinute;
            const std::int32_t endHour =
                gTimeZoneInformation_1086070.StandardDate.wHour;
            if (endRelative) {
                const std::int32_t endWeekDay =
                    gTimeZoneInformation_1086070.StandardDate.wDayOfWeek;
                const std::int32_t endWeek =
                    gTimeZoneInformation_1086070.StandardDate.wDay;
                const std::int32_t endMonth =
                    gTimeZoneInformation_1086070.StandardDate.wMonth;
                const std::int32_t endYear = time->yearMinus1900;
                CvtDate_540F27(
                    0, 1, endYear, endMonth, endWeek, endWeekDay, 0,
                    endHour, endMinute, endSecond, endMillisecond);
            } else {
                const std::int32_t endDay =
                    gTimeZoneInformation_1086070.StandardDate.wDay;
                const std::int32_t endMonth =
                    gTimeZoneInformation_1086070.StandardDate.wMonth;
                const std::int32_t endYear = time->yearMinus1900;
                CvtDate_540F27(
                    0, 0, endYear, endMonth, 0, 0, endDay,
                    endHour, endMinute, endSecond, endMillisecond);
            }
        } else {
            CvtDate_540F27(1, 1, year, 4, 1, 0, 0, 2, 0, 0, 0);
            const std::int32_t endYear = time->yearMinus1900;
            CvtDate_540F27(0, 1, endYear, 10, 5, 0, 0, 2, 0, 0, 0);
        }
    }

    const std::int32_t startDay = gDstStartDate_58FC10.yearDay;
    const std::int32_t endDay = gDstEndDate_58FC20.yearDay;
    const std::int32_t yearDay = time->yearDay0based;
    if (startDay < endDay) {
        if (yearDay < startDay || yearDay > endDay) {
            return 0;
        }
        if (yearDay > startDay && yearDay < endDay) {
            return 1;
        }
    } else {
        if (yearDay < endDay || yearDay > startDay) {
            return 1;
        }
        if (yearDay > endDay && yearDay < startDay) {
            return 0;
        }
    }

    const std::int32_t hour = time->hours;
    const std::int32_t minute = time->minutes;
    const std::int32_t second = time->seconds;
    const std::int32_t milliseconds = ((hour * 60 + minute) * 60 + second) * 1000;
    if (yearDay == startDay) {
        return milliseconds >= gDstStartDate_58FC10.milliseconds ? 1 : 0;
    }
    return milliseconds < gDstEndDate_58FC20.milliseconds ? 1 : 0;
}

// 原 [0x53B8C6,0x53BA26)：两端区间保留 GMT 共享对象上的原字段修正次序。
LegacyTm53B8C6* __cdecl LocalTime_53B8C6(std::int32_t* timeValue)
{
    if (*timeValue < 0) {
        return nullptr;
    }
    TzSet_540B08();
    const std::int32_t time = *timeValue;

    if (time > 259200 && time < 2147224447) {
        std::int32_t adjusted = time - gTimezone_58FB78;
        LegacyTm53B8C6* result = GmTime_541067(&adjusted);
        if (gDaylight_58FB7C != 0 && IsDst_540D7B(result) != 0) {
            adjusted -= gDstBias_58FB80;
            result = GmTime_541067(&adjusted);
            result->dst = 1; // 0x53B933，仅本分支写 1。
        }
        return result;
    }

    LegacyTm53B8C6* result = GmTime_541067(timeValue); // 0x53B940，传原指针。
    const std::int32_t isDst = IsDst_540D7B(result);  // 0x53B948，无条件调用。
    std::int32_t adjusted = result->seconds;
    if (isDst != 0) {
        adjusted -= gDstBias_58FB80;
    }
    adjusted -= gTimezone_58FB78;

    result->seconds = adjusted % 60;
    if (result->seconds < 0) {
        result->seconds += 60;
        adjusted -= 60;
    }
    adjusted = result->minutes + adjusted / 60;
    result->minutes = adjusted % 60;
    if (result->minutes < 0) {
        result->minutes += 60;
        adjusted -= 60;
    }
    adjusted = result->hours + adjusted / 60;
    result->hours = adjusted % 24;
    if (result->hours < 0) {
        result->hours += 24;
        adjusted -= 24;
    }

    const std::int32_t dayOffset = adjusted / 24;
    if (dayOffset > 0) {
        result->weekDay = (result->weekDay + dayOffset) % 7;
        result->day += dayOffset;
        result->yearDay0based += dayOffset;
    } else if (dayOffset < 0) {
        result->weekDay = (result->weekDay + dayOffset + 7) % 7;
        result->day += dayOffset;
        if (result->day > 0) {
            result->yearDay0based += dayOffset;
        } else {
            --result->yearMinus1900;
            result->day += 31;
            result->yearDay0based = 364;
            result->month0based = 11; // 0x53BA10..0x53BA1D，不作通用跨月修正。
        }
    }
    return result;
}

// 原 __loctotime_t@0x541160：本地七字段换算为原 32 位时间值。
std::int32_t __cdecl ConvertLocalTime_541160(
    std::int32_t year, std::int32_t month, std::int32_t day,
    std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t daylight)
{
    const std::int32_t yearMinus1900 = year - 1900;
    if (yearMinus1900 < 70 || yearMinus1900 > 138) {
        return -1;
    }

    std::int32_t yearDay = kPlainDays_592224[month - 1] + day;
    if ((yearMinus1900 & 3) == 0 && month > 2) {
        ++yearDay;
    }

    TzSet_540B08();
    LegacyTm53B8C6 candidate;
    candidate.yearDay0based = yearDay;
    candidate.yearMinus1900 = yearMinus1900;
    candidate.hours = hour;

    const std::int32_t epochDay =
        365 * (yearMinus1900 - 70) + ((yearMinus1900 - 1) >> 2) - 17 + yearDay;
    std::int32_t result = ((epochDay * 24 + hour) * 60 + minute) * 60;
    result += gTimezone_58FB78;
    candidate.month0based = month - 1;
    result += second;

    if (daylight == 1) {
        return result + gDstBias_58FB80;
    }
    if (daylight == -1 && gDaylight_58FB7C != 0 && IsDst_540D7B(&candidate) != 0) {
        return result + gDstBias_58FB80;
    }
    return result;
}

std::int32_t CreateLegacyDirectory_53C654(const char* const path)
{
    const DWORD error = ::CreateDirectoryA(path, nullptr) != FALSE
        ? ERROR_SUCCESS : ::GetLastError();
    if (error != ERROR_SUCCESS) {
        static_cast<void>(MapLegacyFileError_542F44(error));
        return -1;
    }
    return 0;
}

std::int32_t ChangeLegacyDirectory_53DD2F(const char* const path)
{
    char directory[261];
    if (::SetCurrentDirectoryA(path) != 0
        && ::GetCurrentDirectoryA(261, directory) != 0) {
        const char first = directory[0];
        if ((first == '\\' || first == '/') && first == directory[1]) {
            return 0;
        }
        char driveEnvironment[4];
        driveEnvironment[0] = '=';
        const std::uint32_t upper = UpperLegacyDirectoryByte_543D37(
            static_cast<std::uint8_t>(first));
        driveEnvironment[3] = '\0';
        driveEnvironment[1] = std::bit_cast<char>(static_cast<std::uint8_t>(upper));
        driveEnvironment[2] = ':';
        if (::SetEnvironmentVariableA(driveEnvironment, directory) != 0) {
            return 0;
        }
    }
    static_cast<void>(MapLegacyFileError_542F44(::GetLastError()));
    return -1;
}

std::int32_t ReadLegacyDescriptorStatus32_53CCD0(
    const std::int32_t descriptor, LegacyFileStatus32* const status)
{
    const auto descriptorIndex = static_cast<std::uint32_t>(descriptor);
    if (descriptorIndex >= gLegacyDescriptorCapacity1086AC0) {
        gLegacyErrno_1085FE0 = 9;
        return -1;
    }
    const std::uint32_t slot = descriptorIndex & 31u;
    // 原var_8保存表块槽地址；各API前重新从同一块槽取得HANDLE。
    LegacyDescriptorRecord*& block = gLegacyDescriptorBlocks10869C0[descriptorIndex >> 5];
    if ((block[slot].flags & 1u) == 0) {
        gLegacyErrno_1085FE0 = 9;
        return -1;
    }

    const DWORD fileType = ::GetFileType(block[slot].handle) & ~0x8000u;
    if (fileType == FILE_TYPE_CHAR || fileType == FILE_TYPE_PIPE) {
        status->mode = fileType == FILE_TYPE_CHAR ? 0x2000u : 0x1000u;
        status->deviceBits = descriptorIndex;
        status->relatedDeviceBits = descriptorIndex;
        status->linkCount = 1;
        status->inode = 0;
        status->group = 0;
        status->user = 0;
        status->creationTime = 0;
        status->modifiedTime = 0;
        status->accessTime = 0;
        if (fileType == FILE_TYPE_CHAR) {
            status->sizeLowBits = 0;
        } else {
            DWORD available;
            const BOOL peeked = ::PeekNamedPipe(
                block[slot].handle, nullptr, 0, nullptr, &available, nullptr);
            status->sizeLowBits = peeked != FALSE ? available : 0;
        }
        return 0;
    }
    if (fileType == FILE_TYPE_UNKNOWN) {
        gLegacyErrno_1085FE0 = 9;
        return -1;
    }

    BY_HANDLE_FILE_INFORMATION information;
    if (fileType == FILE_TYPE_DISK) {
        status->mode = 0;
        status->group = 0;
        status->user = 0;
        status->inode = 0;
        status->linkCount = 1;
    }
    if (fileType != FILE_TYPE_DISK
        || ::GetFileInformationByHandle(block[slot].handle, &information) == FALSE) {
        static_cast<void>(MapLegacyFileError_542F44(::GetLastError()));
        return -1;
    }
    status->mode |= (information.dwFileAttributes & 1u) != 0 ? 0x124u : 0x1B6u;

    const auto convert = [](const FILETIME& fileTime, std::int32_t& result) {
        FILETIME localTime;
        SYSTEMTIME fields;
        if (::FileTimeToLocalFileTime(&fileTime, &localTime) == FALSE
            || ::FileTimeToSystemTime(&localTime, &fields) == FALSE) {
            return false;
        }
        result = ConvertLocalTime_541160(
            fields.wYear, fields.wMonth, fields.wDay, fields.wHour,
            fields.wMinute, fields.wSecond, -1);
        return true;
    };
    std::int32_t converted;
    if (!convert(information.ftLastWriteTime, converted)) {
        return -1;
    }
    status->modifiedTime = converted;
    if (information.ftLastAccessTime.dwLowDateTime != 0
        || information.ftLastAccessTime.dwHighDateTime != 0) {
        if (!convert(information.ftLastAccessTime, converted)) {
            return -1;
        }
    }
    status->accessTime = converted;
    if (information.ftCreationTime.dwLowDateTime == 0
        && information.ftCreationTime.dwHighDateTime == 0) {
        converted = status->modifiedTime;
    } else if (!convert(information.ftCreationTime, converted)) {
        return -1;
    }
    status->mode |= 0x8000u;
    status->creationTime = converted;
    const std::uint32_t sizeLow = information.nFileSizeLow;
    status->deviceBits = 0;
    status->sizeLowBits = sizeLow;
    status->relatedDeviceBits = 0;
    return 0;
}

std::int32_t ReadLegacyFileCreationTime32(
    LegacyFile* const file, std::int32_t* const output)
{
    LegacyFileStatus32 status;
    const std::int32_t result =
        ReadLegacyDescriptorStatus32_53CCD0(file->descriptor, &status);
    if (result >= 0) {
        *output = status.creationTime;
    }
    return result;
}

std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output)
{
    SYSTEMTIME localTime;
    SYSTEMTIME utcTime;
    TIME_ZONE_INFORMATION timeZone;
    GetLocalTime(&localTime);
    GetSystemTime(&utcTime);

    std::int32_t daylight;
    if (utcTime.wMinute == gUtcTimeCache_1085FD0.wMinute
        && utcTime.wHour == gUtcTimeCache_1085FD0.wHour
        && utcTime.wDay == gUtcTimeCache_1085FD0.wDay
        && utcTime.wMonth == gUtcTimeCache_1085FD0.wMonth
        && utcTime.wYear == gUtcTimeCache_1085FD0.wYear) {
        daylight = gDaylightCache_1085FC8;
    } else {
        const DWORD zone = GetTimeZoneInformation(&timeZone);
        if (zone == TIME_ZONE_ID_INVALID) {
            daylight = -1;
        } else {
            daylight = zone == TIME_ZONE_ID_DAYLIGHT
                && timeZone.DaylightDate.wMonth != 0 && timeZone.DaylightBias != 0 ? 1 : 0;
        }
        std::memcpy(&gUtcTimeCache_1085FD0, &utcTime, sizeof(utcTime));
        gDaylightCache_1085FC8 = daylight;
    }

    const std::int32_t result = ConvertLocalTime_541160(
        localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour,
        localTime.wMinute, localTime.wSecond, daylight);
    if (output != nullptr) {
        *output = result;
    }
    return result;
}

} // namespace cg::platform
