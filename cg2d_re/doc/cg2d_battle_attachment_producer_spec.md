# BAP01 战斗附件生产、槽位发布与清理 SPEC

<a id="battle-attachment-producer-baseline"></a>
## 原基线与实际缺口

沿[BAS01三附件同步](cg2d_battle_attachment_sampling_spec.md#battle-attachment-sampling-source-state)推进其生产/发布与身份清理；[完整原指令/六槽表/源差异](cg2d_battle_attachment_producer_evidence.json)保存八完整入口。

| 原入口 | 完整范围 | 指令/字节 | 入向 |
|---|---|---|---|
| 4221F0骑乘工厂 | [4221F0,4222A3) | 51/179B | 3普通call/3父 |
| 4222B0骑乘发布/移除 | [4222B0,422366) | 57/182B | 5普通call/4父 |
| 422A30主装备工厂 | [422A30,422AF2) | 55/194B | 6普通call/5父 |
| 422B00辅助装备工厂 | [422B00,422BBF) | 55/191B | 4普通call/同一父422C90 |
| 422BC0主装备两参入口 | [422BC0,422C1E) | 32/94B | 2普通call/2父 |
| 422C20主装备三参入口 | [422C20,422C86) | 35/102B | 2普通call/2父 |
| 422C90辅助装备发布/互斥/移除 | [422C90,422E33) | 147/419B | 2普通call/2父 |
| 421F90附件身份清理回调 | [421F90,421FB0) | 10/32B | 3个type1地址发布/3工厂父 |

合计442条1393B，各一chunk；24普通call与3地址发布、15已知父分列。422D1B的type-1表[422E34,422E4C)另为6槽24B，type1/2/3..5/6分别落422D22/422D4E/422D7A/422DD4，不计函数代码字节。

实际缺口：
1. 422BC0源仅调用422C20(action,imageNo,0)，原无此转发：独立固定入口payload，正图号缺槽时直接422A30创建/发布，重新取槽一次供判空及图号更新；非正图号取一次槽供判空/MarkDeath后清槽。三参422C20只有非正图号/非空槽时另判index==0才移除，不能用包装调用消原入口与接线差异。
2. 422C20的正路径重新取槽后，原以同指针判空并更新图号；负路径先取槽后判空/index并以同值MarkDeath，源重复读槽。
3. 4222B0负路径原第一次rider快照供判空/phase=0，之后分别重新取槽供field58清零和MarkDeath再清槽；源判空后又重新读取用于phase写。正路径在两次51C0D0映射调用前后原确需重新取槽，现源正确不能合并；创建返回直接初始化phase的原无额外判空路径保持。
4. 422C90非正图号/type>0时原逐槽取一次供判空和MarkDeath，第一槽消费/清零后才取第二槽；源重复读。index1且type3..5互斥门禁对既有types[0]原只取一次后判上下界，源重复读上界。
5. 辅助type1/2/3..5发布后原重新取槽一次供判空及图号写，源再次读取；type3原先比较==3再发布创建返回值，type4/5比较在发布后；源发布后用三元表达式选scale。scaleX用创建返回值、scaleY原重新取槽，创建失败无额外保护。
6. 骑乘工厂原image写后先OR860040再发布callback/flags，源OR在callback发布之后；辅助工厂原方向发布后取低W16 sortIndex，再phase/parent/payload/position读取，最后field270发布，源转换在发布时。
7. 421F90原位置→注册表槽→parent→比较→身份失败MarkDeath，源比较表达式没有明确读取次序。回调只标死，不添加播放/立即free/清父槽。

原两caller的index已限定核证：42A6C1将ESI清0，42A707将该值作index压栈，42A71A递增/42A71B与2比较后回到42A6C3；4387A5将EBP清0，4387DC压栈，4387E8递增/4387E9与2比较后回到4387A7。当前两实际源消费均为index=0;index<2的循环。证据仅该入参计数/传参和循环尾，不验两完整父或其它参数生产。

<a id="battle-attachment-producer-decision"></a>
## 裁决与实施边界

依AGENTS.md第8条委托采用，七根九片段、主装备工厂全文保持：
- 422BC0恢复独立完整正文，直接调用既有422A30；422C20保三参合同，正路径创建/发布后保存gear供判空/图号，负路径保存gear供先判空再index==0和MarkDeath/清槽，不新helper或再合并入口。
- 4222B0仅负路径固定首rider供phase写，后两独立槽复读与MarkDeath后清槽保持；正路径/原映射次数及调用后重读、原失败/完整S32 image返回不动。
- 422C90清两槽分别保存first/secondGear，第二取样在第一消费后；互斥内才保存firstType供两个边界判定，保原对firstAux无额外判空的MarkDeath。type/slot写入及原生YobiAction**只承接原已证caller index0/1，沿现有合同，不增加index拒绝/钳制、模拟其它index把图号当地址或宣异常等价。
- type1/2/3..5各以独立case作用域固定发布后gear供图号更新；type3..5创建后先保存isTypeThree，再发布，之后按同判定或依次type4/type5写F32 0.25/0.5/0.75，X写创建返回对象、Y原重新取槽。type6只缺槽创建sort3后返回，不追加既有槽图号更新；type1 sort低W16 -1、type2/6 sort3、type3..5 sort0及默认不创建/类型槽已发布保持。
- 骑乘工厂只imagepub后保存configuredFlags再callback/flags发布；辅助工厂只方向pub后保存低W16 field270，再按原phase/parent/payload/position读取/field270 pub。主装备工厂原父空返/分配空返和全部已正确字段/顺序、三工厂NewYobi(2,0)/F32 value54=3、U8层9/S32 rate100及PlayFrame(action,0)后重新取parent→X截断/屏幕X/发布→Y截断/屏幕Y/发布/原对象返回保持。
- 421F90固定位置、注册项、parent原生非拥有快照供原身份比较，原失败MarkDeath/无新guard或播放保持；已有声明按当前CPP/头文件纳管，不新增接口。

三工厂/四发布入口/共用回调原分配域、非拥有parent与节点槽/入口payload、失败分支和返回保持；主装备/辅助工厂有父空门禁、骑乘工厂没有，不统一失败或补原调用者保护。原未判空的创建后初始化/scale写继续保其正常路径，不做异常模拟/保护/回滚或宣故障等价。所有非地址宽度、原类型值不清零、回调和资源owner不改；不新增Read/Ctor/Destroy/RNG、分配/释放入口，网络原格式明文及音频暂停保持。

<a id="battle-attachment-producer-acceptance"></a>
## 可度量验收

1. 八完整442条1393B/各一chunk/连续末界与24call+3地址绑定/15父可追；九页原指令和独立6槽24B另列，不为共用回调或表数据增接口。
2. 全CPP仅从HEAD精确重放七根九片段，BC0原独立正文和直接factory接线恢复、C20/骑乘/辅助各自快照与复读/MarkDeath后清槽、type3先判后pub与三F32/原失败、W16及callback读取顺序成立。正确主装备工厂、其余源/头/汇编和旧同步/主回调保持。
3. 八原已有本体身份纳管，语义1312→1320/有效ID1307→1315、非函数片段5保持；八adapted414→422。YOBI13→21/BATTLE10→18同八F不双计，E289→293仅双域局部I/S共用同九片段/原证据，LOOP31/R7区间67/队列及全部完整门禁保持。
4. U08/U09/G01/G02/R01/R04/V04及全R1/R7/R8/整体仍open；全部绑定父/动作实参、战斗数据资源/owner重进退出和浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行仍需对应证据。
5. 仅必要集中静态/获准文本汇总及-Check/引用/UTF-8无BOM/LF和精确提交，不构建/测试/算法样例/客户端/摘要、不全面复查或例行改总SPEC/索引及压历史。

<a id="battle-attachment-producer-source-state"></a>
## 源状态与剩余

三工厂/四发布入口与共用421F90继续单一Yobi节点队列、原生非拥有parent/载荷和注册表；422BC0直接创建422A30与422C20独立入口恢复原分工，BAS01三同步和BTS01主回调保持。include/cg/YobiRuntime.hpp已有八声明不改，不新暴露节点布局。

本组源/原静态不代所有父与动作/数据资源和owner寿命、SDK/ABI/PC53/时钟RNG/实际画面，不证明本机逐指令/异常重叠或并发等价；沿RPLAN.24不添加异常门禁。固定快照`cg2d-re-battle-attachment-producer-20261006`。
