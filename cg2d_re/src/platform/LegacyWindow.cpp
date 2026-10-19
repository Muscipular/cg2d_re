#include "cg/platform/LegacyWindow.hpp"

#include <Windows.h>

#include <string>

namespace cg::platform {

std::uintptr_t gLegacyWindow = 0; // 原全局：0x009A6F34。

bool IsClientWindowForeground() noexcept
{
    return gLegacyWindow != 0 && GetForegroundWindow() == reinterpret_cast<HWND>(gLegacyWindow);
}

int ShowWideMessage(const std::wstring_view text, const std::wstring_view caption, const std::uint32_t flags)
{
    const std::wstring ownedText(text);
    const std::wstring ownedCaption(caption);
    return MessageBoxW(reinterpret_cast<HWND>(gLegacyWindow), ownedText.c_str(), ownedCaption.c_str(), flags);
}

} // namespace cg::platform
