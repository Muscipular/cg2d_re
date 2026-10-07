# BQW01 战斗命令初始化、结束位与封包生产SPEC

2026-10-07。沿[BQU01命令/倒计时](cg2d_battle_command_state_dispatch_sampling_spec.md#battle-command-state-dispatch-sampling-source-state)和[BQV01活宠索引](cg2d_battle_target_hover_sampling_spec.md#battle-target-hover-sampling-source-state)，补初始化→结束位消费/发布←真实战斗消息生产。依AGENTS.md第8条先记裁决再实施，[四完整原根与源前后态](cg2d_battle_command_init_packet_producer_sampling_evidence.json)为依据。

<a id="battle-command-init-packet-producer-sampling-baseline"></a>
## 完整原基线与差异

| 原入口 | 当前符号/实际文件 | 指令/B/页 | CALL/jump/RET | 台账 |
| --- | --- | --- | --- | --- |
| 43C4E0 | Sub_43C4E0 / world/SceneState.cpp | 23/126/1 | 4/1/1 | 1564新正常F，正确体保持 |
| 43C040 | Sub_43C040 / YobiRuntime.cpp | 10/31/1 | 0/2/1 | 1565新正常F，正确体保持 |
| 43BFF0 | Sub_43BFF0 / YobiRuntime.cpp | 26/73/1 | 0/5/1 | 1566新正常F，一SAR消费修复 |
| 50D3C0 | Sub_50D3C0 / world/SceneState.cpp | 105/312/2 | 3/8/5 | 1567新正常F，两个copy/pub阶段修复 |

合计164条542B/五页，七CALL（六direct/四callee、一GetTickCount IAT）、十六普通jump/八RET，无switch。五真实代码入向/四父：初始化43D01C来自43CBB0、clear43C515来自初始化、结束位43C51A/50D48F来自两生产根、消息5133FE来自512E40。仅登记现父，不推完整分派/owner/运行通过。

| 差异 | 原点 | 裁决 |
| --- | --- | --- |
| live结束位寄存器消费 | 43C017 MOV W32 endBits→43C01F SAR EDX,CL→43C021 TEST DL1 | 每非空action才fresh W32→bit_cast S32→算术右移观察低1，保全局W32 |
| C copy/发布阶段 | 50D3F2词组copy→50D3F9 INC held next→50D3FA尾copy→50D3FC CMP4→50D3FF pub | 一次长度与wordBytes、word copy→next→tail copy→wrap谓词→pub，原可选清零保 |
| M copy/发布阶段 | 50D4D5/50D4DC/50D4DD/50D4DF/50D4E2 | 同阶段恢复，独立M索引和缓冲保 |

两体三片段；普通原低位结果与队列字节一致，不称并发/数值/故障差。不新增越界、溢出或回绕模型，不加拒绝/截断/补零；四槽轮转是原正常业务。两正确初始化/clear体纳管不改。

<a id="battle-command-init-packet-producer-sampling-decision"></a>
## 初始化、结束位与消息合同

1. active W32置1→GetTickCount32→截止W32 tick+30000→seconds/command/skillCount/petCount各零→43C040→43BFF0→X493/Y-106/CommandValue17→SkillUsed0→真实活宠索引→fresh LastPet不等才skillIndex=-1。本体实际写计时、计数与显示状态；窗口动作由场景分发负责，不给初始化增加创建/关闭或“修复”未要求状态。
2. clear前20槽（59E980至59E9D0）每次借action T*，非空才flags共享W32 AND~40。原直接W32内存写，不改成BYTE、仅原bit6清除，现span20正确。
3. 结束位函数先sceneType W32严格10，turnSnapshot W32先读再turn W32，二者相等才扫描20槽；每非空action才fresh endBits W32→S32算术右移slot→低1有值则flags W32 OR40。snapshot/turn可保原U32相等比较，endBits字段不改类型、不把SAR改成逻辑移位。保空槽短路和每槽重取，不能提前cache整endBits或改任意非0/位处理。
4. 消息首BYTE C优先，借旧CWriteIndex W32和独立四槽buffer，每槽1000H；地址按现ptrdiff_t计算，只拓宽地址不扩索引/计数。一次strlen+首NUL长度，wordBytes=byteCount&~size_t{3}，先word区copy→旧index+1→tail区copy→wrap=(next>=4)→pub next→wrap真才pub0。不把两copy并在增量之后，不把比较挪到pub之后或用取模代原分支，保C立即返回/首NUL/原失败与别名边界，不做异常专项。
5. P从text+2按原六%X格式到position/playerFlags/skillFlags/petSkillFlags/rebirthLevel/ridePet六个真实W32输出槽，参数序和字节保持，Scan返回弃置后立即返。不能用成功数门禁拒绝原部分写、清全部槽或引入私有协议。
6. A从text+2按原双%X到endBits/turn；返回弃置后fresh pending严格1才held turn→pending0→snapshot，随后无条件43BFF0/return，包括scan不足的原后续。保partial scan失败路径，不增格式检查/成功数限制、无假成功或shadow结束位。
7. 首字符C/P/A三路之后才第二BYTE text[1]=='U'，命中只gValue5ABCB0=1/return；U优先于M。M原独立MWriteIndex/四槽buffer，恢复同word copy→next→tail copy→wrap→pub/可选0阶段；其余字节无写。首实参在原本体没有读取，现uintptr_t占位声明与调用保持，本批不猜用途/重命名或改ABI。
8. 原格式58B050六%X含NUL18B、58B048双%X含NUL6B共24B，两真实53CADC原点和现ScanLegacyString/ScanLegacyInput/LegacyFile边界复用。保持原uppercase hex输出槽、容量、输入偏移/失败部分写和帧分派，不改封包格式、不加解密，只明文。本批只纳管战斗消息生产叶，不据该叶验整个512E40分派、socket接收或完整网络互通。
9. 上游[既有BMF01完整十状态场景分发](cg2d_battle_event_finish_scene_lifecycle_sampling_spec.md#battle-event-finish-scene-lifecycle-sampling-source-state)已正常源码静态纳管、1349语义/adapted且属BATTLE/LOOP/YOBI三域，不能继续把它当缺少完整静态基线的父。本批冻结并继承既有证据，避免全面复检；完整427470/资源owner/重进退出/借用失效/SDK/ABI/运行仍按其基线未验边界推进。
10. 初始化供BQU完整倒计时30秒deadline/计数与state；活宠复用BQV严格W16叶，结束位与P真实槽供既有主命令/Flush/Check。完整技能/宠物生产和其它数据字段仍逐项推进，不以新增文档数或父已静态验过代完整状态/生命周期/画面。

## 四R7及机器台账

四未审X [43C039,43C040)7B、[43C05F,43C060)1B、[43C55E,43C560)2B、[50D4F8,50D500)8B共18B，各全90/逐byte只读NOP decode/各byte无入向，前本批完整根RET/后原真实函数入口。mapped仅原填充用途，不造F或范围排除；R7 269→273/3061余2788。

四新正常语义1564..1567/四equivalent/四BATTLE成员：语义1564→1568/有效1557→1561、文档1675→1679/3680（45.62%，沿汇总器100*n/d再Round两位）、分类694→698/BATTLE246→250枚举open/E796→798。无F/有效区间各7、差队列172/缺定位171/43待归属及全部原对象/门禁保持；U08/R01/R03/R04/V04五任务仅本步当前/引用、60源码closed10/完整验收0/叶0/global=false。

<a id="battle-command-init-packet-producer-sampling-acceptance"></a>
## 可度量验收

- 四原根单chunk连续真实字节、164/542B/五页、六direct四callee+一IAT/十六jump八RET无switch、五真实入向/四父与两个format原点/24B首NUL字节。
- 两体三片段授权差；C/M各一次长度/word区copy→next→tail区copy→CMP谓词→pub/可选0，以及结束位fresh W32/S32 SAR/低1/flags W32写。初始化/clear两正确体、原20槽/四1000H槽/首BYTE及第二U优先、原P/A输出地址和partial失败后续/所有返回保持。
- 30秒截止及初始化全部pub序、pending严格1与turn-snapshot-结束位阶段，BQU/BQV真实消费接线与既有BMF01上游基线保；不复检未变完整父/CRT，限本条生产消费静态对应。
- 四X18B完整字节/每byteNOP和无入向/前RET后函数用途；四新F/处置/250open成员、两局部I/S/五任务和完整派生快照/冻结定位/编码/引用/diff，measure/-Check只文本汇总。

<a id="battle-command-init-packet-producer-sampling-source-state"></a>
## 本步交付

先落SPEC/证据，修结束位和消息两个体三片段、纳管两正确依赖，再同步台账/五任务/唯一进度/派生看板，精确提交13文件并固定快照`cg2d-re-battle-command-init-packet-producer-sampling-20261007`。完整512E40分派、全技能/宠物生产、owner与窗口寿命、SDK/机器ABI/画面运行/R8继续推进；既有BMF01父完整正常源静态继承，整体完整验收保持未完成。音频暂停/不构建测试运行摘要/RPLAN.24/原格式明文及适用完整门禁保持，不降低标准。
