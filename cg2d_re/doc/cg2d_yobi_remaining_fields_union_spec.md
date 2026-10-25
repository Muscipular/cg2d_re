# Yobi剩余字段与联合体成员续析 SPEC（YUF02）

2026-10-25。承接[YUF01](cg2d_yobi_unresolved_fields_union_spec.md)，按用户要求继续分析剩余变量及union项，已证用途重命名并替换引用，证据不足保留说明，最后执行build.bat编译验证。本项不是完整Yobi业务语义验收。

## 裁决与范围

依既有工程委托，落实下表20个成员的纯名称调整。只改变标识符及说明，不改变类型、声明顺序、偏移断言数值、初始化、有效成员选择、读写／调用顺序和生命周期。沿各结构与当前源文件限定替换，不能对action类短名称做无类型的全仓库替换。

| 结构及原偏移 | 旧名 | 新名 | 已证用途 |
|---|---|---|---|
| MapActorPayload +04 | action04 | overheadMarker04 | 442F70创建、跟随并绘制的头顶标记。 |
| +08 | action08 | transitionEffect08 | 43E490的阶段1／2创建并清理的过渡特效；图号分别为0x1AE10／0x1AE11。 |
| +0C | action0C | statusIcon1AF4D_0C | 映射后的状态位4驱动图号0x1AF4D；不推测具体状态业务名称。 |
| +10 | action10 | statusIcon1AF4E_10 | 状态位1且statusMask为零时使用图号0x1AF4E；不推测具体状态业务名称。 |
| +2C | action2C | attachedActor2C | 43DD00附属实体字段创建441DB0返回对象；可独立更新运动，不据图号称为特定宠物。 |
| +30 | action30 | mainGear30 | 4410F0主挂件；当前DrawMapMainGear406是已有4.06接线，不改其来源合同。 |
| +34／+38 | action34／action38 | firstAuxiliaryGear34／secondAuxiliaryGear38 | 441130创建的两路附属挂件，4412D0／441430／441570按槽消费。 |
| +3C／+40 | kind3C／kind40 | firstAuxiliaryGearKind3C／secondAuxiliaryGearKind40 | 与前述两路挂件分别配对的种类；不改变整数范围或清理策略。 |
| +44 | action44 | rideActor44 | 骑乘图像对象，与主角色共同计算位置、动画及绘制。 |
| +48 | counter48 | transitionProgress48 | 43E490阶段1／2的位移推进及阶段3螺旋过渡的进度；阶段2可能在同次回调内多次递增，不能称为严格帧数或毫秒。 |
| +4C | action4C | statusIcon4C | 根据状态表及覆盖图号创建、替换图像并定位的状态图标。 |
| YobiAction +2E8 | imageNo2E8 | overheadMarkerImage2E8 | 头顶标记模式1／2所取图号；其他模式沿默认0x192BC。 |
| +2EC | refresh2EC | overheadMarkerRefresh2EC | 图号变化时置1；442F70在标记可见条件下触发重新创建并清零。保留U8。 |
| MapTransitionSlots18::attachments | action18／action1C | firstEffect18／secondEffect1C | 模式0／1先后创建的0x18902一次性特效引用；不与values整数臂混用。 |
| BattleActorPayload +1054 | effect1054 | debuffEffect1054 | 41F660创建的减益特效引用，包含既有扩展减益图；不是所有状态效果的通用槽。 |
| +105C | effect105C | attributeReversalEffect105C | 41F860创建，422370按0x800属性反转位恢复或清理。 |
| +1068 | effect1068 | statusEffect1068 | 41FE00创建、41FCD0更新的状态效果；能力升降及扩展模式共用，不只表示增益。 |

上述YobiAction*成员均为非拥有引用。继续由Yobi链表管理对象，原路径标死／清槽及完成通知保持；不新增释放、判空或失败回滚。对外函数名和Lua／协议键不改。

## 只读原端依据

当前打开的cg2d.exe会话为权威；IDA既有结构名只作定位。关键指令已核，未改动或保存数据库。

- 0x443230读取对象+2EC的BYTE，0x443276读取+2E4，0x44327C清+2EC；0x4432DD取+2E8，0x4432ED将444000结果写地图载荷+04。0x4432B8的默认图号路径同写+04；0x443312清槽。当前UpdateMapActorMode保留原图号比较、置1及模式1／2选择顺序。
- 0x43E6C8写载荷+08，0x43E6D1写图号0x1AE10；阶段2在0x43EBFC写同槽、0x43EC05写0x1AE11。0x43E8AC／0x43ED28清槽。+48在0x43E770递增并作15阈值比较，阶段3的0x43EE21比较360、0x43EF88继续递增。只命名通用过渡进度，不称单一骑乘帧数。
- 0x443A57／0x443A64把0x1AF4D的创建结果写+0C；0x443CEB／0x443CF5把0x1AF4E的创建结果写+10。0x443908将状态选图创建结果写+4C，0x44392A更新其图号；这些依据只证明图像行为，不能独立证明具体业务状态。
- 0x43DE60调用4410F0，0x43DE68写+30；0x43DEE1调用441130，0x43DEED／0x43DEF0写+34／+3C；0x43DF44调用同工厂，0x43DF49／0x43DF53写+38／+40。0x43E071写+44骑乘对象，后续把主角色图号映射为骑乘外观；0x43E3CE调用441DB0、0x43E3D6写+2C附属实体。
- 0x446E3D／0x446EA0分别取载荷+18／+1C地址，作为444260的完成回写槽，0x446E80／0x446EE3保存返回对象，之后跟随父世界坐标。446FA0则传空完成槽，0x446FEF／0x447039保存两个对象，不能据共享字段改写不同的完成通知合同。
- 446FA0伪代码因既有union类型显示QWORD比较；实际0x446FB0／0x446FB3先检查+18的W32指针，0x446FB9比较+14的W32时间。用途判断以实际指令为准；本项不把反编译类型误差搬入源码，也不改变现有x64时间承载。
- 0x41F66A／0x41F79F读取并发布+1054，六个减益图同步影响FDC／13D0／13CC；0x41F869／0x41F905读取并发布+105C；0x41FE09／0x41FF30读取并发布+1068。0x4224A2置0x800、0x4224B4以图110152调用41F860，反向分支在0x4224D1标死并于0x4224D9清+105C。命名与各工厂、既有技能／状态编号证据一致。

## 保留项及联合体边界

- WindowOpeningPayload::flag20只见各窗口入口把flags低位转存为0／1及布局断言，当前未见消费；不能称为置顶、可关闭或展开完成标志。value1C及bytes22继续按YUF01保留。
- MapTransitionSlots18::values.state1C仍只有构造及447230零写，保留S32。attachments成员仅在模式0／1生效；其他模式仍用values初始化门闩，不因新名称改变有效成员选择。原x86+18／+1C只是来源偏移，x64两个指针继续占16B。
- YobiAction的field58、field1D0、value1D4、counter1DC及BattleActorPayload的valueFD0／valueFD4／valueFF0／value1030／value1038具有多用途合同，继续共用原存储，不添加同类型union别名。已有+10、+2C、+1D0、YobiPayload及BattleEffectThirdArgument的成员／判定条件沿YUF01不变。
- effect1060涉及克隆／骑乘／缩放，stateFE0包含不同用途的组合位，aliveFF8与死亡处理及等待门禁联动；没有足以覆盖全部消费的更窄业务名称，继续保留。castSkillLevel1044存在技术索引／分档消费，不能据局部参数改称角色等级或与1040合并。
- value190／value1A0、field2D8及未定bytes保留段仍缺少独立业务消费依据；保持原类型、容量及构造／复制顺序。未定业务名称不影响本轮已证引用替换，但不记为用途已解决。

## 验证要求

对本项源／头排除注释、保留字符串，逆向映射20个成员后与修改前逐词核对；检查限定引用、UTF-8、逐文件原换行及Git差异。按本轮明确授权执行既有build.bat Debug，记录实际配置、编译和链接结果；不运行测试或客户端，不安装依赖、不计算摘要。

## 实施与静态核对

- 16份源／头完成20个成员的378处标识符替换：地图载荷13项262处、YobiAction头顶标记两项12处、过渡union两项16处、战斗三项88处。第17份源文件window.cpp仅补flag20的保留说明。
- 17份源／头逆向映射新标识符、排除注释并保留字符串后，与修改前逐词一致；声明类型、成员顺序、初始化、偏移断言数值、常量、分支及调用／读取顺序均保持。源码／头范围旧成员代码引用为零；UTF-8、16份发生名称变动文件各自原LF／CRLF、历史进度前缀及Git差异检查通过。window.cpp仅一行注释变化，保持LF。
- 按用户授权开始build.bat Debug；配置、编译和链接实际结果另记。未执行测试或客户端。

## 编译结果与交付范围

build.bat Debug使用既有MSVC x64预设，配置、编译与链接成功，退出码0，脚本报告产物build/windows-msvc-x64/Debug/cg2d_re.exe。本次实际输出仍有既有C4701可能未初始化变量警告，涉及battle_skill_status／battle_skill_special／battle_message／battle_effect／map_observation；没有为消除警告增加初始化或改写原路径。未执行测试、客户端或算法，未计算摘要、安装依赖或改动IDA／外部参考资料。

本项交付17份源／头、YUF02与唯一进度，共19份文件；沿既有分支精确提交，既有未跟踪内容保留。20项已证行为命名及引用替换完成，保留项仍未证业务用途，编译通过不代替画面、union生命周期或未知语义验收。
