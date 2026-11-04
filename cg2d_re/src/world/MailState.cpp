#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "../scene/login/LoginSavedState.hpp"
#include "MailState.hpp"
#include "cg/text/Encoding.hpp"
#include <format>
#include <memory>
#include <vector>

#include "../scene/login/LoginInputDependencies.hpp"

#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/world/GuildState.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/world/SceneState.hpp"
#include "ChatHistory.hpp"

#include <ctime>
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BCCD、0x0053C680；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstring>

namespace cg::world {

MailRuntimeState::MailRuntimeState()
    : data(std::make_unique<detail::MailRuntimeData>())
{
}
MailRuntimeState::~MailRuntimeState() = default;

} // namespace cg::world

namespace cg::world::detail {

// 文件边界：旧版mail.dat/guildmail.dat布局保持，不直接存取含wstring的对象。
struct MailDiskMessage {
    std::byte unread;
    std::byte kind;
    std::array<char, 18> name;
    std::array<char, 300> text;
};
struct GuildMailDiskEntry {
    std::int32_t key;
    std::uint32_t active;
    std::array<MailDiskMessage, 10> messages;
};
static_assert(sizeof(MailDiskMessage) == 0x140);
static_assert(std::is_trivially_copyable_v<MailDiskMessage>);
static_assert(offsetof(MailDiskMessage, text) == 0x14);
static_assert(sizeof(GuildMailDiskEntry) == 0xC88);
static_assert(std::is_trivially_copyable_v<GuildMailDiskEntry>);
static_assert(offsetof(GuildMailDiskEntry, messages) == 8);

static MailDiskMessage EncodeMailMessage(const MailMessage& message)
{
    MailDiskMessage disk{};
    disk.unread = message.unread;
    disk.kind = message.kind;
    const std::string dateBytes = cg::text::ToGbk(message.name);
    std::size_t dateLength = 0;
    while (dateLength < dateBytes.size()) {
        const auto lead = static_cast<unsigned char>(dateBytes[dateLength]);
        const std::size_t next = dateLength + (lead >= 0x81 && lead <= 0xFE ? 2 : 1);
        if (next >= disk.name.size() || next > dateBytes.size()) {
            break;
        }
        dateLength = next;
    }
    std::copy_n(dateBytes.data(), dateLength, disk.name.data());
    cg::text::StoreGbk(disk.text.data(), disk.text.size(), message.text);
    return disk;
}

// 只发布实际读到的字段，短读不改变尚未覆盖的内部完整日期/正文。
static void DecodeMailMessage(MailMessage& message, const MailDiskMessage& disk, const std::size_t bytes)
{
    if (bytes > 0) { message.unread = disk.unread; }
    if (bytes > 1) { message.kind = disk.kind; }
    if (bytes > offsetof(MailDiskMessage, name)) {
        const auto length = std::find(disk.name.begin(), disk.name.end(), '\0') - disk.name.begin();
        message.name = cg::text::FromGbkWide(std::string_view(disk.name.data(), static_cast<std::size_t>(length)));
    }
    if (bytes > offsetof(MailDiskMessage, text)) {
        const auto length = std::find(disk.text.begin(), disk.text.end(), '\0') - disk.text.begin();
        message.text = cg::text::FromGbkWide(std::string_view(disk.text.data(), static_cast<std::size_t>(length)));
    }
}

static GuildMailDiskEntry EncodeGuildMailEntry(const GuildMailEntry& entry)
{
    GuildMailDiskEntry disk{};
    disk.key = entry.key;
    disk.active = entry.active;
    for (std::size_t i = 0; i < entry.messages.size(); ++i) {
        disk.messages[i] = EncodeMailMessage(entry.messages[i]);
    }
    return disk;
}

static void DecodeGuildMailEntry(GuildMailEntry& entry, const GuildMailDiskEntry& disk, const std::size_t bytes)
{
    if (bytes > 0) { entry.key = disk.key; }
    if (bytes > offsetof(GuildMailDiskEntry, active)) { entry.active = disk.active; }
    for (std::size_t i = 0; i < entry.messages.size(); ++i) {
        const std::size_t offset = offsetof(GuildMailDiskEntry, messages) + i * sizeof(MailDiskMessage);
        if (bytes <= offset) { break; }
        DecodeMailMessage(entry.messages[i], disk.messages[i], bytes - offset);
    }
}

static constexpr wchar_t kProfileDefault58F614[] = L"未设定"; // 原常量参考：0x0058F614。

// 个人资料与类别文件保留旧定长GBK布局；运行时对象只保存宽文本。
struct ProfileDiskEntry {
    std::int32_t value00;
    std::int32_t value04;
    std::array<char, 256> text08;
};
struct ProfileDiskRecord {
    std::int32_t value00;
    std::int32_t value04;
    std::array<char, 256> text08;
    std::int32_t value108;
    std::int32_t value10C;
    std::array<char, 256> text110;
    std::array<char, 256> text210;
    std::array<char, 256> text310;
    std::array<ProfileDiskEntry, 3> entries410;
    std::array<char, 256> text728;
};
struct CategoryDiskRecord {
    std::uint8_t used;
    std::array<std::byte, 3> bytes01;
    std::int32_t value04;
    std::int32_t value08;
    std::array<char, 256> text0C;
    std::uint8_t selected10C;
    std::array<std::byte, 3> bytes10D;
};
struct CategoryDiskStorage {
    std::int32_t value00;
    std::int32_t value04;
    std::int32_t count08;
    std::int32_t count0C;
    std::int32_t value10;
    std::array<CategoryDiskRecord, 300> records;
};
static_assert(sizeof(ProfileDiskEntry) == 0x108);
static_assert(sizeof(ProfileDiskRecord) == 0x828);
static_assert(offsetof(ProfileDiskRecord, entries410) == 0x410);
static_assert(offsetof(ProfileDiskRecord, text728) == 0x728);
static_assert(std::is_trivially_copyable_v<ProfileDiskRecord>);
static_assert(sizeof(CategoryDiskRecord) == 0x110);
static_assert(sizeof(CategoryDiskStorage) == 0x13ED4);
static_assert(offsetof(CategoryDiskStorage, records) == 0x14);
static_assert(std::is_trivially_copyable_v<CategoryDiskStorage>);

static std::wstring DecodeProfileDiskText(const std::array<char, 256>& bytes)
{
    const auto length = std::find(bytes.begin(), bytes.end(), '\0') - bytes.begin();
    return cg::text::FromGbkWide(std::string_view(bytes.data(), static_cast<std::size_t>(length)));
}

static ProfileDiskRecord EncodeProfile(const ProfileRuntimeRecord& record)
{
    ProfileDiskRecord disk{};
    disk.value00 = record.value00;
    disk.value04 = record.value04;
    disk.value108 = record.value108;
    disk.value10C = record.value10C;
    cg::text::StoreGbk(disk.text08.data(), disk.text08.size(), record.text08);
    cg::text::StoreGbk(disk.text110.data(), disk.text110.size(), record.text110);
    cg::text::StoreGbk(disk.text210.data(), disk.text210.size(), record.text210);
    cg::text::StoreGbk(disk.text310.data(), disk.text310.size(), record.text310);
    cg::text::StoreGbk(disk.text728.data(), disk.text728.size(), record.text728);
    for (std::size_t i = 0; i < record.entries410.size(); ++i) {
        disk.entries410[i].value00 = record.entries410[i].value00;
        disk.entries410[i].value04 = record.entries410[i].value04;
        cg::text::StoreGbk(disk.entries410[i].text08.data(), disk.entries410[i].text08.size(), record.entries410[i].text08);
    }
    return disk;
}

// 短读只发布被覆盖的字段；数值和文本尾部来自读取前的磁盘快照。
static void DecodeProfile(ProfileRuntimeRecord& record, const ProfileDiskRecord& disk, const std::size_t bytes)
{
    if (bytes > offsetof(ProfileDiskRecord, value00)) { record.value00 = disk.value00; }
    if (bytes > offsetof(ProfileDiskRecord, value04)) { record.value04 = disk.value04; }
    if (bytes > offsetof(ProfileDiskRecord, value108)) { record.value108 = disk.value108; }
    if (bytes > offsetof(ProfileDiskRecord, value10C)) { record.value10C = disk.value10C; }
    if (bytes > offsetof(ProfileDiskRecord, text08)) { record.text08 = DecodeProfileDiskText(disk.text08); }
    if (bytes > offsetof(ProfileDiskRecord, text110)) { record.text110 = DecodeProfileDiskText(disk.text110); }
    if (bytes > offsetof(ProfileDiskRecord, text210)) { record.text210 = DecodeProfileDiskText(disk.text210); }
    if (bytes > offsetof(ProfileDiskRecord, text310)) { record.text310 = DecodeProfileDiskText(disk.text310); }
    if (bytes > offsetof(ProfileDiskRecord, text728)) { record.text728 = DecodeProfileDiskText(disk.text728); }
    for (std::size_t i = 0; i < record.entries410.size(); ++i) {
        const std::size_t offset = offsetof(ProfileDiskRecord, entries410) + i * sizeof(ProfileDiskEntry);
        if (bytes <= offset) { break; }
        auto& entry = record.entries410[i];
        const auto& source = disk.entries410[i];
        entry.value00 = source.value00;
        if (bytes > offset + offsetof(ProfileDiskEntry, value04)) { entry.value04 = source.value04; }
        if (bytes > offset + offsetof(ProfileDiskEntry, text08)) { entry.text08 = DecodeProfileDiskText(source.text08); }
    }
}

static CategoryDiskStorage EncodeCategories(session::ClientSession& session)
{
    CategoryDiskStorage disk{};
    disk.value00 = session.WorldScene().mail.data->category.value00;
    disk.value04 = session.WorldScene().mail.data->category.value04;
    disk.count08 = session.WorldScene().mail.data->category.count08;
    disk.count0C = session.WorldScene().mail.data->category.count0C;
    disk.value10 = session.WorldScene().mail.data->category.value10;
    for (std::size_t i = 0; i < disk.records.size(); ++i) {
        const auto& record = session.WorldScene().mail.data->category.records[i];
        auto& target = disk.records[i];
        target.used = record.used;
        target.bytes01 = record.bytes01;
        target.value04 = record.value04;
        target.value08 = record.value08;
        target.selected10C = record.selected10C;
        target.bytes10D = record.bytes10D;
        cg::text::StoreGbk(target.text0C.data(), target.text0C.size(), record.text0C);
    }
    return disk;
}

static void DecodeCategories(session::ClientSession& session, const CategoryDiskStorage& disk, const std::size_t bytes)
{
    if (bytes > offsetof(CategoryDiskStorage, value00)) { session.WorldScene().mail.data->category.value00 = disk.value00; }
    if (bytes > offsetof(CategoryDiskStorage, value04)) { session.WorldScene().mail.data->category.value04 = disk.value04; }
    if (bytes > offsetof(CategoryDiskStorage, count08)) { session.WorldScene().mail.data->category.count08 = disk.count08; }
    if (bytes > offsetof(CategoryDiskStorage, count0C)) { session.WorldScene().mail.data->category.count0C = disk.count0C; }
    if (bytes > offsetof(CategoryDiskStorage, value10)) { session.WorldScene().mail.data->category.value10 = disk.value10; }
    for (std::size_t i = 0; i < disk.records.size(); ++i) {
        const std::size_t offset = offsetof(CategoryDiskStorage, records) + i * sizeof(CategoryDiskRecord);
        if (bytes <= offset) { break; }
        auto& record = session.WorldScene().mail.data->category.records[i];
        const auto& source = disk.records[i];
        if (bytes > offset + offsetof(CategoryDiskRecord, used)) { record.used = source.used; }
        if (bytes > offset + offsetof(CategoryDiskRecord, bytes01)) { record.bytes01 = source.bytes01; }
        if (bytes > offset + offsetof(CategoryDiskRecord, value04)) { record.value04 = source.value04; }
        if (bytes > offset + offsetof(CategoryDiskRecord, value08)) { record.value08 = source.value08; }
        if (bytes > offset + offsetof(CategoryDiskRecord, selected10C)) { record.selected10C = source.selected10C; }
        if (bytes > offset + offsetof(CategoryDiskRecord, bytes10D)) { record.bytes10D = source.bytes10D; }
        if (bytes > offset + offsetof(CategoryDiskRecord, text0C)) { record.text0C = DecodeProfileDiskText(source.text0C); }
    }
}

static constexpr wchar_t kReceivedMiniMailFormat58B108[] = // 原常量参考：0x0058B108。
    L"提示！您收到来自 {} 的迷你邮件～";

static MailMessageView MakeMailMessageView(MailMessage& message)
{
    return {&message.unread, &message.kind, message.name.data(), message.text.data()};
}

// 原函数：0x004EC030。
static std::int32_t CompareContactIndices_4EC030(session::ClientSession& session,
    const std::int32_t* const left, const std::int32_t* const right)
{
    const ContactRuntimeRecord& other = session.WorldScene().mail.data->contacts[*right];
    const ContactRuntimeRecord& current = session.WorldScene().mail.data->contacts[*left];
    const bool currentUsed = current.used != 0;
    const bool otherUsed = other.used != 0;
    if (currentUsed != otherUsed) {
        return currentUsed ? -1 : 1;
    }
    if (!currentUsed) {
        return 0;
    }
    const std::uint32_t mode = platform::gContactSortMode_D67CD0;
    if (mode == 1) {
        const int order = current.name10.compare(other.name10);
        if (order != 0) {
            return order < 0 ? -1 : 1;
        }
    } else if (mode != 0) {
        const bool currentState = current.value02 != 0;
        const bool otherState = other.value02 != 0;
        if (currentState != otherState) {
            return currentState ? -1 : 1;
        }
    }
    return (current.value04 > other.value04) - (current.value04 < other.value04);
}

const MiniMailContactRecord& ReadMiniMailContact1075A90(session::ClientSession& session, const std::int32_t index)
{
    return session.WorldScene().mail.data->miniMail.contacts[index];
}

} // namespace cg::world::detail

namespace cg::world {

MailMessageView GetMailMessageF7CA08(session::ClientSession& session,
    const std::int16_t selected, const std::int32_t contact, const std::int32_t page)
{
    return detail::MakeMailMessageView(session.WorldScene().mail.data->messages[
        selected * 60 * 10 + static_cast<std::ptrdiff_t>(contact) * 10 + page]);
}

MailMessageView GetGuildMailMessageDE0B30(session::ClientSession& session,
    const std::int16_t selected, const std::int32_t slot, const std::int32_t page)
{
    return detail::MakeMailMessageView(session.WorldScene().mail.data->guildMessages[
        selected * 200 + static_cast<std::ptrdiff_t>(slot)].messages[page]);
}

MailMessageView GetMiniMailMessage10763A0(session::ClientSession& session,
    const std::int32_t contact, const std::int32_t page)
{
    return detail::MakeMailMessageView(session.WorldScene().mail.data->miniMail.messages[
        static_cast<std::ptrdiff_t>(contact) * 10 + page]);
}

} // namespace cg::world

namespace cg::world::detail {

const ProfileRuntimeRecord& ReadSelectedProfile1074938(session::ClientSession& session)
{
    return session.WorldScene().mail.data->profiles[session.Connection().LoginRequests().selectedCharacterIndex];
}

std::int32_t ReadCategoryValue105FA20(session::ClientSession& session)
{
    return session.WorldScene().mail.data->category.value10;
}

std::int32_t& SelectedProfileValue04(session::ClientSession& session)
{
    return session.WorldScene().mail.data->profiles[session.Connection().LoginRequests().selectedCharacterIndex].value04;
}

const wchar_t* ReadCategoryText105FA24(session::ClientSession& session, const std::int32_t group, const std::int32_t index)
{
    return session.WorldScene().mail.data->category.records[group * 100 + index].text0C.data();
}

std::int32_t ReadCategoryCount105FA18(session::ClientSession& session)
{
    return session.WorldScene().mail.data->category.count08;
}

std::int32_t ReadCategoryCount105FA1C(session::ClientSession& session)
{
    return session.WorldScene().mail.data->category.count0C;
}

const CategoryRecord& ReadCategoryRecord105FA24(session::ClientSession& session,
    const std::int32_t group, const std::int32_t index)
{
    return session.WorldScene().mail.data->category.records[group * 100 + index];
}

// 原函数：0x004EC1A0。
void SortContactIndices_4EC1A0(session::ClientSession& session)
{
    for (std::int32_t index = 0; index < 60; ++index) {
        session.WorldScene().mail.data->contactIndices[index] = index;
    }
    std::sort(session.WorldScene().mail.data->contactIndices.begin(), session.WorldScene().mail.data->contactIndices.end(),
        [&session](const std::int32_t left, const std::int32_t right) {
            return CompareContactIndices_4EC030(session, &left, &right) < 0;
        });
}

// 原函数：0x0050D740。
void HandleContactListReply_50D740(cg::network::CgConnection& connection, const char* const payload)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
        return;
    }
    char name[256];
    char text[256];
    std::int32_t field = 2;
    for (ContactRuntimeRecord& record : connection.Owner().WorldScene().mail.data->contacts) {
        if (network::ParseDelimitedInteger_52CFA0(payload, '|', field - 1) <= 0) {
            record.used = 0;
            record.name10.clear();
            record.text21.clear();
        } else {
            record.used = 1;
            if (network::ExtractDelimitedField_52CED0(
                    payload, '|', field, 255, name) == 1) {
                return;
            }
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            if (std::strlen(name) <= 16) {
                record.name10 = cg::text::FromGbkWide(name);
            }
            network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, text);
            input::UnescapeLegacyTextAndNotify_52D2F0(text);
            if (std::strlen(text) <= 16) {
                record.text21 = cg::text::FromGbkWide(text);
            }
            record.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 2);
            record.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 3);
            record.value02 = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4)));
            record.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 5);
        }
        field += 7;
    }
}

// 原函数：0x0050D8C0。
void HandleContactExtendedTextReply_50D8C0(cg::network::CgConnection& connection, const char* const payload)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
        return;
    }
    char first[256];
    char second[256];
    std::int32_t field = 2;
    for (ContactRuntimeRecord& record : connection.Owner().WorldScene().mail.data->contacts) {
        if (network::ExtractDelimitedField_52CED0(
                payload, '|', field - 1, 255, first) == 1) {
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(first);
        if (std::strlen(first) <= 32) {
            record.text32 = cg::text::FromGbkWide(first);
        }
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, second);
        input::UnescapeLegacyTextAndNotify_52D2F0(second);
        if (std::strlen(second) <= 32) {
            record.text53 = cg::text::FromGbkWide(second);
        }
        field += 2;
    }
    SortContactIndices_4EC1A0(connection.Owner());
}

// 原函数：0x0050D9E0。
void HandleContactUpdateReply_50D9E0(cg::network::CgConnection& connection, const std::int32_t index, const char* const payload)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
        return;
    }
    const std::int32_t active = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    ContactRuntimeRecord& record = connection.Owner().WorldScene().mail.data->contacts[index];
    if (active <= 0) {
        record.used = 0;
        record.name10.clear();
        record.text21.clear();
        return;
    }
    record.used = 1;
    char name[256];
    char text[256];
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 255, name);
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    if (std::strlen(name) <= 16) {
        record.name10 = cg::text::FromGbkWide(name);
    }
    network::ExtractDelimitedField_52CED0(payload, '|', 3, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    if (std::strlen(text) <= 16) {
        record.text21 = cg::text::FromGbkWide(text);
    }
    record.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', 4);
    record.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', 5);
    record.value02 = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', 6)));
    record.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', 7);
    if (network::ParseDelimitedInteger_52CFA0(payload, '|', 8) == 1) {
        static_cast<void>(ClearContactMailMessages_5192C0(connection.Owner(), index));
    }
    network::ExtractDelimitedField_52CED0(payload, '|', 9, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    if (std::strlen(text) <= 32) {
        record.text32 = cg::text::FromGbkWide(text);
    }
    network::ExtractDelimitedField_52CED0(payload, '|', 10, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    if (std::strlen(text) <= 32) {
        record.text53 = cg::text::FromGbkWide(text);
    }
    SortContactIndices_4EC1A0(connection.Owner());
}

// 原函数：0x005191C0。
void PrependContactMailMessage_5191C0(session::ClientSession& session,
    const std::int32_t index, const std::int32_t kind,
    const std::wstring_view name, const std::wstring_view text)
{
    if (session.WorldScene().mail.data->contacts[index].used == 0) {
        return;
    }
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    MailMessage* const entry = session.WorldScene().mail.data->messages.data()
        + selected * 60 * 10 + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::ptrdiff_t message = 9; message > 0; --message) {
        entry[message] = entry[message - 1];
    }
    entry->unread = kind == 0 ? std::byte{1} : std::byte{0};
    entry->kind = static_cast<std::byte>(static_cast<std::uint8_t>(kind));
    entry->name = name;
    entry->text = text;
}

// SHP01：原三类未读提示，迷你倒序；U16类别后发布W32页，未命中持有备用页。
// 原函数：0x00518DA0。
std::int32_t UpdateUnreadMailPageHint_518DA0(session::ClientSession& session)
{
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    for (std::int32_t position = 0; position < 60; ++position) {
        const std::int32_t index = session.WorldScene().mail.data->contactIndices[position];
        if (session.WorldScene().mail.data->contacts[index].used == 0) {
            continue;
        }
        const MailMessage* const entry = session.WorldScene().mail.data->messages.data()
            + selected * 60 * 10 + static_cast<std::ptrdiff_t>(index) * 10;
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry[message].unread != std::byte{0}) {
                session.WorldScene().mail.data->unreadCategory = 1;
                session.WorldScene().mail.data->contactPage = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    for (std::int32_t position = 0; position < 200; ++position) {
        const auto& record = session.WorldScene().guild.orderedRecords[position];
        if (record.value08 == 0) {
            continue;
        }
        const std::int32_t index = FindLastActiveGuildMailEntry_5195F0(session, record.value0C);
        if (index == -1) {
            continue;
        }
        const std::ptrdiff_t currentSelected = session.Connection().LoginRequests().selectedCharacterIndex;
        const GuildMailEntry* const entry = session.WorldScene().mail.data->guildMessages.data()
            + currentSelected * 200 + static_cast<std::ptrdiff_t>(index);
        const std::uint32_t active = entry->active;
        if (active == 0) {
            continue;
        }
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry->messages[message].unread != std::byte{0}) {
                session.WorldScene().mail.data->unreadCategory = 2;
                session.WorldScene().mail.data->contactPage = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        if (session.WorldScene().mail.data->miniMail.contacts[position].used == 0) {
            continue;
        }
        const MailMessage* const entry = session.WorldScene().mail.data->miniMail.messages.data()
            + static_cast<std::ptrdiff_t>(position) * 10;
        for (std::ptrdiff_t message = 9; message >= 0; --message) {
            if (entry[message].unread != std::byte{0}) {
                session.WorldScene().mail.data->unreadCategory = 3;
                session.WorldScene().mail.data->contactPage = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    const std::uint32_t fallbackPage = session.WorldScene().mail.data->fallbackPage;
    session.WorldScene().mail.data->unreadCategory = 0;
    session.WorldScene().mail.data->contactPage = fallbackPage;
    return 0;
}

// SHP01：原三类未读定位，迷你正序；先S32类别再索引，公会fresh字符/key，唯一父弃返。
// 原函数：0x00518F00。
void FindFirstUnreadMailContact_518F00(session::ClientSession& session, std::int32_t* const category, std::int32_t* const index)
{
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    for (std::int32_t position = 0; position < 60; ++position) {
        const std::int32_t contact = session.WorldScene().mail.data->contactIndices[position];
        if (session.WorldScene().mail.data->contacts[contact].used == 0) {
            continue;
        }
        const MailMessage* const entry = session.WorldScene().mail.data->messages.data()
            + selected * 60 * 10 + static_cast<std::ptrdiff_t>(contact) * 10;
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry[message].unread != std::byte{0}) {
                *category = 0;
                *index = session.WorldScene().mail.data->contactIndices[position];
                return;
            }
        }
    }
    for (std::int32_t position = 0; position < 200; ++position) {
        const auto& member = session.WorldScene().guild.orderedRecords[position];
        if (member.value08 == 0) {
            continue;
        }
        const std::int32_t slot = FindLastActiveGuildMailEntry_5195F0(session, member.value0C);
        if (slot == -1) {
            continue;
        }
        const std::ptrdiff_t currentSelected = session.Connection().LoginRequests().selectedCharacterIndex;
        const GuildMailEntry* const entry = session.WorldScene().mail.data->guildMessages.data()
            + currentSelected * 200 + static_cast<std::ptrdiff_t>(slot);
        const std::uint32_t active = entry->active;
        if (active == 0) {
            continue;
        }
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry->messages[message].unread != std::byte{0}) {
                *category = 1;
                const std::ptrdiff_t selectedForKey = session.Connection().LoginRequests().selectedCharacterIndex;
                const GuildMailEntry* const keyEntry = session.WorldScene().mail.data->guildMessages.data()
                    + selectedForKey * 200 + static_cast<std::ptrdiff_t>(slot);
                const std::int32_t key = keyEntry->key;
                *index = cg::world::FindActiveOrderedRecordIndex_5195C0(session, key);
                return;
            }
        }
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        if (session.WorldScene().mail.data->miniMail.contacts[position].used == 0) {
            continue;
        }
        const MailMessage* const entry = session.WorldScene().mail.data->miniMail.messages.data()
            + static_cast<std::ptrdiff_t>(position) * 10;
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry[message].unread != std::byte{0}) {
                *category = 2;
                *index = position;
                return;
            }
        }
    }
    *category = -1;
    *index = -1;
}

// 原函数：0x00519090。
std::int32_t HasUnreadContactMail_519090(session::ClientSession& session, const std::int32_t index)
{
    if (index < 0 || index >= 60) {
        return 0;
    }
    if (session.WorldScene().mail.data->contacts[index].used == 0) {
        return 0;
    }
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    const MailMessage* const entry = session.WorldScene().mail.data->messages.data()
        + selected * 60 * 10 + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::int32_t message = 0; message < 10; ++message) {
        if (entry[message].unread != std::byte{0}) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x005190F0。
std::int32_t HasUnreadGuildMail_5190F0(session::ClientSession& session, const std::int32_t index)
{
    const auto& record = session.WorldScene().guild.orderedRecords[index];
    const std::int32_t key = record.value0C;
    const std::int32_t slot = FindLastActiveGuildMailEntry_5195F0(session, key);
    if (slot == -1) {
        return 0;
    }
    if (index == -1) {
        return 0;
    }
    if (record.value08 == 0) {
        return 0;
    }
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    const MailMessage* const entry = session.WorldScene().mail.data->guildMessages[
        selected * 200 + static_cast<std::ptrdiff_t>(slot)].messages.data();
    for (std::int32_t message = 0; message < 10; ++message) {
        if (entry[message].unread != std::byte{0}) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x00519170。
std::int32_t HasUnreadMiniMail_519170(session::ClientSession& session, const std::int32_t index)
{
    if (index < 0 || index >= 20) {
        return 0;
    }
    if (session.WorldScene().mail.data->miniMail.contacts[index].used == 0) {
        return 0;
    }
    const MailMessage* const entry = session.WorldScene().mail.data->miniMail.messages.data()
        + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::int32_t message = 0; message < 10; ++message) {
        if (entry[message].unread != std::byte{0}) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x0050DC70。
void HandleContactMailReply_50DC70(cg::network::CgConnection& connection, const std::int32_t index,
    const char* const payload, const std::int32_t)
{
    char name[32];
    char text[300];
    std::wstring message;
    network::ExtractDelimitedField_52CED0(payload, '|', 1, 31, name);
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 299, text);
    const std::int32_t kind = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    PrependContactMailMessage_5191C0(connection.Owner(), index, 0, cg::text::FromGbkWide(name), cg::text::FromGbkWide(text));
    const std::wstring& contactName = connection.Owner().WorldScene().mail.data->contacts[index].name10;
    cg::text::FormatTo(message, kind == -1 ? L"收到从{}寄来的邮件！" /* 原常量参考：0x0058B07C。 */ : L"由{}送来了宠物邮件！" /* 原常量参考：0x0058B064。 */, contactName);
    static_cast<void>(SaveContactMailFile_519300(connection.Owner()));
    static_cast<void>(UpdateUnreadMailPageHint_518DA0(connection.Owner()));
    if (connection.Owner().WorldScene().mail.data->viewedContactId == index) {
        const std::int32_t position = connection.Owner().WorldScene().mail.data->viewedMessageIndex;
        if (position < 9) {
            connection.Owner().WorldScene().mail.data->viewedMessageIndex = position + 1;
        }
    }
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    AppendChatMessage_44B050(connection.Owner(), message, 4, 3);
}

// 原函数：0x00534370。
std::int32_t FindMiniMailContactByKey_534370(session::ClientSession& session, const std::int32_t key)
{
    for (std::int32_t index = 0; index < 20; ++index) {
        const MiniMailContactRecord& record = session.WorldScene().mail.data->miniMail.contacts[index];
        if (record.used == 1 && record.key04 == key) {
            return index;
        }
    }
    return -1;
}

// 原函数：0x005343A0。
std::int32_t FindFreeMiniMailContactSlot_5343A0(session::ClientSession& session)
{
    for (std::int32_t index = 0; index < 20; ++index) {
        if (session.WorldScene().mail.data->miniMail.contacts[index].used == 0) {
            return index;
        }
    }
    return -1;
}

// 原函数：0x004C9490。
void ResetMiniMailSequenceAndSelection_4C9490(session::ClientSession& session)
{
    session.WorldScene().mail.data->miniMailSequence = 0;
    session.WorldScene().mail.data->miniMailSelection = -1;
}

// 原函数：0x00534520。
std::int32_t FormatMailTimestamp_534520(session::ClientSession& session, std::wstring& destination)
{
    __time64_t timeValue;
    timeValue = ::_time64(nullptr);
    const std::tm* const localTime =
        session.LocalTime(&timeValue);
    const std::int32_t minutes = localTime->tm_min;
    const std::int32_t hours = localTime->tm_hour;
    const std::int32_t day = localTime->tm_mday;
    const std::int32_t month = localTime->tm_mon;
    const std::int32_t year = localTime->tm_year;
    destination = std::format(L"{:4}/{:2}/{:2} {:02}:{:02}", year + 1900, month + 1, day, hours, minutes);
    return 0;
}

// 原函数：0x005343C0。
void PrependMiniMailMessage_5343C0(session::ClientSession& session,
    const std::int32_t index, const std::int32_t kind,
    const std::wstring_view name, const std::wstring_view text)
{
    if (session.WorldScene().mail.data->miniMail.contacts[index].used == 0) {
        return;
    }
    MailMessage* const entry = session.WorldScene().mail.data->miniMail.messages.data()
        + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::ptrdiff_t message = 9; message > 0; --message) {
        entry[message] = entry[message - 1];
    }
    entry->unread = kind == 0 ? std::byte{1} : std::byte{0};
    entry->kind = static_cast<std::byte>(static_cast<std::uint8_t>(kind));
    entry->name = name;
    entry->text = text;
}

// 原函数：0x00534480。
void RemoveMiniMailContact_534480(session::ClientSession& session, const std::int32_t index)
{
    if (index < 0 || index >= static_cast<std::int32_t>(session.WorldScene().mail.data->miniMail.contacts.size())) {
        return;
    }
    session.WorldScene().mail.data->miniMail.contacts[index].used = 0;
    MailMessage* const first = session.WorldScene().mail.data->miniMail.messages.data()
        + static_cast<std::ptrdiff_t>(index) * 10;
    const auto remaining = static_cast<std::ptrdiff_t>(session.WorldScene().mail.data->miniMail.messages.size()) - index * 10;
    for (std::ptrdiff_t message = 0; message < std::min(std::ptrdiff_t{20}, remaining); ++message) {
        MailMessage* const entry = first + message;
        entry->text.clear();
        entry->name.clear();
        entry->unread = std::byte{0};
        entry->kind = std::byte{0};
    }
}

// 原函数：0x0050F430。
void HandleMiniMailContactRemovalReply_50F430(cg::network::CgConnection& connection, const char* const payload)
{
    const std::int32_t key = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    const std::int32_t index = FindMiniMailContactByKey_534370(connection.Owner(), key);
    RemoveMiniMailContact_534480(connection.Owner(), index);
}

// 原函数：0x0050F7F0。
void HandleMiniMailReply_50F7F0(cg::network::CgConnection& connection, const std::int32_t key,
    const char* const payload, const std::int32_t)
{
    const std::int32_t index = FindMiniMailContactByKey_534370(connection.Owner(), key);
    if (index == -1) {
        return;
    }
    char name[32];
    char text[512];
    std::wstring message;
    network::ExtractDelimitedField_52CED0(payload, '|', 1, 31, name);
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 511, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    PrependMiniMailMessage_5343C0(connection.Owner(), index, 0, cg::text::FromGbkWide(name), cg::text::FromGbkWide(text));
    cg::text::FormatTo(message, kReceivedMiniMailFormat58B108, connection.Owner().WorldScene().mail.data->miniMail.contacts[index].name10);
    static_cast<void>(UpdateUnreadMailPageHint_518DA0(connection.Owner()));
    if (connection.Owner().WorldScene().mail.data->viewedContactId == key) {
        const std::int32_t position = connection.Owner().WorldScene().mail.data->viewedMessageIndex;
        if (position < 9) {
            connection.Owner().WorldScene().mail.data->viewedMessageIndex = position + 1;
        }
    }
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    AppendChatMessage_44B050(connection.Owner(), message, 4, 3);
}

// 原函数：0x0050F9E0。
void HandleMiniMailConversationReply_50F9E0(cg::network::CgConnection& connection, const char* const payload)
{
    const std::int32_t index = FindFreeMiniMailContactSlot_5343A0(connection.Owner());
    if (index == -1) {
        return;
    }
    MailMessage* const first = connection.Owner().WorldScene().mail.data->miniMail.messages.data()
        + static_cast<std::ptrdiff_t>(index) * 10;
    const auto remaining = static_cast<std::ptrdiff_t>(connection.Owner().WorldScene().mail.data->miniMail.messages.size()) - index * 10;
    for (std::ptrdiff_t message = 0; message < std::min(std::ptrdiff_t{20}, remaining); ++message) {
        MailMessage* const entry = first + message;
        entry->text.clear();
        entry->name.clear();
        entry->unread = std::byte{0};
        entry->kind = std::byte{0};
    }

    MiniMailContactRecord& record = connection.Owner().WorldScene().mail.data->miniMail.contacts[index];
    record.used = 1;
    record.key04 = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    record.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', 2);
    record.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    {
        char fieldBytes[17]{};
        network::ExtractDelimitedField_52CED0(payload, '|', 4, 16, fieldBytes);
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldBytes);
        record.name10 = cg::text::FromGbkWide(fieldBytes);
    }
    {
        char fieldBytes[33]{};
        network::ExtractDelimitedField_52CED0(payload, '|', 5, 32, fieldBytes);
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldBytes);
        record.text32 = cg::text::FromGbkWide(fieldBytes);
    }
    {
        char fieldBytes[17]{};
        network::ExtractDelimitedField_52CED0(payload, '|', 6, 16, fieldBytes);
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldBytes);
        record.text21 = cg::text::FromGbkWide(fieldBytes);
    }
    const std::uint32_t sequence = connection.Owner().WorldScene().mail.data->miniMailSequence;
    record.sequence02 = static_cast<std::uint16_t>(sequence);
    connection.Owner().WorldScene().mail.data->miniMailSequence = sequence + 1u;

    const std::int32_t key = network::ParseDelimitedInteger_52CFA0(payload, '|', 7);
    char timestamp[256];
    char text[1024];
    std::wstring message;
    network::ExtractDelimitedField_52CED0(payload, '|', 8, 255, timestamp);
    input::UnescapeLegacyTextAndNotify_52D2F0(timestamp);
    network::ExtractDelimitedField_52CED0(payload, '|', 9, 1023, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::wstring formattedTimestamp;
    static_cast<void>(FormatMailTimestamp_534520(connection.Owner(), formattedTimestamp));
    const std::int16_t selected = connection.LoginRequests().selectedCharacterIndex;
    if (connection.Owner().WorldScene().mail.data->profiles[selected].value00 == key) {
        PrependMiniMailMessage_5343C0(connection.Owner(), index, 1, formattedTimestamp, cg::text::FromGbkWide(text));
        return;
    }
    PrependMiniMailMessage_5343C0(connection.Owner(), index, 0, formattedTimestamp, cg::text::FromGbkWide(text));
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    cg::text::FormatTo(message, kReceivedMiniMailFormat58B108, record.name10);
    AppendChatMessage_44B050(connection.Owner(), message, 4, 3);
}

// 原函数：0x00518C80。
std::int32_t ClearContactAndGuildMailStorage_518C80(session::ClientSession& session)
{
    session.WorldScene().mail.data->messages.fill({});
    session.WorldScene().mail.data->guildMessages.fill({});
    session.WorldScene().mail.data->contactPage = 0;
    return 0;
}

// 原函数：0x00533910。
void ClearMiniMailAndProfiles_533910(session::ClientSession& session)
{
    for (MiniMailContactRecord& record : session.WorldScene().mail.data->miniMail.contacts) {
        record.used = 0;
    }
    session.WorldScene().mail.data->miniMail.messages.fill({});
    session.WorldScene().mail.data->profiles.fill({});
    session.WorldScene().mail.data->currentProfile = {};
}

// 原函数：0x00533950。
void InitializeDefaultProfiles_533950(session::ClientSession& session)
{
    ProfileRuntimeRecord& first = session.WorldScene().mail.data->profiles[0];
    first.text08.clear();
    first.value108 = -1;
    first.value04 = 0;
    first.text110.clear();
    first.text210.clear();
    first.text310.clear();
    for (ProfileEntry& entry : first.entries410) {
        entry.value00 = 0;
        entry.value04 = 0x3C0F0;
        entry.text08 = kProfileDefault58F614;
    }
    // 原533AB5在最后文本复制完成前取S16角色，两次整条复制共用此值。
    const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    first.text728 = kProfileDefault58F614;
    session.WorldScene().mail.data->profiles[1] = first;
    session.WorldScene().mail.data->currentProfile = session.WorldScene().mail.data->profiles[selected];
}

// 原函数：0x00533B00。
std::int32_t HasProfileDraftChanges_533B00(session::ClientSession& session)
{
    const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    const ProfileRuntimeRecord& saved = session.WorldScene().mail.data->profiles[selected];
    const std::int32_t firstCurrent = session.WorldScene().mail.data->currentProfile.entries410[0].value00;
    const std::int32_t firstSaved = saved.entries410[0].value00;
    if (firstSaved != firstCurrent) {
        return 1;
    }
    const std::int32_t secondSaved = saved.entries410[1].value00;
    const std::int32_t secondCurrent = session.WorldScene().mail.data->currentProfile.entries410[1].value00;
    if (secondSaved != secondCurrent) {
        return 1;
    }
    const std::int32_t thirdSaved = saved.entries410[2].value00;
    const std::int32_t thirdCurrent = session.WorldScene().mail.data->currentProfile.entries410[2].value00;
    if (thirdSaved != thirdCurrent) {
        return 1;
    }
    for (std::int32_t index = 0; index < 3; ++index) {
        if (saved.entries410[index].text08 != session.WorldScene().mail.data->currentProfile.entries410[index].text08) {
            return 1;
        }
    }
    return saved.text728 != session.WorldScene().mail.data->currentProfile.text728 ? 1 : 0;
}

// 原函数：0x00533C80。
std::int32_t PublishPlayerProfile_533C80(session::ClientSession& session, const std::int32_t mode)
{
    if (mode != 0 && mode != 1) {
        return mode - 1;
    }
    if (mode == 0) {
        for (ProfileEntry& entry : session.WorldScene().mail.data->currentProfile.entries410) {
            if (entry.text08.empty()) {
                entry.text08 = kProfileDefault58F614;
            }
        }
        if (session.WorldScene().mail.data->currentProfile.text728.empty()) {
            session.WorldScene().mail.data->currentProfile.text728 = kProfileDefault58F614;
        }
    }

    std::wstring sourceText;
    std::array<std::wstring, 4> escaped;
    for (std::int32_t index = 0; index < 3; ++index) {
        const wchar_t* source = L"不公开"; // 原常量参考：0x0058F61C。
        if (session.WorldScene().mail.data->currentProfile.entries410[index].value00 != 0) {
            if (mode == 1) {
                const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
                source = session.WorldScene().mail.data->profiles[selected].entries410[index].text08.data();
            } else {
                source = session.WorldScene().mail.data->currentProfile.entries410[index].text08.data();
            }
        }
        sourceText = source;
        cg::core::NullSub_509080();
        escaped[index] = input::EscapeText(sourceText, 256);
    }
    if (mode == 1) {
        const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
        sourceText = session.WorldScene().mail.data->profiles[selected].text728;
    } else {
        sourceText = session.WorldScene().mail.data->currentProfile.text728;
    }
    cg::core::NullSub_509080();
    escaped[3] = input::EscapeText(sourceText, 256);

    if (mode == 1) {
        const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
        const std::int32_t third = session.WorldScene().mail.data->profiles[selected].entries410[2].value00;
        const std::int32_t second = session.WorldScene().mail.data->profiles[selected].entries410[1].value00;
        const std::int32_t first = session.WorldScene().mail.data->profiles[selected].entries410[0].value00;
        const auto networkSession = session.Connection().CaptureSession();
        return static_cast<std::int32_t>(session.Connection().SendPlayerProfile(
            first, escaped[0], second, escaped[1], third, escaped[2], escaped[3]));
    }

    const std::int32_t third = session.WorldScene().mail.data->currentProfile.entries410[2].value00;
    const std::int32_t second = session.WorldScene().mail.data->currentProfile.entries410[1].value00;
    const std::int32_t first = session.WorldScene().mail.data->currentProfile.entries410[0].value00;
    const auto networkSession = session.Connection().CaptureSession();
    static_cast<void>(session.Connection().SendPlayerProfile(first, escaped[0], second, escaped[1],
        third, escaped[2], escaped[3]));

    const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    ProfileRuntimeRecord& target = session.WorldScene().mail.data->profiles[selected];
    target.text08 = session.WorldScene().mail.data->currentProfile.text08;
    target.entries410[0].value00 = session.WorldScene().mail.data->currentProfile.entries410[0].value00;
    target.entries410[0].value04 = session.WorldScene().mail.data->currentProfile.entries410[0].value04;
    target.entries410[0].text08 = session.WorldScene().mail.data->currentProfile.entries410[0].text08;
    const std::int32_t secondValue = session.WorldScene().mail.data->currentProfile.entries410[1].value04;
    target.entries410[1].value00 = session.WorldScene().mail.data->currentProfile.entries410[1].value00;
    target.entries410[1].value04 = secondValue;
    target.entries410[1].text08 = session.WorldScene().mail.data->currentProfile.entries410[1].text08;
    const std::int32_t thirdValue = session.WorldScene().mail.data->currentProfile.entries410[2].value04;
    target.entries410[2].value00 = session.WorldScene().mail.data->currentProfile.entries410[2].value00;
    target.entries410[2].value04 = thirdValue;
    target.entries410[2].text08 = session.WorldScene().mail.data->currentProfile.entries410[2].text08;
    const auto bytes = static_cast<std::int32_t>(
        cg::text::ToGbk(session.WorldScene().mail.data->currentProfile.text728).size() + 1);
    target.text728 = session.WorldScene().mail.data->currentProfile.text728;
    return bytes;
}

// 原函数：0x00534290。
network::SendResult ResendSavedPlayerProfile_534290(session::ClientSession& session)
{
    const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    const std::int32_t enabled = session.WorldScene().mail.data->profiles[selected].value04;
    static_cast<void>(PublishPlayerProfile_533C80(session, 1));
    const auto networkSession = session.Connection().CaptureSession();
    return session.Connection().SetProfileVisibilityRequest(enabled != 0 ? 1 : 0);
}

// 原函数：0x0050F2A0。
std::int32_t HandlePrimaryProfileCategoriesReply_50F2A0(cg::network::CgConnection& connection, const char* const payload)
{
    for (std::int32_t index = 0; index < 100; ++index) {
        connection.Owner().WorldScene().mail.data->category.records[index].used = 0;
        connection.Owner().WorldScene().mail.data->category.records[index + 100].used = 0;
    }
    connection.Owner().WorldScene().mail.data->category.value00 = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    connection.Owner().WorldScene().mail.data->category.count08 = 0;
    std::int32_t field = 2;
    std::int32_t count = 0;
    for (;;) {
        CategoryRecord& source = connection.Owner().WorldScene().mail.data->category.records[count];
        source.used = 1;
        source.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        source.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        char text0CBytes[256]{};
        const std::int32_t status = network::ExtractDelimitedField_52CED0(
            payload, '|', field++, 255, text0CBytes);
        input::UnescapeLegacyTextAndNotify_52D2F0(text0CBytes);
        source.text0C = cg::text::FromGbkWide(text0CBytes);
        CategoryRecord& target = connection.Owner().WorldScene().mail.data->category.records[count + 100];
        target.used = 1;
        target.value04 = source.value04;
        target.value08 = source.value08;
        target.text0C = source.text0C;
        ++count;
        connection.Owner().WorldScene().mail.data->category.count08 = count;
        if (status == 1) {
            return count;
        }
    }
}

// 原函数：0x0050F3A0。
std::int32_t HandleProfileCategoryVersionsReply_50F3A0(cg::network::CgConnection& networkConnection, const char* const payload)
{
    const std::int32_t first = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    const std::int32_t second = network::ParseDelimitedInteger_52CFA0(payload, '|', 2);
    networkConnection.Owner().WorldScene().mail.data->category.value10 = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    const std::int32_t key = network::ParseDelimitedInteger_52CFA0(payload, '|', 4);
    const std::int16_t selected = networkConnection.LoginRequests().selectedCharacterIndex;
    networkConnection.Owner().WorldScene().mail.data->profiles[selected].value00 = key;
    const std::int32_t previous = networkConnection.Owner().WorldScene().mail.data->category.value00;
    std::int32_t mask = previous != first ? 1 : 0;
    if (networkConnection.Owner().WorldScene().mail.data->category.value04 != second) {
        mask |= 2;
    }
    if (mask != 0) {
        return static_cast<std::int32_t>(networkConnection.RequestProfileCategoryUpdates(mask));
    }
    return previous;
}

// 原函数：0x0050F8E0。
std::int32_t HandleSecondaryProfileCategoriesReply_50F8E0(cg::network::CgConnection& connection, const char* const payload)
{
    for (std::int32_t index = 200; index < 300; ++index) {
        connection.Owner().WorldScene().mail.data->category.records[index].used = 0;
    }
    connection.Owner().WorldScene().mail.data->category.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    connection.Owner().WorldScene().mail.data->category.count0C = 0;
    std::int32_t field = 2;
    std::int32_t count = 0;
    for (;;) {
        CategoryRecord& record = connection.Owner().WorldScene().mail.data->category.records[count + 200];
        record.used = 1;
        record.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        record.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        char text0CBytes[256]{};
        const std::int32_t status = network::ExtractDelimitedField_52CED0(
            payload, '|', field++, 255, text0CBytes);
        input::UnescapeLegacyTextAndNotify_52D2F0(text0CBytes);
        record.text0C = cg::text::FromGbkWide(text0CBytes);
        ++count;
        connection.Owner().WorldScene().mail.data->category.count0C = count;
        if (status == 1) {
            return count;
        }
    }
}

// 原函数：0x004C9E40。
void HandleProfileCategorySelectionReply_4C9E40(session::ClientSession& session, const std::int32_t, char* const payload)
{
    input::UnescapeLegacyText_52D150(payload);
    std::int32_t field = 1;
    for (std::int32_t index = 0; index < session.WorldScene().mail.data->category.count08; ++index) {
        const auto mask = static_cast<std::uint8_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field++));
        session.WorldScene().mail.data->category.records[index].selected10C = mask & 1u;
        session.WorldScene().mail.data->category.records[index + 100].selected10C = (mask >> 1) & 1u;
    }
    for (std::int32_t index = 0; index < session.WorldScene().mail.data->category.count0C; ++index) {
        const auto mask = static_cast<std::uint8_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field++));
        session.WorldScene().mail.data->category.records[index + 200].selected10C = mask & 1u;
    }
}

// 原函数：0x005342F0。
std::int32_t FindProfileCategoryImage_5342F0(session::ClientSession& session, const std::int32_t key, const std::int32_t mode)
{
    if (mode != 0 && mode != 1) {
        return -1;
    }
    const CategoryRecord* record = session.WorldScene().mail.data->category.records.data()
        + (mode == 0 ? 0 : 200);
    const CategoryRecord* const end = record + 100;
    for (; record != end; ++record) {
        if (record->used == 1 && record->value04 == key) {
            return record->value08;
        }
    }
    return -1;
}

// 原函数：0x004CA330。
void HandleProfileSearchResultsReply_4CA330(session::ClientSession& session, const std::int32_t, char* const payload)
{
    for (ProfileSearchRecord& record : session.WorldScene().mail.data->profileSearch.records) {
        record.used = 0;
    }
    input::UnescapeLegacyText_52D150(payload);
    session.WorldScene().mail.data->profileSearch.value00 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    session.WorldScene().mail.data->profileSearch.value04 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 2);
    std::int32_t field = 3;
    for (ProfileSearchRecord& record : session.WorldScene().mail.data->profileSearch.records) {
        if (network::ParseDelimitedInteger_52CFA0(payload, '|', field) == -1) {
            return;
        }
        record.used = 1;
        record.value104 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        record.value108 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        char text04Bytes[256]{};
        network::ExtractDelimitedField_52CED0(
            payload, '|', field++, 255, text04Bytes);
        input::UnescapeLegacyTextAndNotify_52D2F0(text04Bytes);
        record.text04 = cg::text::FromGbkWide(text04Bytes);
        char text10CBytes[256]{};
        network::ExtractDelimitedField_52CED0(
            payload, '|', field++, 255, text10CBytes);
        input::UnescapeLegacyTextAndNotify_52D2F0(text10CBytes);
        record.text10C = cg::text::FromGbkWide(text10CBytes);
    }
}

// 原函数：0x004CAC60。
std::int32_t HandleViewedPlayerProfileReply_4CAC60(session::ClientSession& session, const std::int32_t, char* const payload)
{
    input::UnescapeLegacyText_52D150(payload);
    ProfileRuntimeRecord& record = session.WorldScene().mail.data->viewedProfile;
    char text[256];
    record.value00 = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    record.text08 = cg::text::FromGbkWide(text);
    record.value108 = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    network::ExtractDelimitedField_52CED0(payload, '|', 4, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    record.text210 = cg::text::FromGbkWide(text);
    network::ExtractDelimitedField_52CED0(payload, '|', 5, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    record.text110 = cg::text::FromGbkWide(text);
    network::ExtractDelimitedField_52CED0(payload, '|', 6, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    record.text310 = cg::text::FromGbkWide(text);
    record.value10C = network::ParseDelimitedInteger_52CFA0(payload, '|', 7);
    record.entries410[0].value00 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 8);
    network::ExtractDelimitedField_52CED0(payload, '|', 9, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    record.entries410[0].text08 = cg::text::FromGbkWide(text);
    record.entries410[1].value00 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 10);
    network::ExtractDelimitedField_52CED0(payload, '|', 11, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    record.entries410[1].text08 = cg::text::FromGbkWide(text);
    record.entries410[2].value00 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 12);
    network::ExtractDelimitedField_52CED0(payload, '|', 13, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    record.entries410[2].text08 = cg::text::FromGbkWide(text);
    network::ExtractDelimitedField_52CED0(payload, '|', 14, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    record.text728 = cg::text::FromGbkWide(text);
    record.entries410[1].value04 = 246000;
    record.entries410[0].value04 = 246000;
    record.entries410[2].value04 = 246000;
    const std::int32_t second = FindProfileCategoryImage_5342F0(session, record.entries410[1].value00, 0);
    if (second != -1) {
        record.entries410[1].value04 = second;
    }
    const std::int32_t first = FindProfileCategoryImage_5342F0(session, record.entries410[0].value00, 0);
    if (first != -1) {
        record.entries410[0].value04 = first;
    }
    const std::int32_t third = FindProfileCategoryImage_5342F0(session, record.entries410[2].value00, 1);
    if (third != -1) {
        record.entries410[2].value04 = third;
    }
    return third;
}

// 原函数：0x0050F450。
void HandlePlayerProfileReply_50F450(cg::network::CgConnection& connection, const char* const text1, const std::int32_t value2,
    const char* const text3, const char* const text4, const char* const text5,
    const std::int32_t value6, const std::int32_t value7, const char* const text8,
    const std::int32_t value9, const char* const text10, const std::int32_t value11,
    const char* const text12, const char* const text13)
{
    ProfileRuntimeRecord& record = connection.Owner().WorldScene().mail.data->receivedProfile;
    std::array<std::array<char, 256>, 8> buffers;
    record.value10C = value6;
    std::strcpy(buffers[4].data(), text1);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[4].data());
    record.text08 = cg::text::FromGbkWide(buffers[4].data());
    record.value108 = value2;
    std::strcpy(buffers[2].data(), text3);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[2].data());
    record.text210 = cg::text::FromGbkWide(buffers[2].data());
    std::strcpy(buffers[6].data(), text4);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[6].data());
    record.text110 = cg::text::FromGbkWide(buffers[6].data());
    std::strcpy(buffers[0].data(), text5);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[0].data());
    record.text310 = cg::text::FromGbkWide(buffers[0].data());
    record.entries410[1].value00 = value9;
    std::strcpy(buffers[1].data(), text10);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[1].data());
    record.entries410[1].text08 = cg::text::FromGbkWide(buffers[1].data());
    record.entries410[0].value00 = value7;
    std::strcpy(buffers[3].data(), text8);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[3].data());
    record.entries410[0].text08 = cg::text::FromGbkWide(buffers[3].data());
    record.entries410[2].value00 = value11;
    std::strcpy(buffers[5].data(), text12);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[5].data());
    record.entries410[2].text08 = cg::text::FromGbkWide(buffers[5].data());
    std::strcpy(buffers[7].data(), text13);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[7].data());
    record.text728 = cg::text::FromGbkWide(buffers[7].data());
    record.entries410[1].value04 = 246000;
    record.entries410[0].value04 = 246000;
    record.entries410[2].value04 = 246000;
    const std::int32_t second = FindProfileCategoryImage_5342F0(connection.Owner(), record.entries410[1].value00, 0);
    if (second != -1) {
        record.entries410[1].value04 = second;
    }
    const std::int32_t first = FindProfileCategoryImage_5342F0(connection.Owner(), record.entries410[0].value00, 0);
    if (first != -1) {
        record.entries410[0].value04 = first;
    }
    const std::int32_t third = FindProfileCategoryImage_5342F0(connection.Owner(), record.entries410[2].value00, 1);
    if (third != -1) {
        record.entries410[2].value04 = third;
    }
    static_cast<void>(ui::WindowProfileReply_Open(connection.Owner()));
}

// 原函数：0x005348A0。
std::int32_t SaveProfileCategoryFile_5348A0(session::ClientSession& session)
{
    auto disk = EncodeCategories(session);
    std::FILE* const stream = _wfopen(L"bin\\category.dat", L"wb+");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(std::fwrite(&disk, 1, sizeof(disk), stream));
    static_cast<void>(std::fclose(stream));
    return 1;
}

// 原函数：0x00534910。
std::int32_t LoadProfileCategoryFile_534910(session::ClientSession& session)
{
    auto disk = EncodeCategories(session);
    std::FILE* const stream = _wfopen(L"bin\\category.dat", L"rb+");
    if (stream == nullptr) {
        return 0;
    }
    const std::size_t bytes = std::fread(&disk, 1, sizeof(disk), stream);
    static_cast<void>(std::fclose(stream));
    DecodeCategories(session, disk, bytes);
    return 1;
}

// 原函数：0x005345A0。
std::int32_t ResetCharacterProfile_5345A0(session::ClientSession& session, const std::int32_t index)
{
    ProfileRuntimeRecord& record = session.WorldScene().mail.data->profiles[index];
    record.text08.clear();
    record.value108 = -1;
    record.value04 = 0;
    record.text110.clear();
    record.text210.clear();
    record.text310.clear();
    for (ProfileEntry& entry : record.entries410) {
        entry.value00 = 0;
        entry.value04 = 246000;
        entry.text08 = kProfileDefault58F614;
    }
    // 原534737在末条文本的movsd与movsb之间读取当前角色，不沿用重置实参。
    const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    record.text728 = kProfileDefault58F614;
    session.WorldScene().mail.data->currentProfile = session.WorldScene().mail.data->profiles[selected];
    return selected;
}

// 原函数：0x00534770。
std::int32_t SavePlayerProfilesFile_534770(session::ClientSession& session)
{
    std::array<ProfileDiskRecord, 2> disk{};
    for (std::size_t i = 0; i < disk.size(); ++i) { disk[i] = EncodeProfile(session.WorldScene().mail.data->profiles[i]); }
    const auto path = std::format(L"{}\\profile.dat",
        login::detail::ReadSelectedSavedDirectory(session));
    std::FILE* const stream = _wfopen(path.c_str(), L"wb+");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(std::fwrite(&disk, 1, sizeof(disk), stream));
    static_cast<void>(std::fclose(stream));
    return 1;
}

// 原函数：0x005347F0。
std::int32_t LoadPlayerProfilesFile_5347F0(session::ClientSession& session)
{
    std::array<ProfileDiskRecord, 2> disk{};
    for (std::size_t i = 0; i < disk.size(); ++i) { disk[i] = EncodeProfile(session.WorldScene().mail.data->profiles[i]); }
    const auto path = std::format(L"{}\\profile.dat",
        login::detail::ReadSelectedSavedDirectory(session));
    std::FILE* const stream = _wfopen(path.c_str(), L"rb+");
    if (stream == nullptr) {
        return 0;
    }
    const std::size_t bytes = std::fread(&disk, 1, sizeof(disk), stream);
    static_cast<void>(std::fclose(stream));
    for (std::size_t i = 0; i < disk.size(); ++i) {
        const std::size_t offset = i * sizeof(ProfileDiskRecord);
        if (bytes <= offset) { break; }
        DecodeProfile(session.WorldScene().mail.data->profiles[i], disk[i], bytes - offset);
    }
    const std::int16_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    session.WorldScene().mail.data->currentProfile = session.WorldScene().mail.data->profiles[selected];
    return 1;
}

// 原函数：0x00518CB0。
std::int32_t LoadContactAndGuildMailFiles_518CB0(session::ClientSession& session)
{
    const auto mailPath = std::format(L"{}\\mail.dat",
        login::detail::ReadSelectedSavedDirectory(session));
    std::FILE* stream = _wfopen(mailPath.c_str(), L"rb");
    if (stream == nullptr) {
        return SaveContactMailFile_519300(session);
    }
    std::unique_ptr<std::FILE, decltype(&std::fclose)> file(stream, &std::fclose);
    // 先编码旧值，保留短读时未覆盖的记录部分。
    std::vector<MailDiskMessage> mailDisk(session.WorldScene().mail.data->messages.size());
    for (std::size_t i = 0; i < mailDisk.size(); ++i) {
        mailDisk[i] = EncodeMailMessage(session.WorldScene().mail.data->messages[i]);
    }
    const std::size_t mailBytes = std::fread(mailDisk.data(), 1, mailDisk.size() * sizeof(MailDiskMessage), stream);
    static_cast<void>(std::fclose(file.release()));
    for (std::size_t i = 0; i * sizeof(MailDiskMessage) < mailBytes; ++i) {
        DecodeMailMessage(session.WorldScene().mail.data->messages[i], mailDisk[i], mailBytes - i * sizeof(MailDiskMessage));
    }

    const auto guildPath = std::format(L"{}\\guildmail.dat",
        login::detail::ReadSelectedSavedDirectory(session));
    stream = _wfopen(guildPath.c_str(), L"rb");
    if (stream == nullptr) {
        return SaveGuildMailFile_519550(session);
    }
    file.reset(stream);
    std::vector<GuildMailDiskEntry> guildDisk(session.WorldScene().mail.data->guildMessages.size());
    for (std::size_t i = 0; i < guildDisk.size(); ++i) {
        guildDisk[i] = EncodeGuildMailEntry(session.WorldScene().mail.data->guildMessages[i]);
    }
    const std::size_t guildBytes = std::fread(guildDisk.data(), 1, guildDisk.size() * sizeof(GuildMailDiskEntry), stream);
    for (std::size_t i = 0; i * sizeof(GuildMailDiskEntry) < guildBytes; ++i) {
        DecodeGuildMailEntry(session.WorldScene().mail.data->guildMessages[i], guildDisk[i], guildBytes - i * sizeof(GuildMailDiskEntry));
    }
    return std::fclose(file.release());
}

// 原函数：0x00519300。
std::int32_t SaveContactMailFile_519300(session::ClientSession& session)
{
    const auto path = std::format(L"{}\\mail.dat",
        login::detail::ReadSelectedSavedDirectory(session));
    std::vector<MailDiskMessage> disk(session.WorldScene().mail.data->messages.size());
    for (std::size_t i = 0; i < disk.size(); ++i) {
        disk[i] = EncodeMailMessage(session.WorldScene().mail.data->messages[i]);
    }
    std::FILE* const stream = _wfopen(path.c_str(), L"wb");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(std::fwrite(disk.data(), 1, disk.size() * sizeof(MailDiskMessage), stream));
    return std::fclose(stream);
}

// 原函数：0x00519550。
std::int32_t SaveGuildMailFile_519550(session::ClientSession& session)
{
    const auto path = std::format(L"{}\\guildmail.dat",
        login::detail::ReadSelectedSavedDirectory(session));
    std::vector<GuildMailDiskEntry> disk(session.WorldScene().mail.data->guildMessages.size());
    for (std::size_t i = 0; i < disk.size(); ++i) {
        disk[i] = EncodeGuildMailEntry(session.WorldScene().mail.data->guildMessages[i]);
    }
    std::FILE* const stream = _wfopen(path.c_str(), L"wb");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(std::fwrite(disk.data(), 1, disk.size() * sizeof(GuildMailDiskEntry), stream));
    return std::fclose(stream);
}

// 原函数：0x00519290。
std::int32_t ClearCurrentCharacterContactMail_519290(session::ClientSession& session)
{
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    std::fill_n(session.WorldScene().mail.data->messages.data() + selected * 60 * 10, 60 * 10, MailMessage{});
    return 0;
}

// 原函数：0x005194C0。
std::int32_t ClearCurrentCharacterGuildMail_5194C0(session::ClientSession& session)
{
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    std::fill_n(session.WorldScene().mail.data->guildMessages.data() + selected * 200, 200, GuildMailEntry{});
    return 0;
}

// 原函数：0x005192C0。
std::int32_t ClearContactMailMessages_5192C0(session::ClientSession& session, const std::int32_t number)
{
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    std::fill_n(session.WorldScene().mail.data->messages.data() + selected * 60 * 10
        + static_cast<std::ptrdiff_t>(number) * 10, 10, MailMessage{});
    return 0;
}

// GML01：原selected S16/200邮件槽W32==1最后匹配，无则-1。
// 原函数：0x005195F0。
std::int32_t FindLastActiveGuildMailEntry_5195F0(session::ClientSession& session, const std::int32_t key)
{
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    const GuildMailEntry* entry =
        session.WorldScene().mail.data->guildMessages.data() + selected * 200;
    std::int32_t result = -1;
    for (std::int32_t index = 0; index < 200; ++index) {
        const std::uint32_t active = entry->active;
        if (active == 1) {
            const std::int32_t entryKey = entry->key;
            if (entryKey == key) {
                result = index;
            }
        }
        ++entry;
    }
    return result;
}

// GML01：原末匹配后fresh角色，整C88清零（包含active/key），原S32返回保持。
// 原函数：0x005194F0。
std::int32_t DeleteGuildMailEntry_5194F0(session::ClientSession& session, const std::int32_t key)
{
    const std::int32_t index = FindLastActiveGuildMailEntry_5195F0(session, key);
    if (index == -1) {
        return index;
    }
    const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
    GuildMailEntry* const entry = session.WorldScene().mail.data->guildMessages.data()
        + selected * 200 + static_cast<std::ptrdiff_t>(index);
    *entry = {};
    return 0;
}

// GML01：原查找门槛/分支fresh角色、空槽或九次倒序移位，kind两BYTE及name/text发布序保持。
// 原函数：0x00519370。
std::uintptr_t PrependGuildMailMessage_519370(session::ClientSession& session,
    const std::int32_t key, const std::int32_t kind,
    const std::wstring_view name, const std::wstring_view text)
{
    if (cg::world::FindActiveOrderedRecordIndex_5195C0(session, key) == -1) {
        return 0xFFFFFFFFu;
    }
    const std::int32_t existingIndex = FindLastActiveGuildMailEntry_5195F0(session, key);
    GuildMailEntry* entry;
    if (existingIndex == -1) {
        const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
        const std::ptrdiff_t groupBytes = 200 * sizeof(GuildMailEntry);
        const std::ptrdiff_t groupOffset = selected * groupBytes;
        std::int32_t index = 0;
        for (; index < 200; ++index) {
            entry = session.WorldScene().mail.data->guildMessages.data() + selected * 200 + index;
            const std::uint32_t active = entry->active;
            if (active == 0) {
                break;
            }
        }
        if (index == 200) {
            // 原 EAX 是满组末地址再加 4；保持原生宽度，不构造越界指针。
            return reinterpret_cast<std::uintptr_t>(session.WorldScene().mail.data->guildMessages.data())
                + static_cast<std::uintptr_t>(groupOffset + groupBytes + 4);
        }
        const std::uint32_t active = 1;
        entry->key = key;
        entry->active = active;
    } else {
        const std::ptrdiff_t selected = session.Connection().LoginRequests().selectedCharacterIndex;
        entry = session.WorldScene().mail.data->guildMessages.data() + selected * 200 + existingIndex;
        for (std::ptrdiff_t message = 9; message != 0; --message) {
            entry->messages[message] = entry->messages[message - 1];
        }
    }
    entry->messages[0].unread = static_cast<std::byte>(kind == 0);
    entry->messages[0].kind = static_cast<std::byte>(static_cast<std::uint8_t>(kind));
    entry->messages[0].name = name;
    const auto textBytes = static_cast<std::uint32_t>(cg::text::ToGbk(text).size() + 1);
    entry->messages[0].text = text;
    return textBytes;
}

std::uintptr_t PruneGuildMailAgainstOrderedRecords_50EE4A(session::ClientSession& session, const std::int16_t selected)
{
    GuildMailEntry* entry = session.WorldScene().mail.data->guildMessages.data()
        + static_cast<std::ptrdiff_t>(selected) * 200;
    const std::uintptr_t keyEnd =
        reinterpret_cast<std::uintptr_t>(session.WorldScene().guild.orderedRecords.data())
        + sizeof(session.WorldScene().guild.orderedRecords)
        + offsetof(cg::world::OrderedRecord1051B14, value0C);
    std::uintptr_t result = 0;
    for (std::int32_t index = 0; index < 200; ++index) {
        const std::int32_t key = entry->key;
        bool found = false;
        for (const cg::world::OrderedRecord1051B14& record :
             session.WorldScene().guild.orderedRecords) {
            if (key == record.value0C) {
                found = true;
            }
        }
        result = keyEnd;
        if (!found) {
            result = 0;
            *entry = {};
        }
        ++entry;
    }
    return result;
}

} // namespace cg::world::detail
