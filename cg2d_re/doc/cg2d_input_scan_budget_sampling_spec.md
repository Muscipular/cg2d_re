# BPG01 输入追加的扫描预算与分类判定取样

<a id="input-scan-budget-baseline"></a>
## 具体差异与原证据

AppendChar_44ADB0的原插入、限量追加、截断和回滚已存在，本步只修两处取样次序：源逐字节扫描先++scanned才读预算；分类后先读新预算才判断返回3。原端分别先读取预算再递增、先保存分类判定再读预算，不能以最后正常值相同代替这些源阶段。

[完整原证据及全文前态](cg2d_input_scan_budget_sampling_evidence.json)保F:0004ADB0/[44ADB0,44B044)全部182条660B/三页，6个绝对call、21个内部跳转和1个ret；4普通直接点对应44A160/44AB60/459580/466DC0四父，仅入向身份，不代完整父或运行。

| 原范围 | 当前实施决定 |
|---|---|
| [44AEB9,44AEC4) | 已通过NUL/CR判断的每个普通字节，先读同次上下文S32预算，再推进扫描计数并沿该样本比较 |
| [44AEE4,44AEFA) | 另行保存行预算、strlen低U32/unsigned比较与实际44B1D0调用，正确源保持 |
| [44AEFA,44AF16) | 分类后先重读当前上下文，保存boundary==3，再读取新S32预算并沿已保存判定推进 |

两种预算取样不同：逐字节预算每个普通字节都读，不能冻结为整行一次；44AEE4为余串长度比较/分类保存的另一次预算，也不能拿来代替前者。

<a id="input-scan-budget-decision"></a>
## 工程裁决

依既有委托先记SPEC后实施，只修改Input.cpp的AppendChar两个片段，不新增接口、helper、owner、规则或依赖。

1. NUL/CR两个原分支之后，保存characterBudget=context->maxCharPerLine，再++scanned，用scanned==characterBudget决定停止。S32位宽和原每字节读取次数保持；NUL/CR路线不额外读取预算，不引入负/零预算拒绝或限长规则。
2. 44B1D0返回后先重取gCurrentInputData，保存splitDoubleByte=boundary==3，再读该上下文nextLineBudget。沿该局部判定选择nextLineBudget+lineStart-1或普通推进；bool只是已证判定，不是新业务状态。
3. 原S8字符过滤/允许NUL、两未预清289/288B副本、三StrNCpy Count289/288/288、StrNCat的287减当前strlen低U32、余长unsigned比较、所有当前上下文重读保持。
4. maxLine/626C20仍无条件各取一次并供两超行分支共用。原CR行折叠、孤立高位校正、截断/光标钳制、仅caret==total-1的回滚、先恢复total再caret、626C28=1与接受尾相位20全部保持。
5. 正常无介入的旧文字结果一致；不宣称原数值或画面错误、并发等价、机器指令/ABI通过。RPLAN.24不专项模拟异常、越界或数学溢出，不增保护、补零、拒绝或编码猜测/转换。

原四父和其它实现/头、24项实际预设/聊天存储与输入owner保持；不补猜旧99表或键表外层分派。网络兼容原格式只明文、两加解密叶排除、内存CryptInt及音频暂停保留。

<a id="input-scan-budget-acceptance"></a>
## 可度量验收

必要静态核182条660B/三页6绝对call21内跳1ret和四普通点/四父；两个片段逆换后整Input.cpp与前态逐字一致，预算在普通字节路径正确取样、分类判定在新预算之前，原控制/回写和全部其它源码/头保持。

续记旧368/F:0004ADB0、归档全文旧映射和原处置；equivalent分类与两域成员保持。语义1422条/1417原ID、关联1535/3680（41.71%）、分类543、INPUT14/TEXT22枚举未闭；E703→707仅两域各I/S四条正常局部证据，不算新增四功能或整输入通过。

T01/T02/U04/R01/R04/V04只加进展与证据引用，仍open；60任务源码closed10/完整验收0、叶验收0/global_accepted=false及所有完整门禁保持。旧99路由/输入焦点、所有正常alias-owner/寿命及实际SDK/ABI/链接和运行继续独立验收。

<a id="input-scan-budget-source-state"></a>
## 当前交付

交付AppendChar两个取样片段修正、完整原证据/四父身份、旧368映射续记和四正常局部I/S。命名快照：`cg2d-re-input-scan-budget-sampling-20261006`。

完整四父/原字节生产与文本绘制/键表和焦点、旧UI路由、正常alias-owner/重进与退出寿命、SDK机器ABI/链接及同输入画面运行仍未闭。只必要静态和measure/-Check，未构建、测试、运行或计算摘要，整体SPEC继续。
