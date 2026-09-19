#include "StartupNative.hpp"
#include "MainLoopDependencies.hpp"

#include "cg/platform/MainLoop.hpp"

#include <bit>

namespace cg::platform {

std::uint32_t gRandomSeed = 1; // 原 0x58F870，共享随机源，不改用本机 CRT 的私有状态。

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
