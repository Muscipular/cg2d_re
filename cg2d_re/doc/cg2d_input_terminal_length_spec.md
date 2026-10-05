# ICL01 下移光标终行正常长度算术

<a id="input-terminal-length-baseline"></a>
## 原合同与当前差异

原44A7D0完整137条441字节，唯一直接调用4596D0的4596FC；[本次证据](cg2d_input_terminal_length_evidence.json)保完整指令与七条调用邻窗。[既有编辑合同](cg2d_input_edit_alignment_spec.md)及[下移叶说明](cg2d_input_key_dispatch.md)继续适用；本步只推进终行含NUL长度算术，不重新验完整输入分派、其他编辑或音频。

44A901..917先确定目标行/列；44A91B使用当前上下文+已推进offset，44A923另一次repne scasb，44A92A not ecx得到含NUL的W32数量，44A92E写行长槽，随后44A932才按signed目标行与已记录行数比较早退。前面的44A875保存S32行宽、44A880/882 unsigned余串比较与分类调用预算是另一合同，不改。

当前Input.cpp终行独立strlen结果却存U32，+1U后bit_cast回S32。它显式承载数量回绕；原局部表是288项S32、光标/列/偏移也按S32消费，本步按RPLAN.24恢复正常S32长度+1，保正常含NUL数量，异常域不模拟或称等价。

<a id="input-terminal-length-decision"></a>
## 委托裁决与源码变更

依[工程委托/RPLAN.24](../AGENTS.md)，只将终行terminalLength类型及显式转换改为S32，并将写槽改为terminalLength+1。保现有strlen在同一对象+offset独立取样的位置；不合并余串扫描或把NUL行长改成不含NUL。

[InputContext](../include/cg/Input.hpp)原0x120字节文本、caret@+128、maxLine@+144、maxCharPerLine@+148和S32局部表保持；不扩宽非地址数量，不加溢出、NUL、行数、最大长度保护/拒绝/钳制/补零或回绕模拟。其他bit_cast用于真实位型/字段或既有合同者不顺带删除。

保持null/IME门禁、相位20、maxLine门禁、CR扫描、原unsigned预算比较和Classify参数、分类后的当前对象重读、目标/列决定、末槽写后早退、光标增量及最后边界分类/减1。唯一父调用与私有声明不改，不移动焦点/数据owner或新增处理器。

<a id="input-terminal-length-acceptance"></a>
## 本步限定验收

1. 完整原体137条441B、原库存边界与唯一caller身份及调用邻窗可追；原终行not ecx、槽写、signed早退和之后光标更新保持。全体作为原基线保存，本步限定复核终行/局部接口，不重验完整137条所有语义或全部父。
2. Input.cpp仅三行对应片段变化，逆换后与本步前态全文一致；两处strlen的先后/对象和余串unsigned比较、terminal正常S32、其余编辑源码/声明/父调用保持。
3. 已有F:0004A7D0语义及equivalent处置只加本步依据，身份/原处置类别/成员/分母不增；两个功能原14/22成员保持，I/S各一仅本次源码和限定静态。
4. T01/T02/R04/V04保持open，完整FUNC-INPUT/FUNC-TEXT、所有父/焦点/IME/间接alias/借用与寿命、实际PC53/机器ABI/库优化/SDK/显示/运行仍未验；R1/R7/R8与整体SPEC仍待。数学溢出/越界不专项处理，也不称异常安全或故障等价。

<a id="input-terminal-length-source-state"></a>
## 当前交付

下移光标的终行含NUL长度恢复正常S32算术，独立扫描和末槽写后早退不变；既有SPEC当前说明、语义/处置依据、任务/看板/唯一进度同步。本步不是新开发完整编辑或全输入验收。

固定命名快照：`cg2d-re-input-terminal-length-20261005`。
