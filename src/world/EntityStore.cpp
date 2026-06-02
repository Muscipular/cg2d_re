#include "world/EntityStore.hpp"

#include <utility>

namespace cg::world {
namespace {

[[nodiscard]] std::uint32_t NextGeneration(std::uint32_t generation) noexcept
{
    ++generation;
    return generation == 0U ? 1U : generation;
}

} // namespace

void EntityStore::Reserve(std::size_t capacity)
{
    slots_.reserve(capacity);
    freeList_.reserve(capacity);
}

EntityId EntityStore::Create(EntityData data)
{
    std::uint32_t index = 0U;
    if (!freeList_.empty()) {
        index = freeList_.back();
        freeList_.pop_back();
    } else {
        index = static_cast<std::uint32_t>(slots_.size());
        slots_.push_back(Slot{});
    }

    Slot& slot = slots_[index];
    const EntityId id{index, slot.generation};
    data.id = id;
    slot.data = std::move(data);
    slot.occupied = true;
    ++activeCount_;
    return id;
}

bool EntityStore::Destroy(EntityId id)
{
    if (!IsLive(id)) {
        return false;
    }

    Slot& slot = slots_[id.Index()];
    slot.data = EntityData{};
    slot.occupied = false;
    slot.generation = NextGeneration(slot.generation);
    freeList_.push_back(id.Index());
    --activeCount_;
    return true;
}

EntityData* EntityStore::Find(EntityId id) noexcept
{
    if (!IsLive(id)) {
        return nullptr;
    }

    return &slots_[id.Index()].data;
}

const EntityData* EntityStore::Find(EntityId id) const noexcept
{
    if (!IsLive(id)) {
        return nullptr;
    }

    return &slots_[id.Index()].data;
}

std::size_t EntityStore::ActiveCount() const noexcept
{
    return activeCount_;
}

void EntityStore::Clear()
{
    freeList_.clear();
    freeList_.reserve(slots_.size());

    for (std::size_t index = 0U; index < slots_.size(); ++index) {
        Slot& slot = slots_[index];
        if (slot.occupied) {
            slot.data = EntityData{};
            slot.occupied = false;
            slot.generation = NextGeneration(slot.generation);
        }
        freeList_.push_back(static_cast<std::uint32_t>(index));
    }

    activeCount_ = 0U;
}

bool EntityStore::IsLive(EntityId id) const noexcept
{
    if (!id.IsValid() || id.Index() >= slots_.size()) {
        return false;
    }

    const Slot& slot = slots_[id.Index()];
    return slot.occupied && slot.generation == id.Generation();
}

} // namespace cg::world
