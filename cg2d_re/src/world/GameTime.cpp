#include "cg/world/GameTime.hpp"

#include "cg/platform/LegacyTime.hpp"

namespace cg::world {

GameTime gCgTime {};
std::uint32_t gValue559508 = 0x3AE805E7;
std::uint32_t gValue73ED40 = 0;
std::uint32_t gValue73ED4C = 0;
std::uint32_t gValue73ED68 = 0;

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

} // namespace cg::world
