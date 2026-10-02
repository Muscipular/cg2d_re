#pragma once

#include <cstddef>
#include <cstdint>

struct _EXCEPTION_POINTERS;

namespace cg::platform {

// 原信号处理器地址；浮点信号按原ABI额外传入一个S32子类型。
using LegacySignalHandler = void (*)(std::int32_t);
using LegacyFloatingSignalHandler = void (*)(std::int32_t, std::int32_t);
struct LegacySignalRecord final {
    std::uint32_t exceptionCode;
    std::int32_t signal;
    LegacySignalHandler handler;
};
static_assert(offsetof(LegacySignalRecord, signal) == 4);
static_assert(offsetof(LegacySignalRecord, handler) == 8);
static_assert(sizeof(LegacySignalRecord) == 16);

// 原异常表与raise/filter共用状态；非地址数字不随x64拓宽。
extern LegacySignalRecord gLegacySignalTable5920A0[10];
extern std::int32_t gLegacyFloatingSignalFirst592118;
extern std::int32_t gLegacyFloatingSignalCount59211C;
extern std::uint32_t gLegacySignalTableCount592120;
extern std::int32_t gLegacyFloatingSignalSubtype592124;
extern _EXCEPTION_POINTERS* gLegacySignalExceptionInfo108619C;
extern LegacySignalHandler gLegacySignalInterrupt10862F8;
extern LegacySignalHandler gLegacySignalBreak10862FC;
extern LegacySignalHandler gLegacySignalAbort1086300;
extern LegacySignalHandler gLegacySignalTerminate1086304;

// 原5471C9；仅七个信号合法，不委托宿主raise，不捕获回调异常。
std::int32_t RaiseLegacySignal_5471C9(std::int32_t signal);

// 原543DB2；消费Windows异常指针，保留原表中0/1/5处理器值的语义。
std::int32_t FilterLegacyRuntimeException_543DB2(
    std::uint32_t exceptionCode, _EXCEPTION_POINTERS* information);

} // namespace cg::platform
