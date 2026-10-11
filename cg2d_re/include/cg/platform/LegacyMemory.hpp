#pragma once

#include <cstdint>

namespace cg::platform {

// 原53C3AC/53C0A9；沿用户已批准的std::malloc/std::free合同，数量保持U32。
void* __cdecl Malloc_53C3AC(std::uint32_t size);
// 原函数：0x0053C0A9。
void __cdecl Free_53C0A9(void* memory);

// 原53FCFB；直接查询同CRT域块的_msize实际容量，沿现接口按U32消费。
std::uint32_t GetLegacyAllocationSize_53FCFB(void* memory);
// 原53BFF8/53FA5B：MC01-C4直接std::calloc/realloc，输入数量保持U32。
// 零长度、乘积/请求校验、容量与失败策略按MSVC；只接收同CRT域块。
void* CallocateLegacyMemory_53BFF8(std::uint32_t count, std::uint32_t size);
// 原函数：0x0053FA5B。
void* ReallocateLegacyMemory_53FA5B(void* memory, std::uint32_t byteCount);

// 原541620：直接std::memmove，支持重叠，数量保持U32。
void* CopyLegacyMemory_541620(void* destination, const void* source, std::uint32_t byteCount);

// 原5419D0：直接std::memset，数量保持U32，返回原目标。
void* FillLegacyMemory_5419D0(void* destination, std::int32_t value, std::uint32_t byteCount);
// 原54B160：直接std::memcmp，返回MSVC实际比较结果，不复刻旧W32复读。
std::int32_t CompareLegacyMemory_54B160(
    const void* left, const void* right, std::uint32_t byteCount);

// 裸字节缓冲调用域采用std::malloc，失败返回空，零请求采用MSVC合同。
// 数量保持U32；结果只能交FreeLegacyByteBuffer，不用于类型对象或标准容器分配。
void* AllocateLegacyByteBuffer(std::uint32_t byteCount);

// 释放本分配器返回的块；空指针无操作。所有权及释放顺序由原caller保留。
void FreeLegacyByteBuffer(void* memory) noexcept;

} // namespace cg::platform
