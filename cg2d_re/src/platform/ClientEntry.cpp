#include "StartupNative.hpp"
#include "LegacyFormat.hpp"
#include "LegacyStdio.hpp"

#include "cg/ResourcePaths.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"

#include <cstring>

#ifdef _DEBUG
#include <crtdbg.h>
#include <cstdio>

namespace {

// Debug断言发生时只记录地址；不用业务容器/旧格式器，避免在越界报告中再次触发它们。
int __cdecl RecordDebugAssertionStack(const int reportType, char*, int*)
{
    if (reportType != _CRT_ASSERT) {
        return 0;
    }
    const DWORD previousError = GetLastError();
    void* frames[64];
    const USHORT frameCount = CaptureStackBackTrace(0, 64, frames, nullptr);
    HANDLE file = CreateFileW(
        L"cg2d_assert_stack.txt", FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        char line[160];
        const auto writeLine = [&file, &line](const int length) {
            if (length > 0 && length < static_cast<int>(sizeof(line))) {
                DWORD written;
                static_cast<void>(WriteFile(file, line, static_cast<DWORD>(length), &written, nullptr));
            }
        };
        writeLine(std::snprintf(
            line, sizeof(line), "CRT ASSERT THREAD=%lu FRAMES=%u IMAGE=%p\r\n",
            GetCurrentThreadId(), static_cast<unsigned int>(frameCount),
            static_cast<void*>(GetModuleHandleW(nullptr))));
        for (USHORT index = 0; index < frameCount; ++index) {
            HMODULE module = nullptr;
            static_cast<void>(GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(frames[index]), &module));
            const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(frames[index]);
            const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(module);
            writeLine(std::snprintf(
                line, sizeof(line), "FRAME %u MODULE=%p RVA=0x%llX ADDRESS=%p\r\n",
                static_cast<unsigned int>(index), static_cast<void*>(module),
                static_cast<unsigned long long>(module != nullptr ? address - base : 0),
                frames[index]));
        }
        static_cast<void>(CloseHandle(file));
    }
    SetLastError(previousError);
    return 0; // 让CRT继续原断言弹窗/终止，不吞断言或改变returnValue。
}

// 回调仅覆盖WinMain寿命；Release不编译/注册，未安装成功也不改变客户端流程。
class DebugAssertionReport final {
public:
    DebugAssertionReport()
        : installed_(_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, RecordDebugAssertionStack) > 0)
    {
    }

    ~DebugAssertionReport()
    {
        if (installed_) {
            static_cast<void>(_CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, RecordDebugAssertionStack));
        }
    }

private:
    bool installed_;
};

} // namespace
#endif

namespace cg::platform {

WNDCLASSA gWindowClass {};

} // namespace cg::platform

// 原 WinMain@0x465790；保持真实入口函数边界，只有 Win32 句柄与结构采用 x64 SDK ABI。
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
    using namespace cg::platform;

#ifdef _DEBUG
    const DebugAssertionReport assertionReport;
#endif

    char fileName[64];
    char line[1028];
    char fileCommandLine[4096];

    gShowCommand = showCommand;
    gInstance = reinterpret_cast<std::uintptr_t>(instance);
    gCommandLine = commandLine;

    if (StrStr_53BE40(commandLine, "CGstart") != nullptr) {
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
    if (StrStr_53BE40(gCommandLine, "updated") == nullptr) {
        ShowUtf8Message("請由「線上遊戲領航員」啟動", "確認", 0x10);
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
