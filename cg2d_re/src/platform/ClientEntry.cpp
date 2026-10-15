#include "StartupNative.hpp"
#include "cg/text/Encoding.hpp"
#include "LegacyFormat.hpp"
#include "LegacyStdio.hpp"

#include "cg/resource/ResourcePaths.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"

#include <dbghelp.h>
#include <mmsystem.h>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B226、0x0053B2F1；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstring> // 原CRT接口：0x0053BE40；原参数／返回合同由直接调用承接。
#include <cwchar>
#include <string>

namespace {

// 客户端活动期申请 1ms 定时器精度；仅释放本次成功获得的请求。
class ClientTimerResolution final {
public:
    ClientTimerResolution() noexcept
        : requested_(::timeBeginPeriod(1) == TIMERR_NOERROR)
    {
    }

    ~ClientTimerResolution()
    {
        if (requested_) {
            static_cast<void>(::timeEndPeriod(1));
        }
    }

private:
    bool requested_; // timeBeginPeriod 成功后由析构函数配对释放。
};

constexpr std::size_t kMinidumpPathCapacity = 32768;
wchar_t gMinidumpDirectory[kMinidumpPathCapacity] {};
volatile LONG gMinidumpWriting = 0;
volatile LONG gMinidumpSequence = 0;
LPTOP_LEVEL_EXCEPTION_FILTER gPreviousExceptionFilter = nullptr;

// 请求在调用线程等待期间保持有效；异常现场属于原异常线程，不属于写入线程。
struct MinidumpRequest {
    DWORD threadId;
    EXCEPTION_POINTERS* exceptionPointers;
    const wchar_t* reason;
};

DWORD WINAPI WriteClientMinidumpFile(void* const parameter)
{
    const auto& request = *static_cast<const MinidumpRequest*>(parameter);
    if (gMinidumpDirectory[0] == L'\0') {
        OutputDebugStringW(L"无法确定 minidump 输出目录。\n");
        return 1;
    }

    SYSTEMTIME time;
    GetLocalTime(&time);
    wchar_t path[kMinidumpPathCapacity];
    const int length = std::swprintf(path, kMinidumpPathCapacity,
        L"%lscg2d_%ls_%04u%02u%02u_%02u%02u%02u_%03u_%lu_%lu_%ld.dmp",
        gMinidumpDirectory, request.reason,
        static_cast<unsigned int>(time.wYear), static_cast<unsigned int>(time.wMonth),
        static_cast<unsigned int>(time.wDay), static_cast<unsigned int>(time.wHour),
        static_cast<unsigned int>(time.wMinute), static_cast<unsigned int>(time.wSecond),
        static_cast<unsigned int>(time.wMilliseconds), GetCurrentProcessId(),
        request.threadId, InterlockedIncrement(&gMinidumpSequence));
    if (length < 0 || static_cast<std::size_t>(length) >= kMinidumpPathCapacity) {
        OutputDebugStringW(L"minidump 文件路径过长。\n");
        return 1;
    }

    HANDLE file = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ,
        nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        OutputDebugStringW(L"无法创建 minidump 文件。\n");
        return 1;
    }

    MINIDUMP_EXCEPTION_INFORMATION exception {};
    exception.ThreadId = request.threadId;
    exception.ExceptionPointers = request.exceptionPointers;
    exception.ClientPointers = FALSE;
    const auto type = static_cast<MINIDUMP_TYPE>(MiniDumpWithDataSegs
        | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithThreadInfo);
    const BOOL written = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
        file, type, request.exceptionPointers != nullptr ? &exception : nullptr,
        nullptr, nullptr);
    static_cast<void>(CloseHandle(file));
    OutputDebugStringW(written ? L"minidump 已写入：" : L"minidump 写入失败：");
    OutputDebugStringW(path);
    OutputDebugStringW(L"\n");
    return written ? 0 : 1;
}

// DbgHelp不支持并发调用；用独立线程写入，保留当前断言/异常线程的栈。
void WriteClientMinidump(const wchar_t* const reason, EXCEPTION_POINTERS* const exception)
{
    const DWORD previousError = GetLastError();
    if (InterlockedCompareExchange(&gMinidumpWriting, 1, 0) != 0) {
        return;
    }
    MinidumpRequest request {GetCurrentThreadId(), exception, reason};
    HANDLE thread = CreateThread(nullptr, 0, WriteClientMinidumpFile, &request, 0, nullptr);
    if (thread != nullptr) {
        static_cast<void>(WaitForSingleObject(thread, INFINITE));
        static_cast<void>(CloseHandle(thread));
    } else {
        static_cast<void>(WriteClientMinidumpFile(&request));
    }
    InterlockedExchange(&gMinidumpWriting, 0);
    SetLastError(previousError);
}

LONG WINAPI RecordUnhandledClientException(EXCEPTION_POINTERS* const exception)
{
    WriteClientMinidump(L"crash", exception);
    return gPreviousExceptionFilter != nullptr
        ? gPreviousExceptionFilter(exception) : EXCEPTION_CONTINUE_SEARCH;
}

// 在真实WinMain活动期登记；退出恢复原过滤器，转储不吞掉原异常。
class ClientMinidumpReport final {
public:
    ClientMinidumpReport()
    {
        const DWORD length = GetModuleFileNameW(nullptr, gMinidumpDirectory,
            static_cast<DWORD>(kMinidumpPathCapacity));
        wchar_t* const separator = length != 0 && length < kMinidumpPathCapacity
            ? std::wcsrchr(gMinidumpDirectory, L'\\') : nullptr;
        if (separator != nullptr) {
            separator[1] = L'\0';
        } else {
            gMinidumpDirectory[0] = L'\0';
        }
        gPreviousExceptionFilter = SetUnhandledExceptionFilter(RecordUnhandledClientException);
    }

    ~ClientMinidumpReport()
    {
        static_cast<void>(SetUnhandledExceptionFilter(gPreviousExceptionFilter));
    }
};

// 原0x67/十一CUR由同一资源实例提供；原EXE只映射为数据，不作为代码装载。
// module及LoadIcon/LoadCursor共享句柄归进程寿命，窗口类/SetCursor借用期间不卸载，见CAC11。
HINSTANCE ResolveWindowResourceInstance(HINSTANCE instance)
{
    // RT_GROUP_ICON=14；资源ID与查询统一使用W接口，避免受UNICODE宏影响。
    if (FindResourceW(instance, MAKEINTRESOURCEW(0x67), MAKEINTRESOURCEW(14)) != nullptr) {
        return instance;
    }

    static const HMODULE resourceModule = [instance]() -> HMODULE {
        std::string modulePath(MAX_PATH, '\0');
        for (;;) {
            const DWORD length = GetModuleFileNameA(
                instance, modulePath.data(), static_cast<DWORD>(modulePath.size()));
            if (length == 0) {
                return nullptr;
            }
            if (length < modulePath.size()) {
                modulePath.resize(length);
                break;
            }
            modulePath.resize(modulePath.size() * 2);
        }
        const std::size_t separator = modulePath.find_last_of("\\/");
        if (separator == std::string::npos) {
            return nullptr;
        }
        modulePath.resize(separator + 1);
        modulePath += "cg2d.exe";
        return LoadLibraryExA(modulePath.c_str(), nullptr,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    }();
    return resourceModule != nullptr ? resourceModule : instance;
}

} // namespace

#ifdef _DEBUG
#include <crtdbg.h>

namespace {

// Debug断言记录地址并写minidump；不用业务容器/旧格式器。
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
    WriteClientMinidump(L"assert", nullptr);
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

WNDCLASSW gWindowClass {}; // 原全局：0x009A6ED0。

} // namespace cg::platform

// 原 WinMain@0x465790；保持真实入口函数边界，只有 Win32 句柄与结构采用 x64 SDK ABI。
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
    using namespace cg::platform;

    const ClientTimerResolution timerResolution;
    const ClientMinidumpReport minidumpReport;
#ifdef _DEBUG
    const DebugAssertionReport assertionReport;
#endif

    char fileName[64];
    char line[1028];
    char fileCommandLine[4096];

    gShowCommand = showCommand;
    gInstance = reinterpret_cast<std::uintptr_t>(instance);
    gCommandLine = commandLine;

    if (const_cast<char*>(std::strstr(commandLine, "CGstart")) != nullptr) {
        std::memset(fileCommandLine, 0, sizeof(fileCommandLine));
        std::memset(fileName, 0, sizeof(fileName));
        std::int32_t total = 0;
        std::sprintf(fileName, "%s", ".\\data\\argument.ini");
        LegacyFile* file = std::fopen(fileName, "r");
        if (file != nullptr) {
            do {
                std::memset(line, 0, 1025);
                std::fgets(line, 1024, file);
                total += static_cast<std::int32_t>(std::strlen(line));
                if (total >= 4096) {
                    break;
                }
                std::strcat(fileCommandLine, line);
            } while (std::feof(file) == 0);
            std::fclose(file);
        }
        gCommandLine = fileCommandLine;
    }

    // 原代码不检查 ERROR_ALREADY_EXISTS，也不在 WinMain 中释放该句柄。
    gStartupMutex = reinterpret_cast<std::uintptr_t>(CreateMutexA(nullptr, TRUE, "XZYDRaEvyPQC"));
    if (const_cast<char*>(std::strstr(gCommandLine, "updated")) == nullptr) {
        ShowWideMessage(L"請由「線上遊戲領航員」啟動", L"確認", 0x10);
        return 0;
    }

    if (previousInstance == nullptr) {
        gWindowClass.style = 0x1008;
        gWindowClass.lpfnWndProc = WindowProcedure;
        gWindowClass.cbClsExtra = 0;
        gWindowClass.cbWndExtra = 0;
        gWindowClass.hInstance = instance;
        const HINSTANCE resourceInstance = ResolveWindowResourceInstance(instance);
        gWindowClass.hIcon = LoadIconW(resourceInstance, MAKEINTRESOURCEW(0x67));

        // 原步长为 12，共读取 11 个 ID；通过完整对象的字节视图保留最后一条的重叠布局。
        const auto* cursorBytes = reinterpret_cast<const unsigned char*>(&cg::resource::gCursorPathStorage);
        for (int slot = 0; slot < 11; ++slot) {
            std::uint16_t resourceId;
            std::memcpy(&resourceId, cursorBytes + slot * 12, sizeof(resourceId));
            gCursorHandles[slot] = reinterpret_cast<std::uintptr_t>(
                LoadCursorW(resourceInstance, MAKEINTRESOURCEW(resourceId)));
        }

        gWindowClass.hCursor = nullptr;
        gValueD68DC4 = 0;
        gValue9A6F44 = 0;
        gWindowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        gWindowClass.lpszMenuName = nullptr;
        gWindowClass.lpszClassName = gWindowClassName;
        if (RegisterClassW(&gWindowClass) == 0) {
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
