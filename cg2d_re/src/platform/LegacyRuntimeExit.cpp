#include "LegacyRuntimeExit.hpp"
#include "LegacyRuntimeError.hpp"
#include "LegacyStdio.hpp"
#include "LegacyHeap.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "StartupNative.hpp"
#include "LegacyLowIo.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyFloatEnvironment.hpp"
#include "LegacySignal.hpp"
#include "cg/platform/CommandOptions.hpp"

#include <cstddef>
#include <bit>
#include <cstdlib>
#include <malloc.h>

// 复用ClientEntry.cpp已存在的全局WinMain签名；只声明，不改其本体或启用新启动。
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand);

namespace cg::platform {

// 原PE .data虚拟尾部载入填零；只给真实四W32槽初始存储，不启用启动入口。
std::uint32_t gLegacyRuntimeOsMinor1085FF8 = 0u;
std::uint32_t gLegacyRuntimeOsMajor1085FF4 = 0u;
std::uint32_t gLegacyRuntimeOsCombined1085FF0 = 0u;
std::uint32_t gLegacyRuntimeOsBuild1085FEC = 0u;

std::uint8_t gLegacyRuntimeExitCallable1086020 = 0;
std::int32_t gLegacyRuntimeExitStarted1086024 = 0;
std::int32_t gLegacyRuntimeExitComplete1086028 = 0;
LegacyRuntimeExitCallback* gLegacyRuntimeExitFirst1087AF4 = nullptr;
LegacyRuntimeExitCallback* gLegacyRuntimeExitLast1087AF0 = nullptr;

namespace {

// ETH01仅承接兼容退出表块；原请求、new-mode/handler与发布顺序仍由本组保留。
LegacyRuntimeExitCallback* AllocateLegacyExitTableStorage(const std::uint32_t byteCount)
{
    const std::int32_t enableHandler = gLegacyHeapNewMode1086150;
    if (byteCount > 0xFFFFFFE0u) {
        return nullptr;
    }
    const std::uint32_t requested = byteCount == 0 ? 1u : byteCount;
    for (;;) {
        auto* const result = static_cast<LegacyRuntimeExitCallback*>(std::malloc(requested));
        if (result != nullptr || enableHandler == 0) {
            return result;
        }
        if (CallLegacyHeapNewHandler_541A28(byteCount) == 0) {
            return nullptr;
        }
    }
}

LegacyRuntimeExitCallback* ResizeLegacyExitTableStorage(
    LegacyRuntimeExitCallback* const memory, const std::uint32_t byteCount)
{
    if (memory == nullptr) {
        return AllocateLegacyExitTableStorage(byteCount);
    }
    if (byteCount == 0) {
        std::free(memory);
        return nullptr;
    }
    for (;;) {
        LegacyRuntimeExitCallback* result = nullptr;
        if (byteCount <= 0xFFFFFFE0u) {
            result = static_cast<LegacyRuntimeExitCallback*>(std::realloc(memory, byteCount));
        }
        if (result != nullptr || gLegacyHeapNewMode1086150 == 0) {
            return result;
        }
        if (CallLegacyHeapNewHandler_541A28(byteCount) == 0) {
            return nullptr;
        }
    }
}

// 必须查询同一宿主块的实际容量；不以申请数/已用槽替代，不读旧块头。
std::uint32_t GetLegacyExitTableStorageCapacity(LegacyRuntimeExitCallback* const memory)
{
    return static_cast<std::uint32_t>(::_msize(memory));
}

// 原[553184,5531A0)七槽；第一组[0,3)，第二组[4,6)，保留全部零哨兵。
LegacyRuntimeExitCallback gLegacyTerminationTable553184[] = {
    nullptr,
    FinishLegacyStdio_53EA4A,
    CloseLegacyConsole_54B20C,
    nullptr,
    nullptr,
    RestoreLegacyExceptionFilter_545091,
    nullptr
};

// 原两个已加载HANDLE哨兵均为-2；只在Win32实现边界采用原生句柄。
HANDLE gLegacyConsoleHandle592FA4 = reinterpret_cast<HANDLE>(std::intptr_t{-2});
HANDLE gLegacyConsoleHandle592FA0 = reinterpret_cast<HANDLE>(std::intptr_t{-2});
LPTOP_LEVEL_EXCEPTION_FILTER gLegacyPreviousExceptionFilter10862E0 = nullptr;

} // namespace

LegacyRuntimeExitCallback RegisterLegacyExitCallback_53B399(
    LegacyRuntimeExitCallback const callback)
{
    constexpr auto kSlotBytes =
        static_cast<std::uint32_t>(sizeof(LegacyRuntimeExitCallback));
    const std::uint32_t capacity =
        GetLegacyExitTableStorageCapacity(gLegacyRuntimeExitFirst1087AF4);
    auto* const first = gLegacyRuntimeExitFirst1087AF4;
    auto* last = gLegacyRuntimeExitLast1087AF0;
    const auto required = reinterpret_cast<std::uintptr_t>(last)
        - reinterpret_cast<std::uintptr_t>(first) + kSlotBytes;
    if (capacity < required) {
        const std::uint32_t oldCapacity = GetLegacyExitTableStorageCapacity(first);
        const std::uint32_t newCapacity = oldCapacity + 4u * kSlotBytes;
        auto* const resized = static_cast<LegacyRuntimeExitCallback*>(
            ResizeLegacyExitTableStorage(gLegacyRuntimeExitFirst1087AF4, newCapacity));
        if (resized == nullptr) {
            return nullptr;
        }
        // 原53B3DB在扩容返回后重读共享首末；旧块地址只参与整数差值。
        const auto byteOffset = static_cast<std::intptr_t>(
            reinterpret_cast<std::uintptr_t>(gLegacyRuntimeExitLast1087AF0)
            - reinterpret_cast<std::uintptr_t>(gLegacyRuntimeExitFirst1087AF4));
        gLegacyRuntimeExitFirst1087AF4 = resized;
        last = resized + byteOffset / static_cast<std::intptr_t>(kSlotBytes);
        gLegacyRuntimeExitLast1087AF0 = last;
    }
    *last = callback;
    ++gLegacyRuntimeExitLast1087AF0;
    return callback;
}

std::int32_t RegisterLegacyAtExit_53B406(LegacyRuntimeExitCallback const callback)
{
    return RegisterLegacyExitCallback_53B399(callback) != nullptr ? 0 : -1;
}

LegacyRuntimeExitCallback* InitializeLegacyExitTable_53B418()
{
    constexpr std::uint32_t kInitialSlotCount = 32;
    auto* slots = static_cast<LegacyRuntimeExitCallback*>(
        AllocateLegacyExitTableStorage(kInitialSlotCount
            * static_cast<std::uint32_t>(sizeof(LegacyRuntimeExitCallback))));
    gLegacyRuntimeExitFirst1087AF4 = slots;
    if (slots == nullptr) {
        ExitLegacyRuntimeError_53DEAB(24);
        slots = gLegacyRuntimeExitFirst1087AF4;
    }
    *slots = nullptr;
    slots = gLegacyRuntimeExitFirst1087AF4;
    gLegacyRuntimeExitLast1087AF0 = slots;
    return slots;
}

namespace {

// 原5405F5：仅按异常代码筛选，命中原C++代码即进入原终止，不加参数数目/版本检查。
std::int32_t FilterLegacyArrayUnwindException_5405F5(_EXCEPTION_POINTERS* const information)
{
    if (information->ExceptionRecord->ExceptionCode == 0xE06D7363u) {
        TerminateLegacyRuntime_540A5C();
    }
    return 0;
}

} // namespace

void UnwindLegacyArray_53C560(
    void* const onePast, const std::uint32_t elementBytes, const std::int32_t elementCount,
    const LegacyArrayElementCallback destructor)
{
    auto* current = static_cast<std::byte*>(onePast);
    std::uint32_t remaining = std::bit_cast<std::uint32_t>(elementCount);
    __try {
        for (;;) {
            --remaining;
            if (std::bit_cast<std::int32_t>(remaining) < 0) {
                break;
            }
            current -= elementBytes;
            destructor(current);
        }
    } __except (FilterLegacyArrayUnwindException_5405F5(GetExceptionInformation())) {
        // 原53C5A6仅恢复本帧栈并进入尾声；筛选正常只返回0或终止，无追加业务动作。
    }
}

void ConstructLegacyArray_53C45E(
    void* const storage, const std::uint32_t elementBytes, const std::int32_t elementCount,
    const LegacyArrayElementCallback constructor, const LegacyArrayElementCallback destructor)
{
    auto* current = static_cast<std::byte*>(storage);
    std::int32_t completed = 0;
    std::uint32_t complete = 0;
    __try {
        while (completed < elementCount) {
            constructor(current);
            current += elementBytes;
            ++completed;
        }
        complete = 1;
    } __finally {
        // 原53C4C8共享finally：只在未完成时展开已完成前缀，当前失败元素不计入。
        if (complete == 0) {
            UnwindLegacyArray_53C560(current, elementBytes, completed, destructor);
        }
    }
}

void DestroyLegacyArray_53C4E0(
    void* const storage, const std::uint32_t elementBytes, const std::int32_t elementCount,
    const LegacyArrayElementCallback destructor)
{
    // 原imul只保留W32乘积，随后才作用于地址；数量/步长不按指针扩宽。
    const std::uint32_t endOffset = elementBytes * std::bit_cast<std::uint32_t>(elementCount);
    auto* current = static_cast<std::byte*>(storage) + endOffset;
    std::uint32_t remaining = std::bit_cast<std::uint32_t>(elementCount);
    std::uint32_t complete = 0;
    __try {
        for (;;) {
            --remaining;
            if (std::bit_cast<std::int32_t>(remaining) < 0) {
                break;
            }
            current -= elementBytes;
            destructor(current);
        }
        complete = 1;
    } __finally {
        // 原53C548共享finally：失败对象地址和已递减剩余数交展开，不重试失败析构。
        if (complete == 0) {
            UnwindLegacyArray_53C560(
                current, elementBytes, std::bit_cast<std::int32_t>(remaining), destructor);
        }
    }
}

void RunLegacyInitializers_53C9B5(
    LegacyRuntimeExitCallback* first, LegacyRuntimeExitCallback* const last)
{
    while (reinterpret_cast<std::uintptr_t>(first)
        < reinterpret_cast<std::uintptr_t>(last)) {
        const auto callback = *first;
        if (callback != nullptr) {
            callback();
        }
        ++first;
    }
}

void DispatchLegacyExit_53C91C(const std::int32_t code,
    const std::int32_t quick, const std::int32_t callable)
{
    if (gLegacyRuntimeExitComplete1086028 == 1) {
        const HANDLE process = ::GetCurrentProcess();
        static_cast<void>(::TerminateProcess(process, static_cast<UINT>(code)));
    }
    gLegacyRuntimeExitStarted1086024 = 1;
    gLegacyRuntimeExitCallable1086020 = static_cast<std::uint8_t>(callable);
    if (quick == 0) {
        auto* first = gLegacyRuntimeExitFirst1087AF4;
        if (first != nullptr) {
            auto* cursor = gLegacyRuntimeExitLast1087AF0;
            while (reinterpret_cast<std::uintptr_t>(cursor)
                > reinterpret_cast<std::uintptr_t>(first)) {
                const auto callback = *--cursor;
                if (callback != nullptr) {
                    callback();
                }
                // 原53C973重读首槽地址，末指针不重新采样，已调用槽不清空。
                first = gLegacyRuntimeExitFirst1087AF4;
            }
        }
        RunLegacyInitializers_53C9B5(
            gLegacyTerminationTable553184, gLegacyTerminationTable553184 + 3);
    }
    RunLegacyInitializers_53C9B5(
        gLegacyTerminationTable553184 + 4, gLegacyTerminationTable553184 + 6);
    if (callable == 0) {
        gLegacyRuntimeExitComplete1086028 = 1;
        ::ExitProcess(static_cast<UINT>(code));
    }
}

void ExitLegacyRuntime_53C8FA(const std::int32_t code)
{
    DispatchLegacyExit_53C91C(code, 0, 0);
}

[[noreturn]] void QuickExitLegacyRuntime_53C90B(const std::int32_t code)
{
    DispatchLegacyExit_53C91C(code, 1, 0);
    // callable固定0，调度末尾必经ExitProcess；这里只向MSVC表达已证非返回路径。
    __assume(0);
}

void FinishLegacyStdio_53EA4A()
{
    static_cast<void>(FlushAllLegacyStreams_53BB99());
    if (gLegacyRuntimeExitCallable1086020 != 0) {
        static_cast<void>(CloseAllLegacyStreams_5446AD());
    }
}

void CloseLegacyConsole_54B20C()
{
    const auto closeHandle = &::CloseHandle;
    HANDLE handle = gLegacyConsoleHandle592FA4;
    if (handle != INVALID_HANDLE_VALUE
        && handle != reinterpret_cast<HANDLE>(std::intptr_t{-2})) {
        static_cast<void>(closeHandle(handle));
    }
    handle = gLegacyConsoleHandle592FA0;
    if (handle != INVALID_HANDLE_VALUE
        && handle != reinterpret_cast<HANDLE>(std::intptr_t{-2})) {
        static_cast<void>(closeHandle(handle));
    }
}

// 原5450D6；保留API检查及0/1结果，不按指针非空直接判有效。
std::int32_t ValidateLegacyExecutablePointer_5450D6(FARPROC const function)
{
    return ::IsBadCodePtr(function) == 0 ? 1 : 0;
}

LONG WINAPI FilterLegacyUnhandledException_54503A(EXCEPTION_POINTERS* const information)
{
    const auto* const record = information->ExceptionRecord;
    if (record->ExceptionCode == 0xE06D7363u
        && record->NumberParameters == 3
        && static_cast<std::uint32_t>(record->ExceptionInformation[0]) == 0x19930520u) {
        TerminateLegacyRuntime_540A5C();
    }
    const auto previous = gLegacyPreviousExceptionFilter10862E0;
    if (previous != nullptr
        && ValidateLegacyExecutablePointer_5450D6(
            reinterpret_cast<FARPROC>(previous)) != 0) {
        // 原545072在验证API之后重新读取共享槽。
        return gLegacyPreviousExceptionFilter10862E0(information);
    }
    return 0;
}

void InstallLegacyExceptionFilter_545080()
{
    gLegacyPreviousExceptionFilter10862E0 =
        ::SetUnhandledExceptionFilter(FilterLegacyUnhandledException_54503A);
}

void RestoreLegacyExceptionFilter_545091()
{
    static_cast<void>(::SetUnhandledExceptionFilter(
        gLegacyPreviousExceptionFilter10862E0));
}

namespace {

// 原C表553170目标返回真实槽表指针，__initterm不消费返回；只做类型明确的地址适配。
void InitializeLegacyExitTableForCrt()
{
    static_cast<void>(InitializeLegacyExitTable_53B418());
}

// 原53DE8C..53DE9F筛选片段：先缓存info所指Code并发布本帧槽，再调用真实筛选。
// 不是新增原函数身份；只为原生__except表达单次info采样和原Code保存时点。
std::int32_t FilterLegacyProcessException_53DE8C(
    _EXCEPTION_POINTERS* const information, std::uint32_t* const codeSlot)
{
    const std::uint32_t code = information->ExceptionRecord->ExceptionCode;
    *codeSlot = code;
    return FilterLegacyRuntimeException_543DB2(code, information);
}

} // namespace

// 原58F864已加载53B447；真实原生函数地址，不存原地址整数或指向宿主初始化器。
LegacyRuntimeExitCallback gLegacyFloatingInitializer58F864 = InitializeLegacyFloatingPoint_53B447;
// 原[55316C,553180)五槽，返回不消费；不改为_initterm_e的非零失败短路。
LegacyRuntimeExitCallback gLegacyCInitializationTable55316C[5] = {
    nullptr,
    InitializeLegacyExitTableForCrt,
    InitializeLegacyStdio_53E9A5,
    InitializeLegacyMultibyte_546CAE,
    InstallLegacyExceptionFilter_545080
};

void InitializeLegacyCrt_53C8CD()
{
    const LegacyRuntimeExitCallback floatingInitializer = gLegacyFloatingInitializer58F864;
    if (floatingInitializer != nullptr) {
        floatingInitializer();
    }
    RunLegacyInitializers_53C9B5(
        gLegacyCInitializationTable55316C, gLegacyCInitializationTable55316C + 5);
    RunLegacyInitializers_53C9B5(
        gLegacyCppInitializationTable553000, gLegacyCppInitializationTable553000 + 90);
}

[[noreturn]] void RunLegacyProcessEntry_53DDB5()
{
    const std::uint32_t versionBits = ::GetVersion();
    const std::uint32_t minor = (versionBits >> 8u) & 0xFFu;
    gLegacyRuntimeOsMinor1085FF8 = minor;
    const std::uint32_t major = versionBits & 0xFFu;
    gLegacyRuntimeOsMajor1085FF4 = major;
    gLegacyRuntimeOsCombined1085FF0 = (major << 8u) + minor;
    gLegacyRuntimeOsBuild1085FEC = versionBits >> 16u;
    if (InitializeLegacyHeap_541BB8(0) == 0) {
        FastLegacyRuntimeErrorExit_53DED0(28);
    }

    std::uint32_t exceptionCode;
    __try {
        // 原TryLevel在53DE21才为0，GetVersion/堆初始化及错误28在保护区外。
        InitializeLegacyDescriptors_53F52D();
        CaptureLegacyCommandLine_53DE29();
        CaptureLegacyEnvironment_53DE34();
        InitializeLegacyArguments_544047();
        InitializeLegacyEnvironment_543F8E();
        InitializeLegacyCrt_53C8CD();

        STARTUPINFOA startupInfo;
        startupInfo.dwFlags = 0;
        ::GetStartupInfoA(&startupInfo);
        char* const commandLine = SliceLegacyWinMainCommandLine_543F36();
        const std::int32_t showCommand = (startupInfo.dwFlags & STARTF_USESHOWWINDOW) != 0u
            ? static_cast<std::int32_t>(startupInfo.wShowWindow) : SW_SHOWDEFAULT;
        const HINSTANCE instance = ::GetModuleHandleA(nullptr);
        const std::int32_t exitCode = ::WinMain(instance, nullptr, commandLine, showCommand);
        ExitLegacyRuntime_53C8FA(exitCode);
    } __except (FilterLegacyProcessException_53DE8C(
        GetExceptionInformation(), &exceptionCode)) {
        QuickExitLegacyRuntime_53C90B(std::bit_cast<std::int32_t>(exceptionCode));
    }
    // 原正常/handler均终止进程；只表达已证非返回，不增加备用退出或catch。
    __assume(0);
}

} // namespace cg::platform
