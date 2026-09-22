#include "StartupNative.hpp"
#include "MainLoopDependencies.hpp"

#include "cg/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"

#include <bit>

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
