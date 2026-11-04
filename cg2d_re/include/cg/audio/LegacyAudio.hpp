#pragma once

#include "cg/platform/SdlAudioBackend.hpp"

#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>

namespace cg::session { class ClientSession; }

namespace cg::audio {

// LBG01：文件音乐只借旧设备就绪状态、音乐设置及普通BGM输出抑制。
[[nodiscard]] bool IsFileBgmAudioAvailable() noexcept;
[[nodiscard]] float GetFileBgmGain() noexcept;
[[nodiscard]] bool SetSceneBgmSuppressed(session::ClientSession& session, bool suppressed) noexcept;

// 原103D1C0的50项记录；52CD40只写+04/+00，后三个S32保留。
struct SpatialAudioRecord final {
    std::uint32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。
};

static_assert(offsetof(SpatialAudioRecord, value00) == 0x00);
static_assert(offsetof(SpatialAudioRecord, value04) == 0x04);
static_assert(offsetof(SpatialAudioRecord, value08) == 0x08);
static_assert(offsetof(SpatialAudioRecord, value0C) == 0x0C);
static_assert(offsetof(SpatialAudioRecord, value10) == 0x10);
static_assert(sizeof(SpatialAudioRecord) == 0x14);

// 原104C9B8的50项请求；与空间声源表分离，音频帧消费后清计数。
struct SoundRequestRecord final {
    std::int32_t channelIndex; // 原成员偏移：+0x00。
    std::int32_t seNo; // 原成员偏移：+0x04。
    std::int32_t requestCount; // 原成员偏移：+0x08。
    std::int32_t distanceOrAtten; // 原成员偏移：+0x0C。
    std::int32_t targetVolumeIndex; // 原成员偏移：+0x10。
    std::int32_t appliedVolumeIndex; // 原成员偏移：+0x14。
    std::int32_t screenX; // 原成员偏移：+0x18。
    std::int32_t targetPanIndex; // 原成员偏移：+0x1C。
    std::int32_t appliedPanIndex; // 原成员偏移：+0x20。
};

static_assert(offsetof(SoundRequestRecord, channelIndex) == 0x00);
static_assert(offsetof(SoundRequestRecord, seNo) == 0x04);
static_assert(offsetof(SoundRequestRecord, requestCount) == 0x08);
static_assert(offsetof(SoundRequestRecord, distanceOrAtten) == 0x0C);
static_assert(offsetof(SoundRequestRecord, targetVolumeIndex) == 0x10);
static_assert(offsetof(SoundRequestRecord, appliedVolumeIndex) == 0x14);
static_assert(offsetof(SoundRequestRecord, screenX) == 0x18);
static_assert(offsetof(SoundRequestRecord, targetPanIndex) == 0x1C);
static_assert(offsetof(SoundRequestRecord, appliedPanIndex) == 0x20);
static_assert(sizeof(SoundRequestRecord) == 0x24);

// 会话实际请求与BGM播放；设备、音效通道/缓存及进程设置不在此复制。
struct AudioRuntimeState final {
    std::array<SpatialAudioRecord, 50> spatialSources {}; // 原103D1C0。
    std::array<SoundRequestRecord, 50> soundRequests {}; // 原104C9B8。
    std::int32_t bgmTarget = 0; // 原1050818；切曲与角色保存共用目标。
    std::int32_t bgmRequest = 0; // 原1050810；地图生产、音频帧消费。
    std::int32_t bgmStage = 0; // 原1050814；请求处理阶段，清理入口也写入。
    std::int32_t bgmRecordIndex = -1; // 原58D4F8，真实播放号，不是目标号。
    std::int32_t bgmFadeOutActive = 0; // 原105080C。
    std::int32_t bgmFadeVolumeIndex = 0; // 原103CFB0。
    std::uint32_t bgmActive = 0; // 原1050808。
    std::uint32_t bgmMode = 0; // 原1050820。
    std::atomic<std::uint8_t> bgmStopNotice {0}; // 原105081C，仅所属停止线程发布。
    std::atomic<std::uint32_t> bgmReady {0}; // 原105083C，所属预填握手。
    std::uint32_t bgmLoopStart = 0; // 原103CFB8，字节位置。
    std::uint32_t bgmLoopEnd = 0; // 原103DEA4，先发布PCM字节数，后发布smpl端点。
    std::uint64_t foregroundFileBgm = 0; // 所属会话的replace/takeover实际播放编号；0为无。
    bool sceneBgmRestorePending = false; // 文件音乐释放抑制失败时重试。
    platform::SdlAudioBackend::BgmPlayback bgmPlayback; // 先析构停止线程，再释放上方两原子量。
    platform::SdlAudioBackend::FixedMusicPlayback fixedMusicPlayback; // 借进程只读PCM，实际流及混响独占。

    static_assert(sizeof(spatialSources) == 0x3E8);
    static_assert(sizeof(soundRequests) == 0x708);
};

// 原52BE60：普通BGM加载与播放；音频模块持有实际播放状态。
std::int32_t LoadAndPlayBGM(session::ClientSession& session, std::uint32_t bgmId);
// L04：按现有512项BGM映射播放，不修改场景的切曲请求状态。
std::int32_t LoadMappedBgm(session::ClientSession& session, std::int32_t index);
// 原454C30、454EE0：固定音乐初始化与关闭，由平台生命周期调用。
std::int32_t InitializeMusic_454C30(session::ClientSession& session, std::uintptr_t window);
std::int32_t ShutdownFixedMusic_454EE0();
// 原52CCA0：更新音频播放状态。
void UpdateAudioPlayback_52CCA0(session::ClientSession& session);
// 原458AE0：实际音频设备探测与旧DINPUT入口可用性检查。
std::int32_t CheckAudioAndInputAvailability_458AE0();

extern std::uint32_t gSkipMciDeviceClose_103DE80; // 原 0x103DE80，虚拟零填充 W32；52CD30 写入，52CCE0 读取。
extern std::uint32_t gAudioDisabled_10507D4; // 原 0x10507D4；5241A0 写入，音效/BGM/更新入口共读。
extern volatile std::uint32_t gMciDeviceId; // 原 0x10507EC；两次 MCI 调用各自重读。
extern std::uint32_t gMciCloseResult; // 原 0x10507FC；保存第二次 MCI 返回的完整 W32。

// 原455280：加载并播放内置固定音乐。
std::int32_t PlayFixedMusic_455280(session::ClientSession& session, std::int32_t type, std::uint32_t repeats);

// 原 524060 加载 512 项映射及 128 条配置记录；52C9C0 非负映射发布 BGM 目标。
std::int32_t LoadBgmConfig_524060();
// 原函数：0x005241A0。
std::int32_t InitializeAudio_5241A0(session::ClientSession& session); // 进程启动初始化设备，并初始化首会话请求。
// NSM01：会话请求初始化与播放释放，不重开设备或清共享通道/样本。
void InitializeSessionSoundRequests(session::ClientSession& session);
void ShutdownSessionPlayback(session::ClientSession& session);
// 原函数：0x0052B630。
void ShutdownAudio_52B630(); // 原退出入口；先处理流式 BGM，再按序释放音效与设备。
// 原函数：0x0052C9C0。
std::int32_t SelectBgmTarget_52C9C0(session::ClientSession& session, std::int32_t index);
// 原函数：0x0052C910。
std::int32_t UpdateBgmRequest_52C910(session::ClientSession& session); // 原四阶段跳表，完整透传各分支返回。
// 原函数：0x0052C3D0。
std::int32_t ApplyBgmVolume_52C3D0(session::ClientSession& session); // 同步共享音量到各BGM，返回发起会话原结果。

// 宿主发布当前项后静音切出普通/固定流，再恢复当前流；失败释放对应实际播放。
void SelectBgmOutput() noexcept;
// 切换时释放旧共享音效通道；清请求借用，不清样本、轮转或冷却。
void SelectSoundEffectOutput() noexcept;

// 原 [0x52CCE0,0x52CD25)：标志为 1 时返回 1，否则停止、关闭并只覆写返回低 AL。
std::uint32_t StopAndCloseMciDevice_52CCE0() noexcept;

// 原函数：0x0052CD40。
SpatialAudioRecord* __cdecl ResetSpatialSoundSources_52CD40(session::ClientSession& session); // 原 [0x52CD40,0x52CD5C)，重置 50 项并返回一过尾指针。
// 原函数：0x0052CDE0。
std::int32_t __cdecl UpdateNearestSpatialSoundSource_52CDE0(session::ClientSession& session,
    std::int32_t index, std::int32_t sourceX, std::int32_t sourceY); // 原 [0x52CDE0,0x52CE97)。
// 原 0x52C9E0；50 项请求生产，复用唯一音效定义表。
std::int32_t __cdecl AccumulateSpatialSoundRequest_52C9E0(
    session::ClientSession& session, std::int32_t id, std::int32_t x, std::int32_t y);
// 原 World::PlaySe@52C450，第二参数是声像位置；额外压栈值未被本体读取。
std::int32_t __cdecl PlaySe_52C450(
    session::ClientSession& session, std::int32_t seNo, std::int32_t screenX);
// 原 52CB50 消费请求；52C680 更新同一通道并返回 0/-1。
std::int32_t __cdecl ProcessSpatialSoundRequests_52CB50(session::ClientSession& session);
// 原函数：0x0052C680。
std::int32_t __cdecl ApplySpatialSoundRequest_52C680(
    session::ClientSession& session, std::int32_t requestIndex, std::int32_t isNew);
// 原函数：0x0052CD60。
void __cdecl SubmitSpatialSoundSources_52CD60(session::ClientSession& session); // 原 [0x52CD60,0x52CDDB)，扫描同一 50 项表。
// 原函数：0x0052B990。
std::uint32_t BeginBgmFadeOut_52B990(session::ClientSession& session) noexcept;
// 原函数：0x0052B940。
void ResetBgmPlayback_52B940(session::ClientSession& session); // 原资源清理入口，普通场景复用既有实现。
std::int32_t ReadBgmRecordIndex58D4F8(session::ClientSession& session) noexcept; // 只读所属真实BGM号，不替代目标1050818。

} // namespace cg::audio
