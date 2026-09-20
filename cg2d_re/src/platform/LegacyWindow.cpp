#include "cg/platform/LegacyWindow.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::platform {

std::uintptr_t gLegacyWindow = 0;

int ShowLegacyMessage(const char* text, const char* caption, std::uint32_t flags)
{
    return MessageBoxA(reinterpret_cast<HWND>(gLegacyWindow), text, caption, flags);
}

std::int32_t ShowLegacyCursor(const std::int32_t show)
{
    return static_cast<std::int32_t>(::ShowCursor(show));
}

} // namespace cg::platform
