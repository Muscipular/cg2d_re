# 标准流宿主提供者与兼容 Init 类型 SPEC（SSO01）

> CAB07（2026-10-09）：[旧音频两构造](cg2d_display_settings.md#audio-settings-pages-source-state)已按原槽序接入，当前72源投影＋15 SDK＝87对应，另null1/键盘排除2，90槽身份不变。旧70/85及音频暂停叙述为历史；本次未编译或运行，不关闭完整启动/退出、SDK及所有owner门禁。

2026-10-05按[D03](cg2d_crt_startup_lifecycle_spec.md)推进剩余15个标准流表槽。
固定快照：`cg2d-re-sdk-standard-stream-owners-source-20261005`。

> CAA01（2026-10-08）仅迁移旧53C8CD/53DDB5及私有筛选正文到 `src/platform/LegacyProcessEntry.cpp` 独立归档成员；活动退出及四OS槽仍在LegacyRuntimeExit。真实客户端由宿主CRT进入WinMain，实际链接未抽取旧入口成员；原55300090槽表仍未启用，未补零表/重复宿主构造。[当前链接合同](cg2d_client_link_sdl_audio_spec.md)，完整CRT生命周期门禁保持。

<a id="sdk-standard-owners-baseline"></a>
## 原基线

[证据](cg2d_sdk_standard_stream_owners_evidence.json)保存15个完整登记根90条/330B；
复用SSX01的30个固定构造/退出前缀105条/397B。
十四实际流/缓冲起点有74条直接引用；窗口[1086310,10866F8)共1000B逐地址检索
得到80条直接引用，另含四条晚段Init取址与两条日志tie写入。
检索窗不是任何对象的完整尺寸证明，不排除计算alias、未知虚调/区间外消费者。

548424/5494E7是MOV，把EDI输出流地址写入日志tie；对应之前重新读取vbtable位移。
5483FB/5494BB才是错误流format OR 2。
不能将两日志写点判为清unitbuf，本组不新增错误的unsetf调用。

[SI01](cg2d_standard_stream_init_identity_spec.md)和
[SCG01](cg2d_standard_stream_controller_gates_spec.md)证明：
四独立Init地址只保存/返回，不读业务字段；
两个控制器311条及四固定前缀8条，共319条/993B约束本组有型接口。
原Init完整尺寸未知继续保留。

本机MSVC STL 14.50.35717的cin/cout/cerr/clog和四宽流实现分别定义
实际标准流、stdin/out/err FILE缓冲和初始化提供者；
完整流/缓冲的自然析构及私有登记由SDK管理。
SDK原log各有独立stderr缓冲，当前应用适配把log.rdbuf借用error.rdbuf，
并按原tie/unitbuf及pubsync顺序处理应用侧配置/退出。

<a id="sdk-standard-owners-decision"></a>
## 具名开发与提供者裁决

1. 将四个ConstructLegacy…StandardInitializer与两个控制器的返回类型，
   以及控制器initializer参数，收敛为已完整定义的LegacyStandardStreamInitializer*。
   Header前置声明该既有类型；不改四对象/布局、this身份、原保存返回，
   不改函数正文、计数/门禁、逐对象构造、EH/失败消费或即时登记。
   异质constructed/currentObject展开地址仍按既有void*边界，不扩大重构。
2. 剩余十四流/缓冲表槽使用当前SDK实际完整对象/对应rdbuf：
   窄宽cin/input、cout/output、cerr/error及clog/log。
   不为每槽造查询包装、重复SDK构造、Legacy槽镜像或额外退出登记。
   旧兼容函数/14存储、FILE/registry/locale/codecvt全部保留；
   今后若接活动Legacy消费者必须在该小步核定真实生产与引用，不能直接把SDK对象交给旧接口。
3. 原553138宽Init的私有控制职责由SDK四宽标准流完整对象生命周期承担，
   应用早段WideStandardStreamsStaticInitializer真实配置与原pubsync退出保持；
   不直接引用隐藏_Winit，不改_CRTBLD、不复制声明，也不把窄Init冒充宽控制器。
   原隐藏类型不可调用不等于必须重造其私有计数。
4. 原晚段登记/私有vptr/registry/引用表、SDK独立log缓冲及构析/异常/失败/时点
   差异按D02/D03 adapted；应用可见tie、error unitbuf、log/error缓冲共享及原无sentry刷新保留。
   SDK实际链接/ABI、完整异常/normal/quick/callable退出和运行未验。
5. 源级有序投影仍70，另15项由完整SDK预先提供，合85项提供者对应；
   不能把85写成85个同TU初始化调用。
   原90槽为70源投影＋15 SDK提供者＋null1＋音频暂停2＋键盘排除2。
   标准流表槽不再因“缺少重复旧CRT调用”列待接，但完整消费者/alias/SDK运行仍未闭。
   原回调数组/cinit不启用，音频不排除。

<a id="sdk-standard-owners-acceptance"></a>
## 可度量标准

| 项 | 本组验收 |
|---|---|
| I | 六真实有型接口声明/定义一致，四固定caller取得原独立对象，函数正文保持 |
| W | SDK八完整流与六共享活动缓冲提供者可追；log/error借用、tie/unitbuf和原pubsync应用退出保持；旧70绑定不变 |
| S | 15原根/30前缀、319条身份边界、74/80直接引用和SDK八生产源归档；两文件限定差异全文重放 |
| 对应 | 70有序源投影与15宿主预先提供单列，90槽分类守恒，不用虚假活动调用消缺口 |
| 尚待 | Legacy真实消费者/registry完整容量/较大索引、facet/codecvt/alias/R1、SDK实际构析/失败与全应用退出、R |

C02/C03/C04/C09继续open，任务10/60、叶0/182及完整门禁不提升。
本步不构建/测试/运行/摘要，不将静态提供者关联当完整生命周期验收。

<a id="sdk-standard-owners-source-closure"></a>
## 源码小步收口

LegacyIos.hpp/cpp六接口的十二返回声明/定义、四参数位置已类型化，
Header前置声明既有Init类型；两文件扣除这十六个指针位置/一声明后全文同前提交。
函数正文/四对象、计数/门禁/EH/失败消费/十八登记及所有其他src/include保持。
十四固定Legacy对象无新增外部源码消费者，不将这项检索当全端alias排除。

原15完整根90条/330B、30前缀105条/397B、319条身份边界复用，
十四起点74条/1000B窗80条直接引用与两日志tie字段解释已归档。
SDK八生产源与当前实际流/rdbuf、log共享error缓冲和应用配置/pubSync逐项对应；
宽控制私有角色随SDK完整对象承接，不调用隐藏类型。
原70投影逐项保持，15 SDK角色原槽不重叠，85提供者对应与原90分类守恒。

21旧语义定点更新，1186其他/全政策保持；已有adapted只追加引用，
审核150/3680不重复增加，限定E105→108。
语义1207/1204有效ID、18pending/4分类引用，任务10/60、叶0/182和完整门禁/R不提升。
C02/C03/C04/C09继续open；原完整Legacy容量、计算alias/真实消费者、
facet/codecvt和SDK实际正常/快速/callable退出及运行未闭。

**SSO01交付检查**：14项严格UTF-8无BOM/LF、717个本地Markdown链接、45项限定证据/决议引用及Git差异格式通过；measure生成与独立-Check均退出0。任务10/60、叶0/182及完整门禁不提升。
