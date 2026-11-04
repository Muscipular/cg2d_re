#pragma once

#include "cg/render/SurfaceState.hpp"

#include "cg/render/SdlPresenter.hpp"

#include "cg/render/GraphicCacheState.hpp"

#include "cg/render/Palette.hpp"
#include "cg/platform/Random.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/RendererResources.hpp"

#include <cstdint>

namespace cg::network { class CgConnection; }
namespace cg::session { class ClientSession; }

namespace cg::platform {

// 原 4576B0 的首个直接依赖；父初始化本体位于 ClientInitialization.cpp。
// 原函数：0x0044B5A0。
std::uint32_t ResetSystemLog_44B5A0(); // 打开失败返回 0，成功透传 fclose 的 W32。

// 原 MainLoop 的直接依赖，不提供占位函数；每个后续实现仍须对照其完整原函数。
// 原函数：0x004576B0。
int InitializeClient_4576B0(session::ClientSession& session);
// NSM01：新增仅在帧边界执行；完成所属初始化和切换后返回稳定ID，失败返回0。
std::uint64_t CreateClientSession();
// 后建会话复用中央原构造入口及相对顺序，不重跑进程设备/资源初始化。
void InitializeAdditionalSessionStorage(session::ClientSession& session);
// 原函数：0x00458A40。
void ShowModeChange_458A40(session::ClientSession& session);
// 原函数：0x00457AB0。
void KeepAlive_457AB0(network::CgConnection& connection);
// 原函数：0x0052CD30。
int UpdateAudioAndSkipMciClose_52CD30(session::ClientSession& session, std::int32_t value);
// 原函数：0x00507140。
void UpdateQueuedTextAndGraphicHover_507140(session::ClientSession& session);
// 原函数：0x00423640；后台仅推进偏移/幅度，不回拷共享像素。
void ApplyScreenShake_423640(session::ClientSession& session, bool submitPixels = true);
// 原函数：0x004589D0。
std::uint64_t ReadSystemSeconds_4589D0();

} // namespace cg::platform
