# 文件位置 W32 基线校正 SPEC（FPW01）

> 2026-10-09当前FILE边界：[MC01-B](cg2d_crt_host_migration_spec.md#msvc-crt-file-source-state-20261009)已将全部现FILE/描述符/流格式与状态/位置/关闭改真实MSVC服务，旧表/缓冲/私有入口及C表退出当前链；下文旧内部布局/发布机制只属固定历史。业务字段/字节、真实结果/owner与全部适用门禁保持，未编译/运行；完整C++余组继续。

2026-10-05，继续C10/C03真实文件位置边界。
[stdio合同](cg2d_stdio_contract.md)及[PE初态证据](cg2d_pe_zero_initialization_evidence.json)
约束本组；核对已有实现，纠正含混的字段宽度表述。
固定快照：`cg2d-re-file-position-width-source-20261005`。

<a id="file-position-width-baseline"></a>
## 原基线与具体结论

[机器证据](cg2d_file_position_width_evidence.json)保四完整根174条/434B：
548B68为52条/130B，548BEA为78条/194B，
5498B7与5498EE各22条/55B。四入口有八个原已加载方法槽，
两filebuf共用根不按窄/宽源码实例拆成新库存身份。

| 原根 | 5500E0低W32读 | 5500E4高W32读 | 1087B04状态W32读 | 原身份 |
|---|---|---|---|---|
| 548B68 | 548BBB | 548BC7 | 548BCF | F:00148B68 |
| 548BEA | 548C7D | 548C89 | 548C91 | F:00148BEA |
| 5498B7 | 5498BD | 5498C9 | 5498D1 | F:001498B7 |
| 5498EE | 5498F4 | 549900 | 549908 | F:001498EE |

十二操作均是完整MOV EAX,dword，不是MOVZX WORD或W32读后截U16。
5500E0/E4是64位文件位置的两个W32数值；历史“低半字/高半字”表述含混，
本组明确写低W32/高W32，不能据其将现有类型或数据改成U16。
当前四读源均是uint32_t，实际算法无需修改。

5500E0/E4原加载值均0；1087B04在IDA未加载，不能把FF/未加载读值当初态。
既有PE节头证明整个[1087B04,1087B08)在填零尾[593000,1087B08)内；
当前LegacyStreamBuffer已有独立volatile U32=0真实存储和四消费，
不重复定义、补值或把它当运行时复位/生产已验。
三槽各四个已证直接读取，计算alias不因此排除。

<a id="file-position-width-decision"></a>
## 校正及源码保持裁决

1. 仅将LegacyStreamBuffer.hpp一个存储说明注释改为“两位置W32”；
   stdio合同一处默认入口描述和两个旧语义的含混术语同步纠正。
   函数、类型、声明/定义、宽度、原状态/owner、调用与算法均不改。
2. 548B68保FILE判空→完整0 fseek→重取FILE fgetpos；
   成功写offset0、返回低/高W32、当时state，失败依次取低→offsetFF/低写→高→state。
   548BEA保输入低/offset/高/FILE取样，fsetpos→非零offset才重取FILE fseek(CUR)
   →再重取FILE fgetpos；成功且scratch非空才state恢复/erase，再按原四字段更新。
3. 两默认根独立身份、this/输入位置/模式不消费，先低W32读再offsetFF/低写、
   高W32/状态后读；两默认root不合并为同一个原入口或宿主streampos(-1)。
   四入口均整24B复制/返实际输出地址，+04/+14未写不补值；
   恢复位置root保输入其余对象表示，不预清局部/输出或改变失败FILE副作用。
4. 原八表槽/六实际内部call、volatile读序、三唯一共享槽与现有方法adapter保持。
   源级24B自然填充/by-value/std::memcpy承接仅具名adapted，
   原栈偶然字节、机器返回/ABI、SDK及实际成功失败运行不据静态称一致。
5. 四旧记录定点归档，1203其他记录/全部政策保持；
   四变化记录只代表本轮审核既有实现，不称新增四功能/修改算法。
   三限定I/W/S证据；C10/C03、全FILE/标准流、17活动项及全CRT仍open。
6. 本轮初态和W32事实已经成立，不将缺旧记录误当未实现而改正确源码；
   不因默认槽初值0折常量、加once/保护/新producer或启用旧CRT。

RPLAN.24异常不专项处理/模拟，未写区具体值和故障等价不补猜；
原格式明文、音频暂停与单线程保持。

<a id="file-position-width-acceptance"></a>
## 本步静态门禁

- 四独立完整原体174条/434B、十二dword读取、八加载原槽及三直接读取集合可追。
- 既有四源根的宽度/调用与成功失败/状态及输出写集对应，不改CPP/类型/owner。
- 一个header注释、一个合同术语校正和限定追加/四记录归档外保持，
  三实际共享槽唯一、原PE初值事实不重建或假设运行复位。
- 语义1207/1204有效原ID保持，四adapted/三限定E，完整门禁不提升。
- UTF-8无BOM/LF、引用/Git差异及既有measure/独立-Check；
  不构建/查产物、新增或执行测试、运行或计算摘要。

<a id="file-position-width-source-closure"></a>
## 收口记录

四完整定位根174条/434B、十二完整dword读取与八原加载方法槽
均已同当前源码对应。5500E0/E4明确为64位位置低/高W32，
1087B04既有PE零初态/真实volatile U32存储保持。
一个header说明与合同术语已校正；四原函数CPP、类型/声明/定义、
实际API/状态/复制/返回与owner全部未改，不称新增实现或运行修复。

四既有语义限定归档并纠正两默认条的过时“状态仅声明”记录；
1203其余记录与全政策保持，语义1207/1204有效原ID不变。
四adapted表示本轮审核已有源级承接，使审核149/3680，三限定E使证据99；
固定快照`cg2d-re-file-position-width-source-20261005`。

原24B未写具体字节/机器ABI、SDK/真实API成功失败与完整FILE/标准流/全CRT
及17活动项仍未验，C10/C03与任务10/60、叶0/182/完整门禁不提升。
未构建/查产物、测试、运行或摘要，RPLAN.24异常不专项/记故障等价；
编码/引用/差异及measure/-Check结果同步唯一进度。

**交付检查**：11项UTF-8无BOM/LF、690个本地链接、42项限定登记/决议引用及差异格式通过；measure生成和独立-Check各退出0。1327/3680仅为合同文件关联，完整分母未闭合、整体未验收。
