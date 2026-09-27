#include "cg/AnimationRuntime.hpp"

#include "AnimationRuntimeState.hpp"

namespace cg::resource::animation::detail {

std::array<std::uint32_t, 9> gFileCloseFlags9A29D0 {};
std::array<MainSlot, 256> gMainSlots9A29F8 {};
std::array<std::FILE*, 9> gAnimateFiles9A3A20 {};
std::array<CacheSlot, 256> gCacheSlots9A3A48 {};

} // namespace cg::resource::animation::detail

namespace cg::resource::animation {

AnimateRecord* gAnimateRecords9A4A58 = nullptr;
AnimeCacheRecord* gAnimateCache9A4A5C = nullptr;
std::int32_t gAnimateCount9A4A60 = 0;
std::int32_t gMaxAnimateCount9A4A4C = 0;

std::int32_t __cdecl Compare_45B420(
    const void* const left, const void* const right)
{
    const auto* const a = static_cast<const AnimateRecord*>(left);
    const auto* const b = static_cast<const AnimateRecord*>(right);
    if (a->animeNo < b->animeNo) {
        return -1;
    }
    return a->animeNo > b->animeNo ? 1 : 0;
}

std::int32_t __cdecl BinarySearch_45B390(
    const std::uint32_t animeNo, std::int32_t min, std::int32_t max)
{
    if (min > max) {
        return -1;
    }
    while (max - min >= 10) {
        const std::int32_t middle = min + (max - min) / 2;
        const std::uint32_t current =
            gAnimateRecords9A4A58[middle].animeNo;
        if (current == animeNo) {
            return middle;
        }
        if (current > animeNo) {
            max = middle - 1;
        } else {
            min = middle + 1;
        }
        if (min > max) {
            return -1;
        }
    }
    for (std::int32_t index = min; index <= max; ++index) {
        if (gAnimateRecords9A4A58[index].animeNo == animeNo) {
            return index;
        }
    }
    return -1;
}

std::int32_t __cdecl GetIndex_45B400(const std::uint32_t animeNo)
{
    const std::int32_t result =
        BinarySearch_45B390(animeNo, 0, gAnimateCount9A4A60);
    return result < 0 ? -1 : result;
}

} // namespace cg::resource::animation
