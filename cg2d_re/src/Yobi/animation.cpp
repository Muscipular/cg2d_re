#include "animation.hpp"
#include "map_actor.hpp"
#include "base.hpp"
#include "../resource/AnimationRuntimeState.hpp"
#include "cg/resource/AnimationRuntime.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/GraphicRead.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"

#include <cstdint>
#include <cstring>

namespace cg::yobi {

namespace detail {

// Yobi帧驱动按动画资源记录位发布动作属性，保持原失败返回。
enum AnimeResourceFlag : std::uint32_t {
    AnimeResourceFlag_PublishActionFlags = 0x00000001u,
};


std::uint16_t GetYobiAnimeActionFlags(const YobiAction* const action) noexcept
{
    return render::ReadGraphicAnimeActionFlags(action->graphicIndex);
}

void GetYobiAttachmentCoordinate(
    const YobiAction* const action, std::int32_t& x, std::int32_t& y, const bool applyFlip) noexcept
{
    std::int16_t coordinateX;
    std::int16_t coordinateY;
    render::ReadGraphicCoordinate(action->graphicIndex, coordinateX, coordinateY);
    x = applyFlip && (action->drawMode.mixFlags & render::GraphicMixFlag_FlipX) != 0
        ? -static_cast<std::int32_t>(coordinateX) : coordinateX;
    y = coordinateY;
}

bool GetYobiTitleTopLeft(const YobiAction* const actor, const YobiAction* const title,
    const bool scaled, std::int32_t& x, std::int32_t& y)
{
    if (actor->graphicIndex < 0 || title->graphicIndex < 0
        || render::LoadGraphic_522570(actor->graphicIndex) == 0
        || render::LoadGraphic_522570(title->graphicIndex) == 0) {
        return false;
    }
    const float actorScaleX = scaled ? actor->scaleX : 1.0F;
    const float actorScaleY = scaled ? actor->scaleY : 1.0F;
    const float titleScaleX = scaled ? title->scaleX : 1.0F;
    const float titleScaleY = scaled ? title->scaleY : 1.0F;
    const auto actorWidth = static_cast<std::int32_t>(render::ReadGraphicDxWidth(actor->graphicIndex) * static_cast<double>(actorScaleX));
    const auto titleWidth = static_cast<std::int32_t>(render::ReadGraphicDxWidth(title->graphicIndex) * static_cast<double>(titleScaleX));
    const auto titleHeight = static_cast<std::int32_t>(render::ReadGraphicDxHeight(title->graphicIndex) * static_cast<double>(titleScaleY));
    if (actorWidth <= 0 || titleWidth <= 0 || titleHeight <= 0) {
        return false;
    }
    // graphicOffset已包含当前帧坐标与翻转修正；称号自身偏移不再重复叠加。
    x = actor->x + static_cast<std::int32_t>(actor->graphicOffsetX * static_cast<double>(actorScaleX))
        + (actorWidth - titleWidth) / 2;
    y = actor->y + static_cast<std::int32_t>(actor->graphicOffsetY * static_cast<double>(actorScaleY)) - titleHeight;
    return true;
}

// 原两播放入口的同源翻转段；只有真实图形加载非零后才按需读取各个 DX 尺寸。
static void ApplyYobiGraphicFlip(YobiAction* const action, const std::int32_t loadGraphicIndex)
{
    if (render::LoadGraphic_522570(loadGraphicIndex) == 0) {
        return;
    }
    const std::uint8_t mode = action->drawMode.mixFlags;
    if ((mode & render::GraphicMixFlag_FlipX) != 0) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const auto* const entries = render::gGraphicDxCache103CFA4;
        const std::int32_t offsetX = action->graphicOffsetX;
        const std::int16_t width = render::ReadGraphicDxWidthFromEntries(entries, graphicIndex);
        action->graphicOffsetX = -(offsetX + width);
    }
    if ((mode & render::GraphicMixFlag_FlipY) != 0) {
        const std::int32_t graphicIndex = action->graphicIndex;
        const auto* const entries = render::gGraphicDxCache103CFA4;
        const std::int16_t height = render::ReadGraphicDxHeightFromEntries(entries, graphicIndex);
        const std::int32_t offsetY = action->graphicOffsetY;
        action->graphicOffsetY = -(offsetY + height);
    }
}

// 原 517A8C..517C85 / 518098..518291 的相同阶段；不合并两入口的图号或速率门禁。
static bool ApplyYobiAnimeFrame(
    YobiAction* const action,
    const std::int32_t animeIndex,
    const resource::animation::detail::ActionEntry* const selected,
    const resource::animation::detail::FrameEntry* const frames,
    const YobiAction* const syncSource)
{
    const std::uint16_t flag = frames[action->frameIndex].flag;
    if (flag != 0) {
        if (flag >= 10000) {
            action->frameEvent = flag;
        } else if (action->ownerSession->WorldScene().sceneType != 9 || (action->flags & YobiActionMask_PetOrRiderOrFollowPetOverlay) == 0) {
            static_cast<void>(audio::PlaySe_52C450(flag, action->x));
        }
    }
    const auto* const graphicFrame = &frames[action->frameIndex];
    action->drawMode.mixFlags &= ~cg::render::GraphicMixFlag_FlipMask;
    action->graphicIndex = static_cast<std::int32_t>(graphicFrame->graphicIndex);
    // AF406：4.06@470759按图号共享完整动作属性，不能从mixFlags低字节反推。
    if ((resource::animation::gAnimateRecords9A4A58[animeIndex].flag & AnimeResourceFlag_PublishActionFlags) != 0) {
        if (!render::SetGraphicAnimeActionFlags(graphicFrame->graphicIndex, selected->revert)) {
            return false; // RMO01：两条动画驱动入口均传递资源失败值-1。
        }
    }
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
    // 动画主表保存的是完整资源编号减100000；同号图档匹配使用完整编号。
    action->drawMode.paletteIndex = resource::ResolveAnimationPalette(
        selected->paletteIndex,
        resource::animation::gAnimateRecords9A4A58[animeIndex].animeNo + 100000u);
    std::uint8_t revert;
    std::memcpy(&revert, &selected->revert, sizeof(revert));
    action->drawMode.mixFlags = revert;
    const std::uint32_t color = selected->value14;
    const bool flipGraphic = (revert & render::GraphicMixFlag_FlipMask) != 0;
    action->drawMode.color = color;
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
    const std::uint8_t mode = action->drawMode.mixFlags;
    if ((mode & render::GraphicMixFlag_FlipX) != 0) {
        action->frameX = -action->frameX;
    }
    if ((mode & render::GraphicMixFlag_FlipY) != 0) {
        action->frameY = -action->frameY;
    }
    // 4.06@4707E5..47080B：取帧后同步下一帧，不重放同步源的帧事件。
    const std::int32_t characterAction = action->characterAction;
    if (syncSource != nullptr
        && (GetYobiAnimeActionFlags(action) & resource::animation::AnimeActionFlag_Wing) != 0
        && (characterAction == CharacterAnimation_Attack
            || characterAction == CharacterAnimation_Damage
            || characterAction == CharacterAnimation_Guard
            || characterAction == CharacterAnimation_Down)) {
        action->frameIndex = syncSource->frameIndex;
    } else {
        action->frameIndex = frameIndex + 1u;
        --action->frameRemain;
    }
    return true;
}

// YGOS01：先确定原F32系数再查询；保正常S64向零转换/窄化及当前偏移，沿1252宿主边界。
// 原函数：0x00518300。
std::int32_t GetScaledYobiGraphicOriginOffset_518300(
    YobiAction* const action, const std::uint8_t axis, const std::int32_t kind)
{
    float scale = 0.0F;
    switch (kind) {
    case MapGearKind_DirectionalBatch10: scale = 1.75F; break;
    case MapGearKind_DirectionalBatch11: scale = 1.5F; break;
    case MapGearKind_DirectionalBatch12: scale = 1.25F; break;
    default: break;
    }
    std::int16_t x;
    std::int16_t y;
    resource::GetGraphicXY(static_cast<std::uint32_t>(action->graphicIndex), &x, &y);
    if (axis == YobiGraphicAxis_X) {
        const auto offset = static_cast<std::int32_t>(static_cast<std::int64_t>(
            static_cast<double>(x) * scale));
        return action->graphicOffsetX - offset;
    }
    if (axis == YobiGraphicAxis_Y) {
        const auto offset = static_cast<std::int32_t>(static_cast<std::int64_t>(
            static_cast<double>(y) * scale));
        return action->graphicOffsetY - offset;
    }
    return 0;
}

// 原函数：0x00517DA0。
std::int32_t PlayFrame_517DA0(
    YobiAction* const action, const std::int32_t stopAtEnd, const YobiAction* const syncSource)
{
    if (action == nullptr || action->hidden == 1) {
        return AnimationFrameResult_Continue;
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
        return AnimationFrameResult_Continue;
    }
    if (imageNo <= 999) {
        action->graphicIndex = 0;
        return AnimationFrameResult_Continue;
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
        action->drawMode.paletteIndex = paletteIndex;
        const std::uint32_t color = 0xFFFFFFFFu;
        action->drawMode.color = color;
        action->drawMode.mixFlags = cg::render::GraphicMixFlag_None;
        if (direction >= YobiDirection_UpRight && direction < YobiDirection_Down) {
            action->drawMode.mixFlags = cg::render::GraphicMixFlag_FlipX;
        }
        if ((action->drawMode.mixFlags & render::GraphicMixFlag_FlipMask) != 0) {
            ApplyYobiGraphicFlip(action, staticGraphicIndex);
        }
        action->frameX = 0;
        action->frameY = 0;
        return AnimationFrameResult_Continue;
    }
    // RAC01：记录容量不限制AnimeNo，按最终动画索引查询。
    const std::int32_t animeIndex =
        resource::animation::GetIndex_45B400(static_cast<std::uint32_t>(imageNo - 100000));
    if (animeIndex < 0) {
        return AnimationFrameResult_StoppedOrUnavailable;
    }
    if (resource::animation::AnimeLoadAction_45B760(animeIndex) == 0) {
        return AnimationFrameResult_ResourceFailure;
    }
    const auto* const list = static_cast<resource::animation::detail::ActionEntry*>(
        resource::animation::gAnimateRecords9A4A58[animeIndex].actionList);
    const std::uint16_t count = resource::animation::gAnimateRecords9A4A58[animeIndex].count;
    if (count == 0) {
        return AnimationFrameResult_StoppedOrUnavailable;
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
        return AnimationFrameResult_StoppedOrUnavailable;
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
        if (stopAtEnd == AnimationPlayback_StopAtEnd) {
            action->frameRemain = 255;
            action->completedLoops = 1;
            return AnimationFrameResult_StoppedOrUnavailable;
        }
        const std::uint32_t completedLoops = action->completedLoops;
        action->frameIndex = 0;
        action->completedLoops = completedLoops + 1u;
    }
    return ApplyYobiAnimeFrame(action, animeIndex, selected, frames, syncSource)
        ? AnimationFrameResult_Continue : AnimationFrameResult_ResourceFailure;
}

} // namespace detail

// YGOS01：完整5182B0正常查询；S16输出先取、当前S32偏移后取，低BYTE轴无效仍先查询。
std::int32_t GetYobiGraphicOriginOffset_5182B0(YobiAction* const action, const std::uint8_t axis)
{
    std::int16_t x;
    std::int16_t y;
    resource::GetGraphicXY(
        static_cast<std::uint32_t>(action->graphicIndex), &x, &y);
    if (axis == YobiGraphicAxis_X) {
        const std::int32_t offsetX = x;
        const std::int32_t graphicOffsetX = action->graphicOffsetX;
        return graphicOffsetX - offsetX;
    }
    if (axis == YobiGraphicAxis_Y) {
        const std::int32_t offsetY = y;
        const std::int32_t graphicOffsetY = action->graphicOffsetY;
        return graphicOffsetY - offsetY;
    }
    return 0;
}

// 原函数：0x00517880。
std::int32_t AdvanceYobiAnimationFrame_517880(
    YobiAction* const action,
    const std::int32_t rate,
    const std::int32_t stopAtEnd,
    const std::uint32_t flipStatic,
    const YobiAction* const syncSource)
{
    if (action == nullptr || action->hidden == 1) {
        return AnimationFrameResult_Continue;
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
        return AnimationFrameResult_Continue;
    }
    if (imageNo <= 999) {
        action->graphicIndex = 0;
        return AnimationFrameResult_Continue;
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
        action->drawMode.paletteIndex = paletteIndex;
        action->drawMode.mixFlags = cg::render::GraphicMixFlag_None;
        const std::uint32_t color = 0xFFFFFFFFu;
        action->drawMode.color = color;
        if (flipStaticByte != 0) {
            const std::int32_t direction = action->direction;
            action->drawMode.mixFlags = cg::render::GraphicMixFlag_None;
            if (direction >= YobiDirection_UpRight && direction < YobiDirection_Down) {
                action->drawMode.mixFlags = cg::render::GraphicMixFlag_FlipX;
            }
            if ((action->drawMode.mixFlags & render::GraphicMixFlag_FlipMask) != 0) {
                detail::ApplyYobiGraphicFlip(action, staticGraphicIndex);
            }
        }
        action->frameX = 0;
        action->frameY = 0;
        return AnimationFrameResult_Continue;
    }
    // BLM01：动画编号由实际索引表判定，不与动画记录容量绑定。
    const std::int32_t animeIndex =
        resource::animation::GetIndex_45B400(static_cast<std::uint32_t>(imageNo - 100000));
    if (animeIndex < 0) {
        return AnimationFrameResult_StoppedOrUnavailable;
    }
    if (resource::animation::AnimeLoadAction_45B760(animeIndex) == 0) {
        return AnimationFrameResult_ResourceFailure;
    }
    const auto* const list = static_cast<resource::animation::detail::ActionEntry*>(
        resource::animation::gAnimateRecords9A4A58[animeIndex].actionList);
    const std::uint16_t count = resource::animation::gAnimateRecords9A4A58[animeIndex].count;
    if (count == 0) {
        return AnimationFrameResult_StoppedOrUnavailable;
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
        return AnimationFrameResult_StoppedOrUnavailable;
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
        if (stopAtEnd == AnimationPlayback_StopAtEnd) {
            action->frameRemain = 255;
            action->completedLoops = 1;
            return AnimationFrameResult_StoppedOrUnavailable;
        }
        const std::uint32_t completedLoops = action->completedLoops;
        action->frameIndex = 0;
        action->completedLoops = completedLoops + 1u;
    }
    return detail::ApplyYobiAnimeFrame(action, animeIndex, selected, frames, syncSource)
        ? AnimationFrameResult_Continue : AnimationFrameResult_ResourceFailure;
}

} // namespace cg::yobi
