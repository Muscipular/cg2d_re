#pragma once

#include "cg/render/Palette.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/RendererResources.hpp"

#include <cstdint>

namespace cg::render {

// 原函数：0x00466B80。
void RebuildRenderer_466B80();
// 原函数：0x0044C6B0。
int TestCooperativeLevel_44C6B0();
// 原函数：0x00456D30。
std::uint32_t ResetGraphicCache_456D30();
// 原函数：0x00520790。
void SortTextures_520790();
// 原函数：0x004512C0。
std::int32_t ClearSurface_4512C0() noexcept;
// 原函数：0x0044C560。
void Flip_44C560();

} // namespace cg::render

namespace cg::platform {

using cg::network::Report_512CA0;
using cg::network::NetworkLoop_507DF0;

// 原 4576B0 的首个直接依赖；父初始化本体位于 ClientInitialization.cpp。
// 原函数：0x0044B5A0。
std::uint32_t ResetSystemLog_44B5A0(); // 打开失败返回 0，成功透传 fclose 的 W32。

extern LegacyCryptInt gProtectionChecksum1; // 原全局：0x00594070，在中央启动序列直接构造。
extern LegacyCryptInt gProtectionChecksum2; // 原全局：0x00594080，在中央启动序列直接构造。
extern LegacyCryptInt gProtectionProcessState; // 原全局：0x0059E548，在中央启动序列直接构造。

// 原 MainLoop 的直接依赖，不提供占位函数；每个后续实现仍须对照其完整原函数。
// 原函数：0x004576B0。
int InitializeClient_4576B0();
// 原函数：0x004041E0。
void InitializeProtection_4041E0(const char* moduleName);
// 原函数：0x00404600。
void ParseProtectionTables_404600(const char* input);
// 原函数：0x004048D0。
int ResetProtection_4048D0();
// 原函数：0x00458A40。
void ShowModeChange_458A40();
// 原函数：0x00457AB0。
void KeepAlive_457AB0();
// 原函数：0x0052CD30。
int Sub_52CD30(std::int32_t value);
// 原函数：0x00507140。
void Sub_507140();
// 原函数：0x004043B0。
std::uint8_t CheckProtection_4043B0(const char* functionName);
// 原函数：0x004048E0。
std::int32_t CheckWindows_4048E0();
// 原函数：0x00423640。
void Sub_423640();
// 原函数：0x004587D0。
int EnumerateWindows_4587D0();
// 原函数：0x0053B4BF。
std::uint32_t SeedRandom_53B4BF(std::uint32_t seed);
// 原函数：0x004589D0。
std::uint64_t ReadSystemSeconds_4589D0();
// 原函数：0x0053B4C9。
int Rand16();
// 原 0x46FAF0：按有符号上下界生成随机值。
std::int32_t RandomRange_46FAF0(std::int32_t lower, std::int32_t upper);
// 原函数：0x00458A10。
int CheckSystemSeconds_458A10(std::int32_t seconds);
// 原函数：0x004042B0。
int ReleaseProtection_4042B0();

} // namespace cg::platform
