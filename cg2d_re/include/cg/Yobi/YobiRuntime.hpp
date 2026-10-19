#pragma once

#include <cstdint>

namespace cg::platform {
class LegacyCryptInt;
}

namespace cg::render {
struct GraphicDescriptor16;
struct GraphicCacheItem32;
}

namespace cg::yobi {

// 脚本运行时启动／关闭战斗特效配置与独立Lua技能状态。
void InitializeBattleEffects();
void ShutdownBattleEffects();

// 原 x86 YobiAction 的原生 x64 承载；具体布局只由 Yobi runtime 持有。
// runtime/list 是唯一 owner，场景中的 YobiAction* 均为非拥有观察者。
struct YobiAction;

// 本机载荷类别；与YobiPayload的有效指针成员对应，不写入协议或原端数据。
enum class YobiPayloadKind : std::uint8_t {
    None,
    Raw,
    BattleActor,
    BattleScene,
    BattleRideCarrier,
    BattleRingEmitter,
    BattleFollowOffset,
    BattleTransitionEffect,
    BattleText,
    MapActor,
    MapChara,
    MapTransition,
    WindowOpening,
    WindowUi,
};



// 原46DD40：清除地图注册标记并给对应动作标死。
void Sub_46DD40();


// 原 49BE00：复用既有九宫格入队入口，不复制图形记录或绘制算法。
void DrawNineGrid_49BE00(
    std::int32_t baseImageNo,
    std::int16_t x,
    std::int16_t y,
    std::int16_t width,
    std::int16_t height,
    std::uint8_t layer,
    std::int32_t borderColor,
    std::int32_t centerColor,
    std::int16_t tileWidth,
    std::int16_t tileHeight);

// 原 dword_59E570 与 dword_59E574；后者由 UpdateAll 在 [0, 3] 间循环。
extern std::int32_t gYobiUpdateGateMode; // 原全局：0x0059E570。
extern std::int32_t gYobiUpdatePhaseCounter; // 原全局：0x0059E574。

// 原 0x59F9F4/0x59F9FC/0x59FA00/0x59FA04/0x553C58；
// volatile 保留 sub_423640 的独立 W32 读取及越界值、钳位值两次写入。
extern volatile std::uint32_t gYobiScreenOffsetRising; // 原全局：0x0059F9F4。
extern volatile std::uint32_t gYobiScreenOffsetX; // 原全局：0x0059F9FC。
extern volatile std::uint32_t gYobiScreenOffsetY; // 原全局：0x0059FA00。
extern volatile std::uint32_t gYobiScreenOffsetAmplitude; // 原全局：0x0059FA04。
extern volatile std::uint32_t gYobiScreenOffsetMaximumAmplitude; // 原全局：0x00553C58。

// 原 sub_404A40@[0x404A40,0x404AD1)；初始化首尾哨兵并返回尾哨兵。
// 与原端相同，调用前提是尚未初始化，且不注册进程退出析构。
YobiAction* InitializeYobiActionList_404A40();

// 原 YobiAction::new@[0x404AE0,0x404C42)。priority 的有效范围为 [0, 0xFE]。
// payloadKind只标记本机指针类型；默认Raw保留原按字节分配的调用形式。
YobiAction* NewYobiAction_404AE0(
    std::uint8_t priority,
    std::uint32_t payloadSize,
    YobiPayloadKind payloadKind = YobiPayloadKind::Raw);

// 471440的type2字段写序；已发布的外层slot逐次借用，原内层失败无空保护。
// WPL01：只借用槽，源值不写；原交错读取合同见doc/cg2d_window_payload_creation_spec.md。
void ConfigureWindowElementAction(
    YobiAction* initialAction, YobiAction*& slot, std::int32_t* const& imageNoSlot);
// 原4B4863–4B48A4：先转换图号，再逐次借用共享宠物预览槽写字段。
void ConfigureWindowPetPreview(YobiAction*& slot, std::int32_t imageNo);
// 原CF3B88/D2F3B0的两个静态内嵌预览；不分配、不进入通用Yobi链。
YobiAction* ConstructWindowSelectorPreview(std::int32_t index, std::uint32_t& flags); // 固定预览原位置原生构造，返回真实对象及flags|2快照。
void FinishWindowSelectorPreview(YobiAction* action, std::uint32_t flags); // SIC01：同预览flags/callback/固定技能源图号/layer/direction原序发布，不补影子值或读负宠物槽。
// 原475950完整展开动画创建，失败仍调用真实窗口打开；完成槽为非拥有T**。
YobiAction* CreateWindowOpeningAnimation_475950(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t id, std::uint32_t flags, std::uint32_t mode,
    YobiAction** completionSlot);

// 原48AC60完整数量展开；maximum为S32，callback为非拥有原生函数指针。
YobiAction* CreateQuantityWindowOpeningAnimation_48AC60(
    std::int32_t sourceWindow, std::int32_t sourceElement, std::int32_t imageNo,
    std::int32_t maximum, std::int32_t value14, std::int32_t initialValue,
    void (*callback)(std::int32_t), std::uint32_t flags, std::uint32_t mode,
    std::uint32_t actionFlags); // QIP01：失败直发布/返回空；低BYTE标志与两CryptInt原序。

// 原窗口41/38的完整展开创建；失败仍发布，kind成功路径按低S16传递。
// 原函数：0x004ABEF0。
YobiAction* CreateBankWindowOpeningAnimation_4ABEF0(
    std::int32_t kind, std::int32_t value14, std::int32_t userState,
    std::uint32_t flags, std::uint32_t mode); // SOA01：真实action指针，原失败同步开41并返空，低BYTE和临时CryptInt原序保持。
// 原函数：0x004ACE80。
YobiAction* CreateItemStorageWindowOpeningAnimation_4ACE80(
    std::int32_t kind, std::int32_t value14, std::int32_t userState,
    std::int32_t phase, std::uint32_t flags, std::uint32_t mode); // SOA01：真实action指针，原失败同步开38并返空，低BYTE和临时CryptInt原序保持。

// 原441DB0：独立载荷、四次坐标赋值/投影；4417E0与实体尾442F70正文已有，完整链/运行未验。
YobiAction* Sub_441DB0(
    std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction);
// LDT01／原448040：独立载荷、同临时槽四次坐标赋值/投影与tick；447E60及十种模式已有本体。
YobiAction* Sub_448040(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t direction, std::int16_t mode, std::int16_t value02,
    std::int32_t objectIndex);
// WLR01：519D50/4462E0/4464D0共用八槽/text清理；逐槽标死清引用，不处理+34/+38或释放主action/payload。
void ClearMapActorAttachments(YobiAction* action);
// 519CD0 创建成功后分别操作原 +24/+284；调用方在两次写入间重读玩家。
void SetYobiPlayerFlag(YobiAction* action) noexcept;
void SetYobiWalkSpeed(YobiAction* action, std::int32_t speed) noexcept;
// 519E50 借用地图实体 payload 的原 +44 槽；不增加 payload 空检查。
YobiAction* ReadMapActorAttachment44(const YobiAction* action) noexcept;
// 原46A8B1–46A8BD：先捕获payload，再写phase=2并清payload+48计数。
void BeginMapActorPhase2(YobiAction* action);
// 原46A922只读取方向低W16，用于共享55DE98；不返回地址或完整方向。
std::uint16_t ReadYobiDirectionLow16(const YobiAction* action) noexcept;
// 51A090 对象阶段：重新提取四段文本，每次写入按原顺序重读共享玩家。
YobiAction* PublishPlayerActionIdentity(
    const char* name, const char* titles, std::int32_t originalNameLength,
    std::int32_t level, std::int32_t value188);
// 519A80 分别复制 +170/+180 的载荷；前者复用 readiness，调用方负责重读玩家。
void CopyYobiPlayerHp(YobiAction* action, const platform::LegacyCryptInt* source);
void CopyYobiPlayerMaxHp(YobiAction* action, const platform::LegacyCryptInt* source);
// 51A4B0跨私有布局边界；借用同一玩家来源，按原位置重读共享action。
void CopyInitialPlayerActionIdentity(
    const char* name, const char* title, const std::int32_t* level,
    const platform::LegacyCryptInt* hp, const platform::LegacyCryptInt* maxHp);
// 50B380的2E4/2E8/2EC写序；slot可指向玩家共享槽或本行固定T*。
void UpdateMapActorMode(YobiAction*& slot, std::int32_t mode, std::int32_t imageNo);
// 50B380公共尾：先捕获payload，再探测字段17，不新增action空检查。
void ApplyMapActorReplyAttachments(YobiAction* action, const char* text);
// 50A3E0：以调用方已判非空的同一T*读目标X，随后重读共享槽读目标Y。
void ReadMapActorTarget(
    YobiAction* action, YobiAction*& slot, std::int32_t* x, std::int32_t* y);
// 既有入口：先取槽T*读目标X，再重读同一共享槽读目标Y；不新增空检查。
void ReadMapActorTarget(
    YobiAction*& slot, std::int32_t* x, std::int32_t* y);
void ClearMapActorPathCount(YobiAction* action) noexcept; // 51B080仅清原+270 W16。
bool IsMapActorIdleAtTarget(YobiAction* action); // 46AFE0的S16路径数与逐轴到格门禁。
// 46AFE0：每轴先判符号再重读T*求幅值，X不相邻则不读Y。
bool AreMapActorTargetsAdjacent(YobiAction*& previous, YobiAction*& current);
// 51A840动作60：借用玩家原图号槽，payload固定而共享action按原位置重读。
void ApplyPlayerAppearanceReply(
    std::int32_t imageNo, std::int32_t gearImage, std::int32_t petImage,
    std::int32_t kind, std::int32_t* playerImageNo);
// 51A840动作70：借用玩家value174，保留原创建失败无空保护和重复阶段早退。
void ApplyPlayerRideReply(
    std::int32_t imageNo, std::int32_t steps, std::int32_t* remainingSteps);
void ApplyPlayerPhase3Reply(std::int32_t imageNo); // 51A840动作80，不改玩家原图号。
// 444410满路径回摆的单槽对象阶段；slot为地图五槽原生借用，不等同共享玩家action。
void ResetMapFollowerFromPath(
    YobiAction* leader, std::int32_t position, YobiAction*& slot,
    bool isPlayer, std::int32_t* playerDirection);
// 地图实体的目标格坐标/路径/动作链；指针只借用，布局保持 runtime 私有。
// 原441F60：单点入队；原U16满队列分支清计数并重置到终点。
void Sub_441F60(
    YobiAction* action, std::int32_t targetX, std::int32_t targetY);
// 原441FC0：轴差恰为2时补中间点，按整批容量决定入队或位置重置。
void Sub_441FC0(
    YobiAction* action, std::int32_t targetX, std::int32_t targetY);
// 原442AB0：四次独立坐标赋值、清移动分量与世界坐标重置；不清路径计数。
void Sub_442AB0(
    YobiAction* action, std::int32_t targetX, std::int32_t targetY);
// 原函数：0x00442110。
double Sub_442110(
    YobiAction* action, std::int32_t targetX, std::int32_t targetY,
    std::int32_t speed, float overrideRate);
// 原4422F0：不按路径计数加速，两轴方向先落F32再乘完整S32速度。
void Sub_4422F0(
    YobiAction* action, std::int32_t targetX, std::int32_t targetY,
    std::int32_t speed);
// 原函数：0x00442460。
void Sub_442460(YobiAction* action); // 原 +270 S16 计数及两组路径交错左移。
// 原函数：0x00442730。
void Sub_442730(YobiAction* action); // 原普通跟随推进，不启动或弹出路径。
// 原函数：0x004428F0。
void Sub_4428F0(YobiAction* action); // 原9DAD78倍率推进，当前位置以mask优先写回。
// 原函数：0x0043EFC0。
void Sub_43EFC0(
    YobiAction* action, std::int32_t characterAction,
    std::int32_t restart); // 主对象/坐骑动作映射及附件播放重置。

// 原 46DE50：创建地图动画 action，保留四次 CryptInt 临时赋值及投影顺序。
YobiAction* Sub_46DE50(std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY);
// 原 46DD90：重新投影、读取加密坐标、FrameMove，再向同一地图对象缓存入队。
void Sub_46DD90(YobiAction* action);
// 46CF60 同步借用唯一 399x399 注册表槽；指针不拥有对象，不新增索引门禁。
std::uint8_t& MapYobiRegistryFlagAt(std::int32_t index) noexcept;
YobiAction*& MapYobiRegistryActionAt(std::int32_t index) noexcept;
// 原 +314/+318 分别写入；父函数两次独立索引/借用，不合并成对象快照。
void SetYobiMapOffsetX(YobiAction* action, std::int32_t value) noexcept;
void SetYobiMapOffsetY(YobiAction* action, std::int32_t value) noexcept;
// 原函数：0x0043F1F0。
YobiAction* Sub_43F1F0(std::uint32_t value);
// 原函数：0x0043F220。
void Sub_43F220();
// 原 43F230：标死链上 action 并清头；空链原返回寄存器无对象合同。
void* Sub_43F230();
// 原440920：只拦截动作81/82；未使用方向仍保留，返回完整0/1。
std::int32_t Sub_440920(
    std::int32_t objectIndex, std::int32_t x, std::int32_t y,
    std::int32_t unusedDirection, std::int32_t characterAction, const char* row);
// 原 440F50：按玩家对象号发布五槽源表；非空返回为子指针区起点。
void* Sub_440F50();
// 440FF0 借用同一 5F9740 方向值，不复制观察状态。
std::int32_t ReadMapObservationDirection() noexcept;
// 原 440FE0：创建地图观察节点、更新 BGM 状态并返回 0x25。
std::int32_t Sub_440FE0();
// 原5182B0：先查询，再按低BYTE轴先取S16输出/后取当前S32偏移；完整合同沿YGOS01。
std::int32_t GetYobiGraphicOriginOffset_5182B0(YobiAction* action, std::uint8_t axis);

// 原 YobiGear::new@[0x4410F0,0x441128)：分配零载荷 action 并设置五个字段。
YobiAction* NewYobiGear_4410F0(std::int32_t imageNo);
// 原441130：四组CryptInt与原投影的挂件创建，直接分配无载荷action。
YobiAction* NewYobiGearPet_441130(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY);

// 原 Yobi::UpdateAll@[0x404C50,0x404CF7)。
std::int32_t YobiUpdateAll_404C50();
// 原函数：0x00405C30。
std::int32_t SortBattleActions_405C30(); // flags筛选、原次序排序及低W8绘制层级发布。

// 原 Yobi::MarkDeath@[0x404D00,0x404D10)，只置死亡标志并返回原指针。
YobiAction* MarkDeath(YobiAction* action);

// 原471334内联终止写的native边界；不判断空指针，不对应独立原函数。
void MarkDeathUnchecked(YobiAction* action);
// 原函数：0x0043BAA0。
std::int32_t Sub_43BAA0(); // BEMS01：固定入口载荷、各次取包重取场景对象；CHT入行/BST当前玩家计时，头返回1才结束。
// 原函数：0x004383F0。
void InitializeBattleScene_4383F0(); // 43CBB0先清Yobi再调用；发布创建结果、清两表/六标量并生产宠物快照。
// 原函数：0x00438550。
void ParseBattleC_438550(); // 完整C角色/附件生产及22槽缺席清理；43CBB0消费一槽C后调用。
// 原函数：0x004238F0。
void UpdateBattleAlbumFlag_4238F0(YobiAction* action); // 固定载荷，先清FE0 bit0，再按本人/宠物/相册置位。
// 原函数：0x0043BED0。
void ClearBattleAppendEffects_43BED0(); // 每次发布当前节点、取next再释放，最后清首指针。
// 原函数：0x004990A0。
void Sub_4990A0(std::int32_t index); // callback/独立HP解码门禁，重取对象后按flags绘双条。

// 原43BFF0：场景10且回合一致时，按结束位给同源20槽非空对象flags置40H。
void Sub_43BFF0();
// 原函数：0x0043C040。
void Sub_43C040(); // 命令初始化逐槽清flags bit40，不改变其他位或对象生命周期。
// 原函数：0x0043C120。
std::int32_t GetSelectedBattleEntry_43C120(); // 返回单体/邻接/阵营/全体目标编号，先快照目标位与命中。
// 原函数：0x0043C910。
std::int32_t Sub_43C910(); // 全表未就绪对象置phase=2，全部就绪返回1。
// 原函数：0x0043C9A0。
void Sub_43C9A0(); // 先10..19再重读玩家位处理0..9，重取对象并转向/播放。
// 原函数：0x0043CA50。
std::int32_t Sub_43CA50(); // 非空战斗槽中载荷FDC bit4计数。
// 原函数：0x0043CA80。
void Sub_43CA80(); // 战斗原名字绘制，保留测量前Y/后X以及逐槽颜色状态。
// 原43C560：发布目标标志，依原序按HP/阵营选择并处理前后排遮挡。
std::int32_t CheckBattleCommand_43C560(std::int32_t position, std::uint32_t flags);
// 原43C060：逐槽清选择位、置绘制位及cacheIndex=-2，不销毁对象。
void FlushBattleEntries_43C060();
// 原战斗载荷+106C非拥有对象引用槽；空载荷返空，497C80仅在入口检查，点击后重取槽值。
YobiAction* const* FindBattleRebirthSlot(const YobiAction* action) noexcept;
// 原战斗载荷+13C8的非拥有Yobi引用槽；载荷空返空，调用方保留各自原门禁。
YobiAction* const* FindBattleRiderSlot(const YobiAction* action) noexcept;

// 原 YobiClearUp@[0x404D30,0x404D5B) 与 sub_404D60@[0x404D60,0x404D85)。
void YobiClearUp_404D30();
// 原函数：0x00404D60。
void ShutdownYobiActionList_404D60();

// 原449700：PVfz五参数的三路对象生产，复用唯一Yobi链及回调。
// mode>=100且余数0时，floor实参改作已过秒数，duration写总时长槽。
void Sub_449700(
    std::int32_t mode, std::int32_t duration, std::int32_t floor,
    std::int32_t mapX, std::int32_t mapY);
// 原函数：0x00449160。
void Sub_449160(); // 主循环效果生产，计时回调注册后按64次间隔生成漂移对象。

// 原 49C5C0/49C5E0：dialog 尺寸的有符号整数分量计算。
std::int32_t CalculateDialogTextColumns_49C5C0(std::int16_t width);
// 原函数：0x0049C5E0。
std::int32_t CalculateDialogVisibleLines_49C5E0(
    std::int16_t height,
    std::int32_t marginFlag,
    std::int32_t divisor);

// 原 modal creator sub_5044E0@[0x5044E0,0x5046C6)。
YobiAction* CreateModalAction_5044E0(
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    std::int32_t winId,
    std::int32_t param6);

// action+0x170 的 LegacyCryptInt 窄访问；每次比较只解码一次。
bool IsYobiActionReady(YobiAction* action, std::int32_t threshold);
bool IsYobiActionStateEqual(YobiAction* action, std::int32_t expected);
bool IsYobiActionStateLessOrEqual(
    YobiAction* action,
    std::int32_t maximum);
// 原47BA53直接比较action+1B0；读取同源FP一次，不增加空保护。
bool IsYobiActionFpLessThan(YobiAction* action, std::int32_t cost);
// 原401F80 cost-first FP检查；只消费一次右对象，保持读取计数与RNG。
bool IsCostGreaterThanYobiActionFp(std::int32_t cost, YobiAction* action);
// 原47C78D/47C8ED同源FP比较，各自单次读取，不缓存结果。
bool IsYobiActionFpGreaterEqual(YobiAction* action, std::int32_t cost);
std::int32_t ReadYobiActionFpOnce(YobiAction* action); // 原+1B0，47A7A0每次显示单独读取。
std::int32_t ReadYobiActionMaxFpOnce(YobiAction* action); // 原+1C0，先于当前FP读取。
std::int32_t ReadYobiActionHpOnce(YobiAction* action); // 原+170，4965D0单次读取后发布宠物快照。
std::int32_t ReadYobiActionMaxHpOnce(YobiAction* action); // 原+180，与HP读取分开。

// 原 +5C/+60：队列绘制按轴读取 F32，不向 render 暴露 action 布局。
float GetYobiActionScaleX(const YobiAction* action) noexcept;
float GetYobiActionScaleY(const YobiAction* action) noexcept;
// 原507408..50756F命中阶段分别借读+2C8/+2CC/+24，不复制对象快照。
std::int32_t ReadYobiGraphicOffsetX(const YobiAction* action) noexcept;
std::int32_t ReadYobiGraphicOffsetY(const YobiAction* action) noexcept;
std::uint32_t ReadYobiActionFlags(const YobiAction* action) noexcept;
// 原5076E4..507BE4：完整对象描述阶段，按原位置重取队列中的非拥有对象。
// 原函数：0x00507140。
void DescribeHoveredYobi_507140(const render::GraphicCacheItem32* graphic);

// 当前角色预览/槽位调用所需的语义字段访问，不暴露原 x86 偏移。
// 46F1D0 创建效果对象后清 +8，帧推进后读取 +20 图形索引。
void ClearYobiActionCallback(YobiAction* action);
std::int32_t GetYobiActionField20(YobiAction* action);
// 原472B64–472BD4：先掩码后颜色，原位更新并借用同一对象的描述符。
// WDP01：共享descriptor的原BYTE通道读取，见doc/cg2d_window_element_draw_pipeline_spec.md。
const render::GraphicDescriptor16* ApplyWindowAnimationDrawMode(
    YobiAction* action, bool replacementBeforePrevious = false); // PSP01：预览显式先替换后旧字节；其余调用沿原先旧后替换，两分支取样后同式发布，无新增字段默认值。
// 原宠物预览直接借用+2F8，不执行窗口动画的描述符改写。
const render::GraphicDescriptor16* GetYobiActionDrawMode(const YobiAction* action) noexcept;
void AddYobiActionDrawFlags(YobiAction* action, std::uint8_t flags) noexcept; // 原+305低U8置位，不清其他位。
// 原 +2E4；51A5B0 读取，独立于播放翻转值 +2E0。
std::int32_t GetYobiActionField2E4(const YobiAction* action) noexcept;
std::int32_t GetYobiActionField2DC(const YobiAction* action) noexcept; // 窗口12按S32位型比较同源播放圈数。
const char* GetYobiName(const YobiAction* action) noexcept; // 原+C4名称，只读借用同一对象。
const char* GetYobiTitle(const YobiAction* action) noexcept; // 原+E1标题，空串回退由调用方决定。
std::uint8_t GetYobiActionFlagsLowByte(const YobiAction* action) noexcept; // 原+24低U8，窗口26判bit3。
std::int32_t GetYobiActionCacheIndex(const YobiAction* action) noexcept; // 原+18数值索引，不是地址。
std::int32_t GetYobiActionField2A8(YobiAction* action);
void SetYobiActionField2A8(YobiAction* action, std::int32_t value);
void SetYobiActionField2B0(YobiAction* action, std::int32_t value);
std::int32_t GetYobiActionField2B0(const YobiAction* action) noexcept; // 原动作S32，摆摊预览逐次借读。
void SetYobiActionField15(YobiAction* action, std::uint8_t value);
std::uint8_t GetYobiActionField15(const YobiAction* action) noexcept; // 原+15显示层，选择器预览借读。
std::int32_t GetYobiActionField30(YobiAction* action);
void SetYobiActionField30(YobiAction* action, std::int32_t value);
std::int32_t GetYobiActionField34(YobiAction* action);
void SetYobiActionField34(YobiAction* action, std::int32_t value);
void SetYobiActionField2B8(YobiAction* action, std::int32_t value);
std::int32_t GetYobiActionField2B8(const YobiAction* action) noexcept; // 原方向S32，库存旋转重读。
void SetYobiActionField308(YobiAction* action, std::uint8_t value);
std::uint8_t ReadYobiBatchType(const YobiAction* action) noexcept; // 原+308，战斗场景状态条绘制门禁。
void SetYobiActionField309(YobiAction* action, std::uint8_t value); // 原描述符替换掩码。
void SetYobiActionField310(YobiAction* action, std::uint8_t value); // 原掩码替换值，与309独立写入。
void SetYobiActionField30C(YobiAction* action, std::int32_t value);
std::uint8_t GetYobiActionField30F(YobiAction* action);
void SetYobiActionField30F(YobiAction* action, std::uint8_t value);

// 原45E6E0：创建预览16阶段推进，仅阶段转换时读取一次+170，尾部置上次动作为-1。
std::int32_t AdvanceCharacterPreviewAnimation(YobiAction* action);

// 原 Yobi::FrameMove@[0x517880,0x517D9D)：参数速率、严格 ==1 停播及低 W8 静图翻转开关；取样沿YFPS01 SPEC。
std::int32_t AdvanceYobiAnimationFrame_517880(
    YobiAction* action,
    std::int32_t rate,
    std::int32_t stopAtEnd,
    std::uint32_t flipStatic);

} // namespace cg::yobi
