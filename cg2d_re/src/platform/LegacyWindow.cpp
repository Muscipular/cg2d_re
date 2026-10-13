#include "cg/platform/LegacyWindow.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>

#include <string>

namespace cg::platform {

std::uintptr_t gLegacyWindow = 0; // 原全局：0x009A6F34。

int ShowLegacyMessage(const char* text, const char* caption, std::uint32_t flags)
{
    return MessageBoxA(reinterpret_cast<HWND>(gLegacyWindow), text, caption, flags);
}

int ShowUtf8Message(const char* text, const char* caption, std::uint32_t flags)
{
    const auto convert = [](const char* input, std::wstring& output) {
        const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, input, -1, nullptr, 0);
        if (length == 0) {
            return false;
        }
        output.resize(static_cast<std::size_t>(length));
        return MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS, input, -1, output.data(), length) != 0;
    };

    std::wstring wideText;
    std::wstring wideCaption;
    if (!convert(text, wideText) || !convert(caption, wideCaption)) {
        return 0;
    }
    return MessageBoxW(
        reinterpret_cast<HWND>(gLegacyWindow), wideText.c_str(), wideCaption.c_str(), flags);
}

std::int32_t ShowLegacyCursor(const std::int32_t show)
{
    return static_cast<std::int32_t>(::ShowCursor(show));
}

std::int32_t SetLegacyWindowText(const std::uintptr_t window, const char* const text)
{
    return static_cast<std::int32_t>(
        ::SetWindowTextA(reinterpret_cast<HWND>(window), text));
}

std::int32_t PostLegacyWindowMessage(
    const std::uintptr_t window,
    const std::uint32_t message,
    const std::uintptr_t wParam,
    const std::intptr_t lParam)
{
    return static_cast<std::int32_t>(::PostMessageA(
        reinterpret_cast<HWND>(window),
        static_cast<UINT>(message),
        static_cast<WPARAM>(wParam),
        static_cast<LPARAM>(lParam)));
}

void PostLegacyQuitMessage(const std::int32_t exitCode)
{
    ::PostQuitMessage(exitCode);
}

std::intptr_t ExecuteLegacyShell(
    const std::uintptr_t window,
    const char* const operation,
    const char* const file,
    const char* const parameters,
    const char* const directory,
    const std::int32_t showCommand)
{
    return reinterpret_cast<std::intptr_t>(::ShellExecuteA(
        reinterpret_cast<HWND>(window),
        operation,
        file,
        parameters,
        directory,
        static_cast<int>(showCommand)));
}

} // namespace cg::platform
