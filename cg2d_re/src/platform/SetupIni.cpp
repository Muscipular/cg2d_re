#include "cg/platform/Startup.hpp"

#include <cstdio>
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
    std::sprintf(fileName, "%s%s\\%s", buffer, gDataPath, "CG-Setup.ini");

    GetPrivateProfileStringA("DISPLAY", "ScreenMode", nullptr, screenMode, 128, fileName);
    GetPrivateProfileStringA("DISPLAY", "Effect", nullptr, effect, 256, fileName);
    GetPrivateProfileStringA("DISPLAY", "ActiveMode", nullptr, activeMode, 256, fileName);
    GetPrivateProfileStringA(
        "DISPLAY", "MonitorKeyboard", nullptr, monitorKeyboard, 256, fileName);

    if (std::strcmp(screenMode, "NORMAL") == 0) {
        if (std::strstr(gCommandLine, "windowmode") == nullptr) {
            std::sprintf(screenCommandLine, "windowmode %s", gCommandLine);
            gCommandLine = screenCommandLine;
        }
    } else if (std::strcmp(screenMode, "FULLSCREEN") == 0
        && std::strstr(gCommandLine, "windowmode") != nullptr) {
        std::strcpy(screenCommandLine, gCommandLine);
        // 0x465CE3 从复制缓冲区头跳过长度，不使用 strstr 返回的匹配位置。
        gCommandLine = screenCommandLine + std::strlen("windowmode ");
    }

    // 原 EDI 保留 ScreenMode 处理后的指针，供两个 ActiveMode 分支查找。
    char* const commandAfterScreen = gCommandLine;
    if (std::strcmp(activeMode, "ONLYACTIVE") == 0) {
        if (std::strstr(commandAfterScreen, "onlyactive") == nullptr) {
            std::sprintf(activeCommandLine, "onlyactive %s", gCommandLine);
            gCommandLine = activeCommandLine;
        }
    } else if (std::strcmp(activeMode, "NOACTIVE") == 0
        && std::strstr(commandAfterScreen, "onlyactive") != nullptr) {
        std::strcpy(activeCommandLine, gCommandLine);
        // 0x465DF8 同样从复制缓冲区头跳过长度；该缓冲区原本没有预清零。
        gCommandLine = activeCommandLine + std::strlen("onlyactive ");
    }

    gRequestedRenderMode = 0;
    if (std::strstr(effect, "GENERALEFF") != nullptr) {
        gRequestedRenderMode = 1;
    } else if (std::strstr(effect, "SIMULATEEFF") != nullptr) {
        gRequestedRenderMode = 2;
    } else if (std::strstr(effect, "ALPHAEFF") != nullptr) {
        gRequestedRenderMode = 3;
    } else if (std::strstr(effect, "HARDEFF") != nullptr) {
        gRequestedRenderMode = 4;
    }

    if (std::strcmp(monitorKeyboard, "CLOSE") == 0) {
        gMonitorKeyboard = 0;
    }

    const int result = std::sprintf(gCommandLineStorage, "%s", gCommandLine);
    gCommandLine = gCommandLineStorage;
    return result;
}

} // namespace cg::platform
