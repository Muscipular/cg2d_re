#pragma once

#include <cstdint>

namespace cg::audio {

struct SpatialAudioRecord;

extern std::uint32_t gValue103DE80; // 原 0x103DE80，虚拟零填充 W32；52CD30 写入，52CCE0 读取。
extern volatile std::uint32_t gMciDeviceId; // 原 0x10507EC；两次 MCI 调用各自重读。
extern std::uint32_t gMciCloseResult; // 原 0x10507FC；保存第二次 MCI 返回的完整 W32。

// 原 [0x52CCE0,0x52CD25)：标志为 1 时返回 1，否则停止、关闭并只覆写返回低 AL。
std::uint32_t Sub_52CCE0() noexcept;

SpatialAudioRecord* __cdecl Sub_52CD40(); // 原 [0x52CD40,0x52CD5C)，重置 50 项并返回一过尾指针。
std::int32_t __cdecl Sub_52CDE0(
    std::int32_t index, std::int32_t sourceX, std::int32_t sourceY); // 原 [0x52CDE0,0x52CE97)。
// 原 0x52C9E0；真实空间音效下游仍待落位。
std::int32_t __cdecl Sub_52C9E0(
    std::int32_t id, std::int32_t x, std::int32_t y);
void __cdecl Sub_52CD60(); // 原 [0x52CD60,0x52CDDB)，扫描同一 50 项表。
std::uint32_t Sub_52B990() noexcept;

// 原 57C368 表与 CC236C 三槽的共享音效选择入口。
std::int32_t __cdecl Sub_5044C0(std::uint32_t id);
std::int32_t __cdecl Sub_504410(std::uint32_t id);
std::int32_t __cdecl Sub_504180(std::uint32_t id);
std::int32_t __cdecl Sub_5042D0(std::uint32_t id);
std::int32_t __cdecl Sub_504380(std::uint32_t id);
std::int32_t __cdecl Sub_5043D0(std::int32_t index);
std::int32_t __cdecl Sub_504AF0();

} // namespace cg::audio
