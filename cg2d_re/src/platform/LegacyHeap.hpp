#pragma once

#include "StartupNative.hpp"

#include <cstdint>

namespace cg::platform {

// HMU01：借用宿主CRT堆，不能HeapDestroy；模式仅保原请求数量策略，不选择分配域。
extern HANDLE gLegacyHeap10869A4;
extern std::int32_t gLegacyHeapMode10869A8;

// 原108614C/1086150：原处理器/门禁仍独立保存，每次失败真实调用，不绑定SDK new-mode。
using LegacyHeapNewHandler = std::int32_t (*)(std::uint32_t);
extern LegacyHeapNewHandler gLegacyHeapNewHandler108614C;
extern std::int32_t gLegacyHeapNewMode1086150;
std::int32_t CallLegacyHeapNewHandler_541A28(std::uint32_t byteCount);

// 全部模式统一CRT malloc/free；W32请求/零长度及原失败重试保持，详见HMU01。
void* AllocateLegacyHeapBlock_53C3EA(std::uint32_t byteCount);
void* AllocateLegacyHeapWithHandler_53C3BE(std::uint32_t byteCount, std::int32_t enableHandler);
void* AllocateLegacyHeapMemory_53C3AC(std::uint32_t byteCount);
void FreeLegacyHeapMemory_53C0A9(void* memory);

// 原541BB8调用消费保持；真实查询CRT堆并发布借用句柄，失败仍返回0。
std::int32_t InitializeLegacyHeap_541BB8(std::int32_t serialized);

} // namespace cg::platform
