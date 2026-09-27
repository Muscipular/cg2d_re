#include "MailState.hpp"

#include "../LoginInputDependencies.hpp"

#include "cg/LegacyRecordOrder.hpp"
#include "cg/LoginRequest.hpp"

#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>

namespace cg::world::detail {
namespace {

// 文件与运行记录共用这些原字节区；布局与未接消费者见场景证据 §25–26、57。
alignas(std::uint32_t) std::array<std::byte, 0x5DC00> gMailStorageF7CA08 {};
alignas(std::uint32_t) std::array<std::byte, 0x139480> gGuildMailStorageDE0B30 {};
std::uint32_t gValueCB1384 = 0;

constexpr std::ptrdiff_t kMailGroupBytes = 0x2EE00;
constexpr std::ptrdiff_t kMailNumberBytes = 0xC80;
constexpr std::ptrdiff_t kGuildMailGroupBytes = 0x9CA40;
constexpr std::ptrdiff_t kGuildMailEntryBytes = 0xC88;
constexpr std::ptrdiff_t kMailMessageBytes = 0x140;

static_assert(sizeof(gMailStorageF7CA08) == 2 * 60 * 10 * 0x140);
static_assert(sizeof(gGuildMailStorageDE0B30) == 2 * 200 * 0xC88);

} // namespace

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

} // namespace cg::world::detail
