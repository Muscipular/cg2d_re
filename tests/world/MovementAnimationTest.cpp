#include "world/MovementAnimation.hpp"

#include "resource/ResourceTypes.hpp"

#include <cstdint>
#include <iostream>
#include <optional>

namespace {

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

[[nodiscard]] cg::resource::AnimeFrame MakeFrame(std::int32_t graphicSeqNo)
{
    cg::resource::AnimeFrame frame;
    frame.graphicSeqNo = graphicSeqNo;
    return frame;
}

[[nodiscard]] cg::resource::AnimeAction MakeAction(
    std::int16_t action,
    cg::world::Direction direction,
    std::int32_t duration,
    int frameIndex,
    std::int32_t frameCount)
{
    cg::resource::AnimeAction result;
    result.header.action = static_cast<cg::resource::CG_ActionType>(action);
    result.header.dir = static_cast<cg::resource::CG_Direction>(static_cast<int>(direction));
    result.header.duration = duration;
    result.header.frameCount = frameCount;
    result.frameIndex = frameIndex;
    return result;
}

[[nodiscard]] cg::resource::AnimationClip MakeClip()
{
    cg::resource::AnimationClip clip;
    clip.actions.push_back(MakeAction(
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        cg::world::Direction::East,
        200,
        0,
        2));
    clip.actions.push_back(MakeAction(
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        cg::world::Direction::South,
        200,
        2,
        2));
    clip.frames.push_back(MakeFrame(101));
    clip.frames.push_back(MakeFrame(102));
    clip.frames.push_back(MakeFrame(201));
    clip.frames.push_back(MakeFrame(202));
    return clip;
}

int TestWalkingAnimationFollowsMovement()
{
    const cg::resource::AnimationClip clip = MakeClip();
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {100, 0},
        1000,
        400,
        cg::world::Direction::East);
    cg::world::AnimationState state{};

    const cg::world::MovementAnimationSample sample = cg::world::AdvanceMovementAnimation(
        state,
        movement,
        clip,
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        1100,
        100);

    int failures = 0;
    failures += Expect(sample.moving, "移动中标记 moving");
    failures += Expect(sample.animationAdvanced, "移动中推进动画");
    failures += Expect(state.action == cg::resource::CG_ACTIONTYPE_WALK, "移动中切换走路动作");
    failures += Expect(state.direction == cg::world::Direction::East, "动画方向跟随移动方向");
    failures += Expect(state.frameIndex == 1U, "移动 delta 推进到下一帧");
    failures += Expect(sample.frame.has_value(), "移动中可采样动画帧");
    if (sample.frame.has_value()) {
        failures += Expect(sample.frame->graphicSeqNo == 102, "走路动画采样第二帧");
    }
    failures += Expect(sample.movement.position.x == 25.0F, "移动采样位置跟随时间");
    return failures;
}

int TestBeforeStartDoesNotSwitchToWalk()
{
    const cg::resource::AnimationClip clip = MakeClip();
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {64, 0},
        1000,
        200,
        cg::world::Direction::East);
    cg::world::AnimationState state{
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        cg::world::Direction::South,
        1U,
        20U,
    };

    const cg::world::MovementAnimationSample sample = cg::world::AdvanceMovementAnimation(
        state,
        movement,
        clip,
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        900,
        100);

    int failures = 0;
    failures += Expect(!sample.moving, "开始前不标记 moving");
    failures += Expect(!sample.animationAdvanced, "开始前不推进动画");
    failures += Expect(state.direction == cg::world::Direction::South, "开始前不改动画方向");
    failures += Expect(state.frameIndex == 1U, "开始前不改帧索引");
    return failures;
}

int TestFinishedMovementDoesNotAdvanceAnimation()
{
    const cg::resource::AnimationClip clip = MakeClip();
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {64, 0},
        1000,
        200,
        cg::world::Direction::East);
    cg::world::AnimationState state{
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        cg::world::Direction::East,
        1U,
        0U,
    };

    const cg::world::MovementAnimationSample sample = cg::world::AdvanceMovementAnimation(
        state,
        movement,
        clip,
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        1300,
        100);

    int failures = 0;
    failures += Expect(!sample.moving, "结束后不标记 moving");
    failures += Expect(!sample.animationAdvanced, "结束后不推进动画");
    failures += Expect(state.frameIndex == 1U, "结束后保持现有帧索引");
    failures += Expect(sample.frame.has_value(), "结束后仍可采样当前动画帧");
    if (sample.frame.has_value()) {
        failures += Expect(sample.frame->graphicSeqNo == 102, "结束后采样当前帧");
    }
    return failures;
}

int TestDirectionChangeResetsAnimation()
{
    const cg::resource::AnimationClip clip = MakeClip();
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {0, 64},
        1000,
        400,
        cg::world::Direction::South);
    cg::world::AnimationState state{
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        cg::world::Direction::East,
        1U,
        50U,
    };

    const cg::world::MovementAnimationSample sample = cg::world::AdvanceMovementAnimation(
        state,
        movement,
        clip,
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        1000,
        0);

    int failures = 0;
    failures += Expect(sample.moving, "移动开始时标记 moving");
    failures += Expect(sample.animationAdvanced, "移动开始时尝试推进动画");
    failures += Expect(state.direction == cg::world::Direction::South, "方向变化后动画方向跟随移动");
    failures += Expect(state.frameIndex == 0U, "方向变化重置帧索引");
    failures += Expect(state.elapsedMs == 0U, "方向变化重置帧耗时");
    failures += Expect(sample.frame.has_value(), "方向变化后可采样新方向帧");
    if (sample.frame.has_value()) {
        failures += Expect(sample.frame->graphicSeqNo == 201, "方向变化后采样南向首帧");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestWalkingAnimationFollowsMovement();
    failures += TestBeforeStartDoesNotSwitchToWalk();
    failures += TestFinishedMovementDoesNotAdvanceAnimation();
    failures += TestDirectionChangeResetsAnimation();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
