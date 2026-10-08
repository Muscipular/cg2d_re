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

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand);

namespace cg::platform {
namespace {

std::int32_t FilterLegacyProcessException_53DE8C(
    _EXCEPTION_POINTERS* const information, std::uint32_t* const codeSlot)
{
    const std::uint32_t code = information->ExceptionRecord->ExceptionCode;
    *codeSlot = code;
    return FilterLegacyRuntimeException_543DB2(code, information);
}

} // namespace

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
