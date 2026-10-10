# 聊天历史浏览与输入滚动 SPEC（HBN01）

2026-10-10。沿BTG01/CTS01提交链核历史的实际选择、输入恢复及字节滚动提供者；依子AGENTS第8条先记证据与裁决。统一cg2d_re分支、不打tag；不改变GBK、原格式明文、MSVC直接CRT或WAV-only，也不恢复构建/测试/运行。

<a id="chat-history-baseline"></a>
## 原根与真实共享数据

| 原根 | 完整范围（尾不含） | 指令/字节 | E8/跳/RET |
|---|---|---|---|
| 44A1B0，历史浏览与输入滚动 | 44A1B0–44A40E | 177/606 | 7/21/3 |
| 44B440，字节边界类别 | 44B440–44B49D | 45/93 | 0/13/4 |
| 4A80E0，聊天字节宽度 | 4A80E0–4A80E8 | 3/8 | 0/0/1 |

共225条707B/五页、七实际E8/四目标、34跳八RET。26实际E8入向内部4/外22，仅来源：浏览12点/两父，类别12点/九父，宽度两点/同一浏览父；不把数量当完整父/生命周期验收。

现有历史唯一块ChatHistory621FF8是64项各0x121字节记录及尾W32，共0x4844B，tail偏移0x4840；gCursor62683C是独立S32，不在文件块内。原浏览中index*0x121地址表达式与现有struct/字段和静态断言文本相符，记录地址只原生指针拓宽，索引/尾/游标不拓宽，不造影子历史或新布局。静态断言存在不代编译布局验证。

旧707的44A090和708的449FA0已核两完整文件根131条及原路径/mode/传输尺寸/失败序，本步复用其历史证据和现有MSVC FILE域，不重建CRT或重复原身份/处置。读整0x4844块及短读已写字节保；rb失败后尝试wb创建，游标63/返0。r+b更新先seek index*0x121并写一项，成功才seek0x4840写参数index四BYTE，seek结果弃用、写不足/打开失败原返0，关闭与后续保；本步不改这些正文或新增索引门槛/回滚。

<a id="chat-history-decision"></a>
## 输入门槛、选择与恢复

44A1B0先真实空509080→当前context一次取样，只有626AA0且IME首BYTE为0继续。Ctrl BYTE bit2先读；向上还需Shift bit2为0且Up bit10非零；否则向下复用同一Ctrl值，只再判Down bit10，不增加Shift门槛。按键读取顺序与BYTE宽度保，不改输入控制业务。

向上先读previous S32/尾W32的S32位型视图；previous==-1选tail，否则previous-1，先发布新游标，负值再发布63。先读所选记录首BYTE，空记录或回到tail且previous!=-1则恢复previous并返回。向下先读游标，-1返；与新取tail相等则游标=-1、按入口持有context清length，再新取context清text首BYTE，再新取context清caret，返回。其余先发布selected+1，>=64再发布0，不新增空记录门槛。

有效选择：入口持有context清length→新context清text首BYTE→新context清caret→从当前游标取记录并44A160装入→再次取游标对应记录复制含NUL到未清256B临时区→strlen字节长度S32→4A80E0宽度S32→offset=length-width→44B440按原低W16/S16参数视图判类别→AL==2才offset++→原offset<0归零→新context写field158 S32。保LoadInitialText调用前后两次独立游标读取，不跨callee持有记录或context；两原分支各有上述尾，现共用代码动态每路一次，不把两个原CALL点误成动态两次。原负滚动量归零是业务事实，继续保持。

44B440原将S16参数符号扩到S32，与cursor S32比较；每轮只读当前BYTE，原范围[81,9F]或[E0,FC]时cursor+=2，否则cursor++。结束cursor不等于offset直接返低BYTE2，等于才再读该BYTE并按原范围返0/1。只提供既有字节类别，不是GBK/Unicode合法性检查；不因当前GBK许可擅改原范围，也不增加NUL、空值、长度或容量门槛。三个返回值和负S16原分支保，不专项异常/越界/溢出模拟。

4A80E0原从C9A930读W32后加自身，返S32；当前同一gChatWidthUnitsC9A930*2正常表达保持。该字段所有生产者、文本展示和实际宽度/初始化全owner尚待，不因三指令证明完整渲染/输入功能。原S32运算不新增回绕或溢出模拟/钳制。

三个正确正文保持，零逻辑组；三定义/三真实公开声明六关联注释，不新增函数/API/类型/字段/布局/owner/分配释放。RPLAN.24不加保护/默认值/补零/钳制/索引容量限制/回滚或异常模型；原空/模式门槛、游标环/恢复、清字段和负滚动归零照原保。

<a id="chat-history-acceptance"></a>
## 限定静态验收

核三完整连续原根/实际E8/34跳八RET、26实际入向及宽度/类别取样，整Input.cpp/Input.hpp/WindowRuntime.cpp/WindowRuntime.hpp精确重放六注释，三个body及旧文件根/BTG01/CTS01/UTE-ETE全部其它source保持。现有独立游标、历史struct/字段/断言及原地址步长有据，不对全文件/短读或全部外父新宣运行通过。

三个新语义/F，INPUT/UIRUNTIME各三个成员、MAP一个浏览成员，三限定S零I；U08/U09/U10/U11/G01/R01/R04/V03/V04九任务继续open。所有旧身份/处置/证据/政策、枚举/完整gates/六closure与总SPEC§10不变。必要静态及measure_cg2d.ps1/-Check不代编译/运行。

完整两个外浏览父/类别其余调用、按键及IME、历史初始化/文件/提交/游标/输入/字节宽度所有生产消费与alias-owner寿命、CRT/SDK/机器ABI/数值/优化后读取和实际上下翻页/原字符边界/消息画面与全生命周期待，关联率不是完成率。

<a id="chat-history-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐聊天发送合同并恢复选项字节取样”；拟提交说明“[#] 补齐聊天历史浏览与字节滚动完整合同”。三完整原根及现有共享数据/正确正文、登记、静态汇总/Check与派生范围核对通过。语义2242=2194F+48X，函数处置1680/证据1476；文档关联2304/3680=62.61%仅关联率，INPUT63/UIRUNTIME95/MAP186且枚举未闭。184叶验收0、60任务关闭11/开放49、六closure开放及所有完整门禁保持。未构建/测试/运行，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_chat_history_navigation_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐聊天历史浏览与字节滚动完整合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x44A1B0 | 0x44A40E | 177/606 |
| 0x44B440 | 0x44B49D | 45/93 |
| 0x4A80E0 | 0x4A80E8 | 3/8 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
