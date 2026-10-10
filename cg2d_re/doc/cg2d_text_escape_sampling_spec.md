# 文本转义生产端原位取样 SPEC（ETE01）

2026-10-10。沿UTE01反转义实际提供者核发送端完整原根；按子AGENTS第8条先记方案、证据和裁决。使用同一cg2d_re分支、不打tag，保持当前GBK、原格式明文、MSVC直接CRT及WAV-only边界。

<a id="text-escape-baseline"></a>
## 原根与数据

| 原根 | 完整范围（尾不含） | 指令/字节 | E8/跳/RET |
|---|---|---|---|
| 52D1B0，文本转义生产 | 52D1B0–52D2C6 | 95/278 | 0/11/1 |
| 52D2D0，空操作后转义包装 | 52D2D0–52D2F0 | 13/32 | 2/0/1 |

共108条310B/三页、两个实际E8/两个目标、11跳两RET。112实际E8入向内部1/外111，仅来源：生产端10点/6父，包装102点/67父，不据此宣完整父或功能通过。52D2E6真实CALL生产端、52D2D6真实CALL509080；后者已在UTE01核为单RET/无参数读取，保持当前无参VOID接口，不造通知/成功值或机器EAX残值保证。

58E944原八BYTE与当前数组均为0A 6E 2C 63 7C 7A 5C 79，LF/逗号/ASCII |/反斜杠按原四对顺序转为反斜杠加n/c/z/y。非匹配普通低bit BYTE保持。原表扫描采用地址步进及W32计数；当前pair为非地址size_t，恢复S32，表地址只在原生指针承载拓宽。与UTE01消费端共用同一真实表，不另造协议或编码表。

<a id="text-escape-decision"></a>
## 正常分支与读取写入顺序

生产端固定入口strlen字节长度S32、source/destination索引S32、used=1/nextWidth=2/adjustedLength=sourceLength。原入口长度非正直接末NUL/返回destination；正常先used<capacity，每轮只读首BYTE到BL，bit80分流，末source++与固定入口长度比较。原容量、长度及高bit分支门槛是业务事实，保留，不加新空值/末字节/数组容量限制或补零。

普通未匹配路径在52D23F写已读BL，然后destination++/nextWidth++/used++，不重新读source。高bit路径原先nextWidth<capacity，再used<adjustedLength；通过后52D294写已读BL→destination++→nextWidth++→52D299读source+1→used++→source++→52D29F写所读下一BYTE→destination++→nextWidth++→used++，再共尾source++。现代码重复取首BYTE且先source++后读下一BYTE、合并两个计数更新；恢复原值持有及该读写/更新顺序，保source/destination可能别名时的实际顺序，不假定二者不重叠。

匹配路径原52D253比较nextWidth/capacity，52D255在跳转前读取转义字母BYTE，52D25C才按比较结果早退。源CPP无可见CPU标志位；用先取字母、再按原nextWidth>=capacity条件分支承载同一读取在早退前发生，不宣生成相同指令。通过后52D266/26A先连续两BYTE写反斜杠和已读字母，52D272才destination+=2，随后nextWidth+=2/used+=2/adjustedLength++。现代码容量早退后才取字母且两写之间destination++；恢复先取字母及两写后更新索引。转义扩张仅adjustedLength++，入口sourceLength不改，原截断/NUL/返回不变。

三逻辑组：S32表计数；普通/高bit首BYTE持有、下一BYTE与更新序；转义字母在早退前读取且两写后索引更新。52D2D0正确包装正文保持，原先空操作(source，未读)再生产(source,destination,capacity)，透传destination；现EscapeInputText_52D2D0仅把既有InputContext::text转入同一包装，是现有原生便利适配，无第三原根/新身份，不改该正文。两定义/一个适配定义及三个公开声明共六关联注释，不新增函数/API/类型/布局/owner或分配释放。

RPLAN.24保持：不专项异常/越界/溢出模拟，不添加保护/默认值/补零/钳制/容量限制或回滚；原门槛和末NUL照原保持，不把容量早退改固定成功或省略已有分支。

<a id="text-escape-acceptance"></a>
## 限定静态验收

核两完整原根连续字节/实际CALL/11跳两RET、原八BYTE表和当前literal；整Input.cpp/Input.hpp精确重放三逻辑组/六注释，包装及现有便利适配正确正文、UTE01四根/全部其它source保持。112入向仅登记来源，所有完整外父、发送场景文本生产消费/容量及alias-owner寿命、真实截断/字符与消息画面、SDK/机器ABI/数值/优化后读取或并发等价和全生命周期待。

两个新语义/F；INPUT/UIRUNTIME/NETWORK各两成员，三限定S零I；U08/U09/U10/U11/G01/R01/R04/V03/V04九任务继续open。旧语义/处置/证据/政策、全功能枚举/完整gates/六closure及总SPEC§10保持。只必要静态和measure_cg2d.ps1/-Check，不构建/测试/运行客户端或算法/产物/摘要/依赖；关联率不是完成率。

<a id="text-escape-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐文本反转义合同并恢复原位取样”；拟提交说明“[#] 补齐文本转义生产合同并恢复读取顺序”。两完整原根/原表及三组源码修正、登记、静态汇总/Check和派生范围核对已通过。语义2238=2190F+48X，函数处置1675/证据1470；文档关联2300/3680=62.50%仅关联率，INPUT58/UIRUNTIME92/NETWORK85且枚举均未闭。184叶验收0、60任务关闭11/开放49、六closure开放及所有完整门禁保持。未构建/测试/运行，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_text_escape_sampling_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐文本转义生产合同并恢复读取顺序」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x52D1B0 | 0x52D2C6 | 95/278 |
| 0x52D2D0 | 0x52D2F0 | 13/32 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
