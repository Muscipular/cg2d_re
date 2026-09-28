#pragma once

#include <cstdint>

namespace cg::ui {

// 游戏UI实例，不是Win32窗口或Yobi；调用方只借用，不承担释放。
struct WindowInstance;

WindowInstance* Sub_4CD3E0(); // 原窗口68的state置0x8000，返回原借用引用。
WindowInstance* Sub_4CD400(); // 原窗口67，同上；不直接关闭或清表。
std::uint8_t ReadValueD67BE0() noexcept; // 共享UI门禁W8，world只读。
std::int32_t ReadDragItemIndex_584DA4() noexcept; // 原S32拖动编号，初值-1。

void FreeWindow_471270(WindowInstance* window); // release回调、记录回存、七类元素释放。
void CloseWindowsByCategory_472240(std::uint32_t mask); // 按原71槽顺序关闭并清槽。
void CancelWindowDrag_476410(); // 回调后重读阶段，最后进入2或6，不等同通用重置。

} // namespace cg::ui
