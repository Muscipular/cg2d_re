# 晚段窄标准流宿主 Init 生命周期 SPEC（SLN01）

> 2026-10-10当前C++边界：[MC01-C1](cg2d_crt_host_migration_spec.md#msvc-crt-cpp-source-state-20261010)已退休无现组外消费者的六组私有C++库及NativeCppException头。当前采用真实MSVC标准流/Init/locale及库内string/facet/codecvt/异常支撑，现流配置/缓冲借用/刷新、业务构析与退出登记保持。下文旧Legacy对象/方法表/私有登记/异常payload及其源码状态属固定历史，不再要求复刻；完整业务/owner/未知原alias与SDK实际运行仍待，未编译/运行。

2026-10-05，按[D03](cg2d_crt_startup_lifecycle_spec.md)与
[宿主承接范围](cg2d_crt_host_migration_spec.md)推进原553134。
固定快照：`cg2d-re-sdk-late-narrow-init-source-20261005`。

<a id="sdk-late-narrow-init-baseline"></a>
## 原基线与宿主事实

[原证据](cg2d_sdk_late_narrow_init_evidence.json)保存三完整根19条/81B：
5491D6六条/22B，调用5491E0后在5491EF登记5491F6，直接返回完整S32；
5491E0两条/10B，以独立1086539地址尾入5482DD；
5491F6十一条/49B，含分离共享尾54843C..548463，递减592894，
非零返回，零才按cout→cerr→clog调用5495EE。
三个连续原槽553130→549194、553134→5491D6、553138→549200均已加载。

1086539只有上述构造/退出两条已证直接取址。
[SI01](cg2d_standard_stream_init_identity_spec.md)已核完整311条窄宽控制器：
Init this仅保存/返回，真正流/缓冲为另一变量，不读取Init业务字段。
本组复用该边界，不再审其七阶段构造或把native sizeof当原尺寸证明。
控制器当前兼容接口与计数/被动流仍存在，不据SDK适配称它们已活动。

本机MSVC STL 14.50.35717 xiosbase公开完整ios_base::Init：
构造调用_Init_ctor，析构noexcept调用_Init_dtor。
对应iostream.cpp构造以独立SDK计数非负则加一、否则置1；
析构递减，零时对非空SDK指针依次cerr/clog/cout调用flush。
这些实现文本可追，不证明客户端实际链接ABI/退出注册/运行。

宽流_Winit虽有SDK实现，iostream声明受_CRTBLD门禁隐藏，
普通客户端不可见。本轮不定义该宏、不复制声明或把窄Init冒充宽控制器；
原553138以及其他14流/缓冲继续待接。

<a id="sdk-late-narrow-init-decision"></a>
## 具名裁决与开发

1. 在LegacyMouse.cpp的gLegacyPairedEmptyExit553130之后，
   后续553158/55315C之前定义一个完整、独立的
   `std::ios_base::Init gSdkNarrowLateInit553134`。
   普通同TU有序动态构造与自动析构实际进入SDK两提供者，不新增空包装或查询替代生命周期。
2. SDK完整类型/私有计数/自动退出清理成组承接本项；
   不调用旧RegisterLegacyNarrowStandardInitializer_5491D6或旧Release回调，
   不更新592894、1086539、Legacy registry/14对象，不强转SDK和Legacy布局。
   旧兼容对象/本体保留；今后若活动Legacy消费者须在该小步核定生产/引用和退出。
3. 原cinit弃登记返回；SDK自动退出注册失败/时点/私有计数与末次flush
   按D02/D03为具名adapted差异。SDK窄刷新顺序为cerr→clog→cout，
   带指针门禁及现代flush/sentry，不能声称原cout→cerr→clog、
   无sentry的5495EE、原失败S32或机器ABI等价。
   既有应用侧窄/宽适配析构仍按原pubsync顺序，所有业务登记/清理不跳过。
4. 不补原/宿主影子计数、once、catch、回滚或伪atexit结果；
   宿主构造/析构规则照完整SDK类型执行。未经运行不称SDK实际异常/终止等价。
   不引用受_CRTBLD隐藏的_Winit或改编译宏/工程/依赖。
5. 只553134转宿主窄Init活动提供者。
   当前69→70＝66应用＋2宿主流配置＋1经典locale＋1窄Init；
   待接16→15，其中1宽Init和14标准流/缓冲。
   原null1、音频暂停2、键盘排除2保持；完整90数组/旧cinit不启用。

<a id="sdk-late-narrow-init-acceptance"></a>
## 验收标准

| 项 | 本组标准 |
|---|---|
| I | 一个完整公开SDK Init对象、真实构造及自动析构，独立具名实例 |
| W | 原553130→553134→后续553158顺序成立；旧69绑定/四配对登记/应用刷新保持，无旧窄登记/布局互转 |
| S | 三完整原根19条/81B、三槽/两身份引用可追；一个源码插入重放全文一致，SDK声明/实现及宽门禁归档 |
| 限制 | SDK计数/注册/刷新差异获准为adapted；实际ABI/失败/退出与运行仍待，不提升完整门禁 |

C02/C03/C04/C09继续open，完整registry容量/计算alias/codecvt/facet、15活动项、
全应用normal/quick/callable退出及R未闭。
禁止构建/测试/运行/摘要和单线程要求保持，不降低验收标准。

<a id="sdk-late-narrow-init-source-closure"></a>
## 源码小步收口

独立完整std::ios_base::Init对象已放在原553130之后、553158之前；
实际SDK构造及自动析构承接553134，没有旧计数/登记的双执行。
一个源码插入重放全文一致，其余src/include和应用窄宽刷新/四配对登记保持；
旧69项绑定逐项保持，新70项与原90加载槽/同TU声明序一致。
三完整原根19条/81B、三连续槽/两身份取址和SDK声明/计数/末次刷新、
宽类型_CRTBLD门禁已归档。SDK私有flush的具体差异按本裁决保留，未记原体等价。

三旧语义定点追加，1204其他记录、库存/政策不变；
既有Init/登记两个adapted追加证据，5491F6完整退出根新增adapted，
审核149→150/3680；限定静态证据102→105。
语义1207/1204有效ID、18pending/4分类引用保持，
C02/C03/C04/C09继续open，任务10/60、叶0/182和完整门禁/R不提升。
宽Init及14流/缓冲、SDK实际构析/失败/ABI与完整退出仍待。

**SLN01交付检查**：13项严格UTF-8无BOM/LF、709个本地Markdown链接、30项限定证据/决议引用及Git差异格式通过；measure生成与独立-Check均退出0。任务10/60、叶0/182及完整门禁不提升。
