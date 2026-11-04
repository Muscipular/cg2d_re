#pragma once

#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/world/PlayerItemRecord.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace cg::session { class ClientSession; }
namespace cg::yobi { struct YobiAction; }
namespace cg::world {

// 原F7C950的已消费AC字节前缀；510490生产，4C75A0等UI读取，非完整业务布局。
struct ReplyRecordF7C950 {
    std::int32_t used; // 原成员偏移：+0x00。
    std::wstring text04; // 原成员偏移：+0x04。
    std::uint8_t hasText24; // 原成员偏移：+0x24。 第2文本不等于单空格；空串同样为1。
    std::wstring text25; // 原成员偏移：+0x25。
    std::array<std::byte, 3> bytes45; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
    std::array<std::int32_t, 24> values4C; // 原成员偏移：+0x4C。
};

// 原首块 [FDA700,FF02E0) 已证玩家槽；不将此原生结构当作原连续内存。
struct PlayerRuntimeSlots {
    // 原0x005184A0：成员正序构造及失败前缀清理由C++承接。
    PlayerRuntimeSlots() = default;
    // 原0x00518410：成员自动逆序析构。
    ~PlayerRuntimeSlots() = default;
    void ClearRaw(std::array<PlayerItemRecord, 28>& items) noexcept; // 原序清理所属会话物品，不消费六次构造RNG。
    std::uint16_t value00; // 原成员偏移：+0x00。 原FDA700；509BA0仅写低W16。
    std::int32_t level; // 原 FDA704。
    std::int32_t value08; // 原成员偏移：+0x08。 原FDA708。
    std::int32_t timerRemaining0C; // 原成员偏移：+0x0C。 原FDA70C；458F40按帧扣减的S32。
    std::int32_t timerMode10; // 原成员偏移：+0x10。 原FDA710；保留完整模式，非零才扣减。
    std::uint64_t timerTick14; // 原成员偏移：+0x14。 原FDA714；50C7A0与458F40同源。
    platform::LegacyCryptInt hp; // 原 FDA718；与 action +170 的载荷同步。
    platform::LegacyCryptInt maxHp; // 原 FDA728。
    platform::LegacyCryptInt fp; // 原 FDA738；与宠物数据独立。
    platform::LegacyCryptInt maxFp; // 原 FDA748。
    platform::LegacyCryptInt value58; // 原成员偏移：+0x58。 原 FDA758；5184A0第五构造对象。
    platform::LegacyCryptInt value68; // 原成员偏移：+0x68。 原 FDA768；5184A0第六构造对象。
    std::array<std::int32_t, 14> values78; // 原成员偏移：+0x78。 原FDA778..FDA7AC，509650两段数值。
    std::array<std::int32_t, 2> valuesB0; // 原成员偏移：+0xB0。 原FDA7B0/FDA7B4，509BA0尾部两值。
    std::int32_t valueB8; // 原成员偏移：+0xB8。 原FDA7B8。
    std::array<std::int32_t, 6> valuesBC; // 原成员偏移：+0xBC。 原FDA7BC..FDA7D0。
    std::array<std::int32_t, 4> valuesD4; // 原成员偏移：+0xD4。 原FDA7D4..FDA7E0。
    std::array<std::int32_t, 4> valuesE4; // 原成员偏移：+0xE4。 原FDA7E4..FDA7F0。
    std::int32_t valueF4; // 原成员偏移：+0xF4。 原FDA7F4。
    std::int32_t valueF8; // 原成员偏移：+0xF8。 原FDA7F8。
    std::int32_t valueFC; // 原成员偏移：+0xFC。 原FDA7FC；与末尾15BCC共用回复门禁。
    std::int32_t titleIndex100; // 原成员偏移：+0x100。 原FDA800，直接索引FF0508原表。
    std::int32_t value104; // 原成员偏移：+0x104。 原FDA804。
    std::wstring name; // 原 FDA808；内部玩家名称。
    std::wstring ownTitle; // 原 FDA819。
    std::wstring text12A; // 原成员偏移：+0x12A。 原 FDA82A。
    std::wstring text14B; // 原成员偏移：+0x14B。 原 FDA84B。
    std::int32_t value16C; // 原成员偏移：+0x16C。 原FDA86C。
    std::int32_t walkSpeed; // 原 FDA870；独立于通用 action 的初始步速。
    std::int32_t value174; // 原成员偏移：+0x174。 原 FDA874；46A700仅在到达时按S32递减。
    std::int32_t imageNo; // 原 FDA878，不是 FDA87C。
    std::int32_t value17C; // 原成员偏移：+0x17C。 原FDA87C，不与imageNo合并。
    std::uint32_t objectIndex; // 原0x00FDA880；玩家对象索引。
    std::int32_t direction; // 原 FDA884；其他消费者可读取低 S8。
    std::uint16_t value188; // 原成员偏移：+0x188。 原 FDA888；519A80 符号扩展读取。
    std::uint16_t flags18A; // 原成员偏移：+0x18A。 原FDA88A W16；与flags18C低W16组成跨字段W32观察。
    std::uint32_t flags18C; // 原成员偏移：+0x18C。 承载原 [FDA88C,FDA890)；18C 低 W8、18E 高 W16 共享字节。
    std::int32_t value190; // 原成员偏移：+0x190。 原 FDA890；51BDD0/51BE50 写入的完整动作值。
    std::uint16_t options194; // 原成员偏移：+0x194。 原FDA894低W16；选项切换/回复与聊天门禁共用。
    std::uint16_t value1A0; // 原成员偏移：+0x1A0。 原FDA8A0；50BFC0写入低W16。
    cg::yobi::YobiAction* action;
    std::wstring text15BB8; // 原成员偏移：+0x15BB8。 原 FF02B8，不紧接新端的指针字节布局。
    std::int32_t value15BCC; // 原成员偏移：+0x15BCC。 原FF02CC；509650尾字段。
    std::array<std::int32_t, 3> values15BD0; // 原成员偏移：+0x15BD0。 原FF02D0/FF02D4/FF02D8。
};

static_assert(offsetof(PlayerRuntimeSlots, flags18C)
    == offsetof(PlayerRuntimeSlots, flags18A) + sizeof(std::uint16_t));

// 首会话在原中央顺序初始化；后续会话沿宿主生命周期调用，不重复构造。
void InitializePlayerRuntimeStorage(session::ClientSession& session);

} // namespace cg::world
