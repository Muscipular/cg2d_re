# NI角色回复与非玩家动作消费取样 SPEC（BTN01）

日期：2026-10-07。沿BTM01唯一入向与另一实际消费推进，按AGENTS.md第8条委托先裁决再开发；[完整指令/表/前态](cg2d_actor_reply_consumer_sampling_evidence.json)不替代运行验收。

<a id="actor-reply-consumer-baseline"></a>
## 原端基线

当前cg2d.exe只读取证：50BCD0完整195条580B/三页，18direct CALL/9目标（含原alloca_probe），15jump/1RET，唯一512E40@514D4D。444410完整877条2594B/十三页，80direct CALL/33目标、76jump/31RET，唯一50BCD0@50BEC7。合1072条3174B/十六页、98direct CALL、无间接CALL、91jump/32RET。

NI当前接线为NetworkDispatch_512E40的TryDispatchReply<Sub_50BCD0,T>，原字段/编码不改。50BCD0先原gate→Ripple一次CryptInt取值；2048B row/field、逗号逐行及竖线1..9，1为Base62/2..9为旧S32。观察440920完整EAX非零继续，6..9只在观察未消费后解析。玩家对象索引匹配才同player ptr NULL/图号非0门禁、八参51A840；非玩家同查找ptr九参444410；missing原0x1000B容量与0x1000项阈值并存，增count→写原cursor→增cursor，不新增容量/邻栈模拟/保护或发送。正文正确保持。

444410 NULL action早返，payload/index按原读取；39有效case与默认。case50原distance>1先返，再只TEST flags42高BYTE的2；case1真实W16 flags/unsigned field270门槛保持。402000实际五指令是一次GetVal后setz AL相等，现既有Equal_401D00承接已纳管，不新造包装、不改RNG。case200设置仅flags120高BYTE OR10，取消真实W16 ANDEFFF。

case60非玩家创建后原确实重读共享槽比较NULL，不能套用玩家BTM01的创建结果判断。gear在后续映射后fresh槽写；两个pet各重新取槽，然后同pointer NULL/写图号，不再重复读槽。第二pet原先取+38旧ptr再普通S32++，创建后仍另取fresh槽；三个删除分支同旧ptr NULL/MarkDeath/清槽。case70同ride ptr的既有判断在counter48清之前，正值路径仍无新增NULL保护。

两相邻区共378B：50BF14后12B实际90/NN_NOP；444E32后366B为2B8BFF单MOV EDI,EDI、40 W32目标160B、201BYTE selector、3B NOP。两表只有同父data读、无code入向；不把自move/两表称NOP，不造新F/x64 W32地址表。

<a id="actor-reply-consumer-decision"></a>
## 裁决与实现

只修YobiRuntime.cpp现444410一个正文九唯一片段：两处真实BYTE访问、两pet fresh槽后held ptr写图号、第二pet取旧槽先于++、三删除held ptr、ride判断先于counter48清。NI及原全部其它源码保持，现ResetMapFollowersFromPath/ResetMapFollowerFromPath四slot循环/S16路径与fresh pointer合同正确，不造重构或新helper。

原39case/默认、距离重复CryptInt/普通SUB sign/门槛、速度/方向、两次原图号与gear二次映射、pet直接held图号/四次Y→X/RNG、普通失败/字段/清序/kind尾、骑乘70与phase80、原W16取消/现接口和唯一owner保持。网络NI原格式明文、内部LegacyCryptInt及音频暂停保持，不新增异常、回绕、保护、补零、拒绝、钳制、限长。

<a id="actor-reply-consumer-acceptance"></a>
## 可验收小步

1. 完整YobiRuntime仅九具名片段变化，完整NI/网络/共享声明和其它正文保持；明确创建后fresh槽与同pet ptr后续使用的区别。
2. 十六页1072条/3174B原块与98call/91jump/32RET、两个唯一入向、39有效case/201selector、378B两区逐段用途完整对照。
3. 两既有原F新语义1634-1635，NI equivalent、非玩家 adapted；NETWORK55→56/LOOP47→48、四局部I-S各22来源、三任务仅current/引用，全部旧对象/完整门禁保。
4. 必要静态及纯文本measure/-Check，不编译测试运行或摘要；预期语义1636有效1629/文档1747（47.47%）/处置777/E898、R7 347余2714，结束后记实际结果。

<a id="actor-reply-consumer-source-state"></a>
## 完成边界

BTN01 NI角色回复50BCD0与非玩家444410消费两完整根1072条3174B/十六页两块，98实际direct CALL（含一原alloca_probe）/两根分别18-9与80-33 calls-callees，无indirect CALL、91jump/32RET，各唯一上游512E40与50BCD0。SceneState NI正文正确保持原gate/Ripple一次取值、','逐行与'|'1..9旧Base62/S32、观察整EAX短路及6..9延后、玩家同指针NULL/图号门禁和既有51A840/非玩家同查找指针444410路由；missing原0x1000B容量/0x1000项阈值、无补零/无发送/普通递增及写序保，不扩容量/邻栈或异常模型。实修YobiRuntime同444410一个正文九唯一片段：case50 difference>1短路后的flags42仅高BYTE TEST2；case200设置flags120仅高BYTE OR10，原W16 ANDEFFF取消保持；两个宠物fresh槽ptr同NULL/写图号，第二pet先取旧+38再普通S32++，三个旧子ptr同NULL/MarkDeath/清槽，骑乘同条件在counter48清前取样。非玩家原创建pub后fresh槽比较保持，不能套用玩家BTM01持有创建结果；两原图号转换/gear两次转换但pet直接held图号写、四次Y→X CryptInt/RNG、401FE0距离重复消费/SUB sign、case1原W16flags/unsigned field270/四跟随slot与已纳管Reset helpers、case70/80其余接口字段与owner保持。39有效case/默认对应40原W32目标160B+201BYTE selector；相邻两区12B NOP及366B（2B8BFF MOV EDI,EDI+两表361B+3B NOP）共378B只用途、两同父data读/无code入向，不统称NOP/不造x64 W32地址表。两既有F新语义1634-1635/一equivalent一adapted、NETWORK55→56/LOOP47→48、四局部I-S各22真实来源、R7 345→347；全部旧1634语义/冻结定位/其它成员/原pending18和任务/完整门禁保。原格式NI明文/无网络加解密、内部LegacyCryptInt及音频暂停保持；全512E40、观察440920、提供者全部正常生产消费/owner-alias寿命、SDK-ABI/链接/画面/明文互通/运行R8未验。

全512E40、观察440920及所有provider完整正常生产消费/owner-alias寿命、SDK-ABI/链接/画面/明文互通/运行R8继续；原栈probe与x64宿主局部布局不视为机器ABI验收，原void与弃返关系保持，不能以数量宣称整体完成。

## 本步完成结果（2026-10-07）

BTN01已修复YobiRuntime.cpp同一消费正文九处差异，NI正确生产端及全部其它源码/现接口保持。非玩家创建后fresh槽判断保留，两个pet后续同ptr写图号、三个同旧child清理、第二pet和ride时点、两实际BYTE访问及原W16取消已落实。

完整一源/九片段、原两根与378B每段用途、机器增量、五文档及三任务四组集中静态通过；十六页全部1072条原MCP字段逐项通过，两根3174B原块、98direct CALL/91jump/32RET及39有效case/默认与201selector完整对照。原402000实际相等及已有承接核实，不新增比较包装。两区含真实MOV EDI,EDI与两表，未错误归作纯NOP。

measure_cg2d.ps1及-Check同次顺序执行均终态0，完整派生快照逐对象对照、十三文件UTF-8无BOM/LF、四锚点、四I-S各22真实来源/两功能各I-S唯一及文本差异检查通过。实际语义1636有效1629/文档1747（47.47%）/处置777/E898、NETWORK56/LOOP48、R7已复核347余2714；两既有原F新语义1634-1635/equivalent-adapted及两区用途登记，全部旧对象/任务/完整门禁保持。

已完成本步开发及静态验收。遵守现行禁编译、测试、运行限制；完整512E40/观察440920与提供者全部正常生产消费/owner-alias寿命、SDK-ABI/链接/画面/明文互通及运行R8继续，未写成全局或运行通过。
