#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

#include <cstring>
#include <cstdio>
#include <ctime>

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowMiniMailElementId : ElementId {
    ButtonClose = 1,
    HandleInput = 2,
    Send = 3,
    Send_4 = 4,
    Send_5 = 5,
    Send_6 = 6,
    Send_7 = 7,
    Send_8 = 8,
    Send_9 = 9,
    Send_10 = 10,
    Send_11 = 11,
    Send_12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
};


namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextCA0180 {}; // 原全局：0x00CA0180。


// 回调与内部函数前置声明。

// 原函数：0x004CB550。
static std::int32_t WindowMiniMail_Update(std::int32_t value); // 窗口63首次按目标决定保留输入，再每帧刷新。

// 原函数：0x004CB5D0。
static void WindowMiniMail_Refresh(); // 同源收件人与八位费用刷新，原混合返回无人消费。

// 原函数：0x004CB6A0。
static std::int32_t WindowMiniMail_Send(std::int32_t index, std::int32_t state); // 双层转义发送、插入本地迷你邮件及完整关闭尾。

// 原函数：0x004CB920。
static std::int32_t WindowMiniMail_HandleInput(std::int32_t unusedIndex, std::int32_t state); // 固定元素1输入焦点与非活动绘制。

// 控件载荷、窗口状态、组件表与原型。

static WindowInputPrototype gMiniMailInputPrototype580CB0 { // 原全局：0x00580CB0。
    {{&cg::ui::detail::gInputContextCA0180}, WindowInputReferenceKind::Context},
    22, 70, 3, 12, 0, {}, 10, 28, 12, 0,
};

static WindowGraphicData gWindowGraphic5799D0 {0x3BA0B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005799D0。

static WindowGraphicData gWindowGraphic5799E8 {0x3BA06, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005799E8。

static WindowGraphicData gWindowGraphic579A00 {0x3BA07, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00579A00。

static WindowGraphicData gWindowGraphic579A78 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579A78。

static WindowGraphicData gWindowGraphic579AA8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579AA8。

static WindowGraphicData gWindowGraphic579B08 {0x3C123, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579B08。

static WindowTextPrototype gWindowText579B20 { // 原全局：0x00579B20。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"收件人", // 原常量参考：0x00585CF4。
};

static WindowElementPrototype gWindowElements579B28[16] { // 原全局：0x00579B28。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic579AA8}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowMiniMailElementId::ButtonClose)},
    {WindowElementType_Input, 22, 70, 170, 120, 1, {.unusedValue = 0}, &WindowMiniMail_HandleInput, -1, static_cast<ElementId>(WindowMiniMailElementId::HandleInput)},
    {WindowElementType_Graphic, 154, 28, 50, 18, 1, {.graphic = &gWindowGraphic579A78}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send)},
    {WindowElementType_Graphic, 80, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_4)},
    {WindowElementType_Graphic, 72, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_5)},
    {WindowElementType_Graphic, 64, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_6)},
    {WindowElementType_Graphic, 56, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_7)},
    {WindowElementType_Graphic, 48, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_8)},
    {WindowElementType_Graphic, 40, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_9)},
    {WindowElementType_Graphic, 32, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_10)},
    {WindowElementType_Graphic, 24, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_11)},
    {WindowElementType_Graphic, 20, 28, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send, -1, static_cast<ElementId>(WindowMiniMailElementId::Send_12)},
    {WindowElementType_Text, 21, 48, 0, 0, 1, {.text = &gWindowText579B20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMiniMailElementId::Element13)},
    {WindowElementType_Graphic, 16, 46, 0, 0, 1, {.graphic = &gWindowGraphic5799D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMiniMailElementId::Element14)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5799E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMiniMailElementId::Element15)},
    {WindowElementType_Graphic, 14, 23, 0, 0, 1, {.graphic = &gWindowGraphic579A00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMiniMailElementId::Element16)},
};

// 原型：窗口63；原全局：0x0054EE20。
WindowPrototype gWindowPrototypeMiniMail {0, 4, 383, 81, 232, 210, 0x80000080u, 16, gWindowElements579B28, &WindowMiniMail_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 63: 54EE20

// 窗口函数实现。

// 原函数：0x004CB5D0。
static void WindowMiniMail_Refresh()
{
    if ((gCurrentWindow && (*gCurrentWindow)[WindowMiniMailElementId::Element13])) (*gCurrentWindow)[WindowMiniMailElementId::Element13]->data.text->text = gMiniMailCompose105F508.name;
    WindowInstance* const window = gCurrentWindow;
    static_cast<void>(WindowInput_ActivateElement(&gMiniMailInputPrototype580CB0, window->x, window->y, 2));
    std::int32_t divisor = 1;
    for (std::int32_t index = 0; index < 8; ++index) {
        const std::int32_t cost = world::detail::ReadCategoryValue105FA20();
        if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index + 3)]))) { continue; }
        (*gCurrentWindow)[NativeElementKey(index + 3)]->data.graphic->imageNo =
            cost < divisor ? 0 : cost / divisor % 10 + 245520;
        divisor *= 10;
    }
    if (world::detail::ReadCategoryValue105FA20() == 0) {
        if ((gCurrentWindow && (*gCurrentWindow)[WindowMiniMailElementId::Send_4])) (*gCurrentWindow)[WindowMiniMailElementId::Send_4]->data.graphic->imageNo = 245520;
    }
}

// 原函数：0x004CB550。
static std::int32_t WindowMiniMail_Update(const std::int32_t value)
{
    if (value == -1) {
        const std::int32_t mode =
            gMiniMailCompose105F508.target == world::detail::gMiniMailSelectionD673E4 ? 1 : 0;
        WindowInstance* const window = gCurrentWindow;
        static_cast<void>(WindowInput_ActivateElement(&gMiniMailInputPrototype580CB0, window->x, window->y, mode));
        input::SetContext_44B330(&cg::ui::detail::gInputContextCA0180);
        if (!((gCurrentWindow && (*gCurrentWindow)[WindowMiniMailElementId::HandleInput]))) { return {}; }
        gFocusedElementDataD45F94 = (*gCurrentWindow)[WindowMiniMailElementId::HandleInput]->data.input;
        if (!((gCurrentWindow && (*gCurrentWindow)[WindowMiniMailElementId::HandleInput]))) { return {}; }
        (*gCurrentWindow)[WindowMiniMailElementId::HandleInput]->data.input->context =
            &cg::ui::detail::gInputContextCA0180;
        const std::int32_t target = gMiniMailCompose105F508.target;
        gMiniMailInputPrototype580CB0.text.clear();
        world::detail::gMiniMailSelectionD673E4 = target;
    }
    WindowMiniMail_Refresh();
    return 1;
}

// 原函数：0x004CB920。
static std::int32_t WindowMiniMail_HandleInput(const std::int32_t, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        WindowInstance* const window = gCurrentWindow;
        if (!((window && (*window)[WindowMiniMailElementId::HandleInput]))) { return {}; }
        if (gFocusedElementDataD45F94 != (*window)[WindowMiniMailElementId::HandleInput]->data.input) {
            static_cast<void>(WindowInput_ActivateElement(
                &gMiniMailInputPrototype580CB0, window->x, window->y, 1));
            input::SetContext_44B330(&cg::ui::detail::gInputContextCA0180);
            if (!((gCurrentWindow && (*gCurrentWindow)[WindowMiniMailElementId::HandleInput]))) { return {}; }
            gFocusedElementDataD45F94 = (*gCurrentWindow)[WindowMiniMailElementId::HandleInput]->data.input;
            if (!((gCurrentWindow && (*gCurrentWindow)[WindowMiniMailElementId::HandleInput]))) { return {}; }
            (*gCurrentWindow)[WindowMiniMailElementId::HandleInput]->data.input->context =
                &cg::ui::detail::gInputContextCA0180;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (input::gCurrentInputData != &cg::ui::detail::gInputContextCA0180) {
        render::DrawInputText_4568A0(&cg::ui::detail::gInputContextCA0180);
    }
    return consumed;
}

// 原函数：0x004CB6A0。
static std::int32_t WindowMiniMail_Send(const std::int32_t index, const std::int32_t state)
{
    if (index != 2) {
        return 0;
    }
    if (!((gCurrentWindow && (*gCurrentWindow)[WindowMiniMailElementId::Send]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[WindowMiniMailElementId::Send]->data.graphic;
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = 244253;
        std::wstring hint;
        cg::text::FormatTo(hint, kMiniMailCostHint588588, world::detail::ReadCategoryValue105FA20());
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        graphic->imageNo = 244251;
    }
    if ((state & 2) == 0) {
        return consumed;
    }
    if (cg::ui::detail::gInputContextCA0180.empty()) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return consumed;
    }
    const std::wstring text = input::ContextText(cg::ui::detail::gInputContextCA0180);
    std::wstring storedText; // 内部正文；写文件前仍执行原300字节容量预检。
    {
        char fileBytes[300]{};
        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), text);
        storedText = text;
    }
    const std::wstring escapedName = input::EscapeText(gMiniMailCompose105F508.name, 256);
    const std::wstring escapedText = input::EscapeText(text, 1024);
    const std::wstring combined = std::to_wstring(gMiniMailCompose105F508.target) + L"|" + escapedName + L"|" + escapedText + L"|0";
    const std::wstring packet = input::EscapeTextAndNotify(combined, 1023);
    const std::int32_t target = world::gDialogTargetD2DE6C;
    const std::int32_t request = world::gDialogRequestCC21E0;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(network::SendDialogResponse_512620(
        platform::gConnectionSocket, x, y, request, target, 8, packet));
    const std::int32_t contact = world::detail::FindMiniMailContactByKey_534370(gMiniMailCompose105F508.target);
    if (contact != -1) {
        __time64_t time;
        time = ::_time64(nullptr);
        const std::tm* const calendar = ::_localtime64(&time);
        const std::int32_t minute = calendar->tm_min;
        const std::int32_t hour = calendar->tm_hour;
        const std::int32_t day = calendar->tm_mday;
        const std::int32_t month = calendar->tm_mon;
        const std::int32_t year = calendar->tm_year;
        std::wstring timestamp;
        cg::text::FormatTo(timestamp, L"{:4}/{:2}/{:2} {:02}:{:02}",
            year + 1900, month + 1, day, hour, minute);
        world::detail::PrependMiniMailMessage_5343C0(contact, 1, timestamp, storedText);
    }
    static_cast<void>(WindowProfileView_RequestCategoryClose(3));
    static_cast<void>(WindowProfileView_RequestCategoryClose(4));
    static_cast<void>(audio::PlaySe_52C450(78, 320));
    cg::ui::detail::gInputContextCA0180.clearText();
    world::detail::gMiniMailSelectionD673E4 = -1;
    return 1;
}

} // namespace detail

} // namespace cg::ui
