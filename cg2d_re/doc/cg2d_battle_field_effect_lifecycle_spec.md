# 战斗场地特效与状态清理 SPEC（BFL01）

2026-10-10。四动作140/142/144/145与场地特效创建/完整回调共六正常根。只创建器两索引取样及回调Y转换/偏移一组，共三组；CPP/公头各六注释。当前两表只读，正常旧值不错误；本步明确原读取/转换顺序，不称运行故障修复。无新API、字段、业务默认值、保护、循环上限或依赖。

<a id="battle-field-effect-baseline"></a>
## 原根与已有合同

| 原身份 / 当前提供者 | 完整范围（尾不含） | 指令 / 字节 / CALL |
|---|---|---|
| F:00010730 / UpdateBattleAction140_410730 | 410730–4107C0 | 49 / 144B / 4 |
| F:00010F70 / UpdateBattleAction142_410F70 | 410F70–410FB5 | 20 / 69B / 3 |
| F:00011240 / UpdateBattleAction144_411240 | 411240–411285 | 20 / 69B / 3 |
| F:00011290 / UpdateBattleAction145_411290 | 411290–411312 | 44 / 130B / 8 |
| F:0001FFE0 / NewBattleFieldEffect_41FFE0 | 41FFE0–4200FD | 75 / 285B / 4 |
| F:0001FF90 / UpdateBattleFieldEffect_41FF90 | 41FF90–41FFE0 | 25 / 80B / 4 |

共233条777B/七页、26 E8/七唯一直接callee、十八跳十二RET；无间接CALL/JMP。四动作各唯一实际父404DB0的CALL，沿BIE01已证40573C表基址取140/142/144/145四4B项/四11B case，均push→CALL→JMP40563F。41FFE0有十实际CALL/三父410000、411290、438550；41FF90仅420018的callback字段MOV引用，type1/iscode=false，不能当一次调用。五个旧callee独立身份复用，其中__ftol的本步宿主提供者是两现源S32显式转换，不引入新的Ftoll函数或声称原S64机器ABI；创建器本步新根也为145直接callee，不重复计，PlayFrame只定位既有提供者，不代完整独立合同。

141/143已有BPQ01语义1429/1430、完整193条649B及原选择表/低W8修复；本步两个主体/记录/旧证据保持，不重复取证、计新身份或新增通过。原所有本步证据见[完整指令/差集](cg2d_battle_field_effect_lifecycle_evidence.json)。

## 140、142、144、145

140固定payload，一次读技能：19–30、2068–2079、2055–2057、31、3037仅工厂(source,Y0,type35,0,0)弃返；其它61–63才当前C9C/position查目标AdvanceCast，再AdvanceCast(source)。其它技能直接源推进。无Play、哨兵预判、清零或目标Move替代，主体保持。

142固定payload，无空门禁MarkDeath(effect1054)，返回后fresh完整W32 FDC，掩码FFFFFC0F；先1054=null，再发布保存flags，PlayFrame(source,0)弃返→AdvanceCast。144同序但1068与FFF03FFF。两源完整读取/门禁/发布正确，保持；不将完整掩码改为低BYTE或先清槽后取flags。

145入口payload/一次技能样本，50/51/52/53分别NewFieldEffect(110092/110089/110090/110091)，返回弃置；其它不创建。每条路径AdvanceCast(source)，无Play。原四各自创建/推进由既有共同调用承接，次数/图号/返回消费保持。现局部imageNo=-1仅控制已证“未匹配不创建”，不向载荷补业务值。

## 场地特效创建41FFE0

入口当前全局scene→payload→B0；已有对象仅写其imageNo并返回空，不返回已有T*或重播。不存在时NewYobi(priority2,payloadSize0)，失败返空；成功固定新对象、持其完整flags→绑定完整callback41FF90→priority7/rate100→flags OR40000。

X轴原420036取位置表[15]，42003B取[5]，之后420047/4D分别从两索引对应图形位置记录取第三列S32坐标，保持两索引先齐读。现显式x15Index→x5Index→x15→x5，以(double(x5)-x15)*0.5+x15落F32 offset40。Y轴另一次420067取[15]→42006D取[5]→420077/80取第四列，显式y15Index→y5Index→y15→y5，不复用X轴索引快照；图号/方向0/309和310两BYTE4以及F32 offset44发布保持。

0.5原54D48C四加载字节0000003F；当前double中间/F32字段沿既有宿主数值批准边界，不代x87机器精度。22个S32位置索引88B、22×4个S32图形值352B本步对照当前两个只读模型，共440B/110值；只证明本步模型及两个固定位置消费，不外推全部数据容量/生产或新X分类。

PlayFrame(new,1)弃返后重新读当前全局scene/payload，将新T*发布B0；不复用入口scene。X向零转换→读取共享W32屏幕X偏移→快照Y→写屏幕X；Y向零转换→读取共享W32屏幕Y偏移→清field270原W16→写屏幕Y，最终返新对象。失败/已有对象分支不执行成功尾，不给图号另加查询/保护或创建新payload。

## 完整回调41FF90与B0清理

每帧读当前全局scene.deathFlag严格1，命中MarkDeath(self)，否则PlayFrame(self,0)弃返。两路都继续投影，标死不提前return、不清B0。X向零转换→共享屏幕X偏移读取→Y样本→写屏幕X；原41FFD0完成Y转换后41FFD5读屏幕Y偏移再发布，现显式projectedY→offsetY→y，保F32来源及W32偏移。callback没有自己payload/固定父或完成槽回写，也不新增物理释放。

另只读438550前缀438550–4385FD的48条173B，记录C数据fieldFlags的生产、AL20退出及1/2/4/8/10五创建分支和缺省清理。入口ESI=0；无五标志时先当前scene/payload/B0→MarkDeath，调用后再次当前scene/payload→B0写ESI。当前ParseBattleC_438550对应分支使用真实fieldFlags，重取场景清槽，全文保持。只核该前缀/局部清理，不把整个438550/所有输入或owner寿命纳入六新根通过，未知间接引用和运行回收仍待。

<a id="battle-field-effect-decision"></a>
## 裁决

依子项目AGENTS第8条工程委托，先记本SPEC，恢复三组已证读取/转换顺序；六函数签名、原生非拥有T*与scene B0、S32/W32/F32字段和W16/BYTE发布保持。四正确动作、其它分支/所有随机与共享器/ParseBattleC/表定义不改。不为原callback保留B0行为添加“自动修复”或新owner，正常与失败路径按原合同分别承接。

音频沿当前WAV-only/SDL，数学/CRT沿具名批准边界；旧阶段暂停文字仅历史，不用于本步状态。原越界/溢出异常按RPLAN.24，不专项模拟、不加钳制、默认值、索引门槛或故障验收。执行仍禁构建/测试/客户端或算法样例/产物/摘要，无新依赖。

<a id="battle-field-effect-acceptance"></a>
## 限定验收

必要静态核六完整原根/E8目标、四原表项/四case、十创建入向和一callback MOV、440B模型/4B常量、438550明确前缀；三个源片段/十二注释精确重放后比整CPP/公头，四正确主体/141与143/表/共享器/所有旧记录保持。

登记六新原F语义/适配处置/已知成员。I只41FFE0两组和41FF90一组；S六完整正常源、局部分派、数据及场地引用生产/清理前缀关联，不代全部父-callee/输入域/容量/机器ABI/alias/owner/SDK/声音画面或运行。U08/U09/R01/R04/V04继续open，BATTLE枚举false，完整gates/184叶/60任务及六closure保持，总SPEC§10不降低。

<a id="battle-field-effect-source-state"></a>
## 当前状态

本步前：Git提交说明“[#] 对齐战斗工厂等待与施法身份更新”；本步完成：Git提交说明“[#] 对齐战斗场地特效与状态清理”。使用同一cg2d_re分支，不打tag。三组/十二注释已落实；六完整原根/数据/前缀、整CPP/公头重放、全部旧记录及政策、编码/引用/差异与派生范围静态通过，文本汇总/-Check均退出0；用户七项外部改动保留并排除。

静态结果：语义2171→2177（2129F＋48X），合同2233→2239/3680=60.84%仅关联率，F1599→1605、E1407→1409、BATTLE411→417且枚举false。派生仅六metrics/六F/一功能/五任务；导入16/218、X3060/3061余一、184叶完整验收0、60任务关闭11和六closure保持。完整输入/生产/父-callee/容量/ABI/alias/owner/SDK及声音画面/运行仍待，整体SPEC未完成。
