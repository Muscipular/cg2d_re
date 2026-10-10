# 世界对象删除、整表清理与空引用恢复 SPEC（WLR01）

2026-10-10。沿MOP01/WOL01继续正常生命周期链，核删除、无释放整表重置、已占用槽清理和空action恢复四完整原根；现主体与共享八附件helper正确，全部保持，只补五CPP/五头关联注释、四独立原身份/F及两限定S，零I/新函数/API/布局或业务行为。

<a id="world-object-lifecycle-baseline"></a>
## 完整原根与当前来源

| 原身份 / 符号 | 完整范围（尾不含） | 指令 / 字节 / CALL | 实际入向 CALL |
|---|---|---|---|---|
| F:000462E0 / Sub_4462E0 | 4462E0–446465 | 131 / 389B / 12 | 2 |
| F:00046470 / Sub_446470 | 446470–4464C5 | 18 / 85B / 0 | 4 |
| F:000464D0 / Sub_4464D0 | 4464D0–446604 | 97 / 308B / 10 | 3 |
| F:000467D0 / Sub_4467D0 | 4467D0–4468EC | 82 / 284B / 4 | 10 |

合328条1066B/八页、26 E8/六唯一callee、37跳/六RET，无switch；19实际入向E8/十七父。只核各CALL后≤32B短窗，确认当前void观察边界，窗口含其他分支的部分不当本CALL动态后继或完整父。原指令/引用/短窗、当前差集及共享器见[证据](cg2d_world_object_lifecycle_evidence.json)。

六callee为519D50、444FA0、404D00、53C0A9、401EA0、441DB0，复用1300/2187/222/705/219/1298六已有身份。401EA0已证唯一jmp401EC0，当前两个Read_401EC0调用是同一读取，不恢复私有包装或额外读取/RNG；free当前由共享器std::free承接，沿MSVC直接替代边界，旧705其他样例不冒充本步实际调用。

<a id="world-object-lifecycle-decision"></a>
## 工程裁决

依子AGENTS第8条，先记本合同，再只补十关联注释及完整登记。五正确body、字段/类型、正常及失败/返回、原动作借用和MSVC/SDL等已批准边界保持；不把标死、清引用或普通创建静态合同称完整owner/运行通过。

## 编号删除4462E0

先将objectIndex W32位型与真实玩家objectIndex比较；相等只调用519D50并结束，不查世界槽或减世界计数。当前ReadPlayerObjectIndex_FDA880只是SceneState持有的原W32字段读取，无新CryptInt读取或私有玩家影子状态。

非玩家用444FA0查槽，负结束；非负借record.action，非空交现共享ClearMapActorAttachments，随后重新读record.action调用MarkDeath，再清action槽。不将初次action跨清理复用到主标死，不立即free主action/payload或摘链。

共同尾先持样usedCount，再依次used W16=0、flags42 W16=0、flags120 W16=0、value34 W32=-1；旧计数>0才发布旧-1。freeCursor>被删index才回退至index，不重新排序/清整个记录。最后持旧upperBound，按[upper-1]逆扫used W16非零，找到才发布end；全空直接结束，**保留旧upperBound**，不能补写0。name/category/坐标/其他字段不额外清。

## 共享八附件与文本

删除446329–4463DC、整表清理446503–4465B6复用同一源码helper；已有1300玩家清理519D61–519E14也使用它，旧完整玩家合同保持不重验。该helper是当前代码组织，没有新原F身份。

固定parent.payload，空立即返回；依次检查原+04/+08/+0C/+10/+2C/+4C/+30/+44八非拥有child槽。只保存槽地址，逐槽调用前才读当前child，非空MarkDeath后立即清该槽；不将八T*一次预取或批量清空。原不处理+34/+38，保持。随后当前text1C非空才std::free→清text1C；这是真文本释放，不能合并成标死或拿错误分配域。

两原根各九MarkDeath物理点（八child/一主）与free，现共享helper循环承接八点、调用方承接主，源码表达式数不同不当遗漏；主标死前均fresh record.action。原生T*/char*借用与释放域保持，全部别名/实际最终回收另验。

## 无释放整表重置446470

先usedCount/upperBound/freeCursor三个S32零发布；对同一1000项表每槽依次used W16=0、action T*=空、flags42 W16=0、flags120 W16=0、objectIndex W32=0、value34 W32=-1、name54首BYTE=0，最后findCursor=0。

此入口原无CALL，不标死、free、析构或清payload/坐标/其余文本；current for-array原生步长沿已证x64布局，不能复用有释放的整表清理，也不能改成memset。当前void声明沿已证调用方弃原表外地址的观察边界，不重建x86返回地址/机器ABI。

## 已占用槽整表清理4464D0

同样先三个计数/游标0，但只used W16非零的槽进入处理；未占用槽的action及字段保持。非空action先共享八槽/text清理、fresh主MarkDeath→清槽，再清used/flags42/flags120、objectIndex=0、value34=-1；不清name54。已占用但action空时仍清这些记录字段。

扫完findCursor=0；不增加类型判断、skip已标死、初始化整表或回滚。两个整表入口的清理/选择性和名称差异必须保持，不能因调用者都是换场/退出就合并。只限定19个入向短窗，不代所有换场/退出/重进生命周期。

## 空action恢复4467D0

objectIndex<0先结束，之后一次444FA0，负结束；仅used W16非零且action空才创建。先持样direction，再读取coordinate1C、再coordinate0C各一次，之后才读imageNo完整W32位型交441DB0(image,mapX,mapY,direction)。原DB gameX/gameY名称不独立作换轴依据，保原参数/槽次序及Read计数/RNG。

先发布创建返回至record.action，空结束，不回滚used/计数或清字段。成功used W16=2，重新借record.action.payload写index；再重取action/level写field1D0。分别重取action.nameC4和titleE1目标，在各自strlen之前取目标，用record.name54/title65含NUL原字节复制，无新16字节门槛。最后重新取action与record.flags114作完整flags OR，不重解析其他文本、改flag字段/方向或补field1D8。当前正确正文保持。

<a id="world-object-lifecycle-acceptance"></a>
## 限定静态验收

核四完整连续机器码/26E8/六callee/37跳六RET、19实际入向/十七父及19短窗，当前4body/共享helper、八槽顺序与两free/两主fresh绑定、表选择性/字段宽度和两个恢复读取/创建/原bytes复制；整两CPP/两头仅十关联注释精确重放。真实SceneState玩家字段/清理、WOL01/MOP01、结构/表/其他来源及六旧语义保持。

四新语义/F/MAP与YOBI成员，两限定S四正常根，零I/新函数/行为。U09/U10/U11/G01/R01/R04/V04仍open，全部member枚举/完整gates/184叶验收0/60任务关闭11及六closure和原范围政策不变。全部数据生产/19完整父/传递callee/正常失败/容量/alias/owner/SDK/机器ABI与声音画面/运行仍待，总SPEC§10不降低。

RPLAN.24不专项越界/溢出/异常模拟或恢复，不加保护、默认值、钳制、输入/索引/容量限制、补零或回滚。保WAV-only/SDL、MSVC直接CRT、原明文/文本字节及数学边界；只读当前IDA，不重开/改库，不构建/测试/运行客户端或算法/检查产物/摘要，不引依赖。无tag历史附件沿MOP01，外部删除暂存保留排除。

<a id="world-object-lifecycle-source-state"></a>
## 当前状态

本步前Git提交说明“[#] 对齐世界对象查找领取与类型映射合同”，完成提交说明“[#] 对齐世界对象删除清理与空引用恢复合同”。统一cg2d_re分支，不打tag。四正确正常根/共享器保持，只十关联注释；完整原指令/19入向与短窗、两八槽/free/主fresh绑定、两个整表/恢复阶段差异、整两CPP/两头重放与其余源/字段/存储静态通过，登记、文本汇总、Check及派生范围核对通过。语义2195=2147F+48X，文档关联2257/3680=61.33%仅关联率，F处置1626、证据1429，MAP146/Yobi76仍未验收；184叶验收0、60任务关闭11、六closure开放及全部完整gates保持，整体SPEC未完成。
