#include "cg/ui/HitTest.hpp"
#include "UiControlRecord.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"

#include "cg/LoginInput.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>
#include <cstdint>

namespace cg::ui {

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
    static_cast<void>(cg::render::Sub_45F090(
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
    cg::login::detail::DrawGraphicHover(
        (frameLeft & 0xFFFF),
        (frameTop & 0xFFFF),
        (frameRight & 0xFFFF),
        (frameBottom & 0xFFFF),
        renderLayer,
        std::bit_cast<std::uint32_t>(hoverFrameImageNo),
        hoverFrameMode == 2 ? 1 : 0);
}

} // namespace cg::ui
