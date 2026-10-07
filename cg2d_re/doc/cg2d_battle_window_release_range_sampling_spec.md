# BQQ01 技能与制造窗口关闭区间取样SPEC

2026-10-07。沿[上批四个无IDA父CALL](cg2d_battle_skill_lists_rollback_sampling_spec.md#battle-skill-lists-rollback-sampling-source-state)的实际依赖继续开发，依据已开cg2d.exe只读会话[原字节与前后源体](cg2d_battle_window_release_range_sampling_evidence.json)。按用户委托先写裁决再实施；工程裁决已授权，原事实不猜。

<a id="battle-window-release-range-sampling-baseline"></a>
## 完整原片段与差异

| 原代码入口/排他尾 | 新源 | 指令/实际B/页 | call/jump/ret | 身份 |
| --- | --- | --- | --- | --- |
| 47B0E0..47B10E | src/ui/WindowRuntime.cpp#Sub_47B0E0 | 12/46/1 | 2/3/1 | X:0007B0C1-0007B110内代码 |
| 485690..4856F3 | src/ui/WindowRuntime.cpp#Sub_485690 | 30/99/1 | 4/6/1 | X:00085663-00085700内代码 |

两片段共42条145B、两页、六直接CALL/四实际callee、九普通分支和两RET。每条原代码均没有IDA函数父；485690本身就是入口处CALL，源码名字确为Sub_485690，不能凭“CALL所在地址”否认入口，也不能把猜测485680写成原事实。两原入口各只有原型头数据xref，本步没有新增独立函数身份。

| 差异 | 原实际点 | 修复裁决 |
| --- | --- | --- |
| 引用减计数与窗口借用序 | 4856A7读D67AFD W8、4856AD取window12 T*、4856B2减DL/4856B4发布 | 显式保留旧W8，借window后计算并发布W8 |
| 制造计数发布与谓词 | 4856C1读D67AFE W8、4856C6减AL、4856C8写W8、4856CD消费同减后结果 | 缓存减后W8，先发布，再判断缓存；删除发布后的fresh共享读取 |

一源体两局部片段。普通旧值一致；不以此声称并发、异常或数值故障，不扩展保护、回绕模拟或生命周期逻辑。

<a id="battle-window-release-range-sampling-decision"></a>
## 正常源合同与裁决

1. 窗口10关闭：初次一次D672AC W32 mode为1或2才调用479660。调用后必须fresh mode再判是否2；为2调用4796E0后D67AFD W8减。其余mode直接结束。当前完整源体正确，保持初/后取样差别，不把后mode替换成初值。
2. 窗口12关闭：先479660，再一次CC3E80 W8 mode为1/2才4796E0。旧D67AFD W8取样→借window12 T*→减后W8发布→窗口state W32读写OR 8000→D67AFE减后W8发布/同held结果判0。只有0才51BEC0；原S32 state处于5..7或16才51BE50(0)，弃返回。模式、窗口状态、计数和动作状态不合并，不新空指针分支。
3. 两片段原末尾均EAX=1，但真实绑定为prototype.release。原471276 CALL [ESI+38]后读窗口id，47127E直接覆盖EAX，未消费回调返回。现WindowReleaseCallback=void(*)()及window->release()正确承接已证弃返边界，不为EAX=1统一重构全部释放回调。
4. 原型10 [54E420,54E450)与原型12 [54E480,54E4B0)各48B；实际+2C释放字段54E44C→47B0E0、54E4AC→485690。现WindowPrototypes.cpp对应两行和WindowPrototypeDependencies.hpp声明一致。数据xref来源54E420/54E480是整个原型头，raw头0不等回调地址。原型+2C到实例+38借用函数指针，保持现原生T*，不能截成W32。
5. 479660/4796E0刚已纳管，51BEC0/51BE50既有承接保持。本步只核实际六CALL及该局部调用序，不重验全部父/通用窗口构造与回收，也不重定义MIS或klF格式，传送仍为原格式明文。

## 两X区间全部字节用途

| 原库存区间 | 连续分项 | 真实数据消费者 |
| --- | --- | --- |
| [47B0C1,47B110) 79B | 3B LEA ECX,[ECX]对齐；七W32表28B；关闭代码46B；2NOP | 47A82F JMP [47B0C4+ECX*4]，七目标均在已有47A7A0内 |
| [485663,485700) 157B | 1NOP；四W32表16B；26个W8索引；2NOP；关闭代码99B；13NOP | 4855A4先读485674 W8索引、4855AA JMP [485664+ECX*4]，四目标均在已有4854A0内 |

共236B分项无缺口，不把IDA显示align4的首3B误说成三个NOP；只读decode证实LEA长度3。26索引值为0/1/2/3及末尾2，与原四项目标对应。表内容/唯一外部引用/目标归属均保存，只验表身份和用途，不据此验收47A7A0/4854A0完整源体。两个区间为mapped，parts分别保alignment_bytes/jump_table/indirect_switch_table/source_mapped_code/nop_padding；不排除业务。

身份采用已有code_identity.kind=unassigned_range_fragment，绑定原X和确切片段边界；inventory_id=null。不会新造F、将整个X当一个函数或改3680分母。R7 262→264/3061，剩2799→2797；F分类667、文档关联1654/3680（44.95%）、BATTLE219枚举open保持。语义1541→1543，无独立F记录5→7/有效代码区间身份5→7，有效F1536保持。

<a id="battle-window-release-range-sampling-acceptance"></a>
## 可度量验收

- 两原代码片段逐条连续边界、42/145B/两页、六CALL四callee/九jump/两RET及两个EAX=1。
- 两源体完整前态冻结，源码全文只Sub_485690两授权片段；47B0E0和原W8/W32共享储存保持。原六CALL与实际源码callee、mode前后差别、计数发布/held条件、W32关闭和S32动作条件对应。
- 两原型各48B实际释放字段、现对应原型行/typed声明及局部471276弃返消费一致；不改变全部回调ABI或上层暴露底层类型。
- 两X所有236B连续分项、11个W32目标/26个W8索引/唯一消费者/LEA及NOP可追；原库存/函数目录不修改。
- 语义两有效代码区间身份、两R7 mapped、局部I/S与五任务U08/R01/R03/R04/V04只增本批证据；原完整门禁、60任务状态和用户六项改动保持。定向文本静态与measure/-Check完成后提交。

禁止构建、测试、运行、摘要计算及RPLAN.24专项故障模拟继续有效；当前验收仅正常源和原字节静态对应。完整技能/制造数据生产、其它callback/父、全部窗口/owner寿命、全部dispatch/SDK/机器ABI/画面运行与R8必须分别完成，不以mapped、局部I/S或引用存在代替。

<a id="battle-window-release-range-sampling-source-state"></a>
## 当前交付边界

先落本SPEC和完整证据，再修改一源体两片段、追加两个无F原代码身份/两区间处置、同步五任务/唯一进度并生成量化看板。快照：`cg2d-re-battle-window-release-range-sampling-20261007`。原正确关闭体/void弃返适配、回退/动作正常链与原明文网络保持；不声明完整战斗/制造/整体SPEC已验收。
