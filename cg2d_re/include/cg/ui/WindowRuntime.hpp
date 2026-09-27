#pragma once

#include <cstdint>

namespace cg::ui {

// 游戏UI实例，不是Win32窗口或Yobi；调用方只借用，不承担释放。
struct WindowInstance;

WindowInstance* Sub_4CD3E0(); // 原窗口68的state置0x8000，返回原借用引用。
WindowInstance* Sub_4CD400(); // 原窗口67，同上；不直接关闭或清表。
std::uint8_t ReadValueD67BE0() noexcept; // 共享UI门禁W8，world只读。
std::int32_t ReadDragItemIndex_584DA4() noexcept; // 原S32拖动编号，初值-1。

} // namespace cg::ui
