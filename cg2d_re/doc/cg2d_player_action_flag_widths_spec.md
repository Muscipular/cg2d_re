# 玩家及动作状态真实字节/字访问 SPEC（BTL01）

日期：2026-10-07。承接BTK01共享玩家状态发布修复，按AGENTS.md第8条既有工程委托裁决，先记录方案再实施；不要求重复确认。证据见[完整指令及源码前态](cg2d_player_action_flag_widths_evidence.json)。

<a id="player-action-flag-widths-baseline"></a>
## 原端基线

当前已打开cg2d.exe数据库只读取证，十三完整叶29条133B/十三页十三块，无CALL、无跳转、十三RET；15个实际直接入向来自51A840与440920两个父，仅证明入口接线，未复验两个完整父。各根实际W8/W16操作由opcode 80/66 81及地址支持，不以反编译类型推断宽度。

| 原函数/源码符号 | 指令 / 字节 | 原合同 | 本步处理 |
| --- | --- | --- | --- |
| Sub_51A600 | 2 / 8 | flags18A高BYTE OR40，低BYTE不读写。 | 修复 |
| Sub_51A610 | 2 / 10 | 原flags18A真实W16 AND BFFF，正确正文保持。 | 原正确正文保持 |
| Sub_51A620 | 2 / 8 | flags18A高BYTE OR80，低BYTE不读写。 | 修复 |
| Sub_51A630 | 2 / 10 | 原flags18A真实W16 AND7FFF，正确正文保持。 | 原正确正文保持 |
| Sub_51BDB0 | 2 / 8 | flags18C[2]实际W8 OR1，不触及其余三BYTE。 | 修复 |
| Sub_51BDC0 | 2 / 10 | flags18C[2..3]实际W16 ANDFFFE，不触及低W16；以2B memcpy承载，不造uint16指针别名。 | 修复 |
| Sub_51BDD0 | 4 / 17 | 原arg先取为完整S32，同flags18C[2]实际W8 OR2→value190 W32发布→返回同value，原两调用参数/弃返保。 | 修复 |
| Sub_51BDF0 | 3 / 20 | 同flags18C[2..3]真实W16 ANDFFFD→value190完整W32清0，顺序保；以2B memcpy承载。 | 修复 |
| Sub_51BE10 | 2 / 8 | flags18C[2]实际W8 OR24，其余三BYTE不触及。 | 修复 |
| Sub_51BE20 | 2 / 8 | flags18C[2]实际W8 OR8，其余三BYTE不触及。 | 修复 |
| Sub_51BE30 | 2 / 10 | flags18C[2..3]真实W16 ANDFFF7，以2B memcpy承载，低W16不触及。 | 修复 |
| Sub_51BE40 | 2 / 8 | flags18C[2]实际W8 OR50，其余三BYTE不触及。 | 修复 |
| Sub_51BEE0 | 2 / 8 | flags18C[2]实际W8 OR80，两观察入口实际call点接线保，不将父未验写成完整验收。 | 修复 |

唯一PlayerRuntimeSlots.flags18A为原W16，flags18C为原W32储存容器；FDA88E对应flags18C[2]，FDA88E/FDA88F对应flags18C高W16。此前整W16/W32掩码产生同位普通值，但扩大了原内存读写范围，不能作为宽度等价完成。BDD0先取完整入参、BYTE置bit1、W32发布value190、返回同value；BDF0先真实W16清bit1再完整W32清value190。不存在新的状态分支或网络发送。

十三相邻未处置区共107B，逐字节实际90/NN_NOP、无入向，前为本叶RET、后为真实函数入口，仅登记原对齐用途；不造函数或扩大排除。

<a id="player-action-flag-widths-decision"></a>
## 裁决与实施

修复SceneState.cpp十一已有正文十一唯一片段：八置位用现owner的unsigned char字节视图；三动作清位以同owner+2地址、std::uint16_t及两次恰好2B memcpy表达原W16读改写，避免将W32储存强转成W16指针造成别名问题。两玩家清位真实W16原正文保持。局部变量只承载本次取样，不新增helper、共享owner或私有影子。

完整value190发布/清除时点、BDD0返回、所有既有调用及声明保持。已纳管BEC0的W8→W32按序取样及BE50的原klF明文组包与发布尾保持；两个父仅作为实际调用定位证据，完整正文不改，不声明全父/消费/寿命验收。内部LegacyCryptInt不是网络加解密；网络原格式明文及音频暂停保留。

<a id="player-action-flag-widths-acceptance"></a>
## 可验收小步

1. 完整源文件相对前提交仅十一具名正文片段变化；两正确清位/共享布局/值与返回序/父/接口保持。
2. 十三完整原根全部29指令、133B、15入向两父、无CALL/跳转及十三RET与完整块字节一致；十三区107B用途有逐字节依据。
3. 十三既有原F新增语义1620-1632、十一adapted两equivalent。四玩家状态归LOOP42→46，九动作状态沿既有BQR02 BE50/BEC0共享动作状态合同归BATTLE253→262；枚举与完整门禁均保持open。
4. 两功能各I/S共四条局部静态证据，每条29真实来源；U10/V03/R04仅更新current/引用，原任务条件和open状态保持。全部旧语义/处置/冻结定位/pending及完整门禁不改变。
5. 按现行规则只运行纯文本measure及-Check，核完整派生增量、编码/引用与精确提交范围，不编译、测试或运行。预期语义1633有效1626、文档1744（47.39%）、处置774/E892，R7 344余2717；实际结果在静态及汇总结束后追加。

<a id="player-action-flag-widths-source-state"></a>
## 源码与完成边界

本步实现限SceneState.cpp。BTL01十三完整玩家/动作状态叶29条133B/十三页十三chunk、无CALL无jump十三RET，15直接入向两父51A840/440920。SceneState.cpp十一原正文十一唯一片段：八实际W8 OR（flags18A高BYTE40/80及flags18C[2]的1/2/24/8/50/80），三flags18C[2..3]实际W16 AND FFFE/FFFD/FFF7恢复2B读改写；两flags18A本来正确W16 AND BFFF/7FFF整正文保持。BDD0同入参S32先取/OR2→value190 W32发布→同值返回，BDF0 W16清bit1→value190 W32零顺序保。唯一PlayerRuntimeSlots布局/跨字段W32观察、已纳管BEC0 W8→W32取样及BE50明文发送/状态尾、两父全部原正文与现接口保。十三既有真F新语义1620-1632/十一adapted两equivalent；LOOP42→46四实际成员，BATTLE253→262九实际成员，四局部I-S各29真实来源，十三NOP区107B只用途。不造新F/helper/owner/影子状态/范围排除，不加异常/保护/拒绝/补零/钳制/限长或回绕；普通旧值同位，不称数值/并发故障。网络原格式明文、内部LegacyCryptInt与音频暂停保持；全两父/资料生产消费与全部正常owner-alias寿命、SDK-ABI/链接/画面/明文互通及运行未验。

全父/资料生产消费与全部正常owner-alias寿命、SDK-ABI/链接/画面/明文互通及运行R8仍按整体SPEC推进，不能由叶宽度纳管代替；不设置专项异常/溢出或回绕模型门禁，不新增拒绝、补零、钳制、保护或限长。

## 本步完成结果（2026-10-07）

BTL01已完成十一既有正文十一片段：八处置位恢复真实BYTE、三处动作取消恢复真实W16读改写；两处玩家取消本来正确，整正文保持。BDD0同完整入参/值发布/返回和BDF0清位后清值、唯一owner与全部其它源码和接口保持。

完整源、原根与区间证据、机器增量、文档任务四组集中静态全部通过。区间检查最初误要求IDA列表助记符为nop；原会话实际列表将107B显示成align 10h而助记符为空，实际解码仍为NN_NOP且逐BYTE90/size1。已修正列表显示判定，并保留每条实际解码、字节、边界和无入向检查，重跑本组通过，不改原证据或降低用途标准。

measure_cg2d.ps1及-Check同次顺序执行均终态0，完整派生快照逐对象对照、十二文件UTF-8无BOM/LF与四锚点、四I-S各29真实来源/两功能各I/S唯一、文本差异检查通过。实际语义1633有效1626/文档1744（47.39%）/处置774/E892，LOOP46/BATTLE262、R7已复核344余2717；十三新语义1620-1632与十一adapted两equivalent已登记，原对象和完整门禁保。

已完成本步开发与静态验收。遵守禁编译、禁测试、禁运行限制，全两父/资料生产消费/正常寿命、SDK-ABI/链接/画面/明文互通及运行R8按整体SPEC继续，不写成已运行通过。
