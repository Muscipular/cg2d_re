#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::world::detail {

// 原三项108H资料及828H资料记录；只共享布局，持久/当前/查看/回复区各有唯一存储。
struct ProfileEntry {
    std::int32_t value00;
    std::int32_t value04;
    std::array<char, 0x100> text08;
};

struct ProfileRuntimeRecord {
    std::int32_t value00;
    std::int32_t value04;
    std::array<char, 0x100> text08;
    std::int32_t value108;
    std::int32_t value10C;
    std::array<char, 0x100> text110;
    std::array<char, 0x100> text210;
    std::array<char, 0x100> text310;
    std::array<ProfileEntry, 3> entries410;
    std::array<char, 0x100> text728;
};

const ProfileRuntimeRecord& ReadViewedProfile105ECE0(); // 窗口62借用4CAC60唯一生产区。
const ProfileRuntimeRecord& ReadReceivedProfile1074110(); // 窗口64借用DpSY独立回复区。
std::int32_t ReadCategoryValue105FA20(); // 窗口62/63借用category回复费用，不复制owner。
extern std::int32_t gMiniMailSelectionD673E4; // 同一发信目标，4C9490及窗口63共同更新。
std::int32_t& SelectedProfileValue04(); // 每次按当前S16角色借用持久资料公开标志，不缓存跨发送身份。

// 原F19FB0的74H联系人记录，三路回复生产，普通邮件与窗口共用。
struct ContactRuntimeRecord {
    std::uint16_t used;
    std::int16_t value02; // 排序按S16比较，回复保留低W16位型。
    std::int32_t value04;
    std::int32_t value08;
    std::int32_t value0C;
    std::array<char, 17> name10;
    std::array<char, 17> text21;
    std::array<char, 33> text32;
    std::array<char, 33> text53;
};

static_assert(std::is_standard_layout_v<ContactRuntimeRecord>);
static_assert(std::is_trivially_copyable_v<ContactRuntimeRecord>);
static_assert(offsetof(ContactRuntimeRecord, used) == 0);
static_assert(offsetof(ContactRuntimeRecord, value02) == 2);
static_assert(offsetof(ContactRuntimeRecord, value04) == 4);
static_assert(offsetof(ContactRuntimeRecord, value08) == 8);
static_assert(offsetof(ContactRuntimeRecord, value0C) == 0x0C);
static_assert(offsetof(ContactRuntimeRecord, name10) == 0x10);
static_assert(offsetof(ContactRuntimeRecord, text21) == 0x21);
static_assert(offsetof(ContactRuntimeRecord, text32) == 0x32);
static_assert(offsetof(ContactRuntimeRecord, text53) == 0x53);
static_assert(sizeof(ContactRuntimeRecord) == 0x74);

extern std::array<ContactRuntimeRecord, 60> gContactsF19FB0;
extern std::array<std::int32_t, 60> gContactIndicesFDA608; // 数值索引，不是地址。
extern std::uint16_t gValue62B354; // 518DA0写未读类别0/1/2/3。
extern std::uint32_t gValueCB1384; // 同源邮件页号，518C80清零。
extern std::uint32_t gValueCB0354; // 无未读时恢复的邮件页号。
extern std::int32_t gValueCF4FC0; // 收信比较的原S32编号槽，回复与后续UI共享。
extern std::int32_t gValueD046D4; // 原邮件窗口消息位置，回复只在<9时递增。
void Sub_4EC1A0(); // 原序生成/排序60个索引，比较器相等仍为-1。
void Sub_50D740(std::uintptr_t unusedSocket, const char* payload); // 基础批量回复，无排序尾。
void Sub_50D8C0(std::uintptr_t unusedSocket, const char* payload); // 长文本批量回复，提前结束也排序。
void Sub_50D9E0(
    std::uintptr_t unusedSocket, std::int32_t index, const char* payload); // 单槽回复，可清同源普通邮件。
void Sub_50DC70(
    std::uintptr_t unusedSocket, std::int32_t index,
    const char* payload, std::int32_t unusedFourth); // 普通收信：插入/保存/扫描/通知。
void Sub_5191C0(
    std::int32_t index, std::int32_t kind, const char* name, const char* text); // 九次倒序搬移后写首条。
std::int32_t Sub_518DA0(); // 普通/公会/迷你三类扫描，首命中发布页号并返回1。
std::int32_t Sub_534370(std::int32_t key); // 20个迷你联系人，used严格1，返回首个匹配槽。
std::int32_t Sub_5343A0(); // 返回首个used==0的槽或-1，不预先占用。
void Sub_4C9490(); // 先清迷你序号，再将共享选择槽写-1。
std::int32_t Sub_534520(char* destination); // 原time32/localtime格式化并复制时间戳，返回0。
void Sub_5343C0(
    std::int32_t index, std::int32_t kind, const char* name, const char* text); // 同源迷你邮件九次搬移/写首条。
void Sub_534480(std::int32_t index); // 原20次清理，每轮两次读取共享593850，不新增索引拒绝。
void Sub_50F430(std::uintptr_t unusedSocket, const char* payload); // 查槽后无条件删除，包括-1。
void Sub_50F7F0(
    std::uintptr_t unusedSocket, std::int32_t key,
    const char* payload, std::int32_t unusedFourth); // 迷你收信，无文件保存。
void Sub_50F9E0(std::uintptr_t unusedSocket, const char* payload); // 完整首次生产、时间戳及两路插入。

// 原资料/迷你邮件共享生命周期；不复用94H登录记录或510H保存记录。
void Sub_533910(); // 只清20个used，再清消息、profile双槽和当前资料。
void Sub_533950(); // 默认首槽后复制到第二槽及所选当前资料，保留未写字段。
std::int32_t Sub_533C80(std::int32_t mode); // 两路profile同步；mode0发送后局部写回，mode1透传发送结果。
std::int32_t Sub_534290(); // 先同步profile，再重取连接发送lz；第一次失败不抑制第二次。
std::int32_t Sub_534770(); // profile.dat整块保存，打开成功即返回1。
std::int32_t Sub_5347F0(); // 整块读取后重取角色并复制当前资料，不按短读失败。

// 原category.dat同源三组与profile编号回复；均无场景门禁。
std::int32_t Sub_50F2A0(std::uintptr_t unusedSocket, const char* payload); // 第一/第二组生产，返回计数。
std::int32_t Sub_50F3A0(std::uintptr_t socket, const char* payload); // 写profile编号后比较旧表头，按差异补发。
std::int32_t Sub_50F8E0(std::uintptr_t unusedSocket, const char* payload); // 第三组生产，返回计数。
std::int32_t Sub_5348A0(); // bin/category.dat原13ED4H整块保存。
std::int32_t Sub_534910(); // 同源整块读取，不按短读失败或清尾。

// 原49D8F0三类资料窗口的数据生产；先整串反转义，未用首参数仍保留。
void Sub_4C9E40(std::int32_t unusedValue, char* payload); // 分类三组选择字节，不改used。
void Sub_4CA330(std::int32_t unusedValue, char* payload); // 搜索结果最多20条，仅预清used。
std::int32_t Sub_4CAC60(std::int32_t unusedValue, char* payload); // 独立查看资料，返回末次图号查询。
std::int32_t Sub_5342F0(std::int32_t key, std::int32_t mode); // 分类第1/3组严格used==1首匹配，失败-1。
void Sub_50F450(
    std::uintptr_t unusedSocket, const char* text1, std::int32_t value2,
    const char* text3, const char* text4, const char* text5, std::int32_t value6,
    std::int32_t value7, const char* text8, std::int32_t value9,
    const char* text10, std::int32_t value11, const char* text12,
    const char* text13); // 完整资料回复，无门禁，八独立文本副本后打开64。

// 原 518C80：依次清两段邮件持久区及 CB1384，返回 0。
std::int32_t Sub_518C80();
// 原 [518CB0,518D94)：依次读两文件，返回 fclose 或缺失文件 helper 的整数结果。
std::int32_t Sub_518CB0();
// 原 [519300,51936C) 与 [519550,5195BC)：各自重取保存路径，写唯一共享缓冲。
std::int32_t Sub_519300();
std::int32_t Sub_519550();

// 原 519290/5194C0：按同一 S16 所选角色清第一/第二邮件组，不保存文件，返回 0。
std::int32_t Sub_519290();
std::int32_t Sub_5194C0();
// 原 5192C0：清所选角色的一个普通邮件编号，不检查编号范围，返回 0。
std::int32_t Sub_5192C0(std::int32_t number);
// 原 5195F0：扫描全部 200 项，返回 active==1 且 key 相等的最后索引，未命中为 -1。
std::int32_t Sub_5195F0(std::int32_t key);
// 原 5194F0：查询后重新读取角色号，清该项；未命中返回 -1，命中返回 0。
std::int32_t Sub_5194F0(std::int32_t key);
// 原 519370：混合返回 W32 FFFFFFFF、含 NUL 长度或满组末地址+4；不截断地址域。
std::uintptr_t Sub_519370(
    std::int32_t key, std::int32_t kind, const char* name, const char* text);

// 原 50EC50 的 50EE4A–50EEA1 公共尾：全 key 比较，不检查活动位、不保存文件。
// selected 由父入口在计数后、计数发布前读取；返回最后邮件决定的零或表末游标。
std::uintptr_t PruneGuildMailAgainstOrderedRecords_50EE4A(std::int16_t selected);

} // namespace cg::world::detail
