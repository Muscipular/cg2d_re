# 证据快照收敛为 SPEC 与 Git（EDS01）

2026-10-11，按用户本旁聊“逐一处理”要求执行。本次处理已识别的80份重复快照记录：GAP01已由主线程完成，其余79份由本旁聊接续。77份已有对应SPEC；MC01-A/B两份单独整理为同名SPEC，保留用户对CRT总SPEC的既有修改。其他仅保存原始独有证据的JSON不在本批删除范围。

## 实施合同

1. 每份先读取原证据与对应SPEC，保留原函数范围、关键合同、实际验证层级和剩余门禁；原字节、完整源码/依赖/台账快照由整理前已提交的Git历史保存，不再复制到Markdown。
2. 每个对应SPEC新增“证据归档”小节，记原根简表、原验证状态及Git归档定位。无明确通过结论的记录不补写pass；不重新核对已通过且未变函数。
3. 活动文档、语义/验收登记及仍保留的证据引用同步改到SPEC归档锚点；历史文件名只作Git溯源标识，不作为现行文件链接。原源文件、函数/处置/成员/任务数量、业务行为和所有验收结果保持。
4. 保留用户既有修改和主线程动画清理准备；双方串行写共享文档。本批只改文档与引用，不改业务源码。
5. 对已存在用户修改的CRT总SPEC只改必要引用；提交时仅暂存这部分引用差异，保留其余用户改动。
6. 完成引用检查、JSON可读性/UTF-8无BOM/LF、登记差异与度量门禁守恒、静态汇总及-Check后精确提交。同一cg2d_re分支、不打tag，不构建/测试/运行/产物检查/摘要计算/外部依赖。

## 逐份清单

GAP01已收敛为[分配释放SPEC](cg2d_graphic_allocation_release_spec.md#graphic-allocation-acceptance)，不重复处理。以下79份均以本次整理提交前的已提交版本作为原始归档；当前依据收敛到目标SPEC。

| 原记录（Git历史） | 当前SPEC |
|---|---|
| `cg2d_battle_attached_effect_evidence.json` | [cg2d_battle_attached_effect_spec.md](cg2d_battle_attached_effect_spec.md#evidence-archive-20261011) |
| `cg2d_battle_equipment_interaction_evidence.json` | [cg2d_battle_equipment_interaction_spec.md](cg2d_battle_equipment_interaction_spec.md#evidence-archive-20261011) |
| `cg2d_battle_factory_wait_evidence.json` | [cg2d_battle_factory_wait_spec.md](cg2d_battle_factory_wait_spec.md#evidence-archive-20261011) |
| `cg2d_battle_field_effect_lifecycle_evidence.json` | [cg2d_battle_field_effect_lifecycle_spec.md](cg2d_battle_field_effect_lifecycle_spec.md#evidence-archive-20261011) |
| `cg2d_battle_inspect_exit_evidence.json` | [cg2d_battle_inspect_exit_spec.md](cg2d_battle_inspect_exit_spec.md#evidence-archive-20261011) |
| `cg2d_battle_item_interaction_evidence.json` | [cg2d_battle_item_interaction_spec.md](cg2d_battle_item_interaction_spec.md#evidence-archive-20261011) |
| `cg2d_battle_knockdown_exit_evidence.json` | [cg2d_battle_knockdown_exit_spec.md](cg2d_battle_knockdown_exit_spec.md#evidence-archive-20261011) |
| `cg2d_character_list_record_production_callees_evidence.json` | [cg2d_character_list_record_production_callees_spec.md](cg2d_character_list_record_production_callees_spec.md#evidence-archive-20261011) |
| `cg2d_chat_focus_wheel_draw_evidence.json` | [cg2d_chat_focus_wheel_draw_spec.md](cg2d_chat_focus_wheel_draw_spec.md#evidence-archive-20261011) |
| `cg2d_chat_history_navigation_evidence.json` | [cg2d_chat_history_navigation_spec.md](cg2d_chat_history_navigation_spec.md#evidence-archive-20261011) |
| `cg2d_chat_log_line_drawing_evidence.json` | [cg2d_chat_log_line_drawing_spec.md](cg2d_chat_log_line_drawing_spec.md#evidence-archive-20261011) |
| `cg2d_chat_send_pipeline_evidence.json` | [cg2d_chat_send_pipeline_spec.md](cg2d_chat_send_pipeline_spec.md#evidence-archive-20261011) |
| `cg2d_chat_window_control_pipeline_evidence.json` | [cg2d_chat_window_control_pipeline_spec.md](cg2d_chat_window_control_pipeline_spec.md#evidence-archive-20261011) |
| `cg2d_chat_window_geometry_production_evidence.json` | [cg2d_chat_window_geometry_production_spec.md](cg2d_chat_window_geometry_production_spec.md#evidence-archive-20261011) |
| `cg2d_drag_session_production_lifecycle_evidence.json` | [cg2d_drag_session_production_lifecycle_spec.md](cg2d_drag_session_production_lifecycle_spec.md#evidence-archive-20261011) |
| `cg2d_equipment_attribute_reply_evidence.json` | [cg2d_equipment_attribute_reply_spec.md](cg2d_equipment_attribute_reply_spec.md#evidence-archive-20261011) |
| `cg2d_font_width_arithmetic_evidence.json` | [cg2d_font_width_arithmetic_spec.md](cg2d_font_width_arithmetic_spec.md#evidence-archive-20261011) |
| `cg2d_game_time_production_lifecycle_evidence.json` | [cg2d_game_time_production_lifecycle_spec.md](cg2d_game_time_production_lifecycle_spec.md#evidence-archive-20261011) |
| `cg2d_guild_mail_lookup_production_evidence.json` | [cg2d_guild_mail_lookup_production_spec.md](cg2d_guild_mail_lookup_production_spec.md#evidence-archive-20261011) |
| `cg2d_guild_mail_reply_persistence_evidence.json` | [cg2d_guild_mail_reply_persistence_spec.md](cg2d_guild_mail_reply_persistence_spec.md#evidence-archive-20261011) |
| `cg2d_hover_text_consumption_evidence.json` | [cg2d_hover_text_consumption_spec.md](cg2d_hover_text_consumption_spec.md#evidence-archive-20261011) |
| `cg2d_input_scroll_draw_evidence.json` | [cg2d_input_scroll_draw_spec.md](cg2d_input_scroll_draw_spec.md#evidence-archive-20261011) |
| `cg2d_item_description_rows_evidence.json` | [cg2d_item_description_rows_spec.md](cg2d_item_description_rows_spec.md#evidence-archive-20261011) |
| `cg2d_item_drag_entry_callback_evidence.json` | [cg2d_item_drag_entry_callback_spec.md](cg2d_item_drag_entry_callback_spec.md#evidence-archive-20261011) |
| `cg2d_item_move_request_evidence.json` | [cg2d_item_move_request_spec.md](cg2d_item_move_request_spec.md#evidence-archive-20261011) |
| `cg2d_item_selection_event_pipeline_evidence.json` | [cg2d_item_selection_event_pipeline_spec.md](cg2d_item_selection_event_pipeline_spec.md#evidence-archive-20261011) |
| `cg2d_legacy_ui_add_arithmetic_evidence.json` | [cg2d_legacy_ui_add_arithmetic_spec.md](cg2d_legacy_ui_add_arithmetic_spec.md#evidence-archive-20261011) |
| `cg2d_map_hit_time_classification_evidence.json` | [cg2d_map_hit_time_classification_spec.md](cg2d_map_hit_time_classification_spec.md#evidence-archive-20261011) |
| `cg2d_map_object_production_sampling_evidence.json` | [cg2d_map_object_production_sampling_spec.md](cg2d_map_object_production_sampling_spec.md#evidence-archive-20261011) |
| `cg2d_map_reply_palette_evidence.json` | [cg2d_map_reply_palette_spec.md](cg2d_map_reply_palette_spec.md#evidence-archive-20261011) |
| `cg2d_money_drop_request_evidence.json` | [cg2d_money_drop_request_spec.md](cg2d_money_drop_request_spec.md#evidence-archive-20261011) |
| `cg2d_msvc_crt_file_evidence.json` | [cg2d_msvc_crt_file_spec.md](cg2d_msvc_crt_file_spec.md#evidence-archive-20261011) |
| `cg2d_msvc_crt_scalar_evidence.json` | [cg2d_msvc_crt_scalar_spec.md](cg2d_msvc_crt_scalar_spec.md#evidence-archive-20261011) |
| `cg2d_neighbor_fim_request_plaintext_evidence.json` | [cg2d_neighbor_fim_request_plaintext_spec.md](cg2d_neighbor_fim_request_plaintext_spec.md#evidence-archive-20261011) |
| `cg2d_numeric_panel_input_evidence.json` | [cg2d_numeric_panel_input_spec.md](cg2d_numeric_panel_input_spec.md#evidence-archive-20261011) |
| `cg2d_palette_capability_selection_evidence.json` | [cg2d_palette_capability_selection_spec.md](cg2d_palette_capability_selection_spec.md#evidence-archive-20261011) |
| `cg2d_path_direction_heading_producers_evidence.json` | [cg2d_path_direction_heading_producers_spec.md](cg2d_path_direction_heading_producers_spec.md#evidence-archive-20261011) |
| `cg2d_player_attribute_money_reply_evidence.json` | [cg2d_player_attribute_money_reply_spec.md](cg2d_player_attribute_money_reply_spec.md#evidence-archive-20261011) |
| `cg2d_player_timer_reply_producer_evidence.json` | [cg2d_player_timer_reply_producer_spec.md](cg2d_player_timer_reply_producer_spec.md#evidence-archive-20261011) |
| `cg2d_quantity_window_callbacks_evidence.json` | [cg2d_quantity_window_callbacks_spec.md](cg2d_quantity_window_callbacks_spec.md#evidence-archive-20261011) |
| `cg2d_quantity_window_creation_evidence.json` | [cg2d_quantity_window_creation_spec.md](cg2d_quantity_window_creation_spec.md#evidence-archive-20261011) |
| `cg2d_rectangle_hit_hover_evidence.json` | [cg2d_rectangle_hit_hover_spec.md](cg2d_rectangle_hit_hover_spec.md#evidence-archive-20261011) |
| `cg2d_selector_attribute_pet_tab_evidence.json` | [cg2d_selector_attribute_pet_tab_spec.md](cg2d_selector_attribute_pet_tab_spec.md#evidence-archive-20261011) |
| `cg2d_selector_button_event_evidence.json` | [cg2d_selector_button_event_spec.md](cg2d_selector_button_event_spec.md#evidence-archive-20261011) |
| `cg2d_selector_construction_exit_evidence.json` | [cg2d_selector_construction_exit_spec.md](cg2d_selector_construction_exit_spec.md#evidence-archive-20261011) |
| `cg2d_selector_frame_binding_evidence.json` | [cg2d_selector_frame_binding_spec.md](cg2d_selector_frame_binding_spec.md#evidence-archive-20261011) |
| `cg2d_selector_local_coordinate_hover_evidence.json` | [cg2d_selector_local_coordinate_hover_spec.md](cg2d_selector_local_coordinate_hover_spec.md#evidence-archive-20261011) |
| `cg2d_selector_page_visibility_evidence.json` | [cg2d_selector_page_visibility_spec.md](cg2d_selector_page_visibility_spec.md#evidence-archive-20261011) |
| `cg2d_selector_pet_preview_evidence.json` | [cg2d_selector_pet_preview_spec.md](cg2d_selector_pet_preview_spec.md#evidence-archive-20261011) |
| `cg2d_status_hint_production_evidence.json` | [cg2d_status_hint_production_spec.md](cg2d_status_hint_production_spec.md#evidence-archive-20261011) |
| `cg2d_storage_amount_exchange_evidence.json` | [cg2d_storage_amount_exchange_spec.md](cg2d_storage_amount_exchange_spec.md#evidence-archive-20261011) |
| `cg2d_storage_external_item_panels_evidence.json` | [cg2d_storage_external_item_panels_spec.md](cg2d_storage_external_item_panels_spec.md#evidence-archive-20261011) |
| `cg2d_storage_group_data_reply_evidence.json` | [cg2d_storage_group_data_reply_spec.md](cg2d_storage_group_data_reply_spec.md#evidence-archive-20261011) |
| `cg2d_storage_opening_actions_evidence.json` | [cg2d_storage_opening_actions_spec.md](cg2d_storage_opening_actions_spec.md#evidence-archive-20261011) |
| `cg2d_storage_preview_dispatch_evidence.json` | [cg2d_storage_preview_dispatch_spec.md](cg2d_storage_preview_dispatch_spec.md#evidence-archive-20261011) |
| `cg2d_storage_reply_dispatch_evidence.json` | [cg2d_storage_reply_dispatch_spec.md](cg2d_storage_reply_dispatch_spec.md#evidence-archive-20261011) |
| `cg2d_text_escape_sampling_evidence.json` | [cg2d_text_escape_sampling_spec.md](cg2d_text_escape_sampling_spec.md#evidence-archive-20261011) |
| `cg2d_text_unescape_sampling_evidence.json` | [cg2d_text_unescape_sampling_spec.md](cg2d_text_unescape_sampling_spec.md#evidence-archive-20261011) |
| `cg2d_timer_ui_consumption_evidence.json` | [cg2d_timer_ui_consumption_spec.md](cg2d_timer_ui_consumption_spec.md#evidence-archive-20261011) |
| `cg2d_tooltip_description_render_evidence.json` | [cg2d_tooltip_description_render_spec.md](cg2d_tooltip_description_render_spec.md#evidence-archive-20261011) |
| `cg2d_trade_quantity_panel_evidence.json` | [cg2d_trade_quantity_panel_spec.md](cg2d_trade_quantity_panel_spec.md#evidence-archive-20261011) |
| `cg2d_window_close_front_release_evidence.json` | [cg2d_window_close_front_release_spec.md](cg2d_window_close_front_release_spec.md#evidence-archive-20261011) |
| `cg2d_window_compact_close_evidence.json` | [cg2d_window_compact_close_spec.md](cg2d_window_compact_close_spec.md#evidence-archive-20261011) |
| `cg2d_window_drag_focus_evidence.json` | [cg2d_window_drag_focus_spec.md](cg2d_window_drag_focus_spec.md#evidence-archive-20261011) |
| `cg2d_window_element_draw_pipeline_evidence.json` | [cg2d_window_element_draw_pipeline_spec.md](cg2d_window_element_draw_pipeline_spec.md#evidence-archive-20261011) |
| `cg2d_window_input_context_dispatch_evidence.json` | [cg2d_window_input_context_dispatch_spec.md](cg2d_window_input_context_dispatch_spec.md#evidence-archive-20261011) |
| `cg2d_window_number_draw_evidence.json` | [cg2d_window_number_draw_spec.md](cg2d_window_number_draw_spec.md#evidence-archive-20261011) |
| `cg2d_window_payload_creation_evidence.json` | [cg2d_window_payload_creation_spec.md](cg2d_window_payload_creation_spec.md#evidence-archive-20261011) |
| `cg2d_window_scroll_callback_evidence.json` | [cg2d_window_scroll_callback_spec.md](cg2d_window_scroll_callback_spec.md#evidence-archive-20261011) |
| `cg2d_window_scroll_position_evidence.json` | [cg2d_window_scroll_position_spec.md](cg2d_window_scroll_position_spec.md#evidence-archive-20261011) |
| `cg2d_window_wheel_scroll_evidence.json` | [cg2d_window_wheel_scroll_spec.md](cg2d_window_wheel_scroll_spec.md#evidence-archive-20261011) |
| `cg2d_world_direction_normalization_sampling_evidence.json` | [cg2d_world_direction_normalization_sampling_spec.md](cg2d_world_direction_normalization_sampling_spec.md#evidence-archive-20261011) |
| `cg2d_world_object_bulk_restore_evidence.json` | [cg2d_world_object_bulk_restore_spec.md](cg2d_world_object_bulk_restore_spec.md#evidence-archive-20261011) |
| `cg2d_world_object_grid_query_evidence.json` | [cg2d_world_object_grid_query_spec.md](cg2d_world_object_grid_query_spec.md#evidence-archive-20261011) |
| `cg2d_world_object_lifecycle_evidence.json` | [cg2d_world_object_lifecycle_spec.md](cg2d_world_object_lifecycle_spec.md#evidence-archive-20261011) |
| `cg2d_world_object_lookup_allocation_evidence.json` | [cg2d_world_object_lookup_allocation_spec.md](cg2d_world_object_lookup_allocation_spec.md#evidence-archive-20261011) |
| `cg2d_world_object_record_producers_evidence.json` | [cg2d_world_object_record_producers_spec.md](cg2d_world_object_record_producers_spec.md#evidence-archive-20261011) |
| `cg2d_yobi_frame_playback_sampling_evidence.json` | [cg2d_yobi_frame_playback_sampling_spec.md](cg2d_yobi_frame_playback_sampling_spec.md#evidence-archive-20261011) |
| `cg2d_yobi_graphic_offset_sampling_evidence.json` | [cg2d_yobi_graphic_offset_sampling_spec.md](cg2d_yobi_graphic_offset_sampling_spec.md#evidence-archive-20261011) |

## 验证状态与剩余

79份均完成逐份来源/合同与旧验证层级归档、引用迁移和UTF-8/JSON检查；静态汇总及-Check均终态退出0，派生引用及全部数量/任务/门禁守恒核对通过。业务源码及现有通过/未通过结论保持，不将文档整理计作新增功能验收。完整生命周期、ABI/SDK/资源内容与运行仍按原SPEC逐项推进。
