# cg2d 度量与验收看板

> 自动生成，禁止手改。入口：[体系SPEC](../doc/cg2d_traceability_acceptance_spec.md) · [登记簿](cg2d_acceptance_registry.json) · [完整快照](cg2d_traceability_snapshot.json) · [唯一进度](cg2d_full_reconstruction_progress.md)。
> 验收批次：2026-10-04.1；目录版本：1。全功能分母未闭合；整体未验收。门禁通过数只统计本批归档证据，不等于原有实现数量。

## 1. 分母与可计算指标

| 指标 | 当前值 | 含义/限制 |
|---|---:|---|
| 原函数纳管 | 3680/3680 | 仅库存身份完整 |
| 合同文件关联 | 1351/3680（36.71%） | 文件关联率，不是功能/基线验收完成率 |
| 尚无合同关联 | 2329 | 完整ID见快照metrics.functions_without_document_links；可能已有局部合同尚未机器登记 |
| 历史源码定位 | 2762 | 自动定位快照，非当前实现数 |
| 语义登记 | 1232条/1229个原ID | 无独立ID记录另列3条 |
| 具名分类引用 | 119个原ID | 含排除/混合/合并，不计为实现 |
| 已具名全入口排除 / 混合入口 | 113 / 2 | 不计为实现/验收；混合入口不得整体扣除 |
| 已审核函数处置 | 207/3680 | 初始仅导入关系，未逐ID重新认定 |
| 导入槽 / 未归属区间 | 218 / 3061 | 全部保留稳定ID；已审处置15 / 0 |
| 已登记叶功能 | 182 | 非全功能分母；域不重复计入，暂停保留 |
| 已验收叶功能 | 0/182 | 仅登记集合，不能代表整体完成率 |
| 已知收尾任务 | 60；验收关闭10 | 任务不是全功能目录 |

## 2. 门禁与闭包

| B基线 | I实现 | W接线/寿命 | S静态对照 | R运行 |
|---:|---:|---:|---:|---:|
| 0/182 | 0/182 | 0/182 | 0/182 | 0/182 |

- function_classification：open；尚未按总SPEC收口
- feature_enumeration：open；尚未按总SPEC收口
- data_and_indirect_targets：open；尚未按总SPEC收口
- unassigned_ranges：open；尚未按总SPEC收口
- scope_exceptions：open；尚未按总SPEC收口
- final_acceptance：open；尚未按总SPEC收口

## 3. 功能追踪矩阵

unassessed=未按门禁归档；blocked=执行限制/阻碍；pass/fail=有具名证据的结果。合同存在不等于基线完整。依赖为验收前提，不作为运行架构或调用关系的新事实。同合同候选只按明确文档引用联结，可能含共享/其他功能，必须人工核原成员。

| 功能ID | 范围/名称 | 合同 | 已审/同合同候选成员 | 任务 | B/I/W/S/R |
|---|---|---|---:|---|---|
| FUNC-STARTUP | included：启动与退出 | [cg2d_startup_flow](../doc/cg2d_startup_flow.md)、[cg2d_client_shutdown](../doc/cg2d_client_shutdown.md)、[cg2d_crt_startup_lifecycle_spec](../doc/cg2d_crt_startup_lifecycle_spec.md)、[cg2d_graphic_transform_lifecycle_spec](../doc/cg2d_graphic_transform_lifecycle_spec.md)、[cg2d_resource_container_lifecycle_spec](../doc/cg2d_resource_container_lifecycle_spec.md)、[cg2d_standard_stream_init_identity_spec](../doc/cg2d_standard_stream_init_identity_spec.md)、[cg2d_standard_stream_registry_prefix_spec](../doc/cg2d_standard_stream_registry_prefix_spec.md)、[cg2d_startup_protection_lifecycle_spec](../doc/cg2d_startup_protection_lifecycle_spec.md)、[cg2d_world_state_lifecycle_spec](../doc/cg2d_world_state_lifecycle_spec.md)、[cg2d_ui_input_lifecycle_spec](../doc/cg2d_ui_input_lifecycle_spec.md)、[cg2d_active_initializer_order_spec](../doc/cg2d_active_initializer_order_spec.md)、[cg2d_exit_table_heap_migration_spec](../doc/cg2d_exit_table_heap_migration_spec.md)、[cg2d_standard_stream_exit_migration_spec](../doc/cg2d_standard_stream_exit_migration_spec.md)、[cg2d_standard_stream_controller_gates_spec](../doc/cg2d_standard_stream_controller_gates_spec.md) | 117/385 | C02、C03、C04、V01、V02、C09 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-CRT | included：CRT提供者与兼容边界 | [cg2d_crt_runtime_boundary_spec](../doc/cg2d_crt_runtime_boundary_spec.md)、[cg2d_stdio_contract](../doc/cg2d_stdio_contract.md)、[cg2d_crt_memory_contract](../doc/cg2d_crt_memory_contract.md)、[cg2d_legacy_string_compare](../doc/cg2d_legacy_string_compare.md)、[cg2d_standard_stream_init_identity_spec](../doc/cg2d_standard_stream_init_identity_spec.md)、[cg2d_standard_stream_registry_prefix_spec](../doc/cg2d_standard_stream_registry_prefix_spec.md)、[cg2d_startup_protection_lifecycle_spec](../doc/cg2d_startup_protection_lifecycle_spec.md)、[cg2d_world_state_lifecycle_spec](../doc/cg2d_world_state_lifecycle_spec.md)、[cg2d_ui_input_lifecycle_spec](../doc/cg2d_ui_input_lifecycle_spec.md)、[cg2d_narrow_string_heap_migration_spec](../doc/cg2d_narrow_string_heap_migration_spec.md)、[cg2d_exception_text_heap_migration_spec](../doc/cg2d_exception_text_heap_migration_spec.md)、[cg2d_native_cpp_exception_spec](../doc/cg2d_native_cpp_exception_spec.md)、[cg2d_locale_object_heap_migration_spec](../doc/cg2d_locale_object_heap_migration_spec.md)、[cg2d_exit_table_heap_migration_spec](../doc/cg2d_exit_table_heap_migration_spec.md)、[cg2d_standard_stream_exit_migration_spec](../doc/cg2d_standard_stream_exit_migration_spec.md)、[cg2d_standard_stream_controller_gates_spec](../doc/cg2d_standard_stream_controller_gates_spec.md)、[cg2d_file_buffer_output_alignment_spec](../doc/cg2d_file_buffer_output_alignment_spec.md)、[cg2d_file_position_width_spec](../doc/cg2d_file_position_width_spec.md)、[cg2d_sdk_classic_locale_spec](../doc/cg2d_sdk_classic_locale_spec.md)、[cg2d_sdk_late_narrow_init_spec](../doc/cg2d_sdk_late_narrow_init_spec.md)、[cg2d_sdk_standard_stream_owners_spec](../doc/cg2d_sdk_standard_stream_owners_spec.md)、[cg2d_stdio_table_heap_spec](../doc/cg2d_stdio_table_heap_spec.md)、[cg2d_mode3_heap_facade_spec](../doc/cg2d_mode3_heap_facade_spec.md)、[cg2d_mode3_private_core_spec](../doc/cg2d_mode3_private_core_spec.md)、[cg2d_stream_table_pointer_spec](../doc/cg2d_stream_table_pointer_spec.md)、[cg2d_hosted_runtime_prefix_spec](../doc/cg2d_hosted_runtime_prefix_spec.md)、[cg2d_hosted_stdio_init_spec](../doc/cg2d_hosted_stdio_init_spec.md)、[cg2d_array_counter_spec](../doc/cg2d_array_counter_spec.md)、[cg2d_hosted_exception_filter_spec](../doc/cg2d_hosted_exception_filter_spec.md)、[cg2d_multibyte_string_spec](../doc/cg2d_multibyte_string_spec.md)、[cg2d_cryptint_arithmetic_spec](../doc/cg2d_cryptint_arithmetic_spec.md) | 143/780 | C01、C05、C06、C07、C08、C10、C11、C12 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-LOOP | included：主循环与场景分发 | [cg2d_main_loop](../doc/cg2d_main_loop.md)、[cg2d_scene_state](../doc/cg2d_scene_state.md) | 0/97 | U11、V02、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-LOGIN | included：登录与连接状态 | [cg2d_login_outer_scene](../doc/cg2d_login_outer_scene.md)、[cg2d_login_mode_scene](../doc/cg2d_login_mode_scene.md)、[cg2d_login_state_controller](../doc/cg2d_login_state_controller.md)、[cg2d_login_input_controller](../doc/cg2d_login_input_controller.md)、[cg2d_login_request](../doc/cg2d_login_request.md) | 1/41 | P18、U10、V03 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-CHARACTER | included：角色选择与创建 | [cg2d_character_select_scene](../doc/cg2d_character_select_scene.md)、[cg2d_creation_gate_adaptation_spec](../doc/cg2d_creation_gate_adaptation_spec.md) | 1/13 | P08、U03、V03 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-NETWORK | included：网络帧与协议处理 | [cg2d_network_compatibility_spec](../doc/cg2d_network_compatibility_spec.md)、[cg2d_network_runtime](../doc/cg2d_network_runtime.md)、[cg2d_network_dispatch](../doc/cg2d_network_dispatch.md)、[cg2d_network_plaintext_frame_spec](../doc/cg2d_network_plaintext_frame_spec.md)、[cg2d_winsock_import_binding_spec](../doc/cg2d_winsock_import_binding_spec.md)、[cg2d_network_buffer_lifecycle_spec](../doc/cg2d_network_buffer_lifecycle_spec.md)、[cg2d_command_checksum_publication_spec](../doc/cg2d_command_checksum_publication_spec.md)、[cg2d_append_transform_spec](../doc/cg2d_append_transform_spec.md)、[cg2d_dispatch_account_field_read_spec](../doc/cg2d_dispatch_account_field_read_spec.md)、[cg2d_workspace_byte_pointer_spec](../doc/cg2d_workspace_byte_pointer_spec.md)、[cg2d_header_single_read_spec](../doc/cg2d_header_single_read_spec.md)、[cg2d_protocol_copy_capacity_spec](../doc/cg2d_protocol_copy_capacity_spec.md) | 38/51 | P09、P10、P16、U10、V03 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-SAVED | included：配置与存档 | [cg2d_saved_state_initialize](../doc/cg2d_saved_state_initialize.md)、[cg2d_saved_state_loader](../doc/cg2d_saved_state_loader.md)、[cg2d_saved_state_persist](../doc/cg2d_saved_state_persist.md)、[cg2d_current_saved_record](../doc/cg2d_current_saved_record.md)、[cg2d_saved_codec_arithmetic_spec](../doc/cg2d_saved_codec_arithmetic_spec.md) | 2/4 | U10、V02、V03 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-INPUT | included：平台输入与IME | [cg2d_window_procedure](../doc/cg2d_window_procedure.md)、[cg2d_legacy_mouse](../doc/cg2d_legacy_mouse.md)、[cg2d_input_context](../doc/cg2d_input_context.md)、[cg2d_ime_composition](../doc/cg2d_ime_composition.md)、[cg2d_ime_lifecycle](../doc/cg2d_ime_lifecycle.md)、[cg2d_ui_input_lifecycle_spec](../doc/cg2d_ui_input_lifecycle_spec.md)、[cg2d_ime_composition_alignment_spec](../doc/cg2d_ime_composition_alignment_spec.md)、[cg2d_input_caret_alignment_spec](../doc/cg2d_input_caret_alignment_spec.md)、[cg2d_input_render_alignment_spec](../doc/cg2d_input_render_alignment_spec.md)、[cg2d_input_edit_alignment_spec](../doc/cg2d_input_edit_alignment_spec.md)、[cg2d_input_append_alignment_spec](../doc/cg2d_input_append_alignment_spec.md)、[cg2d_input_text_alignment_spec](../doc/cg2d_input_text_alignment_spec.md) | 14/48 | T01、T02、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-TEXT | included：文本与字体显示 | [cg2d_text_encoding_spec](../doc/cg2d_text_encoding_spec.md)、[cg2d_font_measure](../doc/cg2d_font_measure.md)、[cg2d_text_pending_queue](../doc/cg2d_text_pending_queue.md)、[cg2d_ime_composition_alignment_spec](../doc/cg2d_ime_composition_alignment_spec.md)、[cg2d_input_caret_alignment_spec](../doc/cg2d_input_caret_alignment_spec.md)、[cg2d_input_render_alignment_spec](../doc/cg2d_input_render_alignment_spec.md)、[cg2d_input_edit_alignment_spec](../doc/cg2d_input_edit_alignment_spec.md)、[cg2d_input_append_alignment_spec](../doc/cg2d_input_append_alignment_spec.md)、[cg2d_input_text_alignment_spec](../doc/cg2d_input_text_alignment_spec.md)、[cg2d_text_queue_alignment_spec](../doc/cg2d_text_queue_alignment_spec.md)、[cg2d_font_lifecycle_alignment_spec](../doc/cg2d_font_lifecycle_alignment_spec.md)、[cg2d_multibyte_string_spec](../doc/cg2d_multibyte_string_spec.md) | 20/29 | T01、T03、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-MAP | included：地图与世界数据 | [cg2d_map_scene_drawing](../doc/cg2d_map_scene_drawing.md)、[cg2d_map_draw_cache](../doc/cg2d_map_draw_cache.md)、[cg2d_world_objects](../doc/cg2d_world_objects.md)、[cg2d_world_state_lifecycle_spec](../doc/cg2d_world_state_lifecycle_spec.md) | 17/26 | U11、G01、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-RESOURCE | included：资源加载与来源 | [cg2d_resource_sources](../doc/cg2d_resource_sources.md)、[cg2d_resource_implementation](../doc/cg2d_resource_implementation.md)、[cg2d_graphic_decode](../doc/cg2d_graphic_decode.md)、[cg2d_graphic_info](../doc/cg2d_graphic_info.md)、[cg2d_resource_container_lifecycle_spec](../doc/cg2d_resource_container_lifecycle_spec.md)、[cg2d_graphic_load_alignment_spec](../doc/cg2d_graphic_load_alignment_spec.md)、[cg2d_graphic_upload_alignment_spec](../doc/cg2d_graphic_upload_alignment_spec.md)、[cg2d_queued_draw_alignment_spec](../doc/cg2d_queued_draw_alignment_spec.md)、[cg2d_palette_cache_alignment_spec](../doc/cg2d_palette_cache_alignment_spec.md)、[cg2d_cache_lifecycle_alignment_spec](../doc/cg2d_cache_lifecycle_alignment_spec.md) | 20/69 | G01、G05、V01、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-ANIMATION | included：动画索引与播放 | [cg2d_animation_runtime](../doc/cg2d_animation_runtime.md) | 0/14 | G01、U09、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-RENDER | included：32位渲染与缓存 | [cg2d_graphic_dx_cache_x64_spec](../doc/cg2d_graphic_dx_cache_x64_spec.md)、[cg2d_texture_rendering](../doc/cg2d_texture_rendering.md)、[cg2d_sdl_presenter](../doc/cg2d_sdl_presenter.md)、[cg2d_sub_507140](../doc/cg2d_sub_507140.md)、[cg2d_graphic_transform_lifecycle_spec](../doc/cg2d_graphic_transform_lifecycle_spec.md)、[cg2d_text_queue_alignment_spec](../doc/cg2d_text_queue_alignment_spec.md)、[cg2d_font_lifecycle_alignment_spec](../doc/cg2d_font_lifecycle_alignment_spec.md)、[cg2d_graphic_sort_alignment_spec](../doc/cg2d_graphic_sort_alignment_spec.md)、[cg2d_graphic_load_alignment_spec](../doc/cg2d_graphic_load_alignment_spec.md)、[cg2d_graphic_upload_alignment_spec](../doc/cg2d_graphic_upload_alignment_spec.md)、[cg2d_queued_draw_alignment_spec](../doc/cg2d_queued_draw_alignment_spec.md)、[cg2d_palette_cache_alignment_spec](../doc/cg2d_palette_cache_alignment_spec.md)、[cg2d_cache_lifecycle_alignment_spec](../doc/cg2d_cache_lifecycle_alignment_spec.md) | 26/52 | C09、G01、G02、G03、G04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-UI | included：窗口与旧面板 | [cg2d_ui_window_runtime](../doc/cg2d_ui_window_runtime.md)、[shared_runtime](../doc/ui_windows/shared_runtime.md)、[shared_legacy_panels](../doc/ui_windows/shared_legacy_panels.md) | 0/27 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-UIRUNTIME | included：公共窗口机制与旧表路由 | [shared_runtime](../doc/ui_windows/shared_runtime.md)、[shared_legacy_panels](../doc/ui_windows/shared_legacy_panels.md)、[cg2d_ui_input_lifecycle_spec](../doc/cg2d_ui_input_lifecycle_spec.md) | 4/36 | U04、U10、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-YOBI | included：Yobi创建更新与释放 | [cg2d_yobi_runtime](../doc/cg2d_yobi_runtime.md)、[cg2d_yobi_update_coverage](../doc/cg2d_yobi_update_coverage.md) | 0/12 | U09、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-BATTLE | included：战斗状态与执行 | [shared_battle](../doc/ui_windows/shared_battle.md)、[battle_scene](../doc/yobi_updates/battle_scene.md)、[battle_actor](../doc/yobi_updates/battle_actor.md)、[cg2d_battle_attack_targets_spec](../doc/cg2d_battle_attack_targets_spec.md) | 1/21 | P01、U08、U09、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| FUNC-AUDIO | paused：音频状态与播放 | [cg2d_audio_update](../doc/cg2d_audio_update.md)、[cg2d_display_settings](../doc/cg2d_display_settings.md) | 0/8 | P11、P12、P13、P14、P15、P16、P17、V05 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-001 | included：现代窗口1：状态栏与提示按钮 | [window_1](../doc/ui_windows/window_1.md) | 0/3 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-002 | included：现代窗口2：紧凑状态栏 | [window_2](../doc/ui_windows/window_2.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-003 | included：现代窗口3：折叠状态栏 | [window_3](../doc/ui_windows/window_3.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-004 | included：现代窗口4：快捷交互栏 | [window_4](../doc/ui_windows/window_4.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-005 | included：现代窗口5：折叠快捷交互栏 | [window_5](../doc/ui_windows/window_5.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-006 | included：现代窗口6：底栏与悬停描述 | [window_6](../doc/ui_windows/window_6.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-007 | included：现代窗口7：玩家资料 | [window_7](../doc/ui_windows/window_7.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-008 | included：现代窗口8：玩家明细 | [window_8](../doc/ui_windows/window_8.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-009 | included：现代窗口9：玩家称号 | [window_9](../doc/ui_windows/window_9.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-010 | included：现代窗口10：技能、技术与宠物技能 | [window_10](../doc/ui_windows/window_10.md) | 0/6 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-011 | included：现代窗口11：制造与物品加工 | [window_11](../doc/ui_windows/window_11.md) | 0/9 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-012 | included：现代窗口12：窗口12操作与结果 | [window_12](../doc/ui_windows/window_12.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-013 | included：现代窗口13：窗口13操作与结果 | [window_13](../doc/ui_windows/window_13.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-014 | included：现代窗口14：库存与装备 | [window_14](../doc/ui_windows/window_14.md) | 0/6 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-015 | included：现代窗口15：宠物列表 | [window_15](../doc/ui_windows/window_15.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-016 | included：现代窗口16：宠物详情 | [window_16](../doc/ui_windows/window_16.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-017 | included：现代窗口17：宠物详细属性 | [window_17](../doc/ui_windows/window_17.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-018 | included：现代窗口18：宠物技能详情 | [window_18](../doc/ui_windows/window_18.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-019 | included：现代窗口19：图鉴列表 | [window_19](../doc/ui_windows/window_19.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-020 | included：现代窗口20：图鉴详情 | [window_20](../doc/ui_windows/window_20.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-021 | included：现代窗口21：联系人与邮件列表 | [window_21](../doc/ui_windows/window_21.md) | 0/4 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-022 | included：现代窗口22：发信 | [window_22](../doc/ui_windows/window_22.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-023 | included：现代窗口23：宠物邮件 | [window_23](../doc/ui_windows/window_23.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-024 | included：现代窗口24：设置 | [window_24](../doc/ui_windows/window_24.md) | 0/7 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-025 | included：现代窗口25：快捷键列表与预设文本 | [window_25](../doc/ui_windows/window_25.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-026 | included：现代窗口26：战斗命令 | [window_26](../doc/ui_windows/window_26.md) | 0/4 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-027 | included：现代窗口27：战斗物品 | [window_27](../doc/ui_windows/window_27.md) | 0/4 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-028 | included：现代窗口28：战斗宠物切换 | [window_28](../doc/ui_windows/window_28.md) | 0/3 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-029 | included：现代窗口29：观战结束 | [window_29](../doc/ui_windows/window_29.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-030 | included：现代窗口30：战斗结果 | [window_30](../doc/ui_windows/window_30.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-031 | included：现代窗口31：DP结果 | [window_31](../doc/ui_windows/window_31.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-032 | included：现代窗口32：战斗入场提示 | [window_32](../doc/ui_windows/window_32.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-033 | included：现代窗口33：小地图 | [window_33](../doc/ui_windows/window_33.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-034 | included：现代窗口34：情绪动作 | [window_34](../doc/ui_windows/window_34.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-035 | included：现代窗口35：可调聊天窗口 | [window_35](../doc/ui_windows/window_35.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-036 | included：现代窗口36：通用对话共享实例 | [window_36](../doc/ui_windows/window_36.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-037 | included：现代窗口37：通用对话创建入口 | [window_37](../doc/ui_windows/window_37.md) | 0/4 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-038 | included：现代窗口38：外部物品存放 | [window_38](../doc/ui_windows/window_38.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-039 | included：现代窗口39：计时提示 | [window_39](../doc/ui_windows/window_39.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-040 | included：现代窗口40：读信 | [window_40](../doc/ui_windows/window_40.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-041 | included：现代窗口41：物品与宠物存放 | [window_41](../doc/ui_windows/window_41.md) | 0/6 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-042 | included：现代窗口42：数量输入 | [window_42](../doc/ui_windows/window_42.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-043 | included：现代窗口43：交易 | [window_43](../doc/ui_windows/window_43.md) | 0/9 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-044 | included：现代窗口44：交易目标选择 | [window_44](../doc/ui_windows/window_44.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-045 | included：现代窗口45：文本目标选择 | [window_45](../doc/ui_windows/window_45.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-046 | included：现代窗口46：属性目标选择 | [window_46](../doc/ui_windows/window_46.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-047 | included：现代窗口47：通用确认 | [window_47](../doc/ui_windows/window_47.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-048 | included：现代窗口48：公会详情与职级编辑 | [window_48](../doc/ui_windows/window_48.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-049 | included：现代窗口49：群组选人 | [window_49](../doc/ui_windows/window_49.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-050 | included：现代窗口50：四记录对话 | [window_50](../doc/ui_windows/window_50.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-051 | included：现代窗口51：文本列表 | [window_51](../doc/ui_windows/window_51.md) | 0/5 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-052 | included：现代窗口52：商店选择 | [window_52](../doc/ui_windows/window_52.md) | 0/4 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-053 | included：现代窗口53：商品买卖 | [window_53](../doc/ui_windows/window_53.md) | 0/16 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-054 | included：现代窗口54：宠物与技能选择 | [window_54](../doc/ui_windows/window_54.md) | 0/15 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-055 | included：现代窗口55：公会职级选择 | [window_55](../doc/ui_windows/window_55.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-056 | included：现代窗口56：外观选择 | [window_56](../doc/ui_windows/window_56.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-057 | included：现代窗口57：简化聊天窗口 | [window_57](../doc/ui_windows/window_57.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-058 | included：现代窗口58：自身资料 | [window_58](../doc/ui_windows/window_58.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-059 | included：现代窗口59：自身资料分类选择 | [window_59](../doc/ui_windows/window_59.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-060 | included：现代窗口60：资料分类选择 | [window_60](../doc/ui_windows/window_60.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-061 | included：现代窗口61：资料搜索结果 | [window_61](../doc/ui_windows/window_61.md) | 0/3 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-062 | included：现代窗口62：查看资料 | [window_62](../doc/ui_windows/window_62.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-063 | included：现代窗口63：迷你发信 | [window_63](../doc/ui_windows/window_63.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-064 | included：现代窗口64：资料回复展示 | [window_64](../doc/ui_windows/window_64.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-065 | included：现代窗口65：黑名单管理 | [window_65](../doc/ui_windows/window_65.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-066 | included：现代窗口66：冒险者日志 | [window_66](../doc/ui_windows/window_66.md) | 0/4 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-067 | included：现代窗口67：自身摊位 | [window_67](../doc/ui_windows/window_67.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-068 | included：现代窗口68：浏览摊位 | [window_68](../doc/ui_windows/window_68.md) | 0/8 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-069 | included：现代窗口69：晶石商店69 | [window_69](../doc/ui_windows/window_69.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-M-070 | included：现代窗口70：晶石商店70 | [window_70](../doc/ui_windows/window_70.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-001 | included：旧选择1：地图 | [legacy_map_panel](../doc/ui_windows/legacy_map_panel.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-002 | included：旧选择2：人物三页 | [legacy_profile_panels](../doc/ui_windows/legacy_profile_panels.md) | 0/3 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-003 | included：旧选择3：设置主菜单 | [legacy_settings_menu](../doc/ui_windows/legacy_settings_menu.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-004 | included：旧选择4：退出 | [legacy_logout_panel](../doc/ui_windows/legacy_logout_panel.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-005 | included：旧选择5：聊天设置 | [legacy_chat_settings](../doc/ui_windows/legacy_chat_settings.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-006 | included：旧选择6：预设编辑 | [legacy_preset_editor](../doc/ui_windows/legacy_preset_editor.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-009 | included：旧选择9：鼠标设置 | [legacy_mouse_settings](../doc/ui_windows/legacy_mouse_settings.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-010 | included：旧选择10：快捷键说明 | [legacy_hotkey_help](../doc/ui_windows/legacy_hotkey_help.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-011 | included：旧选择11：扫描线设置 | [legacy_scanline_settings](../doc/ui_windows/legacy_scanline_settings.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-012 | included：旧选择12：按钮设置 | [legacy_button_settings](../doc/ui_windows/legacy_button_settings.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-014 | included：旧选择14：物品 | [legacy_inventory](../doc/ui_windows/legacy_inventory.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-015 | included：旧选择15：金钱 | [legacy_money](../doc/ui_windows/legacy_money.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-016 | included：旧选择16：拆分数量 | [legacy_item_split](../doc/ui_windows/legacy_item_split.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-017 | included：旧选择17：十八动作菜单 | [legacy_emote_menu](../doc/ui_windows/legacy_emote_menu.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-018 | included：旧选择18：技能列表与详情 | [legacy_skill_panels](../doc/ui_windows/legacy_skill_panels.md) | 0/3 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-019 | included：旧选择19：技能列表与详情 | [legacy_skill_panels](../doc/ui_windows/legacy_skill_panels.md) | 1/3 | U04、V04、P02 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-020 | included：旧选择20：宠物列表 | [legacy_pet_list](../doc/ui_windows/legacy_pet_list.md) | 3/2 | U04、V04、P06 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-021 | included：旧选择21：宠物资料 | [legacy_pet_detail](../doc/ui_windows/legacy_pet_detail.md) | 0/3 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-022 | included：旧选择22：宠物改名 | [legacy_pet_rename](../doc/ui_windows/legacy_pet_rename.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-023 | included：旧选择23：图鉴列表 | [legacy_album_list](../doc/ui_windows/legacy_album_list.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-024 | included：旧选择24：图鉴详情 | [legacy_album_detail](../doc/ui_windows/legacy_album_detail.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-025 | included：旧选择25：制造配方 | [legacy_craft_recipes](../doc/ui_windows/legacy_craft_recipes.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-026 | included：旧选择26：制造执行 | [legacy_craft_execution](../doc/ui_windows/legacy_craft_execution.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-027 | included：旧选择27：制造材料 | [legacy_craft_materials](../doc/ui_windows/legacy_craft_materials.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-028 | included：旧选择28：制造结果 | [legacy_craft_result](../doc/ui_windows/legacy_craft_result.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-029 | included：旧选择29：镶嵌执行 | [legacy_gem_execution](../doc/ui_windows/legacy_gem_execution.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-030 | included：旧选择30：镶嵌材料 | [legacy_gem_materials](../doc/ui_windows/legacy_gem_materials.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-031 | included：旧选择31：加工选择 | [legacy_item_work_selection](../doc/ui_windows/legacy_item_work_selection.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-032 | included：旧选择32：加工名称 | [legacy_item_work_name](../doc/ui_windows/legacy_item_work_name.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-033 | included：旧选择33：加工结果 | [legacy_item_work_result](../doc/ui_windows/legacy_item_work_result.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-034 | included：旧选择34：FP不足提示 | [legacy_fp_notice](../doc/ui_windows/legacy_fp_notice.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-035 | included：旧选择35：技能目标名单 | [legacy_skill_target_list](../doc/ui_windows/legacy_skill_target_list.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-036 | included：旧选择36：技能属性目标 | [legacy_skill_status_targets](../doc/ui_windows/legacy_skill_status_targets.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-037 | included：旧选择37：技能结果 | [legacy_skill_result](../doc/ui_windows/legacy_skill_result.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-038 | included：旧选择38：采集 | [legacy_gather_panel](../doc/ui_windows/legacy_gather_panel.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-039 | included：旧选择39：采集结果 | [legacy_gather_result](../doc/ui_windows/legacy_gather_result.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-040 | included：旧选择40：战斗与点数结果 | [legacy_result_panels](../doc/ui_windows/legacy_result_panels.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-041 | included：旧选择41：战斗与点数结果 | [legacy_result_panels](../doc/ui_windows/legacy_result_panels.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-042 | included：旧选择42：联系人登记本 | [legacy_mail_contact_list](../doc/ui_windows/legacy_mail_contact_list.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-043 | included：旧选择43：联系人排序 | [legacy_mail_contact_sort](../doc/ui_windows/legacy_mail_contact_sort.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-044 | included：旧选择44：联系人删除 | [legacy_mail_contact_delete](../doc/ui_windows/legacy_mail_contact_delete.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-045 | included：旧选择45：邮件发送模式 | [legacy_mail_delivery_mode](../doc/ui_windows/legacy_mail_delivery_mode.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-046 | included：旧选择46：普通邮件多人撰写 | [legacy_mail_normal_compose](../doc/ui_windows/legacy_mail_normal_compose.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-047 | included：旧选择47：普通邮件阅读 | [legacy_mail_read](../doc/ui_windows/legacy_mail_read.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-048 | included：旧选择48：送信宠物 | [legacy_mail_pet_list](../doc/ui_windows/legacy_mail_pet_list.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-049 | included：旧选择49：宠物邮件撰写 | [legacy_mail_compose](../doc/ui_windows/legacy_mail_compose.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-050 | included：旧选择50：邮件赠品 | [legacy_mail_gift_items](../doc/ui_windows/legacy_mail_gift_items.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-051 | included：旧选择51：交易目标名单 | [legacy_trade_target_list](../doc/ui_windows/legacy_trade_target_list.md) | 0/1 | U04、V04、U02 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-052 | included：旧选择52：交易 | [legacy_trade_panel](../doc/ui_windows/legacy_trade_panel.md) | 0/7 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-053 | included：旧选择53：普通对话 | [legacy_basic_dialogs](../doc/ui_windows/legacy_basic_dialogs.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-054 | included：旧选择54：普通行选择 | [legacy_basic_dialogs](../doc/ui_windows/legacy_basic_dialogs.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-055 | included：旧选择55：技能选择 | [legacy_skill_choice](../doc/ui_windows/legacy_skill_choice.md) | 0/3 | U04、V04、U07 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-056 | included：旧选择56：技能学习 | [legacy_skill_learning](../doc/ui_windows/legacy_skill_learning.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-057 | included：旧选择57：技能遗忘 | [legacy_skill_forgetting](../doc/ui_windows/legacy_skill_forgetting.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-058 | included：旧选择58：宠物技能目录 | [legacy_pet_skill_catalog](../doc/ui_windows/legacy_pet_skill_catalog.md)、[cg2d_pet_catalog_production_spec](../doc/cg2d_pet_catalog_production_spec.md) | 0/7 | U04、V04、U01 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-059 | included：旧选择59：候选宠物 | [legacy_pet_skill_catalog](../doc/ui_windows/legacy_pet_skill_catalog.md)、[cg2d_pet_catalog_production_spec](../doc/cg2d_pet_catalog_production_spec.md) | 0/7 | U04、V04、U01 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-060 | included：旧选择60：宠物技能槽 | [legacy_pet_skill_catalog](../doc/ui_windows/legacy_pet_skill_catalog.md)、[cg2d_pet_catalog_production_spec](../doc/cg2d_pet_catalog_production_spec.md) | 0/7 | U04、V04、U01 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-061 | included：旧选择61：宠物技能学习确认 | [legacy_pet_skill_catalog](../doc/ui_windows/legacy_pet_skill_catalog.md)、[cg2d_pet_catalog_production_spec](../doc/cg2d_pet_catalog_production_spec.md) | 0/7 | U04、V04、U01 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-062 | included：旧选择62：买卖选择 | [legacy_shop_choice](../doc/ui_windows/legacy_shop_choice.md)、[cg2d_shop_choice_body_spec](../doc/cg2d_shop_choice_body_spec.md) | 2/3 | U04、V04、P07、U05 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-063 | included：旧选择63：买入列表 | [legacy_buy_list](../doc/ui_windows/legacy_buy_list.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-064 | included：旧选择64：卖出列表 | [legacy_sell_list](../doc/ui_windows/legacy_sell_list.md) | 0/2 | U04、V04、U05 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-065 | included：旧选择65：买入确认 | [legacy_buy_confirmation](../doc/ui_windows/legacy_buy_confirmation.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-066 | included：旧选择66：卖出确认 | [legacy_sell_confirmation](../doc/ui_windows/legacy_sell_confirmation.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-067 | included：旧选择67：治疗 | [legacy_healing](../doc/ui_windows/legacy_healing.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-068 | included：旧选择68：鉴定列表 | [legacy_identify_list](../doc/ui_windows/legacy_identify_list.md) | 0/2 | U04、V04、U05 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-069 | included：旧选择69：鉴定确认 | [legacy_identify_confirmation](../doc/ui_windows/legacy_identify_confirmation.md) | 0/1 | U04、V04、U05 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-070 | included：旧选择70：修理列表 | [legacy_repair_list](../doc/ui_windows/legacy_repair_list.md) | 0/2 | U04、V04、U05 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-071 | included：旧选择71：修理确认 | [legacy_repair_confirmation](../doc/ui_windows/legacy_repair_confirmation.md) | 0/1 | U04、V04、U05 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-072 | included：旧选择72：存放 | [legacy_storage_items](../doc/ui_windows/legacy_storage_items.md) | 0/11 | U04、V04、U06 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-073 | included：旧选择73：留言 | [legacy_message_board](../doc/ui_windows/legacy_message_board.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-074 | included：旧选择74：交易选择 | [legacy_exchange_dialog](../doc/ui_windows/legacy_exchange_dialog.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-075 | included：旧选择75：交换列表 | [legacy_barter_list](../doc/ui_windows/legacy_barter_list.md) | 0/2 | U04、V04、U06 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-076 | included：旧选择76：交换确认 | [legacy_barter_confirmation](../doc/ui_windows/legacy_barter_confirmation.md) | 0/1 | U04、V04、U06 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-077 | included：旧选择77：物品目标名单 | [legacy_item_target_list](../doc/ui_windows/legacy_item_target_list.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-078 | included：旧选择78：物品目标属性 | [legacy_item_use_status](../doc/ui_windows/legacy_item_use_status.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-079 | included：旧选择79：物品刻印 | [legacy_item_engraving](../doc/ui_windows/legacy_item_engraving.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-080 | included：旧选择80：埋起物品 | [legacy_item_burial](../doc/ui_windows/legacy_item_burial.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-081 | included：旧选择81：固定文字提示 | [legacy_text_notices](../doc/ui_windows/legacy_text_notices.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-082 | included：旧选择82：固定文字提示 | [legacy_text_notices](../doc/ui_windows/legacy_text_notices.md) | 0/2 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-083 | included：旧选择83：交出/接受对话 | [legacy_give_receive_dialog](../doc/ui_windows/legacy_give_receive_dialog.md) | 0/2 | U04、V04、U06 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-084 | included：旧选择84：交出/接受数量 | [legacy_give_receive_quantity](../doc/ui_windows/legacy_give_receive_quantity.md) | 0/3 | U04、V04、U06 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-085 | included：旧选择85：交出/接受确认 | [legacy_give_receive_confirmation](../doc/ui_windows/legacy_give_receive_confirmation.md) | 1/2 | U04、V04、P05、U06 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-086 | included：旧选择86：公会邮件赠品 | [legacy_guild_mail_gift_items](../doc/ui_windows/legacy_guild_mail_gift_items.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-087 | included：旧选择87：公会宠物邮件 | [legacy_guild_pet_mail_compose](../doc/ui_windows/legacy_guild_pet_mail_compose.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-088 | included：旧选择88：公会送信宠物 | [legacy_guild_mail_pet_list](../doc/ui_windows/legacy_guild_mail_pet_list.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-089 | included：旧选择89：公会邮件阅读 | [legacy_guild_mail_read](../doc/ui_windows/legacy_guild_mail_read.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-090 | included：旧选择90：公会普通多人邮件 | [legacy_guild_normal_mail_compose](../doc/ui_windows/legacy_guild_normal_mail_compose.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-091 | included：旧选择91：公会发送模式 | [legacy_guild_delivery_mode](../doc/ui_windows/legacy_guild_delivery_mode.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-092 | included：旧选择92：公会成员删除 | [legacy_guild_contact_delete](../doc/ui_windows/legacy_guild_contact_delete.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-093 | included：旧选择93：公会排序 | [legacy_guild_contact_sort](../doc/ui_windows/legacy_guild_contact_sort.md) | 0/0 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-094 | included：旧选择94：公会通讯录 | [legacy_guild_contact_list](../doc/ui_windows/legacy_guild_contact_list.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-095 | included：旧选择95：公会资料编辑 | [legacy_guild_details_edit](../doc/ui_windows/legacy_guild_details_edit.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-096 | included：旧选择96：公会职级修改 | [legacy_guild_rank_change](../doc/ui_windows/legacy_guild_rank_change.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-098 | included：旧选择98：旧日志 | [legacy_log_viewer](../doc/ui_windows/legacy_log_viewer.md) | 2/3 | U04、V04、P03、P04 | unassessed/unassessed/unassessed/unassessed/blocked |
| UI-L-099 | included：旧选择99：黑名单编辑 | [legacy_blacklist_editor](../doc/ui_windows/legacy_blacklist_editor.md) | 0/1 | U04、V04 | unassessed/unassessed/unassessed/unassessed/blocked |

## 4. 当前推进规则

先从已有合同补原成员和B证据，再逐项推进I/W/S；R保持blocked。优先处理已知P/C/U任务及无合同关联ID的归属，不把历史定位缺失当作未实现。原格式明文、音频暂停、无构建/测试/运行/摘要限制保持。
本看板只汇总结构化登记；变更后需做人工影响分析。静态汇总器退出0不等于软件验收通过。
