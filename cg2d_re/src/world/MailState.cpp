#include "cg/core/LegacyNoOp.hpp"
#include "../scene/login/LoginSavedState.hpp"
#include "MailState.hpp"
#include "cg/text/Encoding.hpp"

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

namespace cg::world::detail {

ProfileRuntimeRecord gCurrentProfile10738E8 {}; // 原全局：0x010738E8。
ProfileRuntimeRecord gReceivedProfile1074110 {}; // 原全局：0x01074110。
ProfileRuntimeRecord gViewedProfile105ECE0 {}; // 原全局：0x0105ECE0。
ProfileSearchRuntimeSlots gProfileSearch105C398 {}; // 原全局：0x0105C398。

std::array<ContactRuntimeRecord, 60> gContactsF19FB0 {}; // 原全局：0x00F19FB0。
std::array<std::int32_t, 60> gContactIndicesFDA608 {}; // 原全局：0x00FDA608。
std::uint16_t gUnreadMailCategory_62B354 = 0; // 原全局：0x0062B354。
std::uint32_t gMailContactPage_CB1384 = 0; // 原全局：0x00CB1384。
std::uint32_t gMailFallbackPage_CB0354 = 0; // 原全局：0x00CB0354。
std::int32_t gViewedMailContactId_CF4FC0 = 0; // 原全局：0x00CF4FC0。
std::int32_t gViewedMailMessageIndex_D046D4 = 0; // 原全局：0x00D046D4。
std::int32_t gMiniMailSelectionD673E4 = 0; // 原全局：0x00D673E4。 4C9490清-1，窗口63保存同一发信目标编号。
static_assert(sizeof(gContactsF19FB0) == 0x1B30);
static_assert(sizeof(gContactIndicesFDA608) == 0xF0);

// 三类收信共用的140H消息；文件、回复与窗口借用同一存储。
struct MailMessage {
    std::byte unread; // +00：未读标记，窗口可清零。
    std::byte kind; // +01：收信类别低字节。
    std::array<char, 18> name; // +02：原编码名称。
    std::array<char, 300> text; // +14：原编码正文，保既有复制长度规则。
};
struct GuildMailEntry {
    std::int32_t key; // +00：公会成员键。
    std::uint32_t active; // +04：查找要求恰为1，未读扫描只判断非零。
    std::array<MailMessage, 10> messages; // +08：同一成员的十条消息。
};
static_assert(std::is_standard_layout_v<MailMessage>);
static_assert(std::is_trivially_copyable_v<MailMessage>);
static_assert(offsetof(MailMessage, unread) == 0);
static_assert(offsetof(MailMessage, kind) == 1);
static_assert(offsetof(MailMessage, name) == 2);
static_assert(offsetof(MailMessage, text) == 0x14);
static_assert(sizeof(MailMessage) == 0x140);
static_assert(alignof(MailMessage) == 1);
static_assert(std::is_standard_layout_v<GuildMailEntry>);
static_assert(std::is_trivially_copyable_v<GuildMailEntry>);
static_assert(offsetof(GuildMailEntry, key) == 0);
static_assert(offsetof(GuildMailEntry, active) == 4);
static_assert(offsetof(GuildMailEntry, messages) == 8);
static_assert(sizeof(GuildMailEntry) == 0xC88);
static_assert(alignof(GuildMailEntry) == 4);

// 连续消息数组保留原跨联系人步进；UI只经既有视图借用。
static alignas(std::uint32_t) std::array<MailMessage, 2 * 60 * 10> gMailStorageF7CA08 {}; // 原全局：0x00F7CA08。
static std::array<GuildMailEntry, 2 * 200> gGuildMailStorageDE0B30 {}; // 原全局：0x00DE0B30。

struct alignas(std::uint32_t) MiniMailRuntimeSlots {
    std::array<MiniMailContactRecord, 20> contacts;
    std::array<MailMessage, 20 * 10> messages; // 原10763A0，20组各十条140H；保原连续访问。
};

static_assert(sizeof(MiniMailContactRecord) == 0x74);
static_assert(std::is_standard_layout_v<MiniMailRuntimeSlots>);
static_assert(std::is_trivially_copyable_v<MiniMailRuntimeSlots>);
static_assert(offsetof(MiniMailContactRecord, used) == 0);
static_assert(offsetof(MiniMailContactRecord, sequence02) == 2);
static_assert(offsetof(MiniMailContactRecord, key04) == 4);
static_assert(offsetof(MiniMailContactRecord, value08) == 8);
static_assert(offsetof(MiniMailContactRecord, value0C) == 0x0C);
static_assert(offsetof(MiniMailContactRecord, name10) == 0x10);
static_assert(offsetof(MiniMailContactRecord, text21) == 0x21);
static_assert(offsetof(MiniMailContactRecord, text32) == 0x32);
static_assert(offsetof(MiniMailContactRecord, bytes53) == 0x53);
static_assert(offsetof(MiniMailRuntimeSlots, contacts) == 0);
static_assert(offsetof(MiniMailRuntimeSlots, messages) == 0x910);
static_assert(sizeof(MiniMailRuntimeSlots) == 0x10310);
static MiniMailRuntimeSlots gMiniMailRuntime {};
static std::uint32_t gMiniMailSequence1075A88 = 0; // 原全局：0x01075A88。 W32序号，先发布低W16再递增。

static_assert(offsetof(ProfileEntry, value00) == 0);
static_assert(offsetof(ProfileEntry, value04) == 4);
static_assert(offsetof(ProfileEntry, text08) == 8);
static_assert(sizeof(ProfileEntry) == 0x108);
static_assert(std::is_standard_layout_v<ProfileRuntimeRecord>);
static_assert(std::is_trivially_copyable_v<ProfileRuntimeRecord>);
static_assert(offsetof(ProfileRuntimeRecord, value00) == 0);
static_assert(offsetof(ProfileRuntimeRecord, value04) == 4);
static_assert(offsetof(ProfileRuntimeRecord, text08) == 8);
static_assert(offsetof(ProfileRuntimeRecord, value108) == 0x108);
static_assert(offsetof(ProfileRuntimeRecord, value10C) == 0x10C);
static_assert(offsetof(ProfileRuntimeRecord, text110) == 0x110);
static_assert(offsetof(ProfileRuntimeRecord, text210) == 0x210);
static_assert(offsetof(ProfileRuntimeRecord, text310) == 0x310);
static_assert(offsetof(ProfileRuntimeRecord, entries410) == 0x410);
static_assert(offsetof(ProfileRuntimeRecord, text728) == 0x728);
static_assert(sizeof(ProfileRuntimeRecord) == 0x828);

// 持久双槽、当前资料、DpSY回复与4CAC60查看资料各自拥有存储。
static std::array<ProfileRuntimeRecord, 2> gProfiles1074938 {}; // 原全局：0x01074938。
static_assert(sizeof(gProfiles1074938) == 0x1050);
static constexpr wchar_t kProfileDefault58F614[] = L"未设定"; // 原常量参考：0x0058F614。
static_assert(std::is_standard_layout_v<ProfileSearchRecord>);
static_assert(std::is_trivially_copyable_v<ProfileSearchRecord>);
static_assert(offsetof(ProfileSearchRecord, used) == 0);
static_assert(offsetof(ProfileSearchRecord, text04) == 4);
static_assert(offsetof(ProfileSearchRecord, value104) == 0x104);
static_assert(offsetof(ProfileSearchRecord, value108) == 0x108);
static_assert(offsetof(ProfileSearchRecord, text10C) == 0x10C);
static_assert(sizeof(ProfileSearchRecord) == 0x20C);
static_assert(std::is_standard_layout_v<ProfileSearchRuntimeSlots>);
static_assert(std::is_trivially_copyable_v<ProfileSearchRuntimeSlots>);
static_assert(offsetof(ProfileSearchRuntimeSlots, value00) == 0);
static_assert(offsetof(ProfileSearchRuntimeSlots, value04) == 4);
static_assert(offsetof(ProfileSearchRuntimeSlots, records) == 8);
static_assert(sizeof(ProfileSearchRuntimeSlots) == 0x28F8);

struct CategoryRuntimeSlots {
    std::int32_t value00; // 原成员偏移：+0x00。 原105FA10，50F2A0发布、50F3A0比较。
    std::int32_t value04; // 原成员偏移：+0x04。 原105FA14，50F8E0发布、50F3A0比较。
    std::int32_t count08; // 原成员偏移：+0x08。
    std::int32_t count0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。 原105FA20，50F3A0第三字段。
    std::array<CategoryRecord, 300> records; // 原105FA24/1066464/106CEA4三组同源区。
};

static_assert(offsetof(CategoryRecord, used) == 0);
static_assert(offsetof(CategoryRecord, bytes01) == 1);
static_assert(offsetof(CategoryRecord, value04) == 4);
static_assert(offsetof(CategoryRecord, value08) == 8);
static_assert(offsetof(CategoryRecord, text0C) == 0x0C);
static_assert(offsetof(CategoryRecord, selected10C) == 0x10C);
static_assert(offsetof(CategoryRecord, bytes10D) == 0x10D);
static_assert(sizeof(CategoryRecord) == 0x110);
static_assert(std::is_standard_layout_v<CategoryRuntimeSlots>);
static_assert(std::is_trivially_copyable_v<CategoryRuntimeSlots>);
static_assert(offsetof(CategoryRuntimeSlots, value00) == 0);
static_assert(offsetof(CategoryRuntimeSlots, value04) == 4);
static_assert(offsetof(CategoryRuntimeSlots, count08) == 8);
static_assert(offsetof(CategoryRuntimeSlots, count0C) == 0x0C);
static_assert(offsetof(CategoryRuntimeSlots, value10) == 0x10);
static_assert(offsetof(CategoryRuntimeSlots, records) == 0x14);
static_assert(sizeof(CategoryRuntimeSlots) == 0x13ED4);
static CategoryRuntimeSlots gCategory105FA10 {}; // 原全局：0x0105FA10。

static constexpr wchar_t kReceivedMiniMailFormat58B108[] = // 原常量参考：0x0058B108。
    L"提示！您收到来自 {} 的迷你邮件～";

static constexpr std::ptrdiff_t kMailGroupBytes = 0x2EE00;
static constexpr std::ptrdiff_t kMailNumberBytes = 0xC80;
static constexpr std::ptrdiff_t kGuildMailGroupBytes = 0x9CA40;
static constexpr std::ptrdiff_t kGuildMailEntryBytes = 0xC88;
static constexpr std::ptrdiff_t kMailMessageBytes = 0x140;

static_assert(sizeof(gMailStorageF7CA08) == 2 * 60 * 10 * 0x140);
static_assert(sizeof(gGuildMailStorageDE0B30) == 2 * 200 * 0xC88);

static MailMessageView MakeMailMessageView(MailMessage& message)
{
    return {&message.unread, &message.kind, message.name.data(), message.text.data()};
}

// 原函数：0x004EC030。
static std::int32_t CompareContactIndices_4EC030(
    const std::int32_t* const left, const std::int32_t* const right)
{
    const ContactRuntimeRecord& other = gContactsF19FB0[*right];
    const ContactRuntimeRecord& current = gContactsF19FB0[*left];
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
        const int order = std::strcmp(current.name10.data(), other.name10.data());
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

const MiniMailContactRecord& ReadMiniMailContact1075A90(const std::int32_t index)
{
    return gMiniMailRuntime.contacts[index];
}

} // namespace cg::world::detail

namespace cg::world {

MailMessageView GetMailMessageF7CA08(
    const std::int16_t selected, const std::int32_t contact, const std::int32_t page)
{
    return detail::MakeMailMessageView(detail::gMailStorageF7CA08[
        selected * 60 * 10 + static_cast<std::ptrdiff_t>(contact) * 10 + page]);
}

MailMessageView GetGuildMailMessageDE0B30(
    const std::int16_t selected, const std::int32_t slot, const std::int32_t page)
{
    return detail::MakeMailMessageView(detail::gGuildMailStorageDE0B30[
        selected * 200 + static_cast<std::ptrdiff_t>(slot)].messages[page]);
}

MailMessageView GetMiniMailMessage10763A0(
    const std::int32_t contact, const std::int32_t page)
{
    return detail::MakeMailMessageView(detail::gMiniMailRuntime.messages[
        static_cast<std::ptrdiff_t>(contact) * 10 + page]);
}

} // namespace cg::world

namespace cg::world::detail {

const ProfileRuntimeRecord& ReadSelectedProfile1074938()
{
    return gProfiles1074938[login::gSelectedCharacterIndex];
}

std::int32_t ReadCategoryValue105FA20()
{
    return gCategory105FA10.value10;
}

std::int32_t& SelectedProfileValue04()
{
    return gProfiles1074938[login::gSelectedCharacterIndex].value04;
}

const char* ReadCategoryText105FA24(const std::int32_t group, const std::int32_t index)
{
    return gCategory105FA10.records[group * 100 + index].text0C.data();
}

std::int32_t ReadCategoryCount105FA18()
{
    return gCategory105FA10.count08;
}

std::int32_t ReadCategoryCount105FA1C()
{
    return gCategory105FA10.count0C;
}

const CategoryRecord& ReadCategoryRecord105FA24(
    const std::int32_t group, const std::int32_t index)
{
    return gCategory105FA10.records[group * 100 + index];
}

// 原函数：0x004EC1A0。
void SortContactIndices_4EC1A0()
{
    for (std::int32_t index = 0; index < 60; ++index) {
        gContactIndicesFDA608[index] = index;
    }
    std::sort(gContactIndicesFDA608.begin(), gContactIndicesFDA608.end(),
        [](const std::int32_t left, const std::int32_t right) {
            return CompareContactIndices_4EC030(&left, &right) < 0;
        });
}

// 原函数：0x0050D740。
void HandleContactListReply_50D740(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char name[256];
    char text[256];
    std::int32_t field = 2;
    for (ContactRuntimeRecord& record : gContactsF19FB0) {
        if (network::ParseDelimitedInteger_52CFA0(payload, '|', field - 1) <= 0) {
            record.used = 0;
            record.name10[0] = '\0';
            record.text21[0] = '\0';
        } else {
            record.used = 1;
            if (network::ExtractDelimitedField_52CED0(
                    payload, '|', field, 255, name) == 1) {
                return;
            }
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            if (std::strlen(name) <= 16) {
                std::strcpy(record.name10.data(), name);
            }
            network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, text);
            input::UnescapeLegacyTextAndNotify_52D2F0(text);
            if (std::strlen(text) <= 16) {
                std::strcpy(record.text21.data(), text);
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
void HandleContactExtendedTextReply_50D8C0(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char first[256];
    char second[256];
    std::int32_t field = 2;
    for (ContactRuntimeRecord& record : gContactsF19FB0) {
        if (network::ExtractDelimitedField_52CED0(
                payload, '|', field - 1, 255, first) == 1) {
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(first);
        if (std::strlen(first) <= 32) {
            std::strcpy(record.text32.data(), first);
        }
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, second);
        input::UnescapeLegacyTextAndNotify_52D2F0(second);
        if (std::strlen(second) <= 32) {
            std::strcpy(record.text53.data(), second);
        }
        field += 2;
    }
    SortContactIndices_4EC1A0();
}

// 原函数：0x0050D9E0。
void HandleContactUpdateReply_50D9E0(
    const std::uintptr_t, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    const std::int32_t active = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    ContactRuntimeRecord& record = gContactsF19FB0[index];
    if (active <= 0) {
        record.used = 0;
        record.name10[0] = '\0';
        record.text21[0] = '\0';
        return;
    }
    record.used = 1;
    char name[256];
    char text[256];
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 255, name);
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    if (std::strlen(name) <= 16) {
        std::strcpy(record.name10.data(), name);
    }
    network::ExtractDelimitedField_52CED0(payload, '|', 3, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    if (std::strlen(text) <= 16) {
        std::strcpy(record.text21.data(), text);
    }
    record.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', 4);
    record.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', 5);
    record.value02 = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', 6)));
    record.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', 7);
    if (network::ParseDelimitedInteger_52CFA0(payload, '|', 8) == 1) {
        static_cast<void>(ClearContactMailMessages_5192C0(index));
    }
    network::ExtractDelimitedField_52CED0(payload, '|', 9, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    if (std::strlen(text) <= 32) {
        std::strcpy(record.text32.data(), text);
    }
    network::ExtractDelimitedField_52CED0(payload, '|', 10, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    if (std::strlen(text) <= 32) {
        std::strcpy(record.text53.data(), text);
    }
    SortContactIndices_4EC1A0();
}

// 原函数：0x005191C0。
void PrependContactMailMessage_5191C0(
    const std::int32_t index, const std::int32_t kind,
    const char* const name, const char* const text)
{
    if (gContactsF19FB0[index].used == 0) {
        return;
    }
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    MailMessage* const entry = gMailStorageF7CA08.data()
        + selected * 60 * 10 + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::ptrdiff_t message = 9; message > 0; --message) {
        std::memcpy(
            entry + message,
            entry + (message - 1), kMailMessageBytes);
    }
    entry->unread = kind == 0 ? std::byte{1} : std::byte{0};
    entry->kind = static_cast<std::byte>(static_cast<std::uint8_t>(kind));
    std::memcpy(entry->name.data(), name, std::strlen(name) + 1);
    std::memcpy(entry->text.data(), text, std::strlen(text) + 1);
}

// SHP01：原三类未读提示，迷你倒序；U16类别后发布W32页，未命中持有备用页。
// 原函数：0x00518DA0。
std::int32_t UpdateUnreadMailPageHint_518DA0()
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    for (std::int32_t position = 0; position < 60; ++position) {
        const std::int32_t index = gContactIndicesFDA608[position];
        if (gContactsF19FB0[index].used == 0) {
            continue;
        }
        const MailMessage* const entry = gMailStorageF7CA08.data()
            + selected * 60 * 10 + static_cast<std::ptrdiff_t>(index) * 10;
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry[message].unread != std::byte{0}) {
                gUnreadMailCategory_62B354 = 1;
                gMailContactPage_CB1384 = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    for (std::int32_t position = 0; position < 200; ++position) {
        const auto& record = cg::world::gOrderedRecords1051B14[position];
        if (record.value08 == 0) {
            continue;
        }
        const std::int32_t index = FindLastActiveGuildMailEntry_5195F0(record.value0C);
        if (index == -1) {
            continue;
        }
        const std::ptrdiff_t currentSelected = login::gSelectedCharacterIndex;
        const GuildMailEntry* const entry = gGuildMailStorageDE0B30.data()
            + currentSelected * 200 + static_cast<std::ptrdiff_t>(index);
        const std::uint32_t active = entry->active;
        if (active == 0) {
            continue;
        }
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry->messages[message].unread != std::byte{0}) {
                gUnreadMailCategory_62B354 = 2;
                gMailContactPage_CB1384 = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        if (gMiniMailRuntime.contacts[position].used == 0) {
            continue;
        }
        const MailMessage* const entry = gMiniMailRuntime.messages.data()
            + static_cast<std::ptrdiff_t>(position) * 10;
        for (std::ptrdiff_t message = 9; message >= 0; --message) {
            if (entry[message].unread != std::byte{0}) {
                gUnreadMailCategory_62B354 = 3;
                gMailContactPage_CB1384 = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    const std::uint32_t fallbackPage = gMailFallbackPage_CB0354;
    gUnreadMailCategory_62B354 = 0;
    gMailContactPage_CB1384 = fallbackPage;
    return 0;
}

// SHP01：原三类未读定位，迷你正序；先S32类别再索引，公会fresh字符/key，唯一父弃返。
// 原函数：0x00518F00。
void FindFirstUnreadMailContact_518F00(std::int32_t* const category, std::int32_t* const index)
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    for (std::int32_t position = 0; position < 60; ++position) {
        const std::int32_t contact = gContactIndicesFDA608[position];
        if (gContactsF19FB0[contact].used == 0) {
            continue;
        }
        const MailMessage* const entry = gMailStorageF7CA08.data()
            + selected * 60 * 10 + static_cast<std::ptrdiff_t>(contact) * 10;
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry[message].unread != std::byte{0}) {
                *category = 0;
                *index = gContactIndicesFDA608[position];
                return;
            }
        }
    }
    for (std::int32_t position = 0; position < 200; ++position) {
        const auto& member = cg::world::gOrderedRecords1051B14[position];
        if (member.value08 == 0) {
            continue;
        }
        const std::int32_t slot = FindLastActiveGuildMailEntry_5195F0(member.value0C);
        if (slot == -1) {
            continue;
        }
        const std::ptrdiff_t currentSelected = login::gSelectedCharacterIndex;
        const GuildMailEntry* const entry = gGuildMailStorageDE0B30.data()
            + currentSelected * 200 + static_cast<std::ptrdiff_t>(slot);
        const std::uint32_t active = entry->active;
        if (active == 0) {
            continue;
        }
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry->messages[message].unread != std::byte{0}) {
                *category = 1;
                const std::ptrdiff_t selectedForKey = login::gSelectedCharacterIndex;
                const GuildMailEntry* const keyEntry = gGuildMailStorageDE0B30.data()
                    + selectedForKey * 200 + static_cast<std::ptrdiff_t>(slot);
                const std::int32_t key = keyEntry->key;
                *index = cg::world::FindActiveOrderedRecordIndex_5195C0(key);
                return;
            }
        }
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        if (gMiniMailRuntime.contacts[position].used == 0) {
            continue;
        }
        const MailMessage* const entry = gMiniMailRuntime.messages.data()
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
std::int32_t HasUnreadContactMail_519090(const std::int32_t index)
{
    if (index < 0 || index >= 60) {
        return 0;
    }
    if (gContactsF19FB0[index].used == 0) {
        return 0;
    }
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    const MailMessage* const entry = gMailStorageF7CA08.data()
        + selected * 60 * 10 + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::int32_t message = 0; message < 10; ++message) {
        if (entry[message].unread != std::byte{0}) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x005190F0。
std::int32_t HasUnreadGuildMail_5190F0(const std::int32_t index)
{
    const auto& record = cg::world::gOrderedRecords1051B14[index];
    const std::int32_t key = record.value0C;
    const std::int32_t slot = FindLastActiveGuildMailEntry_5195F0(key);
    if (slot == -1) {
        return 0;
    }
    if (index == -1) {
        return 0;
    }
    if (record.value08 == 0) {
        return 0;
    }
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    const MailMessage* const entry = gGuildMailStorageDE0B30[
        selected * 200 + static_cast<std::ptrdiff_t>(slot)].messages.data();
    for (std::int32_t message = 0; message < 10; ++message) {
        if (entry[message].unread != std::byte{0}) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x00519170。
std::int32_t HasUnreadMiniMail_519170(const std::int32_t index)
{
    if (index < 0 || index >= 20) {
        return 0;
    }
    if (gMiniMailRuntime.contacts[index].used == 0) {
        return 0;
    }
    const MailMessage* const entry = gMiniMailRuntime.messages.data()
        + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::int32_t message = 0; message < 10; ++message) {
        if (entry[message].unread != std::byte{0}) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x0050DC70。
void HandleContactMailReply_50DC70(
    const std::uintptr_t, const std::int32_t index,
    const char* const payload, const std::int32_t)
{
    char name[32];
    char text[300];
    std::wstring message;
    network::ExtractDelimitedField_52CED0(payload, '|', 1, 31, name);
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 299, text);
    const std::int32_t kind = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    PrependContactMailMessage_5191C0(index, 0, name, text);
    const char* const contactName = gContactsF19FB0[index].name10.data();
    cg::text::FormatTo(message, kind == -1 ? L"收到从{}寄来的邮件！" /* 原常量参考：0x0058B07C。 */ : L"由{}送来了宠物邮件！" /* 原常量参考：0x0058B064。 */, cg::text::FromGbkWide(contactName).c_str());
    static_cast<void>(SaveContactMailFile_519300());
    static_cast<void>(UpdateUnreadMailPageHint_518DA0());
    if (gViewedMailContactId_CF4FC0 == index) {
        const std::int32_t position = gViewedMailMessageIndex_D046D4;
        if (position < 9) {
            gViewedMailMessageIndex_D046D4 = position + 1;
        }
    }
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    AppendChatMessage_44B050(message, 4, 3);
}

// 原函数：0x00534370。
std::int32_t FindMiniMailContactByKey_534370(const std::int32_t key)
{
    for (std::int32_t index = 0; index < 20; ++index) {
        const MiniMailContactRecord& record = gMiniMailRuntime.contacts[index];
        if (record.used == 1 && record.key04 == key) {
            return index;
        }
    }
    return -1;
}

// 原函数：0x005343A0。
std::int32_t FindFreeMiniMailContactSlot_5343A0()
{
    for (std::int32_t index = 0; index < 20; ++index) {
        if (gMiniMailRuntime.contacts[index].used == 0) {
            return index;
        }
    }
    return -1;
}

// 原函数：0x004C9490。
void ResetMiniMailSequenceAndSelection_4C9490()
{
    gMiniMailSequence1075A88 = 0;
    gMiniMailSelectionD673E4 = -1;
}

// 原函数：0x00534520。
std::int32_t FormatMailTimestamp_534520(char* const destination)
{
    __time64_t timeValue;
    timeValue = ::_time64(nullptr);
    const std::tm* const localTime =
        ::_localtime64(&timeValue);
    const std::int32_t minutes = localTime->tm_min;
    const std::int32_t hours = localTime->tm_hour;
    const std::int32_t day = localTime->tm_mday;
    const std::int32_t month = localTime->tm_mon;
    const std::int32_t year = localTime->tm_year;
    char timestamp[256];
    std::sprintf(
        timestamp, "%4d/%2d/%2d %02d:%02d", year + 1900, month + 1, day, hours, minutes);
    std::strcpy(destination, timestamp);
    return 0;
}

// 原函数：0x005343C0。
void PrependMiniMailMessage_5343C0(
    const std::int32_t index, const std::int32_t kind,
    const char* const name, const char* const text)
{
    if (gMiniMailRuntime.contacts[index].used == 0) {
        return;
    }
    MailMessage* const entry = gMiniMailRuntime.messages.data()
        + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::ptrdiff_t message = 9; message > 0; --message) {
        std::memcpy(
            entry + message,
            entry + (message - 1), kMailMessageBytes);
    }
    entry->unread = kind == 0 ? std::byte{1} : std::byte{0};
    entry->kind = static_cast<std::byte>(static_cast<std::uint8_t>(kind));
    std::memcpy(entry->name.data(), name, std::strlen(name) + 1);
    std::memcpy(entry->text.data(), text, std::strlen(text) + 1);
}

// 原函数：0x00534480。
void RemoveMiniMailContact_534480(const std::int32_t index)
{
    gMiniMailRuntime.contacts[index].used = 0;
    MailMessage* const first = gMiniMailRuntime.messages.data()
        + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::ptrdiff_t message = 0; message < 20; ++message) {
        MailMessage* const entry = first + message;
        const char* text = "";
        std::memcpy(entry->text.data(), text, std::strlen(text) + 1);
        text = "";
        std::memcpy(entry->name.data(), text, std::strlen(text) + 1);
        entry->unread = std::byte{0};
        entry->kind = std::byte{0};
    }
}

// 原函数：0x0050F430。
void HandleMiniMailContactRemovalReply_50F430(const std::uintptr_t, const char* const payload)
{
    const std::int32_t key = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    const std::int32_t index = FindMiniMailContactByKey_534370(key);
    RemoveMiniMailContact_534480(index);
}

// 原函数：0x0050F7F0。
void HandleMiniMailReply_50F7F0(
    const std::uintptr_t, const std::int32_t key,
    const char* const payload, const std::int32_t)
{
    const std::int32_t index = FindMiniMailContactByKey_534370(key);
    if (index == -1) {
        return;
    }
    char name[32];
    char text[512];
    std::wstring message;
    network::ExtractDelimitedField_52CED0(payload, '|', 1, 31, name);
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 511, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    PrependMiniMailMessage_5343C0(index, 0, name, text);
    cg::text::FormatTo(message, kReceivedMiniMailFormat58B108, cg::text::FromGbkWide(gMiniMailRuntime.contacts[index].name10.data()).c_str());
    static_cast<void>(UpdateUnreadMailPageHint_518DA0());
    if (gViewedMailContactId_CF4FC0 == key) {
        const std::int32_t position = gViewedMailMessageIndex_D046D4;
        if (position < 9) {
            gViewedMailMessageIndex_D046D4 = position + 1;
        }
    }
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    AppendChatMessage_44B050(message, 4, 3);
}

// 原函数：0x0050F9E0。
void HandleMiniMailConversationReply_50F9E0(const std::uintptr_t, const char* const payload)
{
    const std::int32_t index = FindFreeMiniMailContactSlot_5343A0();
    if (index == -1) {
        return;
    }
    MailMessage* const first = gMiniMailRuntime.messages.data()
        + static_cast<std::ptrdiff_t>(index) * 10;
    for (std::ptrdiff_t message = 0; message < 20; ++message) {
        MailMessage* const entry = first + message;
        const char* text = "";
        std::memcpy(entry->text.data(), text, std::strlen(text) + 1);
        text = "";
        std::memcpy(entry->name.data(), text, std::strlen(text) + 1);
        entry->unread = std::byte{0};
        entry->kind = std::byte{0};
    }

    MiniMailContactRecord& record = gMiniMailRuntime.contacts[index];
    record.used = 1;
    record.key04 = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    record.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', 2);
    record.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    network::ExtractDelimitedField_52CED0(payload, '|', 4, 16, record.name10.data());
    input::UnescapeLegacyTextAndNotify_52D2F0(record.name10.data());
    network::ExtractDelimitedField_52CED0(payload, '|', 5, 32, record.text32.data());
    input::UnescapeLegacyTextAndNotify_52D2F0(record.text32.data());
    network::ExtractDelimitedField_52CED0(payload, '|', 6, 16, record.text21.data());
    input::UnescapeLegacyTextAndNotify_52D2F0(record.text21.data());
    const std::uint32_t sequence = gMiniMailSequence1075A88;
    record.sequence02 = static_cast<std::uint16_t>(sequence);
    gMiniMailSequence1075A88 = sequence + 1u;

    const std::int32_t key = network::ParseDelimitedInteger_52CFA0(payload, '|', 7);
    char timestamp[256];
    char text[1024];
    std::wstring message;
    network::ExtractDelimitedField_52CED0(payload, '|', 8, 255, timestamp);
    input::UnescapeLegacyTextAndNotify_52D2F0(timestamp);
    network::ExtractDelimitedField_52CED0(payload, '|', 9, 1023, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    static_cast<void>(FormatMailTimestamp_534520(timestamp));
    const std::int16_t selected = login::gSelectedCharacterIndex;
    if (gProfiles1074938[selected].value00 == key) {
        PrependMiniMailMessage_5343C0(index, 1, timestamp, text);
        return;
    }
    PrependMiniMailMessage_5343C0(index, 0, timestamp, text);
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    cg::text::FormatTo(message, kReceivedMiniMailFormat58B108, cg::text::FromGbkWide(record.name10.data()).c_str());
    AppendChatMessage_44B050(message, 4, 3);
}

// 原函数：0x00518C80。
std::int32_t ClearContactAndGuildMailStorage_518C80()
{
    std::memset(gMailStorageF7CA08.data(), 0, sizeof(gMailStorageF7CA08));
    std::memset(gGuildMailStorageDE0B30.data(), 0, sizeof(gGuildMailStorageDE0B30));
    gMailContactPage_CB1384 = 0;
    return 0;
}

// 原函数：0x00533910。
void ClearMiniMailAndProfiles_533910()
{
    for (MiniMailContactRecord& record : gMiniMailRuntime.contacts) {
        record.used = 0;
    }
    std::memset(gMiniMailRuntime.messages.data(), 0, sizeof(gMiniMailRuntime.messages));
    std::memset(gProfiles1074938.data(), 0, sizeof(gProfiles1074938));
    std::memset(&gCurrentProfile10738E8, 0, sizeof(gCurrentProfile10738E8));
}

// 原函数：0x00533950。
void InitializeDefaultProfiles_533950()
{
    ProfileRuntimeRecord& first = gProfiles1074938[0];
    std::strcpy(first.text08.data(), "");
    const char* const source = "";
    const std::size_t bytes = std::strlen(source) + 1;
    first.value108 = -1;
    first.value04 = 0;
    std::memcpy(first.text110.data(), source, bytes);
    std::strcpy(first.text210.data(), "");
    std::strcpy(first.text310.data(), "");
    for (ProfileEntry& entry : first.entries410) {
        entry.value00 = 0;
        entry.value04 = 0x3C0F0;
        cg::text::StoreGbk(entry.text08.data(), entry.text08.size(), kProfileDefault58F614);
    }
    // 原533AB5在最后文本复制完成前取S16角色，两次整条复制共用此值。
    const std::int16_t selected = login::gSelectedCharacterIndex;
    cg::text::StoreGbk(first.text728.data(), first.text728.size(), kProfileDefault58F614);
    std::memcpy(&gProfiles1074938[1], &first, sizeof(first));
    std::memcpy(
        &gCurrentProfile10738E8, &gProfiles1074938[selected],
        sizeof(gCurrentProfile10738E8));
}

// 原函数：0x00533B00。
std::int32_t HasProfileDraftChanges_533B00()
{
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const ProfileRuntimeRecord& saved = gProfiles1074938[selected];
    const std::int32_t firstCurrent = gCurrentProfile10738E8.entries410[0].value00;
    const std::int32_t firstSaved = saved.entries410[0].value00;
    if (firstSaved != firstCurrent) {
        return 1;
    }
    const std::int32_t secondSaved = saved.entries410[1].value00;
    const std::int32_t secondCurrent = gCurrentProfile10738E8.entries410[1].value00;
    if (secondSaved != secondCurrent) {
        return 1;
    }
    const std::int32_t thirdSaved = saved.entries410[2].value00;
    const std::int32_t thirdCurrent = gCurrentProfile10738E8.entries410[2].value00;
    if (thirdSaved != thirdCurrent) {
        return 1;
    }
    for (std::int32_t index = 0; index < 3; ++index) {
        if (std::strcmp(saved.entries410[index].text08.data(),
                gCurrentProfile10738E8.entries410[index].text08.data()) != 0) {
            return 1;
        }
    }
    return std::strcmp(saved.text728.data(), gCurrentProfile10738E8.text728.data()) != 0 ? 1 : 0;
}

// 原函数：0x00533C80。
std::int32_t PublishPlayerProfile_533C80(const std::int32_t mode)
{
    if (mode != 0 && mode != 1) {
        return mode - 1;
    }
    if (mode == 0) {
        for (ProfileEntry& entry : gCurrentProfile10738E8.entries410) {
            if (std::strcmp(entry.text08.data(), "") == 0) {
                cg::text::StoreGbk(entry.text08.data(), entry.text08.size(), kProfileDefault58F614);
            }
        }
        if (std::strcmp(
                gCurrentProfile10738E8.text728.data(), "") == 0) {
            cg::text::StoreGbk(gCurrentProfile10738E8.text728.data(), gCurrentProfile10738E8.text728.size(), kProfileDefault58F614);
        }
    }

    char sourceText[256];
    std::array<std::array<char, 256>, 4> escaped;
    for (std::int32_t index = 0; index < 3; ++index) {
        const std::string hidden = cg::text::ToGbk(L"不公开" /* 原常量参考：0x0058F61C。 */);
        const char* source = hidden.c_str();
        if (gCurrentProfile10738E8.entries410[index].value00 != 0) {
            if (mode == 1) {
                const std::int16_t selected = login::gSelectedCharacterIndex;
                source = gProfiles1074938[selected].entries410[index].text08.data();
            } else {
                source = gCurrentProfile10738E8.entries410[index].text08.data();
            }
        }
        std::strcpy(sourceText, source);
        cg::core::NullSub_509080();
        input::EscapeGbkText(sourceText, escaped[index].data(), 256);
    }
    if (mode == 1) {
        const std::int16_t selected = login::gSelectedCharacterIndex;
        std::strcpy(sourceText, gProfiles1074938[selected].text728.data());
    } else {
        std::strcpy(sourceText, gCurrentProfile10738E8.text728.data());
    }
    cg::core::NullSub_509080();
    input::EscapeGbkText(sourceText, escaped[3].data(), 256);

    if (mode == 1) {
        const std::int16_t selected = login::gSelectedCharacterIndex;
        const std::int32_t third = gProfiles1074938[selected].entries410[2].value00;
        const std::int32_t second = gProfiles1074938[selected].entries410[1].value00;
        const std::int32_t first = gProfiles1074938[selected].entries410[0].value00;
        const std::uintptr_t socket = platform::gConnectionSocket;
        return network::SendPlayerProfile_511640(
            socket, first, cg::text::FromGbkWide(escaped[0].data()), second, cg::text::FromGbkWide(escaped[1].data()),
            third, cg::text::FromGbkWide(escaped[2].data()), cg::text::FromGbkWide(escaped[3].data()));
    }

    const std::int32_t third = gCurrentProfile10738E8.entries410[2].value00;
    const std::int32_t second = gCurrentProfile10738E8.entries410[1].value00;
    const std::int32_t first = gCurrentProfile10738E8.entries410[0].value00;
    const std::uintptr_t socket = platform::gConnectionSocket;
    static_cast<void>(network::SendPlayerProfile_511640(
        socket, first, cg::text::FromGbkWide(escaped[0].data()), second, cg::text::FromGbkWide(escaped[1].data()),
        third, cg::text::FromGbkWide(escaped[2].data()), cg::text::FromGbkWide(escaped[3].data())));

    const std::int16_t selected = login::gSelectedCharacterIndex;
    ProfileRuntimeRecord& target = gProfiles1074938[selected];
    std::strcpy(target.text08.data(), gCurrentProfile10738E8.text08.data());
    target.entries410[0].value00 = gCurrentProfile10738E8.entries410[0].value00;
    target.entries410[0].value04 = gCurrentProfile10738E8.entries410[0].value04;
    std::strcpy(
        target.entries410[0].text08.data(), gCurrentProfile10738E8.entries410[0].text08.data());
    const std::int32_t secondValue = gCurrentProfile10738E8.entries410[1].value04;
    target.entries410[1].value00 = gCurrentProfile10738E8.entries410[1].value00;
    target.entries410[1].value04 = secondValue;
    std::strcpy(
        target.entries410[1].text08.data(), gCurrentProfile10738E8.entries410[1].text08.data());
    const std::int32_t thirdValue = gCurrentProfile10738E8.entries410[2].value04;
    target.entries410[2].value00 = gCurrentProfile10738E8.entries410[2].value00;
    target.entries410[2].value04 = thirdValue;
    std::strcpy(
        target.entries410[2].text08.data(), gCurrentProfile10738E8.entries410[2].text08.data());
    const auto bytes = static_cast<std::int32_t>(
        std::strlen(gCurrentProfile10738E8.text728.data()) + 1);
    std::memcpy(target.text728.data(), gCurrentProfile10738E8.text728.data(), bytes);
    return bytes;
}

// 原函数：0x00534290。
std::int32_t ResendSavedPlayerProfile_534290()
{
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const std::int32_t enabled = gProfiles1074938[selected].value04;
    static_cast<void>(PublishPlayerProfile_533C80(1));
    const std::uintptr_t socket = platform::gConnectionSocket;
    return network::SetProfileVisibilityRequest_5118B0(socket, enabled != 0 ? 1 : 0);
}

// 原函数：0x0050F2A0。
std::int32_t HandlePrimaryProfileCategoriesReply_50F2A0(const std::uintptr_t, const char* const payload)
{
    for (std::int32_t index = 0; index < 100; ++index) {
        gCategory105FA10.records[index].used = 0;
        gCategory105FA10.records[index + 100].used = 0;
    }
    gCategory105FA10.value00 = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    gCategory105FA10.count08 = 0;
    std::int32_t field = 2;
    std::int32_t count = 0;
    for (;;) {
        CategoryRecord& source = gCategory105FA10.records[count];
        source.used = 1;
        source.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        source.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        const std::int32_t status = network::ExtractDelimitedField_52CED0(
            payload, '|', field++, 255, source.text0C.data());
        input::UnescapeLegacyTextAndNotify_52D2F0(source.text0C.data());
        CategoryRecord& target = gCategory105FA10.records[count + 100];
        target.used = 1;
        target.value04 = source.value04;
        target.value08 = source.value08;
        std::strcpy(target.text0C.data(), source.text0C.data());
        ++count;
        gCategory105FA10.count08 = count;
        if (status == 1) {
            return count;
        }
    }
}

// 原函数：0x0050F3A0。
std::int32_t HandleProfileCategoryVersionsReply_50F3A0(const std::uintptr_t socket, const char* const payload)
{
    const std::int32_t first = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    const std::int32_t second = network::ParseDelimitedInteger_52CFA0(payload, '|', 2);
    gCategory105FA10.value10 = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    const std::int32_t key = network::ParseDelimitedInteger_52CFA0(payload, '|', 4);
    const std::int16_t selected = login::gSelectedCharacterIndex;
    gProfiles1074938[selected].value00 = key;
    const std::int32_t previous = gCategory105FA10.value00;
    std::int32_t mask = previous != first ? 1 : 0;
    if (gCategory105FA10.value04 != second) {
        mask |= 2;
    }
    if (mask != 0) {
        return network::RequestProfileCategoryUpdates_5115F0(socket, mask);
    }
    return previous;
}

// 原函数：0x0050F8E0。
std::int32_t HandleSecondaryProfileCategoriesReply_50F8E0(const std::uintptr_t, const char* const payload)
{
    for (std::int32_t index = 200; index < 300; ++index) {
        gCategory105FA10.records[index].used = 0;
    }
    gCategory105FA10.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    gCategory105FA10.count0C = 0;
    std::int32_t field = 2;
    std::int32_t count = 0;
    for (;;) {
        CategoryRecord& record = gCategory105FA10.records[count + 200];
        record.used = 1;
        record.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        record.value08 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        const std::int32_t status = network::ExtractDelimitedField_52CED0(
            payload, '|', field++, 255, record.text0C.data());
        input::UnescapeLegacyTextAndNotify_52D2F0(record.text0C.data());
        ++count;
        gCategory105FA10.count0C = count;
        if (status == 1) {
            return count;
        }
    }
}

// 原函数：0x004C9E40。
void HandleProfileCategorySelectionReply_4C9E40(const std::int32_t, char* const payload)
{
    input::UnescapeLegacyText_52D150(payload);
    std::int32_t field = 1;
    for (std::int32_t index = 0; index < gCategory105FA10.count08; ++index) {
        const auto mask = static_cast<std::uint8_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field++));
        gCategory105FA10.records[index].selected10C = mask & 1u;
        gCategory105FA10.records[index + 100].selected10C = (mask >> 1) & 1u;
    }
    for (std::int32_t index = 0; index < gCategory105FA10.count0C; ++index) {
        const auto mask = static_cast<std::uint8_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field++));
        gCategory105FA10.records[index + 200].selected10C = mask & 1u;
    }
}

// 原函数：0x005342F0。
std::int32_t FindProfileCategoryImage_5342F0(const std::int32_t key, const std::int32_t mode)
{
    if (mode != 0 && mode != 1) {
        return -1;
    }
    const CategoryRecord* record = gCategory105FA10.records.data()
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
void HandleProfileSearchResultsReply_4CA330(const std::int32_t, char* const payload)
{
    for (ProfileSearchRecord& record : gProfileSearch105C398.records) {
        record.used = 0;
    }
    input::UnescapeLegacyText_52D150(payload);
    gProfileSearch105C398.value00 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    gProfileSearch105C398.value04 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 2);
    std::int32_t field = 3;
    for (ProfileSearchRecord& record : gProfileSearch105C398.records) {
        if (network::ParseDelimitedInteger_52CFA0(payload, '|', field) == -1) {
            return;
        }
        record.used = 1;
        record.value104 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        record.value108 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        network::ExtractDelimitedField_52CED0(
            payload, '|', field++, 255, record.text04.data());
        input::UnescapeLegacyTextAndNotify_52D2F0(record.text04.data());
        network::ExtractDelimitedField_52CED0(
            payload, '|', field++, 255, record.text10C.data());
        input::UnescapeLegacyTextAndNotify_52D2F0(record.text10C.data());
    }
}

// 原函数：0x004CAC60。
std::int32_t HandleViewedPlayerProfileReply_4CAC60(const std::int32_t, char* const payload)
{
    input::UnescapeLegacyText_52D150(payload);
    ProfileRuntimeRecord& record = gViewedProfile105ECE0;
    char text[256];
    record.value00 = network::ParseDelimitedInteger_52CFA0(payload, '|', 1);
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(record.text08.data(), text);
    record.value108 = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    network::ExtractDelimitedField_52CED0(payload, '|', 4, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(record.text210.data(), text);
    network::ExtractDelimitedField_52CED0(payload, '|', 5, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(record.text110.data(), text);
    network::ExtractDelimitedField_52CED0(payload, '|', 6, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(record.text310.data(), text);
    record.value10C = network::ParseDelimitedInteger_52CFA0(payload, '|', 7);
    record.entries410[0].value00 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 8);
    network::ExtractDelimitedField_52CED0(payload, '|', 9, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(record.entries410[0].text08.data(), text);
    record.entries410[1].value00 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 10);
    network::ExtractDelimitedField_52CED0(payload, '|', 11, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(record.entries410[1].text08.data(), text);
    record.entries410[2].value00 =
        network::ParseDelimitedInteger_52CFA0(payload, '|', 12);
    network::ExtractDelimitedField_52CED0(payload, '|', 13, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(record.entries410[2].text08.data(), text);
    network::ExtractDelimitedField_52CED0(payload, '|', 14, 255, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(record.text728.data(), text);
    record.entries410[1].value04 = 246000;
    record.entries410[0].value04 = 246000;
    record.entries410[2].value04 = 246000;
    const std::int32_t second = FindProfileCategoryImage_5342F0(record.entries410[1].value00, 0);
    if (second != -1) {
        record.entries410[1].value04 = second;
    }
    const std::int32_t first = FindProfileCategoryImage_5342F0(record.entries410[0].value00, 0);
    if (first != -1) {
        record.entries410[0].value04 = first;
    }
    const std::int32_t third = FindProfileCategoryImage_5342F0(record.entries410[2].value00, 1);
    if (third != -1) {
        record.entries410[2].value04 = third;
    }
    return third;
}

// 原函数：0x0050F450。
void HandlePlayerProfileReply_50F450(
    const std::uintptr_t, const char* const text1, const std::int32_t value2,
    const char* const text3, const char* const text4, const char* const text5,
    const std::int32_t value6, const std::int32_t value7, const char* const text8,
    const std::int32_t value9, const char* const text10, const std::int32_t value11,
    const char* const text12, const char* const text13)
{
    ProfileRuntimeRecord& record = gReceivedProfile1074110;
    std::array<std::array<char, 256>, 8> buffers;
    record.value10C = value6;
    std::strcpy(buffers[4].data(), text1);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[4].data());
    std::strcpy(record.text08.data(), buffers[4].data());
    record.value108 = value2;
    std::strcpy(buffers[2].data(), text3);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[2].data());
    std::strcpy(record.text210.data(), buffers[2].data());
    std::strcpy(buffers[6].data(), text4);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[6].data());
    std::strcpy(record.text110.data(), buffers[6].data());
    std::strcpy(buffers[0].data(), text5);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[0].data());
    std::strcpy(record.text310.data(), buffers[0].data());
    record.entries410[1].value00 = value9;
    std::strcpy(buffers[1].data(), text10);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[1].data());
    std::strcpy(record.entries410[1].text08.data(), buffers[1].data());
    record.entries410[0].value00 = value7;
    std::strcpy(buffers[3].data(), text8);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[3].data());
    std::strcpy(record.entries410[0].text08.data(), buffers[3].data());
    record.entries410[2].value00 = value11;
    std::strcpy(buffers[5].data(), text12);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[5].data());
    std::strcpy(record.entries410[2].text08.data(), buffers[5].data());
    std::strcpy(buffers[7].data(), text13);
    input::UnescapeLegacyTextAndNotify_52D2F0(buffers[7].data());
    std::strcpy(record.text728.data(), buffers[7].data());
    record.entries410[1].value04 = 246000;
    record.entries410[0].value04 = 246000;
    record.entries410[2].value04 = 246000;
    const std::int32_t second = FindProfileCategoryImage_5342F0(record.entries410[1].value00, 0);
    if (second != -1) {
        record.entries410[1].value04 = second;
    }
    const std::int32_t first = FindProfileCategoryImage_5342F0(record.entries410[0].value00, 0);
    if (first != -1) {
        record.entries410[0].value04 = first;
    }
    const std::int32_t third = FindProfileCategoryImage_5342F0(record.entries410[2].value00, 1);
    if (third != -1) {
        record.entries410[2].value04 = third;
    }
    static_cast<void>(ui::WindowProfileReply_Open());
}

// 原函数：0x005348A0。
std::int32_t SaveProfileCategoryFile_5348A0()
{
    char path[1024];
    std::sprintf(
        path, "bin\\%s", "category.dat");
    std::FILE* const stream =
        std::fopen(path, "wb+");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(static_cast<std::uint32_t>(std::fwrite(
        &gCategory105FA10, static_cast<std::uint32_t>(sizeof(gCategory105FA10)), 1, stream)));
    static_cast<void>(std::fclose(stream));
    return 1;
}

// 原函数：0x00534910。
std::int32_t LoadProfileCategoryFile_534910()
{
    char path[1024];
    std::sprintf(
        path, "bin\\%s", "category.dat");
    std::FILE* const stream =
        std::fopen(path, "rb+");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(static_cast<std::uint32_t>(std::fread(
        &gCategory105FA10, static_cast<std::uint32_t>(sizeof(gCategory105FA10)), 1, stream)));
    static_cast<void>(std::fclose(stream));
    return 1;
}

// 原函数：0x005345A0。
std::int32_t ResetCharacterProfile_5345A0(const std::int32_t index)
{
    ProfileRuntimeRecord& record = gProfiles1074938[index];
    std::strcpy(record.text08.data(), "");
    const char* const source = "";
    const std::size_t bytes = std::strlen(source) + 1;
    record.value108 = -1;
    record.value04 = 0;
    std::memcpy(record.text110.data(), source, bytes);
    std::strcpy(record.text210.data(), "");
    std::strcpy(record.text310.data(), "");
    for (ProfileEntry& entry : record.entries410) {
        entry.value00 = 0;
        entry.value04 = 246000;
        cg::text::StoreGbk(entry.text08.data(), entry.text08.size(), kProfileDefault58F614);
    }
    // 原534737在末条文本的movsd与movsb之间读取当前角色，不沿用重置实参。
    const std::string defaultGbk = cg::text::ToGbk(kProfileDefault58F614);
    std::memcpy(record.text728.data(), defaultGbk.data(), 4);
    const std::int16_t selected = login::gSelectedCharacterIndex;
    std::memcpy(
        record.text728.data() + 4, defaultGbk.c_str() + 4,
        defaultGbk.size() + 1 - 4);
    std::memcpy(
        &gCurrentProfile10738E8, &gProfiles1074938[selected],
        sizeof(gCurrentProfile10738E8));
    return selected;
}

// 原函数：0x00534770。
std::int32_t SavePlayerProfilesFile_534770()
{
    char path[1024];
    std::sprintf(
        path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "profile.dat");
    std::FILE* const stream =
        std::fopen(path, "wb+");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(static_cast<std::uint32_t>(std::fwrite(
        gProfiles1074938.data(), static_cast<std::uint32_t>(sizeof(gProfiles1074938)), 1, stream)));
    static_cast<void>(std::fclose(stream));
    return 1;
}

// 原函数：0x005347F0。
std::int32_t LoadPlayerProfilesFile_5347F0()
{
    char path[1024];
    std::sprintf(
        path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "profile.dat");
    std::FILE* const stream =
        std::fopen(path, "rb+");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(static_cast<std::uint32_t>(std::fread(
        gProfiles1074938.data(), static_cast<std::uint32_t>(sizeof(gProfiles1074938)), 1, stream)));
    static_cast<void>(std::fclose(stream));
    const std::int16_t selected = login::gSelectedCharacterIndex;
    std::memcpy(
        &gCurrentProfile10738E8, &gProfiles1074938[selected],
        sizeof(gCurrentProfile10738E8));
    return 1;
}

// 原函数：0x00518CB0。
std::int32_t LoadContactAndGuildMailFiles_518CB0()
{
    char path[128];
    std::sprintf(
        path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "mail.dat");
    std::FILE* stream =
        std::fopen(path, "rb");
    if (stream == nullptr) {
        return SaveContactMailFile_519300();
    }
    static_cast<void>(static_cast<std::uint32_t>(std::fread(
        gMailStorageF7CA08.data(), 1, sizeof(gMailStorageF7CA08), stream)));
    static_cast<void>(std::fclose(stream));

    std::sprintf(
        path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "guildmail.dat");
    stream = std::fopen(path, "rb");
    if (stream == nullptr) {
        return SaveGuildMailFile_519550();
    }
    static_cast<void>(static_cast<std::uint32_t>(std::fread(
        gGuildMailStorageDE0B30.data(), 1,
        sizeof(gGuildMailStorageDE0B30), stream)));
    return std::fclose(stream);
}

// 原函数：0x00519300。
std::int32_t SaveContactMailFile_519300()
{
    char path[128];
    std::sprintf(
        path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "mail.dat");
    std::FILE* const stream =
        std::fopen(path, "wb");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(static_cast<std::uint32_t>(std::fwrite(
        gMailStorageF7CA08.data(), 1, sizeof(gMailStorageF7CA08), stream)));
    return std::fclose(stream);
}

// GMR01：原128B路径/完整两角色139480写/写弃返→关闭S32结果，当前MSVC FILE域保。
// 原函数：0x00519550。
std::int32_t SaveGuildMailFile_519550()
{
    char path[128];
    std::sprintf(
        path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "guildmail.dat");
    std::FILE* const stream =
        std::fopen(path, "wb");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(static_cast<std::uint32_t>(std::fwrite(
        gGuildMailStorageDE0B30.data(), 1,
        sizeof(gGuildMailStorageDE0B30), stream)));
    return std::fclose(stream);
}

// 原函数：0x00519290。
std::int32_t ClearCurrentCharacterContactMail_519290()
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    std::memset(gMailStorageF7CA08.data() + selected * 60 * 10, 0, kMailGroupBytes);
    return 0;
}

// 原函数：0x005194C0。
std::int32_t ClearCurrentCharacterGuildMail_5194C0()
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    std::memset(
        gGuildMailStorageDE0B30.data() + selected * 200,
        0, kGuildMailGroupBytes);
    return 0;
}

// 原函数：0x005192C0。
std::int32_t ClearContactMailMessages_5192C0(const std::int32_t number)
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    std::memset(
        gMailStorageF7CA08.data() + selected * 60 * 10
            + static_cast<std::ptrdiff_t>(number) * 10,
        0, kMailNumberBytes);
    return 0;
}

// GML01：原selected S16/200邮件槽W32==1最后匹配，无则-1。
// 原函数：0x005195F0。
std::int32_t FindLastActiveGuildMailEntry_5195F0(const std::int32_t key)
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    const GuildMailEntry* entry =
        gGuildMailStorageDE0B30.data() + selected * 200;
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
std::int32_t DeleteGuildMailEntry_5194F0(const std::int32_t key)
{
    const std::int32_t index = FindLastActiveGuildMailEntry_5195F0(key);
    if (index == -1) {
        return index;
    }
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    GuildMailEntry* const entry = gGuildMailStorageDE0B30.data()
        + selected * 200 + static_cast<std::ptrdiff_t>(index);
    std::memset(entry, 0, kGuildMailEntryBytes);
    return 0;
}

// GML01：原查找门槛/分支fresh角色、空槽或九次倒序移位，kind两BYTE及name/text发布序保持。
// 原函数：0x00519370。
std::uintptr_t PrependGuildMailMessage_519370(
    const std::int32_t key, const std::int32_t kind,
    const char* const name, const char* const text)
{
    if (cg::world::FindActiveOrderedRecordIndex_5195C0(key) == -1) {
        return 0xFFFFFFFFu;
    }
    const std::int32_t existingIndex = FindLastActiveGuildMailEntry_5195F0(key);
    GuildMailEntry* entry;
    if (existingIndex == -1) {
        const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
        const std::ptrdiff_t groupOffset = selected * kGuildMailGroupBytes;
        std::int32_t index = 0;
        for (; index < 200; ++index) {
            entry = gGuildMailStorageDE0B30.data() + selected * 200 + index;
            const std::uint32_t active = entry->active;
            if (active == 0) {
                break;
            }
        }
        if (index == 200) {
            // 原 EAX 是满组末地址再加 4；保持原生宽度，不构造越界指针。
            return reinterpret_cast<std::uintptr_t>(gGuildMailStorageDE0B30.data())
                + static_cast<std::uintptr_t>(groupOffset + kGuildMailGroupBytes + 4);
        }
        const std::uint32_t active = 1;
        entry->key = key;
        entry->active = active;
    } else {
        const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
        entry = gGuildMailStorageDE0B30.data() + selected * 200 + existingIndex;
        for (std::ptrdiff_t message = 9; message != 0; --message) {
            std::memcpy(
                &entry->messages[message],
                &entry->messages[message - 1],
                kMailMessageBytes);
        }
    }
    entry->messages[0].unread = static_cast<std::byte>(kind == 0);
    entry->messages[0].kind = static_cast<std::byte>(static_cast<std::uint8_t>(kind));
    const std::uint32_t nameBytes = static_cast<std::uint32_t>(std::strlen(name) + 1);
    std::memcpy(entry->messages[0].name.data(), name, nameBytes);
    const std::uint32_t textBytes = static_cast<std::uint32_t>(std::strlen(text) + 1);
    std::memcpy(entry->messages[0].text.data(), text, textBytes);
    return textBytes;
}

std::uintptr_t PruneGuildMailAgainstOrderedRecords_50EE4A(const std::int16_t selected)
{
    GuildMailEntry* entry = gGuildMailStorageDE0B30.data()
        + static_cast<std::ptrdiff_t>(selected) * 200;
    const std::uintptr_t keyEnd =
        reinterpret_cast<std::uintptr_t>(cg::world::gOrderedRecords1051B14.data())
        + sizeof(cg::world::gOrderedRecords1051B14)
        + offsetof(cg::world::OrderedRecord1051B14, value0C);
    std::uintptr_t result = 0;
    for (std::int32_t index = 0; index < 200; ++index) {
        const std::int32_t key = entry->key;
        bool found = false;
        for (const cg::world::OrderedRecord1051B14& record :
             cg::world::gOrderedRecords1051B14) {
            if (key == record.value0C) {
                found = true;
            }
        }
        result = keyEnd;
        if (!found) {
            result = 0;
            std::memset(entry, 0, kGuildMailEntryBytes);
        }
        ++entry;
    }
    return result;
}

} // namespace cg::world::detail
