# 战斗位置缓存映像初值与HP临时复制对齐 SPEC（BCVI01）

2026-10-10；限定已有动作114/F:0000F0A0的共享位置缓存和一负HP归零片段。原完整动作/入向/正常源沿[BPR01](cg2d_battle_action_114_spec.md)，本轮不重复取证整根、不新增函数/身份/成员或扩容量。

<a id="battle-cache-initial-baseline"></a>
## 初值证据与读取边界

当前模型为std::array<int32_t,1025>，对应59E9D8–59F9DC不含尾的4100B；1025仅保现承载容量，**不据相邻变量推原数组容量或owner**。当前原IDA逐BYTE检查4100B均未加载/无值，首末无文件映射；旧“全1025项-1”和摘要来自未加载FF，不能证明原数据初值。原错误全文保在前态Git命名快照，本轮撤销该初值/摘要说法。

只读当前库保存的 .data节头40B（未重开原文件）：原base400000h，RVA153000h/VirtualSizeB34B08h/SizeOfRawData40000h；文件承载到593000h、虚拟尾1087B08h。59E9D8–59F9DC全部位于超出文件原始数据的虚拟尾部，按[Microsoft PE零填充规则](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#section-table-section-headers)，当前1025个S32承载槽的**映像静态初值为0**。不是把未知运行值补0，不改原输入/哨兵/位置门禁，也不加运行时清理/重置。

五个基址直接xref：40F0C5 W32缓存写、40F0CE W32缓存读属于114；40637D/438F4D/4395B1仅为22对象指针表的尾比较，不能当缓存写者或证明原缓存容量。114原currentIndex/position>0才存位置，否则从同index缓存回写，包括position0；随后重新读当前record/target。正文缓存读写逻辑保持，只把全局lambda fill(-1)改聚合值初始化零；S32位置编号仍非T*。

附近九个本来声明-1的S32（58B6D4/D8/DC、55DEA4/A8、581800、553F8C/90/84）逐4B有文件映射/加载值，确为FFFFFFFF，现声明与所有消费/重置保持。未加载FF与真正文件初值必须分列，不将所有负初值统一清零。已有59F9DC游标零初值沿BCPY01保持不重复修改。

## 临时HP零复制

复用BPR01固定的489条1745B/七页完整原材料，当前动作正文与其source_body_after一致。本轮只取已有40F482–40F4C2窗口：Construct临时零之后，40F48D取当前C9C、40F493读取temp.encoded，之后position/全局target查找；40F4A5发布encoded，再fresh mask读取/发布，再fresh BYTE readCount读取/发布，最后Destroy。现共享CopyPayload本身写序正确，但这一caller先查目标再读encoded；本步只展开该调用段恢复顺序，保target token/其它字节。

回复上限的CopyPayload(maxHp)正确保持；共享复制器、缓存门禁/全部其它动作正文、W8消费/高位/原名称与pair表/返回/几何/创建/声音/RNG/原正常负值归零不改。无新保护/默认值、输入限制或循环cap，不模拟异常/故障、不恢复MIDI。

<a id="battle-cache-initial-decision"></a>
## 实施与登记

依第8条工程委托先SPEC后落实：一个CPP仅当前数组定义/一HP复制片段及两注释、公头一注释；旧shared_battle仅纠正缓存初值/摘要说法，114子SPEC补最新局部状态。旧证据JSON保持历史，不把旧source_body_after改成当前；已有1428语义及原F处置补当前初值/复制读序来源、完整门禁仍待，不新增语义/F或成员。

一限定I/S均只已有F:0000F0A0的缓存静态初值/一HP零片段；U08/U09/R03/R04/V04补局部来源且open，不改R03区间数量/剩余用途，不新增数据X或降低未知容量/owner条件。原分母/全部政策/gates/六closure保持；MSVC/WAV-only/原格式明文/RPLAN.24与七外部改动保留、排序待此前选择保持。不编译/测试/运行/算法/产物/摘要或引依赖，总SPEC§10不降。

<a id="battle-cache-initial-acceptance"></a>
## 必要静态与剩余

核40B原节头/逐字段、4100B完整足迹在原零尾部、全范围未加载/首末无文件映射，五xref实际指令/三尾比较与两数组读写；当前容量1025/类型S32保、聚合零初值、九个文件真实-1保持。核已有原HP窗口C9C/encoded查目标前与逐项encoded-mask-BYTEcount读写，源整CPP仅两片段/两注释、公头一注释，正确maxHp Copy/共享源/其它正文/旧证据/全部旧记录（仅1428和对应F补当前来源）保持。

核零新身份/成员，限定I/S范围、五open任务、全部原政策/gates/UTF8无BOM/LF/新SPEC单末LF、差异和文本汇总/-Check。只证明当前模型覆盖区的映像静态初值，不证明原数组1025容量、输入域、所有运行初始化/间接批量写者/跨回合/重入退出/alias/owner、SDK容量失败和声音画面运行；完整动作114其它父/全部callee/数值与机器门禁仍待，总SPEC§10保持。

<a id="battle-cache-initial-source-state"></a>
## 当前状态

原PE节头/4100B未加载范围与九文件真实-1分列有据，当前数组聚合静态零初值及一HP零读序落实；必要静态、文本汇总/-Check及精确派生范围通过。语义2158（2110F＋48X）、合同2220/3680=60.33%仅关联率、F1586、BATTLE398枚举false保持，E1399→1401仅已有114限定I/S；零新身份/成员，五任务open。主metrics不变，派生只已有114/一功能/五任务，其余原分母/政策/gates保持。原容量/运行初始化/间接写者/owner及运行未验；未编译/测试/运行/算法/产物/摘要。前态cg2d-re-battle-cache-initial-history-20261010，提交引用cg2d-re-battle-cache-initial-20261010。整体SPEC未完成。
