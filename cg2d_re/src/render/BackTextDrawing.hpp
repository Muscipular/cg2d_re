#pragma once

#include <Windows.h>

namespace cg::render {

// render 私有边界：DC 的 DIB 与唯一 SDL Back 共享映射，成功时持有面锁。
HDC AcquireCpuBackTextDc() noexcept;
// 原文本消费者忽略释放结果；同步 GDI 后释放本次 DC 与同一面锁。
void ReleaseCpuBackTextDc(HDC dc) noexcept;

} // namespace cg::render
