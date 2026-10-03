#pragma once

#include <cstdint>

namespace cg::platform {

// 原53C3AC/53C0A9；沿用户已批准的std::malloc/std::free合同，数量保持U32。
void* __cdecl Malloc_53C3AC(std::uint32_t size);
void __cdecl Free_53C0A9(void* memory);

// 原53FCFB；真实原堆容量查询。只可用于原堆块，模式2查块依赖尚待实现。
std::uint32_t GetLegacyAllocationSize_53FCFB(void* memory);
// 原53FA5B真实依赖声明；原扩容及分配器归属尚待，不能用宿主替代补齐。
void* ReallocateLegacyMemory_53FA5B(void* memory, std::uint32_t byteCount);

// 原541620；按重叠方向、目标对齐及逐W32读写顺序复制，返回原目标。
void* CopyLegacyMemory_541620(void* destination, const void* source, std::uint32_t byteCount);

// 原541A70；OS/环境/模块链接器版本选择1/2/3，不自动创建或切换堆。
std::int32_t SelectLegacyHeapMode_541A70();

// 原53C9CF/53C3BE的裸字节缓冲调用域；失败可返回空，处理器异常向外传播。
// 数量保持U32；结果只能交FreeLegacyByteBuffer，不用于类型对象或标准容器分配。
void* AllocateLegacyByteBuffer(std::uint32_t byteCount);

// 释放本分配器返回的块；空指针无操作。所有权及释放顺序由原caller保留。
void FreeLegacyByteBuffer(void* memory) noexcept;

} // namespace cg::platform
