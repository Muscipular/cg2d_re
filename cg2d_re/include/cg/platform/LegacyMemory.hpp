#pragma once

#include <cstdint>

namespace cg::platform {

// 原53C9CF/53C3BE的裸字节缓冲调用域；失败可返回空，处理器异常向外传播。
// 数量保持U32；结果只能交FreeLegacyByteBuffer，不用于类型对象或标准容器分配。
void* AllocateLegacyByteBuffer(std::uint32_t byteCount);

// 释放本分配器返回的块；空指针无操作。所有权及释放顺序由原caller保留。
void FreeLegacyByteBuffer(void* memory) noexcept;

} // namespace cg::platform
