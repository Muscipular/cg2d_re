# 玩家动作分发与外观子槽取样 SPEC（BTM01）

日期：2026-10-07。承接BTK01/BTL01，沿51A840实际调用链推进；按AGENTS.md第8条既有委托先记方案再实施。完整证据见[指令/表/前态](cg2d_player_action_dispatch_sampling_evidence.json)。

<a id="player-action-dispatch-baseline"></a>
## 原端基线

只读当前cg2d.exe原会话：51A840完整630条1740B/九页单块，98直接CALL/27callee、无间接CALL、31jump（含表跳）及46RET。唯一入向50BCD0@50BE5C，局部八参x/y/dir/action/option1..4及add esp20→jmp证明调用弃返；不验完整50BCD0，不猜各case残余EAX为统一返回。现void接口保持。

入口完整W32 action以无符号大于200走默认，所以负值也默认；201项BYTE selector映射40个W32目标，其中39个有效case（0..19、30/31、40/41/42/44..51、60/63/64/70/80/200）加默认。X:0011AF0C-0011B080共372B：目标160B、selector201B、尾11B实际90/NN_NOP。仅两同父data读取，无code入向，表目标均指父内原指令；只登记用途，不搬成x64的W32指针表。

case1原51A87D先取玩家指针，51A883发布W32速度，51A88A向同指针field284写，再51A060(1)；-1速度改100，其它值原样。case200原51AEF4对FDA88F实际W8 OR10；零支51AEFF对FDA88E实际W16 ANDEFFF，不是W32掩码。

外观case60内联块由现ApplyPlayerAppearanceReply承接：payload先取/NULL早返、同原image两次转换及全局/玩家图号先后保持。+30/+34/+38子槽各先取并依同值NULL判断；创建EAX先比较NULL再发布槽，不能发布后另读槽决定成功。转换后再取子槽写图号；+38原先取子指针再普通S32++图号；三个删除分支以同一次取得的原生子指针NULL/MarkDeath，再清原槽。宠物两处原Y→X CryptInt读取共四次、转换次数及RNG消费保持。

<a id="player-action-dispatch-decision"></a>
## 裁决与实现

修改两个cpp两个既有正文三唯一替换：SceneState.cpp两局部片段修case1指针取样与case200 W8/W16；YobiRuntime.cpp既有外观helper恢复上述子槽/创建结果/清理取样，以局部bool承载原比较结果，T*承载原地址，不新增函数、共享owner或影子状态。三创建普通NULL分支本来存在，保原失败，不增保护；两次2B memcpy表达case200的高W16读改写，避免宽指针别名。

骑乘case70、phase3 case80和所有其它正文正确保持，保原新实体NULL行为/flags W32字段、fresh全局玩家/子槽读写、值发布/输入参数及接口。39正常case与默认、98原调用、27提供者定位纳入本父基线，不把各提供者和正常寿命写成全通过。原格式明文、内部LegacyCryptInt与音频暂停保持，不新增业务分支或异常/回绕模型。

<a id="player-action-dispatch-acceptance"></a>
## 本步验收

1. 两完整源文件仅三具名替换，外观七子片段有逐项前后态；其它所有正文、owner布局、参数/值发布/返回及声明保持。
2. 原九页全部630指令与1740B块字节、98调用/27目标、31jump/46RET、唯一入向与局部弃返、40目标/201selector/11B填充完整可对照；逐case对现switch覆盖39有效值和默认。
3. 单既有原F新语义1633/adapted与LOOP46→47、两局部I-S各21真实来源，372B只用途；三任务仅current/引用，open及所有旧对象/完整门禁保持。
4. 仅必要静态与纯文本measure及-Check，不构建测试运行或摘要；预期语义1634有效1627/文档1745（47.42%）/处置775/E894/R7 345余2716，终态追加实际结果。

<a id="player-action-dispatch-source-state"></a>
## 完成边界

BTM01完整玩家动作分发51A840父630条1740B/九页单块、98实际直接CALL/27callee、无间接CALL、31jump（含一个表跳）/46RET，唯一50BCD0@50BE5C局部八参实用/弃返有据。两个cpp两个既有正文三唯一替换：case1先取同原生玩家action再发布walkSpeed→同指针field284→51A060(1)；case200非零仅flags18C[3] W8 OR10，零仅flags18C[2..3] W16 ANDEFFF用2B memcpy。外观60既有helper恢复三个子槽先取/三个创建返NULL条件先判后pub且沿同结果进分支、转换后fresh子槽写图号、第二pet原+38先取再普通S32++与三个旧子指针同NULL/MarkDeath/清槽。完整39有效case/默认路由、速度-1→100、原201项selector的39有效case与默认/动作70骑乘与80phase、两次原图号转换/gear-pet二次转换/四次Y→X原CryptInt调用及RNG/字段/普通失败/清序/value与原接口、唯一owner/正常布局保；不造helper/状态/保护/异常/回绕。40真实W32目标160B+201BYTE selector+11B逐90/NN_NOP共372B只用途（两同父data读，无code入向），不造x64 W32地址表。单既有F新语义1633与adapted处置/LOOP46→47/两局部I-S各21来源、R7 344→345；所有旧1633语义/冻结定位/其它成员/原pending18和任务/完整门禁保。完整入向父50BCD0、各27provider全部正常生产消费/owner-alias寿命、SDK-ABI/链接/画面/明文互通/运行R8未验；网络原格式明文/内部LegacyCryptInt、音频暂停保持。

本步覆盖完整原父与其已有外观拆分正文的静态合同，不替代入向50BCD0、27提供者全部正常生产消费/owner-alias寿命、SDK-ABI/链接/画面/明文互通与运行R8。未编译、未运行不能填全局验收pass。

## 本步完成结果（2026-10-07）

BTM01两个cpp两个既有正文三具名替换已完成：速度发布前保存同玩家指针、动作200恢复原W8/W16、既有外观helper恢复三创建结果与三删除指针和第二pet取样。39有效case/默认及其它正文/接口/owner和值序保持，无新helper或原F。

完整两源与外观七子片段、原根/372B每表每case、机器增量、五文档与三任务四组集中静态通过；九页每页70条、全部630条原MCP字段逐项检查通过。完整原块1740B、98直接call/27callee、31jump/46RET、唯一入向局部弃返及372B（160B目标/201BYTE selector/11B实际NOP）用途都有据，不将两表写成纯NOP。

measure_cg2d.ps1及-Check同次顺序执行均终态0，完整派生快照逐对象对照、十四文件UTF-8无BOM/LF、四锚点及两I-S各21真实来源、文本差异检查通过。实际语义1634有效1627/文档1745（47.42%）/处置775/E894、LOOP47/R7已复核345余2716；单既有原F新语义1633/adapted与372B用途已登记，全部旧对象/任务/完整门禁保持。

已完成本步开发及静态验收。未编译、测试或运行；全入向父50BCD0、27提供者完整正常生产消费与owner-alias寿命、SDK-ABI/链接/画面/明文互通和运行R8继续按整体SPEC推进，不填作运行通过。
