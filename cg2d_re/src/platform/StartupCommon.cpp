#include "StartupNative.hpp"
#include "LegacyFormat.hpp"

#include "cg/platform/Startup.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053CADC；直接沿MSVC CRT。

namespace cg::platform {

// 原函数：0x004659F0。
int ParseClientVersion()
{
    return std::sscanf("Ver 1.02.02", "Ver %d.%d.%d", &gVersionMajor, &gVersionMinor, &gVersionPatch);
}

// 原函数：0x00465A20。
std::uint32_t CaptureDesktopColor()
{
    const std::uint32_t color = GetSysColor(COLOR_BACKGROUND);
    gDesktopColor = color;
    return color;
}

// 原函数：0x00532E70。
int CreateSaveDirectories()
{
    char directory[256];
    ChangeLegacyDirectory_53DD2F("data");
    for (int i = 0; i < 10; ++i) {
        std::sprintf(directory, "%d", i);
        CreateLegacyDirectory_53C654(directory);
    }
    ChangeLegacyDirectory_53DD2F("..");
    return 1;
}

std::uint32_t ReadLegacyCurrentDirectory(
    const std::uint32_t capacity, char* const destination)
{
    return ::GetCurrentDirectoryA(capacity, destination);
}

void* AllocateLockedZeroedGlobalMemory(const std::size_t bytes)
{
    return ::GlobalLock(::GlobalAlloc(GPTR, bytes));
}

void UnlockGlobalMemory(void* const memory)
{
    ::GlobalUnlock(::GlobalHandle(memory));
}

void FreeGlobalMemory(void* const memory)
{
    ::GlobalFree(::GlobalHandle(memory));
}

void WriteLegacyDebugString(const char* const message)
{
    ::OutputDebugStringA(message);
}

} // namespace cg::platform
