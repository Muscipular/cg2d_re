# 战斗特效工厂类型命名（BFTN01）

日期：2026-10-25。按用户要求先整理NewBattleEffect_41CD00的type，定义枚举并替换当前源码引用。遵循已确认的原则：业务名称证据不足时按图号、输入字段及回调行为命名，不推测技能名称。

## 范围与裁决

在既有battle_effect.hpp的cg::yobi::detail中定义BattleEffectType，固定底层类型为std::uint32_t，48个成员覆盖0～47，数值与原工厂一致。函数第4参数继续接收U32，同源局部变量继续保留原S32或U32，不改变声明签名、载荷、布局及未知输入承接。

原type>47会将第三槽解释为已有对象指针，直接进入共同尾；本轮只命名48个创建编号，不新增ExistingAction=48之类的别名或拒绝未知值。第三参数仍由调用者显式选择offsetY／existingAction。0x2F边界替换为值47的最后一个创建成员，原无符号比较保持。

替换工厂的type分派、type比较与扩展图替换门禁；调用者只替换第4实参、其同源三元结果和生产赋值。phase、技能号、图号、blend、link、位掩码、计数及其他独立type保持。动作127～129的phase-120是跨域算术偏移，保留原式；-1是局部未选择哨兵，也不归入创建枚举。

## 原端分派证据

当前cg2d.exe实例经MCP只读核对。0x41CD09从arg_C取第4参数至EBP；0x41CD12比较0x2F，0x41CD15按无符号大于跳到0x41E30F的已有对象路径。0x41CD1D用该type索引0x41E3F8的48字节表；0x41CD23再经0x41E3C4的13项表跳转，每项4字节。实际读取全部48个索引和13个地址，并核对各目标指令的原case注释。13是压缩后的顶层分支数，不能当作type最大值。

| 原type集合（十进制） | 顶层实际目标 |
| --- | --- |
| 0、1、2、3、4、5、6、7、8、9、10、11、12、14、15、16、18、20、21、22、23、24、26、27、28、29、37、38、39、40、41、42、44、45、46 | 0x41CD2A |
| 13 | 0x41D776 |
| 17 | 0x41E144 |
| 19 | 0x41E1B9 |
| 25、33 | 0x41DA04 |
| 30 | 0x41D831 |
| 31 | 0x41D891 |
| 32 | 0x41D8EE |
| 34 | 0x41DADB |
| 35 | 0x41DC27 |
| 36 | 0x41D961 |
| 43 | 0x41E290 |
| 47 | 0x41D6EB |

通用目标0x41CD2A内按原type继续选图。原反编译在0x41CDAE将这部分识别为switch，但实际顶层仍以上述字节映射和地址表为准。另有按等级、父图和公共尾载荷类型的独立分派，不将它们的压缩标签、图号或等级数字替换为本枚举。

原有完整行为及25个直接调用点合同参见[完整工厂SPEC](ui_windows/shared_battle.md#battle-effect-factory)。当前源码同样保留25处调用（含递归），其中普通与渐显两个入口分别生产30与23条phase到type映射；118／188无创建分支，仍按原后继推进。

## 48项命名

以下成员均在BattleEffectType内。固定图号名称只证明资源选择；同一图号或回调不合并不同type。业务名称来自已证技能生产、技术查询及现有扩展图选择，其他名称按实际行为。

| type | 枚举成员 | 已证行为与限制 |
| --- | --- | --- |
| 0 | BattleEffectType_FixedImage110009 | 图110009；动作115的普通分支。 |
| 1 | BattleEffectType_FixedImage110063 | 图110063；普通完成通知回调41A160。 |
| 2 | BattleEffectType_MagicAttack | 攻击魔法共用表现；等级偏移+110067，高等级111013。 |
| 3 | BattleEffectType_CrossCounter | 按反击技能11的附加技术等级选110153／111053。 |
| 4 | BattleEffectType_DebuffSpell | 减益魔法表现；等级偏移+110071，高等级111063。 |
| 5 | BattleEffectType_BarrierSpell | 反射／吸收／无效类施放表现；110105族／111021，不等同技能402屏障。 |
| 6 | BattleEffectType_AttributeReversal | 属性反转表现；等级偏移+110161，高等级111001。 |
| 7 | BattleEffectType_PhysicalReflect | 查技能55技术等级，选110109／111025。 |
| 8 | BattleEffectType_PhysicalAbsorb | 查技能57技术等级，选110117／111027。 |
| 9 | BattleEffectType_PhysicalNullify | 查技能59技术等级，选110125／111023。 |
| 10 | BattleEffectType_MagicReflect | 查技能56技术等级，选110113／111024。 |
| 11 | BattleEffectType_MagicAbsorb | 查技能58技术等级，两分支均用110121，查询调用保留。 |
| 12 | BattleEffectType_MagicNullify | 查技能60技术等级，选110129／111022。 |
| 13 | BattleEffectType_Recovery | 恢复魔法表现；110176族／111019，保留Recovery扩展图替换。 |
| 14 | BattleEffectType_LevelScaledImage110165 | 等级偏移+110165；等级>=9递归创建19附加层。 |
| 15 | BattleEffectType_CenteredLevelScaledImage110170 | 等级偏移+110170／111001；按源图偏移居中Y。 |
| 16 | BattleEffectType_Heal | 补血魔法表现；110085族，等级>=9创建19，随后做Heal扩展图替换。 |
| 17 | BattleEffectType_ParentBoundImage110184 | 图110184、层203、回调41A1C0；父死亡时标死，不跟随父坐标。 |
| 18 | BattleEffectType_AmountTieredImage111194 | 按1030的2000／3500阈值选111194／111195／111196。 |
| 19 | BattleEffectType_ParentImageTierAura | 按父图及1D0等级选110184／111015／111020／0；14／16递归使用。 |
| 20 | BattleEffectType_FixedImage110104 | 图110104；完成后通知关联父对象。 |
| 21 | BattleEffectType_LevelScaledImage110100 | 等级偏移+110100／111016；3100／3101先递减1040取等级。 |
| 22 | BattleEffectType_ParentBoundImage110097 | 图110097、回调41A1C0；保持创建坐标，不是跟随特效。 |
| 23 | BattleEffectType_ParentBoundImage110096 | 图110096、回调41A1C0；与22独立编号。 |
| 24 | BattleEffectType_ScalePulseImage110095 | 图110095、层8、初始缩放0，回调41A270放大／缩小。 |
| 25 | BattleEffectType_StatusRecoveryEmitter | 洁净魔法表现；顺序发射41A7D0，音效281。 |
| 26 | BattleEffectType_CenteredImage110156 | 图110156，按源图偏移居中Y。 |
| 27 | BattleEffectType_CenteredImage111162 | 图111162，按源图偏移居中Y。 |
| 28 | BattleEffectType_CenteredImage111163 | 图111163，按源图偏移居中Y。 |
| 29 | BattleEffectType_CenteredImage110157 | 图110157，设备混合并居中Y；不推测独占技能名。 |
| 30 | BattleEffectType_FixedImage110012 | 图110012；源浮点X，源Y加第三数值。 |
| 31 | BattleEffectType_FixedImage110014 | 图110014；与30／32保持独立编号。 |
| 32 | BattleEffectType_FixedImage110013 | 图110013；与30／31保持独立编号。 |
| 33 | BattleEffectType_ReverseEmitter | 环形载荷、反向发射41A590，音效280。 |
| 34 | BattleEffectType_CastingImage1028 | 源1028施法图及扩展替换，保留原图音效／定位选择。 |
| 35 | BattleEffectType_SkillImpact | 按103C分派完整命中／移动初始化，保留Magic扩展图替换。 |
| 36 | BattleEffectType_RandomEmitter | 回调41A310随机发射；分配优先级取源位置低U8*10+1的低U8。 |
| 37 | BattleEffectType_ParameterChange | 能力变化表现；按等级选110189／111057。 |
| 38 | BattleEffectType_LevelIndexedImage111239 | 等级1～7选111240～111246，其余111239。 |
| 39 | BattleEffectType_BombLevel | 按全局爆弹等级选110103～110100或111016，居中Y。 |
| 40 | BattleEffectType_RandomOffsetImage110474 | 图110474；依次取源宽及Y偏移，随机定位。 |
| 41 | BattleEffectType_PayloadImage102CType41 | 直接取源102C；与42共用表现，保留独立类型。 |
| 42 | BattleEffectType_PayloadImage102CType42 | 直接取源102C；与41保持独立编号。 |
| 43 | BattleEffectType_ParentBoundImage13E4 | 角色载荷、图源13E4、回调41A1C0；复制源坐标。 |
| 44 | BattleEffectType_FixedImage111179 | 图111179；技能2070命中链生产。 |
| 45 | BattleEffectType_FixedImage111188 | 图111188；技能2076命中链生产。 |
| 46 | BattleEffectType_FixedImage111191 | 图111191；技能2079命中链生产。 |
| 47 | BattleEffectType_ScaledPayloadImage102CAtPosition | 绝对X／Y、源102C、XY缩放各乘1.5；创建类型上界。 |

## 关键域边界

- type7～12分别查询技能55／57／59／56／58／60，是物理／魔法反射、吸收及无效的表现。type5由55～60共用的施放链生产，不与技能402屏障混为同一编号。
- type13／16分别在现有公共尾接受Recovery／Heal扩展图替换；type35接受Magic替换。14／16先按原图和等级产生type19附加层，再执行现有替换，不调整先后顺序。
- type17／19／22／23／34／43使用父绑定回调41A1C0；父严格死亡时标死效果，随后仍播放／投影自身坐标，不能据父指针命名为跟随特效。
- type41／42虽同读image102C，仍分别具名；type30／31／32图号顺序为110012／110014／110013，不能按编号顺序调换图像。
- UpdateBattleAction139_410000内还有供DrawBattleTiledEffect_41E890使用的type0～4，属于另一域，本轮保持；同名type不做全文替换。其他铺屏、技能、状态类型也保持独立。
- type35包含按技能选择的多个初始化与回调，不能简化成单一固定坐标效果。type36原优先级低U8及随机消费、type40的宽度／偏移重读和RNG顺序均保持。
- 保留type34／43原有先写deathFlag再判空、所有原空指针分支、blend／link共同尾和资源扩展门禁，不以命名修改异常或生命周期行为。

## 验证边界

仅做必要源码静态核对：成员0～47唯一连续覆盖、每项有引用、25处调用数量及其余实参保持、53条phase映射保持、独立type域不混用。排除新增枚举声明及说明注释，将新引用还原为原整数值，按词法比较源／头文件；数值允许十六进制／十进制及对应无符号枚举表达同一值，原语句、运算、字符串及所有参数位置保持。

显式UTF-8，既有头文件LF及CPP／进度CRLF各自保持。进度仅追加到本线唯一进度文件。未编译、调试、测试、运行客户端、修改IDA数据库／服务端／游戏数据或计算文件摘要。

## 实施结果

- 已定义48项BattleEffectType，完整覆盖0～47，每项有中文说明及实际分派引用；函数及同源变量原有整数类型保持。
- 四份CPP共替换156处引用：57处case、21处type比较、20处直接调用实参、56处同源生产赋值及2处三元结果。25处工厂调用、53条普通／渐显phase映射、118／188无创建分支、已有对象公共尾和原扩展图替换顺序保持。
- 五份源／头排除新增枚举声明及注释后，按整数值逆向映射并逐词比较通过；成员唯一连续覆盖、引用闭合、独立铺屏type、原有符号门禁、phase-120和其余调用实参静态核对通过。UTF-8及各文件原换行保持，未进行编译或运行验收。
