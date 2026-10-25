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

## YMN13：BM结果位与组合掩码

裁决四组57项U32，置于既有battle_base.hpp的detail域：BattleResultFlag的31项仅覆盖客户端已有消费的位；BattleResultMask的22项保留原组合常量；BattleResultByte1Flag三项及BattleResultByte3Flag一项分别表示右移8位／24位后的字节掩码，保持原读取宽度、移位、括号及运算顺序。原普通S32字面量以static_cast<std::int32_t>引用，原U32字面量直接引用，避免改变整型提升和按位取反类型；字段／接口均保持原整数类型。

服务端include/battle.h的572～618行提供BM_FLAG各位定义；battle_event.c的反射／吸收／无效结果生产、客户端解析到effects1C.flags、FE4以及5A4A20／5ABA84，再到排队与表现消费形成对应。BC角色位、CHAR位、绘制flags及其他全局flags不混用。未出现客户端独立消费的bit26不新增成员。预扫描的flags、extraFlags、targetFlags逐入口按实际字段顺序核对，RID中角色预览位仍保持独立。

命名取舍：bit6／7依实际排队34／35命名BounceOut／FlyOut；bit8会播放210并设置flash1004后闪避，命名FlashDodge，服务端标签为MIKAWASHI，不另猜技能名。bit23命名HasAuxiliaryGraphics：客户端按此位消费两张图及两组图／数值共8字段，并在执行中更新骑乘／挂件；服务端WEAPON_BROKEN仅为对应来源，不将该位简化成纯图标。bit22／24／25分别按服务端装备标签及22330／22328消费命名WeaponDamageIcon／EquipmentDamageIcon／EquipmentBrokenIcon；bit30／31只据数字显示追加“+”“-”命名NumberSuffixPlus／NumberSuffixMinus。

原端只读核对：4063BD／4063F0／406423／40650A／40653D依次检查200000h、400000h、800000h、1000000h、2000000h并创建22329／22330／22328；附加读取推进后的末两块仍消费入口快照。40FBBE的1000h选择CrossCounter特效；40AA92的4与C00h选择格挡动画，40AC50的10与3000h控制后退结束速度。43BF38／43BF3B区分实际／预览读取，43BF6C起消费两图及两组3字段。41F1D7检查40000000h追加“+”，41F20D检查符号位追加“-”，与当前flags参数对应；不把其误归绘制对象flags。

组合掩码按组成位命名，不新增业务推断。FC1FFFDFh清除死亡、受伤、武器损伤、附加图形、装备损伤／损坏六类结果，其余位原样保留；不用补集表达式重写。高字节20h／80h分别是物理反射／无效，第二字节2是护卫，最高字节10h是再生保护，与低字节同值标志分开定义。准备按同域数据流逐处替换并进行包含字面量数值及S32／U32类型的逆映射核对，随后编译验证。

YMN13实施：十二份CPP及既有battle_base.hpp新增四组57项，替换832处数字（完整结果位707、组合116、第二字节8、最高字节1）；普通S32引用通过显式转换保持原类型。排除新增声明，保留全部字符串及原数值和S32／U32类型的逆映射逐词一致；各项取值／顺序／唯一性、引用闭合、原换行／UTF-8、历史进度及差异检查通过。首轮编译发现battle_display.cpp缺少声明头，退出码1；已补充既有battle_base.hpp引用，并补齐直接数组成员的漏引，重新静态核对通过，开始重新编译。

YMN13验证：重新执行build.bat Debug，配置、编译及链接成功，退出码0；仍有既有C4701（amount／result／lastAmount／statusType／selfFlags），没有改动对应路径。十三份源／头、本文及唯一进度共15份文件精确提交，继续对象标志及方向编号取证。

## YMN14：对象绘制标志、方向及颜色通道

裁决五组53项，均置于既有公开YobiRuntime.hpp，字段／参数保持原整数类型。YobiActionFlag为22项U32、YobiActionMask为16项U32；与BM、BC、地图协议flags42／flags120及队列batchType严格分域。只按YobiAction.flags真实生产／消费替换，不把以对象位作为条件的其他状态值误归此域。PendingValue1／2只表示绘制队列value4的1／2；OtherMapObjectName只证悬停名称分支，不猜具体对象类别；FollowPetOverlay仅表达随行宠物条值等绘制规则，不宣称完整宠物类型。

已证位包括隐藏2、悬停说明10h、队列batchType强制FFh的20h、待机动画40h、地图悬停及角色／物品展示位、禁止战斗HP条2000h、按帧奇偶跳过的8000h／10000h、参与战斗绘制排序20000h、四相更新门禁40000h、强制alpha为80h的100000h、禁止附属对象跟随路径200000h、以deltaY排序800000h以及辅助战斗挂件2000000h。各工厂的60054h／860040h／200034h和附件继承FFEA7FFDh／820000h／118000h按真实组成及创建／继承用途命名，仍保留原完整组合值。原kFourPhaseFlag保留别名，只将其初始化连到已证枚举。

YobiDirection为八项S32，依屏幕朝向依次UpLeft=0、Up=1、UpRight=2、Right=3、DownRight=4、Down=5、DownLeft=6、Left=7。只替换方向字段、八项挂件分派、已证方向局部比较及既有方向设置API的直接常量；模8、方向数量8、角度及特殊资源的独立0／1变体不套朝向名。无方向意义特效的字段零值只表示使用第0方向资源，不外推其实际移动方向。

YobiUpdateGatePolicy两项S32为FullRate=0／DeferFlaggedUntilPhaseZero=1；全局仍为S32，不把轮转计数器0～3定义成阶段。YobiColorChannelFlag五项U32为None=0、Red=1、Green=2、Blue=4、Alpha=8，只解释YobiAction的+308：两处消费者分别覆盖color2的2／1／0／3字节。同步既有SetYobiActionField308调用，包括登录预览两处，队列记录的batchType不受此枚举解释。

证据：原521F67／521F82按8000h／10000h与628FC8跳过；52200A检查隐藏2，522083／522095／5220A7／5220B9覆盖四通道，5220EC强制alpha80h，522134置队列batchType为FFh。404C91／404CB2以40000h、59E570等于1及59E574非零分别延后更新／释放，404CDE～404CEC按四轮回绕。405C53筛20000h，405BED／405BF9以800000h改用deltaY排序。方向表55D9DC的八组W16为(0,-1)、(1,-1)、(1,0)、(1,1)、(0,1)、(-1,1)、(-1,0)、(-1,-1)；46CE68／46CE78投影横向为和、纵向为Y减X，结合405D08～405E4E的八段角度写入，证明上述屏幕朝向而非猜世界方位。

准备替换上述同域数字并保持原S32／U32操作数；公开API的窗口／预览直接引用同步具名，不调整这些调用方逻辑。静态逆映射通过后执行build.bat Debug，结果随后追加。

YMN14实施：37份CPP及既有YobiRuntime.hpp新增五组53项，替换423处数字（对象位200、组合47、方向97、更新策略44、颜色通道35）；公开接口的参数／返回与对象字段类型保持，未新增头依赖。38份源／头按原整数值及S32／U32类型逆映射逐词一致；取值／顺序／唯一性、引用闭合、完整字符串、UTF-8／原换行、历史进度及差异检查通过，开始编译验证。

YMN14验证：build.bat Debug配置、编译及链接成功，退出码0。公开头触发较广重编译，仍有Yobi及WindowBank／Contacts／Shop／Trade／Skill的既有C4701、WindowShop的S32到S16既有C4244；原路径保持。精确提交38份源／头、本文及唯一进度，共40份文件，继续独立输入／状态位与残余分派。

## YMN15：独立状态／输入位及残余命令

裁决新增24组枚举，并扩展既有BattleResultByte1Flag三项，共新增72项。battle_base.hpp的detail域承接目标选择12位及810h组合、目标编码（无目标-1、邻接编码基值20、两侧40／41、全部42）、BP入场两种突袭／直接武器位及突袭组合、BC场地六位、FE0两个运行位、属性图标显示策略三位、BM第二字节的闪避／特殊格挡／物理吸收和E0h物理反应组合。除目标编码为S32外均为U32；现有第二字节三项保留完整值和消费，新增三项不与完整结果位混用。

服务端battle.h的355～369行B_TARGET、520～527行BP_FLAG、628～640行BC_FIELD及TARGET编码与客户端消费对应。目标800h未由该服务端给出同名定义，客户端只筛同阵营其他玩家，按OtherAllyPlayers命名；目标门禁先发布5ABC44再按原顺序检查和标记，保持武器／前后排处理。场地20h是End，原438573直接结束整次C解析；其余五位依优先级选择110092／110089／110090／110091／110155，不把End简化成仅保留特效。

FE0 bit1由4238F0按自身／自宠／对方图鉴匹配发布，在4209C0控制属性图标资格，命名AttributeEffectsEligible；bit2由击飞／反应结束路径写入、40C190清除、4394A0作为非活动参与者跳过条件，命名InactiveSkipReady，不擅推死亡或空闲。属性图标全局策略三位仅表示在类型5／6之外、模式1、类型5／6之内是否显示，保留原条件组合。BM字节补齐保持原右移和低BYTE读取，secondaryFlagByte1的1经赋值链证明是原bit8闪避，不能套用Normal。

map_actor.hpp承接九组U32：附件输入三位及9h装备列表掩码、地图显示六位／三掩码／第二字节200h对应位、地图动作九位、类别的循环表情位、地图对象静态翻转门禁的一位／一掩码。只解释实际flags42／玩家18A、flags120／玩家动作、flags114、category和附件报文各自域，彼此同值不混用。未证状态业务名按1AF41／1AF42／1AF43／1AF4D／1AF4E／1824F图及声音命名；随宠显示先把8／10h／40h映射为1／4／20h，清位时仍按原对象类别选源位，不重排。聊天到期只清本地DFFFh掩码及原正文存储，保留后续游标别名行为。

其他局部域：map_observation.cpp的S32命令DepartingGroup=81／ArrivingGroup=82，仅用于440920报文分派，不并入CharacterAnimationId；map_transition.cpp的两个U32位为FirstEffectAttempted／SecondEffectAttempted，创建失败也照原逻辑置位，不能称成功；window.cpp的S32展开模式Vertical=0／BothAxes=1及U32的UseStoredCoordinates=1，只解释低mode几何初始化和窗口坐标读取，不给入口flags的flag20快照猜业务。animation.cpp的U32位PublishActionFlags=1对应资源记录的动作属性发布；base.hpp的S32哨兵NotQueued=-2对应对象cacheIndex，队列容量及其他负索引不混用。

原端只读核对43C560目标位与810h／玩家筛选、438550场地位及图号、4238F0资格发布与440920的81／82观察链。当前源码的属性／地图状态／附件解析／窗口两轴初始化逐字段闭合；已有图号／字段说明继续保留，不猜玩家状态名称。另复用既有BattleAction_Idle／WaitIdle补SKL参与者与Lua原生收尾的四处比较，复用YobiActionFlag补五处已证全局角色数组的绘制位引用。

继续保留RID预读20整数／实际19整数的差异，尚不能据此把预读bit2无条件归入BC死亡位；窗口入口flags bit0只证存入flag20而未见业务读取，保留原值及说明。准备按原数值／S32／U32类型替换、对新增及既有枚举做双向逆映射静态核对，随后编译验证。

YMN15实施：18份CPP及三份既有头新增24组、扩展既有第二字节枚举一组，共新增72项，替换140处数字，包括复用既有角色动作四处和对象绘制位五处；没有新增头依赖。21份源／头在排除新声明并核对旧声明后，对新旧两侧的具名引用按原数值／S32／U32做逆映射，逐词一致；枚举值／顺序／唯一性、引用增量、完整字符串、UTF-8／原换行、历史进度与差异检查通过，开始编译验证。

YMN15验证：build.bat Debug配置、编译及链接成功，退出码0；仍有Yobi的amount／result／lastAmount／statusType／selfFlags／relativeX／relativeY既有C4701，原路径保持。精确提交21份源／头、本文及唯一进度，共23份文件。按原BSI01确认的GBK重新只读核查技能数据，尚有技术函数可进一步对应的编号，继续核对后再作最终汇总。

## YMN16：补证技能名称及最后几类局部解释域

按BSI01既有用户确认的编码与命名规则，只读GBK的skill.txt／tech.txt，技术表有效记录1736条；按TECH_ID第4列除100核对分派ID，TECH_SKILLID仅保留所属来源。新确认14项扩展既有BattleSkillDispatchId：Steal=72、BloodAttack=81、Bomb=82、Sacrifice=83、EnergyDrain=84、EquipBreakAttack=85、Death=86、GoldAttack=87、TreatTypeSilence=88、Summon=93、AttackAll=106、DetectEnemy=107、ThrowItem=2002、BlastWave=2005。同ID的等级／名称变体共享已证技术函数，不合并不同ID；未知108等不以邻号补齐。

证据链：tech.c的1320／1541／1800～2010及tech2.c的66／90／327分别由对应TECH函数调用BATTLE_ActionSelect，携带TECH_ID并选择BLASTWAVE／TREAT_TYPE／STEAL／BLOODATTACK等各自战斗命令；battle.c再调用ItemSteal／BreakAttack／Bomb／Sacrifice／Death／EnergyDrain／Summon／DetectEnemy或对应攻击分支。battle.h定义BATTLE_MAGIC_SKILL_SILENCE=88，BATTLE_Treat_Type按所属技能与可选覆盖选择BC_FIELD_FLAG_SILENCE；客户端88选择110155，与新场地位一致，故共享Treat_Type的88按Silence细分。原端427490大函数中205000阈值及techId／100归一化已只读核对；不把技术号／原始Lua注册ID与分派号混用，不改变归一化算法、条件编译或现有报文。

新增四组八项局部枚举：battle_cast.cpp的S32 BattleAction110TargetPassState以BeforePass=0／AfterPass=1表示1038在目标遍历前后状态，40E864写1、40E93E清0，原60次预备、目标缺失跳过、动画事件搜索及退行倒数保持；battle_skill_special.cpp的S32 BattleDancePairResource仅定义Hp=0，两处判零选择SetHp，其他所有值仍走原SetFp，不臆造Fp=1；map_render.cpp的S32 HoverShortcutKey三项Control=17／Insert=45／Delete=46及U32 HoverShortcutKeyStateFlag的Down=2／FirstPress=4，仅用于现有名单快捷键。平台WindowProcedure的按下写2、仅未锁存时写0Ch、释放清2／8及每轮清4证明键态，不混用另一个10h重复按键位。

既有120项技能枚举追加14项，旧项顺序／值／名称保持；四组新枚举共八项，合计新增22项。只替换Yobi内已归一化skill／castSkill103C／对应case与上述局部状态、选择值和键码／键态；不改函数地址名、标签、坐标、图号、容量72或原始技能注册键。techfuncname.txt与techarea.txt未通过已确认GBK严格解码，不猜换编码，不用其内容命名；当前tech.txt没有SK覆盖记录。准备双向逆映射核对并编译验证。

YMN16实施：九份CPP及既有battle_base.hpp新增四组、扩展既有技能枚举一组，共新增22项，替换116处数字（技能103、动作110状态5、跳舞HP选择2、键码3、键态3）；没有新增头依赖。十份源／头双向逆映射逐词一致，核对新增及既有枚举的数值／顺序／唯一性、引用增量、原S32／U32类型、完整字符串、UTF-8／原换行、历史进度与差异检查通过。旧技能项只要求保持原声明及引用，不把本批未涉及文件中的消费者误当缺失；新增22项均有消费。残余14种数字只见坐标／角度／容量、其他域声明、数值表及历史注释，开始build.bat Debug。

YMN16验证：build.bat Debug配置、编译及链接成功，退出码0；仍有amount／result／lastAmount／statusType／selfFlags的既有C4701，对应原路径保持。精确提交十份源／头、本文及唯一进度，共12份文件。最终残余扫描只见未知技能／资源图号的数字case，另发现AppendSkill报文的两处完整flags等值及局部1038门禁需要单独核对，继续最后一批取证。

## YMN17：追加效果选择、合击门禁及既有枚举漏引

原端427490的DSK分支按完整字段1024／4096等值选择AddAppendEffect的技能8／11，43BE00按技能号保存节点，43BEA0按技能号查询。当前外部服务端没有DSK生产记录，故不把该字段直接归入BM结果位，新增局部U32 BattleAppendSkillSelector的SpecialGuard=1024／CrossCounter=4096，只解释已证客户端选择，保留完整等值及未知值不追加的原分支；15处追加／查询的技能实参复用BattleSkill_SpecialGuard／CrossCounter。

原端427490在合击无物理反应时写1038=1，409680在帧事件达到20000或此值为1时进入命中路径并在原分支清0，41B440以非1控制普通帧事件处理。新增共享S32 BattleComboFrameHitGate的UseFrameEvent=0／ForceHitPath=1，只替换这条合击生产／消费链的五处数字；通用队列及Lua初始化仍清原0，不用合击或动作110枚举替代混用字段的通用复位，也不改变value1038字段名。

动作队列另分两组S32单项：BattleCastQueueValue_End=-1用于4392A0／4392F0写队尾、439340读取终止及同域初始化／传参；BattleCastQueueIndex_BeforeFirst=-1用于FC8的首项前索引及Lua原范围检查，二者不与其他位置／技能／资源索引的-1混用。窗口局部U32 LinkedWindowStateFlag_CloseRequested=32768用于数量窗口旧窗关闭请求，ui/WindowRuntime.cpp:4081读取此位调用WindowRuntime_Free，只替换Yobi内消费，不扩大UI模块重构。合计新增五组七项。

其余只复用既有符号：两段魔法反应的优先选择及完整等值使用NullifyMagic／AbsorbMagic／ReflectMagic，合击DeathOrPhysicalReflect组合比较右值补齐；两个设备分支的对象位及SyncBattleAttachmentAlpha实参沿用既有绘制位，原S32实参显式转换保持；Lua终止常量236复用ResetBusyAndIdle，异常状态影子清位复用U32的~BattleActorAbnormalMask。最后一处需额外核对六异常位组合为3F0h、32位取反为原FFFFFC0Fh，双向逆映射仅对此已证恒值表达式归一。所有原字段、类型、位宽、条件、初始化及调用顺序保持，准备实施及编译验证。

YMN17实施：十一份CPP及既有battle_base.hpp新增五组七项，替换78处数字（追加选择2、合击门禁5、队尾值16、队列索引13、窗口关闭1；复用技能15、魔法反应10、结果组合1、对象位13、Lua终止动作1／异常位取反1）。十二份源／头按原数值及S32／U32做双向逆映射逐词一致，取值／顺序／唯一性、引用增量、完整字符串、UTF-8／原换行、历史进度及差异检查通过；六异常位取反另核对为FFFFFC0Fh，开始build.bat Debug。

首轮build.bat Debug配置、编译及链接成功，退出码0。补充扫描复合赋值及异或运算，另外确认11处对象Hidden位和两处FE0的InactiveSkipReady位（后者由stateFE0取值，不套BC死亡位）；在本批内继续复用既有枚举，共91处引用、十三份CPP及一份既有头。新增组／项保持五组七项，补齐后重新核对并编译。

YMN17最终验证：十四份源／头的双向逆映射逐词、原数值／S32／U32、声明／引用增量、完整字符串、UTF-8／原换行、历史进度及差异检查重新通过。再次build.bat Debug配置、编译及链接成功，退出码0；仍有amount／result／lastAmount／statusType／selfFlags的既有C4701，对应原路径保持。精确提交十四份源／头、本文及唯一进度，共16份文件；剩余说明与九批总表另作最终报告。

## 最终汇总与当前证据边界（2026-10-25）

本次持续推进YMN09～YMN17共九批，新增66组枚举，扩展两组既有枚举，合计新增308项值、替换1968处数字引用；复用旧枚举也计入替换引用，旧成员不重复计入新增项。各批已在同一cg2d_re分支分别精确提交。最后一批补齐后再次执行build.bat Debug，配置、编译及链接成功，退出码0。

| 批次 | 主要解释域 | 新增组 | 扩展旧组 | 新增项 | 替换引用 | 最终构建退出码 |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| YMN09 | 状态图号、附属特效模式及放置方式 | 3 | 0 | 17 | 77 | 0 |
| YMN10 | 图形轴、地图挂件、环境类型及文本槽 | 7 | 0 | 29 | 121 | 0 |
| YMN11 | 跳舞模式及消息内部阶段 | 6 | 0 | 32 | 91 | 0 |
| YMN12 | 就绪策略、尾迹及变身／Lua控制 | 8 | 0 | 19 | 77 | 0 |
| YMN13 | BM结果位与组合掩码 | 4 | 0 | 57 | 832 | 0 |
| YMN14 | 对象绘制位、方向及颜色通道 | 5 | 0 | 53 | 423 | 0 |
| YMN15 | 协议输入位、地图状态及独立命令 | 24 | 1 | 72 | 140 | 0 |
| YMN16 | 补证技能分派、动作局部状态及快捷键 | 4 | 1 | 22 | 116 | 0 |
| YMN17 | 追加效果选择、合击门禁、队列哨兵及漏引 | 5 | 0 | 7 | 91 | 0 |
| 合计 | 九批 | 66 | 2 | 308 | 1968 | 各批均为0 |

YMN13首轮因缺少既有声明头编译失败，补齐后重新通过；YMN17首轮通过后补充13处漏引，再次核对并编译通过，以上统计均取最终结果。最终构建仍有Yobi中amount／result／lastAmount／statusType／selfFlags的既有C4701；YMN14公开头重编译时还出现原有其他模块C4701／C4244，详见批次记录。没有修改这些原路径。

所有源码批次均核对原枚举及新声明、引用增量、原数值与S32／U32（另有YMN10的U8域）、完整字符串、UTF-8／原换行、历史进度前缀和Git差异；在移除新增声明并将具名引用逆映射后，逐词保持原逻辑。唯一恒值表达式例外为已验算的U32六异常位取反，数值和运算类型与原FFFFFC0Fh一致。上述属于静态与编译／链接验证；未执行测试、客户端或算法样例，未检查编译产物、安装依赖、计算摘要，也未改动IDA数据库或外部资料。

### 最后扫描结果

最终对src/Yobi下27份CPP再查裸数字case、mode／type／phase／state／kind比较、位运算、复合赋值／异或及既有技能接口实参。仍有73种数字case、275处引用：61种未证业务名的技能号263处，以及12种资源图号各一处。这是剩余数字case的分类统计，不等于模块所有数值常量，也不表示它们应一律定义枚举。

| 保留项 | 已证事实及保留原因 | 后续所需证据 |
| --- | --- | --- |
| 未命名技能61种 | 108～111、113、117、2050～2052、2055～2079、2089～2095、3001～3016、3019、3037、3038、4002；已有独立分派，但当前GBK技术表没有对应TECH_ID／100记录，不能从相邻编号、共享入口或图号补业务名。 | 对应版本技术表、服务端生产函数或明确的技能业务说明。 |
| 图号111177～111179、111183～111191 | 十二个case按资源图号选择表现；当前证据不足以恢复完整技能／元素业务名称，保留原图号及现有消费说明，不新增仅重复数字的别名。 | 图形资源的可靠业务映射，及对应技能生产关系。 |
| 全局主战斗场景phase=8 | battle_attack三处及battle_impact十五处读取的是主场景对象；与角色动作、CMB消息头的同值均属不同域，生产／转换关系未闭合。 | 主场景8的所有写入、生命周期与消息推进关系。 |
| RID预读及两组bit2判断 | 预读20个整数、实际解析19个；四处bit2比较的字段对应尚未闭合，不能统一套用BC死亡位。RID只证0与非0布局有别，不能推定非零值为1。 | 对应版本完整RID生产格式与逐字段对应。 |
| 窗口入口flags bit0 | 四处只把入口低位快照写入flag20，尚无业务读取证据；不同于已证的保存坐标位和关闭请求位。 | flag20真实消费者或同版本窗口调用约定。 |
| 环境入口mode=0 | 右侧入口会更新计时并返回，不删除已存在对象；左侧／根入口直接返回，不能共用“没有活动效果”等业务名称。 | 三入口状态生产／切换与资源保留的完整关系。 |
| 1038、FD4等混用字段 | 已分别具名动作110遍历状态与合击帧事件门禁，但通用复位／其他解释仍混用；FD4返回阶段2只证消费，不能推定全字段业务含义。 | 其余生产链及跨动作复用约定；现有字段名继续保留。 |
| 跳舞模式14的非零选择、SKL的255字段 | 前者仅证0选HP，所有非零仍走FP；后者仅证完整字段255时跳过一条效果记录。没有把未知值补成Fp=1或通用BM掩码。 | 实际生产端的取值范围、字段语义及异常输入约定。 |
| 已具名但保留图号／行为的枚举 | 如22326／22327附属模式、部分地图状态图、环境Ripple4及投射／尾迹分支；局部表现可证，完整业务身份仍未证。 | 对应版本业务或资源说明；目前名称与已证行为保持一致。 |
| 数学、资源及连续数值 | 坐标、速度、缩放、计时、容量、排序优先级、动画事件10000／20000边界、技术号归一化阈值、位提取的1／3／7／1Fh／3Fh、24位颜色保存及其他资源索引保留原算式。它们没有因此成为独立离散业务域，不为清除数字而造枚举。 | 如另行要求集中常量或资源目录，应按其所属模块单独设计；不在本轮扩大重构。 |

资料边界：本次按既有用户确认GBK只读skill.txt／tech.txt；tech.txt有1736条有效记录，未发现SK覆盖记录。techfuncname.txt／techarea.txt未通过该编码的严格解码，不改用猜测编码取名。服务端未找到DSK生产记录，因此新增DSK选择枚举只声明已证客户端含义，未冒充服务端BM定义。

当前已核对候选中，没有更多能在现有证据下安全具名的离散业务项。剩余内容按上表保留数值、行为说明及所缺证据；后续取得相应来源后可继续分析。本结论不表示整个Yobi没有数字常量，也不代替运行验收。
