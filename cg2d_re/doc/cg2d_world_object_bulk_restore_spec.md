# 世界对象整表清引用与批量补建 SPEC（WBR01）

2026-10-10。沿WLR01/WOP01核两个既有完整正常根和五实际接线；当前主体正确，全部保持，只补两CPP/两头关联注释、两个独立语义/F及MAP/YOBI两限定S。无新功能代码、接口、布局或业务门槛。

<a id="world-object-bulk-restore-baseline"></a>
## 原根、表范围与接线

| 原身份 / 当前符号 | 完整范围（尾不含） | 指令 / 字节 / E8 | 实际入向 |
|---|---|---|---|
| F:00046610 / Sub_446610 | 446610–44663B | 10 / 43B / 0 | 43D2A8、43D43A、50D228 |
| F:00046640 / Sub_446640 | 446640–4467C4 | 124 / 388B / 3 | 51C82A、51CF8E |

合134条431B/三页，三E8/两callee、15跳/两RET，无switch。两callee为401EA0（两次）与441DB0，复用219/1298；401EA0已证唯一jmp401EC0，当前Read_401EC0是同一读取，计数/RNG不增减，不恢复私有包装。

两个循环都扫1000项：清引用原游标从5AC5DC至5F983C、步长13Ch，都是表base加action偏移11Ch；补建从5AC4DC至5F973C、步长13Ch，都是base加coordinate1C偏移1Ch。原base为5AC4C0，末界5F9720；只拓宽已证action指针，当前数组原生步长沿既有x64布局。不能把IDA终点名称或表外成员地址当新容量/SDK返回。

五实际入向/三父43CBB0、50D120、51C7D0，只核每CALL前六/后八条短窗，不称三完整父或把相邻RET/其他分支当动态后继。三清引用CALL前都实际调用404D30，源码三个对应点先YobiClearUp_404D30再Sub_446610；两补建点都在当前普通场景Sub_519A80→Sub_455C60→Sub_446640序。保当前SceneState.cpp接线、三父旧1349/1568/1580及清理旧1132，不代完整父/全owner取证。完整原指令、短窗、当前源码与差集见[证据](cg2d_world_object_bulk_restore_spec.md#evidence-archive-20261011)。

<a id="world-object-bulk-restore-decision"></a>
## 工程裁决

依子AGENTS第8条先记本合同，再保持两个正确body和所有父/共享提供者，只补四关联注释及完整登记。清引用与有释放整表清理4464D0、无释放全字段重置446470各自保持；批量补建与单编号4467D0各自保持，不因调用者属于换场/退出或重进就合并。

两个现void观察边界保持，清引用原尾EAX是表外成员地址，两个父的三个实际调用点均弃该值；不恢复x86机器ABI、返回SDK地址或新指针接口。原生action T*是槽的非拥有引用，单次清槽不证明主对象或payload已释放；404D30沿旧合同，仅确认先后接线，不在本步重判完整owner。

## 清引用446610

全1000槽只按used W16非零选中，依次action置空、value34 W32=-1。未占用槽全部保持，选中槽的used、category、计数/上界/查找/空闲游标、flags、名字、坐标等也保持。原无CALL，不标死/free主对象、payload、附件或文本，不增加析构、重置或全表memset；主体正确保持。

## 批量补建446640

全1000槽仅used W16非零且action空时处理；先direction、coordinate1C Read、coordinate0C Read各一次，再完整imageNo W32位型交441DB0(image,mapX,mapY,direction)。原DB gameX/gameY名字不独立作换轴依据，保参数次序/Read计数/RNG。创建返回先发布action，空就进入下一槽，不回滚计数/used或填默认对象。

成功used W16=2，fresh action.payload.index=当前槽S32，然后取完整category W16。类别1/8先fresh action与level2C写field1D0，再fresh action.nameC4目标先于strlen，用name54含NUL原字节复制，无新16门槛。类别2/4用nameC9，原28/16门槛；成功在第二strlen前重取动作名字目标，过长复制原554590处"???"含NUL四字节，沿WOP01已证常量。

四已识别类别最后fresh action、fresh flags114完整W32 OR action.flags。**其他类别仍保已创建action、used=2与payload.index，只跳过名字/附加flags**；不能撤销创建、清槽、释放或把类型筛选提到创建前。任何类别都不补titleE1、field1D8、payload.value18或类别2专用值；方向/图片基础初始化沿既有441DB0，不重解析记录文本。保当前主体。

<a id="world-object-bulk-restore-acceptance"></a>
## 限定静态验收

核两完整连续机器码/1000项原边界与步长、三E8两callee/15跳两RET、五真实入向与五短窗；两个完整当前body、W16门槛/未知类别成功后继续、坐标读取/创建先pub、fresh对象/名称/level/flags及原四字节常量绑定。整两CPP/一头仅两定义前注释与两声明注释精确重放；SceneState.cpp五真实调用、三个旧父/清理/两callee六旧身份、结构与其它来源全部保持，不对未变内容机械重验。

两新语义/F、MAP/YOBI各两成员、两限定S零I，七任务仍open，184叶验收0、60任务关闭11、全部member枚举/完整gates与六closure保持。所有输入/三完整父/传递callee/正常失败/容量/alias/owner/SDK/机器ABI与画面声音/运行仍待，不以清引用或补建局部合同称完整生命周期。

RPLAN.24不专项处理/模拟异常、越界、溢出，不新增保护、默认值、补零、钳制、输入限制或回滚。保MSVC直接CRT、SDL/WAV-only、原明文/文本字节和已批准数学边界；只读当前IDA，不重开/改库，不构建/测试/运行客户端或算法/检查产物/摘要，不引依赖，总SPEC§10不降。

<a id="world-object-bulk-restore-source-state"></a>
## 当前状态

本步前Git提交说明“[#] 对齐世界对象记录生产与坐标取样”，完成提交说明“[#] 对齐世界对象整表引用清理与批量补建合同”。统一cg2d_re分支、不打tag，先记SPEC，两个正确body及五接线保持，仅四关联注释；核心静态、登记、文本汇总、Check及派生范围核对通过。语义2200=2152F+48X，文档关联2262/3680=61.47%仅关联率，F处置1631/证据1434，MAP151/Yobi81枚举及验收仍false；派生仅六metrics/两F/两功能/七任务变化，184叶验收0、60任务关闭11、六closure开放及全部完整gates/其它字段守恒。清引用父数措辞已更正为两个父的三个调用点，五总入向/三总父、机器码及正文保持。外部七项及删除暂存保留排除，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_world_object_bulk_restore_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 对齐世界对象整表引用清理与批量补建合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x446610 | 0x44663b | 10/43 |
| 0x446640 | 0x4467c4 | 124/388 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`measurement_check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
