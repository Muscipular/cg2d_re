#pragma once

#include "world/AnimationState.hpp"
#include "world/Movement.hpp"

#include <cstdint>
#include <optional>

namespace cg::resource {

struct AnimationClip;

} // namespace cg::resource

namespace cg::world {

// MovementAnimationSample 表达移动采样与随移动推进后的动画帧。
struct MovementAnimationSample {
    MovementSample movement{};
    std::optional<AnimationFrameSample> frame{};
    bool moving{};
    bool animationAdvanced{};
};

[[nodiscard]] MovementAnimationSample AdvanceMovementAnimation(
    AnimationState& state,
    const Movement& movement,
    const cg::resource::AnimationClip& clip,
    std::int16_t movingAction,
    std::uint64_t nowTimeMs,
    std::uint64_t deltaMs) noexcept;

} // namespace cg::world
