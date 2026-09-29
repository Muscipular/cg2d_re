# cg2d 完整复刻独立进度

> 当前增量：补齐事件队列推进、效果追加、预读与收尾十依赖，保留哨兵写序及两条生命周期分支；缺口仍36组元素、53主回调、1暂缓回复，见§2。仅静态核对，整体SPEC未完成。
> 本文件是本线唯一执行进度；[总 SPEC](cg2d_full_reconstruction_plan.md)、[执行规则](../AGENTS.md)保持有效，旧 M 系列及根 `plans/progess.md` 不参与本线完成度。
> 本轮压缩前进度固定于 `7c266049`；拆分前全文为 `810c6f75`，前次516行日志基线为 `928e20a3778fa56e340a37e9b30e3b30188369f8`。详细证据保留各窗口/共享明细，不复制另一份日志，追溯方法见§5。

## 1. 当前结论

**整体 SPEC 未完成。** 当前 CMake 只有 `cg2d_re_core/startup/render` 三个静态库目标，没有完整客户端链接目标；函数已有本体不等于全部依赖已闭合。没有真实画面、音频、收发或完整流程运行验收；全量分类和覆盖清单尚未收口，不能给出可靠的整体完成百分比。

当前仅做必要静态核对：**不构建、不新增或执行测试、不运行客户端、不检查编译产物、不使用 subAgent 或独立复核。** 音频继续暂缓，不是范围排除。82f5c889文档整理未重验历史实现；后续仅在原已打开IDA会话中串行补证当前依赖。

| 功能链 | 当前实现与接线 | 仍缺或未验收 | 证据入口 |
|---|---|---|---|
| 入口、主循环、加载 | WinMain、配置/参数、窗口、MainLoop、World::Update 及多项退出叶已有本体；InitializeClient 已接渲染、资源、动画、坐标、相册、InitLoading 34阶段及两段淡变；启动UI/窗口保存读取已接；主循环已有网络、清屏、DrawAllQueued、Flip | 完整命中507140、其他窗口内容、暂缓次级音乐及完整启动/退出未闭合 | [入口](../doc/cg2d_startup_flow.md)、[加载](../doc/cg2d_loading_screen.md)、[主循环](../doc/cg2d_main_loop.md)、[退出](../doc/cg2d_client_shutdown.md) |
| 地图文件、移动、绘制 | 三平面及第四平面、文件/校验/回复、地图对象与挂件四源缓存、原排序合并已有本体；46CF60两路、469020/43CB10包装与快照已接；方向发送、路径生成、469FC0/469C90调度与输入、46CE80/46F680目标格生产已实现 | 普通/战斗场景大父函数、完整窗口许可/失效及世界流程未闭合；路径、画面未运行验收 | [场景](../doc/cg2d_scene_state.md)、[移动](../doc/cg2d_player_move_input.md)、[地图父绘制](../doc/cg2d_map_scene_drawing.md)、[地图缓存](../doc/cg2d_map_draw_cache.md) |
| Graphic 与 DX 缓存 | 解码/查询/读取/Palette、四表分配释放、九槽GraphicInit、DBF/独立调色板、5222D0恒空建链、520B10上传、4096节点、动态/静态调色板池已有本体；64位T*双缓存、两纹理入口、CPU色键Blt、四顶点采样/混合、FillRect已接；auto.dat及唯一自动地图颜色表已实现 | 场景清理调用未闭合；文件缓存互通、DST、像素和生命周期未运行验收；不以AutoMap替代非零独立调色板索引 | [Graphic](../doc/cg2d_graphic_info.md)、[DX](../doc/cg2d_graphic_dx_cache.md)、[x64 SPEC](../doc/cg2d_graphic_dx_cache_x64_spec.md)、[来源](../doc/cg2d_resource_sources.md) |
| SDL 呈现与队列 | Init/Flip/Destroy/Rebuild已接；Init生产唯一Back/格式、独立接口/设备、模式/尺寸及Set/GetTexture探测；DrawAllQueued完整13类批次、真实场景状态、Temp回拷、上传计数/复用帧/老化尾已接；Rebuild接场景9快照及场景10地图 | 战斗背景和普通/战斗上游仍缺；模式2/3/4、全屏重建及完整画面未验收，不称已有可运行客户端 | [呈现器](../doc/cg2d_sdl_presenter.md)、[纹理](../doc/cg2d_texture_rendering.md) |
| Anime | 查询、P1/P4分配、清理/索引池、AnimeLoadAction/CoordInit、FrameMove/PlayFrame、AnimeInit/CoordLoad/CoordLoadInit已有本体，初始化已接InitializeClient | 战斗caller及完整流程未闭合、画面未验收；9A4A54以void*保留非空释放不清槽；locale邮件跨界生产尚未落位，不能视为恒C locale | [动画](../doc/cg2d_animation_runtime.md)、[比较 SPEC](../doc/cg2d_legacy_string_compare.md) |
| World、Yobi、效果 | 500项效果链、观察节点/父子回调、玩家唯一身份/动作、1000项World表及四生产入口、十种动作模式、移动/跟随、43E490实体阶段、4417E0与442F70绘制已有本体；称号、宠物、玩家属性/物品、技能及批量对象回复复用同源状态 | 整块玩家重置、普通场景清计数caller、设置UI及其他附件生产仍未全闭合；未消费字段不冻结布局；数值/RNG、借用槽生命周期及画面未运行验收，新增MASM未汇编/链接 | [Yobi](../doc/cg2d_yobi_runtime.md)、[对象表](../doc/cg2d_world_objects.md)、[场景](../doc/cg2d_scene_state.md) |
| 登录、窗口、交互 | 71槽窗口承载、70主原型记录、拖动表、七类生命周期、打开/展开、保存/初始化/退出已有本体；对话/库存/交易/存放/摆摊/资料/技能结果等已有局部完整父链；窗口10七模式及30元素、11六模式、12/13交互、26五模式及14元素已齐；三技能命令与关闭/快捷/按钮、28完整8元素与两快照生产者、初始化/倒计时、498520六态分派与目标依赖、入场就绪/转向/名字及32完整两元素已有本体 | 仍缺36组元素、53个主回调；43CBB0、快照/载荷完整生产父链、D64E13生产、旧详情及7/8/9/59资料父链、窗口42其他caller、父鼠标和普通/战斗完整链未闭合；字形/文本/完整交互未验收 | [窗口 SPEC](../doc/cg2d_ui_window_runtime.md)、[登录](../doc/cg2d_login_scene.md)、[控制器](../doc/cg2d_login_state_controller.md) |
| 网络 | workspace/队列owner、593850连续输入、组包/发送、保活、507DF0收发循环及534EF0适配器已有本体；512E40完整108路解析/工作区复制/分发已接caller；物品、玩家/宠物、对象、场景、聊天、邮件、资料、公会、交易、摆摊、技能等回复已续接 | 只余1个回复本体murD随音频暂缓；加密链最终排除接线尚缺，不能记完整收发/客户端完成；108路末三路命中仍继续比较并返回-1 | [网络](../doc/cg2d_network_runtime.md)、[分派](../doc/cg2d_network_dispatch.md) |
| 输入、存档、相册、时间 | 回车提交/聊天收发、快捷预设/选项、窗口焦点/鼠标、输入与字体/文本队列已有局部实现；保存槽/codec/520160、邮件文件及清理、profile/category/公会已接；相册初始化、5186A0/518780文件及518870/518A40同步、两路回复已有本体；时间和指定std入口已有实现 | 507140完整命中/对象身份、剩余邮件与相册UI、共享初始化、网络校时未闭合；相册原4B读写及短写不关闭已保留，不称完整持久化成功或异常修复 | [命中](../doc/cg2d_sub_507140.md)、[存档](../doc/cg2d_login_saved_state_storage.md)、[场景](../doc/cg2d_scene_state.md)、[时间](../doc/cg2d_game_time.md) |
| 音频（暂缓） | SDL设备/样本/64通道、音效、BGM、线程通知及退出入口已有局部实现 | 完整客户端接线、解码/发声、起停/通知未运行验收；暂缓不撤销已有源码，也不计范围排除 | [音频](../doc/cg2d_audio_update.md) |

窗口元素及主回调已齐的编号：10、11、12、13、14、26、27、28、32、36/37、38、39、41、42、43、44、45、46、50、51、52、53、54、56、60、61、62、63、64、67、68、69、70。36/37共享一组。36组元素按地址去重、53个主回调按符号去重，不按引用槽重复计数；murD是分发直接回复本体缺口，均不是传递依赖或整体验收数量，也不表示全部上游完成。

## 2. 窗口10与战斗技能增量

| 提交 | 完成内容 |
|---|---|
| `7faa21b9` | 技能等级与配方完整选择父链 |
| `b39aa705` | 技术初始化、滚动、释放 |
| `82e38f0f` | 列表选择与完整拖动 |
| `d3fc6833` | 战斗技能列表、技术选择及三个设置入口 |
| `88ac12ae` | 宠物/骑乘模式八入口 |
| `712e01b3` | 主更新、返回按钮与战斗返回入口 |
| `89b6e028` | 转生/略过按钮及玩家计时窄读 |
| `fac04f7a` | 七模式主绘制、职业名借用及FP单读接口 |
| `928e20a3` | 完整30元素及11图形/6文本/1鼠标flags来源 |

窗口10三主绑定为 `47A620/47A7A0/47B0E0`，13种初始元素回调均有唯一本体；元素原型、类型化T*载荷及同源状态已落位。五开窗入口、所有按钮和七模式内部功能已实现，不表示战斗命令上游、实际创建/交互或完整技能发送已验收。

必须保留的局部合同：

- `CC4230` 实际为双S32参数 `void(skill, technique)`；`CC4D14` 为独立单参数回调，不按伪代码漏参或伪第三参数改接口。
- `479EA0` 过滤表读固定尾字节 `[15]` 作计数门槛，不改为逐行过滤；负配对索引按RPLAN.24，不新增保护。
- `47C950` 非mode5/6路径有原未初始化位置局部；正常显隐仅允许mode5/6。不补零、安全返回或模拟栈残留，不承诺异常结果。
- 独立CryptInt读取、回调后重取窗口/技能/动作、宠物重复查询与骑乘快照差异均保留；槽数列表/主绘制取低S8，构造4740F0取S16。
- 主绘制配方描述不增hover非负门槛；宠物描述直接用hover，不经配对转换；最大FP先于当前FP，两次读取及相关重取次序分别保留。
- 略过按钮原型图号 `3B57A` 与运行时 `3BE7E` 不同；type4原型仅 `uint32_t* mouseFlags`，实例另分配12B；文本 `"10/10"` 仍由构造替换。

已完成的静态回映：30元素240字段/960B、11图形66字段/264B、六文本元数据/指针48B、六原文本33B，四组摘要一致。完整摘要、原指令、来源地址和调用次序见[窗口10明细](../doc/ui_windows/window_10.md)。未执行构建、测试、客户端或画面验收。

战斗近期增量的完整分支、读取/调用次序、同源字段与原字节摘要见[窗口26明细](../doc/ui_windows/window_26.md)，不在进度重复正文：

| 提交 | 已实现与静态核对 | 当步剩余元素/主回调/回复 |
|---|---|---|
| `dfa0ae96` | 三技能命令、两关闭、快捷及两按钮共八入口；真实回调和四个非拥有展开T*已接 | 39 / 57 / 1 |
| `7416aa52` | 五模式创建4966B0、更新496B90、绘制496D50；D6976C唯一S32承载，不代替507140生产 | 39 / 55 / 1 |
| `810c6f75` | 七补充按钮、完整14元素/12载荷源；448B/216B/24B/15B回映摘要一致，十种元素回调有唯一本体 | 38 / 55 / 1 |
| `d7979839` | 窗口28完整8元素/4图形源与两快照生产者；43C4E0/43C040/4982C0初始化及倒计时；9原入口/3窄接口、256B/96B/45B/22B摘要静态核对 | 37 / 54 / 1 |
| `aa00eb19` | 498520完整六态分派与43C0A0/43C120/43C3E0/43C490；三跳表、75B提示/42B格式摘要、阶段采样和公共计时尾静态核对 | 37 / 54 / 1 |
| `e71d7ebe` | 入场六依赖与窗口32更新/完整两元素；父十项跳表、40B角度及64B/16B/12B原型来源静态核对 | 36 / 53 / 1 |
| `b278f886` | 附加效果链四入口及4990A0血条门禁；54技能集合与两字节跳表、节点T*与释放序静态核对；确认20槽外两对象指针 | 36 / 53 / 1 |
| `b4f66b66` | M头/整数/字符串、50/51位置映射、CHT/BST退出五入口及同源计时扣减；独立游标、固定载荷与重取、25项头表100B文本/100B指针静态核对 | 36 / 53 / 1 |
| `a24dc796` | 22槽T*存储/20槽固定视图、动作队列与回合就绪六入口；88B/352B原表摘要、重置差异尾和逐对象恢复序静态核对 | 36 / 53 / 1 |
| `da01fc78` | M预扫描读取、RID/攻击参与槽与附加图号八依赖；独立游标、RID20整数/2字符串、U8计数及五个S32输出指针静态核对 | 36 / 53 / 1 |
| `7c266049` | 439B30/43B290完整预扫描及冲突判定；25消息/17布局/193技能、382索引项与19直接分支、RID差异和共享效果/名称副作用静态核对 | 36 / 53 / 1 |

`9438fac2`按[文档组织SPEC](../doc/ui_windows/README.md)将190个详细章节唯一迁入32个window id及11份共享文档；原文件改为77行索引，正文和765处链接静态核对，近期战斗日志压为上表。拆分前全文见810c6f75，该文档步未改变38/55/1或验收级别。

**本轮功能小步：** [事件队列、效果与收尾](../doc/ui_windows/shared_battle.md#battle-event-leaves)4392F0/439340/4393A0/439400/43BC10/43B7F0/43B860/423960/43BF00/43B6C0十入口落位。效果数组扩展200个16B记录，目标数组200个S32，旧重置范围不变；32事件槽用T*、位图U32。先哨兵后计数、原200上界/phase236、追加后重读旧效果、预读无游标写、NUL头差异及两收尾分支仅静态核对，20B阈值摘要一致；36/53/1不变。未构建/测试/运行/检查产物，427470完整回调和父调度/创建仍缺。

## 3. 后续任务与规则

下一开发小步沿 `43CBB0` 完整父链继续：43B610实际回调入口为427470（范围至437AD3，427810只是内部），其RID解析4239D0、角色创建438290等真实依赖仍须补齐；不注册占位回调。4383F0→4383B0/43B360初始化/调度、战斗C解析/载荷生产仍缺；预扫描、退出、效果链、队列/就绪及本轮十叶已有本体，不重复取证。32事件槽创建/父调度与位图生产、冲突累计/清理、名称表清理及40F2B1消费者仍未闭合；5A8A40只映射已消费22位置，尾部不冻结布局。原88B对象表清理父函数仍须覆盖22槽，常规路径仍20槽。43843B/41402E所在完整父函数、`507140`命中及`D64E13`的`4FF650`生产仍缺，不以零初值代替真实实现。

其后仍须沿完整SPEC补齐普通/战斗场景、其余窗口、507140命中、共享初始化、排除接线和最终完整客户端门禁。未列入本摘要的债务不视为完成、豁免或范围排除；具体失败残留、未接caller及验证前提以各证据文档和固定历史为准。

| 规则/裁决 | 当前口径 |
|---|---|
| 权威与工程委托 | 当前选定cg2d IDA为权威；未知事实须取证。工程方案先记SPEC、取舍/边界，由本线程按用户委托裁决，不逐项等待；不扩大功能或降低验收 |
| 参考范围 | cg3a地图/graphic/anime为可信参考；封包不确定时可参考cg3a网络处理，必须注明具体源码、适用范围与差异，不覆盖已证cg2d行为、不转记本线完成。STONAGE只读GBK，仅架构/命名线索 |
| x64与地址 | 原x86按字段取证；地址缓存用原生非拥有T*，确需擦除才void*，不引入PVOID或句柄表；仅地址排序边界转uintptr_t，非地址数值不扩宽 |
| DX §13–16 | 调色板/DBF归GraphicRead；5222D0第二参数恒空，只保留未使用参数，不实现非空分支。48B队列用YobiAction*/uint32_t textToken联合字段，保留4B排序槽和前缀别名 |
| 渲染/音频 | SDL3 SDL_Renderer自动driver、统一CPU Back/Temp及节点面，只做32位色渲染；保留8位索引资源/调色板。DX7原生ABI/错误机制获批替换，上层像素、坐标、裁剪、随机次序、共享生命周期不豁免。音频替换已批但暂缓 |
| 原字节与文本 | 源码/文档UTF-8；原兼容数据保持原编码字节、容量、偏移及处理次序，现代边界才转换；不猜全为Big5/CP950、不静默替换或新增严格门槛 |
| RPLAN.24 | 正常逻辑及失败分支、宽度、别名、状态、调用/RNG顺序保留；越界/溢出等异常不专项模拟、不作实现阻塞、不称异常修复或等价通过，也不擅加拒绝、补零、钳制、保护 |
| 局部适配 | 四个地图短头函数仅已批未写栈字节补零，不推广至anime或其他路径；队列负计数、特定表索引、网络分配失败、x87等窄适配沿原范围，不由压缩扩大 |
| 指定CRT | Malloc/Free/Atol/GetEnv采用对应std，保留正常失败、借用身份及调用顺序；许可不外推其他CRT |
| 工具与执行 | 本线程完成，不用subAgent/独立review；禁代码图谱。仅原已打开IDA会话串行只读MCP，不重开/重载/重启/保存/搬动、不用IDALib替代；新取证先发现实例，不把临时ID当身份 |
| 验证限制 | 不构建、不检查产物、不新增或执行测试、不运行客户端；静态核对不等于编译/链接/运行。旧Debug/复核只作历史，不重新计算EXE/.i64哈希门禁 |
| 提交与进度 | 每可验证小步同步本文件、精确暂存提交，排除用户改动；日志只保留当前结论/证据入口/验证级别/剩余项，全文固定Git追溯 |

范围排除：8/16/24位色渲染、屏幕键盘、登录newest五函数检查链及用户指定网络加密链。排除项保留证据和必要排除接线，不算已实现；newest参数与LoginScene的-3/-4保留，非加密组包/分发不排除，音频不在排除列表。

## 4. 证据与压缩基线

全量基线、台账与历史阶段入口：[基线](../doc/cg2d_reconstruction_baseline.md)、[盘点台账](../doc/cg2d_reconstruction_inventory.md)、[资源实现](../doc/cg2d_resource_implementation.md)、[解码](../doc/cg2d_graphic_decode.md)、[角色选择](../doc/cg2d_character_select_scene.md)、[输入](../doc/cg2d_input_context.md)、[窗口消息](../doc/cg2d_window_procedure.md)、[编码 SPEC](../doc/cg2d_text_encoding_spec.md)、[当前存档](../doc/cg2d_current_saved_record.md)、[随机数](../doc/cg2d_random_range.md)。R0/R1/R3及R6.1.2/3各小步编号和历史验证域见§5固定全文，不以旧编号推算覆盖率。

82f5c889修正总SPEC中“网络分发仍缺”“渲染探测未接”“父绘制仍缺”及相册文件/同步的旧状态，并补齐封包参考授权。UI SPEC增加当前摘要，早期“未实现open/不分配载荷/主表仅extern”等保留为阶段证据，不再作为当前缺口。该次仅文档提交，后续源码增量见§2；详细原端合同不删除、验收标准不降低，用户既有改动不夹带。

该次对齐依据为窗口表/回调、NetworkDispatch、RendererInit、DrawAllQueued及相册入口源码。静态复点确认69组元素缺39组、133个去重主回调符号缺57个（含适配入口口径，识别noexcept）；窗口10的30元素、13种初始回调及三主回调定义唯一，分发有108路。三份文档的差异、相对链接、UTF-8无BOM/LF及三份历史快照可读性已核对；日志当时516行压至101行，后续只记精简增量，不是全项目复验或运行验收。

## 5. 历史追溯

| 快照 | 用途 |
|---|---|
| `7c266049` | 本轮压缩前进度及完整预扫描/冲突判定记录；原合同继续保留在共享明细 |
| `da01fc78` | M预扫描八依赖记录 |
| `a24dc796` | 22槽动作队列/就绪实施记录 |
| `b4f66b66` | M读取/退出实施记录 |
| `b278f886` | 效果链/血条实施记录 |
| `e71d7ebe` | 入场/窗口32实施记录 |
| `aa00eb19` | 498520完整分派实施记录 |
| `d7979839` | 窗口28与共享计时完整实施记录 |
| `810c6f75` | 本轮按window id拆分前完整UI文档、近期战斗日志与窗口26最终14元素实现 |
| `928e20a3778fa56e340a37e9b30e3b30188369f8` | 本次压缩前516行全文及窗口10最终实现，包含RPLAN.32后全部增量与当时限定验证结论 |
| `a0831a64d01113b14edeed366f6bdefd3f9afdd3` | RPLAN.32压缩前952行旧记录 |
| `3808ef2d309e1f407117bfe47390f36e7b8a4786` | RPLAN.17首次压缩前记录 |

```powershell
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show 7c266049:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show da01fc78:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show a24dc796:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show b4f66b66:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show b278f886:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show e71d7ebe:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show aa00eb19:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show d7979839:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show 810c6f75:cg2d_re/doc/cg2d_ui_window_runtime.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show 810c6f75:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show 928e20a3778fa56e340a37e9b30e3b30188369f8:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show a0831a64d01113b14edeed366f6bdefd3f9afdd3:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 show 3808ef2d309e1f407117bfe47390f36e7b8a4786:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c core.autocrlf=false -c i18n.logOutputEncoding=utf-8 log --oneline -- cg2d_re
```

历史“当前/仍缺/待批”仅表示该提交时点；以最新明确裁决和对应实现更新状态，不把历史计划当新完成项。旧代理/独立复核规则由RPLAN.29覆盖；保留原历史事实，不另建归档日志。
