# 聊天发送门禁与六字段封包 SPEC（CTS01）

2026-10-10。沿ETE01转义生产端补齐实际聊天发送者与唯一六字段封包消费者，依子AGENTS第8条先记证据、方案和裁决。统一cg2d_re分支、不打tag；保持GBK提示、原格式明文、MSVC直接CRT及WAV-only，不扩大运行权限。

<a id="chat-send-baseline"></a>
## 原根与接线

| 原根 | 完整范围（尾不含） | 指令/字节 | E8/跳/RET |
|---|---|---|---|
| 50C940，聊天发送门禁与文本生产 | 50C940–50CAD6 | 106/406 | 9/14/3 |
| 511900，六字段封包及发送 | 511900–5119F7 | 77/247 | 14/0/1 |

共183条653B/四页、23实际E8/11目标、14跳四RET；另两个FF15调用同一GetTickCount导入54D1F8，分别属于受限与普通分支，不计入E8。两实际入向内部1/外1：44AB60的44AC4E唯一调用50C940，50CA90唯一调用511900。入向数量只证明来源；外父BTG01已有完整175条591B证据，本步关联其发送与清理窗口，不新增其整项验收或重复原身份。

原58B2A0命令BYTE是75 53 72 00，即uSr；58AFF8为50 7C 25 73 00，即P|%s。两原提示58AFEC/58B000各含NUL为11/21B；当前已获批GBK提示及容量/格式保持，不声称与旧原文字节一致。当前BuildProtocolCommand_517510按既有本机服务端明文合同生成“命令+空格”，字段/LF接当前提供者，不恢复历史命令混淆、W16外壳或加解密。

<a id="chat-send-decision"></a>
## 发送门禁、字段与返回

50C940颜色4映射5，其余完整S32保。入口先W32 gate，再BYTE disabled；disabled非零，或gate==0且跟随W16非零且options194低BYTE bit8非零时，进入受限计时。tick减当前W32时刻按原U32比较：大于180000则disabled=0后继续发送，不更新两时间字段；否则依次重新取gate、跟随W16、选项低BYTE，在原互斥条件下提示禁言/按音效W32配置决定SE74/320/240并返回配置零或SE的S32结果。仍处于原跟随/选项条件时保原继续发送路径，不添加禁止发送的门槛。

普通分支先当前W32时刻写previous，再取tick，再读previous，再将tick写current；差值小于1000则提示重复发言→重新取音效配置/可选SE74/320/240→disabled=1→返回原S32结果，否则发送。原业务0/1写入、1000与180000门槛和未预清两个1024B栈区保持，不补回滚/默认成功或时钟保护。

原50C99B及50CA20均为BYTE TEST FDA894的8位，源options194存储是W16；现static_cast<uint8_t>(options194)先读整个W16，不符原取样宽度，尽管正常位判定值吻合。裁决复用现有ReadPlayerOptions194LowByte，其实现以unsigned char读取同一真实W16字段低BYTE，分别在原两短路点调用，不提前读取/跨分支缓存，不新增reader/API/原身份或改变字段存储与写方。一个逻辑组、两调用表达式修正；所有其它门禁、调用和发布序保持，不宣优化后独立机器读取或并发等价。

发送路径先52D2D0(context文本,escaped,1024)弃返→P|%s格式到另一未清1024B packet→mode W32→range W32→Y原整数getter→X getter→新socket→511900(socket,x,y,packet,color,range,mode)透传S32。字符串借用仅在同步调用内，不新增分配/释放或变更协议、容量。整数getter原401EA0与当前已适配reader边界保持，不为本步扩改原数值/机器ABI或恢复加密叶。

511900先uSr命令，再依次x整数、y整数、文本、颜色整数、范围整数、模式整数，每字段先读outBufferLen W32/S32→格式token→新读共享buffer指针→append弃返；不将容量/指针跨token调用缓存。六字段后新读buffer→取socket→517480透传S32，原builder/append失败不新增阻断/回滚。该正文和全部声明签名保持，仅关联注释，不造struct或字段宽度。

BTG01提交者既有颜色取样→cursor=-1→50C940弃返→三次新取当前context分别清length/caret/text首BYTE→再次取context的序保，发送早返也按原清输入，不因提示/发送失败保留输入或回滚历史。现有历史去重、IME、六文本context、黑名单及消息清理全正文不改；其全部外父/owner与运行仍待。

<a id="chat-send-acceptance"></a>
## 限定静态验收

核两完整原根/连续字节、实际E8/FF15/分支返回、原命令格式与提示长度、唯一发送父局部窗口、真实低BYTE reader与W16存储。整SceneState.cpp/WorldUpdateDependencies.hpp/NetworkRuntime.cpp/NetworkRuntime.hpp精确重放一组两点及四注释；封包正确body、所有其它source/ETE01与BTG01正文保持。旧1075原格式/字节/接线子记录保历史追加；511900一新语义、两新F处置，INPUT/NETWORK各两成员、MAP一个发送者成员，三限定S零I；九任务继续open，不改变全枚举/完整gates/六closure/总SPEC§10。

只必要文本静态与measure_cg2d.ps1/-Check；未构建/测试/运行客户端或算法/产物/摘要/依赖。完整输入与收信gate生产、历史/输入/文本工作区/协议共享buffer全生产消费与alias-owner寿命、时钟/SDK/机器ABI/数值/优化后读取或并发、真实禁言/连续发送/服务器互通/文本声音画面和全生命周期待；关联率不是完成率。

<a id="chat-send-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐文本转义生产合同并恢复读取顺序”；拟提交说明“[#] 补齐聊天发送合同并恢复选项字节取样”。两完整原根/实际原父发送窗口及一组两点BYTE修正、登记、静态汇总/Check和派生范围核对通过。语义2239=2191F+48X，函数处置1677/证据1473；文档关联2301/3680=62.53%仅关联率，INPUT60/NETWORK87/MAP185且枚举未闭。184叶验收0、60任务关闭11/开放49、六closure开放及所有完整门禁保持。未构建/测试/运行，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_chat_send_pipeline_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐聊天发送合同并恢复选项字节取样」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x50C940 | 0x50CAD6 | 106/406 |
| 0x511900 | 0x5119F7 | 77/247 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
