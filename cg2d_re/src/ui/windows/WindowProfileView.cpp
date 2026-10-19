#include "../WindowRuntimeInternal.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/MailState.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
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
    {WindowElementType_Graphic, 257, 32, 40, 30, 1, {.graphic = &gWindowGraphic5796B0}, &WindowProfileView_ButtonMail},
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText5796C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText5796D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText5796D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText5796E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText5796E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText5796F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText579700}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic5795A8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic5795A8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic579620}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic579638}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic579650}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 307, 0, 30, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
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
    const world::detail::ProfileRuntimeRecord& profile =
        world::detail::gViewedProfile105ECE0;
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[14].data)->imageNo =
        profile.value10C;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index + 15].data)->imageNo =
            profile.entries410[index].value04;
    }
    static_cast<WindowTextData*>(gCurrentWindow->elements[1].data)->text = cg::text::FromGbkWide(profile.text08.data());
    std::wstring number;
    cg::text::FormatTo(number, L"{:3}", profile.value108);
    static_cast<WindowTextData*>(gCurrentWindow->elements[2].data)->text = number;
    static_cast<WindowTextData*>(gCurrentWindow->elements[3].data)->text = cg::text::FromGbkWide(profile.text110.data());
    static_cast<WindowTextData*>(gCurrentWindow->elements[4].data)->text = cg::text::FromGbkWide(profile.text210.data());
    static_cast<WindowTextData*>(gCurrentWindow->elements[5].data)->text = cg::text::FromGbkWide(profile.text310.data());
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<WindowTextData*>(gCurrentWindow->elements[index + 6].data)->text = cg::text::FromGbkWide(profile.entries410[index].text08.data());
    }
    std::wstring line;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<void>(input::ExtractGbkTextLine(line, profile.text728.data(), index, 28));
        static_cast<WindowTextData*>(gCurrentWindow->elements[index + 9].data)->text = line;
    }
    static_cast<void>(input::ExtractGbkTextLine(line, profile.text728.data(), 3, 28));
    std::wstring& destination = static_cast<WindowTextData*>(gCurrentWindow->elements[12].data)->text;
    const auto units = static_cast<std::uint32_t>(line.size() + 1);
    destination = line;
    return units;
}

// 原函数：0x004CAF60。
// 关联原入口：0x004CAF88（共享尾已并入本函数正文）。
static std::int32_t WindowProfileView_Update(const std::int32_t)
{
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
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
    if (index != 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[0].data)->imageNo = 244253;
        std::wstring hint;
        cg::text::FormatTo(hint, kMiniMailCostHint588588, world::detail::ReadCategoryValue105FA20());
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[0].data)->imageNo = 244251;
    }
    if ((state & 2) != 0) {
        const std::int32_t money = world::gPlayerRuntimeSlots.valueFC;
        if (money >= world::detail::ReadCategoryValue105FA20()) {
            const auto& profile = world::detail::gViewedProfile105ECE0;
            gMiniMailCompose105F508.target = profile.value00;
            std::strcpy(gMiniMailCompose105F508.name.data(), profile.text08.data());
            std::strcpy(gMiniMailCompose105F508.text104.data(), "");
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
