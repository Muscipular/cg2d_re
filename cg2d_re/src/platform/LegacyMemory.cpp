#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <atomic>
#include <cstring>
#include <memory>

#include <malloc.h>
#include <new.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace cg::platform {

namespace {

constexpr char kHeapSelectName54FAEC[] = "__MSVCRT_HEAP_SELECT";
constexpr char kGlobalHeapSelection54FAD4[] = "__GLOBAL_HEAP_SELECTED";
static_assert(sizeof(kHeapSelectName54FAEC) == 21);
static_assert(sizeof(kGlobalHeapSelection54FAD4) == 23);
static_assert(sizeof(OSVERSIONINFOA) == 0x94);

// 原54B160的W32取样：完整载荷复制前后约束编译器，不复用失配前的取值。
// 只供CompareLegacyMemory_54B160三个读取阶段使用，字段宽度和对象别名规则保持。
std::uint32_t ReadMemoryComparisonWord(const unsigned char* const source) noexcept
{
    std::atomic_signal_fence(std::memory_order_seq_cst);
    std::uint32_t word;
    std::memcpy(&word, source, sizeof(word));
    std::atomic_signal_fence(std::memory_order_seq_cst);
    return word;
}

// 原541A43；输出先清W32，再按原顺序写链接器主/次版本两个U8。
void ReadLegacyModuleLinkerVersion_541A43(std::uint32_t* const version)
{
    *version = 0;
    const auto* const module =
        reinterpret_cast<const unsigned char*>(::GetModuleHandleA(nullptr));
    const auto* const dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return;
    }
    const std::int32_t headerOffset = dosHeader->e_lfanew;
    if (headerOffset == 0) {
        return;
    }
    const unsigned char* const ntHeader = module + headerOffset;
    auto* const bytes = reinterpret_cast<unsigned char*>(version);
    bytes[0] = ntHeader[0x1A];
    bytes[1] = ntHeader[0x1B];
}

} // namespace

// 原541A70；只选模式，不发布堆句柄、创建私有池或改变当前分配器。
std::int32_t SelectLegacyHeapMode_541A70()
{
    OSVERSIONINFOA information;
    information.dwOSVersionInfoSize = sizeof(information);
    if (::GetVersionExA(&information) != 0
        && information.dwPlatformId == VER_PLATFORM_WIN32_NT
        && information.dwMajorVersion >= 5u) {
        return 1;
    }

    char environment[0x1090];
    if (::GetEnvironmentVariableA(kHeapSelectName54FAEC, environment,
            sizeof(environment)) != 0) {
        for (char* cursor = environment; *cursor != '\0'; ++cursor) {
            const char value = *cursor;
            if (value >= 'a' && value <= 'z') {
                *cursor = static_cast<char>(value - 0x20);
            }
        }

        char* selection;
        if (StrNCmp_53BFC0(kGlobalHeapSelection54FAD4, environment, 22) == 0) {
            selection = environment;
        } else {
            char filename[0x104];
            // 原忽略API返回，不补未写局部内容或强制终止字节。
            static_cast<void>(::GetModuleFileNameA(nullptr, filename, sizeof(filename)));
            for (char* cursor = filename; *cursor != '\0'; ++cursor) {
                const char value = *cursor;
                if (value >= 'a' && value <= 'z') {
                    *cursor = static_cast<char>(value - 0x20);
                }
            }
            selection = StrStr_53BE40(environment, filename);
        }
        if (selection != nullptr) {
            char* const comma = StrChr_53C2F0(selection, ',');
            if (comma != nullptr) {
                char* const value = comma + 1;
                for (char* cursor = value; *cursor != '\0'; ++cursor) {
                    if (*cursor == ';') {
                        *cursor = '\0';
                        break;
                    }
                }
                const std::int32_t mode = Strtol_53DA45(value, nullptr, 10);
                if (mode == 2 || mode == 3 || mode == 1) {
                    return mode;
                }
            }
        }
    }

    std::uint32_t linkerVersion;
    ReadLegacyModuleLinkerVersion_541A43(&linkerVersion);
    // 原541BAB是80 /7字节比较，仅主版本低U8参与回退。
    return static_cast<std::uint8_t>(linkerVersion) < 6 ? 2 : 3;
}

void* CopyLegacyMemory_541620(void* const destination,
    const void* const source, const std::uint32_t byteCount)
{
    auto* const output = static_cast<unsigned char*>(destination);
    const auto* const input = static_cast<const unsigned char*>(source);
    const auto destinationAddress = reinterpret_cast<std::uintptr_t>(destination);
    const auto sourceAddress = reinterpret_cast<std::uintptr_t>(source);
    std::uint32_t remaining = byteCount;
    if (destinationAddress > sourceAddress
        && destinationAddress - sourceAddress < byteCount) {
        // 以尾后索引表达原逆向路径，避免构造源/目标起点之前的指针。
        if (remaining >= 4) {
            const auto prefix = static_cast<std::uint32_t>(
                (destinationAddress + remaining) & 3u);
            for (std::uint32_t index = 0; index < prefix; ++index) {
                --remaining;
                const unsigned char value = input[remaining];
                output[remaining] = value;
            }
            while (remaining >= 4) {
                remaining -= 4;
                std::uint32_t value;
                std::memcpy(&value, input + remaining, sizeof(value));
                std::memcpy(output + remaining, &value, sizeof(value));
            }
        }
        while (remaining != 0) {
            --remaining;
            const unsigned char value = input[remaining];
            output[remaining] = value;
        }
    } else {
        std::uint32_t offset = 0;
        if (remaining >= 4) {
            const auto prefix = static_cast<std::uint32_t>(
                (4u - (destinationAddress & 3u)) & 3u);
            for (std::uint32_t index = 0; index < prefix; ++index) {
                const unsigned char value = input[offset];
                output[offset] = value;
                ++offset;
                --remaining;
            }
            while (remaining >= 4) {
                std::uint32_t value;
                std::memcpy(&value, input + offset, sizeof(value));
                std::memcpy(output + offset, &value, sizeof(value));
                offset += 4;
                remaining -= 4;
            }
        }
        while (remaining != 0) {
            const unsigned char value = input[offset];
            output[offset] = value;
            ++offset;
            --remaining;
        }
    }
    return destination;
}

// 原5419D0；低U8填充，先按4B对齐写前缀，再逐W32/尾字节，返回原目标。
void* FillLegacyMemory_5419D0(
    void* const destination, const std::int32_t value, std::uint32_t byteCount)
{
    if (byteCount == 0) {
        return destination;
    }
    auto* output = static_cast<unsigned char*>(destination);
    const auto byte = static_cast<std::uint8_t>(value);
    if (byteCount >= 4) {
        void* aligned = output;
        std::size_t alignmentSpace = 4;
        // 至多3B调整；这里只求原4B对齐位置，不保存整数地址。
        static_cast<void>(std::align(4, 1, aligned, alignmentSpace));
        while (output != aligned) {
            *output++ = byte;
            --byteCount;
        }
        const std::uint32_t word = static_cast<std::uint32_t>(byte) * 0x01010101u;
        while (byteCount >= 4) {
            std::memcpy(output, &word, sizeof(word));
            output += 4;
            byteCount -= 4;
        }
    }
    while (byteCount != 0) {
        *output++ = byte;
        --byteCount;
    }
    return destination;
}

// 原54B160及分离chunk；未对齐逐U8，双4B对齐按原W32读/复读及尾掩码。
std::int32_t CompareLegacyMemory_54B160(
    const void* const leftBuffer, const void* const rightBuffer, std::uint32_t byteCount)
{
    if (byteCount == 0) {
        return 0;
    }
    const auto* left = static_cast<const unsigned char*>(leftBuffer);
    const auto* right = static_cast<const unsigned char*>(rightBuffer);
    // 原54B174/54B176只比较地址低两位；不构造小缓冲范围外的对齐指针。
    if (((reinterpret_cast<std::uintptr_t>(leftBuffer)
            | reinterpret_cast<std::uintptr_t>(rightBuffer)) & 3u) != 0) {
        if ((byteCount & 1u) != 0) {
            const unsigned char leftByte = *left;
            const unsigned char rightByte = *right;
            if (leftByte != rightByte) {
                return leftByte < rightByte ? -1 : 1;
            }
            ++left;
            ++right;
            --byteCount;
        }
        while (byteCount != 0) {
            for (std::uint32_t index = 0; index != 2; ++index) {
                const unsigned char leftByte = left[index];
                const unsigned char rightByte = right[index];
                if (leftByte != rightByte) {
                    return leftByte < rightByte ? -1 : 1;
                }
            }
            left += 2;
            right += 2;
            byteCount -= 2;
        }
        return 0;
    }

    std::uint32_t words = byteCount >> 2;
    byteCount &= 3u;
    while (words != 0) {
        std::uint32_t leftWord;
        std::uint32_t rightWord;
        leftWord = ReadMemoryComparisonWord(left);
        rightWord = ReadMemoryComparisonWord(right);
        left += 4;
        right += 4;
        --words;
        if (leftWord != rightWord) {
            // 原失配块再次读取，不能改成复用第一次读取的值。
            leftWord = ReadMemoryComparisonWord(left - 4);
            rightWord = ReadMemoryComparisonWord(right - 4);
            for (std::uint32_t shift = 0; shift != 24; shift += 8) {
                const auto leftByte = static_cast<std::uint8_t>(leftWord >> shift);
                const auto rightByte = static_cast<std::uint8_t>(rightWord >> shift);
                if (leftByte != rightByte) {
                    return leftByte < rightByte ? -1 : 1;
                }
            }
            return (leftWord >> 24) < (rightWord >> 24) ? -1 : 1;
        }
    }
    if (byteCount == 0) {
        return 0;
    }
    // 原对齐尾仍读取两W32，随后只按剩余1/2/3字节比较；不改原读取合同。
    std::uint32_t leftWord;
    std::uint32_t rightWord;
    leftWord = ReadMemoryComparisonWord(left);
    rightWord = ReadMemoryComparisonWord(right);
    const auto firstLeft = static_cast<std::uint8_t>(leftWord);
    const auto firstRight = static_cast<std::uint8_t>(rightWord);
    if (firstLeft != firstRight) {
        return firstLeft < firstRight ? -1 : 1;
    }
    if (--byteCount == 0) {
        return 0;
    }
    const auto secondLeft = static_cast<std::uint8_t>(leftWord >> 8);
    const auto secondRight = static_cast<std::uint8_t>(rightWord >> 8);
    if (secondLeft != secondRight) {
        return secondLeft < secondRight ? -1 : 1;
    }
    if (--byteCount == 0) {
        return 0;
    }
    leftWord &= 0x00FF0000u;
    rightWord &= 0x00FF0000u;
    if (leftWord != rightWord) {
        return leftWord < rightWord ? -1 : 1;
    }
    return 0;
}

void* AllocateLegacyByteBuffer(const std::uint32_t byteCount)
{
    // 原53C3BE先拒绝该范围，不调用分配器或new-handler。
    if (byteCount > 0xFFFFFFE0u) {
        return nullptr;
    }
    // 原系统堆路径不清零；边界检查使以下U32对齐运算不溢出。
    const std::uint32_t requested = byteCount == 0 ? 1u : byteCount;
    const std::uint32_t capacity = (requested + 15u) & ~15u;
    for (;;) {
        void* const memory = ::HeapAlloc(
            reinterpret_cast<HANDLE>(::_get_heap_handle()), 0, capacity);
        if (memory != nullptr) {
            return memory;
        }

        // 原541A28每次重读处理器，传原请求长度；不吞处理器抛出的异常。
        const _PNH handler = ::_query_new_handler();
        if (handler == nullptr || handler(byteCount) == 0) {
            return nullptr;
        }
    }
}

void FreeLegacyByteBuffer(void* const memory) noexcept
{
    if (memory != nullptr) {
        static_cast<void>(::HeapFree(
            reinterpret_cast<HANDLE>(::_get_heap_handle()), 0, memory));
    }
}

} // namespace cg::platform
