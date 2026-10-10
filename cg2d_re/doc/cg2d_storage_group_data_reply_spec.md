# 存放记录与组金额回复 SPEC（SGM01）

2026-10-11。方案先记录，源码对齐/登记及静态汇总/-Check/派生核对待执行。沿子AGENTS第8条工程委托裁决，同一cg2d_re分支，不打tag。

<a id="storage-group-baseline"></a>
## 原端合同

SGM01仅外部存放数据解析4A84A0完整1002条2900B/十六页、86E8四目标、39跳（含一个switch间接跳）四RET。首1024B未清buffer Extract字段1/1023失败返0；Unescape后strlen必须1，原signedBYTE到A..P双表分派，A/G/I/P四业务分支其余0，16B索引表/五W32目标表和原ASCII???保，不新增业务命令或按UTF8重解码头。A原清第一组20*C44物品/5*600宠物，不清两组金额/未触区域，不把current side替代首组固定目标；20物品按field2+6*index/六字段，imageNoS32<=0跳该记录，成功usedW16=1→image+4/count+8/valueC40S32→name提取失败保已发布字段/继续下一记录，成功Unescape、strlen>28用???否则原bytes名；memo提取失败跳，成功非空才写C29；description提取在A必须成功才Unescape→4E02B0(index,0,buffer)，原固定描述32行/页数提供者保持，不新增回滚或清空已写字段。5宠物从field122+35*index：name提取失败跳，Unescape名>16用???/目标5D4，level+8先写，S32<0跳；成功usedW16/参数+2 W16→九S32 firstOffsets→描述5E5提取结果原不检查/Unescape-copy→十二S32 lastOffsets→十技能每0x8C块从+62写原文本/>24用???。名字/描述失败继续后续固定记录，不补默认/额外字段；A末Parse字段297 S32 amount后才fresh D67618 S32 side，在同CC4D58+side*FBF8+FBF4写金额，返回完整S32 1。G仅Parse字段2→fresh side→同金额地址写S32返0。I字段2起index S32，非负才清对应index*C44记录并六字段读，name/memo仍原失败跳该记录，但description提取原不检查结果仍Unescape/描述提供者；每轮无条件field+=7/new index再判，不按helper早返改变字段基准或添20上界。P每非负index清600记录→同宠物读，每轮field+=36/new index再判，不添5上界；其它分支/失败返0，原回滚/默认字节或范围保护不新增。现完整业务正文正确全部保持，仅定义/声明两注释；old1970 CAC18表关联历史全保，只完整原根/字段/返回合同追加，不重复F。六入CALL来自49D2E0同一父，每点后TEST完整EAX结果/四有限后邻有据，现父合并若干case成四源码调用点保持；不据本根记完整父/source合并等价、数据所有其它消费者或接线全部验收。两个全局实际2*FBF8 item字节区/5*600 pet区、现WriteTradeValue模板和解析/解码/描述五提供者、两既有声明、两当前消费源与五旧语义保持，金额存取SDM01仍借同side同FBF4，不复制新owner。一个adapted、NETWORK/UI各一成员、两限定S零I八open；全部物品/宠物/金额生产与格式显示/其余回复消费、description/input/shared owner-alias寿命、SDK/生成读取与机器ABI、原完整协议互通/画面与操作运行待。原各级field门槛和容量/长度正常合同保持，不引字段/type/helper/API/owner、索引/容量/空保护、补零/默认/新钳制/失败回滚或异常模型；MSVC/WAV-only/原命令字段LF明文及全部完整门禁保，整体未完成。

完整原指令、六CALL/后邻和双switch表见[证据](cg2d_storage_group_data_reply_spec.md#evidence-archive-20261011)。[SDM01](cg2d_storage_amount_exchange_spec.md)同组金额消费复用；描述提供者/单一字节区来源保持，不把源已接、引用数量或表项当完整owner/所有父的验收。

<a id="storage-group-decision"></a>
## 方案及取舍

现有readItem/readPet与四分支正常和局部失败合同正确，完整正文保持，只两注释及旧1970合同追加。明确原A与I的description失败差别、P/A名字与level门槛和字段续基准，不补默认或回滚部分记录；A全量只清指定记录域，金额写在解析后当前组，G不清记录。现memcpy标量和MSVC正常strlen/strcpy/memset承接原字节/宽度，不重建CRT算法或冻结所有未证字段。

<a id="storage-group-acceptance"></a>
## 本步验收

完整1002条2900B/十六页、86E8四目标/39跳四RET及五W32目标/16BYTE索引双表、原???；六CALL同49D2E0及每点TEST EAX/四后邻只有局部来源。两整源精确回放/只两注释、正确正文，五提供者/两消费源/两声明/两唯一全局及五旧语义保持；old1970仅合同追加/一个adapted、两限定S零I八open。派生只有一metric/旧F/两功能/八任务，汇总/-Check必须终态0，其余度量及全部完整门禁保。未构建/测试/运行/产物/摘要。

<a id="storage-group-source-state"></a>
## 当前状态

旧1970历史追加存放A/G/I/P完整解析根1002条2900B/十六页，86E8四目标/39跳四RET及双switch表有据。正确正文保持，仅两合同注释；原物品/宠物部分失败续行、I/P字段步进和A/G金额解析后fresh组写入明确。同组金额与存取界面共享原字节区，一个adapted、两限定S零I八open；六CALL同父仅局部消费，完整父/全部数据owner、ABI及互通运行待。

语义2329=2281F关联+48有界代码和关联2391/3680=64.97%只关联率保持，处置1788/证据1592。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

## 2026-10-11 SRD01：上层原父合同追加

[SRD01](cg2d_storage_reply_dispatch_spec.md#storage-reply-source-state)旧1964历史追加49D2E0完整回复父515条1238B/九页，52E8三十六目标/38跳含三间接分派/30RET及五原表有据。正文正确保，仅两合同注释；三阶段分派、六原存放CALL与四现合并CALL、fresh窗口/creator/global重入及Y-X RNG顺序完整静态核对。BZ当前两bridge/路由保持，一个处置、两限定S零I八open，全部callee/动态owner/机器ABI及互通画面运行待。

SGM01当时六CALL仅局部来源的历史保持；本轮已核完整父/五表与当前正常合并分支，仍不据此验全部callee/owner或网络互通运行。原六点至当前四点的正常type/mode与fresh判断次序见SRD01，不补新parser或改本SGM01正文。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_storage_group_data_reply_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐存放记录与组金额回复合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4A84A0 | 0x4A8FF4 | 1002/2900 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
