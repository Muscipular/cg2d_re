#pragma once

#include "world/EntityData.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace cg::world {

// EntityStore 维护 EntityData 槽位、空闲列表与 generation，使删除后的旧 EntityId 失效。
class EntityStore {
public:
    void Reserve(std::size_t capacity);

    [[nodiscard]] EntityId Create(EntityData data = {});
    [[nodiscard]] bool Destroy(EntityId id);

    [[nodiscard]] EntityData* Find(EntityId id) noexcept;
    [[nodiscard]] const EntityData* Find(EntityId id) const noexcept;

    [[nodiscard]] std::size_t ActiveCount() const noexcept;

    void Clear();

private:
    struct Slot {
        EntityData data{};
        std::uint32_t generation{1U};
        bool occupied{};
    };

    [[nodiscard]] bool IsLive(EntityId id) const noexcept;

    std::vector<Slot> slots_;
    std::vector<std::uint32_t> freeList_;
    std::size_t activeCount_{};
};

} // namespace cg::world
