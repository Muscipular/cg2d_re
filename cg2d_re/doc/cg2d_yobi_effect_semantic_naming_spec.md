# Yobi剩余特效语义命名（BFTN02）

日期：2026-10-25。按用户要求继续整理按图号、字段及type编号命名的特效；能证实用途的重命名并替换引用，证据不足保留说明，完成后执行build.bat Debug编译验证。

## 范围与裁决

按既有委托先记SPEC，由本线程以原端指令、当前调用生产及只读外部资料裁决名称。本轮仅重命名12项BattleEffectType及15项关联BattleActionId，补充中文说明；不改变数值、顺序、底层类型、布局、分配、回调、参数、运算、RNG、分支或生命周期。

BattleEffectType仍为U32的48项0～47；BattleActionId仍为S32的257项0～256。工厂参数及局部整数类型保持。type>47仍解释第三槽为已有对象并进入公共尾；不增加边界门禁。type41／42保持独立，14／16创建19与扩展图替换顺序保持，34／43先写deathFlag再判空的原顺序保持。

## 已证命名

下表省略成员前缀BattleEffectType_。技术2053／2054只证实所属技能及选择条件，保留技术编号，不推测其独立招式名称。

| type | 原成员 | 新成员 | 用途及证据 |
| --- | --- | --- | --- |
| 14 | LevelScaledImage110165 | StatusHpRecovery | STV状态报文mode1生产动作131，随后应用数值并设置IncreaseHp；等级选110165族，等级>=9递归创建19。不是技能Recovery或Heal的别名。 |
| 18 | AmountTieredImage111194 | AmountTieredHpRecovery | 物品报文value==7选择动作150，随后IncreaseHp；按value1030的2000／3500阈值分图，不推测具体药品名。 |
| 21 | LevelScaledImage110100 | SkillActivation | 多技能的发动侧队列生产动作151；服务端SPR_sk_start_big=110100旁证。3100／3101仍先递减castLevel1040，其他用castSkillLevel1044。 |
| 22 | ParentBoundImage110097 | Seal | MON收回、封印卡缩放及其他共用路径使用；SPR_fuuin=110097。命名为封印视觉，不限定为宠物收回或捕获成功。 |
| 23 | ParentBoundImage110096 | Summon | MON放出及其他进入路径使用；SPR_tukaima=110096。只表示召唤视觉，不修改进入／变形逻辑。 |
| 24 | ScalePulseImage110095 | SealCard | ITM mode6排队154→155→156，结果1才移除目标；SPR_card=110095。表示封印卡效果，不表示已经捕获成功。 |
| 26 | CenteredImage110156 | InstantDeath | 技能86及技能98模式9生产动作134，成功标志后另排死亡、HP归零及死亡处理；SPR_sokusi=110156。效果本身不执行即死结算。 |
| 27 | CenteredImage111162 | DeathScytheTechnique2053 | 技能86“死神的镰刀”，技术号除100为2053时选动作135；图111162。技术业务名称尚未证实。 |
| 28 | CenteredImage111163 | DeathScytheTechnique2054 | 同一技能技术号除100为2054时选动作136；图111163。技术业务名称尚未证实。 |
| 34 | CastingImage1028 | SpellCasting | 施法、弧形投射准备等在角色施法槽保存该效果；源image1028经既有扩展替换，音效与定位仍读取原图号。 |
| 43 | ParentBoundImage13E4 | CastingAttachment | 现有生产为SoloDance写image13E4，动作253创建并保存施法槽、254清理；按父死亡结束，复制创建坐标，不持续跟随父坐标。名称不宣称独占某一技能。 |
| 47 | ScaledPayloadImage102CAtPosition | AreaAttackImpactAtPosition | 动作255处理有效攻击帧事件时，首目标创建范围表现；取源image102C，绝对XY，XY缩放各乘1.5。 |

同步重命名关联动作：131／199为PlayStatusHpRecoveryEffect及渐显版；151／185为PlaySkillActivationEffect及渐显版；152／186为PlaySealEffect及渐显版；153／187为PlaySummonEffect及渐显版；154／156为StartSealCardEffect／WaitSealCardEffectEnd，155为PulseScaleWithSealEffect；134／197为PlayInstantDeathEffect及渐显版；135／136为PlayDeathScytheTechnique2053Effect／PlayDeathScytheTechnique2054Effect。普通与渐显入口保持各自后继及播放条件，不创建新动作。

## 原端证据与外部资料边界

- 当前cg2d.exe会话经MCP只读核对。0x40FC10的普通入口中，0x40FCA2为150→18，0x40FCB4为151→21，0x40FCC6／40FCD8为152／153→22／23，0x40FD6A／40FD7C／40FD8E为134／135／136→26／27／28，0x40FDB2为131→14。渐显入口沿既有0x412310映射，不误引后继0x4124E0。
- 原0x429C24判断状态mode1，0x429C3C排队131，0x429C55排队160，0x429C6D写value1030，0x429C83写numberDisplayType1034=5。当前BattleNumber_IncreaseHp与该值一致，命名只反映状态HP恢复表现。
- 原0x41CDFB～41CE62核实21的3100／3101特殊等级读取及110100族；0x41CE6F／41CE8A为22／23的110097／110096及41A1C0回调。0x40BF92在缩小退出中调用22，0x40C05B在放大进入中调用23。
- 原0x41D1E5～41D1FD为24的110095、41A270、层8及初始缩放0；物品路径0x436C65／436C7D／436C96依次排队154／155／156，效果阶段1与3保留等待握手。
- 原0x41D6A0～41D6E3区分26／27／28的110156／111162／111163并居中Y。0x4335EA起读取技术号并作有符号除100，0x433603比较2053，0x433611处理2054，选择135／136／134，0x433631排队；死亡结算另行排队。技能86名称来自已确认编码的游戏skill.txt第86行。
- 原34分支0x41DADB起分配角色载荷，0x41DB2C取1028，0x41DB43后按原图选音效与位置；43分支0x41E290起分配，0x41E2E3取13E4，0x41E2FD／41E303复制坐标。0x40FE94创建43、原40FEB0清理；当前SoloDance生产及清理队列与之衔接。
- 原0x418728起按首目标阵营选绝对坐标，0x41874C以47调用工厂；0x41D728／41D74A写绝对XY，0x41D756／41D762各缩放1.5，0x41D76B读102C。
- 服务端include/anim_tbl.h以UTF-8严格读取：1017～1019行card／tukaima／fuuin，1023行sk_start_big，1093行sokusi；宏英文及数值仅作旁证，原有乱码注释不作中文名称依据。游戏gmsv/data/skill.txt、tech.txt按既有BAN01已确认的GBK严格读取，未容错转换或写入。现有资料未找到2053／2054／2070／2076／2079对应的技术／技能业务名称；造头盔的20700等不与2070混淆。

## 保留项目与缺失证据

| type | 保留名称／行为 | 保留原因 |
| --- | --- | --- |
| 1 | FixedImage110063 | 动画头有SPR_dorein_big1的编号线索，但当前只见普通／渐显分派，缺少专属生产链，不能仅靠资源名断言吸血用途。 |
| 15 | CenteredLevelScaledImage110170 | 等级分档并居中Y；当前仅见通用分派，未找到业务生产及资源名称对应。 |
| 17 | ParentBoundImage110184 | 固定图、层203、父死亡结束；没有足够业务生产证据。 |
| 19 | ParentImageTierAura | 已证14／16的分档附加层，名称已描述实际父图选择；缺少独立资源业务名称，不改为某个技能专属。 |
| 30／31／32 | FixedImage110012／110014／110013 | 动画头life系列只提供资源线索，当前无直接业务生产；不据此猜生命、吸血或复活。原图号顺序保持。 |
| 38 | LevelIndexedImage111239 | 技能3001受术侧生产，等级1～7取111240～111246；技能及图族的具体业务名称未证实。 |
| 40 | RandomOffsetImage110474 | 技能402路径在选定一侧现有角色上生产；宽／Y偏移及随机定位已证，资源的独立业务名称不足，不与type5共用屏障类施放效果混称。 |
| 41／42 | PayloadImage102CType41／42 | 不同动作及多技能生产都可写102C，共用图源不足以合并类型或冠单一技能名。 |
| 44／45／46 | FixedImage111179／111188／111191 | 已证技能2070／2076／2079命中链选择；现有技能资料缺失这些编号的业务名称。 |

## 验证与实施记录

实施前记录上述范围；实施后逆映射新标识符逐词核对源／头，保留完整字符串及原类型／数值／调用顺序；检查48项创建类型及257项动作数值与顺序不变、旧引用清零、UTF-8及原换行保持。差异核对通过后按用户授权执行build.bat Debug配置、编译、链接，不测试或运行客户端。实际结果完成后追加。

### 实施结果

- 已在11份源／头重命名12项特效及15项动作，共124处标识符替换：特效55处、动作69处。另补充保留项的生产、资源线索及缺失证据；技术2053／2054已知属于死神的镰刀，但独立技术业务名仍未确认。
- 逐词逆映射核对通过：排除注释并保留完整字符串后，11份源／头与实施前一致。48项特效及257项动作的数值、顺序、唯一性、类型和全部调用逻辑保持，旧引用清零；UTF-8、各文件原LF／CRLF、历史进度前缀及差异检查通过。
- build.bat Debug使用既有MSVC x64配置，配置、编译及链接成功，退出码0；脚本报告产物build/windows-msvc-x64/Debug/cg2d_re.exe。输出含既有C4701警告，涉及lastAmount、statusType、selfFlags、amount、result的可能未初始化路径，本轮未改这些分支。
- 未运行测试、客户端或算法，未安装外部依赖、计算文件摘要或修改IDA数据库、服务端及游戏资料。编译通过仅验证本轮命名及引用可构建，不代替未证业务名称或画面验收。
