#include "StartupNative.hpp"
#include "LegacyFormat.hpp"
#include "LegacyStdio.hpp"

#include "cg/ResourcePaths.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"

#include <cstring>

namespace cg::platform {

WNDCLASSA gWindowClass {};

} // namespace cg::platform

// 原 WinMain@0x465790；保持真实入口函数边界，只有 Win32 句柄与结构采用 x64 SDK ABI。
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
    using namespace cg::platform;

    char fileName[64];
    char line[1028];
    char fileCommandLine[4096];

    gShowCommand = showCommand;
    gInstance = reinterpret_cast<std::uintptr_t>(instance);
    gCommandLine = commandLine;

    if (std::strstr(commandLine, "CGstart") != nullptr) {
        std::memset(fileCommandLine, 0, sizeof(fileCommandLine));
        std::memset(fileName, 0, sizeof(fileName));
        std::int32_t total = 0;
        FormatLegacyString_53B304(fileName, "%s", ".\\data\\argument.ini");
        LegacyFile* file = OpenLegacyStream_53B2F1(fileName, "r");
        if (file != nullptr) {
            do {
                std::memset(line, 0, 1025);
                ReadLegacyStreamLine_53B226(line, 1024, file);
                total += static_cast<std::int32_t>(std::strlen(line));
                if (total >= 4096) {
                    break;
                }
                std::strcat(fileCommandLine, line);
            } while ((static_cast<std::uint8_t>(file->flags) & 0x10u) == 0);
            CloseLegacyStream_53B1D0(file);
        }
        gCommandLine = fileCommandLine;
    }

    // 原代码不检查 ERROR_ALREADY_EXISTS，也不在 WinMain 中释放该句柄。
    gStartupMutex = reinterpret_cast<std::uintptr_t>(CreateMutexA(nullptr, TRUE, "XZYDRaEvyPQC"));
    if (std::strstr(gCommandLine, "updated") == nullptr) {
        ShowLegacyMessage(
            "\xBD\xD0\xA5\xD1\xA1\x75\xBD\x75\xA4\x57\xB9\x43\xC0\xB8\xBB\xE2"
            "\xAF\xE8\xAD\xFB\xA1\x76\xB1\xD2\xB0\xCA",
            "\xBD\x54\xBB\x7B", 0x10);
        return 0;
    }

    if (previousInstance == nullptr) {
        gWindowClass.style = 0x1008;
        gWindowClass.lpfnWndProc = WindowProcedure;
        gWindowClass.cbClsExtra = 0;
        gWindowClass.cbWndExtra = 0;
        gWindowClass.hInstance = instance;
        gWindowClass.hIcon = LoadIconA(instance, MAKEINTRESOURCEA(0x67));

        // 原步长为 12，共读取 11 个 ID；通过完整对象的字节视图保留最后一条的重叠布局。
        const auto* cursorBytes = reinterpret_cast<const unsigned char*>(&cg::resource::gCursorPathStorage);
        for (int slot = 0; slot < 11; ++slot) {
            std::uint16_t resourceId;
            std::memcpy(&resourceId, cursorBytes + slot * 12, sizeof(resourceId));
            gCursorHandles[slot] = reinterpret_cast<std::uintptr_t>(
                LoadCursorA(instance, MAKEINTRESOURCEA(resourceId)));
        }

        gWindowClass.hCursor = nullptr;
        gValueD68DC4 = 0;
        gValue9A6F44 = 0;
        gWindowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        gWindowClass.lpszMenuName = nullptr;
        gWindowClass.lpszClassName = gWindowClassName;
        if (RegisterClassA(&gWindowClass) == 0) {
            return 0;
        }
    }

    CaptureDesktopColor();
    ReadSetupIni();
    ParseClientVersion();
    CreateSaveDirectories();
    InitializeSavedState_533300();
    ParseCommandOptions();
    cg::resource::BuildResourcePaths();
    InitializeWindow();
    MainLoop();
    return 0;
}
