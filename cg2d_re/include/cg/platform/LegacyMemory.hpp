#pragma once

#include <cstdint>

namespace cg::platform {

// 原53C3AC/53C0A9；沿用户已批准的std::malloc/std::free合同，数量保持U32。
void* __cdecl Malloc_53C3AC(std::uint32_t size);
void __cdecl Free_53C0A9(void* memory);

// 原53FCFB；HMU01全部模式返回真实CRT malloc/calloc/realloc块容量，按U32消费。
std::uint32_t GetLegacyAllocationSize_53FCFB(void* memory);
// 原53BFF8/53FA5B：HMU01全部模式统一标准分配族，保W32/零长度与父失败消费。
// 旧池机制由宿主承接，仍只接收同CRT域块，完整所有权/SDK及启动运行待验。
void* CallocateLegacyMemory_53BFF8(std::uint32_t count, std::uint32_t size);
void* ReallocateLegacyMemory_53FA5B(void* memory, std::uint32_t byteCount);

// 原541620：直接std::memmove，支持重叠，数量保持U32。
void* CopyLegacyMemory_541620(void* destination, const void* source, std::uint32_t byteCount);

// 原5419D0：直接std::memset，数量保持U32，返回原目标。
void* FillLegacyMemory_5419D0(void* destination, std::int32_t value, std::uint32_t byteCount);
// 原54B160：直接std::memcmp，返回MSVC实际比较结果，不复刻旧W32复读。
std::int32_t CompareLegacyMemory_54B160(
    const void* left, const void* right, std::uint32_t byteCount);

// 原541A70；OS/环境/模块链接器版本选择1/2/3，不自动创建或切换堆。
std::int32_t SelectLegacyHeapMode_541A70();

// 裸字节缓冲调用域采用std::malloc，失败返回空，零请求采用MSVC合同。
// 数量保持U32；结果只能交FreeLegacyByteBuffer，不用于类型对象或标准容器分配。
void* AllocateLegacyByteBuffer(std::uint32_t byteCount);

// 释放本分配器返回的块；空指针无操作。所有权及释放顺序由原caller保留。
void FreeLegacyByteBuffer(void* memory) noexcept;

} // namespace cg::platform
