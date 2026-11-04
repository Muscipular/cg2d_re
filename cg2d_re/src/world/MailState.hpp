#pragma once

#include <cstdint>

namespace cg::session { class ClientSession; }

namespace cg::network {
class CgConnection;
enum class SendResult : std::int32_t;
}

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>

namespace cg::world {

// 窗口40借用同源140H消息成员；仅未读位可写，其余保持只读。
struct MailMessageView {
    std::byte* unread00; // 所指原成员偏移：+0x00。
    const std::byte* kind01; // 所指原成员偏移：+0x01。
    const wchar_t* text02; // 所指原成员偏移：+0x02。
    const wchar_t* text14; // 所指原成员偏移：+0x14。
};

MailMessageView GetMailMessageF7CA08(session::ClientSession& session, std::int16_t selected, std::int32_t contact, std::int32_t page);
MailMessageView GetGuildMailMessageDE0B30(session::ClientSession& session, std::int16_t selected, std::int32_t slot, std::int32_t page);
MailMessageView GetMiniMailMessage10763A0(session::ClientSession& session, std::int32_t contact, std::int32_t page);
} // namespace cg::world

namespace cg::world::detail {

// 原三项108H资料及828H资料记录；只共享布局，持久/当前/查看/回复区各有唯一存储。
struct ProfileEntry {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::wstring text08; // 原成员偏移：+0x08。
};

struct ProfileRuntimeRecord {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::wstring text08; // 原成员偏移：+0x08。
    std::int32_t value108; // 原成员偏移：+0x108。
    std::int32_t value10C; // 原成员偏移：+0x10C。
    std::wstring text110; // 原成员偏移：+0x110。
    std::wstring text210; // 原成员偏移：+0x210。
    std::wstring text310; // 原成员偏移：+0x310。
    std::array<ProfileEntry, 3> entries410; // 原成员偏移：+0x410。
    std::wstring text728; // 原成员偏移：+0x728。
};

// 资料解析／保存与窗口共用原唯一对象；读取者保留const引用。
const ProfileRuntimeRecord& ReadSelectedProfile1074938(session::ClientSession& session); // 每次按当前S16角色借用持久资料。
// 原函数：0x00533B00。
std::int32_t HasProfileDraftChanges_533B00(session::ClientSession& session); // 固定一次角色，三数值/三文本/正文按原顺序短路比较。
std::int32_t ReadCategoryValue105FA20(session::ClientSession& session); // 窗口62/63借用category回复费用，不复制owner。
std::int32_t& SelectedProfileValue04(session::ClientSession& session); // 每次按当前S16角色借用持久资料公开标志，不缓存跨发送身份。

// 原4CA330生产的20条20CH搜索记录，窗口61只读借用。
struct ProfileSearchRecord {
    std::int32_t used; // 原成员偏移：+0x00。
    std::wstring text04; // 原成员偏移：+0x04。
    std::int32_t value104; // 原成员偏移：+0x104。
    std::int32_t value108; // 原成员偏移：+0x108。
    std::wstring text10C; // 原成员偏移：+0x10C。
};

struct ProfileSearchRuntimeSlots {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::array<ProfileSearchRecord, 20> records; // 原成员偏移：+0x08。
};

const wchar_t* ReadCategoryText105FA24(session::ClientSession& session, std::int32_t group, std::int32_t index); // 三组标题借用，无额外索引拒绝。

// 原category.dat三组各100条；窗口60只读借用，存储仍归MailState。
struct CategoryRecord {
    std::uint8_t used; // 原成员偏移：+0x00。
    std::array<std::byte, 3> bytes01; // 原成员偏移：+0x01。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::wstring text0C; // 原成员偏移：+0x0C。
    std::uint8_t selected10C; // 原成员偏移：+0x10C。 4C9E40写选择位，与used独立。
    std::array<std::byte, 3> bytes10D; // 原成员偏移：+0x10D。
};

const CategoryRecord& ReadCategoryRecord105FA24(session::ClientSession& session, std::int32_t group, std::int32_t index);
std::int32_t ReadCategoryCount105FA18(session::ClientSession& session); // 类别0/1共用count08，每次读取真实回复状态。
std::int32_t ReadCategoryCount105FA1C(session::ClientSession& session); // 类别2的count0C，与前两类独立。

// 原F19FB0的74H联系人记录，三路回复生产，普通邮件与窗口共用。
struct ContactRuntimeRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::int16_t value02; // 原成员偏移：+0x02。 排序按S16比较，回复保留低W16位型。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::wstring name10; // 原成员偏移：+0x10。
    std::wstring text21; // 原成员偏移：+0x21。
    std::wstring text32; // 原成员偏移：+0x32。
    std::wstring text53; // 原成员偏移：+0x53。
};

// 原1075A90起的20条74H记录；50F9E0生产，查找/收信/未读扫描共用。
struct MiniMailContactRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t sequence02; // 原成员偏移：+0x02。
    std::int32_t key04; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::wstring name10; // 原成员偏移：+0x10。
    std::wstring text21; // 原成员偏移：+0x21。
    std::wstring text32; // 原成员偏移：+0x32。
    std::array<std::byte, 0x21> bytes53; // 原成员偏移：+0x53。
};

const MiniMailContactRecord& ReadMiniMailContact1075A90(session::ClientSession& session, std::int32_t index); // 窗口21借用，存储仍归MailState。
// 原函数：0x004EC1A0。
void SortContactIndices_4EC1A0(session::ClientSession& session); // 原序生成/排序60个索引，比较器相等仍为-1。
// 原函数：0x0050D740。
void HandleContactListReply_50D740(cg::network::CgConnection& networkConnection, const char* payload); // 基础批量回复，无排序尾。
// 原函数：0x0050D8C0。
void HandleContactExtendedTextReply_50D8C0(cg::network::CgConnection& networkConnection, const char* payload); // 长文本批量回复，提前结束也排序。
// 原函数：0x0050D9E0。
void HandleContactUpdateReply_50D9E0(cg::network::CgConnection& networkConnection, std::int32_t index, const char* payload); // 单槽回复，可清同源普通邮件。
// 原函数：0x0050DC70。
void HandleContactMailReply_50DC70(cg::network::CgConnection& networkConnection, std::int32_t index,
    const char* payload, std::int32_t unusedFourth); // 普通收信：插入/保存/扫描/通知。
// 原函数：0x005191C0。
void PrependContactMailMessage_5191C0(session::ClientSession& session,
    std::int32_t index, std::int32_t kind, std::wstring_view name, std::wstring_view text); // 九次倒序搬移后写首条。
// 原函数：0x00518DA0。
std::int32_t UpdateUnreadMailPageHint_518DA0(session::ClientSession& session); // SHP01：三类首未读U16类别/页提示，迷你倒序，无则原备用页/返0。
// 原函数：0x00518F00。
void FindFirstUnreadMailContact_518F00(session::ClientSession& session, std::int32_t* category, std::int32_t* index); // SHP01：三类定位/迷你正序，类别后索引/无则-1，父弃返。
// 原函数：0x00519090。
std::int32_t HasUnreadContactMail_519090(session::ClientSession& session, std::int32_t index); // 普通未读查询，范围/used门禁后固定角色扫描十条。
// 原函数：0x005190F0。
std::int32_t HasUnreadGuildMail_5190F0(session::ClientSession& session, std::int32_t index); // 公会先查邮件槽再查成员状态，扫描前重读角色。
// 原函数：0x00519170。
std::int32_t HasUnreadMiniMail_519170(session::ClientSession& session, std::int32_t index); // 迷你未读查询，无角色分组，正序扫描十条。
// 原函数：0x00534370。
std::int32_t FindMiniMailContactByKey_534370(session::ClientSession& session, std::int32_t key); // 20个迷你联系人，used严格1，返回首个匹配槽。
// 原函数：0x005343A0。
std::int32_t FindFreeMiniMailContactSlot_5343A0(session::ClientSession& session); // 返回首个used==0的槽或-1，不预先占用。
// 原函数：0x004C9490。
void ResetMiniMailSequenceAndSelection_4C9490(session::ClientSession& session); // 先清迷你序号，再将共享选择槽写-1。
// 原函数：0x00534520。
std::int32_t FormatMailTimestamp_534520(session::ClientSession& session, std::wstring& destination); // 原time32/localtime格式化并复制时间戳，返回0。
// 原函数：0x005343C0。
void PrependMiniMailMessage_5343C0(session::ClientSession& session,
    std::int32_t index, std::int32_t kind, std::wstring_view name, std::wstring_view text); // 同源迷你邮件九次搬移/写首条。
// 原函数：0x00534480。
void RemoveMiniMailContact_534480(session::ClientSession& session, std::int32_t index); // 保留最多20条清理，负索引或超出存储范围不访问。
// 原函数：0x0050F430。
void HandleMiniMailContactRemovalReply_50F430(cg::network::CgConnection& networkConnection, const char* payload); // 查槽后删除，-1不访问记录。
// 原函数：0x0050F7F0。
void HandleMiniMailReply_50F7F0(cg::network::CgConnection& networkConnection, std::int32_t key,
    const char* payload, std::int32_t unusedFourth); // 迷你收信，无文件保存。
// 原函数：0x0050F9E0。
void HandleMiniMailConversationReply_50F9E0(cg::network::CgConnection& networkConnection, const char* payload); // 完整首次生产、时间戳及两路插入。

// 原资料/迷你邮件共享生命周期；不复用94H登录记录或510H保存记录。
// 原函数：0x00533910。
void ClearMiniMailAndProfiles_533910(session::ClientSession& session); // 只清20个used，再清消息、profile双槽和当前资料。
// 原函数：0x00533950。
void InitializeDefaultProfiles_533950(session::ClientSession& session); // 默认首槽后复制到第二槽及所选当前资料，保留未写字段。
// 原函数：0x00533C80。
std::int32_t PublishPlayerProfile_533C80(session::ClientSession& session, std::int32_t mode); // 两路profile同步；mode0发送后局部写回，mode1透传发送结果。
// 原函数：0x00534290。
network::SendResult ResendSavedPlayerProfile_534290(session::ClientSession& session); // 先同步profile，再重取连接发送lz；第一次失败不抑制第二次。
// 原函数：0x005345A0。
std::int32_t ResetCharacterProfile_5345A0(session::ClientSession& session, std::int32_t index); // 局部重置指定资料槽，再复制当前角色整条资料，返回当前S16角色。
// 原函数：0x00534770。
std::int32_t SavePlayerProfilesFile_534770(session::ClientSession& session); // profile.dat整块保存，打开成功即返回1。
// 原函数：0x005347F0。
std::int32_t LoadPlayerProfilesFile_5347F0(session::ClientSession& session); // 整块读取后重取角色并复制当前资料，不按短读失败。

// 原category.dat同源三组与profile编号回复；均无场景门禁。
// 原函数：0x0050F2A0。
std::int32_t HandlePrimaryProfileCategoriesReply_50F2A0(cg::network::CgConnection& networkConnection, const char* payload); // 第一/第二组生产，返回计数。
// 原函数：0x0050F3A0。
std::int32_t HandleProfileCategoryVersionsReply_50F3A0(cg::network::CgConnection& networkConnection, const char* payload); // 写profile编号后比较旧表头，按差异补发。
// 原函数：0x0050F8E0。
std::int32_t HandleSecondaryProfileCategoriesReply_50F8E0(cg::network::CgConnection& networkConnection, const char* payload); // 第三组生产，返回计数。
// 原函数：0x005348A0。
std::int32_t SaveProfileCategoryFile_5348A0(session::ClientSession& session); // bin/category.dat原13ED4H整块保存。
// 原函数：0x00534910。
std::int32_t LoadProfileCategoryFile_534910(session::ClientSession& session); // 同源整块读取，不按短读失败或清尾。

// 原49D8F0三类资料窗口的数据生产；先整串反转义，未用首参数仍保留。
// 原函数：0x004C9E40。
void HandleProfileCategorySelectionReply_4C9E40(session::ClientSession& session, std::int32_t unusedValue, char* payload); // 分类三组选择字节，不改used。
// 原函数：0x004CA330。
void HandleProfileSearchResultsReply_4CA330(session::ClientSession& session, std::int32_t unusedValue, char* payload); // 搜索结果最多20条，仅预清used。
// 原函数：0x004CAC60。
std::int32_t HandleViewedPlayerProfileReply_4CAC60(session::ClientSession& session, std::int32_t unusedValue, char* payload); // 独立查看资料，返回末次图号查询。
// 原函数：0x005342F0。
std::int32_t FindProfileCategoryImage_5342F0(session::ClientSession& session, std::int32_t key, std::int32_t mode); // 分类第1/3组严格used==1首匹配，失败-1。
// 原函数：0x0050F450。
void HandlePlayerProfileReply_50F450(cg::network::CgConnection& networkConnection, const char* text1, std::int32_t value2,
    const char* text3, const char* text4, const char* text5, std::int32_t value6,
    std::int32_t value7, const char* text8, std::int32_t value9,
    const char* text10, std::int32_t value11, const char* text12,
    const char* text13); // 完整资料回复，无门禁，八独立文本副本后打开64。

// 原 518C80：依次清两段邮件持久区及 CB1384，返回 0。
std::int32_t ClearContactAndGuildMailStorage_518C80(session::ClientSession& session);
// 原 [518CB0,518D94)：依次读两文件，返回 fclose 或缺失文件 helper 的整数结果。
std::int32_t LoadContactAndGuildMailFiles_518CB0(session::ClientSession& session);
// 原 [519300,51936C) 与 [519550,5195BC)：各自重取保存路径，写唯一共享缓冲。
std::int32_t SaveContactMailFile_519300(session::ClientSession& session);
// 原函数：0x00519550。
std::int32_t SaveGuildMailFile_519550(session::ClientSession& session); // GMR01：完整两角色共享区/当前MSVC FILE，写弃返后透传关闭S32结果。

// 原 519290/5194C0：按同一 S16 所选角色清第一/第二邮件组，不保存文件，返回 0。
std::int32_t ClearCurrentCharacterContactMail_519290(session::ClientSession& session);
// 原函数：0x005194C0。
std::int32_t ClearCurrentCharacterGuildMail_5194C0(session::ClientSession& session);
// 原 5192C0：清所选角色的一个普通邮件编号，不检查编号范围，返回 0。
std::int32_t ClearContactMailMessages_5192C0(session::ClientSession& session, std::int32_t number);
// GML01：原5195F0，200槽active W32==1/key最后匹配，selected只取一次，无则-1。
std::int32_t FindLastActiveGuildMailEntry_5195F0(session::ClientSession& session, std::int32_t key);
// GML01：原5194F0，末匹配后fresh角色/active-key零写及整槽清零，返-1或0。
std::int32_t DeleteGuildMailEntry_5194F0(session::ClientSession& session, std::int32_t key);
// GML01：原519370，查找后分支fresh角色；W32哨兵/含NUL长度或原生满组地址+4。
std::uintptr_t PrependGuildMailMessage_519370(session::ClientSession& session,
    std::int32_t key, std::int32_t kind, std::wstring_view name, std::wstring_view text);

// 原 50EC50 的 50EE4A–50EEA1 公共尾：全 key 比较，不检查活动位、不保存文件。
// selected 由父入口在计数后、计数发布前读取；返回最后邮件决定的零或表末游标。
std::uintptr_t PruneGuildMailAgainstOrderedRecords_50EE4A(session::ClientSession& session, std::int16_t selected);

// 三类收信仍共用完整运行值类型；磁盘布局只在原存取实现中转换。
struct MailMessage {
    std::byte unread; // +00：未读标记，窗口可清零。
    std::byte kind; // +01：收信类别低字节。
    std::wstring name; // 原+02：完整日期文本。
    std::wstring text; // 原+14：内部邮件正文。
};
struct GuildMailEntry {
    std::int32_t key; // +00：公会成员键。
    std::uint32_t active; // +04：查找要求恰为1，未读扫描只判断非零。
    std::array<MailMessage, 10> messages; // +08：同一成员的十条消息。
};

struct MiniMailRuntimeSlots {
    std::array<MiniMailContactRecord, 20> contacts;
    std::array<MailMessage, 20 * 10> messages; // 原10763A0，20组各十条140H；保原连续访问。
};

struct CategoryRuntimeSlots {
    std::int32_t value00; // 原成员偏移：+0x00。 原105FA10，50F2A0发布、50F3A0比较。
    std::int32_t value04; // 原成员偏移：+0x04。 原105FA14，50F8E0发布、50F3A0比较。
    std::int32_t count08; // 原成员偏移：+0x08。
    std::int32_t count0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。 原105FA20，50F3A0第三字段。
    std::array<CategoryRecord, 300> records; // 原105FA24/1066464/106CEA4三组同源区。
};

// 同会话的资料展示/草稿、三类邮件和未读窗口共用这些实际记录。
struct MailRuntimeData {
    ProfileRuntimeRecord currentProfile {}; // 原10738E8。
    ProfileRuntimeRecord receivedProfile {}; // 原1074110。
    ProfileRuntimeRecord viewedProfile {}; // 原105ECE0。
    ProfileSearchRuntimeSlots profileSearch {}; // 原105C398。
    std::array<ContactRuntimeRecord, 60> contacts {}; // 原F19FB0。
    std::array<std::int32_t, 60> contactIndices {}; // 原FDA608。
    std::uint16_t unreadCategory = 0; // 原62B354。
    std::uint32_t contactPage = 0; // 原CB1384。
    std::uint32_t fallbackPage = 0; // 原CB0354。
    std::int32_t viewedContactId = 0; // 原CF4FC0。
    std::int32_t viewedMessageIndex = 0; // 原D046D4。
    std::int32_t miniMailSelection = 0; // 原D673E4，重置入口写-1。
    std::array<MailMessage, 2 * 60 * 10> messages {}; // 原F7CA08，保留跨联系人连续步进。
    std::array<GuildMailEntry, 2 * 200> guildMessages {}; // 原DE0B30。
    MiniMailRuntimeSlots miniMail {};
    std::uint32_t miniMailSequence = 0; // 原1075A88，先发布低W16再递增。
    std::array<ProfileRuntimeRecord, 2> profiles {}; // 原1074938。
    CategoryRuntimeSlots category {}; // 原105FA10。
};
static_assert(sizeof(MailRuntimeData::contactIndices) == 0xF0);

} // namespace cg::world::detail
