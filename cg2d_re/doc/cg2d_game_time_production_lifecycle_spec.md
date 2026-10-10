# 游戏时间生产、校时与倒计时 SPEC（GTP01）

2026-10-10。沿MHT01实际时段依赖补458F00/50C870/458F40/519EB0四个原身份，并将既有时间写入197/458DE0补为完整根。依子AGENTS第8条先记本SPEC，恢复倒计时两取样组和PudO持有category一组，保持现类型/调用/业务门槛及MSVC时间承接。

<a id="game-time-production-baseline"></a>
## 五完整根与真实链

| 原身份 / 当前符号 | 完整范围（尾不含） | 指令 / 字节 / E8 / 跳 / RET | 本步 |
|---|---|---|---|
| F:00058F00 / Sub_458F00 | 458F00–458F3A | 15 / 58B / 2 / 3 / 0 | 新身份，body保 |
| F:0010C870 / Sub_50C870 | 50C870–50C8C8 | 22 / 88B / 5 / 0 / 1 | 新身份，持有category |
| F:00058F40 / Sub_458F40 | 458F40–458FC2 | 33 / 130B / 0 / 8 / 1 | 新身份，恢复两组 |
| F:00119EB0 / Sub_519EB0 | 519EB0–519EBA | 3 / 10B / 0 / 0 / 1 | 新身份，body保 |
| F:00058DE0 / UpdateGameTime（既有197） | 458DE0–458EBB | 76 / 219B / 1 / 0 / 1 | 完整登记补齐，body保 |

合149条505B/六页、八E8五callee、十一跳四RET，无switch；另458F40两个原间接GetTickCount物理点，按场景路径只执行一个；458F35为外尾跳458F40，不是普通E8。十二E8入向含PudO→setter/writer两个内部，十外点来自51C7D0/512E40/43CBB0/457020四父；另一个内部尾跳，均按实际字节分类，不把code xref自动算CALL。

全部原/现来源、十二调用短窗、尾跳和两SDK槽见[证据](cg2d_game_time_production_lifecycle_spec.md#evidence-archive-20261011)。MHT时段2212、调色板171、MSVC时间2025与四外父旧身份保持；writer197旧来源/边界保历史后追加当前完整证据，不新建同一原身份。原PudO分派按现四I字段交五参数（socket保持既有原生宽度且原未读）实际源有据，不修改命令/字段或明文队列。

<a id="game-time-production-decision"></a>
## 完整正常合同与局部裁决

458F00先调用GetCate(&gCgTime)，再fresh旧category比较；变化时先读mode73ED44，再发布新category73ED48，持有mode==1才SelectPalette(category,3600)，调用参数为持有category。无论变化/模式如何，原尾跳进入458F40；现void调用同副作用接线保持，不改成变化时才推进倒计时。

458F40场景9要求同源玩家action非空；场景10不加该门槛，其余直接退出。通过后唯一一次GetTickCount，fresh完整S32 timerMode10非0才扣减。原先读U32旧tick14，再读S32 remaining0C，之后计算原W32差并加剩余值，发布新remaining；直接依据持有的ADD结果符号判断，负才按原业务发布remaining=0→mode=0，末尾无论模式/归零均发布当前tick14。**现delta减法在remaining读取前，且发布后重读remaining判断；恢复旧tick/remaining先齐取及持有updated发布/判断两组。**保原业务归零，不增新保护/时间上限/循环或另一个时钟。

50C870先将playerValue交519EB0并弃返回；随后发布timeOffset73ED4C→ReadLegacyTime32(NULL)→发布采样73ED68→UpdateGameTime(&gCgTime)弃返回→GetCate。原持有category作为调色板参数，发布73ED48后SelectPalette(category,0)，随后才发布pathValue9E0434→adjustment73ED40，返回原完整S32 adjustment。**现从刚写的73ED48重读调色板参数；只增加持有category并使用它。**writer内部另一次独立time32采样保，不能把两个time调用合成一次或提前发布adjustment；这次writer使用原旧adjustment，后续更新才使用新值。原socket未读，現已批准原生socket宽度/协议字段保持。

519EB0只将原S32 value位型写玩家首块的唯一U32对象号槽，完整S32原样返回；当前正确body保，不扩为创建/归零/owner变更或bool成功。

458DE0先独立time32(NULL)，按base559508→sample73ED68→offset73ED4C→offset减base→adjustment73ED40→offset减sample→now加offset→加adjustment序形成U32总值。按原普通无符号分解发布year /2592000、month余数/216000、day余数/7200、hours余数/300、minutes余数/5，再按192*(总值%7200)/7200+168发布sTick余192，返回最后除192的商。minutes偏移14h先于sTick偏移10h，六W32/24B/非拥有输出指针保，返回不能当成功或秒。既有正确body/旧静态数学合同保持，本步完整取证与处置补齐，不重写原mul-high机器机制或运行除法样例。CRT内部时间沿MC01实际MSVC提供者，业务时间/字段不改成本地tm/SDK布局。

### 返回和ABI界限

PudO唯一父512E40随后XOR EAX，setter/writer本组调用弃返回，原setter值/writer商/PudO adjustment仍保持。timer两直接外父窗口覆盖SDK EAX；outer六处中**51CDE9后pop/ret继续透传残值**，不能把六处全称弃返。当前DrawNormalScene_51C7D0和两个时间推进提供者为既有void API，本步保持并明确此局部适配；原scene/null/SDK残值经尾跳和完整上层ABI的消费仍未验，不伪造返回0/成功/tick，不以void声明证明原机器返回合同。R04/完整门禁继续开放，所有未变父/既有BSA/BMF合同保。

只恢复三组，两个CPP/两头限这些组及五定义/五声明关联注释；无新全局函数/API/布局/owner、SDK采样、RNG或协议。RPLAN.24不增保护、默认值、补零、钳制、容量索引限制、失败回滚或异常模型；原倒计时负数归零为已证业务分支，不能因现禁令删除。现普通整数/已批MSVC时间与ACP01边界保，不宣优化后独立机器读取/并发/数值或ABI等价。

<a id="game-time-production-acceptance"></a>
## 限定静态验收

核五根连续原字节/边界、八E8五callee/两个IAT tick/一个外尾跳/十一跳四RET、十二调用窗与四外父；倒计时字段先读/持有ADD符号与原归零、两独立time采样/旧adjustment使用、持有category/调色板前后发布、六字段写序/返回商及对象号返值有据。整SceneState.cpp/GameTime.cpp、WorldUpdateDependencies.hpp/GameTime.hpp精确重放三组与十关联注释；其余三根body/全部其它源、MHT时段/原调色板/MSVC提供者及四父保持。

四新语义、197完整追加，五新增F处置；MAP/Yobi各五成员，BATTLE只timer、NETWORK只PudO四限定S零I。U08/U09/U10/G01/R01/R04/V03/V04八任务保持open；原所有枚举/完整gates/六closure、其它登记及总SPEC§10守恒。

只做必要静态文本/measure_cg2d.ps1/-Check，不构建/测试/运行客户端或算法/产物/摘要/依赖。四完整外父/全部输入与正常失败/共享玩家和time生产消费/alias-owner、调色板及网络校时、SDK/数值/机器ABI/画面与全生命周期待；WAV-only、原格式明文/MSVC直接CRT和现文本字节边界保持。

<a id="game-time-production-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐地图命中与时段分类并恢复边界取样”；本步拟提交说明“[#] 补齐游戏校时与倒计时并恢复持有取样”。统一cg2d_re分支、不打tag。先记SPEC，原完整取证/源码三组、核心静态/登记、文本汇总/Check及派生范围核对通过。语义2217=2169F+48X，文档关联2279/3680=61.93%仅关联率，F处置1649/证据1450，MAP169/Yobi99/BATTLE423/NETWORK75仍未验收；派生仅六metrics/五F/四功能/八任务变化，184叶验收0、60任务关闭11、六closure开放/全部完整gates及其它字段守恒，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_game_time_production_lifecycle_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐游戏校时与倒计时并恢复持有取样」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x458f00 | 0x458f3a | 15/58 |
| 0x50c870 | 0x50c8c8 | 22/88 |
| 0x458f40 | 0x458fc2 | 33/130 |
| 0x519eb0 | 0x519eba | 3/10 |
| 0x458de0 | 0x458ebb | 76/219 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`generated_machine_reads_verified`=false；`runtime_verified`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
