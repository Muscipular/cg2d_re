#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {


// 原474FD1..4753A6：每组先清显隐后读属性，图号与显隐各自重取窗口。
void WindowPlayerProfile_RefreshAttributeBar(
    const std::int32_t attribute, const std::int32_t firstElement,
    const std::int32_t middleImage, const std::int32_t firstImage,
    const std::int32_t lastImage, const std::int32_t singleImage)
{
    for (std::int32_t index = 0; index < 10; ++index) {
        gCurrentWindow->elements[firstElement + index].visible = 0;
    }
    const std::int32_t count = world::detail::ReadPlayerAttribute(attribute) / 10;
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
} // namespace


// 原474BE0：五按钮及默认分支；位置请求不写玩家状态，返回仍为悬停值。
// 原函数：0x00474BE0。
std::int32_t WindowPlayerProfile_HandleButtons(const std::int32_t index, const std::int32_t state)
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
        hint = L"切換成玩家明細視窗。";
        break;
    case 20:
        idleImage = 0x3BAB8;
        hoverImage = 0x3BABA;
        targetWindow = 9;
        hint = L"切換成玩家稱號視窗。";
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
        hint = L"切換成玩家簡介視窗";
        break;
    case 23:
        idleImage = 0x3BAC0;
        hoverImage = 0x3BAC1;
        hint = L"把戰鬥時的位置變更為前排。";
        break;
    case 24:
        idleImage = 0x3BABD;
        hoverImage = 0x3BABE;
        hint = L"把戰鬥時的位置變更為後排。";
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
        } else if (world::detail::ReadPlayerValue1A0() == (index == 23 ? 0 : 1)) {
            static_cast<void>(network::Sub_512570(platform::gConnectionSocket));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    return consumed;
}

// 原474FB0：窗口7完整刷新，复制阶段的elements/data快照差异见window_7.md。
// 原函数：0x00474FB0。
void WindowPlayerProfile_Refresh()
{
    WindowElement* const portraitElements = gCurrentWindow->elements;
    const std::int32_t portrait = world::detail::ReadPlayerValue17C();
    static_cast<WindowGraphicData*>(portraitElements[14].data)->imageNo = portrait;
    WindowPlayerProfile_RefreshAttributeBar(0, 25, 0x3BAC7, 0x3BACA, 0x3BAC8, 0x3BAC9);
    WindowPlayerProfile_RefreshAttributeBar(1, 35, 0x3BACC, 0x3BACF, 0x3BACD, 0x3BACE);
    WindowPlayerProfile_RefreshAttributeBar(2, 45, 0x3BAD1, 0x3BAD4, 0x3BAD2, 0x3BAD3);
    WindowPlayerProfile_RefreshAttributeBar(3, 55, 0x3BAD6, 0x3BAD9, 0x3BAD7, 0x3BAD8);

    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
    const char* const name = world::detail::BorrowPlayerDisplayText(1);
    const std::size_t nameLength = std::strlen(name) + 1;
    nameData->text = cg::text::FromGbkWide(name);

    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", world::detail::ReadPlayerLevel());
    auto* const levelData = static_cast<WindowTextData*>(gCurrentWindow->elements[1].data);
    const std::size_t levelLength = buffer.size() + 1;
    levelData->text = buffer;

    WindowElement* const jobElements = gCurrentWindow->elements;
    const char* const job = world::detail::BorrowPlayerJobName();
    const std::size_t jobLength = std::strlen(job) + 1;
    auto* const jobData = static_cast<WindowTextData*>(jobElements[3].data);
    jobData->text = cg::text::FromGbkWide(job);

    WindowInstance* const guildWindow = gCurrentWindow;
    const std::int32_t guildIndex = login::detail::ReadGuildTextIndex10514B0();
    auto* const guildData = static_cast<WindowTextData*>(guildWindow->elements[4].data);
    if (guildIndex != -1) {
        const char* const guild = login::detail::BorrowGuildName10514BC();
        const std::size_t guildLength = std::strlen(guild) + 1;
        guildData->text = cg::text::FromGbkWide(guild);
        const std::int32_t rankIndex = login::detail::ReadGuildTextIndex10514B0();
        auto* const rankData = static_cast<WindowTextData*>(gCurrentWindow->elements[5].data);
        const char* const rank = login::detail::BorrowGuildRankText1051518(rankIndex);
        const std::size_t rankLength = std::strlen(rank) + 1;
        rankData->text = cg::text::FromGbkWide(rank);
    } else {
        const char* const guild = input::LegacyText593850();
        const std::size_t guildLength = std::strlen(guild) + 1;
        guildData->text = cg::text::FromGbkWide(guild);
        auto* const rankData = static_cast<WindowTextData*>(gCurrentWindow->elements[5].data);
        const char* const rank = input::LegacyText593850();
        const std::size_t rankLength = std::strlen(rank) + 1;
        rankData->text = cg::text::FromGbkWide(rank);
    }

    const std::int32_t titleIndex = world::detail::ReadPlayerTitleIndex();
    auto* const titleData = static_cast<WindowTextData*>(gCurrentWindow->elements[6].data);
    const char* const title = world::detail::BorrowTitleText(titleIndex);
    const std::size_t titleLength = std::strlen(title) + 1;
    titleData->text = cg::text::FromGbkWide(title);

    WindowElement* const ownTitleElements = gCurrentWindow->elements;
    const char* const ownTitle = world::detail::BorrowPlayerDisplayText(2);
    const std::size_t ownTitleLength = std::strlen(ownTitle) + 1;
    auto* const ownTitleData = static_cast<WindowTextData*>(ownTitleElements[7].data);
    ownTitleData->text = cg::text::FromGbkWide(ownTitle);

    WindowInstance* const tribeWindow = gCurrentWindow;
    const std::int16_t tribeIndex = world::detail::ReadPlayerTribe();
    WindowElement* const tribeElements = tribeWindow->elements;
    const wchar_t* const tribe = gTribeNames57CB54[tribeIndex];
    auto* const tribeData = static_cast<WindowTextData*>(tribeElements[8].data);
    tribeData->text = tribe;

    cg::text::FormatTo(buffer, L"{:9}", world::detail::ReadPlayerExperienceOnce());
    auto* const experienceData = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
    const std::size_t experienceLength = buffer.size() + 1;
    experienceData->text = buffer;

    const std::int32_t maxHp = world::detail::ReadPlayerMaxHpOnce();
    const std::int32_t hp = world::detail::ReadPlayerHpOnce();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", hp, maxHp);
    auto* const hpData = static_cast<WindowTextData*>(gCurrentWindow->elements[11].data);
    const std::size_t hpLength = buffer.size() + 1;
    hpData->text = buffer;

    const std::int32_t maxFp = world::detail::ReadPlayerMaxFpOnce();
    const std::int32_t fp = world::detail::ReadPlayerFpOnce();
    cg::text::FormatTo(buffer, L"{:4}/{:4}", fp, maxFp);
    auto* const fpData = static_cast<WindowTextData*>(gCurrentWindow->elements[12].data);
    const std::size_t fpLength = buffer.size() + 1;
    fpData->text = buffer;

    std::wstring_view nextExperience;
    if (world::detail::IsPlayerNextExperienceEqual(-1)) {
        nextExperience = L"        -";
    } else {
        cg::text::FormatTo(buffer, L"{:9}", world::detail::ReadPlayerNextExperienceOnce());
        nextExperience = buffer;
    }
    auto* const nextData = static_cast<WindowTextData*>(gCurrentWindow->elements[10].data);
    const std::size_t nextLength = nextExperience.size() + 1;
    nextData->text = nextExperience;

    cg::text::FormatTo(buffer, L"{:9}", world::detail::ReadPlayerDuelPoint());
    auto* const duelData = static_cast<WindowTextData*>(gCurrentWindow->elements[13].data);
    const std::size_t duelLength = buffer.size() + 1;
    duelData->text = buffer;

    if (world::detail::ReadPlayerValue1A0() == 1) {
        gCurrentWindow->elements[22].x = 227;
    } else {
        gCurrentWindow->elements[22].x = 263;
    }
    gCurrentWindow->elements[22].y = 203;

    const std::int32_t injury = world::detail::ReadPlayerInjury();
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

    const std::int32_t penalty = world::detail::ReadPlayerPenalty();
    if (penalty > 0) {
        cg::text::FormatTo(buffer, L"x{}", penalty);
        WindowElement* const penaltyElements = gCurrentWindow->elements;
        const std::size_t penaltyLength = buffer.size() + 1;
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
std::int32_t WindowPlayerProfile_Update(const std::int32_t)
{
    WindowPlayerProfile_Refresh();
    return 1;
}

// 原函数：0x00474310。
void WindowPlayerProfile_Release()
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
