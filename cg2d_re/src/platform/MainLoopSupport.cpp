#include "StartupNative.hpp"
#include "MainLoopDependencies.hpp"

#include "cg/LegacyAudio.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/SurfaceState.hpp"

#include <bit>
#include <cstdint>

namespace cg::network {

std::int32_t Sub_512C60(std::uintptr_t socket); // 原 0x512C60；真实发送实现尚缺。

} // namespace cg::network

namespace cg::audio {

void Sub_52CCA0(); // 原 0x52CCA0，IDA 为 void __cdecl()；真实实现尚缺。

} // namespace cg::audio

namespace cg::platform {
namespace {

// 原 0x73ED1C、0x73ED20；volatile 保留两个独立 W32 全局写入。
volatile std::uint32_t gKeepAliveBaselineTick = 0;
volatile std::uint32_t gKeepAliveRecentTick = 0;

std::uint32_t RandomSymmetricW32(const std::uint32_t amplitudeBits)
{
    const std::uint32_t lowerBits = std::uint32_t{0} - amplitudeBits;
    const std::uint32_t upperBits = amplitudeBits;
    if (upperBits + std::uint32_t{1} == 0) {
        return 0;
    }

    // 对称 W32 区间宽度恒为奇数，因此 signed32 除数不可能为 0。
    const std::uint32_t divisorBits =
        upperBits - lowerBits + std::uint32_t{1};
    const std::int32_t divisor =
        std::bit_cast<std::int32_t>(divisorBits);
    const std::int32_t remainder =
        static_cast<std::int32_t>(NextRandom_53B4C9()) % divisor;
    return std::bit_cast<std::uint32_t>(remainder) + lowerBits;
}

std::int32_t SignedLowWord(const std::uint32_t valueBits) noexcept
{
    return static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(
            static_cast<std::uint16_t>(valueBits)));
}

} // namespace

std::uint32_t gRandomSeed = 1; // 原 0x58F870，共享随机源，不改用本机 CRT 的私有状态。

// 原 0x52CD30：先调用 0x52CCA0，再将 0x103DE80 置 1；不读取实参。
int Sub_52CD30(std::int32_t)
{
    cg::audio::Sub_52CCA0();
    cg::audio::gValue103DE80 = 1;
    return 1;
}

std::uint32_t ReadTickCount32() noexcept
{
    return static_cast<std::uint32_t>(GetTickCount());
}

// 原 KeepAlive@[0x457AB0,0x457B03)：保留零基线哨兵及 signed32 差值门槛。
void KeepAlive_457AB0()
{
    if (network::ReadNetworkRuntimeActive() == 0) {
        return;
    }

    if (gKeepAliveBaselineTick == 0) {
        const std::uint32_t tick = ReadTickCount32();
        gKeepAliveRecentTick = tick;
        gKeepAliveBaselineTick = tick;
        return;
    }

    const std::uint32_t tick = ReadTickCount32();
    const std::uint32_t baselineTick = gKeepAliveBaselineTick;
    const std::uint32_t elapsedBits = tick - baselineTick;
    gKeepAliveRecentTick = tick;
    if (std::bit_cast<std::int32_t>(elapsedBits) < 60000) {
        return;
    }

    const std::uintptr_t socket = gConnectionSocket;
    gKeepAliveBaselineTick = tick;
    static_cast<void>(network::Sub_512C60(socket));
}

std::uint32_t SeedRandom_53B4BF(std::uint32_t seed)
{
    gRandomSeed = seed;
    return seed;
}

int NextRandom_53B4C9()
{
    const std::uint32_t seed = gRandomSeed * 0x343FDu + 0x269EC3u;
    gRandomSeed = seed;
    // 原 sar 后的 0x7FFF 掩码只保留原位 16..30，无符号移位得到相同结果。
    return static_cast<int>((seed >> 16) & 0x7FFFu);
}

// 原 0x46FAF0：上界为 -1 时不消费随机数；否则先取随机数，再计算有符号区间。
std::int32_t RandomRange_46FAF0(std::int32_t lower, std::int32_t upper)
{
    if (upper == -1) {
        return 0;
    }

    const std::int32_t random = NextRandom_53B4C9();
    const std::int32_t divisor = upper - lower + 1;
    return random % divisor + lower;
}

// 原 sub_423640@[0x423640,0x4237D5)：更新偏移/幅度后，将共享 Temp 裁剪回拷 Back。
void Sub_423640()
{
    std::uint32_t offsetYBits;
    if (login::detail::gYobiUpdateGateMode == 1
        && login::detail::gYobiUpdatePhaseCounter != 0) {
        offsetYBits = login::detail::gYobiScreenOffsetY;
    } else {
        const std::uint32_t amplitudeX =
            login::detail::gYobiScreenOffsetAmplitude;
        const std::uint32_t randomXBits = RandomSymmetricW32(amplitudeX);
        login::detail::gYobiScreenOffsetX = randomXBits;

        const std::uint32_t amplitudeY =
            login::detail::gYobiScreenOffsetAmplitude;
        offsetYBits = RandomSymmetricW32(amplitudeY);
        login::detail::gYobiScreenOffsetY = offsetYBits;
    }

    const std::uint32_t rising =
        login::detail::gYobiScreenOffsetRising;
    const std::uint32_t offsetXBits =
        login::detail::gYobiScreenOffsetX;
    const std::uint32_t amplitude =
        login::detail::gYobiScreenOffsetAmplitude;
    if (rising == 1) {
        const std::uint32_t maximum =
            login::detail::gYobiScreenOffsetMaximumAmplitude;
        const std::uint32_t incremented = amplitude + std::uint32_t{1};
        const bool exceedsMaximum =
            std::bit_cast<std::int32_t>(incremented)
            > std::bit_cast<std::int32_t>(maximum);
        login::detail::gYobiScreenOffsetAmplitude = incremented;
        if (exceedsMaximum) {
            login::detail::gYobiScreenOffsetAmplitude = maximum;
        }
    } else {
        const std::uint32_t decremented = amplitude - std::uint32_t{1};
        login::detail::gYobiScreenOffsetAmplitude = decremented;
        if (std::bit_cast<std::int32_t>(decremented) < 0) {
            login::detail::gYobiScreenOffsetAmplitude = 0;
        }
    }

    const std::int32_t offsetX = SignedLowWord(offsetXBits);
    const std::int32_t offsetY = SignedLowWord(offsetYBits);
    std::int32_t destinationX = offsetX;
    std::int32_t destinationY = offsetY;
    std::int32_t sourceLeft = 0;
    std::int32_t sourceTop = 0;
    std::int32_t sourceRight = render::kCpuSurfaceWidth;
    std::int32_t sourceBottom = render::kCpuSurfaceHeight;

    if (destinationX < 0) {
        sourceLeft = -destinationX;
        destinationX = 0;
    }
    if (destinationY < 0) {
        sourceTop = -destinationY;
        destinationY = 0;
    }
    if (destinationX >= render::kCpuSurfaceWidth
        || destinationY >= render::kCpuSurfaceHeight
        || sourceLeft >= sourceRight
        || sourceTop >= sourceBottom) {
        return;
    }

    const std::int32_t availableWidth =
        render::kCpuSurfaceWidth - destinationX;
    if (sourceRight - sourceLeft > availableWidth) {
        sourceRight = sourceLeft + availableWidth;
    }
    const std::int32_t availableHeight =
        render::kCpuSurfaceHeight - destinationY;
    if (sourceBottom - sourceTop > availableHeight) {
        sourceBottom = sourceTop + availableHeight;
    }
    if (sourceLeft >= sourceRight || sourceTop >= sourceBottom) {
        return;
    }

    static_cast<void>(render::BlitTempToBack(
        destinationX,
        destinationY,
        sourceLeft,
        sourceTop,
        sourceRight,
        sourceBottom));
}

std::uint64_t ReadSystemSeconds_4589D0()
{
    SYSTEMTIME systemTime;
    FILETIME fileTime;
    GetSystemTime(&systemTime);
    SystemTimeToFileTime(&systemTime, &fileTime);
    const std::uint32_t high = fileTime.dwHighDateTime;
    const std::uint32_t low = fileTime.dwLowDateTime;
    return ((static_cast<std::uint64_t>(high) << 32) | low) / 10000000u;
}

int CheckSystemSeconds_458A10(std::int32_t seconds)
{
    const std::uint32_t now = static_cast<std::uint32_t>(ReadSystemSeconds_4589D0());
    const std::uint32_t elapsed = now - gLastSystemTime.low;
    return std::bit_cast<std::int32_t>(elapsed) >= seconds && seconds > 0 ? 1 : 0;
}

} // namespace cg::platform
