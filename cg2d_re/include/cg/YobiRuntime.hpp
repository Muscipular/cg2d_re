#pragma once

#include <cstdint>

namespace cg::platform {
struct LegacyCryptInt;
}

namespace cg::render {
struct GraphicDescriptor16;
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
// 原43F330/43FEB0：观察链两种生产者，绑定43F520/440070；回调本体待实现。
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

// 原441DB0：独立载荷、四次坐标赋值/投影；4417E0已有本体，挂件尾442F70仍待实现。
YobiAction* Sub_441DB0(
    std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction);
// 原448040：独立载荷、四次坐标赋值/投影与tick；447E60父分派及十种模式已有本体。
YobiAction* Sub_448040(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t direction, std::int16_t mode, std::int16_t value02,
    std::int32_t objectIndex);
// 原 519D61–519E14：主 action 已非空，条件清八个附件及文本；不释放主 action/payload。
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
// 50A3E0/46AFE0：先读目标X，重读同一共享槽后读目标Y；入口非空由调用方处理。
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
// 原 5182B0：先查询图形偏移，再按低 W8 选择 +2C8/+2CC 差值。
std::int32_t Sub_5182B0(YobiAction* action, std::uint8_t axis);
// 原518300：先查询图形偏移，再按挂件kind系数截断；不改变graphicOffset。
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

// 原 Yobi::MarkDeath@[0x404D00,0x404D10)，只置死亡标志并返回原指针。
YobiAction* MarkDeath(YobiAction* action);

// 原43BFF0：场景10且回合一致时，按结束位给同源20槽非空对象flags置40H。
void Sub_43BFF0();
// 原43C560：发布目标标志，依原序按HP/阵营选择并处理前后排遮挡。
std::int32_t CheckBattleCommand_43C560(std::int32_t position, std::uint32_t flags);
// 原43C060：逐槽清选择位、置绘制位及cacheIndex=-2，不销毁对象。
void FlushBattleEntries_43C060();

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

// 原 +5C/+60：队列绘制按轴读取 F32，不向 render 暴露 action 布局。
float GetYobiActionScaleX(const YobiAction* action) noexcept;
float GetYobiActionScaleY(const YobiAction* action) noexcept;

// 当前角色预览/槽位调用所需的语义字段访问，不暴露原 x86 偏移。
// 46F1D0 创建效果对象后清 +8，帧推进后读取 +20 图形索引。
void ClearYobiActionCallback(YobiAction* action);
std::int32_t GetYobiActionField20(YobiAction* action);
// 原472B64–472BD4：先掩码后颜色，原位更新并借用同一对象的描述符。
const render::GraphicDescriptor16* ApplyWindowAnimationDrawMode(YobiAction* action);
// 原宠物预览直接借用+2F8，不执行窗口动画的描述符改写。
const render::GraphicDescriptor16* BorrowYobiActionDrawMode(const YobiAction* action) noexcept;
void AddYobiActionDrawFlags(YobiAction* action, std::uint8_t flags) noexcept; // 原+305低U8置位，不清其他位。
// 原 +2E4；51A5B0 读取，独立于播放翻转值 +2E0。
std::int32_t GetYobiActionField2E4(const YobiAction* action) noexcept;
std::int32_t GetYobiActionField2DC(const YobiAction* action) noexcept; // 窗口12按S32位型比较同源播放圈数。
const char* BorrowYobiNameC4(const YobiAction* action) noexcept; // 原+C4名称，只读借用同一对象。
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
void SetYobiActionField309(YobiAction* action, std::uint8_t value); // 原描述符替换掩码。
void SetYobiActionField310(YobiAction* action, std::uint8_t value); // 原掩码替换值，与309独立写入。
void SetYobiActionField30C(YobiAction* action, std::int32_t value);
std::uint8_t GetYobiActionField30F(YobiAction* action);
void SetYobiActionField30F(YobiAction* action, std::uint8_t value);

// 原 Yobi::FrameMove@[0x517880,0x517D9D)：参数速率、严格 ==1 停播及低 W8 静图翻转开关。
std::int32_t Sub_517880(
    YobiAction* action,
    std::int32_t value1,
    std::int32_t value2,
    std::uint32_t value3);

// 原 Yobi::PlayFrame@[517DA0,5182A1)：使用对象播放速率；静图仅小于 100000。
std::int32_t PlayFrame_517DA0(YobiAction* action, std::int32_t stopAtEnd);

} // namespace detail
} // namespace cg::login
