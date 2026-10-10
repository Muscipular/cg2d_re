# 存放开窗动作与完成回调 SPEC（SOA01）

<a id="storage-opening-baseline"></a>
## 原端证据与范围

2026-10-11，已打开cg2d.exe只读会话。两creator/两callback完整共416条1208B/八页、24E8九唯一目标/12跳八RET。五CALL均来自SRD01完整回复父，两callback各一个动作发布原点；本族失败同步开窗/返回空与完成callback开窗/标死有完整原证。当前manager/实际callback执行和所有owner正常寿命另待，不据创建指针或callback字段代运行验收。

SOA01四完整原根：4ABEF0(154条462B/三页)、4ACE80(158条474B/三页)、4ABE60(51条134B/一页)、4ACDF0(53条138B/一页)，共416条1208B/八页、24E8九唯一目标/12跳八RET。两creator原五CALL均同49D2E0（41三点/38两点），callback各仅一个creator MOV offset发布；这里只证明本族及来源，不据单引用宣不可变或完整动态调度。四新F四adapted，UI-YOBI各四成员，两限定S零I八open。原动作priority3/payload size24H，现sizeof(WindowOpeningPayload)承接已存在纯标量36B struct；其id/counter W16，width/height/centerX-centerY/halfWidth-halfHeight/value1C S32、flag20 W16及bytes22未写域保，本四根value1C仅清S32零，不当完成槽指针或拓宽到uintptr_t，不据本族冻结所有其它族用途。NewAction/MarkDeath当前真实manager及通用MSVC分配族复用，不新增影子owner或接口。
两creator先声56/X320（原三参中的Y240仍沿已核音频接口）→真实NewYobiAction(priority3,size36)，失败原各三/四S32参数直接Sub4A83A0(kind,value14,userState)或4AC0C0(kind,value14,userState,phase)，弃开窗callee结果，返回空action，故SRD01检查created空后仍fresh window的原因有完整本族原证；不将同步fallback成功伪成action成功指针。成功后一次prototype41/38指针，savedPosition只读原flags低BYTE；原先defaultY→width→defaultX→height S32取样，命中保存位才原x→y覆盖，不移窗口保存flag到helper或合宽字段。action/payload非拥有原指针保，action flags W32 OR6→callback真实指针→displayPriority BYTE DF，payload value1C S32零→flag20 W16零，入flags只低BYTE bit1命中写flag20=1；action x/y S32、payload width/height S32及kind低W16按原发布，38另phase S32在首CryptInt构造前发布。
四临时CryptInt原encoded在+4、mask在+8、readCount BYTE+C，不交换语义字段。先Construct(value14)/RNG，再一次取全三临时字段后按原顺序发布mapX，Destroy；再Construct(userState)/RNG，取全三字段后按原序发布mapY，Destroy。41的X读/写序count→encoded→mask，Y序mask→encoded→count；38的X序encoded→mask→count，Y序count→encoded→mask。原source三读先于任何target三写，现恢复四组先取临时后发布，写序本来正确不交换；vftToken/reserved不复制或补零，不改变构造/析构或随机次数。counter末清W16零，mode仅低BYTE：0原宽/高除2加pos发布centerX-Y→action deltaX0→fresh centerY算deltaY/10→fresh centerX算halfWidth；1原centerX新算值持寄存器，发布centerX-Y后用持值算deltaX/10，再fresh centerY算deltaY/10，本步恢复持值不重读centerX；原不在mode1写halfWidth、其它mode不写centers/deltas及本族不写halfHeight，保既有真实allocator的清零，不在creator补默认或新钳制。两个返回真实新action指针/失败空，不改callee/源协议参数。
两callback正确body全部保，只注释：先held payload，S32 halfHeight→centerY→halfWidth→centerX，现Hover传低W16矩形/层D4/image0/step0，图后fresh deltaX→previousHalfWidth→previousHalfHeight，发布halfWidth增量，再fresh deltaY，先按counter低S16>=10比较，再halfHeight发布，原完成与末递增不同阶段不能互换。完成41先Read(mapY)userState再Read(mapX)value14，再kind低S16→4A83A0；38先持phase S32，之后同Y→X→kind→4AC0C0。开窗副作用后MarkDeath，再末counter W16++，不立即free/重复开窗守卫或按指针变化重取payload；原callback残留EAX随阶段不同，现void只保副作用，全部machine callback ABI待。两callback并不消费本族value1C/flag20，不为其添加完成槽、保护或私有状态。原dispatcher是否每帧及全manager释放/初始队列owner待完整闭环，本族callback发布与实际执行/回收验收分别记录。
两creator合八组八处：两个saved BYTE、四Crypt先取后发布、两个mode1持centerX；四定义/两既有声明六注释，无公头decl的两个callback不新增接口。两整源精确回放/两正确callback、五类型/九真实provider/一个SRD01父/两个prototype行及五旧语义保持，结构字段宽度及原正常失败/读取发布保持；不据本族重验所有provider或关真实调度/owner/SDK/生成读取及完整ABI/画面运行。MSVC/WAV-only/原命令字段LF明文与所有完整门禁保，不引新helper/类型/字段/API/owner、空/容量/index保护、补零/新默认/钳制/回滚或异常模型，整体未完成。

<a id="storage-opening-decision"></a>
## 方案及取舍

按子AGENTS第8条委托先记方案，沿本族既有纯标量WindowOpeningPayload/非拥有action指针。两savedPosition改原BYTE、四临时CryptInt改先取全三字段再按原写序发布、mode1中心X持原新算值；八组八处。两callback正确正文全保，不新增完成槽或重置未写字段。构造/析构及RNG消费、低BYTE模式/旗标、同步fallback但空指针返回保持，原各宽度及异常域按RPLAN.24不专项处理。

<a id="storage-opening-acceptance"></a>
## 本步验收

四根连续覆盖、E8位移/两BYTE/四临时读写及模式/callback实际发布，完整416条/24E8九目标/12跳八RET；两整源精确回放只八组八处/六注释，正确callback/原未写字段与两个Crypt构造销毁次数保。五类型/九provider/一个SRD01父/两prototype行及五旧语义保持，原0x24标量payload承接不新增布局。四新F/四处置、两功能各四成员/两限定S零I八open；派生六metric/四F/两功能/八任务及汇总/-Check终态0，其它和完整manager-owner/SDK/ABI/运行门禁保持。未构建/测试/运行/产物/摘要。

<a id="storage-opening-source-state"></a>
## 当前状态

补齐窗口41/38两个creator及两个完成callback，四完整原根416条1208B/八页、24E8九目标/12跳八RET。恢复两处saved flags BYTE、四CryptInt先取后发布及两个mode1中心X持值，八组八处/六注释；两正确callback、失败同步开窗但仍返空、原S16完成/W16末递增保持。四新F/处置、两限定S零I八open，完整manager/owner/ABI与画面运行待。

语义2337=2289F关联+48有界代码，关联2399/3680=65.19%仅关联率；处置1801/证据1605，UIRUNTIME211/YOBI115，其它保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_storage_opening_actions_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 对齐存放开窗动作临时值与完成合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4ABEF0 | 0x4AC0BE | 154/462 |
| 0x4ACE80 | 0x4AD05A | 158/474 |
| 0x4ABE60 | 0x4ABEE6 | 51/134 |
| 0x4ACDF0 | 0x4ACE7A | 53/138 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
