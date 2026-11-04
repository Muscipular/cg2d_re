#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../render/GraphicDxCacheState.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "MainLoopDependencies.hpp"
#include "../ui/WindowRuntimeState.hpp"

#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <cstring> // 原CRT接口：0x0053BE40；原参数／返回合同由直接调用承接。

namespace cg::platform {

static constexpr std::array<std::uint8_t, 60> kHoverColors58ACC8 { // 原常量参考：0x0058ACC8。
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA,
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA,
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA,
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA,
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
};

// 原5071C6..507242：宽度留给命中后的绘框，四边各自读取CryptInt。
static bool HitQueuedText(session::ClientSession& session, const render::TextPendingRecord& text, std::int32_t& width)
{
    if (!IsSessionInputTarget(session)) return false;
    width = cg::render::TextMeasure(text.text.c_str(), text.fontType);
    if (!(session.UiFrame().mouse.state->mouseX <= static_cast<std::int32_t>(text.x + width + 2))) {
        return false;
    }
    if (!(static_cast<std::int32_t>(text.x - 2) <= session.UiFrame().mouse.state->mouseX)) {
        return false;
    }
    const std::uint8_t fontType = text.fontType;
    const std::int32_t y = text.y;
    const std::int32_t height = render::GetFontMetrics(fontType).doubleHeight;
    if (!(session.UiFrame().mouse.state->mouseY <= static_cast<std::int32_t>(y + height + 2))) {
        return false;
    }
    return (static_cast<std::int32_t>(text.y - 2) <= session.UiFrame().mouse.state->mouseY);
}

// 原5072AD..50756F；调用方已经通过value4和真实图形加载门禁。
static bool HitQueuedGraphic(session::ClientSession& session, const render::GraphicCacheItem32& graphic)
{
    if (!IsSessionInputTarget(session)) return false;
    if (graphic.reference.action == nullptr) {
        const std::int32_t imageNo = graphic.imageNo;
        const render::GraphicDxCache* const base = render::gGraphicDxCache103CFA4;
        const std::int32_t width = std::bit_cast<std::int16_t>(base->Read(imageNo).width);
        const std::int32_t x = graphic.x;
        if (!(session.UiFrame().mouse.state->mouseX <= static_cast<std::int32_t>(x + width))) {
            return false;
        }
        if (!(static_cast<std::int32_t>(graphic.x) <= session.UiFrame().mouse.state->mouseX)) {
            return false;
        }
        const std::int32_t heightImage = graphic.imageNo;
        const render::GraphicDxCache* const heightBase = render::gGraphicDxCache103CFA4;
        const std::int32_t y = graphic.y;
        const std::int32_t height = std::bit_cast<std::int16_t>(heightBase->Read(heightImage).height);
        if (!(session.UiFrame().mouse.state->mouseY <= static_cast<std::int32_t>(y + height))) {
            return false;
        }
        return (static_cast<std::int32_t>(graphic.y) <= session.UiFrame().mouse.state->mouseY);
    }

    const std::int32_t scene = session.WorldScene().sceneType;
    if (scene != 10 && scene != 3) {
        const cg::yobi::YobiAction* const xAction = graphic.reference.action;
        const std::int32_t x = graphic.x;
        const std::int32_t centerX = x - cg::yobi::ReadYobiGraphicOffsetX(xAction);
        if (static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX) > centerX + 16) {
            return false;
        }
        if (!(static_cast<std::int32_t>(centerX - 16) <= session.UiFrame().mouse.state->mouseX)) {
            return false;
        }
        const cg::yobi::YobiAction* const upperAction = graphic.reference.action;
        const std::int32_t upperY = graphic.y;
        const std::int32_t upperOffset = cg::yobi::ReadYobiGraphicOffsetY(upperAction);
        if (!(session.UiFrame().mouse.state->mouseY <= static_cast<std::int32_t>(upperY - upperOffset + 12))) {
            return false;
        }
        const cg::yobi::YobiAction* const lowerAction = graphic.reference.action;
        const std::int32_t lowerY = graphic.y;
        const std::int32_t lowerOffset = cg::yobi::ReadYobiGraphicOffsetY(lowerAction);
        return (static_cast<std::int32_t>(lowerY - lowerOffset - 12) <= session.UiFrame().mouse.state->mouseY);
    }

    const std::int32_t imageNo = graphic.imageNo;
    const render::GraphicDxCache* const base = render::gGraphicDxCache103CFA4;
    const std::int32_t height = std::bit_cast<std::int16_t>(base->Read(imageNo).height);
    const std::int32_t hitHeight = height < 96 ? height : 96;
    const cg::yobi::YobiAction* const xAction = graphic.reference.action;
    const std::int32_t x = graphic.x;
    const std::int32_t center = x - cg::yobi::ReadYobiGraphicOffsetX(xAction);
    const double centerX = center;
    if (static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX) > centerX + 24.0) {
        return false;
    }
    const double lowerX = centerX - 24.0;
    if (static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX) < lowerX) {
        return false;
    }
    const std::int32_t upperImage = graphic.imageNo;
    const render::GraphicDxCache* const upperBase = render::gGraphicDxCache103CFA4;
    const std::int32_t upperHeight = std::bit_cast<std::int16_t>(upperBase->Read(upperImage).height);
    const std::int32_t upperY = graphic.y;
    if (!(session.UiFrame().mouse.state->mouseY <= static_cast<std::int32_t>(upperHeight + upperY))) {
        return false;
    }
    const std::int32_t lowerImage = graphic.imageNo;
    const render::GraphicDxCache* const lowerBase = render::gGraphicDxCache103CFA4;
    const std::int32_t lowerHeight = std::bit_cast<std::int16_t>(lowerBase->Read(lowerImage).height);
    const std::int32_t lowerY = graphic.y;
    if (!(static_cast<std::int32_t>(lowerHeight - hitHeight + lowerY) <= session.UiFrame().mouse.state->mouseY)) {
        return false;
    }
    return (cg::yobi::ReadYobiActionFlags(graphic.reference.action) & 0x02000000u) == 0;
}

// 原5075A3..5076B4；只由value4==2路径进入，不在这里重取排序层。
static void DrawQueuedGraphicHover(session::ClientSession& session,
    const render::GraphicCacheItem32& graphic, const std::uint8_t layer)
{
    std::int32_t x;
    std::int32_t y;
    std::uint32_t hover;
    const render::DxGraphicEntry* entry;
    std::uint8_t frameLayer;
    if (layer >= 0xE2) {
        const std::int32_t imageNo = graphic.imageNo;
        const render::GraphicDxCache* const base = render::gGraphicDxCache103CFA4;
        hover = session.UiFrame().hoverFramePaletteIndex;
        x = graphic.x;
        entry = &base->Read(imageNo);
        y = graphic.y;
        frameLayer = 0xE4;
    } else if (layer >= 0xD4) {
        const std::int32_t imageNo = graphic.imageNo;
        const render::GraphicDxCache* const base = render::gGraphicDxCache103CFA4;
        hover = session.UiFrame().hoverFramePaletteIndex;
        entry = &base->Read(imageNo);
        y = graphic.y;
        x = graphic.x;
        frameLayer = 0xDA;
    } else if (graphic.reference.action != nullptr) {
        const std::int32_t imageNo = graphic.imageNo;
        const render::GraphicDxCache* const base = render::gGraphicDxCache103CFA4;
        hover = session.UiFrame().hoverFramePaletteIndex;
        y = graphic.y;
        entry = &base->Read(imageNo);
        x = graphic.x;
        const std::int32_t height = std::bit_cast<std::int16_t>(entry->height);
        const double scaledHeight = static_cast<double>(height)
            * cg::yobi::GetYobiActionScaleY(graphic.reference.action);
        const std::int32_t bottom =
            static_cast<std::int32_t>(static_cast<std::int64_t>(scaledHeight)) + y + 2;
        const std::int32_t width = std::bit_cast<std::int16_t>(entry->width);
        const double scaledWidth = static_cast<double>(width)
            * cg::yobi::GetYobiActionScaleX(graphic.reference.action);
        const std::int32_t right =
            static_cast<std::int32_t>(static_cast<std::int64_t>(scaledWidth)) + x + 2;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(x - 2), static_cast<std::uint16_t>(y - 2),
            static_cast<std::uint16_t>(right), static_cast<std::uint16_t>(bottom),
            0xCE, hover, 0);
        return;
    } else {
        const std::int32_t imageNo = graphic.imageNo;
        const render::GraphicDxCache* const base = render::gGraphicDxCache103CFA4;
        hover = session.UiFrame().hoverFramePaletteIndex;
        y = graphic.y;
        entry = &base->Read(imageNo);
        x = graphic.x;
        frameLayer = 0xCE;
    }
    const std::int32_t height = std::bit_cast<std::int16_t>(entry->height);
    const std::int32_t width = std::bit_cast<std::int16_t>(entry->width);
    cg::render::DrawGraphicHover(
        static_cast<std::uint16_t>(x - 2), static_cast<std::uint16_t>(y - 2),
        static_cast<std::uint16_t>(x + width + 2), static_cast<std::uint16_t>(y + height + 2),
        frameLayer, hover, 0);
}

// 原函数：0x00507140。
void UpdateQueuedTextAndGraphicHover_507140(session::ClientSession& session)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& uiFrame = session.UiFrame();
    const std::int16_t initialCount = std::bit_cast<std::int16_t>(cg::render::gGraphicQueue.count);
    std::int32_t orderIndex = static_cast<std::int32_t>(initialCount) - 1;
    const std::uint32_t oldColorIndex = uiFrame.hoverColorIndex;
    const std::uint32_t colorIndex = oldColorIndex < 59 ? oldColorIndex + 1 : 0;
    uiFrame.hoverColorIndex = colorIndex;
    const std::uint32_t color = kHoverColors58ACC8[colorIndex];
    const std::uint32_t focus = uiFrame.windowHasFocus;
    const bool hasFocus = focus != 0;
    session.UiFrame().hoverFramePaletteIndex = color;
    session.UiFrame().hoveredGraphicPendingIndex = -1;
    if (hasFocus) {
        uiFrame.windowHasFocus = 0;
        return;
    }

    std::int32_t textIndex = 0;
    std::int32_t textCount = std::bit_cast<std::int32_t>(cg::render::gTextQueue.count);
    while (textIndex < textCount) {
        const render::TextPendingRecord& text = cg::render::gTextQueue.records[static_cast<std::size_t>(textIndex)];
        std::int32_t width;
        if (text.value7 != 0 && HitQueuedText(session, text, width)) {
            session.UiFrame().hoveredTextPendingIndex = textIndex;
            const render::TextPendingRecord& selected =
                cg::render::gTextQueue.records[static_cast<std::size_t>(textIndex)];
            const std::uint8_t type = selected.value7;
            if (type == 2 || type == 3) {
                std::uint32_t marker;
                std::int32_t y;
                std::int32_t x;
                if (type == 2) {
                    marker = session.UiFrame().hoverFramePaletteIndex;
                    y = selected.y;
                    x = selected.x;
                } else {
                    y = selected.y;
                    x = selected.x;
                    marker = selected.marker;
                }
                const std::int32_t height = render::GetFontMetrics(selected.fontType).doubleHeight;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 3), static_cast<std::uint16_t>(y - 4),
                    static_cast<std::uint16_t>(x + width + 2), static_cast<std::uint16_t>(y + height + 4),
                    type == 2 ? 218 : 214, marker, type == 2 ? 0 : 1);
                render::SortTextures_520790();
            }
            const std::int32_t inputGate = uiFrame.inputAndHoverSuppressed;
            session.UiFrame().hoveredGraphicPendingIndex = -1;
            const bool inputGateIsZero = inputGate == 0;
            *reinterpret_cast<std::uint8_t*>(&session.UiFrame().mouse.state->valueD68DB4) = 0xD4;
            if (inputGateIsZero && interaction.hoverDescriptionEnabled != 0) {
                interaction.hoverObjectValue = 0;
            }
            return;
        }
        textCount = std::bit_cast<std::int32_t>(cg::render::gTextQueue.count);
        ++textIndex;
    }

    // 这里只重取遍历次数，排序槽位置继续用入口确定的orderIndex。
    std::int32_t remaining = std::bit_cast<std::int16_t>(cg::render::gGraphicQueue.count) - 1;
    session.UiFrame().hoveredTextPendingIndex = -1;
    while (remaining >= 0) {
        const std::int16_t graphicIndex =
            cg::render::gGraphicQueue.pending.cache3[static_cast<std::size_t>(orderIndex)].cacheIndex;
        const render::GraphicCacheItem32& graphic =
            cg::render::gGraphicQueue.pending.records[static_cast<std::size_t>(graphicIndex)];
        if (graphic.value4 != 0 && render::LoadGraphic_522570(graphic.imageNo) != 0
            && HitQueuedGraphic(session, graphic)) {
            session.UiFrame().hoveredGraphicPendingIndex =
                cg::render::gGraphicQueue.pending.cache3[static_cast<std::size_t>(orderIndex)].cacheIndex;
            if (graphic.value4 == 2) {
                const std::uint8_t layer =
                    cg::render::gGraphicQueue.pending.cache3[static_cast<std::size_t>(orderIndex)].layer;
                DrawQueuedGraphicHover(session, graphic, layer);
            }
            const std::int32_t value4 = graphic.value4;
            if (value4 >= 3) {
                render::DrawMatchingGraphicHover_507090(session, value4);
            }
            render::SortTextures_520790();
            if (uiFrame.inputAndHoverSuppressed == 0 && interaction.hoverDescriptionEnabled != 0) {
                cg::yobi::DescribeHoveredYobi_507140(session, &graphic);
                if (uiFrame.hoverText.find(L"[han]") != std::wstring::npos) {
                    interaction.hoverObjectValue = 1;
                } else if (uiFrame.hoverText.find(L"[kok]") != std::wstring::npos) {
                    interaction.hoverObjectValue = 5;
                }
            }
            const std::uint8_t finalLayer =
                cg::render::gGraphicQueue.pending.cache3[static_cast<std::size_t>(orderIndex)].layer;
            *reinterpret_cast<std::uint8_t*>(&session.UiFrame().mouse.state->valueD68DB4) = finalLayer;
            return;
        }
        --remaining;
        --orderIndex;
    }
    *reinterpret_cast<std::uint8_t*>(&session.UiFrame().mouse.state->valueD68DB4) = 5;
    session.UiFrame().hoveredGraphicPendingIndex = -1;
    interaction.hoverObjectValue = 0;
}

} // namespace cg::platform
