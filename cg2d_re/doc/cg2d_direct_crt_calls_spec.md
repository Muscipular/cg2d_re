# 直接使用MSVC CRT调用SPEC

<a id="direct-format-calls"></a>
## 格式化与扫描包装退休（DCF01）

2026-10-14用户要求FormatLegacyString_53B304及同类接口直接使用标准库；有界部分已明确选择保持现有截断合同、直接调用::_snprintf。

| 原入口／旧接口 | 当前直接调用 | 实际CPP调用数 |
| --- | --- | --- |
| 0x0053B304／FormatLegacyString_53B304 | std::sprintf | 800 |
| 0x0053D21D／FormatLegacyBoundedString_53D21D | ::_snprintf | 9 |
| 0x0053BC0F／FormatLegacyStream_53BC0F | std::fprintf | 11 |
| 0x0053EB28／FormatLegacyOutput_53EB28 | MSVC std::vfprintf，无项目消费者 | 0 |
| 0x0053DEF4／ScanLegacyInput_53DEF4 | MSVC std::vfscanf，无项目消费者 | 0 |
| 0x0053CADC／ScanLegacyString_53CADC | std::sscanf | 21 |

只替换841处callee；原缓冲地址、格式串字节、容量、变参和返回值消费逐字保持，不增加格式校验、预清、容量推断或参数转换。九处容量实参只为28／100／256，直接::_snprintf保原MSVC负返回／满长不另补NUL的合同，不改为std::snprintf。
退休六个声明／定义，删除只有两扫描包装的LegacyScan.cpp及单一CMake源项；LegacyFormat保留店铺命令W32取样intrinsic和普通U64除法，不扩展两者的业务／异常合同。
消费者显式引入cstdio；原六个地址引用注释放在实际CRT引用旁，无当前消费者的vfprintf／vfscanf地址留在现有依赖头文件的CRT导入说明，不能冒充新增业务调用。
旧CPP包装／源码接口及其指向删除文件的绑定存具名历史，当前表与语义指向真实标准库／MSVC直接提供者及消费者；原入口身份、原端事实、阶段历史和完整门禁保持，不重新取证或新增已验收结论。
本步只迁移格式化／扫描；字符串和时间按下一单项推进，保持32位时间与真实字段／出参适配，不泛改文件、网络、堆或业务算法。
按第8条工程委托先记录范围，再实施必要静态核对与文本汇总；不编译、测试、运行、查产物、计算摘要、引依赖或委派，按同一cg2d_re分支精确提交，不打tag。

### DCF01源码与静态结果

34个消费者的841处callee已迁移：sprintf800／_snprintf9／fprintf11／sscanf21；两无消费者va_list包装退休，六个旧接口在src／include无残留。
逐文件静态核对仅callee、cstdio导入和原地址引用注释变化，原格式串、缓冲／容量／变参、返回消费及其它源码逐字保持。九容量仅28／100／256，项目未开启标准名安全模板重载，本机UCRT默认0；有界仍沿用户选择的::_snprintf合同。
LegacyScan.cpp和CMake一行源项退休，LegacyFormat保留原店铺W32取样与U64除法正文；本机cstdio真实导入sprintf／sscanf／vfprintf／vfscanf，不把无调用库提供者记成新增业务。
197条既有语义和306个具名历史绑定同步，扫描内存填充旧证据完整转历史；26条既有证据、六处置、CRT功能域及C04／U04／U11开放引用同步。
当前全量表更新直接调用／CRT导入和受影响源行号，3680入口身份／状态、所有原端事实、阶段历史和完整门禁保持，无活动绑定指向删除扫描单元。
UTF-8无BOM／LF、841调用／九容量、必要适配、SDK导入、历史／门禁、原身份及Git差异静态核对通过；文本汇总器及最终-Check通过3680函数／218导入／3061区间／184叶功能／60任务。
仅静态，未编译、测试或运行，不把调用改写计为完整格式／扫描流程或互通通过；精确提交本步并保留其它用户修改，不打tag。
