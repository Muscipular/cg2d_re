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
