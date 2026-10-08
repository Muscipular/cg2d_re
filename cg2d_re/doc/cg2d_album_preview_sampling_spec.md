# 图鉴详情预览取样SPEC（BVX01）

<a id="album-preview-baseline"></a>
## 已证差异与保持项

沿BVW01取样修正，单独核旧24图鉴详情`Sub_4F6220`，已有原新1044。该页的两处`SetYobiActionFlag2`处于不同阶段，不能按表面同名调用一律缓存指针。

| 阶段 | 原范围 | 实际合同 | 源状态 |
| --- | --- | --- | --- |
| 有记录 | 4F6501..4F6544，16条67B | 先取action，清record.newFlag，以初始样本判空；映图callee返回后分别fresh取action写图号、清bit1、清0CH | 当前正确保持 |
| 空记录 | 4F6A2D..4F6A3A，四条13B | 一次取D67D14，同指针判空，再OR flags的W32 bit1 | 当前判空/实参重复读取，需持样 |

共20条80B定点。原父边界4F6220..4F6BEC/688条沿已存合同，仅本阶段原证，不宣重核完整父。EBX置零来源及关闭分支恢复后返回另存上下文，空记录从4F64FB直接进入4F6A2D，跳过有记录显示流程。

<a id="album-preview-decision"></a>
## 实施决定

仅将空记录的`else if (共享action != nullptr)`片段改为`else`内一次保存`YobiAction* const previewAction`，同样本判空后调用既有`SetYobiActionFlag2(previewAction,true)`。原空记录支路、W32位1及其它bits、随后格式/绘制位置保持，不增加action保护或其它业务条件。

有记录支路保持：原初始指针在newFlag清零前取样，图号转换后仍分别重读共享action做图号写/清bit1/清0CH。不能拿最初action替换三次fresh样本，也不能把两个掩码合成一次写。原native setter/公共头及全部其它父正文/预览生产、图号数据、格式/原字节/消费者保持，无新增helper、接口、owner、原函数身份或异常模拟。

<a id="album-preview-acceptance"></a>
## 可度量验收

| 项目 | 本步标准 |
| --- | --- |
| 原点 | 有记录16/67B、空记录4/13B分列；相应原输入/比较寄存器和跳转可追 |
| 源修 | 一CPP一片段，只空记录持样；有记录三次fresh和整个其余父/边界源码保持 |
| 原新 | 1044所有旧字段/格式/字节证据保持，新增当前信息；语义1805/有效1798和合同1915保持 |
| 处置/成员 | 一adapted F，UI-L-024纳一个主父，枚举及完整gates仍open |
| 限定证据 | 两I/S，I仅空记录持样；F960→961、E1120→1122 |
| 任务 | U04/U11/V04/R04追加实际进度/引用，仍open；pending18及混合/全门禁保持 |

只做定点必要静态与机器/派生差集、文本汇总/`-Check`。网络兼容原格式、仅明文，音频暂停，RPLAN.24不加保护/异常模拟；不构建、测试、运行、看产物或计算hash。完整父/其它成员及上游/旧外路由、资源生产和正常owner-alias寿命、SDK/机器ABI/画面及运行仍按原门禁验收。

<a id="album-preview-source-state"></a>
## 状态

依工程委托决定只修空记录取样，正确有记录阶段保持；实际修复及静态结果随后记录。本局部不代整个图鉴/全部旧页面或整体SPEC验收。

### BVX01本步实际静态结果

已修复空记录分支D67D14重复读取：一次previewAction持样，同判空和既有SetYobiActionFlag2(true)借同指针置W32 bit1。原四条13B与现一片段对应，EBX零值源及关闭支路恢复后直接返回有据；原空记录从4F64FB进入4F6A2D，不消费有记录显示阶段。

有记录16条67B正确保持：先取action后清newFlag，原初始样本判断；映图51B740返回后分别fresh取action写图号、清bit1和清0CH，原两掩码不合并。整份LegacyUiRecord.cpp只有空记录片段变化，所有其它父/预览生产、格式/字节/消费者、native helper/公共与私有头全文保持，无新函数/helper/API/owner、保护或异常模拟。

集中静态、完整机器和派生差集及文本汇总/-Check通过。旧1044全部原字段保持并追加当前信息，一adapted F、UI-L-024一个实际主父与两限定I/S；实际语义1805/有效1798、合同1915（52.04%）保持，F960→961、E1120→1122。U04/U11/V04/R04仍open；pending18/旧62mixed-pending6/创建pending7、R7已审467余2594/closed11/全叶0/globalfalse和原差队列/CRT43保持。完整688父/其它成员与上游/旧外路由、正常owner-alias寿命、SDK/机器ABI/画面及运行未验；原格式明文、音频暂停/RPLAN.24及执行限制保，整体SPEC目标继续推进。
