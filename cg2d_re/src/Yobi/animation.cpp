#include "animation.hpp"
#include "base.hpp"
#include "../AnimationRuntimeState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/AnimationRuntime.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/world/SceneState.hpp"

#include <cstdint>
#include <cstring>

namespace cg::login {

namespace detail {

namespace {

// 原两播放入口的同源翻转段；只有真实图形加载非零后才按需读取各个 DX 尺寸。
void ApplyYobiGraphicFlip(YobiAction* const action, const std::int32_t loadGraphicIndex)
{
    if (render::LoadGraphic_522570(loadGraphicIndex) == 0) {
        return;
    }
    const std::uint8_t mode = action->drawMode.bytes[0x0D];
    if ((mode & 1u) != 0) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const auto* const entries = render::gGraphicDxCache103CFA4;
        const std::int32_t offsetX = action->graphicOffsetX;
        const std::int16_t width = render::ReadGraphicDxWidthFromEntries(entries, graphicIndex);
        action->graphicOffsetX = -(offsetX + width);
    }
    if ((mode & 2u) != 0) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const auto* const entries = render::gGraphicDxCache103CFA4;
        const std::int16_t height = render::ReadGraphicDxHeightFromEntries(entries, graphicIndex);
        const std::int32_t offsetY = action->graphicOffsetY;
        action->graphicOffsetY = -(offsetY + height);
    }
}

// 原 517A8C..517C85 / 518098..518291 的相同阶段；不合并两入口的图号或速率门禁。
void ApplyYobiAnimeFrame(
    YobiAction* const action,
    const std::int32_t animeIndex,
    const resource::animation::detail::ActionEntry* const selected,
    const resource::animation::detail::FrameEntry* const frames)
{
    const std::uint16_t flag = frames[action->frameIndex].flag;
    if (flag != 0) {
        if (flag >= 10000) {
            action->frameEvent = flag;
        } else if (world::gCurrentSceneType != 9 || (action->flags & 0x80200u) == 0) {
            static_cast<void>(audio::PlaySe_52C450(flag, action->x));
        }
    }
    const auto* const graphicFrame = &frames[action->frameIndex];
    action->drawMode.bytes[0x0D] &= 0xFCu;
    action->graphicIndex = static_cast<std::int32_t>(graphicFrame->graphicIndex);
    std::int16_t offsetX;
    std::int16_t offsetY;
    resource::GetGraphicXY(graphicFrame->graphicIndex, &offsetX, &offsetY);
    const auto* const offsetFrame = &frames[action->frameIndex];
    const std::int32_t graphicOffsetX = offsetX;
    const std::int32_t frameOffsetX = offsetFrame->x;
    action->graphicOffsetX = frameOffsetX + graphicOffsetX;
    const std::int32_t frameOffsetY = offsetFrame->y;
    const std::int32_t graphicOffsetY = offsetY;
    action->graphicOffsetY = frameOffsetY + graphicOffsetY;
    std::memcpy(action->drawMode.bytes.data() + 0x0E,
        &selected->paletteIndex, sizeof(selected->paletteIndex));
    std::uint8_t revert;
    std::memcpy(&revert, &selected->revert, sizeof(revert));
    action->drawMode.bytes[0x0D] = revert;
    const std::uint32_t color = selected->value14;
    const bool flipGraphic = (revert & 3u) != 0;
    std::memcpy(action->drawMode.bytes.data() + 8, &color, sizeof(color));
    if (flipGraphic) {
        ApplyYobiGraphicFlip(action, action->graphicIndex);
    }

    const auto* const coordinates = static_cast<resource::animation::detail::CoordinateEntry*>(
        resource::animation::gAnimateRecords9A4A58[animeIndex].coordinates);
    std::int32_t coordinateIndex = 0;
    if (coordinates != nullptr) {
        const auto count = resource::animation::gAnimateRecords9A4A58[animeIndex].coordinateCount;
        if (count > 0) {
            const std::uint32_t graphicIndex = static_cast<std::uint32_t>(action->graphicIndex);
            for (; coordinateIndex < count; ++coordinateIndex) {
                if (coordinates[coordinateIndex].graphicIndex == graphicIndex) {
                    break;
                }
            }
        }
    }
    const std::uint32_t frameIndex = action->frameIndex;
    action->frameX = frames[frameIndex].x;
    action->frameY = frames[frameIndex].y;
    if (coordinates != nullptr
        && coordinateIndex < resource::animation::gAnimateRecords9A4A58[animeIndex].coordinateCount) {
        const std::int32_t coordinateX = coordinates[coordinateIndex].x;
        const std::int32_t frameX = action->frameX;
        const std::int32_t x = frameX + coordinateX;
        const std::int32_t frameY = action->frameY;
        action->frameX = x;
        const std::int32_t coordinateY = coordinates[coordinateIndex].y;
        action->frameY = frameY + coordinateY;
    }
    const std::uint8_t mode = action->drawMode.bytes[0x0D];
    if ((mode & 1u) != 0) {
        action->frameX = -action->frameX;
    }
    if ((mode & 2u) != 0) {
        action->frameY = -action->frameY;
    }
    const std::int32_t remaining = action->frameRemain - 1;
    action->frameIndex = frameIndex + 1u;
    action->frameRemain = remaining;
}

} // namespace

// YGOS01：完整5182B0正常查询；S16输出先取、当前S32偏移后取，低BYTE轴无效仍先查询。
std::int32_t Sub_5182B0(YobiAction* const action, const std::uint8_t axis)
{
    std::int16_t x;
    std::int16_t y;
    resource::GetGraphicXY(
        static_cast<std::uint32_t>(action->graphicIndex), &x, &y);
    if (axis == 0) {
        const std::int32_t offsetX = x;
        const std::int32_t graphicOffsetX = action->graphicOffsetX;
        return graphicOffsetX - offsetX;
    }
    if (axis == 1) {
        const std::int32_t offsetY = y;
        const std::int32_t graphicOffsetY = action->graphicOffsetY;
        return graphicOffsetY - offsetY;
    }
    return 0;
}

// YGOS01：先确定原F32系数再查询；保正常S64向零转换/窄化及当前偏移，沿1252宿主边界。
// 原函数：0x00518300。
std::int32_t Sub_518300(
    YobiAction* const action, const std::uint8_t axis, const std::int32_t kind)
{
    float scale = 0.0F;
    switch (kind) {
    case 3: scale = 1.75F; break;
    case 4: scale = 1.5F; break;
    case 5: scale = 1.25F; break;
    default: break;
    }
    std::int16_t x;
    std::int16_t y;
    resource::GetGraphicXY(static_cast<std::uint32_t>(action->graphicIndex), &x, &y);
    if (axis == 0) {
        const auto offset = static_cast<std::int32_t>(static_cast<std::int64_t>(
            static_cast<double>(x) * scale));
        return action->graphicOffsetX - offset;
    }
    if (axis == 1) {
        const auto offset = static_cast<std::int32_t>(static_cast<std::int64_t>(
            static_cast<double>(y) * scale));
        return action->graphicOffsetY - offset;
    }
    return 0;
}

// 原函数：0x00517880。
std::int32_t Sub_517880(
    YobiAction* const action,
    const std::int32_t rate,
    const std::int32_t stopAtEnd,
    const std::uint32_t flipStatic)
{
    if (action == nullptr || action->hidden == 1) {
        return 0;
    }
    const std::int32_t characterAction = action->characterAction;
    action->playRevert = stopAtEnd;
    std::int32_t imageNo;
    if (action->lastCharacterAction != characterAction
        || action->direction != action->lastDirection
        || (imageNo = action->imageNo) != action->lastImageNo) {
        imageNo = action->imageNo;
        action->lastCharacterAction = characterAction;
        const std::int32_t direction = action->direction;
        action->lastImageNo = imageNo;
        action->lastDirection = direction;
        action->frameRemain = 0;
        action->frameIndex = 0;
        action->completedLoops = 0;
    }
    if (action->frameRemain != 0) {
        --action->frameRemain;
        return 0;
    }
    if (imageNo <= 999) {
        action->graphicIndex = 0;
        return 0;
    }
    if (imageNo < 100000 || imageNo >= 200000) {
        std::int32_t graphicIndex;
        resource::GetGraphicMapNoIndex(imageNo, &graphicIndex);
        std::int16_t offsetX;
        std::int16_t offsetY;
        resource::GetGraphicXY(static_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
        const std::int32_t staticOffsetY = offsetY;
        const std::int32_t staticOffsetX = offsetX;
        const std::int32_t staticGraphicIndex = graphicIndex;
        action->graphicOffsetY = staticOffsetY;
        const std::uint8_t flipStaticByte = static_cast<std::uint8_t>(flipStatic);
        action->graphicIndex = staticGraphicIndex;
        action->graphicOffsetX = staticOffsetX;
        action->frameRemain = 0x7FFFFFFF;
        const std::int16_t paletteIndex = 0;
        std::memcpy(action->drawMode.bytes.data() + 0x0E, &paletteIndex, sizeof(paletteIndex));
        action->drawMode.bytes[0x0D] = 0;
        const std::uint32_t color = 0xFFFFFFFFu;
        std::memcpy(action->drawMode.bytes.data() + 8, &color, sizeof(color));
        if (flipStaticByte != 0) {
            const std::int32_t direction = action->direction;
            action->drawMode.bytes[0x0D] = 0;
            if (direction >= 2 && direction < 5) {
                action->drawMode.bytes[0x0D] = 1;
            }
            if ((action->drawMode.bytes[0x0D] & 3u) != 0) {
                ApplyYobiGraphicFlip(action, staticGraphicIndex);
            }
        }
        action->frameX = 0;
        action->frameY = 0;
        return 0;
    }
    if (imageNo > resource::animation::gMaxAnimateCount9A4A4C + 100000) {
        return 1;
    }
    const std::int32_t animeIndex =
        resource::animation::GetIndex_45B400(static_cast<std::uint32_t>(imageNo - 100000));
    if (animeIndex < 0) {
        return 1;
    }
    if (resource::animation::AnimeLoadAction_45B760(animeIndex) == 0) {
        return -1;
    }
    const auto* const list = static_cast<resource::animation::detail::ActionEntry*>(
        resource::animation::gAnimateRecords9A4A58[animeIndex].actionList);
    const std::uint16_t count = resource::animation::gAnimateRecords9A4A58[animeIndex].count;
    if (count == 0) {
        return 1;
    }
    std::int32_t selectedIndex = 0;
    const std::int32_t direction = action->direction;
    for (; selectedIndex < count; ++selectedIndex) {
        if (direction == static_cast<std::uint16_t>(list[selectedIndex].direction)
            && static_cast<std::uint16_t>(list[selectedIndex].action) == action->characterAction) {
            break;
        }
    }
    if (selectedIndex >= count) {
        return 1;
    }
    const auto* const selected = &list[selectedIndex];
    const std::uint32_t product = selected->duration * static_cast<std::uint32_t>(rate);
    const auto* const frames =
        static_cast<resource::animation::detail::FrameEntry*>(selected->frames);
    action->frameRemain = static_cast<std::int32_t>(product / 100u);
    if (action->frameRemain < 1) {
        action->frameRemain = 1;
    }
    if (action->frameIndex >= selected->frameCount) {
        if (stopAtEnd == 1) {
            action->frameRemain = 255;
            action->completedLoops = 1;
            return 1;
        }
        const std::uint32_t completedLoops = action->completedLoops;
        action->frameIndex = 0;
        action->completedLoops = completedLoops + 1u;
    }
    ApplyYobiAnimeFrame(action, animeIndex, selected, frames);
    return 0;
}

// 原函数：0x00517DA0。
std::int32_t PlayFrame_517DA0(YobiAction* const action, const std::int32_t stopAtEnd)
{
    if (action == nullptr || action->hidden == 1) {
        return 0;
    }
    const std::int32_t characterAction = action->characterAction;
    action->playRevert = stopAtEnd;
    std::int32_t imageNo;
    if (action->lastCharacterAction != characterAction
        || action->direction != action->lastDirection
        || (imageNo = action->imageNo) != action->lastImageNo) {
        imageNo = action->imageNo;
        action->lastCharacterAction = characterAction;
        const std::int32_t direction = action->direction;
        action->lastImageNo = imageNo;
        action->lastDirection = direction;
        action->frameRemain = 0;
        action->frameIndex = 0;
        action->completedLoops = 0;
    }
    if (action->frameRemain != 0) {
        --action->frameRemain;
        return 0;
    }
    if (imageNo <= 999) {
        action->graphicIndex = 0;
        return 0;
    }
    if (imageNo < 100000) {
        std::int32_t graphicIndex;
        resource::GetGraphicMapNoIndex(imageNo, &graphicIndex);
        std::int16_t offsetX;
        std::int16_t offsetY;
        resource::GetGraphicXY(static_cast<std::uint32_t>(graphicIndex), &offsetX, &offsetY);
        const std::int32_t staticOffsetX = offsetX;
        const std::int32_t staticOffsetY = offsetY;
        const std::int32_t staticGraphicIndex = graphicIndex;
        action->graphicOffsetX = staticOffsetX;
        const std::int32_t direction = action->direction;
        action->graphicIndex = staticGraphicIndex;
        action->graphicOffsetY = staticOffsetY;
        action->frameRemain = 0x7FFFFFFF;
        const std::int16_t paletteIndex = 0;
        std::memcpy(action->drawMode.bytes.data() + 0x0E, &paletteIndex, sizeof(paletteIndex));
        const std::uint32_t color = 0xFFFFFFFFu;
        std::memcpy(action->drawMode.bytes.data() + 8, &color, sizeof(color));
        action->drawMode.bytes[0x0D] = 0;
        if (direction >= 2 && direction < 5) {
            action->drawMode.bytes[0x0D] = 1;
        }
        if ((action->drawMode.bytes[0x0D] & 3u) != 0) {
            ApplyYobiGraphicFlip(action, staticGraphicIndex);
        }
        action->frameX = 0;
        action->frameY = 0;
        return 0;
    }
    if (imageNo > resource::animation::gMaxAnimateCount9A4A4C + 100000) {
        return 1;
    }
    const std::int32_t animeIndex =
        resource::animation::GetIndex_45B400(static_cast<std::uint32_t>(imageNo - 100000));
    if (animeIndex < 0) {
        return 1;
    }
    if (resource::animation::AnimeLoadAction_45B760(animeIndex) == 0) {
        return -1;
    }
    const auto* const list = static_cast<resource::animation::detail::ActionEntry*>(
        resource::animation::gAnimateRecords9A4A58[animeIndex].actionList);
    const std::uint16_t count = resource::animation::gAnimateRecords9A4A58[animeIndex].count;
    if (count == 0) {
        return 1;
    }
    std::int32_t selectedIndex = 0;
    const std::int32_t direction = action->direction;
    for (; selectedIndex < count; ++selectedIndex) {
        if (direction == static_cast<std::uint16_t>(list[selectedIndex].direction)
            && static_cast<std::uint16_t>(list[selectedIndex].action) == action->characterAction) {
            break;
        }
    }
    if (selectedIndex >= count) {
        return 1;
    }
    const auto* const selected = &list[selectedIndex];
    const std::uint32_t product =
        selected->duration * static_cast<std::uint32_t>(action->framePlayingRate);
    const auto* const frames =
        static_cast<resource::animation::detail::FrameEntry*>(selected->frames);
    action->frameRemain = static_cast<std::int32_t>(product / 100u);
    if (action->frameRemain < 1) {
        action->frameRemain = 1;
    }
    if (action->frameIndex >= selected->frameCount) {
        if (stopAtEnd == 1) {
            action->frameRemain = 255;
            action->completedLoops = 1;
            return 1;
        }
        const std::uint32_t completedLoops = action->completedLoops;
        action->frameIndex = 0;
        action->completedLoops = completedLoops + 1u;
    }
    ApplyYobiAnimeFrame(action, animeIndex, selected, frames);
    return 0;
}

} // namespace detail

} // namespace cg::login

namespace cg::world::detail {

// YGOS01：完整S32动画图号区间判定；仅445350方向门禁调用，不新增公头接口。
// 原函数：0x005183B0。
std::int32_t Sub_5183B0(const std::int32_t imageNo)
{
    return imageNo >= 100000 && imageNo < 200000 ? 1 : 0;
}

} // namespace cg::world::detail
