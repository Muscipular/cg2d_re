#include "WorldObjectState.hpp"
#include "cg/session/ClientSession.hpp"

#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/world/SceneState.hpp"
#include "PlayerState.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::world {
namespace detail {

void InitializeWorldObjectStorage(session::ClientSession& session)
{
    auto& objects = session.WorldScene().objects;
    if (objects == nullptr) {
        objects = session::MakeSessionStorage<std::array<WorldObjectRecord, kWorldObjectCapacity>>(session);
    }
}

// WBR01：只清占用槽action/value34，无释放/标死；两个父的三个调用点先YobiClearUp。
// 原函数：0x00446610。
void DetachWorldObjectActions_446610(session::ClientSession& session)
{
    for (auto& record : (*session.WorldScene().objects)) {
        if (record.used != 0) {
            record.action = nullptr;
            record.value34 = -1;
        }
    }
}

// 原函数：0x004468F0。
void ResetMapActorDrawKeys_4468F0(session::ClientSession& session)
{
    session.WorldScene().mapActorDrawKeyCount = 0;
}

// WOL01：完整两段查槽，start/end各持样一次，used W16非零/编号W32命中后发布查找游标。
// 原函数：0x00444FA0。
std::int32_t FindWorldObjectSlot_444FA0(session::ClientSession& session, const std::int32_t objectIndex)
{
    const std::int32_t start = session.WorldScene().objectFindCursor;
    const std::int32_t end = session.WorldScene().objectUpperBound;
    for (std::int32_t index = start; index < end; ++index) {
        const auto& record = (*session.WorldScene().objects)[static_cast<std::size_t>(index)];
        if (record.used != 0 && record.objectIndex == objectIndex) {
            session.WorldScene().objectFindCursor = index;
            return index;
        }
    }
    for (std::int32_t index = 0; index < start; ++index) {
        const auto& record = (*session.WorldScene().objects)[static_cast<std::size_t>(index)];
        if (record.used != 0 && record.objectIndex == objectIndex) {
            session.WorldScene().objectFindCursor = index;
            return index;
        }
    }
    return -1;
}

// WOL01：只推进计数/高水位/空槽游标并返回旧槽，不写记录或标占用/分配资源。
// 原函数：0x00446130。
std::int32_t ReserveWorldObjectSlot_446130(session::ClientSession& session)
{
    if (session.WorldScene().objectUsedCount >= 1000) {
        return -1;
    }
    const std::int32_t used = session.WorldScene().objectUsedCount;
    const std::int32_t upperBound = session.WorldScene().objectUpperBound;
    const std::int32_t selected = session.WorldScene().objectFreeCursor;
    session.WorldScene().objectUsedCount = used + 1;
    std::int32_t next = selected + 1;
    if (next > upperBound) {
        session.WorldScene().objectUpperBound = next;
    }
    for (; next < 1000; ++next) {
        if ((*session.WorldScene().objects)[static_cast<std::size_t>(next)].used == 0) {
            session.WorldScene().objectFreeCursor = next;
            break;
        }
    }
    if (selected > 0 && next >= 1000) {
        for (std::int32_t index = 0; index < selected; ++index) {
            if ((*session.WorldScene().objects)[static_cast<std::size_t>(index)].used == 0) {
                session.WorldScene().objectFreeCursor = index;
                break;
            }
        }
    }
    return selected;
}

// WOL01：原类型到完整W32旗位的正常映射，无全局发布/资源或RNG消费。
// 原函数：0x004461E0。
std::uint32_t MapWorldObjectTypeToDrawFlags_4461E0(const std::int32_t type)
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
std::uint32_t MapWorldObjectTypeToCategory_446210(const std::int32_t type)
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

// WLR01：无释放整表重置，只原七字段/名称首BYTE及四计数游标，不合并为有释放清理。
// 原函数：0x00446470。
void ResetWorldObjectTable_446470(session::ClientSession& session)
{
    session.WorldScene().objectUsedCount = 0;
    session.WorldScene().objectUpperBound = 0;
    session.WorldScene().objectFreeCursor = 0;
    for (auto& record : (*session.WorldScene().objects)) {
        record.used = 0;
        record.action = nullptr;
        record.flags42 = 0;
        record.flags120 = 0;
        record.objectIndex = 0;
        record.value34 = -1;
        record.name54.clear();
    }
    session.WorldScene().objectFindCursor = 0;
}

// WLR01：只清已占用槽，附件/文本先清再fresh主标死；保名称和未占用槽字段。
// 原函数：0x004464D0。
void __cdecl ClearWorldObjectsAndActions_4464D0(session::ClientSession& session)
{
    session.WorldScene().objectUsedCount = 0;
    session.WorldScene().objectUpperBound = 0;
    session.WorldScene().objectFreeCursor = 0;
    for (auto& record : (*session.WorldScene().objects)) {
        if (record.used == 0) {
            continue;
        }
        cg::yobi::YobiAction* const action = record.action;
        if (action != nullptr) {
            cg::yobi::ClearMapActorAttachments(action);
            cg::yobi::MarkDeath(record.action);
            record.action = nullptr;
        }
        record.used = 0;
        record.flags42 = 0;
        record.flags120 = 0;
        record.objectIndex = 0;
        record.value34 = -1;
    }
    session.WorldScene().objectFindCursor = 0;
}

} // namespace detail

} // namespace cg::world

namespace cg::world {

// 原函数：0x00446900。
std::int32_t RegisterMapActorDrawKey_446900(
    cg::yobi::YobiAction* const source,
    const std::int32_t graphicIndex, const std::int32_t mapX,
    const std::int32_t mapY)
{
    auto& scene = source->ownerSession->WorldScene();
    const std::int32_t count = scene.mapActorDrawKeyCount;
    for (std::int32_t index = 0; index < count; ++index) {
        const auto& key = scene.mapActorDrawKeys[static_cast<std::size_t>(index)];
        if (key.graphicIndex == graphicIndex && key.mapX == mapX && key.mapY == mapY) {
            return 1;
        }
    }
    if (count < 100) {
        auto& key = scene.mapActorDrawKeys[static_cast<std::size_t>(count)];
        const std::int32_t nextCount = count + 1;
        key.graphicIndex = graphicIndex;
        key.mapX = mapX;
        key.mapY = mapY;
        scene.mapActorDrawKeyCount = nextCount;
    }
    return 0;
}

// WOL01：一次真实查槽后借用原生action指针，保查槽游标副作用，不转移所有权。
// 原函数：0x004462B0。
cg::yobi::YobiAction* FindObjectYobi_4462B0(session::ClientSession& session, const std::int32_t objectIndex)
{
    const std::int32_t index = detail::FindWorldObjectSlot_444FA0(session, objectIndex);
    return index < 0 ? nullptr : (*session.WorldScene().objects)[index].action;
}

// WOP01：无action记录生产，原两坐标三值先持样后发布；保三个独立文本缓冲。
// 原函数：0x00445A60。
std::int32_t RegisterWorldObjectWithoutAction_445A60(
    session::ClientSession& session,
    const std::int32_t objectIndex, const std::int32_t imageNo,
    const std::int32_t mapX, const std::int32_t mapY,
    const std::int32_t direction, const char* const name,
    const char* const titles, const std::int32_t level,
    const std::uint16_t value40, const std::int32_t flag1,
    const std::int32_t flag2, const std::int32_t type)
{
    if (detail::FindWorldObjectSlot_444FA0(session, objectIndex) >= 0) {
        return 0;
    }
    const std::int32_t index = detail::ReserveWorldObjectSlot_446130(session);
    if (index < 0) {
        return 0;
    }
    auto& record = (*session.WorldScene().objects)[static_cast<std::size_t>(index)];
    record.action = nullptr;
    record.used = 2;
    record.category = static_cast<std::uint16_t>(detail::MapWorldObjectTypeToCategory_446210(type));
    record.objectIndex = objectIndex;
    record.imageNo = static_cast<std::uint32_t>(imageNo);
    record.value40 = value40;
    record.level2C = level;
    record.coordinate0C = static_cast<std::int32_t>(mapX);
    record.coordinate1C = static_cast<std::int32_t>(mapY);
    record.direction = direction;
    // 原两处只写+42低W8；+43高字节与其他清理入口仍共享同一W16。
    if (flag1 != 0) {
        *reinterpret_cast<unsigned char*>(&record.flags42) |= 0x40u;
    }
    if (flag2 != 0) {
        *reinterpret_cast<unsigned char*>(&record.flags42) |= 0x80u;
    }
    if (std::strlen(name) <= 16) {
        record.name54 = cg::text::FromGbkWide(name);
    }
    // 原var_100/var_200/var_300；ordinal1/2/3分别借100/300/200。
    char text100[256];
    char text200[256];
    char text300[256];
    network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, text100);
    if (std::strlen(text100) <= 16) {
        record.title65 = cg::text::FromGbkWide(text100);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, text300);
    input::UnescapeLegacyTextAndNotify_52D2F0(text300);
    if (std::strlen(text300) <= 32) {
        record.text76 = cg::text::FromGbkWide(text300);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, text200);
    input::UnescapeLegacyTextAndNotify_52D2F0(text200);
    if (std::strlen(text200) <= 32) {
        record.text97 = cg::text::FromGbkWide(text200);
    }
    record.flags114 = detail::MapWorldObjectTypeToDrawFlags_4461E0(type);
    return 1;
}

// WLR01：完整编号删除，先共享附件清理/fresh主标死，按原计数/游标收尾；全空保旧上界。
// 原函数：0x004462E0。
void RemoveWorldObject_4462E0(session::ClientSession& session, const std::int32_t objectIndex)
{
    if (session.WorldScene().player->objectIndex == static_cast<std::uint32_t>(objectIndex)) {
        static_cast<void>(detail::DestroyPlayerMapPresence_519D50(session));
        return;
    }
    const std::int32_t index = detail::FindWorldObjectSlot_444FA0(session, objectIndex);
    if (index < 0) {
        return;
    }
    auto& record = (*session.WorldScene().objects)[static_cast<std::size_t>(index)];
    cg::yobi::YobiAction* const action = record.action;
    if (action != nullptr) {
        cg::yobi::ClearMapActorAttachments(action);
        cg::yobi::MarkDeath(record.action);
        record.action = nullptr;
    }
    const std::int32_t used = session.WorldScene().objectUsedCount;
    record.used = 0;
    record.flags42 = 0;
    record.flags120 = 0;
    record.value34 = -1;
    if (used > 0) {
        session.WorldScene().objectUsedCount = used - 1;
    }
    if (session.WorldScene().objectFreeCursor > index) {
        session.WorldScene().objectFreeCursor = index;
    }
    for (std::int32_t end = session.WorldScene().objectUpperBound; end > 0; --end) {
        if ((*session.WorldScene().objects)[static_cast<std::size_t>(end - 1)].used != 0) {
            session.WorldScene().objectUpperBound = end;
            return;
        }
    }
    // 原44645A在全空时直接返回，保留旧上界。
}

} // namespace cg::world
