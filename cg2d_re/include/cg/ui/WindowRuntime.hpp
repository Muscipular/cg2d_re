#pragma once

#include <cstdint>

namespace cg::ui {

// 游戏UI实例，不是Win32窗口或Yobi；调用方只借用，不承担释放。
struct WindowInstance;

WindowInstance* Sub_4CD3E0(); // 原窗口68的state置0x8000，返回原借用引用。
WindowInstance* Sub_4CD400(); // 原窗口67，同上；不直接关闭或清表。
std::uint8_t ReadValueD67BE0() noexcept; // 共享UI门禁W8，world只读。
std::uint8_t ReadValue5F9754() noexcept; // 技能回复与观察进入共用的UI标志。
std::int32_t ReadDragItemIndex_584DA4() noexcept; // 原S32拖动编号，初值-1。

void FreeWindow_471270(WindowInstance* window); // release回调、记录回存、七类元素释放。
void CloseWindowsByCategory_472240(std::uint32_t mask); // 按原71槽顺序关闭并清槽。
void CancelWindowDrag_476410(); // 回调后重读阶段，最后进入2或6，不等同通用重置。
void ApplyObservationInputGate_440ADB(); // 同源5F9754及D67B08前八个U8的观察进入写序。
void InitializeNumberSpacing_477170(); // 原CRT 553098，四种数字图片步进的唯一初始化。
void DrawCurrentWindow_4729B0(); // 严格visible==1的七类分派，尾部清文本层3并绘范围提示。

// 交易回复：按场景门禁后单次CryptInt门禁，socket由原dispatcher传入但不消费。
std::int32_t Sub_50E440(std::uintptr_t unusedSocket);
std::int32_t Sub_50E470(std::uintptr_t unusedSocket, std::int32_t value);
std::int32_t Sub_50E4A0(std::uintptr_t unusedSocket, const char* text);
std::int32_t Sub_50E4D0(
    std::uintptr_t unusedSocket, std::int32_t index, const char* text);
std::int32_t Sub_50E500(
    std::uintptr_t unusedSocket, std::int32_t index, const char* text);
std::int32_t Sub_50E530(std::uintptr_t unusedSocket); // 清第0组，返回D2E2B0改写前值。
void Sub_50E570(std::uintptr_t unusedSocket, std::int32_t value); // 关闭只置标志，弃原混合返回。

} // namespace cg::ui
