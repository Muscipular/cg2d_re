#include <cstdio> // 原CRT：0x0053B1D0；直接沿MSVC文件流。
#include "cg/resource/AnimationRuntime.hpp"

#include "AnimationRuntimeState.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::resource::animation {

// ACL01：原45B440完整78条复用ATP01；index/held table条件重取、同快照free/原末发布保持。
std::int32_t ReleaseAnimationSlotResources_45B440(std::int32_t* const slotIndex)
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
std::int32_t ReleaseAnimationCacheSlotResources_45B650(std::int32_t* const slotIndex)
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
std::int32_t ReleaseAllAnimationSlotResources_45B630()
{
    std::int32_t result = 0;
    for (detail::MainSlot& slot : detail::gAnimationSlots_9A29F8) {
        result = ReleaseAnimationSlotResources_45B440(&slot.index);
    }
    return result;
}

// ACL01：原45B740完整10条；固定256个实际P4槽清理，返回最后一次S32结果，正文保持。
std::int32_t ReleaseAllAnimationCacheSlotResources_45B740()
{
    std::int32_t result = 0;
    for (detail::CacheSlot& slot : detail::gCacheSlots9A3A48) {
        result = ReleaseAnimationCacheSlotResources_45B650(&slot.index);
    }
    return result;
}

// BLM01：共享坐标FILE只关闭一次；先清同值别名，再关闭该文件。
void CloseCoordinateFiles()
{
    for (std::FILE*& file : detail::gCoordinateFiles9A2988) {
        std::FILE* const current = file;
        if (current == nullptr) {
            continue;
        }
        for (std::FILE*& alias : detail::gCoordinateFiles9A2988) {
            if (alias == current) {
                alias = nullptr;
            }
        }
        std::fclose(current);
    }
}

static void CloseAnimationFiles()
{
    for (std::FILE*& file : detail::gAnimateFiles9A3A20) {
        std::FILE* const current = file;
        if (current != nullptr) {
            std::fclose(current);
            file = nullptr;
        }
    }
}

// 原函数：0x0045BEF0。
// BLM01：关闭32动画槽和共享坐标文件，再按原活动槽清理并返回真实末值。
std::int32_t AnimeUnload_45BEF0()
{
    CloseAnimationFiles();
    CloseCoordinateFiles();
    return ReleaseAllAnimationSlotResources_45B630();
}

// ACL01：原45BF20；BLM01标志槽扩为32，MC01-B已批清FILE owner差异保持。
std::int32_t CloseFlaggedAnimationFilesAndClearCache_45BF20()
{
    for (std::size_t slot = 0; slot < detail::gFileCloseFlags9A29D0.size(); ++slot) {
        if (detail::gFileCloseFlags9A29D0[slot] != 0) {
            std::fclose(detail::gAnimateFiles9A3A20[slot]);
            // MC01-B：释放已关闭FILE的owner引用，后续总清理不得重复关闭。
            detail::gAnimateFiles9A3A20[slot] = nullptr;
            detail::gFileCloseFlags9A29D0[slot] = 0;
        }
    }
    return ReleaseAllAnimationCacheSlotResources_45B740();
}

void ShutdownAnimationResources()
{
    CloseAnimationFiles();
    detail::gFileCloseFlags9A29D0.fill(0);
    CloseCoordinateFiles();
    if (gAnimateRecords9A4A58 != nullptr) {
        static_cast<void>(ReleaseAllAnimationSlotResources_45B630());
    }
    if (gAnimateCache9A4A5C != nullptr) {
        static_cast<void>(ReleaseAllAnimationCacheSlotResources_45B740());
    }
}

} // namespace cg::resource::animation
