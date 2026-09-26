#include "cg/LegacyRecordOrder.hpp"

#include "cg/platform/CommandOptions.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::login::detail {

std::array<OrderedRecord1051B14, 200> gOrderedRecords1051B14 {};
std::uint32_t gOrderMode1057C78 = 0;

int CompareOrderedRecords_52FB60(const void* const left, const void* const right)
{
    const auto leftIndex = *static_cast<const std::uint32_t*>(left);
    const auto rightIndex = *static_cast<const std::uint32_t*>(right);
    const OrderedRecord1051B14& other = gOrderedRecords1051B14[rightIndex];
    if (other.value08 == 0) {
        return -1;
    }
    const OrderedRecord1051B14& current = gOrderedRecords1051B14[leftIndex];
    if (current.value08 == 0) {
        return 1;
    }

    if (gOrderMode1057C78 == 0) {
        return current.value04 > other.value04 ? 1 : -1;
    }
    if (gOrderMode1057C78 == 1) {
        const int textOrder = std::strcmp(current.text18, other.text18);
        if (textOrder > 0) {
            return 1;
        }
        return textOrder == 0 && current.value0C > other.value0C ? 1 : -1;
    }
    if (current.value0A == 0) {
        if (other.value0A > 0) {
            return 1;
        }
        return other.value0A == 0 && current.value0C > other.value0C ? 1 : -1;
    }
    if (current.value0A > 0 && other.value0A > 0
        && current.value0C > other.value0C) {
        return 1;
    }
    return -1;
}

void BuildOrderedRecordIndices_52FCC0()
{
    std::uint32_t* const indices = platform::BgmRuntimeOrderWords();
    for (std::uint32_t index = 0; index < gOrderedRecords1051B14.size(); ++index) {
        indices[index] = index;
    }
    std::qsort(
        indices, gOrderedRecords1051B14.size(), sizeof(std::uint32_t),
        &CompareOrderedRecords_52FB60);
}

std::int32_t CountRecordsValue08EqualOne_50EF50() noexcept
{
    std::int32_t count = 0;
    for (const OrderedRecord1051B14& record : gOrderedRecords1051B14) {
        if (record.value08 == 1) {
            ++count;
        }
    }
    return count;
}

} // namespace cg::login::detail
