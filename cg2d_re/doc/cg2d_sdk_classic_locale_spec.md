# 经典 locale 宿主活动提供者 SPEC（SCL01）

2026-10-05，按[D03启动与退出方案](cg2d_crt_startup_lifecycle_spec.md)
和[宿主承接范围](cg2d_crt_host_migration_spec.md)，推进原55315C的活动接线。
固定快照：`cg2d-re-sdk-classic-locale-source-20261005`。

<a id="sdk-classic-locale-baseline"></a>
## 原基线

[证据](cg2d_sdk_classic_locale_evidence.json)保存三完整根9条/33B。
54A3E3六条/22B，先调用54A3ED（单RET，一条/1B），
再在54A3F3登记54A3FA并直接返回完整S32。
54A3FA两条/10B，以1086728为对象地址转54867D析构。
晚段RET不重置已由54A404初始化的经典对象，也不新增引用。

原三个连续已加载槽为553158→549F20、55315C→54A3E3、553160→54A638。
既有AIS01当前投影只接553158和553160。
旧三共享槽1086728/2C/30的已证直接引用分属54A404、54865E、
54A4BD、54A3FA；十三条直接引用仅限定这次观察，不排除计算alias或未归属代码。
旧生产/构析证据复用[LCH01](cg2d_locale_object_heap_migration_spec.md)，本轮不改旧对象。

本机MSVC 14.50.35717的xlocale声明
`static const locale& classic()`，返回标准库持有的经典C locale；
析构为标准库完整locale/实现引用清理。声明本身不证明库内初始化、
退出登记的具体时机、失败数值、线程/ABI或实际运行。

<a id="sdk-classic-locale-decision"></a>
## 具名裁决与开发范围

1. 当前活动标准流已由宿主std::cin/cout等对象提供。本组在同一LegacyMouse.cpp
   翻译单元、553158配对登记之后、553160配对登记之前，
   新增平凡析构模块适配，真实调用`std::locale::classic()`并弃返回引用。
   显式include locale；不复制locale、不保存指针或额外增加应用引用。
2. 完整对象、经典实现及其私有退出由SDK提供者承接；不调用旧54A3E3，
   不登记旧54A3FA，不把SDK对象转成LegacyLocaleImplementation或写旧三个槽。
   原宿主清理时机/内部登记与旧表不要求相同，作为D02/D03具名adapted差异；
   应用四配对退出登记和两个宿主流适配析构保持原文，不能据此跳过应用退出。
3. 新适配不捕获异常、不加once/真假成功或回滚；不新增noexcept。
   原cinit弃初始化返回；本组不制造宿主atexit结果或宣称与原0/−1一致。
   SDK实际异常/失败和生命周期运行未验。
4. 旧Locale/FileBuffer/Ios接口、字段、owner、引用重读和LCH01登记保持；
   旧三槽不是宿主镜像，不假称它们已经初始化。
   以后若接入依赖这些Legacy槽的活动消费者，必须在该消费者小步核定提供者，
   不能仅凭本组使用SDK就调用未构造对象的旧退出。
5. 仅55315C从“待接”转“宿主经典locale提供者”。当前投影变为
   66应用＋2宿主流配置＋1宿主经典locale＝69；
   原null1、音频暂停2、键盘排除2、其余标准流待接16仍显式保留。
   旧90数组不造占位，不执行旧cinit，不宣称完整90或全应用构析闭合。

<a id="sdk-classic-locale-acceptance"></a>
## 本组可度量验收

| 项 | 验收标准 |
|---|---|
| I | 新平凡析构模块适配包含一次真实classic调用、一个实例、显式locale头 |
| W | 553158→55315C→553160源声明序成立，四旧配对实例/登记函数正文保持，无旧经典登记/混用 |
| S | 三原根9条/33B、三个加载槽和旧三槽十三直接引用可追；限定源码重放全文一致 |
| 边界 | 69项源绑定/原槽单调顺序成立；旧68项绑定保持，17待接仅减少经典1项 |
| 尚待 | SDK实际构析/失败、计算alias/R1、其余16流、registry全容量及codecvt、全应用normal/quick/callable退出、R验收 |

I/W/S仅记录本组静态通过，不提升完整功能门禁；禁止构建/测试/运行/摘要的现行门禁保持。
C02/C03/C04/C09仍open，10/60已闭任务和0/182叶完成不变。

<a id="sdk-classic-locale-source-closure"></a>
## 源码小步收口

一次真实SDK classic调用及平凡析构实例已接在553158和553160之间。
一处include和一处适配插入外，LegacyMouse.cpp全文与前提交一致；
其余src/include、四配对登记和旧Locale/FILE/Ios算法未变。
旧68项源绑定逐项保持，新增经典项后69项与原90已加载槽逐项核对，
同TU声明序单调；三完整根9条/33B与既有LCH01原指令一致，
旧三槽十三直接引用、三个连续槽与SDK声明已归档。

三旧语义定点追加活动绑定，1204其他记录及原库存/政策保持；
既有54A3E3 adapted只追加证据，不新建或重复审核其单RET/退出辅助身份。
语义1207/1204有效ID、审核149/3680、18pending/4分类引用保持；
三限定静态证据使99→102。C02/C03/C04/C09继续open，任务10/60、
叶0/182、完整成员/功能门禁及R均不提升。
SDK具体构析/异常与旧三槽消费者的活动归属仍按本合同待验。

**SCL01交付检查**：13项严格UTF-8无BOM/LF、704个本地Markdown链接、30项限定证据/决议引用及Git差异格式通过；measure生成与独立-Check均退出0。任务10/60、叶0/182及完整门禁不提升。
