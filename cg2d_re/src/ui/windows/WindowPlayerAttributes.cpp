#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原475E00：先按原顺序采样六值，连默认分支也计算门槛；区别于4767E0重读。
// 原函数：0x00475E00。
std::int32_t WindowPlayerAttributes_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::array<std::int32_t, 6> values;
    values[1] = world::detail::ReadPlayerValue78(1);
    values[2] = world::detail::ReadPlayerValue78(2);
    values[3] = world::detail::ReadPlayerValue78(3);
    values[4] = world::detail::ReadPlayerValue78(4);
    values[5] = world::detail::gValueD2F788;
    values[0] = world::detail::ReadPlayerValue78(0);
    std::int32_t total = 0;
    for (const std::int32_t value : values) {
        total += value;
    }
    const std::int32_t limit = total / 2;

    if (index >= 29 && index <= 33) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B53D;
            gHoverTextD68AE8 = L"分配點數至目前的能力數值。";
            consumed = 1;
        } else {
            graphic->imageNo = 0x3B53B;
        }
        if ((state & 2) != 0) {
            if (values[index - 29] < limit) {
                static_cast<void>(network::Sub_5123B0(platform::gConnectionSocket, index - 29));
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            }
            return 1;
        }
        return consumed;
    }

    std::int32_t idleImage;
    std::int32_t hoverImage;
    std::int32_t targetWindow;
    const wchar_t* hint;
    switch (index) {
    case 35:
        idleImage = 0x3BAB5;
        hoverImage = 0x3BAB7;
        targetWindow = 7;
        hint = L"切換成玩家狀態視窗。";
        break;
    case 37:
        idleImage = 0x3BAB8;
        hoverImage = 0x3BABA;
        targetWindow = 9;
        hint = L"切換成玩家稱號視窗。";
        break;
    case 38:
        if (character_select::state::gPukVersion < 8u) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
                0x3C111;
            return 0;
        }
        idleImage = 0x3C111;
        hoverImage = 0x3C112;
        targetWindow = 58;
        hint = L"切換成玩家簡介視窗";
        break;
    default:
        return 0;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = hoverImage;
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        graphic->imageNo = idleImage;
    }
    if ((state & 2) != 0 && gWindowStates[targetWindow].window == nullptr) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(WindowRuntime_Open(targetWindow));
        WindowInstance* const xSource = gCurrentWindow;
        WindowInstance* const xTarget = gWindowStates[targetWindow].window;
        xTarget->x = xSource->x;
        WindowInstance* const ySource = gCurrentWindow;
        WindowInstance* const yTarget = gWindowStates[targetWindow].window;
        yTarget->y = ySource->y;
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        consumed = 1;
    }
    return consumed;
}

// 原4767E0：逐项格式化后重取元素；点数门槛阶段独立读取，见window_8.md。
// 原函数：0x004767E0。
void WindowPlayerAttributes_Refresh()
{
    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
    const char* const name = world::detail::BorrowPlayerDisplayText(1);
    const std::size_t nameLength = std::strlen(name) + 1;
    nameData->text = cg::text::FromGbkWide(name);

    std::wstring buffer;
    const auto writeNumber = [&buffer](
        const std::int32_t index, const std::int32_t value, const std::wstring_view format) {
        cg::text::FormatTo(buffer, format, value);
        auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
        const std::size_t length = buffer.size() + 1;
        text->text = buffer;
    };
    writeNumber(1, world::detail::ReadPlayerLevel(), L"{:3}");
    writeNumber(2, world::detail::gValueD2F788, L"{:5}");
    for (std::int32_t index = 0; index < 5; ++index) {
        writeNumber(index + 3, world::detail::ReadPlayerValue78(index), L"{:5}");
    }
    for (std::int32_t index = 8; index < 11; ++index) {
        writeNumber(index, world::detail::ReadPlayerValue78(index), L"{:5}");
    }
    writeNumber(11, world::detail::ReadPlayerValueB0(0), L"{:5}");
    for (std::int32_t index = 11; index < 14; ++index) {
        writeNumber(index + 1, world::detail::ReadPlayerValue78(index), L"{:5}");
    }
    writeNumber(15, world::detail::ReadPlayerValueB0(1), L"{:5}");
    for (std::int32_t index = 0; index < 6; ++index) {
        writeNumber(index + 16, world::detail::ReadPlayerValueBC(index), L"{:4}");
    }
    for (std::int32_t index = 0; index < 4; ++index) {
        writeNumber(index + 22, world::detail::ReadPlayerValueD4(index), L"{:4}");
    }
    for (std::int32_t index = 5; index < 8; ++index) {
        writeNumber(index + 21, world::detail::ReadPlayerValue78(index), L"{:4}");
    }

    const std::int32_t remainingPoints = world::detail::gValueD2F788;
    if (remainingPoints > 0) {
        const std::int32_t quick = world::detail::ReadPlayerValue78(3);
        std::int32_t total = world::detail::ReadPlayerValue78(4);
        const std::int32_t toughness = world::detail::ReadPlayerValue78(2);
        total += quick;
        const std::int32_t strength = world::detail::ReadPlayerValue78(1);
        total += toughness;
        total += strength;
        const std::int32_t vitality = world::detail::ReadPlayerValue78(0);
        total += vitality;
        total += remainingPoints;
        const std::int32_t limit = total / 2;
        gCurrentWindow->elements[29].visible = vitality < limit ? 1 : 0;
        for (std::int32_t index = 1; index < 5; ++index) {
            const std::int32_t value = world::detail::ReadPlayerValue78(index);
            gCurrentWindow->elements[29 + index].visible = value < limit ? 1 : 0;
        }
    } else {
        for (std::int32_t index = 29; index < 34; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    }
}

// 原函数：0x00475DF0。
std::int32_t WindowPlayerAttributes_Update(const std::int32_t)
{
    WindowPlayerAttributes_Refresh();
    return 1;
}

// 原函数：0x00474360。
void WindowPlayerAttributes_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_PlayerAttributes].window;
    if (window != nullptr) {
        gWindowStates[WindowId_PlayerProfile].x = window->x;
        gWindowStates[WindowId_PlayerProfile].y = window->y;
        gWindowStates[WindowId_PlayerTitles].x = window->x;
        gWindowStates[WindowId_PlayerTitles].y = window->y;
        gWindowStates[WindowId_OwnProfile].x = window->x;
        gWindowStates[WindowId_OwnProfile].y = window->y;
    }
}

} // namespace detail

} // namespace cg::ui

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic560680 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560680。

WindowGraphicData gWindowGraphic560698 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560698。

WindowGraphicData gWindowGraphic5606B0 {0x3B584, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606B0。

WindowGraphicData gWindowGraphic5606C8 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606C8。

WindowGraphicData gWindowGraphic5606E0 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606E0。

WindowGraphicData gWindowGraphic5606F8 {0x3BAB4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606F8。

WindowGraphicData gWindowGraphic560710 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560710。

WindowGraphicData gWindowGraphic560728 {0x3BAB1, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00560728。

WindowGraphicData gWindowGraphic560740 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560740。

WindowTextPrototype gWindowText560758 { // 原全局：0x00560758。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};

WindowTextPrototype gWindowText560760 { // 原全局：0x00560760。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};

WindowTextPrototype gWindowText560768 { // 原全局：0x00560768。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"新增點數", // 原常量参考：0x005861A8。
};

WindowTextPrototype gWindowText560770 { // 原全局：0x00560770。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"體力", // 原常量参考：0x005861A0。
};

WindowTextPrototype gWindowText560778 { // 原全局：0x00560778。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"力量", // 原常量参考：0x00586198。
};

WindowTextPrototype gWindowText560780 { // 原全局：0x00560780。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"強度", // 原常量参考：0x00586190。
};

WindowTextPrototype gWindowText560788 { // 原全局：0x00560788。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"速度", // 原常量参考：0x00586188。
};

WindowTextPrototype gWindowText560790 { // 原全局：0x00560790。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔法", // 原常量参考：0x00586180。
};

WindowTextPrototype gWindowText560798 { // 原全局：0x00560798。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"攻擊", // 原常量参考：0x00586178。
};

WindowTextPrototype gWindowText5607A0 { // 原全局：0x005607A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"防禦", // 原常量参考：0x00586170。
};

WindowTextPrototype gWindowText5607A8 { // 原全局：0x005607A8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"敏捷", // 原常量参考：0x00586168。
};

WindowTextPrototype gWindowText5607B0 { // 原全局：0x005607B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"精神", // 原常量参考：0x00586160。
};

WindowTextPrototype gWindowText5607B8 { // 原全局：0x005607B8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"回復", // 原常量参考：0x00586158。
};

WindowTextPrototype gWindowText5607C0 { // 原全局：0x005607C0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魅力", // 原常量参考：0x00586150。
};

WindowTextPrototype gWindowText5607C8 { // 原全局：0x005607C8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗毒", // 原常量参考：0x00586148。
};

WindowTextPrototype gWindowText5607D0 { // 原全局：0x005607D0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗昏睡", // 原常量参考：0x00586140。
};

WindowTextPrototype gWindowText5607D8 { // 原全局：0x005607D8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗石化", // 原常量参考：0x00586138。
};

WindowTextPrototype gWindowText5607E0 { // 原全局：0x005607E0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗醉", // 原常量参考：0x00586130。
};

WindowTextPrototype gWindowText5607E8 { // 原全局：0x005607E8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗混亂", // 原常量参考：0x00586128。
};

WindowTextPrototype gWindowText5607F0 { // 原全局：0x005607F0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗遺忘", // 原常量参考：0x00586120。
};

WindowTextPrototype gWindowText5607F8 { // 原全局：0x005607F8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"必殺率", // 原常量参考：0x00586118。
};

WindowTextPrototype gWindowText560800 { // 原全局：0x00560800。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"反擊率", // 原常量参考：0x00586110。
};

WindowTextPrototype gWindowText560808 { // 原全局：0x00560808。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"命中率", // 原常量参考：0x00586108。
};

WindowTextPrototype gWindowText560810 { // 原全局：0x00560810。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"閃躲率", // 原常量参考：0x00586100。
};

WindowTextPrototype gWindowText560818 { // 原全局：0x00560818。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"耐力", // 原常量参考：0x005860F8。
};

WindowTextPrototype gWindowText560820 { // 原全局：0x00560820。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"靈巧", // 原常量参考：0x005860F0。
};

WindowTextPrototype gWindowText560828 { // 原全局：0x00560828。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"智力", // 原常量参考：0x005860E8。
};

WindowTextPrototype gWindowText560830 { // 原全局：0x00560830。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔攻", // 原常量参考：0x005860E0。
};

WindowTextPrototype gWindowText560838 { // 原全局：0x00560838。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔防", // 原常量参考：0x005860D8。
};

} // namespace

// 原型：窗口8；原全局：0x0054E390。
WindowPrototype gWindowPrototypePlayerAttributes {0, 4, 15, 97, 315, 261, 0x80000080u, 43, gWindowElements560840, &WindowPlayerAttributes_Update, &WindowRuntime_DrawDefault, &WindowPlayerAttributes_Release}; // 8: 54E390

WindowElementPrototype gWindowElements560840[43] { // 原全局：0x00560840。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText560758}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText560760}, &WindowElement_NoOp},
    {WindowElementType_Text, 130, 31, 0, 0, 1, {.text = &gWindowText560768}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 48, 0, 0, 1, {.text = &gWindowText560770}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 64, 0, 0, 1, {.text = &gWindowText560778}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 80, 0, 0, 1, {.text = &gWindowText560780}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 96, 0, 0, 1, {.text = &gWindowText560788}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 112, 0, 0, 1, {.text = &gWindowText560790}, &WindowElement_NoOp},
    {WindowElementType_Text, 46, 159, 0, 0, 1, {.text = &gWindowText560798}, &WindowElement_NoOp},
    {WindowElementType_Text, 46, 175, 0, 0, 1, {.text = &gWindowText5607A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 46, 191, 0, 0, 1, {.text = &gWindowText5607A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 46, 207, 0, 0, 1, {.text = &gWindowText560830}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 159, 0, 0, 1, {.text = &gWindowText5607B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 175, 0, 0, 1, {.text = &gWindowText5607B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 191, 0, 0, 1, {.text = &gWindowText5607C0}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 207, 0, 0, 1, {.text = &gWindowText560838}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 48, 0, 0, 1, {.text = &gWindowText5607C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 64, 0, 0, 1, {.text = &gWindowText5607D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 80, 0, 0, 1, {.text = &gWindowText5607D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 48, 0, 0, 1, {.text = &gWindowText5607E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 64, 0, 0, 1, {.text = &gWindowText5607E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 80, 0, 0, 1, {.text = &gWindowText5607F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 137, 0, 0, 1, {.text = &gWindowText5607F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 153, 0, 0, 1, {.text = &gWindowText560800}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 137, 0, 0, 1, {.text = &gWindowText560808}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 153, 0, 0, 1, {.text = &gWindowText560810}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 191, 0, 0, 1, {.text = &gWindowText560818}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 191, 0, 0, 1, {.text = &gWindowText560820}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 207, 0, 0, 1, {.text = &gWindowText560828}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 94, 48, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 64, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 80, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 96, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 112, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic560680}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic560698}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic5606B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic5606C8}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic560740}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic5606F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic560710}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic560728}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

} // namespace cg::ui::detail
