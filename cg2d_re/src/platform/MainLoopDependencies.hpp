#pragma once

#include "cg/render/SurfaceState.hpp"

#include "cg/render/SdlPresenter.hpp"

#include "cg/render/GraphicCacheState.hpp"

#include "cg/render/Palette.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/RendererResources.hpp"

#include <cstdint>


namespace cg::platform {

// 原 4576B0 的首个直接依赖；父初始化本体位于 ClientInitialization.cpp。
// 原函数：0x0044B5A0。
std::uint32_t ResetSystemLog_44B5A0(); // 打开失败返回 0，成功透传 fclose 的 W32。

// 原 MainLoop 的直接依赖，不提供占位函数；每个后续实现仍须对照其完整原函数。
// 原函数：0x004576B0。
int InitializeClient_4576B0();
// 原函数：0x00458A40。
void ShowModeChange_458A40();
// 原函数：0x00457AB0。
void KeepAlive_457AB0();
// 原函数：0x0052CD30。
int UpdateAudioAndSkipMciClose_52CD30(std::int32_t value);
// 原函数：0x00507140。
void UpdateQueuedTextAndGraphicHover_507140();
// 原函数：0x00423640。
void ApplyScreenShake_423640();
// 原函数：0x0053B4BF。
std::uint32_t SeedRandom_53B4BF(std::uint32_t seed);
// 原函数：0x004589D0。
std::uint64_t ReadSystemSeconds_4589D0();
// 原函数：0x0053B4C9。
int Rand16();
// 原0x46FAF0；RRU01：合法闭区间均匀分布，upper == -1返回0，倒置区间沿原算法。
std::int32_t RandomRange_46FAF0(std::int32_t lower, std::int32_t upper);

} // namespace cg::platform
