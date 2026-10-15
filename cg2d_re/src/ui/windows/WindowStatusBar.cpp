#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004723D0。
static std::int32_t WindowStatusBar_HandleHintButton(std::int32_t index, std::int32_t state); // SHP01：三项提示完整点击/取样序，原门槛和关闭开窗保持。

// 原函数：0x00470AC0。
static void WindowStatusBar_Refresh(); // TUC01：窗口1HP/FP、时钟/四位倒计时/提示原生产序；机器ABI与运行待。

// 原函数：0x004718F0。
static void WindowCompactStatusBar_Refresh(); // TUC01：窗口2直接FP复制/四位倒计时，首窗口取样晚于描述符发布。

// 原函数：0x00471CD0。
static std::int32_t WindowStatusBar_ButtonCompact(std::int32_t unused, std::int32_t state); // 状态栏1点击切2，调用后发布保存模式。

// 原函数：0x00471D10。
static std::int32_t WindowCompactStatusBar_ButtonCollapse(std::int32_t unused, std::int32_t state); // 状态栏2点击切3。

// 原函数：0x00471D60。
static std::int32_t WindowCollapsedStatusBar_ButtonExpand(std::int32_t unused, std::int32_t state); // 状态栏3按场景返回2或1。

// 原函数：0x00471DD0。
static std::int32_t WindowStatusBar_HandleHover(std::int32_t unused, std::int32_t state); // 仅hover命中消费。

// 原函数：0x00471DF0。
static std::int32_t WindowStatusBar_HandleOptionButtons(std::int32_t index, std::int32_t state); // 六选项命中区操作前15项图形，发送后SE51。

// 原函数：0x00473080。
static std::int32_t WindowStatusBar_ReadInjuredPetIndex(); // SHP01：玩家伤势优先返0，否则宠物原编号+10/未命中-1。

// 原函数：0x004731B0。
static std::int32_t WindowStatusBar_FindSortedPetIndex(std::int32_t index); // SHP01：五个S16配对首匹配位置，无则-1，不查used。

// 原函数：0x00470A80。
// 关联原入口：0x00470A9F（共享尾已并入本函数正文）。
static std::int32_t WindowStatusBar_Update(std::int32_t value);

// 原函数：0x00470AB0。
static std::int32_t WindowCompactStatusBar_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00D40B18。 状态栏type7共用原文本跨度，非新截断门槛。
static std::wstring gStatusHpTextD40B18 {}; // 原全局：0x00D40B18。

static std::wstring gStatusFpTextD45D50 {}; // 原全局：0x00D45D50。

static constexpr double kStatusTimeScale54F638 = std::bit_cast<double>(std::uint64_t{0x3FC5555555555556}); // 原常量参考：0x0054F638。

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
    {WindowElementType_Number, 20, 19, 0, 0, 1, {.number = &gWindowNumber55F090}, &WindowElement_NoOp},
    {WindowElementType_Number, 20, 29, 0, 0, 1, {.number = &gWindowNumber55F098}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 63, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF18}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 82, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 101, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF48}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 120, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF60}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 139, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 158, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF90}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F008}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 158, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F020}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F038}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 6, 42, 36, 20, 1, {.graphic = &gWindowGraphic55EFA8}, &WindowStatusBar_HandleHintButton},
    {WindowElementType_Graphic, 51, 41, 36, 20, 1, {.graphic = &gWindowGraphic55EFC0}, &WindowStatusBar_HandleHintButton},
    {WindowElementType_Graphic, 83, 41, 36, 20, 1, {.graphic = &gWindowGraphic55EFD8}, &WindowStatusBar_HandleHintButton},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55EF00}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 3, 4, 0, 0, 1, {.graphic = &gWindowGraphic55EFF0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 175, 0, 15, 33, 1, {}, &WindowStatusBar_ButtonCompact},
    {WindowElementType_MouseArea, 63, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 82, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 101, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 120, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 139, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 158, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_EventArea, 0, 40, 120, 30, 1, {}, &WindowStatusBar_HandleHover},
};

static WindowElementPrototype gWindowElements55F3A0[7] { // 原全局：0x0055F3A0。
    {WindowElementType_Number, 20, 2, 0, 0, 1, {.number = &gWindowNumber55F090}, &WindowElement_NoOp},
    {WindowElementType_Number, 20, 12, 0, 0, 1, {.number = &gWindowNumber55F098}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 146, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F008}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 158, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F020}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F038}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F050}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 181, 0, 16, 37, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowCompactStatusBar_ButtonCollapse},
};

static WindowElementPrototype gWindowElements55F480[2] { // 原全局：0x0055F480。
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F068}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 0, 0, 19, 34, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowCollapsedStatusBar_ButtonExpand},
};

// 原型：窗口1；原全局：0x0054E2A0。
WindowPrototype gWindowPrototypeStatusBar {0, 5, 0, 0, 197, 40, 0x80000080u, 24, gWindowElements55F0A0, &WindowStatusBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 1: 54E2A0

// 原型：窗口2；原全局：0x0054E2D0。
WindowPrototype gWindowPrototypeCompactStatusBar {0, 5, 0, 0, 197, 32, 0x80000080u, 7, gWindowElements55F3A0, &WindowCompactStatusBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 2: 54E2D0

// 原型：窗口3；原全局：0x0054E300。
WindowPrototype gWindowPrototypeCollapsedStatusBar {0, 5, 0, 0, 19, 34, 0x80000080u, 2, gWindowElements55F480, &WindowRuntime_ReturnOne, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 3: 54E300

// 窗口函数实现。

// 两刷新同序读取/复制HP并格式化FP；FP复制阶段的窗口采样差别留给caller。
static void WindowStatusBar_FormatVitals(std::wstring& buffer)
{
    login::YobiAction* battlePlayer = nullptr;
    if (world::gCurrentSceneType == 10) {
        const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
        if (position < 20) {
            battlePlayer = world::detail::gBattleEntries59E980[position];
        }
    }
    if (battlePlayer != nullptr) {
        const std::int32_t maxHp = login::detail::ReadYobiActionMaxHpOnce(battlePlayer);
        const std::int32_t hpPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t hp = login::detail::ReadYobiActionHpOnce(world::detail::gBattleEntries59E980[hpPosition]);
        cg::text::FormatTo(buffer, L"{:4}/{:4}" /* 原常量参考：0x00553FE8。 */, hp, maxHp);
        gStatusHpTextD40B18 = buffer;
        const std::int32_t maxFpPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t maxFp =
            login::detail::ReadYobiActionMaxFpOnce(world::detail::gBattleEntries59E980[maxFpPosition]);
        const std::int32_t fpPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t fp = login::detail::ReadYobiActionFpOnce(world::detail::gBattleEntries59E980[fpPosition]);
        cg::text::FormatTo(buffer, L"{:4}/{:4}" /* 原常量参考：0x00553FE8。 */, fp, maxFp);
    } else {
        const std::int32_t maxHp = world::detail::gPlayerRuntimeSlots.maxHp.value();
        const std::int32_t hp = world::detail::gPlayerRuntimeSlots.hp.value();
        cg::text::FormatTo(buffer, L"{:4}/{:4}" /* 原常量参考：0x00553FE8。 */, hp, maxHp);
        gStatusHpTextD40B18 = buffer;
        const std::int32_t maxFp = world::detail::gPlayerRuntimeSlots.maxFp.value();
        const std::int32_t fp = world::detail::gPlayerRuntimeSlots.fp.value();
        cg::text::FormatTo(buffer, L"{:4}/{:4}" /* 原常量参考：0x00553FE8。 */, fp, maxFp);
    }
}

// TUC01：两状态栏四位倒计时；首窗口及各位坐标按原取样序，原单位/图号/门槛保持。
static void WindowStatusBar_DrawTimer(const std::int32_t firstElement, render::GraphicDescriptor16* const descriptor)
{
    const std::int32_t duration = world::detail::gPlayerRuntimeSlots.timerRemaining0C;
    const std::int32_t hours = duration / 3600000;
    const std::int32_t hoursTens = hours / 10;
    const std::int32_t hoursUnits = hours % 10;
    const std::int32_t minutes = (duration - hours * 3600000) / 60000;
    const std::int32_t digits[4] {hoursTens, hoursUnits, minutes / 10, minutes % 10};
    gCurrentWindow->elements[firstElement].visible = 0;
    gCurrentWindow->elements[firstElement + 1].visible = 0;
    gCurrentWindow->elements[firstElement + 2].visible = 0;
    const std::int32_t mode = world::detail::gPlayerRuntimeSlots.timerMode10;
    WindowInstance* firstWindow;
    if (firstElement == 8) {
        firstWindow = gCurrentWindow;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor->color = color;
    descriptor->mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (firstElement != 8) {
        firstWindow = gCurrentWindow;
    }
    const std::int32_t baseImage = mode != 0 ? 0x3B6A0 : 0x3B664;
    for (std::int32_t digit = 0; digit < 4; ++digit) {
        WindowInstance* const window = digit == 0 ? firstWindow : gCurrentWindow;
        const WindowElement& element = window->elements[firstElement + (digit == 0 ? 0 : digit - 1)];
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
        static_cast<void>(login::detail::DrawGraphic(x, y, 0xDF, baseImage + digits[digit], 0, descriptor));
    }
}

// 原函数：0x004708A0。
std::int32_t WindowStatusBar_ButtonEmote(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* window = gWindowStates[WindowId_Emote].window;
    if (window != nullptr) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3B921;
        window = gWindowStates[WindowId_Emote].window;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    if (window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Emote, 1, 0, nullptr));
    } else {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x00470910。
std::int32_t WindowStatusBar_ButtonMiniMap(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* window = gWindowStates[WindowId_MiniMap].window;
    if (window != nullptr) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3B924;
        window = gWindowStates[WindowId_MiniMap].window;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    if (window == nullptr) {
        static_cast<void>(WindowMiniMap_Open(0, 0, nullptr));
    } else {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x00470A80。
// 关联原入口：0x00470A9F（共享尾已并入本函数正文）。
static std::int32_t WindowStatusBar_Update(const std::int32_t)
{
    if (world::gCurrentSceneType == 10) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(WindowRuntime_Open(WindowId_CompactStatusBar));
    }
    WindowStatusBar_Refresh();
    return 1;
}

// 原函数：0x00470AB0。
static std::int32_t WindowCompactStatusBar_Update(const std::int32_t)
{
    WindowCompactStatusBar_Refresh();
    return 1;
}

// TUC01：原窗口1完整根；描述符原零初始化、HP/FP/时钟/提示生产保持，四位消费见共用 helper。
// 原函数：0x00470AC0。
static void WindowStatusBar_Refresh()
{
    render::GraphicDescriptor16 descriptor {};
    std::wstring buffer;
    WindowStatusBar_FormatVitals(buffer);
    WindowInstance* const timeWindow = gCurrentWindow;
    gStatusFpTextD45D50 = buffer;
    WindowElement* const elements = timeWindow->elements;
    const std::int32_t hours = std::bit_cast<std::int32_t>(world::gCgTime.hours);
    auto* const timeGraphic = static_cast<WindowGraphicData*>(elements[15].data);
    const std::int32_t minutes = std::bit_cast<std::int32_t>(world::gCgTime.minutes);
    const std::int32_t totalMinutes = hours * 60 + minutes;
    const double offset = totalMinutes * kStatusTimeScale54F638;
    timeGraphic->offsetX = static_cast<std::int32_t>(static_cast<std::int64_t>(offset));
    timeGraphic->offsetY = 0;
    timeGraphic->width = 100;
    timeGraphic->height = 9;
    WindowStatusBar_DrawTimer(8, &descriptor);

    const std::int32_t points = world::detail::Sub_504B10();
    const std::int32_t initialInjury = world::detail::gPlayerRuntimeSlots.valueF4;
    gLoginWindowRuntime.statusHasPointsD66E24 = points != 6 ? 1 : 0;
    if (initialInjury == 0) {
        const std::int32_t pet = world::detail::Sub_504B60();
        gLoginWindowRuntime.statusHasInjuryD046C8 = 0;
        if (pet != -1) {
            gLoginWindowRuntime.statusHasInjuryD046C8 = 1;
        }
    } else {
        gLoginWindowRuntime.statusHasInjuryD046C8 = 1;
    }
    gLoginWindowRuntime.statusHasMailCC4B5C = world::detail::Sub_518DA0() != 0 ? 1 : 0;
    std::int32_t phase = gLoginWindowRuntime.statusBlinkPhaseCF4FBC + 1;
    gLoginWindowRuntime.statusBlinkPhaseCF4FBC = phase;
    if (phase > 60) {
        phase = 0;
        gLoginWindowRuntime.statusBlinkPhaseCF4FBC = phase;
    }
    const bool pointsBlink = gLoginWindowRuntime.statusHasPointsD66E24 != 0 && phase >= 30;
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[13].data)->imageNo =
        pointsBlink ? 0x3B953 : 0x3B954;

    bool injuryBlink = false;
    if (gLoginWindowRuntime.statusHasInjuryD046C8 != 0) {
        injuryBlink = gLoginWindowRuntime.statusBlinkPhaseCF4FBC >= 30;
    }
    const std::int32_t injury = world::detail::gPlayerRuntimeSlots.valueF4;
    const std::int32_t injuryImage = injury == 0 ? 0x3B941
        : injury <= 25 ? 0x3B944
        : injury <= 50 ? 0x3B947
        : injury <= 75 ? 0x3B94A : 0x3B94D;
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[11].data)->imageNo =
        injuryImage - (injuryBlink ? 1 : 0);
    const bool mailBlink = gLoginWindowRuntime.statusHasMailCC4B5C != 0
        && gLoginWindowRuntime.statusBlinkPhaseCF4FBC >= 30;
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[12].data)->imageNo =
        mailBlink ? 0x3B94F : 0x3B950;
}

// TUC01：原窗口2完整根；直接FP复制/原零描述符保持，仅消费四位倒计时。
// 原函数：0x004718F0。
static void WindowCompactStatusBar_Refresh()
{
    render::GraphicDescriptor16 descriptor {};
    std::wstring buffer;
    WindowStatusBar_FormatVitals(buffer);
    gStatusFpTextD45D50 = buffer;
    WindowStatusBar_DrawTimer(2, &descriptor);
}

// 原函数：0x00471CD0。
static std::int32_t WindowStatusBar_ButtonCompact(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(WindowId_CompactStatusBar));
    gValueCF4690 = 1;
    return 1;
}

// 原函数：0x00471D10。
static std::int32_t WindowCompactStatusBar_ButtonCollapse(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(WindowId_CollapsedStatusBar));
    gValueCF4690 = 2;
    return 1;
}

// 原函数：0x00471D60。
static std::int32_t WindowCollapsedStatusBar_ButtonExpand(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    const std::int32_t scene = world::gCurrentSceneType;
    gCurrentWindow->state |= 0x8000u;
    if (scene == 10) {
        static_cast<void>(WindowRuntime_Open(WindowId_CompactStatusBar));
        gValueCF4690 = 1;
    } else {
        static_cast<void>(WindowRuntime_Open(WindowId_StatusBar));
        gValueCF4690 = 0;
    }
    return 1;
}

// 原函数：0x00471DD0。
static std::int32_t WindowStatusBar_HandleHover(const std::int32_t, const std::int32_t state)
{
    return (state & 1) != 0 ? 1 : 0;
}

// 原函数：0x00471DF0。
static std::int32_t WindowStatusBar_HandleOptionButtons(const std::int32_t index, const std::int32_t state)
{
    // 原先访问对应图形，再分派命中区编号；不将默认分支改成入口保护。
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index - 15].data);
    const bool hover = (state & 1) != 0;
    std::int32_t consumed = hover ? 1 : 0;
    graphic->imageNo = 0x3B8FC + index * 3 + (hover ? 1 : 0);
    if (index < 17 || index > 22) {
        return consumed;
    }
    if (index == 22 && character_select::state::gPukVersion < 7u) {
        if (hover) {
            graphic->imageNo = 0x3B93E;
            gHoverTextD68AE8 = kStatusGuildUnavailable586270;
        }
        return consumed;
    }
    const std::uint8_t options = world::detail::ReadPlayerOptions194LowByte();
    constexpr std::uint8_t masks[6] {0x04, 0x08, 0x01, 0x10, 0x20, 0x40};
    const wchar_t* const hints[6][2] {
        {L"現在不能跟別人對戰。" /* 原常量参考：0x00556E14。 */, L"現在可以跟別人對戰。" /* 原常量参考：0x00556E2C。 */},
        {L"現在進行普通的聊天。" /* 原常量参考：0x00556E44。 */, L"現在進行小隊的聊天。" /* 原常量参考：0x00556E5C。 */},
        {L"現在禁止別人與自己組隊。" /* 原常量参考：0x00556E74。 */, L"現在允許別人與自己組隊。" /* 原常量参考：0x00556E90。 */},
        {L"現在禁止別人與自己交換名片。" /* 原常量参考：0x00556EAC。 */, L"現在允許別人與自己交換名片。" /* 原常量参考：0x00556ECC。 */},
        {L"現在禁止別人與自己交易。" /* 原常量参考：0x00556EEC。 */, L"現在允許別人與自己交易。" /* 原常量参考：0x00556F08。 */},
        {L"現在禁止別人邀請自己參加家族。" /* 原常量参考：0x005862D4。 */, L"現在允許別人邀請自己參加家族。" /* 原常量参考：0x005862F4。 */},
    };
    std::int32_t (*const toggleCallbacks[6])() {
        &world::detail::Sub_472F40, &world::detail::Sub_472F70, &world::detail::Sub_472FA0,
        &world::detail::Sub_472FD0, &world::detail::Sub_473000, &world::detail::Sub_473030,
    };
    const std::int32_t slot = index - 17;
    const bool enabled = (options & masks[slot]) != 0;
    if (hover) {
        gHoverTextD68AE8 = hints[slot][enabled ? 1 : 0];
    }
    if (enabled) {
        graphic->imageNo = 0x3B92E + slot * 3;
    }
    if ((state & 2) != 0) {
        static_cast<void>(toggleCallbacks[slot]());
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    return consumed;
}

// SHP01：原玩家伤势优先返0，否则宠物编号+10或-1。
// 原函数：0x00473080。
static std::int32_t WindowStatusBar_ReadInjuredPetIndex()
{
    if (world::detail::gPlayerRuntimeSlots.valueF4 != 0) {
        return 0;
    }
    const std::int32_t pet = world::detail::Sub_504B60();
    return pet == -1 ? -1 : pet + 10;
}

// SHP01：原五S16配对首个编号匹配位置，无则-1。
// 原函数：0x004731B0。
static std::int32_t WindowStatusBar_FindSortedPetIndex(const std::int32_t index)
{
    for (std::int32_t position = 0; position < 5; ++position) {
        const std::int16_t candidate =
            std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
        if (index == candidate) {
            return position;
        }
    }
    return -1;
}

// SHP01：原三项提示/关闭开窗；hover持有及姓名/分档/窗口读取序，原业务门槛保持。
// 原函数：0x004723D0。
static std::int32_t WindowStatusBar_HandleHintButton(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    std::wstring buffer;
    switch (index) {
    case 13: {
        const std::int32_t candidate = world::detail::Sub_504B10();
        const bool hover = (state & 1) != 0;
        if (hover) {
            if (gLoginWindowRuntime.statusHasPointsD66E24 != 0) {
                if (candidate <= 4) {
                    const std::int16_t petIndex =
                        std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[candidate].index);
                    const auto& pet = world::detail::gPetRuntimeRecords[petIndex];
                    const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    const std::int32_t points = pet.lvFlag6C;
                    cg::text::FormatTo(buffer, L"現在{}擁有{}點的能力點數。" /* 原常量参考：0x00586314。 */, cg::text::FromGbkWide(name).c_str(), points);
                } else {
                    const std::int32_t points = world::detail::gValueD2F788;
                    cg::text::FormatTo(buffer, L"現在有增加點數{}點。" /* 原常量参考：0x00556D50。 */, points);
                }
            } else {
                buffer = L"現在沒有任何的增加點數。" /* 原常量参考：0x00556D34。 */;
            }
            gHoverTextD68AE8 = buffer;
        }
        if (gLoginWindowRuntime.statusHasPointsD66E24 == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* const hoverWindow = gCurrentWindow;
            consumed = 1;
            static_cast<WindowGraphicData*>(hoverWindow->elements[13].data)->imageNo = 0x3B955;
        }
        if ((state & 2) != 0) {
            if (candidate <= 4) {
                WindowInstance* const details = gWindowStates[WindowId_PetDetail].window;
                gLoginWindowRuntime.selectedPetCC3E5C = candidate;
                if (details != nullptr) {
                    details->state |= 0x8000u;
                }
                WindowInstance* const skills = gWindowStates[WindowId_PetSkills].window;
                if (skills != nullptr) {
                    skills->state |= 0x8000u;
                }
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetAttributes, 1, 0, nullptr));
            } else {
                WindowInstance* const profile = gWindowStates[WindowId_PlayerProfile].window;
                if (profile != nullptr) {
                    profile->state |= 0x8000u;
                }
                WindowInstance* const titles = gWindowStates[WindowId_PlayerTitles].window;
                if (titles != nullptr) {
                    titles->state |= 0x8000u;
                }
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PlayerAttributes, 1, 0, nullptr));
            }
            consumed = 1;
        }
        return consumed;
    }
    case 12: {
        const std::int32_t hasMail = gLoginWindowRuntime.statusHasMailCC4B5C;
        const bool hover = (state & 1) != 0;
        if (hover) {
            gHoverTextD68AE8 = hasMail != 0 ? L"現在有新郵件。" /* 原常量参考：0x00556D24。 */ : L"現在沒有郵件。" /* 原常量参考：0x00556D14。 */;
            if (hasMail == 0) {
                return 0;
            }
        }
        if (hasMail == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* const hoverWindow = gCurrentWindow;
            consumed = 1;
            static_cast<WindowGraphicData*>(hoverWindow->elements[12].data)->imageNo = 0x3B951;
        }
        if ((state & 2) != 0) {
            world::detail::Sub_518F00(
                &gLoginWindowRuntime.statusMailCategoryD66E34, &gLoginWindowRuntime.statusMailIndexD2E2D8);
            const std::int32_t category = gLoginWindowRuntime.statusMailCategoryD66E34;
            if (category == 0 || category == 1 || category == 2) {
                const std::int32_t mailIndex = gLoginWindowRuntime.statusMailIndexD2E2D8;
                gLoginWindowRuntime.mailCategoryD67B64 = category;
                world::detail::gValueCF4FC0 = mailIndex;
                gLoginWindowRuntime.mailCategoryCC4D50 = category;
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailRead, 1, 0, nullptr));
            }
            consumed = 1;
        }
        return consumed;
    }
    case 11: {
        const std::int32_t target = WindowStatusBar_ReadInjuredPetIndex();
        std::int32_t injury = world::detail::gPlayerRuntimeSlots.valueF4;
        const bool hover = (state & 1) != 0;
        const std::int32_t petIndex = target - 10;
        if (hover) {
            if (injury != 0) {
                const wchar_t* const text = injury <= 25 ? L"現在受了一點小傷。" /* 原常量参考：0x00556E00。 */
                    : injury <= 50 ? L"現在受了輕傷。" /* 原常量参考：0x00556DF0。 */
                    : injury <= 75 ? L"現在受到重傷。" /* 原常量参考：0x00556DE0。 */ : L"現在正面臨死亡邊緣。" /* 原常量参考：0x00556DC8。 */;
                gHoverTextD68AE8 = text;
            } else if (target == -1) {
                gHoverTextD68AE8 = L"現在十分健康。" /* 原常量参考：0x00556D68。 */;
            } else {
                const auto& pet = world::detail::gPetRuntimeRecords[petIndex];
                const std::int32_t petInjury = pet.values70[25];
                const char* name;
                const wchar_t* format;
                if (petInjury <= 25) {
                    format = L"現在{}受到擦傷。" /* 原常量参考：0x00556DB4。 */;
                    name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                } else if (petInjury <= 50) {
                    format = L"現在{}受到輕傷。" /* 原常量参考：0x00556DA0。 */;
                    name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                } else {
                    const char nameFirst = pet.name6BD[0];
                    const bool critical = petInjury > 75;
                    name = nameFirst != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    format = critical ? L"現在{}瀕臨死亡。" /* 原常量参考：0x00556D78。 */ : L"現在{}受了重傷。" /* 原常量参考：0x00556D8C。 */;
                }
                cg::text::FormatTo(buffer, format, cg::text::FromGbkWide(name).c_str());
                gHoverTextD68AE8 = buffer;
                injury = world::detail::gPlayerRuntimeSlots.valueF4;
                static_cast<void>(0);
            }
        }
        if (gLoginWindowRuntime.statusHasInjuryD046C8 == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* imageWindow;
            std::int32_t image;
            if (injury == 0) {
                imageWindow = gCurrentWindow;
                image = 0x3B942;
            } else if (injury <= 25) {
                imageWindow = gCurrentWindow;
                image = 0x3B945;
            } else if (injury <= 50) {
                imageWindow = gCurrentWindow;
                image = 0x3B948;
            } else {
                imageWindow = gCurrentWindow;
                image = injury <= 75 ? 0x3B94B : 0x3B94E;
            }
            static_cast<WindowGraphicData*>(imageWindow->elements[11].data)->imageNo = image;
            consumed = 1;
        }
        if ((state & 2) != 0) {
            if (target != -1) {
                if (target == 0) {
                    WindowInstance* const attributes = gWindowStates[WindowId_PlayerAttributes].window;
                    if (attributes != nullptr) {
                        attributes->state |= 0x8000u;
                    }
                    WindowInstance* const titles = gWindowStates[WindowId_PlayerTitles].window;
                    if (titles != nullptr) {
                        titles->state |= 0x8000u;
                    }
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PlayerProfile, 1, 0, nullptr));
                } else {
                    WindowInstance* const attributes = gWindowStates[WindowId_PetAttributes].window;
                    if (attributes != nullptr) {
                        attributes->state |= 0x8000u;
                    }
                    WindowInstance* const skills = gWindowStates[WindowId_PetSkills].window;
                    if (skills != nullptr) {
                        skills->state |= 0x8000u;
                    }
                    gLoginWindowRuntime.selectedPetCC3E5C = WindowStatusBar_FindSortedPetIndex(petIndex);
                    gLoginWindowRuntime.valueD673D0 = 5;
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetDetail, 1, 0, nullptr));
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
