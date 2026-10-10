#include "cg/world/GameTime.hpp"

#include "cg/platform/LegacyTime.hpp"

#include <bit>

namespace cg::world {

GameTime gCgTime {};
std::uint32_t gValue559508 = 0x3AE805E7;
std::uint32_t gValue73ED40 = 0;
std::uint32_t gValue73ED4C = 0;
std::uint32_t gValue73ED68 = 0;

// GTP01：原 [0x458DE0,0x458EBB)；六 W32 按 year/month/day/hours/minutes/sTick 发布，返 phase 商。
int UpdateGameTime(GameTime* output)
{
    const std::uint32_t now = static_cast<std::uint32_t>(platform::ReadLegacyTime32_53BA26(nullptr));
    const std::uint32_t base = gValue559508;
    const std::uint32_t sample = gValue73ED68;
    std::uint32_t offset = gValue73ED4C;
    offset -= base;
    const std::uint32_t adjustment = gValue73ED40;
    offset -= sample;
    std::uint32_t time = now + offset;
    time += adjustment;

    output->year = time / 2592000u;
    std::uint32_t remainder = time % 2592000u;
    output->month = remainder / 216000u;
    remainder %= 216000u;
    output->day = remainder / 7200u;
    remainder %= 7200u;
    output->hours = remainder / 300u;
    remainder %= 300u;
    output->minutes = remainder / 5u;
    const std::uint32_t phase = 192u * (time % 7200u) / 7200u + 168u;
    output->sTick = phase % 192u;
    return static_cast<int>(phase / 192u);
}

// MHT01：原 [0x458EC0,0x458EF6)；单读 hours 位型，按原有符号门槛分类，完整 body 保持。
std::int32_t GetCate_458EC0(const GameTime* time)
{
    // 保留 hours 的原位模式，后续仅按有符号值判断。
    const std::int32_t h = std::bit_cast<std::int32_t>(time->hours);
    if (19 <= h && h < 24) {
        return 2;
    }
    if (0 <= h && h < 4) {
        return 2;
    }
    if (h < 7) {
        return 3;
    }
    if (h >= 16) {
        return 1;
    }
    return 0;
}

} // namespace cg::world
