#pragma once

namespace cg::network { class CgConnection; }
namespace cg::session { class ClientSession; }

#include <cstdint>
#include <string>

namespace cg::yobi {
struct YobiAction;
}

namespace cg::ui {

// 游戏UI实例，不是Win32窗口或Yobi；调用方只借用，不承担释放。
struct WindowInstance;

using WindowValueCallback = void (*)(WindowInstance& window, std::int32_t value);
using WindowQuantityCallback = void (*)(session::ClientSession& session, std::int32_t value);
using ElementId = std::uint32_t;
using WindowCallback = std::int32_t (*)(WindowInstance& window, std::int32_t value); // 更新/绘制显式携所属实例。
using WindowReleaseCallback = void (*)(WindowInstance& window); // 释放对象显式携owner，不借当前窗口现场。
using WindowElementCallback = std::int32_t (*)(WindowInstance& window, ElementId elementId, std::int32_t state); // 所属窗口及窗口内稳定元素ID，非排序位置。
using WindowDialogReplyCallback = std::int32_t (*)(
    session::ClientSession& session, std::int32_t button, std::int32_t row,
    const std::wstring* texts, std::uint8_t inputCount);
using WindowDialogFrameCallback = std::int32_t (*)(
    session::ClientSession& session, WindowInstance* window,
    std::int32_t button, std::int32_t row, std::int32_t events);
// 原拖放消息及来源编号不变，额外携带发起会话。
using WindowDragCallback = void (*)(
    session::ClientSession& session, std::int32_t message,
    std::int32_t flags, std::int32_t sourceIndex);

// 原生窗口、控件及 Lua MouseFlags 共用的事件位；由底层鼠标状态映射产生。
enum MouseFlags : std::uint32_t {
    MouseFlag_None = 0x000, // 无事件。
    MouseFlag_Hover = 0x001, // 悬停。
    MouseFlag_LDownEvent = 0x002, // 左键按下，驱动 onClick。
    MouseFlag_RDownEvent = 0x004, // 右键按下。
    MouseFlag_LHoldPhysical = 0x008, // 左键持续按住。
    MouseFlag_RHoldPhysical = 0x010, // 右键持续按住。
    MouseFlag_Dragging = 0x020, // 拖动。
    MouseFlag_DragFinish = 0x040, // 拖放结束。
    MouseFlag_Leave = 0x080, // 离开。
    MouseFlag_LDblClickEvent = 0x100, // 左键双击。
    MouseFlag_RDblClickEvent = 0x200, // 右键双击。
    MouseFlag_LRepeatDown = 0x400, // 左键按下及重复脉冲。
    MouseFlag_RRepeatDown = 0x800, // 右键按下及重复脉冲。
};

// 现代窗口编号；原57C130／C8B094／C9A9B0按此索引，不能用于旧选择编号。
enum WindowId : std::int32_t {
    WindowId_None = 0, // 无窗口。
    WindowId_StatusBar = 1, // 窗口1：状态栏与提示按钮。
    WindowId_CompactStatusBar = 2, // 窗口2：紧凑状态栏。
    WindowId_CollapsedStatusBar = 3, // 窗口3：折叠状态栏。
    WindowId_ShortcutBar = 4, // 窗口4：快捷交互栏。
    WindowId_CollapsedShortcutBar = 5, // 窗口5：折叠快捷交互栏。
    WindowId_BottomBar = 6, // 窗口6：底栏与悬停描述。
    WindowId_PlayerProfile = 7, // 窗口7：玩家资料。
    WindowId_PlayerAttributes = 8, // 窗口8：玩家明细。
    WindowId_PlayerTitles = 9, // 窗口9：玩家称号。
    WindowId_Skill = 10, // 窗口10：技能、技术与宠物技能。
    WindowId_Craft = 11, // 窗口11：制造与物品加工。
    WindowId_Gather = 12, // 窗口12：窗口12操作与结果。
    WindowId_SkillResult = 13, // 窗口13：窗口13操作与结果。
    WindowId_Inventory = 14, // 窗口14：库存与装备。
    WindowId_PetList = 15, // 窗口15：宠物列表。
    WindowId_PetDetail = 16, // 窗口16：宠物详情。
    WindowId_PetAttributes = 17, // 窗口17：宠物详细属性。
    WindowId_PetSkills = 18, // 窗口18：宠物技能详情。
    WindowId_AlbumList = 19, // 窗口19：图鉴列表。
    WindowId_AlbumDetail = 20, // 窗口20：图鉴详情。
    WindowId_Contacts = 21, // 窗口21：联系人与邮件列表。
    WindowId_MailCompose = 22, // 窗口22：发信。
    WindowId_PetMailCompose = 23, // 窗口23：宠物邮件。
    WindowId_Settings = 24, // 窗口24：设置。
    WindowId_Hotkeys = 25, // 窗口25：快捷键列表与预设文本编辑。
    WindowId_BattleCommand = 26, // 窗口26：战斗命令。
    WindowId_BattleItems = 27, // 窗口27：战斗物品。
    WindowId_BattlePets = 28, // 窗口28：战斗宠物切换。
    WindowId_Spectator = 29, // 窗口29：观战结束。
    WindowId_BattleResult = 30, // 窗口30：战斗结果。
    WindowId_DpResult = 31, // 窗口31：DP结果。
    WindowId_BattleStateNotice = 32, // 窗口32运行期明细。
    WindowId_MiniMap = 33, // 窗口33：小地图。
    WindowId_Emote = 34, // 窗口34：情绪动作。
    WindowId_Chat = 35, // 窗口35：可调聊天窗口。
    WindowId_Dialog = 36, // 窗口36：通用对话共享实例。
    WindowId_DialogCreate = 37, // 窗口37：通用对话创建入口。
    WindowId_ItemStorage = 38, // 窗口38：外部物品存放。
    WindowId_TimerNotice = 39, // 窗口39：计时提示。
    WindowId_MailRead = 40, // 窗口40：读信。
    WindowId_Bank = 41, // 窗口41：物品与宠物存放。
    WindowId_Quantity = 42, // 窗口42：数量输入。
    WindowId_Trade = 43, // 窗口43：交易。
    WindowId_TradeTarget = 44, // 窗口44：交易目标选择。
    WindowId_TextTarget = 45, // 窗口45：文本目标选择。
    WindowId_AttributeTarget = 46, // 窗口46：属性目标选择。
    WindowId_Confirmation = 47, // 窗口47：通用确认。
    WindowId_GuildDetails = 48, // 窗口48：公会详情与职级编辑。
    WindowId_GroupSelect = 49, // 窗口49：群组选人。
    WindowId_MessageBoard = 50, // 窗口50：四记录对话。
    WindowId_TextList = 51, // 窗口51：文本列表。
    WindowId_ShopChoice = 52, // 窗口52：商店选择。
    WindowId_Shop = 53, // 窗口53：商品买卖。
    WindowId_PetSkillChoice = 54, // 窗口54：宠物与技能选择。
    WindowId_GuildRank = 55, // 窗口55：公会职级选择。
    WindowId_Appearance = 56, // 窗口56：外观选择。
    WindowId_CompactChat = 57, // 窗口57：简化聊天窗口。
    WindowId_OwnProfile = 58, // 窗口58：自身资料。
    WindowId_OwnProfileCategory = 59, // 窗口59：自身资料分类选择。
    WindowId_ProfileCategory = 60, // 窗口60：资料分类选择。
    WindowId_ProfileSearch = 61, // 窗口61：资料搜索结果。
    WindowId_ProfileView = 62, // 窗口62：查看资料。
    WindowId_MiniMail = 63, // 窗口63：迷你发信。
    WindowId_ProfileReply = 64, // 窗口64：资料回复展示。
    WindowId_Blacklist = 65, // 窗口65：黑名单管理。
    WindowId_AdventureLog = 66, // 窗口66：冒险者日志。
    WindowId_OwnStall = 67, // 窗口67：自身摊位。
    WindowId_StallBrowse = 68, // 窗口68：浏览摊位。
    WindowId_CrystalShop = 69, // 窗口69：晶石商店69。
    WindowId_CrystalExchangeShop = 70, // 窗口70：晶石商店70。
    WindowId_BigMap = 71, // BMWP01：大地图，仅运行期窗口，不写入旧窗口存档。
    WindowId_PetEquipment = 72, // 宠物装备页，与详情三页共用当前宠物；仅运行期窗口。
};
static_assert(sizeof(WindowId) == sizeof(std::int32_t));

// R02：仅接收原画布坐标；按最近边缘保留有符号边距，等距选左／上。
std::int32_t AdaptWindowAxisPosition(
    std::int32_t position, std::int32_t size, std::int32_t baseSize, std::int32_t targetSize);
// 原640×480矩形中心的两轴均在±50内才整体居中，否则两轴各自贴近最近边缘。
// 只适配一次，不能传入已转换的当前画布坐标。
void AdaptWindowPosition(
    std::int32_t& x, std::int32_t& y, std::int32_t width, std::int32_t height);
void AdaptWindowPosition(
    std::int16_t& x, std::int16_t& y, std::int32_t width, std::int32_t height);
// 按初始化前的原型矩形决定是否居中，且当前位置两轴距适配后位置均须在±10内。
// 非中心窗口或任一轴超出容差时保留当前位置，返回false。
// 动态对话的零宽原型不参与居中，实际矩形在WindowDialog_Open中适配。
bool CenterPopupWindowPosition(std::int32_t id, std::int32_t width, std::int32_t height,
    std::int32_t& x, std::int32_t& y);
// 首次窗口初始化按已发布客户区尺寸调整原型和当前坐标，支持只指定一个轴。
void InitializeWindowLayout();

// G07：INVCF发布背包／银行页数及0基背包页；新增按钮在下次开窗时创建。
void WindowRuntime_SetInventoryPages(session::ClientSession& session,
    std::int32_t bagPageCount, std::int32_t bankPageCount, std::int32_t bagCurrentPage);

// WPL01：完整创建/载荷owner合同，见doc/cg2d_window_payload_creation_spec.md。
// 原函数：0x00471440。
WindowInstance* WindowRuntime_Open(std::int32_t id); // 完整七类创建；原型表绑定仍是外部依赖。
WindowInstance* WindowRuntime_Open(session::ClientSession& session, std::int32_t id);
void WindowBigMap_Open(); // 小地图按钮唯一入口，复用原生原型与单实例生命周期。
// 原函数：0x005342E0。
WindowInstance* WindowProfileReply_Open(); // DpSY回复打开窗口64，透传借用实例指针。
// 原函数：0x00475AA0。
cg::yobi::YobiAction* WindowRuntime_CreateAnimation(
    std::int32_t id, std::uint32_t flags, std::uint32_t mode,
    cg::yobi::YobiAction** completionSlot); // 完成槽只借用；已有实例仅置关闭标记。
cg::yobi::YobiAction* WindowRuntime_CreateAnimation(
    session::ClientSession& session, std::int32_t id, std::uint32_t flags, std::uint32_t mode,
    cg::yobi::YobiAction** completionSlot);

// 原函数：0x004CD3E0。
WindowInstance* WindowStallBrowse_RequestClose(session::ClientSession& session); // 原窗口68的state置0x8000，返回原借用引用。
// 原函数：0x004CD400。
WindowInstance* WindowOwnStall_RequestClose(session::ClientSession& session); // 原窗口67，同上；不直接关闭或清表。
// 原函数：0x004AFF00。
std::int32_t WindowGuildDetails_RequestClose(); // 借用窗口48置关闭标记，空返回0，否则透传音效结果。
std::uint8_t WindowRuntime_ReadUiGate() noexcept; // 共享UI门禁W8，world只读。
std::uint8_t WindowRuntime_ReadObservationFlag(session::ClientSession& session) noexcept; // 技能回复与观察进入共用的UI标志。
// 原数据：0x00584DA4。
std::int32_t WindowItemDrag_ReadIndex() noexcept; // 原S32拖动编号，初值-1。
// 原函数：0x004A80E0。
std::int32_t WindowChat_ReadInputByteWidth(session::ClientSession& session); // 原C9A930两倍，供历史翻页计算原字节滚动量。 // HBN01：见历史浏览/字节滚动完整合同。

// 原函数：0x00471270。
void WindowRuntime_Free(WindowInstance* window); // release回调、记录回存、七类元素释放。
// 原函数：0x00472160。
void WindowRuntime_Close(WindowInstance* window); // WCR01：同记录地址发布/置bit1再释放，不清表槽。
// 原函数：0x00472190。
std::int32_t WindowRuntime_CloseAll(session::ClientSession& session); // 完整关闭与观察退出请求，返回0或发送结果。
// 原函数：0x00503E80。
void WindowRuntime_ProcessScene(session::ClientSession& session); // 场景切换的标记、关闭、保存和三路重开；原caller弃返回。
// 原函数：0x00473F80。
std::int32_t WindowRuntime_Update(session::ClientSession& session); // 完整鼠标/窗口/快捷键/预设输入父循环，返回1。
// 原函数：0x00504120。
std::int32_t WindowRuntime_Render(session::ClientSession& session); // 转生计时到期发送/清标志后，透传完整窗口更新返回。
// 原函数：0x00472240。
void WindowRuntime_CloseByCategory(std::uint32_t mask); // 按原71槽顺序关闭并清槽。
void WindowRuntime_CloseByCategory(session::ClientSession& session, std::uint32_t mask);
// DLP01：非零phase才取消，可选message1/4后重取phase；八槽清理末按该样本进入2/6。
// 原函数：0x00476410。
void WindowDrag_Cancel(session::ClientSession& session); // 回调后重读阶段，最后进入2或6，不等同通用重置。
// 原代码位置：0x00440ADB。
void WindowRuntime_ApplyObservationInputGate(session::ClientSession& session); // 同源5F9754及D67B08前八个U8的观察进入写序。
// 原CRT 553098入口仅尾跳既有初始化，无退出登记。
// 原函数：0x00477160。
void WindowNumber_InitializeLegacySpacing();
// WND01：477160尾跳的四S32步进生产，见doc/cg2d_window_number_draw_spec.md。
// 原函数：0x00477170。
void WindowNumber_InitializeSpacing(); // 477160尾跳的四种数字图片步进业务初始化。
// 原CRT 55309C/5530A0：两启动入口分别创建固定静态预览，退出由实际对象自动逆序析构。
// 启动类型只负责原位置调用；登记与异常规则由MSVC承接。
// 原函数：0x004883F0。
class FirstItemSelectorStartup final {
public:
    // 原函数：0x004883F0；只接管同组原始载荷的启动寿命。
    FirstItemSelectorStartup();
    FirstItemSelectorStartup(const FirstItemSelectorStartup&) = delete;
    FirstItemSelectorStartup& operator=(const FirstItemSelectorStartup&) = delete;
};

// 原函数：0x004998F0。
class SecondItemSelectorStartup final {
public:
    // 原函数：0x004998F0；只接管同组原始载荷的启动寿命。
    SecondItemSelectorStartup();
    SecondItemSelectorStartup(const SecondItemSelectorStartup&) = delete;
    SecondItemSelectorStartup& operator=(const SecondItemSelectorStartup&) = delete;
};
// 对应原CRT 5530A4；浏览宠物拥有者接管同组构析。
// 原函数：0x004CD240。
class StallBrowsePetValuesStartup final {
public:
    // 原函数：0x004CD240；只接管同组原始载荷的启动寿命。
    StallBrowsePetValuesStartup();
    StallBrowsePetValuesStartup(const StallBrowsePetValuesStartup&) = delete;
    StallBrowsePetValuesStartup& operator=(const StallBrowsePetValuesStartup&) = delete;
};
void WindowStall_ConstructBrowsePetValues(); // 原CRT 5530A4，浏览区五宠物各六CryptInt顺序构造。
// WDP01：完整分发与字段取样合同，见doc/cg2d_window_element_draw_pipeline_spec.md。
// 原函数：0x004729B0。
void WindowRuntime_DrawCurrent(WindowInstance& window); // 严格visible==1的七类分派，尾部清文本层3并绘范围提示。

// 对话BZ回复：三重门禁、关闭/完整路由及对象朝向，socket保持未使用实参。
// 原函数：0x0050E110。
void WindowDialog_ReceiveReply(cg::network::CgConnection& networkConnection, std::int32_t type, std::int32_t value,
    std::int32_t request, std::int32_t target, char* text);

// 资料DyAG回复：三重门禁后完整38/39/40路由，无BZ关闭特例或对象朝向尾。
// 原函数：0x0050F990。
void WindowProfile_ReceiveReply(cg::network::CgConnection& networkConnection, std::int32_t type, std::int32_t value,
    std::int32_t request, std::int32_t target, char* text);

// 摆摊开/收回复，无场景门禁，原返回地址均被dispatcher丢弃。
// 原函数：0x0050E7D0。
void WindowStall_ReceiveOpenReply(cg::network::CgConnection& networkConnection, std::int32_t result);
// 原函数：0x0050E7E0。
void WindowStall_ReceiveCloseReply(cg::network::CgConnection& networkConnection, std::int32_t result);
// 原函数：0x0050E7F0。
void WindowStall_ReceiveBrowseReply(cg::network::CgConnection& networkConnection, std::int32_t status, std::int32_t target,
    const char* stallName, const char* description,
    const char* itemPayload, const char* petPayload); // ft完整浏览生产，无场景门禁。
// 原函数：0x0050E820。
void WindowStall_ReceivePurchaseReply(cg::network::CgConnection& networkConnection, std::int32_t result); // 购买结果，无额外门禁。

// LY物品使用名单：仅一次CryptInt门禁，完整生产自己/地图五槽/服务器目标后打开45。
// 原函数：0x0050E380。
void WindowItemUse_ReceiveTextTargetReply(cg::network::CgConnection& networkConnection, const char* text);
// 原函数：0x0050E3A0。
void WindowItemUse_ReceiveAttributeTargetReply(cg::network::CgConnection& networkConnection, const char* text); // nFV完整自身/远端属性生产后打开46。
// 原函数：0x0050E340。
void WindowSkill_ReceiveTextTargetReply(cg::network::CgConnection& networkConnection, const char* text); // ysVc技能目标名单及45选择回调。
// 原函数：0x0050E360。
void WindowSkill_ReceiveAttributeTargetReply(cg::network::CgConnection& networkConnection, const char* text); // ND技能属性列表及46选择回调。
// 原函数：0x0050C7F0。
void WindowWorkResult_ReceiveReply(cg::network::CgConnection& networkConnection, std::int32_t result,
    std::int32_t kind, const char* text); // JgO四类结果，场景门禁后仅一次Ripple读取。

// 交易回复：按场景门禁后单次CryptInt门禁，socket由原dispatcher传入但不消费。
// 原函数：0x0050E3C0。
void WindowTrade_ReceiveTargetReply(cg::network::CgConnection& networkConnection, const char* text); // 名单解析/回调生产与窗口44展开。
// 原函数：0x0050E3F0。
void WindowTrade_ReceiveStartReply(cg::network::CgConnection& networkConnection, char* text, std::int32_t unusedValue); // 特殊状态发送拒绝；正常路径原位反转义。
// 原函数：0x0050E440。
std::int32_t WindowTrade_ReceiveCancelReply(cg::network::CgConnection& networkConnection);
// 原函数：0x0050E470。
std::int32_t WindowTrade_ReceiveStateReply(cg::network::CgConnection& networkConnection, std::int32_t value);
// 原函数：0x0050E4A0。
std::int32_t WindowTrade_ReceiveItemReply(cg::network::CgConnection& networkConnection, const char* text);
// 原函数：0x0050E4D0。
std::int32_t WindowTrade_ReceivePetReply(cg::network::CgConnection& networkConnection, std::int32_t index, const char* text);
// 原函数：0x0050E500。
std::int32_t WindowTrade_ReceivePetSkillReply(cg::network::CgConnection& networkConnection, std::int32_t index, const char* text);
// 原函数：0x0050E530。
std::int32_t WindowTrade_ReceiveRemoteResetReply(cg::network::CgConnection& networkConnection); // 清第0组，返回D2E2B0改写前值。
// 原函数：0x0050E570。
void WindowTrade_ReceiveCloseReply(cg::network::CgConnection& networkConnection, std::int32_t value); // 关闭只置标志，弃原混合返回。

// 读取原生拖动阶段，供脚本控件判断当前鼠标操作。
int WindowDrag_ReadMousePhase(session::ClientSession& session);
// 消费阶段4的拖放回调，并按原顺序重置鼠标拖动状态。
void WindowDrag_ConsumeDrop(session::ClientSession& session);

} // namespace cg::ui
