#include "cg/LegacyRecordOrder.hpp"

#include "cg/Input.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"

#include <bit>
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

OrderedRecord1051B14* ResetOrderedRecord_50EF20(
    OrderedRecord1051B14* const record) noexcept
{
    record->value04 = 0;
    record->value00 = 0;
    record->text18[0] = '\0';
    record->text18[0x11] = '\0';
    record->text18[0x22] = '\0';
    record->text18[0x43] = '\0';
    record->value08 = 0;
    record->value0A = 0;
    record->value0C = -1;
    record->value14 = 0;
    record->value10 = 0;
    return record;
}

std::int32_t ParseOrderedRecord_50EF70(
    const std::int32_t index, const char* const text)
{
    const std::int32_t key =
        network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    OrderedRecord1051B14& record =
        gOrderedRecords1051B14[static_cast<std::size_t>(index)];
    if (network::ParseDelimitedInteger_52CFA0(text, '|', 1) <= 0) {
        ResetOrderedRecord_50EF20(&record);
        return 0;
    }

    record.value08 = 1;
    record.value0C = key;
    std::array<char, 256> encoded;
    std::array<char, 256> first;
    std::array<char, 256> second;
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        text, '|', 3, 255, encoded.data()));
    input::UnescapeLegacyText_52D150(encoded.data());
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        encoded.data(), '|', 1, 255, first.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(first.data());
    if (std::strlen(first.data()) <= 0x10) {
        std::strcpy(record.text18, first.data());
    }
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        encoded.data(), '|', 2, 255, second.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(second.data());
    if (std::strlen(second.data()) <= 0x10) {
        std::strcpy(record.text18 + 0x11, second.data());
    }
    record.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    record.value10 = network::ParseDelimitedInteger_52CFA0(text, '|', 5);
    record.value0A = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(text, '|', 6)));
    record.value14 = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    record.value00 = network::ParseDelimitedInteger_52CFA0(text, '|', 8);
    return 1;
}

} // namespace cg::login::detail
