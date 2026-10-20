#include "../WindowRuntimeInternal.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/MailState.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowProfileViewElementId : ElementId {
    ButtonMail = 1,
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
    Element12 = 12,
    Element13 = 13,
    ButtonClose = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    Element20 = 20,
    Element21 = 21,
    HandleSideArea = 22,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004CAF60。
// 关联原入口：0x004CAF88（共享尾已并入本函数正文）。
static std::int32_t WindowProfileView_Update(std::int32_t unusedValue); // 窗口62更新，标关闭后仍刷新全部资料。

// 原函数：0x004CAFA0。
static std::uint32_t WindowProfileView_Refresh(); // 四图号、八资料文本和四分行，返回末行含NUL字节数。

// 原函数：0x004CB530。
static void WindowProfileView_Release(); // 先清对话类型，再标记关联窗口63。

// 原函数：0x004CB2F0。
static std::int32_t WindowProfileView_ButtonMail(std::int32_t index, std::int32_t state); // 查看资料发信，先金额门禁再打开63。

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic5795A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005795A8。

static WindowGraphicData gWindowGraphic579620 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579620。

static WindowGraphicData gWindowGraphic579638 {0x3C10D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579638。

static WindowGraphicData gWindowGraphic579650 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00579650。

static WindowGraphicData gWindowGraphic579698 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579698。

static WindowGraphicData gWindowGraphic5796B0 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005796B0。

static WindowTextPrototype gWindowText5796C8 { // 原全局：0x005796C8。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"姓名", // 原常量参考：0x00585D4C。
};

static WindowTextPrototype gWindowText5796D0 { // 原全局：0x005796D0。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等级", // 原常量参考：0x00585D44。
};

static WindowTextPrototype gWindowText5796D8 { // 原全局：0x005796D8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"称号", // 原常量参考：0x00585D38。
};

static WindowTextPrototype gWindowText5796E0 { // 原全局：0x005796E0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"职业", // 原常量参考：0x00585D28。
};

static WindowTextPrototype gWindowText5796E8 { // 原全局：0x005796E8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族", // 原常量参考：0x00585D20。
};

static WindowTextPrototype gWindowText5796F8 { // 原全局：0x005796F8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"出售", // 原常量参考：0x00585D10。
};

static WindowTextPrototype gWindowText579700 { // 原全局：0x00579700。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"购买", // 原常量参考：0x00585D08。
};

static WindowTextPrototype gWindowText579708 { // 原全局：0x00579708。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"自我介绍", // 原常量参考：0x00585D00。
};

static WindowElementPrototype gWindowElements579710[22] { // 原全局：0x00579710。
    {WindowElementType_Graphic, 257, 32, 40, 30, 1, {.graphic = &gWindowGraphic5796B0}, &WindowProfileView_ButtonMail, -1, static_cast<ElementId>(WindowProfileViewElementId::ButtonMail)},
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText5796C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element2)},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText5796D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element3)},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText5796D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element4)},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText5796E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element5)},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText5796E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element6)},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText5796F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element7)},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText579700}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element8)},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element9)},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element10)},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element11)},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element12)},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element13)},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic5795A8}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowProfileViewElementId::ButtonClose)},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic5795A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element15)},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element16)},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element17)},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element18)},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic579620}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element19)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic579638}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element20)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic579650}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileViewElementId::Element21)},
    {WindowElementType_EventArea, 307, 0, 30, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowProfileViewElementId::HandleSideArea)},
};

// 原型：窗口62；原全局：0x0054EDF0。
WindowPrototype gWindowPrototypeProfileView {0, 4, 5, 20, 317, 250, 0x80000080u, 22, gWindowElements579710, &WindowProfileView_Update, &WindowRuntime_DrawDefault, &WindowProfileView_Release}; // 62: 54EDF0

// 窗口函数实现。

// 原函数：0x004CB430。
std::int32_t WindowProfileView_ReadSelection(const std::int32_t index)
{
    if (index - 79 < 32) {
        return world::detail::ReadCategoryRecord105FA24(0, index - 78).selected10C != 0 ? 1 : 0;
    }
    if (index - 79 < 64) {
        return world::detail::ReadCategoryRecord105FA24(1, index - 110).selected10C != 0 ? 1 : 0;
    }
    return world::detail::ReadCategoryRecord105FA24(2, index - 142).selected10C != 0 ? 1 : 0;
}

// 原函数：0x004CAFA0。
static std::uint32_t WindowProfileView_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    const world::detail::ProfileRuntimeRecord& profile =
        world::detail::gViewedProfile105ECE0;
    auto* const element15 = currentWindow->FindElementById(WindowProfileViewElementId::Element15);
    if (element15) {
        element15->data.graphic->imageNo =
            profile.value10C;
    }
    for (std::int32_t index = 0; index < 3; ++index) {
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index + 15));
        if (indexedElement) {
            indexedElement->data.graphic->imageNo =
                profile.entries410[index].value04;
        }
    }
    if (auto* const element2 = currentWindow->FindElementById(WindowProfileViewElementId::Element2); element2) element2->data.text->text = profile.text08;
    std::wstring number;
    cg::text::FormatTo(number, L"{:3}", profile.value108);
    if (auto* const element3 = currentWindow->FindElementById(WindowProfileViewElementId::Element3); element3) element3->data.text->text = number;
    if (auto* const element4 = currentWindow->FindElementById(WindowProfileViewElementId::Element4); element4) element4->data.text->text = profile.text110;
    if (auto* const element5 = currentWindow->FindElementById(WindowProfileViewElementId::Element5); element5) element5->data.text->text = profile.text210;
    if (auto* const element6 = currentWindow->FindElementById(WindowProfileViewElementId::Element6); element6) element6->data.text->text = profile.text310;
    for (std::int32_t index = 0; index < 3; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index + 6)); indexedElement) indexedElement->data.text->text = profile.entries410[index].text08;
    }
    std::wstring line;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<void>(input::ExtractTextLine(line, profile.text728.data(), index, 28));
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index + 9)); indexedElement) indexedElement->data.text->text = line;
    }
    static_cast<void>(input::ExtractTextLine(line, profile.text728.data(), 3, 28));
    auto* const element13 = currentWindow->FindElementById(WindowProfileViewElementId::Element13);
    if (element13) {
        std::wstring& destination = element13->data.text->text;
        const auto units = static_cast<std::uint32_t>(line.size() + 1);
        destination = line;
        return units;
    }
    return {};
}

// 原函数：0x004CAF60。
// 关联原入口：0x004CAF88（共享尾已并入本函数正文）。
static std::int32_t WindowProfileView_Update(const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        currentWindow->state |= 0x8000u;
    }
    static_cast<void>(WindowProfileView_Refresh());
    return 1;
}

// 原函数：0x004CB9B0。
WindowInstance* WindowProfileView_RequestCategoryClose(const std::int32_t index)
{
    WindowInstance* const window = gWindowStates[59 + index].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

// 原函数：0x004CB530。
static void WindowProfileView_Release()
{
    world::gCurrentDialogType_581800 = -1;
    static_cast<void>(WindowProfileView_RequestCategoryClose(4));
}

// 原函数：0x004CB2F0。
static std::int32_t WindowProfileView_ButtonMail(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    if (index != 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        if (auto* const buttonMailElement = currentWindow->FindElementById(WindowProfileViewElementId::ButtonMail); buttonMailElement) buttonMailElement->data.graphic->imageNo = 244253;
        std::wstring hint;
        cg::text::FormatTo(hint, kMiniMailCostHint588588, world::detail::ReadCategoryValue105FA20());
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        if (auto* const buttonMailElement = currentWindow->FindElementById(WindowProfileViewElementId::ButtonMail); buttonMailElement) buttonMailElement->data.graphic->imageNo = 244251;
    }
    if ((state & 2) != 0) {
        const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
        if (money >= world::detail::ReadCategoryValue105FA20()) {
            const auto& profile = world::detail::gViewedProfile105ECE0;
            gMiniMailCompose105F508.target = profile.value00;
            gMiniMailCompose105F508.name = profile.text08;
            gMiniMailCompose105F508.text104.clear();
            static_cast<void>(WindowRuntime_Open(WindowId_MiniMail));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        return 1;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
