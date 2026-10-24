#include "StartupNative.hpp"

#include "cg/platform/Startup.hpp"

#include <string>
#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053CADC；直接沿MSVC CRT。
#include <direct.h> // 原CRT接口：0x0053DD2F、0x0053C654；原参数／返回合同由直接调用承接。

#include "cg/core/vm.h"

namespace cg::platform {

// 原函数：0x004659F0。
__declspec(noinline)
int ParseClientVersion()
{
    VM_LIGHT_START
    gVersionMajor = 1;
    gVersionMinor = 0;
    gVersionPatch = 0;
    VM_LIGHT_END
    return 0;
}

// 原函数：0x00465A20。
__declspec(noinline)
std::uint32_t CaptureDesktopColor()
{
    VM_LIGHT_START
    const std::uint32_t color = GetSysColor(COLOR_BACKGROUND);
    gDesktopColor = color;
    VM_LIGHT_END
    return color;
}

// 原函数：0x00532E70。
__declspec(noinline)
int CreateSaveDirectories()
{
    VM_LIGHT_START
    ::_wchdir(L"data");
    for (int i = 0; i < 10; ++i) {
        const auto directory = std::to_wstring(i);
        ::_wmkdir(directory.c_str());
    }
    ::_wchdir(L"..");
    VM_LIGHT_END
    return 1;
}

} // namespace cg::platform
