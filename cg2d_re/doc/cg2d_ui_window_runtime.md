# UI 窗口运行期与移动父链 SPEC

> 本文件仅保留总览与按ID索引。各窗口明细位于 `ui_windows/window_<ID>.md`；共享合同保留唯一来源，维护规则见[文档组织 SPEC](ui_windows/README.md)。

## 当前对齐摘要

- 71槽窗口承载、70个主原型（36/37共享）、拖动表、通用创建/展开、生命周期、输入/绘制与启动/保存已有本体。
- 窗口10的七模式、五开窗入口、三主回调及30元素/18载荷来源已齐；13种初始元素回调均有唯一本体，完整合同与四组静态回映摘要见[窗口10](ui_windows/window_10.md)。
- 窗口26的五模式创建、三主回调及完整14元素/12载荷来源已齐；十种元素回调均有本体，七补充按钮见[窗口26](ui_windows/window_26.md)。D6976C有唯一承载，但507140生产仍缺。
- 三技能命令4974C0/497E30/497FF0、两关闭496AE0/496F10、快捷497000及按钮4975A0/498160已有本体并传入真实回调；窗口28的更新/交互/绘制、两个快照生产者及完整8元素已落位，见[窗口28](ui_windows/window_28.md)。
- 43C4E0初始化、43C040清对象标志、4982C0倒计时、498520分派、入场/附加效果/血条依赖及窗口32已有本体；M读取/退出、队列/就绪、预扫描、事件收尾、附件/特效、相册匹配、技能目标队列、效果消费及动作73/74/208至216/218至223/235/237已有本体。[41CD00完整特效工厂](ui_windows/shared_battle.md#battle-effect-factory)、五初始化与十三回调已有本体；48类、25个原调用点实参与公共尾已核，第三槽采用S32 Y/原生T*联合类型。工厂内部递归已接；[214/215](ui_windows/shared_battle.md#battle-actions-214-215)两完整动作已接真实特效与动作推进，308/30F复用既有颜色字段。[216与重播种/平铺依赖](ui_windows/shared_battle.md#battle-action-216)已有本体，保留20槽计数、同源随机种子、六型图号和共享相位；[218与对象克隆](ui_windows/shared_battle.md#battle-action-218)已落位，97成员覆盖原730B复制范围、联合槽保留原生指针，1060/13AC/13B0接入渐变完成路径。[219/220与居中跟随特效](ui_windows/shared_battle.md#battle-actions-219-220)已有本体，保留原图恢复/同步终帧、完成尾差异及1064发布后跟随。[221至223](ui_windows/shared_battle.md#battle-actions-221-223)已有本体，保留HP写回差异、追踪移动及窗口28的41402E快照接线。404DB0父分派及1028/1030/13E4生产仍缺，不以动作本体存在代表完整战斗链闭合。[424620](ui_windows/shared_battle.md#battle-skill-dispatch-evidence)未写入值的可达性仍待核，补零待确认。旧常规视图仍20槽；424620/427470分派、438290/404DB0角色生产与完整动作播放、四W16/1080及其他载荷/59F9E0配置生产、43CBB0、M父调度/初始化、快照上游、D64E13的4FF650生产仍未闭合。
- [动作224所需状态特效恢复链](ui_windows/shared_battle.md#battle-state-effect-restore)七入口已有本体：4062A0坐标距离、1054/105C/1068三个创建入口、两个真实回调及422370恢复；599条指令已静态核对。保留失败前状态写入、成对清位与缩放比较差异。224本体、1060专用载荷和404DB0父回调仍未完成，不记为动作224已实现。
- [404DB0基础依赖](ui_windows/shared_battle.md#battle-core-actions-0-6)的0至6/236与残影、[7至13及对象平铺](ui_windows/shared_battle.md#battle-actions-7-13)、[14完整命中](ui_windows/shared_battle.md#battle-action-14)、[26至31受击动作](ui_windows/shared_battle.md#battle-actions-26-31)、[16/17投射命中、退场与事件探测](ui_windows/shared_battle.md#battle-actions-16-17)已有本体，骑乘双T*前缀已有生产证据。总更新257表项对应200个不同动作入口，重复映射及公共尾已取证；15、18/19等其他动作、224及父分派仍缺，投射物创建所需完整404DB0不以部分回调替代。
- [投射物投影与相对方向](ui_windows/shared_battle.md#battle-projection-dependencies)三依赖已有本体，创建绑定完整418D80回调；244指令/16处原调用、四坐标源及五常量已核，保留先截目标再转向、两次额外位移、父死亡后帧尾及负基准两次相减。419390/4195F0及其完整404DB0依赖仍缺。
- [20至25投射/回旋动作与两个角度依赖](ui_windows/shared_battle.md#battle-actions-20-25)已有本体，1232指令/97处原调用、20B图号表及三个F32常量已核；保留20/22免疫门禁差异、22效果推进及3/7帧延迟、23固定caster/动态other与动作追加、25坐标分别重取。19的4083AA仍依赖419390，不记创建链或父分派完成。
- [32至40动作与散射特效](ui_windows/shared_battle.md#battle-actions-32-40)十一入口已有本体，660指令/51处原调用及五F32常量已核；保留34四边反弹/完成循环、35每帧尾迹、36至38播放后计时、39三次播放及散射失败/RNG顺序。散射回调已接，不表示完整404DB0已接。
- [41至56动作与FP扣减](ui_windows/shared_battle.md#battle-actions-41-56)十七入口已有本体，592指令/58处原调用、64B分派表及两个F32常量已核；保留倒地/附件收尾、两设备退场、宠物目标重取、缩放比较和HP/FP三字段写序。完整404DB0及其余动作/创建链仍缺。
- [57至69、71/72动作及70依赖取证](ui_windows/shared_battle.md#battle-actions-57-72)十五入口已有本体，335指令/36处原调用与64B分派表已核；保留动画完成尾、旧FF0判断、单圈方向修正及哨兵后额外推进。1014只新增S32承载，生产父链仍缺；70的418B70绑定完整404DB0，仅取证未实现。
- [75至82与偏移跟随特效](ui_windows/shared_battle.md#battle-actions-75-82)十入口已有本体，218指令/24处原调用与32B分派表已核。106C经生产/标死证据修正为原生T*，三附属槽同样使用T*；窗口26借用接口同步，保留刷新后重取。完整发布父链及404DB0仍缺，不记完整转生流程完成。
- [83至90与屏幕坐标特效](ui_windows/shared_battle.md#battle-actions-83-90)九入口已有本体，594指令/53处原调用、696B名称图号表及五F32常量已核；保留缩放未舍入比较、固定/重取载荷差异、动态目标、十位置随机顺序及延迟/越界效果推进。完整404DB0与创建父链仍缺。
- [91至98显隐、位移与施法平铺](ui_windows/shared_battle.md#battle-actions-91-98)八入口已有本体，280指令/23处原调用与32B分派已核；保留目标坐标分别重取、旧FF0门禁、推进后目标HP清零及98事件/完成双效果与独立平铺尾。完整404DB0及创建父链仍缺。
- [99至106与复制体创建依赖](ui_windows/shared_battle.md#battle-actions-99-106)中七入口已有本体，814指令/64处原调用、200B序列表及原格式字节已核；保留共享游标-1、逐效果重读、原始CryptInt格式实参及绕目标定位。103/4205F0因绑定完整404DB0仅取证，父分派与创建链仍缺。
- [107至114与抛物线效果消费](ui_windows/shared_battle.md#battle-actions-107-114)中六入口已有本体，1298指令/98处原调用、1025项位置缓存和六型减益表已核；保留旧计数/事件门禁、抛物线原字节槽及动态源对象，已接40F2B1共享表消费。109/112依赖完整创建仍只取证，父分派未闭合。
- [工厂动作、状态与场地特效](ui_windows/shared_battle.md#battle-factory-actions-115-162)及[139/140与背景淡变](ui_windows/shared_battle.md#battle-actions-139-140)已有本体；[154至156/159/160与数值比较](ui_windows/shared_battle.md#battle-actions-154-160)补齐缩放链和两种不同数值收尾，七入口822指令/90处原调用已核。115至162动作本体现已齐，仍不等于完整404DB0或场景父链闭合；163及其他未列动作仍缺。
- 当前仍缺**36组元素、53个主回调**，网络另余**1个murD回复随音频暂缓**；数量不是全客户端完成率。音频暂缓不等于排除。
- 文档拆分已提交9438fac2；本轮只静态核对上述新增指令/字节、源码与差异，未重验全部历史实现，不构建、不新增或执行测试、不运行客户端、不检查产物。实际创建、交互、文本、像素、完整技能发送及整体SPEC仍未运行验收。

当前结论与后续任务以[唯一进度](../plan/cg2d_full_reconstruction_progress.md)为准。原端证据和验收标准不因拆分或压缩而删除；各明细中的早期“未实现open”“主表仅extern”等仅描述原提交时点。

## 按窗口ID索引

下列54个ID已有明细或明确共享合同，各文件分别记录元素、主回调及接线状态；静态实现/绑定不等于完整流程验收。其余ID未补写推测明细，也不因未列出而排除范围。

| window id | 明细 | 原型地址 | 元素数 |
|---|---|---|---|
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
| 32 | [战斗入场提示](ui_windows/window_32.md) | `54E840` | 2 |
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
| 50 | [四记录对话](ui_windows/window_50.md) | `54EC30` | 36 |
| 51 | [文本列表](ui_windows/window_51.md) | `54EB70` | 26 |
| 52 | [商店选择](ui_windows/window_52.md) | `54E960` | 3 |
| 53 | [商品买卖](ui_windows/window_53.md) | `54EC60` | 66 |
| 54 | [宠物与技能选择](ui_windows/window_54.md) | `54EC90` | 107 |
| 55 | [公会职级选择](ui_windows/window_55.md) | `54E9C0` | 29 |
| 56 | [外观选择](ui_windows/window_56.md) | `54ED20` | 15 |
| 58 | [自身资料](ui_windows/window_58.md) | `54E3F0` | 34 |
| 59 | [自身资料分类选择](ui_windows/window_59.md) | `54ED60` | 32 |
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
