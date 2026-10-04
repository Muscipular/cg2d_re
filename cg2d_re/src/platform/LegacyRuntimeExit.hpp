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

// 原53C45E/53C4E0/53C560的元素this回调；新端显式传非拥有地址，返回不消费。
using LegacyArrayElementCallback = void (*)(void*);
// storage为首元素，elementBytes保持U32，elementCount保持S32；无空回调/数量补保护。
void ConstructLegacyArray_53C45E(
    void* storage, std::uint32_t elementBytes, std::int32_t elementCount,
    LegacyArrayElementCallback constructor, LegacyArrayElementCallback destructor);
void DestroyLegacyArray_53C4E0(
    void* storage, std::uint32_t elementBytes, std::int32_t elementCount,
    LegacyArrayElementCallback destructor);
// onePast为已完成/剩余前缀的尾地址，先递减数量/地址再析构；C++二次异常走原终止。
void UnwindLegacyArray_53C560(
    void* onePast, std::uint32_t elementBytes, std::int32_t elementCount,
    LegacyArrayElementCallback destructor);

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

// 原浮点初始化槽及C表真实原生定义；首null/四目标顺序，不消费返回。
extern LegacyRuntimeExitCallback gLegacyFloatingInitializer58F864;
extern LegacyRuntimeExitCallback gLegacyCInitializationTable55316C[5];
// 原C++[553000,553168)90槽仅真实外部声明；原完整表/宿主迁移及音频仍未启用。
extern LegacyRuntimeExitCallback gLegacyCppInitializationTable553000[90];
void InitializeLegacyCrt_53C8CD();

// 原GetVersion发布四W32；PE节头/载入规则已证初值0，真实存储见LegacyRuntimeExit.cpp。
extern std::uint32_t gLegacyRuntimeOsMinor1085FF8;
extern std::uint32_t gLegacyRuntimeOsMajor1085FF4;
extern std::uint32_t gLegacyRuntimeOsCombined1085FF0;
extern std::uint32_t gLegacyRuntimeOsBuild1085FEC;
// 原完整入口源码顺序/原生SEH筛选与两退出，当前不替换宿主入口或自动调用。
[[noreturn]] void RunLegacyProcessEntry_53DDB5();

} // namespace cg::platform
