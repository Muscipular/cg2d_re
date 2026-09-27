#include "WorldObjectState.hpp"

#include "WorldUpdateDependencies.hpp"
#include "cg/world/WorldObjects.hpp"

namespace cg::world {
namespace detail {

std::array<WorldObjectRecord, kWorldObjectCapacity> gWorldObjects {};
std::int32_t gWorldObjectFindCursor = 0;
std::int32_t gWorldObjectFreeCursor = 0;
std::int32_t gWorldObjectUsedCount = 0;
std::int32_t gWorldObjectUpperBound = 0;

std::int32_t Sub_444FA0(const std::int32_t objectIndex)
{
    const std::int32_t start = gWorldObjectFindCursor;
    const std::int32_t end = gWorldObjectUpperBound;
    for (std::int32_t index = start; index < end; ++index) {
        const auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0 && record.objectIndex == objectIndex) {
            gWorldObjectFindCursor = index;
            return index;
        }
    }
    for (std::int32_t index = 0; index < start; ++index) {
        const auto& record = gWorldObjects[static_cast<std::size_t>(index)];
        if (record.used != 0 && record.objectIndex == objectIndex) {
            gWorldObjectFindCursor = index;
            return index;
        }
    }
    return -1;
}

std::int32_t Sub_446130()
{
    if (gWorldObjectUsedCount >= 1000) {
        return -1;
    }
    const std::int32_t used = gWorldObjectUsedCount;
    const std::int32_t upperBound = gWorldObjectUpperBound;
    const std::int32_t selected = gWorldObjectFreeCursor;
    gWorldObjectUsedCount = used + 1;
    std::int32_t next = selected + 1;
    if (next > upperBound) {
        gWorldObjectUpperBound = next;
    }
    for (; next < 1000; ++next) {
        if (gWorldObjects[static_cast<std::size_t>(next)].used == 0) {
            gWorldObjectFreeCursor = next;
            break;
        }
    }
    if (selected > 0 && next >= 1000) {
        for (std::int32_t index = 0; index < selected; ++index) {
            if (gWorldObjects[static_cast<std::size_t>(index)].used == 0) {
                gWorldObjectFreeCursor = index;
                break;
            }
        }
    }
    return selected;
}

void Sub_446470()
{
    gWorldObjectUsedCount = 0;
    gWorldObjectUpperBound = 0;
    gWorldObjectFreeCursor = 0;
    for (auto& record : gWorldObjects) {
        record.used = 0;
        record.action = nullptr;
        record.flags42 = 0;
        record.flags120 = 0;
        record.objectIndex = 0;
        record.value34 = -1;
        record.bytes54[0] = std::byte{0};
    }
    gWorldObjectFindCursor = 0;
}

} // namespace detail

void ConstructWorldObjects()
{
    for (auto& record : detail::gWorldObjects) {
        platform::legacy_crypt_int::ConstructZero_401C70(&record.coordinate0C);
        platform::legacy_crypt_int::ConstructZero_401C70(&record.coordinate1C);
    }
}

void DestroyWorldObjects()
{
    for (std::size_t index = detail::gWorldObjects.size(); index != 0; --index) {
        auto& record = detail::gWorldObjects[index - 1];
        platform::legacy_crypt_int::Destroy_401CF0(&record.coordinate1C);
        platform::legacy_crypt_int::Destroy_401CF0(&record.coordinate0C);
    }
}

} // namespace cg::world
