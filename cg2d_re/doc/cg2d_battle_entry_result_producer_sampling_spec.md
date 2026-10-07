# BQX01 战斗开始/退出、战果生产与W16消息叶SPEC

2026-10-07。沿[BQW01原消息队列与初始化](cg2d_battle_command_init_packet_producer_sampling_spec.md#battle-command-init-packet-producer-sampling-source-state)，补原战斗入口、战果生产及相邻W16消息叶。依AGENTS.md第8条先裁决再实施，[三完整原根、源前后态与W32桥](cg2d_battle_entry_result_producer_sampling_evidence.json)为依据。

<a id="battle-entry-result-producer-sampling-baseline"></a>
## 完整原基线

| 原入口 | 现符号/文件 | 指令/B/页 | CALL/jump/RET | 台账 |
| --- | --- | --- | --- | --- |
| 50D120 | Sub_50D120 / world/SceneState.cpp | 161/656/3 | 20/16/3 | 1568新正常F，五片段修复 |
| 50D500 | Sub_50D500 / world/SceneState.cpp | 159/509/2 | 14/8/1 | 1569新正常F，六片段修复 |
| 50D3B0 | Sub_50D3B0 / network/NetworkRuntime.cpp | 3/12/1 | 0/0/1 | 1570新正常F，正确体保持 |

323条1177B/六页，34直接CALL/19callee、24jump/五RET无switch；三真实caller51328A/513306/51513A均来自512E40，只登记局部绑定，不推完整5779条分派验收。50D500的__alloca_probe是原callee元信息，本批用既有三2048B自动buffer承接正常存储；不验其叶/栈故障、ABI或新增CRT身份。

| 差异 | 原证据 | 本步裁决 |
| --- | --- | --- |
| 数值载体四处取序 | 三正路径mask→encoded→W8 count读齐后pub；非正type count→encoded→mask读齐后pub | 四组局部cache，保各组独立序/真实construct-destroy与RNG，不把赋值边读边发布 |
| type谓词阶段 | 50D2AD type2 CMP→flag/type pub；非2 50D2E8 type3 CMP→ModeFlag0 pub | 显式type2/3/其余分支及原W32/W16写序 |
| field递增阶段 | 50D54F/50D6B4 INC在首parser前 | 四结果与十五技能各Extract后、首Parse前递增，去各末重复 |
| 负值判断载体 | 50D6BD pub AX→50D6C1 TEST AX；50D6DD pub EAX→50D6DF TEST EAX | 持有解析低S16/S32值，pub后据原返回值判负，不重读输出字段；原零写保 |
| 玩家flags跨W32桥 | 50D28D MOV DWORD FDA88A；旧helper分读W16与W32再拼 | 一次4B memcpy观察flags18A及相邻flags18C低W16，布局adjacency assert，不扩共享字段 |

两入口11片段（5/6），共用W32桥一处、同文件布局assert及注释两片段。普通旧结果一致，不称并发/数值/故障修复。root ReadPlayerFlags18A既有声明为U32，参数/返回与所有旧caller不改；原存储为flags18A W16和flags18C W32，禁止改字段宽度或新shadow位槽。

<a id="battle-entry-result-producer-sampling-decision"></a>
## 入口、结果及原消息裁决

1. 禁用9A6E0C W32非0直接返。D67E78 W32非0先CAFEA0零才原504410(1)/真实W32返回pub；随后519D50/4464D0/4681B0/51B0F0、62B5B0零/455FC0/455C60原调用序保，不合并清理或凭业务直觉加失败早退，音频继续暂停。
2. 9FB918载体原mask/encoded/count三读先齐再pub C821E8的现代码正确保持。之后Construct0→mask/encoded/W8 count读齐→C821D8依序pub→Destroy；Construct1→同读齐→ripple gate依序pub→Destroy。三个W16状态清零→SetScene(9,1)→YobiClearUp→446610→Construct0→同读齐pub gate→Destroy→fresh socket/原5108F0(...,0)明文弃返。数值载体是现正常数据机制，不增加网络加解密。
3. 正type保follow W16门禁、玩家FDA88A跨W32/AH bit100、CAFEA0 W32短路与原音频调用。type2谓词先flag5ABFC4/type pub；type2 ModeFlag W32=1/C821D0 W16=0/B65A7C W16=1。非2 type3谓词先ModeFlag0，type3 C821D0=1/B65=0，其它C821D0=0/B65按原2/4/5/6条件。背景参数与其前后两个状态、四队列读写索引/两个临时文本首BYTE、四槽各1000H仅首BYTE清零、pending1和return按原序保，不清整缓冲或新增状态。
4. 非正type Construct0后W8 count→encoded W32→mask W32先读齐，再count→encoded→mask pub C821D8→Destroy→FF03D8/F7CA04两个W16零，区别于正路径；四次Construct/Destroy顺序和原RNG消费保持，禁止改固定成功或纯常量载体。
5. 玩家原跨字段W32读通过现ReadPlayerFlags18A实现：std::memcpy4B从真实flags18A起始，只读flags18A与flags18C低W16；private PlayerRuntimeSlots的相邻offsetof断言保证该读取布局，字段注释同步。不新开独立hpp/cpp、不暴露玩家首块或SDK类型，不新增字段/拒绝/钳制，其他位写者和旧调用不改。
6. 战果入口同禁用门禁，ready FF0C10真实W16=1；三个2048B未清栈buffer row/entry/name和每次原2047提取容量保。原__alloca_probe属自动栈承载机制，本批仅记录，其全CRT/机器ABI门禁不据char数组代验。
7. 原第1..4列各','提row，field在第一个'|'整数parser前加1；各8B记录低U16 value00/value02分别第1/2字段，第3字段原Base62写S32 value04。第五列提row后field加1，三组用'|'分entry→原Unescape52D150→图号整数第1字段→姓名第2字段/原Unescape2 52D2F0。原姓名strlen<=28复制含NUL至29B槽，否则只首BYTE=0，不清其余28B或替换编码/截字符串；三个graph/name输出槽保原序。
8. 第6..20列十五8B技能结果：','提row后field在首'|'parser前加1；第1字段低S16 pub后按持有AX判负，负则原W16零；第2 Base62 S32 pub后按持有EAX判负，负则原W32零。两pub-before-test及原负值业务保，不能因RPLAN.24删除原分支，也不扩成新保护/异常模型。record+2两字节原不写，源码与布局保持。
9. 原4/3/15数量与8B/29B布局已正确，不改上限/表/字段类型。comma/bar、原Base62、两不同Unescape链及返回弃置都保，不拒绝提取/解析失败或补零整个输出，不改协议/加解密；所有host边界保持现原字节编码和通知路径，完整delimiter/escape/CRT及所有result consumer另按既有基线推进。
10. 相邻50D3B0经实际get_func与快照确认是已有三指令F（非无父片段）：MOV AX第二形参低W16→MOV FF0D34 WORD→RET，当前uint16返回源码正确。原51513F清实参后515142 XOR EAX覆盖返回，upperEAX不消费；first/socket参数未读，不借FF0D36邻槽，现sZWu路由/声明/返回保持。不把[50D3B0,50D3C0)全16B当R7，仅实际后4B填充归X。
11. 三入口来自现512E40的局部接线，父已有1216局部解码映射但完整5779条未验，不全面重核；入口复用已纳管清理/场景/Yobi与BQW队列生产链。全部owner重进/退出、其它战果consumer、资源/SDK/ABI/画面及全技能/pet生产门禁继续推进，音频暂停不作范围排除。

## 两R7与台账

真实X [50D3BC,50D3C0)4B、[50D6FD,50D700)3B共7B，各全90、逐byte只读NOP decode/无入向，前完整根RET/后真函数入口。初看[50D3B0,50D3C0)包含12B已知函数，经快照/IDA及源定位当步纠正；不伪造无F identity/扩大R7区间。mapped只填充用途，R7 273→275/3061余2786。

三新正常F1568..1570/三equivalent/BATTLE250→253枚举open；语义1568→1571/有效1561→1564、文档1679→1682/3680（45.71%）、分类698→701/E798→800。无F/有效区间各7、差队列172/缺定位171/43待归属、五任务U08/R01/R03/R04/V04只本批当前与引用、60源码closed10/完整验收0/叶0/global=false及全部完整门禁保。

<a id="battle-entry-result-producer-sampling-acceptance"></a>
## 可度量验收

- 三完整单chunk连续原字节、323/1177B/六页、34directCALL19callee/24jump五RET无switch、三个父局部入向与W16叶caller覆盖EAX；不据栈探测或暂停音频元信息验callee本体。
- SceneState整文件仅两主根11片段、W32桥一处/布局assert-注释两片段；正确network叶全文/旧声明与布局/RNG叶/实际delimiter-Base62-Unescape边界保持。四载体先读齐后pub、type2/3谓词阶段、field递增与pub后持有返回值判断以及所有原失败/负值零写保。
- 原4/3/15数量、W16/W32与三2048B/2047提取、29B/长度28/只清首BYTE、未写+2 padding、全部返回/重读/清理/原格式明文对应，按静态证据验不运行样例或增异常模型。
- 两X共7B全文/NOP逐byte/无入向/前RET后函数用途；三新F/处置/253open成员、两局部I/S/五任务/完整派生快照及冻结定义/引用/编码/diff、measure/-Check仅文本汇总。

<a id="battle-entry-result-producer-sampling-source-state"></a>
## 本步交付

先落SPEC与证据，实施两主根11片段和既有W32桥/adjacency注释-断言，纳管正确W16叶，再同步台账/五任务/唯一进度/派生看板，精确提交12文件并固定快照`cg2d-re-battle-entry-result-producer-sampling-20261007`。所有适用运行门禁仍未验，整体目标保持；全分派/全部owner和窗口寿命/全生产/其它战果consumer/CRT/SDK/机器ABI/画面与R8另验，不降低标准。音频暂停/不构建测试运行摘要/RPLAN.24和原协议明文保持。
