# Yobi剩余魔法数字持续分析记录（YMN09起）

日期：2026-10-25。用户要求持续分析，能证实用途的定义枚举并替换引用，证据不足保留说明，完成后执行build.bat编译验证，无可继续分析时汇总报告。

## 执行边界与裁决

按既有工程委托，先记录各批SPEC、证据和取舍，再由本线程实施、静态核对、编译及精确提交。仅处理Yobi及其既有接口中的已证离散解释域；保留原整数类型、布局、全部值、未知输入分支、初始化、解析／调用／RNG顺序。连续坐标、计数、时长、缩放系数和表容量不为消除数字强行定义枚举。业务名未证时按已证图号、字段或行为命名，不把同值数字视为同域。

初轮已扫描各CPP的裸数字case和mode／type／phase／subState／kind比较，候选包括状态图号及工厂模式、消息内部阶段及跳舞模式、地图挂件／绘制模式、环境类型、碰撞门禁、窗口样式和共享标志。已有BSI01明确保留的技能业务名继续受证据约束，不能以同处理入口或同图号判定同一技能。后续按实际结果追加各批及最终剩余清单。

## YMN09：状态图号及两类特效模式

在既有battle_effect.hpp的detail域新增三组17项S32枚举，不新增头依赖。BattleStatusGraphic共13项：Poison=110075、Sleep=110076、Stone=110077、Inebriety=110078、Confusion=110079、Forget=110080；DefenseUp=103018、DefenseDown=103019、AttackUp=103020、AttackDown=103021、AgilityUp=103022、AgilityDown=103023、RecoveryUp=103033。这些值是图号，不是特效工厂type或角色状态编号。

BattleAttachedEffectMode两项为DisplayImage22326=1、DisplayImage22327=2，只证明选择两张图；模式1在BC两动标志消费点使用，模式2见SUP分支，尚不足以给两个模式统一扩写业务名。BattleStatusEffectPlacement两项为Default=0、RaisedAndScaled=1；后者置deltaY=-50、scaleX／Y=0.3，其他模式仍走原deltaY=0分支，不补缩放值。

替换上述图号的直接创建、状态匹配、既有表项、图号连续区间边界和跳舞模式加基图号处；保留type+110074、type+103017等独立算式偏移。替换两种mode的所有已知调用和内部比较；现有对象只换图并返回空、分配失败、扩展状态跳过基础状态位写入、恢复能力图103033映射为显示图103018等路径保持。

原端证据：

- 41F660从arg_4读图号，41F684检查归一化索引，41F68D经41F7E0起六项W32表分派。110075～110080实际目标依次为41F694、41F6AA、41F6C0、41F6D6、41F6EC、41F702，case注释和目标均已核对；分别写FDC的10h／20h／40h／80h／100h／200h和13D0的1～6，分配前写入顺序保留。
- 41FE00在41FE4F～41FEC3依次比较103018～103023及103033，写对应4000h～100000h状态位；41FEDD把103033改成103018供显示。与当前已具名BattleActorFlag及参数状态生产一致，不猜图像内容。41FF5F仅判第三参数等于1，41FF66／41FF6D／41FF70写-50／0.3／0.3。
- 41FBE0在41FBF8和41FC61判第二参数为1，分别在已有／新对象路径选择22326，否则22327。当前调用只传1或2，未知值仍走原else，不新增默认枚举或校验。
- 当前BC输入和状态同步分别在battle_message.cpp与battle_effect.cpp生产图号，battle_attack.cpp在已证中毒／混乱分支创建减益图；battle_cast.cpp按跳舞mode或参数状态生成图号，Lua操作的图号边界也使用同一6图范围。只具名边界，不调整校验或操作协议。

每批验证将排除新增声明／注释，保留字符串，把新标识符逆映射成原数字逐词核对；另核取值、顺序、唯一性、同域引用、UTF-8、原换行及历史进度前缀。随后执行build.bat Debug配置、编译及链接，结果按批追加。不测试、运行客户端／算法、安装依赖、计算摘要或改动IDA／外部资料。

YMN09实施：五份CPP及既有battle_effect.hpp新增三组17项，替换77处数字（图号58、附属模式5、放置模式14）。排除新增声明／注释、保留字符串的逆映射逐词核对通过；取值／顺序／唯一性、引用闭合、原整数接口／全部分支、UTF-8、原换行、历史进度前缀及差异检查通过，开始编译验证。

YMN09验证：build.bat Debug的MSVC x64配置、编译及链接成功，退出码0，脚本报告build/windows-msvc-x64/Debug/cg2d_re.exe。本次输出仍有battle_message的amount及battle_effect的result可能未初始化的既有C4701，原路径保持。提交范围限定六份源／头、本文及唯一进度，共8份文件；其他候选继续分析。

## YMN10：地图绘制解释域

裁决七组29项：YobiGraphicAxis为U8的X=0／Y=1，置于既有YobiRuntime.hpp，替换两个原点函数的比较及所有调用，包括跨轴残影传参和窗口预览四处调用；参数仍为U8，跨轴残影的Y偏移继续取X轴，不纠正原行为。

MapGearKind为S32：MainGear=0、StandOrDown=1、StandLoop=2、DirectionalBatch10=3、DirectionalBatch11=4、DirectionalBatch12=5、AdjacentImagePair=6，置于map_actor.hpp。0对应主挂件，2强制站立，3～5走方向／动作分支并以kind+7选择绘制batch，518300分别使用1.75／1.5／1.25原点系数；1及6沿原站立／倒地分支。玩家更新的6额外生成图号+1的第二挂件，与BattleAuxGearType的6不同域。animation.cpp为此增加既有map_actor.hpp引用，其余依赖不扩。

environment.cpp的detail域定义三组S32：SceneEffectGraphicType为Image14645=100、Image14646=101、Image301122=201、Image1002065=202；SceneDriftVariant为FromPositiveWorldX=0、FromNegativeWorldX=1、FromNegativeWorldY=2、FromPositiveWorldY=3、RippleWithExitVariant0=4，最后一项只证明帧表第4行按index读取、附加波纹及复用变体0退出规则，未证入口方向，生成入口仍只随机0～3；SceneEffectFrameGroup为Overlay=0、Drift=1，保持表及读取次数。

map_render.cpp局部S16的ActorOverheadTextSlot六项为NameC4=1、TitleE1=2、Text144=3、Text102=4、Text123=5、Text31C=6，只表示远端角色已证文本字段，后三类业务名未证。window.cpp局部S32的WindowFrameGraphicGroup三项为Image245300=2、Image245310=3、Image245380=4，只解释winId右移1后的分组，原winId、负样式、tileStyle和基础22620／22630分支保持。

证据：5182B0的5182D2／5182E6和518300的518355／51837C分别判低字节0／1，查询后的无效轴仍返回0；51830D起三次比较落到51832C／518322／518318写三个系数。4412D0在441301判kind2、44130A／441310筛3～5；当前441570按kind+7绘制，地图解析与玩家更新分别写两路kind字段，未据同值复用战斗枚举。

负环境类型在448DFA归一化type-100，449070起103字节压缩索引再查44905C的W32目标表，100／101／201／202实际落448E19／448E20／448E27／448E2E，分别写四图；第二张偏移分派经4490EC索引及4490D8的W32表落448ED2／448EE0／448EFF／448F1E，目标及case已核。漂移生成449261通过4493AC四项W32落449268／4492B7／449309／44935B，与四种世界坐标入口对应；退出4494CC经449554四项W32落4494D3／4494F3／449513／449533。mode4特例及低两位退出判断均保留。窗口5044E0在5045D3按winId>>1判断2／3／4，分别于5045ED／5045E4／5045DB写245300／245310／245380，未改样式含义。

YMN10实施：八份CPP及两份既有头新增七组29项，替换121处数字（轴64、地图挂件22、环境类型8、漂移14、帧组3、头顶文本7、窗口分组3）。地图kind6原有倒地清图消费也具名，第二挂件创建与清图顺序保持。排除新增枚举及animation.cpp的一处既有头引用后，十份源／头逆映射逐词一致；各项数值／顺序／唯一性、同域引用、UTF-8／原换行、历史进度与差异检查通过，开始编译验证。

YMN10验证：build.bat Debug的MSVC x64配置、编译、链接成功，退出码0，报告同一cg2d_re.exe路径。共享头触发相关模块重编译，仍有Yobi的amount／result／lastAmount／statusType／selfFlags／relativeX／relativeY，以及WindowBank／Contacts／Shop／Trade／Skill原路径的C4701，另见WindowShop原S32到S16的C4244；没有改变这些路径。精确提交十份源／头、本文及唯一进度，共12份文件，继续消息内部模式及阶段分析。

## YMN11：跳舞内部模式及消息子阶段

裁决六组32项S32。共享BattleDanceMode的19项按已证处理命名：0～5为Poison／Sleep／Stone／Inebriety／Confusion／Forget；6为ApplyDamageUpAndAttackAnimation；7为SwapActorAndPetPositions；8为ShrinkAndRemoveTargets；9为InstantDeath；10为RecoverDebuff；11为Revive；12为SelectedActorSpinAttackAndExit；13为SelectedActorHealAndExit；14为SetHpOrFpPairs；15／16为DecreaseHpTargets／DecreaseFpTargets；17为SetHpFpOne；18为ReadTargetFlags。除已证状态和资源操作外不猜舞蹈名称，Mode6等既有helper名保持。

共享BattleMSkillPhase三项为ReadAndQueue=0、WaitForTurnReady=1、WaitForEntryReady=2。0和1对应SKL解析／回合就绪，2为已排队目标就绪门禁；保留最后参与者ECX推进、原技能集合和回退到1的顺序，Lua接管SKL的同域阶段也引用此枚举。两组共享枚举置于既有battle_base.hpp，不改变字段或整数参数。

battle_message.cpp内部另定义四组：BattleMQueuedPhase两项ReadAndQueue=0／WaitForTurnReady=1，仅用于Transition／Status／POS／ITM／GMN／RID既有两阶段门禁；BattleMMonsterPhase三项ReadActor=0／WaitForActors=1／WaitForDefenderNotBusy=2；BattleMChatPhase两项ReadAndShow=0／CountDown=1；BattleMGmnMode三项RemoveActorAndRevealTeam=0／EnterWithEntrance2=1／EnterWithRandomEntranceAndEmitter=2。CMB继续使用独立枚举，统一初始化和其他消息子状态不套用本批枚举。GMN的其他非零模式仍创建对象但入口参数保留0，模式0预扫描与执行同域具名。

证据：42CF73写5A4A34，42CFDD及42E901把同值写角色1048。42D0D0起依次判0～5、9～11和18并到42E80D的目标／标志公共读取；其他分支分别于42D13A判6、42D285判7、42D6B4判8、42D7D1判12、42DD12判13、42E015判14、42E1E4／42E43B判15／16、42E69E判17。当前队列明确对应增伤／换槽／缩退／即死／洁净／复活／旋转攻击退出／治疗退出／成对设值／HP或FP减少／HP与FP置1；18只走目标标志与公共收尾。0～5经141动作以mode+Poison基图创建六种已证减益，预扫描19项分派、实际执行19项及cast中的12／13门禁同域。

原SKL的42A76C读取+58，0进入42ACFD解析、1进入42A949等待，2进入目标计数检查，42A8CC／42A93D写1；当前五类生产与Lua路径保持。MON在4288FB读取+58，0／1／2分支分别解析、等待角色及重试旧配对槽；忙碌时保存2，完成后显式写1，不改成递增。CHT在42A22E判0／1，原定时递减完成路径保持。GMN在437207判0／1，等待20／21槽不忙后再判回合就绪；437290读取mode，43729C判断0决定退场，其余路径1指定入口2、2随机入口并创建421700发射器；具体GMN业务身份仍未证。

YMN11实施：七份CPP及既有battle_base.hpp新增六组32项，替换91处数字（跳舞模式48、SKL阶段20、两阶段消息门禁12、MON阶段5、CHT阶段2、GMN模式4），没有新增头依赖。八份源／头逆映射逐词一致，取值／顺序／唯一性、引用闭合、完整字符串、原整数类型／控制流／顺序、UTF-8／原换行、历史进度和差异检查通过，开始编译验证。

YMN11验证：build.bat Debug配置、编译和链接成功，退出码0；仍有既有C4701（amount／result／lastAmount／statusType／selfFlags），原未初始化分支保持。精确提交八份源／头、本文及唯一进度，共10份文件；其他候选继续分析。

## YMN12：就绪策略、效果门禁、变身阶段及Lua操作

裁决八组19项S32。BattleTurnReadyPolicy两项AllowInactiveSkip=0／CheckAllSelectedEntries=1，置于battle_base.hpp；只表达4394A0是否允许FE0 bit1、aliveFF8及角色phase组合条件跳过参与者，不把这些字段扩写为死亡状态。BattleArcShotMode只定义ResolveProjectileHit=0，原419DA0的其他非零输入仍选择动作243；当前调用只传0，没有臆造1号枚举。BattleTrailRenderMode两项LeaveFlag20000=0／AddFlag20000=1，仅表达41E7C0的mode==1追加20000h位，不给该位猜业务名。后二组置于battle_effect.hpp。

battle_impact.cpp内部BattleImpactPhase两项WaitForTargetIdle=0／Running=1，仅用于41A9E0／41AF00／41B440／41BB00／41C3B0／4219D0六类效果的首次目标待机门禁；不解释全局主场景phase8，也不把新建动作的统一零初始化套用此枚举。

battle_status.cpp内部对共享临时字段FD4分三域：BattleRebirthEnterPhase的Start=0／FadeAndWaitTransition=1／CreatePersistentEffects=2，对应163的进入分支；BattleRebirthExitPhase的PrepareFadeIn=0／FadeIn=1，对应163的退出分支；BattleRebirthReturnPhase的StartTransition=0／FadeInAndMarkEffectDead=1／FadeInWithoutMarkDeath=2，对应164回原动作。后者2只证实消费行为：淡入结束清保存引用但不主动标死，生产来源未闭合；保留原>0且<=2门禁，不新增2的写入。

battle_lua_skill.cpp内部LuaBattleOperation四项SetHp=1／ApplyDebuff=2／ClearDebuff=3／AppendHit=4，以当前Lua读取—校验—暂存—提交为证，不声称原EXE存在同一操作协议。只替换opId比较及输入上下界，保留其int字段、初值0、错误路径、值校验、命中顺序及全部Lua字符串。

证据：4394CD读父phase、4394D3判24，4394D8判enable零，4394DC～4394F1检查FE0／FF8／角色phase组合；当前所有调用的0／1及条件参数同域，超时1200仍为数量参数。419E8A／419E8C判最后参数零，419E97写动作16，非零于419E8E写243；41E827仅判第四参数为1，41E829 OR20000h，随机及其余标志操作不变。41A9E9／41A9EF读并判自身phase0，41AA07／41AA0A判目标phase0，41AA15再写自身1；当前其余五函数具有相同门禁，目标仍用BattleAction_Idle。

4113C7读取FD4，进入分支依0／1／2初始化、淡变／等待及创建持久效果；4116A6写2，进入结束清0。当前163退出支路分别准备淡入、增加alpha后清0。411A6D读取返回阶段，411C97写1；164对1／2的不同标死行为及原保存phase恢复顺序保持。Lua的opId生产由LuaRequireIntField要求1～4，提交分别写readiness、写已暂存减益、清异常、追加命中及反应队列，当前逐项对应。

保留AppendBattleSkillTail_424500的mode==0：调用方传入完整解析数值（level／primaryValue），并非已证有限模式集合，不能擅造0／1枚举。其他计数、效果资源号及全局主场景phase8仍独立处理。

YMN12实施：九份CPP及两份既有头新增八组19项，替换77处数字（就绪策略16、弧形投射5、尾迹8、效果门禁12、变身进入6／退出4／返回6、Lua操作20），没有新增头依赖。十一份源／头逆映射逐词、各项数值／顺序／唯一性、引用闭合、字符串与原控制流／顺序、UTF-8／原换行、历史进度和差异检查通过，开始编译验证。

YMN12验证：build.bat Debug配置、编译、链接成功，退出码0；仍有既有C4701（amount／result／lastAmount／statusType／selfFlags），原路径保持。精确提交十一份源／头、本文及唯一进度，共13份文件，继续核查剩余分派与位标志。
