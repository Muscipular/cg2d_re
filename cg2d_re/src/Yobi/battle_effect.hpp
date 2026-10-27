#pragma once

#include "base.hpp"

#include "battle_base.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

// 419DA0仅证实调用输入0；其他非零仍选原动作243，不臆造编号。
enum BattleArcShotMode : std::int32_t {
    BattleArcShotMode_ResolveProjectileHit = 0,
};

// 41E7C0只在mode1追加20000h位；该位业务名未证，其他模式不改该位。
enum BattleTrailRenderMode : std::int32_t {
    BattleTrailRenderMode_LeaveFlag20000 = 0,
    BattleTrailRenderMode_AddFlag20000 = 1,
};


// 原41F660／41FE00图号；与状态编号及工厂type独立，RecoveryUp沿原规则显示DefenseUp图。
enum BattleStatusGraphic : std::int32_t {
    BattleStatusGraphic_Poison = 110075,
    BattleStatusGraphic_Sleep = 110076,
    BattleStatusGraphic_Stone = 110077,
    BattleStatusGraphic_Inebriety = 110078,
    BattleStatusGraphic_Confusion = 110079,
    BattleStatusGraphic_Forget = 110080,
    BattleStatusGraphic_DefenseUp = 103018,
    BattleStatusGraphic_DefenseDown = 103019,
    BattleStatusGraphic_AttackUp = 103020,
    BattleStatusGraphic_AttackDown = 103021,
    BattleStatusGraphic_AgilityUp = 103022,
    BattleStatusGraphic_AgilityDown = 103023,
    BattleStatusGraphic_RecoveryUp = 103033,
};

// 原41FBE0两种已知输入；其他值仍按原else显示22327，完整业务名未证。
enum BattleAttachedEffectMode : std::int32_t {
    BattleAttachedEffectMode_DisplayImage22326 = 1,
    BattleAttachedEffectMode_DisplayImage22327 = 2,
};

// 原41FF5F放置模式；仅1抬高并缩放，其他值保留原分支。
enum BattleStatusEffectPlacement : std::int32_t {
    BattleStatusEffectPlacement_Default = 0,
    BattleStatusEffectPlacement_RaisedAndScaled = 1,
};

// BFTN01：41CD00的创建类型，覆盖0～47；不是角色phase、技能号或顶层压缩分支号。
// BFTN02补充已证用途；未证业务名仍按图号／行为命名，证据缺口见对应SPEC。
// 函数仍接收U32，保留type>47使用已有对象的公共尾。
enum BattleEffectType : std::uint32_t {
    BattleEffectType_AttackClash = 0, // 图110009；动作115普通交锋效果，反击另用type3。
    BattleEffectType_FixedImage110063 = 1, // 图110063、回调41A160；资源宏有dorein线索，缺业务生产证据。
    BattleEffectType_MagicAttack = 2, // 攻击魔法共用表现；等级偏移+110067，高等级111013。
    BattleEffectType_CrossCounter = 3, // 按反击技能11的附加技术等级选110153／111053。
    BattleEffectType_DebuffSpell = 4, // 减益魔法表现；等级偏移+110071，高等级111063。
    BattleEffectType_BarrierSpell = 5, // 反射／吸收／无效类施放表现；110105族／111021，不等同技能402屏障。
    BattleEffectType_AttributeReversal = 6, // 属性反转表现；等级偏移+110161，高等级111001。
    BattleEffectType_PhysicalReflect = 7, // 查技能55技术等级，选110109／111025。
    BattleEffectType_PhysicalAbsorb = 8, // 查技能57技术等级，选110117／111027。
    BattleEffectType_PhysicalNullify = 9, // 查技能59技术等级，选110125／111023。
    BattleEffectType_MagicReflect = 10, // 查技能56技术等级，选110113／111024。
    BattleEffectType_MagicAbsorb = 11, // 查技能58技术等级，两分支均用110121，查询调用保留。
    BattleEffectType_MagicNullify = 12, // 查技能60技术等级，选110129／111022。
    BattleEffectType_Recovery = 13, // 恢复魔法表现；110176族／111019，保留Recovery扩展图替换。
    BattleEffectType_StatusHpRecovery = 14, // STV mode1的HP恢复表现；110165族，等级>=9创建19，不执行数值结算。
    BattleEffectType_CenteredLevelScaledImage110170 = 15, // 等级偏移+110170／111001、居中Y；未证业务生产及图族名称。
    BattleEffectType_Heal = 16, // 补血魔法表现；110085族，等级>=9创建19，随后做Heal扩展图替换。
    BattleEffectType_ParentBoundImage110184 = 17, // 图110184、层203、41A1C0；父死亡时标死，不跟随坐标，业务名未证。
    BattleEffectType_AmountTieredHpRecovery = 18, // 物品HP恢复表现；按1030的2000／3500阈值分图，具体药品名未证。
    BattleEffectType_ParentImageTierAura = 19, // 按父图及1D0等级选110184／111015／111020／0；14／16递归使用。
    BattleEffectType_ItemUse = 20, // 图110104；物品使用动作138的表现，完成后通知关联父对象。
    BattleEffectType_SkillActivation = 21, // 多技能发动侧110100族／111016；3100／3101先递减1040取等级。
    BattleEffectType_Seal = 22, // 封印／收回视觉图110097、41A1C0；保持创建坐标，不限定捕获成功。
    BattleEffectType_Summon = 23, // 召唤／进入视觉图110096、41A1C0；保持创建坐标，与22独立。
    BattleEffectType_SealCard = 24, // ITM mode6封印卡图110095、层8；41A270放大／缩小，不结算捕获结果。
    BattleEffectType_StatusRecoveryEmitter = 25, // 洁净魔法表现；顺序发射41A7D0，音效281。
    BattleEffectType_InstantDeath = 26, // 即死受术图110156、居中Y；技能86及98模式9使用，死亡结算另排队。
    BattleEffectType_DeathScytheTechnique2053 = 27, // 死神的镰刀技术号/100==2053：图111162、居中Y；技术业务名未证。
    BattleEffectType_DeathScytheTechnique2054 = 28, // 死神的镰刀技术号/100==2054：图111163、居中Y；技术业务名未证。
    BattleEffectType_HealingCast = 29, // 补血施放侧图110157，设备混合并居中Y；其他技能可共用。
    BattleEffectType_FixedImage110012 = 30, // life系列图110012；源浮点X、源Y加第三数值，缺业务生产证据。
    BattleEffectType_FixedImage110014 = 31, // life系列图110014；缺业务生产证据，与30／32保持独立编号。
    BattleEffectType_FixedImage110013 = 32, // 图110013；动画头仅有注释宏线索，业务未证，与30／31独立。
    BattleEffectType_ReviveEmitter = 33, // 复活受术表现；环形载荷、反向发射41A590、音效280，不在此恢复HP。
    BattleEffectType_SpellCasting = 34, // 施法槽效果，源1028施法图及扩展替换；保留原图音效／定位选择。
    BattleEffectType_SkillImpact = 35, // 按103C分派完整命中／移动初始化，保留Magic扩展图替换。
    BattleEffectType_RandomEmitter = 36, // 回调41A310随机发射；分配优先级取源位置低U8*10+1的低U8。
    BattleEffectType_ParameterChange = 37, // 能力变化表现；按等级选110189／111057。
    BattleEffectType_LevelIndexedImage111239 = 38, // 技能3001受术侧；等级1～7选111240～111246，其余111239，业务名未证。
    BattleEffectType_BombLevel = 39, // 按全局爆弹等级选110103～110100或111016，居中Y。
    BattleEffectType_RandomOffsetImage110474 = 40, // 技能402选侧生产图110474；依次取宽／Y偏移并随机定位，图族名称未证。
    BattleEffectType_PayloadImage102CType41 = 41, // 多技能直接取源102C；不能冠单一技能名，与42保持独立类型。
    BattleEffectType_PayloadImage102CType42 = 42, // 多技能直接取源102C；业务生产与41不同，不合并编号。
    BattleEffectType_CastingAttachment = 43, // 施法附件，现由SoloDance写13E4并排创建／清理；41A1C0，只复制创建坐标。
    BattleEffectType_FixedImage111179 = 44, // 图111179、技能2070命中链生产；技能业务名缺资料。
    BattleEffectType_FixedImage111188 = 45, // 图111188、技能2076命中链生产；技能业务名缺资料。
    BattleEffectType_FixedImage111191 = 46, // 图111191、技能2079命中链生产；技能业务名缺资料。
    BattleEffectType_AreaAttackImpactAtPosition = 47, // 动作255首目标的范围攻击表现；绝对XY、源102C、各缩放1.5，类型上界。
};

// YMN02：41E890／41E920的553A74图表行号；与41CD00的创建type独立。
// 只描述视觉图族；物理技能等可复用，参数与局部S32继续承接未知值。
enum BattleTiledEffectType : std::int32_t {
    BattleTiledEffect_Earth = 0, // 图28881～28888；地魔法及部分物理命中共用。
    BattleTiledEffect_Water = 1, // 图28889～28896；水魔法及酸液等共用。
    BattleTiledEffect_Fire = 2, // 图28897～28904；火魔法及部分技能队列共用。
    BattleTiledEffect_Wind = 3, // 图28905～28912；风魔法及MonWind等共用。
    BattleTiledEffect_Drain = 4, // Dorain／3037施法链使用，图28895与28904交替；不代表单次扣血结算。
    BattleTiledEffect_SharedRow5 = 5, // 图28913～28916；即死、冲击波及其他技能共用，独立业务名未证。
};

// 仅用于工厂SealCard效果的phase；动作154～156与41A270握手，不是角色动作或捕获结果。
enum BattleSealCardPhase : std::int32_t {
    BattleSealCardPhase_Expanding = 0, // 放大中；沿原对象构造的初始0，不额外初始化。
    BattleSealCardPhase_Expanded = 1, // 已放大，等待角色往复缩放结束。
    BattleSealCardPhase_Shrinking = 2, // 角色动作155写入，令卡效果开始缩小。
    BattleSealCardPhase_Collapsed = 3, // 已缩小，等待角色动作156标死／清槽。
};

// 419C10／419BE0过渡效果的phase；仍以S32存储，消费者保留非零判断。
enum BattleTransitionPhase : std::int32_t {
    BattleTransitionPhase_Playing = 0, // 调用单次帧播放，等待严格返回1。
    BattleTransitionPhase_HiddenAfterStop = 1, // 停播或动画不可用后隐藏；不代表对象已销毁。
};

// 原421570/412210/412260生产、421480消费；背景效果phase，保留贯穿及延后标死。
enum BattleBackgroundFadePhase : std::int32_t {
    BattleBackgroundFadePhase_IncreasingAlpha = 0, // 增加color2高字节，完成后贯穿显示分支。
    BattleBackgroundFadePhase_ShowingWithAlphaOverride = 1, // 清隐藏位并启用alpha覆盖，按原条件发布背景。
    BattleBackgroundFadePhase_DecreasingAlpha = 2, // 减少alpha，到零后贯穿隐藏及结束分支。
    BattleBackgroundFadePhase_HidingAndFinishing = 3, // 清覆盖掩码、置隐藏位，本轮继续标死。
    BattleBackgroundFadePhase_Finishing = 4, // 直接标死；阶段0/1发布背景后下一轮进入。
};

// YUF01：41CD00第三槽按type选择；0..47是S32绝对／相对Y，其他类型借用已有对象，不能跨臂读取。
union BattleEffectThirdArgument {
    std::int32_t yValue; // type47直接作绝对Y，其他消费者可能加源Y／offset44，不能统一叫offsetY。
    YobiAction* existingAction; // type>47的已有对象引用，直接进入公共尾，不分配或转移所有权。
};


// 共享原状态，只在本组实现中定义；原全局：0x0059E9D8。
extern std::array<std::int32_t, 1025> gBattleEffectPositions59E9D8;

// 原函数：0x0041E5C0。
void UpdateBattleAfterimage_41E5C0(YobiAction* action); // 递增FF0，新计数达到12才标死。
// 原函数：0x0041E5F0。
YobiAction* NewBattleAfterimage_41E5F0(YobiAction* source); // 原克隆路径无失败门禁，复用完整浅复制。
// 原函数：0x00419FA0。
void UpdateBattleOneShotEffect_419FA0(YobiAction* action); // PlayFrame(1)严格返回1才标死。
// 原函数：0x00418D80。
void UpdateBattleProjection_418D80(YobiAction* action); // 父FD0门禁仅影响移动，标死后仍播放/投影。
// 原函数：0x00418DF0。
void NewBattleProjection_418DF0(
    YobiAction* parent, float x, float y, float targetX, float targetY,
    std::int32_t imageNo); // 目标先截S32再转向；原三caller不消费返回，指定图号播放后额外移动两次（BPR01）。
// 原函数：0x00418F10。
void UpdateBattleArcProjection_418F10(YobiAction* action); // 固定父引用，标死后仍残影/屏幕投影，保留两次角度读取。
// 原函数：0x00418FF0。
YobiAction* NewBattleArcProjection_418FF0(
    YobiAction* parent, std::int32_t offsetX, std::int32_t offsetY,
    YobiAction* distanceSource, YobiAction* target, std::int32_t imageNo); // 原rand角度，距离分两次有符号除法。
// 原函数：0x0041E4D0。
void UpdateBattleScatterEffect_41E4D0(YobiAction* action); // BSEX01：移动/终帧标死仍投影，Y取样先于X发布。
// 原函数：0x0041E510。
YobiAction* NewBattleScatterEffect_41E510(YobiAction* parent, std::int32_t imageNo); // BSEX01：成功才三RNG；X发布后Y取样/W16零归，再发布Y。
// 原函数：0x0041E640。
void UpdateBattlePositionEffect_41E640(YobiAction* action); // PlayFrame(1)严格完成才标死，所有路径投影X/Y。
// 原函数：0x0041E680。
YobiAction* NewBattlePositionEffect_41E680(YobiAction* parent, std::int32_t imageNo);
// 原函数：0x0041E720。
YobiAction* NewBattleCoordinateEffect_41E720(float x, float y, std::int32_t imageNo); // 无父引用，不提前播放。
// 原函数：0x0041E7C0。
YobiAction* NewBattleTrailEffect_41E7C0(
    float x, float y, std::int32_t imageNo, std::int32_t mode,
    std::uint8_t priority); // 失败不消耗随机；成功绑定41E640，设备分支决定混合标志。
// 原函数：0x0041A160。
void UpdateBattleCompletingEffect_41A160(YobiAction* action); // 播放完成后通知父1024，标死仍投影。
// 原函数：0x0041A1C0。
void UpdateBattleParentEffect_41A1C0(YobiAction* action); // 父严格死亡时标死自身，仍播放并投影。
// 原函数：0x0041A200。
void UpdateBattleRisingEffect_41A200(YobiAction* action); // 固定父引用，未舍入Y比较；标死后仍播放/投影。
// 原函数：0x0041A270。
void UpdateBattleScalingEffect_41A270(YobiAction* action); // phase0放大、phase2缩小，以未舍入X判定阶段。
// 原函数：0x0041A310。
void UpdateBattleRandomEmitter_41A310(YobiAction* action); // 50次随机生成；父死亡早退，完成数通知父1024。
// 原函数：0x0041A530。
void UpdateBattleFallingEffect_41A530(YobiAction* action); // 未舍入Y超过nameY才递增父58，仍播放/投影。
// 原函数：0x0041A590。
void UpdateBattleReverseEmitter_41A590(YobiAction* action); // 132B数值载荷，每槽四粒子，倒序坐标。
// 原函数：0x0041A7D0。
void UpdateBattleIndexedEmitter_41A7D0(YobiAction* action); // 32次顺序生成，失败也递增nameX。
// 原函数：0x00420D50。
YobiAction* NewBattleRisingEffect_420D50(YobiAction* parent); // 先随机位置再分配，成功后可再随机混合标志。
// 原函数：0x0041CD00。
YobiAction* NewBattleEffect_41CD00(
    YobiAction* source, std::int32_t offsetX, BattleEffectThirdArgument third,
    std::uint32_t type, std::int32_t blend, std::int32_t link); // type见BattleEffectType；完整48类及已有对象公共尾。
// 原函数：0x0041E890。
std::int32_t DrawBattleTiledEffect_41E890(std::int32_t type); // BSTL01：共享相位先推进，X外Y内100格/每格fresh相位/末次返回。
// 原函数：0x0041E920。
void UpdateBattleTiledEffect_41E920(YobiAction* action); // 独立58相位/1D4反向，绘制后判寿命且仍自增。
// 原函数：0x0041E9D0。
YobiAction* NewBattleTiledEffect_41E9D0(std::int32_t type, std::int32_t lifetime); // maxHp/readiness依次临时构造赋值。
// 原函数：0x00419FC0。
YobiAction* NewBattleRandomEffect_419FC0(
    YobiAction* parent, std::int32_t imageNo, std::int32_t direction); // X/Y分别取随机并重读尺寸。
// 原函数：0x0041A0D0。
YobiAction* NewBattleScreenEffect_41A0D0(
    std::int32_t x, std::int32_t y, std::int32_t imageNo, std::int32_t direction); // 屏幕坐标直接创建，绑定完整419FA0，不提前播放。
// 原函数：0x004208F0。
void NewBattleRebirthEffects_4208F0(YobiAction* source, YobiAction* target); // BRAE01：1080低S8，首段后重取目标类型。
// 原函数：0x004209C0。
std::uint8_t NewBattleAttributeEffects_4209C0(
    YobiAction* source, YobiAction* target); // BRAE01：四W16非零匹配；U8掩码表示匹配并调用。
// 原函数：0x0041F660。
YobiAction* NewBattleBuffEffect_41F660(YobiAction* parent, std::int32_t imageNo); // BAS01：六图号先改FDC/13D0/13CC，分配失败不回滚；播放后fresh父载荷写1054再绑定父。
// 原函数：0x0041F800。
void UpdateBattleDebuffEffect_41F800(YobiAction* action); // BAS01：1054/105C共用借父回调，Play后重借父跟随；标死仍投影，Y转换后取屏幕偏移。
// 原函数：0x0041F860。
YobiAction* NewBattleReverseEffect_41F860(YobiAction* parent, std::int32_t imageNo); // BAS01：105C已有返空，设备后持flags两次发布；播放后fresh父载荷写105C再绑定父。
// 原函数：0x0041F950。
void UpdateBattleCenteredEffect_41F950(YobiAction* action); // BDE01：父严格标死后仍共同投影尾。
// 原函数：0x00419CB0。
void UpdateBattleOffsetFollowEffect_419CB0(YobiAction* action); // 父标死仍按屏幕坐标/S32偏移跟随并播放。
// 原函数：0x00419BE0。
void UpdateBattleTransitionEffect_419BE0(YobiAction* action); // phase0播放严格完成才置phase1/隐藏，不自动标死。
// 原函数：0x00419C10。
YobiAction* NewBattleTransitionEffect_419C10(YobiAction* parent, std::int32_t imageNo);
// 原函数：0x00419CF0。
YobiAction* NewBattleOffsetFollowEffect_419CF0(
    YobiAction* parent, std::int32_t offsetX, std::int32_t offsetY, std::int32_t imageNo); // 播放后按浮点坐标加偏移投影。
// 原函数：0x0041FA00。
YobiAction* NewBattleCenteredEffect_41FA00(YobiAction* parent, std::int32_t imageNo); // BDE01：1064已有即空返，S16半宽高/播放后发布/投影。
// 原函数：0x0041FB20。
void UpdateBattleAttachedEffect_41FB20(YobiAction* action);
// 原函数：0x0041FBE0。
YobiAction* NewBattleAttachedEffect_41FBE0(YobiAction* parent, std::int32_t mode); // 已有槽只换图并返回空。
// 原函数：0x0041FCD0。
void UpdateBattleStatusEffect_41FCD0(YobiAction* action);
// 原函数：0x0041FE00。
YobiAction* NewBattleStatusEffect_41FE00(YobiAction* parent, std::int32_t imageNo, std::int32_t mode); // 1068发布后执行缩放/下降。
// 原函数：0x0041FF90。
void UpdateBattleFieldEffect_41FF90(YobiAction* action); // BFL01：当前场景death严格1标死自身，仍投影；Y转换后再取屏幕Y偏移。
// 原函数：0x0041FFE0。
YobiAction* NewBattleFieldEffect_41FFE0(std::int32_t imageNo); // BFL01：B0已有只换图返空；两组索引先齐读坐标，播放后fresh场景发布，原投影尾。
// 原函数：0x00421480。
void UpdateBattleBackgroundFade_421480(YobiAction* action); // 完整五态淡变，同源背景发布与延后标死。
// 原函数：0x00421570。
YobiAction* NewBattleBackgroundFade_421570(
    std::int32_t imageNo, std::uint8_t step, std::int32_t publishBackground);
// 原函数：0x00422370。
void RestoreBattleStateEffects_422370(YobiAction* parent); // 固定入口FDC快照，保留成对共用清位掩码。
// 原函数：0x00421610。
YobiAction* NewBattleScreenEffect_421610(std::int32_t x, std::int32_t y);
// 原函数：0x004216B0。
void UpdateBattleScreenEmitter_4216B0(YobiAction* action); // 原rand先Y后X，生成后重取阶段并递减。
// 原函数：0x00421700。
YobiAction* NewBattleScreenEmitter_421700(std::int32_t phase);
// 原函数：0x00418B70。
YobiAction* NewBattleSpiracleShot_418B70(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
// 原函数：0x00419180。
YobiAction* NewBattleBlastWave_419180(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
// 原函数：0x00419390。
YobiAction* NewBattleRangeHit_419390(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
// 原函数：0x004195F0。
YobiAction* NewBattleBoomerang_4195F0(YobiAction* parent); // B22C01：原取样/几何发布，同步总回调推进后返回原生T*。
// 原函数：0x00419DA0。
YobiAction* NewBattleArcShot_419DA0(YobiAction* parent, const BattleEntryEffectRecord* effect,
    std::int32_t imageNo, std::int32_t count, std::int32_t mode);
// 原函数：0x004205F0。
YobiAction* NewBattleCastCopy_4205F0(YobiAction* parent); // BCPY01：独立战斗载荷与父借用，三载荷重取/末坐标发布；非浅克隆。

} // namespace detail

} // namespace cg::yobi
