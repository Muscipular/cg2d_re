#include "WorldObjectState.hpp"

#include "WorldUpdateDependencies.hpp"
#include "../platform/LegacyRuntimeExit.hpp"
#include "cg/Input.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/world/WorldObjects.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::world {
namespace detail {

std::array<WorldObjectRecord, kWorldObjectCapacity> gWorldObjects {}; // 原全局：0x005AC4C0。
std::int32_t gWorldObjectFindCursor = 0; // 原全局：0x005AC498。
std::int32_t gWorldObjectFreeCursor = 0; // 原全局：0x005AC49C。
std::int32_t gWorldObjectUsedCount = 0; // 原全局：0x005AC4B4。
std::int32_t gWorldObjectUpperBound = 0; // 原全局：0x005F9738。

namespace {

// 原[5ABFE0,5AC490)三列数值缓存，与后方World对象表独立。
struct MapActorDrawKey {
    std::int32_t graphicIndex;
    std::int32_t mapX;
    std::int32_t mapY;
};

std::array<MapActorDrawKey, 100> gMapActorDrawKeys {};
std::int32_t gMapActorDrawKeyCount = 0; // 原5AC494。
static_assert(sizeof(MapActorDrawKey) == 12);
static_assert(sizeof(gMapActorDrawKeys) == 0x4B0);

} // namespace

// WBR01：只清占用槽action/value34，无释放/标死；两个父的三个调用点先YobiClearUp。
// 原函数：0x00446610。
void Sub_446610()
{
    for (auto& record : gWorldObjects) {
        if (record.used != 0) {
            record.action = nullptr;
            record.value34 = -1;
        }
    }
}

// 原函数：0x004468F0。
void Sub_4468F0()
{
    gMapActorDrawKeyCount = 0;
}

// 原函数：0x00446900。
std::int32_t Sub_446900(
    const std::int32_t graphicIndex, const std::int32_t mapX,
    const std::int32_t mapY)
{
    const std::int32_t count = gMapActorDrawKeyCount;
    for (std::int32_t index = 0; index < count; ++index) {
        const auto& key = gMapActorDrawKeys[static_cast<std::size_t>(index)];
        if (key.graphicIndex == graphicIndex && key.mapX == mapX && key.mapY == mapY) {
            return 1;
        }
    }
    if (count < 100) {
        auto& key = gMapActorDrawKeys[static_cast<std::size_t>(count)];
        const std::int32_t nextCount = count + 1;
        key.graphicIndex = graphicIndex;
        key.mapX = mapX;
        key.mapY = mapY;
        gMapActorDrawKeyCount = nextCount;
    }
    return 0;
}

// WOL01：完整两段查槽，start/end各持样一次，used W16非零/编号W32命中后发布查找游标。
// 原函数：0x00444FA0。
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

// WOL01：一次真实查槽后借用原生action指针，保查槽游标副作用，不转移所有权。
// 原函数：0x004462B0。
login::YobiAction* FindObjectYobi_4462B0(const std::int32_t objectIndex)
{
    const std::int32_t index = Sub_444FA0(objectIndex);
    return index < 0 ? nullptr : gWorldObjects[index].action;
}

// WOL01：只推进计数/高水位/空槽游标并返回旧槽，不写记录或标占用/分配资源。
// 原函数：0x00446130。
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

// WOL01：原类型到完整W32旗位的正常映射，无全局发布/资源或RNG消费。
// 原函数：0x004461E0。
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

// WOL01：已有1920/CAC07五target/71selector补完整正常入口；低W16仅由写入父消费。
// 原函数：0x00446210。
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

// WOP01：无action记录生产，原两坐标三值先持样后发布；保三个独立文本缓冲。
// 原函数：0x00445A60。
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
    temporary.ConstructFromInt_401CC0(mapX);
    const std::uint8_t xReadCount = temporary.readCount;
    const std::uint32_t xEncoded = temporary.encoded;
    const std::uint32_t xMask = temporary.mask;
    record.coordinate0C.readCount = xReadCount;
    record.coordinate0C.encoded = xEncoded;
    record.coordinate0C.mask = xMask;
    temporary.Destroy_401CF0();
    temporary.ConstructFromInt_401CC0(mapY);
    const std::uint32_t yMask = temporary.mask;
    const std::uint32_t yEncoded = temporary.encoded;
    const std::uint8_t yReadCount = temporary.readCount;
    record.coordinate1C.mask = yMask;
    record.coordinate1C.encoded = yEncoded;
    record.coordinate1C.readCount = yReadCount;
    temporary.Destroy_401CF0();
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
    // 原var_100/var_200/var_300；ordinal1/2/3分别借100/300/200。
    char text100[256];
    char text200[256];
    char text300[256];
    network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text100);
    if (std::strlen(text100) <= 16) {
        std::memmove(record.title65.data(), text100, std::strlen(text100) + 1);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, text300);
    input::UnescapeLegacyTextAndNotify_52D2F0(text300);
    if (std::strlen(text300) <= 32) {
        std::memmove(record.text76.data(), text300, std::strlen(text300) + 1);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, text200);
    input::UnescapeLegacyTextAndNotify_52D2F0(text200);
    if (std::strlen(text200) <= 32) {
        std::memmove(record.text97.data(), text200, std::strlen(text200) + 1);
    }
    record.flags114 = Sub_4461E0(type);
    return 1;
}

// WLR01：完整编号删除，先共享附件清理/fresh主标死，按原计数/游标收尾；全空保旧上界。
// 原函数：0x004462E0。
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

// WLR01：无释放整表重置，只原七字段/名称首BYTE及四计数游标，不合并为有释放清理。
// 原函数：0x00446470。
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

// WLR01：只清已占用槽，附件/文本先清再fresh主标死；保名称和未占用槽字段。
// 原函数：0x004464D0。
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

namespace {

// 原43D7C0按0C/1C构造；第一成员成功后才承担其C++失败清理，返回真实对象。
detail::WorldObjectRecord* ConstructLegacyWorldElement_43D7C0(detail::WorldObjectRecord* const record)
{
    record->coordinate0C.ConstructZero_401C70();
    try {
        record->coordinate1C.ConstructZero_401C70();
    } catch (...) {
        record->coordinate0C.Destroy_401CF0();
        throw;
    }
    return record;
}

// 原43D810逆序1C/0C，首析构失败只继续剩余0C，不重试失败成员。
void DestroyLegacyWorldElement_43D810(detail::WorldObjectRecord* const record)
{
    try {
        record->coordinate1C.Destroy_401CF0();
    } catch (...) {
        record->coordinate0C.Destroy_401CF0();
        throw;
    }
    record->coordinate0C.Destroy_401CF0();
}

// 原迭代器不消费构造返回；只适配显式元素地址/函数签名，不计新原身份。
void ConstructLegacyWorldElementForArray(void* const storage)
{
    static_cast<void>(ConstructLegacyWorldElement_43D7C0(
        static_cast<detail::WorldObjectRecord*>(storage)));
}

void DestroyLegacyWorldElementForArray(void* const storage)
{
    DestroyLegacyWorldElement_43D810(static_cast<detail::WorldObjectRecord*>(storage));
}

} // namespace

// 原CRT 553014：千项对象构造完成后，真实登记整体退出并透传完整S32。
// 原函数：0x0043D760。
std::int32_t InitializeLegacyWorldObjects_43D760()
{
    ConstructWorldObjects();
    return std::atexit(DestroyWorldObjects);
}

// 原函数：0x0043D770。
void ConstructWorldObjects()
{
    platform::ConstructLegacyArray_53C45E(
        detail::gWorldObjects.data(), static_cast<std::uint32_t>(sizeof(detail::WorldObjectRecord)),
        static_cast<std::int32_t>(detail::kWorldObjectCapacity),
        ConstructLegacyWorldElementForArray, DestroyLegacyWorldElementForArray);
}

// 原函数：0x0043D7A0。
void DestroyWorldObjects()
{
    platform::DestroyLegacyArray_53C4E0(
        detail::gWorldObjects.data(), static_cast<std::uint32_t>(sizeof(detail::WorldObjectRecord)),
        static_cast<std::int32_t>(detail::kWorldObjectCapacity), DestroyLegacyWorldElementForArray);
}

} // namespace cg::world
