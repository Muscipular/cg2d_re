#pragma once

#include "cg/Palette.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/RendererResources.hpp"

#include <cstdint>

namespace cg::render {

void RebuildRenderer_466B80();
int TestCooperativeLevel_44C6B0();
std::uint32_t ResetGraphicCache_456D30();
void SortTextures_520790();
std::int32_t ClearSurface_4512C0() noexcept;
void Flip_44C560();

} // namespace cg::render

namespace cg::platform {

using cg::network::Report_512CA0;
using cg::network::NetworkLoop_507DF0;

// 原 4576B0 的首个直接依赖；父初始化本体位于 ClientInitialization.cpp。
std::uint32_t ResetSystemLog_44B5A0(); // 打开失败返回 0，成功透传 fclose 的 W32。

// 原CRT 553008/55300C/553010；各自独立登记退出，复用保护逻辑的唯一状态。
// 原553008入口：先构造同一对象，再以原53B406登记对应退出回调。
std::int32_t InitializeLegacyProtectionChecksum1_404120();
void ConstructProtectionChecksum1_404130(); // 原594070。
void DestroyProtectionChecksum1_404150();
// 原55300C入口：先构造同一对象，再以原53B406登记对应退出回调。
std::int32_t InitializeLegacyProtectionChecksum2_404160();
void ConstructProtectionChecksum2_404170(); // 原594080。
void DestroyProtectionChecksum2_404190();
// 原553010入口：先构造同一对象，再以原53B406登记对应退出回调。
std::int32_t InitializeLegacyProtectionProcessState_4041A0();
void ConstructProtectionProcessState_4041B0(); // 原59E548。
void DestroyProtectionProcessState_4041D0();

// 原 MainLoop 的直接依赖，不提供占位函数；每个后续实现仍须对照其完整原函数。
int InitializeClient_4576B0();
void InitializeProtection_4041E0(const char* moduleName);
void ParseProtectionTables_404600(const char* input);
int ResetProtection_4048D0();
void ShowModeChange_458A40();
void KeepAlive_457AB0();
int Sub_52CD30(std::int32_t value);
void Sub_507140();
std::uint8_t CheckProtection_4043B0(const char* functionName);
std::int32_t CheckWindows_4048E0();
void Sub_423640();
int EnumerateWindows_4587D0();
extern std::uint32_t gRandomSeed; // 原 0x58F870；所有原播种与 rand 调用共享。
std::uint32_t SeedRandom_53B4BF(std::uint32_t seed);
std::uint64_t ReadSystemSeconds_4589D0();
int NextRandom_53B4C9();
// 原 0x46FAF0：按有符号上下界生成随机值。
std::int32_t RandomRange_46FAF0(std::int32_t lower, std::int32_t upper);
int CheckSystemSeconds_458A10(std::int32_t seconds);
int ReleaseProtection_4042B0();

} // namespace cg::platform
