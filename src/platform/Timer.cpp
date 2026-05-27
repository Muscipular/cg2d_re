#include "platform/Timer.hpp"

#include <SDL3/SDL_timer.h>

namespace cg::platform {
namespace {

constexpr double NanosPerSecond = 1000000000.0;

[[nodiscard]] double NanosToSeconds(Uint64 nanos) noexcept
{
    return static_cast<double>(nanos) / NanosPerSecond;
}

[[nodiscard]] Uint64 SecondsToNanos(double seconds) noexcept
{
    return static_cast<Uint64>(seconds * NanosPerSecond);
}

} // namespace

FrameTimer::FrameTimer() noexcept
{
    Reset();
}

void FrameTimer::Reset() noexcept
{
    lastTicksNs_ = SDL_GetTicksNS();
    deltaSeconds_ = 0.0;
    totalSeconds_ = 0.0;
}

double FrameTimer::Tick() noexcept
{
    const Uint64 currentTicksNs = SDL_GetTicksNS();
    const Uint64 elapsedTicksNs = currentTicksNs - lastTicksNs_;

    lastTicksNs_ = currentTicksNs;
    deltaSeconds_ = NanosToSeconds(elapsedTicksNs);
    totalSeconds_ += deltaSeconds_;
    return deltaSeconds_;
}

double FrameTimer::GetDeltaSeconds() const noexcept
{
    return deltaSeconds_;
}

double FrameTimer::GetTotalSeconds() const noexcept
{
    return totalSeconds_;
}

void FrameTimer::DelayToCap(double targetFps) const noexcept
{
    if (targetFps <= 0.0) {
        return;
    }

    const Uint64 targetFrameNs = SecondsToNanos(1.0 / targetFps);
    if (targetFrameNs == 0) {
        return;
    }

    const Uint64 currentTicksNs = SDL_GetTicksNS();
    const Uint64 elapsedTicksNs = currentTicksNs - lastTicksNs_;
    if (elapsedTicksNs >= targetFrameNs) {
        return;
    }

    SDL_DelayNS(targetFrameNs - elapsedTicksNs);
}

FixedStepTimer::FixedStepTimer(double fixedDelta) noexcept
    : fixedDelta_(fixedDelta)
{
}

void FixedStepTimer::Add(double delta) noexcept
{
    if (delta <= 0.0 || fixedDelta_ <= 0.0) {
        return;
    }

    accumulator_ += delta;
}

bool FixedStepTimer::ConsumeStep() noexcept
{
    if (fixedDelta_ <= 0.0 || accumulator_ < fixedDelta_) {
        return false;
    }

    accumulator_ -= fixedDelta_;
    return true;
}

double FixedStepTimer::GetAlpha() const noexcept
{
    if (fixedDelta_ <= 0.0) {
        return 0.0;
    }

    return accumulator_ / fixedDelta_;
}

} // namespace cg::platform
