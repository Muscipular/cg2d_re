# 宿主客户端可执行目标 SPEC（BRS01）

2026-10-07，按“启动 → 选择服务器 → 登录 → 选择人物／创建人物 → 主地图”的新优先级推进。工程委托沿子项目规则第8条和 D03；本步先解除工程入口没有目标的实际缺口，不以新增目标代替链接、运行或完整流程验收。

<a id="hosted-client-target-baseline"></a>
## 实证与既有基线

- 本步前 CMake 只有 core/startup/render 三个 STATIC 目标。真实 `WinMain` 在 `src/platform/ClientEntry.cpp`，原端范围 `[465790,4659ED)`，已经有函数体，原初始化九个调用顺序已记录在[启动基线](cg2d_startup_flow.md)；不重复实现。
- D03 要求宿主 MSVC/UCRT 完成自身启动后进入应用；旧 `53DDB5` 启动父和旧 C++90 表不能改成自定义入口再次执行。现有 LegacyMouse 同一 TU 的应用相对初始化及 heap/lowio/stdio/multibyte/filter 前置沿已提交合同，不在 WinMain 中重新调用。
- 已有 startup 目标包含真实 MainLoop、场景分发、登录、角色、网络和地图源码；render 目标提供 SDL 呈现。新目标把两个现有库接入，core 由 startup 的 PUBLIC 依赖承接；系统和已有 SDL SDK 链接依赖保持。
- `DrawNormalScene_51C7D0` 已有定义；本次不把历史未取证状态当作缺失本体。

<a id="hosted-client-target-decision"></a>
## 委托裁决与实施

1. 在本线 CMake 定义唯一 `cg2d_re_client`、`WIN32` 可执行目标，直接编译已有 ClientEntry.cpp；从 startup 源列表移除该文件，使入口归属唯一。
2. 显式链接 startup 和 render，使用既有 PUBLIC C++20 和 include 合同；目标关闭 C++ 扩展、RTTI，使用 /utf-8、/W4、/fp:strict、_CRT_SECURE_NO_WARNINGS。
3. 由宿主选择标准 Windows CRT 入口。禁止 /ENTRY、替代 main/SDL_main、第二份 WinMain、整表重调、假依赖体和固定成功。原命令行、updated 门禁、保存/窗口/主循环调用及返回保持。
4. 不修改根 CMake，不下载依赖，不调整既有配置预设，不复制或检查编译产物。本步只编辑源目标，仍禁止配置、构建、测试、运行与摘要计算。
5. 启动目标缺口在源码层收口；登录负分类采样和 U03 版本正文按真实 owner/原消费者继续开发，不以本目标新增为它们背书。

<a id="hosted-client-target-acceptance"></a>
## 可度量验收

| 指标 | 本步前 | 本步后／静态验收条件 |
|---|---|---|
| 本线客户端可执行目标 | 0 | 1，WIN32 且使用 ClientEntry.cpp |
| 入口编译归属 | startup 中 1 次 | 客户端目标中 1 次，startup 中 0 次 |
| 入口函数体、旧启动及活动初始化 | 既有实现 | 全文保持，不增调用或替代入口 |
| 显式客户端业务库 | 无 | startup/render 两条，core 沿既有传递依赖 |
| 构建配置和平台约束 | 三库、Windows/MSVC/x64、SDL3 3.4.8 | 三库保留，原约束和预设保持 |
| 全局完整门禁 | 未验 | 未验，C02/C04/V02 继续 open |
| 静态登记 | E800 | E802，仅本目标局部 I/S 两条；完整 feature gates 不改 |

静态核对只比较本步 CMake 差异、入口全文和已有生命周期文件未改、源文件路径存在、唯一归属、禁止入口选项未引入，及登记/派生快照增量。不得把目标声明称为编译、链接或可运行。

<a id="hosted-client-target-source-state"></a>
## 当前推进链与边界

| 顺序 | 已有实现及本步结果 | 下一实际开发缺口 |
|---|---|---|
| 启动 | 真实 WinMain、初始化和 MainLoop 已有；本步接唯一可执行目标 | 所有传递依赖与完整构析/退出、资源接线逐项收口 |
| 选择服务器 | 登录状态机、服务器记录和解析/查询已有 | ReadNegativeSelectionAvailability 实缺定义；20个共享宿主字节完整对应 |
| 登录 | 真实连接/输入和原格式明文网络实现已有 | 沿连接/回复/角色状态真实依赖推进，不改协议 |
| 选择／创建人物 | 角色槽、创建及升级发送消费者已有 | 独立9A5C10版本正文容量、正常内容生产和寿命闭合 |
| 主地图 | DrawNormalScene 已有定义及地图源码 | 沿进入主地图的实际状态/资源依赖补缺口；已有本体不重造 |

阶段表用于确定开发顺序，不是完成率。原函数分母仍为3680；本步不新增语义函数、处置、成员或 R7 条目。音频暂停、原网络格式明文、RPLAN.24及所有完整验收标准保持。
