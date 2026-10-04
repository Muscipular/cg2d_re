# 文件缓冲输出地址差与数量 SPEC（FBO01）

2026-10-05，按工程委托继续C05真实消费者链。
[stdio合同](cg2d_stdio_contract.md)、[运行库边界](cg2d_crt_runtime_boundary_spec.md)
及[追踪验收体系](cg2d_traceability_acceptance_spec.md)约束本步；
只改既有窄宽写路径的数量算式，不新增facet/owner或活动启动。
固定源码快照：`cg2d-re-file-buffer-output-source-20261005`。

<a id="file-buffer-output-baseline"></a>
## 原端基线与实际差异

[机器证据](cg2d_file_buffer_output_alignment_evidence.json)保原548843与54999A
各124条，合248条/638B，完整独立边界分别[548843,548979)、
[54999A,549AE2)。唯一直接data引用54FFD8/550074原已加载值就是两根；
原转换器vft+14是间接槽，不制造一个固定callee/生产者地址。

| 根 | 转换虚调 | 数量差 | 再取串并Freeze | 字节写 |
|---|---|---|---|---|
| 548843 | 54890D，原vft+14 | 548944取outputNext；548947 SUB当前保存串buffer | 54894A/54894F | 54895E→53C680 |
| 54999A | 549A6C，原vft+14 | 549AAD取outputNext；549AB0 SUB当前保存串buffer | 549AB3/549AB8 | 549AC7→53C680 |

两根在转换返回0，或返回1且inputNext移动后，重取scratch串并Freeze；
再以outputNext整数地址减该保存串的当前buffer地址，形成W32 produced。
随后另取scratch串并Freeze，再取实际FILE和该次buffer，
调用53C680(buffer,1,produced,file)，完整U32返回等produced才成功。
不是用字符串length、申请8B、两个next之差或回调最初buffer代替数量。

当前源码阶段/返回消费已保留，但写成C++ `outputNext - countBytes->buffer`；
该语言算式要求指向同一数组，原SUB是整数地址差。本组不把不同阶段的
借用指针强当同一数组，也不借此声称观察到回调必然替换了某个缓冲。
原生地址只用于数量求差，最终计数仍U32，不能先截两个地址成W32。

另核8个未改依赖根：窄宽Init、close、析构及read，共398条/1107B。
关闭失败保filebuf绑定、析构先取一次scratch再Tidy/释放同指针，
两locale/方法表次序、两state读写及输入转换阶段本步没有差异。
总646唯一条/1745B；这些辅助仅界定保持范围，不新增本组处置/关闭门禁。

<a id="file-buffer-output-decision"></a>
## 具名实施裁决

1. 仅在LegacyFileBuffer.cpp两produced表达式，将两个真实指针各转
   std::uintptr_t，作原生地址差后static_cast到std::uint32_t。
   原地址拓宽，数量仍W32；不用C++数组指针相减，不补符号/范围检查、
   容量上限、负值拒绝或地址先截断，不新增helper/header/type/依赖。
2. 只改变求差表示。countBytes先保存/Freeze，outputNext后读且countBytes
   buffer按当时字段读取；writeBytes独立重新取/Freeze，再取FILE/buffer。
   produced保存一次，53C680参数1/produced和成功等数量比较保持。
   不按单次输出固定1/2/8字节，不重复写、钳制或补重试。
3. 全窄S32 EOF与宽W16 FFFF门禁、快路径游标、FILE空分支、
   facet空直写、erase→Append(8,0)、两次转换前Freeze/首buffer保存/
   旧首串length后读与末指针空选择保持。次序不合并成一份默认临时buffer。
4. 转换result0/1/3/其他分支与实际inputNext/局部值消费保持；
   result3窄先取实际局部低S8再FILE，宽先FILE再实际局部W16；
   返回实际原字符/EOF、短写失败、state副作用与不回滚/不清串保持。
   next/输出局部不预清，不加“产出/进度正确”检查。
5. 两表达式外整CPP、8辅助体及其他src/include、原方法表/adapter、
   字符串字节/对象分配域、真实FILE/locale/state和其他算法不变。
   两原data槽与原数字call保留；未知facet实际生产/完整表或其间接alias不补猜。
   C05、17活动项/全CRT/SDK/实际文件与正常快速退出运行仍未闭。
6. 两既有语义定点补整数差表示，其他1205记录/元数据与政策保持；
   两变化根记adapted，三限定I/W/S证据。只有本组表示与正常调用消费静态，
   不声称原机器发射、ABI或全部文件/转换器等价。

RPLAN.24异常不专项模拟或加安全策略；本组不是溢出/越界加固，
不记x64安全或故障等价。网络原格式明文、音频暂停与单线程保持。

<a id="file-buffer-output-acceptance"></a>
## 本步可度量验收

- 两根248条/638B、两加载data槽、两真实53C680调用与两间接vft+14
  consumer；8辅助原体边界与保持角色可追，原身份不从清单删除。
- 两数量以原生地址整数差取U32，count/write两次独立串取样、
  Freeze/FILE/buffer及等数量返回消费保持；无新增保护或假数量。
- 两限定表达式外整CPP/其他源头保持，不改变任何模式分支/字段/owner。
- 两旧记录外1205语义与全政策保持，C05保持open、三限定证据不提升全功能门禁。
- UTF-8无BOM/LF、引用/Git差异、既有measure生成及独立-Check；
  不构建/查产物、新增或执行测试、运行客户端/算法/故障样例或计算摘要。

<a id="file-buffer-output-source-closure"></a>
## 实施记录

窄宽两输出produced已改两个原生uintptr_t地址相减后取U32，
保countBytes保存/Freeze及当时buffer、另一次writeBytes重取/Freeze，
实际FILE/buffer和53C680的1/produced、等数量返回比较。
两表达式外整CPP、8未改依赖及其他src/include、原分支/字段/owner保持；
无新facet/默认缓冲、范围保护或活动初始化。

原两完整根248条/638B、8辅助398条/1107B共646唯一条/1745B，
两SUB/原vft+14、两真实data槽及53C680写消费均有证。
两旧语义定点更新、1205其他记录及全政策保持，语义仍1207/1204有效原ID；
两新增adapted使审核145/3680，三限定E使证据96。
快照为`cg2d-re-file-buffer-output-source-20261005`。

C05保持open；真实codecvt/facet生产/完整表与间接alias、17活动项及全CRT/
文件/SDK/正常快速退出运行未闭。任务10/60、叶0/182与完整门禁不提升。
不声称已发生callback换buffer，不记机器发射/ABI或故障等价；
未构建/查产物、测试、运行或摘要。编码/引用/差异与measure/-Check见唯一进度。

**交付检查**：11项UTF-8无BOM/LF、682个本地链接、33项限定登记/决议引用及差异格式通过；measure生成与独立-Check各退出0。1327/3680仅为合同文件关联，完整分母未闭合、整体未验收。
