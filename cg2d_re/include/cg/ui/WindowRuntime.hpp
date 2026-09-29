#pragma once

#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::ui {

// 游戏UI实例，不是Win32窗口或Yobi；调用方只借用，不承担释放。
struct WindowInstance;

WindowInstance* OpenWindow_471440(std::int32_t id); // 完整七类创建；原型表绑定仍是外部依赖。
WindowInstance* Sub_5342E0(); // DpSY回复打开窗口64，透传借用实例指针。
login::YobiAction* CreateWindowAnimation_475AA0(
    std::int32_t id, std::uint32_t flags, std::uint32_t mode,
    login::YobiAction** completionSlot); // 完成槽只借用；已有实例仅置关闭标记。

WindowInstance* Sub_4CD3E0(); // 原窗口68的state置0x8000，返回原借用引用。
WindowInstance* Sub_4CD400(); // 原窗口67，同上；不直接关闭或清表。
std::int32_t Sub_4AFF00(); // 借用窗口48置关闭标记，空返回0，否则透传音效结果。
std::uint8_t ReadValueD67BE0() noexcept; // 共享UI门禁W8，world只读。
std::uint8_t ReadValue5F9754() noexcept; // 技能回复与观察进入共用的UI标志。
std::int32_t ReadDragItemIndex_584DA4() noexcept; // 原S32拖动编号，初值-1。

void FreeWindow_471270(WindowInstance* window); // release回调、记录回存、七类元素释放。
void CloseWindow_472160(WindowInstance* window); // 记录置bit1后释放，不清gWindows槽。
std::int32_t CloseAllWindows_472190(); // 完整关闭与观察退出请求，返回0或发送结果。
void ProcessSceneWindows_503E80(); // 场景切换的标记、关闭、保存和三路重开；原caller弃返回。
void CloseWindowsByCategory_472240(std::uint32_t mask); // 按原71槽顺序关闭并清槽。
void CancelWindowDrag_476410(); // 回调后重读阶段，最后进入2或6，不等同通用重置。
void ApplyObservationInputGate_440ADB(); // 同源5F9754及D67B08前八个U8的观察进入写序。
void InitializeNumberSpacing_477170(); // 原CRT 553098，四种数字图片步进的唯一初始化。
void ConstructItemSelectors(); // 原CRT 55309C/5530A0，两静态选择对象按序构造。
void DestroyItemSelectors(); // 两对象逆序析构，不释放其静态预览。
void DrawCurrentWindow_4729B0(); // 严格visible==1的七类分派，尾部清文本层3并绘范围提示。

// 对话BZ回复：三重门禁、关闭/完整路由及对象朝向，socket保持未使用实参。
void Sub_50E110(
    std::uintptr_t unusedSocket, std::int32_t type, std::int32_t value,
    std::int32_t request, std::int32_t target, char* text);

// 资料DyAG回复：三重门禁后完整38/39/40路由，无BZ关闭特例或对象朝向尾。
void Sub_50F990(
    std::uintptr_t unusedSocket, std::int32_t type, std::int32_t value,
    std::int32_t request, std::int32_t target, char* text);

// 摆摊开/收回复，无场景门禁，原返回地址均被dispatcher丢弃。
void Sub_50E7D0(std::uintptr_t unusedSocket, std::int32_t result);
void Sub_50E7E0(std::uintptr_t unusedSocket, std::int32_t result);
void Sub_50E820(std::uintptr_t unusedSocket, std::int32_t result); // 购买结果，无额外门禁。

// 交易回复：按场景门禁后单次CryptInt门禁，socket由原dispatcher传入但不消费。
void Sub_50E3C0(std::uintptr_t unusedSocket, const char* text); // 名单解析/回调生产与窗口44展开。
void Sub_50E3F0(
    std::uintptr_t unusedSocket, char* text, std::int32_t unusedValue); // 特殊状态发送拒绝；正常路径原位反转义。
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
