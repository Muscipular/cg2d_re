#pragma once

#include <cstdint>

namespace cg::world {

// EntityId 是 world 实体的稳定强类型句柄，index 指向 EntityStore 槽位，generation 用于识别旧句柄。
class EntityId {
public:
    static constexpr std::uint32_t InvalidIndex = UINT32_MAX;

    constexpr EntityId() noexcept = default;

    constexpr EntityId(std::uint32_t index, std::uint32_t generation) noexcept
        : index_(index)
        , generation_(generation)
    {
    }

    [[nodiscard]] constexpr bool IsValid() const noexcept
    {
        return index_ != InvalidIndex && generation_ != 0U;
    }

    [[nodiscard]] constexpr std::uint32_t Index() const noexcept
    {
        return index_;
    }

    [[nodiscard]] constexpr std::uint32_t Generation() const noexcept
    {
        return generation_;
    }

    [[nodiscard]] constexpr bool operator==(const EntityId& other) const noexcept = default;

    [[nodiscard]] constexpr bool operator!=(const EntityId& other) const noexcept
    {
        return !(*this == other);
    }

private:
    std::uint32_t index_{InvalidIndex};
    std::uint32_t generation_{0U};
};

} // namespace cg::world
