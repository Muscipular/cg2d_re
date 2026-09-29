# UI 窗口运行期与移动父链 SPEC

> 本文件仅保留总览与按ID索引。各窗口明细位于 `ui_windows/window_<ID>.md`；共享合同保留唯一来源，维护规则见[文档组织 SPEC](ui_windows/README.md)。

## 当前对齐摘要

- 71槽窗口承载、70个主原型（36/37共享）、拖动表、通用创建/展开、生命周期、输入/绘制与启动/保存已有本体。
- 窗口10的七模式、五开窗入口、三主回调及30元素/18载荷来源已齐；13种初始元素回调均有唯一本体，完整合同与四组静态回映摘要见[窗口10](ui_windows/window_10.md)。
- 窗口26的五模式创建、三主回调及完整14元素/12载荷来源已齐；十种元素回调均有本体，七补充按钮见[窗口26](ui_windows/window_26.md)。D6976C有唯一承载，但507140生产仍缺。
- 三技能命令4974C0/497E30/497FF0、两关闭496AE0/496F10、快捷497000及按钮4975A0/498160已有本体并传入真实回调；窗口28的更新/交互/绘制、两个快照生产者及完整8元素已落位，见[窗口28](ui_windows/window_28.md)。
- 43C4E0初始化、43C040清对象标志、4982C0倒计时、498520分派、入场/附加效果/血条依赖及窗口32已有本体；M读取/退出、队列/就绪、完整预扫描、事件收尾、骑乘/装备附件、特效、相册匹配、426FC0、43B9B0/424500、效果游标/动作73/74及转生/属性特效已有本体。本轮补齐[附加效果消费与动作235](ui_windows/shared_battle.md#battle-equipment-effect-consumer)：406390完整五块、422BC0无模式装备入口、416DB0真实调用链已落位，复用既有音效，不接部分404DB0。[424620](ui_windows/shared_battle.md#battle-skill-dispatch-evidence)未写入值的可达性仍待核，补零待确认，父函数未落本体。旧常规视图仍20槽；424620/427470完整分派、438290/404DB0角色生产与动作播放、四W16/1080及59F9E0配置生产、43CBB0、M父调度/初始化、快照/载荷生产父链、D64E13的4FF650生产仍未闭合。
- 当前仍缺**36组元素、53个主回调**，网络另余**1个murD回复随音频暂缓**；数量不是全客户端完成率。音频暂缓不等于排除。
- 文档拆分已提交9438fac2；本轮只静态核对上述新增指令/字节、源码与差异，未重验全部历史实现，不构建、不新增或执行测试、不运行客户端、不检查产物。实际创建、交互、文本、像素、完整技能发送及整体SPEC仍未运行验收。

当前结论与后续任务以[唯一进度](../plan/cg2d_full_reconstruction_progress.md)为准。原端证据和验收标准不因拆分或压缩而删除；各明细中的早期“未实现open”“主表仅extern”等仅描述原提交时点。

## 按窗口ID索引

下列34个ID已有明细或明确共享合同，元素与主回调已齐仅指当前静态实现/绑定，不等于完整流程验收。其余ID未补写推测明细，也不因未列出而排除范围。

| window id | 明细 | 原型地址 | 元素数 |
|---|---|---|---|
| 10 | [技能、技术与宠物技能](ui_windows/window_10.md) | `54E420` | 30 |
| 11 | [制造与物品加工](ui_windows/window_11.md) | `54E450` | 31 |
| 12 | [窗口12操作与结果](ui_windows/window_12.md) | `54E480` | 15 |
| 13 | [窗口13操作与结果](ui_windows/window_13.md) | `54E4B0` | 14 |
| 14 | [库存与装备](ui_windows/window_14.md) | `54E4E0` | 24 |
| 26 | [战斗命令](ui_windows/window_26.md) | `54E720` | 14 |
| 27 | [战斗物品](ui_windows/window_27.md) | `54E7E0` | 22 |
| 28 | [战斗宠物切换](ui_windows/window_28.md) | `54E810` | 8 |
| 32 | [战斗入场提示](ui_windows/window_32.md) | `54E840` | 2 |
| 36 | [通用对话共享实例](ui_windows/window_36.md) | `54E870` | 19 |
| 37 | [通用对话创建入口](ui_windows/window_37.md) | `54E870` | 19 |
| 38 | [外部物品存放](ui_windows/window_38.md) | `54EB40` | 6 |
| 39 | [计时提示](ui_windows/window_39.md) | `54E8A0` | 1 |
| 41 | [物品与宠物存放](ui_windows/window_41.md) | `54EB10` | 38 |
| 42 | [数量输入](ui_windows/window_42.md) | `54E510` | 8 |
| 43 | [交易](ui_windows/window_43.md) | `54EBD0` | 20 |
| 44 | [交易目标选择](ui_windows/window_44.md) | `54E8D0` | 19 |
| 45 | [文本目标选择](ui_windows/window_45.md) | `54E900` | 19 |
| 46 | [属性目标选择](ui_windows/window_46.md) | `54E930` | 34 |
| 50 | [四记录对话](ui_windows/window_50.md) | `54EC30` | 36 |
| 51 | [文本列表](ui_windows/window_51.md) | `54EB70` | 26 |
| 52 | [商店选择](ui_windows/window_52.md) | `54E960` | 3 |
| 53 | [商品买卖](ui_windows/window_53.md) | `54EC60` | 66 |
| 54 | [宠物与技能选择](ui_windows/window_54.md) | `54EC90` | 107 |
| 56 | [外观选择](ui_windows/window_56.md) | `54ED20` | 15 |
| 60 | [资料分类选择](ui_windows/window_60.md) | `54ED90` | 162 |
| 61 | [资料搜索结果](ui_windows/window_61.md) | `54EDC0` | 71 |
| 62 | [查看资料](ui_windows/window_62.md) | `54EDF0` | 22 |
| 63 | [迷你发信](ui_windows/window_63.md) | `54EE20` | 16 |
| 64 | [资料回复展示](ui_windows/window_64.md) | `54EE50` | 21 |
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

## 迁移基线

拆分前完整正文固定于 `810c6f75`，保留全部详细证据章节并校验唯一迁移；不另建整份归档。跨文件的历史“前节/上述/下一步”需结合证据导航与该快照理解。

```powershell
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show 810c6f75:cg2d_re/doc/cg2d_ui_window_runtime.md
```
