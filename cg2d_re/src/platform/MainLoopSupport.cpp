#include "StartupNative.hpp"
#include "MainLoopDependencies.hpp"

#include "cg/audio/LegacyAudio.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/SurfaceState.hpp"

#include <bit>
#include <cstdint>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。
#include <random>

namespace cg::platform {

using LegacyRandomEngine =
    std::linear_congruential_engine<std::uint32_t, 0x343FDu, 0x269EC3u, 0>;

// 原0x0058F870：唯一共享W32状态，初始种子1；首次使用初始化不推进序列。
static LegacyRandomEngine& SharedRandomEngine()
{
    static LegacyRandomEngine engine{1};
    return engine;
}

// 原 0x73ED1C、0x73ED20；volatile 保留两个独立 W32 全局写入。
static volatile std::uint64_t gKeepAliveBaselineTick = 0; // 原全局：0x0073ED1C。
static volatile std::uint64_t gKeepAliveRecentTick = 0; // 原全局：0x0073ED20。

// 原 W32 0x73ED38；能力探测包装入口先写此槽，再比较返回码。
static volatile std::uint32_t gDxCapabilityCode73ED38 = 0;

static std::uint32_t RandomSymmetricW32(const std::uint32_t amplitudeBits)
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
        static_cast<std::int32_t>(Rand16()) % divisor;
    return std::bit_cast<std::uint32_t>(remainder) + lowerBits;
}

// 原函数：0x0044B5A0。
std::uint32_t ResetSystemLog_44B5A0()
{
    std::FILE* const file = std::fopen("systemlog.txt", "w");
    if (file == nullptr) {
        return 0;
    }
    return static_cast<std::uint32_t>(std::fclose(file));
}

// 原 0x52CD30：先调用 0x52CCA0，再将 0x103DE80 置 1；不读取实参。
int Sub_52CD30(std::int32_t)
{
    cg::audio::UpdateAudioPlayback_52CCA0();
    cg::audio::gValue103DE80 = 1;
    return 1;
}

std::uint64_t ReadTickCount64() noexcept
{
    static const std::uint64_t start = ::GetTickCount64();
    return ::GetTickCount64() - start;
}

// 原函数：0x00457010。
void RequestRender_457010() noexcept
{
    gForceRender = 1;
}

// 原 [0x458DC0,0x458DD5)：先发布完整 EAX32，再作无符号 <=0x500 比较。
std::int32_t CheckLegacyDxCapability_458DC0()
{
    const std::uint32_t code = std::bit_cast<std::uint32_t>(cg::audio::CheckAudioAndInputAvailability_458AE0());
    gDxCapabilityCode73ED38 = code;
    return code <= 0x500u ? 1 : 0;
}

// 原KeepAlive@[0x457AB0,0x457B03)；CTM01基线／差值64位，零哨兵与60000ms门槛保持。
void KeepAlive_457AB0()
{
    if (network::ReadNetworkRuntimeActive() == 0) {
        return;
    }

    if (gKeepAliveBaselineTick == 0) {
        const std::uint64_t tick = ReadTickCount64();
        gKeepAliveRecentTick = tick;
        gKeepAliveBaselineTick = tick;
        return;
    }

    const std::uint64_t tick = ReadTickCount64();
    const std::uint64_t baselineTick = gKeepAliveBaselineTick;
    const std::uint64_t elapsedBits = tick - baselineTick;
    gKeepAliveRecentTick = tick;
    if (std::bit_cast<std::int64_t>(elapsedBits) < 60000) {
        return;
    }

    const std::uintptr_t socket = gConnectionSocket;
    gKeepAliveBaselineTick = tick;
    static_cast<void>(network::Sub_512C60(socket));
}

// 原函数：0x0053B4BF。
std::uint32_t SeedRandom_53B4BF(std::uint32_t seed)
{
    SharedRandomEngine().seed(seed);
    return seed;
}

// 原函数：0x0053B4C9。
int Rand16()
{
    const std::uint32_t seed = SharedRandomEngine()();
    // 原 sar 后的 0x7FFF 掩码只保留原位 16..30，无符号移位得到相同结果。
    return static_cast<int>((seed >> 16) & 0x7FFFu);
}

// 原 0x46FAF0：保留上界-1哨兵；RRU01将合法闭区间改为标准均匀分布。
// 倒置区间保留原Rand16取模算法；合法区间的随机值及引擎消费次数允许变化。
std::int32_t RandomRange_46FAF0(std::int32_t lower, std::int32_t upper)
{
    if (upper == -1) {
        return 0;
    }

    if (lower <= upper) {
        std::uniform_int_distribution<std::int32_t> distribution(lower, upper);
        return distribution(SharedRandomEngine());
    }

    const std::int32_t random = Rand16();
    const std::int32_t divisor = upper - lower + 1;
    return random % divisor + lower;
}

// 原 sub_423640@[0x423640,0x4237D5)：更新偏移/幅度后，将共享 Temp 裁剪回拷 Back。
void Sub_423640()
{
    std::uint32_t offsetYBits;
    if (cg::yobi::gYobiUpdateGateMode == 1
        && cg::yobi::gYobiUpdatePhaseCounter != 0) {
        offsetYBits = cg::yobi::gYobiScreenOffsetY;
    } else {
        const std::uint32_t amplitudeX =
            cg::yobi::gYobiScreenOffsetAmplitude;
        const std::uint32_t randomXBits = RandomSymmetricW32(amplitudeX);
        cg::yobi::gYobiScreenOffsetX = randomXBits;

        const std::uint32_t amplitudeY =
            cg::yobi::gYobiScreenOffsetAmplitude;
        offsetYBits = RandomSymmetricW32(amplitudeY);
        cg::yobi::gYobiScreenOffsetY = offsetYBits;
    }

    const std::uint32_t rising =
        cg::yobi::gYobiScreenOffsetRising;
    const std::uint32_t offsetXBits =
        cg::yobi::gYobiScreenOffsetX;
    const std::uint32_t amplitude =
        cg::yobi::gYobiScreenOffsetAmplitude;
    if (rising == 1) {
        const std::uint32_t maximum =
            cg::yobi::gYobiScreenOffsetMaximumAmplitude;
        const std::uint32_t incremented = amplitude + std::uint32_t{1};
        const bool exceedsMaximum =
            std::bit_cast<std::int32_t>(incremented)
            > std::bit_cast<std::int32_t>(maximum);
        cg::yobi::gYobiScreenOffsetAmplitude = incremented;
        if (exceedsMaximum) {
            cg::yobi::gYobiScreenOffsetAmplitude = maximum;
        }
    } else {
        const std::uint32_t decremented = amplitude - std::uint32_t{1};
        cg::yobi::gYobiScreenOffsetAmplitude = decremented;
        if (std::bit_cast<std::int32_t>(decremented) < 0) {
            cg::yobi::gYobiScreenOffsetAmplitude = 0;
        }
    }

    const std::int32_t offsetX = static_cast<std::int16_t>(offsetXBits);
    const std::int32_t offsetY = static_cast<std::int16_t>(offsetYBits);
    std::int32_t destinationX = offsetX;
    std::int32_t destinationY = offsetY;
    std::int32_t sourceLeft = 0;
    std::int32_t sourceTop = 0;
    std::int32_t sourceRight = render::GetCpuSurfaceWidth();
    std::int32_t sourceBottom = render::GetCpuSurfaceHeight();

    if (destinationX < 0) {
        sourceLeft = -destinationX;
        destinationX = 0;
    }
    if (destinationY < 0) {
        sourceTop = -destinationY;
        destinationY = 0;
    }
    if (destinationX >= render::GetCpuSurfaceWidth()
        || destinationY >= render::GetCpuSurfaceHeight()
        || sourceLeft >= sourceRight
        || sourceTop >= sourceBottom) {
        return;
    }

    const std::int32_t availableWidth =
        render::GetCpuSurfaceWidth() - destinationX;
    if (sourceRight - sourceLeft > availableWidth) {
        sourceRight = sourceLeft + availableWidth;
    }
    const std::int32_t availableHeight =
        render::GetCpuSurfaceHeight() - destinationY;
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

// 原函数：0x004589D0。
std::uint64_t ReadSystemSeconds_4589D0()
{
    SYSTEMTIME systemTime;
    FILETIME fileTime;
    GetSystemTime(&systemTime);
    SystemTimeToFileTime(&systemTime, &fileTime);
    const std::uint32_t high = fileTime.dwHighDateTime;
    const std::uint32_t low = fileTime.dwLowDateTime;
    // 原53CB10：保持U64整除，直接由MSVC生成对应运算。
    return ((static_cast<std::uint64_t>(high) << 32) | low) / 10000000u;
}

} // namespace cg::platform
