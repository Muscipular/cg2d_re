#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004CB9E0。
static std::int32_t WindowProfileReply_Update(std::int32_t unusedValue); // 窗口64每帧完整刷新，返回1。

// 原函数：0x004CB9F0。
static std::uint32_t WindowProfileReply_Refresh(); // 图号转换、八文本及256B四行提取，返回末行含NUL长度。

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic579F20 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579F20。

static WindowGraphicData gWindowGraphic579F98 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579F98。

static WindowGraphicData gWindowGraphic579FB0 {0x3C10D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579FB0。

static WindowGraphicData gWindowGraphic579FC8 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00579FC8。

static WindowGraphicData gWindowGraphic57A010 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A010。

static WindowTextPrototype gWindowText57A028 { // 原全局：0x0057A028。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"姓名", // 原常量参考：0x00585D4C。
};

static WindowTextPrototype gWindowText57A030 { // 原全局：0x0057A030。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等级", // 原常量参考：0x00585D44。
};

static WindowTextPrototype gWindowText57A038 { // 原全局：0x0057A038。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"称号", // 原常量参考：0x00585D38。
};

static WindowTextPrototype gWindowText57A040 { // 原全局：0x0057A040。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"职业", // 原常量参考：0x00585D28。
};

static WindowTextPrototype gWindowText57A048 { // 原全局：0x0057A048。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族", // 原常量参考：0x00585D20。
};

static WindowTextPrototype gWindowText57A058 { // 原全局：0x0057A058。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"出售", // 原常量参考：0x00585D10。
};

static WindowTextPrototype gWindowText57A060 { // 原全局：0x0057A060。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"购买", // 原常量参考：0x00585D08。
};

static WindowTextPrototype gWindowText57A068 { // 原全局：0x0057A068。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"自我介绍", // 原常量参考：0x00585D00。
};

static WindowElementPrototype gWindowElements57A070[21] { // 原全局：0x0057A070。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText57A028}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText57A030}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText57A038}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText57A040}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText57A048}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText57A058}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText57A060}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic579F20}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic579F20}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic579F98}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic579FB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic579FC8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口64；原全局：0x0054EE50。
WindowPrototype gWindowPrototypeProfileReply {0, 4, 15, 97, 317, 250, 0x80000080u, 21, gWindowElements57A070, &WindowProfileReply_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 64: 54EE50

// 窗口函数实现。

// 原函数：0x004CB9F0。
static std::uint32_t WindowProfileReply_Refresh()
{
    const world::detail::ProfileRuntimeRecord& profile =
        world::detail::gReceivedProfile1074110;
    const std::int32_t image = character_select::detail::ResolveCompatibleCharacterGraphic_51BC50(profile.value10C);
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[13].data)->imageNo = image;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index + 14].data)->imageNo =
            profile.entries410[index].value04;
    }
    static_cast<WindowTextData*>(gCurrentWindow->elements[0].data)->text = profile.text08;
    std::wstring number;
    cg::text::FormatTo(number, L"{}", profile.value108);
    static_cast<WindowTextData*>(gCurrentWindow->elements[1].data)->text = number;
    static_cast<WindowTextData*>(gCurrentWindow->elements[2].data)->text = profile.text110;
    static_cast<WindowTextData*>(gCurrentWindow->elements[3].data)->text = profile.text210;
    static_cast<WindowTextData*>(gCurrentWindow->elements[4].data)->text = profile.text310;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<WindowTextData*>(gCurrentWindow->elements[index + 5].data)->text = profile.entries410[index].text08;
    }
    std::wstring line;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<void>(input::ExtractTextLine(line, profile.text728.data(), index, 28));
        static_cast<WindowTextData*>(gCurrentWindow->elements[index + 8].data)->text = line;
    }
    static_cast<void>(input::ExtractTextLine(line, profile.text728.data(), 3, 28));
    std::wstring& destination = static_cast<WindowTextData*>(gCurrentWindow->elements[11].data)->text;
    const auto units = static_cast<std::uint32_t>(line.size() + 1);
    destination = line;
    return units;
}

// 原函数：0x004CB9E0。
static std::int32_t WindowProfileReply_Update(const std::int32_t)
{
    static_cast<void>(WindowProfileReply_Refresh());
    return 1;
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x005342E0。
WindowInstance* WindowProfileReply_Open()
{
    return WindowRuntime_Open(WindowId_ProfileReply);
}

} // namespace cg::ui
