#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"
#include "LegacyPanelRuntime.hpp"
#include "LoginSceneDependencies.hpp"
#include "platform/MainLoopDependencies.hpp"
#include "platform/LegacyFormat.hpp"
#include "ui/WindowRuntimeState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/LoginState.hpp"
#include "cg/LoginScene.hpp"
#include "cg/LoginInput.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/DisplaySettings.hpp"
#include "cg/GraphicAlloc.hpp"
#include "cg/LegacyRecordOrder.hpp"
#include "cg/Input.hpp"
#include "cg/Blacklist.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#include "LegacyPanelTexts.hpp"
#include "LoginStateDependencies.hpp"

namespace cg::login::detail {

std::int32_t gLegacyPetSkillCatalogImageCC3E64 = 0;
std::int32_t gLegacyPetSkillCatalogMessageD2E818 = 0;
std::int32_t gLegacyPetSkillCatalogPageCountC8AF84 = 0;
std::int32_t gLegacyPetSkillCatalogPageD45F74 = 0;
std::int32_t gLegacyPetSkillCatalogSelectionC8AF70 = 0;
std::int32_t gLegacyPetSkillPetSelectionC8A94C = 0;
std::int32_t gLegacyTradeTargetCountD30134 = 0;
std::int32_t gLegacyTradeTargetPageCC3208 = 0;
std::int32_t gLegacyTradeTargetPageCountCF46D0 = 0;

namespace {

YobiAction* gLegacyPetSkillCatalogActionD67C5C = nullptr;
LegacyPanelRecord gLegacyPetSkillCatalogPanel581FB8 {
    0, 0, 576, 432, 58, {23090, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyPetSkillConfirmPanel582210 {
    160, 108, 320, 240, 61, {}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyPetSkillPetActionD67C60 = nullptr;
LegacyPanelRecord gLegacyPetSkillPetPanel582080 {
    0, 0, 576, 432, 59, {23091, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyPetSkillSlotCF4B64 = 0;
YobiAction* gLegacyPetSkillSlotsActionD67C64 = nullptr;
LegacyPanelRecord gLegacyPetSkillSlotsPanel582148 {
    0, 0, 576, 432, 60, {23092, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyTradeTargetPanel582E08 {
    160, 132, 320, 192, 51, {}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyTradeTargetSelectionCC4D48 = 0;
constexpr char kLegacyPetSkillAccept588D34[] = "\xD7\x8C\x8C\x99\xCE\xEF\x8C\x57\xC1\x95\xBC\xBC\xC4\xDC\xA1\xA3";
constexpr char kLegacyPetSkillCatalogCancel588CD0[] = "\xC8\xA1\xCF\xFB\xD7\x8C\x8C\x99\xCE\xEF\x8C\x57\xC1\x95\xBC\xBC\xC4\xDC\xA1\xA3";
constexpr char kLegacyPetSkillCatalogLearn588268[] = "\xD7\x8C\x8C\x99\xCE\xEF\x8C\x57\xC1\x95\xB4\xCB\xBC\xBC\xC4\xDC\xA1\xA3";
constexpr char kLegacyPetSkillEmptySlot5882F4[] = "\xCA\xB9\xD3\xC3\xA1\xBA\xBF\xD5\x99\xDA\xCE\xBB\xA1\xBB\xB5\xC4\xCE\xBB\xD6\xC3";
constexpr char kLegacyPetSkillGold588D5C[] = "\xB3\xD6\xD3\xD0\xBD\xF0\xEE\x7E\xA1\xA1\x20\x25\x37\x64\x20\xBD\xF0\x8E\xC5";
constexpr char kLegacyPetSkillLearnLine5882E8[] = "\x81\xED\x8C\x57\xC1\x95\xA1\xA3";
constexpr char kLegacyPetSkillPetCancel588CE8[] = "\xC8\xA1\xCF\xFB\xD7\x8C\x8C\x99\xCE\xEF\x8C\x57\xC1\x95\xB4\xCB\xBC\xBC\xC4\xDC\xA1\xA3";
constexpr char kLegacyPetSkillPetMeasure588320[] = "\xD7\x8C\xA1\xBA\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x36\xA1\xBB";
constexpr char kLegacyPetSkillPetName588338[] = "\xD7\x8C\xA1\xBA\x25\x73\xA1\xBB";
constexpr char kLegacyPetSkillPrice588D48[] = "\xD9\x8F\xD9\x49\xBD\xF0\xEE\x7E\x20\x20\x20\x25\x37\x64\x20\xBD\xF0\x8E\xC5";
constexpr char kLegacyPetSkillQuestion55C9B0[] = "\xBF\xC9\xD2\xD4\x86\xE1\xA3\xBF";
constexpr char kLegacyPetSkillSlotHint588D00[] = "\xCA\xB9\xD3\xC3\xB4\xCB\x99\xDA\x81\xED\x8C\x57\xC1\x95\xBC\xBC\xC4\xDC\xA1\xA3";
constexpr char kLegacyPetSkillStop588D18[] = "\xCD\xA3\xD6\xB9\xCA\xB9\xD3\xC3\xB4\xCB\x99\xDA\x81\xED\x8C\x57\xC1\x95\xBC\xBC\xC4\xDC\xA1\xA3";
constexpr char kLegacyPetSkillTitle588344[] = "\x8C\x57\xC1\x95\xA1\xBA\x25\x73\xA1\xBB";
constexpr char kLegacyPetSkillUsedSlot58830C[] = "\xCA\xB9\xD3\xC3\xA1\xBA\x25\x73\xA1\xBB\xB5\xC4\xCE\xBB\xD6\xC3";
constexpr char kLegacyTradeTargetHint589368[] = "\xC5\x63\xDF\x40\x82\x80\xBD\xC7\xC9\xAB\xDF\x4D\xD0\xD0\xBD\xBB\xD2\xD7\xA1\xA3";

std::uint16_t Low16(const std::int32_t value)
{
    return static_cast<std::uint16_t>(
        std::bit_cast<std::uint32_t>(value));
}

} // namespace

// 原函数：0x004E4EA0。
std::uint32_t Sub_4E4EA0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyTradeTargetPanel582E08.x;
    const std::int32_t y = gLegacyTradeTargetPanel582E08.y;
    const std::array<character_select::detail::CharacterModalUiRecord, 2> cancels {{
        {x + 133, y + 162, x + 131, y + 162, 57, 18, kLegacyItemUseCancel589008},
        {x + 79, y + 162, x + 77, y + 162, 57, 18, kLegacyItemUseCancel589008},
    }};
    const std::array<character_select::detail::CharacterModalUiRecord, 2> pages {{
        {x + 195, y + 162, x + 193, y + 162, 37, 18, kLegacySkillTargetNext589360},
        {x + 195, y + 162, x + 193, y + 162, 37, 18, kLegacySkillTargetPrevious589358},
    }};
    const std::uint32_t state = static_cast<std::uint32_t>(
        Sub_503160(opened, &gLegacyTradeTargetPanel582E08, nullptr, 0));
    if (state == 0 || state == 2) {
        return state;
    }
    const bool manyPages = gLegacyTradeTargetPageCountCF46D0 > 1;
    std::int32_t page = gLegacyTradeTargetPageCC3208;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 5; ++row) {
        if (row + 5 * page >= gLegacyTradeTargetCountD30134) {
            continue;
        }
        const std::int32_t top = gLegacyTradeTargetPanel582E08.y + 20 * row + 50;
        const std::int32_t left = gLegacyTradeTargetPanel582E08.x + 86;
        if (Sub_5049A0(left, top, left + 148, top + 20, -1) != 0) {
            hovered = row;
            break;
        }
        page = gLegacyTradeTargetPageCC3208;
    }
    const auto* const cancel = &cancels[manyPages ? 1 : 0];
    const std::int32_t cancelState = character_select::detail::Sub_503990(cancel);
    const std::int32_t pageState = manyPages
        ? character_select::detail::Sub_503990(&pages[gLegacyTradeTargetPageCC3208]) : 0;
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1U) != 0) {
        const std::int32_t currentPage = gLegacyTradeTargetPageCC3208;
        const auto socket = platform::gConnectionSocket;
        const std::int32_t selected = hovered + 5 * currentPage;
        gLegacyTradeTargetSelectionCC4D48 = selected;
        static_cast<void>(network::Sub_511FF0(socket, selected));
        const char* const name = gLegacyTradeTargetNamesCB1D1C[gLegacyTradeTargetSelectionCC4D48].data();
        static_cast<void>(ui::detail::Sub_4E5620(name));
    } else if ((cancelState & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyTradeTargetPanel582E08.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((pageState & 1) != 0) {
        gLegacyTradeTargetPageCC3208 = (gLegacyTradeTargetPageCC3208 - 1) & 1;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.x + 58),
        static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.y + 24),
        2, 0, 4, kLegacyItemUseTitle585F54, 0, 0));
    if (manyPages) {
        const std::int32_t currentPage = gLegacyTradeTargetPageCC3208;
        const std::int32_t pageCount = gLegacyTradeTargetPageCountCF46D0;
        char buffer[128];
        platform::FormatLegacyString_53B304(buffer, "%d/%d", currentPage + 1, pageCount);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.x + 270),
            static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.y + 24), 2, 0, 0, buffer, 0, 0));
    }
    page = gLegacyTradeTargetPageCC3208;
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t index = row + 5 * page;
        if (index < gLegacyTradeTargetCountD30134) {
            const char* const name = gLegacyTradeTargetNamesCB1D1C[index].data();
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.x + 88),
                static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.y + 52 + 20 * row), 2, 0, 0, name, 0, 0));
            page = gLegacyTradeTargetPageCC3208;
        }
    }
    static_cast<void>(character_select::detail::Sub_503A20(cancel, 2, 4, 1, gValueD69768, 218));
    if (manyPages) {
        const auto* const button = &pages[gLegacyTradeTargetPageCC3208];
        static_cast<void>(character_select::detail::Sub_503A20(button, 2, 4, 1, gValueD69768, 218));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyTradeTargetPanel582E08.y + 50 + 20 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyTradeTargetPanel582E08.x + 86;
        DrawGraphicHover(Low16(left), Low16(top), Low16(left + 148), Low16(top + 20), 218, color, 0);
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyTradeTargetHint589368);
    } else if ((cancelState & 0x40) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacyTradeCancel5889AC);
    }
    return Sub_503440(&gLegacyTradeTargetPanel582E08, 0);
}

// 原函数：0x004D7440。
YobiAction* Sub_4D7440()
{
    const std::int32_t y = gLegacyPetSkillCatalogPanel581FB8.y;
    const std::int32_t x = gLegacyPetSkillCatalogPanel581FB8.x;
    const std::int32_t image = gLegacyPetSkillCatalogImageCC3E64;
    YobiAction* const action = Sub_4D6640(image, x + 468, y + 245);
    gLegacyPetSkillCatalogActionD67C5C = action;
    return action;
}

// 原旧58完整消费路径；目录、图号、页数三项定义/生产尚缺，不提前注册页面。
// 原函数：0x004D7470。
std::uint32_t Sub_4D7470(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetSkillCatalogPanel581FB8.x;
    const std::int32_t y = gLegacyPetSkillCatalogPanel581FB8.y;
    LegacyPanelButtonRecord cancel {x + 465, y + 390, x + 425, y + 374, 80, 32, 23076, 23077};
    LegacyPanelButtonRecord previous {x + 140, y + 52, x + 126, y + 40, 28, 24, 23022, 23026};
    LegacyPanelButtonRecord next {x + 230, y + 52, x + 216, y + 40, 28, 24, 23023, 23027};
    const std::int32_t state = Sub_503160(opened, &gLegacyPetSkillCatalogPanel581FB8,
        +[] { static_cast<void>(Sub_4D7440()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* const action = gLegacyPetSkillCatalogActionD67C5C;
        world::detail::gValue581800 = -1;
        if (action != nullptr) {
            static_cast<void>(MarkDeath(action));
            gLegacyPetSkillCatalogActionD67C5C = nullptr;
        }
        return static_cast<std::uint32_t>(audio::PlaySe_52C450(57, 320));
    }
    YobiAction* const previewAction = gLegacyPetSkillCatalogActionD67C5C;
    if (previewAction != nullptr) {
        SetYobiActionFlag2(previewAction, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillCatalogPanel581FB8.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 8; ++row) {
        if (gLegacyPetSkillCatalogCA0478[row + 8 * gLegacyPetSkillCatalogPageD45F74].used != 0
            && Sub_5049A0(gLegacyPetSkillCatalogPanel581FB8.x + 18,
                gLegacyPetSkillCatalogPanel581FB8.y + 84 + 32 * row,
                gLegacyPetSkillCatalogPanel581FB8.x + 345,
                gLegacyPetSkillCatalogPanel581FB8.y + 105 + 32 * row, -1) != 0) {
            hovered = row;
            break;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1) != 0) {
        const std::int32_t index = hovered + 8 * gLegacyPetSkillCatalogPageD45F74;
        if (world::detail::ReadPlayerValueFC() < gLegacyPetSkillCatalogCA0478[index].price08) {
            gLegacyPetSkillCatalogMessageD2E818 = 1;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else if (world::detail::gPetCountDE0B00 == 0) {
            gLegacyPetSkillCatalogMessageD2E818 = 2;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            char buffer[16];
            char escaped[16];
            gLegacyPetSkillCatalogSelectionC8AF70 = hovered;
            platform::FormatLegacyString_53B304(buffer, "%d", index);
            static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            const auto socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
            static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillCatalogPanel581FB8.id));
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillCatalogPanel581FB8.id));
    } else if ((QueryPanelButtonState(&previous) & 0x10) != 0) {
        --gLegacyPetSkillCatalogPageD45F74;
        if (gLegacyPetSkillCatalogPageD45F74 < 0) {
            gLegacyPetSkillCatalogPageD45F74 = gLegacyPetSkillCatalogPageCountC8AF84 - 1;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else if ((QueryPanelButtonState(&next) & 0x10) != 0) {
        ++gLegacyPetSkillCatalogPageD45F74;
        if (gLegacyPetSkillCatalogPageD45F74 >= gLegacyPetSkillCatalogPageCountC8AF84) {
            gLegacyPetSkillCatalogPageD45F74 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 288 - titleWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 8),
        2, 0, 0, lines[0].text.data(), 0, 0));
    for (std::int32_t row = 0; row < 4; ++row) {
        const char* const text = lines[1 + 4 * gLegacyPetSkillCatalogMessageD2E818 + row].text.data();
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 377),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 59 + 20 * row),
            2, 0, 0, text, 0, 0));
    }
    char buffer[256];
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 286), 2, 0, 0, buffer, 0, 0));
    platform::FormatLegacyString_53B304(buffer, "%d/%d", gLegacyPetSkillCatalogPageD45F74 + 1, gLegacyPetSkillCatalogPageCountC8AF84);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 172),
        static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 44), 2, 0, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::int32_t index = row + 8 * gLegacyPetSkillCatalogPageD45F74;
        if (gLegacyPetSkillCatalogCA0478[index].used == 0) {
            continue;
        }
        const std::int32_t nameWidth = TextMeasure(gLegacyPetSkillCatalogCA0478[index].text0C.data(), 0);
        const char* const name = gLegacyPetSkillCatalogCA0478[row + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 97 - nameWidth / 2),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 86 + 32 * row), 2, 0, 0, name, 0, 0));
        const std::int32_t value = gLegacyPetSkillCatalogCA0478[row + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
        platform::FormatLegacyString_53B304(buffer, "%4d", value);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 203),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 88 + 32 * row), 2, 1, 0, buffer, 0, 0));
        const std::int32_t page = gLegacyPetSkillCatalogPageD45F74;
        const std::int32_t gold = world::detail::ReadPlayerValueFC();
        const std::int32_t price = gLegacyPetSkillCatalogCA0478[row + 8 * page].price08;
        const std::uint8_t color = gold >= price ? 0 : 6;
        platform::FormatLegacyString_53B304(buffer, "%7d", price);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 261),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 88 + 32 * row), 2, 1, color, buffer, 0, 0));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyPetSkillCatalogPanel581FB8.y + 32 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyPetSkillCatalogPanel581FB8.x;
        DrawGraphicHover(Low16(left + 18), Low16(top + 84), Low16(left + 345),
            Low16(top + 105), 214, color, 0);
        for (std::int32_t row = 0; row < 2; ++row) {
            const char* const text = gLegacyPetSkillCatalogCA0478[
                hovered + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data() + 25 + 29 * row;
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 70),
                static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 359 + 32 * row),
                2, 0, 0, text, 0, 0));
        }
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyPetSkillCatalogPanel581FB8) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetSkillCatalogCancel588CD0;
    } else if ((QueryPanelButtonState(&previous) & 0x40) != 0 || (QueryPanelButtonState(&next) & 0x40) != 0) {
        hint = kLegacyRecipePage586D2C;
    } else if (hovered >= 0) {
        hint = kLegacyPetSkillCatalogLearn588268;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetSkillCatalogPanel581FB8, 1);
}

// 原函数：0x004D7BF0。
YobiAction* Sub_4D7BF0()
{
    const std::int32_t y = gLegacyPetSkillPetPanel582080.y;
    const std::int32_t x = gLegacyPetSkillPetPanel582080.x;
    const std::int32_t image = gLegacyPetSkillCatalogImageCC3E64;
    YobiAction* const action = Sub_4D6640(image, x + 468, y + 245);
    gLegacyPetSkillPetActionD67C60 = action;
    return action;
}

// 原函数：0x004D7C20。
std::uint32_t Sub_4D7C20(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetSkillPetPanel582080.x;
    const std::int32_t y = gLegacyPetSkillPetPanel582080.y;
    LegacyPanelButtonRecord cancel {x + 465, y + 390, x + 425, y + 374, 80, 32, 23076, 23077};
    const std::int32_t state = Sub_503160(opened, &gLegacyPetSkillPetPanel582080,
        +[] { static_cast<void>(Sub_4D7BF0()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        YobiAction* const action = gLegacyPetSkillPetActionD67C60;
        world::detail::gValue581800 = -1;
        if (action != nullptr) {
            static_cast<void>(MarkDeath(action));
            gLegacyPetSkillPetActionD67C60 = nullptr;
        }
        return static_cast<std::uint32_t>(audio::PlaySe_52C450(57, 320));
    }
    YobiAction* const previewAction = gLegacyPetSkillPetActionD67C60;
    if (previewAction != nullptr) {
        SetYobiActionFlag2(previewAction, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillPetPanel582080.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto& option = ui::detail::gWindow54PetOptionsD67448[row];
        if (option.allowed != 0 && option.used != 0
            && Sub_5049A0(gLegacyPetSkillPetPanel582080.x + 70,
                gLegacyPetSkillPetPanel582080.y + 131 + 40 * row,
                gLegacyPetSkillPetPanel582080.x + 301,
                gLegacyPetSkillPetPanel582080.y + 152 + 40 * row, -1) != 0) {
            hovered = row;
            break;
        }
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1) != 0) {
        char buffer[16];
        char escaped[16];
        platform::FormatLegacyString_53B304(buffer, "%d", hovered);
        static_cast<void>(input::EscapeLegacyTextAndNotify_52D2D0(buffer, escaped, 15));
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
        const std::uint32_t panelId = gLegacyPetSkillPetPanel582080.id;
        gLegacyPetSkillPetSelectionC8A94C = hovered;
        static_cast<void>(audio::Sub_5042D0(panelId));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 2, input::LegacyText593850()));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillPetPanel582080.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 288 - titleWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 8), 2, 0, 0, lines[0].text.data(), 0, 0));
    const char* const measuredName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    const std::int32_t skillWidth = TextMeasure(measuredName, 0);
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 96 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 44), 2, 0, 0, skillName, 0, 0));
    char buffer[64];
    const std::int32_t skillValue = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
    platform::FormatLegacyString_53B304(buffer, "%4d", skillValue);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 202),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 46), 2, 1, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    platform::FormatLegacyString_53B304(buffer, "%7d", price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 260),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 46), 2, 0, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto& option = ui::detail::gWindow54PetOptionsD67448[row];
        if (option.used == 0) {
            continue;
        }
        const std::uint8_t color = option.allowed != 0 ? 0 : 7;
        const std::int32_t width = TextMeasure(option.name.data(), 0);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 147 - width / 2),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 133 + 40 * row),
            2, 0, color, option.name.data(), 0, 0));
        platform::FormatLegacyString_53B304(buffer, "%3d", option.value04);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 272),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 135 + 40 * row), 2, 1, color, buffer, 0, 0));
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 377),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 59 + 20 * row),
            2, 0, 0, lines[row + 1].text.data(), 0, 0));
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 286), 2, 0, 0, buffer, 0, 0));
    if (hovered >= 0) {
        const std::int32_t top = gLegacyPetSkillPetPanel582080.y + 40 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyPetSkillPetPanel582080.x;
        DrawGraphicHover(Low16(left + 70), Low16(top + 131), Low16(left + 301), Low16(top + 152), 214, color, 0);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyPetSkillPetPanel582080) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetSkillPetCancel588CE8;
    } else if (hovered >= 0) {
        hint = kLegacyPetSkillCatalogLearn588268;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetSkillPetPanel582080, 1);
}

// 原函数：0x004D8200。
YobiAction* Sub_4D8200()
{
    const std::int32_t y = gLegacyPetSkillSlotsPanel582148.y;
    const std::int32_t x = gLegacyPetSkillSlotsPanel582148.x;
    const std::int32_t image = gLegacyPetSkillCatalogImageCC3E64;
    YobiAction* const action = Sub_4D6640(image, x + 468, y + 245);
    gLegacyPetSkillSlotsActionD67C64 = action;
    return action;
}

// 原函数：0x004D8230。
std::uint32_t Sub_4D8230(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetSkillSlotsPanel582148.x;
    const std::int32_t y = gLegacyPetSkillSlotsPanel582148.y;
    LegacyPanelButtonRecord cancel {x + 465, y + 390, x + 425, y + 374, 80, 32, 23076, 23077};
    const std::int32_t state = Sub_503160(opened, &gLegacyPetSkillSlotsPanel582148,
        +[] { static_cast<void>(Sub_4D8200()); }, 1);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        if (audio::Sub_504410(61) == 0) {
            world::detail::gValue581800 = -1;
        }
        if (gLegacyPetSkillSlotsActionD67C64 != nullptr) {
            static_cast<void>(MarkDeath(gLegacyPetSkillSlotsActionD67C64));
            gLegacyPetSkillSlotsActionD67C64 = nullptr;
        }
        return static_cast<std::uint32_t>(audio::PlaySe_52C450(57, 320));
    }
    YobiAction* const previewAction = gLegacyPetSkillSlotsActionD67C64;
    if (previewAction != nullptr) {
        SetYobiActionFlag2(previewAction, false);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillSlotsPanel582148.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    std::int32_t selected = gLegacyPetSkillPetSelectionC8A94C;
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 10; ++row) {
        if (ui::detail::gWindow54PetOptionsD67448[selected].slots <= row) {
            continue;
        }
        if (Sub_5049A0(gLegacyPetSkillSlotsPanel582148.x + 60,
                gLegacyPetSkillSlotsPanel582148.y + 116 + 24 * row,
                gLegacyPetSkillSlotsPanel582148.x + 302,
                gLegacyPetSkillSlotsPanel582148.y + 136 + 24 * row, -1) != 0) {
            hovered = row;
            break;
        }
        selected = gLegacyPetSkillPetSelectionC8A94C;
    }
    if (hovered >= 0 && (static_cast<std::uint8_t>(gValueD68D9C) & 1) != 0) {
        gLegacyPetSkillSlotCF4B64 = hovered;
        static_cast<void>(audio::Sub_504180(61));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 2, input::LegacyText593850()));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillSlotsPanel582148.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 288 - titleWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 8), 2, 0, 0, lines[0].text.data(), 0, 0));
    const char* const measuredName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    const std::int32_t skillWidth = TextMeasure(measuredName, 0);
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 96 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 44), 2, 0, 0, skillName, 0, 0));
    char buffer[64];
    const std::int32_t skillValue = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
    platform::FormatLegacyString_53B304(buffer, "%4d", skillValue);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 202),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 46), 2, 1, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    platform::FormatLegacyString_53B304(buffer, "%7d", price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 260),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 46), 2, 0, 0, buffer, 0, 0));
    const std::int32_t petWidth = TextMeasure(
        ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data(), 0);
    const char* const petName = ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 142 - petWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 81), 2, 0, 0, petName, 0, 0));
    platform::FormatLegacyString_53B304(buffer, "%3d", ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].value04);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 267),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 83), 2, 1, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 10; ++row) {
        const auto& slot = ui::detail::gWindow54TechniqueSlotsD66E38[row];
        if (slot.used != 0) {
            const std::int32_t width = TextMeasure(slot.name.data(), 0);
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 156 - width / 2),
                static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 118 + 24 * row),
                2, 0, 0, slot.name.data(), 0, 0));
            platform::FormatLegacyString_53B304(buffer, "%4d", slot.value04);
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 260),
                static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 120 + 24 * row), 2, 1, 0, buffer, 0, 0));
        }
        if (ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].slots > row) {
            static_cast<void>(DrawGraphic(gLegacyPetSkillSlotsPanel582148.x + 181,
                gLegacyPetSkillSlotsPanel582148.y + 126 + 24 * row, 213, 23093, 0, nullptr));
        }
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 377),
            static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 59 + 20 * row),
            2, 0, 0, lines[row + 1].text.data(), 0, 0));
    }
    if (hovered >= 0) {
        const std::int32_t top = gLegacyPetSkillSlotsPanel582148.y + 24 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyPetSkillSlotsPanel582148.x;
        DrawGraphicHover(Low16(left + 60), Low16(top + 116), Low16(left + 302), Low16(top + 136), 214, color, 0);
        const auto& slot = ui::detail::gWindow54TechniqueSlotsD66E38[hovered];
        for (std::int32_t row = 0; row < 2; ++row) {
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 69),
                static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 369 + 20 * row),
                2, 0, 0, slot.description[row].data(), 0, 0));
        }
    }
    platform::FormatLegacyString_53B304(buffer, "%7d", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 286), 2, 0, 0, buffer, 0, 0));
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const char* hint = nullptr;
    if (Sub_503590(&gLegacyPetSkillSlotsPanel582148) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetSkillPetCancel588CE8;
    } else if (hovered >= 0) {
        hint = kLegacyPetSkillSlotHint588D00;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetSkillSlotsPanel582148, 1);
}

// 原函数：0x004D88C0。
std::uint32_t Sub_4D88C0(const std::int32_t opened)
{
    const std::int32_t x = gLegacyPetSkillConfirmPanel582210.x;
    const std::int32_t y = gLegacyPetSkillConfirmPanel582210.y;
    character_select::detail::CharacterModalUiRecord accept {
        x + 82, y + 200, x + 80, y + 200, 54, 18, kLegacyBarterYes588A08,
    };
    character_select::detail::CharacterModalUiRecord decline {
        x + 188, y + 200, x + 186, y + 200, 54, 18, kLegacyBarterNo588A00,
    };
    const std::int32_t state = Sub_503160(opened, &gLegacyPetSkillConfirmPanel582210, nullptr, 0);
    if (state == 0) {
        return 0;
    }
    if (state == 2) {
        const std::int32_t result = audio::Sub_504410(60);
        if (result == 0) {
            world::detail::gValue581800 = -1;
        }
        return static_cast<std::uint32_t>(result);
    }
    if (ui::detail::Sub_4D3040(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillConfirmPanel582210.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        char requestText[16];
        platform::FormatLegacyString_53B304(requestText, "%d", gLegacyPetSkillSlotCF4B64);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t playerX = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 0, requestText));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillConfirmPanel582210.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((character_select::detail::Sub_503990(&decline) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(60));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    char buffer[256];
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].text0C.data();
    platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillTitle588344, skillName);
    const std::int32_t skillWidth = TextMeasure(buffer, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 20), 2, 0, 0, buffer, 0, 0));
    const char* const petName = ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data();
    platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillPetName588338, petName);
    // 原4D8AEE测量固定示例文本，绘制仍使用实际名称，不能改成测量buffer。
    const std::int32_t petWidth = TextMeasure(kLegacyPetSkillPetMeasure588320, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - petWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 44), 2, 0, 0, buffer, 0, 0));
    const auto& slot = ui::detail::gWindow54TechniqueSlotsD66E38[gLegacyPetSkillSlotCF4B64];
    if (slot.used != 0) {
        platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillUsedSlot58830C, slot.name.data());
    } else {
        std::strcpy(buffer, kLegacyPetSkillEmptySlot5882F4);
    }
    const std::int32_t slotWidth = TextMeasure(buffer, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - slotWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 68), 2, 0, 0, buffer, 0, 0));
    const std::int32_t learnWidth = TextMeasure(kLegacyPetSkillLearnLine5882E8, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - learnWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 88),
        2, 0, 0, kLegacyPetSkillLearnLine5882E8, 0, 0));
    platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillGold588D5C, world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 85),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 120), 2, 0, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    platform::FormatLegacyString_53B304(buffer, kLegacyPetSkillPrice588D48, price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 85),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 140), 2, 0, 0, buffer, 0, 0));
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 92),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 172),
        2, 0, 0, kLegacyPetSkillQuestion55C9B0, 0, 0));
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&decline, 2, 4, 1, gValueD69768, 218));
    const char* hint = nullptr;
    if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacyPetSkillAccept588D34;
    } else if ((character_select::detail::Sub_503990(&decline) & 0x40) != 0) {
        hint = kLegacyPetSkillStop588D18;
    }
    if (hint != nullptr) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
    }
    return Sub_503440(&gLegacyPetSkillConfirmPanel582210, 0);
}


} // namespace cg::login::detail
