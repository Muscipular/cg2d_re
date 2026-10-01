#include "CharacterSelectSceneDependencies.hpp"
#include "LegacyPanelRuntime.hpp"
#include "LoginSceneDependencies.hpp"
#include "ui/WindowRuntimeState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/LoginInput.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/platform/Startup.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

using QueryStateFunction = std::int32_t (*)(const void* uiRecord);
using DrawBaseFunction = std::int32_t (*)(
    std::int32_t x,
    std::int32_t y,
    std::uint8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    std::int32_t value5);

constexpr std::size_t kXOffset = 0x00;
constexpr std::size_t kYOffset = 0x04;
constexpr std::size_t kWidthOffset = 0x08;
constexpr std::size_t kHeightOffset = 0x0C;
constexpr std::size_t kNormalImageOffset = 0x10;
constexpr std::size_t kActiveImageOffset = 0x14;

std::int32_t ReadRecordW32(
    const void* const uiRecord,
    const std::size_t offset)
{
    std::int32_t value;
    const auto* const bytes = static_cast<const std::byte*>(uiRecord);
    std::memcpy(&value, bytes + offset, sizeof(value));
    return value;
}

std::int32_t QueryLegacyUiRecordState_45F100(
    const void* const uiRecord)
{
    if (uiRecord == nullptr) {
        return 0;
    }

    cg::login::LoginSceneControl32 control {};
    control.top = ReadRecordW32(uiRecord, kYOffset);
    control.left = ReadRecordW32(uiRecord, kXOffset);
    control.height = ReadRecordW32(uiRecord, kHeightOffset);
    control.width = ReadRecordW32(uiRecord, kWidthOffset);
    return cg::login::QueryLoginControlState_5036F0(&control);
}

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right)
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(right));
}

std::uint16_t Low16(const std::int32_t value)
{
    return static_cast<std::uint16_t>(
        std::bit_cast<std::uint32_t>(value));
}

std::int32_t QueryCharacterSelectState(const void* const uiRecord)
{
    return cg::character_select::detail::Sub_45F100(
        static_cast<
            const cg::character_select::detail::CharacterSelectUiRecord*>(
                uiRecord));
}

std::int32_t QueryPanelButtonState(const cg::login::detail::LegacyPanelButtonRecord* const button)
{
    // 503990仅消费矩形；避免将图号尾部当作CharacterModalUiRecord的文本指针。
    const cg::character_select::detail::CharacterModalUiRecord rectangle {
        0, 0, button->left, button->top, button->width, button->height, nullptr,
    };
    return cg::character_select::detail::Sub_503990(&rectangle);
}

void DrawLegacyUiRecord_45EFD0(
    const void* const uiRecord,
    const std::uint8_t layer,
    const std::int32_t overlayMode,
    const std::int32_t overlayImageNo,
    const std::int32_t style,
    const QueryStateFunction queryState,
    const DrawBaseFunction drawBase)
{
    if (uiRecord == nullptr) {
        return;
    }

    std::int32_t imageNo = ReadRecordW32(uiRecord, kNormalImageOffset);
    if (style == 0) {
        const std::uint8_t input =
            static_cast<std::uint8_t>(queryState(uiRecord));
        if ((input & 0x40U) != 0) {
            imageNo = ReadRecordW32(uiRecord, kActiveImageOffset);
        }
    } else if (style == 2) {
        imageNo = ReadRecordW32(uiRecord, kActiveImageOffset);
    }

    const std::int32_t baseY = ReadRecordW32(uiRecord, kYOffset);
    const std::int32_t baseX = ReadRecordW32(uiRecord, kXOffset);
    static_cast<void>(drawBase(
        baseX,
        baseY,
        layer,
        imageNo,
        1,
        -1));

    if (overlayMode == 0) {
        return;
    }

    const std::int32_t hitY = ReadRecordW32(uiRecord, kYOffset);
    const std::int32_t hitHeight = ReadRecordW32(uiRecord, kHeightOffset);
    const std::int32_t hitX = ReadRecordW32(uiRecord, kXOffset);
    const std::int32_t hitBottom = AddW32(hitHeight, hitY);
    const std::int32_t hitWidth = ReadRecordW32(uiRecord, kWidthOffset);
    const std::int32_t hitRight = AddW32(hitWidth, hitX);
    if (cg::login::detail::Sub_5049A0(
            hitX,
            hitY,
            hitRight,
            hitBottom,
            -1)
        == 0) {
        return;
    }

    if (style != 0) {
        return;
    }

    const std::int32_t hoverY = ReadRecordW32(uiRecord, kYOffset);
    const std::int32_t hoverX = ReadRecordW32(uiRecord, kXOffset);
    const std::int32_t hoverHeight =
        ReadRecordW32(uiRecord, kHeightOffset);
    const std::int32_t hoverBottom = AddW32(
        AddW32(hoverHeight, hoverY),
        2);
    const std::int32_t hoverTop = AddW32(hoverY, -2);
    const std::int32_t hoverWidth = ReadRecordW32(uiRecord, kWidthOffset);
    const std::int32_t hoverRight = AddW32(
        AddW32(hoverWidth, hoverX),
        2);
    const std::int32_t hoverLeft = AddW32(hoverX, -2);
    cg::login::detail::DrawGraphicHover(
        Low16(hoverLeft),
        Low16(hoverTop),
        Low16(hoverRight),
        Low16(hoverBottom),
        layer,
        std::bit_cast<std::uint32_t>(overlayImageNo),
        overlayMode == 2 ? 1 : 0);
}

} // namespace

namespace cg::login::detail {

std::uint32_t gLegacyPanelHitIdD046C0 = 0;
LegacyPanelRecord gLegacySkillList585708 {
    0, 0, 272, 438, 18, {0x58DE, 0, 0, 0, 0}, {}, 0, 0, {}, {},
};
std::uint8_t gLegacySkillPendingD67C48 = 0;

namespace {

constexpr char kLegacySkillClose55331C[] =
    "\xC3\xF6\xB3\xAC\xB3\x6F\xAD\xD3\xB5\xF8\xB5\xA1\xA1\x43";
constexpr char kLegacySkillOpen5864C8[] =
    "\xA5\xB4\xB6\x7D\xB3\x6F\xAD\xD3\xA7\xDE\xAF\xE0\xAA\xBA\xA7\xDE"
    "\xAF\xE0\xB5\xF8\xB5\xA1\xA1\x43";

} // namespace

std::int32_t Sub_500370(const std::int32_t opened)
{
    std::uint8_t usedSlots = 0;
    const std::int32_t result = Sub_503160(opened, &gLegacySkillList585708, nullptr, 1);
    if (result == 0 || result == 2) {
        return result;
    }
    std::int32_t hovered = -1;
    for (std::int32_t row = 0; row < 15; ++row) {
        const std::int32_t top = gLegacySkillList585708.y;
        const std::int32_t bottom = row * 29 + top + 139;
        const std::int32_t y = row * 29 + top + 119;
        const std::int32_t left = gLegacySkillList585708.x;
        const std::int32_t right = left + 263;
        if (Sub_5049A0(left + 5, y, right, bottom, -1) != 0) {
            hovered = row;
            const std::uint8_t clicks = static_cast<std::uint8_t>(gValueD68D9C);
            if ((clicks & 1u) != 0 && world::detail::gSkillPairsF63370[row].used != 0) {
                static_cast<void>(audio::Sub_504180(19));
                ui::detail::gSkillPairIndexD67AD8 = static_cast<std::uint8_t>(row);
                gLegacySkillPendingD67C48 = 0;
            } else if ((clicks & 2u) != 0 && world::detail::gSkillPairsF63370[row].used != 0
                && world::detail::gValueDA3B20 == 0) {
                const std::int32_t skill = std::bit_cast<std::int16_t>(
                    world::detail::gSkillPairsF63370[row].index);
                const std::uintptr_t socket = platform::gConnectionSocket;
                static_cast<void>(network::Sub_512470(socket, skill, -1));
                world::detail::gValueDA3B20 = 1;
            }
            break;
        }
    }
    char buffer[128];
    const std::int32_t nameWidth = TextMeasure(world::detail::BorrowPlayerJobName(), 0);
    const std::int32_t nameTop = gLegacySkillList585708.y;
    const std::int32_t nameLeft = gLegacySkillList585708.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(nameLeft - nameWidth / 2 + 147),
        static_cast<std::int16_t>(nameTop + 45), 2, 0, 0,
        world::detail::BorrowPlayerJobName(), 0, 0));
    for (std::int32_t row = 0; row < 15; ++row) {
        const world::detail::SkillPairRecord* const pair = &world::detail::gSkillPairsF63370[row];
        if (pair->used == 0) {
            continue;
        }
        const std::int32_t skillIndex = std::bit_cast<std::int16_t>(pair->index);
        const world::detail::SkillRuntimeRecord* const skill = &world::detail::gSkillsF1BAFC[skillIndex];
        usedSlots = static_cast<std::uint8_t>(usedSlots + skill->requiredSlots19[0]);
        const std::int32_t skillTop = gLegacySkillList585708.y;
        const std::int32_t skillLeft = gLegacySkillList585708.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(skillLeft + 30),
            static_cast<std::int16_t>(skillTop + row * 29 + 120), 2, 0, 0,
            skill->name.data(), 0, 0));
        std::sprintf(buffer, "%2d", skill->level);
        const std::int32_t levelTop = gLegacySkillList585708.y;
        const std::int32_t levelLeft = gLegacySkillList585708.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(levelLeft + 203),
            static_cast<std::int16_t>(levelTop + row * 29 + 120), 2, 0, 0, buffer, 0, 0));
        const std::int32_t required = std::bit_cast<std::int8_t>(skill->requiredSlots19[0]);
        std::sprintf(buffer, "%d", required);
        const std::int32_t slotsTop = gLegacySkillList585708.y;
        const std::int32_t slotsLeft = gLegacySkillList585708.x;
        static_cast<void>(DrawText(static_cast<std::int16_t>(slotsLeft + 250),
            static_cast<std::int16_t>(slotsTop + row * 29 + 120), 2, 0, 0, buffer, 0, 0));
        const std::int32_t imageTop = gLegacySkillList585708.y;
        const std::int32_t imageLeft = gLegacySkillList585708.x;
        static_cast<void>(DrawGraphic(imageLeft + 134, row * 29 + imageTop + 129,
            212, 0x58E4, 0, nullptr));
        if (row == hovered) {
            const std::int32_t hoverTop = gLegacySkillList585708.y;
            const std::uint32_t image = gValueD69768;
            const std::int32_t hoverLeft = gLegacySkillList585708.x;
            DrawGraphicHover(
                static_cast<std::uint16_t>(hoverLeft + 5),
                static_cast<std::uint16_t>(row * 29 + hoverTop + 119),
                static_cast<std::uint16_t>(hoverLeft + 263),
                static_cast<std::uint16_t>(row * 29 + hoverTop + 139),
                214, image, 0);
        }
    }
    const std::int32_t signedUsed = std::bit_cast<std::int8_t>(usedSlots);
    std::sprintf(buffer, "%2d", 10 - signedUsed);
    const std::int32_t footerTop = gLegacySkillList585708.y;
    const std::int32_t footerLeft = gLegacySkillList585708.x;
    static_cast<void>(DrawText(static_cast<std::int16_t>(footerLeft + 156),
        static_cast<std::int16_t>(footerTop + 73), 2, 0, 0, buffer, 0, 0));
    if (Sub_503590(&gLegacySkillList585708) != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillClose55331C);
    } else if (hovered >= 0 && world::detail::gSkillPairsF63370[hovered].used != 0) {
        std::strcpy(ui::detail::gHoverTextD68AE8.data(), kLegacySkillOpen5864C8);
    }
    return std::bit_cast<std::int32_t>(Sub_503440(&gLegacySkillList585708, 1));
}

std::int32_t Sub_5030A0(LegacyPanelTextButtonRecord* const records, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        LegacyPanelTextButtonRecord* const record = &records[index];
        const std::int32_t state = character_select::detail::Sub_503990(&record->label);
        if ((state & 0x40) != 0) {
            const char* const hint = record->hint;
            if (hint != nullptr) {
                std::strcpy(ui::detail::gHoverTextD68AE8.data(), hint);
            }
        }
        if ((state & 0x10) != 0) {
            auto const callback = record->callback;
            if (callback != nullptr && callback() != 0) {
                return 1;
            }
        }
    }
    return 0;
}

void Sub_503120(LegacyPanelTextButtonRecord* const records, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        const std::uint32_t hoverImage = gValueD69768;
        const std::int8_t color = std::bit_cast<std::int8_t>(records[index].color);
        static_cast<void>(character_select::detail::Sub_503A20(
            &records[index].label, 2, color, 1, hoverImage, 214));
    }
}

std::int32_t Sub_503160(
    const std::int32_t opened, LegacyPanelRecord* const panel,
    void (*const initialize)(), const std::int32_t flags)
{
    const std::int32_t right = panel->x + panel->width;
    const std::int32_t top = panel->y;
    panel->closeButton.normalImage = 22509;
    panel->closeButton.pressedImage = 22510;
    panel->closeButton.left = right - 26;
    panel->closeButton.imageX = right - 16;
    panel->closeButton.width = 20;
    panel->closeButton.height = 20;
    const std::uint32_t id = panel->id;
    panel->closeButton.imageY = top + 16;
    panel->closeButton.top = top + 6;
    const std::int32_t index = audio::Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    YobiAction*& actionSlot = audio::BorrowLegacyPanelAction57C374(index);
    if (opened == 0) {
        if (actionSlot != nullptr) {
            static_cast<void>(MarkDeath(actionSlot));
            actionSlot = nullptr;
        }
        std::uint8_t& dirty = audio::BorrowLegacyPanelDirty57C378(index);
        if (dirty != 0) {
            dirty = 0;
            return 2;
        }
        return 0;
    }
    if (actionSlot == nullptr) {
        const std::int32_t background = panel->backgrounds[0];
        std::int32_t x;
        std::int32_t y;
        std::int32_t width;
        const std::int32_t height = panel->height;
        const std::int32_t style = static_cast<std::uint32_t>(background) <= 999 ? background : -2;
        if (static_cast<std::uint32_t>(background) > 999) {
            width = panel->width;
            y = panel->y;
            x = panel->x;
        } else {
            y = panel->y;
            width = panel->width;
            x = panel->x;
        }
        actionSlot = Sub_5044E0(x, y, width, height, style, 1);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        if (initialize != nullptr) {
            initialize();
        }
    }
    YobiAction* const action = actionSlot;
    if (action == nullptr || IsYobiActionStateLessOrEqual(action, 0)) {
        return 0;
    }
    const std::int32_t firstTabImage = panel->tabs[0].image;
    panel->hovered = -1;
    if (firstTabImage != 0) {
        for (std::int32_t tabIndex = 0; tabIndex < 5; ++tabIndex) {
            if (panel->selected == tabIndex) {
                continue;
            }
            LegacyPanelTabRecord* const tab = &panel->tabs[tabIndex];
            const std::int32_t tabY = tab->y;
            const std::int32_t tabHeight = tab->height;
            const std::int32_t panelY = panel->y;
            const std::int32_t tabX = tab->x;
            const std::int32_t panelX = panel->x;
            const std::int32_t bottom = panelY + tabY + tabHeight;
            const std::int32_t tabRight = panelX + tabX + tab->width;
            if (Sub_5049A0(panelX + tabX, panelY + tabY, tabRight, bottom, -1) != 0
                && tab->image != 0) {
                panel->hovered = static_cast<std::int8_t>(tabIndex);
                const char* const hint = tab->hint;
                if (hint != nullptr) {
                    char buffer[128];
                    std::sprintf(buffer, "%s", hint);
                    std::strcpy(ui::detail::gHoverTextD68AE8.data(), buffer);
                }
                if ((gValueD68D9C & 1u) != 0) {
                    panel->selected = static_cast<std::int8_t>(tabIndex);
                    static_cast<void>(audio::PlaySe_52C450(52, 320));
                    break;
                }
            }
        }
    }
    const std::int32_t panelY = panel->y;
    const std::int32_t panelHeight = panel->height;
    const std::int32_t panelX = panel->x;
    const std::int32_t bottom = panelHeight + panelY + 1;
    const std::int32_t panelRight = panelX + panel->width + 1;
    if (Sub_5049A0(panelX, panelY, panelRight, bottom, -1) != 0
        && (QueryPanelButtonState(&panel->closeButton) & 0x40) == 0) {
        gLegacyPanelHitIdD046C0 = panel->id;
    }
    if ((flags & 1) != 0 && (QueryPanelButtonState(&panel->closeButton) & 1) != 0) {
        static_cast<void>(audio::Sub_5042D0(panel->id));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

std::uint32_t Sub_503440(LegacyPanelRecord* const panel, const std::int32_t flags)
{
    const std::int32_t hovered = panel->hovered;
    GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (hovered >= 0) {
        const std::uint32_t hoverImage = gValueD69768;
        const std::int32_t panelX = panel->x;
        const LegacyPanelTabRecord* const tab = &panel->tabs[hovered];
        const std::int32_t tabY = tab->y;
        const std::int32_t panelY = panel->y;
        const std::int32_t tabX = tab->x;
        const std::int32_t tabHeight = tab->height;
        const std::int32_t tabWidth = tab->width;
        DrawGraphicHover(
            static_cast<std::uint16_t>(panelX + tabX - 3),
            static_cast<std::uint16_t>(panelY + tabY - 3),
            static_cast<std::uint16_t>(panelX + tabWidth + tabX + 4),
            static_cast<std::uint16_t>(panelY + tabY + tabHeight + 3),
            214, hoverImage, 0);
    }
    for (std::int32_t index = 0; index < 5; ++index) {
        if (panel->selected != index) {
            const std::int32_t image = panel->tabs[index].image;
            const std::int32_t height = panel->height;
            const std::int32_t top = panel->y;
            const std::int32_t y = height / 2 + top;
            const std::int32_t width = panel->width;
            const std::int32_t left = panel->x;
            const std::int32_t x = width / 2 + left;
            static_cast<void>(DrawGraphic(x, y, 218, image, 0, &descriptor));
        }
    }
    if ((flags & 1) != 0) {
        const std::uint32_t hoverImage = gValueD69768;
        Sub_5035E0(&panel->closeButton, 218, 1, hoverImage, 0);
    }
    const std::int32_t selected = panel->selected;
    const std::int32_t image = panel->backgrounds[selected];
    if (static_cast<std::uint32_t>(image) <= 999) {
        return static_cast<std::uint32_t>(image);
    }
    const std::int32_t height = panel->height;
    const std::int32_t top = panel->y;
    const std::int32_t y = height / 2 + top;
    const std::int32_t width = panel->width;
    const std::int32_t left = panel->x;
    const std::int32_t x = width / 2 + left;
    return static_cast<std::uint32_t>(DrawGraphic(x, y, 212, image, 1, &descriptor));
}

std::int32_t Sub_503590(LegacyPanelRecord* const panel)
{
    const std::int32_t y = panel->y;
    const std::int32_t height = panel->height;
    const std::int32_t x = panel->x;
    const std::int32_t bottom = height + y + 1;
    const std::int32_t right = panel->width + x + 1;
    if (Sub_5049A0(x, y, right, bottom, -1) != 0
        && (QueryPanelButtonState(&panel->closeButton) & 0x40) != 0) {
        return 1;
    }
    return 0;
}

void Sub_5035E0(
    LegacyPanelButtonRecord* const button, const std::uint8_t layer,
    const std::int32_t hoverMode, const std::uint32_t hoverImage, const std::int32_t style)
{
    GraphicDescriptor16 descriptor {};
    const std::int32_t top = button->top;
    const std::int32_t height = button->height;
    const std::int32_t left = button->left;
    const std::int32_t bottom = top + height;
    const std::int32_t right = left + button->width;
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    const bool pressed = Sub_5049A0(left, top, right, bottom, -1) != 0
        && (gValueD68D98 & 1u) != 0 && style == 0;
    const std::int32_t image = pressed ? button->pressedImage : button->normalImage;
    const std::int32_t y = button->imageY;
    const std::int32_t x = button->imageX;
    static_cast<void>(DrawGraphic(x, y, layer, image, 1, &descriptor));
    if (hoverMode != 0) {
        const std::int32_t hoverTop = button->top;
        const std::int32_t hoverLeft = button->left;
        const std::int32_t hoverBottom = hoverTop + button->height;
        const std::int32_t hoverRight = button->width + hoverLeft;
        if (Sub_5049A0(hoverLeft, hoverTop, hoverRight, hoverBottom, -1) != 0 && style == 0) {
            const std::int32_t frameTop = button->top;
            const std::int32_t frameLeft = button->left;
            const std::int32_t frameBottom = frameTop + button->height + 2;
            const std::int32_t frameRight = button->width + frameLeft + 2;
            DrawGraphicHover(
                static_cast<std::uint16_t>(frameLeft - 2),
                static_cast<std::uint16_t>(frameTop - 2),
                static_cast<std::uint16_t>(frameRight),
                static_cast<std::uint16_t>(frameBottom),
                layer, hoverImage, hoverMode == 2 ? 1 : 0);
        }
    }
}

std::int32_t Sub_45F100(const void* const uiRecord)
{
    return QueryLegacyUiRecordState_45F100(uiRecord);
}

void Sub_45EFD0(
    const void* const uiRecord,
    const std::uint8_t layer,
    const std::int32_t overlayMode,
    const std::int32_t overlayImageNo,
    const std::int32_t style)
{
    DrawLegacyUiRecord_45EFD0(
        uiRecord,
        layer,
        overlayMode,
        overlayImageNo,
        style,
        Sub_45F100,
        Sub_45F090);
}

} // namespace cg::login::detail

namespace cg::character_select::detail {

std::int32_t Sub_45F100(
    const CharacterSelectUiRecord* const uiRecord)
{
    return QueryLegacyUiRecordState_45F100(uiRecord);
}

void Sub_45EFD0(
    const CharacterSelectUiRecord* const uiRecord,
    const std::uint8_t layer,
    const std::int32_t overlayMode,
    const std::int32_t overlayImageNo,
    const std::int32_t style)
{
    DrawLegacyUiRecord_45EFD0(
        uiRecord,
        layer,
        overlayMode,
        overlayImageNo,
        style,
        QueryCharacterSelectState,
        Sub_45F090);
}

} // namespace cg::character_select::detail
