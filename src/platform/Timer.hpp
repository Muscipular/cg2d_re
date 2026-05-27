#pragma once

#include <SDL3/SDL_stdinc.h>

namespace cg::platform {

// FrameTimer 负责记录帧间隔、累计运行时间，并按目标帧率等待。
class FrameTimer {
public:
    FrameTimer() noexcept;

    // Reset 重置计时基准，清空 delta 和累计时间。
    void Reset() noexcept;

    // Tick 更新当前帧 delta，返回距离上次 Tick/Reset 的秒数。
    [[nodiscard]] double Tick() noexcept;

    // GetDeltaSeconds 获取最近一次 Tick 得到的帧间隔秒数。
    [[nodiscard]] double GetDeltaSeconds() const noexcept;

    // GetTotalSeconds 获取自 Reset 后累计的 Tick 时间秒数。
    [[nodiscard]] double GetTotalSeconds() const noexcept;

    // DelayToCap 按目标 FPS 补足当前帧耗时；targetFps <= 0 时不等待。
    void DelayToCap(double targetFps) const noexcept;

private:
    Uint64 lastTicksNs_ = 0;
    double deltaSeconds_ = 0.0;
    double totalSeconds_ = 0.0;
};

// FixedStepTimer 负责把可变帧间隔转换为固定步长消费。
class FixedStepTimer {
public:
    // fixedDelta 为每个固定更新步长的秒数；fixedDelta <= 0 时不会消费 step。
    explicit FixedStepTimer(double fixedDelta) noexcept;

    // Add 累加一段可变帧间隔；delta <= 0 时忽略。
    void Add(double delta) noexcept;

    // ConsumeStep 消费一个固定步长；成功消费返回 true。
    [[nodiscard]] bool ConsumeStep() noexcept;

    // GetAlpha 获取剩余累计时间相对 fixedDelta 的比例，用于插值。
    [[nodiscard]] double GetAlpha() const noexcept;

private:
    double fixedDelta_ = 0.0;
    double accumulator_ = 0.0;
};

} // namespace cg::platform
