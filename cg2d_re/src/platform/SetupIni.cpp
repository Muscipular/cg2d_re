#include "cg/platform/Startup.hpp"
#include "cg/platform/LegacyTime.hpp"

#include "LegacyFormat.hpp"

#include <cstring>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::platform {

// 原 ReadSetupIni@0x465A80；字节边界和有效缓冲区范围见 doc/cg2d_setup_ini.md。
int ReadSetupIni()
{
    char screenMode[128] {};
    char fileName[256] {};
    char effect[256] {};
    char buffer[256] {};
    char activeMode[256];
    char monitorKeyboard[256];
    char screenCommandLine[4096] {};
    char activeCommandLine[4096];

    // 用户排除屏幕键盘；保留配置读取，生产路径始终使用普通输入。
    gMonitorKeyboard = 0;
    GetCurrentDirectoryA(256, buffer);
    // 0x465AEF 检查固定下标 255，不是当前目录字符串的末字符。
    if (buffer[255] != '\\') {
        std::strcat(buffer, "\\");
    }
    FormatLegacyString_53B304(fileName, "%s%s\\%s", buffer, gDataPath, "CG-Setup.ini");

    GetPrivateProfileStringA("DISPLAY", "ScreenMode", nullptr, screenMode, 128, fileName);
    GetPrivateProfileStringA("DISPLAY", "Effect", nullptr, effect, 256, fileName);
    GetPrivateProfileStringA("DISPLAY", "ActiveMode", nullptr, activeMode, 256, fileName);
    GetPrivateProfileStringA(
        "DISPLAY", "MonitorKeyboard", nullptr, monitorKeyboard, 256, fileName);

    if (std::strcmp(screenMode, "NORMAL") == 0) {
        if (StrStr_53BE40(gCommandLine, "windowmode") == nullptr) {
            FormatLegacyString_53B304(screenCommandLine, "windowmode %s", gCommandLine);
            gCommandLine = screenCommandLine;
        }
    } else if (std::strcmp(screenMode, "FULLSCREEN") == 0
        && StrStr_53BE40(gCommandLine, "windowmode") != nullptr) {
        std::strcpy(screenCommandLine, gCommandLine);
        // 0x465CE3 从复制缓冲区头跳过长度，不使用 strstr 返回的匹配位置。
        gCommandLine = screenCommandLine + std::strlen("windowmode ");
    }

    // 原 EDI 保留 ScreenMode 处理后的指针，供两个 ActiveMode 分支查找。
    char* const commandAfterScreen = gCommandLine;
    if (std::strcmp(activeMode, "ONLYACTIVE") == 0) {
        if (StrStr_53BE40(commandAfterScreen, "onlyactive") == nullptr) {
            FormatLegacyString_53B304(activeCommandLine, "onlyactive %s", gCommandLine);
            gCommandLine = activeCommandLine;
        }
    } else if (std::strcmp(activeMode, "NOACTIVE") == 0
        && StrStr_53BE40(commandAfterScreen, "onlyactive") != nullptr) {
        std::strcpy(activeCommandLine, gCommandLine);
        // 0x465DF8 同样从复制缓冲区头跳过长度；该缓冲区原本没有预清零。
        gCommandLine = activeCommandLine + std::strlen("onlyactive ");
    }

    gRequestedRenderMode = 0;
    if (StrStr_53BE40(effect, "GENERALEFF") != nullptr) {
        gRequestedRenderMode = 1;
    } else if (StrStr_53BE40(effect, "SIMULATEEFF") != nullptr) {
        gRequestedRenderMode = 2;
    } else if (StrStr_53BE40(effect, "ALPHAEFF") != nullptr) {
        gRequestedRenderMode = 3;
    } else if (StrStr_53BE40(effect, "HARDEFF") != nullptr) {
        gRequestedRenderMode = 4;
    }

    if (std::strcmp(monitorKeyboard, "CLOSE") == 0) {
        gMonitorKeyboard = 0;
    }

    const int result = FormatLegacyString_53B304(gCommandLineStorage, "%s", gCommandLine);
    gCommandLine = gCommandLineStorage;
    return result;
}

} // namespace cg::platform
