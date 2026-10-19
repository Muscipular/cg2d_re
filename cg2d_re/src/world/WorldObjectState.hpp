#pragma once

#include <string_view>
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/Yobi/YobiRuntime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::yobi {
struct YobiAction;
}

namespace cg::world::detail {

// 原0x13C记录的增量映射；未消费区不计最终指针/字段映射，见World对象SPEC。
struct WorldObjectRecord {
    // 原0x0043D7C0：成员正序构造及失败前缀清理由C++承接。
    WorldObjectRecord() = default;
    // 原0x0043D810：成员自动逆序析构。
    ~WorldObjectRecord() = default;
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t category; // 原成员偏移：+0x02。
    std::int32_t objectIndex; // 原成员偏移：+0x04。
    std::uint32_t imageNo; // 原成员偏移：+0x08。
    platform::LegacyCryptInt coordinate0C; // 原成员偏移：+0x0C。
    platform::LegacyCryptInt coordinate1C; // 原成员偏移：+0x1C。
    std::int32_t level2C; // 原成员偏移：+0x2C。
    std::int32_t direction; // 原成员偏移：+0x30。
    std::int32_t value34; // 原成员偏移：+0x34。
    std::int32_t value38; // 原+38；value34待转方向对应的目标X。
    std::int32_t value3C; // 原+3C；同一待转方向对应的目标Y。
    std::uint16_t value40; // 原成员偏移：+0x40。
    std::uint16_t flags42; // 原成员偏移：+0x42。
    std::uint8_t marker44; // 原成员偏移：+0x44。 442BD0置1，449CC0读取；与flags42分属不同槽。
    std::array<std::byte, 3> bytes45; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
    std::int32_t value4C; // 原成员偏移：+0x4C。
    std::int32_t value50; // 原成员偏移：+0x50。
    std::array<char, 17> name54; // 原成员偏移：+0x54。 446470只清首字节；生产入口按原字节长度门槛复制。
    std::array<char, 17> title65; // 原成员偏移：+0x65。
    std::array<char, 33> text76; // 原成员偏移：+0x76。
    std::array<char, 33> text97; // 原成员偏移：+0x97。
    std::array<char, 17> textB8; // 原成员偏移：+0xB8。
    std::array<char, 61> nameC9; // 原成员偏移：+0xC9。 445D20/445F40仅在长度<=60时复制含NUL原字节。
    std::array<std::byte, 6> bytes106; // 原成员偏移：+0x106。
    std::int32_t value10C; // 原成员偏移：+0x10C。 442CA0/442D00/442D60的共同数值槽。
    std::array<std::byte, 4> bytes110; // 原成员偏移：+0x110。
    std::uint32_t flags114; // 原成员偏移：+0x114。
    std::int32_t value118; // 原成员偏移：+0x118。
    cg::yobi::YobiAction* action; // 原+11C，Yobi链拥有，本表只借用。
    std::uint16_t flags120; // 原成员偏移：+0x120。
    std::array<std::byte, 2> bytes122; // 原成员偏移：+0x122。
    std::int32_t value124; // 原+124，442F70状态图号表下标。
    std::array<char, 17> text128; // 原+128；445350的第4段称号。
    std::array<std::byte, 3> bytes139; // 原成员偏移：+0x139。
};

inline constexpr std::size_t kWorldObjectCapacity = 1000;
extern std::array<WorldObjectRecord, kWorldObjectCapacity> gWorldObjects; // 原全局：0x005AC4C0。
extern std::int32_t gWorldObjectFindCursor; // 原5AC498。
extern std::int32_t gWorldObjectFreeCursor; // 原5AC49C。
extern std::int32_t gWorldObjectUsedCount; // 原5AC4B4。
extern std::int32_t gWorldObjectUpperBound; // 原5F9738；不是占用数。

static_assert(offsetof(WorldObjectRecord, coordinate0C) == 0x0C);
static_assert(offsetof(WorldObjectRecord, coordinate1C) == 0x1C);
static_assert(offsetof(WorldObjectRecord, level2C) == 0x2C);
static_assert(offsetof(WorldObjectRecord, direction) == 0x30);
static_assert(offsetof(WorldObjectRecord, value34) == 0x34);
static_assert(offsetof(WorldObjectRecord, value38) == 0x38);
static_assert(offsetof(WorldObjectRecord, value3C) == 0x3C);
static_assert(offsetof(WorldObjectRecord, flags42) == 0x42);
static_assert(offsetof(WorldObjectRecord, marker44) == 0x44);
static_assert(offsetof(WorldObjectRecord, value48) == 0x48);
static_assert(offsetof(WorldObjectRecord, value4C) == 0x4C);
static_assert(offsetof(WorldObjectRecord, value50) == 0x50);
static_assert(offsetof(WorldObjectRecord, name54) == 0x54);
static_assert(offsetof(WorldObjectRecord, title65) == 0x65);
static_assert(offsetof(WorldObjectRecord, text76) == 0x76);
static_assert(offsetof(WorldObjectRecord, text97) == 0x97);
static_assert(offsetof(WorldObjectRecord, textB8) == 0xB8);
static_assert(offsetof(WorldObjectRecord, nameC9) == 0xC9);
static_assert(offsetof(WorldObjectRecord, bytes106) == 0x106);
static_assert(offsetof(WorldObjectRecord, value10C) == 0x10C);
static_assert(offsetof(WorldObjectRecord, bytes110) == 0x110);
static_assert(offsetof(WorldObjectRecord, flags114) == 0x114);
static_assert(offsetof(WorldObjectRecord, value118) == 0x118);
static_assert(offsetof(WorldObjectRecord, action) == 0x120);
static_assert(offsetof(WorldObjectRecord, flags120) == 0x128);
static_assert(offsetof(WorldObjectRecord, bytes122) == 0x12A);
static_assert(offsetof(WorldObjectRecord, value124) == 0x12C);
static_assert(offsetof(WorldObjectRecord, text128) == 0x130);
static_assert(offsetof(WorldObjectRecord, bytes139) == 0x141);
static_assert(sizeof(WorldObjectRecord) == 0x148);

} // namespace cg::world::detail

namespace cg::world {
// 原函数：0x004462B0。
cg::yobi::YobiAction* FindObjectYobi_4462B0(std::int32_t objectIndex); // WOL01：查槽一次/负返空，非负借用action。
// 原函数：0x0050AEF0。
void HandlePartyActorVitalsReply_50AEF0(std::uintptr_t unusedSocket, const char* payload); // 最多五组对象HP/FP，无全局门禁。
// 原函数：0x0050B380。
void HandleMapObjectListReply_50B380(std::uintptr_t socket, const char* payload); // 完整TAB对象批量回复，socket未用。
// 原函数：0x0050BCD0。
void Sub_50BCD0(std::uintptr_t socket, const char* payload); // 逗号动作批量回复，先观察拦截。
// 原函数：0x0050BF20。
void Sub_50BF20(std::uintptr_t unusedSocket, const char* payload); // 删除编号后清首个匹配地图槽引用。
// 原函数：0x0043DB00。
std::uint16_t* SuspendWorldActorForObservation_43DB00(cg::yobi::YobiAction* action); // flags120观察低BYTE置80，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x0043DB30。
std::uint16_t* ResumeWorldActorAfterObservation_43DB30(cg::yobi::YobiAction* action); // flags120原W16清80，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x00446900。
std::int32_t RegisterMapActorDrawKey_446900(
    std::int32_t graphicIndex, std::int32_t mapX, std::int32_t mapY);
// WOP01：只生产无action记录；已存在/领取失败返0，原字段/文本生产完成返1。
// 原函数：0x00445A60。
std::int32_t RegisterWorldObjectWithoutAction_445A60(
    std::int32_t objectIndex, std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction, const char* name,
    const char* titles, std::int32_t level, std::uint16_t value40,
    std::int32_t flag1, std::int32_t flag2, std::int32_t type);
// 原函数：0x004462E0。
void RemoveWorldObject_4462E0(std::int32_t objectIndex); // WLR01：玩家专用/世界删除，按原清理与上界收尾。
// 原函数：0x004467D0。
void RecreateWorldObjectAction_4467D0(std::int32_t objectIndex); // WLR01：空action恢复，原坐标读序/借用/字节发布保持。
// 原函数：0x00519EE0。
void SynchronizePlayerMapActionPosition_519EE0(); // 原入口不读取参数；检查当前坐标并同步 action 载荷。

namespace detail {
// 原函数：0x00521F20。
void RenderChara_521F20();                 // 原 World::RenderChara@0x521F20。
// 原函数：0x0046B4B0。
void TurnPlayerAndSendDirection_46B4B0(std::int32_t direction); // 原完整转向门禁、发布与节流发送。
// 原函数：0x00444FA0。
std::int32_t FindWorldObjectSlot_444FA0(std::int32_t objectIndex); // WOL01：两段编号查槽，命中更新查找游标。
// 原函数：0x00449C90。
std::uint16_t EnableWorldActorModeMarker_449C90(cg::yobi::YobiAction* action); // 同一次W16 flags42取样/条件置100并返低W16结果；NULL返0。
// 原函数：0x00449CC0。
void ClearWorldActorModeMarkerUnlessLeader_449CC0(cg::yobi::YobiAction* action); // marker44为0才清flags42的100。
// 原函数：0x00442C40。
std::uint16_t* MarkWorldActorAsPartyMember_442C40(cg::yobi::YobiAction* action); // flags42高BYTE置2，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x00442C70。
void ClearWorldActorPartyMembership_442C70(cg::yobi::YobiAction* action); // 先场景门禁，再通过payload清flags42的200。
// 原函数：0x00442BD0。
void MarkWorldActorAsPartyLeader_442BD0(cg::yobi::YobiAction* action); // flags42置100，再marker44=1。
// 原函数：0x00442C00。
void ClearWorldActorLeaderMarker_442C00(cg::yobi::YobiAction* action); // 清marker44，非模式2才清flags42的100。
// 原函数：0x00442CA0。
void Sub_442CA0(cg::yobi::YobiAction* action, std::int32_t value); // 置4000，再写value10C。
// 原函数：0x00442CD0。
std::uint16_t* Sub_442CD0(cg::yobi::YobiAction* action); // flags42原W16清4000/保value10C，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x00442D00。
void Sub_442D00(cg::yobi::YobiAction* action, std::int32_t value); // 置8000，再写value10C。
// 原函数：0x00442D30。
std::uint16_t* Sub_442D30(cg::yobi::YobiAction* action); // flags42原W16清8000，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x00442D60。
void Sub_442D60(cg::yobi::YobiAction* action, std::int32_t value); // 置0400，再写value10C。
// 原函数：0x00442D90。
std::uint16_t* Sub_442D90(cg::yobi::YobiAction* action); // flags42原W16 AND EBFF，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x0043D970。
std::uint16_t* ShowWorldActorPersistentStatusIcon_43D970(cg::yobi::YobiAction* action); // flags120低BYTE置1，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x0043D9A0。
std::uint16_t* HideWorldActorPersistentStatusIcon_43D9A0(cg::yobi::YobiAction* action); // flags120原W16清1，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x0043D9D0。
void SetWorldActorStatusIcon_43D9D0(cg::yobi::YobiAction* action, std::int32_t value); // 原范围回退后置2/写value124。
// 原函数：0x0043DA10。
void ClearWorldActorStatusIcon_43DA10(cg::yobi::YobiAction* action); // flags120清2，再清value124。
// 原函数：0x0043DA40。
std::uint16_t* ShowWorldActorTimedStatusIcon_43DA40(cg::yobi::YobiAction* action); // flags120低BYTE置24，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x0043DA70。
std::uint16_t* ShowActorPetPersistentStatusIcon_43DA70(cg::yobi::YobiAction* action); // flags120低BYTE置8，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x0043DAA0。
std::uint16_t* HideActorPetPersistentStatusIcon_43DAA0(cg::yobi::YobiAction* action); // flags120原W16清8，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x0043DAD0。
std::uint16_t* ShowActorPetTimedStatusIcon_43DAD0(cg::yobi::YobiAction* action); // flags120低BYTE置50，返同字段非拥有地址；NULL返nullptr。
// 原函数：0x00444410。
void ApplyWorldActorActionCommand_444410(
    cg::yobi::YobiAction* action, std::int32_t x, std::int32_t y,
    std::int32_t direction, std::int32_t characterAction, std::int32_t option1,
    std::int32_t option2, std::int32_t option3, std::int32_t option4);
// 原函数：0x00446130。
std::int32_t ReserveWorldObjectSlot_446130(); // WOL01：空槽领取返回旧游标，只推进计数/游标，保持记录。
// 原函数：0x004461E0。
std::uint32_t MapWorldObjectTypeToDrawFlags_4461E0(std::int32_t type); // WOL01：类型转flags114，返回完整W32。
// 原函数：0x00446210。
std::uint32_t MapWorldObjectTypeToCategory_446210(std::int32_t type); // WOL01：71索引/五目标完整W32返回，父写category才截低W16。
// 原函数：0x005183B0。
std::int32_t IsAnimationImageNumber_5183B0(std::int32_t imageNo); // 原图号[100000,200000)判断，不读取资源表。
// 原函数：0x004468F0。
void ResetMapActorDrawKeys_4468F0(); // 原普通场景51D246：只清三值去重计数，不清表。
// 原445350完整实体生产/更新；仅新建方向门禁、两坐标取样与四文本槽两阶段复用沿MOP01 SPEC。
void UpsertWorldActor_445350(
    std::int32_t objectIndex, std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction, const char* name,
    const char* titles, const char* extraText, std::int32_t level,
    std::int32_t value40, std::int32_t flag1, std::int32_t flag2,
    std::int32_t type);
// WOP01：类别2生产或更新；已有记录不补建action，保领取/创建失败及两坐标取样序。
// 原函数：0x00445D20。
void UpsertGroundItemObject_445D20(
    std::int32_t objectIndex, std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction, std::int32_t value48,
    const char* name, std::int32_t value50);
// WOP01：类别4生产或更新；原两坐标先取齐再发布，保原名字门槛和flags尾。
// 原函数：0x00445F40。
void UpsertGroundGoldObject_445F40(
    std::int32_t objectIndex, std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction, std::int32_t value4C,
    std::wstring_view name);
// WGQ01：类别/必需flags120命中返对象编号，否则-1；掩码0不命中。
// 原函数：0x00445030。
std::int32_t FindWorldObjectAtWithActionFlags_445030(
    std::int32_t x, std::int32_t y, std::uint16_t category, std::uint16_t flags);
// 原函数：0x004450D0。
std::int32_t HasWorldObjectAt_4450D0(
    std::int32_t x, std::int32_t y, std::uint16_t category); // WGQ01：类别命中返完整0/1。
// 原函数：0x00445160。
std::int32_t FindWorldObjectAt_445160(
    std::int32_t x, std::int32_t y, std::uint16_t category); // WGQ01：命中返对象编号，否则-1。
// WGQ01：包含掩码0分别跳过对应字段，非零要求原W16交集，返完整0/1。
// 原函数：0x004451F0。
std::int32_t HasWorldObjectAtWithFlags_4451F0(
    std::int32_t x, std::int32_t y, std::uint16_t category,
    std::uint16_t flags42Mask, std::uint16_t flags120Mask);
// WGQ01：排除掩码0仍读取原W16字段，42失败不取120，返完整0/1。
// 原函数：0x004452A0。
std::int32_t HasWorldObjectAtWithoutFlags_4452A0(
    std::int32_t x, std::int32_t y, std::uint16_t category,
    std::uint16_t excluded42, std::uint16_t excluded120);
// 原函数：0x00446470。
void ResetWorldObjectTable_446470(); // WLR01：只清引用/原字段的整表重置，无释放或标死。
// 原函数：0x00446610。
void DetachWorldObjectActions_446610(); // WBR01：全表仅占用项清action/value34，无标死/free或计数重置。
// 原函数：0x0046A5F0。
void AdvanceFollowersBehindPlayerLeader_46A5F0(); // 原完整跟随路径调度；到达后启动首路径并向后继传点。
// 原函数：0x0046A190。
std::uint32_t UpdatePartyFollowMovement_46A190(); // 完整两轮跟随调度，返回公共尾先读取的世界X位型。
// 原函数：0x0046B1B0。
void MovePlayerOneStep_46B1B0(
    std::int32_t x, std::int32_t y, std::int32_t speed); // 普通移动：先通知跟随对象，再快照/计算速度。
// 原函数：0x0051A640。
cg::yobi::YobiAction* MarkPlayerMovementActive_51A640(); // 原+272置1，返回入口借用指针。
// 原函数：0x0051A660。
cg::yobi::YobiAction* ClearPlayerMovementActive_51A660(); // 原+272清0，返回入口借用指针。
// 原函数：0x0051A680。
std::int32_t ReadPlayerMovementActive_51A680(); // 空玩家为0，否则MOVSX读取原+272，非指针返回。
// 原函数：0x004464D0。
void __cdecl ClearWorldObjectsAndActions_4464D0(); // WLR01：已占用槽附件/text清理后fresh主标死，保名称。
// 原函数：0x00446640。
void RecreateMissingWorldObjectActions_446640(); // WBR01：全1000槽补建，原四类名字/flags尾；未知类创建仍保。

} // namespace detail

} // namespace cg::world
