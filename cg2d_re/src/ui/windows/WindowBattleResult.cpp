#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00499880。
static std::int32_t WindowRuntime_ButtonCloseResult(std::int32_t index, std::int32_t state); // 结果窗关闭按钮，固定入口图形载荷。

// 原函数：0x00499750。
static std::int32_t WindowBattleResult_ButtonClose(std::int32_t index, std::int32_t state); // 窗口30点击关闭，索引未使用。

// 原函数：0x004991C0。
static std::int32_t WindowBattleResult_Update(std::int32_t value);

// 原函数：0x004996C0。
static std::int32_t WindowBattleResult_Draw(std::int32_t value);

// 原函数：0x00499790。
static std::int32_t WindowDpResult_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static std::uint32_t gBattleResultCloseTickCB0F78 = 0; // 原全局：0x00CB0F78。 原窗口30/31共用到期tick，不按窗口复制。

static WindowGraphicData gWindowGraphic56B918 {0x3BA7E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B918。

static WindowGraphicData gWindowGraphic56B930 {0x3BA7F, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056B930。

static WindowGraphicData gWindowGraphic56B960 {0x3B614, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B960。

static WindowGraphicData gWindowGraphic56B978 {0x3BA80, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B978。

static WindowTextPrototype gWindowText56B990 { // 原全局：0x0056B990。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？", // 原常量参考：0x00585FA0。
};

static WindowTextPrototype gWindowText56B998 { // 原全局：0x0056B998。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"+000000", // 原常量参考：0x00585F98。
};

static WindowTextPrototype gWindowText56B9A0 {{}, L"？？？？？？？？"}; // 原全局：0x0056B9A0。 // 原常量参考：0x00585FA0。

static WindowTextPrototype gWindowText56B9A8 { // 原全局：0x0056B9A8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？", // 原常量参考：0x00585FA0。
};

static WindowTextPrototype gWindowText56B9B0 { // 原全局：0x0056B9B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"+000", // 原常量参考：0x00585F90。
};

static WindowGraphicData gWindowGraphic56C2A0 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C2A0。

static WindowTextPrototype gWindowText56C2B8 { // 原全局：0x0056C2B8。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"結果", // 原常量参考：0x00585F88。
};

static WindowTextPrototype gWindowText56C2C0 { // 原全局：0x0056C2C0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"?????  +000000", // 原常量参考：0x00585F78。
};

static WindowElementPrototype gWindowElements56B9B8[66] { // 原全局：0x0056B9B8。
    {WindowElementType_EventArea, 0, 27, 490, 274, 1, {.unusedValue = 0}, &WindowBattleResult_ButtonClose},
    {WindowElementType_Graphic, 268, 40, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 268, 59, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 268, 78, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 268, 97, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 72, 47, 0, 0, 1, {.graphic = &gWindowGraphic56B960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 72, 66, 0, 0, 1, {.graphic = &gWindowGraphic56B960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 72, 85, 0, 0, 1, {.graphic = &gWindowGraphic56B960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 72, 104, 0, 0, 1, {.graphic = &gWindowGraphic56B960}, &WindowElement_NoOp},
    {WindowElementType_Text, 86, 46, 0, 0, 1, {.text = &gWindowText56B990}, &WindowElement_NoOp},
    {WindowElementType_Text, 86, 65, 0, 0, 1, {.text = &gWindowText56B990}, &WindowElement_NoOp},
    {WindowElementType_Text, 86, 84, 0, 0, 1, {.text = &gWindowText56B990}, &WindowElement_NoOp},
    {WindowElementType_Text, 86, 103, 0, 0, 1, {.text = &gWindowText56B990}, &WindowElement_NoOp},
    {WindowElementType_Text, 226, 46, 0, 0, 1, {.text = &gWindowText56B998}, &WindowElement_NoOp},
    {WindowElementType_Text, 226, 65, 0, 0, 1, {.text = &gWindowText56B998}, &WindowElement_NoOp},
    {WindowElementType_Text, 226, 84, 0, 0, 1, {.text = &gWindowText56B998}, &WindowElement_NoOp},
    {WindowElementType_Text, 226, 103, 0, 0, 1, {.text = &gWindowText56B998}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 44, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 60, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 76, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 92, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 108, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 124, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 140, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 156, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 172, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 188, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 204, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 220, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 236, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 252, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 50, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 66, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 82, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 98, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 114, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 130, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 146, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 162, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 178, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 194, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 210, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 226, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 242, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 258, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 50, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 66, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 82, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 98, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 114, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 130, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 146, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 162, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 178, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 194, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 210, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 226, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 242, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 258, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 72, 171, 0, 0, 1, {.text = &gWindowText56B9A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 72, 220, 0, 0, 1, {.text = &gWindowText56B9A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 72, 269, 0, 0, 1, {.text = &gWindowText56B9A0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B918}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 27, 0, 0, 1, {.graphic = &gWindowGraphic56B930}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 9, 0, 481, 27, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
    {WindowElementType_EventArea, 493, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

static WindowElementPrototype gWindowElements56C2C8[4] { // 原全局：0x0056C2C8。
    {WindowElementType_Graphic, 42, 106, 66, 17, 1, {.graphic = &gWindowGraphic56C2A0}, &WindowRuntime_ButtonCloseResult},
    {WindowElementType_Text, 20, 10, 0, 0, 1, {.text = &gWindowText56C2B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 40, 0, 0, 1, {.text = &gWindowText56C2C0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 65, 0, 0, 1, {.text = &gWindowText56C2C0}, &WindowElement_NoOp},
};

// 原型：窗口30；原全局：0x0054E780。
WindowPrototype gWindowPrototypeBattleResult {1, 3, 40, 40, 490, 301, 0x80010101u, 66, gWindowElements56B9B8, &WindowBattleResult_Update, &WindowBattleResult_Draw, &WindowRuntime_ReleaseDefault}; // 30: 54E780

// 原型：窗口31；原全局：0x0054E7B0。
WindowPrototype gWindowPrototypeDpResult {3, 3, 245, 170, 150, 140, 0x80010101u, 4, gWindowElements56C2C8, &WindowDpResult_Update, &WindowRuntime_DrawLargeFrame, &WindowRuntime_ReleaseDefault}; // 31: 54E7B0

// 窗口函数实现。

// 原函数：0x00499880。
static std::int32_t WindowRuntime_ButtonCloseResult(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    const std::int32_t consumed = (state & 1) != 0 ? 1 : 0;
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    graphic->imageNo = 0x3B568;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B56A;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B569;
    }
    return consumed;
}

// 原函数：0x004991C0。
static std::int32_t WindowBattleResult_Update(const std::int32_t value)
{
    if (value == -1) {
        const auto& results = world::detail::gBattleResultsFF0C14;
        for (std::int32_t row = 0; row < 4; ++row) {
            const world::detail::BattleResultRecord* const result = &results[row];
            gCurrentWindow->elements[row + 5].visible = 0;
            gCurrentWindow->elements[row + 9].visible = 0;
            gCurrentWindow->elements[row + 13].visible = 0;
            gCurrentWindow->elements[row + 1].visible = 0;
            if (result->value00 == 0xFFFFu) {
                continue;
            }
            gCurrentWindow->elements[row + 5].visible = 1;
            gCurrentWindow->elements[row + 9].visible = 1;
            gCurrentWindow->elements[row + 13].visible = 1;
            const std::uint16_t target = result->value00;
            const char* name;
            if (target == 0xFFFEu) {
                const std::int32_t health = WindowItem_HealthImageLevel(world::detail::ReadPlayerInjury());
                WindowInstance* const window = gCurrentWindow;
                name = world::detail::GetPlayerDisplayText(1);
                auto* const graphic = static_cast<WindowGraphicData*>(window->elements[row + 5].data);
                graphic->imageNo = gHealthImages57E6A4[health];
            } else {
                const auto slot = static_cast<std::int16_t>(target);
                const std::int32_t health = WindowItem_HealthImageLevel(world::detail::gPetRuntimeRecords[slot].values70[25]);
                WindowInstance* const window = gCurrentWindow;
                auto* const graphic = static_cast<WindowGraphicData*>(window->elements[row + 5].data);
                graphic->imageNo = gHealthImages57E6A4[health];
                const auto nameSlot = static_cast<std::int16_t>(result->value00);
                const auto& pet = world::detail::gPetRuntimeRecords[nameSlot];
                name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            }
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 9].data)->text = cg::text::FromGbkWide(name);
            if (result->value02 != 0) {
                gCurrentWindow->elements[row + 1].visible = 1;
            }
            const std::uint16_t experienceTarget = result->value00;
            const bool atLimit = experienceTarget == 0xFFFEu
                ? world::detail::IsPlayerNextExperienceEqual(-1)
                : (world::detail::gPetRuntimeRecords[static_cast<std::int16_t>(experienceTarget)].values0C[5] == -1);
            if (atLimit) {
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 13].data)->text = L"  ---  ";
            } else {
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t experience = result->value04;
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[row + 13].data)->text, L"{:+7}", experience);
            }
        }
        const auto& names = world::detail::gBattleResultNamesFF0C34;
        for (std::int32_t row = 0; row < 3; ++row) {
            WindowInstance* const window = gCurrentWindow;
            const char* const name = names[row].data();
            static_cast<WindowTextData*>(window->elements[row + 59].data)->text = cg::text::FromGbkWide(name);
        }

        const auto& skillResults = world::detail::gBattleSkillResultsFF0C8C;
        std::int32_t row = 0;
        for (const auto& pair : world::detail::gSkillPairsF63370) {
            const std::uint16_t used = pair.used;
            const auto index = static_cast<std::int16_t>(pair.index);
            if (used == 0) {
                continue;
            }
            const auto* const skill = &world::detail::gSkillsF1BAFC[index];
            const std::int32_t category = skill->canRebirth;
            if (category != 0 && category != 5) {
                continue;
            }
            const world::detail::BattleSkillResultRecord* const result = &skillResults[index];
            if (result->value00 == 0 && result->value04 == 0) {
                continue;
            }
            const std::int32_t level = skill->level;
            const std::int32_t maxLevel = skill->maxLevel;
            if (level < maxLevel) {
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t experience = result->value04;
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[row + 45].data)->text, L"{:+5}", experience);
            } else {
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 45].data)->text = L" --- ";
            }
            WindowInstance* const window = gCurrentWindow;
            const char* const name = skill->name.data();
            static_cast<WindowTextData*>(window->elements[row + 31].data)->text = cg::text::FromGbkWide(name);
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 31].data)->style.color = std::byte{0};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 45].data)->style.color = std::byte{0};
            gCurrentWindow->elements[row + 17].visible = 0;
            if (result->value00 != 0) {
                gCurrentWindow->elements[row + 17].visible = 1;
            }
            ++row;
            if (row >= 10) {
                break;
            }
        }
        for (; row < 14; ++row) {
            WindowInstance* window = gCurrentWindow;
            const char* text = input::LegacyText593850();
            static_cast<WindowTextData*>(window->elements[row + 31].data)->text = cg::text::FromGbkWide(text);
            window = gCurrentWindow;
            text = input::LegacyText593850();
            static_cast<WindowTextData*>(window->elements[row + 45].data)->text = cg::text::FromGbkWide(text);
            gCurrentWindow->elements[row + 17].visible = 0;
        }
        gBattleResultCloseTickCB0F78 = platform::ReadTickCount32() + 5000u;
    }
    const std::uint32_t now = platform::ReadTickCount32();
    if (gBattleResultCloseTickCB0F78 < now) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x004996C0。
static std::int32_t WindowBattleResult_Draw(const std::int32_t)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const auto& graphics = world::detail::gBattleResultGraphicsFF0D0C;
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t image = graphics[row];
        if (image >= 0) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            login::detail::DrawGraphic(x + 39, y + 166 + row * 49, 223, image, 0, &descriptor);
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x00499750。
static std::int32_t WindowBattleResult_ButtonClose(const std::int32_t, const std::int32_t state)
{
    std::int32_t consumed = (state & 1) != 0 ? 1 : 0;
    if ((state & 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    return consumed;
}

// 原函数：0x00499790。
static std::int32_t WindowDpResult_Update(const std::int32_t value)
{
    if (value == -1) {
        const std::uint32_t now = platform::ReadTickCount32();
        WindowInstance* window = gCurrentWindow;
        gBattleResultCloseTickCB0F78 = now + 5000u;
        const std::int32_t delta = world::detail::gValueFF0D04;
        cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[2].data)->text, L"DP   {:+9}", delta);
        window = gCurrentWindow;
        const std::int32_t total = world::detail::gValueFF0D08;
        cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[3].data)->text, L"Total{:9}", total);
    }
    const std::uint32_t now = platform::ReadTickCount32();
    if (gBattleResultCloseTickCB0F78 < now) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
