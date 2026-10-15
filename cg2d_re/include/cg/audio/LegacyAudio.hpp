#pragma once

#include <cstdint>
#include <cstddef>

namespace cg::audio {

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

// 原52BE60：普通BGM加载与播放；音频模块持有实际播放状态。
std::int32_t LoadAndPlayBGM(std::uint32_t bgmId);
// 原454C30、454EE0：固定音乐初始化与关闭，由平台生命周期调用。
std::int32_t InitializeMusic_454C30(std::uintptr_t window);
std::int32_t Sub_454EE0();
// 原52CCA0：更新音频播放状态。
void Sub_52CCA0();
// 原458AE0：实际音频设备探测与旧DINPUT入口可用性检查。
std::int32_t Sub_458AE0();

extern std::uint32_t gValue103DE80; // 原 0x103DE80，虚拟零填充 W32；52CD30 写入，52CCE0 读取。
extern std::uint32_t gValue10507D4; // 原 0x10507D4；5241A0 写入，音效/BGM/更新入口共读。
extern std::int32_t gBgmTarget1050818; // 原 0x1050818；BGM 请求与当前存档共用的目标 W32。
extern std::int32_t gBgmRequest1050810; // 原 0x1050810；世界场景生产、52C910 消费的请求 W32。
extern std::int32_t gBgmStage1050814; // 原 0x1050814；52C910 与清理入口共用的阶段 W32。
extern volatile std::uint32_t gMciDeviceId; // 原 0x10507EC；两次 MCI 调用各自重读。
extern std::uint32_t gMciCloseResult; // 原 0x10507FC；保存第二次 MCI 返回的完整 W32。

// 原455280：加载并播放内置固定音乐。
std::int32_t PlayFixedMusic_455280(std::int32_t type, std::uint32_t repeats);

// 原 524060 加载 512 项映射及 128 条配置记录；52C9C0 非负映射发布 BGM 目标。
std::int32_t LoadBgmConfig_524060();
// 原函数：0x005241A0。
std::int32_t InitializeAudio_5241A0(); // 原初始化包装；524230 使用 SDL3 加载音效。
// 原函数：0x0052B630。
void Sub_52B630(); // 原退出入口；先处理流式 BGM，再按序释放音效与设备。
// 原函数：0x0052C9C0。
std::int32_t SelectBgmTarget_52C9C0(std::int32_t index);
// 原函数：0x0052C910。
std::int32_t UpdateBgmRequest_52C910(); // 原四阶段跳表，完整透传各分支返回。
// 原函数：0x0052C3D0。
std::int32_t Sub_52C3D0(); // 原 [52C3D0,52C449)，沿原门槛和音量表应用实际SDL BGM音量。

// 原 [0x52CCE0,0x52CD25)：标志为 1 时返回 1，否则停止、关闭并只覆写返回低 AL。
std::uint32_t Sub_52CCE0() noexcept;

// 原函数：0x0052CD40。
SpatialAudioRecord* __cdecl Sub_52CD40(); // 原 [0x52CD40,0x52CD5C)，重置 50 项并返回一过尾指针。
// 原函数：0x0052CDE0。
std::int32_t __cdecl Sub_52CDE0(
    std::int32_t index, std::int32_t sourceX, std::int32_t sourceY); // 原 [0x52CDE0,0x52CE97)。
// 原 0x52C9E0；50 项请求生产，复用唯一音效定义表。
std::int32_t __cdecl Sub_52C9E0(
    std::int32_t id, std::int32_t x, std::int32_t y);
// 原 World::PlaySe@52C450，第二参数是声像位置；额外压栈值未被本体读取。
std::int32_t __cdecl PlaySe_52C450(std::int32_t seNo, std::int32_t screenX);
// 原 52CB50 消费请求；52C680 更新同一通道并返回 0/-1。
std::int32_t __cdecl Sub_52CB50();
// 原函数：0x0052C680。
std::int32_t __cdecl Sub_52C680(
    std::int32_t requestIndex, std::int32_t isNew);
// 原函数：0x0052CD60。
void __cdecl Sub_52CD60(); // 原 [0x52CD60,0x52CDDB)，扫描同一 50 项表。
// 原函数：0x0052B990。
std::uint32_t Sub_52B990() noexcept;
// 原函数：0x0052B940。
void Sub_52B940(); // 原资源清理入口，普通场景复用既有实现。
std::int32_t ReadBgmRecordIndex58D4F8() noexcept; // 只读真实当前BGM号，不替代目标1050818。

} // namespace cg::audio
