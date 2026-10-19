#include "StartupNative.hpp"

#include "cg/platform/Startup.hpp"

#include <string>
#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053CADC；直接沿MSVC CRT。
#include <direct.h> // 原CRT接口：0x0053DD2F、0x0053C654；原参数／返回合同由直接调用承接。

namespace cg::platform {

// 原函数：0x004659F0。
int ParseClientVersion()
{
    gVersionMajor = 1;
    gVersionMinor = 0;
    gVersionPatch = 0;
    return 0;
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
    ::_wchdir(L"data");
    for (int i = 0; i < 10; ++i) {
        const auto directory = std::to_wstring(i);
        ::_wmkdir(directory.c_str());
    }
    ::_wchdir(L"..");
    return 1;
}

} // namespace cg::platform
