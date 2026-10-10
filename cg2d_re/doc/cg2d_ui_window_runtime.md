# UI 窗口运行期与移动父链 SPEC

> 本文件仅保留总览与按ID索引。各窗口明细位于 `ui_windows/window_<ID>.md`；共享合同保留唯一来源，维护规则见[文档组织 SPEC](ui_windows/README.md)。

## 当前对齐摘要

2026-10-04随CRT库本体及父入口源码阶段同步当前计数。对齐前完整进度/合同已固定于 `cg2d-re-spec-history-20261004-crt-body-frontier`；前次及更早历史见 `cg2d-re-spec-history-20261004-format-frontier`、`cg2d-re-spec-history-20261003`。下方现代/旧ID索引、明细历史及验证等级保持。

- 现代窗口71槽/70主原型（含0号空记录，36/37共享），69组去重元素/133主回调有唯一本体，直接缺口0/0；不含旧99条handler或全部传递依赖/生产者。
- 通用创建/生命周期/输入绘制、504120→473F80→4C7AE0更新及507140两caller完整命中已有；现代24的52C3D0继续暂停。
- 战斗404DB0的257表项/200动作和43CBB0至427470已接真实调用/回调；四M头复制、七25头查找及M索引提示已接。424620攻击生产/未写局部、载荷清理和真实战斗验收仍待。
- 旧64/66、68/69、70/71、73、35/36/77/80至82、42至61邮件/技能、86至96公会及6/17/99已有本体，旧20列表/旧62初始化已补；旧表实际路由、内嵌选择器及独立数据生产仍待。
- 旧58的CC3E64/C8AF84与旧51的D30134/CF46D0已有装载零初值和真实存储，运行时写者仍缺；CA0478目录、CB1D1C名单的容量/定义/生产仍待。D45F74/CC3208页号已落位。八项局部适配源码均已对齐，完整功能另验，旧62父仍待原事实，见[收尾表](../plan/cg2d_spec_closure_checklist.md)。
- 窗口、人物/宠物、技能/物品/设置、邮件/制造/商店/交易/仓库/公会等原格式已沿真实生产消费接入核心；全线真实1000/1023，余23为排除13/暂停6/替换3/待事实或实施1。共享表达式分别映射原点，缓冲/字节/符号/调用次序见局部合同；不把格式接线当页面或完整链验收。
- 限长复制37真实+2键盘排除、追加3/3；完整复制9真实+2宿主承接=11、长度19真实+4宿主承接+1待=24；字符24真实+1宿主承接=25、子串49真实+4newest排除=53。宿主承接沿已批GetEnv范围，内部尾不增加入口引用，普通输入/IME保留。
- 网络108路分发已有，直接回复缺口仍1个murD随音频暂停；原格式明文两帧父源码/既有接线已落位，完整网络功能和实际互通仍待。现代0/0/1不是全客户端完成率，暂停不是排除。
- 本次仅文档对齐及必要静态核对，不复检已接点、不构建/查产物/新增或执行测试/运行客户端或算法样例/计算比对SHA/hash；完整输入/收发/画面/生命周期及总SPEC门禁仍未完成。

当前状态与未决见[唯一进度](../plan/cg2d_full_reconstruction_progress.md#3-未闭合与明确待决)和[总SPEC](../plan/cg2d_full_reconstruction_plan.md#43-当前未闭合与明确待决)。现代window id与旧选择ID分别导航，旧证据不批量改写。


## 按窗口ID索引

下列70个ID已有明细或明确共享合同，各文件分别记录元素、主回调及接线状态；有明细不代表已实现全部内容，静态实现/绑定不等于完整流程验收。

| window id | 明细 | 原型地址 | 元素数 |
|---|---|---|---|
| 1 | [状态栏与提示按钮](ui_windows/window_1.md) | `54E2A0` | 24 |
| 2 | [紧凑状态栏](ui_windows/window_2.md) | `54E2D0` | 7 |
| 3 | [折叠状态栏](ui_windows/window_3.md) | `54E300` | 2 |
| 4 | [快捷交互栏](ui_windows/window_4.md) | `54E240` | 11 |
| 5 | [折叠快捷交互栏](ui_windows/window_5.md) | `54E270` | 2 |
| 6 | [底栏与悬停描述](ui_windows/window_6.md) | `54E330` | 14 |
| 7 | [玩家资料](ui_windows/window_7.md) | `54E360` | 73 |
| 8 | [玩家明细](ui_windows/window_8.md) | `54E390` | 43 |
| 9 | [玩家称号](ui_windows/window_9.md) | `54E3C0` | 37 |
| 10 | [技能、技术与宠物技能](ui_windows/window_10.md) | `54E420` | 30 |
| 11 | [制造与物品加工](ui_windows/window_11.md) | `54E450` | 31 |
| 12 | [窗口12操作与结果](ui_windows/window_12.md) | `54E480` | 15 |
| 13 | [窗口13操作与结果](ui_windows/window_13.md) | `54E4B0` | 14 |
| 14 | [库存与装备](ui_windows/window_14.md) | `54E4E0` | 24 |
| 15 | [宠物列表](ui_windows/window_15.md) | `54E540` | 259 |
| 16 | [宠物详情](ui_windows/window_16.md) | `54E570` | 66 |
| 17 | [宠物详细属性](ui_windows/window_17.md) | `54E5A0` | 46 |
| 18 | [宠物技能详情](ui_windows/window_18.md) | `54E5D0` | 51 |
| 19 | [图鉴列表](ui_windows/window_19.md) | `54E600` | 71 |
| 20 | [图鉴详情](ui_windows/window_20.md) | `54E630` | 82 |
| 21 | [联系人与邮件列表](ui_windows/window_21.md) | `54E990` | 111 |
| 22 | [发信](ui_windows/window_22.md) | `54EA20` | 9 |
| 23 | [宠物邮件](ui_windows/window_23.md) | `54EA50` | 13 |
| 24 | [设置](ui_windows/window_24.md) | `54E660` | 32 |
| 25 | [快捷键列表与预设文本](ui_windows/window_25.md) | `54E690` | 25 |
| 26 | [战斗命令](ui_windows/window_26.md) | `54E720` | 14 |
| 27 | [战斗物品](ui_windows/window_27.md) | `54E7E0` | 22 |
| 28 | [战斗宠物切换](ui_windows/window_28.md) | `54E810` | 8 |
| 29 | [观战结束](ui_windows/window_29.md) | `54E750` | 2 |
| 30 | [战斗结果](ui_windows/window_30.md) | `54E780` | 66 |
| 31 | [DP结果](ui_windows/window_31.md) | `54E7B0` | 4 |
| 32 | [战斗入场提示](ui_windows/window_32.md) | `54E840` | 2 |
| 33 | [小地图](ui_windows/window_33.md) | `54E6C0` | 7 |
| 34 | [情绪动作](ui_windows/window_34.md) | `54E6F0` | 20 |
| 35 | [可调聊天窗口](ui_windows/window_35.md) | `54EAB0` | 20 |
| 36 | [通用对话共享实例](ui_windows/window_36.md) | `54E870` | 19 |
| 37 | [通用对话创建入口](ui_windows/window_37.md) | `54E870` | 19 |
| 38 | [外部物品存放](ui_windows/window_38.md) | `54EB40` | 6 |
| 39 | [计时提示](ui_windows/window_39.md) | `54E8A0` | 1 |
| 40 | [读信](ui_windows/window_40.md) | `54E9F0` | 21 |
| 41 | [物品与宠物存放](ui_windows/window_41.md) | `54EB10` | 38 |
| 42 | [数量输入](ui_windows/window_42.md) | `54E510` | 8 |
| 43 | [交易](ui_windows/window_43.md) | `54EBD0` | 20 |
| 44 | [交易目标选择](ui_windows/window_44.md) | `54E8D0` | 19 |
| 45 | [文本目标选择](ui_windows/window_45.md) | `54E900` | 19 |
| 46 | [属性目标选择](ui_windows/window_46.md) | `54E930` | 34 |
| 47 | [通用确认](ui_windows/window_47.md) | `54EA80` | 34 |
| 48 | [公会详情与职级编辑](ui_windows/window_48.md) | `54EBA0` | 76 |
| 49 | [群组选人](ui_windows/window_49.md) | `54EC00` | 37 |
| 50 | [四记录对话](ui_windows/window_50.md) | `54EC30` | 36 |
| 51 | [文本列表](ui_windows/window_51.md) | `54EB70` | 26 |
| 52 | [商店选择](ui_windows/window_52.md) | `54E960` | 3 |
| 53 | [商品买卖](ui_windows/window_53.md) | `54EC60` | 66 |
| 54 | [宠物与技能选择](ui_windows/window_54.md) | `54EC90` | 107 |
| 55 | [公会职级选择](ui_windows/window_55.md) | `54E9C0` | 29 |
| 56 | [外观选择](ui_windows/window_56.md) | `54ED20` | 15 |
| 57 | [简化聊天窗口](ui_windows/window_57.md) | `54EAE0` | 2 |
| 58 | [自身资料](ui_windows/window_58.md) | `54E3F0` | 34 |
| 59 | [自身资料分类选择](ui_windows/window_59.md) | `54ED60` | 32 |
| 60 | [资料分类选择](ui_windows/window_60.md) | `54ED90` | 162 |
| 61 | [资料搜索结果](ui_windows/window_61.md) | `54EDC0` | 71 |
| 62 | [查看资料](ui_windows/window_62.md) | `54EDF0` | 22 |
| 63 | [迷你发信](ui_windows/window_63.md) | `54EE20` | 16 |
| 64 | [资料回复展示](ui_windows/window_64.md) | `54EE50` | 21 |
| 65 | [黑名单管理](ui_windows/window_65.md) | `54EE80` | 71 |
| 66 | [冒险者日志](ui_windows/window_66.md) | `54EEB0` | 25 |
| 67 | [自身摊位](ui_windows/window_67.md) | `54EF58` | 48 |
| 68 | [浏览摊位](ui_windows/window_68.md) | `54EF88` | 43 |
| 69 | [晶石商店69](ui_windows/window_69.md) | `54ECC0` | 70 |
| 70 | [晶石商店70](ui_windows/window_70.md) | `54ECF0` | 70 |

36/37只算一组元素。共享原型、共享回调及同地址载荷的别名关系以对应正文为准，不按文件数量计算实现进度。

## 公共与共享

- [公共窗口运行期与移动父链](ui_windows/shared_runtime.md)
- [物品、宠物与拖动共享依赖](ui_windows/shared_items.md)
- [对话父路由与BZ回复](ui_windows/shared_dialog_routes.md)
- [资料共享交互与父路由](ui_windows/shared_profile.md)
- [战斗目标选择与命令计时共享依赖](ui_windows/shared_battle.md)
- [窗口36/37共享原型与运行合同](ui_windows/shared_36_37.md)
- [窗口38/41共享数据与存放合同](ui_windows/shared_38_41.md)
- [窗口43/44交易选择与打开链](ui_windows/shared_43_44.md)
- [窗口62/63资料按钮与迷你发信链](ui_windows/shared_62_63.md)
- [窗口67/68共享摆摊合同](ui_windows/shared_67_68.md)
- [窗口69/70共享晶石商店合同](ui_windows/shared_69_70.md)

## 旧选择ID索引

本表列已有明细的57C368旧选择编号，不是全99条handler完成清单，也不映射同号现代窗口。共享明细保留单一来源；“本体已有”只指该页或明确内部链，旧表外层分派、独立数据生产和运行验收仍待。旧64/66等已补本体按当前状态登记，不再列为待实现。

| 旧选择ID | 明细 | 当前本体状态 |
|---|---|---|
| 1 | [地图](ui_windows/legacy_map_panel.md) | 本体/构造已接 |
| 2 | [人物三页](ui_windows/legacy_profile_panels.md) | 三页及外层已有本体 |
| 3 | [设置主菜单](ui_windows/legacy_settings_menu.md) | 本体/构造已接 |
| 4 | [退出](ui_windows/legacy_logout_panel.md) | 完整本体已有 |
| 5 | [聊天设置](ui_windows/legacy_chat_settings.md) | 本体/构造已接 |
| 6 | [预设编辑](ui_windows/legacy_preset_editor.md) | 完整本体/同源聊天地址借用已有 |
| 9 | [鼠标设置](ui_windows/legacy_mouse_settings.md) | 本体/构造已接 |
| 10 | [快捷键说明](ui_windows/legacy_hotkey_help.md) | 本体/构造已接 |
| 11 | [扫描线设置](ui_windows/legacy_scanline_settings.md) | 本体/构造已接 |
| 12 | [按钮设置](ui_windows/legacy_button_settings.md) | 本体/构造已接 |
| 14 | [物品](ui_windows/legacy_inventory.md) | 完整本体/初始化已有 |
| 15 | [金钱](ui_windows/legacy_money.md) | 完整本体/初始化已有 |
| 16 | [拆分数量](ui_windows/legacy_item_split.md) | 完整本体已有；屏幕键盘专属功能沿既有排除 |
| 17 | [十八动作菜单](ui_windows/legacy_emote_menu.md) | 完整本体/真实动作请求已有 |
| 18、19 | [技能列表与详情](ui_windows/legacy_skill_panels.md) | 18列表/19详情完整本体及D64E13生产、第四按钮适配已接；外层/运行仍待 |
| 20 | [宠物列表](ui_windows/legacy_pet_list.md) | 初始化/列表与内嵌4F7870完整本体/悬停适配已接；真实外层和运行仍待 |
| 21 | [宠物资料](ui_windows/legacy_pet_detail.md) | 三页/初始化/外层已有 |
| 22 | [宠物改名](ui_windows/legacy_pet_rename.md) | 完整本体已有 |
| 23 | [图鉴列表](ui_windows/legacy_album_list.md) | 完整本体/初始化已有 |
| 24 | [图鉴详情](ui_windows/legacy_album_detail.md) | 完整本体/预览已有 |
| 25 | [制造配方](ui_windows/legacy_craft_recipes.md) | 完整本体/页数查询已有 |
| 26 | [制造执行](ui_windows/legacy_craft_execution.md) | 完整本体/初始化/动作已有 |
| 27 | [制造材料](ui_windows/legacy_craft_materials.md) | 完整本体/材料生产已有 |
| 28 | [制造结果](ui_windows/legacy_craft_result.md) | 完整本体已有 |
| 29 | [镶嵌执行](ui_windows/legacy_gem_execution.md) | 完整本体/初始化/动作已有 |
| 30 | [镶嵌材料](ui_windows/legacy_gem_materials.md) | 完整本体/初始化/校验已有 |
| 31 | [加工选择](ui_windows/legacy_item_work_selection.md) | 完整本体/资格/预览已有 |
| 32 | [加工名称](ui_windows/legacy_item_work_name.md) | 完整本体/名称生产/发送已有 |
| 33 | [加工结果](ui_windows/legacy_item_work_result.md) | 完整本体已有 |
| 34 | [FP不足提示](ui_windows/legacy_fp_notice.md) | 完整本体已有 |
| 35 | [技能目标名单](ui_windows/legacy_skill_target_list.md) | 本体/真实名单生产消费已有 |
| 36 | [技能属性目标](ui_windows/legacy_skill_status_targets.md) | 本体/ND同源视图已有 |
| 37 | [技能结果](ui_windows/legacy_skill_result.md) | 完整本体/再次使用已有 |
| 38 | [采集](ui_windows/legacy_gather_panel.md) | 完整本体/动画依赖已有 |
| 39 | [采集结果](ui_windows/legacy_gather_result.md) | 完整本体/预览依赖已有 |
| 40、41 | [战斗与点数结果](ui_windows/legacy_result_panels.md) | 完整本体/计时初始化已有 |
| 42 | [联系人登记本](ui_windows/legacy_mail_contact_list.md) | 本体/目标/未读动画生产已有 |
| 43 | [联系人排序](ui_windows/legacy_mail_contact_sort.md) | 本体/原保存链已有 |
| 44 | [联系人删除](ui_windows/legacy_mail_contact_delete.md) | 本体/确认/一次保存已有 |
| 45 | [邮件发送模式](ui_windows/legacy_mail_delivery_mode.md) | 本体/旧42导航生产已有 |
| 46 | [普通邮件多人撰写](ui_windows/legacy_mail_normal_compose.md) | 本体/选择/逐项发送及保存已有 |
| 47 | [普通邮件阅读](ui_windows/legacy_mail_read.md) | 本体/已读/导航及发送入口已有 |
| 48 | [送信宠物](ui_windows/legacy_mail_pet_list.md) | 本体/旧49宠物选择生产已有 |
| 49 | [宠物邮件撰写](ui_windows/legacy_mail_compose.md) | 本体/草稿/赠品消费/发送已有 |
| 50 | [邮件赠品](ui_windows/legacy_mail_gift_items.md) | 本体/同源物品与选择生产已有 |
| 51 | [交易目标名单](ui_windows/legacy_trade_target_list.md) | 消费本体/页号已有；名单/数量/页数生产仍缺 |
| 52 | [交易](ui_windows/legacy_trade_panel.md) | 六页/外层/预览已有 |
| 53 | [普通对话](ui_windows/legacy_basic_dialogs.md) | 完整本体/初始化已有 |
| 54 | [普通行选择](ui_windows/legacy_basic_dialogs.md) | 完整本体/同源输入已有 |
| 55 | [技能选择](ui_windows/legacy_skill_choice.md) | 本体/初始化/同源图号已有；正文完整生产待 |
| 56 | [技能学习](ui_windows/legacy_skill_learning.md) | 本体/真实正文/费用消费已有 |
| 57 | [技能遗忘](ui_windows/legacy_skill_forgetting.md) | 本体/数值表/正文消费已有 |
| 58 | [宠物技能目录](ui_windows/legacy_pet_skill_catalog.md) | 初始化/消费/页号及图号/页数存储已有；目录定义和实际生产仍缺 |
| 59 | [候选宠物](ui_windows/legacy_pet_skill_catalog.md) | 本体/初始化/真实回复候选消费已有 |
| 60 | [宠物技能槽](ui_windows/legacy_pet_skill_catalog.md) | 本体/初始化/真实技能记录消费已有 |
| 61 | [宠物技能学习确认](ui_windows/legacy_pet_skill_catalog.md) | 完整本体已有；目录三项缺口保留 |
| 62 | [买卖选择](ui_windows/legacy_shop_choice.md) | 初始化/同源图号已有；父mode0第二命令待证 |
| 63 | [买入列表](ui_windows/legacy_buy_list.md) | 完整本体/初始化/共享详情已有 |
| 64 | [卖出列表](ui_windows/legacy_sell_list.md) | 完整本体/初始化已有；独立生产待 |
| 65 | [买入确认](ui_windows/legacy_buy_confirmation.md) | 完整本体已有 |
| 66 | [卖出确认](ui_windows/legacy_sell_confirmation.md) | 完整本体/原物品索引发送已有 |
| 67 | [治疗](ui_windows/legacy_healing.md) | 完整本体/真实记录及同源图号已有 |
| 68 | [鉴定列表](ui_windows/legacy_identify_list.md) | 本体/初始化/详情已有；独立生产待 |
| 69 | [鉴定确认](ui_windows/legacy_identify_confirmation.md) | 完整本体/原索引发送已有 |
| 70 | [修理列表](ui_windows/legacy_repair_list.md) | 本体/初始化/详情已有；独立生产待 |
| 71 | [修理确认](ui_windows/legacy_repair_confirmation.md) | 完整本体/原索引发送已有 |
| 72 | [存放](ui_windows/legacy_storage_items.md) | 四页/外层已有；容量及展示生产仍待 |
| 73 | [留言](ui_windows/legacy_message_board.md) | 完整本体/四记录消费/输入生命周期已有 |
| 74 | [交易选择](ui_windows/legacy_exchange_dialog.md) | 完整本体/初始化已有 |
| 75 | [交换列表](ui_windows/legacy_barter_list.md) | 完整本体/初始化/详情已有 |
| 76 | [交换确认](ui_windows/legacy_barter_confirmation.md) | 完整本体/固定字体测宽已有 |
| 77 | [物品目标名单](ui_windows/legacy_item_target_list.md) | 完整本体/LY真实名单消费已有 |
| 78 | [物品目标属性](ui_windows/legacy_item_use_status.md) | 完整本体/同源借用已有 |
| 79 | [物品刻印](ui_windows/legacy_item_engraving.md) | 完整本体/初始化已有 |
| 80 | [埋起物品](ui_windows/legacy_item_burial.md) | 完整本体/独立选择状态已有 |
| 81 | [固定文字提示](ui_windows/legacy_text_notices.md) | 完整本体/固定旧字节已有 |
| 82 | [固定文字提示](ui_windows/legacy_text_notices.md) | 完整本体/固定旧字节已有 |
| 83 | [交出/接受对话](ui_windows/legacy_give_receive_dialog.md) | 完整本体/初始化已有；模式生产仍待 |
| 84 | [交出/接受数量](ui_windows/legacy_give_receive_quantity.md) | 完整本体/初始化/详情已有 |
| 85 | [交出/接受确认](ui_windows/legacy_give_receive_confirmation.md) | 完整确认本体/格式后首字节1与共享数据消费已接；外层/完整流程和运行仍待 |
| 86 | [公会邮件赠品](ui_windows/legacy_guild_mail_gift_items.md) | 本体/同源物品消费及选择生产已有 |
| 87 | [公会宠物邮件](ui_windows/legacy_guild_pet_mail_compose.md) | 本体/独立输入/赠品与宠物消费已有 |
| 88 | [公会送信宠物](ui_windows/legacy_guild_mail_pet_list.md) | 本体/候选/选择生产已有 |
| 89 | [公会邮件阅读](ui_windows/legacy_guild_mail_read.md) | 本体/已读/回复导航已有 |
| 90 | [公会普通多人邮件](ui_windows/legacy_guild_normal_mail_compose.md) | 本体/草稿/200项选择/发送保存已有 |
| 91 | [公会发送模式](ui_windows/legacy_guild_delivery_mode.md) | 完整本体/导航已有 |
| 92 | [公会成员删除](ui_windows/legacy_guild_contact_delete.md) | 本体/权限消费/确认已有 |
| 93 | [公会排序](ui_windows/legacy_guild_contact_sort.md) | 本体/原排序保存链已有 |
| 94 | [公会通讯录](ui_windows/legacy_guild_contact_list.md) | 本体/目标/未读动画及导航已有 |
| 95 | [公会资料编辑](ui_windows/legacy_guild_details_edit.md) | 本体/三输入/职级权限及缓存已有 |
| 96 | [公会职级修改](ui_windows/legacy_guild_rank_change.md) | 本体/目标/资格消费与请求已有 |
| 98 | [旧日志](ui_windows/legacy_log_viewer.md) | 完整装载/页面及两入口适配已有；外层路由/运行仍待 |
| 99 | [黑名单编辑](ui_windows/legacy_blacklist_editor.md) | 本体/同源输入/名单及持久化调用已有 |

旧公共机制与无单独选择ID的初始化：[旧面板公共运行期](ui_windows/shared_legacy_panels.md)、[455490组初始化](ui_windows/legacy_455490_initialization.md)、[底部栏初始化](ui_windows/legacy_bottom_bar_initialization.md)。音乐/音效设置继续随音频暂缓，不因未列明细而排除。

## 迁移基线

拆分前完整正文固定于 `810c6f75`，保留全部详细证据章节并校验唯一迁移；不另建整份归档。跨文件的历史“前节/上述/下一步”需结合证据导航与该快照理解。

```powershell
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show 810c6f75:cg2d_re/doc/cg2d_ui_window_runtime.md
```


## WMB01 窗口更新低字节状态修正（2026-10-05）

[具名SPEC](cg2d_window_mouse_byte_spec.md#window-mouse-byte-source-state)与[原103条/368B](cg2d_window_mouse_byte_evidence.json)对应现有UpdateWindows_473F80。
焦点与尾部两次D68DB4赋值改为只写低W8，尾部阈值判断取低U8；修复整W32写入/比较偏差，保留高三字节和唯一owner。
原窗口/元素、共享索引重读、焦点/拖动、update/draw返回传递和快捷键/预设热键顺序及504120入口保持。
只登记本根语义、处置和限定静态；旧99表外层/键表、全回调与alias/寿命、输入/画面/ABI及运行仍未验收。


## MIC01 鼠标阶段、命中与元素事件合同（2026-10-05）

[三根合同](cg2d_mouse_input_chain_spec.md#mouse-input-chain-source-state)及[原327条/1106B](cg2d_mouse_input_chain_evidence.json)补472F00、473E40、473560的原新映射。
既有X命中短路/跳过Y读取、阶段1回调后重读及阶段2入口限定、type4 X后Y、入口element借用与按钮低W8映射可追。
三根源码及共享helper保持，不新增旧表外层或输入框架；两void根只针对当前caller弃返回，不假设原EAX固定值或全ABI等价。
这只是基线/登记收口，全上游/父/间接目标、owner/alias、完整输入/寿命、旧路由及运行仍未验收。

## BVL01公共旧面板原生取样及表用途边界（2026-10-08）

[BVL01完整原基线](cg2d_legacy_panel_native_sampling_spec.md#legacy-panel-native-baseline)、[实施决定](cg2d_legacy_panel_native_sampling_spec.md#legacy-panel-native-decision)与[原证](cg2d_legacy_panel_native_sampling_evidence.json)：503160完整231条735B/四页，13直接九目标/一初始化间接调用、22跳五RET，97真实CALL只定位96已归父/一未归点。

一cpp两片段恢复入口x→width、关闭一次非拥有action供判断/清理，原normal数值及全部公共页签/提示/关闭等余体保持。共享99表仍借LegacyAudio唯一owner的action/dirty，模块名称不作为新音频恢复授权；现不写音频源/头，不加原+04回调分派或新表。43个未归属幂/浮点入口保既有专用x87裁决，无直接入向不能当不可达/宿主已迁移证明。

1061原格式/%s/128B及97入向等全部旧字段保，新增单adapted F处置/公共UI流15→16和两限定I-S，各19真实来源；U04/U11/V03/R04仍open。预期F914/E1090，语义1766/有效1759、合同1876（50.98%）及R7已审467余2594保持。所有父/callee、生产和正常owner-alias寿命、旧输入外路由/机器ABI链接/运行及R8未验，音频暂停/原格式明文/RPLAN.24和完整门禁保持。

## BVM01旧面板绘制/查询三根及共用桥实修（2026-10-08）

[BVM01原基线](cg2d_legacy_panel_button_bridge_sampling_spec.md#panel-button-bridge-baseline)、[取样裁决](cg2d_legacy_panel_button_bridge_sampling_spec.md#panel-button-bridge-decision)及[原证](cg2d_legacy_panel_button_bridge_sampling_evidence.json)：三完整绘制/关闭/按钮根252条662B/五页、10直接五目标/零间接/15跳4RET，373实际CALL仅引用定位；503990既有BSP基线复用，本步25条只取样邻窗不重计身份。

桥接四源字段按原顺序取齐，不引入新原地址、helper或第二记录owner；Query core不用图号尾作文本，既有局部转换保。三原根已正确，补真实原新/处置即可；原descriptor全清与mode差异有完整指令证据，不为统一而删除、改造正常像素或新增拒绝。七整源只一桥片段，其余六源/声明与BVL公共入口保。

新1766..1768/三adapted F，公共UI流16→19/两限定I-S各18来源，I仅现有桥接一个取样片段；U04/U11/V03/R04仍open。预期语义1769/有效1762、合同1879（51.06%）、F917/E1092，R7已审467余2594及pending18/closed11/全叶0/globalfalse保。原格式明文/音频暂停/99项同owner及0/100空handler、完整门禁和执行限制保持；全373调用处/其他父-callee、生产/正常owner-alias寿命/像素/机器ABI链接运行与R8未验。

## BVN01现代快捷键完整根与四处实修（2026-10-08）

[BVN01基线](cg2d_window_shortcut_publication_spec.md#window-shortcut-baseline)、[实施决定](cg2d_window_shortcut_publication_spec.md#window-shortcut-decision)、[验收](cg2d_window_shortcut_publication_spec.md#window-shortcut-acceptance)及[原证](cg2d_window_shortcut_publication_evidence.json)。

完整原根1507条5500B/22页、256直接32目标、零间接224跳5RET；唯一473F80父复用WRS01已有原证。本步只现有快捷键根四片段：范围>5/<1、聊天>count/<0判定保存后才发布原S32共享值，再据同判定分流；四片段承接两套键位八原点。原范围轮换/聊天回退、计时180、键位/修饰键优先、逐键新读取、四窗口与动作/RNG/音效顺序及其它正文保持。

新增1769/F:000C7AE0和adapted处置，公共UI19→20、两个限定I/S，各15来源；零新实现函数/helper/owner。U04/U11/V03/R04仍open；预期语义1770/有效1763、基线1880（51.09%）、F918/E1094。完整输入生产/焦点、32callee/owner寿命、机器ABI/画面运行与R8待验，旧99表路由不因此闭合；音频暂停、原格式明文及执行限制保持。

## BVO01十二快捷请求/选项根与同源字节承载（2026-10-08）

[BVO01基线](cg2d_shortcut_request_option_sampling_spec.md#shortcut-request-option-baseline)、[实施与类型差异](cg2d_shortcut_request_option_sampling_spec.md#shortcut-request-option-decision)、[验收](cg2d_shortcut_request_option_sampling_spec.md#shortcut-request-option-acceptance)及[原证](cg2d_shortcut_request_option_sampling_evidence.json)。

十二完整根511条1915B/16页、62直接16目标/13时钟间接、72跳15RET，36真实CALL定位；六个473060共尾跳保原归属，不新F。六正确请求与十二包装全文保持，两个现有helper三片段恢复同源BYTE读及值样本→socket→低W16/S32消费，原U16位写和实际网络返回保。

原生玩家结构是稀疏承载；原473060 W32高W16随即被掩掉。本步按具名裁决保现有同源U16消费值，不读下一1A0冒充原196，不扩字段/补零或宣机器访问宽度等价。471DF0只六BYTE窗口取证，401EA0一跳及既有219复用，不验全父/读取核心。

新1770..1781/十二adapted F，公共UI20→32，两限定I/S各28来源，I仅两既有helper实修；六正确请求不计新I。预期语义1782/有效1775、基线1892（51.41%）、F930/E1096；U04/U11/V03/R04仍open。全部父/16callee、输入/资源生产、正常owner-alias寿命、机器ABI/画面运行与R8待验；音频暂停、原格式明文及执行限制保持。

## BVP01九UI依赖与原MIS基线（2026-10-08）

[BVP01基线](cg2d_emote_chat_mis_chain_spec.md#emote-chat-mis-baseline)、[保持与登记决定](cg2d_emote_chat_mis_chain_spec.md#emote-chat-mis-decision)、[验收](cg2d_emote_chat_mis_chain_spec.md#emote-chat-mis-acceptance)及[原证](cg2d_emote_chat_mis_chain_evidence.json)。

十完整原根241条879B/11页、20直接11目标/两时钟间接、49跳11RET及168真实CALL定位；两十八项S32动作表、原MIS命令/三字段、十八BYTE选中与512聊天首字节清理可对照。44B170第二chunk在4A7700，既有源拆分不新原身份。七相关整源/十三正文正确保持，零源码修改/零新I。

新1782..1791十adapted F，公共UI32→41仅前九、NETWORK67→68仅MIS，两域各一个限定S/29来源；保原混合返回/void包装、图号及受限动作、Y/X/协议发送→本地动作→fresh tick，MIS每次容量先持样再格式化/以fresh buffer追加和完整返回，聊天模式/尺寸和清理副作用保。预期语义1792/有效1785、基线1902（51.68%）、F940/E1098；四任务仍open，所有父/callee、生产/正常owner-alias寿命、ABI/互通与画面运行待，音频暂停/原格式明文和执行限制保持。

## BVQ01五依赖根与四判定阶段实修（2026-10-08）

[BVQ01基线](cg2d_window_close_mode_publication_spec.md#window-close-mode-baseline)、[实施决定](cg2d_window_close_mode_publication_spec.md#window-close-mode-decision)、[验收](cg2d_window_close_mode_publication_spec.md#window-close-mode-acceptance)及[原证](cg2d_window_close_mode_publication_evidence.json)。

五完整原根159条494B/六页、九direct四目标/零间接18跳4RET及21真实CALL与两旧键表数据引用定位；原两表各71槽、随机位置编号10/11/12/14/27有据。单CPP四既有正文四判定阶段/五文本替换，10号首判定/技能窗口存在/S32负号/S16阈值先持样再共享发布并据同判定分流。正确分类关闭保原fresh槽参数；所有其它Restore/fresh实例、六原RAND区间/消费、业务位置修正/回退与真实返回保持。

新1792..1796五adapted F、公共UI41→46，两限定I/S各21来源，I仅四阶段实修/正确分类关闭不计新I；无新实现函数/helper/API/owner。预期语义1797/有效1790、基线1907（51.82%）、F945/E1100；四任务仍open，所有21调用处/各父/四callee、生产和正常owner-alias寿命、生成EFLAGS/机器ABI/画面运行仍待。网络原格式明文、音频暂停/RPLAN.24与执行限制保持。

## BVR01释放/全关闭/marker三根（2026-10-08）

[BVR01基线](cg2d_window_release_source_sampling_spec.md#window-release-source-baseline)、[实施决定](cg2d_window_release_source_sampling_spec.md#window-release-source-decision)、[验收](cg2d_window_release_source_sampling_spec.md#window-release-source-acceptance)及[原证](cg2d_window_release_source_sampling_evidence.json)。

三完整根184条565B/四页、14direct六目标/一个release间接、28跳3RET；11CALL/四尾跳及七类释放表分列有据。FreeWindow单既有根六阶段实修：焦点/载荷按原顺序读取，同data/text/数组样本判断和释放，类型2经唯一Yobi native边界承接直接deathFlag=1恢复原写、撤本调用点借通用MarkDeath产生的额外空判断。原release后fresh字段/槽、两W32 flags、循环数量与释放次序保，通用MarkDeath正文/载荷与窗口接口头及全关闭/marker两正确主体不动。

新1797..1799三adapted F、公共UI46→49，两限定I/S各24来源，I仅单根六阶段，两个正确主体不计新I；两个CPP/一个Yobi公共头，一个必要native边界helper/API不新原身份，无新owner/保护或故障模拟。预期语义1800/有效1793、基线1910（51.90%）、F948/E1102；U04/U11/V03/R04仍open。MIS62/七W32占用清理/71标记和实际返回保；全部release目标/15入向及父/callee、生产和正常owner-alias寿命、机器ABI/画面运行待，原格式明文/音频暂停/RPLAN.24及执行限制保持。

## BVS01表情请求类型边界修正（2026-10-08）

[BVS01基线](cg2d_emote_chat_mis_chain_spec.md#emote-opaque-baseline)、[实施决定](cg2d_emote_chat_mis_chain_spec.md#emote-opaque-decision)、[验收](cg2d_emote_chat_mis_chain_spec.md#emote-opaque-acceptance)及[状态](cg2d_emote_chat_mis_chain_spec.md#emote-opaque-source-state)。原证JSON追加`emote_opaque_boundary_correction`。

BVS01修复4FA8E0表情请求跨opaque YobiAction直接取imageNo的真实源码缺口；原4FA9C1一次W32读取由既有GetYobiActionField2A8原样承接，仍仅在附件44为空且计时门禁已过后同action读取。一个CPP一个调用表达式，零新函数/helper/API/owner/保护；正文其余、现有getter/公共头和两动作表保持。复用BVP01完整117条437B/两页基线，本步只重取一实际指令；既有1782/F:000FA8E0和公共UI49不重计，两个限定I/S，原格式明文/音频暂停/RPLAN.24保。只验本源码边界，未宣编译链接、机器ABI或运行通过，四任务仍open。

BVP01七整源/十三正文保持是原命名快照历史；本次只纠正4FA8E0当前跨opaque直接访问，既有getter/声明及其它源保持。语义1800/有效1793、F948、公共UI49、基线1910（51.90%）保持，仅E1102→1104；编译、链接与运行未执行。

## 2026-10-10 WDP01：完整元素绘制父及原门槛

[WDP01](cg2d_window_element_draw_pipeline_spec.md#window-draw-source-state)补齐4729B0完整277条846B及七槽分发表/51CALL与34父来源。六组十三处恢复入口坐标、入口图号缓存、四低W16裁剪、两图形坐标路径、数字载荷四字段和四动画颜色BYTE；所有原visible/图号/焦点/context/text/数字null门槛、六业务分发/每元素新窗口count/尾层3清队列与范围提示保持。共用Yobi helper改读选中BYTE，S32存储/其他调用源/GetField20正确body不改，不外推其他父；四整源/四注释精确静态通过。旧1932全部CAC08来源/表/范围子记录保后追加，一F处置/四限定S零I，INPUT/UIRUNTIME/RENDER/YOBI各一成员；51外父/元素载荷/资源动画字体输入日志提示全生产消费及owner寿命、机器ABI/真实像素文字动画与运行仍待，整体未完成。

## 2026-10-10 WND01：数字绘制与步进业务初始化

[WND01](cg2d_window_number_draw_spec.md#number-draw-source-state)补齐4771B0/477460/477170三完整原根255条774B、两E8与38CALL/21父来源，120W32/480B四表与源常量吻合。旧86初始化尾跳、553098原槽及现MSVC构造接线复用，不新建同一身份或恢复私有CRT机制。测宽明确先运行步进乘字节长度再读扣宽；原S8样式/颜色、flags正反字节绘制/四符号/未知BYTE仍步进、descriptor、四S32初始化序和所有其他源保持，七关联注释。三新语义F、UIRUNTIME/RENDER各三成员、两限定S零I；全父/数字载荷字符串图号步进队列生产消费及owner寿命、构造退出时序/机器ABI/真实数字符号像素与运行仍待，整体未完成。

## 2026-10-10 WIC01：输入原型四模式分发

[WIC01](cg2d_window_input_context_dispatch_spec.md#input-create-source-state)补齐476570/45D720/45D760三完整原根128条367B、三E8/四跳七RET及80CALL内2/外78/53父来源和四槽旧表。三个正确正文保持：既有x64带标记引用承接原高字节拒绝值，mode1落2并两次新取原型context、mode3颜色取入口context，原坐标/滚动零/四BYTE模式/颜色/字体发布与真实context返回有据；本链不分配新载荷。零逻辑组/六注释，旧1935全CAC08来源/表/范围保后追加，两新语义三F处置、INPUT/UIRUNTIME各三成员/两限定S零I。53外父/原型和context/输入载荷全生产消费与owner寿命、其余依赖/机器ABI/实际初始化切换焦点编辑和运行仍待，整体未完成。

## 2026-10-10 WPL01：完整七类载荷创建与owner

[WPL01](cg2d_window_payload_creation_spec.md#window-payload-source-state)补齐471440完整384条1159B、13E8/五目标/40跳五RET及一次update间接、52CALL/21父来源和七槽旧表。七类载荷是既有真实结构：外层由UI管理，type3另own256B文本、type7借字符串、type5借context、type2 action由Yobi管理；旧1797/BVR释放正文保。现有唯一helper改借初始action及现有action/图号槽，恢复首flags目标、延迟图号指针和图号指针→action槽→值的交错，一组三处/四注释；声明/定义/唯一调用同步，其余创建/释放/其它source和类型保持。旧1929全CAC08历史保后追加、一F处置/四域各一成员/四限定S零I；全父/七载荷及嵌套对象生产消费与owner寿命、SDK/机器ABI/实际创建更新释放和运行仍待，整体未完成。

## 2026-10-10 WCR01：窗口表整理和直接关闭

[WCR01](cg2d_window_compact_close_spec.md#window-compact-close-source-state)补齐4730A0/472160两完整原根102条312B、两实际E8同一释放目标/16跳两RET和两个真实父入向。整理根两个状态分支共用原W32样本，直接关闭发布和flags置bit1使用同一保存记录地址；71槽压紧、关闭后左移清末、等层前插/每轮新layer、重排末fresh清位、S32计数及既有释放/父正文保持。两组/三处/四注释，三整源及两个结构静态差异通过；两新语义F、两adapted处置、UIRUNTIME两成员和一限定S零I，九任务仍open。完整状态/窗口表生产消费与回调alias-owner寿命、机器ABI/优化后读取、实际关闭重排和运行仍待，整体未完成。

## 2026-10-10 WSB01：滚动边界与事件回调

[WSB01](cg2d_window_scroll_callback_spec.md#window-scroll-source-state)补齐4741A0/4B0F10两完整原根76条211B、一E8/六跳两RET，七实际CALL和十九代码地址发布有据；四滚动原父未归属不猜。整数组xref按实际偏移核58项57536B、16滚动原型（15type4/1type1）和1125零回调槽，未把整表引用当CALL。滚动根上下界和差值三个原阶段读取顺序已明确，初始data持样/低BYTE与W32标志/每阶段现场数组和原边界调整保持；零回调确为33C0/C3只返0，正确body保。两整源三段/四注释及三个结构静态差异通过；两新F/两adapted处置，INPUT/UIRUNTIME各两成员、两限定S零I、九任务open。全父/间接绑定/跨类型载荷生产消费与alias-owner寿命、实际滚动画面、机器ABI和运行仍待，整体未完成。

## 2026-10-10 WSP01：滚动值到页码生产

[WSP01](cg2d_window_scroll_position_spec.md#window-scroll-position-source-state)补齐两个父及位置换算三完整F原根，复用四既有X代码体，共162条431B/十二E8/十一跳八RET。四曾无IDA父的滚动调用已按旧1959/1961/1992/1999 code_identity解析，保空inventory_id和原X用途，不造F。六父各原非零/恰1/statebit3条件、参数58/950/count/count-10/90和保存S32返回保持；聊天换算后新取count，既有焦点子不改。474270两整数除法和非正maximum分支保持，仅明确先当前/前项height一取样段；两整源十四注释/六父与共享子正文保持。两个新原型和31换算入向内五/外26有据；三新F/三处置、INPUT/UIRUNTIME各三成员、两限定S零I。全部间接绑定/外父/数据生产消费与owner、机器ABI/实际拖动画面和运行仍待，整体未完成。

## 2026-10-10 WWS01：滚轮余量与坐标

[WWS01](cg2d_window_wheel_scroll_spec.md#window-wheel-source-state)补齐4743B0完整89条231B、四E8/两目标/六跳两RET与29实际CALL；F32因子原四BYTE、已批宿主数学/原double-F32落存和转换图保持。三段修正原余量/像素Y持样跨转换、高度/上下界以及尾部flags取样和data+8再+0发布；原入口data借用/wheel零、max及position钳制/余量零、S32返回保持。两整源三段/两注释及三个类型静态差异通过；旧1533 setter/CRT/聊天父复用不改。一新F/一处置，INPUT/UIRUNTIME各一成员，两限定S零I；完整29父/间接绑定/生产消费与owner、宿主数值和滚轮手感/画面、机器ABI及运行仍待，整体未完成。

## 2026-10-10 WDF01：拖动及焦点父

[WDF01](cg2d_window_drag_focus_spec.md#window-drag-focus-source-state)补齐472D20完整152条466B、九E8/三目标/十八跳两RET和唯一更新父CALL。四组明确原一次W32 state/phase门槛、四低BYTE消费、矩形字段/focus/count和两全窗height-width-y-x取样；W32储宽和正确坐标调用后值/held目标地址/new窗口写回保持。Read_401EC0对应原401EA0，旧219/矩形命中/更新父复用不改。两整源四组/两注释与两个类型静态差异通过，原bit800/2000和结果1/2、无新增十项上限保持；一新F/一处置，INPUT/UIRUNTIME各一成员，两限定S零I/九任务open。完整输入生产消费/所有Crypto和父/owner、机器ABI与生成BYTE访问、实际拖动焦点及运行仍待，整体未完成。

## 2026-10-10 RHL01：矩形命中共享消费

[RHL01](cg2d_rectangle_hit_hover_spec.md#rectangle-hit-hover-source-state)补齐5049A0/401FC0两完整原根60条150B、六实际E8/四目标、五跳三RET。矩形根135实际CALL/102父纳盘；比较叶16CALL/三父均仅消费AL，不提升全父验收。单组四处接回现有比较器，四次原短路/读取及RNG、半开边界、负layer仍命中和可描边后S32 1保持；比较叶正确body不改，四整源仅两已有头/四调用/四注释。旧Less/绘框和悬停父复用不改；原SETLE只覆盖AL，高24位不记等价。两新F/两处置，INPUT两成员、RENDER/UIRUNTIME各一成员，三限定S零I、十任务open。完整父/所有图像和坐标生产消费、队列及alias-owner寿命、机器ABI/像素与运行仍待，整体未完成。

## 2026-10-10 HWT01：窗口6完整消费

[HWT01](cg2d_hover_text_consumption_spec.md#hover-text-consumption-source-state)补齐窗口6悬停消费4731E0及IME查询/描述借用459090/4590D0三完整原根237条910B、五实际E8/四目标、两IAT调用、十跳三RET。4734F0是旧822根内共用尾，仅现平台helper，不造原F；窗口6原型+36真实main绑定复用并保持，五IME实际CALL有据。两处颜色源明确低BYTE访问，存储仍S32/W32；主/副标题含NUL复制、原payload借用跨复制、逐项fresh窗口显示、IME尾和四槽清理及S32 1保持。三IME正文正确不改，四整源只两BYTE源/八注释；旧822保全历史追加完整合同，两新F/三处置，INPUT/UIRUNTIME各三成员，两限定S零I/十任务open。三API进口仍unassessed，不据调用点提升完整进口验收；全生产消费/容量和alias-owner寿命、SDK/机器ABI/实际输入显示与运行仍待，整体未完成。

## 2026-10-10 DLP01：会话启动/绑定/退出阶段

[DLP01](cg2d_drag_session_production_lifecycle_spec.md#drag-session-source-state)补齐475DA0/476490两个会话生产、4763B0焦点绑定、4763E0五槽重置、476410取消及476500完成六完整原根129条483B/六页、零E8/两实际寄存器回调、十三跳十一RET；116直接CALL/50父只核点与四条后邻，不代完整外父/返回消费。阶段1/3生产的原焦点/回调/来源及S16坐标写序、已有kind的两槽绑定、五槽重置保来源/坐标、取消回调后新phase到2/6及完成入口4/5到0保持。六正文正确不改，三整源仅十二定义/声明注释；四S32结果与两void当前适配保持，原残留EAX/全部父机器ABI仍待。六新F/六处置，INPUT/UIRUNTIME各六成员，两限定S零I/十任务open；全事件生产消费/全部回调和payload/alias-owner寿命、真实拖动/退出与运行待，整体未完成。

## 2026-10-10 ISP01：窗口14特殊选物

[ISP01](cg2d_item_selection_event_pipeline_spec.md#item-selection-source-state)补齐对话29选物入口、两显示回调、列表借用与两坐标六完整原根828条2692B/十六页，43实际E8/22目标、64跳12RET。26直接CALL和八代码回调发布有据，发布不是直接CALL；旧920完整598条取代本轮仅相邻读取范围，但全部旧历史保持。五组八处对齐：主四BYTE源、伴随state BYTE/循环count先取再推进、X/Y元素与窗口取序；入口及列表正确body保，两整源六正文其余/四类型和原字段储宽保持，十二注释。原mode1/2门槛、两独立退出/占用递减/恢复回调、未清256B/独立16B cap15及原发送和绘制/结果保持。五新F/旧920追加、六处置，INPUT/UIRUNTIME各六成员、RENDER两成员，三限定S零I/十任务open；全父/间接回调、物品/宠物/占用/协议与alias-owner寿命、机器ABI/像素与运行待，整体未完成。

## 2026-10-10 SLC01：选择器范围/索引/计数显示

[SLC01](cg2d_selector_local_coordinate_hover_spec.md#selector-local-source-state)补齐局部X/Y、宠物范围/索引借用及连续悬停显示五完整原根243条771B/八页、九E8五目标/26跳12RET与26直接CALL。四S32坐标表112B与现源逐项一致；八宠物判定消费者七TEST AL/一AND FF，只证明低W8，不记原EAX高24位等价。悬停父四组五处对齐：两轴window坐标先element、两flags BYTE及未饱和alpha计数BYTE；原5*W32>=256饱和255/三descriptor、第一选择器宠物号/原非负门槛与计数/索引尾序保持，四叶正确body不改。四整源仅四组五处/十注释，四类型/表项/字段储宽保持。4C6790一父两CALL首点尾RET转发原残留EAX，当前void保持，完整上行/机器ABI未据本步等价。五新F/五处置，INPUT/UIRUNTIME各五成员、RENDER一成员，三限定S零I/十任务open；全父/callee/生产消费与alias-owner寿命、实际悬停/像素与运行待，整体未完成。

## 2026-10-10 SFD01：选择器页状态与元素绑定

[SFD01](cg2d_selector_frame_binding_spec.md#selector-frame-source-state)补齐逐帧分发、两模式元素/回调绑定及首次初始化三完整原根362条1298B/七页，20实际E8/十目标、20跳十RET，五入CALL与三表48B有据。两次按钮绘制明确各自四字段取序、先Y后X及descriptor0共借，三组三处/六注释；绑定与初始化正文正确保持。原四页/step递减/-182业务钳制、非零模式槽19不写及callback0先slot18保持。已知悬停残留EAX经分发/初始化返回，两窗口回调立即MOV EAX,1/RET，局部上行不消费已有依据；全机器ABI/所有callee及alias-owner寿命、实际像素与运行仍待。旧1988/CAC18历史与旧2285/SLC01历史保持追加，两新F/三处置、三限定S零I/十任务open，整体未完成。

## 2026-10-10 SPG01：隐藏/显示/回调发布

[SPG01](cg2d_selector_page_visibility_spec.md#selector-page-source-state)补齐玩家/宠物显示、隐藏、五页状态切换、位置发布及mode切换六完整原根323条937B/八页，19E8九目标/35跳十一RET，24入CALL与20B五case表有据。两组五处明确CC40D4低BYTE和四回调目标先于回调读取；其余五正文与原动态slot/每轮fresh宠物count、页发布/隐藏/图号及step21、模式页3切换保持。两整源只上述差异/十二注释，三类型与原六坐标表不改。旧1987/CAC18保历史追加、五新F/六处置、三限定S零I/十open；原残留EAX及7340仅AL=1不外推全ABI弃用，全外父/callee、生产消费/alias-owner寿命、生成读取/机器ABI/像素与运行待，整体未完成。

## 2026-10-10 PSP01：宠物名称负重与预览消费

[PSP01](cg2d_selector_pet_preview_spec.md#selector-pet-source-state)补齐S8序号发布、五排序对映射、名称负重及同宠物预览四完整原根238条690B/六页，九E8七目标/21跳六RET，八入CALL有据。两测宽分别按原字体→文本、文本→字体取样；预览沿原replacement→previous读取，现detail helper增加取序参数，四个既有单参调用保previous→replacement。四组四处/十注释，四整源其它正文与两正确叶/五类型保持。复用BOT01已核layer BYTE消费，高24位不记整ABI等价。旧892格式与历史追加、三新F/四处置、四限定S零I/十open；全部排序对/宠物/文本/descriptor生产消费、callee/外父及alias-owner寿命、生成取样/机器ABI/预览像素与运行待，整体未完成。

## 2026-10-10 SIC01：初始化字段与固定退出对象

[SIC01](cg2d_selector_construction_exit_spec.md#selector-construction-source-state)补齐选择器完整构造及两个固定构造/退出包装五原根，220条1049B/八页、20E8两目标/24跳两RET。构造/共享退出各有29条136B EH；488420主chunk另有六B间隙，指令字节与跨度分开，488430为其内部共享尾，不造F。同预览十成员构造和逆序析构、flags/开关/图号/三descriptor发布保持，仅一组一处在构造入口持址供Construct/Finish。两静态对象0/1独立，退出固定借用，不重读可改preview；四包装/四helper及两独立std::atexit正确body保持，十注释。四旧历史追加、一新F五处置、四限定S零I/十open；零模板初值有据，空Xrefs不代不可变或alias闭包，完整EH/ABI/全生产消费、全应用正常owner与快速退出/真实生命周期和运行仍待，整体未完成。

## 2026-10-10 SEV01：页签/开关/旋转与结果消费

[SEV01](cg2d_selector_button_event_spec.md#selector-event-source-state)补齐页签、预览开关、装备开关及双方向旋转四完整原根352条1028B/八页，九E8五目标/46跳九RET，十入CALL/64B图表有据。四组四处对齐中心取样/descriptor先图号、目标先回调和循环尾selected先推进/count后取；两正确body/五类型/十caller和既有声效接口保持，八注释。原失效宠物索引false早返保开关/回调部分写、双旋转条件独立及两声/方向原余数保持。六CALL仅消费AL、四CALL不消费，未记高位或全机器ABI等价。四新F四处置/四限定S零I/十open；全生产消费、所有外父/callee/alias-owner寿命、实际按钮/声音像素及运行仍待，整体未完成。

## 2026-10-10 SAP01：属性页及页签结果

[SAP01](cg2d_selector_attribute_pet_tab_spec.md#selector-attribute-source-state)补齐附加属性显示与两宠物页签三完整原根391条1176B/八页、十三E8五目标/39跳六RET。24项384B属性表、两22B包装父及六原型callback数据槽有据。两组四处明确descriptor持址和三种文字坐标读取序，原24项/14行/第8换列/512B未清及GBK/MSVC格式保持；两页签正确正文保持，库存成功低AL归一、战斗成功恒0分界明确。六外层注释及一局部枚举说明，不改公开接口、类型、caller及原型绑定。旧921格式历史追加、两新F/三处置/三限定S零I十open；完整生产消费/回调分发、callee/alias-owner寿命、生成取样/实际画面交互与运行待，整体未完成。

## 2026-10-10 IER01：库存装备与属性回复

[IER01](cg2d_equipment_attribute_reply_spec.md#equipment-reply-source-state)补齐库存装备与属性回复两完整根662条2141B/十二页，61E8二十七目标/84跳五RET、七装备data引用及一个回复入CALL有据。四组四处落实两物品低BYTE/动作BYTE接口与selected先于hover写回；原拖放/使用/特殊回调/occupancy/分页/绘图及已批GBK/MSVC格式保持。属性回复正确正文保持，首field失败本根仅清used，既有提取器写首文本首字节NUL、两文本/单空格门槛与field4..27逐项写24值，原TgX分发/当前LF明文沿既有合同。四源限定差异/四注释，旧919格式历史追加、一新F两处置/四限定S零I八open；完整生产消费、callback/callee/网络发送、alias-owner寿命/生成取样与实际操作运行待，整体未完成。

## 2026-10-10 BEI01：战斗装备和空槽

[BEI01](cg2d_battle_equipment_interaction_spec.md#battle-equipment-source-state)补齐战斗装备与背包空槽两个完整原根764条2531B/十三页，56E8十八目标/103跳五RET。四组四处明确三BYTE及区域描边颜色/窗口/元素读取序；保原宠物禁用页、玩家拖放/卸武器、容量/未鉴定/无武器失败提示、两个Q命令和发布序，末两次独立禁图不合并。空槽正确正文保持，slot8..27/used W16，仅统计唯一玩家区。两data回调与十九空槽入CALL/后邻有据，其余十七调用点/十五外父仅来源登记。四源限定diff/四注释、旧924格式历史追加、一新F两处置/五限定S零I九open；全数据生产/动态callback/19外父和命令、callee/alias-owner寿命/SDK/生成取样、实际交互像素声音与运行待，整体未完成。

## 2026-10-10 BIP01：完整背包与目标发布

[BIP01](cg2d_battle_item_interaction_spec.md#battle-item-source-state)补齐战斗背包完整1175条4105B/十九页，62E8十九目标/154跳两RET和两data绑定有据。三逻辑组十二片段落实六BYTE、五grid及一区域取序，保未加8的容量标志索引/level0经验成本、十五技能/技术/级限、两Q一I格式及目标pending/直接使用发布序和原失败分支/重复禁图。BQI01目标选择/BEI01空槽真实提供者复用，正文字节不改、不重复取完整根；两整源只十二片段/两注释。旧900格式/CAB30及既有adapted/既有UI成员保历史追加，无新增F/处置；四功能新增本根、五限定S零I九open。完整生产消费/后续目标消费者/所有callee与alias-owner寿命、SDK/生成取样/实际操作运行待，整体未完成。

## 2026-10-10 IDE01：拖动入口及分类

[IDE01](cg2d_item_drag_entry_callback_spec.md#item-drag-source-state)补齐物品拖动入口/回调/分类三完整根219条719B/五页，23E8十一目标/33跳八RET。十七CALL及四后邻、两个callback data持址有据，只登记外父来源。两组六处明确动作和两点击BYTE/三绘制分支descriptor持址；mode先发布/原结果透传、两次fresh分类、失效reset/取消后仍继续绘制、玩家/宠物/C44来源及两轴CryptInt序保持。入口/分类正确正文保，两整源仅六片段/六注释，DLP01真实会话及既有目标消费者保持。三新F三处置/五限定S零I九open；完整调用方/动态回调/数据生产/alias-owner寿命、SDK/生成取样/实际拖放与运行待，整体未完成。

## 2026-10-11 IMT01：数量回调与两组模式

[IMT01](cg2d_item_move_request_spec.md#item-move-source-state)补齐丢物请求/直接拆分/模式间移动/数量消费四完整原根310条957B/七页，30E8十五目标/28跳八RET，十七CALL/四后邻及一个callback data持址有据。四正文正确保持，仅八合同注释；原四容量先于格式/输出重读、同mode两次fresh组读取、Ctrl和反转开关、直接source先target/拆分target先source、窗口41/38优先及转义后状态/Y-X发送序保持。原S32返回透传与数量lambda只具名副作用边界，不推完整callback ABI/互通。旧934格式历史追加、三新F四处置/三限定S零I八open；完整输入/数量/数据/缓冲区生产、callback/callee/所有外父和alias-owner寿命、SDK/生成读取/实际操作及运行待，整体未完成。

## 2026-10-11 QIP01：窗口42状态发布

[QIP01](cg2d_quantity_window_creation_spec.md#quantity-source-state)补齐数量框创建、展开与窗口42状态发布三个完整原根282条873B/五页；低BYTE flags与两次窗口指针取样两组三处对齐，动画正确正文保持。callback持址、两CryptInt、S16完成判定/W16末增和原失败直发布有据。三新F三adapted、YOBI/UI各三成员，两限定S零I八open；确认/取消/关闭及完整callback/owner/ABI、画面声音与运行待，整体未完成。

## 2026-10-11 QVC01：数量面板与增减回调

[QVC01](cg2d_quantity_window_callbacks_spec.md#quantity-callback-source-state)补齐数量初始化、输入激活、确认面板和增减按钮五完整原根230条714B/六页；三读取合同对齐，三正确正文保持，五定义/五声明十注释。原callback非空调用后重取窗口，关闭/声效后再次重取窗口并格式化；保两次原上限钳回及增减门槛。旧937格式历史追加、四新F五adapted，INPUT/UI各五成员/RENDER四成员，三限定S零I八open；完整取消/关闭/释放owner及全callback/ABI/运行待。

## 2026-10-11 WCL01：关闭标记与实际回收

[WCL01](cg2d_window_close_front_release_spec.md#window-close-source-state)补齐共享关闭按钮、原端返回叶和前置元素三完整原根70条212B/三页，三正确正文保持，仅七合同注释。窗口42关闭只置bit15/声57，前置只置bit13；真实资源回收复用窗口整理→FreeWindow，release返回叶原端只返回1，不能记作完整释放成功。三新F三adapted，INPUT两/UI三/RENDER两成员，三限定S零I九open；完整取消/共享调度/owner、ABI及运行仍待。

## 2026-10-11 NPI01：共享数值面板完整合同

[NPI01](cg2d_numeric_panel_input_spec.md#numeric-input-source-state)补齐数值面板474550完整540条1675B/九页，逐键四次CryptInt短路、数字/Backspace优先级、四鼠标操作与AL0/1/3返回有据。两逻辑组五处坐标及绘制取样对齐；42图号和既有业务正文/原上限钳回保持。本根没有Enter/Esc处理，不添加退出键行为。一新F一adapted、三限定S零I八open，完整输入生产消费/owner、ABI及运行待。

## 2026-10-11 TQN01：共享交易数量父链

[TQN01](cg2d_trade_quantity_panel_spec.md#trade-quantity-source-state)旧949格式历史追加共享交易面板4B1260完整524条1693B/九页，31E8十一目标/一个间接业务callback、41跳一RET。五逻辑组六处低宽坐标/滚动标志、滚动值先元素与前一滚动框坐标取样对齐；原数值确认结果、callback哨兵适配、四格式及S32 0/2返回保持。一处置、INPUT/UI/RENDER各一成员，三限定S零I八open；完整共享生产消费/业务callback、owner/ABI及交易运行待。

## 2026-10-11 SDM01：存取金额及请求提供者

[SDM01](cg2d_storage_amount_exchange_spec.md#storage-amount-source-state)补齐存取金额4ABB20及xD发送512620两个完整原根310条1075B/六页；两正确正文保持，仅四合同注释。原存/取上限、AL确认和钳回、转义后字段/Y-X发送及发送后fresh金额记录/清槽/双文本有据，xD五整数+文本各保六次容量和输出取样。旧894格式历史追加、一新F两adapted、三限定S零I八open；69CALL/58父仅局部来源，完整金额/协议生产消费与owner、ABI及互通运行待。

## 2026-10-11 MDR01：数量callback和丢钱按钮

[MDR01](cg2d_money_drop_request_spec.md#money-drop-source-state)补齐丢钱回调/按钮和woz发送三完整原根153条494B/四页，14E8九目标/15跳三RET。仅动作标志改既有BYTE读取一处；按钮/发送正确正文保持，三次fresh窗口匹配、金额同步和三字段容量/token/输出原序保。六合同注释/三新F三adapted、四限定S零I八open；完整回调/共享数据owner、ABI与互通运行待。

## 2026-10-11 MPR01：玩家金额与三上限生产

[MPR01](cg2d_player_attribute_money_reply_spec.md#player-money-source-state)补齐主属性aPc和附加属性UU两个完整回复根676条2005B/十一页，70E8六目标/60跳两RET。主根六CryptInt的临时三字段先取样后发布已对齐；原掩码/字段推进、当前金额和三上限、文本长度门槛及步速同步保持。两新F两adapted、三限定S零I八open；当前金额读取/存取与丢钱消费共用真实PlayerRuntimeSlots，完整数据/owner、ABI及回复互通运行待。

## 2026-10-11 SGM01：两组存放数据与金额

[SGM01](cg2d_storage_group_data_reply_spec.md#storage-group-source-state)旧1970历史追加存放A/G/I/P完整解析根1002条2900B/十六页，86E8四目标/39跳四RET及双switch表有据。正确正文保持，仅两合同注释；原物品/宠物部分失败续行、I/P字段步进和A/G金额解析后fresh组写入明确。同组金额与存取界面共享原字节区，一个adapted、两限定S零I八open；六CALL同父仅局部消费，完整父/全部数据owner、ABI及互通运行待。

## 2026-10-11 IDR01：描述行及页数生产

[IDR01](cg2d_item_description_rows_spec.md#item-description-source-state)补齐4E02B0完整物品描述生产根83条202B/两页，一E8/九跳两RET。正确正文保，仅两合同注释；32行、仅行首清零、last前一行回扫及S32页数发布明确。两CALL同SGM01父和两个消费者页数邻窗有据；一个新F/处置，UI一成员/一限定S零I八open，完整消费者/owner/ABI及运行待。

## 2026-10-11 SEP01：窗口41/38物品消费

[SEP01](cg2d_storage_external_item_panels_spec.md#storage-item-panel-source-state)旧971/973补齐两个外部物品面板完整904条2885B/十五页，50E8十六唯一目标/94跳两RET。恢复四处flags低BYTE及两根十二组十四处持址/坐标取样，原短路、窗口38类型门槛、41快捷取回、页数与图后数量消费保持。两原prototype 35/3槽及一个code-offset发布有据；历史追加/两处置、三限定S零I八open，完整owner/全部描绘与请求互通/ABI及运行待。

## 2026-10-11 SPD01：实际物品预览链

[SPD01](cg2d_storage_preview_dispatch_spec.md#storage-preview-source-state)补齐窗口41/38物品描述预览分发及48B350标题/八行描述消费，含窗口38三条公共尾，共201条667B/六页、21E8十个唯一目标/16跳七RET。恢复两个滚动flags的BYTE读取，两组两处/四注释；W32标题字段和四次鼠标CryptInt、原预览及真实返回保持。两个旧历史追加/两个新F/四处置，三限定S零I八open，全部owner/文字及画面/ABI与运行待。

## 2026-10-11 TDR01：完整描述绘制入口

[TDR01](cg2d_tooltip_description_render_spec.md#tooltip-description-source-state)补齐层5描述提示绘制48B810完整108条283B/两页，六E8四目标/五跳一RET，九CALL八父局部来源有据。现正文正确全保，仅两合同注释；标题/正文不同截断阶段、跨行色彩、可选footer及背景最后入队/真实返回明确。当前文字含NUL复制与分色前缀释放的正常接线有据，一新F/处置、两限定S零I八open，完整owner/像素画面/ABI及运行待。

## 2026-10-11 SRD01：三阶段窗口回复分派

[SRD01](cg2d_storage_reply_dispatch_spec.md#storage-reply-source-state)旧1964历史追加49D2E0完整回复父515条1238B/九页，52E8三十六目标/38跳含三间接分派/30RET及五原表有据。正文正确保，仅两合同注释；三阶段分派、六原存放CALL与四现合并CALL、fresh窗口/creator/global重入及Y-X RNG顺序完整静态核对。BZ当前两bridge/路由保持，一个处置、两限定S零I八open，全部callee/动态owner/机器ABI及互通画面运行待。

## 2026-10-11 SOA01：存放开窗动作及完成回调

[SOA01](cg2d_storage_opening_actions_spec.md#storage-opening-source-state)补齐窗口41/38两个creator及两个完成callback，四完整原根416条1208B/八页、24E8九目标/12跳八RET。恢复两处saved flags BYTE、四CryptInt先取后发布及两个mode1中心X持值，八组八处/六注释；两正确callback、失败同步开窗但仍返空、原S16完成/W16末递增保持。四新F/处置、两限定S零I八open，完整manager/owner/ABI与画面运行待。
