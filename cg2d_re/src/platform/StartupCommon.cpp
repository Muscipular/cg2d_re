#include "StartupNative.hpp"

#include "cg/platform/Startup.hpp"

#include <cstdio>
#include <direct.h>

namespace cg::platform {

int ParseClientVersion()
{
    return std::sscanf("Ver 1.02.02", "Ver %d.%d.%d", &gVersionMajor, &gVersionMinor, &gVersionPatch);
}

std::uint32_t CaptureDesktopColor()
{
    const std::uint32_t color = GetSysColor(COLOR_BACKGROUND);
    gDesktopColor = color;
    return color;
}

int CreateSaveDirectories()
{
    char directory[256];
    _chdir("data");
    for (int i = 0; i < 10; ++i) {
        std::sprintf(directory, "%d", i);
        _mkdir(directory);
    }
    _chdir("..");
    return 1;
}

} // namespace cg::platform
