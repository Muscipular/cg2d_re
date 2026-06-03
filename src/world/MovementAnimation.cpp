#include "world/MovementAnimation.hpp"

namespace cg::world {

MovementAnimationSample AdvanceMovementAnimation(
    AnimationState& state,
    const Movement& movement,
    const cg::resource::AnimationClip& clip,
    std::int16_t movingAction,
    std::uint64_t nowTimeMs,
    std::uint64_t deltaMs) noexcept
{
    MovementAnimationSample result;
    result.movement = SampleMovement(movement, nowTimeMs);
    result.moving = nowTimeMs >= movement.startTimeMs && !result.movement.finished;

    if (result.moving) {
        SetAnimation(state, movingAction, result.movement.direction);
        result.animationAdvanced = AdvanceAnimation(state, clip, deltaMs);
    }

    result.frame = SampleAnimationFrame(state, clip);
    return result;
}

} // namespace cg::world
