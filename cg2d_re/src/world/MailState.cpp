#include "MailState.hpp"

#include "../LoginInputDependencies.hpp"

#include <array>
#include <cstddef>
#include <cstdio>

namespace cg::world::detail {
namespace {

// 文件与运行记录共用这些原字节区；布局与未接消费者见场景证据 §25–26、57。
alignas(std::uint32_t) std::array<std::byte, 0x5DC00> gMailStorageF7CA08 {};
alignas(std::uint32_t) std::array<std::byte, 0x139480> gGuildMailStorageDE0B30 {};
std::uint32_t gValueCB1384 = 0;

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

} // namespace cg::world::detail
