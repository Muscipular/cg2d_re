#include "AnimationRuntimeState.hpp"

namespace cg::resource::animation {

// ACL01：原45B590完整8条；先取尾前驱，再按原四次链接发布，返回零。
std::int32_t AppendAnimationSlotToTail_45B590(detail::MainSlot* const slot)
{
    detail::MainSlot* const previous = detail::gAnimationSlotListTail_9A3A00.previous;
    slot->next = &detail::gAnimationSlotListTail_9A3A00;
    slot->previous = previous;
    previous->next = slot;
    detail::gAnimationSlotListTail_9A3A00.previous = slot;
    return 0;
}

// ACL01：原45B5B0完整10条；previous/next先读齐再摘链，挂尾后返回零。
std::int32_t MoveAnimationSlotToTail_45B5B0(detail::MainSlot* const slot)
{
    detail::MainSlot* const previous = slot->previous;
    detail::MainSlot* const next = slot->next;
    previous->next = next;
    next->previous = previous;
    AppendAnimationSlotToTail_45B590(slot);
    return 0;
}

// ACL01：原45B5D0完整22条；256槽清零/index=-1及哨兵接线顺序正确，正文保持。
std::int32_t InitializeAnimationSlotList_45B5D0()
{
    detail::gAnimationSlotListTail_9A3A00.next = nullptr;
    detail::gAnimationSlots_9A29F8 = {};
    detail::gAnimationSlotListTail_9A3A00.previous = &detail::gAnimationSlotListHead_9A3A10;
    detail::gAnimationSlotListHead_9A3A10.next = &detail::gAnimationSlotListTail_9A3A00;
    detail::gAnimationSlotListHead_9A3A10.previous = nullptr;

    for (detail::MainSlot& slot : detail::gAnimationSlots_9A29F8) {
        slot.index = -1;
        AppendAnimationSlotToTail_45B590(&slot);
    }
    return 0;
}

// ACL01：原45B530完整26条；先取数量/字节再减数量，清理真实返回和oldest指针保持。
detail::MainSlot* AcquireOldestAnimationSlot_45B530()
{
    detail::MainSlot* const oldest = detail::gAnimationSlotListHead_9A3A10.next;
    if (oldest == &detail::gAnimationSlotListTail_9A3A00) {
        return nullptr;
    }
    oldest->next->previous = &detail::gAnimationSlotListHead_9A3A10;
    detail::gAnimationSlotListHead_9A3A10.next = oldest->next;
    if (Sub_45B440(&oldest->index) != 0) {
        const std::uint32_t count = detail::gLoadedAnimationCount_9A4A68;
        const std::uint32_t bytes = detail::gLoadedAnimationAccountedBytes_9A4A6C;
        detail::gLoadedAnimationCount_9A4A68 = count - 1u;
        const std::uint32_t removedBytes = oldest->accountedBytes_04;
        detail::gLoadedAnimationAccountedBytes_9A4A6C = bytes - removedBytes;
    }
    return oldest;
}

} // namespace cg::resource::animation
