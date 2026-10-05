# 数组析构剩余计数 SPEC（ACI01）

2026-10-05，按RPLAN.24和既有工程委托对齐本步已发现的算术残留。
上位：[CRT生命周期](cg2d_crt_startup_lifecycle_spec.md)及[具名决议](cg2d_closure_decisions_spec.md)。
固定快照：`cg2d-re-array-counter-source-20261005`。

<a id="array-counter-baseline"></a>
## 原基线与缺口

[原证据](cg2d_array_counter_evidence.json)归档两已有根：
53C4E0析构42条/128B、53C560展开35条/94B，共77条/222B。
原数量参数为S32，步长U32；53C517/53C589递减数量后以js判停，
随后减当前地址并调用元素析构。
析构finally在未完成时传当前地址与已递减剩余数到展开，失败元素不重试。
三个直接析构父及两个展开入向沿既有三组实际接线，不新增caller。

当前两循环将S32数量bit_cast成U32，递减后再bit_cast回S32判负，
finally也转回S32。这为数量回绕提供专项表达，须沿最新异常裁决撤去。
本步只核这四类源码片段及必要原点，不全面复检已过成员/构造/provider。

<a id="array-counter-decision"></a>
## 实施裁决

1. 两个remaining使用std::int32_t直接接elementCount，保持--remaining先于remaining<0。
2. 析构finally直接传remaining，保当前地址/步长/回调及complete门禁。
3. endOffset仍为U32，表达式简化为elementBytes * elementCount；
   原U32步长与S32数量按普通语言数值转换计算W32结果，不扩大为size_t或地址宽度。
   这是既有尺寸/地址偏移数据规则，数量循环不再借U32与bit_cast模拟S32回绕。
4. 原逆序/失败前缀、__try/__finally/__except及5405F5筛选保持，
   构造器/三组父调用、owner/成员RNG和其它源码不改。不是把SEH改成catch或宿主新terminate。
5. 不新增数量保护、拒绝/钳制、溢出模拟、补零或故障门禁；
   数学溢出/越界等异常沿RPLAN.24不专项处理，不声明x64安全或故障等价。
   原计数0的正常判停与非负数量的逆序过程继续保留。

<a id="array-counter-acceptance"></a>
## 限定验收

- 一CPP四类片段/六处文本调整，两S32计数、两负判定及一finally直接传递。
- 原W32 endOffset、先递减数量/地址后析构、complete门禁与失败剩余前缀保持。
- 两完整原根77条/222B、三析构父/两展开入向、本步源码与台账/新引用静态可追。
- 两既有语义补裁决；R1登记adapted为源码/批准差异处置，不作完整EH/ABI/运行验收。
- 当前三组元素析构正常只写token，任意回调异常/SDK传播、成员与数组EH/硬件SEH、
  完整normal/quick/callable退出及运行仍待；C04/C06/C07/V02保持open。
- 禁构建/测试/运行/摘要；完整功能门禁、关闭10/60、叶0/182不提升。

<a id="array-counter-source-closure"></a>
## 源码小步收口

两数组清理循环撤去数量专项回绕，保持原逆序及失败前缀传递；
步长/尺寸偏移仍W32，原构造/筛选/业务三组父及其余源码保持。
两既有记录补局部合同，其余1224语义记录与政策保持；
语义1226/1223有效原ID不增，两个adapted使处置195→197/3680，
限定E153→156，15/218导入不变；四任务保持open，不提升完整门禁。

限定静态检查通过：一CPP四类片段/六处，原构造/筛选/全部父与失败前缀保持；
两语义/两个adapted及台账精确转换、11项UTF-8无BOM/LF、10个新增Markdown链接、
27个本组E引用及Git差异格式通过。看板生成和独立Check通过，不扫描历史Markdown，不提升完整门禁。
