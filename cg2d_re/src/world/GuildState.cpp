#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/world/GuildState.hpp"

#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"
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

// 532D80和532DF0的同序窄写；两条路径的邮件副作用由各自caller处理。
static void ResetGuildRuntimeFields(session::ClientSession& session)
{
    session.WorldScene().guild.header.key00 = -1;
    session.WorldScene().guild.header.value08 = -1;
    session.WorldScene().guild.header.value04 = -1;
    session.WorldScene().guild.header.texts14[0].clear();
    session.WorldScene().guild.header.texts14[1].clear();
    for (OrderedRecord1051B14& record : session.WorldScene().guild.orderedRecords) {
        record.value00 = -1;
        record.value08 = 0;
        record.name18.clear();
        record.title29.clear();
    }
    for (GuildTextRecord1051514& record : session.WorldScene().guild.textRecords) {
        record.value00 = 0;
        record.text04.clear();
    }
    for (auto& text : session.WorldScene().guild.texts) {
        text.clear();
    }
    session.WorldScene().guild.orderedRecordCount = 0;
}

std::int32_t ReadGuildValue10514B4(session::ClientSession& session) noexcept
{
    return session.WorldScene().guild.header.value0C;
}

std::int32_t ReadGuildTextIndex10514B0(session::ClientSession& session) noexcept
{
    return session.WorldScene().guild.header.value08;
}

std::int32_t ReadGuildRankFlags1051514(session::ClientSession& session, const std::int32_t index) noexcept
{
    if (index < 0 || static_cast<std::size_t>(index) >= session.WorldScene().guild.textRecords.size()) { return 0; }
    return session.WorldScene().guild.textRecords[index].value00;
}

std::uint8_t ReadGuildRankFlagsByte1051514(session::ClientSession& session, const std::int32_t index) noexcept
{
    if (index < 0 || static_cast<std::size_t>(index) >= session.WorldScene().guild.textRecords.size()) { return 0; }
    const auto* const bytes = reinterpret_cast<const unsigned char*>(
        &session.WorldScene().guild.textRecords[index].value00);
    return bytes[0];
}

std::int32_t ReadGuildStatus10514AC(session::ClientSession& session) noexcept
{
    return session.WorldScene().guild.header.value04;
}

std::int32_t ReadGuildOwnKey10514B8(session::ClientSession& session) noexcept
{
    return session.WorldScene().guild.header.value10;
}

const wchar_t* GetGuildName(session::ClientSession& session) noexcept
{
    return session.WorldScene().guild.header.texts14[0].data();
}

const wchar_t* GetGuildSecondText(session::ClientSession& session) noexcept
{
    return session.WorldScene().guild.header.texts14[1].data();
}

const wchar_t* GetGuildAuxText(session::ClientSession& session, const std::int32_t index) noexcept
{
    if (index < 0 || static_cast<std::size_t>(index) >= session.WorldScene().guild.texts.size()) { return L""; }
    return session.WorldScene().guild.texts[index].data();
}

const wchar_t* GetGuildRankText(session::ClientSession& session, const std::int32_t index) noexcept
{
    if (index < 0 || static_cast<std::size_t>(index) >= session.WorldScene().guild.textRecords.size()) { return L""; }
    return session.WorldScene().guild.textRecords[index].text04.data();
}

// 原函数：0x0050E850。
void HandleGuildInfoReply_50E850(cg::network::CgConnection& connection, const char* const text)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
        return;
    }
    const auto mask = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(text, '|', 1));
    std::int32_t field = 2;
    std::array<char, 256> first;
    std::array<char, 256> second;
    std::array<char, 256> tail;
    const auto readText = [&](char* const buffer, std::wstring& destination) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field, 255, buffer));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        if (std::strlen(buffer) <= 40) {
            destination = cg::text::FromGbkWide(buffer);
        }
        ++field;
    };
    if ((mask & 0x03u) != 0) {
        connection.Owner().WorldScene().guild.header.key00 =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
        ++field;
    }
    if ((mask & 0x05u) != 0) {
        readText(first.data(), connection.Owner().WorldScene().guild.header.texts14[0]);
    }
    if ((mask & 0x09u) != 0) {
        readText(second.data(), connection.Owner().WorldScene().guild.header.texts14[1]);
    }
    if ((mask & 0x11u) != 0) {
        connection.Owner().WorldScene().guild.header.value08 =
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
        connection.Owner().WorldScene().guild.header.value04 =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
        ++field;
    }
    if ((mask & 0x101u) != 0) {
        connection.Owner().WorldScene().guild.header.value0C =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
        ++field;
    }
    if ((mask & 0x201u) != 0) {
        readText(tail.data(), connection.Owner().WorldScene().guild.texts[0]);
    }
    if ((mask & 0x401u) != 0) {
        readText(tail.data(), connection.Owner().WorldScene().guild.texts[1]);
    }
    if ((mask & 0x801u) != 0) {
        readText(tail.data(), connection.Owner().WorldScene().guild.texts[2]);
    }
    if ((mask & 0x1001u) != 0) {
        connection.Owner().WorldScene().guild.header.value10 =
            network::ParseDelimitedInteger_52CFA0(text, '|', field);
    }
}

// 原函数：0x0050EB40。
void HandleGuildRankDefinitionsReply_50EB40(cg::network::CgConnection& connection, const char* const text)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
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
        if (index < 0 || static_cast<std::size_t>(index) >= connection.Owner().WorldScene().guild.textRecords.size()) {
            field += 3;
            --remaining;
            continue;
        }
        if (value == -1) {
            connection.Owner().WorldScene().guild.textRecords[index].value00 = 0;
            connection.Owner().WorldScene().guild.textRecords[index].text04.clear();
        } else {
            static_cast<void>(network::ExtractDelimitedField_52CED0(
                text, '|', field, 255, buffer.data()));
            input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
            if (std::strlen(buffer.data()) <= 40) {
                connection.Owner().WorldScene().guild.textRecords[index].value00 = value;
                connection.Owner().WorldScene().guild.textRecords[index].text04 = cg::text::FromGbkWide(buffer.data());
            }
        }
        field += 3;
        --remaining;
    } while (remaining != 0);
}

// 原函数：0x0050F280。
void HandleGuildRemovalReply_50F280(cg::network::CgConnection& connection, const std::int32_t key)
{
    if (connection.Owner().WorldScene().guild.header.key00 != key) {
        return;
    }
    ResetGuildRuntimeFields(connection.Owner());
    static_cast<void>(cg::world::detail::ClearCurrentCharacterGuildMail_5194C0(connection.Owner()));
    static_cast<void>(cg::world::detail::SaveGuildMailFile_519550(connection.Owner()));
    static_cast<void>(ui::WindowGuildDetails_RequestClose(connection.Owner()));
}

// 原函数：0x00532D80。
std::int32_t ResetGuildStateAndCloseDetails_532D80(session::ClientSession& session)
{
    ResetGuildRuntimeFields(session);
    return ui::WindowGuildDetails_RequestClose(session);
}

// GMR01：原200排序记录W16==1计数，S32结果0..200，正文正确保持。
// 原函数：0x0050EF50。
std::int32_t CountRecordsValue08EqualOne_50EF50(session::ClientSession& session) noexcept
{
    std::int32_t count = 0;
    for (const OrderedRecord1051B14& record : session.WorldScene().guild.orderedRecords) {
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
    record->name18.clear();
    record->title29.clear();
    record->text3A.clear();
    record->text5B.clear();
    record->value08 = 0;
    record->value0A = 0;
    record->value0C = -1;
    record->value14 = 0;
    record->value10 = 0;
    return record;
}

// 原函数：0x0050EF70。
static std::int32_t ParseOrderedRecord_50EF70(session::ClientSession& session,
    const std::int32_t index, const char* const text)
{
    const std::int32_t key =
        network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    OrderedRecord1051B14& record =
        session.WorldScene().guild.orderedRecords[static_cast<std::size_t>(index)];
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
        record.name18 = cg::text::FromGbkWide(first.data());
    }
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        encoded.data(), '|', 2, 255, second.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(second.data());
    if (std::strlen(second.data()) <= 0x10) {
        record.title29 = cg::text::FromGbkWide(second.data());
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
std::uintptr_t UpdateOrderedRecords_50EEB0(cg::network::CgConnection& connection, std::uint32_t, const char* const text)
{
    const std::uint32_t gate = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (gate != 0) {
        return gate;
    }

    const std::int32_t key =
        network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    for (std::size_t index = 0; index < connection.Owner().WorldScene().guild.orderedRecords.size(); ++index) {
        if (connection.Owner().WorldScene().guild.orderedRecords[index].value0C == key) {
            ParseOrderedRecord_50EF70(connection.Owner(), static_cast<std::int32_t>(index), text);
            connection.Owner().WorldScene().guild.orderedRecordCount = CountRecordsValue08EqualOne_50EF50(connection.Owner());
            return static_cast<std::uintptr_t>(connection.Owner().WorldScene().guild.orderedRecordCount);
        }
    }
    for (std::size_t index = 0; index < connection.Owner().WorldScene().guild.orderedRecords.size(); ++index) {
        if (connection.Owner().WorldScene().guild.orderedRecords[index].value0C == -1) {
            ParseOrderedRecord_50EF70(connection.Owner(), static_cast<std::int32_t>(index), text);
            connection.Owner().WorldScene().guild.orderedRecordCount = CountRecordsValue08EqualOne_50EF50(connection.Owner());
            return static_cast<std::uintptr_t>(connection.Owner().WorldScene().guild.orderedRecordCount);
        }
    }

    return reinterpret_cast<std::uintptr_t>(connection.Owner().WorldScene().guild.orderedRecords.data())
        + sizeof(connection.Owner().WorldScene().guild.orderedRecords)
        + offsetof(OrderedRecord1051B14, value0C);
}

// 原函数：0x0050EC50。
std::uintptr_t UpdateOrderedRecordBatch_50EC50(cg::network::CgConnection& connection, const char* const text)
{
    const std::uint32_t gate = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (gate != 0) {
        return gate;
    }
    for (OrderedRecord1051B14& record : connection.Owner().WorldScene().guild.orderedRecords) {
        ResetOrderedRecord_50EF20(&record);
    }

    std::array<char, 256> encoded;
    std::array<char, 256> first;
    std::array<char, 256> second;
    std::int32_t field = 2;
    for (OrderedRecord1051B14& record : connection.Owner().WorldScene().guild.orderedRecords) {
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
                record.name18 = cg::text::FromGbkWide(first.data());
            }
            static_cast<void>(network::ExtractDelimitedField_52CED0(
                encoded.data(), '|', 2, 255, second.data()));
            input::UnescapeLegacyTextAndNotify_52D2F0(second.data());
            if (std::strlen(second.data()) <= 0x10) {
                record.title29 = cg::text::FromGbkWide(second.data());
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

    const std::int32_t count = CountRecordsValue08EqualOne_50EF50(connection.Owner());
    const std::int16_t selected = connection.LoginRequests().selectedCharacterIndex;
    connection.Owner().WorldScene().guild.orderedRecordCount = count;
    return world::detail::PruneGuildMailAgainstOrderedRecords_50EE4A(connection.Owner(), selected);
}

// GML01：原200排序项W16==1首匹配，计数/返回S32，无则-1。
// 原函数：0x005195C0。
std::int32_t FindActiveOrderedRecordIndex_5195C0(session::ClientSession& session,
    const std::int32_t key) noexcept
{
    for (std::int32_t index = 0; index < 200; ++index) {
        const OrderedRecord1051B14& record = session.WorldScene().guild.orderedRecords[index];
        if (record.value08 == 1 && record.value0C == key) {
            return index;
        }
    }
    return -1;
}

} // namespace cg::world
