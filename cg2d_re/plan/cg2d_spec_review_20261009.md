# cg2d_re 当前 SPEC 复盘（2026-10-09）

<a id="current-spec-review"></a>
## 范围、证据与当前限制

本次按用户要求复盘总基线、子合同、看板与登记簿。下列逐文件索引覆盖 doc/plan 中380份 *_spec.md 的标题、章节和后续说明；另定点读取总计划、60项收尾表、唯一进度末尾、UI总览与黑名单合同。索引及合同边界复盘不等于380份原指令/全部源码重新验收，局部历史通过只属于原观察范围。

用户本次确认“此前故障已好，继续整体缺口”。不再重复改动已恢复的连接流程；此前CAC04选人入场反馈保留，当前确认未提供新的版本、环境或完整轨迹，不新增整项R通过。

本次重申不构建、不测试、不运行、不计算摘要、单线程。登记簿 no_build 恢复 true；旧构建例外与结果保留历史，不作为本轮授权。仅按既有追踪SPEC执行文档/JSON静态汇总，运行门禁等待明确恢复。用户既有基线、根进度、配置和删除文件均不改动、不夹带提交。

## 有效边界与真实缺口

| 板块 | 当前有效合同 | 仍需完成 |
|---|---|---|
| R0/R1/R7 | 当前已打开cg2d IDA为基线，原3680函数/218导入/3061区间身份保留 | 完整处置、功能/数据/间接边分母和未知差集；不能把有文档或定位当基线已验 |
| 启动/CRT | D02/D03受限宿主承接，应用按原表相对序，一owner一活动构析 | 全部业务可见状态、owner/alias、正常/快速/可调用退出及完整提供者验收 |
| 网络/登录 | 本机服务端既有原命令/字段/LF明文；两加解密叶排除 | 全108业务、失败/重连/断线/生命周期和适用互通对照；局部已恢复，不再造修复 |
| 世界/UI/战斗 | 真实状态分发、生产消费和原调用/字段宽度；原间接表不猜用途 | 旧99外层、CA0478/CB1D1C等容量/写者、各业务完整生产及owner正常寿命 |
| 渲染/资源/动画 | SDL3自动driver、CPU Back/Temp、32位渲染，8位资源/调色板保留；已证指针才拓宽 | 全模式、非空源/别名、池与缓存装载/回收/重建/退出、上层数值和像素对照 |
| 文本/输入 | 已确认静态文字GBK、宋体、ASCII光标；原输入/协议/资源字节边界保持 | 各来源容量/NUL/字节索引/像素及焦点、IME、字体、正常寿命完整接线 |
| 音频 | SDL3/WAV、第二段空音频，旧7/8开发已恢复；原上层时序保留 | 全成员、旧外层、资源/设备/失败/通知/控制/声音及正常寿命；不恢复运行时MIDI |
| R8 | 总SPEC§10及B/I/W/S/R独立门禁，批准差异/排除分别报告 | V01–V06全部适用证据及最终报告；未闭条件保留，禁止局部数量代整体完成 |

读前看板：语义1906条/1896个原ID，处置1087/3680，未归属区间已审485/3061，184叶功能整项验收0，60已知任务关闭11，六类closure均open。没有据此估算总体完成率。

开发差集104项由103项具名排除回执和1项NOP元数据组成，需范围/定位核定为0；这是登记定位差集，不证明源码/数据/功能缺口为0。不能再从该队列按缺文件实现假体。下一步沿真实正常链补源差异、成员与原新映射，再集中补全适用验收。

## 发现与处理

1. 登记簿 no_build=false仍承载旧构建许可，与用户本次重申禁令冲突；本步改为true并明确历史授权不延续。runtime_allowed=false/no_tests/no_hash和全部门禁、成员、处置、构建历史保持。
2. UI总览和收尾数量表仍有音频“暂停”、旧八项链接缺口、1144语义/23格式余项等历史摘要；后续CAB/CAC与看板已有更新，本步明确按局部后续和机器实值解释，不重做已完成本体，不重写原历史证据。
3. 旧99目录/路由的原事实仍缺，不据静态地址表猜造生产者或轮询。黑名单已有实际加载/持久化及UI/聊天读写，但七个核心原ID尚无语义登记或处置；已取得七完整原体及18入向邻窗，下一小步核对正常比较/复制取样并补原新关系。

本报告不新增I/W/S/R通过、不关闭R04或全量closure。实际实现和验收结果仍只写唯一进度及登记簿。

## 逐 SPEC 索引

下表的“末节”仅标明本次逐份读取时最后一个二级章节；章节存在不是验收证明，当前状态须结合该合同后续增量、登记证据和开放门禁。

| SPEC | 标题 | 末节 |
|---|---|---|
| [doc/cg2d_active_initializer_order_spec.md](../doc/cg2d_active_initializer_order_spec.md) | 当前活动初始化的原顺序接线 SPEC | SSO01 SDK 预先提供角色与源投影分列 |
| [doc/cg2d_actor_attachment_sampling_spec.md](../doc/cg2d_actor_attachment_sampling_spec.md) | AAS01 实体附件槽位取样 SPEC | 源状态与剩余 |
| [doc/cg2d_actor_chat_consumer_sampling_spec.md](../doc/cg2d_actor_chat_consumer_sampling_spec.md) | 聊天气泡消费与释放取样 SPEC（BTJ01） | 实施状态 |
| [doc/cg2d_actor_chat_text_sampling_spec.md](../doc/cg2d_actor_chat_text_sampling_spec.md) | 头顶聊天文本标记与取样 SPEC（BTI01） | 实施状态 |
| [doc/cg2d_actor_coordinate_producer_spec.md](../doc/cg2d_actor_coordinate_producer_spec.md) | ACP01 实体与挂件坐标生产 SPEC | 源状态与剩余 |
| [doc/cg2d_actor_flag_leaf_returns_spec.md](../doc/cg2d_actor_flag_leaf_returns_spec.md) | 非玩家状态叶真实BYTE与标志地址返回 SPEC（BTO01） | 本步完成结果（2026-10-07） |
| [doc/cg2d_actor_follow_target_sampling_spec.md](../doc/cg2d_actor_follow_target_sampling_spec.md) | 跟随目标坐标取样 SPEC（BTS01） | 本步静态终态（2026-10-07） |
| [doc/cg2d_actor_motion_update_sampling_spec.md](../doc/cg2d_actor_motion_update_sampling_spec.md) | 运动位置临时字段取样 SPEC（BTV01） | 当前实现与证据状态 |
| [doc/cg2d_actor_motion_variants_sampling_spec.md](../doc/cg2d_actor_motion_variants_sampling_spec.md) | 普通与倍率移动取样 SPEC（BTW01） | 当前实现与证据状态 |
| [doc/cg2d_actor_movement_flag_sampling_spec.md](../doc/cg2d_actor_movement_flag_sampling_spec.md) | 角色移动标志真实取样与返回 SPEC（BTP01） | 元数据描述更正（BTQ01，2026-10-07） |
| [doc/cg2d_actor_path_append_sampling_spec.md](../doc/cg2d_actor_path_append_sampling_spec.md) | 路径追加与坐标复位取样 SPEC（BTT01） | 本步静态终态（2026-10-07） |
| [doc/cg2d_actor_phase_effect_creation_spec.md](../doc/cg2d_actor_phase_effect_creation_spec.md) | 骑乘效果对象创建指针 SPEC（BTX01） | 当前实现与证据状态 |
| [doc/cg2d_actor_reply_consumer_sampling_spec.md](../doc/cg2d_actor_reply_consumer_sampling_spec.md) | NI角色回复与非玩家动作消费取样 SPEC（BTN01） | 本步完成结果（2026-10-07） |
| [doc/cg2d_actor_update_byte_reads_spec.md](../doc/cg2d_actor_update_byte_reads_spec.md) | 主角色更新 BYTE 取样 SPEC（BTY01） | 当前实现与证据状态 |
| [doc/cg2d_actor_velocity_coordinate_sampling_spec.md](../doc/cg2d_actor_velocity_coordinate_sampling_spec.md) | 速度入口坐标三字段取样 SPEC（BTU01） | 本步静态终态（2026-10-07） |
| [doc/cg2d_album_preview_lifecycle_baseline_spec.md](../doc/cg2d_album_preview_lifecycle_baseline_spec.md) | 图鉴预览生产与评级生命周期基线SPEC（BVY01） | 状态 |
| [doc/cg2d_album_preview_sampling_spec.md](../doc/cg2d_album_preview_sampling_spec.md) | 图鉴详情预览取样SPEC（BVX01） | 状态 |
| [doc/cg2d_animation_capacity_spec.md](../doc/cg2d_animation_capacity_spec.md) | ACB01 动画主表分配预算与查询身份 | 当前交付 |
| [doc/cg2d_animation_pointer_spec.md](../doc/cg2d_animation_pointer_spec.md) | ATP01 动画载荷具名指针与清理身份 | 当前交付 |
| [doc/cg2d_append_transform_spec.md](../doc/cg2d_append_transform_spec.md) | 网络文本追加与命令变换 SPEC（NAT01） | 源码小步收口 |
| [doc/cg2d_array_counter_spec.md](../doc/cg2d_array_counter_spec.md) | 数组析构剩余计数 SPEC（ACI01） | 源码小步收口 |
| [doc/cg2d_audio_control_paused_scope_spec.md](../doc/cg2d_audio_control_paused_scope_spec.md) | BOY01 音频控制子入口的暂停范围纠错 | 交付与后续 |
| [doc/cg2d_audio_paused_identity_spec.md](../doc/cg2d_audio_paused_identity_spec.md) | APS01 暂停音频的原身份范围关联 | BOY01 旧7/旧8控制子入口归属纠错（2026-10-06） |
| [doc/cg2d_authentication_failure_sampling_spec.md](../doc/cg2d_authentication_failure_sampling_spec.md) | 认证失败提示取样子SPEC（BSN01） | 实施追踪 |
| [doc/cg2d_automap_palette_color_production_spec.md](../doc/cg2d_automap_palette_color_production_spec.md) | BOP01 自动地图调色板取色生产 SPEC | 当前状态与后续开发 |
| [doc/cg2d_back_temp_full_source_snapshot_spec.md](../doc/cg2d_back_temp_full_source_snapshot_spec.md) | Back→Temp整源复制与转场准备返回 SPEC | 收口 |
| [doc/cg2d_battle_action_114_spec.md](../doc/cg2d_battle_action_114_spec.md) | 战斗动作114完整正常本体 SPEC（BPR01） | 当前交付 |
| [doc/cg2d_battle_action_141_143_spec.md](../doc/cg2d_battle_action_141_143_spec.md) | 战斗动作141/143完整正常本体 SPEC（BPQ01） | 当前交付 |
| [doc/cg2d_battle_attachment_alpha_background_spec.md](../doc/cg2d_battle_attachment_alpha_background_spec.md) | 战斗附件透明度、调色板与背景状态 SPEC（BPW01） | 当前交付 |
| [doc/cg2d_battle_attachment_producer_spec.md](../doc/cg2d_battle_attachment_producer_spec.md) | BAP01 战斗附件生产、槽位发布与清理 SPEC | 源状态与剩余 |
| [doc/cg2d_battle_attachment_recovery_spec.md](../doc/cg2d_battle_attachment_recovery_spec.md) | BAR01 战斗退场清槽与装备/状态恢复取样 SPEC | 源状态与剩余 |
| [doc/cg2d_battle_attachment_sampling_spec.md](../doc/cg2d_battle_attachment_sampling_spec.md) | BAS01 三战斗附件同步的字节门禁与取样 SPEC | 源状态与剩余 |
| [doc/cg2d_battle_attack_targets_spec.md](../doc/cg2d_battle_attack_targets_spec.md) | 战斗攻击目标生产实施子 SPEC | 6. BNK01 三父参数取样桥修正（2026-10-06） |
| [doc/cg2d_battle_bounce_reflection_sampling_spec.md](../doc/cg2d_battle_bounce_reflection_sampling_spec.md) | 战斗转段、反弹与反伤取样SPEC（BQD01） | 当前交付 |
| [doc/cg2d_battle_button_reopen_click_sampling_spec.md](../doc/cg2d_battle_button_reopen_click_sampling_spec.md) | BQL01 战斗按钮再操作点击取样SPEC | 当前交付 |
| [doc/cg2d_battle_clone_restore_sampling_spec.md](../doc/cg2d_battle_clone_restore_sampling_spec.md) | 战斗克隆/图像还原取样SPEC（BPZ01） | 当前交付 |
| [doc/cg2d_battle_command_init_packet_producer_sampling_spec.md](../doc/cg2d_battle_command_init_packet_producer_sampling_spec.md) | BQW01 战斗命令初始化、结束位与封包生产SPEC | 本步交付 |
| [doc/cg2d_battle_command_state_dispatch_sampling_spec.md](../doc/cg2d_battle_command_state_dispatch_sampling_spec.md) | BQU01 战斗命令状态分发、倒计时与目标提示SPEC | 本步交付 |
| [doc/cg2d_battle_command_target_width_sampling_spec.md](../doc/cg2d_battle_command_target_width_sampling_spec.md) | BQI01 战斗命令目标读取宽度SPEC | 当前交付 |
| [doc/cg2d_battle_completion_scale_sampling_spec.md](../doc/cg2d_battle_completion_scale_sampling_spec.md) | BOA01 完成通知、父关联与缩放回调 | 当前收尾 |
| [doc/cg2d_battle_copy_sampling_spec.md](../doc/cg2d_battle_copy_sampling_spec.md) | BCS01 战斗克隆字组复制与创建/残影取样 SPEC | 源状态与剩余 |
| [doc/cg2d_battle_count_timer_flags_spec.md](../doc/cg2d_battle_count_timer_flags_spec.md) | 战斗计数、玩家计时与字节状态发布 SPEC（BPV01） | 当前交付 |
| [doc/cg2d_battle_effect_callbacks_sampling_spec.md](../doc/cg2d_battle_effect_callbacks_sampling_spec.md) | BNW01 两特效回调与三个关联创建入口 | 当前源码与推进 |
| [doc/cg2d_battle_effect_creation_sampling_spec.md](../doc/cg2d_battle_effect_creation_sampling_spec.md) | BNV01 位置与随机特效创建取样 | 源码与收尾 |
| [doc/cg2d_battle_effect_factory_sampling_spec.md](../doc/cg2d_battle_effect_factory_sampling_spec.md) | BNZ01 战斗特效共同创建入口 | 当前推进 |
| [doc/cg2d_battle_effect_pair_distance_sampling_spec.md](../doc/cg2d_battle_effect_pair_distance_sampling_spec.md) | 战斗成对效果/距离状态取样SPEC（BQA02） | 当前交付 |
| [doc/cg2d_battle_entry_result_producer_sampling_spec.md](../doc/cg2d_battle_entry_result_producer_sampling_spec.md) | BQX01 战斗开始/退出、战果生产与W16消息叶SPEC | 本步交付 |
| [doc/cg2d_battle_event_execution_gates_spec.md](../doc/cg2d_battle_event_execution_gates_spec.md) | BMG01 战斗事件入口、失活耗尽与倒计时条件 | 当前状态与后续 |
| [doc/cg2d_battle_event_finish_scene_lifecycle_sampling_spec.md](../doc/cg2d_battle_event_finish_scene_lifecycle_sampling_spec.md) | BMF01 战斗事件完成与场景退出取样 | 当前状态与后续 |
| [doc/cg2d_battle_event_production_sampling_spec.md](../doc/cg2d_battle_event_production_sampling_spec.md) | 战斗场景/事件工厂与动作推进取样SPEC（BEV01） | 当前源状态与剩余 |
| [doc/cg2d_battle_factory_batch_exit_sampling_spec.md](../doc/cg2d_battle_factory_batch_exit_sampling_spec.md) | BQG01 战斗工厂、批次显示与退出取样SPEC | 当前交付 |
| [doc/cg2d_battle_flags_aux_icon_consumer_spec.md](../doc/cg2d_battle_flags_aux_icon_consumer_spec.md) | BOH01 战斗标志、附加记录与浮动图标消费 SPEC | 5. 状态与后续 |
| [doc/cg2d_battle_frame_event_callback_sampling_spec.md](../doc/cg2d_battle_frame_event_callback_sampling_spec.md) | BOD01 帧事件消费与动画消退回调 | 当前收尾 |
| [doc/cg2d_battle_homing_impact_callback_sampling_spec.md](../doc/cg2d_battle_homing_impact_callback_sampling_spec.md) | BOF01 追踪命中与出界运动尾回调 | 当前收尾 |
| [doc/cg2d_battle_impact_callbacks_sampling_spec.md](../doc/cg2d_battle_impact_callbacks_sampling_spec.md) | BOC01 命中与命中音效更新回调 | 当前收尾 |
| [doc/cg2d_battle_initializers_sampling_spec.md](../doc/cg2d_battle_initializers_sampling_spec.md) | BOB01 五个战斗特效初始化助手 | 当前收尾 |
| [doc/cg2d_battle_m_attack_nonzero_sampling_spec.md](../doc/cg2d_battle_m_attack_nonzero_sampling_spec.md) | 普通攻击非零模式取样SPEC（BNL01） | 源码与剩余 |
| [doc/cg2d_battle_m_attack_targets_call_sampling_spec.md](../doc/cg2d_battle_m_attack_targets_call_sampling_spec.md) | 战斗M公共攻击三父调用取样SPEC（BNK01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_data_sampling_spec.md](../doc/cg2d_battle_m_data_sampling_spec.md) | BMH01 战斗六类数据消息取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_dispatch_sampling_spec.md](../doc/cg2d_battle_m_dispatch_sampling_spec.md) | BMD01 战斗 M 分派、参与计数与特效生产 | 当前状态与后续 |
| [doc/cg2d_battle_m_event_dispatch_sampling_spec.md](../doc/cg2d_battle_m_event_dispatch_sampling_spec.md) | BME01 战斗 M 冲突判定、事件调度与初始化 | 当前状态与后续 |
| [doc/cg2d_battle_m_item_combo_sampling_spec.md](../doc/cg2d_battle_m_item_combo_sampling_spec.md) | BMM01 战斗道具与连击消息取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_position_monster_sampling_spec.md](../doc/cg2d_battle_m_position_monster_sampling_spec.md) | BMK01 战斗位置交换与怪物消息取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_read_sampling_spec.md](../doc/cg2d_battle_m_read_sampling_spec.md) | BMR01 战斗 M 数据读取与游标取样 | 实际状态与下一步 |
| [doc/cg2d_battle_m_ride_gmn_sampling_spec.md](../doc/cg2d_battle_m_ride_gmn_sampling_spec.md) | BML01 战斗骑乘与群体消息取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_scan_sampling_spec.md](../doc/cg2d_battle_m_scan_sampling_spec.md) | BMS01 战斗 M 预扫描与对象/预览读取 | 当前状态与下一步 |
| [doc/cg2d_battle_m_skill106_family_sampling_spec.md](../doc/cg2d_battle_m_skill106_family_sampling_spec.md) | 战斗M技能106族取样SPEC（BNJ01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill107_sampling_spec.md](../doc/cg2d_battle_m_skill107_sampling_spec.md) | 战斗M技能107取样SPEC（BNC01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill108_sampling_spec.md](../doc/cg2d_battle_m_skill108_sampling_spec.md) | 战斗M技能108取样SPEC（BND01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill109_sampling_spec.md](../doc/cg2d_battle_m_skill109_sampling_spec.md) | 战斗M技能109取样SPEC（BNE01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill112_sampling_spec.md](../doc/cg2d_battle_m_skill112_sampling_spec.md) | 战斗M技能112取样SPEC（BNF01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill114_sampling_spec.md](../doc/cg2d_battle_m_skill114_sampling_spec.md) | 战斗M技能114取样SPEC（BNG01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill117_sampling_spec.md](../doc/cg2d_battle_m_skill117_sampling_spec.md) | 战斗M技能117取样SPEC（BNH01） | BNH01 路由元数据修正 |
| [doc/cg2d_battle_m_skill12_82_97_sampling_spec.md](../doc/cg2d_battle_m_skill12_82_97_sampling_spec.md) | BMT01 战斗技能12/82/97与共用目标循环取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill19_family_sampling_spec.md](../doc/cg2d_battle_m_skill19_family_sampling_spec.md) | BNR01 技能19族累计与反弹取样 | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill2028_2005_sampling_spec.md](../doc/cg2d_battle_m_skill2028_2005_sampling_spec.md) | BNP01 技能2028累计反馈与2005/4003攻击取样 | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill263_400_401_sampling_spec.md](../doc/cg2d_battle_m_skill263_400_401_sampling_spec.md) | BNM01 技能263、400、401角色、图号与效果取样 | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill267_sampling_spec.md](../doc/cg2d_battle_m_skill267_sampling_spec.md) | BNS01 技能267累计反馈取样 | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill32_family_sampling_spec.md](../doc/cg2d_battle_m_skill32_family_sampling_spec.md) | BNQ01 技能32族状态生产取样 | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill402_405_sampling_spec.md](../doc/cg2d_battle_m_skill402_405_sampling_spec.md) | BNN01 技能402、405位置、图号与效果取样 | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill404_family_sampling_spec.md](../doc/cg2d_battle_m_skill404_family_sampling_spec.md) | BNO01 技能404/406共用主体取样与骑乘生产 | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill4_sampling_spec.md](../doc/cg2d_battle_m_skill4_sampling_spec.md) | BMO01 战斗技能4取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill50_sampling_spec.md](../doc/cg2d_battle_m_skill50_sampling_spec.md) | BMQ01 战斗技能50/51/52/53/88共用主体取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill61_sampling_spec.md](../doc/cg2d_battle_m_skill61_sampling_spec.md) | BMP01 战斗技能61/62/63/83共用主体取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill73_sampling_spec.md](../doc/cg2d_battle_m_skill73_sampling_spec.md) | 战斗M技能73非零模式取样SPEC（BNI01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill93_sampling_spec.md](../doc/cg2d_battle_m_skill93_sampling_spec.md) | BMU01 战斗技能93角色重建取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill98_mode12_13_14_sampling_spec.md](../doc/cg2d_battle_m_skill98_mode12_13_14_sampling_spec.md) | BMY01 战斗技能98 mode12/13/14选中角色与双位置取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill98_mode15_16_sampling_spec.md](../doc/cg2d_battle_m_skill98_mode15_16_sampling_spec.md) | BMZ01 战斗技能98 mode15/16伤害目标取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill98_mode17_status_sampling_spec.md](../doc/cg2d_battle_m_skill98_mode17_status_sampling_spec.md) | BNA01 战斗技能98 mode17、状态目标与默认分派 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill98_mode6_sampling_spec.md](../doc/cg2d_battle_m_skill98_mode6_sampling_spec.md) | BMW01 战斗技能98 mode6目标取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill98_mode7_8_sampling_spec.md](../doc/cg2d_battle_m_skill98_mode7_8_sampling_spec.md) | BMX01 战斗技能98 mode7/8换位与重置取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill98_prepare_sampling_spec.md](../doc/cg2d_battle_m_skill98_prepare_sampling_spec.md) | BMV01 战斗技能98入口准备取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_skill99_family_sampling_spec.md](../doc/cg2d_battle_m_skill99_family_sampling_spec.md) | 战斗M技能99族状态取样SPEC（BNB01） | 源码、进度与剩余 |
| [doc/cg2d_battle_m_skill_entry_waiting_sampling_spec.md](../doc/cg2d_battle_m_skill_entry_waiting_sampling_spec.md) | BMN01 战斗技能入口与等待态取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_status_sampling_spec.md](../doc/cg2d_battle_m_status_sampling_spec.md) | BMJ01 战斗装备与状态消息取样 | 当前状态与后续 |
| [doc/cg2d_battle_m_transition_sampling_spec.md](../doc/cg2d_battle_m_transition_sampling_spec.md) | BMI01 战斗转场与宠物消息取样 | 当前状态与后续 |
| [doc/cg2d_battle_motion_angle_dependency_spec.md](../doc/cg2d_battle_motion_angle_dependency_spec.md) | BOG01 战斗距离、转向与原表角度依赖 SPEC | 5. 实施状态与接续 |
| [doc/cg2d_battle_motion_scale_lifecycle_spec.md](../doc/cg2d_battle_motion_scale_lifecycle_spec.md) | 战斗运动与缩放效果生命周期 SPEC（BPS01） | 当前交付 |
| [doc/cg2d_battle_moving_impact_callback_sampling_spec.md](../doc/cg2d_battle_moving_impact_callback_sampling_spec.md) | BOE01 移动命中与运动尾回调 | 当前收尾 |
| [doc/cg2d_battle_number_production_update_spec.md](../doc/cg2d_battle_number_production_update_spec.md) | BOI01 战斗数字创建与更新 SPEC | BOJ01 对本SPEC测宽接线与旧I结论的更正（2026-10-06） |
| [doc/cg2d_battle_participant_turn_name_sampling_spec.md](../doc/cg2d_battle_participant_turn_name_sampling_spec.md) | BQH01 战斗参战遍历、朝向与名字取样SPEC | 当前交付 |
| [doc/cg2d_battle_pet_command_button_sampling_spec.md](../doc/cg2d_battle_pet_command_button_sampling_spec.md) | BQT01 宠物命令按钮、普通与骑乘回调SPEC | BQU01当前名称与剩余更新（2026-10-07） |
| [doc/cg2d_battle_pet_rider_sampling_spec.md](../doc/cg2d_battle_pet_rider_sampling_spec.md) | 战斗宠物快照与骑乘取样SPEC（BQB01） | 当前交付 |
| [doc/cg2d_battle_pet_skill_selection_sampling_spec.md](../doc/cg2d_battle_pet_skill_selection_sampling_spec.md) | BQS01 宠物技能选择消费与费用行取样SPEC | 当前交付 |
| [doc/cg2d_battle_queue_readiness_sampling_spec.md](../doc/cg2d_battle_queue_readiness_sampling_spec.md) | 战斗参与者初始化、追加与就绪恢复取样SPEC（BQR01） | 当前源状态与剩余 |
| [doc/cg2d_battle_rebirth_transition_spec.md](../doc/cg2d_battle_rebirth_transition_spec.md) | 战斗图像切换、淡入淡出与恢复 SPEC（BPU01） | 当前交付 |
| [doc/cg2d_battle_redirect_aux_baseline_spec.md](../doc/cg2d_battle_redirect_aux_baseline_spec.md) | BNU01 三方转向与辅助效果独立基线 | 源码、进度与剩余 |
| [doc/cg2d_battle_reset_append_baseline_spec.md](../doc/cg2d_battle_reset_append_baseline_spec.md) | 战斗重置与动作/效果追加基线SPEC（BQA01） | 当前源状态与剩余 |
| [doc/cg2d_battle_resource_copy_spec.md](../doc/cg2d_battle_resource_copy_spec.md) | 战斗资源变化临时载荷取样 SPEC（BPT01） | 当前交付 |
| [doc/cg2d_battle_ride_animation_sampling_spec.md](../doc/cg2d_battle_ride_animation_sampling_spec.md) | 战斗动作227骑乘退场与动画复制取样SPEC（BRB01） | 当前源状态与剩余 |
| [doc/cg2d_battle_ride_exit_sampling_spec.md](../doc/cg2d_battle_ride_exit_sampling_spec.md) | 战斗动作226退场、位置与角色恢复取样SPEC（BRA01） | 源状态与剩余 |
| [doc/cg2d_battle_ride_transition_sampling_spec.md](../doc/cg2d_battle_ride_transition_sampling_spec.md) | 战斗动作224骑乘切换、运动与恢复取样SPEC（BRC01） | 当前源状态及剩余 |
| [doc/cg2d_battle_rising_emitter_fade_sampling_spec.md](../doc/cg2d_battle_rising_emitter_fade_sampling_spec.md) | BNX01 上升特效、屏幕发射与背景五态 | 当前推进 |
| [doc/cg2d_battle_rising_emitters_sampling_spec.md](../doc/cg2d_battle_rising_emitters_sampling_spec.md) | BNY01 上升粒子的两个上游发射回调 | 当前收尾 |
| [doc/cg2d_battle_scene_words_death_sampling_spec.md](../doc/cg2d_battle_scene_words_death_sampling_spec.md) | 战斗场景分字/死亡状态与取样SPEC（BPY01） | 当前交付 |
| [doc/cg2d_battle_selection_cleanup_entry_sampling_spec.md](../doc/cg2d_battle_selection_cleanup_entry_sampling_spec.md) | BQJ01 战斗选择清理与命令入口取样SPEC | 当前交付 |
| [doc/cg2d_battle_shared_cast_arc_sampling_spec.md](../doc/cg2d_battle_shared_cast_arc_sampling_spec.md) | 战斗共用施法与弧射取样SPEC（BQE01） | 当前交付 |
| [doc/cg2d_battle_shortcut_defense_state_sampling_spec.md](../doc/cg2d_battle_shortcut_defense_state_sampling_spec.md) | BQK01 战斗快捷入口与防御状态取样SPEC | 当前交付 |
| [doc/cg2d_battle_skill_display_motion_sampling_spec.md](../doc/cg2d_battle_skill_display_motion_sampling_spec.md) | 战斗技能显示与移动参数取样SPEC（BQF01） | 当前交付 |
| [doc/cg2d_battle_skill_lists_rollback_sampling_spec.md](../doc/cg2d_battle_skill_lists_rollback_sampling_spec.md) | BQP01 技能列表重建与制造回退取样 SPEC | 当前交付 |
| [doc/cg2d_battle_skill_state_producer_sampling_spec.md](../doc/cg2d_battle_skill_state_producer_sampling_spec.md) | BQR02 技能引用计数、选择与动作状态生产SPEC | 当前交付 |
| [doc/cg2d_battle_skill_targets_core_spec.md](../doc/cg2d_battle_skill_targets_core_spec.md) | BNT01 十参技能目标生产与独立身份 | 源码、进度与剩余 |
| [doc/cg2d_battle_skill_window_click_sampling_spec.md](../doc/cg2d_battle_skill_window_click_sampling_spec.md) | BQM01 战斗技能窗口与列表取样 SPEC | 当前交付 |
| [doc/cg2d_battle_skill_window_lifecycle_sampling_spec.md](../doc/cg2d_battle_skill_window_lifecycle_sampling_spec.md) | BQO01 战斗技能窗口收尾与初始化取样 SPEC | 当前交付 |
| [doc/cg2d_battle_tail_sampling_spec.md](../doc/cg2d_battle_tail_sampling_spec.md) | BTS01 战斗主回调公共尾与附件消费取样 SPEC | BQX01 战斗入口、战果及W16消息叶（2026-10-07） |
| [doc/cg2d_battle_target_hover_sampling_spec.md](../doc/cg2d_battle_target_hover_sampling_spec.md) | BQV01 战斗目标选择、高亮与两数据叶SPEC | 本步交付 |
| [doc/cg2d_battle_technique_selection_sampling_spec.md](../doc/cg2d_battle_technique_selection_sampling_spec.md) | BQN01 战斗技术选择取样与命令回调 SPEC | 当前交付 |
| [doc/cg2d_battle_time_default_measure_spec.md](../doc/cg2d_battle_time_default_measure_spec.md) | BOJ01 时间数字与默认测宽接线 SPEC | 5. 当前状态与接续 |
| [doc/cg2d_battle_timing_coordinate_damage_sampling_spec.md](../doc/cg2d_battle_timing_coordinate_damage_sampling_spec.md) | 战斗计时、坐标与恢复扣减取样SPEC（BQC01） | 当前交付 |
| [doc/cg2d_battle_transition_effect_sampling_spec.md](../doc/cg2d_battle_transition_effect_sampling_spec.md) | 战斗转场/目标坐标与209取样SPEC（BPX01） | 当前交付 |
| [doc/cg2d_battle_window_release_range_sampling_spec.md](../doc/cg2d_battle_window_release_range_sampling_spec.md) | BQQ01 技能与制造窗口关闭区间取样SPEC | 当前交付边界 |
| [doc/cg2d_boundary_merged_identity_spec.md](../doc/cg2d_boundary_merged_identity_spec.md) | 页面前缀与分配失败返回的身份归属 SPEC（BMR01） | 当前边界 |
| [doc/cg2d_build_acceptance_spec.md](../doc/cg2d_build_acceptance_spec.md) | 当前项目编译与链接（BWZ01 / CAA01） | CAC04 用户确认选人入场正常（2026-10-09） |
| [doc/cg2d_cache_lifecycle_alignment_spec.md](../doc/cg2d_cache_lifecycle_alignment_spec.md) | 缓存老化、重置与建面阶段 SPEC（GCL01） | BPF01 对老化计数表达式的具名更新（2026-10-06） |
| [doc/cg2d_capability3_input_normal_sampling_spec.md](../doc/cg2d_capability3_input_normal_sampling_spec.md) | 能力3输入正常加减与提交取样SPEC（BSE01） | 实现与计量状态 |
| [doc/cg2d_capability3_scene_normal_sampling_spec.md](../doc/cg2d_capability3_scene_normal_sampling_spec.md) | 能力3场景正常运算与透明度取样SPEC（BSF01） | 当前追踪状态 |
| [doc/cg2d_capability4_input_normal_arithmetic_spec.md](../doc/cg2d_capability4_input_normal_arithmetic_spec.md) | 能力4登录输入的正常S32算式与上界调用 SPEC（BSC01） | 本步范围 |
| [doc/cg2d_capability4_scene_sampling_spec.md](../doc/cg2d_capability4_scene_sampling_spec.md) | 能力4场景状态与透明度取样SPEC（BSH01） | 追踪状态 |
| [doc/cg2d_capability4_submit_length_sampling_spec.md](../doc/cg2d_capability4_submit_length_sampling_spec.md) | 能力4提交长度取样SPEC（BSG01） | 状态与计量 |
| [doc/cg2d_character_deletion_progress_geometry_spec.md](../doc/cg2d_character_deletion_progress_geometry_spec.md) | 人物删除进度动态几何子SPEC（BSM01） | 实施追踪 |
| [doc/cg2d_character_enqueue_record_production_spec.md](../doc/cg2d_character_enqueue_record_production_spec.md) | BPC01 角色入队的实际记录与字节发布 | 当前交付 |
| [doc/cg2d_character_list_progress_length_sampling_spec.md](../doc/cg2d_character_list_progress_length_sampling_spec.md) | 人物列表进度文本长度与布局取样子SPEC（BSL01） | 实施追踪 |
| [doc/cg2d_character_list_record_production_callees_spec.md](../doc/cg2d_character_list_record_production_callees_spec.md) | 角色列表回复与记录生产公用原语 SPEC（BRV01） | 当前范围 |
| [doc/cg2d_character_modal_action_gate_spec.md](../doc/cg2d_character_modal_action_gate_spec.md) | 人物模态共享对象与状态门禁子SPEC（BSQ01） | 实施追踪 |
| [doc/cg2d_character_name_byte_sampling_spec.md](../doc/cg2d_character_name_byte_sampling_spec.md) | 人物名称查重字节取样 SPEC（BTF01） | 实施状态 |
| [doc/cg2d_character_preview_slot_sampling_spec.md](../doc/cg2d_character_preview_slot_sampling_spec.md) | 人物预览共享槽取样 SPEC（BSY01） | 实施状态 |
| [doc/cg2d_character_select_dispatch_sampling_spec.md](../doc/cg2d_character_select_dispatch_sampling_spec.md) | 角色选择主分派与图号取样 SPEC（BRX01） | 当前边界 |
| [doc/cg2d_character_slot_normal_mode_spec.md](../doc/cg2d_character_slot_normal_mode_spec.md) | 人物槽普通样式推进 SPEC（BSZ01） | 实施状态 |
| [doc/cg2d_chat_receive_byte_sampling_spec.md](../doc/cg2d_chat_receive_byte_sampling_spec.md) | 聊天接收文本比较字节取样 SPEC（BTH01） | 实施状态 |
| [doc/cg2d_client_link_sdl_audio_spec.md](../doc/cg2d_client_link_sdl_audio_spec.md) | CAA01 客户端链接与 SDL 音频依赖 | CAB24 样本与运行表的原生产顺序（2026-10-09） |
| [doc/cg2d_closure_decisions_spec.md](../doc/cg2d_closure_decisions_spec.md) | 收尾决议总 SPEC | 3. 台账与后续顺序 |
| [doc/cg2d_code_range_identity_spec.md](../doc/cg2d_code_range_identity_spec.md) | 原代码区间身份与用途 SPEC（CRI01） | RPA01 首批64区间全字节用途（2026-10-05） |
| [doc/cg2d_command_checksum_publication_spec.md](../doc/cg2d_command_checksum_publication_spec.md) | 网络命令校验和计算与状态发布 SPEC（NCS01） | 源码小步收口 |
| [doc/cg2d_compiler_helper_spec.md](../doc/cg2d_compiler_helper_spec.md) | CPH01 编译器帮助入口与调色板正常算术 | 当前交付 |
| [doc/cg2d_confirmation_modal_action_sampling_spec.md](../doc/cg2d_confirmation_modal_action_sampling_spec.md) | 确认对话框共享弹窗对象取样子SPEC（BSO01） | 实施追踪 |
| [doc/cg2d_coordinate_reset_sampling_spec.md](../doc/cg2d_coordinate_reset_sampling_spec.md) | 坐标重置载荷取样与快照发布 SPEC（BUG01） | 当前状态 |
| [doc/cg2d_creation_controller_baseline_spec.md](../doc/cg2d_creation_controller_baseline_spec.md) | 创建控制器完整原新基线 SPEC | 收口 |
| [doc/cg2d_creation_gate_adaptation_spec.md](../doc/cg2d_creation_gate_adaptation_spec.md) | 创建场景入口门禁适配 SPEC | BVH01当前接线与完整静态本体纳管（2026-10-08） |
| [doc/cg2d_creation_initialization_sampling_spec.md](../doc/cg2d_creation_initialization_sampling_spec.md) | 创建初始化递增／版本发布与对象取样子SPEC（BST01） | 实施追踪 |
| [doc/cg2d_creation_progress_layout_sampling_spec.md](../doc/cg2d_creation_progress_layout_sampling_spec.md) | 创建请求进度布局取样子SPEC（BSR01） | 实施追踪 |
| [doc/cg2d_creation_request_tick_sampling_spec.md](../doc/cg2d_creation_request_tick_sampling_spec.md) | 创建请求计时取样与原格式组包 SPEC | 收口 |
| [doc/cg2d_crt_host_migration_spec.md](../doc/cg2d_crt_host_migration_spec.md) | CRT 宿主承接总 SPEC | BPM01 专用幂输入及控制字身份补齐（2026-10-07） |
| [doc/cg2d_crt_runtime_boundary_spec.md](../doc/cg2d_crt_runtime_boundary_spec.md) | CRT 堆、异常与文件边界子 SPEC | BWY01完整七参数NLS分类开发（2026-10-08） |
| [doc/cg2d_crt_startup_lifecycle_spec.md](../doc/cg2d_crt_startup_lifecycle_spec.md) | CRT 启动与生命周期迁移子 SPEC | HFI01 x64异常筛选与宿主固定终止接线（2026-10-05） |
| [doc/cg2d_cryptint_arithmetic_spec.md](../doc/cg2d_cryptint_arithmetic_spec.md) | CryptInt 普通算术对齐 SPEC（CQA01） | 本组源码状态 |
| [doc/cg2d_cursor_flag_sampling_restore_spec.md](../doc/cg2d_cursor_flag_sampling_restore_spec.md) | 光标取样与鼠标旗位原地址纠正 SPEC（BUW01） | 本步状态 |
| [doc/cg2d_cursor_grid_byte_sampling_spec.md](../doc/cg2d_cursor_grid_byte_sampling_spec.md) | 鼠标目标格生产六处 BYTE 取样 SPEC（BUE01） | 当前证据与实现状态 |
| [doc/cg2d_declared_exclusion_registry_spec.md](../doc/cg2d_declared_exclusion_registry_spec.md) | 已批准范围排除的R1登记 SPEC（SER01） | 本组状态 |
| [doc/cg2d_delimited_field_identity_spec.md](../doc/cg2d_delimited_field_identity_spec.md) | DFI01 原字节字段解析的四身份基线 | BPD01 邻接三数值消费者独立纳管（2026-10-06） |
| [doc/cg2d_delimited_numeric_normal_sampling_spec.md](../doc/cg2d_delimited_numeric_normal_sampling_spec.md) | BPD01 分段数值解析的正常算术与字节取样 | 当前交付与后续 |
| [doc/cg2d_development_gap_queue_spec.md](../doc/cg2d_development_gap_queue_spec.md) | 开发差集队列 SPEC（GQ01） | MPL01 纯NOP回执与真实定位缺口分开度量（2026-10-05） |
| [doc/cg2d_disconnect_dialog_sampling_spec.md](../doc/cg2d_disconnect_dialog_sampling_spec.md) | 断线提示对象与布局取样 SPEC（BTC01） | 实施状态 |
| [doc/cg2d_dispatch_account_field_read_spec.md](../doc/cg2d_dispatch_account_field_read_spec.md) | 接收分派账号与字段读取 SPEC（NDR01） | 源码小步收口 |
| [doc/cg2d_dispatch_header_byte_spec.md](../doc/cg2d_dispatch_header_byte_spec.md) | 网络分派头逐字节比较SPEC（BWU01） | 状态 |
| [doc/cg2d_emote_chat_mis_chain_spec.md](../doc/cg2d_emote_chat_mis_chain_spec.md) | 表情、聊天与 MIS 调用链基线 SPEC | BVS01表情请求的opaque类型边界修正（2026-10-08） |
| [doc/cg2d_enter_text_byte_sampling_spec.md](../doc/cg2d_enter_text_byte_sampling_spec.md) | 回车文本比较字节取样 SPEC（BTG01） | 实施状态 |
| [doc/cg2d_exception_text_heap_migration_spec.md](../doc/cg2d_exception_text_heap_migration_spec.md) | 异常基类拥有文本域宿主迁移 SPEC | 5. 本组源码收口 |
| [doc/cg2d_exit_table_heap_migration_spec.md](../doc/cg2d_exit_table_heap_migration_spec.md) | 兼容退出表内存域宿主迁移 SPEC（ETH01） | 实施记录 |
| [doc/cg2d_file_buffer_output_alignment_spec.md](../doc/cg2d_file_buffer_output_alignment_spec.md) | 文件缓冲输出地址差与数量 SPEC（FBO01） | 实施记录 |
| [doc/cg2d_file_position_width_spec.md](../doc/cg2d_file_position_width_spec.md) | 文件位置 W32 基线校正 SPEC（FPW01） | 收口记录 |
| [doc/cg2d_fill_rectangle_dimension_sampling_spec.md](../doc/cg2d_fill_rectangle_dimension_sampling_spec.md) | 矩形填充逻辑尺寸取样 SPEC | 源状态与收口 |
| [doc/cg2d_float_classification_leaf_spec.md](../doc/cg2d_float_classification_leaf_spec.md) | 浮点分类两个叶函数实现SPEC（BWC01） | 状态 |
| [doc/cg2d_float_decimal_estimate_spec.md](../doc/cg2d_float_decimal_estimate_spec.md) | 浮点十进制指数估算正常算术 SPEC（BPN01） | 当前交付状态 |
| [doc/cg2d_follow_gate_word_sampling_spec.md](../doc/cg2d_follow_gate_word_sampling_spec.md) | 跟随门禁跨字段 W32 取样 SPEC（BUA01） | 当前实现与证据状态 |
| [doc/cg2d_font_lifecycle_alignment_spec.md](../doc/cg2d_font_lifecycle_alignment_spec.md) | 字体初始化取样、派生写序及文本消费对照 SPEC | 5. 源码收口 |
| [doc/cg2d_font_width_arithmetic_spec.md](../doc/cg2d_font_width_arithmetic_spec.md) | FWA01 字体测宽正常算术与 RPLAN.24 | 当前交付 |
| [doc/cg2d_fx_strip_rectangle_publication_spec.md](../doc/cg2d_fx_strip_rectangle_publication_spec.md) | 正反向FX条带矩形发布 SPEC | 收口 |
| [doc/cg2d_fx_trigger_sampling_spec.md](../doc/cg2d_fx_trigger_sampling_spec.md) | 正反向FX共享触发槽取样 SPEC | 收口 |
| [doc/cg2d_gather_result_sampling_spec.md](../doc/cg2d_gather_result_sampling_spec.md) | GRS01 采集结果页描述符与状态/坐标取样 SPEC | 源状态与剩余 |
| [doc/cg2d_graphic_dx_cache_x64_spec.md](../doc/cg2d_graphic_dx_cache_x64_spec.md) | DX 图像缓存 x64 承载 SPEC | BPC01 角色实际游标与描述符逐对发布（2026-10-06） |
| [doc/cg2d_graphic_enqueue_descriptor_production_spec.md](../doc/cg2d_graphic_enqueue_descriptor_production_spec.md) | BOS01 图形入队与描述符生产 SPEC | 当前状态与后续 |
| [doc/cg2d_graphic_load_alignment_spec.md](../doc/cg2d_graphic_load_alignment_spec.md) | 图形装载与节点建链取样对照 SPEC | 5. GDL01源码小步结论（2026-10-05） |
| [doc/cg2d_graphic_read_decode_sampling_spec.md](../doc/cg2d_graphic_read_decode_sampling_spec.md) | BOL01 图形读盘与解码正常取样 SPEC | 状态与继续推进 |
| [doc/cg2d_graphic_record_production_lookup_spec.md](../doc/cg2d_graphic_record_production_lookup_spec.md) | BOK01 图形记录生产与直接查询 SPEC | 源码状态与后续推进 |
| [doc/cg2d_graphic_secondary_enqueue_production_spec.md](../doc/cg2d_graphic_secondary_enqueue_production_spec.md) | BOT01 其余图形入队与描述符生产 SPEC | 当前状态与后续 |
| [doc/cg2d_graphic_sort_alignment_spec.md](../doc/cg2d_graphic_sort_alignment_spec.md) | 图形队列保留槽与排序计数对照 SPEC | 5. GQS01源码小步结论（2026-10-05） |
| [doc/cg2d_graphic_transform_lifecycle_spec.md](../doc/cg2d_graphic_transform_lifecycle_spec.md) | 图形双树构析与宿主登记子 SPEC | BUY01 查询、发布与业务清理的真实生产基线（2026-10-08） |
| [doc/cg2d_graphic_upload_alignment_spec.md](../doc/cg2d_graphic_upload_alignment_spec.md) | 图形节点上传、条件取样与调色板借用 SPEC | 5. GUP01源码小步结论（2026-10-05） |
| [doc/cg2d_header_single_read_spec.md](../doc/cg2d_header_single_read_spec.md) | 取包头首指针单次读取 SPEC（NHS01） | 源码小步收口 |
| [doc/cg2d_hosted_client_executable_target_spec.md](../doc/cg2d_hosted_client_executable_target_spec.md) | 宿主客户端可执行目标 SPEC（BRS01） | 当前推进链与边界 |
| [doc/cg2d_hosted_exception_filter_spec.md](../doc/cg2d_hosted_exception_filter_spec.md) | 宿主异常筛选与终止接线 SPEC（HFI01） | 本组源码收口 |
| [doc/cg2d_hosted_runtime_prefix_spec.md](../doc/cg2d_hosted_runtime_prefix_spec.md) | 宿主兼容堆与低层IO前置 SPEC（HPI01） | 源码小步收口 |
| [doc/cg2d_hosted_stdio_init_spec.md](../doc/cg2d_hosted_stdio_init_spec.md) | 宿主兼容 FILE 表初始化 SPEC（HSI01） | 源码小步收口 |
| [doc/cg2d_hover_batch_unconditional_publication_spec.md](../doc/cg2d_hover_batch_unconditional_publication_spec.md) | BOV01 描边批次无条件发布 SPEC | 当前状态和后续 |
| [doc/cg2d_hover_input_publication_spec.md](../doc/cg2d_hover_input_publication_spec.md) | BPL01 悬停判定发布与点击查询基线 | 当前交付 |
| [doc/cg2d_ime_composition_alignment_spec.md](../doc/cg2d_ime_composition_alignment_spec.md) | IME 组合与提交顺序对齐 SPEC | 5. 源码收口 |
| [doc/cg2d_ime_display_arithmetic_spec.md](../doc/cg2d_ime_display_arithmetic_spec.md) | IMA01 输入法显示正常S32算术 | 当前交付 |
| [doc/cg2d_independent_palette_initialization_sampling_spec.md](../doc/cg2d_independent_palette_initialization_sampling_spec.md) | BOR01 独立调色板初始化与索引发布 SPEC | 当前状态 |
| [doc/cg2d_independent_palette_production_refresh_spec.md](../doc/cg2d_independent_palette_production_refresh_spec.md) | BOM01 独立调色板生产与版本刷新 SPEC | 状态与后续推进 |
| [doc/cg2d_input_append_alignment_spec.md](../doc/cg2d_input_append_alignment_spec.md) | 输入追加前置门禁与局部存储对齐 SPEC | BPG01 两个原取样片段的具名更新（2026-10-06） |
| [doc/cg2d_input_caret_alignment_spec.md](../doc/cg2d_input_caret_alignment_spec.md) | 输入光标绘制与计数取样对齐 SPEC | ICM01 光标坐标/相位正常算术（2026-10-05） |
| [doc/cg2d_input_caret_arithmetic_spec.md](../doc/cg2d_input_caret_arithmetic_spec.md) | ICM01 光标坐标与相位正常算术 | 当前交付 |
| [doc/cg2d_input_edit_alignment_spec.md](../doc/cg2d_input_edit_alignment_spec.md) | 输入编辑行宽、局部存储与取样对齐 SPEC | ICL01 当前终行正常长度算术（2026-10-05） |
| [doc/cg2d_input_render_alignment_spec.md](../doc/cg2d_input_render_alignment_spec.md) | 输入法绘制、候选与共享读取对齐 SPEC | IMA01 当前显示正常S32（2026-10-05） |
| [doc/cg2d_input_scan_budget_sampling_spec.md](../doc/cg2d_input_scan_budget_sampling_spec.md) | BPG01 输入追加的扫描预算与分类判定取样 | 当前交付 |
| [doc/cg2d_input_terminal_length_spec.md](../doc/cg2d_input_terminal_length_spec.md) | ICL01 下移光标终行正常长度算术 | 当前交付 |
| [doc/cg2d_input_text_alignment_spec.md](../doc/cg2d_input_text_alignment_spec.md) | 输入文本绘制重复取样与原路径对齐 SPEC | BPI01 分类判定与多行光标发布（2026-10-07） |
| [doc/cg2d_input_text_arithmetic_spec.md](../doc/cg2d_input_text_arithmetic_spec.md) | ITA01 输入绘制正常S32算术 | 当前交付 |
| [doc/cg2d_integer_left_compare_dispatch_spec.md](../doc/cg2d_integer_left_compare_dispatch_spec.md) | 整数左侧比较的原入口与八处调用 SPEC（BSB01） | 当前范围 |
| [doc/cg2d_integer_parity_rounding_spec.md](../doc/cg2d_integer_parity_rounding_spec.md) | 整数奇偶判定与舍入依赖实现SPEC（BWD01） | 状态 |
| [doc/cg2d_key_index_preset_source_spec.md](../doc/cg2d_key_index_preset_source_spec.md) | BPH01 按键索引宽度与预设热键生产消费 | 当前交付 |
| [doc/cg2d_legacy_line_sampling_spec.md](../doc/cg2d_legacy_line_sampling_spec.md) | LSP01 旧行提取取样与技能正文同源 SPEC | 当前交付 |
| [doc/cg2d_legacy_panel_button_bridge_sampling_spec.md](../doc/cg2d_legacy_panel_button_bridge_sampling_spec.md) | 旧面板按钮桥接取样 SPEC | 收口要求 |
| [doc/cg2d_legacy_panel_native_sampling_spec.md](../doc/cg2d_legacy_panel_native_sampling_spec.md) | 公共旧面板原生取样 SPEC | 收口要求 |
| [doc/cg2d_legacy_preview_flag_sampling_spec.md](../doc/cg2d_legacy_preview_flag_sampling_spec.md) | 旧页面预览标志取样SPEC（BVW01） | 状态 |
| [doc/cg2d_legacy_result_stage_baseline_spec.md](../doc/cg2d_legacy_result_stage_baseline_spec.md) | 旧结果阶段生产、消费与选择恢复基线SPEC（BWB01） | 状态 |
| [doc/cg2d_legacy_selection_category_sampling_spec.md](../doc/cg2d_legacy_selection_category_sampling_spec.md) | 旧面板选择类别取样与控制链SPEC（BWA01） | 状态 |
| [doc/cg2d_legacy_separator_byte_sampling_spec.md](../doc/cg2d_legacy_separator_byte_sampling_spec.md) | 旧分隔符文本扫描单BYTE取样子SPEC（BSX01） | 实施追踪 |
| [doc/cg2d_legacy_ui_add_arithmetic_spec.md](../doc/cg2d_legacy_ui_add_arithmetic_spec.md) | LUA01 旧 UI 共用加法正常算术 | 当前交付 |
| [doc/cg2d_local_determinism_spec.md](../doc/cg2d_local_determinism_spec.md) | 局部确定化适配子 SPEC | 2026-10-04：八项源码收口增量 |
| [doc/cg2d_locale_object_heap_migration_spec.md](../doc/cg2d_locale_object_heap_migration_spec.md) | locale 实现对象存储与退出迁移 SPEC | SCL01 对经典表槽的后续裁决 |
| [doc/cg2d_login_character_reply_byte_sampling_spec.md](../doc/cg2d_login_character_reply_byte_sampling_spec.md) | 登录与人物回复字节取样 SPEC（BRW01） | 当前边界 |
| [doc/cg2d_login_control_normal_geometry_spec.md](../doc/cg2d_login_control_normal_geometry_spec.md) | 登录控件正常坐标与字段取样SPEC（BSD01） | 源码与追踪状态 |
| [doc/cg2d_login_deletion_send_chain_baseline_spec.md](../doc/cg2d_login_deletion_send_chain_baseline_spec.md) | 登录与删除请求发送链基线 SPEC | 收口要求 |
| [doc/cg2d_login_mode_normal_message_branch_spec.md](../doc/cg2d_login_mode_normal_message_branch_spec.md) | 登录模式正常递增与消息分支SPEC（BSI01） | 追踪状态 |
| [doc/cg2d_login_negative_selection_byte_owner_spec.md](../doc/cg2d_login_negative_selection_byte_owner_spec.md) | 登录负分类20字节读取与owner SPEC（BRU01） | 固定池原字节视图与原生链接迁移（BUV01，2026-10-08） |
| [doc/cg2d_login_progress_layout_sampling_spec.md](../doc/cg2d_login_progress_layout_sampling_spec.md) | 登录完成前的进度布局与状态转换 SPEC（BRZ01） | 本步范围与后续 |
| [doc/cg2d_logout_deletion_request_tick_sampling_spec.md](../doc/cg2d_logout_deletion_request_tick_sampling_spec.md) | 登出与删除请求计时取样 SPEC | 收口要求 |
| [doc/cg2d_logout_progress_sampling_spec.md](../doc/cg2d_logout_progress_sampling_spec.md) | 场景7退出请求进度取样 SPEC（BTD01） | 实施状态 |
| [doc/cg2d_logout_reply_sampling_spec.md](../doc/cg2d_logout_reply_sampling_spec.md) | 登出回复字节取样与返回合同 SPEC（BTE01） | 实施状态 |
| [doc/cg2d_main_entry_state_carrier_sampling_spec.md](../doc/cg2d_main_entry_state_carrier_sampling_spec.md) | 主地图入口状态与数值载体发布 SPEC（BRY01） | 当前范围 |
| [doc/cg2d_map_action_coordinate_sampling_spec.md](../doc/cg2d_map_action_coordinate_sampling_spec.md) | 地图动作坐标载荷取样 SPEC（BUJ01） | 当前状态 |
| [doc/cg2d_map_boundary_span_sampling_spec.md](../doc/cg2d_map_boundary_span_sampling_spec.md) | 地图边界跨度取样与回退裁剪 SPEC（BUH01） | 当前状态 |
| [doc/cg2d_map_chara_link_spec.md](../doc/cg2d_map_chara_link_spec.md) | MCL01 观察节点追加/摘链读取点与已有清头身份 | 当前交付 |
| [doc/cg2d_map_chara_lookup_spec.md](../doc/cg2d_map_chara_lookup_spec.md) | MQC01 地图观察链查找取样与已有清理身份 | 当前交付 |
| [doc/cg2d_map_chara_producer_spec.md](../doc/cg2d_map_chara_producer_spec.md) | MCP01 观察节点生产字段复制与原取样 | 当前交付 |
| [doc/cg2d_map_child_exit_sampling_spec.md](../doc/cg2d_map_child_exit_sampling_spec.md) | MCE01 观察退场阶段与共用子绘制取样 | 当前交付 |
| [doc/cg2d_map_child_stage_sampling_spec.md](../doc/cg2d_map_child_stage_sampling_spec.md) | MCS01 观察子节点阶段取样与生产消费合同 | 当前交付 |
| [doc/cg2d_map_colored_enqueue_production_spec.md](../doc/cg2d_map_colored_enqueue_production_spec.md) | BOU01 地图批次与着色图形入队生产 SPEC | 当前状态与后续 |
| [doc/cg2d_map_coordinate_arithmetic_spec.md](../doc/cg2d_map_coordinate_arithmetic_spec.md) | MCA01 像素运动与投影增量正常S32 | 当前交付 |
| [doc/cg2d_map_draw_merge_word_sampling_spec.md](../doc/cg2d_map_draw_merge_word_sampling_spec.md) | 地图缓存合并逐 W32 复制 SPEC（BUM01） | 当前状态 |
| [doc/cg2d_map_effect_arithmetic_spec.md](../doc/cg2d_map_effect_arithmetic_spec.md) | MFA01 地图效果固定点正常算术与幂路径身份 | BWL01两个活动幂原点已接完整CI（2026-10-08） |
| [doc/cg2d_map_effect_pool_spec.md](../doc/cg2d_map_effect_pool_spec.md) | MEP01 效果池初始化写宽与链表取样 | 关联快照口径（MSS01，2026-10-05） |
| [doc/cg2d_map_effect_producer_spec.md](../doc/cg2d_map_effect_producer_spec.md) | MFP01 效果记录生产取样与正常固定点 | 关联池合同（MEP01，2026-10-05） |
| [doc/cg2d_map_incremental_reply_sampling_spec.md](../doc/cg2d_map_incremental_reply_sampling_spec.md) | 地图增量回包取样与连续帧 SPEC（BUQ01） | 当前状态 |
| [doc/cg2d_map_observation_branch_spec.md](../doc/cg2d_map_observation_branch_spec.md) | MOB01 观察主回调阶段与分支取样 | 当前交付 |
| [doc/cg2d_map_observation_source_spec.md](../doc/cg2d_map_observation_source_spec.md) | MOS01 五槽观察源取样与薄父合同 | 当前交付 |
| [doc/cg2d_map_observer_callback_order_spec.md](../doc/cg2d_map_observer_callback_order_spec.md) | MOC01 观察回调参数与排序读取序 | 当前交付 |
| [doc/cg2d_map_projection_cache_sampling_spec.md](../doc/cg2d_map_projection_cache_sampling_spec.md) | 地图缓存写入与投影取样 SPEC（BUR01） | 当前状态 |
| [doc/cg2d_map_record_identity_spec.md](../doc/cg2d_map_record_identity_spec.md) | MFI01 地图记录原身份与生产合同 | 当前交付 |
| [doc/cg2d_map_reply_reset_sampling_spec.md](../doc/cg2d_map_reply_reset_sampling_spec.md) | 地图回包与双快照复位 SPEC（BUP01） | 当前状态 |
| [doc/cg2d_map_request_original_tables_spec.md](../doc/cg2d_map_request_original_tables_spec.md) | UUN 请求与原 CRC 表承接 SPEC（BUO01） | 当前状态 |
| [doc/cg2d_map_scene_snapshot_sampling_spec.md](../doc/cg2d_map_scene_snapshot_sampling_spec.md) | 地图绘制父尾部快照预取 SPEC（BUK01） | 当前状态 |
| [doc/cg2d_map_switch_registry_baseline_spec.md](../doc/cg2d_map_switch_registry_baseline_spec.md) | 地图设置与注册表清理基线 SPEC（BUI01） | 当前状态 |
| [doc/cg2d_map_walkability_byte_sampling_spec.md](../doc/cg2d_map_walkability_byte_sampling_spec.md) | 地图第四平面 BYTE 取样与空格短路 SPEC（BUL01） | 当前状态 |
| [doc/cg2d_map_window_search_baseline_spec.md](../doc/cg2d_map_window_search_baseline_spec.md) | 地图窗口请求与索引表基线 SPEC（BUN01） | 当前状态 |
| [doc/cg2d_matching_hover_record_sampling_spec.md](../doc/cg2d_matching_hover_record_sampling_spec.md) | BPA01 按匹配值绘框的记录与坐标生产 | 当前交付 |
| [doc/cg2d_math_divide_shared_spec.md](../doc/cg2d_math_divide_shared_spec.md) | 原共享指数链的完整除法依赖SPEC（BWN01） | BWP01原64项分派及三个包装已接完整核心（2026-10-08） |
| [doc/cg2d_math_division_dispatch_spec.md](../doc/cg2d_math_division_dispatch_spec.md) | 原64项除法分派与三包装入口SPEC（BWP01） | BWQ01正逆包装的八个真实内存加载父已开发（2026-10-08） |
| [doc/cg2d_math_error_hardware_leaves_spec.md](../doc/cg2d_math_error_hardware_leaves_spec.md) | 数学错误链的硬件依赖叶实现SPEC（BWG01） | 状态 |
| [doc/cg2d_math_exception_dispatch_spec.md](../doc/cg2d_math_exception_dispatch_spec.md) | 原数学异常记录生产与分派SPEC（BWI01） | 状态 |
| [doc/cg2d_math_final_exits_spec.md](../doc/cg2d_math_final_exits_spec.md) | 原指数读取、精度与溢出出口SPEC（BWS01） | 状态 |
| [doc/cg2d_math_masked_handler_spec.md](../doc/cg2d_math_masked_handler_spec.md) | 原数学掩码处理与状态/分解依赖实现SPEC（BWH01） | 状态 |
| [doc/cg2d_math_memory_parents_spec.md](../doc/cg2d_math_memory_parents_spec.md) | 原八个内存除法加载父与两个共享区间SPEC（BWQ01） | 状态 |
| [doc/cg2d_math_record_range_spec.md](../doc/cg2d_math_record_range_spec.md) | 原数学错误记录生产与范围出口SPEC（BWJ01） | BWS01原溢出入口真实共享本体（2026-10-08） |
| [doc/cg2d_math_remainder_chain_spec.md](../doc/cg2d_math_remainder_chain_spec.md) | 原余数双链、环境恢复与四共享入口SPEC（BWR01） | 状态 |
| [doc/cg2d_math_residual_leaves_spec.md](../doc/cg2d_math_residual_leaves_spec.md) | 原剩余浮点指令片段与对齐区间SPEC（BWW01） | 状态 |
| [doc/cg2d_math_shared_dispatch_spec.md](../doc/cg2d_math_shared_dispatch_spec.md) | 原共享数学分派全入口与特殊值目标SPEC（BWM01） | BWO01共享幂、指数、对数目标已完整开发（2026-10-08） |
| [doc/cg2d_math_shared_targets_spec.md](../doc/cg2d_math_shared_targets_spec.md) | 原共享幂、指数与对数目标完整实现SPEC（BWO01） | 状态 |
| [doc/cg2d_memory_compare_sampling_spec.md](../doc/cg2d_memory_compare_sampling_spec.md) | 分块内存比较的取样与共享返回 SPEC（MRC01） | 当前边界 |
| [doc/cg2d_metadata_padding_disposition_spec.md](../doc/cg2d_metadata_padding_disposition_spec.md) | NOP 元数据身份处置 SPEC（MPL01） | 本组边界 |
| [doc/cg2d_mixed_upload_identity_spec.md](../doc/cg2d_mixed_upload_identity_spec.md) | 混合范围纹理上传的身份与基线 SPEC（MUP01） | 当前状态与验收边界 |
| [doc/cg2d_modal_button_geometry_sampling_spec.md](../doc/cg2d_modal_button_geometry_sampling_spec.md) | 公共模态按钮几何与取样子SPEC（BSP01） | 实施追踪 |
| [doc/cg2d_mode3_heap_facade_spec.md](../doc/cg2d_mode3_heap_facade_spec.md) | 兼容堆模式3成组宿主承接 SPEC（M3H01） | 源码小步收口 |
| [doc/cg2d_mode3_private_core_spec.md](../doc/cg2d_mode3_private_core_spec.md) | 模式3旧私有核心接口退役与宿主映射 SPEC（M3P01） | 源码收口 |
| [doc/cg2d_mouse_coordinate_publication_spec.md](../doc/cg2d_mouse_coordinate_publication_spec.md) | 鼠标坐标临时量取样与发布 SPEC（BUU01） | BUU01旗位通过断言失效与纠正（BUW01，2026-10-08） |
| [doc/cg2d_mouse_input_chain_spec.md](../doc/cg2d_mouse_input_chain_spec.md) | 鼠标输入生产与元素消费合同 SPEC（MIC01） | 真实上游窗口坐标路径修正（BUT01，2026-10-08） |
| [doc/cg2d_mouse_tracker_native_links_spec.md](../doc/cg2d_mouse_tracker_native_links_spec.md) | 鼠标追踪原生链接与失败时序 SPEC（BUV01） | 本步状态 |
| [doc/cg2d_multibyte_string_spec.md](../doc/cg2d_multibyte_string_spec.md) | 旧多字节整串转换 SPEC（MWS01） | 本组源码状态 |
| [doc/cg2d_narrow_string_heap_migration_spec.md](../doc/cg2d_narrow_string_heap_migration_spec.md) | 原窄字符串字节分配域宿主迁移 SPEC | 5. 本组源码收口 |
| [doc/cg2d_native_cpp_exception_spec.md](../doc/cg2d_native_cpp_exception_spec.md) | 五类本机 C++ 异常接线 SPEC | BPK01 私有查找与捕获复制职责承接（2026-10-07） |
| [doc/cg2d_network_adapter_sampling_spec.md](../doc/cg2d_network_adapter_sampling_spec.md) | NAS01 适配器标识取样与局部清零 SPEC | 当前交付 |
| [doc/cg2d_network_buffer_lifecycle_spec.md](../doc/cg2d_network_buffer_lifecycle_spec.md) | 网络缓冲生产与运行期生命周期 SPEC（NLC01） | 源码小步收口 |
| [doc/cg2d_network_compatibility_spec.md](../doc/cg2d_network_compatibility_spec.md) | 原格式明文传送子 SPEC | CAB32 IP2实际服务来源与运行待项（2026-10-09） |
| [doc/cg2d_network_exclusion_wiring_spec.md](../doc/cg2d_network_exclusion_wiring_spec.md) | 原格式明文网络接线总 SPEC | 三个发送侧状态入口的显式存储关联（2026-10-02） |
| [doc/cg2d_network_plaintext_frame_spec.md](../doc/cg2d_network_plaintext_frame_spec.md) | 原格式明文帧实施与静态对照子 SPEC | 6. P09/P10源码与局部证据收口 |
| [doc/cg2d_network_queue_arithmetic_spec.md](../doc/cg2d_network_queue_arithmetic_spec.md) | NQA01 原格式明文队列正常长度算术 | 当前交付 |
| [doc/cg2d_nls_map_general_spec.md](../doc/cg2d_nls_map_general_spec.md) | 原通用NLS映射、W32大写与计数读取SPEC（BWX01） | 状态 |
| [doc/cg2d_nls_type_general_spec.md](../doc/cg2d_nls_type_general_spec.md) | 原通用NLS字符分类与W32栈请求SPEC（BWY01） | 状态 |
| [doc/cg2d_normal_scene_controller_callee_order_spec.md](../doc/cg2d_normal_scene_controller_callee_order_spec.md) | 主地图控制器调用与数值发布 SPEC（BSA01） | 当前范围 |
| [doc/cg2d_outline_rectangle_dimension_sampling_spec.md](../doc/cg2d_outline_rectangle_dimension_sampling_spec.md) | 描边与填色逻辑尺寸取样 SPEC | 收口 |
| [doc/cg2d_palette_age_normal_counter_spec.md](../doc/cg2d_palette_age_normal_counter_spec.md) | BPF01 调色板帧末老化的正常计数 | 当前交付 |
| [doc/cg2d_palette_cache_alignment_spec.md](../doc/cg2d_palette_cache_alignment_spec.md) | 调色板缓存生成与池借用阶段 SPEC（GPC01） | 实施与验收记录 |
| [doc/cg2d_palette_dbf_index_production_spec.md](../doc/cg2d_palette_dbf_index_production_spec.md) | BOQ01 独立调色板DBF索引生产 SPEC | 当前进度与后续 |
| [doc/cg2d_palette_global_memory_spec.md](../doc/cg2d_palette_global_memory_spec.md) | 独立调色板全局内存生命周期 SPEC（PGM01） | BOR01 初始化数量与取得表发布补充（2026-10-06） |
| [doc/cg2d_palette_initialization_read_sampling_spec.md](../doc/cg2d_palette_initialization_read_sampling_spec.md) | BOO01 调色板初始化与读取取样 SPEC | 当前进度与后续开发 |
| [doc/cg2d_palette_transition_channel_sampling_spec.md](../doc/cg2d_palette_transition_channel_sampling_spec.md) | BON01 调色板过渡、通道与查色表取样 SPEC | 当前状态与继续推进 |
| [doc/cg2d_pet_catalog_production_spec.md](../doc/cg2d_pet_catalog_production_spec.md) | 旧宠物技能目录生产取证 SPEC（PCP01） | TTP01 共用采集基类的范围检查修正（2026-10-05） |
| [doc/cg2d_pet_slot_shutdown_sampling_spec.md](../doc/cg2d_pet_slot_shutdown_sampling_spec.md) | 五槽退组指针与标志取样 SPEC（BTQ01） | 本步静态终态（2026-10-07） |
| [doc/cg2d_player_action_dispatch_sampling_spec.md](../doc/cg2d_player_action_dispatch_sampling_spec.md) | 玩家动作分发与外观子槽取样 SPEC（BTM01） | 本步完成结果（2026-10-07） |
| [doc/cg2d_player_action_flag_widths_spec.md](../doc/cg2d_player_action_flag_widths_spec.md) | 玩家及动作状态真实字节/字访问 SPEC（BTL01） | 本步完成结果（2026-10-07） |
| [doc/cg2d_player_move_control_sampling_spec.md](../doc/cg2d_player_move_control_sampling_spec.md) | 玩家移动控制取样与队列分支 SPEC（BUB01） | 当前实施状态 |
| [doc/cg2d_player_move_coordinate_sampling_spec.md](../doc/cg2d_player_move_coordinate_sampling_spec.md) | 玩家移动坐标生产消费取样 SPEC（BTZ01） | 当前实现与证据状态 |
| [doc/cg2d_player_single_step_sampling_spec.md](../doc/cg2d_player_single_step_sampling_spec.md) | 玩家单步临时坐标取样 SPEC（BUC01） | 当前实现状态 |
| [doc/cg2d_player_state_byte_publication_spec.md](../doc/cg2d_player_state_byte_publication_spec.md) | 玩家状态 BYTE 发布 SPEC（BTK01） | 本步完成结果（2026-10-07） |
| [doc/cg2d_player_turn_path_baseline_spec.md](../doc/cg2d_player_turn_path_baseline_spec.md) | 玩家转向与路径生成追踪基线 SPEC（BUD01） | 当前证据状态 |
| [doc/cg2d_power_application_wiring_spec.md](../doc/cg2d_power_application_wiring_spec.md) | 完整幂接通两地图原调用点SPEC（BWL01） | 状态 |
| [doc/cg2d_power_general_helpers_spec.md](../doc/cg2d_power_general_helpers_spec.md) | 通用幂依赖帮助体开发SPEC（BWF01） | 状态 |
| [doc/cg2d_power_generic_body_spec.md](../doc/cg2d_power_generic_body_spec.md) | 完整通用幂本体与原FPU生命周期SPEC（BWK01） | BWM01共享幂帮助/CRT数学多入口已开发（2026-10-08） |
| [doc/cg2d_power_special_helper_spec.md](../doc/cg2d_power_special_helper_spec.md) | 幂特殊值辅助函数实现SPEC（BWE01） | 状态 |
| [doc/cg2d_projection_sampling_return_spec.md](../doc/cg2d_projection_sampling_return_spec.md) | 双向投影取样与指针返回 SPEC（BUF01） | CAB20 实际源码与限定结果（2026-10-09） |
| [doc/cg2d_protocol_copy_capacity_spec.md](../doc/cg2d_protocol_copy_capacity_spec.md) | 协议复制容量表达式与返回合同 SPEC（NCP01） | 源码小步收口 |
| [doc/cg2d_protocol_integer_normal_arithmetic_spec.md](../doc/cg2d_protocol_integer_normal_arithmetic_spec.md) | BPE01 协议整数解析的正常算术 | 当前交付 |
| [doc/cg2d_quadrant_transition_blit_baseline_spec.md](../doc/cg2d_quadrant_transition_blit_baseline_spec.md) | 四块转场与源矩形复制的完整基线 SPEC | 收口 |
| [doc/cg2d_queued_draw_alignment_spec.md](../doc/cg2d_queued_draw_alignment_spec.md) | 完整队列绘制与DX基址取样对照 SPEC | 整数纹理实参与当前合同（BUX01，2026-10-08） |
| [doc/cg2d_queued_rectangle_consumption_sampling_spec.md](../doc/cg2d_queued_rectangle_consumption_sampling_spec.md) | BOZ01 队列矩形消费者的取样与栈发布 | BOZ01正确case2消费与BVA01下游边界实修（2026-10-08） |
| [doc/cg2d_renderer_lost_probe_sampling_spec.md](../doc/cg2d_renderer_lost_probe_sampling_spec.md) | BOX01 渲染器失效探测取样 SPEC | 当前状态与后续 |
| [doc/cg2d_renderer_node_release_sampling_spec.md](../doc/cg2d_renderer_node_release_sampling_spec.md) | BOW01 渲染器节点释放取样 SPEC | 当前状态与后续 |
| [doc/cg2d_renderer_rebuild_baseline_spec.md](../doc/cg2d_renderer_rebuild_baseline_spec.md) | 渲染重建与场景快照基线 SPEC（BUS01） | 当前状态 |
| [doc/cg2d_resource_container_lifecycle_spec.md](../doc/cg2d_resource_container_lifecycle_spec.md) | 图形索引与 DBF 容器构析子 SPEC | AIS01四资源provider的当前活动位置（2026-10-05） |
| [doc/cg2d_ripple_full_strip_production_spec.md](../doc/cg2d_ripple_full_strip_production_spec.md) | BPB01 波纹完整条带生产与正常有符号运算 | 当前交付与后续 |
| [doc/cg2d_saved_codec_arithmetic_spec.md](../doc/cg2d_saved_codec_arithmetic_spec.md) | 保存字节编码算术与基线 SPEC（SCA01） | 源码小步收口 |
| [doc/cg2d_scanline_trigger_sampling_spec.md](../doc/cg2d_scanline_trigger_sampling_spec.md) | 上下扫描线转场触发槽取样 SPEC | 收口 |
| [doc/cg2d_scene_snapshot_width_spec.md](../doc/cg2d_scene_snapshot_width_spec.md) | MSS01 场景快照半字宽度与复读取样 | 当前交付 |
| [doc/cg2d_screenshot_chain_baseline_spec.md](../doc/cg2d_screenshot_chain_baseline_spec.md) | 截图链基线与验收SPEC（BVT01） | 状态 |
| [doc/cg2d_sdk_classic_locale_spec.md](../doc/cg2d_sdk_classic_locale_spec.md) | 经典 locale 宿主活动提供者 SPEC（SCL01） | 源码小步收口 |
| [doc/cg2d_sdk_late_narrow_init_spec.md](../doc/cg2d_sdk_late_narrow_init_spec.md) | 晚段窄标准流宿主 Init 生命周期 SPEC（SLN01） | 源码小步收口 |
| [doc/cg2d_sdk_standard_stream_owners_spec.md](../doc/cg2d_sdk_standard_stream_owners_spec.md) | 标准流宿主提供者与兼容 Init 类型 SPEC（SSO01） | 源码小步收口 |
| [doc/cg2d_server_category_click_sampling_spec.md](../doc/cg2d_server_category_click_sampling_spec.md) | 选服分类与点击取样子SPEC（BSJ01） | 实施追踪 |
| [doc/cg2d_server_connection_phase_layout_sampling_spec.md](../doc/cg2d_server_connection_phase_layout_sampling_spec.md) | 服务器连接阶段与布局取样子SPEC（BSK01） | 实施追踪 |
| [doc/cg2d_shared_random_source_baseline_spec.md](../doc/cg2d_shared_random_source_baseline_spec.md) | 共享随机源基线与验收SPEC（BVU01） | 状态 |
| [doc/cg2d_shop_choice_body_spec.md](../doc/cg2d_shop_choice_body_spec.md) | 旧62买卖选择完整本体 SPEC（SCP01） | BWT01未生产命令的实际读取边界修复（2026-10-08） |
| [doc/cg2d_shop_command_read_spec.md](../doc/cg2d_shop_command_read_spec.md) | 旧商店命令原宽度读取边界SPEC（BWT01） | 状态 |
| [doc/cg2d_shortcut_request_option_sampling_spec.md](../doc/cg2d_shortcut_request_option_sampling_spec.md) | 快捷请求与选项取样 SPEC | 收口顺序 |
| [doc/cg2d_skill_preview_flag_boundary_spec.md](../doc/cg2d_skill_preview_flag_boundary_spec.md) | 技能与宠物技能预览类型边界修复SPEC（BVZ01） | 状态 |
| [doc/cg2d_source_gap_status_spec.md](../doc/cg2d_source_gap_status_spec.md) | 源码缺口标记校正 SPEC（HSC01） | 本组状态 |
| [doc/cg2d_standard_stream_controller_gates_spec.md](../doc/cg2d_standard_stream_controller_gates_spec.md) | 标准流控制器原构造门禁 SPEC（SCG01） | 实施记录 |
| [doc/cg2d_standard_stream_exit_migration_spec.md](../doc/cg2d_standard_stream_exit_migration_spec.md) | 标准流退出登记域迁移 SPEC（SSX01） | 实施记录 |
| [doc/cg2d_standard_stream_init_identity_spec.md](../doc/cg2d_standard_stream_init_identity_spec.md) | 标准流四个 Init 对象的地址承载 SPEC | SSO01 已知类型接口与宿主私有角色对应 |
| [doc/cg2d_standard_stream_registry_prefix_spec.md](../doc/cg2d_standard_stream_registry_prefix_spec.md) | 标准流登记数据已证前缀 SPEC | 4. 本组源码收口 |
| [doc/cg2d_startup_protection_lifecycle_spec.md](../doc/cg2d_startup_protection_lifecycle_spec.md) | 启动保护值四项构析与宿主登记 SPEC | 4. 本组源码收口 |
| [doc/cg2d_stdio_table_heap_spec.md](../doc/cg2d_stdio_table_heap_spec.md) | stdio 指针索引表宿主清零域 SPEC（SCH01） | 源码小步收口 |
| [doc/cg2d_stream_table_pointer_spec.md](../doc/cg2d_stream_table_pointer_spec.md) | 流缓冲方法表指针类型 SPEC（STP01） | 源码小步收口 |
| [doc/cg2d_surface_transform_cache_baseline_spec.md](../doc/cg2d_surface_transform_cache_baseline_spec.md) | 变换双缓存查询、像素生成与场景清理基线 SPEC | 源状态和后续收口 |
| [doc/cg2d_team_reply_sampling_spec.md](../doc/cg2d_team_reply_sampling_spec.md) | 团队回复对象与目标取样 SPEC（BTR01） | 本步静态终态（2026-10-07） |
| [doc/cg2d_text_encoding_spec.md](../doc/cg2d_text_encoding_spec.md) | cg2d_re 文本编码与跨函数契约 SPEC | 2026-10-09：静态Big5转GBK、宋体与ASCII光标 |
| [doc/cg2d_text_queue_alignment_spec.md](../doc/cg2d_text_queue_alignment_spec.md) | 文本队列生产复制阶段与原门禁对齐 SPEC | 5. 源码收口 |
| [doc/cg2d_texture_viewport_sampling_spec.md](../doc/cg2d_texture_viewport_sampling_spec.md) | 纹理逻辑尺寸取样与整数坐标宽度 SPEC | 源状态与收口 |
| [doc/cg2d_traceability_acceptance_spec.md](../doc/cg2d_traceability_acceptance_spec.md) | 功能追踪、度量与验收 SPEC | CAA01 客户端链接与必要SDL音频（2026-10-08） |
| [doc/cg2d_trade_target_production_spec.md](../doc/cg2d_trade_target_production_spec.md) | TTP01 旧交易名单生产取证 | 当前交付 |
| [doc/cg2d_transition_dispatch_grid_baseline_spec.md](../doc/cg2d_transition_dispatch_grid_baseline_spec.md) | 转场控制父与64块、960块、双半幅基线 SPEC | 收口 |
| [doc/cg2d_u64_multiply_shift_spec.md](../doc/cg2d_u64_multiply_shift_spec.md) | 原U64通用乘法与左移及输入接线SPEC（BWV01） | 状态 |
| [doc/cg2d_ui_input_lifecycle_spec.md](../doc/cg2d_ui_input_lifecycle_spec.md) | UI与输入五项活动构析 SPEC | 4. 本组源码收口 |
| [doc/cg2d_ui_preview_sampling_spec.md](../doc/cg2d_ui_preview_sampling_spec.md) | UPS01 旧面板预览状态取样与代码区间 SPEC | 源状态与剩余 |
| [doc/cg2d_unassigned_range_classification_spec.md](../doc/cg2d_unassigned_range_classification_spec.md) | RPA01 未归属区间首批完整字节分类 | BPP01 当前交付 |
| [doc/cg2d_version_payload_zero_prefix_owner_spec.md](../doc/cg2d_version_payload_zero_prefix_owner_spec.md) | 版本正文零前缀与唯一 owner SPEC（BRT01） | 当前结果与边界 |
| [doc/cg2d_version_upgrade_action_sampling_spec.md](../doc/cg2d_version_upgrade_action_sampling_spec.md) | 版本升级场景对象取样 SPEC（BTA01） | 实施状态 |
| [doc/cg2d_window_close_mode_publication_spec.md](../doc/cg2d_window_close_mode_publication_spec.md) | 窗口关闭与模式判定发布 SPEC | 收口顺序 |
| [doc/cg2d_window_input_gate_restore_spec.md](../doc/cg2d_window_input_gate_restore_spec.md) | 窗口按键写序与坐标路径恢复 SPEC（BUT01） | 本步状态 |
| [doc/cg2d_window_mouse_byte_spec.md](../doc/cg2d_window_mouse_byte_spec.md) | 窗口更新鼠标状态字节合同 SPEC（WMB01） | 本组状态 |
| [doc/cg2d_window_release_source_sampling_spec.md](../doc/cg2d_window_release_source_sampling_spec.md) | 窗口释放取样与终止标志 SPEC | 收口顺序 |
| [doc/cg2d_window_routing_sampling_spec.md](../doc/cg2d_window_routing_sampling_spec.md) | WRS01 现代窗口循环判定取样与真实场景调用 SPEC | CAB30 当前正常绘制消费与编译（2026-10-09） |
| [doc/cg2d_window_shortcut_publication_spec.md](../doc/cg2d_window_shortcut_publication_spec.md) | 窗口快捷键判定发布 SPEC | 收口顺序 |
| [doc/cg2d_winsock_import_binding_spec.md](../doc/cg2d_winsock_import_binding_spec.md) | Winsock导入映射与连接共享状态重读 SPEC（NWI01） | 源码小步收口 |
| [doc/cg2d_workspace_byte_pointer_spec.md](../doc/cg2d_workspace_byte_pointer_spec.md) | 网络工作区字节指针与解码基线 SPEC（NWT01） | BPE01 对5176B0当前算术的具名更新（2026-10-06） |
| [doc/cg2d_world_dispatch_state_sampling_spec.md](../doc/cg2d_world_dispatch_state_sampling_spec.md) | 场景分派状态取样与正常递增 SPEC（BTB01） | 实施状态 |
| [doc/cg2d_world_state_lifecycle_spec.md](../doc/cg2d_world_state_lifecycle_spec.md) | 世界状态十七项活动构析与宿主登记 SPEC | 玩家坐标比较的原读取副作用（BSB01，2026-10-07） |
| [doc/cg2d_yobi_allocation_sampling_spec.md](../doc/cg2d_yobi_allocation_sampling_spec.md) | YAS01 Yobi 分配与插链取样 SPEC | 源状态与剩余 |
| [doc/cg2d_yobi_cleanup_sampling_spec.md](../doc/cg2d_yobi_cleanup_sampling_spec.md) | YLC01 Yobi 摘链与回收取样 SPEC | 源状态与剩余 |
| [doc/cg2d_yobi_sentinel_sampling_spec.md](../doc/cg2d_yobi_sentinel_sampling_spec.md) | YSS01 Yobi 哨兵初始化与清场/终止 SPEC | 源状态与剩余 |
