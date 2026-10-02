#include "LegacySignal.hpp"
#include "LegacyRuntimeExit.hpp"
#include "StartupNative.hpp"

namespace cg::platform {

LegacySignalRecord gLegacySignalTable5920A0[10] = {
    {0xC0000005u, 11, nullptr},
    {0xC000001Du, 4, nullptr},
    {0xC0000096u, 4, nullptr},
    {0xC000008Du, 8, nullptr},
    {0xC000008Eu, 8, nullptr},
    {0xC000008Fu, 8, nullptr},
    {0xC0000090u, 8, nullptr},
    {0xC0000091u, 8, nullptr},
    {0xC0000092u, 8, nullptr},
    {0xC0000093u, 8, nullptr}
};
std::int32_t gLegacyFloatingSignalFirst592118 = 3;
std::int32_t gLegacyFloatingSignalCount59211C = 7;
std::uint32_t gLegacySignalTableCount592120 = 10;
std::int32_t gLegacyFloatingSignalSubtype592124 = 0x8C;
EXCEPTION_POINTERS* gLegacySignalExceptionInfo108619C = nullptr;
LegacySignalHandler gLegacySignalInterrupt10862F8 = nullptr;
LegacySignalHandler gLegacySignalBreak10862FC = nullptr;
LegacySignalHandler gLegacySignalAbort1086300 = nullptr;
LegacySignalHandler gLegacySignalTerminate1086304 = nullptr;

namespace {

// 原5472F6；先读首项，再按一次采样的W32表数推进，最后重复字段/边界判断。
LegacySignalRecord* LookupLegacySignal_5472F6(const std::int32_t signal)
{
    const std::uint32_t count = gLegacySignalTableCount592120;
    auto* record = gLegacySignalTable5920A0;
    auto* const end = gLegacySignalTable5920A0 + count;
    if (record->signal != signal) {
        do {
            ++record;
            if (reinterpret_cast<std::uintptr_t>(record)
                >= reinterpret_cast<std::uintptr_t>(end)) {
                break;
            }
        } while (record->signal != signal);
    }
    if (reinterpret_cast<std::uintptr_t>(record)
            >= reinterpret_cast<std::uintptr_t>(end)
        || record->signal != signal) {
        return nullptr;
    }
    return record;
}

// 原543EF3；同一表按异常码查找，保留独立入口和原读取字段。
LegacySignalRecord* LookupLegacyException_543EF3(const std::uint32_t exceptionCode)
{
    const std::uint32_t count = gLegacySignalTableCount592120;
    auto* record = gLegacySignalTable5920A0;
    auto* const end = gLegacySignalTable5920A0 + count;
    if (record->exceptionCode != exceptionCode) {
        do {
            ++record;
            if (reinterpret_cast<std::uintptr_t>(record)
                >= reinterpret_cast<std::uintptr_t>(end)) {
                break;
            }
        } while (record->exceptionCode != exceptionCode);
    }
    if (reinterpret_cast<std::uintptr_t>(record)
            >= reinterpret_cast<std::uintptr_t>(end)
        || record->exceptionCode != exceptionCode) {
        return nullptr;
    }
    return record;
}

} // namespace

std::int32_t RaiseLegacySignal_5471C9(const std::int32_t signal)
{
    LegacySignalHandler* slot;
    switch (signal) {
    case 2: slot = &gLegacySignalInterrupt10862F8; break;
    case 4:
    case 8:
    case 11: slot = &LookupLegacySignal_5472F6(signal)->handler; break;
    case 15: slot = &gLegacySignalTerminate1086304; break;
    case 21: slot = &gLegacySignalBreak10862FC; break;
    case 22: slot = &gLegacySignalAbort1086300; break;
    default: return -1;
    }
    const auto handler = *slot;
    if (reinterpret_cast<std::uintptr_t>(handler) == 1) {
        return 0;
    }
    if (handler == nullptr) {
        QuickExitLegacyRuntime_53C90B(3);
    }

    EXCEPTION_POINTERS* previousInformation;
    std::int32_t previousSubtype;
    const bool exceptionSignal = signal == 8 || signal == 11 || signal == 4;
    if (exceptionSignal) {
        previousInformation = gLegacySignalExceptionInfo108619C;
        gLegacySignalExceptionInfo108619C = nullptr;
    }
    if (signal == 8) {
        previousSubtype = gLegacyFloatingSignalSubtype592124;
        gLegacyFloatingSignalSubtype592124 = 0x8C;
        const std::int32_t first = gLegacyFloatingSignalFirst592118;
        const std::int32_t end = first + gLegacyFloatingSignalCount59211C;
        for (std::int32_t index = first; index < end; ++index) {
            gLegacySignalTable5920A0[index].handler = nullptr;
        }
        reinterpret_cast<LegacyFloatingSignalHandler>(handler)(
            8, gLegacyFloatingSignalSubtype592124);
    } else {
        *slot = nullptr;
        handler(signal);
    }
    // 原只在回调正常返回后恢复，顺序为异常指针、浮点子类型。
    if (exceptionSignal) {
        gLegacySignalExceptionInfo108619C = previousInformation;
        if (signal == 8) {
            gLegacyFloatingSignalSubtype592124 = previousSubtype;
        }
    }
    return 0;
}

std::int32_t FilterLegacyRuntimeException_543DB2(
    const std::uint32_t exceptionCode, EXCEPTION_POINTERS* const information)
{
    auto* const record = LookupLegacyException_543EF3(exceptionCode);
    const auto handler = record != nullptr ? record->handler : nullptr;
    if (handler == nullptr) {
        return ::UnhandledExceptionFilter(information);
    }
    if (reinterpret_cast<std::uintptr_t>(handler) == 5) {
        record->handler = nullptr;
        return 1;
    }
    if (reinterpret_cast<std::uintptr_t>(handler) == 1) {
        return -1;
    }

    auto* const previousInformation = gLegacySignalExceptionInfo108619C;
    gLegacySignalExceptionInfo108619C = information;
    const std::int32_t signal = record->signal;
    if (signal == 8) {
        const std::int32_t first = gLegacyFloatingSignalFirst592118;
        const std::int32_t end = first + gLegacyFloatingSignalCount59211C;
        for (std::int32_t index = first; index < end; ++index) {
            gLegacySignalTable5920A0[index].handler = nullptr;
        }
        const std::uint32_t code = record->exceptionCode;
        const std::int32_t previousSubtype = gLegacyFloatingSignalSubtype592124;
        switch (code) {
        case 0xC000008Eu: gLegacyFloatingSignalSubtype592124 = 0x83; break;
        case 0xC0000090u: gLegacyFloatingSignalSubtype592124 = 0x81; break;
        case 0xC0000091u: gLegacyFloatingSignalSubtype592124 = 0x84; break;
        case 0xC0000093u: gLegacyFloatingSignalSubtype592124 = 0x85; break;
        case 0xC000008Du: gLegacyFloatingSignalSubtype592124 = 0x82; break;
        case 0xC000008Fu: gLegacyFloatingSignalSubtype592124 = 0x86; break;
        case 0xC0000092u: gLegacyFloatingSignalSubtype592124 = 0x8A; break;
        }
        reinterpret_cast<LegacyFloatingSignalHandler>(handler)(
            8, gLegacyFloatingSignalSubtype592124);
        // 原543EC8在公共尾恢复异常指针之前恢复子类型。
        gLegacyFloatingSignalSubtype592124 = previousSubtype;
    } else {
        record->handler = nullptr;
        handler(signal);
    }
    gLegacySignalExceptionInfo108619C = previousInformation;
    return -1;
}

} // namespace cg::platform
