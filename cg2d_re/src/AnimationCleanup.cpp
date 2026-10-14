#include <cstdio> // 原CRT：0x0053B1D0；直接沿MSVC文件流。
#include "cg/AnimationRuntime.hpp"

#include "AnimationRuntimeState.hpp"
#include "platform/LegacyStdio.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::resource::animation {

// ACL01：原45B440完整78条复用ATP01；index/held table条件重取、同快照free/原末发布保持。
std::int32_t Sub_45B440(std::int32_t* const slotIndex)
{
    std::int32_t index = *slotIndex;
    if (index < 0) {
        return 0;
    }

    AnimateRecord* table = gAnimateRecords9A4A58;
    AnimateRecord* record = &table[index];
    std::uint16_t count = record->count;
    std::int32_t action = 0;
    if (count != 0) {
        for (;;) {
            detail::ActionEntry* const list = record->actionList;
            detail::FrameEntry* const frames = list[action].frames;
            if (frames != nullptr) {
                std::free(frames);
                index = *slotIndex;
                table = gAnimateRecords9A4A58;
                table[index].actionList[action].frames = nullptr;
                table = gAnimateRecords9A4A58;
            }
            index = *slotIndex;
            ++action;
            record = &table[index];
            count = record->count;
            if (action >= count) {
                break;
            }
        }
    }

    index = *slotIndex;
    detail::ActionEntry* const list = table[index].actionList;
    if (list != nullptr) {
        std::free(list);
        table = gAnimateRecords9A4A58;
    }
    index = *slotIndex;
    table[index].actionList = nullptr;
    index = *slotIndex;
    table = gAnimateRecords9A4A58;
    table[index].listNode = nullptr;
    index = *slotIndex;
    table = gAnimateRecords9A4A58;
    detail::CoordinateEntry* const coordinates = table[index].coordinates;
    if (coordinates != nullptr) {
        std::free(coordinates);
    }
    index = *slotIndex;
    table = gAnimateRecords9A4A58;
    table[index].coordinates = nullptr;
    *slotIndex = -1;
    return 1;
}

// ACL01：原45B650完整78条复用ATP01；count单W16，held table/条件重取与同快照free，不猜P4生产。
std::int32_t Sub_45B650(std::int32_t* const slotIndex)
{
    std::int32_t index = *slotIndex;
    if (index < 0) {
        return 0;
    }

    AnimeCacheRecord* table = gAnimateCache9A4A5C;
    AnimeCacheRecord* record = &table[index];
    std::uint16_t count;
    std::memcpy(&count, &record->values[1], sizeof(count));
    std::int32_t action = 0;
    if (count != 0) {
        for (;;) {
            detail::CacheActionEntry* const list = record->actionList;
            void* const frames = list[action].frames;
            if (frames != nullptr) {
                std::free(frames);
                index = *slotIndex;
                table = gAnimateCache9A4A5C;
                table[index].actionList[action].frames = nullptr;
                table = gAnimateCache9A4A5C;
            }
            index = *slotIndex;
            ++action;
            record = &table[index];
            std::memcpy(&count, &record->values[1], sizeof(count));
            if (action >= count) {
                break;
            }
        }
    }

    index = *slotIndex;
    detail::CacheActionEntry* const list = table[index].actionList;
    if (list != nullptr) {
        std::free(list);
        table = gAnimateCache9A4A5C;
    }
    index = *slotIndex;
    table[index].actionList = nullptr;
    index = *slotIndex;
    table = gAnimateCache9A4A5C;
    table[index].values[4] = 0;
    index = *slotIndex;
    table = gAnimateCache9A4A5C;
    void* const buffer = table[index].buffer;
    if (buffer != nullptr) {
        std::free(buffer);
    }
    index = *slotIndex;
    table = gAnimateCache9A4A5C;
    table[index].buffer = nullptr;
    *slotIndex = -1;
    return 1;
}

// ACL01：原45B630完整10条；固定256个实际主槽清理，返回最后一次S32结果，正文保持。
std::int32_t Sub_45B630()
{
    std::int32_t result = 0;
    for (detail::MainSlot& slot : detail::gMainSlots9A29F8) {
        result = Sub_45B440(&slot.index);
    }
    return result;
}

// ACL01：原45B740完整10条；固定256个实际P4槽清理，返回最后一次S32结果，正文保持。
std::int32_t Sub_45B740()
{
    std::int32_t result = 0;
    for (detail::CacheSlot& slot : detail::gCacheSlots9A3A48) {
        result = Sub_45B650(&slot.index);
    }
    return result;
}

// ACL01：原45BEF0完整15条；九FILE一次取样/同值关闭后清槽，再scan主表，返回真实末值。
std::int32_t AnimeUnload_45BEF0()
{
    for (platform::LegacyFile*& file : detail::gAnimateFiles9A3A20) {
        platform::LegacyFile* const current = file;
        if (current != nullptr) {
            std::fclose(current);
            file = nullptr;
        }
    }
    return Sub_45B630();
}

// ACL01：原45BF20完整16条；九标志关闭后scanP4，MC01-B已批清FILE owner差异保持。
std::int32_t Sub_45BF20()
{
    for (std::size_t slot = 0; slot < detail::gFileCloseFlags9A29D0.size(); ++slot) {
        if (detail::gFileCloseFlags9A29D0[slot] != 0) {
            std::fclose(detail::gAnimateFiles9A3A20[slot]);
            // MC01-B：释放已关闭FILE的owner引用，后续总清理不得重复关闭。
            detail::gAnimateFiles9A3A20[slot] = nullptr;
            detail::gFileCloseFlags9A29D0[slot] = 0;
        }
    }
    return Sub_45B740();
}

} // namespace cg::resource::animation
