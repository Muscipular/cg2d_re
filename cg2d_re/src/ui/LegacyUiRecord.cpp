#include "cg/ui/HitTest.hpp"
#include "UiControlRecord.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"

#include "cg/scene/login/LoginInput.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <bit>
#include <cstdint>

namespace cg::ui {

LegacyUiLayoutScope::LegacyUiLayoutScope()
    : graphicStart_(platform::gPendingGraphicCount),
      textStart_(platform::gPendingGraphicCount2)
{
    offsetX_ = (platform::gWindowWidth - 640) / 2;
    offsetY_ = (platform::gWindowHeight - 480) / 2;
}

LegacyUiLayoutScope::~LegacyUiLayoutScope()
{
    // 边框／实色矩形每个 W32 装有两个 W16 坐标，分别平移，不能跨半字进位。
    const auto shiftCoordinatePair = [](const std::int32_t value, const std::int32_t offset) {
        const std::uint32_t bits = std::bit_cast<std::uint32_t>(value);
        const auto low = static_cast<std::uint16_t>((bits & 0xFFFFU) + static_cast<std::uint32_t>(offset));
        const auto high = static_cast<std::uint16_t>((bits >> 16U) + static_cast<std::uint32_t>(offset));
        return std::bit_cast<std::int32_t>((static_cast<std::uint32_t>(high) << 16U) | low);
    };
    for (std::uint16_t index = graphicStart_; index < platform::gPendingGraphicCount; ++index) {
        auto& record = render::gLegacyPendingStorage.records[index];
        switch (record.batchType) {
        case 0:
        case 1:
        case 10:
        case 11:
        case 12:
            record.x += offsetX_;
            record.y += offsetY_;
            break;
        case 2:
        case 3:
            record.x = shiftCoordinatePair(record.x, offsetX_);
            record.y = shiftCoordinatePair(record.y, offsetY_);
            break;
        default:
            // 文本位置在独立队列；全屏背景直接覆盖窗口，不参与控件平移。
            break;
        }
    }
    for (std::uint32_t index = textStart_; index < platform::gPendingGraphicCount2; ++index) {
        auto& record = render::gTextPendingRecords[index];
        record.x = static_cast<std::int16_t>(record.x + offsetX_);
        record.y = static_cast<std::int16_t>(record.y + offsetY_);
    }
    offsetX_ = 0;
    offsetY_ = 0;
}

// 原函数：0x0045F100。 查询控件矩形命中的鼠标输入位图。
std::int32_t QueryUiControlInputState_45F100(
    const UiControlRecord* const controlRecord)
{
    if (controlRecord == nullptr) {
        return 0;
    }

    cg::login::LoginSceneControl32 hitBounds {};
    hitBounds.top = controlRecord->bounds.top;
    hitBounds.left = controlRecord->bounds.left;
    hitBounds.height = controlRecord->bounds.height;
    hitBounds.width = controlRecord->bounds.width;
    return cg::login::QueryLoginControlState_5036F0(&hitBounds);
}

// 原函数：0x0045EFD0。 按图像状态模式绘制底图及可选悬停边框。
void DrawUiControl_45EFD0(
    const UiControlRecord* const controlRecord,
    const std::uint8_t renderLayer,
    const std::int32_t hoverFrameMode,
    const std::int32_t hoverFrameImageNo,
    const std::int32_t imageStateMode)
{
    if (controlRecord == nullptr) {
        return;
    }

    std::int32_t selectedImageNo = controlRecord->normalImageNo;
    if (imageStateMode == 0) {
        const std::uint8_t inputState =
            static_cast<std::uint8_t>(QueryUiControlInputState_45F100(controlRecord));
        if ((inputState & 0x40U) != 0) {
            selectedImageNo = controlRecord->activeImageNo;
        }
    } else if (imageStateMode == 2) {
        selectedImageNo = controlRecord->activeImageNo;
    }

    const std::int32_t drawY = controlRecord->bounds.top;
    const std::int32_t drawX = controlRecord->bounds.left;
    static_cast<void>(cg::render::QueueOriginalPaletteGraphic_45F090(
        drawX,
        drawY,
        renderLayer,
        selectedImageNo,
        1,
        -1));

    if (hoverFrameMode == 0) {
        return;
    }

    const std::int32_t hitTop = controlRecord->bounds.top;
    const std::int32_t hitHeight = controlRecord->bounds.height;
    const std::int32_t hitLeft = controlRecord->bounds.left;
    const std::int32_t hitBottom = hitHeight + hitTop;
    const std::int32_t hitWidth = controlRecord->bounds.width;
    const std::int32_t hitRight = hitWidth + hitLeft;
    if (cg::ui::HitTestMouseRect_5049A0(
            hitLeft,
            hitTop,
            hitRight,
            hitBottom,
            -1)
        == 0) {
        return;
    }

    if (imageStateMode != 0) {
        return;
    }

    const std::int32_t controlTop = controlRecord->bounds.top;
    const std::int32_t controlLeft = controlRecord->bounds.left;
    const std::int32_t controlHeight =
        controlRecord->bounds.height;
    const std::int32_t frameBottom = (controlHeight + controlTop) + 2;
    const std::int32_t frameTop = controlTop - 2;
    const std::int32_t controlWidth = controlRecord->bounds.width;
    const std::int32_t frameRight = (controlWidth + controlLeft) + 2;
    const std::int32_t frameLeft = controlLeft - 2;
    cg::render::DrawGraphicHover(
        (frameLeft & 0xFFFF),
        (frameTop & 0xFFFF),
        (frameRight & 0xFFFF),
        (frameBottom & 0xFFFF),
        renderLayer,
        std::bit_cast<std::uint32_t>(hoverFrameImageNo),
        hoverFrameMode == 2 ? 1 : 0);
}

} // namespace cg::ui
