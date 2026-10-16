# twcg_hook 功能盘点与 cg2d_re 迁移候选

盘点日期：2026-10-16。参考项目：`D:/Source/cgmsv/twcg_hook`；目标项目：`D:/Source/cgmsv/cg3a/cg2d_re`。

twcg_hook 通过 DLL 注册回调、特征匹配、函数 Hook 和固定地址补丁修改原 x86 客户端，主要覆盖资源扩展、客户端行为、渲染、物品分页、Lua UI、自动战斗、自动寻路和技能特效。cg2d_re 是 x64／SDL3 工程，迁移需要提取功能逻辑并接入对应源码，不能直接沿用原地址、内联汇编和旧对象布局。

以下编号用于选择后续迁移范围。安装链可达只表示源码存在注册和调用，不代表运行验收；配置和脚本启用条件分别列出。当前仅完成盘点，尚未实施功能迁移，也未批准新的依赖、协议或业务差异。cg2d_re 原函数实现状态以[IDA 全量函数实现表](../plan/cg2d_ida_function_implementation_table.md)为准。

## 基础客户端与资源

| 编号 | 功能点 | 具体行为 |
|---|---|---|
| A01 | 自定义资源组 | 从 `cgmsv.yml` 的 `BinLoader` 序列加载 Graphic、Anime、Coordinate 及对应 Info 文件；文件槽表容量为 32 组。 |
| A02 | 资源容量扩展 | 图形默认上限 250 万，动画默认 30500；配置的 `MAX_GRAPHIC`、`MAX_ANIME` 可覆盖。 |
| A03 | 大偏移文件读取 | 用 64 位文件定位接口承接原 `fseek`；`SEEK_SET` 的原 32 位参数按无符号值解释。 |
| A04 | 扩展地图颜色索引 | 建立 80 万项 `MapNoIndex`；根据图形资源生成地图颜色，并处理 220000～265535 的新地图资源范围。 |
| A05 | 简体化 | 将原客户端静态 Big5 字符串转为 GBK，替换部分文案及字体名；`/notrans` 可关闭。 |
| A06 | 分类文件转换 | 检测并转换 `bin/category.dat` 中的文字，会写回原文件。 |
| A07 | 剪贴板粘贴 | 支持 Ctrl+V、Shift+Insert、`WM_PASTE`；读取 `CF_TEXT` 后逐字节调用原输入追加函数。 |
| A08 | 自动登录与选角色 | 使用 `/uu:`、`/pw:`、`/sv:`、`/cv:` 填写账号密码和服务器；在选人状态选择可用角色槽。 |
| A09 | 启动行为修改 | 跳过 `setup.ini` 读取和 Loading；窗口类名改为 `CROSSGATE`。 |
| A10 | 服务器地址特殊处理 | 对 `taomoli` 做固定地址映射；其他地址在非调试模式下进入 DNS 检查及提示流程，相关分支可能退出客户端。 |
| A11 | 计时与诊断 | 用启动后相对计时替换 `GetTickCount`；支持调试控制台和异常转储；调试模式显示 FPS／场景状态，并定期更新窗口标题。 |

源码依据：[资源加载](D:/Source/cgmsv/twcg_hook/src/hooks/binLoader.cpp:537)、[基础修改](D:/Source/cgmsv/twcg_hook/src/hooks/core.cpp:270)、[文字转换](D:/Source/cgmsv/twcg_hook/src/hooks/lang.cpp:17)、[自动登录](D:/Source/cgmsv/twcg_hook/src/hooks/password.cpp:64)、[Loading 修改](D:/Source/cgmsv/twcg_hook/src/hooks/misc.cpp:18)、[诊断初始化](D:/Source/cgmsv/twcg_hook/src/hook.cpp:94)、[窗口标题与 FPS](D:/Source/cgmsv/twcg_hook/src/hooks/UI/UIExt.cpp:332)。

## 渲染与显示

| 编号 | 功能点 | 具体行为 |
|---|---|---|
| R01 | DX11 替换 DX7 | 替换 `DirectDrawCreateEx` 导入项，返回自建 DirectDraw／D3D7 兼容对象；锁定窗口模式和渲染模式 4。安装失败时退出。 |
| R02 | 自定义分辨率 | 支持 `/width:`、`/height:`；修改地图绘制范围、裁剪、屏幕中心、窗口位置和战斗布局。 |
| R03 | 战斗画面适配 | 战斗背景缩放至窗口大小；角色阵位偏移；倒计时文字按实际宽度居中。 |
| R04 | PNG 图形支持 | 在原 Graphic 数据链识别 PNG，通过 WIC 解码；另支持 Lua UI 外部 PNG 图片及缓存。 |
| R05 | 32 位透明渲染 | 设置 ARGB 像素格式，将索引图转为 32 位色，按资源标志和透明索引处理 Alpha。 |
| R06 | 文字渲染扩展 | 字形纹理缓存和文字阴影；支持 `$0…$9`、`$a…$z`、`$A…$Z` 切换颜色，`$$` 转义。 |
| R07 | 对话文本扩展 | 扩大对话区域；换行处理识别双字节字符、颜色标记、回车、换行和分页符。 |
| R08 | 绘制队列扩展 | 增加矩形、PNG、缩放战斗背景和 Canvas 批次。 |
| R09 | 地图与转场适配 | 重写小地图颜色及索引／ARGB 绘制路径；适配转场类型 0～35 的非标准分辨率显示。 |
| R10 | 底层补丁 | 改写扫描线等待、UV 补偿、图形宽度缓存和动画播放返回路径；停止指定音效缓冲。 |

源码依据：[渲染安装总入口](D:/Source/cgmsv/twcg_hook/src/hooks/UI/rendererHook.cpp:3)、[DX11 安装](D:/Source/cgmsv/twcg_hook/src/hooks/UI/dx11/dx11Hook.cpp:112)、[布局适配](D:/Source/cgmsv/twcg_hook/src/hooks/UI/rendererWindow.cpp:124)、[PNG 与索引图](D:/Source/cgmsv/twcg_hook/src/hooks/UI/rendererSurface.cpp:44)、[文字处理](D:/Source/cgmsv/twcg_hook/src/hooks/UI/rendererText.cpp:78)、[扩展批次](D:/Source/cgmsv/twcg_hook/src/hooks/UI/rendererBatch.cpp:340)、[小地图安装](D:/Source/cgmsv/twcg_hook/src/hooks/UI/automap.cpp:449)、[转场分派](D:/Source/cgmsv/twcg_hook/src/hooks/UI/transitionHook.cpp:1255)。

R10 按源码中的具体改写列出；个别固定地址补丁的完整原端语义尚未独立核验，不将其统一认定为性能优化或缺陷修复。

## 战斗与物品

| 编号 | 功能点 | 具体行为与依赖 |
|---|---|---|
| G01 | 战斗等待时间 | 默认改为 5 分钟；接收 `BtWt` 更新等待时间和血条开关。 |
| G02 | 常显战斗血条 | 修改原血条门禁，并给符合条件的 10～19 号战斗对象追加血条。 |
| G03 | 状态效果收尾修正 | 原 `0x00410F70` 执行后，将 `debuffType` 写入 `field_13DC`，清零 `field_13D8`。 |
| G04 | 受伤宠物选择修正 | 替换 `0x00504B60`，只返回已使用且 `injury > 0` 的宠物，未命中返回 -1。 |
| G05 | 取消观战地图限制 | 将 `0x00455A90` 地图白名单检查改为恒返回 1。 |
| G06 | 输入限时阈值调整 | 源码称为 `blockTextInput` 的比较值由 1000 改为 200；`SpDt` 可更新该值。 |
| G07 | 背包与银行分页 | 普通背包、战斗背包、合成和银行增加分页按钮；接收 `INVCF` 配置页数及当前背包页，发送 `SWITM`／`rh` 切页。 |
| G08 | 物品数量选择扩展 | 已安装路径的数量比较上限改为 9999；按住超过一秒后，增减逻辑采用约 20% 的步幅。 |

源码依据：[战斗修改](D:/Source/cgmsv/twcg_hook/src/hooks/battle.cpp:70)、[宠物选择](D:/Source/cgmsv/twcg_hook/src/hooks/core.cpp:228)、[输入比较值](D:/Source/cgmsv/twcg_hook/src/hooks/misc.cpp:18)、[背包与数量扩展](D:/Source/cgmsv/twcg_hook/src/hooks/UI/invExt.cpp:442)、[窗口接入分页](D:/Source/cgmsv/twcg_hook/src/hooks/UI/UIExt.cpp:363)。

G06 的定位器按字节模式找到比较立即数；本次确认安装代码和可配置阈值，没有仅凭名称进一步认定完整输入或发送业务。G07 需要服务端承接分页协议。G08 中 `Shop_COUNT_A`、`Shop_COUNT_B` 的安装已注释，不能扩大为所有商店路径均已修改。

## 网络收发

| 编号 | 功能点 | 具体行为 |
|---|---|---|
| N01 | 扩展消息回调 | 按消息头注册接收／发送回调，供战斗、背包及 Lua 使用；匹配后执行首个对应回调。 |
| N02 | 接收压缩报文 | 接收端支持普通 LF 明文，以及 `0xEF` 标记的 LZ4 分块报文；解压结果继续按 LF 提取协议行。 |
| N03 | 特殊发送处理 | 遇到 `JFVf` 前缀时清空发送队列，先加入 `XCNY \n`；当前正常发送链直接追加明文。 |

源码依据：[消息回调](D:/Source/cgmsv/twcg_hook/src/hooks/network.cpp:41)、[发送处理](D:/Source/cgmsv/twcg_hook/src/hooks/network.cpp:156)、[LZ4 解码](D:/Source/cgmsv/twcg_hook/src/hooks/network.cpp:280)、[接收接线](D:/Source/cgmsv/twcg_hook/src/hooks/network.cpp:410)。

`LZ4_encode` 有正文，但没有当前调用点；旧发送压缩循环已注释。因此不能认定当前链支持双向压缩。`PACKET_ENC` 控制的字典初始化也不能单独证明整包加密。迁移网络扩展需先核对 cg2d_re 现有明文合同及服务端命令，不能直接替换整份 `network.cpp`。

## Lua UI 与自动化

| 编号 | 功能点 | 具体内容 |
|---|---|---|
| L01 | 游戏数据绑定 | 向 Lua 暴露人物、战斗对象、物品、宠物、技能及技能等级数据。 |
| L02 | Lua 窗口与控件 | 创建、查找、聚焦、关闭窗口；提供图片、PNG、动画、文字、输入框、滚动视图、MapEx 和 Canvas。 |
| L03 | 窗口和输入调度 | 接管窗口更新、绘制、释放；维护层级、焦点和命中顺序；接入滚轮及输入框失焦。 |
| L04 | Lua 事件和工具 | 场景、窗口、键盘、聊天、收发包事件；聊天回调可阻止发送；提供音频、协议编码、文件／YAML 读写，以及 Lua UI 与自动战斗之间的 Bridge。 |
| L05 | 自动战斗 | 记录手动 `Rg` 动作，向 Lua 提供上回合动作和战斗数据，通过 `AutoBattleEvent` 执行指令；提供完整／简化面板。 |
| L06 | 自动寻路 | 全图、局部和未知区域规划；分段请求移动；卡住后标记障碍并重规划；到达或换地图时停止。 |
| L07 | 扩展地图控件 | 地图分块缓存、视野更新、标记点及鼠标地图坐标查询。 |
| L08 | Canvas | 绘制线、矩形、圆、路径及边框，参与原绘制队列和裁剪。 |

源码依据：[数据绑定](D:/Source/cgmsv/twcg_hook/src/hooks/lua.cpp:679)、[窗口接管](D:/Source/cgmsv/twcg_hook/src/hooks/UI/UIExt.cpp:169)、[Lua API 注册](D:/Source/cgmsv/twcg_hook/src/hooks/UI/luaUI.cpp:2911)、[文件接口](D:/Source/cgmsv/twcg_hook/src/hooks/UI/luaIO.cpp:408)、[自动战斗](D:/Source/cgmsv/twcg_hook/src/hooks/UI/autoBattle.cpp:792)、[自动寻路](D:/Source/cgmsv/twcg_hook/src/hooks/UI/copilot.cpp:1542)、[地图控件](D:/Source/cgmsv/twcg_hook/src/hooks/UI/UIControls/MapExControl.cpp:200)、[Canvas 类型](D:/Source/cgmsv/twcg_hook/src/hooks/UI/UIControls/CanvasControl.h:9)。

自动补血、遇一级怪／BOSS暂停、背包满暂停、队友离队／死亡暂停、目标与技能等级选择等选项已传给 Lua，但策略脚本不在当前目录，不能认定这些策略已完整实现。`MakeAutoBattleCommand` 是空函数。

## 技能特效

YAML 特效替换保留原动作链，主要改变图片／动画号；Lua 技能状态机可接管指定技能的动作和表现，迁移规模不同。

| 编号 | 特效功能 | Hook 位置与行为 |
|---|---|---|
| E01 | 技能元数据贯穿 | 战斗数据分配由 `0x13E8` 扩为 `0x1400`，附加原始 `skillId`、`techId` 和 Lua 节点信息；同步分配、绑定、复制、清理。 |
| E02 | 气功弹 | `0x00418B70`：替换动画号。 |
| E03 | 冲击波 | `0x00419180`：替换动画号。 |
| E04 | 魔法特效 | `0x0041CD00`：在模式 `0x23` 及 `techId 1900～3199` 范围内替换生成对象的动画号。 |
| E05 | 补血与恢复 | `0x0041E368`：按模式分别为 `6100～6399`、`6400～6699` 替换受术动画。 |
| E06 | 状态恢复与复活 | `0x0041A913`、`0x0041A6FA`：分别替换粒子动画，保留原数量和移动逻辑。 |
| E07 | 施法闪光 | `0x0041DB2C`：为 `6100～6899` 使用可选 `castingAnime`，未配置时保留原图号。 |
| E08 | 弓箭、小刀与回力镖 | `0x00419390`、`0x00419692`：替换弹道图片或动画。 |
| E09 | 第二技能等级保存 | `0x00435BB5`：将当前技能等级写入 `castSkillLv2`。 |
| E10 | Lua 技能演绎状态机 | 在 `0x0042A768` 接管已注册的 SKL 技能；支持动作队列、阶段回调、子对象、逐帧视觉对象、受击反应，以及客户端 HP／状态表现更新和清理。 |

源码依据：[特效安装入口](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/effectHook.cpp:50)、[技能元数据](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/battleSkillMeta.cpp:245)、[气功弹](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/spiracleShot.cpp:27)、[冲击波](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/blastWave.cpp:27)、[魔法](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/magicEffect.cpp:27)、[补血与恢复分派](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/recoveryEffectDispatch.cpp:8)、[状态恢复](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/statusRecoverEffect.cpp:26)、[复活](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/reviveEffect.cpp:26)、[施法闪光](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/castingAnime.cpp:15)、[远程弹道](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/rangeHit.cpp:56)、[Lua 技能接管](D:/Source/cgmsv/twcg_hook/src/hooks/EffectHooks/luaSkillEffect.cpp:2823)。

`effectHook.yml` 中的 `skillId` 虽被读取，原生特效替换代码实际只比较 `techId`；Lua 技能注册才同时匹配两个 ID。配置样例的 ID 均为 0，不能视为已配置真实技能。E10 的 HP 更新发生在客户端战斗对象中，不代表服务端伤害计算能力。

## 配置条件与静态问题

| 条件或问题 | 当前盘点结果 | 对迁移的影响 |
|---|---|---|
| `cgmsv.yml` | 当前参考目录不存在；资源初始化读取失败分支会提示并退出。 | 资源配置格式和实际资源组需要另行确定。 |
| `lua/autobattle_base.lua` | 当前参考目录不存在。 | 自动战斗执行依赖的策略不完整。 |
| `luaUI/init.lua` | 当前参考目录不存在；初始化代码会跳过入口脚本。 | 控件和 API 存在，不代表已有实际 Lua 页面。 |
| `lua/battleSkillEffect.lua` | 文件存在，但只有注释示例，没有实际技能注册。 | 状态机具备接线，当前没有可据此验收的自定义技能。 |
| 动画临时容量 | 默认动画上限 30500，加载临时表仍为 `0x4000` 项。 | 容量不一致；扩容需核完整数组、索引及分配链。 |
| 固定布局与地址 | 大量使用 x86 地址、裸偏移、32 位指针转换及 MSVC 内联汇编。 | x64 迁移需接入新端真实类型和函数，不能照搬补丁。 |
| 外部组件 | 参考项目使用 Lua、YAML、LZ4、Detours 和 DX11 等组件。 | 本盘点不授权新增或复制依赖；后续按目标工程当时的依赖状态和选定范围确定方案。 |

## cg2d_re 接入位置

以下是盘点时已定位的源码接入点。原函数有正文不等于 Hook 扩展已经迁入，也不等于完整业务、接线或运行通过。

| 迁移范围 | cg2d_re 接入依据 |
|---|---|
| E02 气功弹 | [battle_effect.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/Yobi/battle_effect.cpp:1213) 的 `NewBattleSpiracleShot_418B70`。 |
| E03 冲击波 | [battle_effect.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/Yobi/battle_effect.cpp:1284) 的 `NewBattleBlastWave_419180`。 |
| E04 通用魔法特效 | [battle_effect.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/Yobi/battle_effect.cpp:437) 的 `NewBattleEffect_41CD00`。 |
| E08 远程弹道 | [battle_effect.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/Yobi/battle_effect.cpp:1355) 的 `NewBattleRangeHit_419390`。 |
| G03 状态收尾 | [battle_cast.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/Yobi/battle_cast.cpp:2457) 的 `UpdateBattleAction142_410F70`，当前未包含 Hook 追加的两项字段写入。 |
| G04 受伤宠物选择 | [SceneState.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/world/SceneState.cpp:7761) 的 `Sub_504B60`，当前保留原取样方式。 |
| G05 观战地图限制 | [SceneState.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/world/SceneState.cpp:8099) 的 `Sub_455A90`，当前仍查地图白名单。 |
| G01 战斗等待时间 | [SceneState.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/world/SceneState.cpp:3631) 的 `Sub_43C4E0`，当前仍写入 30 秒截止时间。 |
| A01／A02 资源扩展 | [GraphicAlloc.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/resource/GraphicAlloc.cpp:29) 保留 220 万图形和 20500 动画；资源路径和文件表仍为 9 槽。 |
| A07／L02 输入 | [Input.hpp](D:/Source/cgmsv/cg3a/cg2d_re/include/cg/input/Input.hpp:69) 当前接口使用 UTF-16；需适配旧 GBK／逐字节追加逻辑。 |
| A09 跳过 Loading | [LoadingScreen.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/platform/LoadingScreen.cpp:28) 已将初始阶段设为 21，复用跳过后的清理阶段。 |
| A05 字体 | [Font.cpp](D:/Source/cgmsv/cg3a/cg2d_re/src/render/Font.cpp:16) 已统一宋体；运行时扫描原 EXE 字符串的转换方式不宜直接迁入。 |
| R01 渲染后端 | 新端使用 SDL3；迁移应提取布局、PNG、透明度、文字和转场行为，按现有渲染边界接入。 |
| L05／E10 战斗状态机 | `src/Yobi/battle_skill.cpp`、`battle_base.cpp`、`battle_actor.cpp` 等已有对应原业务入口；仍需单独明确接管边界、节点寿命和完成时序。 |

## 后续范围选择

若先迁移依赖较少的能力，可从 A07／A08、G01～G06、G08、E02～E08 中选择；E02～E08 仍需要技能元数据的生产与传递，以及图号配置方案。

背包分页需先确认服务端的 `INVCF`、`SWITM`、`rh` 合同。Lua UI、自动战斗和 E10 需分别确定脚本入口、依赖、对象寿命和状态机接管方案。资源扩容需同时核对所有固定容量和加载／释放路径。

选定功能编号后，迁移 SPEC 应明确新增行为、与原复刻基线的具名差异、涉及模块、协议／资源／编码合同及验收标准，再实施对应改动。本盘点不改变现有函数实现状态或验收登记。
