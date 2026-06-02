#include "world/AnimationState.hpp"

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

[[nodiscard]] cg::resource::AnimeFrame MakeFrame(std::int32_t graphicSeqNo, std::int16_t x, std::int16_t y)
{
    cg::resource::AnimeFrame frame;
    frame.graphicSeqNo = graphicSeqNo;
    frame.x = x;
    frame.y = y;
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

[[nodiscard]] cg::resource::AnimationClip MakeClip(
    std::int16_t action = 1,
    cg::world::Direction direction = cg::world::Direction::East,
    std::int32_t duration = 300)
{
    cg::resource::AnimationClip clip;
    clip.actions.push_back(MakeAction(action, direction, duration, 0, 3));
    clip.frames.push_back(MakeFrame(101, -3, 4));
    clip.frames.push_back(MakeFrame(102, 5, -6));
    clip.frames.push_back(MakeFrame(103, 7, 8));
    return clip;
}

int TestSetAnimationResetRules()
{
    cg::world::AnimationState state{1, cg::world::Direction::East, 2U, 50U};

    int failures = 0;
    cg::world::SetAnimation(state, 1, cg::world::Direction::East);
    failures += Expect(state.frameIndex == 2U, "相同动作和方向不重置帧");
    failures += Expect(state.elapsedMs == 50U, "相同动作和方向不重置耗时");

    cg::world::SetAnimation(state, 2, cg::world::Direction::East);
    failures += Expect(state.action == 2, "动作变化写入新动作");
    failures += Expect(state.frameIndex == 0U, "动作变化重置帧");
    failures += Expect(state.elapsedMs == 0U, "动作变化重置耗时");

    state.frameIndex = 2U;
    state.elapsedMs = 50U;
    cg::world::SetAnimation(state, 2, cg::world::Direction::South);
    failures += Expect(state.direction == cg::world::Direction::South, "方向变化写入新方向");
    failures += Expect(state.frameIndex == 0U, "方向变化重置帧");
    failures += Expect(state.elapsedMs == 0U, "方向变化重置耗时");
    return failures;
}

int TestMissingActionAndFrames()
{
    int failures = 0;
    const cg::resource::AnimationClip emptyClip;
    cg::world::AnimationState state{1, cg::world::Direction::East, 0U, 0U};

    failures += Expect(!cg::world::AdvanceAnimation(state, emptyClip, 100U), "缺动作时不推进");
    failures += Expect(!cg::world::SampleAnimationFrame(state, emptyClip).has_value(), "缺动作时不采样");

    cg::resource::AnimationClip noFrameClip;
    noFrameClip.actions.push_back(MakeAction(1, cg::world::Direction::East, 100, 0, 0));
    failures += Expect(!cg::world::AdvanceAnimation(state, noFrameClip, 100U), "缺帧时不推进");
    failures += Expect(!cg::world::SampleAnimationFrame(state, noFrameClip).has_value(), "缺帧时不采样");
    return failures;
}

int TestFrameDuration()
{
    int failures = 0;
    failures += Expect(
        cg::world::ComputeAnimationFrameDurationMs(MakeAction(1, cg::world::Direction::East, 90, 0, 3), 3U) == 30U,
        "duration 按帧数均分");
    failures += Expect(
        cg::world::ComputeAnimationFrameDurationMs(MakeAction(1, cg::world::Direction::East, 15, 0, 1), 1U) == 16U,
        "duration 最小 16ms");
    failures += Expect(
        cg::world::ComputeAnimationFrameDurationMs(MakeAction(1, cg::world::Direction::East, 0, 0, 1), 1U) == 100U,
        "duration 为 0 fallback 100ms");
    failures += Expect(
        cg::world::ComputeAnimationFrameDurationMs(MakeAction(1, cg::world::Direction::East, -1, 0, 1), 1U) == 100U,
        "duration 为负 fallback 100ms");
    failures += Expect(
        cg::world::ComputeAnimationFrameDurationMs(MakeAction(1, cg::world::Direction::East, 90, 0, 0), 0U) == 100U,
        "frameCount 为 0 fallback 100ms");
    return failures;
}

int TestAdvanceAnimation()
{
    const cg::resource::AnimationClip clip = MakeClip(1, cg::world::Direction::East, 300);
    cg::world::AnimationState state{1, cg::world::Direction::East, 0U, 0U};

    int failures = 0;
    failures += Expect(cg::world::AdvanceAnimation(state, clip, 99U), "合法动画可推进");
    failures += Expect(state.frameIndex == 0U, "不足一帧不切帧");
    failures += Expect(state.elapsedMs == 99U, "不足一帧累计耗时");

    failures += Expect(cg::world::AdvanceAnimation(state, clip, 1U), "满一帧可推进");
    failures += Expect(state.frameIndex == 1U, "满一帧切到下一帧");
    failures += Expect(state.elapsedMs == 0U, "满一帧清空当前帧耗时");

    failures += Expect(cg::world::AdvanceAnimation(state, clip, 450U), "大 delta 可跨多帧");
    failures += Expect(state.frameIndex == 2U, "大 delta 后帧索引正确");
    failures += Expect(state.elapsedMs == 50U, "大 delta 保留余量");

    failures += Expect(cg::world::AdvanceAnimation(state, clip, 50U), "循环推进成功");
    failures += Expect(state.frameIndex == 0U, "最后一帧后循环到首帧");
    failures += Expect(state.elapsedMs == 0U, "循环推进后耗时正确");
    return failures;
}

int TestSampleAnimationFrame()
{
    const cg::resource::AnimationClip clip = MakeClip();
    const cg::world::AnimationState state{1, cg::world::Direction::East, 1U, 0U};
    const std::optional<cg::world::AnimationFrameSample> sample = cg::world::SampleAnimationFrame(state, clip);

    int failures = 0;
    failures += Expect(sample.has_value(), "合法动画可采样");
    if (!sample.has_value()) {
        return failures;
    }

    failures += Expect(sample->frame != nullptr, "采样返回帧指针");
    failures += Expect(sample->graphicSeqNo == 102, "采样返回图档号");
    failures += Expect(sample->frameIndex == 1U, "采样返回安全帧索引");
    failures += Expect(sample->frameOffset == cg::world::AnimationFrameOffset{5, -6}, "采样返回帧偏移");
    return failures;
}

int TestFrameOffsets()
{
    const cg::resource::AnimeFrame frame = MakeFrame(100, -5, 8);

    int failures = 0;
    failures += Expect(
        cg::world::ComputeAnimationFrameOffset(frame) == cg::world::AnimationFrameOffset{-5, 8},
        "基础帧偏移使用 frame x/y");
    failures += Expect(
        cg::world::ComputeAnimationDrawOffset(frame, 10, -20) == cg::world::AnimationFrameOffset{5, -12},
        "绘制偏移叠加图档偏移");
    return failures;
}

int TestDirectionEastResourceValue()
{
    const cg::resource::AnimationClip clip = MakeClip(1, cg::world::Direction::East, 300);

    int failures = 0;
    failures += Expect(static_cast<int>(cg::world::Direction::East) == 2, "Direction::East 值为资源方向 2");
    failures += Expect(
        cg::world::SampleAnimationFrame({1, cg::world::Direction::East, 0U, 0U}, clip).has_value(),
        "East 可匹配资源方向 2");
    failures += Expect(
        !cg::world::SampleAnimationFrame({1, cg::world::Direction::West, 0U, 0U}, clip).has_value(),
        "非匹配方向不采样");
    return failures;
}

int TestOutOfRangeFrameIndex()
{
    const cg::resource::AnimationClip clip = MakeClip();
    const std::optional<cg::world::AnimationFrameSample> sample =
        cg::world::SampleAnimationFrame({1, cg::world::Direction::East, 5U, 0U}, clip);

    int failures = 0;
    failures += Expect(sample.has_value(), "越界帧索引可安全取模");
    if (!sample.has_value()) {
        return failures;
    }

    failures += Expect(sample->frameIndex == 2U, "越界帧索引取模后为 2");
    failures += Expect(sample->graphicSeqNo == 103, "越界帧索引采样正确帧");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSetAnimationResetRules();
    failures += TestMissingActionAndFrames();
    failures += TestFrameDuration();
    failures += TestAdvanceAnimation();
    failures += TestSampleAnimationFrame();
    failures += TestFrameOffsets();
    failures += TestDirectionEastResourceValue();
    failures += TestOutOfRangeFrameIndex();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
