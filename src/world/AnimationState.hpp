#pragma once

#include "resource/ResourceTypes.hpp"
#include "world/Transform.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>

namespace cg::world {

// AnimationFrameOffset 表示动画帧提供的像素偏移。
struct AnimationFrameOffset {
    int x{};
    int y{};

    [[nodiscard]] constexpr bool operator==(const AnimationFrameOffset&) const noexcept = default;
};

// AnimationFrameSample 表示当前动画帧采样结果，不持有图像资源。
struct AnimationFrameSample {
    const resource::AnimeFrame* frame{};
    std::int32_t graphicSeqNo{};
    std::size_t frameIndex{};
    AnimationFrameOffset frameOffset{};
};

// AnimationState 表示实体当前动作、方向和帧推进状态。
struct AnimationState {
    std::int16_t action{-1};
    Direction direction{Direction::None};
    std::size_t frameIndex{};
    std::uint64_t elapsedMs{};
};

void SetAnimation(AnimationState& state, std::int16_t action, Direction direction) noexcept;

[[nodiscard]] bool AdvanceAnimation(
    AnimationState& state,
    const resource::AnimationClip& clip,
    std::uint64_t deltaMs) noexcept;

[[nodiscard]] std::optional<AnimationFrameSample> SampleAnimationFrame(
    const AnimationState& state,
    const resource::AnimationClip& clip) noexcept;

[[nodiscard]] std::uint64_t ComputeAnimationFrameDurationMs(
    const resource::AnimeAction& action,
    std::size_t frameCount) noexcept;

[[nodiscard]] AnimationFrameOffset ComputeAnimationFrameOffset(const resource::AnimeFrame& frame) noexcept;
[[nodiscard]] AnimationFrameOffset ComputeAnimationDrawOffset(
    const resource::AnimeFrame& frame,
    int graphicOffsetX,
    int graphicOffsetY) noexcept;

} // namespace cg::world
