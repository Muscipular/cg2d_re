#include "GraphicDxCacheState.hpp"
#include "../LoginMessageDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "../ui/WindowRuntimeState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include "cg/YobiRuntime.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>

namespace cg::login::detail {

// 原D69768零填充W32槽；唯一生产在下方完整507140，既有消费者仍共用此符号。
std::uint32_t gValueD69768 = 0;

} // namespace cg::login::detail

namespace cg::platform {
namespace {

using namespace render;
namespace crypt = legacy_crypt_int;

std::uint32_t gHoverColorIndexD6978C = 0;
constexpr std::array<std::uint8_t, 60> kHoverColors58ACC8 {
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA,
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA,
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA,
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA,
    0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
};

// 原5071C6..507242：宽度留给命中后的绘框，四边各自读取CryptInt。
bool HitQueuedText(const TextPendingRecord& text, std::int32_t& width)
{
    width = login::detail::TextMeasure(text.text, text.fontType);
    if (!crypt::LessEqual_401DA0(&gLegacyMouseState.mouseX, text.x + width + 2)) {
        return false;
    }
    if (!crypt::IntLessEqualCrypt_401FC0(text.x - 2, &gLegacyMouseState.mouseX)) {
        return false;
    }
    const std::uint8_t fontType = text.fontType;
    const std::int32_t y = text.y;
    const std::int32_t height = gFontMetrics[fontType].doubleHeight;
    if (!crypt::LessEqual_401DA0(&gLegacyMouseState.mouseY, y + height + 2)) {
        return false;
    }
    return crypt::IntLessEqualCrypt_401FC0(text.y - 2, &gLegacyMouseState.mouseY);
}

// 原5072AD..50756F；调用方已经通过value4和真实图形加载门禁。
bool HitQueuedGraphic(const GraphicCacheItem32& graphic)
{
    using namespace login::detail;
    if (graphic.reference.action == nullptr) {
        const std::int32_t imageNo = graphic.imageNo;
        const DxGraphicEntry* const base = gGraphicDxCache103CFA4;
        const std::int32_t width = std::bit_cast<std::int16_t>(base[imageNo].width);
        const std::int32_t x = graphic.x;
        if (!crypt::LessEqual_401DA0(&gLegacyMouseState.mouseX, x + width)) {
            return false;
        }
        if (!crypt::IntLessEqualCrypt_401FC0(graphic.x, &gLegacyMouseState.mouseX)) {
            return false;
        }
        const std::int32_t heightImage = graphic.imageNo;
        const DxGraphicEntry* const heightBase = gGraphicDxCache103CFA4;
        const std::int32_t y = graphic.y;
        const std::int32_t height = std::bit_cast<std::int16_t>(heightBase[heightImage].height);
        if (!crypt::LessEqual_401DA0(&gLegacyMouseState.mouseY, y + height)) {
            return false;
        }
        return crypt::IntLessEqualCrypt_401FC0(graphic.y, &gLegacyMouseState.mouseY);
    }

    const std::int32_t scene = world::gCurrentSceneType;
    if (scene != 10 && scene != 3) {
        const login::YobiAction* const xAction = graphic.reference.action;
        const std::int32_t x = graphic.x;
        const std::int32_t centerX = x - ReadYobiGraphicOffsetX(xAction);
        if (ReadLegacyMouseX() > centerX + 16) {
            return false;
        }
        if (!crypt::IntLessEqualCrypt_401FC0(centerX - 16, &gLegacyMouseState.mouseX)) {
            return false;
        }
        const login::YobiAction* const upperAction = graphic.reference.action;
        const std::int32_t upperY = graphic.y;
        const std::int32_t upperOffset = ReadYobiGraphicOffsetY(upperAction);
        if (!crypt::LessEqual_401DA0(&gLegacyMouseState.mouseY, upperY - upperOffset + 12)) {
            return false;
        }
        const login::YobiAction* const lowerAction = graphic.reference.action;
        const std::int32_t lowerY = graphic.y;
        const std::int32_t lowerOffset = ReadYobiGraphicOffsetY(lowerAction);
        return crypt::IntLessEqualCrypt_401FC0(
            lowerY - lowerOffset - 12, &gLegacyMouseState.mouseY);
    }

    const std::int32_t imageNo = graphic.imageNo;
    const DxGraphicEntry* const base = gGraphicDxCache103CFA4;
    const std::int32_t height = std::bit_cast<std::int16_t>(base[imageNo].height);
    const std::int32_t hitHeight = height < 96 ? height : 96;
    const login::YobiAction* const xAction = graphic.reference.action;
    const std::int32_t x = graphic.x;
    const std::int32_t center = x - ReadYobiGraphicOffsetX(xAction);
    const double centerX = static_cast<double>(center);
    if (static_cast<double>(ReadLegacyMouseX()) > centerX + 24.0) {
        return false;
    }
    const double lowerX = centerX - 24.0;
    if (static_cast<double>(ReadLegacyMouseX()) < lowerX) {
        return false;
    }
    const std::int32_t upperImage = graphic.imageNo;
    const DxGraphicEntry* const upperBase = gGraphicDxCache103CFA4;
    const std::int32_t upperHeight = std::bit_cast<std::int16_t>(upperBase[upperImage].height);
    const std::int32_t upperY = graphic.y;
    if (!crypt::LessEqual_401DA0(&gLegacyMouseState.mouseY, upperHeight + upperY)) {
        return false;
    }
    const std::int32_t lowerImage = graphic.imageNo;
    const DxGraphicEntry* const lowerBase = gGraphicDxCache103CFA4;
    const std::int32_t lowerHeight = std::bit_cast<std::int16_t>(lowerBase[lowerImage].height);
    const std::int32_t lowerY = graphic.y;
    if (!crypt::IntLessEqualCrypt_401FC0(
            lowerHeight - hitHeight + lowerY, &gLegacyMouseState.mouseY)) {
        return false;
    }
    return (ReadYobiActionFlags(graphic.reference.action) & 0x02000000u) == 0;
}

// 原5075A3..5076B4；只由value4==2路径进入，不在这里重取排序层。
void DrawQueuedGraphicHover(const GraphicCacheItem32& graphic, const std::uint8_t layer)
{
    using namespace login::detail;
    std::int32_t x;
    std::int32_t y;
    std::uint32_t hover;
    const DxGraphicEntry* entry;
    std::uint8_t frameLayer;
    if (layer >= 0xE2) {
        const std::int32_t imageNo = graphic.imageNo;
        const DxGraphicEntry* const base = gGraphicDxCache103CFA4;
        hover = gValueD69768;
        x = graphic.x;
        entry = base + imageNo;
        y = graphic.y;
        frameLayer = 0xE4;
    } else if (layer >= 0xD4) {
        const std::int32_t imageNo = graphic.imageNo;
        const DxGraphicEntry* const base = gGraphicDxCache103CFA4;
        hover = gValueD69768;
        entry = base + imageNo;
        y = graphic.y;
        x = graphic.x;
        frameLayer = 0xDA;
    } else if (graphic.reference.action != nullptr) {
        const std::int32_t imageNo = graphic.imageNo;
        const DxGraphicEntry* const base = gGraphicDxCache103CFA4;
        hover = gValueD69768;
        y = graphic.y;
        entry = base + imageNo;
        x = graphic.x;
        const std::int32_t height = std::bit_cast<std::int16_t>(entry->height);
        const double scaledHeight = static_cast<double>(height)
            * static_cast<double>(GetYobiActionScaleY(graphic.reference.action));
        const std::int32_t bottom =
            static_cast<std::int32_t>(static_cast<std::int64_t>(scaledHeight)) + y + 2;
        const std::int32_t width = std::bit_cast<std::int16_t>(entry->width);
        const double scaledWidth = static_cast<double>(width)
            * static_cast<double>(GetYobiActionScaleX(graphic.reference.action));
        const std::int32_t right =
            static_cast<std::int32_t>(static_cast<std::int64_t>(scaledWidth)) + x + 2;
        DrawGraphicHover(
            static_cast<std::uint16_t>(x - 2), static_cast<std::uint16_t>(y - 2),
            static_cast<std::uint16_t>(right), static_cast<std::uint16_t>(bottom),
            0xCE, hover, 0);
        return;
    } else {
        const std::int32_t imageNo = graphic.imageNo;
        const DxGraphicEntry* const base = gGraphicDxCache103CFA4;
        hover = gValueD69768;
        y = graphic.y;
        entry = base + imageNo;
        x = graphic.x;
        frameLayer = 0xCE;
    }
    const std::int32_t height = std::bit_cast<std::int16_t>(entry->height);
    const std::int32_t width = std::bit_cast<std::int16_t>(entry->width);
    DrawGraphicHover(
        static_cast<std::uint16_t>(x - 2), static_cast<std::uint16_t>(y - 2),
        static_cast<std::uint16_t>(x + width + 2), static_cast<std::uint16_t>(y + height + 2),
        frameLayer, hover, 0);
}

} // namespace

void Sub_507140()
{
    using namespace render;
    using namespace login::detail;
    using namespace ui::detail;
    const std::int16_t initialCount = std::bit_cast<std::int16_t>(gPendingGraphicCount);
    std::int32_t orderIndex = static_cast<std::int32_t>(initialCount) - 1;
    const std::uint32_t oldColorIndex = gHoverColorIndexD6978C;
    const std::uint32_t colorIndex = oldColorIndex < 59 ? oldColorIndex + 1 : 0;
    gHoverColorIndexD6978C = colorIndex;
    const std::uint32_t color = kHoverColors58ACC8[colorIndex];
    const std::uint32_t focus = gWindowHasFocusCA0AB4;
    const bool hasFocus = focus != 0;
    gValueD69768 = color;
    gHoveredGraphicPendingIndex = -1;
    if (hasFocus) {
        gWindowHasFocusCA0AB4 = 0;
        return;
    }

    std::int32_t textIndex = 0;
    std::int32_t textCount = std::bit_cast<std::int32_t>(gPendingGraphicCount2);
    while (textIndex < textCount) {
        const TextPendingRecord& text = gTextPendingRecords[static_cast<std::size_t>(textIndex)];
        std::int32_t width;
        if (text.value7 != 0 && HitQueuedText(text, width)) {
            gHoveredTextPendingIndex = textIndex;
            const TextPendingRecord& selected =
                gTextPendingRecords[static_cast<std::size_t>(textIndex)];
            const std::uint8_t type = selected.value7;
            if (type == 2 || type == 3) {
                std::uint32_t marker;
                std::int32_t y;
                std::int32_t x;
                if (type == 2) {
                    marker = gValueD69768;
                    y = selected.y;
                    x = selected.x;
                } else {
                    y = selected.y;
                    x = selected.x;
                    marker = selected.marker;
                }
                const std::int32_t height = gFontMetrics[selected.fontType].doubleHeight;
                DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 3), static_cast<std::uint16_t>(y - 4),
                    static_cast<std::uint16_t>(x + width + 2), static_cast<std::uint16_t>(y + height + 4),
                    type == 2 ? 218 : 214, marker, type == 2 ? 0 : 1);
                SortTextures_520790();
            }
            const std::int32_t inputGate = world::detail::gValueD460C8;
            gHoveredGraphicPendingIndex = -1;
            const bool inputGateIsZero = inputGate == 0;
            *reinterpret_cast<std::uint8_t*>(&gLegacyMouseState.valueD68DB4) = 0xD4;
            if (inputGateIsZero && gValue58589C != 0) {
                gHoverObjectValueD68AC8 = 0;
            }
            return;
        }
        textCount = std::bit_cast<std::int32_t>(gPendingGraphicCount2);
        ++textIndex;
    }

    // 这里只重取遍历次数，排序槽位置继续用入口确定的orderIndex。
    std::int32_t remaining = std::bit_cast<std::int16_t>(gPendingGraphicCount) - 1;
    gHoveredTextPendingIndex = -1;
    while (remaining >= 0) {
        const std::int16_t graphicIndex =
            gLegacyPendingStorage.cache3[static_cast<std::size_t>(orderIndex)].cacheIndex;
        const GraphicCacheItem32& graphic =
            gLegacyPendingStorage.records[static_cast<std::size_t>(graphicIndex)];
        if (graphic.value4 != 0 && LoadGraphic_522570(graphic.imageNo) != 0
            && HitQueuedGraphic(graphic)) {
            gHoveredGraphicPendingIndex =
                gLegacyPendingStorage.cache3[static_cast<std::size_t>(orderIndex)].cacheIndex;
            if (graphic.value4 == 2) {
                const std::uint8_t layer =
                    gLegacyPendingStorage.cache3[static_cast<std::size_t>(orderIndex)].layer;
                DrawQueuedGraphicHover(graphic, layer);
            }
            const std::int32_t value4 = graphic.value4;
            if (value4 >= 3) {
                DrawMatchingGraphicHover_507090(value4);
            }
            SortTextures_520790();
            if (world::detail::gValueD460C8 == 0 && gValue58589C != 0) {
                DescribeHoveredYobi_507140(&graphic);
                if (StrStr_53BE40(gHoverTextD68AE8.data(), "[han]") != nullptr) {
                    gHoverObjectValueD68AC8 = 1;
                } else if (StrStr_53BE40(gHoverTextD68AE8.data(), "[kok]") != nullptr) {
                    gHoverObjectValueD68AC8 = 5;
                }
            }
            const std::uint8_t finalLayer =
                gLegacyPendingStorage.cache3[static_cast<std::size_t>(orderIndex)].layer;
            *reinterpret_cast<std::uint8_t*>(&gLegacyMouseState.valueD68DB4) = finalLayer;
            return;
        }
        --remaining;
        --orderIndex;
    }
    *reinterpret_cast<std::uint8_t*>(&gLegacyMouseState.valueD68DB4) = 5;
    gHoveredGraphicPendingIndex = -1;
    gHoverObjectValueD68AC8 = 0;
}

} // namespace cg::platform
