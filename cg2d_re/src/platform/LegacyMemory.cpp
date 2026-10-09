#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <cstring>
#include <cstdlib>

#include <malloc.h>

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
    return std::memmove(destination, source, byteCount);
}

// MC01：填充直接交MSVC memset；值、数量和返回沿调用方接口。
void* FillLegacyMemory_5419D0(
    void* const destination, const std::int32_t value, std::uint32_t byteCount)
{
    return std::memset(destination, value, byteCount);
}

// MC01：比较直接交MSVC memcmp，消费其实际有符号结果。
std::int32_t CompareLegacyMemory_54B160(
    const void* const leftBuffer, const void* const rightBuffer, std::uint32_t byteCount)
{
    return std::memcmp(leftBuffer, rightBuffer, byteCount);
}

void* AllocateLegacyByteBuffer(const std::uint32_t byteCount)
{
    return std::malloc(byteCount);
}

void FreeLegacyByteBuffer(void* const memory) noexcept
{
    std::free(memory);
}

} // namespace cg::platform
