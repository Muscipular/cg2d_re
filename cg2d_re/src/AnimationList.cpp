#include "AnimationRuntimeState.hpp"

namespace cg::resource::animation {

std::int32_t Sub_45B590(detail::MainSlot* const slot)
{
    slot->next = &detail::gLeftSentinel9A3A00;
    slot->previous = detail::gLeftSentinel9A3A00.previous;
    slot->previous->next = slot;
    detail::gLeftSentinel9A3A00.previous = slot;
    return 0;
}

std::int32_t Sub_45B5B0(detail::MainSlot* const slot)
{
    slot->previous->next = slot->next;
    slot->next->previous = slot->previous;
    Sub_45B590(slot);
    return 0;
}

std::int32_t Sub_45B5D0()
{
    detail::gLeftSentinel9A3A00.next = nullptr;
    detail::gMainSlots9A29F8 = {};
    detail::gLeftSentinel9A3A00.previous = &detail::gRightSentinel9A3A10;
    detail::gRightSentinel9A3A10.next = &detail::gLeftSentinel9A3A00;
    detail::gRightSentinel9A3A10.previous = nullptr;

    for (detail::MainSlot& slot : detail::gMainSlots9A29F8) {
        slot.index = -1;
        Sub_45B590(&slot);
    }
    return 0;
}

detail::MainSlot* Sub_45B530()
{
    detail::MainSlot* const oldest = detail::gRightSentinel9A3A10.next;
    if (oldest == &detail::gLeftSentinel9A3A00) {
        return nullptr;
    }
    oldest->next->previous = &detail::gRightSentinel9A3A10;
    detail::gRightSentinel9A3A10.next = oldest->next;
    if (Sub_45B440(&oldest->index) != 0) {
        --detail::gActiveCount9A4A68;
        detail::gActiveBytes9A4A6C -= oldest->value04;
    }
    return oldest;
}

} // namespace cg::resource::animation
