# Yobi未定字段与联合体分析 SPEC（YUF01）

2026-10-25。用户要求再分析Yobi未确定变量及union项，随后要求完成后执行build.bat编译验证。本步以当前源码的生产／消费为入口，在当前已打开的cg2d.exe数据库只读核实关键地址；不把IDA既有字段名当作原始业务声明。

## 范围与裁决

覆盖YobiAction仍模糊的字段、BattleActorPayload／辅助载荷的保留项，以及src/Yobi当前六组union。未知包括三种不同情况：尚无业务消费依据、已证跨场景复用、用途已证但名称未反映。只有最后一种按行为命名；前两种补注释及边界，不为凑业务名拆字段或添加union。

依本线工程委托实施以下五个纯名称调整，类型、成员顺序、布局、初始化和读写／调用顺序保持；现有函数名、Lua键、协议及资源格式不变。

| 对象 | 旧名 → 新名 | 已证生产与消费 |
|---|---|---|
| YobiAction +1D8 | field1D8 → hoverTextColor1D8 | 玩家51A090、地图445350／445D20和43DD00附属实体承接颜色数值；507140发布到D68AC8，4731E0消费低BYTE作为底栏悬停文字色。完整S32保留，不改成颜色BYTE。 |
| YobiAction +2E4 | field2E4 → overheadMarkerMode2E4 | UpdateMapActorMode生产0／1／2；442F70的头顶标记在1／2时取+2E8自定义图，否则取默认图。51A5B0／442C00在模式2保留队长相关标记位；不据此把1／2另起未证业务名。 |
| MapCharaPayload +00 | values → objectIds00 | 4408C0写入五个对象编号；43F290／43FE00与当前玩家objectIndex比较，否则传FindObjectYobi_4462B0。不是world表槽下标，也不是对象地址。 |
| MapTransitionSlots18::values +18 | state18 → initialized18 | 当前模式2～9的对应回调在零时置1并执行一次初始化；后续按非零更新。保持S32及原比较，不改成bool。 |
| BattleEffectThirdArgument的数值臂 | offsetY → yValue | type47直接作为绝对Y，部分类型加源对象Y／offset44；旧名误示所有类型都是偏移。按type解释为S32 Y数值，与existingAction的指针臂互斥。 |

43DD00的同源局部value1D8与Yobi身份复制接口的value188参数也明确为hoverTextColor；外部PlayerRuntimeSlots::value188、WorldObjectRecord::value40／value48仅作为来源，不在本轮重命名。它们的位宽转换和当前来源读取时机保持。

## 本轮原端证据

- 0x51A48E取第四参数，0x51A495完整写入对象+1D8；0x5076FF／0x507708、0x50772C／0x507732等把+1D8发布到D68AC8；0x473363读该全局低BYTE，0x47336E写文字元素颜色。当前WindowBottomBar的mainText->style.color承接同一消费。
- 0x443276取+2E4，0x443283／0x443288比较1／2，0x4432DD取+2E8传444000创建标记。0x51A5C0严格比较完整+2E4与2，0x51A5C9只在其余路径清0x100位。命名为头顶标记模式，不外推为角色类别或全部附件类型。
- 0x4442A2绑定4441A0，0x4443FD写对象+10完成槽；0x43F319写成员序号+1D0，0x43F320写+10父对象。这两条+10路径的指针层级不同。
- 0x48AD65写+2C回调，0x48AD6D写+1D0最大数量；0x4759F4则写+1D0完成回写槽，0x475932在窗口展开结束时读取该槽。相同WindowOpening载荷类别也不足以决定+1D0应读哪一臂，仍须结合创建入口／callback。
- 0x44723C读过渡载荷+18，0x447247置1，0x44724E清+1C；后者在当前源码只有零写入，无业务读取。原448040在0x448220／0x448223均写零，当前x64承载按mode选择对应联合体成员初始化；不把这个本机成员选择声称为原端额外分支。
- 0x41CD12比较type与47、0x41CD15按无符号大于跳到已有对象公共尾；结合既有[BFTN01](cg2d_battle_effect_type_naming_spec.md)及当前case47直接发布第三槽为Y，确认数值／指针两臂边界。当前所有直接调用选择数值臂，不能据此删除已有对象分支。
- 0x41EF44／0x41EF4A、0x41EF5C／0x41EF62及0x41F01C／0x41F022只是复制+290／+298／+2D8，不构成这些字段业务含义的证据。value190／value1A0在当前源码仍仅有自身构造和复制；map_render同名value190属于PlayerRuntimeSlots，不能误记为YobiAction的读取。

## 六组union的有效成员及所有权

| 联合体 | 成员／选择依据 | 已证用途及限制 |
|---|---|---|
| YobiPayload，原对象+0C | raw与12个具体载荷T*；由本机payloadKind选择 | 12类为BattleActor、BattleScene、BattleRideCarrier、BattleRingEmitter、BattleFollowOffset、BattleTransitionEffect、BattleText、MapActor、MapChara、MapTransition、WindowOpening、WindowUi。通用NewYobiAction按类别写有效成员，DeleteYobiActionStorage按同类别取回分配地址。raw只用于None／Raw，不能跨臂读取来做任意指针擦除。类别为本机扩展，不是协议标志。 |
| YobiAction原+10 | completionSlot10 | YobiAction**完成回写槽，444260创建、4441A0结束时写空；槽及其指向对象均非本字段所有。 |
| 同组+10 | observationParent10 | 43F290／43FE00创建观察组子成员，指回MapChara父对象；非拥有YobiAction*。 |
| 同组+10 | battleAttachmentParent10 | 战斗附件、骑手、粒子／跟随特效的非拥有父引用；按各工厂与回调解释。 |
| 同组+10 | battleOpponent10 | 命中链保存来源／对手，416DD0及412900等消费；与附件父引用不同。 |
| 同组+10 | battleCastEffect10 | 施法动作暂存工厂返回的效果引用，后续标死并清槽；本字段不直接释放对象。 |
| YobiAction原+2C | phase／numberWindowCallback2C | 普通对象S32阶段与48AC60数量窗口回调互斥；后者为本机函数指针，x64不能按S32搬运。按callback家族选择，不按phase数值猜函数地址。 |
| YobiAction原+1D0 | field1D0／windowCompletionSlot1D0 | 数值臂承接等级、数量上限、观察成员序号、预览帧计数、转向符号和序列延迟；475950窗口展开家族则是YobiAction**完成槽。数值臂不是单一level。 |
| BattleEffectThirdArgument | yValue／existingAction | type0～47数值臂；type>47指针臂直接进入公共尾。后者为已有对象借用，不新增分配、拷贝或所有权转移。不能把yValue强转指针，也不能把pointer窄化成原S32。 |
| MapTransitionSlots18，原载荷+18／+1C | attachments.action18／action1C，或values.initialized18／state1C | mode0／1使用两个非拥有特效引用；其他模式使用S32初始化门闩及保留状态槽。state1C仅见零写，不命名成第二阶段。x64附件结构为16B、数值结构为8B，两者由外层mode区分，不按原x86偏移交叉读取。 |

当前base.cpp复制YobiAction的三个匿名联合槽分别按原生槽宽memcpy；YobiPayload随类型标记复制。这个承载方式保留全部位型，不代表可同时解引用所有成员。仅拥有载荷分配及其专用内部资源；各YobiAction*／YobiAction**观察引用继续由Yobi链表的生命周期管理，不能沿引用递归释放。

## 仍保留原名的字段

以下结论主要来自当前源码已存在的读写链；限定“未见”指本轮源码范围，不声称原EXE全库不存在其他引用。

| 字段／保留段 | 已证用途／缺口 | 本轮处理 |
|---|---|---|
| YobiAction::field58 | 观察／地图保存方向；骑乘插值计时；命中特效延迟；发射器完成计数；部分动作逐步计数。 | 多用途S32，不统一命名成direction或timer。 |
| field1D0 | 见+1D0联合槽；同一整数在不同callback家族含义不同。 | 保留数值臂名，补具体用途。 |
| value1D4 | 战斗附件位置、观察轨迹帧索引、铺屏效果反向开关。 | 保留共享槽及原存储，不添加同类型union别名。 |
| counter1DC | 动态图标寿命／序列计数、观察动画覆盖值；部分命中链读取恢复数值。 | 不视为统一计时单位，也不收窄范围。 |
| value190／value1A0 | 当前只有自身构造及复制；两个LegacyCryptInt各有构造／RNG合同。 | 业务未定，不能删除、合并或改成普通整数。 |
| field2D8 | 41EB10复制、骑乘同步复制，410E30／4124E0清零；未证位含义。 | 保留U32，不能仅据邻近动画字段命名为动画标志。 |
| bytes155；extraState290.bytes290／bytes298 | 只保留／复制原字节；extraState290仅+294的施法时长已证。 | 保留字节容量、顺序与完整复制，不猜成指针／字符串／计数。 |
| BattleActorPayload::valueFD0／valueFD4／valueFF0 | 等待、一次性状态、碰撞编号、计数及保存phase，部分动作跨阶段转交。 | 沿[YVN01](cg2d_yobi_value_naming_spec.md)继续保留，不拆成互相失联的字段。 |
| value1030 | HP／FP数值、效果选图参数及动作模式。 | 不统一为damage。 |
| value1038 | 409680强制命中门禁，40E560族兼作阶段标记。 | 多用途整数，保留。 |
| stateFE0 | 位0由4238F0按玩家／宠物／相册关系生产，4209C0控制属性效果；位1参与死亡退场／忙等待路径。 | 是组合位用途，不把整个字段改成单一阶段枚举；其余位业务不推测。 |
| aliveFF8 | 与Dead位、更新节流、死亡处理和忙等待条件联动。 | 现名不能据字面理解为角色是否存活，补边界注释。 |
| bytesFFC／bytes104C／bytes1081／bytes13D4 | 载荷清零、整体复制或保留分配范围，缺少独立业务消费。 | 四段共15B继续显式承载；不删、不按邻接字段猜地址。 |
| BattleScenePayload::value70 | gBattleMValue5ABA7C快照；既保存位置也承接累计技能的重复标记。 | 不统一为施法者位置。 |
| BattleRideCarrierPayload::bytes10 | 原18H载荷末8B；当前有清零，无具名业务读取。 | 保留8B。 |
| BattleTransitionEffectPayload::bytes04 | 原12B载荷尾8B；419BE0不消费此载荷。 | 保留8B，不推断为偏移数组。 |
| BattleTextPayload::bytes14 | 原18H文本载荷末4B，未见独立读取。 | 保留4B，不猜状态。 |
| WindowOpeningPayload::value1C／bytes22 | 前者三处初始化零与布局断言，后者保留2B；未见业务读取。 | 保留，不收缩载荷。 |
| MapTransitionSlots18::values.state1C | 构造与447230写零，无当前业务读取。 | 保留S32并注明，禁止推测第二阶段。 |

MapActorPayload的action04已证为头顶标记，action30为主挂件，action34／38为两路附属挂件，action44为骑乘对象，均为非拥有引用。action0C／10／4C已证为按状态位和图号创建／绘制的图标，但具体业务图标名称未核；action2C为43DD00解析的附属地图实体，不按图号推断为某类宠物。本轮只补注释，不批量替换这些成员名。

## 验证与实施边界

逐词核对本项源码，逆向映射新名称并排除注释后与修改前一致；保留全部字符串、数字、类型、初始化、布局断言及控制流。确认五个旧成员在本轮适用范围无残留，未把其他结构同名值或通用offsetY／values变量误改。显式UTF-8，逐文件保留LF／CRLF。

按用户本轮新增授权，在分析和源码整理完成后执行build.bat Debug，核对配置、编译与链接的实际结果；不新增／执行测试，不运行客户端或算法，不修改IDA或外部资料，不计算摘要。构建结果另记，编译成功不代表所有未定字段、union生命周期或画面均已验收。

## 实施与静态核对

- 五个成员及同源局部／参数共92处标识符替换：field1D8为12处，field2E4为7处，MapChara的values为18处，过渡state18为18处，特效第三槽offsetY为32处，两组局部／参数为5处。14份源／头发生名称调整，另两份头仅补说明，共16份源／头。
- 已补通用共享槽、六组union的有效成员边界、地图附属引用、stateFE0及aliveFF8的用途说明。field1D0、field58及其他多用途字段继续共用原存储；业务未定字节及只见零写的state1C保留。未新增union或替代状态。
- 16份源／头排除注释并逆向映射新标识符后，与修改前逐词一致，数字、字符串、类型、布局断言、初始化、控制流和调用／读取顺序保持。适用旧成员、同名外部结构排除、UTF-8及逐文件原LF／CRLF、Git差异检查通过；编译实际结果见下节。

## 本轮编译修正

首次build.bat Debug配置通过，编译在battle_scene.cpp的ReturnBattleActorsToFormation_43C910报告C2065：BattleAction_EnterFormation缺少detail限定；构建退出码1。该成员已经定义在cg::yobi::detail中，当前调用处位于cg::yobi。只补detail::，不增枚举或改动作号；原0x43C921检查入场完成位，0x43C92A向+2C写2，与成员值一致。此额外一份CPP单独静态核对限定修正，随后重跑同一构建。

第二次build.bat Debug使用既有MSVC x64预设，配置、编译和链接成功，退出码0，产物为build/windows-msvc-x64/Debug/cg2d_re.exe。本轮编译输出仍包含既有C4701与C4244告警；未改写相关未初始化路径或窄化规则。两次构建均未运行客户端或测试，未新增依赖、计算摘要或改变IDA会话。

最终包含17份源／头及本SPEC、唯一进度，共19份文件；精确提交本轮差异，保留既有未跟踪内容。本轮临时补丁记录核对归属后清理。五项名称与六组union的文档分析已经落实，保留段及多用途字段的未知语义继续保持未决，不以构建成功替代业务或生命周期验收。
