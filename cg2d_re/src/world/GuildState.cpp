#include "cg/world/GuildState.hpp"

#include "cg/input/Input.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/world/SceneState.hpp"

#include "MailState.hpp"

#include <bit>
#include <cstdlib>
#include <cstring>

namespace cg::world {

std::array<OrderedRecord1051B14, 200> gOrderedRecords1051B14 {}; // 原全局：0x01051B14。
static std::uint32_t gOrderMode1057C78 = 0; // 原全局：0x01057C78。
std::int32_t gOrderedRecordCount1051510 = 0; // 原全局：0x01051510。

// 原10514A8的五S32和两条原字节文本；重置保留+0C/+10及文本尾部。
struct GuildHeader10514A8 {
    std::int32_t key00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。
    std::array<std::array<char, 41>, 2> texts14; // 原成员偏移：+0x14。
    std::array<std::byte, 2> bytes66; // 原成员偏移：+0x66。
};

// 原1051514的32条30H文本记录，末3B未消费，不据此补业务含义。
struct GuildTextRecord1051514 {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::array<char, 41> text04; // 原成员偏移：+0x04。
    std::array<std::byte, 3> bytes2D; // 原成员偏移：+0x2D。
};

static_assert(offsetof(GuildHeader10514A8, value04) == 4);
static_assert(offsetof(GuildHeader10514A8, value08) == 8);
static_assert(offsetof(GuildHeader10514A8, value0C) == 0x0C);
static_assert(offsetof(GuildHeader10514A8, value10) == 0x10);
static_assert(offsetof(GuildHeader10514A8, texts14) == 0x14);
static_assert(offsetof(GuildHeader10514A8, bytes66) == 0x66);
static_assert(sizeof(GuildHeader10514A8) == 0x68);
static_assert(offsetof(GuildTextRecord1051514, text04) == 4);
static_assert(offsetof(GuildTextRecord1051514, bytes2D) == 0x2D);
static_assert(sizeof(GuildTextRecord1051514) == 0x30);
static GuildHeader10514A8 gGuildHeader10514A8 {}; // 原全局：0x010514A8。
static std::array<GuildTextRecord1051514, 32> gGuildTextRecords1051514 {}; // 原全局：0x01051514。
static std::array<std::array<char, 41>, 3> gGuildTexts1057BF4 {}; // 原全局：0x01057BF4。
static_assert(sizeof(gGuildTextRecords1051514) == 0x600);
static_assert(sizeof(gGuildTexts1057BF4) == 0x7B);

// 532D80和532DF0的同序窄写；两条路径的邮件副作用由各自caller处理。
static void ResetGuildRuntimeFields()
{
    gGuildHeader10514A8.key00 = -1;
    gGuildHeader10514A8.value08 = -1;
    gGuildHeader10514A8.value04 = -1;
    gGuildHeader10514A8.texts14[0][0] = '\0';
    gGuildHeader10514A8.texts14[1][0] = '\0';
    for (OrderedRecord1051B14& record : gOrderedRecords1051B14) {
        record.value00 = -1;
        record.value08 = 0;
        record.text18[0] = '\0';
        record.text18[0x11] = '\0';
    }
    for (GuildTextRecord1051514& record : gGuildTextRecords1051514) {
        record.value00 = 0;
        record.text04[0] = '\0';
    }
    for (auto& text : gGuildTexts1057BF4) {
        text[0] = '\0';
    }
    gOrderedRecordCount1051510 = 0;
}

std::int32_t ReadGuildValue10514B4() noexcept
{
    return gGuildHeader10514A8.value0C;
}

std::int32_t ReadGuildTextIndex10514B0() noexcept
{
    return gGuildHeader10514A8.value08;
}

std::int32_t ReadGuildRankFlags1051514(const std::int32_t index) noexcept
{
    return gGuildTextRecords1051514[index].value00;
}

std::uint8_t ReadGuildRankFlagsByte1051514(const std::int32_t index) noexcept
{
    const auto* const bytes = reinterpret_cast<const unsigned char*>(
        &gGuildTextRecords1051514[index].value00);
    return bytes[0];
}

std::int32_t ReadGuildStatus10514AC() noexcept
{
    return gGuildHeader10514A8.value04;
}

std::int32_t ReadGuildOwnKey10514B8() noexcept
{
    return gGuildHeader10514A8.value10;
}

const char* GetGuildName() noexcept
{
    return gGuildHeader10514A8.texts14[0].data();
}

const char* GetGuildSecondText() noexcept
{
    return gGuildHeader10514A8.texts14[1].data();
}

const char* GetGuildAuxText(const std::int32_t index) noexcept
{
    return gGuildTexts1057BF4[index].data();
}

const char* GetGuildRankText(const std::int32_t index) noexcept
{
    return gGuildTextRecords1051514[index].text04.data();
}

// 原函数：0x0050E850。
void HandleGuildInfoReply_50E850(const std::uintptr_t, const char* const text)
{
    if (world::gGameplayRepliesSuppressed_9A6E0C != 0) {
        return;
    }
    const auto mask = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(text, '|', 1));
    std::int32_t field = 2;
    std::array<char, 256> first;
    std::array<char, 256> second;
    std::array<char, 256> tail;
    const auto readText = [&](char* const buffer, char* const destination) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field, 255, buffer));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        if (std::strlen(buffer) <= 40) {
            std::strcpy(destination, buffer);
        }
        ++field;
    };
    if ((mask & 0x03u) != 0) {
        gGuildHeader10514A8.key00 =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
        ++field;
    }
    if ((mask & 0x05u) != 0) {
        readText(first.data(), gGuildHeader10514A8.texts14[0].data());
    }
    if ((mask & 0x09u) != 0) {
        readText(second.data(), gGuildHeader10514A8.texts14[1].data());
    }
    if ((mask & 0x11u) != 0) {
        gGuildHeader10514A8.value08 =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
        ++field;
    }
    if ((mask & 0x21u) != 0) {
        static_cast<void>(network::ParseDelimitedInteger_52CFA0(text, '|', field));
        ++field;
    }
    if ((mask & 0x41u) != 0) {
        static_cast<void>(network::ParseDelimitedInteger_52CFA0(text, '|', field));
        ++field;
    }
    if ((mask & 0x81u) != 0) {
        gGuildHeader10514A8.value04 =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
        ++field;
    }
    if ((mask & 0x101u) != 0) {
        gGuildHeader10514A8.value0C =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
        ++field;
    }
    if ((mask & 0x201u) != 0) {
        readText(tail.data(), gGuildTexts1057BF4[0].data());
    }
    if ((mask & 0x401u) != 0) {
        readText(tail.data(), gGuildTexts1057BF4[1].data());
    }
    if ((mask & 0x801u) != 0) {
        readText(tail.data(), gGuildTexts1057BF4[2].data());
    }
    if ((mask & 0x1001u) != 0) {
        gGuildHeader10514A8.value10 =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
    }
}

// 原函数：0x0050EB40。
void HandleGuildRankDefinitionsReply_50EB40(const std::uintptr_t, const char* const text)
{
    if (world::gGameplayRepliesSuppressed_9A6E0C != 0) {
        return;
    }
    std::int32_t remaining = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    if (remaining <= 0) {
        return;
    }
    std::array<char, 256> buffer;
    std::int32_t field = 4;
    do {
        const std::int32_t index =
            network::ParseDelimitedInteger_52CFA0(text, '|', field - 2);
        const std::int32_t value =
            network::ParseDelimitedInteger_52CFA0(text, '|', field - 1);
        if (value == -1) {
            gGuildTextRecords1051514[index].value00 = 0;
            gGuildTextRecords1051514[index].text04[0] = '\0';
        } else {
            static_cast<void>(network::ExtractDelimitedField_52CED0(
                text, '|', field, 255, buffer.data()));
            input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
            if (std::strlen(buffer.data()) <= 40) {
                gGuildTextRecords1051514[index].value00 = value;
                std::strcpy(gGuildTextRecords1051514[index].text04.data(), buffer.data());
            }
        }
        field += 3;
        --remaining;
    } while (remaining != 0);
}

// 原函数：0x0050F280。
void HandleGuildRemovalReply_50F280(const std::uintptr_t, const std::int32_t key)
{
    if (gGuildHeader10514A8.key00 != key) {
        return;
    }
    ResetGuildRuntimeFields();
    static_cast<void>(cg::world::detail::ClearCurrentCharacterGuildMail_5194C0());
    static_cast<void>(cg::world::detail::SaveGuildMailFile_519550());
    static_cast<void>(ui::WindowGuildDetails_RequestClose());
}

// 原函数：0x00532D80。
std::int32_t ResetGuildStateAndCloseDetails_532D80()
{
    ResetGuildRuntimeFields();
    return ui::WindowGuildDetails_RequestClose();
}

// GMR01：原200排序记录W16==1计数，S32结果0..200，正文正确保持。
// 原函数：0x0050EF50。
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

// 原函数：0x0050EF20。
static OrderedRecord1051B14* ResetOrderedRecord_50EF20(
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

// 原函数：0x0050EF70。
static std::int32_t ParseOrderedRecord_50EF70(
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

// 原函数：0x0050EEB0。
std::uintptr_t UpdateOrderedRecords_50EEB0(
    std::uintptr_t, std::uint32_t, const char* const text)
{
    const std::uint32_t gate = world::gGameplayRepliesSuppressed_9A6E0C;
    if (gate != 0) {
        return gate;
    }

    const std::int32_t key =
        network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    for (std::size_t index = 0; index < gOrderedRecords1051B14.size(); ++index) {
        if (gOrderedRecords1051B14[index].value0C == key) {
            ParseOrderedRecord_50EF70(static_cast<std::int32_t>(index), text);
            gOrderedRecordCount1051510 = CountRecordsValue08EqualOne_50EF50();
            return static_cast<std::uintptr_t>(gOrderedRecordCount1051510);
        }
    }
    for (std::size_t index = 0; index < gOrderedRecords1051B14.size(); ++index) {
        if (gOrderedRecords1051B14[index].value0C == -1) {
            ParseOrderedRecord_50EF70(static_cast<std::int32_t>(index), text);
            gOrderedRecordCount1051510 = CountRecordsValue08EqualOne_50EF50();
            return static_cast<std::uintptr_t>(gOrderedRecordCount1051510);
        }
    }

    return reinterpret_cast<std::uintptr_t>(gOrderedRecords1051B14.data())
        + sizeof(gOrderedRecords1051B14)
        + offsetof(OrderedRecord1051B14, value0C);
}

// 原函数：0x0050EC50。
std::uintptr_t UpdateOrderedRecordBatch_50EC50(
    std::uintptr_t, const char* const text)
{
    const std::uint32_t gate = world::gGameplayRepliesSuppressed_9A6E0C;
    if (gate != 0) {
        return gate;
    }
    for (OrderedRecord1051B14& record : gOrderedRecords1051B14) {
        ResetOrderedRecord_50EF20(&record);
    }

    std::array<char, 256> encoded;
    std::array<char, 256> first;
    std::array<char, 256> second;
    std::int32_t field = 2;
    for (OrderedRecord1051B14& record : gOrderedRecords1051B14) {
        if (network::ParseDelimitedInteger_52CFA0(text, '|', field - 1) <= 0) {
            ResetOrderedRecord_50EF20(&record);
        } else {
            record.value08 = 1;
            const std::int32_t key =
                network::ParseDelimitedInteger_52CFA0(text, '|', field);
            if (key == -1) {
                break;
            }
            record.value0C = key;
            if (network::ExtractDelimitedField_52CED0(
                    text, '|', field + 1, 255, encoded.data()) == 1) {
                break;
            }
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
            record.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field + 2);
            record.value10 = network::ParseDelimitedInteger_52CFA0(text, '|', field + 3);
            record.value0A = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(text, '|', field + 4)));
            record.value14 = network::ParseDelimitedInteger_52CFA0(text, '|', field + 5);
            record.value00 = network::ParseDelimitedInteger_52CFA0(text, '|', field + 6);
        }
        field += 8;
    }

    const std::int32_t count = CountRecordsValue08EqualOne_50EF50();
    const std::int16_t selected = login::gSelectedCharacterIndex;
    gOrderedRecordCount1051510 = count;
    return world::detail::PruneGuildMailAgainstOrderedRecords_50EE4A(selected);
}

// GML01：原200排序项W16==1首匹配，计数/返回S32，无则-1。
// 原函数：0x005195C0。
std::int32_t FindActiveOrderedRecordIndex_5195C0(
    const std::int32_t key) noexcept
{
    for (std::int32_t index = 0; index < 200; ++index) {
        const OrderedRecord1051B14& record = gOrderedRecords1051B14[index];
        if (record.value08 == 1 && record.value0C == key) {
            return index;
        }
    }
    return -1;
}

} // namespace cg::world
