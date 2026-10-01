#pragma once

#include <cstdint>

namespace cg::audio {

struct SpatialAudioRecord;

extern std::uint32_t gValue103DE80; // 原 0x103DE80，虚拟零填充 W32；52CD30 写入，52CCE0 读取。
extern std::uint32_t gValue10507D4; // 原 0x10507D4；5241A0 写入，音效/BGM/更新入口共读。
extern std::int32_t gBgmTarget1050818; // 原 0x1050818；BGM 请求与当前存档共用的目标 W32。
extern std::int32_t gBgmRequest1050810; // 原 0x1050810；世界场景生产、52C910 消费的请求 W32。
extern std::int32_t gBgmStage1050814; // 原 0x1050814；52C910 与清理入口共用的阶段 W32。
extern volatile std::uint32_t gMciDeviceId; // 原 0x10507EC；两次 MCI 调用各自重读。
extern std::uint32_t gMciCloseResult; // 原 0x10507FC；保存第二次 MCI 返回的完整 W32。

// 原 524060 加载 512 项映射及 128 条配置记录；52C9C0 非负映射发布 BGM 目标。
std::int32_t LoadBgmConfig_524060();
std::int32_t InitializeAudio_5241A0(); // 原初始化包装；524230 使用 SDL3 加载音效。
void Sub_52B630(); // 原退出入口；先处理流式 BGM，再按序释放音效与设备。
std::int32_t SelectBgmTarget_52C9C0(std::int32_t index);
std::int32_t UpdateBgmRequest_52C910(); // 原四阶段跳表，完整透传各分支返回。
std::int32_t Sub_52C3D0(); // 原 [52C3D0,52C449)，应用当前BGM音量；本体暂缓，仅真实依赖声明。

// 原 [0x52CCE0,0x52CD25)：标志为 1 时返回 1，否则停止、关闭并只覆写返回低 AL。
std::uint32_t Sub_52CCE0() noexcept;

SpatialAudioRecord* __cdecl Sub_52CD40(); // 原 [0x52CD40,0x52CD5C)，重置 50 项并返回一过尾指针。
std::int32_t __cdecl Sub_52CDE0(
    std::int32_t index, std::int32_t sourceX, std::int32_t sourceY); // 原 [0x52CDE0,0x52CE97)。
// 原 0x52C9E0；50 项请求生产，复用唯一音效定义表。
std::int32_t __cdecl Sub_52C9E0(
    std::int32_t id, std::int32_t x, std::int32_t y);
// 原 World::PlaySe@52C450，第二参数是声像位置；额外压栈值未被本体读取。
std::int32_t __cdecl PlaySe_52C450(std::int32_t seNo, std::int32_t screenX);
// 原 52CB50 消费请求；52C680 更新同一通道并返回 0/-1。
std::int32_t __cdecl Sub_52CB50();
std::int32_t __cdecl Sub_52C680(
    std::int32_t requestIndex, std::int32_t isNew);
void __cdecl Sub_52CD60(); // 原 [0x52CD60,0x52CDDB)，扫描同一 50 项表。
std::uint32_t Sub_52B990() noexcept;
void Sub_52B940(); // 原资源清理入口，普通场景复用既有实现。
std::int32_t ReadBgmRecordIndex58D4F8() noexcept; // 只读真实当前BGM号，不替代目标1050818。

// 原 57C368 表与 CC236C 三槽的共享音效选择入口。
std::int32_t __cdecl Sub_5044C0(std::uint32_t id);
std::int32_t __cdecl Sub_504410(std::uint32_t id);
std::int32_t __cdecl Sub_504180(std::uint32_t id);
std::int32_t __cdecl Sub_5042D0(std::uint32_t id);
std::int32_t __cdecl Sub_504380(std::uint32_t id);
std::int32_t __cdecl Sub_5043D0(std::int32_t index);
std::int32_t __cdecl Sub_504AF0();

} // namespace cg::audio
