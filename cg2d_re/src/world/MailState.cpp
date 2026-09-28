#include "MailState.hpp"
#include "WorldUpdateDependencies.hpp"

#include "../LoginInputDependencies.hpp"

#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/LegacyRecordOrder.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/world/SceneState.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstring>

namespace cg::world::detail {

std::array<ContactRuntimeRecord, 60> gContactsF19FB0 {};
std::array<std::int32_t, 60> gContactIndicesFDA608 {};
std::uint16_t gValue62B354 = 0;
std::uint32_t gValueCB1384 = 0;
std::uint32_t gValueCB0354 = 0;
std::int32_t gValueCF4FC0 = 0;
std::int32_t gValueD046D4 = 0;
static_assert(sizeof(gContactsF19FB0) == 0x1B30);
static_assert(sizeof(gContactIndicesFDA608) == 0xF0);

namespace {

// 文件与运行记录共用这些原字节区；布局与未接消费者见场景证据 §25–26、57。
alignas(std::uint32_t) std::array<std::byte, 0x5DC00> gMailStorageF7CA08 {};
alignas(std::uint32_t) std::array<std::byte, 0x139480> gGuildMailStorageDE0B30 {};

// 原1075A90起的20条74H记录，当前只消费used；不凭相同步长猜其余字段。
struct MiniMailContactRecord {
    std::uint16_t used;
    std::array<std::byte, 0x72> bytes02;
};

struct alignas(std::uint32_t) MiniMailRuntimeSlots {
    std::array<MiniMailContactRecord, 20> contacts;
    std::array<std::byte, 0xFA00> messages; // 原10763A0，20组各十条140H。
};

static_assert(sizeof(MiniMailContactRecord) == 0x74);
static_assert(std::is_standard_layout_v<MiniMailRuntimeSlots>);
static_assert(std::is_trivially_copyable_v<MiniMailRuntimeSlots>);
static_assert(offsetof(MiniMailContactRecord, used) == 0);
static_assert(offsetof(MiniMailContactRecord, bytes02) == 2);
static_assert(offsetof(MiniMailRuntimeSlots, contacts) == 0);
static_assert(offsetof(MiniMailRuntimeSlots, messages) == 0x910);
static_assert(sizeof(MiniMailRuntimeSlots) == 0x10310);
MiniMailRuntimeSlots gMiniMailRuntime {};

constexpr char kReceivedMailFormat58B07C[] =
    "\xA6\xAC\xA8\xEC\xB1\x71" "%s" "\xB1\x48\xA8\xD3\xAA\xBA"
    "\xB6\x6C\xA5\xF3\xA1\x49";
constexpr char kReceivedPetMailFormat58B064[] =
    "\xA5\xD1" "%s" "\xB0\x65\xA8\xD3\xA4\x46\xC3\x64\xAA\xAB"
    "\xB6\x6C\xA5\xF3\xA1\x49";
static_assert(sizeof(kReceivedMailFormat58B07C) == 21);
static_assert(sizeof(kReceivedPetMailFormat58B064) == 21);

constexpr std::ptrdiff_t kMailGroupBytes = 0x2EE00;
constexpr std::ptrdiff_t kMailNumberBytes = 0xC80;
constexpr std::ptrdiff_t kGuildMailGroupBytes = 0x9CA40;
constexpr std::ptrdiff_t kGuildMailEntryBytes = 0xC88;
constexpr std::ptrdiff_t kMailMessageBytes = 0x140;

static_assert(sizeof(gMailStorageF7CA08) == 2 * 60 * 10 * 0x140);
static_assert(sizeof(gGuildMailStorageDE0B30) == 2 * 200 * 0xC88);

std::int32_t CompareContactIndices_4EC030(
    const std::int32_t* const left, const std::int32_t* const right)
{
    const ContactRuntimeRecord& other = gContactsF19FB0[*right];
    if (other.used == 0) {
        return -1;
    }
    const ContactRuntimeRecord& current = gContactsF19FB0[*left];
    if (current.used == 0) {
        return 1;
    }
    const std::uint32_t mode = platform::gValueD67CD0;
    if (mode == 0) {
        return current.value04 > other.value04 ? 1 : -1;
    }
    if (mode == 1) {
        const int order = std::strcmp(current.name10.data(), other.name10.data());
        if (order > 0) {
            return 1;
        }
        return order == 0 && current.value04 > other.value04 ? 1 : -1;
    }
    if (current.value02 == 0) {
        if (other.value02 > 0) {
            return 1;
        }
        return other.value02 == 0 && current.value04 > other.value04 ? 1 : -1;
    }
    return current.value02 > 0 && other.value02 > 0
        && current.value04 > other.value04 ? 1 : -1;
}

} // namespace

void Sub_4EC1A0()
{
    for (std::int32_t index = 0; index < 60; ++index) {
        gContactIndicesFDA608[index] = index;
    }
    // 沿已证53C112/53C266的4B路径，保持非对称比较器的比较/交换次序。
    std::int32_t* first = gContactIndicesFDA608.data();
    std::int32_t* last = first + 59;
    std::array<std::int32_t*, 30> firstStack;
    std::array<std::int32_t*, 30> lastStack;
    std::int32_t stackSize = 0;
    for (;;) {
        const auto length = last - first + 1;
        if (length <= 8) {
            while (last > first) {
                std::int32_t* maximum = first;
                for (std::int32_t* item = first + 1; item <= last; ++item) {
                    if (CompareContactIndices_4EC030(item, maximum) > 0) {
                        maximum = item;
                    }
                }
                std::swap(*maximum, *last);
                --last;
            }
        } else {
            std::swap(*(first + length / 2), *first);
            std::int32_t* left = first;
            std::int32_t* right = last + 1;
            for (;;) {
                do {
                    ++left;
                } while (left <= last && CompareContactIndices_4EC030(left, first) <= 0);
                do {
                    --right;
                } while (right > first && CompareContactIndices_4EC030(right, first) >= 0);
                if (right < left) {
                    break;
                }
                std::swap(*left, *right);
            }
            std::swap(*first, *right);
            if (right - first > last - left) {
                if (first + 1 < right) {
                    firstStack[stackSize] = first;
                    lastStack[stackSize] = right - 1;
                    ++stackSize;
                }
                if (left < last) {
                    first = left;
                    continue;
                }
            } else {
                if (left < last) {
                    firstStack[stackSize] = left;
                    lastStack[stackSize] = last;
                    ++stackSize;
                }
                if (first + 1 < right) {
                    last = right - 1;
                    continue;
                }
            }
        }
        if (--stackSize < 0) {
            return;
        }
        first = firstStack[stackSize];
        last = lastStack[stackSize];
    }
}

void Sub_50D740(const std::uintptr_t, const char* const payload)
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

void Sub_50D8C0(const std::uintptr_t, const char* const payload)
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
    Sub_4EC1A0();
}

void Sub_50D9E0(
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
        static_cast<void>(Sub_5192C0(index));
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
    Sub_4EC1A0();
}

void Sub_5191C0(
    const std::int32_t index, const std::int32_t kind,
    const char* const name, const char* const text)
{
    if (gContactsF19FB0[index].used == 0) {
        return;
    }
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    std::byte* const entry = gMailStorageF7CA08.data()
        + selected * kMailGroupBytes + static_cast<std::ptrdiff_t>(index) * kMailNumberBytes;
    for (std::ptrdiff_t message = 9; message > 0; --message) {
        std::memcpy(
            entry + message * kMailMessageBytes,
            entry + (message - 1) * kMailMessageBytes, kMailMessageBytes);
    }
    entry[0] = kind == 0 ? std::byte{1} : std::byte{0};
    entry[1] = static_cast<std::byte>(static_cast<std::uint8_t>(kind));
    std::memcpy(entry + 2, name, std::strlen(name) + 1);
    std::memcpy(entry + 0x14, text, std::strlen(text) + 1);
}

std::int32_t Sub_518DA0()
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    for (std::int32_t position = 0; position < 60; ++position) {
        const std::int32_t index = gContactIndicesFDA608[position];
        if (gContactsF19FB0[index].used == 0) {
            continue;
        }
        const std::byte* const entry = gMailStorageF7CA08.data()
            + selected * kMailGroupBytes
            + static_cast<std::ptrdiff_t>(index) * kMailNumberBytes;
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry[message * kMailMessageBytes] != std::byte{0}) {
                gValue62B354 = 1;
                gValueCB1384 = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    for (std::int32_t position = 0; position < 200; ++position) {
        const auto& record = login::detail::gOrderedRecords1051B14[position];
        if (record.value08 == 0) {
            continue;
        }
        const std::int32_t index = Sub_5195F0(record.value0C);
        if (index == -1) {
            continue;
        }
        const std::ptrdiff_t currentSelected = login::gSelectedCharacterIndex;
        const std::byte* const entry = gGuildMailStorageDE0B30.data()
            + currentSelected * kGuildMailGroupBytes
            + static_cast<std::ptrdiff_t>(index) * kGuildMailEntryBytes;
        std::uint32_t active;
        std::memcpy(&active, entry + 4, sizeof(active));
        if (active == 0) {
            continue;
        }
        for (std::ptrdiff_t message = 0; message < 10; ++message) {
            if (entry[8 + message * kMailMessageBytes] != std::byte{0}) {
                gValue62B354 = 2;
                gValueCB1384 = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        if (gMiniMailRuntime.contacts[position].used == 0) {
            continue;
        }
        const std::byte* const entry = gMiniMailRuntime.messages.data()
            + static_cast<std::ptrdiff_t>(position) * kMailNumberBytes;
        for (std::ptrdiff_t message = 9; message >= 0; --message) {
            if (entry[message * kMailMessageBytes] != std::byte{0}) {
                gValue62B354 = 3;
                gValueCB1384 = static_cast<std::uint32_t>(position / 4);
                return 1;
            }
        }
    }
    const std::uint32_t fallbackPage = gValueCB0354;
    gValue62B354 = 0;
    gValueCB1384 = fallbackPage;
    return 0;
}

void Sub_50DC70(
    const std::uintptr_t, const std::int32_t index,
    const char* const payload, const std::int32_t)
{
    char name[32];
    char text[300];
    char message[256];
    network::ExtractDelimitedField_52CED0(payload, '|', 1, 31, name);
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 299, text);
    const std::int32_t kind = network::ParseDelimitedInteger_52CFA0(payload, '|', 3);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    Sub_5191C0(index, 0, name, text);
    const char* const contactName = gContactsF19FB0[index].name10.data();
    std::sprintf(
        message, kind == -1 ? kReceivedMailFormat58B07C : kReceivedPetMailFormat58B064,
        contactName);
    static_cast<void>(Sub_519300());
    static_cast<void>(Sub_518DA0());
    if (gValueCF4FC0 == index) {
        const std::int32_t position = gValueD046D4;
        if (position < 9) {
            gValueD046D4 = position + 1;
        }
    }
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    Sub_44B050(message, 4, 3);
}

std::int32_t Sub_518C80()
{
    gMailStorageF7CA08.fill(std::byte{0});
    gGuildMailStorageDE0B30.fill(std::byte{0});
    gValueCB1384 = 0;
    return 0;
}

std::int32_t Sub_518CB0()
{
    char path[128];
    std::sprintf(path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "mail.dat");
    std::FILE* stream = std::fopen(path, "rb");
    if (stream == nullptr) {
        return Sub_519300();
    }
    static_cast<void>(std::fread(
        gMailStorageF7CA08.data(), 1, gMailStorageF7CA08.size(), stream));
    static_cast<void>(std::fclose(stream));

    std::sprintf(path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "guildmail.dat");
    stream = std::fopen(path, "rb");
    if (stream == nullptr) {
        return Sub_519550();
    }
    static_cast<void>(std::fread(
        gGuildMailStorageDE0B30.data(), 1, gGuildMailStorageDE0B30.size(), stream));
    return std::fclose(stream);
}

std::int32_t Sub_519300()
{
    char path[128];
    std::sprintf(path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "mail.dat");
    std::FILE* const stream = std::fopen(path, "wb");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(std::fwrite(
        gMailStorageF7CA08.data(), 1, gMailStorageF7CA08.size(), stream));
    return std::fclose(stream);
}

std::int32_t Sub_519550()
{
    char path[128];
    std::sprintf(path, "%s\\%s", login::detail::ReadSelectedSavedDirectory(), "guildmail.dat");
    std::FILE* const stream = std::fopen(path, "wb");
    if (stream == nullptr) {
        return 0;
    }
    static_cast<void>(std::fwrite(
        gGuildMailStorageDE0B30.data(), 1, gGuildMailStorageDE0B30.size(), stream));
    return std::fclose(stream);
}

std::int32_t Sub_519290()
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    std::memset(gMailStorageF7CA08.data() + selected * kMailGroupBytes, 0, kMailGroupBytes);
    return 0;
}

std::int32_t Sub_5194C0()
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    std::memset(
        gGuildMailStorageDE0B30.data() + selected * kGuildMailGroupBytes,
        0, kGuildMailGroupBytes);
    return 0;
}

std::int32_t Sub_5192C0(const std::int32_t number)
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    std::memset(
        gMailStorageF7CA08.data() + selected * kMailGroupBytes
            + static_cast<std::ptrdiff_t>(number) * kMailNumberBytes,
        0, kMailNumberBytes);
    return 0;
}

std::int32_t Sub_5195F0(const std::int32_t key)
{
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    const std::byte* entry =
        gGuildMailStorageDE0B30.data() + selected * kGuildMailGroupBytes;
    std::int32_t result = -1;
    for (std::int32_t index = 0; index < 200; ++index) {
        std::uint32_t active;
        std::memcpy(&active, entry + 4, sizeof(active));
        if (active == 1) {
            std::int32_t entryKey;
            std::memcpy(&entryKey, entry, sizeof(entryKey));
            if (entryKey == key) {
                result = index;
            }
        }
        entry += kGuildMailEntryBytes;
    }
    return result;
}

std::int32_t Sub_5194F0(const std::int32_t key)
{
    const std::int32_t index = Sub_5195F0(key);
    if (index == -1) {
        return index;
    }
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    std::byte* const entry = gGuildMailStorageDE0B30.data()
        + selected * kGuildMailGroupBytes
        + static_cast<std::ptrdiff_t>(index) * kGuildMailEntryBytes;
    const std::uint32_t zero = 0;
    std::memcpy(entry + 4, &zero, sizeof(zero));
    std::memcpy(entry, &zero, sizeof(zero));
    std::memset(entry, 0, kGuildMailEntryBytes);
    return 0;
}

std::uintptr_t Sub_519370(
    const std::int32_t key, const std::int32_t kind,
    const char* const name, const char* const text)
{
    if (login::detail::FindActiveOrderedRecordIndex_5195C0(key) == -1) {
        return 0xFFFFFFFFu;
    }
    const std::int32_t existingIndex = Sub_5195F0(key);
    const std::ptrdiff_t selected = login::gSelectedCharacterIndex;
    const std::ptrdiff_t groupOffset = selected * kGuildMailGroupBytes;
    std::byte* entry;
    if (existingIndex == -1) {
        std::int32_t index = 0;
        for (; index < 200; ++index) {
            entry = gGuildMailStorageDE0B30.data() + groupOffset
                + static_cast<std::ptrdiff_t>(index) * kGuildMailEntryBytes;
            std::uint32_t active;
            std::memcpy(&active, entry + 4, sizeof(active));
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
        std::memcpy(entry, &key, sizeof(key));
        std::memcpy(entry + 4, &active, sizeof(active));
    } else {
        entry = gGuildMailStorageDE0B30.data() + groupOffset
            + static_cast<std::ptrdiff_t>(existingIndex) * kGuildMailEntryBytes;
        for (std::ptrdiff_t message = 9; message != 0; --message) {
            std::memcpy(
                entry + 8 + message * kMailMessageBytes,
                entry + 8 + (message - 1) * kMailMessageBytes,
                kMailMessageBytes);
        }
    }
    entry[8] = static_cast<std::byte>(kind == 0);
    entry[9] = static_cast<std::byte>(static_cast<std::uint8_t>(kind));
    const std::uint32_t nameBytes = static_cast<std::uint32_t>(std::strlen(name) + 1);
    std::memcpy(entry + 0x0A, name, nameBytes);
    const std::uint32_t textBytes = static_cast<std::uint32_t>(std::strlen(text) + 1);
    std::memcpy(entry + 0x1C, text, textBytes);
    return textBytes;
}

std::uintptr_t PruneGuildMailAgainstOrderedRecords_50EE4A(const std::int16_t selected)
{
    std::byte* entry = gGuildMailStorageDE0B30.data()
        + static_cast<std::ptrdiff_t>(selected) * kGuildMailGroupBytes;
    const std::uintptr_t keyEnd =
        reinterpret_cast<std::uintptr_t>(login::detail::gOrderedRecords1051B14.data())
        + sizeof(login::detail::gOrderedRecords1051B14)
        + offsetof(login::detail::OrderedRecord1051B14, value0C);
    std::uintptr_t result = 0;
    for (std::int32_t index = 0; index < 200; ++index) {
        std::int32_t key;
        std::memcpy(&key, entry, sizeof(key));
        bool found = false;
        for (const login::detail::OrderedRecord1051B14& record :
             login::detail::gOrderedRecords1051B14) {
            if (key == record.value0C) {
                found = true;
            }
        }
        result = keyEnd;
        if (!found) {
            const std::uint32_t zero = 0;
            std::memcpy(entry + 4, &zero, sizeof(zero));
            std::memcpy(entry, &zero, sizeof(zero));
            result = 0;
            std::memset(entry + 8, 0, kMailNumberBytes);
        }
        entry += kGuildMailEntryBytes;
    }
    return result;
}

} // namespace cg::world::detail
