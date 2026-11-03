#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPlayerAttributesElementId : ElementId {
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
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    Element20 = 20,
    Element21 = 21,
    Element22 = 22,
    Element23 = 23,
    Element24 = 24,
    Element25 = 25,
    Element26 = 26,
    Element27 = 27,
    Element28 = 28,
    Element29 = 29,
    HandleButtons = 30,
    HandleButtons_31 = 31,
    HandleButtons_32 = 32,
    HandleButtons_33 = 33,
    HandleButtons_34 = 34,
    ButtonClose = 35,
    HandleButtons_36 = 36,
    Element37 = 37,
    HandleButtons_38 = 38,
    HandleButtons_39 = 39,
    Element40 = 40,
    Element41 = 41,
    Element42 = 42,
    HandleSideArea = 43,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00475E00。
static std::int32_t WindowPlayerAttributes_HandleButtons(ElementId elementId, std::int32_t state); // 窗口8入口六值快照、加点及切页。

// 原函数：0x00474360。
static void WindowPlayerAttributes_Release(WindowInstance&);

// 原函数：0x00475DF0。
static std::int32_t WindowPlayerAttributes_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic560680 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560680。

static WindowGraphicData gWindowGraphic560698 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560698。

static WindowGraphicData gWindowGraphic5606B0 {0x3B584, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606B0。

static WindowGraphicData gWindowGraphic5606C8 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606C8。

static WindowGraphicData gWindowGraphic5606E0 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606E0。

static WindowGraphicData gWindowGraphic5606F8 {0x3BAB4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606F8。

static WindowGraphicData gWindowGraphic560710 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560710。

static WindowGraphicData gWindowGraphic560728 {0x3BAB1, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00560728。

static WindowGraphicData gWindowGraphic560740 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560740。

static WindowTextPrototype gWindowText560758 { // 原全局：0x00560758。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名称", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText560760 { // 原全局：0x00560760。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等级", // 原常量参考：0x00586228。
};

static WindowTextPrototype gWindowText560768 { // 原全局：0x00560768。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"新增点数", // 原常量参考：0x005861A8。
};

static WindowTextPrototype gWindowText560770 { // 原全局：0x00560770。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"体力", // 原常量参考：0x005861A0。
};

static WindowTextPrototype gWindowText560778 { // 原全局：0x00560778。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"力量", // 原常量参考：0x00586198。
};

static WindowTextPrototype gWindowText560780 { // 原全局：0x00560780。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"强度", // 原常量参考：0x00586190。
};

static WindowTextPrototype gWindowText560788 { // 原全局：0x00560788。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"速度", // 原常量参考：0x00586188。
};

static WindowTextPrototype gWindowText560790 { // 原全局：0x00560790。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔法", // 原常量参考：0x00586180。
};

static WindowTextPrototype gWindowText560798 { // 原全局：0x00560798。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"攻击", // 原常量参考：0x00586178。
};

static WindowTextPrototype gWindowText5607A0 { // 原全局：0x005607A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"防御", // 原常量参考：0x00586170。
};

static WindowTextPrototype gWindowText5607A8 { // 原全局：0x005607A8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"敏捷", // 原常量参考：0x00586168。
};

static WindowTextPrototype gWindowText5607B0 { // 原全局：0x005607B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"精神", // 原常量参考：0x00586160。
};

static WindowTextPrototype gWindowText5607B8 { // 原全局：0x005607B8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"回复", // 原常量参考：0x00586158。
};

static WindowTextPrototype gWindowText5607C0 { // 原全局：0x005607C0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魅力", // 原常量参考：0x00586150。
};

static WindowTextPrototype gWindowText5607C8 { // 原全局：0x005607C8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗毒", // 原常量参考：0x00586148。
};

static WindowTextPrototype gWindowText5607D0 { // 原全局：0x005607D0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗昏睡", // 原常量参考：0x00586140。
};

static WindowTextPrototype gWindowText5607D8 { // 原全局：0x005607D8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗石化", // 原常量参考：0x00586138。
};

static WindowTextPrototype gWindowText5607E0 { // 原全局：0x005607E0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗醉", // 原常量参考：0x00586130。
};

static WindowTextPrototype gWindowText5607E8 { // 原全局：0x005607E8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗混乱", // 原常量参考：0x00586128。
};

static WindowTextPrototype gWindowText5607F0 { // 原全局：0x005607F0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗遗忘", // 原常量参考：0x00586120。
};

static WindowTextPrototype gWindowText5607F8 { // 原全局：0x005607F8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"必杀率", // 原常量参考：0x00586118。
};

static WindowTextPrototype gWindowText560800 { // 原全局：0x00560800。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"反击率", // 原常量参考：0x00586110。
};

static WindowTextPrototype gWindowText560808 { // 原全局：0x00560808。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"命中率", // 原常量参考：0x00586108。
};

static WindowTextPrototype gWindowText560810 { // 原全局：0x00560810。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"闪躲率", // 原常量参考：0x00586100。
};

static WindowTextPrototype gWindowText560818 { // 原全局：0x00560818。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"耐力", // 原常量参考：0x005860F8。
};

static WindowTextPrototype gWindowText560820 { // 原全局：0x00560820。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"灵巧", // 原常量参考：0x005860F0。
};

static WindowTextPrototype gWindowText560828 { // 原全局：0x00560828。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"智力", // 原常量参考：0x005860E8。
};

static WindowTextPrototype gWindowText560830 { // 原全局：0x00560830。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔攻", // 原常量参考：0x005860E0。
};

static WindowTextPrototype gWindowText560838 { // 原全局：0x00560838。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔防", // 原常量参考：0x005860D8。
};

static WindowElementPrototype gWindowElements560840[43] { // 原全局：0x00560840。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText560758}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element1)},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText560760}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element2)},
    {WindowElementType_Text, 130, 31, 0, 0, 1, {.text = &gWindowText560768}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element3)},
    {WindowElementType_Text, 124, 48, 0, 0, 1, {.text = &gWindowText560770}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element4)},
    {WindowElementType_Text, 124, 64, 0, 0, 1, {.text = &gWindowText560778}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element5)},
    {WindowElementType_Text, 124, 80, 0, 0, 1, {.text = &gWindowText560780}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element6)},
    {WindowElementType_Text, 124, 96, 0, 0, 1, {.text = &gWindowText560788}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element7)},
    {WindowElementType_Text, 124, 112, 0, 0, 1, {.text = &gWindowText560790}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element8)},
    {WindowElementType_Text, 46, 159, 0, 0, 1, {.text = &gWindowText560798}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element9)},
    {WindowElementType_Text, 46, 175, 0, 0, 1, {.text = &gWindowText5607A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element10)},
    {WindowElementType_Text, 46, 191, 0, 0, 1, {.text = &gWindowText5607A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element11)},
    {WindowElementType_Text, 46, 207, 0, 0, 1, {.text = &gWindowText560830}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element12)},
    {WindowElementType_Text, 124, 159, 0, 0, 1, {.text = &gWindowText5607B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element13)},
    {WindowElementType_Text, 124, 175, 0, 0, 1, {.text = &gWindowText5607B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element14)},
    {WindowElementType_Text, 124, 191, 0, 0, 1, {.text = &gWindowText5607C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element15)},
    {WindowElementType_Text, 124, 207, 0, 0, 1, {.text = &gWindowText560838}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element16)},
    {WindowElementType_Text, 212, 48, 0, 0, 1, {.text = &gWindowText5607C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element17)},
    {WindowElementType_Text, 212, 64, 0, 0, 1, {.text = &gWindowText5607D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element18)},
    {WindowElementType_Text, 212, 80, 0, 0, 1, {.text = &gWindowText5607D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element19)},
    {WindowElementType_Text, 284, 48, 0, 0, 1, {.text = &gWindowText5607E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element20)},
    {WindowElementType_Text, 284, 64, 0, 0, 1, {.text = &gWindowText5607E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element21)},
    {WindowElementType_Text, 284, 80, 0, 0, 1, {.text = &gWindowText5607F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element22)},
    {WindowElementType_Text, 212, 137, 0, 0, 1, {.text = &gWindowText5607F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element23)},
    {WindowElementType_Text, 212, 153, 0, 0, 1, {.text = &gWindowText560800}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element24)},
    {WindowElementType_Text, 284, 137, 0, 0, 1, {.text = &gWindowText560808}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element25)},
    {WindowElementType_Text, 284, 153, 0, 0, 1, {.text = &gWindowText560810}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element26)},
    {WindowElementType_Text, 212, 191, 0, 0, 1, {.text = &gWindowText560818}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element27)},
    {WindowElementType_Text, 284, 191, 0, 0, 1, {.text = &gWindowText560820}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element28)},
    {WindowElementType_Text, 284, 207, 0, 0, 1, {.text = &gWindowText560828}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element29)},
    {WindowElementType_Graphic, 94, 48, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons)},
    {WindowElementType_Graphic, 94, 64, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons_31)},
    {WindowElementType_Graphic, 94, 80, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons_32)},
    {WindowElementType_Graphic, 94, 96, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons_33)},
    {WindowElementType_Graphic, 94, 112, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons_34)},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic560680}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::ButtonClose)},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic560698}, &WindowPlayerAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons_36)},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic5606B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element37)},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic5606C8}, &WindowPlayerAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons_38)},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic560740}, &WindowPlayerAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons_39)},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic5606F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element40)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic560710}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element41)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic560728}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::Element42)},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPlayerAttributesElementId::HandleSideArea)},
};

// 原型：窗口8；原全局：0x0054E390。
WindowPrototype gWindowPrototypePlayerAttributes {0, 4, 15, 97, 315, 261, 0x80000080u, 43, gWindowElements560840, &WindowPlayerAttributes_Update, &WindowRuntime_DrawDefault, &WindowPlayerAttributes_Release}; // 8: 54E390

// 窗口函数实现。

// 原475E00：先按原顺序采样六值，连默认分支也计算门槛；区别于4767E0重读。
// 原函数：0x00475E00。
static std::int32_t WindowPlayerAttributes_HandleButtons(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::array<std::int32_t, 6> values;
    values[1] = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(1)];
    values[2] = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(2)];
    values[3] = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(3)];
    values[4] = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(4)];
    values[5] = world::gPlayerUnspentAttributePoints_D2F788;
    values[0] = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(0)];
    std::int32_t total = 0;
    for (const std::int32_t value : values) {
        total += value;
    }
    const std::int32_t limit = total / 2;

    if (elementId >= static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons) && elementId <= static_cast<ElementId>(WindowPlayerAttributesElementId::HandleButtons_34)) {
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            std::int32_t consumed = 0;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = 0x3B53D;
                gHoverTextD68AE8 = L"分配点数至目前的能力数值。";
                consumed = 1;
            } else {
                graphic->imageNo = 0x3B53B;
            }
            if ((state & MouseFlag_LDownEvent) != 0) {
                if (values[index - 29] < limit) {
                    static_cast<void>(cg::network::GetGameConnection().IncreasePlayerAttributeRequest(index - 29));
                    static_cast<void>(audio::PlaySe_52C450(78, 320));
                }
                return 1;
            }
            return consumed;
        }
    }

    std::int32_t idleImage;
    std::int32_t hoverImage;
    std::int32_t targetWindow;
    const wchar_t* hint;
    switch (static_cast<WindowPlayerAttributesElementId>(elementId)) {
    case WindowPlayerAttributesElementId::HandleButtons_36:
        idleImage = 0x3BAB5;
        hoverImage = 0x3BAB7;
        targetWindow = 7;
        hint = L"切换成玩家状态视窗。";
        break;
    case WindowPlayerAttributesElementId::HandleButtons_38:
        idleImage = 0x3BAB8;
        hoverImage = 0x3BABA;
        targetWindow = 9;
        hint = L"切换成玩家称号视窗。";
        break;
    case WindowPlayerAttributesElementId::HandleButtons_39:
        if (network::GetGameConnection().PukVersion() < 8u) {
            auto* const indexedElement = currentWindow->FindElementById(elementId);
            if (indexedElement) {
                indexedElement->data.graphic->imageNo =
                    0x3C111;
            }
            return 0;
        }
        idleImage = 0x3C111;
        hoverImage = 0x3C112;
        targetWindow = 58;
        hint = L"切换成玩家简介视窗";
        break;
    default:
        return 0;
    }
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = hoverImage;
            gHoverTextD68AE8 = hint;
            consumed = 1;
        } else {
            graphic->imageNo = idleImage;
        }
        if ((state & MouseFlag_LDownEvent) != 0 && WindowRuntime_States()[targetWindow].window == nullptr) {
            currentWindow->state |= 0x8000u;
            static_cast<void>(WindowRuntime_Open(targetWindow));
            WindowInstance* const xSource = currentWindow;
            WindowInstance* const xTarget = WindowRuntime_States()[targetWindow].window;
            xTarget->x = xSource->x;
            WindowInstance* const ySource = currentWindow;
            WindowInstance* const yTarget = WindowRuntime_States()[targetWindow].window;
            yTarget->y = ySource->y;
            static_cast<void>(audio::PlaySe_52C450(56, 320));
            WindowRuntime_QueuePagePresentation(*currentWindow, *yTarget);
            consumed = 1;
        }
        return consumed;
    }
    return 0;
}

// 原4767E0：逐项格式化后重取元素；点数门槛阶段独立读取，见window_8.md。
// 原函数：0x004767E0。
static void WindowPlayerAttributes_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    auto* const element1 = currentWindow->FindElementById(WindowPlayerAttributesElementId::Element1);
    if (element1) {
        auto* const nameData = element1->data.text;
        const wchar_t* const name = world::GetPlayerDisplayText(1);
        nameData->text = name;
    }

    std::wstring buffer;
    const auto writeNumber = [&buffer, currentWindow](
        const std::int32_t index, const std::int32_t value, const std::wstring_view format) {
        cg::text::FormatTo(buffer, format, value);
        auto* const textElement = currentWindow->FindElementById(NativeElementKey(index));
        if (textElement) {
            auto* const text = textElement->data.text;
            text->text = buffer;
        }
        return;
    };
    writeNumber(1, world::gPlayerRuntimeSlots.level, L"{:3}");
    writeNumber(2, world::gPlayerUnspentAttributePoints_D2F788, L"{:5}");
    for (std::int32_t index = 0; index < 5; ++index) {
        writeNumber(index + 3, world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(index)], L"{:5}");
    }
    for (std::int32_t index = 8; index < 11; ++index) {
        writeNumber(index, world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(index)], L"{:5}");
    }
    writeNumber(11, world::gPlayerRuntimeSlots.valuesB0[static_cast<std::int32_t>(0)], L"{:5}");
    for (std::int32_t index = 11; index < 14; ++index) {
        writeNumber(index + 1, world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(index)], L"{:5}");
    }
    writeNumber(15, world::gPlayerRuntimeSlots.valuesB0[static_cast<std::int32_t>(1)], L"{:5}");
    for (std::int32_t index = 0; index < 6; ++index) {
        writeNumber(index + 16, world::gPlayerRuntimeSlots.valuesBC[static_cast<std::int32_t>(index)], L"{:4}");
    }
    for (std::int32_t index = 0; index < 4; ++index) {
        writeNumber(index + 22, world::gPlayerRuntimeSlots.valuesD4[static_cast<std::int32_t>(index)], L"{:4}");
    }
    for (std::int32_t index = 5; index < 8; ++index) {
        writeNumber(index + 21, world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(index)], L"{:4}");
    }

    const std::int32_t remainingPoints = world::gPlayerUnspentAttributePoints_D2F788;
    if (remainingPoints > 0) {
        const std::int32_t quick = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(3)];
        std::int32_t total = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(4)];
        const std::int32_t toughness = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(2)];
        total += quick;
        const std::int32_t strength = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(1)];
        total += toughness;
        total += strength;
        const std::int32_t vitality = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(0)];
        total += vitality;
        total += remainingPoints;
        const std::int32_t limit = total / 2;
        if (auto* const handleButtonsElement = currentWindow->FindElementById(WindowPlayerAttributesElementId::HandleButtons); handleButtonsElement) handleButtonsElement->visible = vitality < limit ? 1 : 0;
        for (std::int32_t index = 1; index < 5; ++index) {
            const std::int32_t value = world::gPlayerRuntimeSlots.values78[static_cast<std::int32_t>(index)];
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(29 + index)); indexedElement) indexedElement->visible = value < limit ? 1 : 0;
        }
    } else {
        for (std::int32_t index = 29; index < 34; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
    }
}

// 原函数：0x00475DF0。
static std::int32_t WindowPlayerAttributes_Update(const std::int32_t)
{
    WindowPlayerAttributes_Refresh();
    return 1;
}

// 原函数：0x00474360。
static void WindowPlayerAttributes_Release(WindowInstance& releasedWindow)
{
    WindowInstance* const window = WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerAttributes].window;
    if (window != nullptr) {
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerProfile].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerProfile].y = window->y;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerTitles].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PlayerTitles].y = window->y;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_OwnProfile].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_OwnProfile].y = window->y;
    }
}

} // namespace detail

} // namespace cg::ui
