#include "cg/session/ClientSession.hpp"
#include "../scene/login/LoginSavedState.hpp"
#include "StartupNative.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/resource/ResourcePaths.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/session/ClientHost.hpp"

#include <dbghelp.h>
#include <bcrypt.h>
#include <mmsystem.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B226、0x0053B2F1；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstring> // 原CRT接口：0x0053BE40；原参数／返回合同由直接调用承接。
#include <cwchar>
#include <format>
#include <string>
#include <string_view>
#include <vector>

#include "cg/core/vm.h"

namespace cg::platform {

static bool gClientDiagnosticsEnabled = false; // CTM01/A11：仅WinMain寿命内启用。

enum class LaunchResult { RunClient, SenderComplete, Failed };

// NSM01：仅父子进程交接使用，不是游戏协议；长度单位为UTF-16代码单元，不含末尾NUL。
static constexpr std::size_t kLaunchArgumentCapacity = 32767;
struct LaunchPacket {
    DWORD magic;
    DWORD version;
    DWORD receiverId;
    DWORD argumentLength;
    std::uint64_t nonce;
    volatile LONG received;
    wchar_t arguments[kLaunchArgumentCapacity];
};

// 同EXE两侧各自拥有继承后的句柄/视图；失败也清理参数正文。
struct LaunchHandoff final {
    HANDLE mapping = nullptr;
    HANDLE ready = nullptr;
    LaunchPacket* packet = nullptr;
    std::vector<std::byte> attributeStorage;
    LPPROC_THREAD_ATTRIBUTE_LIST attributes = nullptr;
    PROCESS_INFORMATION child {};
    bool suspended = false;

    LaunchHandoff() = default;
    LaunchHandoff(const LaunchHandoff&) = delete;
    LaunchHandoff& operator=(const LaunchHandoff&) = delete;
    ~LaunchHandoff()
    {
        if (suspended && child.hProcess != nullptr) {
            static_cast<void>(TerminateProcess(child.hProcess, 1));
        }
        if (child.hThread != nullptr) CloseHandle(child.hThread);
        if (child.hProcess != nullptr) CloseHandle(child.hProcess);
        if (attributes != nullptr) DeleteProcThreadAttributeList(attributes);
        if (packet != nullptr) {
            SecureZeroMemory(packet->arguments, sizeof(packet->arguments));
            UnmapViewOfFile(packet);
        }
        if (ready != nullptr) CloseHandle(ready);
        if (mapping != nullptr) CloseHandle(mapping);
    }
};

// token只含两个继承句柄及随机身份，共48位十六进制字符，不含原始参数。
static bool ParseLaunchToken(const std::wstring_view token, std::array<std::uint64_t, 3>& words) noexcept
{
    if (token.size() != 48) return false;
    words.fill(0);
    for (std::size_t index = 0; index < token.size(); ++index) {
        const wchar_t character = token[index];
        const int digit = character >= L'0' && character <= L'9' ? character - L'0'
            : character >= L'A' && character <= L'F' ? character - L'A' + 10
            : character >= L'a' && character <= L'f' ? character - L'a' + 10 : -1;
        if (digit < 0) return false;
        auto& word = words[index / 16];
        word = (word << 4) | static_cast<std::uint64_t>(digit);
    }
    return words[0] != 0 && words[1] != 0 && words[0] != words[1]
        && words[0] != UINT64_MAX && words[1] != UINT64_MAX;
}

static LaunchResult PrepareLaunchCommandLine(const wchar_t* const arguments, const int showCommand)
{
    std::wstring_view command(arguments);
    const auto first = command.find_first_not_of(L" \t");
    command = first == command.npos ? std::wstring_view {} : command.substr(first);
    if (command == L"/token" || command.starts_with(L"/token ") || command.starts_with(L"/token\t")) {
        auto token = command.substr(6);
        const auto tokenFirst = token.find_first_not_of(L" \t");
        if (tokenFirst == token.npos) return LaunchResult::Failed;
        token.remove_prefix(tokenFirst);
        token = token.substr(0, token.find_last_not_of(L" \t") + 1);
        std::array<std::uint64_t, 3> words {};
        if (!ParseLaunchToken(token, words)) return LaunchResult::Failed;
        LaunchHandoff handoff;
        handoff.mapping = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(words[0]));
        handoff.ready = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(words[1]));
        if (!SetHandleInformation(handoff.mapping, HANDLE_FLAG_INHERIT, 0)
            || !SetHandleInformation(handoff.ready, HANDLE_FLAG_INHERIT, 0)) {
            return LaunchResult::Failed;
        }
        handoff.packet = static_cast<LaunchPacket*>(MapViewOfFile(
            handoff.mapping, FILE_MAP_WRITE, 0, 0, sizeof(LaunchPacket)));
        if (handoff.packet == nullptr) return LaunchResult::Failed;
        const auto& packet = *handoff.packet;
        if (packet.magic != 0x314D534Eu || packet.version != 1
            || packet.receiverId != GetCurrentProcessId() || packet.nonce != words[2]
            || packet.received != 0 || packet.argumentLength >= kLaunchArgumentCapacity
            || packet.arguments[packet.argumentLength] != L'\0'
            || std::wmemchr(packet.arguments, L'\0', packet.argumentLength) != nullptr) {
            return LaunchResult::Failed;
        }
        gCommandLine.assign(packet.arguments, packet.argumentLength);
        SecureZeroMemory(handoff.packet->arguments, sizeof(handoff.packet->arguments));
        InterlockedExchange(&handoff.packet->received, 1);
        return SetEvent(handoff.ready) ? LaunchResult::RunClient : LaunchResult::Failed;
    }

    const std::size_t length = std::wcslen(arguments);
    if (length >= kLaunchArgumentCapacity) return LaunchResult::Failed;
    std::wstring executable(MAX_PATH, L'\0');
    for (;;) {
        const DWORD count = GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
        if (count == 0) return LaunchResult::Failed;
        if (count < executable.size()) {
            executable.resize(count);
            break;
        }
        executable.resize(executable.size() * 2);
    }
    LaunchHandoff handoff;
    SECURITY_ATTRIBUTES security {sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
    handoff.mapping = CreateFileMappingW(INVALID_HANDLE_VALUE, &security, PAGE_READWRITE,
        0, static_cast<DWORD>(sizeof(LaunchPacket)), nullptr);
    handoff.ready = CreateEventW(&security, TRUE, FALSE, nullptr);
    if (handoff.mapping == nullptr || handoff.ready == nullptr) return LaunchResult::Failed;
    handoff.packet = static_cast<LaunchPacket*>(MapViewOfFile(
        handoff.mapping, FILE_MAP_WRITE, 0, 0, sizeof(LaunchPacket)));
    if (handoff.packet == nullptr) return LaunchResult::Failed;
    SecureZeroMemory(handoff.packet, sizeof(LaunchPacket));
    handoff.packet->magic = 0x314D534Eu;
    handoff.packet->version = 1;
    handoff.packet->argumentLength = static_cast<DWORD>(length);
    if (BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(&handoff.packet->nonce),
        static_cast<ULONG>(sizeof(handoff.packet->nonce)), BCRYPT_USE_SYSTEM_PREFERRED_RNG) < 0) {
        return LaunchResult::Failed;
    }
    std::wmemcpy(handoff.packet->arguments, arguments, length + 1);
    std::wstring childCommand = std::format(L"\"{}\" /token {:016X}{:016X}{:016X}", executable,
        reinterpret_cast<std::uintptr_t>(handoff.mapping),
        reinterpret_cast<std::uintptr_t>(handoff.ready), handoff.packet->nonce);

    SIZE_T attributeBytes = 0;
    static_cast<void>(InitializeProcThreadAttributeList(nullptr, 1, 0, &attributeBytes));
    if (attributeBytes == 0) return LaunchResult::Failed;
    handoff.attributeStorage.resize(attributeBytes);
    auto* const attributes = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(handoff.attributeStorage.data());
    if (!InitializeProcThreadAttributeList(attributes, 1, 0, &attributeBytes)) return LaunchResult::Failed;
    handoff.attributes = attributes;
    HANDLE inherited[] {handoff.mapping, handoff.ready};
    if (!UpdateProcThreadAttribute(attributes, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        inherited, sizeof(inherited), nullptr, nullptr)) {
        return LaunchResult::Failed;
    }
    STARTUPINFOEXW startup {};
    startup.StartupInfo.cb = static_cast<DWORD>(sizeof(startup));
    startup.StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startup.StartupInfo.wShowWindow = static_cast<WORD>(showCommand);
    startup.lpAttributeList = attributes;
    if (!CreateProcessW(executable.c_str(), childCommand.data(), nullptr, nullptr, TRUE,
        EXTENDED_STARTUPINFO_PRESENT | CREATE_SUSPENDED, nullptr, nullptr,
        &startup.StartupInfo, &handoff.child)) {
        return LaunchResult::Failed;
    }
    handoff.suspended = true;
    handoff.packet->receiverId = handoff.child.dwProcessId;
    if (ResumeThread(handoff.child.hThread) == static_cast<DWORD>(-1)) return LaunchResult::Failed;
    handoff.suspended = false;
    // 只等内存复制或子进程退出，不等窗口、连接或登录；进程退出可解除失败等待。
    const HANDLE completion[] {handoff.ready, handoff.child.hProcess};
    const DWORD result = WaitForMultipleObjects(2, completion, FALSE, INFINITE);
    return result == WAIT_OBJECT_0 && InterlockedCompareExchange(&handoff.packet->received, 1, 1) == 1
        ? LaunchResult::SenderComplete : LaunchResult::Failed;
}

// 借用已连接的控制台；只释放本次AllocConsole成功创建的实例。
class ClientDiagnosticsConsole final {
public:
    explicit ClientDiagnosticsConsole(const std::wstring& commandLine) noexcept
    {
        gClientDiagnosticsEnabled = commandLine.find(L"--debug") != std::wstring::npos;
#ifdef MY_DEBUG
        gClientDiagnosticsEnabled = true;
#endif
        if (!gClientDiagnosticsEnabled) {
            return;
        }
        owned_ = ::AllocConsole() != FALSE;
        if (!owned_ && ::GetConsoleWindow() == nullptr) {
            return;
        }
        std::FILE* stream = nullptr;
        static_cast<void>(::freopen_s(&stream, "CONOUT$", "w", stdout));
        static_cast<void>(::freopen_s(&stream, "CONOUT$", "w", stderr));
    }

    ~ClientDiagnosticsConsole()
    {
        if (owned_) {
            static_cast<void>(std::fflush(stdout));
            static_cast<void>(std::fflush(stderr));
            static_cast<void>(::FreeConsole());
        }
        gClientDiagnosticsEnabled = false;
    }

private:
    bool owned_ = false;
};

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

static constexpr std::size_t kMinidumpPathCapacity = 32768;
static wchar_t gMinidumpDirectory[kMinidumpPathCapacity] {};
static volatile LONG gMinidumpWriting = 0;
static volatile LONG gMinidumpSequence = 0;
static LPTOP_LEVEL_EXCEPTION_FILTER gPreviousExceptionFilter = nullptr;

// 请求在调用线程等待期间保持有效；异常现场属于原异常线程，不属于写入线程。
struct MinidumpRequest {
    DWORD threadId;
    EXCEPTION_POINTERS* exceptionPointers;
    const wchar_t* reason;
};

static DWORD WINAPI WriteClientMinidumpFile(void* const parameter)
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
static void WriteClientMinidump(const wchar_t* const reason, EXCEPTION_POINTERS* const exception)
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

static LONG WINAPI RecordUnhandledClientException(EXCEPTION_POINTERS* const exception)
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

// 保留原0x67资源查询规则供十一CUR选择实例；原EXE只映射为数据，不作为代码装载。
// module及LoadCursor共享句柄归进程寿命，SetCursor借用期间不卸载，见CAC11。
// 保持独立边界，避免动态static初始化被合并进调用方的VM区域。
__declspec(noinline)
static HINSTANCE ResolveWindowResourceInstance(HINSTANCE instance)
{
    // RT_GROUP_ICON=14；资源ID与查询统一使用W接口，避免受UNICODE宏影响。
    if (FindResourceW(instance, MAKEINTRESOURCEW(0x67), MAKEINTRESOURCEW(14)) != nullptr) {
        return instance;
    }

    static const HMODULE resourceModule = [instance]() -> HMODULE {
        std::wstring modulePath(MAX_PATH, L'\0');
        for (;;) {
            const DWORD length = GetModuleFileNameW(
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
        const std::size_t separator = modulePath.find_last_of(L"\\/");
        if (separator == std::wstring::npos) {
            return nullptr;
        }
        modulePath.resize(separator + 1);
        modulePath += L"cg2d.exe";
        return LoadLibraryExW(modulePath.c_str(), nullptr,
            LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    }();
    return resourceModule != nullptr ? resourceModule : instance;
}

} // namespace cg::platform

#ifdef MY_DEBUG
#include <crtdbg.h>

namespace cg::platform {

// Debug断言记录地址并写minidump；不用业务容器/旧格式器。
static int __cdecl RecordDebugAssertionStack(const int reportType, char*, int*)
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

} // namespace cg::platform
#endif

namespace cg::platform {

bool IsClientDiagnosticsEnabled() noexcept
{
    return gClientDiagnosticsEnabled;
}

WNDCLASSW gWindowClass {}; // 原全局：0x009A6ED0。

} // namespace cg::platform

// 原 WinMain@0x465790；保持真实入口函数边界，只有 Win32 句柄与结构采用 x64 SDK ABI。
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previousInstance, LPWSTR commandLine, int showCommand)
{
    using namespace cg::platform;
    LaunchResult launch = LaunchResult::Failed;
    try {
        launch = PrepareLaunchCommandLine(commandLine, showCommand);
    } catch (...) {
        // 交接资源由各自owner回收；错误提示不输出参数正文。
    }
    if (launch != LaunchResult::RunClient) {
        if (launch == LaunchResult::Failed) {
            MessageBoxW(nullptr, L"启动参数交接失败，客户端未启动。", L"确认", MB_OK | MB_ICONERROR);
        }
        return launch == LaunchResult::SenderComplete ? 0 : 1;
    }
    static_cast<void>(ReadTickCount64()); // CTM01：在入口建立统一启动计时基线。

    const ClientTimerResolution timerResolution;
    const ClientMinidumpReport minidumpReport;
#ifdef MY_DEBUG
    const DebugAssertionReport assertionReport;
#endif

    gShowCommand = showCommand;
    gInstance = reinterpret_cast<std::uintptr_t>(instance);

    if (previousInstance == nullptr) {
        gWindowClass.style = 0x1008;
        gWindowClass.lpfnWndProc = WindowProcedure;
        gWindowClass.cbClsExtra = 0;
        gWindowClass.cbWndExtra = 0;
        gWindowClass.hInstance = instance;
        const HINSTANCE resourceInstance = ResolveWindowResourceInstance(instance);
        // app.ico以独立资源ID1内嵌于客户端，光标继续使用原资源实例。
        gWindowClass.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1));

        // 原11个光标ID；最后一项独立读取，不再依赖路径成员布局。
        for (int slot = 0; slot < 11; ++slot) {
            const auto resourceId = static_cast<std::uint16_t>(slot < 10
                ? cg::resource::gCursorPathStorage.cursor[slot].resourceId
                : cg::resource::gCursorPathStorage.lastCursorResourceId);
            gCursorHandles[slot] = reinterpret_cast<std::uintptr_t>(
                LoadCursorW(resourceInstance, MAKEINTRESOURCEW(resourceId)));
        }

        gWindowClass.hCursor = nullptr;
        cg::session::GetClientHost().FirstSession().UiFrame().mouse.requestedCursor = 0;
        cg::session::GetClientHost().FirstSession().UiFrame().mouse.consumedCursor = 0;
        gWindowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        gWindowClass.lpszMenuName = nullptr;
        gWindowClass.lpszClassName = gWindowClassName;
        if (RegisterClassW(&gWindowClass) == 0) {
            return 0;
        }
    }

    // 回调取址和窗口类登记保持原生，避免自动字符串保护重定向代码指针。
    VM_LIGHT_START
    {
        CaptureDesktopColor();
        ParseClientVersion();
        CreateSaveDirectories();
        auto& session = cg::session::GetClientHost().FirstSession();
        cg::login::detail::InitializeSavedState_533300(session);
        ParseCommandOptions(session.Connection());
        const ClientDiagnosticsConsole diagnosticsConsole(gCommandLine);
        if (cg::resource::BuildResourcePaths() != 0) {
            goto END;
        }
        InitializeWindow(session);
        MainLoop(session);
    }
    END:
    VM_LIGHT_END
    return 0;
}
