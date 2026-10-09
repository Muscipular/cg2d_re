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
    std::int32_t remaining = elementCount;
    __try {
        for (;;) {
            --remaining;
            if (remaining < 0) {
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
    const std::uint32_t endOffset = elementBytes * elementCount;
    auto* current = static_cast<std::byte*>(storage) + endOffset;
    std::int32_t remaining = elementCount;
    std::uint32_t complete = 0;
    __try {
        for (;;) {
            --remaining;
            if (remaining < 0) {
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
                current, elementBytes, remaining, destructor);
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
    // HFI01：本机MSVC x64多一个镜像基址参数；保原魔数及终止/转调职责。
    if (record->ExceptionCode == 0xE06D7363u
        && record->NumberParameters == 4
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

} // namespace cg::platform
