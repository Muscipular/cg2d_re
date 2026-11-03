#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowProfileReplyElementId : ElementId {
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
    Element12 = 12,
    ButtonClose = 13,
    Element14 = 14,
    HandleButtons = 15,
    HandleButtons_16 = 16,
    HandleButtons_17 = 17,
    Element18 = 18,
    Element19 = 19,
    Element20 = 20,
    HandleSideArea = 21,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004CB9E0。
static std::int32_t WindowProfileReply_Update(WindowInstance& callbackWindow, std::int32_t unusedValue); // 窗口64每帧完整刷新，返回1。

// 原函数：0x004CB9F0。
static std::uint32_t WindowProfileReply_Refresh(WindowInstance* callbackWindow); // 图号转换、八文本及256B四行提取，返回末行含NUL长度。

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
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText57A028}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element1)},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText57A030}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element2)},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText57A038}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element3)},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText57A040}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element4)},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText57A048}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element5)},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText57A058}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element6)},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText57A060}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element7)},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element8)},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element9)},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element10)},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element11)},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element12)},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic579F20}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowProfileReplyElementId::ButtonClose)},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic579F20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element14)},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowProfileReplyElementId::HandleButtons)},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowProfileReplyElementId::HandleButtons_16)},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons, -1, static_cast<ElementId>(WindowProfileReplyElementId::HandleButtons_17)},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic579F98}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element18)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic579FB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element19)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic579FC8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowProfileReplyElementId::Element20)},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowProfileReplyElementId::HandleSideArea)},
};

// 原型：窗口64；原全局：0x0054EE50。
WindowPrototype gWindowPrototypeProfileReply {0, 4, 15, 97, 317, 250, 0x80000080u, 21, gWindowElements57A070, &WindowProfileReply_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 64: 54EE50

// 窗口函数实现。

// 原函数：0x004CB9F0。
static std::uint32_t WindowProfileReply_Refresh(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    const world::detail::ProfileRuntimeRecord& profile =
        world::detail::gReceivedProfile1074110;
    const std::int32_t image = character_select::detail::ResolveCompatibleCharacterGraphic_51BC50(profile.value10C);
    if (auto* const element14 = currentWindow->FindElementById(WindowProfileReplyElementId::Element14); element14) element14->data.graphic->imageNo = image;
    for (std::int32_t index = 0; index < 3; ++index) {
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index + 14));
        if (indexedElement) {
            indexedElement->data.graphic->imageNo =
                profile.entries410[index].value04;
        }
    }
    if (auto* const element1 = currentWindow->FindElementById(WindowProfileReplyElementId::Element1); element1) element1->data.text->text = profile.text08;
    std::wstring number;
    cg::text::FormatTo(number, L"{}", profile.value108);
    if (auto* const element2 = currentWindow->FindElementById(WindowProfileReplyElementId::Element2); element2) element2->data.text->text = number;
    if (auto* const element3 = currentWindow->FindElementById(WindowProfileReplyElementId::Element3); element3) element3->data.text->text = profile.text110;
    if (auto* const element4 = currentWindow->FindElementById(WindowProfileReplyElementId::Element4); element4) element4->data.text->text = profile.text210;
    if (auto* const element5 = currentWindow->FindElementById(WindowProfileReplyElementId::Element5); element5) element5->data.text->text = profile.text310;
    for (std::int32_t index = 0; index < 3; ++index) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index + 5)); indexedElement) indexedElement->data.text->text = profile.entries410[index].text08;
    }
    std::wstring line;
    for (std::int32_t index = 0; index < 3; ++index) {
        static_cast<void>(input::ExtractTextLine(line, profile.text728.data(), index, 28));
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index + 8)); indexedElement) indexedElement->data.text->text = line;
    }
    static_cast<void>(input::ExtractTextLine(line, profile.text728.data(), 3, 28));
    auto* const element12 = currentWindow->FindElementById(WindowProfileReplyElementId::Element12);
    if (element12) {
        std::wstring& destination = element12->data.text->text;
        const auto units = static_cast<std::uint32_t>(line.size() + 1);
        destination = line;
        return units;
    }
    return 0;
}

// 原函数：0x004CB9E0。
static std::int32_t WindowProfileReply_Update(WindowInstance& callbackWindow, const std::int32_t)
{
    static_cast<void>(WindowProfileReply_Refresh(&callbackWindow));
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
