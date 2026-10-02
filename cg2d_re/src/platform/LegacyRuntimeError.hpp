#pragma once

#include <cstdint>

namespace cg::platform {

// 原运行库共享状态；类型/模式保持S32，钩子按真实函数指针拓宽。
using LegacyRuntimeBannerHook = void (*)();
extern std::int32_t gLegacyRuntimeErrorMode1086038;
extern std::int32_t gLegacyRuntimeApplicationType58F8A4;
extern LegacyRuntimeBannerHook gLegacyRuntimeBannerHook10862A8;

// 原58F8A0的间接退出槽；初始指向原快速退出，不替换成直接ExitProcess。
using LegacyRuntimeProcessExit = void (*)(std::int32_t);
extern LegacyRuntimeProcessExit gLegacyRuntimeProcessExit58F8A0;
void ExitLegacyRuntimeError_53DEAB(std::int32_t errorCode);

// 原546D0C；懒加载三个窗口API，失败返回0，不改变调用方文字或标志。
std::int32_t ShowLegacyRuntimeMessage_546D0C(
    const char* text, const char* caption, std::uint32_t flags);

// 原5443FF/5443C6；共享错误模式/消息表，banner两次报告之间允许调用原钩子。
void ReportLegacyRuntimeError_5443FF(std::int32_t errorCode);
void ReportLegacyRuntimeBanner_5443C6();

// 原53DED0；报告后直接ExitProcess(255)，不是经退出表的53DEAB。
[[noreturn]] void FastLegacyRuntimeErrorExit_53DED0(std::int32_t errorCode);

} // namespace cg::platform
