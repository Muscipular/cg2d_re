#pragma once

#include <cstdint>

namespace cg::platform {

// 原53C3AC/53C0A9；沿用户已批准的std::malloc/std::free合同，数量保持U32。
void* __cdecl Malloc_53C3AC(std::uint32_t size);
void __cdecl Free_53C0A9(void* memory);

// 原53FCFB；mode3返回真实SDK块容量，mode1/2仍原容量；须与本域及稳定模式配对。
std::uint32_t GetLegacyAllocationSize_53FCFB(void* memory);
// 原53BFF8/53FA5B：mode3分配/清零/扩容为同一SDK域，mode1/2仍原域，禁止跨域。
// M3H01保W32与父失败消费；旧核心不模拟，完整所有权/模式寿命、SDK及启动运行仍待。
void* CallocateLegacyMemory_53BFF8(std::uint32_t count, std::uint32_t size);
void* ReallocateLegacyMemory_53FA5B(void* memory, std::uint32_t byteCount);

// 原541620；按重叠方向、目标对齐及逐W32读写顺序复制，返回原目标。
void* CopyLegacyMemory_541620(void* destination, const void* source, std::uint32_t byteCount);

// 原5419D0；值仅低U8参与，Count保持U32，0数量不写，返回原目标。
void* FillLegacyMemory_5419D0(void* destination, std::int32_t value, std::uint32_t byteCount);
// 原54B160/分离chunk与54B1AC共用返回；精确-1/0/+1，0数量不读缓冲。
std::int32_t CompareLegacyMemory_54B160(
    const void* left, const void* right, std::uint32_t byteCount);

// 原541A70；OS/环境/模块链接器版本选择1/2/3，不自动创建或切换堆。
std::int32_t SelectLegacyHeapMode_541A70();

// 原53C9CF/53C3BE的裸字节缓冲调用域；失败可返回空，处理器异常向外传播。
// 数量保持U32；结果只能交FreeLegacyByteBuffer，不用于类型对象或标准容器分配。
void* AllocateLegacyByteBuffer(std::uint32_t byteCount);

// 释放本分配器返回的块；空指针无操作。所有权及释放顺序由原caller保留。
void FreeLegacyByteBuffer(void* memory) noexcept;

} // namespace cg::platform
