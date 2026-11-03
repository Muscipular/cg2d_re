#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowShortcutBarElementId : ElementId {
    HandleButtons = 1,
    HandleButtons_2 = 2,
    HandleButtons_3 = 3,
    HandleButtons_4 = 4,
    HandleButtons_5 = 5,
    HandleButtons_6 = 6,
    HandleButtons_7 = 7,
    HandleButtons_8 = 8,
    HandleButtons_9 = 9,
    Element10 = 10,
    ButtonCollapse = 11,
};

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowShortcutBarElementId_gWindowElements55EEC0 : ElementId {
    Element1 = 1,
    ButtonExpand = 2,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0046FBD0。
static std::int32_t WindowShortcutBar_ButtonCollapse(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 快捷栏4点击收起到5，打开后发布CA0440。

// 原函数：0x0046FC10。
static std::int32_t WindowShortcutBar_ButtonExpand(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 快捷栏5点击展开到4。

// 原函数：0x0046FC60。
static std::int32_t WindowShortcutBar_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 完整九按钮，保留6/7子入口消费覆盖。

// 主原型中的真实回调依赖；本声明只约束窗口调用侧，不替代各本体取证。
// 原函数：0x0046FBA0。
static std::int32_t WindowShortcutBar_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic55EC58 {0x18229, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EC58。

static WindowGraphicData gWindowGraphic55EC70 {0x3B90E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EC70。

static WindowGraphicData gWindowGraphic55EC88 {0x3B911, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EC88。

static WindowGraphicData gWindowGraphic55ECA0 {0x3B914, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ECA0。

static WindowGraphicData gWindowGraphic55ECB8 {0x3B917, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ECB8。

static WindowGraphicData gWindowGraphic55ECD0 {0x3B91A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ECD0。

static WindowGraphicData gWindowGraphic55ECE8 {0x3B91D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ECE8。

static WindowGraphicData gWindowGraphic55ED00 {0x3B920, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ED00。

static WindowGraphicData gWindowGraphic55ED18 {0x3B923, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ED18。

static WindowGraphicData gWindowGraphic55ED30 {0x18223, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ED30。

static WindowGraphicData gWindowGraphic55ED48 {0x3B90D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ED48。

static std::uint32_t gWindowMouseFlagsD67620 = 0; // 原全局：0x00D67620。

WindowPrototype& gWindowPrototype54E240 = gWindowPrototypeShortcutBar; // 原全局：0x0054E240。

// 原57C130指向的元素数组；实际初值和回调绑定位于windows/各窗口文件；共享载荷与索引表位于WindowPrototypes.cpp，本头声明供窗口调用侧使用。
static WindowElementPrototype gWindowElements55ED60[11] { // 原全局：0x0055ED60。
    {WindowElementType_Graphic, 14, 3, 20, 20, 1, {.graphic = &gWindowGraphic55EC70}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons)},
    {WindowElementType_Graphic, 35, 3, 20, 20, 1, {.graphic = &gWindowGraphic55EC88}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_2)},
    {WindowElementType_Graphic, 56, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ECA0}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_3)},
    {WindowElementType_Graphic, 77, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ECB8}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_4)},
    {WindowElementType_Graphic, 98, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ECD0}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_5)},
    {WindowElementType_Graphic, 119, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ECE8}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_6)},
    {WindowElementType_Graphic, 140, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ED00}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_7)},
    {WindowElementType_Graphic, 161, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ED18}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_8)},
    {WindowElementType_Graphic, 182, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ED30}, &WindowShortcutBar_HandleButtons, -1, static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_9)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55EC58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShortcutBarElementId::Element10)},
    {WindowElementType_MouseArea, 0, 0, 14, 33, 1, {.mouseFlags = &gWindowMouseFlagsD67620}, &WindowShortcutBar_ButtonCollapse, -1, static_cast<ElementId>(WindowShortcutBarElementId::ButtonCollapse)},
};

static WindowElementPrototype gWindowElements55EEC0[2] { // 原全局：0x0055EEC0。
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55ED48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowShortcutBarElementId_gWindowElements55EEC0::Element1)},
    {WindowElementType_MouseArea, 0, 0, 14, 33, 1, {.mouseFlags = &gWindowMouseFlagsD67620}, &WindowShortcutBar_ButtonExpand, -1, static_cast<ElementId>(WindowShortcutBarElementId_gWindowElements55EEC0::ButtonExpand)},
};

// 原型：窗口4；原全局：0x0054E240。
WindowPrototype gWindowPrototypeShortcutBar {0, 5, 437, 0, 210, 28, 0x80000080u, 11, gWindowElements55ED60, &WindowShortcutBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 4: 54E240

// 原型：窗口5；原全局：0x0054E270。
WindowPrototype gWindowPrototypeCollapsedShortcutBar {0, 5, 626, 0, 14, 33, 0x80000080u, 2, gWindowElements55EEC0, &WindowShortcutBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 5: 54E270

// 窗口函数实现。

// 原函数：0x0046FBA0。
static std::int32_t WindowShortcutBar_Update(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (world::gCurrentSceneType == 10) {
        currentWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x0046FBD0。
static std::int32_t WindowShortcutBar_ButtonCollapse(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    currentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, WindowId_CollapsedShortcutBar));
    WindowRuntime_Settings(*callbackWindow.ownerSession).shortcutBarCollapsed = 1;
    return 1;
}

// 原函数：0x0046FC10。
static std::int32_t WindowShortcutBar_ButtonExpand(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    currentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, WindowId_ShortcutBar));
    WindowRuntime_Settings(*callbackWindow.ownerSession).shortcutBarCollapsed = 0;
    return 1;
}

// 原函数：0x0046FC60。
static std::int32_t WindowShortcutBar_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        std::int32_t consumed = hover ? 1 : 0;
        if (elementId == static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_9)) {
            if (hover) {
                gHoverTextD68AE8 = L"进行摆摊" /* 原常量参考：0x005862A0。 */;
                graphic->imageNo = 0x18224;
            } else {
                graphic->imageNo = 0x18225;
            }
            if ((state & MouseFlag_LHoldPhysical) != 0) {
                graphic->imageNo = 0x18223;
                consumed = 1;
            }
            if ((state & MouseFlag_LDownEvent) == 0) {
                return consumed;
            }
            if (world::gCurrentSceneType != 10) {
                const std::uint64_t now = platform::ReadTickCount64();
                if (gWindowActionTick62B5AC + 500u < now) {
                    WindowInstance* const window = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_OwnStall].window;
                    if (window == nullptr) {
                        static_cast<void>(WindowStallBrowse_RequestClose());
                        static_cast<void>(audio::PlaySe_52C450(56, 320));
                        static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_OwnStall, 1, 0, nullptr));
                    } else {
                        window->state |= 0x8000u;
                        static_cast<void>(audio::PlaySe_52C450(57, 320));
                    }
                    gWindowActionTick62B5AC = platform::ReadTickCount64();
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
            }
            return 1;
        }

        graphic->imageNo = 0x3B90E + index * 3 + (hover ? 2 : 0);
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B90F + index * 3;
        }
        if (static_cast<std::uint32_t>(index) > 7u) {
            return consumed;
        }
        if (elementId < static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_6)) {
            const wchar_t* const hints[5] {
                L"要求进行对战。" /* 原常量参考：0x00556C80。 */, L"要求进行观战。" /* 原常量参考：0x00556C90。 */, L"要求加入冒险队伍。" /* 原常量参考：0x00556CA0。 */,
                L"要求交换名片。" /* 原常量参考：0x00556CB4。 */, L"要求进行交易。" /* 原常量参考：0x00556CD8。 */,
            };
            void (*const requests[5])() {
                &world::RequestDuelWithFacingPlayer_470050, &world::RequestSpectateFacingBattle_470150,
                &world::RequestPartyJoinOrLeave_470270, &world::RequestFacingPlayerContactCard_4703C0,
                &world::RequestNearbyPlayerTrade_4704A0,
            };
            if (hover) {
                gHoverTextD68AE8 = hints[index];
            }
            if ((state & MouseFlag_LDownEvent) != 0) {
                requests[index]();
                return 1;
            }
            return consumed;
        }
        if (elementId == static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_6)) {
            if (callbackWindow.ownerSession->Connection().PukVersion() < 7u) {
                if (hover) {
                    graphic->imageNo = 0x3B91D;
                    gHoverTextD68AE8 = kStatusGuildUnavailable586270;
                }
                return consumed;
            }
            if (hover) {
                gHoverTextD68AE8 = L"邀请对方加入自己的家族。" /* 原常量参考：0x00586284。 */;
            }
            if ((state & MouseFlag_LDownEvent) != 0) {
                world::RequestFacingPlayerGuildInvitation_470670();
                return 1;
            }
            return consumed;
        }
        if (elementId == static_cast<ElementId>(WindowShortcutBarElementId::HandleButtons_7)) {
            if (hover) {
                gHoverTextD68AE8 = L"打开情绪动作视窗。" /* 原常量参考：0x00556CE8。 */;
            }
            return WindowStatusBar_ButtonEmote(callbackWindow, NativeElementKey(6), state);
        }
        if (hover) {
            gHoverTextD68AE8 = L"打开小地图视窗。" /* 原常量参考：0x0058625C。 */;
        }
    }
    return WindowStatusBar_ButtonMiniMap(callbackWindow, NativeElementKey(7), state);
}

} // namespace detail

} // namespace cg::ui
