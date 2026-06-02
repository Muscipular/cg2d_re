#include "world/AnimationState.hpp"

#include "resource/AnimationLoader.hpp"

#include <algorithm>
#include <span>

namespace cg::world {

namespace {

[[nodiscard]] int ToResourceDirection(Direction direction) noexcept
{
    return static_cast<int>(direction);
}

[[nodiscard]] const resource::AnimeAction* FindStateAction(
    const AnimationState& state,
    const resource::AnimationClip& clip) noexcept
{
    return resource::FindAnimeAction(clip, state.action, ToResourceDirection(state.direction));
}

} // namespace

void SetAnimation(AnimationState& state, std::int16_t action, Direction direction) noexcept
{
    if (state.action == action && state.direction == direction) {
        return;
    }

    state.action = action;
    state.direction = direction;
    state.frameIndex = 0U;
    state.elapsedMs = 0U;
}

bool AdvanceAnimation(AnimationState& state, const resource::AnimationClip& clip, std::uint64_t deltaMs) noexcept
{
    const resource::AnimeAction* action = FindStateAction(state, clip);
    if (action == nullptr) {
        return false;
    }

    const std::span<const resource::AnimeFrame> frames = resource::GetAnimeActionFrames(clip, *action);
    if (frames.empty()) {
        return false;
    }

    const std::size_t frameCount = frames.size();
    const std::uint64_t frameDurationMs = ComputeAnimationFrameDurationMs(*action, frameCount);
    const std::uint64_t baseFrames = state.elapsedMs / frameDurationMs;
    const std::uint64_t deltaFrames = deltaMs / frameDurationMs;
    std::uint64_t elapsedMs = (state.elapsedMs % frameDurationMs) + (deltaMs % frameDurationMs);
    std::uint64_t advancedFrames = baseFrames + deltaFrames;

    if (elapsedMs >= frameDurationMs) {
        elapsedMs -= frameDurationMs;
        ++advancedFrames;
    }

    const std::size_t frameAdvance = static_cast<std::size_t>(advancedFrames % static_cast<std::uint64_t>(frameCount));
    state.frameIndex = ((state.frameIndex % frameCount) + frameAdvance) % frameCount;
    state.elapsedMs = elapsedMs;
    return true;
}

std::optional<AnimationFrameSample> SampleAnimationFrame(
    const AnimationState& state,
    const resource::AnimationClip& clip) noexcept
{
    const resource::AnimeAction* action = FindStateAction(state, clip);
    if (action == nullptr) {
        return std::nullopt;
    }

    const std::span<const resource::AnimeFrame> frames = resource::GetAnimeActionFrames(clip, *action);
    if (frames.empty()) {
        return std::nullopt;
    }

    const std::size_t frameIndex = state.frameIndex % frames.size();
    const resource::AnimeFrame& frame = frames[frameIndex];
    return AnimationFrameSample{
        &frame,
        frame.graphicSeqNo,
        frameIndex,
        ComputeAnimationFrameOffset(frame),
    };
}

std::uint64_t ComputeAnimationFrameDurationMs(
    const resource::AnimeAction& action,
    std::size_t frameCount) noexcept
{
    if (action.header.duration <= 0 || frameCount == 0U) {
        return 100U;
    }

    const std::uint64_t durationMs = static_cast<std::uint64_t>(action.header.duration);
    return std::max<std::uint64_t>(16U, durationMs / frameCount);
}

AnimationFrameOffset ComputeAnimationFrameOffset(const resource::AnimeFrame& frame) noexcept
{
    return AnimationFrameOffset{
        frame.x,
        frame.y,
    };
}

AnimationFrameOffset ComputeAnimationDrawOffset(
    const resource::AnimeFrame& frame,
    int graphicOffsetX,
    int graphicOffsetY) noexcept
{
    return AnimationFrameOffset{
        static_cast<int>(frame.x) + graphicOffsetX,
        static_cast<int>(frame.y) + graphicOffsetY,
    };
}

} // namespace cg::world
