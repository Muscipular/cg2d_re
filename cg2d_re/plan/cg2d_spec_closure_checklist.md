# cg2d SPEC 差异与收尾任务表

> 2026-10-09按用户要求压缩对齐，覆盖至CAC11。压缩前302行完整任务流水固定于Git命名快照 `cg2d-re-spec-history-20261009-before-progress-compression`，按原文件路径可读；不复制整份归档。
> 本表只保留当前结论、下一步和主证据；逐步原证/局部验收及历史失败沿[验收登记簿](cg2d_acceptance_registry.json)、对应子合同与上述快照保留。[唯一进度](cg2d_full_reconstruction_progress.md)记录执行结果。
> **整体SPEC未完成。60任务关闭11、开放49；这不是全功能分母或整体完成率。** 未显式登记的任务按open处理。关闭只覆盖其具名源码/差异合同，完整功能仍受[总SPEC§10](cg2d_full_reconstruction_plan.md#10-完成门禁)约束。

## 1. 当前口径

- 原函数3680，语义1964条/1943个原ID＋21条区间身份；合同关联2053/3680（55.79%），已审F处置1097。关联率不是实现或验收完成率。
- 导入16/218、区间1509/3061已审，区间剩1552；18个pending是历史跟踪入口，不等于18个未实现函数。
- 184个登记叶功能整项验收0，B/I/W/S/R完整门禁均0通过，六类closure均open；局部证据1310条不得代替完整门禁。
- 历史Debug链接及CAC04选人入场用户复验保留；当前不构建、不测试、不运行、不查产物、不计算摘要、单线程。音频开发已恢复，运行验收仍受限。
- 已知格式库存1007真实＋13排除＋3替换＝1023；旧“6音频暂停”“链接剩8项”“version缺定义”“G05只有使用关系”等摘要已被后续实施覆盖。
- 当前网络以服务端既有原命令/原字段/LF明文合同为准，旧W16帧只作历史对照；只排除两加解密叶。当前文字按已批静态GBK/宋体/ASCII竖线光标，其他数据来源边界保持。

## 2. P01–P18：既有显式入口

关闭集合：P01–P06、P08–P10、P18及U03。源码关闭不外推到对应页面/网络/战斗完整功能。音频七项均开放，不再标为暂停。

| 编号 / 范围 | 任务状态与已有成果 | 下一步及关闭边界 | 主证据 |
|---|---|---|---|
| P01 / 424620 | 已关闭（局部源码合同）；完整攻击生产、五表/四输出及三父已接，D01局部适配已落位 | D01次级图号入口0一次、跨轮状态/完整技能/收尾保持；全战斗/生命周期与运行另验 | [BNK01](../doc/cg2d_battle_m_attack_targets_call_sampling_spec.md#battle-m-attack-targets-call-sampling-source-state)；[源码收口](../doc/cg2d_battle_attack_targets_spec.md#attack-target-source-closure) |
| P02 / 4FF650 | 已关闭（局部源码合同）；完整技能页、第四按钮适配及同源写者已接 | 条件布局、全部消费与W32返回保持；外层/完整后续及运行仍待 | [源码收口](../doc/ui_windows/legacy_skill_panels.md#skill-detail-source-closure) |
| P03 / 402020 | 已关闭（局部源码合同）；日志装载/计数及两内部调用已接，独立1000B入口清零 | 独立1000B仅入口清零一次；完整旧路由/文件与运行仍待 | [静态收口](../doc/ui_windows/legacy_log_viewer.md#legacy-log-source-closure) |
| P04 / 4021A0 | 已关闭（局部源码合同）；日志页面及内部链已接，独立1000B入口清零 | 独立1000B仅入口清零；旧表外层、输入/音频与运行仍待 | [静态收口](../doc/ui_windows/legacy_log_viewer.md#legacy-log-source-closure) |
| P05 / 4D62A0 | 已关闭（局部源码合同）；交出/接受确认及共享消费已接，仅格式后首字节置1 | 其余255B不清，原循环/发送保持；外层/完整流程/运行仍待 | [源码收口](../doc/ui_windows/legacy_give_receive_confirmation.md#give-receive-source-closure) |
| P06 / 4F7870 | 已关闭（局部源码合同）；宠物状态选择器及唯一父已接，两hover入口置-1 | 两hover入口-1；正常扫描/尾重查保持，旧外层与运行仍待 | [源码收口](../doc/ui_windows/legacy_pet_list.md#pet-state-source-closure) |
| P07 / 4D8DA0 | 开放；旧62完整本体及单W32命令读取已接；实际模式/第二命令原值仍待证 | C9A94C实际生产/输入域、未写第二命令原值及旧外层/完整页与机器门禁；CPP直接未生产整数求值已修，不猜值/删行或评原栈等价 | [SCP01状态](../doc/cg2d_shop_choice_body_spec.md#shop-choice-body-source-state) |
| P08 / 4626A0 | 已关闭（局部源码合同）；WorldUpdate显式gate=1，本体与调用顺序已对齐 | 本体及顺序保持；完整创建/数据/请求验收沿U03/V03继续 | [静态证据](../doc/cg2d_creation_and_module_adaptation_evidence.md#p08-创建入口显式gate) |
| P09 / 403C50 | 已关闭（局部源码合同）；旧帧源码工作项历史关闭；当前接服务端既有LF明文取行 | CAC04仅证明一次选人入场恢复；半行/连续行、重连/断线、完整互通与生命周期仍按V03验收。 | [当前源码状态](../doc/cg2d_network_compatibility_spec.md#local-server-plaintext-source-state) |
| P10 / 403E50 | 已关闭（局部源码合同）；旧帧源码工作项历史关闭；当前接原命令/原字段明文入队 | 原字段、队列owner和部分发送保留；完整互通及V03仍待运行证据 | [当前源码状态](../doc/cg2d_network_compatibility_spec.md#local-server-plaintext-source-state) |
| P11 / 454C30 | 开放；固定WAV、独立音乐流和音量表已有提供者，历史链接通过；一次转换已交付，后续只推进客户端WAV链 | 验现有WAV质量、实际资源/设备失败/声音及适用音源结果 | [固定WAV交付](../doc/cg2d_client_link_sdl_audio_spec.md#fixed-music-wav-source-state)；[当前范围](../doc/cg2d_client_link_sdl_audio_spec.md#wav-only-development-scope-20261009) |
| P12 / 454EE0 | 开放；停止/销毁流、释放PCM及共享设备清理已接，历史链接通过 | 验回调停止、共享设备及真实退出寿命 | [固定WAV交付](../doc/cg2d_client_link_sdl_audio_spec.md#fixed-music-wav-source-state) |
| P13 / 458AE0 | 开放；实际SDL设备能力与独立输入入口已有提供者，历史链接通过 | 验实际能力及设备/输入失败分支 | [CAA01](../doc/cg2d_client_link_sdl_audio_spec.md#client-link-sdl-audio-source-state) |
| P14 / 4FDBD0 | 开放；旧音乐页面、两初始化/五控件及回调绑定已有，开发已恢复 | 实际旧99表外层、完整保存/声音及owner寿命待；历史链接不代当前版本或完整流程验收。 | [当前源码](../doc/cg2d_display_settings.md#audio-settings-pages-source-state) |
| P15 / 4FE0E0 | 开放；旧音效页面、初始化/八控件及音量/声道回调已有，开发已恢复 | 实际旧99表外层、完整保存/声音/声道与owner寿命待；原15/0界限及S32读取保留。 | [当前源码](../doc/cg2d_display_settings.md#audio-settings-pages-source-state) |
| P16 / 50E680 | 开放；murD真实提供者、固定WAV/混响已接，历史链接通过；MIDI一次转换及离线工具只保留历史，不继续开发 | 验现WAV质量/适用原音源结果、效果创建/格式、声音/设备/失败/通知/寿命、有限无限重复/停止与murD互通 | [固定WAV](../doc/cg2d_client_link_sdl_audio_spec.md#fixed-music-wav-source-state)；[混响](../doc/cg2d_client_link_sdl_audio_spec.md#fixed-wav-reverb-source-state)；[当前范围](../doc/cg2d_client_link_sdl_audio_spec.md#wav-only-development-scope-20261009) |
| P17 / 52C3D0 | 开放；真实BGM音量应用及后续运行表/控制已接，历史链接通过 | 验声音变化及实际失败/owner | [CAA01](../doc/cg2d_client_link_sdl_audio_spec.md#client-link-sdl-audio-source-state) |
| P18 / 5202E0 | 已关闭（局部源码合同）；Filename[256]入口一次预清的局部适配已落位 | 原API/后续失败与成功消费保持；完整登录/API及运行沿V03继续 | [静态证据](../doc/cg2d_creation_and_module_adaptation_evidence.md#p18-模块路径预清) |

## 3. C01–C12：CRT与启动

| 编号 / 范围 | 任务状态与已有成果 | 下一步及关闭边界 | 主证据 |
|---|---|---|---|
| C01 / CRT 总策略 | 开放；受限宿主策略及大量CRT/数学/NLS本体已有局部实施和接线证据 | 全部owner/模式寿命/alias/R1/R7和SDK异常失败/完整CRT运行未闭 | [M3P01](../doc/cg2d_mode3_private_core_spec.md#mode3-private-core-source-closure)；[运行库](../doc/cg2d_crt_runtime_boundary_spec.md) |
| C02 / C++90 初始化表 | 开放；CAB07原表72有序源投影＋15 SDK提供＝87；另1空槽/2键盘排除 | 两音频初始化已接，完整入口/完整消费者/alias/SDK运行仍待；null1/键盘排除2另列 | [SSO01](../doc/cg2d_sdk_standard_stream_owners_spec.md#sdk-standard-owners-source-closure)；[BRS01](../doc/cg2d_hosted_client_executable_target_spec.md#hosted-client-target-source-state) |
| C03 / 四Init与登记 | 开放；六兼容Init接口、SDK标准流/缓冲与私有登记已有实际提供者 | 原完整容量/较大索引/alias、Legacy活动消费者/facet/codecvt、SDK构析和退出运行仍待 | [SSO01](../doc/cg2d_sdk_standard_stream_owners_spec.md#sdk-standard-owners-source-closure)；[SR01](../doc/cg2d_standard_stream_registry_prefix_spec.md#standard-registry-source-closure) |
| C04 / 构析与入口迁移 | 开放；唯一宿主入口、活动初始化、失败前缀/逆序清理及filter恢复已有 | 全Legacy消费者/owner/成员EH/硬件SEH、跨TU及完整normal/quick/callable和SDK运行 | [HFI01](../doc/cg2d_hosted_exception_filter_spec.md#hosted-filter-source-closure)；[BRS01](../doc/cg2d_hosted_client_executable_target_spec.md#hosted-client-target-source-state) |
| C05 / codecvt | 开放；四已知streambuf/filebuf方法表及十个消费字段已有类型对应 | 非空codecvt/facet真实生产/完整表、全活动流/owner/alias、SDK ABI与运行仍待 | [STP01](../doc/cg2d_stream_table_pointer_spec.md#stream-table-pointer-source-closure)；[FBO01](../doc/cg2d_file_buffer_output_alignment_spec.md#file-buffer-output-source-closure) |
| C06 / 异常查找捕获 | 开放；原异常查找/捕获私有职责由宿主承接；NLS原SEH消费有局部证据 | 旧5402AB/5407BC由本机职责承接；实际typed捕获/清理、成员与硬件SEH、SDK生成ABI/运行待验 | [BPK01](../doc/cg2d_native_cpp_exception_spec.md#cpp-catch-private-host-source-state)；[HFI01](../doc/cg2d_hosted_exception_filter_spec.md#hosted-filter-source-closure) |
| C07 / 抛出与SDK | 开放；八处实际throw/一处重抛、六类型及单payload owner已接 | 实际C++20生成ABI、按值捕获/展开/非法重抛与SDK及完整生命周期仍待 | [BPK01](../doc/cg2d_native_cpp_exception_spec.md#cpp-catch-private-host-source-state)；[NCE01收口](../doc/cg2d_native_cpp_exception_spec.md#native-cpp-exception-source-closure) |
| C08 / 原堆模式3 | 开放；五SDK操作及七原调用角色已接，三个无本体私有核心声明已撤 | 模式稳定/同域、全部owner/未知alias/R1/R7与SDK实际失败/退出运行仍待 | [M3P01](../doc/cg2d_mode3_private_core_spec.md#mode3-private-core-source-closure) |
| C09 / 图形双树 | 开放；双树、逐节点面及调色板清理有局部接线和顺序修正 | 完整应用入口及逐surface/快速退出清理和SDK运行仍待 | [双树源码](../doc/cg2d_graphic_transform_lifecycle_spec.md#transform-cache-lifecycle-source-closure)；[SSO01](../doc/cg2d_sdk_standard_stream_owners_spec.md#sdk-standard-owners-source-closure) |
| C10 / FILE 剩余原调用 | 开放；唯一stdio表初始化已接；FILE核心、业务/库调用及共享状态持续落实 | 其他原点/全FILE配置、跨TU首次使用/描述符/owner/alias、SDK实际成功失败与退出运行仍待 | [HSI01](../doc/cg2d_hosted_stdio_init_spec.md#hosted-stdio-source-closure) |
| C11 / 应用分配域 | 开放；模式3宿主域及两独立SDK域已落实部分owner配对，保持域边界 | 全部producer/owner配对、稳定模式/未知alias与SDK运行仍待 | [M3P01](../doc/cg2d_mode3_private_core_spec.md#mode3-private-core-source-closure)；[内存](../doc/cg2d_crt_memory_contract.md) |
| C12 / 字符串与格式剩余 | 开放；CAB25已知1023格式原点归属齐：1007真实、13排除、3替换；音频扫描已接 | 已知调用库存归属不代完整语法/所有间接父；P07未写参数、全状态/SDK/CRT正常寿命及运行仍待。 | [格式/扫描合同](../doc/cg2d_stdio_contract.md) |

## 4. U01–U11：世界、UI与战斗

| 编号 / 范围 | 任务状态与已有成果 | 下一步及关闭边界 | 主证据 |
|---|---|---|---|
| U01 / 宠物技能目录 | 开放；旧58–61页面/标量已有；目录生产采集器已准备但未执行 | CA0478数组定义/容量/生产、图号/页数运行时生产；先获本项原端动态许可，捕写者后追完整循环/owner | [旧58–61](../doc/ui_windows/legacy_pet_skill_catalog.md)；[生产取证SPEC](../doc/cg2d_pet_catalog_production_spec.md) |
| U02 / 交易名单 | 开放；旧51消费/页号/标量已有；名单生产采集器已准备但未执行 | CB1D1C的17B名单容量/定义/生产及数量/页数写者 | [TTP01](../doc/cg2d_trade_target_production_spec.md#trade-target-production-source-state)；[旧51](../doc/ui_windows/legacy_trade_target_list.md) |
| U03 / version payload | 已关闭（局部源码合同）；BRT01已落实const char[1]零前缀owner、两次只读外传和首BYTE零分支 | 本源码合同已收口；完整角色/互通/生命周期仍按V03等验收。1B为正常读取足迹对应新端容量，原未使用预留尾部不伪称已知 | [创建](../doc/cg2d_character_select_scene.md#version-zero-prefix-current)；[声明](../src/CharacterSelectSceneDependencies.hpp) |
| U04 / 旧表与键表真实分派 | 开放；现代及大量旧页面/键入口已有；CAC10资料切页两轴取样已修 | 57C368外层间接路由、键表真实分派及输入焦点 | [BVL01](../doc/cg2d_legacy_panel_native_sampling_spec.md#legacy-panel-native-source-state)；[BVM01](../doc/cg2d_legacy_panel_button_bridge_sampling_spec.md#panel-button-bridge-source-state) ；[CAC10](../doc/ui_windows/shared_profile.md#profile-page-coordinate-source-state-20261009) |
| U05 / 商店/修理/鉴定 | 开放；旧62完整父、63–71页面和单W32命令读取已有；模式/生产仍待 | 独立商品/图号/页数/初页与C9A94C实际生产；未写第二命令/旧路由及完整回复/寿命 | [旧62](../doc/ui_windows/legacy_shop_choice.md)；[SCP01](../doc/cg2d_shop_choice_body_spec.md#shop-choice-body-source-state) |
| U06 / 仓库/交换/交出 | 开放；旧72–76、83–85页面/格式与确认局部适配已有 | D30404/CB1DDC容量、宠物展示、独立商品/页数、共用owner与实际生产闭环；P05局部已关闭不代全链。 | [仓库](../doc/ui_windows/legacy_storage_items.md)；[交换](../doc/ui_windows/legacy_barter_list.md) |
| U07 / 旧55技能正文 | 开放；旧55五行正文同源生产和字段解析已有局部证据 | 其他独立状态/图号生产消费、旧表外路由/请求与跨页覆写后的借用寿命 | [LSP01](../doc/cg2d_legacy_line_sampling_spec.md#legacy-line-sampling-source-state)；[技能选择](../doc/ui_windows/legacy_skill_choice.md) |
| U08 / 战斗执行闭环 | 开放；攻击生产、动作分派、初始化/目标/战果与部分回调清理已有 | 既有P01攻击生产不重做；完整载荷字段/归属、附属清理、全部事件生产—执行—回调—释放及寿命仍待。 | [BQW01](../doc/cg2d_battle_command_init_packet_producer_sampling_spec.md#battle-command-init-packet-producer-sampling-source-state)；[BQX01](../doc/cg2d_battle_entry_result_producer_sampling_spec.md#battle-entry-result-producer-sampling-source-state) |
| U09 / Yobi 生命周期 | 开放；基础构析/克隆、正常地图临时对象及称号-1别名等已落实 | 全部creator/cleanup、载荷浅复制后的共享寿命 | [BRY01](../doc/cg2d_main_entry_state_carrier_sampling_spec.md#main-entry-carrier-source-state)；[BSA01](../doc/cg2d_normal_scene_controller_callee_order_spec.md#normal-scene-callee-source-state) ；[称号别名](../doc/cg2d_yobi_runtime.md#title-none-alias-20261009) |
| U10 / 跨页持久化与请求 | 开放；108路分发及大量请求/持久化链已有；CAC06黑名单两处正常取样已修 | 各业务生产/回复/权限/回滚，整分派/整数/108回复与owner/alias | [BVJ01](../doc/cg2d_logout_deletion_request_tick_sampling_spec.md#request-tick-source-state)；[BVK01](../doc/cg2d_login_deletion_send_chain_baseline_spec.md#send-chain-source-state) |
| U11 / 场景/主循环传递依赖 | 开放；MainLoop/场景更新与大量真实依赖已有；CAC09地图方向桥已修 | 逐状态核所有传递依赖、退出/断线/重进与资源清理 | [BVL01](../doc/cg2d_legacy_panel_native_sampling_spec.md#legacy-panel-native-source-state)；[BVM01](../doc/cg2d_legacy_panel_button_bridge_sampling_spec.md#panel-button-bridge-source-state) ；[CAC09](../doc/cg2d_scene_state.md#map-direction-byte-bridge-source-state-20261009) |

## 5. G01–G05：渲染与资源

| 编号 / 范围 | 任务状态与已有成果 | 下一步及关闭边界 | 主证据 |
|---|---|---|---|
| G01 / 图形资源完整生命周期 | 开放；CPU面/池/双缓存及场景清理已有，角色入队和地图桥有局部实修 | 可选1020EC4源生产与非空第六参地址/alias；全部父、文件/加载/上传/池与节点面分配释放/重建/场景/退出的借用和失效闭环 | [DX SPEC](../doc/cg2d_graphic_dx_cache_x64_spec.md)；[BUY01](../doc/cg2d_surface_transform_cache_baseline_spec.md#surface-transform-cache-source-state) |
| G02 / 队列与命中 | 开放；48B队列、命中/文字消费及条带发布顺序有局部实现/核对 | 全场景/14完整父、命中/文本token/对象与节点借用、上游/间接alias/清理后访问 | [BVF01](../doc/cg2d_back_temp_full_source_snapshot_spec.md#back-temp-source-state)；[BVG01](../doc/cg2d_fx_strip_rectangle_publication_spec.md#fx-strip-rectangle-source-state) |
| G03 / 模式2/3/4 | 开放；MakeSurface尺寸/格式及设备生产/探测已有 | 模式切换/回退、裁剪、采样/混合、提交及重建真实流程 | [纹理](../doc/cg2d_texture_rendering.md)；[呈现](../doc/cg2d_sdl_presenter.md) |
| G04 / 渲染替换差异 | 开放；SDL/CPU替换、调色板/上传/缩放/波纹等有局部原新对照 | 其他上层RAND/宽度/坐标/色键/提交序、全部模式/浮点ABI、资源寿命和像素画面 | [GPC01](../doc/cg2d_palette_cache_alignment_spec.md#palette-cache-source-closure)；[总SPEC §10](cg2d_full_reconstruction_plan.md#10-完成门禁) |
| G05 / 原图标/光标 | 开放；CAC11已实现同目录原客户端只读资源映像提供者及图标/十一光标真实加载点 | 原磁盘文件与选定IDA基线匹配、资源权限/像素/热点、实际加载成功失败、共享句柄及进程寿命待验；不造替代图。 | [装载源码与边界](../doc/cg2d_startup_flow.md#window-resource-module-source-state-20261009) |

## 6. T01–T03：文本与输入

| 编号 / 范围 | 任务状态与已有成果 | 下一步及关闭边界 | 主证据 |
|---|---|---|---|
| T01 / 文本生产消费合同 | 开放；文本生产、字节预算/测宽、绘制/悬停及发布顺序有局部实修 | 各来源编码、容量/NUL、共享存储、字节/像素单位及全部上游/命中/排序父/资源/生命周期逐链归档 | [FLC01](../doc/cg2d_font_lifecycle_alignment_spec.md#font-lifecycle-source-closure)；[文本SPEC §5](../doc/cg2d_text_encoding_spec.md#5-跨函数复核模板) |
| T02 / 输入/IME差异 | 开放；输入池/焦点/鼠标/键盘/IME取样已有修正；旧过宽通过保留失败与修复证据 | 全部上游/原新参数、排序/初始化退出/资源父、焦点/索引、原API候选/间接alias及完整寿命，SDK/画面运行未验 | [BUV01](../doc/cg2d_mouse_tracker_native_links_spec.md#mouse-tracker-native-links-source-state)；[BUW01](../doc/cg2d_cursor_flag_sampling_restore_spec.md#cursor-flag-sampling-source-state) |
| T03 / 字体与显示 | 开放；已确认静态文字GBK、宋体/GB2312_CHARSET、ASCII竖线光标已落实 | 宋体字形、原字节显示、各来源编码/不可表示字符/超长策略及实际画面待验；未授权数据不改编码。 | [文本合同](../doc/cg2d_text_encoding_spec.md) |

## 7. R01–R05：覆盖与追踪

| 编号 / 范围 | 任务状态与已有成果 | 下一步及关闭边界 | 主证据 |
|---|---|---|---|
| R01 / 函数分母 | 开放；原3680函数纳管；1964语义记录/1943个原ID，1097个F处置 | 21条无独立F记录按原区间身份绑定；其余逐ID处置、功能成员/完整本体、混合共享与未知继续核定，不造函数填数。 | [机器语义清单](../doc/cg2d_semantic_function_mappings.json) |
| R02 / 数据/间接目标 | 开放；218导入已审16；已知表、数据、回调持续登记，完整闭包未定 | 未知间接alias/完整owner/重进/动态边，202导入及数据/常量/生命周期 | [NAS01](../doc/cg2d_network_adapter_sampling_spec.md#adapter-sampling-source-state)；[NWT01](../doc/cg2d_workspace_byte_pointer_spec.md#workspace-byte-source-closure) |
| R03 / 未归属区间 | 开放；3061区间已审1509，剩1552；CAC07/08两批各512已纳管 | 沿候选清单处理尚未归属区间，不重做已证分类 | [区间分类合同](../doc/cg2d_unassigned_range_classification_spec.md) |
| R04 / 台账一致性 | 开放；本次对齐当前摘要与60任务；验收登记簿/原证据和历史结论保持 | 按实际功能小步同步同一记录及证据；继续处理后续差异，不把本次摘要对齐当全库语义复审或关闭R04。 | [追踪验收体系](../doc/cg2d_traceability_acceptance_spec.md) |
| R05 / 例外收口 | 开放；屏幕键盘/newest/两加密叶/低色深及混合入口已有具名边界 | 核共享callee、混合32位分支、参数/状态保留及排除接线 | [总SPEC](cg2d_full_reconstruction_plan.md) |

## 8. V01–V06：完整验收

| 编号 / 范围 | 任务状态与已有成果 | 下一步及关闭边界 | 主证据 |
|---|---|---|---|
| V01 / 环境与输入 | 开放；基线/部分来源已有；原资源权限/匹配和完整环境仍未冻结 | 在运行准备获准后记录实际工具链、SDK、系统/编码、参数/配置/资源 | [总SPEC](cg2d_full_reconstruction_plan.md) |
| V02 / 完整启动退出/链接 | 开放；历史Debug客户端及离线工具链接通过；CAC06–11后续源码未构建 | 当前源码编译/链接、跨TU首次使用/SDK ABI、所有owner/alias及normal/quick/callable完整退出仍待相应授权与验收。 | [历史构建证据](../doc/cg2d_build_acceptance_spec.md) |
| V03 / 登录与网络真实流程 | 开放；当前原命令/字段/LF明文链已接；CAC04用户确认本次选人入场正常 | 全部108业务、原字段/整数、失败/重连/断线/数据/owner及适用互通对照待验；不重复修已恢复连接。 | [用户复验](../doc/cg2d_build_acceptance_spec.md#character-entry-confirmed-20261009) |
| V04 / 世界/UI/渲染/输入 | 开放；世界/UI/战斗/渲染/输入局部实现持续补齐；近期MAP/UI/资源仅静态核对 | 获准后按同输入、资源、初值核地图/UI/战斗、焦点/IME、模式和画面 | [总SPEC门禁](cg2d_full_reconstruction_plan.md#10-完成门禁) |
| V05 / 音频真实流程 | 开放；SDL3/WAV和旧7/8开发已恢复，原控制/空间声效/混响等已有；完整声音未验。停止MIDI/DLS工具方向，后续只WAV客户端实际流程 | 继续真实旧外层、WAV播放和全部状态/失败/生命周期；获运行授权后验WAV质量/适用原音源结果、效果创建/格式、声音、通知、音量、循环与退出 | [当前范围](../doc/cg2d_client_link_sdl_audio_spec.md#wav-only-development-scope-20261009)；[当前状态](../doc/cg2d_client_link_sdl_audio_spec.md#wav-only-scope-source-state-20261009) |
| V06 / 最终验收报告 | 开放；尚无完整R8结果；六类closure均open，184叶整项验收0 | 汇总R01–R05和V01–V05，逐适用门禁列结果/未决/例外 | [MRC01](../doc/cg2d_memory_compare_sampling_spec.md#memory-compare-sampling-source-state) |

## 9. 后续推进与维护

1. 按真实功能/数据/依赖链推进源码与接线；优先处理有证据且可实施的缺口，不能以取样检查或登记数量替代功能开发。
2. U01/U02容量/写者、P07模式/第二命令、U04旧外层、C05非空codecvt等事实不足时停止受影响项；不猜容量/路由，不造默认值。动态取证/运行等待对应授权。
3. 每步先记录局部SPEC/边界，实施后集中静态核对，并同步唯一进度、对应任务及必要登记；精确提交，排除用户改动。
4. R01–R05随开发维护；V01–V06及全部B/I/W/S/R门禁保持。静态汇总器通过仅证明台账结构与派生文本一致。
