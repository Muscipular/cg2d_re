#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <cstring>

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
        if (std::strncmp(kGlobalHeapSelection54FAD4, environment, 22) == 0) {
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
            selection = std::strstr(environment, filename);
        }
        if (selection != nullptr) {
            char* const comma = std::strchr(selection, ',');
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
