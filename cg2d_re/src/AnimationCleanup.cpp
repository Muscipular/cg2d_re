#include "cg/AnimationRuntime.hpp"

#include "AnimationRuntimeState.hpp"
#include "platform/LegacyStdio.hpp"

#include <cstdlib>

namespace cg::resource::animation {

std::int32_t Sub_45B440(std::int32_t* const slotIndex)
{
    if (*slotIndex < 0) {
        return 0;
    }

    for (std::uint32_t action = 0;
         action < gAnimateRecords9A4A58[*slotIndex].count; ++action) {
        auto* const list = static_cast<detail::ActionEntry*>(
            gAnimateRecords9A4A58[*slotIndex].actionList);
        if (list[action].frames != nullptr) {
            std::free(list[action].frames);
            static_cast<detail::ActionEntry*>(
                gAnimateRecords9A4A58[*slotIndex].actionList)[action].frames = nullptr;
        }
    }

    if (gAnimateRecords9A4A58[*slotIndex].actionList != nullptr) {
        std::free(gAnimateRecords9A4A58[*slotIndex].actionList);
    }
    gAnimateRecords9A4A58[*slotIndex].actionList = nullptr;
    gAnimateRecords9A4A58[*slotIndex].listNode = nullptr;
    if (gAnimateRecords9A4A58[*slotIndex].coordinates != nullptr) {
        std::free(gAnimateRecords9A4A58[*slotIndex].coordinates);
    }
    gAnimateRecords9A4A58[*slotIndex].coordinates = nullptr;
    *slotIndex = -1;
    return 1;
}

std::int32_t Sub_45B650(std::int32_t* const slotIndex)
{
    if (*slotIndex < 0) {
        return 0;
    }

    for (std::uint32_t action = 0;
         action < (gAnimateCache9A4A5C[*slotIndex].values[1] & 0xFFFFu);
         ++action) {
        auto* const list = static_cast<detail::CacheActionEntry*>(
            gAnimateCache9A4A5C[*slotIndex].actionList);
        if (list[action].frames != nullptr) {
            std::free(list[action].frames);
            static_cast<detail::CacheActionEntry*>(
                gAnimateCache9A4A5C[*slotIndex].actionList)[action].frames = nullptr;
        }
    }

    if (gAnimateCache9A4A5C[*slotIndex].actionList != nullptr) {
        std::free(gAnimateCache9A4A5C[*slotIndex].actionList);
    }
    gAnimateCache9A4A5C[*slotIndex].actionList = nullptr;
    gAnimateCache9A4A5C[*slotIndex].values[4] = 0;
    if (gAnimateCache9A4A5C[*slotIndex].buffer != nullptr) {
        std::free(gAnimateCache9A4A5C[*slotIndex].buffer);
    }
    gAnimateCache9A4A5C[*slotIndex].buffer = nullptr;
    *slotIndex = -1;
    return 1;
}

std::int32_t Sub_45B630()
{
    std::int32_t result = 0;
    for (detail::MainSlot& slot : detail::gMainSlots9A29F8) {
        result = Sub_45B440(&slot.index);
    }
    return result;
}

std::int32_t Sub_45B740()
{
    std::int32_t result = 0;
    for (detail::CacheSlot& slot : detail::gCacheSlots9A3A48) {
        result = Sub_45B650(&slot.index);
    }
    return result;
}

std::int32_t AnimeUnload_45BEF0()
{
    for (platform::LegacyFile*& file : detail::gAnimateFiles9A3A20) {
        if (file != nullptr) {
            platform::CloseLegacyStream_53B1D0(file);
            file = nullptr;
        }
    }
    return Sub_45B630();
}

std::int32_t Sub_45BF20()
{
    for (std::size_t slot = 0; slot < detail::gFileCloseFlags9A29D0.size(); ++slot) {
        if (detail::gFileCloseFlags9A29D0[slot] != 0) {
            platform::CloseLegacyStream_53B1D0(detail::gAnimateFiles9A3A20[slot]);
            detail::gFileCloseFlags9A29D0[slot] = 0;
        }
    }
    return Sub_45B740();
}

} // namespace cg::resource::animation
