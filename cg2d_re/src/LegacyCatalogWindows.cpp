#include "CharacterSelectSceneDependencies.hpp"
#include "cg/text/Encoding.hpp"
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
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstring>
#include <string>

#include "LegacyPanelTexts.hpp"
#include "LoginStateDependencies.hpp"

namespace cg::login::detail {

std::int32_t gLegacyPetSkillCatalogImageCC3E64 = 0; // 原全局：0x00CC3E64。
std::int32_t gLegacyPetSkillCatalogMessageD2E818 = 0; // 原全局：0x00D2E818。
std::int32_t gLegacyPetSkillCatalogPageCountC8AF84 = 0; // 原全局：0x00C8AF84。
std::int32_t gLegacyPetSkillCatalogPageD45F74 = 0; // 原全局：0x00D45F74。
std::int32_t gLegacyPetSkillCatalogSelectionC8AF70 = 0; // 原全局：0x00C8AF70。
std::int32_t gLegacyPetSkillPetSelectionC8A94C = 0; // 原全局：0x00C8A94C。
std::int32_t gLegacyTradeTargetCountD30134 = 0; // 原全局：0x00D30134。
std::int32_t gLegacyTradeTargetPageCC3208 = 0; // 原全局：0x00CC3208。
std::int32_t gLegacyTradeTargetPageCountCF46D0 = 0; // 原全局：0x00CF46D0。

namespace {

YobiAction* gLegacyPetSkillCatalogActionD67C5C = nullptr; // 原全局：0x00D67C5C。
LegacyPanelRecord gLegacyPetSkillCatalogPanel581FB8 { // 原全局：0x00581FB8。
    0, 0, 576, 432, 58, {23090, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyPetSkillConfirmPanel582210 { // 原全局：0x00582210。
    160, 108, 320, 240, 61, {}, {}, 0, 0, {}, {},
};
YobiAction* gLegacyPetSkillPetActionD67C60 = nullptr; // 原全局：0x00D67C60。
LegacyPanelRecord gLegacyPetSkillPetPanel582080 { // 原全局：0x00582080。
    0, 0, 576, 432, 59, {23091, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyPetSkillSlotCF4B64 = 0; // 原全局：0x00CF4B64。
YobiAction* gLegacyPetSkillSlotsActionD67C64 = nullptr; // 原全局：0x00D67C64。
LegacyPanelRecord gLegacyPetSkillSlotsPanel582148 { // 原全局：0x00582148。
    0, 0, 576, 432, 60, {23092, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
LegacyPanelRecord gLegacyTradeTargetPanel582E08 { // 原全局：0x00582E08。
    160, 132, 320, 192, 51, {}, {}, 0, 0, {}, {},
};
std::int32_t gLegacyTradeTargetSelectionCC4D48 = 0; // 原全局：0x00CC4D48。
constexpr wchar_t kLegacyPetSkillAccept588D34[] = L"讓寵物學習技能。"; // 原常量参考：0x00588D34。
constexpr wchar_t kLegacyPetSkillCatalogCancel588CD0[] = L"取消讓寵物學習技能。"; // 原常量参考：0x00588CD0。
constexpr wchar_t kLegacyPetSkillCatalogLearn588268[] = L"讓寵物學習此技能。"; // 原常量参考：0x00588268。
constexpr wchar_t kLegacyPetSkillEmptySlot5882F4[] = L"使用『空欄位』的位置"; // 原常量参考：0x005882F4。
constexpr wchar_t kLegacyPetSkillGold588D5C[] = L"持有金額　 {:7} 金幣"; // 原常量参考：0x00588D5C。
constexpr wchar_t kLegacyPetSkillLearnLine5882E8[] = L"來學習。"; // 原常量参考：0x005882E8。
constexpr wchar_t kLegacyPetSkillPetCancel588CE8[] = L"取消讓寵物學習此技能。"; // 原常量参考：0x00588CE8。
constexpr wchar_t kLegacyPetSkillPetMeasure588320[] = L"讓『1234567890123456』"; // 原常量参考：0x00588320。
constexpr wchar_t kLegacyPetSkillPetName588338[] = L"讓『{}』"; // 原常量参考：0x00588338。
constexpr wchar_t kLegacyPetSkillPrice588D48[] = L"購買金額   {:7} 金幣"; // 原常量参考：0x00588D48。
constexpr wchar_t kLegacyPetSkillQuestion55C9B0[] = L"可以嗎？"; // 原常量参考：0x0055C9B0。
constexpr wchar_t kLegacyPetSkillSlotHint588D00[] = L"使用此欄來學習技能。"; // 原常量参考：0x00588D00。
constexpr wchar_t kLegacyPetSkillStop588D18[] = L"停止使用此欄來學習技能。"; // 原常量参考：0x00588D18。
constexpr wchar_t kLegacyPetSkillTitle588344[] = L"學習『{}』"; // 原常量参考：0x00588344。
constexpr wchar_t kLegacyPetSkillUsedSlot58830C[] = L"使用『{}』的位置"; // 原常量参考：0x0058830C。
constexpr wchar_t kLegacyTradeTargetHint589368[] = L"與這個角色進行交易。"; // 原常量参考：0x00589368。

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
        static_cast<void>(ui::detail::WindowTrade_ReceiveStart(name));
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
        std::wstring buffer;
        cg::text::FormatTo(buffer, L"{}/{}", currentPage + 1, pageCount);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.x + 270),
            static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.y + 24), 2, 0, 0, buffer, 0, 0));
    }
    page = gLegacyTradeTargetPageCC3208;
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t index = row + 5 * page;
        if (index < gLegacyTradeTargetCountD30134) {
            const char* const name = gLegacyTradeTargetNamesCB1D1C[index].data();
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.x + 88),
                static_cast<std::int16_t>(gLegacyTradeTargetPanel582E08.y + 52 + 20 * row), 2, 0, 0, cg::text::FromGbkWide(name).c_str(), 0, 0));
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
        ui::detail::gHoverTextD68AE8 = kLegacyTradeTargetHint589368;
    } else if ((cancelState & 0x40) != 0) {
        ui::detail::gHoverTextD68AE8 = kLegacyTradeCancel5889AC;
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
    if (ui::detail::WindowProfile_CheckDistance(2, 2) != 0) {
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
            std::wstring buffer;
            std::wstring escaped;
            gLegacyPetSkillCatalogSelectionC8AF70 = hovered;
            cg::text::FormatTo(buffer, L"{}", index);
            escaped = input::EscapeTextAndNotify(buffer, 15);
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t playerY = world::detail::BorrowPlayerYStorage()->value();
            const std::int32_t playerX = world::detail::BorrowPlayerXStorage()->value();
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
        const std::wstring& text = lines[1 + 4 * gLegacyPetSkillCatalogMessageD2E818 + row].text;
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 377),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 59 + 20 * row),
            2, 0, 0, text, 0, 0));
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:7}", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 286), 2, 0, 0, buffer, 0, 0));
    cg::text::FormatTo(buffer, L"{}/{}", gLegacyPetSkillCatalogPageD45F74 + 1, gLegacyPetSkillCatalogPageCountC8AF84);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 172),
        static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 44), 2, 0, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::int32_t index = row + 8 * gLegacyPetSkillCatalogPageD45F74;
        if (gLegacyPetSkillCatalogCA0478[index].used == 0) {
            continue;
        }
        const std::int32_t nameWidth = TextMeasure(cg::text::FromGbkWide(gLegacyPetSkillCatalogCA0478[index].name.data()).c_str(), 0);
        const char* const name = gLegacyPetSkillCatalogCA0478[row + 8 * gLegacyPetSkillCatalogPageD45F74].name.data();
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 97 - nameWidth / 2),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 86 + 32 * row), 2, 0, 0, cg::text::FromGbkWide(name).c_str(), 0, 0));
        const std::int32_t value = gLegacyPetSkillCatalogCA0478[row + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
        cg::text::FormatTo(buffer, L"{:4}", value);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 203),
            static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 88 + 32 * row), 2, 1, 0, buffer, 0, 0));
        const std::int32_t page = gLegacyPetSkillCatalogPageD45F74;
        const std::int32_t gold = world::detail::ReadPlayerValueFC();
        const std::int32_t price = gLegacyPetSkillCatalogCA0478[row + 8 * page].price08;
        const std::uint8_t color = gold >= price ? 0 : 6;
        cg::text::FormatTo(buffer, L"{:7}", price);
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
                hovered + 8 * gLegacyPetSkillCatalogPageD45F74].description[row].data();
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.x + 70),
                static_cast<std::int16_t>(gLegacyPetSkillCatalogPanel581FB8.y + 359 + 32 * row),
                2, 0, 0, cg::text::FromGbkWide(text).c_str(), 0, 0));
        }
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    Sub_5035E0(&previous, 213, 1, gValueD69768, 0);
    Sub_5035E0(&next, 213, 1, gValueD69768, 0);
    const wchar_t* hint = nullptr;
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
        ui::detail::gHoverTextD68AE8 = hint;
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
    if (ui::detail::WindowProfile_CheckDistance(2, 2) != 0) {
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
        std::wstring buffer;
        std::wstring escaped;
        cg::text::FormatTo(buffer, L"{}", hovered);
        escaped = input::EscapeTextAndNotify(buffer, 15);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t playerX = world::detail::BorrowPlayerXStorage()->value();
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(socket, playerX, playerY, request, target, 0, escaped));
        const std::uint32_t panelId = gLegacyPetSkillPetPanel582080.id;
        gLegacyPetSkillPetSelectionC8A94C = hovered;
        static_cast<void>(audio::Sub_5042D0(panelId));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else if ((QueryPanelButtonState(&cancel) & 1) != 0) {
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t playerX = world::detail::BorrowPlayerXStorage()->value();
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 2, cg::text::FromGbkWide(input::LegacyText593850())));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillPetPanel582080.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 288 - titleWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 8), 2, 0, 0, lines[0].text.data(), 0, 0));
    const char* const measuredName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].name.data();
    const std::int32_t skillWidth = TextMeasure(cg::text::FromGbkWide(measuredName).c_str(), 0);
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].name.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 96 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 44), 2, 0, 0, cg::text::FromGbkWide(skillName).c_str(), 0, 0));
    std::wstring buffer;
    const std::int32_t skillValue = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
    cg::text::FormatTo(buffer, L"{:4}", skillValue);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 202),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 46), 2, 1, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    cg::text::FormatTo(buffer, L"{:7}", price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 260),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 46), 2, 0, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto& option = ui::detail::gWindow54PetOptionsD67448[row];
        if (option.used == 0) {
            continue;
        }
        const std::uint8_t color = option.allowed != 0 ? 0 : 7;
        const std::int32_t width = TextMeasure(cg::text::FromGbkWide(option.name.data()).c_str(), 0);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 147 - width / 2),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 133 + 40 * row),
            2, 0, color, cg::text::FromGbkWide(option.name.data()).c_str(), 0, 0));
        cg::text::FormatTo(buffer, L"{:3}", option.value04);
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 272),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 135 + 40 * row), 2, 1, color, buffer, 0, 0));
    }
    for (std::int32_t row = 0; row < 4; ++row) {
        static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 377),
            static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 59 + 20 * row),
            2, 0, 0, lines[row + 1].text.data(), 0, 0));
    }
    cg::text::FormatTo(buffer, L"{:7}", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillPetPanel582080.y + 286), 2, 0, 0, buffer, 0, 0));
    if (hovered >= 0) {
        const std::int32_t top = gLegacyPetSkillPetPanel582080.y + 40 * hovered;
        const std::uint32_t color = gValueD69768;
        const std::int32_t left = gLegacyPetSkillPetPanel582080.x;
        DrawGraphicHover(Low16(left + 70), Low16(top + 131), Low16(left + 301), Low16(top + 152), 214, color, 0);
    }
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const wchar_t* hint = nullptr;
    if (Sub_503590(&gLegacyPetSkillPetPanel582080) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetSkillPetCancel588CE8;
    } else if (hovered >= 0) {
        hint = kLegacyPetSkillCatalogLearn588268;
    }
    if (hint != nullptr) {
        ui::detail::gHoverTextD68AE8 = hint;
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
    if (ui::detail::WindowProfile_CheckDistance(2, 2) != 0) {
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
        const std::int32_t playerY = world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t playerX = world::detail::BorrowPlayerXStorage()->value();
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 2, cg::text::FromGbkWide(input::LegacyText593850())));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillSlotsPanel582148.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    const auto* const lines = ui::detail::BorrowNpcDialogLinesCC24D4();
    const std::int32_t titleWidth = TextMeasure(lines[0].text.data(), 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 288 - titleWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 8), 2, 0, 0, lines[0].text.data(), 0, 0));
    const char* const measuredName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].name.data();
    const std::int32_t skillWidth = TextMeasure(cg::text::FromGbkWide(measuredName).c_str(), 0);
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].name.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 96 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 44), 2, 0, 0, cg::text::FromGbkWide(skillName).c_str(), 0, 0));
    std::wstring buffer;
    const std::int32_t skillValue = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].value04;
    cg::text::FormatTo(buffer, L"{:4}", skillValue);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 202),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 46), 2, 1, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    cg::text::FormatTo(buffer, L"{:7}", price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 260),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 46), 2, 0, 0, buffer, 0, 0));
    const std::int32_t petWidth = TextMeasure(
        cg::text::FromGbkWide(ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data()).c_str(), 0);
    const char* const petName = ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data();
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 142 - petWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 81), 2, 0, 0, cg::text::FromGbkWide(petName).c_str(), 0, 0));
    cg::text::FormatTo(buffer, L"{:3}", ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].value04);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 267),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 83), 2, 1, 0, buffer, 0, 0));
    for (std::int32_t row = 0; row < 10; ++row) {
        const auto& slot = ui::detail::gWindow54TechniqueSlotsD66E38[row];
        if (slot.used != 0) {
            const std::int32_t width = TextMeasure(cg::text::FromGbkWide(slot.name.data()).c_str(), 0);
            static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 156 - width / 2),
                static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 118 + 24 * row),
                2, 0, 0, cg::text::FromGbkWide(slot.name.data()).c_str(), 0, 0));
            cg::text::FormatTo(buffer, L"{:4}", slot.value04);
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
                2, 0, 0, cg::text::FromGbkWide(slot.description[row].data()).c_str(), 0, 0));
        }
    }
    cg::text::FormatTo(buffer, L"{:7}", world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.x + 465),
        static_cast<std::int16_t>(gLegacyPetSkillSlotsPanel582148.y + 286), 2, 0, 0, buffer, 0, 0));
    Sub_5035E0(&cancel, 213, 1, gValueD69768, 0);
    const wchar_t* hint = nullptr;
    if (Sub_503590(&gLegacyPetSkillSlotsPanel582148) != 0) {
        hint = kLegacyLogout553FD0;
    } else if ((QueryPanelButtonState(&cancel) & 0x40) != 0) {
        hint = kLegacyPetSkillPetCancel588CE8;
    } else if (hovered >= 0) {
        hint = kLegacyPetSkillSlotHint588D00;
    }
    if (hint != nullptr) {
        ui::detail::gHoverTextD68AE8 = hint;
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
    if (ui::detail::WindowProfile_CheckDistance(2, 2) != 0) {
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillConfirmPanel582210.id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if ((character_select::detail::Sub_503990(&accept) & 1) != 0) {
        std::wstring requestText;
        cg::text::FormatTo(requestText, L"{}", gLegacyPetSkillSlotCF4B64);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t playerY = world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t playerX = world::detail::BorrowPlayerXStorage()->value();
        const auto socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_512620(
            socket, playerX, playerY, request, target, 0, requestText));
        static_cast<void>(audio::Sub_5042D0(gLegacyPetSkillConfirmPanel582210.id));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    } else if ((character_select::detail::Sub_503990(&decline) & 1) != 0) {
        static_cast<void>(audio::Sub_504180(60));
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    std::wstring buffer;
    const char* const skillName = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].name.data();
    cg::text::FormatTo(buffer, kLegacyPetSkillTitle588344, cg::text::FromGbkWide(skillName).c_str());
    const std::int32_t skillWidth = TextMeasure(buffer, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - skillWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 20), 2, 0, 0, buffer, 0, 0));
    const char* const petName = ui::detail::gWindow54PetOptionsD67448[gLegacyPetSkillPetSelectionC8A94C].name.data();
    cg::text::FormatTo(buffer, kLegacyPetSkillPetName588338, cg::text::FromGbkWide(petName).c_str());
    // 原4D8AEE测量固定示例文本，绘制仍使用实际名称，不能改成测量buffer。
    const std::int32_t petWidth = TextMeasure(kLegacyPetSkillPetMeasure588320, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - petWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 44), 2, 0, 0, buffer, 0, 0));
    const auto& slot = ui::detail::gWindow54TechniqueSlotsD66E38[gLegacyPetSkillSlotCF4B64];
    if (slot.used != 0) {
        cg::text::FormatTo(buffer, kLegacyPetSkillUsedSlot58830C, cg::text::FromGbkWide(slot.name.data()).c_str());
    } else {
        buffer = kLegacyPetSkillEmptySlot5882F4;
    }
    const std::int32_t slotWidth = TextMeasure(buffer, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - slotWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 68), 2, 0, 0, buffer, 0, 0));
    const std::int32_t learnWidth = TextMeasure(kLegacyPetSkillLearnLine5882E8, 0);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 160 - learnWidth / 2),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 88),
        2, 0, 0, kLegacyPetSkillLearnLine5882E8, 0, 0));
    cg::text::FormatTo(buffer, kLegacyPetSkillGold588D5C, world::detail::ReadPlayerValueFC());
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 85),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 120), 2, 0, 0, buffer, 0, 0));
    const std::int32_t price = gLegacyPetSkillCatalogCA0478[
        gLegacyPetSkillCatalogSelectionC8AF70 + 8 * gLegacyPetSkillCatalogPageD45F74].price08;
    cg::text::FormatTo(buffer, kLegacyPetSkillPrice588D48, price);
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 85),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 140), 2, 0, 0, buffer, 0, 0));
    static_cast<void>(DrawText(static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.x + 92),
        static_cast<std::int16_t>(gLegacyPetSkillConfirmPanel582210.y + 172),
        2, 0, 0, kLegacyPetSkillQuestion55C9B0, 0, 0));
    static_cast<void>(character_select::detail::Sub_503A20(&accept, 2, 4, 1, gValueD69768, 218));
    static_cast<void>(character_select::detail::Sub_503A20(&decline, 2, 4, 1, gValueD69768, 218));
    const wchar_t* hint = nullptr;
    if ((character_select::detail::Sub_503990(&accept) & 0x40) != 0) {
        hint = kLegacyPetSkillAccept588D34;
    } else if ((character_select::detail::Sub_503990(&decline) & 0x40) != 0) {
        hint = kLegacyPetSkillStop588D18;
    }
    if (hint != nullptr) {
        ui::detail::gHoverTextD68AE8 = hint;
    }
    return Sub_503440(&gLegacyPetSkillConfirmPanel582210, 0);
}


} // namespace cg::login::detail
