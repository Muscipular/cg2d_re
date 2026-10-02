#include "LegacyRuntimeExit.hpp"
#include "LegacyRuntimeError.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "StartupNative.hpp"

#include <cstddef>

namespace cg::platform {

// 原真实依赖，尚无本体；不以宿主FILE清理或固定过滤结果代替。
std::int32_t FlushAllLegacyStreams_53BB99();
std::int32_t CloseAllLegacyStreams_5446AD();
LONG WINAPI FilterLegacyUnhandledException_54503A(EXCEPTION_POINTERS* information);

std::uint8_t gLegacyRuntimeExitCallable1086020 = 0;
std::int32_t gLegacyRuntimeExitStarted1086024 = 0;
std::int32_t gLegacyRuntimeExitComplete1086028 = 0;
LegacyRuntimeExitCallback* gLegacyRuntimeExitFirst1087AF4 = nullptr;
LegacyRuntimeExitCallback* gLegacyRuntimeExitLast1087AF0 = nullptr;

namespace {

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

LegacyRuntimeExitCallback* InitializeLegacyExitTable_53B418()
{
    constexpr std::uint32_t kInitialSlotCount = 32;
    auto* slots = static_cast<LegacyRuntimeExitCallback*>(
        Malloc_53C3AC(kInitialSlotCount
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

void QuickExitLegacyRuntime_53C90B(const std::int32_t code)
{
    DispatchLegacyExit_53C91C(code, 1, 0);
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
