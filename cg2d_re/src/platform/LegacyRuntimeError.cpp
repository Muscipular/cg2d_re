#include "LegacyRuntimeError.hpp"

#include <cstddef>
#include <cstring>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace cg::platform {

std::int32_t gLegacyRuntimeErrorMode1086038 = 0;
std::int32_t gLegacyRuntimeApplicationType58F8A4 = 2;
LegacyRuntimeBannerHook gLegacyRuntimeBannerHook10862A8 = nullptr;

namespace {

// 原592128的W32错误码/消息指针对；只有地址拓宽，末项仍参与最终码值比较。
struct RuntimeErrorMessage final {
    std::int32_t code;
    const char* text;
};
RuntimeErrorMessage gRuntimeErrorMessages592128[] = {
    {2, "R6002\r\n- floating point not loaded\r\n"},
    {8, "R6008\r\n- not enough space for arguments\r\n"},
    {9, "R6009\r\n- not enough space for environment\r\n"},
    {10, "\r\nabnormal program termination\r\n"},
    {16, "R6016\r\n- not enough space for thread data\r\n"},
    {17, "R6017\r\n- unexpected multithread lock error\r\n"},
    {18, "R6018\r\n- unexpected heap error\r\n"},
    {19, "R6019\r\n- unable to open console device\r\n"},
    {24, "R6024\r\n- not enough space for _onexit/atexit table\r\n"},
    {25, "R6025\r\n- pure virtual function call\r\n"},
    {26, "R6026\r\n- not enough space for stdio initialization\r\n"},
    {27, "R6027\r\n- not enough space for lowio initialization\r\n"},
    {28, "R6028\r\n- unable to initialize heap\r\n"},
    {120, "DOMAIN error\r\n"},
    {121, "SING error\r\n"},
    {122, "TLOSS error\r\n"},
    {252, "\r\n"},
    {255, "runtime error "},
    {0, nullptr},
};
static_assert(sizeof(gRuntimeErrorMessages592128) / sizeof(RuntimeErrorMessage) == 19);

// 原10862EC/F0/F4的唯一API缓存；模块引用由原加载流程保留，不另外卸载。
decltype(&::MessageBoxA) gRuntimeMessageBox10862EC = nullptr;
decltype(&::GetActiveWindow) gRuntimeGetActiveWindow10862F0 = nullptr;
decltype(&::GetLastActivePopup) gRuntimeGetLastActivePopup10862F4 = nullptr;

// 原5443FF的局部连续420字节；不预清API未写部分。
struct RuntimeErrorText final {
    char filename[260];
    char message[160];
};
static_assert(offsetof(RuntimeErrorText, message) == 260);
static_assert(sizeof(RuntimeErrorText) == 420);

} // namespace

std::int32_t ShowLegacyRuntimeMessage_546D0C(
    const char* const text, const char* const caption, const std::uint32_t flags)
{
    if (gRuntimeMessageBox10862EC == nullptr) {
        const HMODULE module = ::LoadLibraryA("user32.dll");
        if (module == nullptr) {
            return 0;
        }
        const auto getProcedure = &::GetProcAddress;
        const auto messageBox = reinterpret_cast<decltype(gRuntimeMessageBox10862EC)>(
            getProcedure(module, "MessageBoxA"));
        gRuntimeMessageBox10862EC = messageBox;
        if (messageBox == nullptr) {
            return 0;
        }
        gRuntimeGetActiveWindow10862F0 =
            reinterpret_cast<decltype(gRuntimeGetActiveWindow10862F0)>(
                getProcedure(module, "GetActiveWindow"));
        gRuntimeGetLastActivePopup10862F4 =
            reinterpret_cast<decltype(gRuntimeGetLastActivePopup10862F4)>(
                getProcedure(module, "GetLastActivePopup"));
    }

    HWND owner = nullptr;
    const auto getActiveWindow = gRuntimeGetActiveWindow10862F0;
    if (getActiveWindow != nullptr) {
        owner = getActiveWindow();
        if (owner != nullptr) {
            const auto getLastActivePopup = gRuntimeGetLastActivePopup10862F4;
            if (getLastActivePopup != nullptr) {
                owner = getLastActivePopup(owner);
            }
        }
    }
    return gRuntimeMessageBox10862EC(owner, text, caption, flags);
}

void ReportLegacyRuntimeError_5443FF(const std::int32_t errorCode)
{
    std::uint32_t index = 0;
    while (index < 18 && gRuntimeErrorMessages592128[index].code != errorCode) {
        ++index;
    }
    if (gRuntimeErrorMessages592128[index].code != errorCode) {
        return;
    }

    const std::int32_t errorMode = gLegacyRuntimeErrorMode1086038;
    if (errorMode == 1
        || (errorMode == 0 && gLegacyRuntimeApplicationType58F8A4 == 1)) {
        // 原API写入参数的W32槽；初值保持传入错误码位值，不预置零。
        DWORD bytesWritten = static_cast<DWORD>(errorCode);
        const DWORD length = static_cast<DWORD>(
            std::strlen(gRuntimeErrorMessages592128[index].text));
        const char* const text = gRuntimeErrorMessages592128[index].text;
        const HANDLE standardError = ::GetStdHandle(STD_ERROR_HANDLE);
        static_cast<void>(::WriteFile(
            standardError, text, length, &bytesWritten, nullptr));
        return;
    }
    if (errorCode == 252) {
        return;
    }

    RuntimeErrorText buffers;
    if (::GetModuleFileNameA(nullptr, buffers.filename, 260) == 0) {
        std::strcpy(buffers.filename, "<program name unknown>");
    }
    char* program = buffers.filename;
    if (std::strlen(buffers.filename) + 1 > 60) {
        program = buffers.filename + std::strlen(buffers.filename) - 59;
        std::strncpy(program, "...", 3);
    }
    std::strcpy(buffers.message, "Runtime Error!\n\nProgram: ");
    std::strcat(buffers.message, program);
    std::strcat(buffers.message, "\n\n");
    std::strcat(buffers.message, gRuntimeErrorMessages592128[index].text);
    static_cast<void>(ShowLegacyRuntimeMessage_546D0C(
        buffers.message, "Microsoft Visual C++ Runtime Library", 0x12010));
}

void ReportLegacyRuntimeBanner_5443C6()
{
    const std::int32_t errorMode = gLegacyRuntimeErrorMode1086038;
    if (errorMode != 1
        && (errorMode != 0 || gLegacyRuntimeApplicationType58F8A4 != 1)) {
        return;
    }
    ReportLegacyRuntimeError_5443FF(252);
    const auto hook = gLegacyRuntimeBannerHook10862A8;
    if (hook != nullptr) {
        hook();
    }
    ReportLegacyRuntimeError_5443FF(255);
}

[[noreturn]] void FastLegacyRuntimeErrorExit_53DED0(const std::int32_t errorCode)
{
    if (gLegacyRuntimeErrorMode1086038 == 1) {
        ReportLegacyRuntimeBanner_5443C6();
    }
    ReportLegacyRuntimeError_5443FF(errorCode);
    ::ExitProcess(255);
}

} // namespace cg::platform
