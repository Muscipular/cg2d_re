#include "WorldObjectState.hpp"

#include "WorldUpdateDependencies.hpp"
#include "cg/Input.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/world/WorldObjects.hpp"

#include <cstring>

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

std::uint32_t Sub_4461E0(const std::int32_t type)
{
    switch (type) {
    case 1:
    case 32:
        return 0x100u;
    case 3:
        return 0x200u;
    default:
        return 0x1000u;
    }
}

std::uint32_t Sub_446210(const std::int32_t type)
{
    switch (type) {
    case 1:
        return 8u;
    case 6:
    case 13:
    case 27:
        return 0x11u;
    case 32:
        return 0x21u;
    case 71:
        return 0x101u;
    default:
        return 1u;
    }
}

std::int32_t Sub_445A60(
    const std::int32_t objectIndex, const std::int32_t imageNo,
    const std::int32_t mapX, const std::int32_t mapY,
    const std::int32_t direction, const char* const name,
    const char* const titles, const std::int32_t level,
    const std::uint16_t value40, const std::int32_t flag1,
    const std::int32_t flag2, const std::int32_t type)
{
    if (Sub_444FA0(objectIndex) >= 0) {
        return 0;
    }
    const std::int32_t index = Sub_446130();
    if (index < 0) {
        return 0;
    }
    auto& record = gWorldObjects[static_cast<std::size_t>(index)];
    record.action = nullptr;
    record.used = 2;
    record.category = static_cast<std::uint16_t>(Sub_446210(type));
    record.objectIndex = objectIndex;
    record.imageNo = static_cast<std::uint32_t>(imageNo);
    record.value40 = value40;
    record.level2C = level;
    platform::LegacyCryptInt temporary;
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, mapX);
    record.coordinate0C.readCount = temporary.readCount;
    record.coordinate0C.encoded = temporary.encoded;
    record.coordinate0C.mask = temporary.mask;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    platform::legacy_crypt_int::ConstructFromInt_401CC0(&temporary, mapY);
    record.coordinate1C.mask = temporary.mask;
    record.coordinate1C.encoded = temporary.encoded;
    record.coordinate1C.readCount = temporary.readCount;
    platform::legacy_crypt_int::Destroy_401CF0(&temporary);
    record.direction = direction;
    // 原两处只写+42低W8；+43高字节与其他清理入口仍共享同一W16。
    if (flag1 != 0) {
        *reinterpret_cast<unsigned char*>(&record.flags42) |= 0x40u;
    }
    if (flag2 != 0) {
        *reinterpret_cast<unsigned char*>(&record.flags42) |= 0x80u;
    }
    if (std::strlen(name) <= 16) {
        std::memmove(record.name54.data(), name, std::strlen(name) + 1);
    }
    char first[256];
    char second[256];
    char third[256];
    network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, first);
    if (std::strlen(first) <= 16) {
        std::memmove(record.title65.data(), first, std::strlen(first) + 1);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, second);
    input::UnescapeLegacyTextAndNotify_52D2F0(second);
    if (std::strlen(second) <= 32) {
        std::memmove(record.text76.data(), second, std::strlen(second) + 1);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, third);
    input::UnescapeLegacyTextAndNotify_52D2F0(third);
    if (std::strlen(third) <= 32) {
        std::memmove(record.text97.data(), third, std::strlen(third) + 1);
    }
    record.flags114 = Sub_4461E0(type);
    return 1;
}

void Sub_4462E0(const std::int32_t objectIndex)
{
    if (ReadPlayerObjectIndex_FDA880() == static_cast<std::uint32_t>(objectIndex)) {
        static_cast<void>(Sub_519D50());
        return;
    }
    const std::int32_t index = Sub_444FA0(objectIndex);
    if (index < 0) {
        return;
    }
    auto& record = gWorldObjects[static_cast<std::size_t>(index)];
    login::YobiAction* const action = record.action;
    if (action != nullptr) {
        login::detail::ClearMapActorAttachments(action);
        login::detail::MarkDeath(record.action);
        record.action = nullptr;
    }
    const std::int32_t used = gWorldObjectUsedCount;
    record.used = 0;
    record.flags42 = 0;
    record.flags120 = 0;
    record.value34 = -1;
    if (used > 0) {
        gWorldObjectUsedCount = used - 1;
    }
    if (gWorldObjectFreeCursor > index) {
        gWorldObjectFreeCursor = index;
    }
    for (std::int32_t end = gWorldObjectUpperBound; end > 0; --end) {
        if (gWorldObjects[static_cast<std::size_t>(end - 1)].used != 0) {
            gWorldObjectUpperBound = end;
            return;
        }
    }
    // 原44645A在全空时直接返回，保留旧上界。
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
        record.name54[0] = '\0';
    }
    gWorldObjectFindCursor = 0;
}

void __cdecl Sub_4464D0()
{
    gWorldObjectUsedCount = 0;
    gWorldObjectUpperBound = 0;
    gWorldObjectFreeCursor = 0;
    for (auto& record : gWorldObjects) {
        if (record.used == 0) {
            continue;
        }
        login::YobiAction* const action = record.action;
        if (action != nullptr) {
            login::detail::ClearMapActorAttachments(action);
            login::detail::MarkDeath(record.action);
            record.action = nullptr;
        }
        record.used = 0;
        record.flags42 = 0;
        record.flags120 = 0;
        record.objectIndex = 0;
        record.value34 = -1;
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
