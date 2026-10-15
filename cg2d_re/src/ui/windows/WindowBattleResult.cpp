#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {


std::uint32_t gBattleResultCloseTickCB0F78 = 0; // 原全局：0x00CB0F78。 原窗口30/31共用到期tick，不按窗口复制。
} // namespace


// 原函数：0x004991C0。
std::int32_t WindowBattleResult_Update(const std::int32_t value)
{
    if (value == -1) {
        const auto& results = world::detail::BorrowBattleResults();
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
                name = world::detail::BorrowPlayerDisplayText(1);
                auto* const graphic = static_cast<WindowGraphicData*>(window->elements[row + 5].data);
                graphic->imageNo = gHealthImages57E6A4[health];
            } else {
                const auto slot = static_cast<std::int16_t>(target);
                const std::int32_t health = WindowItem_HealthImageLevel((*world::detail::BorrowPetRuntimeF633F8Storage())[slot].values70[25]);
                WindowInstance* const window = gCurrentWindow;
                auto* const graphic = static_cast<WindowGraphicData*>(window->elements[row + 5].data);
                graphic->imageNo = gHealthImages57E6A4[health];
                const auto nameSlot = static_cast<std::int16_t>(result->value00);
                const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[nameSlot];
                name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            }
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 9].data)->text = cg::text::FromGbkWide(name);
            if (result->value02 != 0) {
                gCurrentWindow->elements[row + 1].visible = 1;
            }
            const std::uint16_t experienceTarget = result->value00;
            const bool atLimit = experienceTarget == 0xFFFEu
                ? world::detail::IsPlayerNextExperienceEqual(-1)
                : ((*world::detail::BorrowPetRuntimeF633F8Storage())[static_cast<std::int16_t>(experienceTarget)].values0C[5] == -1);
            if (atLimit) {
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 13].data)->text = L"  ---  ";
            } else {
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t experience = result->value04;
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[row + 13].data)->text, L"{:+7}", experience);
            }
        }
        const auto& names = world::detail::BorrowBattleResultNames();
        for (std::int32_t row = 0; row < 3; ++row) {
            WindowInstance* const window = gCurrentWindow;
            const char* const name = names[row].data();
            static_cast<WindowTextData*>(window->elements[row + 59].data)->text = cg::text::FromGbkWide(name);
        }

        const auto& skillResults = world::detail::BorrowBattleSkillResults();
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
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 31].data)->bytes00[0] = std::byte{0};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 45].data)->bytes00[0] = std::byte{0};
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
std::int32_t WindowBattleResult_Draw(const std::int32_t)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFF;
    descriptor.bytes[9] = 0xFF;
    descriptor.bytes[10] = 0xFF;
    descriptor.bytes[11] = 0xFF;
    descriptor.bytes[13] = 4;
    const auto& graphics = world::detail::BorrowBattleResultGraphics();
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
std::int32_t WindowBattleResult_ButtonClose(const std::int32_t, const std::int32_t state)
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
std::int32_t WindowDpResult_Update(const std::int32_t value)
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
