# 邻格 fIM 请求与明文整数分隔 SPEC（NFI01）

2026-10-10。沿WGQ01补完整51B3E0父和510B80发送器；父主体与现读取/时序正确保持，当前发送器的原0x17写入与已批准本机明文整数格式冲突。依子AGENTS第8条先记本SPEC，再按既有[CAB05](cg2d_network_compatibility_spec.md#local-server-plaintext-contract)裁决只删除该字节改写，保原扫描/time/seed/rand消费、字段和发送/父时序，不改服务端或新增协议。

<a id="neighbor-fim-baseline"></a>
## 完整原根与真实接线

| 原身份 / 当前符号 | 完整范围（尾不含） | 指令 / 字节 / 直接E8 | 入向 |
|---|---|---|---|
| F:0011B3E0 / Sub_51B3E0 | 51B3E0–51B530 | 95 / 336B / 13 | 469D22，父469C90 |
| F:00110B80 / network::Sub_510B80 | 510B80–510CD4 | 132 / 340B / 12 | 51B51B，父51B3E0 |

合227条676B/五页、25 E8/十三callee、20跳/三RET，无switch；另两个间接GetTickCount CALL共27物理CALL点。父X/Y减法各三物理点是第一调用加正负两分支的第二调用，执行每轴两次；发送器两个末端发送点同样只走一个。不能按源码合并表达式数漏计或把两分支都当执行。469D22只前八/后十条短窗，父下一条重取targetY覆盖EAX，确认现返回全W32但唯一caller弃值，不称469C90重新完整取证。

两现实现、唯一父、原"fIM\0"四字节、IAT GetTickCount加载与两个call edi有据；实际来源SceneState.cpp/NetworkRuntime.cpp、时钟MainLoopSupport.cpp直接Win32 GetTickCount保持。完整指令/当前提供者/服务端片段/差集见[证据](cg2d_neighbor_fim_request_plaintext_evidence.json)。六callees及七发送叶沿既有实际源；其中46CDF0缺独立身份，记真实提供者不伪称本步完整math叶，旧读取/算术/协议/RNG身份保持，51B3E0/510B80本步补独立身份。

<a id="neighbor-fim-decision"></a>
## 已确认明文边界与局部裁决

本机现服务端UTF-8源码只读：nrproto_serv.c的RECV_PI="fIM"分派顺序读token[1]/[2]/[3]为x/y/dir；nrproto_util.c的nrproto_splitString接受空格或0x17作为下一个token起点，但终止字符的第二循环只把空格/LF/CR写NUL，**没有终止0x17**。nrproto_demkstr_int调用nrproto_a62toi，后者遇非数字/字母立即返回0。因而原510CB7控制字节可留在前一个整数串中，使X或Y按无效串返回0；这是源码判据，不是试包/算法运行结果。

当前客户端BuildProtocolCommand_517510已按CAB05组装原"fIM "，三个Base62整数token自带普通空格；SendProtocolBuffer_517480仍原LF/NUL/日志/观察器/结果合同，InitializeNetworkRuntime当前绑定QueuePlaintextProtocolBytes，它只复制字节，没有0x17终止/规范化。故本步仅从510B80选中空格分支删除buffer[index]='\x17'，保空间字符、fresh buffer重取与break，不改token/base62/参数、日志/回调/发送队列/部分发送、容量或失败。

扫描、selected/ordinal、time32→SeedRandom_53B4BF→NextRandom_53B4C9与原一次播种/一次取数保，不能因移除控制字节顺带删RNG流程或另造随机流；实际工程gRandomSeed提供者及已确认MC01/普通算术边界保持。此为**沿CAB05现格式的具名发送差异**，不宣原0x17机器码等价，不恢复旧W16/命令变换/加解密，不修改服务端的分词或整数失败分支。

## 邻格生产51B3E0

先读gValue581800，非-1直接返原W32位型；不能当bool或默认0。否则X先Subtract(playerX,targetX)取符号，再fresh targetX调用第二Subtract，按首符号决定第二值是否取负，S32结果>1返其位型；通过才同序Y两次，>1返其位型。不合并abs/缓存delta/target或增第三读取，正常及RNG消费保持；异常/溢出不专项处理或模拟。

通过后先targetY再targetX采样，Sub_4450D0(x,y,7)返回0直接0；沿WGQ01完整查询，不把类别检查前移/改记录坐标。随后fresh targetX经IntMinusCrypt(targetX,playerX)，先保存deltaX、再取fresh targetY、将deltaX转换F32，再IntMinusCrypt(targetY,playerY)转换F32；两个原fild/fstp32桥交现46CDF0，取原方向低W32交现S32，不改变原F32桥、算术调用数/读序或新数值差异。

方向后第一次GetTickCount，再fresh lastSent+500u与now作原无符号比较；>=不发，返now。通过先playerY Read、再playerX Read，fresh连接socket交510B80(socket,x,y,direction)，发送返回原被弃；**无论队列/发送结果如何**再第二GetTickCount→发布同一gValueFF0D38→返该值，不回滚/补发/复用首次tick。只此发送器分隔字节更正，父完整主体保持。

## fIM发送510B80

按现CAB05命令后依次x/y/direction，各先capacity取样→format token→fresh目标buffer append，原限长/共享scratch/返回及失效分支保持。随后fresh buffer扫描至第三个空格或末尾；有空间才time32(NULL)/seed/一rand，按spaces余数、零改1、fresh buffer，selected原默认1。第二扫描按原ordinal选点、重取buffer并break，保整个控制流和RNG；唯一变更为不再写0x17。尾原SendProtocolBuffer结果透传，主/缓冲/队列owner不新增或互换。

<a id="neighbor-fim-acceptance"></a>
## 限定静态验收

核两完整连续机器码/25E8十三callee/两个IAT tick/20跳三RET、两实际入向和唯一外父弃返短窗；原cached值/两轴各两执行减法/Word7查询、target取样/F32桥、unsigned500/先Y后X/两tick与不按发送结果回滚、原fIM/三个token与扫描/RNG/唯一17写点。对现UTF-8服务端四片段、当前明文组装/入队/初始化绑定作纯文本对照，原序不猜；不发送样例、试包、跑算法或服务。

整SceneState.cpp/NetworkRuntime.cpp/两头按两定义前注释、两声明注释及唯一字节写删除/说明精确重放；父body/所有其它源/时钟/方向/读取/算术/协议与RNG提供者、唯一外父旧合同和WGQ01源/旧语义保持。两新语义/F、MAP/YOBI仅父、NETWORK仅发送器、三限定S零I；U09/U10/G01/R01/R04/V03/V04七任务open，184叶验收0/60任务关闭11、全部member枚举/完整gates与六closure/原政策保。

两个完整根的输入/所有生产/全callee/外父/alias/owner/SDK/机器ABI/声音画面、实际fIM互通与完整网络/生命周期仍待，不以源码修复称运行成功。RPLAN.24不新增保护、默认值、补零、钳制、索引/容量限制、回滚或异常模型；只读当前IDA不重开/改库，无构建/测试/运行客户端或算法/产物/摘要、新依赖。SDL/WAV-only、MSVC直接CRT及已批准数学/明文/原文本字节边界和总SPEC§10保持。

<a id="neighbor-fim-source-state"></a>
## 当前状态

本步前Git提交说明“[#] 对齐世界对象交互格查询与比较消费合同”，完成提交说明“[#] 对齐邻格请求并修复明文整数分隔”。统一cg2d_re分支、不打tag，先记SPEC，父正确body保持、子只删17写入/保RNG；核心静态、登记、文本汇总、Check及派生范围核对通过。语义2207=2159F+48X，文档关联2269/3680=61.66%仅关联率，F处置1638/证据1439，MAP157/Yobi87/NETWORK74枚举与验收仍false；派生仅六metrics/两F/三功能/七任务变化，184叶验收0、60任务关闭11、六closure开放及全部完整gates/其它字段守恒。首次汇总发现三条具名差异缺description/decision_ref共六登记错误，派生未写；补实际说明/决议锚点后通过，不删差异或改校验器/标准。服务端只读取证，七外部项/删除暂存保留排除，完整fIM互通/运行仍待，整体active/未完成。
