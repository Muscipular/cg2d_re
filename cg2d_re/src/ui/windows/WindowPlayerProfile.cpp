#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00474BE0。
static std::int32_t WindowPlayerProfile_HandleButtons(std::int32_t index, std::int32_t state); // 窗口7资料切页及前后排请求。

// 原函数：0x00474310。
static void WindowPlayerProfile_Release();

// 原函数：0x00474540。
static std::int32_t WindowPlayerProfile_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic55F780 {0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F780。

static WindowGraphicData gWindowGraphic55F798 {0x3B614, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F798。

static WindowGraphicData gWindowGraphic55F7B0 {0x3BAC5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7B0。

static WindowGraphicData gWindowGraphic55F7C8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7C8。

static WindowGraphicData gWindowGraphic55F7E0 {0x3BAB6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7E0。

static WindowGraphicData gWindowGraphic55F7F8 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7F8。

static WindowGraphicData gWindowGraphic55F810 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F810。

static WindowGraphicData gWindowGraphic55F828 {0x3BAC0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F828。

static WindowGraphicData gWindowGraphic55F840 {0x3BABD, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F840。

static WindowGraphicData gWindowGraphic55F858 {0x3BABB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F858。

static WindowGraphicData gWindowGraphic55F870 {0x3BAC7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F870。

static WindowGraphicData gWindowGraphic55F960 {0x3BACC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F960。

static WindowGraphicData gWindowGraphic55FA50 {0x3BAD1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FA50。

static WindowGraphicData gWindowGraphic55FB40 {0x3BAD6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FB40。

static WindowGraphicData gWindowGraphic55FC30 {0x3BAC6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC30。

static WindowGraphicData gWindowGraphic55FC48 {0x3BACB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC48。

static WindowGraphicData gWindowGraphic55FC60 {0x3BAD0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC60。

static WindowGraphicData gWindowGraphic55FC78 {0x3BAD5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC78。

static WindowGraphicData gWindowGraphic55FC90 {0x3BAB2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC90。

static WindowGraphicData gWindowGraphic55FCA8 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FCA8。

static WindowGraphicData gWindowGraphic55FCC0 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x0055FCC0。

static WindowGraphicData gWindowGraphic55FCD8 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FCD8。

static WindowTextPrototype gWindowText55FCF0 { // 原全局：0x0055FCF0。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名称", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText55FCF8 { // 原全局：0x0055FCF8。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等级", // 原常量参考：0x00586228。
};

static WindowTextPrototype gWindowText55FD00 { // 原全局：0x0055FD00。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"x5", // 原常量参考：0x00586224。
};

static WindowTextPrototype gWindowText55FD08 { // 原全局：0x0055FD08。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"职业", // 原常量参考：0x0058621C。
};

static WindowTextPrototype gWindowText55FD10 { // 原全局：0x0055FD10。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族", // 原常量参考：0x00586214。
};

static WindowTextPrototype gWindowText55FD18 { // 原全局：0x0055FD18。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族称号", // 原常量参考：0x00586208。
};

static WindowTextPrototype gWindowText55FD20 { // 原全局：0x0055FD20。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"特殊称号", // 原常量参考：0x005861FC。
};

static WindowTextPrototype gWindowText55FD28 { // 原全局：0x0055FD28。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"自定称号", // 原常量参考：0x005861F0。
};

static WindowTextPrototype gWindowText55FD30 { // 原全局：0x0055FD30。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"种族", // 原常量参考：0x005861E8。
};

static WindowTextPrototype gWindowText55FD38 { // 原全局：0x0055FD38。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"经验值", // 原常量参考：0x005861E0。
};

static WindowTextPrototype gWindowText55FD40 { // 原全局：0x0055FD40。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"下一级经验值", // 原常量参考：0x005861D0。
};

static WindowTextPrototype gWindowText55FD48 { // 原全局：0x0055FD48。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"生命值", // 原常量参考：0x005861C8。
};

static WindowTextPrototype gWindowText55FD50 { // 原全局：0x0055FD50。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};

static WindowTextPrototype gWindowText55FD58 { // 原全局：0x0055FD58。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"对战胜利值", // 原常量参考：0x005861B4。
};

static WindowElementPrototype gWindowElements55FD60[73] { // 原全局：0x0055FD60。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText55FCF0}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText55FCF8}, &WindowElement_NoOp},
    {WindowElementType_Text, 70, 101, 0, 0, 1, {.text = &gWindowText55FD00}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 31, 0, 0, 1, {.text = &gWindowText55FD08}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 49, 0, 0, 1, {.text = &gWindowText55FD10}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 68, 0, 0, 1, {.text = &gWindowText55FD18}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 86, 0, 0, 1, {.text = &gWindowText55FD20}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 104, 0, 0, 1, {.text = &gWindowText55FD28}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 123, 0, 0, 1, {.text = &gWindowText55FD30}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 141, 0, 0, 1, {.text = &gWindowText55FD38}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 158, 0, 0, 1, {.text = &gWindowText55FD40}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 176, 0, 0, 1, {.text = &gWindowText55FD48}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 194, 0, 0, 1, {.text = &gWindowText55FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 211, 0, 0, 1, {.text = &gWindowText55FD58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic55F780}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 48, 103, 0, 0, 1, {.graphic = &gWindowGraphic55F798}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 60, 104, 0, 0, 1, {.graphic = &gWindowGraphic55F7B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic55F7C8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 151, 230, 0, 0, 1, {.graphic = &gWindowGraphic55F7E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic55F7F8}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic55F810}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic55FCD8}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F858}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 217, 200, 36, 25, 1, {.graphic = &gWindowGraphic55F828}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 253, 200, 36, 25, 1, {.graphic = &gWindowGraphic55F840}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 229, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 237, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 245, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 253, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 261, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 285, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 293, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 301, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 229, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 237, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 245, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 253, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 261, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 285, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 293, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 301, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 229, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 237, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 245, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 253, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 261, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 285, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 293, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 301, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 229, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 237, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 245, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 253, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 261, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 285, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 293, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 301, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 195, 130, 0, 0, 1, {.graphic = &gWindowGraphic55FC30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 195, 147, 0, 0, 1, {.graphic = &gWindowGraphic55FC48}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 195, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FC60}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 195, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FC78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 29, 0, 0, 1, {.graphic = &gWindowGraphic55FC90}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55FCA8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic55FCC0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口7；原全局：0x0054E360。
WindowPrototype gWindowPrototypePlayerProfile {0, 4, 15, 97, 315, 261, 0x80000080u, 73, gWindowElements55FD60, &WindowPlayerProfile_Update, &WindowRuntime_DrawDefault, &WindowPlayerProfile_Release}; // 7: 54E360

// 窗口函数实现。

// 原474FD1..4753A6：每组先清显隐后读属性，图号与显隐各自重取窗口。
static void WindowPlayerProfile_RefreshAttributeBar(
    const std::int32_t attribute, const std::int32_t firstElement,
    const std::int32_t middleImage, const std::int32_t firstImage,
    const std::int32_t lastImage, const std::int32_t singleImage)
{
    for (std::int32_t index = 0; index < 10; ++index) {
        gCurrentWindow->elements[firstElement + index].visible = 0;
    }
    const std::int32_t count = world::detail::gPlayerRuntimeSlots.valuesE4[static_cast<std::int32_t>(attribute)] / 10;
    if (count == 1) {
        auto* const graphic =
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[firstElement].data);
        graphic->imageNo = singleImage;
        gCurrentWindow->elements[firstElement].visible = 1;
    } else {
        for (std::int32_t index = 0; index < count; ++index) {
            auto* const graphic = static_cast<WindowGraphicData*>(
                gCurrentWindow->elements[firstElement + index].data);
            graphic->imageNo = middleImage;
            gCurrentWindow->elements[firstElement + index].visible = 1;
            if (index == 0) {
                auto* const first = static_cast<WindowGraphicData*>(
                    gCurrentWindow->elements[firstElement].data);
                first->imageNo = firstImage;
                gCurrentWindow->elements[firstElement].visible = 1;
            }
            if (index == count - 1) {
                auto* const last = static_cast<WindowGraphicData*>(
                    gCurrentWindow->elements[firstElement + index].data);
                last->imageNo = lastImage;
                gCurrentWindow->elements[firstElement + index].visible = 1;
            }
        }
    }
}

// 原474BE0：五按钮及默认分支；位置请求不写玩家状态，返回仍为悬停值。
// 原函数：0x00474BE0。
static std::int32_t WindowPlayerProfile_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t idleImage;
    std::int32_t hoverImage;
    std::int32_t targetWindow = 0;
    const wchar_t* hint;
    switch (index) {
    case 19:
        idleImage = 0x3B583;
        hoverImage = 0x3B585;
        targetWindow = 8;
        hint = L"切换成玩家明细视窗。";
        break;
    case 20:
        idleImage = 0x3BAB8;
        hoverImage = 0x3BABA;
        targetWindow = 9;
        hint = L"切换成玩家称号视窗。";
        break;
    case 21:
        if (character_select::state::gPukVersion < 8u) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
                0x3C111;
            return 0;
        }
        idleImage = 0x3C111;
        hoverImage = 0x3C112;
        targetWindow = 58;
        hint = L"切换成玩家简介视窗";
        break;
    case 23:
        idleImage = 0x3BAC0;
        hoverImage = 0x3BAC1;
        hint = L"把战斗时的位置变更为前排。";
        break;
    case 24:
        idleImage = 0x3BABD;
        hoverImage = 0x3BABE;
        hint = L"把战斗时的位置变更为后排。";
        break;
    default:
        return 0;
    }

    std::int32_t consumed = 0;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        graphic->imageNo = hoverImage;
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        graphic->imageNo = idleImage;
    }
    if ((state & 2) != 0) {
        if (targetWindow != 0) {
            if (gWindowStates[targetWindow].window == nullptr) {
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
        } else if (world::detail::gPlayerRuntimeSlots.value1A0 == (index == 23 ? 0 : 1)) {
            static_cast<void>(network::Sub_512570(platform::gConnectionSocket));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    return consumed;
}

// 原474FB0：窗口7完整刷新，复制阶段的elements/data快照差异见window_7.md。
// 原函数：0x00474FB0。
static void WindowPlayerProfile_Refresh()
{
    WindowElement* const portraitElements = gCurrentWindow->elements;
    const std::int32_t portrait = world::detail::gPlayerRuntimeSlots.value17C;
    static_cast<WindowGraphicData*>(portraitElements[14].data)->imageNo = portrait;
    WindowPlayerProfile_RefreshAttributeBar(0, 25, 0x3BAC7, 0x3BACA, 0x3BAC8, 0x3BAC9);
    WindowPlayerProfile_RefreshAttributeBar(1, 35, 0x3BACC, 0x3BACF, 0x3BACD, 0x3BACE);
    WindowPlayerProfile_RefreshAttributeBar(2, 45, 0x3BAD1, 0x3BAD4, 0x3BAD2, 0x3BAD3);
    WindowPlayerProfile_RefreshAttributeBar(3, 55, 0x3BAD6, 0x3BAD9, 0x3BAD7, 0x3BAD8);

    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
    const char* const name = world::detail::GetPlayerDisplayText(1);
    nameData->text = cg::text::FromGbkWide(name);

    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", world::detail::gPlayerRuntimeSlots.level);
    auto* const levelData = static_cast<WindowTextData*>(gCurrentWindow->elements[1].data);
    levelData->text = buffer;

    WindowElement* const jobElements = gCurrentWindow->elements;
    const char* const job = world::detail::GetPlayerJobName();
    auto* const jobData = static_cast<WindowTextData*>(jobElements[3].data);
    jobData->text = cg::text::FromGbkWide(job);

    WindowInstance* const guildWindow = gCurrentWindow;
    const std::int32_t guildIndex = cg::world::detail::ReadGuildTextIndex10514B0();
    auto* const guildData = static_cast<WindowTextData*>(guildWindow->elements[4].data);
    if (guildIndex != -1) {
        const char* const guild = cg::world::detail::GetGuildName();
        guildData->text = cg::text::FromGbkWide(guild);
        const std::int32_t rankIndex = cg::world::detail::ReadGuildTextIndex10514B0();
        auto* const rankData = static_cast<WindowTextData*>(gCurrentWindow->elements[5].data);
        const char* const rank = cg::world::detail::GetGuildRankText(rankIndex);
        rankData->text = cg::text::FromGbkWide(rank);
    } else {
        const char* const guild = "";
        guildData->text = cg::text::FromGbkWide(guild);
        auto* const rankData = static_cast<WindowTextData*>(gCurrentWindow->elements[5].data);
        const char* const rank = "";
        rankData->text = cg::text::FromGbkWide(rank);
    }

    const std::int32_t titleIndex = world::detail::gPlayerRuntimeSlots.titleIndex100;
    auto* const titleData = static_cast<WindowTextData*>(gCurrentWindow->elements[6].data);
    const char* const title = world::detail::GetTitleText(titleIndex);
    titleData->text = cg::text::FromGbkWide(title);

    WindowElement* const ownTitleElements = gCurrentWindow->elements;
    const char* const ownTitle = world::detail::GetPlayerDisplayText(2);
    auto* const ownTitleData = static_cast<WindowTextData*>(ownTitleElements[7].data);
    ownTitleData->text = cg::text::FromGbkWide(ownTitle);

    WindowInstance* const tribeWindow = gCurrentWindow;
    const std::int16_t tribeIndex = world::detail::ReadPlayerTribe();
    WindowElement* const tribeElements = tribeWindow->elements;
    const wchar_t* const tribe = gTribeNames57CB54[tribeIndex];
    auto* const tribeData = static_cast<WindowTextData*>(tribeElements[8].data);
    tribeData->text = tribe;

    cg::text::FormatTo(buffer, L"{:9}", world::detail::gPlayerRuntimeSlots.value58.value());
    auto* const experienceData = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
    experienceData->text = buffer;

    const std::int32_t maxHp = world::detail::gPlayerRuntimeSlots.maxHp.value();
    const std::int32_t hp = world::detail::gPlayerRuntimeSlots.hp.value();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    auto* const hpData = static_cast<WindowTextData*>(gCurrentWindow->elements[11].data);
    hpData->text = buffer;

    const std::int32_t maxFp = world::detail::gPlayerRuntimeSlots.maxFp.value();
    const std::int32_t fp = world::detail::gPlayerRuntimeSlots.fp.value();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    auto* const fpData = static_cast<WindowTextData*>(gCurrentWindow->elements[12].data);
    fpData->text = buffer;

    std::wstring_view nextExperience;
    if ((world::detail::gPlayerRuntimeSlots.value68 == -1)) {
        nextExperience = L"        -";
    } else {
        cg::text::FormatTo(buffer, L"{:9}", world::detail::gPlayerRuntimeSlots.value68.value());
        nextExperience = buffer;
    }
    auto* const nextData = static_cast<WindowTextData*>(gCurrentWindow->elements[10].data);
    nextData->text = nextExperience;

    cg::text::FormatTo(buffer, L"{:9}", world::detail::gPlayerRuntimeSlots.value104);
    auto* const duelData = static_cast<WindowTextData*>(gCurrentWindow->elements[13].data);
    duelData->text = buffer;

    if (world::detail::gPlayerRuntimeSlots.value1A0 == 1) {
        gCurrentWindow->elements[22].x = 227;
    } else {
        gCurrentWindow->elements[22].x = 263;
    }
    gCurrentWindow->elements[22].y = 203;

    const std::int32_t injury = world::detail::gPlayerRuntimeSlots.valueF4;
    std::int32_t injuryImage;
    if (injury == 0) {
        injuryImage = 0x3B614;
    } else if (injury <= 25) {
        injuryImage = 0x3B615;
    } else if (injury <= 50) {
        injuryImage = 0x3B616;
    } else if (injury <= 75) {
        injuryImage = 0x3B617;
    } else {
        injuryImage = 0x3B618;
    }
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[15].data)->imageNo = injuryImage;

    const std::int32_t penalty = world::detail::gPlayerRuntimeSlots.valueF8;
    if (penalty > 0) {
        cg::text::FormatTo(buffer, L"x{}", penalty);
        WindowElement* const penaltyElements = gCurrentWindow->elements;
        auto* const penaltyData = static_cast<WindowTextData*>(penaltyElements[2].data);
        penaltyData->text = buffer;
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[16].visible = 1;
    } else {
        gCurrentWindow->elements[2].visible = 0;
        gCurrentWindow->elements[16].visible = 0;
    }
}

// 原函数：0x00474540。
static std::int32_t WindowPlayerProfile_Update(const std::int32_t)
{
    WindowPlayerProfile_Refresh();
    return 1;
}

// 原函数：0x00474310。
static void WindowPlayerProfile_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_PlayerProfile].window;
    if (window != nullptr) {
        gWindowStates[WindowId_PlayerAttributes].x = window->x;
        gWindowStates[WindowId_PlayerAttributes].y = window->y;
        gWindowStates[WindowId_PlayerTitles].x = window->x;
        gWindowStates[WindowId_PlayerTitles].y = window->y;
        gWindowStates[WindowId_OwnProfile].x = window->x;
        gWindowStates[WindowId_OwnProfile].y = window->y;
    }
}

} // namespace detail

} // namespace cg::ui
