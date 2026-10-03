#pragma once

#include <cstdint>

namespace cg::platform {

// 原退出/初始化表的函数地址槽；回调返回值不被调用方消费。
using LegacyRuntimeExitCallback = void (*)();
extern std::uint8_t gLegacyRuntimeExitCallable1086020;
extern std::int32_t gLegacyRuntimeExitStarted1086024;
extern std::int32_t gLegacyRuntimeExitComplete1086028;
extern LegacyRuntimeExitCallback* gLegacyRuntimeExitFirst1087AF4;
extern LegacyRuntimeExitCallback* gLegacyRuntimeExitLast1087AF0;

// 原53B399/53B406；按实际块容量追加，空回调也入表，外层据返回指针给0/-1。
// 原堆外层与模式2已实现，模式3核心/启动及宿主析构登记迁移尚待。
LegacyRuntimeExitCallback RegisterLegacyExitCallback_53B399(LegacyRuntimeExitCallback callback);
std::int32_t RegisterLegacyAtExit_53B406(LegacyRuntimeExitCallback callback);

// 原53B418；分配32槽，仅清首槽；失败走原运行库错误24。
LegacyRuntimeExitCallback* InitializeLegacyExitTable_53B418();

// 原53C9B5；按[first,last)正序调非空回调，不读返回值。
void RunLegacyInitializers_53C9B5(
    LegacyRuntimeExitCallback* first, LegacyRuntimeExitCallback* last);

// 原53C91C；quick非零跳普通退出表，callable完整S32决定是否返回。
void DispatchLegacyExit_53C91C(
    std::int32_t code, std::int32_t quick, std::int32_t callable);
void ExitLegacyRuntime_53C8FA(std::int32_t code);
[[noreturn]] void QuickExitLegacyRuntime_53C90B(std::int32_t code);

// 原两个终止表中的真实叶；stdio刷新/关闭已接LegacyStdio本体。
void FinishLegacyStdio_53EA4A();
void CloseLegacyConsole_54B20C();
void InstallLegacyExceptionFilter_545080();
void RestoreLegacyExceptionFilter_545091();

} // namespace cg::platform
