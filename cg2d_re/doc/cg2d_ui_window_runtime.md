# UI 窗口运行期与移动父链 SPEC

> 本文件仅保留总览与按ID索引。各窗口明细位于 `ui_windows/window_<ID>.md`；共享合同保留唯一来源，维护规则见[文档组织 SPEC](ui_windows/README.md)。

## 当前对齐摘要

2026-10-02按用户要求对齐，压缩前总览与完整进度固定于 `c53a6de1`。以下是当前状态；明细中的“仍缺/仅声明/未接”按各自提交时点理解，不批量删除原证据。

- 现代窗口承载71槽，主原型记录70条（含0号空记录，36/37共享）。window id 1..70的69组去重元素、133个去重主回调均有唯一定义，直接缺口为**0组/0个**。这不是旧选择表覆盖数、传递依赖数或全客户端完成率。
- 通用创建/展开、生命周期、输入/绘制、启动/保存，以及504120→473F80→4C7AE0窗口更新/快捷链已有本体。507140完整命中已接MainLoop与加载caller，相关命中索引/颜色已有真实生产，不再列为缺失本体。
- 窗口10七模式/30元素、26五模式/14元素、28八元素/快照、32入场提示及其他现代窗口的专属合同见下表。窗口24音量依赖52C3D0仍随音频暂缓；主回调存在不等于传递依赖齐全。
- 战斗404DB0完整257表项/200动作入口及其创建/恢复依赖已有本体；43CBB0→4383F0→4383B0→43B360→43B610→427470已接真实调用与回调，25消息与193个非默认技能分派已有入口。424620仍只有真实声明，其var_50未写值待决，不能据此称完整战斗可运行。[战斗合同](ui_windows/shared_battle.md)
- 旧面板公共层及多条人物、设置、制造/加工、交易/存放、图鉴/宠物、商店内部链已有本体，见独立旧选择索引。57C368 handlerToken仍是原地址证据编号，实际外层调用尚待；D64E13的4FF650生产、旧20/62/85及旧日志等未决项保持。
- 网络108路分发已有本体，直接回复尚余**1个murD随音频暂缓**；音频不是范围排除。0/0/1仅表示上述三类直接缺口，不含旧UI、所有数据生产、真实流程或完整链接缺口。
- 本次只复点源码定义/接线、文档链接与差异；不重新验证历史算法，不构建、不新增或执行测试、不运行客户端、不检查产物。整体SPEC、完整输入/收发、画面及资源生命周期均未完成验收。

当前任务与六项未写值边界见[唯一进度](../plan/cg2d_full_reconstruction_progress.md)和[总SPEC待决项](../plan/cg2d_full_reconstruction_plan.md#43-当前未闭合与明确待决)。文档索引不替代原端合同，旧选择ID也不等同现代window id。

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

本表列已有明细的57C368旧选择编号，不是全99条handler完成清单，也不映射同号现代窗口。共享明细保留单一来源；“完整本体已有”只指该页或明确列出的内部链，旧表外层分派及运行验收仍待。旧64卖出列表及真实确认依赖是当前下一步，不从买入页外推其合同。

| 旧选择ID | 明细 | 当前本体状态 |
|---|---|---|
| 1 | [地图](ui_windows/legacy_map_panel.md) | 本体/构造已接 |
| 2 | [人物三页](ui_windows/legacy_profile_panels.md) | 三页及外层已有本体 |
| 3 | [设置主菜单](ui_windows/legacy_settings_menu.md) | 本体/构造已接 |
| 4 | [退出](ui_windows/legacy_logout_panel.md) | 完整本体已有 |
| 5 | [聊天设置](ui_windows/legacy_chat_settings.md) | 本体/构造已接 |
| 9 | [鼠标设置](ui_windows/legacy_mouse_settings.md) | 本体/构造已接 |
| 10 | [快捷键说明](ui_windows/legacy_hotkey_help.md) | 本体/构造已接 |
| 11 | [扫描线设置](ui_windows/legacy_scanline_settings.md) | 本体/构造已接 |
| 12 | [按钮设置](ui_windows/legacy_button_settings.md) | 本体/构造已接 |
| 14 | [物品](ui_windows/legacy_inventory.md) | 完整本体/初始化已有 |
| 15 | [金钱](ui_windows/legacy_money.md) | 完整本体/初始化已有 |
| 16 | [拆分数量](ui_windows/legacy_item_split.md) | 完整本体/键盘依赖已有 |
| 18、19 | [技能列表与详情](ui_windows/legacy_skill_panels.md) | 18列表已有；19详情4FF650待决 |
| 20 | [宠物列表](ui_windows/legacy_pet_list.md) | 未写局部的跨调用命中稳定性待证 |
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
| 37 | [技能结果](ui_windows/legacy_skill_result.md) | 完整本体/再次使用已有 |
| 38 | [采集](ui_windows/legacy_gather_panel.md) | 完整本体/动画依赖已有 |
| 39 | [采集结果](ui_windows/legacy_gather_result.md) | 完整本体/预览依赖已有 |
| 40、41 | [战斗与点数结果](ui_windows/legacy_result_panels.md) | 完整本体/计时初始化已有 |
| 52 | [交易](ui_windows/legacy_trade_panel.md) | 六页/外层/预览已有 |
| 62 | [买卖选择](ui_windows/legacy_shop_choice.md) | mode0第二命令未写，尚未实现 |
| 63 | [买入列表](ui_windows/legacy_buy_list.md) | 完整本体/初始化/共享详情已有 |
| 65 | [买入确认](ui_windows/legacy_buy_confirmation.md) | 完整本体已有 |
| 72 | [存放](ui_windows/legacy_storage_items.md) | 四页/外层已有；容量及展示生产仍待 |
| 74 | [交易选择](ui_windows/legacy_exchange_dialog.md) | 完整本体/初始化已有 |
| 75 | [交换列表](ui_windows/legacy_barter_list.md) | 完整本体/初始化/详情已有 |
| 76 | [交换确认](ui_windows/legacy_barter_confirmation.md) | 完整本体/固定字体测宽已有 |
| 78 | [物品目标属性](ui_windows/legacy_item_use_status.md) | 完整本体/同源借用已有 |
| 79 | [物品刻印](ui_windows/legacy_item_engraving.md) | 完整本体/初始化已有 |
| 83 | [交出/接受对话](ui_windows/legacy_give_receive_dialog.md) | 完整本体/初始化已有；模式生产仍待 |
| 84 | [交出/接受数量](ui_windows/legacy_give_receive_quantity.md) | 完整本体/初始化/详情已有 |
| 85 | [交出/接受确认](ui_windows/legacy_give_receive_confirmation.md) | 首次分行读未写文本，尚未实现 |
| 98 | [旧日志](ui_windows/legacy_log_viewer.md) | 402020补零待批；4021A0仍待完整取证 |

旧公共机制与无单独选择ID的初始化：[旧面板公共运行期](ui_windows/shared_legacy_panels.md)、[455490组初始化](ui_windows/legacy_455490_initialization.md)、[底部栏初始化](ui_windows/legacy_bottom_bar_initialization.md)。音乐/音效设置继续随音频暂缓，不因未列明细而排除。

## 迁移基线

拆分前完整正文固定于 `810c6f75`，保留全部详细证据章节并校验唯一迁移；不另建整份归档。跨文件的历史“前节/上述/下一步”需结合证据导航与该快照理解。

```powershell
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show 810c6f75:cg2d_re/doc/cg2d_ui_window_runtime.md
```
