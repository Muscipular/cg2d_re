#include "../CharacterSelectSceneDependencies.hpp"
#include "../LoginSceneDependencies.hpp"

#include "cg/LoginInput.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace {

// 查询控件鼠标输入位图的场景回调。
using QueryUiControlInputStateFunction = std::int32_t (*)(const void* controlRecord);
// 绘制控件底图；graphicValue4沿用底层未证语义参数，color为颜色参数。
using DrawUiControlGraphicFunction = std::int32_t (*)(
    std::int32_t x,
    std::int32_t y,
    std::uint8_t renderLayer,
    std::int32_t imageNo,
    std::int32_t graphicValue4,
    std::int32_t color);

// 共用控件记录前24字节：矩形、普通图号和激活图号。
constexpr std::size_t kLeftOffset = 0x00;
constexpr std::size_t kTopOffset = 0x04;
constexpr std::size_t kWidthOffset = 0x08;
constexpr std::size_t kHeightOffset = 0x0C;
constexpr std::size_t kNormalImageNoOffset = 0x10;
constexpr std::size_t kActiveImageNoOffset = 0x14;

// 按已证字段偏移读取有符号32位值，保留逐次memcpy取样。
std::int32_t ReadUiControlInt32(
    const void* const controlRecord,
    const std::size_t fieldOffset)
{
    std::int32_t fieldValue;
    const auto* const recordBytes = static_cast<const std::byte*>(controlRecord);
    std::memcpy(&fieldValue, recordBytes + fieldOffset, sizeof(fieldValue));
    return fieldValue;
}

// 原函数：0x0045F100。 查询控件矩形命中的鼠标输入位图。
std::int32_t QueryUiControlInputStateFromRecord_45F100(
    const void* const controlRecord)
{
    if (controlRecord == nullptr) {
        return 0;
    }

    cg::login::LoginSceneControl32 hitBounds {};
    hitBounds.top = ReadUiControlInt32(controlRecord, kTopOffset);
    hitBounds.left = ReadUiControlInt32(controlRecord, kLeftOffset);
    hitBounds.height = ReadUiControlInt32(controlRecord, kHeightOffset);
    hitBounds.width = ReadUiControlInt32(controlRecord, kWidthOffset);
    return cg::login::QueryLoginControlState_5036F0(&hitBounds);
}

// 将角色选择的具名记录适配为共享输入查询回调。
std::int32_t QueryCharacterSelectControlInputState(const void* const controlRecord)
{
    return cg::character_select::detail::QueryUiControlInputState_45F100(
        static_cast<
            const cg::character_select::detail::CharacterSelectUiRecord*>(
                controlRecord));
}


// 原函数：0x0045EFD0。 按图像状态模式绘制底图及可选悬停边框。
void DrawUiControlWithCallbacks_45EFD0(
    const void* const controlRecord,
    const std::uint8_t renderLayer,
    const std::int32_t hoverFrameMode,
    const std::int32_t hoverFrameImageNo,
    const std::int32_t imageStateMode,
    const QueryUiControlInputStateFunction queryInputState,
    const DrawUiControlGraphicFunction drawBaseGraphic)
{
    if (controlRecord == nullptr) {
        return;
    }

    std::int32_t selectedImageNo = ReadUiControlInt32(controlRecord, kNormalImageNoOffset);
    if (imageStateMode == 0) {
        const std::uint8_t inputState =
            static_cast<std::uint8_t>(queryInputState(controlRecord));
        if ((inputState & 0x40U) != 0) {
            selectedImageNo = ReadUiControlInt32(controlRecord, kActiveImageNoOffset);
        }
    } else if (imageStateMode == 2) {
        selectedImageNo = ReadUiControlInt32(controlRecord, kActiveImageNoOffset);
    }

    const std::int32_t drawY = ReadUiControlInt32(controlRecord, kTopOffset);
    const std::int32_t drawX = ReadUiControlInt32(controlRecord, kLeftOffset);
    static_cast<void>(drawBaseGraphic(
        drawX,
        drawY,
        renderLayer,
        selectedImageNo,
        1,
        -1));

    if (hoverFrameMode == 0) {
        return;
    }

    const std::int32_t hitTop = ReadUiControlInt32(controlRecord, kTopOffset);
    const std::int32_t hitHeight = ReadUiControlInt32(controlRecord, kHeightOffset);
    const std::int32_t hitLeft = ReadUiControlInt32(controlRecord, kLeftOffset);
    const std::int32_t hitBottom = hitHeight + hitTop;
    const std::int32_t hitWidth = ReadUiControlInt32(controlRecord, kWidthOffset);
    const std::int32_t hitRight = hitWidth + hitLeft;
    if (cg::login::detail::Sub_5049A0(
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

    const std::int32_t controlTop = ReadUiControlInt32(controlRecord, kTopOffset);
    const std::int32_t controlLeft = ReadUiControlInt32(controlRecord, kLeftOffset);
    const std::int32_t controlHeight =
        ReadUiControlInt32(controlRecord, kHeightOffset);
    const std::int32_t frameBottom = (controlHeight + controlTop) + 2;
    const std::int32_t frameTop = controlTop - 2;
    const std::int32_t controlWidth = ReadUiControlInt32(controlRecord, kWidthOffset);
    const std::int32_t frameRight = (controlWidth + controlLeft) + 2;
    const std::int32_t frameLeft = controlLeft - 2;
    cg::login::detail::DrawGraphicHover(
        static_cast<std::uint16_t>(
            std::bit_cast<std::uint32_t>(frameLeft)),
        static_cast<std::uint16_t>(
            std::bit_cast<std::uint32_t>(frameTop)),
        static_cast<std::uint16_t>(
            std::bit_cast<std::uint32_t>(frameRight)),
        static_cast<std::uint16_t>(
            std::bit_cast<std::uint32_t>(frameBottom)),
        renderLayer,
        std::bit_cast<std::uint32_t>(hoverFrameImageNo),
        hoverFrameMode == 2 ? 1 : 0);
}

} // namespace

namespace cg::login::detail {

// 原函数：0x0045F100。 查询控件矩形命中的鼠标输入位图。
std::int32_t QueryUiControlInputState_45F100(const void* const controlRecord)
{
    return QueryUiControlInputStateFromRecord_45F100(controlRecord);
}

// 原函数：0x0045EFD0。 按图像状态模式绘制底图及可选悬停边框。
void DrawUiControl_45EFD0(
    const void* const controlRecord,
    const std::uint8_t renderLayer,
    const std::int32_t hoverFrameMode,
    const std::int32_t hoverFrameImageNo,
    const std::int32_t imageStateMode)
{
    DrawUiControlWithCallbacks_45EFD0(
        controlRecord,
        renderLayer,
        hoverFrameMode,
        hoverFrameImageNo,
        imageStateMode,
        QueryUiControlInputState_45F100,
        Sub_45F090);
}

} // namespace cg::login::detail

namespace cg::character_select::detail {

// 原函数：0x0045F100。 查询控件矩形命中的鼠标输入位图。
std::int32_t QueryUiControlInputState_45F100(
    const CharacterSelectUiRecord* const controlRecord)
{
    return QueryUiControlInputStateFromRecord_45F100(controlRecord);
}

// 原函数：0x0045EFD0。 按图像状态模式绘制底图及可选悬停边框。
void DrawUiControl_45EFD0(
    const CharacterSelectUiRecord* const controlRecord,
    const std::uint8_t renderLayer,
    const std::int32_t hoverFrameMode,
    const std::int32_t hoverFrameImageNo,
    const std::int32_t imageStateMode)
{
    DrawUiControlWithCallbacks_45EFD0(
        controlRecord,
        renderLayer,
        hoverFrameMode,
        hoverFrameImageNo,
        imageStateMode,
        QueryCharacterSelectControlInputState,
        Sub_45F090);
}

} // namespace cg::character_select::detail
