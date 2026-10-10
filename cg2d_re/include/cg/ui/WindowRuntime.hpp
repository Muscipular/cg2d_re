#pragma once

#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::ui {

// 游戏UI实例，不是Win32窗口或Yobi；调用方只借用，不承担释放。
struct WindowInstance;

// WPL01：完整创建/载荷owner合同，见doc/cg2d_window_payload_creation_spec.md。
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
std::int32_t ReadChatInputByteWidth_4A80E0(); // 原C9A930两倍，供历史翻页计算原字节滚动量。 // HBN01：见历史浏览/字节滚动完整合同。

void FreeWindow_471270(WindowInstance* window); // release回调、记录回存、七类元素释放。
void CloseWindow_472160(WindowInstance* window); // WCR01：同记录地址发布/置bit1再释放，不清表槽。
std::int32_t CloseAllWindows_472190(); // 完整关闭与观察退出请求，返回0或发送结果。
void ProcessSceneWindows_503E80(); // 场景切换的标记、关闭、保存和三路重开；原caller弃返回。
std::int32_t UpdateWindows_473F80(); // 完整鼠标/窗口/快捷键/预设输入父循环，返回1。
std::int32_t RenderWindows_504120(); // 转生计时到期发送/清标志后，透传完整窗口更新返回。
void CloseWindowsByCategory_472240(std::uint32_t mask); // 按原71槽顺序关闭并清槽。
void CancelWindowDrag_476410(); // 回调后重读阶段，最后进入2或6，不等同通用重置。
void ApplyObservationInputGate_440ADB(); // 同源5F9754及D67B08前八个U8的观察进入写序。
// 原CRT 553098入口仅尾跳既有初始化，无退出登记。
void InitializeLegacyNumberSpacing_477160();
// WND01：477160尾跳的四S32步进生产，见doc/cg2d_window_number_draw_spec.md。
void InitializeNumberSpacing_477170(); // 477160尾跳的四种数字图片步进业务初始化。
// 原CRT 55309C/5530A0：每项构造后独立登记退出；登记失败仍继续，不释放静态预览。
// 原CRT 55309C/5530A0两独立选择器入口，各自构造后宿主即时登记固定内嵌预览退出，传出真实S32。
std::int32_t InitializeLegacyFirstItemSelector_4883F0(); // 原CF3B10及其内嵌预览。
std::int32_t InitializeLegacySecondItemSelector_4998F0(); // 原D2F338及其内嵌预览。
void InitializeItemSelectors();
// 原CRT 5530A4入口先构造后宿主std::atexit即时登记同一退出，传出真实S32；全应用序待。
std::int32_t InitializeLegacyStallBrowsePetValues_4CD240();
void ConstructStallBrowsePetValues(); // 原CRT 5530A4，浏览区五宠物各六CryptInt顺序构造。
void DestroyStallBrowsePetValues(); // 原4CD270退出链，记录与成员都逆序，不释放浏览区。
// WDP01：完整分发与字段取样合同，见doc/cg2d_window_element_draw_pipeline_spec.md。
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
void Sub_50E7F0(
    std::uintptr_t unusedSocket, std::int32_t status, std::int32_t target,
    const char* stallName, const char* description,
    const char* itemPayload, const char* petPayload); // ft完整浏览生产，无场景门禁。
void Sub_50E820(std::uintptr_t unusedSocket, std::int32_t result); // 购买结果，无额外门禁。

// LY物品使用名单：仅一次CryptInt门禁，完整生产自己/地图五槽/服务器目标后打开45。
void Sub_50E380(std::uintptr_t unusedSocket, const char* text);
void Sub_50E3A0(std::uintptr_t unusedSocket, const char* text); // nFV完整自身/远端属性生产后打开46。
void Sub_50E340(std::uintptr_t unusedSocket, const char* text); // ysVc技能目标名单及45选择回调。
void Sub_50E360(std::uintptr_t unusedSocket, const char* text); // ND技能属性列表及46选择回调。
void Sub_50C7F0(
    std::uintptr_t unusedSocket, std::int32_t result,
    std::int32_t kind, const char* text); // JgO四类结果，场景门禁后仅一次Ripple读取。

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
