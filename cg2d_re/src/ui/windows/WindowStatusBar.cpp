#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

std::wstring gStatusHpTextD40B18 {}; // 原全局：0x00D40B18。

std::wstring gStatusFpTextD45D50 {}; // 原全局：0x00D45D50。
namespace {


constexpr wchar_t kStatusVitalsFormat553FE8[] = L"{:4}/{:4}"; // 原常量参考：0x00553FE8。

constexpr double kStatusTimeScale54F638 = std::bit_cast<double>(std::uint64_t{0x3FC5555555555556}); // 原常量参考：0x0054F638。

// 两刷新同序读取/复制HP并格式化FP；FP复制阶段的窗口采样差别留给caller。
void WindowStatusBar_FormatVitals(std::wstring& buffer)
{
    login::YobiAction* battlePlayer = nullptr;
    if (world::gCurrentSceneType == 10) {
        const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
        if (position < 20) {
            battlePlayer = world::detail::gBattleEntries59E980[position];
        }
    }
    if (battlePlayer != nullptr) {
        const std::int32_t maxHp = login::detail::ReadYobiActionMaxHpOnce(battlePlayer);
        const std::int32_t hpPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t hp = login::detail::ReadYobiActionHpOnce(world::detail::gBattleEntries59E980[hpPosition]);
        cg::text::FormatTo(buffer, kStatusVitalsFormat553FE8, hp, maxHp);
        gStatusHpTextD40B18 = buffer;
        const std::int32_t maxFpPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t maxFp =
            login::detail::ReadYobiActionMaxFpOnce(world::detail::gBattleEntries59E980[maxFpPosition]);
        const std::int32_t fpPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t fp = login::detail::ReadYobiActionFpOnce(world::detail::gBattleEntries59E980[fpPosition]);
        cg::text::FormatTo(buffer, kStatusVitalsFormat553FE8, fp, maxFp);
    } else {
        const std::int32_t maxHp = world::detail::ReadPlayerMaxHpOnce();
        const std::int32_t hp = world::detail::ReadPlayerHpOnce();
        cg::text::FormatTo(buffer, kStatusVitalsFormat553FE8, hp, maxHp);
        gStatusHpTextD40B18 = buffer;
        const std::int32_t maxFp = world::detail::ReadPlayerMaxFpOnce();
        const std::int32_t fp = world::detail::ReadPlayerFpOnce();
        cg::text::FormatTo(buffer, kStatusVitalsFormat553FE8, fp, maxFp);
    }
}

// TUC01：两状态栏四位倒计时；首窗口及各位坐标按原取样序，原单位/图号/门槛保持。
void WindowStatusBar_DrawTimer(const std::int32_t firstElement, render::GraphicDescriptor16* const descriptor)
{
    const std::int32_t duration = world::detail::ReadPlayerTimerRemaining0C();
    const std::int32_t hours = duration / 3600000;
    const std::int32_t hoursTens = hours / 10;
    const std::int32_t hoursUnits = hours % 10;
    const std::int32_t minutes = (duration - hours * 3600000) / 60000;
    const std::int32_t digits[4] {hoursTens, hoursUnits, minutes / 10, minutes % 10};
    gCurrentWindow->elements[firstElement].visible = 0;
    gCurrentWindow->elements[firstElement + 1].visible = 0;
    gCurrentWindow->elements[firstElement + 2].visible = 0;
    const std::int32_t mode = world::detail::ReadPlayerTimerMode10();
    WindowInstance* firstWindow;
    if (firstElement == 8) {
        firstWindow = gCurrentWindow;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor->bytes.data() + 8, &color, sizeof(color));
    descriptor->bytes[13] = 4;
    if (firstElement != 8) {
        firstWindow = gCurrentWindow;
    }
    const std::int32_t baseImage = mode != 0 ? 0x3B6A0 : 0x3B664;
    for (std::int32_t digit = 0; digit < 4; ++digit) {
        WindowInstance* const window = digit == 0 ? firstWindow : gCurrentWindow;
        const WindowElement& element = window->elements[firstElement + (digit == 0 ? 0 : digit - 1)];
        std::int32_t elementY;
        std::int32_t elementX;
        std::int32_t windowY;
        if (digit == 0) {
            elementY = element.y;
            elementX = element.x;
            windowY = window->y;
        } else if (digit < 3) {
            windowY = window->y;
            elementY = element.y;
            elementX = element.x;
        } else {
            elementY = element.y;
            windowY = window->y;
            elementX = element.x;
        }
        const std::int32_t y = elementY + windowY + 3;
        const std::int32_t windowX = window->x;
        const std::int32_t x = elementX + windowX + (digit == 0 ? -6 : 1);
        static_cast<void>(login::detail::DrawGraphic(x, y, 0xDF, baseImage + digits[digit], 0, descriptor));
    }
}

constexpr wchar_t kStatusNoMail556D14[] = L"現在沒有郵件。"; // 原常量参考：0x00556D14。

constexpr wchar_t kStatusNewMail556D24[] = L"現在有新郵件。"; // 原常量参考：0x00556D24。

constexpr wchar_t kStatusNoPoints556D34[] = // 原常量参考：0x00556D34。
    L"現在沒有任何的增加點數。";

constexpr wchar_t kStatusPlayerPoints556D50[] = // 原常量参考：0x00556D50。
    L"現在有增加點數{}點。";

constexpr wchar_t kStatusHealthy556D68[] = L"現在十分健康。"; // 原常量参考：0x00556D68。

constexpr wchar_t kStatusPetInjury556D78[] = L"現在{}瀕臨死亡。"; // 原常量参考：0x00556D78。

constexpr wchar_t kStatusPetInjury556D8C[] = L"現在{}受了重傷。"; // 原常量参考：0x00556D8C。

constexpr wchar_t kStatusPetInjury556DA0[] = L"現在{}受到輕傷。"; // 原常量参考：0x00556DA0。

constexpr wchar_t kStatusPetInjury556DB4[] = L"現在{}受到擦傷。"; // 原常量参考：0x00556DB4。

constexpr wchar_t kStatusPlayerInjury556DC8[] = // 原常量参考：0x00556DC8。
    L"現在正面臨死亡邊緣。";

constexpr wchar_t kStatusPlayerInjury556DE0[] = L"現在受到重傷。"; // 原常量参考：0x00556DE0。

constexpr wchar_t kStatusPlayerInjury556DF0[] = L"現在受了輕傷。"; // 原常量参考：0x00556DF0。

constexpr wchar_t kStatusPlayerInjury556E00[] = // 原常量参考：0x00556E00。
    L"現在受了一點小傷。";

constexpr wchar_t kStatusPetPoints586314[] = // 原常量参考：0x00586314。
    L"現在{}擁有{}點的能力點數。";

constexpr wchar_t kStatusDuelOff556E14[] = L"現在不能跟別人對戰。"; // 原常量参考：0x00556E14。

constexpr wchar_t kStatusDuelOn556E2C[] = L"現在可以跟別人對戰。"; // 原常量参考：0x00556E2C。

constexpr wchar_t kStatusChatOff556E44[] = L"現在進行普通的聊天。"; // 原常量参考：0x00556E44。

constexpr wchar_t kStatusChatOn556E5C[] = L"現在進行小隊的聊天。"; // 原常量参考：0x00556E5C。

constexpr wchar_t kStatusPartyOff556E74[] = // 原常量参考：0x00556E74。
    L"現在禁止別人與自己組隊。";

constexpr wchar_t kStatusPartyOn556E90[] = // 原常量参考：0x00556E90。
    L"現在允許別人與自己組隊。";

constexpr wchar_t kStatusCardOff556EAC[] = // 原常量参考：0x00556EAC。
    L"現在禁止別人與自己交換名片。";

constexpr wchar_t kStatusCardOn556ECC[] = // 原常量参考：0x00556ECC。
    L"現在允許別人與自己交換名片。";

constexpr wchar_t kStatusTradeOff556EEC[] = // 原常量参考：0x00556EEC。
    L"現在禁止別人與自己交易。";

constexpr wchar_t kStatusTradeOn556F08[] = // 原常量参考：0x00556F08。
    L"現在允許別人與自己交易。";

constexpr wchar_t kStatusGuildOff5862D4[] = // 原常量参考：0x005862D4。
    L"現在禁止別人邀請自己參加家族。";

constexpr wchar_t kStatusGuildOn5862F4[] = // 原常量参考：0x005862F4。
    L"現在允許別人邀請自己參加家族。";
} // namespace


// 原函数：0x004708A0。
std::int32_t WindowStatusBar_ButtonEmote(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* window = gWindowStates[WindowId_Emote].window;
    if (window != nullptr) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3B921;
        window = gWindowStates[WindowId_Emote].window;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    if (window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Emote, 1, 0, nullptr));
    } else {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x00470910。
std::int32_t WindowStatusBar_ButtonMiniMap(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* window = gWindowStates[WindowId_MiniMap].window;
    if (window != nullptr) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3B924;
        window = gWindowStates[WindowId_MiniMap].window;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    if (window == nullptr) {
        static_cast<void>(WindowMiniMap_Open(0, 0, nullptr));
    } else {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

// 原函数：0x00470A80。
// 关联原入口：0x00470A9F（共享尾已并入本函数正文）。
std::int32_t WindowStatusBar_Update(const std::int32_t)
{
    if (world::gCurrentSceneType == 10) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(WindowRuntime_Open(WindowId_CompactStatusBar));
    }
    WindowStatusBar_Refresh();
    return 1;
}

// 原函数：0x00470AB0。
std::int32_t WindowCompactStatusBar_Update(const std::int32_t)
{
    WindowCompactStatusBar_Refresh();
    return 1;
}

// TUC01：原窗口1完整根；描述符原零初始化、HP/FP/时钟/提示生产保持，四位消费见共用 helper。
// 原函数：0x00470AC0。
void WindowStatusBar_Refresh()
{
    render::GraphicDescriptor16 descriptor {};
    std::wstring buffer;
    WindowStatusBar_FormatVitals(buffer);
    WindowInstance* const timeWindow = gCurrentWindow;
    gStatusFpTextD45D50 = buffer;
    WindowElement* const elements = timeWindow->elements;
    const std::int32_t hours = std::bit_cast<std::int32_t>(world::gCgTime.hours);
    auto* const timeGraphic = static_cast<WindowGraphicData*>(elements[15].data);
    const std::int32_t minutes = std::bit_cast<std::int32_t>(world::gCgTime.minutes);
    const std::int32_t totalMinutes = hours * 60 + minutes;
    const double offset = static_cast<double>(totalMinutes) * kStatusTimeScale54F638;
    timeGraphic->offsetX = static_cast<std::int32_t>(static_cast<std::int64_t>(offset));
    timeGraphic->offsetY = 0;
    timeGraphic->width = 100;
    timeGraphic->height = 9;
    WindowStatusBar_DrawTimer(8, &descriptor);

    const std::int32_t points = world::detail::Sub_504B10();
    const std::int32_t initialInjury = world::detail::ReadPlayerInjury();
    gLoginWindowRuntime.statusHasPointsD66E24 = points != 6 ? 1 : 0;
    if (initialInjury == 0) {
        const std::int32_t pet = world::detail::Sub_504B60();
        gLoginWindowRuntime.statusHasInjuryD046C8 = 0;
        if (pet != -1) {
            gLoginWindowRuntime.statusHasInjuryD046C8 = 1;
        }
    } else {
        gLoginWindowRuntime.statusHasInjuryD046C8 = 1;
    }
    gLoginWindowRuntime.statusHasMailCC4B5C = world::detail::Sub_518DA0() != 0 ? 1 : 0;
    std::int32_t phase = gLoginWindowRuntime.statusBlinkPhaseCF4FBC + 1;
    gLoginWindowRuntime.statusBlinkPhaseCF4FBC = phase;
    if (phase > 60) {
        phase = 0;
        gLoginWindowRuntime.statusBlinkPhaseCF4FBC = phase;
    }
    const bool pointsBlink = gLoginWindowRuntime.statusHasPointsD66E24 != 0 && phase >= 30;
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[13].data)->imageNo =
        pointsBlink ? 0x3B953 : 0x3B954;

    bool injuryBlink = false;
    if (gLoginWindowRuntime.statusHasInjuryD046C8 != 0) {
        injuryBlink = gLoginWindowRuntime.statusBlinkPhaseCF4FBC >= 30;
    }
    const std::int32_t injury = world::detail::ReadPlayerInjury();
    const std::int32_t injuryImage = injury == 0 ? 0x3B941
        : injury <= 25 ? 0x3B944
        : injury <= 50 ? 0x3B947
        : injury <= 75 ? 0x3B94A : 0x3B94D;
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[11].data)->imageNo =
        injuryImage - (injuryBlink ? 1 : 0);
    const bool mailBlink = gLoginWindowRuntime.statusHasMailCC4B5C != 0
        && gLoginWindowRuntime.statusBlinkPhaseCF4FBC >= 30;
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[12].data)->imageNo =
        mailBlink ? 0x3B94F : 0x3B950;
}

// TUC01：原窗口2完整根；直接FP复制/原零描述符保持，仅消费四位倒计时。
// 原函数：0x004718F0。
void WindowCompactStatusBar_Refresh()
{
    render::GraphicDescriptor16 descriptor {};
    std::wstring buffer;
    WindowStatusBar_FormatVitals(buffer);
    gStatusFpTextD45D50 = buffer;
    WindowStatusBar_DrawTimer(2, &descriptor);
}

// 原函数：0x00471CD0。
std::int32_t WindowStatusBar_ButtonCompact(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(WindowId_CompactStatusBar));
    gValueCF4690 = 1;
    return 1;
}

// 原函数：0x00471D10。
std::int32_t WindowCompactStatusBar_ButtonCollapse(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(WindowRuntime_Open(WindowId_CollapsedStatusBar));
    gValueCF4690 = 2;
    return 1;
}

// 原函数：0x00471D60。
std::int32_t WindowCollapsedStatusBar_ButtonExpand(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    const std::int32_t scene = world::gCurrentSceneType;
    gCurrentWindow->state |= 0x8000u;
    if (scene == 10) {
        static_cast<void>(WindowRuntime_Open(WindowId_CompactStatusBar));
        gValueCF4690 = 1;
    } else {
        static_cast<void>(WindowRuntime_Open(WindowId_StatusBar));
        gValueCF4690 = 0;
    }
    return 1;
}

// 原函数：0x00471DD0。
std::int32_t WindowStatusBar_HandleHover(const std::int32_t, const std::int32_t state)
{
    return (state & 1) != 0 ? 1 : 0;
}

// 原函数：0x00471DF0。
std::int32_t WindowStatusBar_HandleOptionButtons(const std::int32_t index, const std::int32_t state)
{
    // 原先访问对应图形，再分派命中区编号；不将默认分支改成入口保护。
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index - 15].data);
    const bool hover = (state & 1) != 0;
    std::int32_t consumed = hover ? 1 : 0;
    graphic->imageNo = 0x3B8FC + index * 3 + (hover ? 1 : 0);
    if (index < 17 || index > 22) {
        return consumed;
    }
    if (index == 22 && character_select::state::gPukVersion < 7u) {
        if (hover) {
            graphic->imageNo = 0x3B93E;
            gHoverTextD68AE8 = kStatusGuildUnavailable586270;
        }
        return consumed;
    }
    const std::uint8_t options = world::detail::ReadPlayerOptions194LowByte();
    constexpr std::uint8_t masks[6] {0x04, 0x08, 0x01, 0x10, 0x20, 0x40};
    const wchar_t* const hints[6][2] {
        {kStatusDuelOff556E14, kStatusDuelOn556E2C},
        {kStatusChatOff556E44, kStatusChatOn556E5C},
        {kStatusPartyOff556E74, kStatusPartyOn556E90},
        {kStatusCardOff556EAC, kStatusCardOn556ECC},
        {kStatusTradeOff556EEC, kStatusTradeOn556F08},
        {kStatusGuildOff5862D4, kStatusGuildOn5862F4},
    };
    std::int32_t (*const toggleCallbacks[6])() {
        &world::detail::Sub_472F40, &world::detail::Sub_472F70, &world::detail::Sub_472FA0,
        &world::detail::Sub_472FD0, &world::detail::Sub_473000, &world::detail::Sub_473030,
    };
    const std::int32_t slot = index - 17;
    const bool enabled = (options & masks[slot]) != 0;
    if (hover) {
        gHoverTextD68AE8 = hints[slot][enabled ? 1 : 0];
    }
    if (enabled) {
        graphic->imageNo = 0x3B92E + slot * 3;
    }
    if ((state & 2) != 0) {
        static_cast<void>(toggleCallbacks[slot]());
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    return consumed;
}

// SHP01：原玩家伤势优先返0，否则宠物编号+10或-1。
// 原函数：0x00473080。
std::int32_t WindowStatusBar_ReadInjuredPetIndex()
{
    if (world::detail::ReadPlayerInjury() != 0) {
        return 0;
    }
    const std::int32_t pet = world::detail::Sub_504B60();
    return pet == -1 ? -1 : pet + 10;
}

// SHP01：原五S16配对首个编号匹配位置，无则-1。
// 原函数：0x004731B0。
std::int32_t WindowStatusBar_FindSortedPetIndex(const std::int32_t index)
{
    for (std::int32_t position = 0; position < 5; ++position) {
        const std::int16_t candidate =
            std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
        if (index == candidate) {
            return position;
        }
    }
    return -1;
}

// SHP01：原三项提示/关闭开窗；hover持有及姓名/分档/窗口读取序，原业务门槛保持。
// 原函数：0x004723D0。
std::int32_t WindowStatusBar_HandleHintButton(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    std::wstring buffer;
    switch (index) {
    case 13: {
        const std::int32_t candidate = world::detail::Sub_504B10();
        const bool hover = (state & 1) != 0;
        if (hover) {
            if (gLoginWindowRuntime.statusHasPointsD66E24 != 0) {
                if (candidate <= 4) {
                    const std::int16_t petIndex =
                        std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[candidate].index);
                    const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex];
                    const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    const std::int32_t points = pet.lvFlag6C;
                    cg::text::FormatTo(buffer, kStatusPetPoints586314, cg::text::FromGbkWide(name).c_str(), points);
                } else {
                    const std::int32_t points = world::detail::gValueD2F788;
                    cg::text::FormatTo(buffer, kStatusPlayerPoints556D50, points);
                }
            } else {
                buffer = kStatusNoPoints556D34;
            }
            gHoverTextD68AE8 = buffer;
        }
        if (gLoginWindowRuntime.statusHasPointsD66E24 == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* const hoverWindow = gCurrentWindow;
            consumed = 1;
            static_cast<WindowGraphicData*>(hoverWindow->elements[13].data)->imageNo = 0x3B955;
        }
        if ((state & 2) != 0) {
            if (candidate <= 4) {
                WindowInstance* const details = gWindowStates[WindowId_PetDetail].window;
                gLoginWindowRuntime.selectedPetCC3E5C = candidate;
                if (details != nullptr) {
                    details->state |= 0x8000u;
                }
                WindowInstance* const skills = gWindowStates[WindowId_PetSkills].window;
                if (skills != nullptr) {
                    skills->state |= 0x8000u;
                }
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetAttributes, 1, 0, nullptr));
            } else {
                WindowInstance* const profile = gWindowStates[WindowId_PlayerProfile].window;
                if (profile != nullptr) {
                    profile->state |= 0x8000u;
                }
                WindowInstance* const titles = gWindowStates[WindowId_PlayerTitles].window;
                if (titles != nullptr) {
                    titles->state |= 0x8000u;
                }
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PlayerAttributes, 1, 0, nullptr));
            }
            consumed = 1;
        }
        return consumed;
    }
    case 12: {
        const std::int32_t hasMail = gLoginWindowRuntime.statusHasMailCC4B5C;
        const bool hover = (state & 1) != 0;
        if (hover) {
            gHoverTextD68AE8 = hasMail != 0 ? kStatusNewMail556D24 : kStatusNoMail556D14;
            if (hasMail == 0) {
                return 0;
            }
        }
        if (hasMail == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* const hoverWindow = gCurrentWindow;
            consumed = 1;
            static_cast<WindowGraphicData*>(hoverWindow->elements[12].data)->imageNo = 0x3B951;
        }
        if ((state & 2) != 0) {
            world::detail::Sub_518F00(
                &gLoginWindowRuntime.statusMailCategoryD66E34, &gLoginWindowRuntime.statusMailIndexD2E2D8);
            const std::int32_t category = gLoginWindowRuntime.statusMailCategoryD66E34;
            if (category == 0 || category == 1 || category == 2) {
                const std::int32_t mailIndex = gLoginWindowRuntime.statusMailIndexD2E2D8;
                gLoginWindowRuntime.mailCategoryD67B64 = category;
                world::detail::gValueCF4FC0 = mailIndex;
                gLoginWindowRuntime.mailCategoryCC4D50 = category;
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailRead, 1, 0, nullptr));
            }
            consumed = 1;
        }
        return consumed;
    }
    case 11: {
        const std::int32_t target = WindowStatusBar_ReadInjuredPetIndex();
        std::int32_t injury = world::detail::ReadPlayerInjury();
        const bool hover = (state & 1) != 0;
        const std::int32_t petIndex = target - 10;
        if (hover) {
            if (injury != 0) {
                const wchar_t* const text = injury <= 25 ? kStatusPlayerInjury556E00
                    : injury <= 50 ? kStatusPlayerInjury556DF0
                    : injury <= 75 ? kStatusPlayerInjury556DE0 : kStatusPlayerInjury556DC8;
                gHoverTextD68AE8 = text;
            } else if (target == -1) {
                gHoverTextD68AE8 = kStatusHealthy556D68;
            } else {
                const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex];
                const std::int32_t petInjury = pet.values70[25];
                const char* name;
                const wchar_t* format;
                if (petInjury <= 25) {
                    format = kStatusPetInjury556DB4;
                    name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                } else if (petInjury <= 50) {
                    format = kStatusPetInjury556DA0;
                    name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                } else {
                    const char nameFirst = pet.name6BD[0];
                    const bool critical = petInjury > 75;
                    name = nameFirst != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                    format = critical ? kStatusPetInjury556D78 : kStatusPetInjury556D8C;
                }
                cg::text::FormatTo(buffer, format, cg::text::FromGbkWide(name).c_str());
                const std::size_t length = buffer.size() + 1;
                const std::size_t bulk = length & ~std::size_t{3};
                gHoverTextD68AE8 = buffer;
                injury = world::detail::ReadPlayerInjury();
                static_cast<void>(0);
            }
        }
        if (gLoginWindowRuntime.statusHasInjuryD046C8 == 0) {
            return 0;
        }
        if (hover) {
            WindowInstance* imageWindow;
            std::int32_t image;
            if (injury == 0) {
                imageWindow = gCurrentWindow;
                image = 0x3B942;
            } else if (injury <= 25) {
                imageWindow = gCurrentWindow;
                image = 0x3B945;
            } else if (injury <= 50) {
                imageWindow = gCurrentWindow;
                image = 0x3B948;
            } else {
                imageWindow = gCurrentWindow;
                image = injury <= 75 ? 0x3B94B : 0x3B94E;
            }
            static_cast<WindowGraphicData*>(imageWindow->elements[11].data)->imageNo = image;
            consumed = 1;
        }
        if ((state & 2) != 0) {
            if (target != -1) {
                if (target == 0) {
                    WindowInstance* const attributes = gWindowStates[WindowId_PlayerAttributes].window;
                    if (attributes != nullptr) {
                        attributes->state |= 0x8000u;
                    }
                    WindowInstance* const titles = gWindowStates[WindowId_PlayerTitles].window;
                    if (titles != nullptr) {
                        titles->state |= 0x8000u;
                    }
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PlayerProfile, 1, 0, nullptr));
                } else {
                    WindowInstance* const attributes = gWindowStates[WindowId_PetAttributes].window;
                    if (attributes != nullptr) {
                        attributes->state |= 0x8000u;
                    }
                    WindowInstance* const skills = gWindowStates[WindowId_PetSkills].window;
                    if (skills != nullptr) {
                        skills->state |= 0x8000u;
                    }
                    gLoginWindowRuntime.selectedPetCC3E5C = WindowStatusBar_FindSortedPetIndex(petIndex);
                    gLoginWindowRuntime.valueD673D0 = 5;
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetDetail, 1, 0, nullptr));
                }
            }
            consumed = 1;
        }
        return consumed;
    }
    default:
        return 0;
    }
}

} // namespace detail

} // namespace cg::ui
