#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::world::detail {

// 原三项108H资料及828H资料记录；只共享布局，持久/当前/查看/回复区各有唯一存储。
struct ProfileEntry {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::array<char, 0x100> text08; // 原成员偏移：+0x08。
};

struct ProfileRuntimeRecord {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::array<char, 0x100> text08; // 原成员偏移：+0x08。
    std::int32_t value108; // 原成员偏移：+0x108。
    std::int32_t value10C; // 原成员偏移：+0x10C。
    std::array<char, 0x100> text110; // 原成员偏移：+0x110。
    std::array<char, 0x100> text210; // 原成员偏移：+0x210。
    std::array<char, 0x100> text310; // 原成员偏移：+0x310。
    std::array<ProfileEntry, 3> entries410; // 原成员偏移：+0x410。
    std::array<char, 0x100> text728; // 原成员偏移：+0x728。
};

// 资料解析／保存与窗口共用原唯一对象；读取者保留const引用。
extern ProfileRuntimeRecord gViewedProfile105ECE0; // 原0x0105ECE0。
extern ProfileRuntimeRecord gReceivedProfile1074110; // 原0x01074110。
extern ProfileRuntimeRecord gCurrentProfile10738E8; // 原0x010738E8。
const ProfileRuntimeRecord& ReadSelectedProfile1074938(); // 每次按当前S16角色借用持久资料。
// 原函数：0x00533B00。
std::int32_t Sub_533B00(); // 固定一次角色，三数值/三文本/正文按原顺序短路比较。
std::int32_t ReadCategoryValue105FA20(); // 窗口62/63借用category回复费用，不复制owner。
extern std::int32_t gMiniMailSelectionD673E4; // 原全局：0x00D673E4。 同一发信目标，4C9490及窗口63共同更新。
std::int32_t& SelectedProfileValue04(); // 每次按当前S16角色借用持久资料公开标志，不缓存跨发送身份。

// 原4CA330生产的20条20CH搜索记录，窗口61只读借用。
struct ProfileSearchRecord {
    std::int32_t used; // 原成员偏移：+0x00。
    std::array<char, 0x100> text04; // 原成员偏移：+0x04。
    std::int32_t value104; // 原成员偏移：+0x104。
    std::int32_t value108; // 原成员偏移：+0x108。
    std::array<char, 0x100> text10C; // 原成员偏移：+0x10C。
};

struct ProfileSearchRuntimeSlots {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::array<ProfileSearchRecord, 20> records; // 原成员偏移：+0x08。
};

extern ProfileSearchRuntimeSlots gProfileSearch105C398; // 原0x0105C398；同一搜索回复对象。
const char* ReadCategoryText105FA24(std::int32_t group, std::int32_t index); // 三组标题借用，无额外索引拒绝。

// 原category.dat三组各100条；窗口60只读借用，存储仍归MailState。
struct CategoryRecord {
    std::uint8_t used; // 原成员偏移：+0x00。
    std::array<std::byte, 3> bytes01; // 原成员偏移：+0x01。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::array<char, 0x100> text0C; // 原成员偏移：+0x0C。
    std::uint8_t selected10C; // 原成员偏移：+0x10C。 4C9E40写选择位，与used独立。
    std::array<std::byte, 3> bytes10D; // 原成员偏移：+0x10D。
};

const CategoryRecord& ReadCategoryRecord105FA24(std::int32_t group, std::int32_t index);
std::int32_t ReadCategoryCount105FA18(); // 类别0/1共用count08，每次读取真实回复状态。
std::int32_t ReadCategoryCount105FA1C(); // 类别2的count0C，与前两类独立。

// 原F19FB0的74H联系人记录，三路回复生产，普通邮件与窗口共用。
struct ContactRuntimeRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::int16_t value02; // 原成员偏移：+0x02。 排序按S16比较，回复保留低W16位型。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::array<char, 17> name10; // 原成员偏移：+0x10。
    std::array<char, 17> text21; // 原成员偏移：+0x21。
    std::array<char, 33> text32; // 原成员偏移：+0x32。
    std::array<char, 33> text53; // 原成员偏移：+0x53。
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

extern std::array<ContactRuntimeRecord, 60> gContactsF19FB0; // 原全局：0x00F19FB0。
// 原1075A90起的20条74H记录；50F9E0生产，查找/收信/未读扫描共用。
struct MiniMailContactRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t sequence02; // 原成员偏移：+0x02。
    std::int32_t key04; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::array<char, 17> name10; // 原成员偏移：+0x10。
    std::array<char, 17> text21; // 原成员偏移：+0x21。
    std::array<char, 33> text32; // 原成员偏移：+0x32。
    std::array<std::byte, 0x21> bytes53; // 原成员偏移：+0x53。
};

const MiniMailContactRecord& ReadMiniMailContact1075A90(std::int32_t index); // 窗口21借用，存储仍归MailState。
// 窗口40借用原140H消息字节；不复制状态，也不在原字节区构造结构体对象。
struct MailMessageView {
    std::byte* unread00; // 所指原成员偏移：+0x00。
    const std::byte* kind01; // 所指原成员偏移：+0x01。
    const char* text02; // 所指原成员偏移：+0x02。
    const char* text14; // 所指原成员偏移：+0x14。
};

MailMessageView GetMailMessageF7CA08(std::int16_t selected, std::int32_t contact, std::int32_t page);
MailMessageView GetGuildMailMessageDE0B30(std::int16_t selected, std::int32_t slot, std::int32_t page);
MailMessageView GetMiniMailMessage10763A0(std::int32_t contact, std::int32_t page);
extern std::array<std::int32_t, 60> gContactIndicesFDA608; // 原全局：0x00FDA608。 数值索引，不是地址。
extern std::uint16_t gValue62B354; // 原全局：0x0062B354。 518DA0写未读类别0/1/2/3。
extern std::uint32_t gValueCB1384; // 原全局：0x00CB1384。 同源邮件页号，518C80清零。
extern std::uint32_t gValueCB0354; // 原全局：0x00CB0354。 无未读时恢复的邮件页号。
extern std::int32_t gValueCF4FC0; // 原全局：0x00CF4FC0。 收信比较的原S32编号槽，回复与后续UI共享。
extern std::int32_t gValueD046D4; // 原全局：0x00D046D4。 原邮件窗口消息位置，回复只在<9时递增。
// 原函数：0x004EC1A0。
void Sub_4EC1A0(); // 原序生成/排序60个索引，比较器相等仍为-1。
// 原函数：0x0050D740。
void Sub_50D740(std::uintptr_t unusedSocket, const char* payload); // 基础批量回复，无排序尾。
// 原函数：0x0050D8C0。
void Sub_50D8C0(std::uintptr_t unusedSocket, const char* payload); // 长文本批量回复，提前结束也排序。
// 原函数：0x0050D9E0。
void Sub_50D9E0(
    std::uintptr_t unusedSocket, std::int32_t index, const char* payload); // 单槽回复，可清同源普通邮件。
// 原函数：0x0050DC70。
void Sub_50DC70(
    std::uintptr_t unusedSocket, std::int32_t index,
    const char* payload, std::int32_t unusedFourth); // 普通收信：插入/保存/扫描/通知。
// 原函数：0x005191C0。
void Sub_5191C0(
    std::int32_t index, std::int32_t kind, const char* name, const char* text); // 九次倒序搬移后写首条。
// 原函数：0x00518DA0。
std::int32_t Sub_518DA0(); // SHP01：三类首未读U16类别/页提示，迷你倒序，无则原备用页/返0。
// 原函数：0x00518F00。
void Sub_518F00(std::int32_t* category, std::int32_t* index); // SHP01：三类定位/迷你正序，类别后索引/无则-1，父弃返。
// 原函数：0x00519090。
std::int32_t Sub_519090(std::int32_t index); // 普通未读查询，范围/used门禁后固定角色扫描十条。
// 原函数：0x005190F0。
std::int32_t Sub_5190F0(std::int32_t index); // 公会先查邮件槽再查成员状态，扫描前重读角色。
// 原函数：0x00519170。
std::int32_t Sub_519170(std::int32_t index); // 迷你未读查询，无角色分组，正序扫描十条。
// 原函数：0x00534370。
std::int32_t Sub_534370(std::int32_t key); // 20个迷你联系人，used严格1，返回首个匹配槽。
// 原函数：0x005343A0。
std::int32_t Sub_5343A0(); // 返回首个used==0的槽或-1，不预先占用。
// 原函数：0x004C9490。
void Sub_4C9490(); // 先清迷你序号，再将共享选择槽写-1。
// 原函数：0x00534520。
std::int32_t Sub_534520(char* destination); // 原time32/localtime格式化并复制时间戳，返回0。
// 原函数：0x005343C0。
void Sub_5343C0(
    std::int32_t index, std::int32_t kind, const char* name, const char* text); // 同源迷你邮件九次搬移/写首条。
// 原函数：0x00534480。
void Sub_534480(std::int32_t index); // 原20次清理，每轮两次读取共享593850，不新增索引拒绝。
// 原函数：0x0050F430。
void Sub_50F430(std::uintptr_t unusedSocket, const char* payload); // 查槽后无条件删除，包括-1。
// 原函数：0x0050F7F0。
void Sub_50F7F0(
    std::uintptr_t unusedSocket, std::int32_t key,
    const char* payload, std::int32_t unusedFourth); // 迷你收信，无文件保存。
// 原函数：0x0050F9E0。
void Sub_50F9E0(std::uintptr_t unusedSocket, const char* payload); // 完整首次生产、时间戳及两路插入。

// 原资料/迷你邮件共享生命周期；不复用94H登录记录或510H保存记录。
// 原函数：0x00533910。
void Sub_533910(); // 只清20个used，再清消息、profile双槽和当前资料。
// 原函数：0x00533950。
void Sub_533950(); // 默认首槽后复制到第二槽及所选当前资料，保留未写字段。
// 原函数：0x00533C80。
std::int32_t Sub_533C80(std::int32_t mode); // 两路profile同步；mode0发送后局部写回，mode1透传发送结果。
// 原函数：0x00534290。
std::int32_t Sub_534290(); // 先同步profile，再重取连接发送lz；第一次失败不抑制第二次。
// 原函数：0x005345A0。
std::int32_t Sub_5345A0(std::int32_t index); // 局部重置指定资料槽，再复制当前角色整条资料，返回当前S16角色。
// 原函数：0x00534770。
std::int32_t Sub_534770(); // profile.dat整块保存，打开成功即返回1。
// 原函数：0x005347F0。
std::int32_t Sub_5347F0(); // 整块读取后重取角色并复制当前资料，不按短读失败。

// 原category.dat同源三组与profile编号回复；均无场景门禁。
// 原函数：0x0050F2A0。
std::int32_t Sub_50F2A0(std::uintptr_t unusedSocket, const char* payload); // 第一/第二组生产，返回计数。
// 原函数：0x0050F3A0。
std::int32_t Sub_50F3A0(std::uintptr_t socket, const char* payload); // 写profile编号后比较旧表头，按差异补发。
// 原函数：0x0050F8E0。
std::int32_t Sub_50F8E0(std::uintptr_t unusedSocket, const char* payload); // 第三组生产，返回计数。
// 原函数：0x005348A0。
std::int32_t Sub_5348A0(); // bin/category.dat原13ED4H整块保存。
// 原函数：0x00534910。
std::int32_t Sub_534910(); // 同源整块读取，不按短读失败或清尾。

// 原49D8F0三类资料窗口的数据生产；先整串反转义，未用首参数仍保留。
// 原函数：0x004C9E40。
void Sub_4C9E40(std::int32_t unusedValue, char* payload); // 分类三组选择字节，不改used。
// 原函数：0x004CA330。
void Sub_4CA330(std::int32_t unusedValue, char* payload); // 搜索结果最多20条，仅预清used。
// 原函数：0x004CAC60。
std::int32_t Sub_4CAC60(std::int32_t unusedValue, char* payload); // 独立查看资料，返回末次图号查询。
// 原函数：0x005342F0。
std::int32_t Sub_5342F0(std::int32_t key, std::int32_t mode); // 分类第1/3组严格used==1首匹配，失败-1。
// 原函数：0x0050F450。
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
// 原函数：0x00519550。
std::int32_t Sub_519550(); // GMR01：完整两角色共享区/当前MSVC FILE，写弃返后透传关闭S32结果。

// 原 519290/5194C0：按同一 S16 所选角色清第一/第二邮件组，不保存文件，返回 0。
std::int32_t Sub_519290();
// 原函数：0x005194C0。
std::int32_t Sub_5194C0();
// 原 5192C0：清所选角色的一个普通邮件编号，不检查编号范围，返回 0。
std::int32_t Sub_5192C0(std::int32_t number);
// GML01：原5195F0，200槽active W32==1/key最后匹配，selected只取一次，无则-1。
std::int32_t Sub_5195F0(std::int32_t key);
// GML01：原5194F0，末匹配后fresh角色/active-key零写及整槽清零，返-1或0。
std::int32_t Sub_5194F0(std::int32_t key);
// GML01：原519370，查找后分支fresh角色；W32哨兵/含NUL长度或原生满组地址+4。
std::uintptr_t Sub_519370(
    std::int32_t key, std::int32_t kind, const char* name, const char* text);

// 原 50EC50 的 50EE4A–50EEA1 公共尾：全 key 比较，不检查活动位、不保存文件。
// selected 由父入口在计数后、计数发布前读取；返回最后邮件决定的零或表末游标。
std::uintptr_t PruneGuildMailAgainstOrderedRecords_50EE4A(std::int16_t selected);

} // namespace cg::world::detail
