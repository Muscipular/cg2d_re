#pragma once

#include <cstdint>

namespace cg::platform {
struct LegacyCryptInt;
}

namespace cg::render {
struct GraphicDescriptor16;
struct GraphicCacheItem32;
}

namespace cg::login {

// 原 x86 YobiAction 的原生 x64 承载；具体布局只由 Yobi runtime 持有。
// runtime/list 是唯一 owner，场景中的 YobiAction* 均为非拥有观察者。
struct YobiAction;

namespace detail {

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
extern std::int32_t gYobiUpdateGateMode;
extern std::int32_t gYobiUpdatePhaseCounter;

// 原 0x59F9F4/0x59F9FC/0x59FA00/0x59FA04/0x553C58；
// volatile 保留 sub_423640 的独立 W32 读取及越界值、钳位值两次写入。
extern volatile std::uint32_t gYobiScreenOffsetRising;
extern volatile std::uint32_t gYobiScreenOffsetX;
extern volatile std::uint32_t gYobiScreenOffsetY;
extern volatile std::uint32_t gYobiScreenOffsetAmplitude;
extern volatile std::uint32_t gYobiScreenOffsetMaximumAmplitude;

// 原 sub_404A40@[0x404A40,0x404AD1)；初始化首尾哨兵并返回尾哨兵。
// 与原端相同，调用前提是尚未初始化，且不注册进程退出析构。
YobiAction* Sub_404A40();

// 原 YobiAction::new@[0x404AE0,0x404C42)。priority 的有效范围为 [0, 0xFE]。
YobiAction* NewYobiAction_404AE0(
    std::uint8_t priority,
    std::uint32_t payloadSize);

// 原41EAB0：克隆战斗对象及原生载荷，保留新节点链表位置，内部T*仍为借用引用。
YobiAction* CloneBattleAction_41EAB0(YobiAction* source);
void UpdateBattleAfterimage_41E5C0(YobiAction* action); // 递增FF0，新计数达到12才标死。
YobiAction* NewBattleAfterimage_41E5F0(YobiAction* source); // 原克隆路径无失败门禁，复用完整浅复制。

// 471440的type2字段写序；已发布的外层slot逐次借用，原内层失败无空保护。
void ConfigureWindowElementAction(YobiAction*& slot, const std::int32_t* imageNo);
// 原4B4863–4B48A4：先转换图号，再逐次借用共享宠物预览槽写字段。
void ConfigureWindowPetPreview(YobiAction*& slot, std::int32_t imageNo);
// 原CF3B88/D2F3B0的两个静态内嵌预览；不分配、不进入通用Yobi链。
YobiAction* BorrowWindowSelectorPreview(std::int32_t index) noexcept;
std::uint32_t ConstructWindowSelectorPreview(YobiAction* action); // 十CryptInt，返回flags|2快照。
void FinishWindowSelectorPreview(YobiAction* action, std::uint32_t flags);
void DestroyWindowSelectorPreview(YobiAction* action); // 只逆序析构十CryptInt。
// 原475950完整展开动画创建，失败仍调用真实窗口打开；完成槽为非拥有T**。
YobiAction* Sub_475950(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t id, std::uint32_t flags, std::uint32_t mode,
    YobiAction** completionSlot);

// 原48AC60完整数量展开；maximum为S32，callback为非拥有原生函数指针。
YobiAction* Sub_48AC60(
    std::int32_t sourceWindow, std::int32_t sourceElement, std::int32_t imageNo,
    std::int32_t maximum, std::int32_t value14, std::int32_t initialValue,
    void (*callback)(std::int32_t), std::uint32_t flags, std::uint32_t mode,
    std::uint32_t actionFlags);

// 原窗口41/38的完整展开创建；失败仍发布，kind成功路径按低S16传递。
YobiAction* Sub_4ABEF0(
    std::int32_t kind, std::int32_t value14, std::int32_t userState,
    std::uint32_t flags, std::uint32_t mode);
YobiAction* Sub_4ACE80(
    std::int32_t kind, std::int32_t value14, std::int32_t userState,
    std::int32_t phase, std::uint32_t flags, std::uint32_t mode);

// 三种地图角色生产者共用的 x64 payload 分配，不改变通用 404AE0 的尺寸合同。
YobiAction* NewMapCharaAction();
// 原43F330/43FEB0：观察链两种生产者，43F520/440070绑定及正文已有，完整寿命/运行未验。
YobiAction* Sub_43F330(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t direction, std::int32_t value170);
YobiAction* Sub_43FEB0(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t direction);
// 原4408C0：仅正编号写观察载荷五槽，入口无action空保护，不改子指针。
void Sub_4408C0(
    YobiAction* action, std::int32_t first, std::int32_t second,
    std::int32_t third, std::int32_t fourth, std::int32_t fifth);

// 原441DB0：独立载荷、四次坐标赋值/投影；4417E0与实体尾442F70正文已有，完整链/运行未验。
YobiAction* Sub_441DB0(
    std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction);
// 原448040：独立载荷、四次坐标赋值/投影与tick；447E60父分派及十种模式已有本体。
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
double Sub_442110(
    YobiAction* action, std::int32_t targetX, std::int32_t targetY,
    std::int32_t speed, float overrideRate);
// 原4422F0：不按路径计数加速，两轴方向先落F32再乘完整S32速度。
void Sub_4422F0(
    YobiAction* action, std::int32_t targetX, std::int32_t targetY,
    std::int32_t speed);
void Sub_442460(YobiAction* action); // 原 +270 S16 计数及两组路径交错左移。
void Sub_4424B0(YobiAction* action); // 完整逐帧推进与当前位置 CryptInt 更新。
void Sub_442730(YobiAction* action); // 原普通跟随推进，不启动或弹出路径。
void Sub_4428F0(YobiAction* action); // 原9DAD78倍率推进，当前位置以mask优先写回。
// 原43E490：完整阶段0..3及默认分支；非零阻断4417E0后续处理。
std::int32_t Sub_43E490(YobiAction* action);
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

// 原 5F9750 独立观察链；引用非拥有，清头不释放 action。
YobiAction* Sub_43F180(YobiAction* action);
void* Sub_43F1B0(YobiAction* action);
YobiAction* Sub_43F1F0(std::uint32_t value);
void Sub_43F220();
// 原 43F230：标死链上 action 并清头；空链原返回寄存器无对象合同。
void* Sub_43F230();

// 原 440B70：从 440F50 生产的五槽状态创建地图观察节点。
YobiAction* Sub_440B70();
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
// 原 43FD90：按主图号、子槽号和方向累加原表偏移，返回 y。
std::int32_t* Sub_43FD90(
    std::int32_t imageNo, std::int32_t slot, std::int32_t direction,
    std::int32_t* x, std::int32_t* y);
// 原5182B0：先查询，再按低BYTE轴先取S16输出/后取当前S32偏移；完整合同沿YGOS01。
std::int32_t Sub_5182B0(YobiAction* action, std::uint8_t axis);
// 原518300：先确定kind的F32系数再查询，按低BYTE轴向零转换/窄化；完整合同沿YGOS01。
std::int32_t Sub_518300(YobiAction* action, std::uint8_t axis, std::int32_t kind);
std::int32_t Sub_4417A0(std::int32_t imageNo); // 原40项角色高度表及-88回退。
// 原444000：无回调挂件创建，四次CryptInt赋值和世界/屏幕投影。
YobiAction* Sub_444000(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t characterAction, std::int32_t direction, std::uint8_t displayPriority);
// 原两轮kind调整；primaryIndex未使用，输出按原指针写序，不返回聚合值。
void Sub_4412D0(
    YobiAction* action, std::int32_t primaryIndex,
    std::int32_t* characterAction, std::int32_t* revert);
void Sub_441430(
    YobiAction* action, std::int32_t primaryIndex,
    std::int32_t* x, std::int32_t* y, std::uint8_t* before);
void DrawGear_441570(
    YobiAction* action, std::int32_t primaryIndex,
    std::int32_t offsetX, std::int32_t offsetY, std::uint8_t before);
// 原43DB60：独立读两条CryptInt数值，绘条后总是绘外框，返回最后入队结果。
std::int32_t ShowHpBar_43DB60(
    YobiAction* action, std::int16_t offsetX, std::int16_t offsetY, std::int32_t color);

// 原 YobiGear::new@[0x4410F0,0x441128)：分配零载荷 action 并设置五个字段。
YobiAction* NewYobiGear_4410F0(std::int32_t imageNo);
// 原441130：四组CryptInt与原投影的挂件创建，直接分配无载荷action。
YobiAction* NewYobiGearPet_441130(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY);
// 原43DD00：按字段消费EF挂件、骑乘及附属实体，返回下一字段序号。
std::int32_t Sub_43DD00(YobiAction* action, const char* payload, std::int32_t field);

// 原 Yobi::UpdateAll@[0x404C50,0x404CF7)。
std::int32_t YobiUpdateAll_404C50();
std::int32_t SortBattleActions_405C30(); // flags筛选、原次序排序及低W8绘制层级发布。

// 原 Yobi::MarkDeath@[0x404D00,0x404D10)，只置死亡标志并返回原指针。
YobiAction* MarkDeath(YobiAction* action);

// 原471334内联终止写的native边界；不判断空指针，不对应独立原函数。
void MarkDeathUnchecked(YobiAction* action);

std::int32_t GetBattleMHeader_439070(char* destination); // 独立头游标，读3B并补NUL，结束返回1。
std::int32_t TakeBattleMInteger_438F70(YobiAction* action); // 对象载荷64游标读取大写十六进制。
void TakeBattleMString_438FE0(YobiAction* action, char* destination, std::int32_t capacity);
std::int32_t GetBattlePosition_4237E0(std::int32_t position); // 50/51转20/21，其余原值。
std::int32_t Sub_43BAA0(); // BEMS01：固定入口载荷、各次取包重取场景对象；CHT入行/BST当前玩家计时，头返回1才结束。
std::int32_t TakeBattleMPreviewInteger_439600(); // 局部扫描，仅结束时发布5AAA60。
void TakeBattleMPreviewString_439670(char* destination, std::int32_t capacity);
std::int32_t GetBattleMPreviewHeader_439700(char* destination); // 预扫描头，不推进5A8A3C。
std::int32_t TakeBattleMIntegerAt_43B940(std::int32_t* offset); // 显式游标，逐字节发布。
void TakeBattleMStringAt_43B8C0(std::int32_t* offset, char* destination, std::int32_t capacity);
void ScanBattleMRideParticipants_439770(std::uint8_t* participants); // 22槽计数，RID试读成功才推进游标。
std::int32_t ScanBattleMAttackParticipants_439920(std::int32_t skill, std::uint8_t* participants);
void ScanBattleMParticipants_439B30(
    std::int32_t kind, std::uint8_t* participants, std::int32_t* source, std::int32_t* flag);
// 原43B290：冲突返回0，保留此前槽写入和预扫描副作用；成功OR参与位，不清调用方mask。
std::int32_t CheckBattleMParticipants_43B290(
    std::int32_t* source, std::int32_t previousSource, std::uint32_t* mask, std::int32_t* flag);
// 原43B360完整事件调度，4383B0已绑定；424620攻击生产已落位，完整事件/数据与运行另验。
void DispatchBattleEvents_43B360(YobiAction* scene);
// 原43B610：绑定完整427470、原序初始化22位置并发布32槽事件T*。
YobiAction* NewBattleEvent_43B610(
    std::int32_t kind, std::int32_t slot, std::uint32_t participants, std::int32_t group);
void UpdateBattleEvent_427470(YobiAction* scene); // 完整分派与恢复/回存；424620本体已接，完整战斗另验。
std::int32_t PeekBattleMInteger_43B860(YobiAction* action); // 只预读，不推进对象或全局游标。
std::int32_t GetBattleMObjectHeader_423960(YobiAction* action, char* destination);
YobiAction* NewBattleScene_4383B0(); // 优先级1，原生场景载荷、完整43B360绑定及A8全位置1。
void InitializeBattleScene_4383F0(); // 43CBB0先清Yobi再调用；发布创建结果、清两表/六标量并生产宠物快照。
YobiAction* NewBattleActor_438290(std::int32_t position); // 基础创建与404DB0绑定；完整角色属性由各caller生产。
void ParseBattleMRide_4239D0(YobiAction* action); // RID预读/实际消费差异，双新对象交226恢复发布。
std::int32_t TakeBattleCInteger_438450(); // C独立全局游标，大写十六进制读取。
void TakeBattleCString_4384C0(char* destination, std::int32_t capacity); // 原双字节截断，尾部反转义/空叶。
void ParseBattleC_438550(); // 完整C角色/附件生产及22槽缺席清理；43CBB0消费一槽C后调用。
void FinishBattleEvent_43B6C0(YobiAction* action); // 推进同一事件消息或注销当前槽后MarkDeath。
void AppendBattleQueuedTarget_43B7F0(YobiAction* action, std::int32_t value);
std::int32_t AdvanceBattleQueuedTarget_43B830(YobiAction* action); // 先推进13A4，再判断新槽是否为-1。
std::int32_t GetBattleValueBand_43BF00(std::int32_t value); // B22H01：原1..15按五个S32阈值计档1..5，范围外-1。
void UpdateBattleAttachment_421F90(YobiAction* action); // 父战斗槽身份变化时只标死附件。
void SyncBattleRider_421FB0(YobiAction* action); // 非零phase仅同步绘制属性；零phase另映射动作并投影。
void SyncBattleGear_4225D0(YobiAction* action); // 骑乘两轴均用父scaleX，Y轴重取骑乘引用。
void SyncBattleAuxGear_422780(YobiAction* action, std::int32_t index); // type/动作/方向映射；身份检查前复制方向。
YobiAction* NewBattleRider_4221F0(YobiAction* parent, std::int32_t imageNo);
YobiAction* NewBattleGear_422A30(YobiAction* parent, std::int32_t imageNo);
YobiAction* NewBattleAuxGear_422B00(
    YobiAction* parent, std::int32_t imageNo, std::int16_t sortIndex);
std::int32_t SetBattleRider_4222B0(
    YobiAction* action, std::int32_t imageNo, std::int32_t ridingImageNo);
void SetBattleGear_422BC0(YobiAction* action, std::int32_t imageNo); // 原无模式门槛，等价422C20的index0。
void SetBattleGear_422C20(YobiAction* action, std::int32_t imageNo, std::int32_t index);
// 原两caller的index均0/1；type6不更新已有图号，type3..5仅首次创建设置缩放。
void SetBattleAuxGear_422C90(
    YobiAction* action, std::int32_t imageNo, std::int32_t type, std::int32_t index);
void UpdateBattleOneShotEffect_419FA0(YobiAction* action); // PlayFrame(1)严格返回1才标死。
void UpdateBattleProjection_418D80(YobiAction* action); // 父FD0门禁仅影响移动，标死后仍播放/投影。
void NewBattleProjection_418DF0(
    YobiAction* parent, float x, float y, float targetX, float targetY,
    std::int32_t imageNo); // 目标先截S32再转向；原三caller不消费返回，指定图号播放后额外移动两次（BPR01）。
void UpdateBattleArcProjection_418F10(YobiAction* action); // 固定父引用，标死后仍残影/屏幕投影，保留两次角度读取。
YobiAction* NewBattleArcProjection_418FF0(
    YobiAction* parent, std::int32_t offsetX, std::int32_t offsetY,
    YobiAction* distanceSource, YobiAction* target, std::int32_t imageNo); // 原rand角度，距离分两次有符号除法。
void UpdateBattleScatterEffect_41E4D0(YobiAction* action); // BSEX01：移动/终帧标死仍投影，Y取样先于X发布。
YobiAction* NewBattleScatterEffect_41E510(YobiAction* parent, std::int32_t imageNo); // BSEX01：成功才三RNG；X发布后Y取样/W16零归，再发布Y。
void UpdateBattlePositionEffect_41E640(YobiAction* action); // PlayFrame(1)严格完成才标死，所有路径投影X/Y。
YobiAction* NewBattlePositionEffect_41E680(YobiAction* parent, std::int32_t imageNo);
YobiAction* NewBattleCoordinateEffect_41E720(float x, float y, std::int32_t imageNo); // 无父引用，不提前播放。
YobiAction* NewBattleTrailEffect_41E7C0(
    float x, float y, std::int32_t imageNo, std::int32_t mode,
    std::uint8_t priority); // 失败不消耗随机；成功绑定41E640，设备分支决定混合标志。
void UpdateBattleCompletingEffect_41A160(YobiAction* action); // 播放完成后通知父1024，标死仍投影。
void UpdateBattleParentEffect_41A1C0(YobiAction* action); // 父严格死亡时标死自身，仍播放并投影。
void UpdateBattleRisingEffect_41A200(YobiAction* action); // 固定父引用，未舍入Y比较；标死后仍播放/投影。
void UpdateBattleScalingEffect_41A270(YobiAction* action); // phase0放大、phase2缩小，以未舍入X判定阶段。
void UpdateBattleRandomEmitter_41A310(YobiAction* action); // 50次随机生成；父死亡早退，完成数通知父1024。
void UpdateBattleFallingEffect_41A530(YobiAction* action); // 未舍入Y超过nameY才递增父58，仍播放/投影。
void UpdateBattleReverseEmitter_41A590(YobiAction* action); // 132B数值载荷，每槽四粒子，倒序坐标。
void UpdateBattleIndexedEmitter_41A7D0(YobiAction* action); // 32次顺序生成，失败也递增nameX。
YobiAction* NewBattleRisingEffect_420D50(YobiAction* parent); // 先随机位置再分配，成功后可再随机混合标志。
void UpdateBattleImpactEffect_41A9E0(YobiAction* action); // 完成后按当前效果处理HP/动作，固定目标与独立重读并存。
void UpdateBattleImpactSoundEffect_41AF00(YobiAction* action); // 默认分类末尾另播132，其余合同同41A9E0。
void UpdateBattleFrameEventEffect_41B440(YobiAction* action); // 先消费帧事件，再按旧58/FD0推进动画与nameY消退。
void UpdateBattleMovingImpactEffect_41BB00(YobiAction* action); // 实时目标中心命中，保留提前标死后的运动/投影尾。
void UpdateBattleHomingImpactEffect_41C3B0(YobiAction* action); // 距离门禁/HP分类，命中后出界仍继续移动与尾迹。
void UpdateBattleCarryingEffect_4219D0(YobiAction* action); // 当帧carry独立快照，出界推进/标死后仍带动当前目标。
// 41CD00第三槽：创建类型0..47使用S32 Y槽；其他类型消费已有对象T*，由caller显式选择成员。
union BattleEffectThirdArgument {
    std::int32_t offsetY;
    YobiAction* existingAction;
};
YobiAction* NewBattleEffect_41CD00(
    YobiAction* source, std::int32_t offsetX, BattleEffectThirdArgument third,
    std::uint32_t type, std::int32_t blend, std::int32_t link); // 完整48类及已有对象公共尾。
std::int32_t DrawBattleTiledEffect_41E890(std::int32_t type); // BSTL01：共享相位先推进，X外Y内100格/每格fresh相位/末次返回。
void UpdateBattleTiledEffect_41E920(YobiAction* action); // 独立58相位/1D4反向，绘制后判寿命且仍自增。
YobiAction* NewBattleTiledEffect_41E9D0(std::int32_t type, std::int32_t lifetime); // maxHp/readiness依次临时构造赋值。
// 工厂初始化：仅借用现有对象与真实回调，空action/source原样返回，不分配/播放。
void InitBattleTargetEffect_422E50(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*)); // 固定目标坐标与图号音效。
void InitBattleOffsetEffect_423040(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*)); // 使用DX的S16半高，复制103C/1044。
void InitBattleCenterEffect_4231D0(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*)); // 中心坐标、随机速度/角度，转向时重取目标。
void InitBattleLowerEffect_423350(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*)); // 使用对象2CC的S32半偏移，不复制技能。
void InitBattleScreenYEffect_4234B0(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*),
    std::int32_t* side); // 可空数值输出；写输出后才读取目标屏幕Y。
YobiAction* NewBattleRandomEffect_419FC0(
    YobiAction* parent, std::int32_t imageNo, std::int32_t direction); // X/Y分别取随机并重读尺寸。
YobiAction* NewBattleScreenEffect_41A0D0(
    std::int32_t x, std::int32_t y, std::int32_t imageNo, std::int32_t direction); // 屏幕坐标直接创建，绑定完整419FA0，不提前播放。
void UpdateBattleFloatingIcon_41F450(YobiAction* action); // 未舍入速度推进Y，停留60帧后标死并投影。
void UpdateBattleNumber_41F110(YobiAction* action); // 延迟后整数累加屏幕Y，停留50帧；独立投影文字坐标。
std::int32_t NewBattleNumber_41F1A0(
    std::int32_t x, std::int32_t y, std::int32_t value, std::int32_t style,
    std::int32_t fontType, std::uint32_t flags); // 逐字符创建，失败保留已建对象，恒返回0。
YobiAction* NewBattleFloatingIcon_41F4C0(
    YobiAction* parent, std::int32_t imageNo, std::int32_t offsetX, std::int32_t offsetY);
void NewBattleRebirthEffects_4208F0(YobiAction* source, YobiAction* target); // BRAE01：1080低S8，首段后重取目标类型。
std::uint8_t NewBattleAttributeEffects_4209C0(
    YobiAction* source, YobiAction* target); // BRAE01：四W16非零匹配；U8掩码表示匹配并调用。
void UpdateBattleHitEffect_41F570(YobiAction* action); // BHE01：原计数/上限持样，标死后Y取样先于X发布。
// BHE01：原五参数/零载荷请求和字段发布，播放后Y取样先于X发布，返回原生T*。
YobiAction* NewBattleHitEffect_41F5B0(
    YobiAction* parent, std::int32_t offsetX, std::int32_t offsetY,
    std::int32_t lifetime, std::int32_t imageNo);
YobiAction* NewBattleDebuffEffect_41F660(YobiAction* parent, std::int32_t imageNo); // BAS01：六图号先改FDC/13D0/13CC，分配失败不回滚；播放后fresh父载荷写1054再绑定父。
void UpdateBattleDebuffEffect_41F800(YobiAction* action); // BAS01：1054/105C共用借父回调，Play后重借父跟随；标死仍投影，Y转换后取屏幕偏移。
YobiAction* NewBattleReverseEffect_41F860(YobiAction* parent, std::int32_t imageNo); // BAS01：105C已有返空，设备后持flags两次发布；播放后fresh父载荷写105C再绑定父。
void UpdateBattleCenteredEffect_41F950(YobiAction* action); // BDE01：父严格标死后仍共同投影尾。
void UpdateBattleOffsetFollowEffect_419CB0(YobiAction* action); // 父标死仍按屏幕坐标/S32偏移跟随并播放。
void UpdateBattleTransitionEffect_419BE0(YobiAction* action); // phase0播放严格完成才置phase1/隐藏，不自动标死。
YobiAction* NewBattleTransitionEffect_419C10(YobiAction* parent, std::int32_t imageNo);
YobiAction* NewBattleOffsetFollowEffect_419CF0(
    YobiAction* parent, std::int32_t offsetX, std::int32_t offsetY, std::int32_t imageNo); // 播放后按浮点坐标加偏移投影。
YobiAction* NewBattleCenteredEffect_41FA00(YobiAction* parent, std::int32_t imageNo); // BDE01：1064已有即空返，S16半宽高/播放后发布/投影。
void UpdateBattleAttachedEffect_41FB20(YobiAction* action);
YobiAction* NewBattleAttachedEffect_41FBE0(YobiAction* parent, std::int32_t mode); // 已有槽只换图并返回空。
void UpdateBattleStatusEffect_41FCD0(YobiAction* action);
YobiAction* NewBattleStatusEffect_41FE00(YobiAction* parent, std::int32_t imageNo, std::int32_t mode); // 1068发布后执行缩放/下降。
void UpdateBattleFieldEffect_41FF90(YobiAction* action); // BFL01：当前场景death严格1标死自身，仍投影；Y转换后再取屏幕Y偏移。
YobiAction* NewBattleFieldEffect_41FFE0(std::int32_t imageNo); // BFL01：B0已有只换图返空；两组索引先齐读坐标，播放后fresh场景发布，原投影尾。
void UpdateBattleBackgroundFade_421480(YobiAction* action); // 完整五态淡变，同源背景发布与延后标死。
YobiAction* NewBattleBackgroundFade_421570(
    std::int32_t imageNo, std::uint8_t step, std::int32_t publishBackground);
void RestoreBattleStateEffects_422370(YobiAction* parent); // 固定入口FDC快照，保留成对共用清位掩码。
YobiAction* NewBattleScreenEffect_421610(std::int32_t x, std::int32_t y);
void UpdateBattleScreenEmitter_4216B0(YobiAction* action); // 原rand先Y后X，生成后重取阶段并递减。
YobiAction* NewBattleScreenEmitter_421700(std::int32_t phase);
void NewBattleSequence_420100(std::int8_t value, std::int32_t position); // 逐个创建9/10项，失败保留已建对象。
void UpdateBattleSequence_420300(YobiAction* action); // 原2A8六态，CryptInt自增和比较分别消费读取。
void MoveBattleSequence_420590(YobiAction* action); // X从F32投影，Y从未舍入计算值投影。
void UpdateBattleText_421750(YobiAction* action); // 面板就绪才绘制/扣帧，逐行释放后依次标死。
YobiAction* NewBattleText_4217F0(
    std::int8_t style, const char* text, std::int32_t fontType, std::int32_t duration);
std::int32_t MatchBattleAlbum_423800(const YobiAction* action); // 名称原字节与四W16零/非零状态。
void UpdateBattleAlbumFlag_4238F0(YobiAction* action); // 固定载荷，先清FE0 bit0，再按本人/宠物/相册置位。
std::int32_t SelectIntValue_404D90(const std::int32_t* values, std::int32_t count, std::int32_t index);
// 原4242B0：三方动作/效果按序追加，返回redirect，不缓存对象表中的T*。
std::int32_t AppendBattleRedirect_4242B0(
    YobiAction* scene, std::int32_t source, std::int32_t target,
    std::uint32_t flags, std::int32_t value, std::int32_t redirect);
// 原426FC0：四个输出保持独立指针；255终止与空对象收尾路径不同。
void TakeBattleMSkillTargets_426FC0(
    YobiAction* scene, std::int32_t skill, std::int32_t level,
    std::int32_t technique, std::int32_t fpUsed, std::int32_t attackMode,
    std::int32_t* sourcePosition, std::int32_t* initialPosition,
    std::int32_t* targetPosition, std::int32_t* redirectedPosition);
// 原424620完整九参攻击生产；D01次级图号仅入口0一次，保留跨轮复用。
void TakeBattleMSkillAttackTargets_424620(
    YobiAction* scene, std::int32_t skill, std::int32_t level,
    std::int32_t levelIndex, std::int32_t fpUsed,
    std::int32_t* sourcePosition, std::int32_t* initialPosition,
    std::int32_t* targetPosition, std::int32_t* redirectedPosition);
// 原43B9B0：只推进局部游标，flags&1010h非零且位置匹配时返回次位置，否则-1。
std::int32_t FindBattleMTarget_43B9B0(YobiAction* scene, std::int32_t position, std::int32_t skip);
// 原424500：两处目标动作可重复追加；flags保持W32，只消费E000h/20h。
void AppendBattleSkillTail_424500(
    YobiAction* scene, std::int32_t position, std::uint32_t flags,
    std::int32_t mode, std::int32_t target);

// 原1C起的16B效果值记录；仅传数值，不含底层对象指针。
struct BattleEntryEffectRecord {
    std::int32_t position; // 原记录+00，-1为队列哨兵。
    std::uint32_t flags; // 原记录+04。
    std::int32_t position3; // 原记录+08，4393A0/439400写入。
    std::int32_t position4; // 原记录+0C，仅439400写入。
};
void UpdateBattleActor_404DB0(YobiAction* action); // 完整257槽与固定入口载荷公共尾，场景7直接返回。
YobiAction* NewBattleSpiracleShot_418B70(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
YobiAction* NewBattleBlastWave_419180(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
YobiAction* NewBattleRangeHit_419390(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
YobiAction* NewBattleBoomerang_4195F0(YobiAction* parent); // B22C01：原取样/几何发布，同步总回调推进后返回原生T*。
YobiAction* NewBattleArcShot_419DA0(YobiAction* parent, const BattleEntryEffectRecord* effect,
    std::int32_t imageNo, std::int32_t count, std::int32_t mode);
YobiAction* NewBattleCastCopy_4205F0(YobiAction* parent); // BCPY01：独立战斗载荷与父借用，三载荷重取/末坐标发布；非浅克隆。
void UpdateBattleAction15_406E10(YobiAction* action); // B15P01：独立事件/音效持样，回旋返回发布和258低档/高档循环保原序。
void UpdateBattleAction18_408170(YobiAction* action); // BSP01：258三图/22050范围，单次门禁和音效event-X；95/3019重播。
void UpdateBattleAction19_4082F0(YobiAction* action); // BSP01：探测后33速率/22090范围，单次门禁和音效event-X；仅95重播。
void UpdateBattleAction70_4070E0(YobiAction* action); // BTE01：播放后一次事件/逐轮投射/末声清事件。
void UpdateBattleAction103_40DBE0(YobiAction* action); // BCPY01：独立复制体/七动作/效果四值按原序取样。
void UpdateBattleAction109_40E4B0(YobiAction* action); // BKE01：独立复制体25/110/107及四值4-flags-3-position读取，设备后完整flags与双推进保持。
void UpdateBattleAction112_40EB00(YobiAction* action); // BKE01：双复制体/第三列表选图、一次事件/三Y-X及运动读序、两Append不同四值顺序。
void UpdateBattleAction224_414050(YobiAction* action);
void UpdateBattleAction226_4154C0(YobiAction* action);
void UpdateBattleAction227_415940(YobiAction* action);
void UpdateBattleAction241_417B80(YobiAction* action);
void UpdateBattleAction242_417CC0(YobiAction* action);
void UpdateBattleAction249_4071C0(YobiAction* action);
void AppendBattleEffectList_4393A0(
    YobiAction* action, std::int32_t position, std::uint32_t flags, std::int32_t position3);
void AppendBattleEffect_439400(YobiAction* action, BattleEntryEffectRecord effect);
void AppendBattleAuxEffects_43BC10(
    YobiAction* action, std::int32_t position, std::uint32_t flags,
    std::int32_t value3, std::int32_t value4, std::int32_t value5);
// 原439470：先推进C9C，返回新项position是否为-1；空对象返回0。
std::int32_t AdvanceBattleEffect_439470(YobiAction* action);
// 原43BC90：三次推进、四输出原序写入并分别重读游标，返回末次推进结果。
std::int32_t TakeBattleAuxEffects_43BC90(
    YobiAction* action, std::int32_t* position, std::int32_t* value3,
    std::int32_t* value4, std::int32_t* value5);
// 原406390：固定当前flags/position，五块独立执行；附加块消费四输出，逐调用重取对象。
void ApplyBattleEffectFlags_406390(std::int32_t sourcePosition, YobiAction* action);
void ApplyBattleCastEffects_416DB0(YobiAction* action); // 动作235：先应用效果，再推进动作队列。
void UpdateBattleAction0_406650(YobiAction* action); // BIDLE01：完整FDC持样/CL消费与归零后fresh BYTE、特殊上一项门禁。
void UpdateBattleAction1_406720(YobiAction* action); // BIDLE01：动作归零后独立FDC低BYTE60播放门禁，不套动作0条件。
std::int32_t UpdateBattleAction2_406760(YobiAction* action); // BRCP01：回00/04，X发布/Y取样/phase0，末播放。
std::int32_t UpdateBattleAction3_406C30(YobiAction* action); // BRCP01：转向后fresh目标；距离<70推进，另一支移动。
std::int32_t UpdateBattleAction4_406D70(YobiAction* action); // BRCP01：转向后fresh目标；距离<8推进，另一支移动。
std::int32_t UpdateBattleAction5_40CFA0(YobiAction* action); // BPAW01：比较旧FF0，推进后清零/末播放。
std::int32_t UpdateBattleAction6_406CD0(YobiAction* action); // BRCP01：FE8/FEC两组取样，抵达X先发布再取Y/推进。
void UpdateBattleAction7_409410(YobiAction* action); // BPAW01：持position转向；终帧后fresh BYTE调速/推进。
void UpdateBattleAction8_4094A0(YobiAction* action); // BPAW01：FE8/FEC转向；终帧后fresh BYTE调速/推进。
std::int32_t UpdateBattleAction9_409510(YobiAction* action); // BPAW01：前两项14回位<5；其它fresh目标<70，未抵达才移动。
void UpdateBattleAction10_409610(YobiAction* action); // BPAW01：终帧后上一项11/低BYTE门禁，重查本位恢复方向。
void UpdateBattleAction11_4067F0(YobiAction* action); // BRCP01：骑乘双T*低BYTE短路；回位尾fresh动作播放。
void UpdateBattleAction12_406930(YobiAction* action); // B12P01：入口低BYTE；Y-X随机、旧FF0周期/held速率与fresh FCC门禁。
void UpdateBattleAction13_406A80(YobiAction* action); // B12P01：入口低BYTE；先播放/转向/移动，<5后X发布再读Y，末不补播放。
void UpdateBattleAction14_409680(YobiAction* action); // B14H01：四类命中/六归零复制持样、独立BYTE读取与14-28当帧延时/FD4尾。
std::int32_t ProbeBattleAttackEvent_4065E0(YobiAction* action); // BSP01：终帧优先/一次事件/原无上限循环，末依次清上次动作与事件。
void UpdateBattleAction16_4072E0(YobiAction* action); // 投射命中与退场；保原空/索引门禁、目标重取/特殊推进和双播放尾。
void UpdateBattleAction17_4093B0(YobiAction* action); // 本叶仅移动/超界标死；404DB0仍执行公共尾，404C50/404D10后续摘链/释放（BPEX01）。
void UpdateBattleAction20_408410(YobiAction* action); // B20H01：近距命中保低BYTE/HP归零取样，免疫跳过扣血/激活位，标死后仍移动。
std::int32_t UpdateBattleAction21_408750(YobiAction* action); // B22L01：两次Y-X取样，距20以内转phase22，移动后返回播放结果。
std::int32_t UpdateBattleAction22_4087C0(YobiAction* action); // B22H01：回旋命中保独立HP/低BYTE与取样、3/7帧延迟及真实返回。
std::int32_t UpdateBattleAction23_4090F0(YobiAction* action); // B22L01：固定caster/父载荷与266列表，标死后仍移动/播放并返回其结果。
void UpdateBattleAction24_409380(YobiAction* action); // BPAW01：新FF0>20，先归零后推进，无播放。
void UpdateBattleAction25_406BA0(YobiAction* action); // BPAW01：X/Y两次查目标发布；等待归零后fresh BYTE门禁。
void UpdateBattleAction26_40A9E0(YobiAction* action); // BHRC01：低WORD门禁，角度持样反转/移动/播放，无计数推进。
std::int32_t UpdateBattleAction27_40AA70(YobiAction* action); // BHRC01：入口FDC时长与fresh边界分开；计数先于FF8清零。
std::int32_t UpdateBattleAction28_40ACF0(YobiAction* action); // BHRC01：132音效后fresh FCC，14/34/54三段与原负速归零。
std::int32_t UpdateBattleAction29_40AE50(YobiAction* action); // BHRC01：新count<14后退，>=44归零/推进，末播放。
std::int32_t UpdateBattleAction30_40AF30(YobiAction* action); // BHRC01：count>=14才fresh完整FDC选14/28阈值。
std::int32_t UpdateBattleAction31_40B020(YobiAction* action); // BHRC01：偶数旋转/fresh FCC，等待other14，方向后fresh低WORD。
void UpdateBattleAction32_40B410(YobiAction* action); // BSEX01：8/100，严格终帧才推进。
void UpdateBattleAction33_40B450(YobiAction* action); // BSEX01：9/100，严格终帧才推进。
std::int32_t UpdateBattleAction34_40B5E0(YobiAction* action); // BSEX01：低WORD/BYTE、四边反弹退场，播放直到非零。
std::int32_t UpdateBattleAction35_40B930(YobiAction* action); // BSEX01：持样边界/40次散射，每帧尾迹后直线退场。
void UpdateBattleAction36_40BAD0(YobiAction* action); // BSEX01：低BYTE门禁/播放后fresh FF0，>=30归零推进。
void UpdateBattleAction37_40BB20(YobiAction* action); // BWT01：低BYTE门禁/播放后fresh计数，>=60归零推进。
void UpdateBattleAction38_40BB70(YobiAction* action); // BWT01：低BYTE门禁/播放后fresh计数，>=120归零推进。
std::int32_t UpdateBattleAction39_40BBC0(YobiAction* action); // BWT01：36短路/原声和两方向，末连续三播放。
void UpdateBattleAction40_40BD40(YobiAction* action); // BWT01：恢复方向后推进，不增播放。
void ConsumeBattleFpCost_405B60(YobiAction* action); // BSA01：正cost扣FP/负值零归/末-1，保token。
void UpdateBattleAction41_40BD60(YobiAction* action); // BWT01：静态两22项表，固定point先Y后X。
void UpdateBattleAction42_40BDB0(YobiAction* action); // BWT01：全局目标先Y后X，转向本对象。
std::int32_t UpdateBattleAction43_40BE30(YobiAction* action); // BWT01：有other才转向，共同100/0/播放返回。
void UpdateBattleAction44_40BDF0(YobiAction* action); // BWT01：目标朝本对象转向，推进本对象。
void UpdateBattleAction45_40B130(YobiAction* action); // BDE01：三字段读齐/按原序发布、临时销毁后推进。
void UpdateBattleAction46_40B200(YobiAction* action); // BDE01：骑乘短路/缩放/清理及不同219-48-创建推进分支。
void UpdateBattleAction47_40B3F0(YobiAction* action); // BDE01：整FDC置2后推进。
void UpdateBattleAction48_40B490(YobiAction* action); // BDE01：两设备退场，旧alpha前置/新alpha重取。
void UpdateBattleAction49_40B5B0(YobiAction* action); // BDE01：1024严格2才清零/切51。
void UpdateBattleAction50_40B3C0(YobiAction* action); // BDE01：仅动作10/严格终帧推进。
YobiAction* UpdateBattleAction51_40BE70(YobiAction* action); // BDE01：清全局槽先标死，保T*返回。
void UpdateBattleAction52_40BE90(YobiAction* action); // BDE01：固定caster/逐次目标，追加13-47-51后启动。
std::int32_t UpdateBattleAction53_40BF20(YobiAction* action); // BDE01：未舍入缩放/创建后重读，末共同播放返回。
std::int32_t UpdateBattleAction54_40BFE0(YobiAction* action); // BSA01：未舍入放大，type23后精确1/重查设备。
void UpdateBattleAction55_40C0C0(YobiAction* action); // BSA01：播放→实际扣FP→推进。
void UpdateBattleAction56_40C160(YobiAction* action); // BSA01：动作5终帧清index/remain/event。
void UpdateBattleAction57_40C1A0(YobiAction* action); // BSA01：动作13终帧只清index/remain。
void UpdateBattleAction58_40C1E0(YobiAction* action); // BSA01：动作14终帧只清index/remain。
void UpdateBattleAction59_40C240(YobiAction* action); // BSA01：动作15终帧只清index/remain。
void UpdateBattleAction60_40C220(YobiAction* action); // BSA01：仅动作15/推进，无播放。
void UpdateBattleAction61_40C280(YobiAction* action); // BSA01：动作12终帧推进，不清帧。
void UpdateBattleAction62_40C2B0(YobiAction* action); // BSA01：动作18终帧推进，不清帧。
std::int32_t UpdateBattleAction63_40C2E0(YobiAction* action); // BSA01：旧70推进，20/50方向与互斥播放。
void UpdateBattleAction64_40CBB0(YobiAction* action); // BTE01：仅动作0/推进，无播放。
std::int32_t UpdateBattleAction65_40C380(YobiAction* action); // BTE01：旧70/八计数点、fresh方向与互斥播放。
void UpdateBattleAction66_40C480(YobiAction* action); // BTE01：1014图号/推进后清事件。
void UpdateBattleAction67_4106B0(YobiAction* action); // BTE01：入口事件/逐目标启动/哨兵额外推进。
void UpdateBattleAction68_40C0E0(YobiAction* action); // BTE01：创建后BYTE门禁/总推进。
void UpdateBattleAction69_40C120(YobiAction* action); // BTE01：30帧创建后BYTE门禁/总推进。
void UpdateBattleAction71_40C4D0(YobiAction* action); // BTE01：逐目标启动/哨兵推进，原无播放。
void UpdateBattleAction72_40CAA0(YobiAction* action); // BTE01：caster原00计数/源推进。
void UpdateBattleAction75_40CB10(YobiAction* action); // BCLC01：1004置1/推进，无播放。
void UpdateBattleAction76_40CB30(YobiAction* action); // BCLC01：1004清零/推进，无播放。
void UpdateBattleAction77_40CB50(YobiAction* action); // BCLC01：查询后仅置设备对应绘制位。
void UpdateBattleAction78_40CB80(YobiAction* action); // BCLC01：查询后仅清设备对应绘制位。
void UpdateBattleAction79_40CBD0(YobiAction* action); // BCLC01：五槽逐次清理、两标志持样/发布后推进。
void UpdateBattleAction80_40CC80(YobiAction* action); // BCLC01：X先持样/独立1018音效。
void UpdateBattleAction81_40CCB0(YobiAction* action); // BCLC01：X先持样/独立101C音效。
void UpdateBattleAction82_40CCE0(YobiAction* action); // BCLC01：X先持样/独立1020音效。
std::int32_t UpdateBattleAction83_40CD10(YobiAction* action); // BCLC01：未舍入阈值/推进后重取载荷置1000位。
void UpdateBattleAction84_40CD70(YobiAction* action); // BRAF01：新载荷清位/入口载荷BYTE门禁。
void UpdateBattleAction85_40CDE0(YobiAction* action); // BRAF01：放大旋转/2002名称表/入口BYTE门禁。
void UpdateBattleAction86_40C520(YobiAction* action); // BRAF01：固定视觉与动态HP/phase目标，临时复制原读序。
std::int32_t UpdateBattleAction87_40C780(YobiAction* action); // BRAF01：十位置RNG序/新270清零推进/末播放。
void UpdateBattleAction88_40C8D0(YobiAction* action); // BRAF01：振幅6/启用1/推进。
void UpdateBattleAction89_40C8F0(YobiAction* action); // BRAF01：仅启用0/推进。
void UpdateBattleAction90_40C910(YobiAction* action); // BRAF01：逐目标/事件重取/同帧延迟减1/四边界退出。
std::int32_t UpdateBattleAction91_40CF30(YobiAction* action); // BRAF01：对象flags置2→推进→播放返回。
std::int32_t UpdateBattleAction92_40CF50(YobiAction* action); // BRAF01：对象flags清2→推进→播放返回。
void UpdateBattleAction93_40CF70(YobiAction* action); // BSTL01：活动数量先判，再清alive并推进。
void UpdateBattleAction94_40D010(YobiAction* action); // BSTL01：四次独立目标读取/单圈角度/50步进。
void UpdateBattleAction95_40D0B0(YobiAction* action); // BSTL01：旧16门禁/推进后清FF0与原HP零复制读序。
void UpdateBattleAction96_40D170(YobiAction* action); // BSTL01：当前目标hidden置0/source推进。
void UpdateBattleAction97_40D1A0(YobiAction* action); // BSTL01：当前目标hidden置1/source推进。
void UpdateBattleAction98_40D1D0(YobiAction* action); // BSTL01：双选图门禁/两次skill读取/铺格生产。
std::int32_t UpdateBattleAction99_40D390(YobiAction* action); // BCPY01：rate25→推进→清两帧字段→返回0。
void UpdateBattleAction100_40D3C0(YobiAction* action); // BCPY01：入口frameIndex先目标，S8序列/分档/fresh结束哨兵。
std::int32_t UpdateBattleAction101_40D580(YobiAction* action); // BCPY01：位置逐次重取，两侧中心/面对点及末播放返回。
void UpdateBattleAction102_40D6C0(YobiAction* action); // BCPY01：完整逐效果伤害/回复与原BYTE消费、动态HP零复制。
void UpdateBattleAction104_40DC90(YobiAction* action); // BIE01：先读全局玩家位，成功分支重取；按原CryptInt前两W32显示并保额外效果推进。
std::int32_t UpdateBattleAction105_40DE40(YobiAction* action); // BIE01：游标先源位置，两次门禁独立；旧计数>=120后推进源/入口目标，末播放返回。
void UpdateBattleAction106_40DF50(YobiAction* action); // BIE01：完整动作106仅角色动作13→Advance，无播放/速率/帧清零。
YobiAction* UpdateBattleAction107_40DF70(YobiAction* action); // BIE01：完整动作107调用MarkDeath并返回真实T*；物理释放沿scheduler。
void UpdateBattleAction108_40DF80(YobiAction* action); // BIE01：入口事件持样/逐效果do，13 BYTE与临时HP复制读序；免疫只按完整flags。
void UpdateBattleAction110_40E560(YobiAction* action); // BKE01：旧FF0/1038蓄力及两侧HP零逐项复制；五BYTE与真实事件循环/旧FD0收尾。
std::int32_t UpdateBattleAction111_40E970(YobiAction* action); // BKE01：两次fresh BYTE FDC/40次散射，原角度与四边界后仍Move/Play S32。
void UpdateBattleAction113_40EFE0(YobiAction* action); // BKE01：worldY先载荷；两轴各自先借用父再数值，旧294/FF0及末fresh父标死保持。
void UpdateBattleAction114_40F0A0(YobiAction* action); // BCVI01：缓存静态零初值/负HP复制原读序，完整动作合同沿BPR01。
void UpdateBattleAction115_40FB80(YobiAction* action); // BFW01：固定载荷/完整flags的CH门禁，工厂后重取目标隐藏；全路径phase162。
void UpdateBattleFactoryAction_40FC10(YobiAction* action); // 31动作共享30种映射；118不创建但仍phase162。
void UpdateBattleActions127To129_40FF90(YobiAction* action); // BFW01：完整FDC持样/条件播放，后取phase选7/8/9末参0，全路径推进。
void UpdateBattleAction138_40FF40(YobiAction* action); // BFW01：完整FDC持样/条件播放，后取1024创建type20，全路径phase162。
void UpdateBattleAction139_410000(YobiAction* action); // BFW01：事件/完成两次独立特效指针持样，98单次属性；405不同取样/四临时复制原序。
void UpdateBattleAction140_410730(YobiAction* action); // BFL01：入口技能持样，特效末参0或先推进61–63目标，最后推进源。
void UpdateBattleAction141_410E90(YobiAction* action); // 13D8/13DC与技能选择减益图，重读FDC决定播放。
void UpdateBattleAction142_410F70(YobiAction* action); // BFL01：无空门禁标死1054，fresh完整FDC取样后先清槽再发布掩码值，播放/推进。
void UpdateBattleAction143_410FC0(YobiAction* action); // 两型状态图号/13E0索引，调用后条件播放并推进。
void UpdateBattleAction144_411240(YobiAction* action); // BFL01：无空门禁标死1068，fresh完整FDC取样后先清槽再发布掩码值，播放/推进。
void UpdateBattleAction145_411290(YobiAction* action); // BFL01：一次技能选择四图，创建返回弃置；其它技能亦共同推进，无播放。
void UpdateBattleAction147_411320(YobiAction* action); // BAS01：完整FDC的AH800开关；创建失败仍置位，标死后fresh取旗位、先清105C再发布。
void UpdateBattleAction148_40FEF0(YobiAction* action); // BAS01：完整FDC持样/AL60播放门禁，后取1024创建type33，全路径phase162。
void UpdateBattleAction154_40F790(YobiAction* action); // type24发布1060，等待特效phase严格1。
void UpdateBattleAction155_40F800(YobiAction* action); // 两次独立重播种/缩放目标与方向差异音效，收尾令1060.phase=2。
void UpdateBattleAction156_40FB30(YobiAction* action); // 1060.phase严格3才标死、清槽并推进。
void UpdateBattleAction159_4107C0(YobiAction* action); // 1034选择HP/FP操作，独立比较/复制后推进动作。
void UpdateBattleAction160_410B00(YobiAction* action); // 同159数值处理，但只转phase37，不推进动作队列。
void UpdateBattleAction162_410E30(YobiAction* action); // 1024严格2才清通知/隐藏/2D8并推进。
void UpdateBattleAction163_4113A0(YobiAction* action); // 转生进入/退出、四槽发布与差异透明度/清理尾。
void UpdateBattleAction164_411A50(YobiAction* action); // 强制退出转生，完成时恢复FF0保存的动作阶段。
std::int32_t UpdateBattleAction165_411D30(YobiAction* action); // 无条件旋转受击，保留入口FDC和未舍入运动比较。
void UpdateBattleAction166_411CB0(YobiAction* action); // 同源计时扣减、浮动时间及队列/动作依次推进。
void UpdateBattleAction167_411FA0(YobiAction* action); // batch8后推进动作。
void UpdateBattleAction168_411FC0(YobiAction* action); // batch0后推进动作。
void UpdateBattleAction169_411FE0(YobiAction* action); // 仅309=255后推进动作。
void UpdateBattleAction170_412000(YobiAction* action); // 仅309=0后推进动作。
void UpdateBattleAction171_412020(YobiAction* action); // 自身及九附属槽淡入，完成后推进。
void UpdateBattleActions172_173_4120F0(YobiAction* action); // phase173减2，其余减3，完成尾保留附属8000位。
void UpdateBattleAction174_4121D0(YobiAction* action); // 当前效果选择调色板，再推进效果/动作。
void UpdateBattleAction175_412210(YobiAction* action); // 1060背景淡变创建/复用，重取对象令phase0。
void UpdateBattleAction176_412260(YobiAction* action); // 1060令phase2并清槽，不标死。
void UpdateBattleAction177_412290(YobiAction* action); // 创建发布背景的淡变对象，不保存1060。
void UpdateBattleAction178_4122C0(YobiAction* action); // 推进后覆盖self.phase为0。
void UpdateBattleAction179_4122E0(YobiAction* action); // 动作6，播放严格返回1才推进。
void UpdateBattleActions180_203_412310(YobiAction* action); // 23种工厂映射，188不创建，统一转204。
void UpdateBattleAction204_4124E0(YobiAction* action); // 九槽淡入、HP/FP结算及独立等待/完成检查。
void UpdateBattleAction205_4127B0(YobiAction* action); // 动作16完成后清帧索引/剩余时长。
void UpdateBattleAction206_4127F0(YobiAction* action); // 朝当前效果目标转向后推进。
void UpdateBattleAction207_412830(YobiAction* action); // 当前效果写FE8/FEC，推进效果后转phase6。
void UpdateBattleAction225_4150F0(YobiAction* action); // 骑乘副本抛物线及1060坐标/原图恢复。
std::int32_t UpdateBattleAction228_416000(YobiAction* action); // 清FF0后立即执行完整229。
std::int32_t UpdateBattleAction229_416020(YobiAction* action); // 旧FF0决定播放速率，满60推进后仍播放。
void UpdateBattleAction230_4160A0(YobiAction* action); // 三段重取效果坐标，flags按S32转F32。
std::int32_t UpdateBattleAction231_416150(YobiAction* action); // 未舍入减速、两个独立尾迹条件，返回旧FF0。
std::int32_t UpdateBattleAction232_416220(YobiAction* action); // 动态目标命中，先推进自身动作再推进效果，每帧原rand尾迹。
std::int32_t UpdateBattleAction233_416860(YobiAction* action); // 无位移和减速的分段旋转，保留入口FDC与旧计数。
std::int32_t UpdateBattleAction234_416AB0(YobiAction* action); // 首帧散射及四边反弹，播放至非零，不加循环上限。
void SyncBattleAttachmentAlpha_420F10(
    YobiAction* action, std::int32_t alpha, std::int8_t delta,
    std::uint32_t addMask, std::uint32_t removeMask); // 按原九槽顺序重取，不处理1060/1070。
std::int32_t NewBattleTimeNumber_4206C0(std::int32_t x, std::int32_t y, std::int32_t seconds);
void UpdateBattleAction236_406630(YobiAction* action); // 清phase/FD8后执行完整动作0。
void SetBattleAction208Target_412870(YobiAction* action); // 按目标坐标界线写FE8/FEC，最后phase6。
std::int32_t UpdateBattleAction209_412900(YobiAction* action); // 完整动作209，效果按调用边界重读。
YobiAction* UpdateBattleAction210_412F70(YobiAction* action); // 追近后播放，再取一次随机创建位置特效。
void ApplyBattleSceneEffects_413070(YobiAction* action); // 动作211：四独立W16写，效果先于动作推进。
void UpdateBattleAction212_413110(YobiAction* action); // 新计数与当前flags按S32比较，达到才推进效果/动作。
std::int32_t UpdateBattleAction213_4131B0(YobiAction* action); // 动画结束顺序清四附件，保留分支返回值。
void UpdateBattleAction214_4132B0(YobiAction* action); // 两设备分支：闪烁后建特效，或先快照/后重读30F衰减。
void UpdateBattleAction215_4133D0(YobiAction* action); // 1024严格为2才清槽并推进动作。
void UpdateBattleAction216_413400(YobiAction* action); // 施法特效、帧事件逐项消费及播放后独立平铺尾。
void UpdateBattleAction218_413550(YobiAction* action); // 克隆换图、双对象渐变及13AC/13B0完成回写。
void UpdateBattleAction219_413750(YobiAction* action); // 恢复原图后按FDC走48或居中特效/动作推进。
void UpdateBattleAction220_413980(YobiAction* action); // 恢复原图、动作10终帧及透明度后转214。
void UpdateBattleAction221_413B40(YobiAction* action); // 效果双次推进、固定目标HP及原源HP清零差异。
std::int32_t UpdateBattleAction222_413E90(YobiAction* action); // 追踪/返回转向，标死后仍移动播放并返回帧结果。
void UpdateBattleAction223_413FE0(YobiAction* action); // 发布或先刷新后清宠物快照槽，再按效果/动作顺序推进。
std::int32_t UpdateBattleAction237_416DD0(YobiAction* action); // 完整动作237，返回最终PlayFrame结果。
void UpdateBattleActions238_239_417410(YobiAction* action); // 动态效果命中，FD0归零当帧双播放，按写回F32速度转阶段。
void UpdateBattleAction240_417AC0(YobiAction* action); // 播放后消费帧事件，清施法特效后重取+10决定平铺。
void UpdateBattleAction243_417D80(YobiAction* action); // 技能图号及动态flags命中，普通路径不扣HP/显示伤害数字。
void UpdateBattleAction244_418550(YobiAction* action); // FDC置2、门禁和FF8置1后推进动作。
void UpdateBattleAction245_418580(YobiAction* action); // 当前效果指定平铺类型/寿命，创建后推进效果及动作。
void UpdateBattleAction246_4185C0(YobiAction* action); // position3乘原F32比例设置速度，再推进效果及动作。
void UpdateBattleAction247_4185F0(YobiAction* action); // 原F32坐标按X/Y分别截断到载荷10/14。
std::int32_t UpdateBattleAction248_418620(YobiAction* action); // 返回载荷10/14坐标，距离<5时重读坐标并推进。
void UpdateBattleAction251_40B180(YobiAction* action); // 两次独立CryptInt(1)，按mask/encoded/count分别写HP和FP。
void UpdateBattleAction253_40FE50(YobiAction* action); // 播放后判phase253/1024，创建type43后总是推进。
void UpdateBattleAction254_40FEB0(YobiAction* action); // 仅+10非空才标死并清+10/1024，空槽保留1024。
void UpdateBattleAction255_4186B0(YobiAction* action); // 每轮固定效果记录，治疗用目标1DC，保留空目标收尾。
void UpdateBattleAction256_418B50(YobiAction* unused); // 对象实参未使用，仅顺序写5ABB1C/5ABB20。
// 原43BF30：mode1只消费预扫描8整数；mode0读对象字段，五输出均为S32非拥有指针。
void TakeBattleMAuxGraphics_43BF30(
    YobiAction* action, std::int32_t mode, std::int32_t* firstImage, std::int32_t* secondImage,
    std::int32_t* firstImages, std::int32_t* secondImages, std::int32_t* values);

std::uint8_t IsBattleAppendEffectSkill_43BD00(std::int32_t skill); // 原54项技能准入集合，返回0/1；不查询链，见追加效果链SPEC。
void AddBattleAppendEffect_43BE00(
    std::int32_t skill, std::int32_t technique, std::int32_t position); // 匹配首节点替换，否则尾插。
std::int32_t FindBattleAppendEffectTechnique_43BEA0(
    std::int32_t skill, std::int32_t position); // 按位置/技能查首节点，未命中-1。
void ClearBattleAppendEffects_43BED0(); // 每次发布当前节点、取next再释放，最后清首指针。
void Sub_4990A0(std::int32_t index); // callback/独立HP解码门禁，重取对象后按flags绘双条。

// 原43BFF0：场景10且回合一致时，按结束位给同源20槽非空对象flags置40H。
void Sub_43BFF0();
void Sub_43C040(); // 命令初始化逐槽清flags bit40，不改变其他位或对象生命周期。
void DrawBattleEntryHover_43C0A0(std::int32_t index); // 固定可选对象，按DX尺寸和播放偏移画框。
std::int32_t GetSelectedBattleEntry_43C120(); // 返回单体/邻接/阵营/全体目标编号，先快照目标位与命中。
void Sub_405CB0(YobiAction* action); // 角度只校正一圈，按八个不等宽区间写方向。
void NormalizeBattleAngle_405E60(YobiAction* action); // BSTL01：上限首样/第二角度持样/负值重取，一次加减360。
void MoveBattleByAngle_405EA0(YobiAction* action); // X存回F32后才计算Y，角度/速度按原序读取。
std::int8_t GetBattleRelativeDirection_405ED0(
    float sourceX, float sourceY, float targetX, float targetY,
    float tolerance); // 第二组坐标作基准，原负基准两次相减及三闭区间保持独立。
double GetBattleAngleDifference_406060(YobiAction* source, YobiAction* target); // 空引用返回0，未舍入角差只校正一圈。
void TurnBattleTowardPosition_4060D0(
    YobiAction* action, float x, float y, std::int32_t updateDirection,
    float step); // 无空指针门禁；角差转S32取绝对值后限步，严格1才更新方向。
void TurnBattleToward_406190(
    YobiAction* action, YobiAction* target, std::int32_t updateDirection, float step);
std::int32_t GetBattleDistanceToPosition_4062A0(YobiAction* action, float x, float y); // BRCP01：先X后Y求差，平方和存F32/正常S32返回。
std::int32_t GetBattleDistance_406300(YobiAction* first, YobiAction* second); // 平方和先舍入F32再开方。
std::int32_t CountBattleActiveEntries_406360(); // BHRC01：22槽含20/21，FF8完整S32严格==1，无新payload保护。
std::int32_t SeedBattleRandom_4065A0(std::int32_t value); // 前20槽非空数乘活动M字节长度与输入，重播同源随机种子。
void Sub_406260(YobiAction* action); // 载荷反向坐标差转角度，再更新方向。
void InitBattleCastList_4390E0(std::int32_t index); // 22槽索引，保留禁用/phase13门禁。
void ResetBattleCastList_4391E0(std::int32_t index); // 恢复重置，不清忙状态、技能或转向。
void AppendBattleCast_4392A0(YobiAction* caster, YobiAction* actor, std::int32_t state);
void AppendBattleCastAt_4392F0(
    YobiAction* caster, YobiAction* actor, std::int32_t state, std::int32_t index);
void AdvanceBattleCastState_439340(YobiAction* action); // 先推进FC4，到200或-1哨兵写phase236。
void AdvanceBattleLinkedCast_40CAC0(YobiAction* action); // 动作73：当前效果目标先推进，再推进自身。
void AdvanceBattleEffectAndCast_40CAF0(YobiAction* action); // 动作74：推进效果后总是推进动作。
void MoveBattleCastState_439380(YobiAction* action); // 播放索引对应项写phase，不推进索引。
std::int32_t CheckBattleTurnReady_4394A0(YobiAction* parent, std::int32_t enable); // 22槽就绪及超时恢复。
std::int32_t Sub_43C910(); // 全表未就绪对象置phase=2，全部就绪返回1。
void Sub_43C9A0(); // 先10..19再重读玩家位处理0..9，重取对象并转向/播放。
std::int32_t Sub_43CA50(); // 非空战斗槽中载荷FDC bit4计数。
void Sub_43CA80(); // 战斗原名字绘制，保留测量前Y/后X以及逐槽颜色状态。
// 原43C560：发布目标标志，依原序按HP/阵营选择并处理前后排遮挡。
std::int32_t CheckBattleCommand_43C560(std::int32_t position, std::uint32_t flags);
// 原43C060：逐槽清选择位、置绘制位及cacheIndex=-2，不销毁对象。
void FlushBattleEntries_43C060();
// 原战斗载荷+106C非拥有对象引用槽；空载荷返空，497C80仅在入口检查，点击后重取槽值。
YobiAction* const* BorrowBattleRebirthSlot106C(const YobiAction* action) noexcept;
// 原战斗载荷+13C8的非拥有Yobi引用槽；载荷空返空，调用方保留各自原门禁。
YobiAction* const* BorrowBattleRiderSlot13C8(const YobiAction* action) noexcept;

// 原 YobiClearUp@[0x404D30,0x404D5B) 与 sub_404D60@[0x404D60,0x404D85)。
void YobiClearUp_404D30();
void Sub_404D60();

// 原 sub_448370@[0x448370,0x44839C)；查找首个 callback 匹配且 deathFlag 为零的普通节点。
// 调用前提为已初始化的有效链；允许空 callback，返回非拥有指针，未命中返回空。
YobiAction* Sub_448370(void (*callback)(YobiAction*));

// 原449700：PVfz五参数的三路对象生产，复用唯一Yobi链及回调。
// mode>=100且余数0时，floor实参改作已过秒数，duration写总时长槽。
void Sub_449700(
    std::int32_t mode, std::int32_t duration, std::int32_t floor,
    std::int32_t mapX, std::int32_t mapY);
void Sub_449160(); // 主循环效果生产，计时回调注册后按64次间隔生成漂移对象。

// 原 49C5C0/49C5E0：dialog 尺寸的有符号整数分量计算。
std::int32_t Sub_49C5C0(std::int16_t width);
std::int32_t Sub_49C5E0(
    std::int16_t height,
    std::int32_t marginFlag,
    std::int32_t divisor);

// 原 modal creator sub_5044E0@[0x5044E0,0x5046C6)。
YobiAction* Sub_5044E0(
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
void SetYobiActionFlag2(YobiAction* action, bool enabled) noexcept; // 旧29初始化/显示只改原+24的2位。
void ClearYobiActionFlags0C(YobiAction* action) noexcept; // 旧24换图后独立清原+24的0CH位。
// 原5076E4..507BE4：完整对象描述阶段，按原位置重取队列中的非拥有对象。
void DescribeHoveredYobi_507140(const render::GraphicCacheItem32* graphic);

// 当前角色预览/槽位调用所需的语义字段访问，不暴露原 x86 偏移。
// 46F1D0 创建效果对象后清 +8，帧推进后读取 +20 图形索引。
void ClearYobiActionCallback(YobiAction* action);
std::int32_t GetYobiActionField20(YobiAction* action);
// 原472B64–472BD4：先掩码后颜色，原位更新并借用同一对象的描述符。
// WDP01：共享descriptor的原BYTE通道读取，见doc/cg2d_window_element_draw_pipeline_spec.md。
const render::GraphicDescriptor16* ApplyWindowAnimationDrawMode(YobiAction* action);
// 原宠物预览直接借用+2F8，不执行窗口动画的描述符改写。
const render::GraphicDescriptor16* BorrowYobiActionDrawMode(const YobiAction* action) noexcept;
void AddYobiActionDrawFlags(YobiAction* action, std::uint8_t flags) noexcept; // 原+305低U8置位，不清其他位。
// 原 +2E4；51A5B0 读取，独立于播放翻转值 +2E0。
std::int32_t GetYobiActionField2E4(const YobiAction* action) noexcept;
std::int32_t GetYobiActionField2DC(const YobiAction* action) noexcept; // 窗口12按S32位型比较同源播放圈数。
const char* BorrowYobiNameC4(const YobiAction* action) noexcept; // 原+C4名称，只读借用同一对象。
const char* BorrowYobiTitleE1(const YobiAction* action) noexcept; // 原+E1标题，空串回退由调用方决定。
std::uint8_t GetYobiActionFlagsLowByte(const YobiAction* action) noexcept; // 原+24低U8，窗口26判bit3。
std::int32_t GetYobiActionCacheIndex(const YobiAction* action) noexcept; // 原+18数值索引，不是地址。
std::int32_t GetYobiActionField2A8(YobiAction* action);
void SetYobiActionField2A8(YobiAction* action, std::int32_t value);
void SetYobiActionField2B0(YobiAction* action, std::int32_t value);
std::int32_t GetYobiActionField2B0(const YobiAction* action) noexcept; // 原动作S32，摆摊预览逐次借读。
void SetYobiActionField2B4(YobiAction* action, std::int32_t value); // 原上次动作，旧加工启动时置-1。
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
std::uint8_t GetYobiActionField310(const YobiAction* action) noexcept; // 旧结果页按位或，不能替换其他位。
void SetYobiActionField30C(YobiAction* action, std::int32_t value);
std::uint8_t GetYobiActionField30F(YobiAction* action);
void SetYobiActionField30F(YobiAction* action, std::uint8_t value);

// 原45E6E0：创建预览16阶段推进，仅阶段转换时读取一次+170，尾部置上次动作为-1。
std::int32_t Sub_45E6E0(YobiAction* action);

// 旧UI通用预览；工厂直写图号，详情页自行决定是否做51B740转换。
YobiAction* Sub_4D6640(std::int32_t image, std::int32_t x, std::int32_t y); // 旧对话固定DD层预览，失败返回空T*。
YobiAction* Sub_4F9FF0(
    std::int32_t image, std::int32_t x, std::int32_t y, std::uint8_t displayPriority);
// 旧42未读提示专用预览；方向0/flags14H，包含动作5的八动作循环集合。
YobiAction* Sub_4EBF70(
    std::int32_t image, std::int32_t x, std::int32_t y, std::uint8_t displayPriority);
std::int32_t Sub_4EBFD0(YobiAction* action); // 原回调返回完整517880结果，注册时显式弃返回。
std::int32_t Sub_4D6690(YobiAction* action); // 旧UI通用预览，指定七动作循环播放。
// 旧技能/采集结果面板共用预览；保留100900/100901和原图号分类，不套普通对象创建。
YobiAction* Sub_4FAAA0(std::int32_t image, std::int32_t x, std::int32_t y);
std::int32_t Sub_4FACE0(YobiAction* action); // 完整回调，注册时显式弃返回。
void Sub_4EF5F0(YobiAction* action, std::int32_t result); // 非零/零结果动作，低图号可保留旧动作。

// 原 Yobi::FrameMove@[0x517880,0x517D9D)：参数速率、严格 ==1 停播及低 W8 静图翻转开关；取样沿YFPS01 SPEC。
std::int32_t Sub_517880(
    YobiAction* action,
    std::int32_t rate,
    std::int32_t stopAtEnd,
    std::uint32_t flipStatic);

// 原 Yobi::PlayFrame@[517DA0,5182A1)：使用对象播放速率；静图仅小于 100000；两入口共享帧消费沿YFPS01 SPEC。
std::int32_t PlayFrame_517DA0(YobiAction* action, std::int32_t stopAtEnd);

} // namespace detail
} // namespace cg::login
