# 共享关闭、前置及释放返回 SPEC（WCL01）

2026-10-11。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。沿子AGENTS第8条工程委托裁决，同一cg2d_re分支，不打tag。

<a id="window-close-baseline"></a>
## 原端合同

WCL01仅共享关闭按钮4C1FB0完整53条163B/一页、release/存档共用返回叶4A4210完整两条6B/一页、窗口前置元素474170完整15条43B/一页；合70条212B/三页、一个E8目标52C450、六跳四RET。三业务正文正确保持，仅三个定义/三声明及既有release适配七注释。关闭按钮先当前窗口/目标graphic持址，hover提示沿已批静态文字迁GBK现源保持，bit2置同窗口bit15→SE57→result1，之后仍向held graphic按默认3B538→hover3B53A→pressed3B539发布图号，返回0/1；不立即free、不业务callback/取消发送、不跨SE换目标。4A4210原端仅MOV EAX,1/RET，所有原参数不消费，既有void ReleaseWindow适配只弃真实返回；不能把这个既有原端常量叶说成缺失资源实现，也不能把返回1计全释放成功。存档父5201B0唯一CALL后TEST EAX/JZ局部关系，不代完整存档父/机器ABI。前置474170 bit2置当前window bit13，bit1优先返回1，否则原state低BYTE bit5返回0/1；图层前置不等于立即关闭。两按钮持址都仅data引用：关闭29个data引用项/29匹配槽，前置26项/28匹配槽；返回叶34项/36匹配槽；除窗口42两个元素+28和原型release+2C有已证布局，其余只记录指针对齐匹配，不外推全部slot消费/owner。窗口42真实元素1关闭/7前置、prototype release指向既有适配保持。实际关闭回收复用完整WCR01整理4730A0/BVR01释放471270/WRS01更新473F80：下一整理读bit15调用真实FreeWindow，release先于fresh id/state坐标/flags/焦点清理与七类元素释放；本步对应三提供者及四已有记录全文保持、不重复原根/身份验收。三新F三adapted，INPUT两/UI三/RENDER两成员，三限定S零I九open；仍待全调度/全部上游和回调消费、键盘取消路径、窗口/输入/Yobi owner完整寿命、生成取样/机器ABI与资源SDK及画面声效运行。所有完整门禁和WAV-only/MSVC/明文协议保持；无新API/helper/字段/owner/保护/补零/钳制/回滚/假成功体，整体未完成。

完整原证见[证据](cg2d_window_close_front_release_spec.md#evidence-archive-20261011)。实际回收已有[WCR01](cg2d_window_compact_close_spec.md)与[BVR01](cg2d_window_release_source_sampling_spec.md)，调度复用[WRS01](cg2d_window_routing_sampling_spec.md)，不重新读取已有完整根或外推它们全部消费者通过。

<a id="window-close-decision"></a>
## 方案及取舍

三现有业务正文均保，三定义/三声明及void release适配补七注释。明确关闭标志/前置标志与真正FreeWindow回收各自责任，保共享非拥有window/graphic跨声效引用。返回叶是原端真实常量1，不增加假资源释放/业务取消或计作owner验收，不推所有存档/窗口回调ABI；已证窗口42绑定及其他data槽引用定位分列。

<a id="window-close-acceptance"></a>
## 本步验收

三个完整根70条212B/三页、一个E8/六跳四RET；一入CALL局部后邻与29/34/26 data项和29/36/28指针对齐匹配证据。仅数量42明确元素1/7及prototype release槽，其他匹配不猜布局或动态调用。三整源精确回放/只七注释，三正确正文、两类型、三释放/调度提供者、四已登记根及窗口42原型表保持。三新F/三adapted、三限定S零I九open，静态汇总/-Check须实际终态0；未构建/测试/运行/产物/摘要。

<a id="window-close-source-state"></a>
## 当前状态

补齐共享关闭按钮、原端返回叶和前置元素三完整原根70条212B/三页，三正确正文保持，仅七合同注释。窗口42关闭只置bit15/声57，前置只置bit13；真实资源回收复用窗口整理→FreeWindow，release返回叶原端只返回1，不能记作完整释放成功。三新F三adapted，INPUT两/UI三/RENDER两成员，三限定S零I九open；完整取消/共享调度/owner、ABI及运行仍待。

语义预期2322=2274F关联+48有界代码，处置1778/证据1574；文档关联2384/3680=64.78%仅关联率。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_window_close_front_release_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐窗口关闭前置与释放返回合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4C1FB0 | 0x4C2053 | 53/163 |
| 0x4A4210 | 0x4A4216 | 2/6 |
| 0x474170 | 0x47419B | 15/43 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
