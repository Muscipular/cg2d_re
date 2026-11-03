#include "../WindowRuntimeInternal.hpp"
#include "cg/session/ClientSession.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowEmoteElementId : ElementId {
    ButtonEmote = 1,
    ButtonEmote_2 = 2,
    ButtonEmote_3 = 3,
    ButtonEmote_4 = 4,
    ButtonEmote_5 = 5,
    ButtonEmote_6 = 6,
    ButtonEmote_7 = 7,
    ButtonEmote_8 = 8,
    ButtonEmote_9 = 9,
    ButtonEmote_10 = 10,
    ButtonEmote_11 = 11,
    ButtonEmote_12 = 12,
    ButtonEmote_13 = 13,
    ButtonEmote_14 = 14,
    ButtonEmote_15 = 15,
    ButtonEmote_16 = 16,
    ButtonEmote_17 = 17,
    ButtonEmote_18 = 18,
    ButtonClose = 19,
    Element20 = 20,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00495FC0。
static std::int32_t WindowEmote_ResetSelection(session::ClientSession& session); // 清零唯一18B选择区，返回0。

// 原函数：0x00495FE0。
static std::int32_t WindowEmote_ButtonEmote(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 情绪窗18按钮，请求被拒仍更新选择并发声。

// 原函数：0x00495F50。
static std::int32_t WindowEmote_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static constexpr const wchar_t* kEmoteHints5871B0[18] { // 原常量参考：0x005871B0。
    L"做「坐下」这个动作。",
    L"做「挥手」这个动作。",
    L"做「点头」这个动作。",
    L"做「高兴」这个动作。",
    L"做「愤怒」这个动作。",
    L"做「悲伤」这个动作。",
    L"做「投掷」这个动作。",
    L"做「出剪刀」这个动作。",
    L"做「出布」这个动作。",
    L"做「站立」这个动作。",
    L"做「走路」这个动作。",
    L"做「倒下」这个动作。",
    L"做「攻击」这个动作。",
    L"做「防御」这个动作。",
    L"做「受伤」这个动作。",
    L"做「魔法」这个动作。",
    L"做「移动中」这个动作。",
    L"做「出石头」这个动作。",
};

static constexpr std::int32_t kEmoteButtonActions[18] {
    0, 2, 4, 6, 8, 10, 12, 14, 16, 1, 3, 5, 7, 9, 11, 13, 15, 17,
};

static WindowGraphicData gWindowGraphic56B0D8 {0x3B985, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B0D8。

static WindowGraphicData gWindowGraphic56B0F0 {0x3B988, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B0F0。

static WindowGraphicData gWindowGraphic56B108 {0x3B98B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B108。

static WindowGraphicData gWindowGraphic56B120 {0x3B98E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B120。

static WindowGraphicData gWindowGraphic56B138 {0x3B991, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B138。

static WindowGraphicData gWindowGraphic56B150 {0x3B994, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B150。

static WindowGraphicData gWindowGraphic56B168 {0x3B997, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B168。

static WindowGraphicData gWindowGraphic56B180 {0x3B99A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B180。

static WindowGraphicData gWindowGraphic56B198 {0x3B99D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B198。

static WindowGraphicData gWindowGraphic56B1B0 {0x3B9A0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1B0。

static WindowGraphicData gWindowGraphic56B1C8 {0x3B9A3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1C8。

static WindowGraphicData gWindowGraphic56B1E0 {0x3B9A6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1E0。

static WindowGraphicData gWindowGraphic56B1F8 {0x3B9A9, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1F8。

static WindowGraphicData gWindowGraphic56B210 {0x3B9AC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B210。

static WindowGraphicData gWindowGraphic56B228 {0x3B9AF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B228。

static WindowGraphicData gWindowGraphic56B240 {0x3B9B2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B240。

static WindowGraphicData gWindowGraphic56B258 {0x3B9B5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B258。

static WindowGraphicData gWindowGraphic56B270 {0x3B9B8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B270。

static WindowGraphicData gWindowGraphic56B288 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B288。

static WindowGraphicData gWindowGraphic56B2A0 {0x3B984, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B2A0。

static WindowElementPrototype gWindowElements56B2B8[20] { // 原全局：0x0056B2B8。
    {WindowElementType_Graphic, 5, 25, 23, 23, 1, {.graphic = &gWindowGraphic56B0D8}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote)},
    {WindowElementType_Graphic, 5, 50, 23, 23, 1, {.graphic = &gWindowGraphic56B0F0}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_2)},
    {WindowElementType_Graphic, 5, 75, 23, 23, 1, {.graphic = &gWindowGraphic56B108}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_3)},
    {WindowElementType_Graphic, 5, 100, 23, 23, 1, {.graphic = &gWindowGraphic56B120}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_4)},
    {WindowElementType_Graphic, 5, 125, 23, 23, 1, {.graphic = &gWindowGraphic56B138}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_5)},
    {WindowElementType_Graphic, 5, 150, 23, 23, 1, {.graphic = &gWindowGraphic56B150}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_6)},
    {WindowElementType_Graphic, 5, 175, 23, 23, 1, {.graphic = &gWindowGraphic56B168}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_7)},
    {WindowElementType_Graphic, 5, 200, 23, 23, 1, {.graphic = &gWindowGraphic56B180}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_8)},
    {WindowElementType_Graphic, 5, 225, 23, 23, 1, {.graphic = &gWindowGraphic56B198}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_9)},
    {WindowElementType_Graphic, 29, 25, 23, 23, 1, {.graphic = &gWindowGraphic56B1B0}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_10)},
    {WindowElementType_Graphic, 29, 50, 23, 23, 1, {.graphic = &gWindowGraphic56B1C8}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_11)},
    {WindowElementType_Graphic, 29, 75, 23, 23, 1, {.graphic = &gWindowGraphic56B1E0}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_12)},
    {WindowElementType_Graphic, 29, 100, 23, 23, 1, {.graphic = &gWindowGraphic56B1F8}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_13)},
    {WindowElementType_Graphic, 29, 125, 23, 23, 1, {.graphic = &gWindowGraphic56B210}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_14)},
    {WindowElementType_Graphic, 29, 150, 23, 23, 1, {.graphic = &gWindowGraphic56B228}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_15)},
    {WindowElementType_Graphic, 29, 175, 23, 23, 1, {.graphic = &gWindowGraphic56B240}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_16)},
    {WindowElementType_Graphic, 29, 200, 23, 23, 1, {.graphic = &gWindowGraphic56B258}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_17)},
    {WindowElementType_Graphic, 29, 225, 23, 23, 1, {.graphic = &gWindowGraphic56B270}, &WindowEmote_ButtonEmote, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonEmote_18)},
    {WindowElementType_Graphic, 45, 7, 11, 11, 1, {.graphic = &gWindowGraphic56B288}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowEmoteElementId::ButtonClose)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B2A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowEmoteElementId::Element20)},
};

// 原型：窗口34；原全局：0x0054E6F0。
WindowPrototype gWindowPrototypeEmote {0, 4, 575, 97, 61, 260, 0x80010101u, 20, gWindowElements56B2B8, &WindowEmote_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 34: 54E6F0

// 窗口函数实现。

static void WindowEmote_ClearSelection(session::ClientSession& session)
{
    session.UiFrame().native.emoteSelection.fill(0);
}

// 原函数：0x00495F50。
static std::int32_t WindowEmote_Update(WindowInstance& callbackWindow, const std::int32_t)
{
    if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 1, 1) != 0) {
        static_cast<void>(WindowEmote_ResetSelection(*callbackWindow.ownerSession));
    }
    return 1;
}

// 原函数：0x00495F70。
std::int32_t WindowEmote_Select(session::ClientSession& session, const std::int32_t index)
{
    WindowEmote_ClearSelection(session);
    session.UiFrame().native.emoteSelection[index] = 1;
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    return WindowProfile_SaveOrigin(session, x, y);
}

// 原函数：0x00495FC0。
static std::int32_t WindowEmote_ResetSelection(session::ClientSession& session)
{
    WindowEmote_ClearSelection(session);
    return 0;
}

// 原函数：0x00495FE0。
static std::int32_t WindowEmote_ButtonEmote(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        std::int32_t consumed = 0;
        if (hover) {
            graphic->imageNo = 0x3B987 + index * 3;
            consumed = 1;
        } else {
            const bool selected = callbackWindow.ownerSession->UiFrame().native.emoteSelection[index] == 1;
            graphic->imageNo = (selected ? 0x3B985 : 0x3B986) + index * 3;
        }
        if (static_cast<std::uint32_t>(index) > 17u) {
            return consumed;
        }
        if (hover) {
            uiFrame.hoverText = kEmoteHints5871B0[index];
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            world::RequestPlayerEmote_4FA8E0(*callbackWindow.ownerSession, kEmoteButtonActions[index]);
            static_cast<void>(WindowEmote_Select(*callbackWindow.ownerSession, index));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        return consumed;
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
