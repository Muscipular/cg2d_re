#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/BattleState.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/MailState.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowStatusBarElementId : ElementId {
    Element1 = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    HandleHintButton = 12,
    HandleHintButton_13 = 13,
    HandleHintButton_14 = 14,
    Element15 = 15,
    Element16 = 16,
    ButtonCompact = 17,
    HandleOptionButtons = 18,
    HandleOptionButtons_19 = 19,
    HandleOptionButtons_20 = 20,
    HandleOptionButtons_21 = 21,
    HandleOptionButtons_22 = 22,
    HandleOptionButtons_23 = 23,
    HandleHover = 24,
};

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowStatusBarElementId_gWindowElements55F3A0 : ElementId {
    Element1 = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    ButtonCollapse = 7,
};

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowStatusBarElementId_gWindowElements55F480 : ElementId {
    Element1 = 1,
    ButtonExpand = 2,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004723D0。
static std::int32_t WindowStatusBar_HandleHintButton(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // SHP01：三项提示完整点击/取样序，原门槛和关闭开窗保持。

// 原函数：0x00470AC0。
static void WindowStatusBar_Refresh(WindowInstance* callbackWindow); // TUC01：窗口1HP/FP、时钟/四位倒计时/提示原生产序；机器ABI与运行待。

// 原函数：0x004718F0。
static void WindowCompactStatusBar_Refresh(WindowInstance* callbackWindow); // TUC01：窗口2直接FP复制/四位倒计时，首窗口取样晚于描述符发布。

// 原函数：0x00471CD0。
static std::int32_t WindowStatusBar_ButtonCompact(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 状态栏1点击切2，调用后发布保存模式。

// 原函数：0x00471D10。
static std::int32_t WindowCompactStatusBar_ButtonCollapse(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 状态栏2点击切3。

// 原函数：0x00471D60。
static std::int32_t WindowCollapsedStatusBar_ButtonExpand(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 状态栏3按场景返回2或1。

// 原函数：0x00471DD0。
static std::int32_t WindowStatusBar_HandleHover(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 仅hover命中消费。

// 原函数：0x00471DF0。
static std::int32_t WindowStatusBar_HandleOptionButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 六选项命中区操作前15项图形，发送后SE51。

// 原函数：0x00473080。
static std::int32_t WindowStatusBar_ReadInjuredPetIndex(session::ClientSession& session); // SHP01：玩家伤势优先返0，否则宠物原编号+10/未命中-1。

// 原函数：0x004731B0。
static std::int32_t WindowStatusBar_FindSortedPetIndex(session::ClientSession& session, std::int32_t index); // SHP01：五个S16配对首匹配位置，无则-1，不查used。

// 原函数：0x00470A80。
// 关联原入口：0x00470A9F（共享尾已并入本函数正文）。
static std::int32_t WindowStatusBar_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x00470AB0。
static std::int32_t WindowCompactStatusBar_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00D40B18。 状态栏type7共用原文本跨度，非新截断门槛。
static std::wstring gStatusHpTextD40B18 {}; // 原全局：0x00D40B18。

static std::wstring gStatusFpTextD45D50 {}; // 原全局：0x00D45D50。

static constexpr double kStatusTimeScale54F638 = 0.16666666666666669; // 原常量参考：0x0054F638。

static WindowGraphicData gWindowGraphic55EF00 {0x3B92B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF00。

static WindowGraphicData gWindowGraphic55EF18 {0x3B92F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF18。

static WindowGraphicData gWindowGraphic55EF30 {0x3B932, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF30。

static WindowGraphicData gWindowGraphic55EF48 {0x3B935, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF48。

static WindowGraphicData gWindowGraphic55EF60 {0x3B938, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF60。

static WindowGraphicData gWindowGraphic55EF78 {0x3B93B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF78。

static WindowGraphicData gWindowGraphic55EF90 {0x3B93E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF90。

static WindowGraphicData gWindowGraphic55EFA8 {0x3B941, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EFA8。

static WindowGraphicData gWindowGraphic55EFC0 {0x3B950, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EFC0。

static WindowGraphicData gWindowGraphic55EFD8 {0x3B954, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EFD8。

static WindowGraphicData gWindowGraphic55EFF0 {0x3B92D, 0, 3, 60, 15, 0xFFFFFFFFu}; // 原全局：0x0055EFF0。

static WindowGraphicData gWindowGraphic55F008 {0x3B664, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F008。

static WindowGraphicData gWindowGraphic55F020 {0x3B664, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F020。

static WindowGraphicData gWindowGraphic55F038 {0x3B664, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F038。

static WindowGraphicData gWindowGraphic55F050 {0x3B92A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F050。

static WindowGraphicData gWindowGraphic55F068 {0x3B92C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F068。

static WindowNumberData gWindowNumber55F090 {&gStatusHpTextD40B18, 10, 0, 0, 0}; // 原全局：0x0055F090。

static WindowNumberData gWindowNumber55F098 {&gStatusFpTextD45D50, 10, 0, 0, 0}; // 原全局：0x0055F098。

static std::uint32_t gWindowMouseFlagsD67650 = 0; // 原全局：0x00D67650。

WindowPrototype& gWindowPrototype54E2A0 = gWindowPrototypeStatusBar; // 原全局：0x0054E2A0。

static WindowElementPrototype gWindowElements55F0A0[24] { // 原全局：0x0055F0A0。
    {WindowElementType_Number, 20, 19, 0, 0, 1, {.number = &gWindowNumber55F090}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element1)},
    {WindowElementType_Number, 20, 29, 0, 0, 1, {.number = &gWindowNumber55F098}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element2)},
    {WindowElementType_Graphic, 63, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF18}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element3)},
    {WindowElementType_Graphic, 82, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element4)},
    {WindowElementType_Graphic, 101, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element5)},
    {WindowElementType_Graphic, 120, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF60}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element6)},
    {WindowElementType_Graphic, 139, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element7)},
    {WindowElementType_Graphic, 158, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF90}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element8)},
    {WindowElementType_Graphic, 145, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F008}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element9)},
    {WindowElementType_Graphic, 158, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element10)},
    {WindowElementType_Graphic, 165, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F038}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element11)},
    {WindowElementType_Graphic, 6, 42, 36, 20, 1, {.graphic = &gWindowGraphic55EFA8}, &WindowStatusBar_HandleHintButton, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleHintButton)},
    {WindowElementType_Graphic, 51, 41, 36, 20, 1, {.graphic = &gWindowGraphic55EFC0}, &WindowStatusBar_HandleHintButton, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleHintButton_13)},
    {WindowElementType_Graphic, 83, 41, 36, 20, 1, {.graphic = &gWindowGraphic55EFD8}, &WindowStatusBar_HandleHintButton, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleHintButton_14)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55EF00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element15)},
    {WindowElementType_Graphic, 3, 4, 0, 0, 1, {.graphic = &gWindowGraphic55EFF0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId::Element16)},
    {WindowElementType_EventArea, 175, 0, 15, 33, 1, {}, &WindowStatusBar_ButtonCompact, -1, static_cast<ElementId>(WindowStatusBarElementId::ButtonCompact)},
    {WindowElementType_MouseArea, 63, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons)},
    {WindowElementType_MouseArea, 82, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons_19)},
    {WindowElementType_MouseArea, 101, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons_20)},
    {WindowElementType_MouseArea, 120, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons_21)},
    {WindowElementType_MouseArea, 139, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons_22)},
    {WindowElementType_MouseArea, 158, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons_23)},
    {WindowElementType_EventArea, 0, 40, 120, 30, 1, {}, &WindowStatusBar_HandleHover, -1, static_cast<ElementId>(WindowStatusBarElementId::HandleHover)},
};

static WindowElementPrototype gWindowElements55F3A0[7] { // 原全局：0x0055F3A0。
    {WindowElementType_Number, 20, 2, 0, 0, 1, {.number = &gWindowNumber55F090}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F3A0::Element1)},
    {WindowElementType_Number, 20, 12, 0, 0, 1, {.number = &gWindowNumber55F098}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F3A0::Element2)},
    {WindowElementType_Graphic, 146, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F008}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F3A0::Element3)},
    {WindowElementType_Graphic, 158, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F3A0::Element4)},
    {WindowElementType_Graphic, 165, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F038}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F3A0::Element5)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F050}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F3A0::Element6)},
    {WindowElementType_MouseArea, 181, 0, 16, 37, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowCompactStatusBar_ButtonCollapse, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F3A0::ButtonCollapse)},
};

static WindowElementPrototype gWindowElements55F480[2] { // 原全局：0x0055F480。
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F068}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F480::Element1)},
    {WindowElementType_MouseArea, 0, 0, 19, 34, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowCollapsedStatusBar_ButtonExpand, -1, static_cast<ElementId>(WindowStatusBarElementId_gWindowElements55F480::ButtonExpand)},
};

// 原型：窗口1；原全局：0x0054E2A0。
WindowPrototype gWindowPrototypeStatusBar {0, 5, 0, 0, 197, 40, 0x80000080u, 24, gWindowElements55F0A0, &WindowStatusBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 1: 54E2A0

// 原型：窗口2；原全局：0x0054E2D0。
WindowPrototype gWindowPrototypeCompactStatusBar {0, 5, 0, 0, 197, 32, 0x80000080u, 7, gWindowElements55F3A0, &WindowCompactStatusBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 2: 54E2D0

// 原型：窗口3；原全局：0x0054E300。
WindowPrototype gWindowPrototypeCollapsedStatusBar {0, 5, 0, 0, 19, 34, 0x80000080u, 2, gWindowElements55F480, &WindowRuntime_UpdateDefault, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 3: 54E300

// 窗口函数实现。

// 两刷新同序读取/复制HP并格式化FP；FP复制阶段的窗口采样差别留给caller。
static void WindowStatusBar_FormatVitals(session::ClientSession& session, std::wstring& buffer)
{
    cg::yobi::YobiAction* battlePlayer = nullptr;
    if (session.WorldScene().sceneType == 10) {
        const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
        if (position < 20) {
            battlePlayer = world::gBattleEntries59E980[position];
        }
    }
    if (battlePlayer != nullptr) {
        const std::int32_t maxHp = cg::yobi::ReadYobiActionMaxHpOnce(battlePlayer);
        const std::int32_t hpPosition = world::gBattleCurrentPosition5ABCA4;
        const std::int32_t hp = cg::yobi::ReadYobiActionHpOnce(world::gBattleEntries59E980[hpPosition]);
        cg::text::FormatTo(buffer, L"{:4}/{:4}" /* 原常量参考：0x00553FE8。 */, hp, maxHp);
        gStatusHpTextD40B18 = buffer;
        const std::int32_t maxFpPosition = world::gBattleCurrentPosition5ABCA4;
        const std::int32_t maxFp =
            cg::yobi::ReadYobiActionMaxFpOnce(world::gBattleEntries59E980[maxFpPosition]);
        const std::int32_t fpPosition = world::gBattleCurrentPosition5ABCA4;
        const std::int32_t fp = cg::yobi::ReadYobiActionFpOnce(world::gBattleEntries59E980[fpPosition]);
        cg::text::FormatTo(buffer, L"{:4}/{:4}" /* 原常量参考：0x00553FE8。 */, fp, maxFp);
    } else {
        const std::int32_t maxHp = static_cast<std::int32_t>(session.WorldScene().player->maxHp);
        const std::int32_t hp = static_cast<std::int32_t>(session.WorldScene().player->hp);
        cg::text::FormatTo(buffer, L"{:4}/{:4}" /* 原常量参考：0x00553FE8。 */, hp, maxHp);
        gStatusHpTextD40B18 = buffer;
        const std::int32_t maxFp = static_cast<std::int32_t>(session.WorldScene().player->maxFp);
        const std::int32_t fp = static_cast<std::int32_t>(session.WorldScene().player->fp);
        cg::text::FormatTo(buffer, L"{:4}/{:4}" /* 原常量参考：0x00553FE8。 */, fp, maxFp);
    }
}

// TUC01：两状态栏四位倒计时；首窗口及各位坐标按原取样序，原单位/图号/门槛保持。
static void WindowStatusBar_DrawTimer(WindowInstance* callbackWindow, const std::int32_t firstElement, render::GraphicDescriptor16* const descriptor)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    const std::int32_t duration = callbackWindow->ownerSession->WorldScene().player->timerRemaining0C;
    const std::int32_t hours = duration / 3600000;
    const std::int32_t hoursTens = hours / 10;
    const std::int32_t hoursUnits = hours % 10;
    const std::int32_t minutes = (duration - hours * 3600000) / 60000;
    const std::int32_t digits[4] {hoursTens, hoursUnits, minutes / 10, minutes % 10};
    if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(firstElement)); indexedElement) indexedElement->visible = 0;
    if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(firstElement + 1)); indexedElement2) indexedElement2->visible = 0;
    if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(firstElement + 2)); indexedElement3) indexedElement3->visible = 0;
    const std::int32_t mode = callbackWindow->ownerSession->WorldScene().player->timerMode10;
    WindowInstance* firstWindow = nullptr; // firstElement的互补分支在使用前必定覆盖。
    if (firstElement == 8) {
        firstWindow = currentWindow;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor->color = color;
    descriptor->mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (firstElement != 8) {
        firstWindow = currentWindow;
    }
    const std::int32_t baseImage = mode != 0 ? 0x3B6A0 : 0x3B664;
    for (std::int32_t digit = 0; digit < 4; ++digit) {
        WindowInstance* const window = digit == 0 ? firstWindow : currentWindow;
        auto* const elementPtr = window ? window->FindElementById(NativeElementKey(firstElement + (digit == 0 ? 0 : digit - 1))) : nullptr;
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            std::int32_t elementY;
            std::int32_t elementX;
            std::int32_t windowY;
            if (digit == 0) {
                elementY = element.y;
                elementX = element.x;
                windowY = window->y;
            } else if (digit < 3) {
                windowY = window->y;
                elementY = element.y;
                elementX = element.x;
            } else {
                elementY = element.y;
                windowY = window->y;
                elementX = element.x;
            }
            const std::int32_t y = elementY + windowY + 3;
            const std::int32_t windowX = window->x;
            const std::int32_t x = elementX + windowX + (digit == 0 ? -6 : 1);
            static_cast<void>(cg::render::DrawGraphic(x, y, 0xDF, baseImage + digits[digit], 0, descriptor));
        }
    }
}

// 原函数：0x004708A0。
std::int32_t WindowStatusBar_ButtonEmote(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* window = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Emote].window;
    if (window != nullptr) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3B921;
        window = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Emote].window;
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    if (window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_Emote, 1, 0, nullptr));
    } else {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x00470910。
std::int32_t WindowStatusBar_ButtonMiniMap(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* window = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_MiniMap].window;
    if (window != nullptr) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3B924;
        window = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_MiniMap].window;
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    if (window == nullptr) {
        static_cast<void>(WindowMiniMap_Open(*callbackWindow.ownerSession, 0, 0, nullptr));
    } else {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x00470A80。
// 关联原入口：0x00470A9F（共享尾已并入本函数正文）。
static std::int32_t WindowStatusBar_Update(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (callbackWindow.ownerSession->WorldScene().sceneType == 10) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, WindowId_CompactStatusBar));
    }
    WindowStatusBar_Refresh(&callbackWindow);
    return 1;
}

// 原函数：0x00470AB0。
static std::int32_t WindowCompactStatusBar_Update(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowCompactStatusBar_Refresh(&callbackWindow);
    return 1;
}

// TUC01：原窗口1完整根；描述符原零初始化、HP/FP/时钟/提示生产保持，四位消费见共用 helper。
// 原函数：0x00470AC0。
static void WindowStatusBar_Refresh(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    render::GraphicDescriptor16 descriptor {};
    std::wstring buffer;
    WindowStatusBar_FormatVitals(*callbackWindow->ownerSession, buffer);
    WindowInstance* const timeWindow = currentWindow;
    gStatusFpTextD45D50 = buffer;
    auto* elementsWindow = timeWindow;
    const std::int32_t hours = std::bit_cast<std::int32_t>(world::gCgTime.hours);
    auto* const element16 = elementsWindow->FindElementById(WindowStatusBarElementId::Element16);
    if (element16) {
        auto* const timeGraphic = element16->data.graphic;
        const std::int32_t minutes = std::bit_cast<std::int32_t>(world::gCgTime.minutes);
        const std::int32_t totalMinutes = hours * 60 + minutes;
        const double offset = totalMinutes * kStatusTimeScale54F638;
        timeGraphic->offsetX = static_cast<std::int32_t>(static_cast<std::int64_t>(offset));
        timeGraphic->offsetY = 0;
        timeGraphic->width = 100;
        timeGraphic->height = 9;
    }
    WindowStatusBar_DrawTimer(callbackWindow, 8, &descriptor);

    const std::int32_t points = world::FindAttributeAllocationHintTarget_504B10(*callbackWindow->ownerSession);
    const std::int32_t initialInjury = callbackWindow->ownerSession->WorldScene().player->valueF4;
    business.statusHasPointsD66E24 = points != 6 ? 1 : 0;
    if (initialInjury == 0) {
        const std::int32_t pet = world::FindFirstInjuredPet_504B60(*callbackWindow->ownerSession);
        business.statusHasInjuryD046C8 = 0;
        if (pet != -1) {
            business.statusHasInjuryD046C8 = 1;
        }
    } else {
        business.statusHasInjuryD046C8 = 1;
    }
    business.statusHasMailCC4B5C = world::detail::UpdateUnreadMailPageHint_518DA0() != 0 ? 1 : 0;
    std::int32_t phase = business.statusBlinkPhaseCF4FBC + 1;
    business.statusBlinkPhaseCF4FBC = phase;
    if (phase > 60) {
        phase = 0;
        business.statusBlinkPhaseCF4FBC = phase;
    }
    const bool pointsBlink = business.statusHasPointsD66E24 != 0 && phase >= 30;
    auto* const handleHintButton_14Element = currentWindow->FindElementById(WindowStatusBarElementId::HandleHintButton_14);
    if (handleHintButton_14Element) {
        handleHintButton_14Element->data.graphic->imageNo =
            pointsBlink ? 0x3B953 : 0x3B954;
    }

    bool injuryBlink = false;
    if (business.statusHasInjuryD046C8 != 0) {
        injuryBlink = business.statusBlinkPhaseCF4FBC >= 30;
    }
    const std::int32_t injury = callbackWindow->ownerSession->WorldScene().player->valueF4;
    const std::int32_t injuryImage = injury == 0 ? 0x3B941
        : injury <= 25 ? 0x3B944
        : injury <= 50 ? 0x3B947
        : injury <= 75 ? 0x3B94A : 0x3B94D;
    auto* const handleHintButtonElement = currentWindow->FindElementById(WindowStatusBarElementId::HandleHintButton);
    if (handleHintButtonElement) {
        handleHintButtonElement->data.graphic->imageNo =
            injuryImage - (injuryBlink ? 1 : 0);
    }
    const bool mailBlink = business.statusHasMailCC4B5C != 0
        && business.statusBlinkPhaseCF4FBC >= 30;
    auto* const handleHintButton_13Element = currentWindow->FindElementById(WindowStatusBarElementId::HandleHintButton_13);
    if (handleHintButton_13Element) {
        handleHintButton_13Element->data.graphic->imageNo =
            mailBlink ? 0x3B94F : 0x3B950;
    }
    return;
}

// TUC01：原窗口2完整根；直接FP复制/原零描述符保持，仅消费四位倒计时。
// 原函数：0x004718F0。
static void WindowCompactStatusBar_Refresh(WindowInstance* callbackWindow)
{
    render::GraphicDescriptor16 descriptor {};
    std::wstring buffer;
    WindowStatusBar_FormatVitals(*callbackWindow->ownerSession, buffer);
    gStatusFpTextD45D50 = buffer;
    WindowStatusBar_DrawTimer(callbackWindow, 2, &descriptor);
}

// 原函数：0x00471CD0。
static std::int32_t WindowStatusBar_ButtonCompact(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    currentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, WindowId_CompactStatusBar));
    WindowRuntime_Settings(*callbackWindow.ownerSession).statusBarMode = 1;
    return 1;
}

// 原函数：0x00471D10。
static std::int32_t WindowCompactStatusBar_ButtonCollapse(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    currentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, WindowId_CollapsedStatusBar));
    WindowRuntime_Settings(*callbackWindow.ownerSession).statusBarMode = 2;
    return 1;
}

// 原函数：0x00471D60。
static std::int32_t WindowCollapsedStatusBar_ButtonExpand(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    const std::int32_t scene = callbackWindow.ownerSession->WorldScene().sceneType;
    currentWindow->state |= 0x8000u;
    if (scene == 10) {
        static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, WindowId_CompactStatusBar));
        WindowRuntime_Settings(*callbackWindow.ownerSession).statusBarMode = 1;
    } else {
        static_cast<void>(WindowRuntime_Open(*callbackWindow.ownerSession, WindowId_StatusBar));
        WindowRuntime_Settings(*callbackWindow.ownerSession).statusBarMode = 0;
    }
    return 1;
}

// 原函数：0x00471DD0。
static std::int32_t WindowStatusBar_HandleHover(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    return (state & MouseFlag_Hover) != 0 ? 1 : 0;
}

// 原函数：0x00471DF0。
static std::int32_t WindowStatusBar_HandleOptionButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    // 原先访问对应图形，再分派命中区编号；不将默认分支改成入口保护。
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index - 15));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        std::int32_t consumed = hover ? 1 : 0;
        graphic->imageNo = 0x3B8FC + index * 3 + (hover ? 1 : 0);
        if (elementId < static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons) || elementId > static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons_23)) {
            return consumed;
        }
        if (elementId == static_cast<ElementId>(WindowStatusBarElementId::HandleOptionButtons_23) && callbackWindow.ownerSession->Connection().PukVersion() < 7u) {
            if (hover) {
                graphic->imageNo = 0x3B93E;
                uiFrame.hoverText = kStatusGuildUnavailable586270;
            }
            return consumed;
        }
        const std::uint8_t options = world::ReadPlayerOptions194LowByte(*callbackWindow.ownerSession);
        constexpr std::uint8_t masks[6] {0x04, 0x08, 0x01, 0x10, 0x20, 0x40};
        const wchar_t* const hints[6][2] {
            {L"现在不能跟别人对战。" /* 原常量参考：0x00556E14。 */, L"现在可以跟别人对战。" /* 原常量参考：0x00556E2C。 */},
            {L"现在进行普通的聊天。" /* 原常量参考：0x00556E44。 */, L"现在进行小队的聊天。" /* 原常量参考：0x00556E5C。 */},
            {L"现在禁止别人与自己组队。" /* 原常量参考：0x00556E74。 */, L"现在允许别人与自己组队。" /* 原常量参考：0x00556E90。 */},
            {L"现在禁止别人与自己交换名片。" /* 原常量参考：0x00556EAC。 */, L"现在允许别人与自己交换名片。" /* 原常量参考：0x00556ECC。 */},
            {L"现在禁止别人与自己交易。" /* 原常量参考：0x00556EEC。 */, L"现在允许别人与自己交易。" /* 原常量参考：0x00556F08。 */},
            {L"现在禁止别人邀请自己参加家族。" /* 原常量参考：0x005862D4。 */, L"现在允许别人邀请自己参加家族。" /* 原常量参考：0x005862F4。 */},
        };
        network::SendResult (*const toggleCallbacks[6])(session::ClientSession&) {
            &world::ToggleDuelPermission_472F40, &world::TogglePartyChat_472F70, &world::TogglePartyJoinPermission_472FA0,
            &world::ToggleContactCardExchangePermission_472FD0, &world::ToggleTradePermission_473000, &world::ToggleGuildInvitePermission_473030,
        };
        const std::int32_t slot = index - 17;
        const bool enabled = (options & masks[slot]) != 0;
        if (hover) {
            uiFrame.hoverText = hints[slot][enabled ? 1 : 0];
        }
        if (enabled) {
            graphic->imageNo = 0x3B92E + slot * 3;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(toggleCallbacks[slot](*callbackWindow.ownerSession));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        return consumed;
    }
    return 0;
}

// SHP01：原玩家伤势优先返0，否则宠物编号+10或-1。
// 原函数：0x00473080。
static std::int32_t WindowStatusBar_ReadInjuredPetIndex(session::ClientSession& session)
{
    if (session.WorldScene().player->valueF4 != 0) {
        return 0;
    }
    const std::int32_t pet = world::FindFirstInjuredPet_504B60(session);
    return pet == -1 ? -1 : pet + 10;
}

// SHP01：原五S16配对首个编号匹配位置，无则-1。
// 原函数：0x004731B0。
static std::int32_t WindowStatusBar_FindSortedPetIndex(session::ClientSession& session, const std::int32_t index)
{
    for (std::int32_t position = 0; position < 5; ++position) {
        const std::int16_t candidate =
            std::bit_cast<std::int16_t>(session.WorldScene().petPairs[position].index);
        if (index == candidate) {
            return position;
        }
    }
    return -1;
}

// SHP01：原三项提示/关闭开窗；hover持有及姓名/分档/窗口读取序，原业务门槛保持。
// 原函数：0x004723D0。
static std::int32_t WindowStatusBar_HandleHintButton(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    std::wstring buffer;
    switch (static_cast<WindowStatusBarElementId>(elementId)) {
    case WindowStatusBarElementId::HandleHintButton_14: {
        const std::int32_t candidate = world::FindAttributeAllocationHintTarget_504B10(*callbackWindow.ownerSession);
        const bool hover = (state & MouseFlag_Hover) != 0;
        if (hover) {
            if (business.statusHasPointsD66E24 != 0) {
                if (candidate <= 4) {
                    const std::int16_t petIndex =
                        std::bit_cast<std::int16_t>(callbackWindow.ownerSession->WorldScene().petPairs[candidate].index);
                    const auto& pet = (*callbackWindow.ownerSession->WorldScene().petRecords)[petIndex];
                    const wchar_t* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    const std::int32_t points = pet.lvFlag6C;
                    cg::text::FormatTo(buffer, L"现在{}拥有{}点的能力点数。" /* 原常量参考：0x00586314。 */, name, points);
                } else {
                    const std::int32_t points = callbackWindow.ownerSession->WorldScene().unspentAttributePoints;
                    cg::text::FormatTo(buffer, L"现在有增加点数{}点。" /* 原常量参考：0x00556D50。 */, points);
                }
            } else {
                buffer = L"现在没有任何的增加点数。" /* 原常量参考：0x00556D34。 */;
            }
            uiFrame.hoverText = buffer;
        }
        if (business.statusHasPointsD66E24 == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* const hoverWindow = currentWindow;
            consumed = 1;
            if (auto* const handleHintButton_14Element = hoverWindow->FindElementById(WindowStatusBarElementId::HandleHintButton_14); handleHintButton_14Element) handleHintButton_14Element->data.graphic->imageNo = 0x3B955;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (candidate <= 4) {
                WindowInstance* const details = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetDetail].window;
                business.selectedPetCC3E5C = candidate;
                if (details != nullptr) {
                    details->state |= 0x8000u;
                }
                WindowInstance* const skills = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetSkills].window;
                if (skills != nullptr) {
                    skills->state |= 0x8000u;
                }
                if (WindowInstance* const equipment = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetEquipment].window) {
                    equipment->state |= 0x8000u;
                }
                static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_PetAttributes, 1, 0, nullptr));
            } else {
                WindowInstance* const profile = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PlayerProfile].window;
                if (profile != nullptr) {
                    profile->state |= 0x8000u;
                }
                WindowInstance* const titles = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PlayerTitles].window;
                if (titles != nullptr) {
                    titles->state |= 0x8000u;
                }
                static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_PlayerAttributes, 1, 0, nullptr));
            }
            consumed = 1;
        }
        return consumed;
    }
    case WindowStatusBarElementId::HandleHintButton_13: {
        const std::int32_t hasMail = business.statusHasMailCC4B5C;
        const bool hover = (state & MouseFlag_Hover) != 0;
        if (hover) {
            uiFrame.hoverText = hasMail != 0 ? L"现在有新邮件。" /* 原常量参考：0x00556D24。 */ : L"现在没有邮件。" /* 原常量参考：0x00556D14。 */;
            if (hasMail == 0) {
                return 0;
            }
        }
        if (hasMail == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* const hoverWindow = currentWindow;
            consumed = 1;
            if (auto* const handleHintButton_13Element = hoverWindow->FindElementById(WindowStatusBarElementId::HandleHintButton_13); handleHintButton_13Element) handleHintButton_13Element->data.graphic->imageNo = 0x3B951;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            world::detail::FindFirstUnreadMailContact_518F00(
                &business.statusMailCategoryD66E34, &business.statusMailIndexD2E2D8);
            const std::int32_t category = business.statusMailCategoryD66E34;
            if (category == 0 || category == 1 || category == 2) {
                const std::int32_t mailIndex = business.statusMailIndexD2E2D8;
                business.mailCategoryD67B64 = category;
                world::detail::gViewedMailContactId_CF4FC0 = mailIndex;
                business.mailCategoryCC4D50 = category;
                static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_MailRead, 1, 0, nullptr));
            }
            consumed = 1;
        }
        return consumed;
    }
    case WindowStatusBarElementId::HandleHintButton: {
        const std::int32_t target = WindowStatusBar_ReadInjuredPetIndex(*callbackWindow.ownerSession);
        std::int32_t injury = callbackWindow.ownerSession->WorldScene().player->valueF4;
        const bool hover = (state & MouseFlag_Hover) != 0;
        const std::int32_t petIndex = target - 10;
        if (hover) {
            if (injury != 0) {
                const wchar_t* const text = injury <= 25 ? L"现在受了一点小伤。" /* 原常量参考：0x00556E00。 */
                    : injury <= 50 ? L"现在受了轻伤。" /* 原常量参考：0x00556DF0。 */
                    : injury <= 75 ? L"现在受到重伤。" /* 原常量参考：0x00556DE0。 */ : L"现在正面临死亡边缘。" /* 原常量参考：0x00556DC8。 */;
                uiFrame.hoverText = text;
            } else if (target == -1) {
                uiFrame.hoverText = L"现在十分健康。" /* 原常量参考：0x00556D68。 */;
            } else {
                const auto& pet = (*callbackWindow.ownerSession->WorldScene().petRecords)[petIndex];
                const std::int32_t petInjury = pet.values70[25];
                const wchar_t* name;
                const wchar_t* format;
                if (petInjury <= 25) {
                    format = L"现在{}受到擦伤。" /* 原常量参考：0x00556DB4。 */;
                    name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                } else if (petInjury <= 50) {
                    format = L"现在{}受到轻伤。" /* 原常量参考：0x00556DA0。 */;
                    name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                } else {
                    const wchar_t nameFirst = pet.name6BD[0];
                    const bool critical = petInjury > 75;
                    name = nameFirst != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    format = critical ? L"现在{}濒临死亡。" /* 原常量参考：0x00556D78。 */ : L"现在{}受了重伤。" /* 原常量参考：0x00556D8C。 */;
                }
                cg::text::FormatTo(buffer, format, name);
                uiFrame.hoverText = buffer;
                injury = callbackWindow.ownerSession->WorldScene().player->valueF4;
                static_cast<void>(0);
            }
        }
        if (business.statusHasInjuryD046C8 == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* imageWindow;
            std::int32_t image;
            if (injury == 0) {
                imageWindow = currentWindow;
                image = 0x3B942;
            } else if (injury <= 25) {
                imageWindow = currentWindow;
                image = 0x3B945;
            } else if (injury <= 50) {
                imageWindow = currentWindow;
                image = 0x3B948;
            } else {
                imageWindow = currentWindow;
                image = injury <= 75 ? 0x3B94B : 0x3B94E;
            }
            if (auto* const handleHintButtonElement = imageWindow->FindElementById(WindowStatusBarElementId::HandleHintButton); handleHintButtonElement) handleHintButtonElement->data.graphic->imageNo = image;
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (target != -1) {
                if (target == 0) {
                    WindowInstance* const attributes = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PlayerAttributes].window;
                    if (attributes != nullptr) {
                        attributes->state |= 0x8000u;
                    }
                    WindowInstance* const titles = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PlayerTitles].window;
                    if (titles != nullptr) {
                        titles->state |= 0x8000u;
                    }
                    static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_PlayerProfile, 1, 0, nullptr));
                } else {
                    WindowInstance* const attributes = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetAttributes].window;
                    if (attributes != nullptr) {
                        attributes->state |= 0x8000u;
                    }
                    WindowInstance* const skills = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetSkills].window;
                    if (skills != nullptr) {
                        skills->state |= 0x8000u;
                    }
                    if (WindowInstance* const equipment = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetEquipment].window) {
                        equipment->state |= 0x8000u;
                    }
                    business.selectedPetCC3E5C = WindowStatusBar_FindSortedPetIndex(*callbackWindow.ownerSession, petIndex);
                    business.valueD673D0 = 5;
                    static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_PetDetail, 1, 0, nullptr));
                }
            }
            consumed = 1;
        }
        return consumed;
    }
    default:
        return 0;
    }
}

} // namespace detail

} // namespace cg::ui
