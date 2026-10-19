# 业务符号重命名 SPEC（BSR01）

日期：2026-10-19。状态：用户以“GO”确认，按以下规则分组实施。

## 目标与范围

分析当前 `cg2d_re` 源码中的 `Sub_xxxxxx`，将能证实含义的函数及关联变量改为业务名称。仅修改标识符、声明／引用和必要中文说明；不调整算法、类型、布局、调用顺序、返回值或模块归属。

依据为当前源码、就近注释及当前已打开的 `cg2d.exe` IDA 会话。IDA只读，不修改或保存数据库。第三方库、历史函数实现表和证据快照不批量替换；不生成JSON台账，不构建、测试、运行或计算文件摘要，不使用子Agent。

## 初步盘点

扫描 `src/`、`include/` 的C/C++源／头文件，剔除普通注释和字符串后按 `\b[Ss]ub_[0-9A-Fa-f]+\b` 查得 **685个不同代码标识符，涉及163份文件**。此为词法盘点，包含声明和引用，不是函数定义数或已分析数量。

各源码目录引用的不同符号数：world 442、network 170、ui 133、Yobi 127、scene 74、audio 20、platform 19、input 15、resource 15、render 13、script 4。同一函数可能跨目录引用，不能相加求总数。

## 已确认的命名规则

1. `$name_$address` 作为占位符，实际名称不含 `$`。函数使用 `业务名称_原入口地址`，地址为大写十六进制，不带 `0x` 或补齐零，如 `AcquireOldestAnimationSlot_45B530`。
2. 有已证绝对地址的全局／静态变量使用同一格式，沿当前 `g` 前缀，如 `gAnimationSlotListTail_9A3A00`。
3. **成员和局部变量没有统一的绝对地址**。成员使用业务名称加已证原偏移，如 `accountedBytes_04`，注释标明偏移；局部变量／参数只用语义名称，借所属函数地址注释追溯，不虚构地址。
4. 已有清楚业务名的符号不为统一风格而全库改名；只处理本次分析链中含义模糊或需要澄清的变量，保留原地址注释。
5. 仅修改有行为／数据流证据的名称；证据不足保留旧名，登记未知项，不由数值、自动名或邻近函数猜业务用途。

## 首组映射：动画缓存槽链

四根本次均已串行读取IDA伪代码并对照源码。定义在 `src/resource/AnimationList.cpp`，声明在 `src/resource/AnimationRuntimeState.hpp`，调用在 `src/resource/AnimationRuntime.cpp`。

| 旧函数 | 建议名称 | 证据及含义 |
|---|---|---|
| `Sub_45B530` | `AcquireOldestAnimationSlot_45B530` | `0x45B531` 取链头后的槽，`0x45B547–0x45B551` 摘链，`0x45B557` 清理旧资源，清理非零才扣计数／字节；返回该槽，空链返回空。 |
| `Sub_45B590` | `AppendAnimationSlotToTail_45B590` | `0x45B594–0x45B5A7` 保存尾前驱并完成四次双向链接写入，返回零。 |
| `Sub_45B5B0` | `MoveAnimationSlotToTail_45B5B0` | `0x45B5B5–0x45B5BE` 取两邻居并摘链，`0x45B5C1` 调挂尾；当前装载入口的已加载分支调用它。 |
| `Sub_45B5D0` | `InitializeAnimationSlotList_45B5D0` | `0x45B5DE–0x45B5F9` 清槽及连接哨兵，`0x45B604–0x45B61B` 将256槽索引置-1并逐个挂尾。 |

| 旧变量 | 建议名称 | 证据／限制 |
|---|---|---|
| `gLeftSentinel9A3A00` | `gAnimationSlotListTail_9A3A00` | 挂尾函数将它写入slot.next，并更新其previous。 |
| `gRightSentinel9A3A10` | `gAnimationSlotListHead_9A3A10` | 领取函数从其next取最旧槽，初始化将其previous置空。 |
| `gMainSlots9A29F8` | `gAnimationSlots_9A29F8` | 初始化遍历的256个主槽，实际数组。 |
| `gActiveCount9A4A68` | `gLoadedAnimationCount_9A4A68` | 当前 `AnimeLoadAction_45B760` 成功时递增，领取且清理非零时递减。 |
| `gActiveBytes9A4A6C` | `gLoadedAnimationAccountedBytes_9A4A6C` | 当前装载成功累加槽值，`0x45B57A` 扣旧槽值，是原口径记账。 |
| `MainSlot::value04` | `MainSlot::accountedBytes_04` | 当前装载按 `28*动作数 + 12*各动作帧数 + 8*坐标数` 累计；04为原成员偏移。 |

特别限制：x64的 `ActionEntry` 为32字节，记账仍使用原端28字节，故不能将上述字段命名为实际分配容量／实际内存占用，也不修改公式。此项生产证据来自当前源码；IDA本次核了消费，未重新完整核装载函数。

## 未决项与后续推进

`Sub_45B370` 只转调 `Sub_45B380`，后者把 `gValue9A4A50` 快照复制到 `gValue9A4A48` 并返回。当前片段不足以证明业务用途，首组保留，后续追踪来源及消费者；不得直接命名为缓存容量初始化或内存预算设置。`CacheActionEntry::prefix`、`ActionEntry::value14` 等未证字段同样保留，不外推其他表的含义。

确认后先实施上述首组，再按真实调用／数据链分组处理其余候选，逐组记录旧名、新名、证据和未知项。首组不是全项目完成，不承诺所有685个符号均能取得可靠业务名。

每组同步定义、声明、调用、函数指针及活动脚本的源码符号依赖，检查字符串绑定和名称冲突；不改变协议字节或历史证据。成员限定在对应类型内修改，禁止全库替换通用 `value04`。

静态验收：新旧符号及全部活动引用核对；按映射反向替换后确认业务代码token保持；类型、常量、操作符、布局和执行顺序保持；显式UTF-8／原换行与 `git diff --check`。静态结果不记为编译或运行通过。

每个可验证小步更新进度并精确提交，排除原有未跟踪文件。

## BSR01-A 实施结果

四份资源源／头文件完成首组四函数、五全局和一个成员重命名；同步清理扫描对槽数组的引用，补记账字段说明。全部改动按名称反向替换并剔除注释后与提交前正文一致；活动源／头／脚本中九个旧函数／全局符号无残留，其他类型的value04保持。UTF-8无BOM／LF及差异静态检查通过，未编译、测试或运行。全项目其余符号继续按证据分组分析。

## BSR01-B 资源清理及自动地图颜色

| 旧函数 | 新名称 | 当前源码及地址证据 |
|---|---|---|
| `Sub_45B440` | `ReleaseAnimationSlotResources_45B440` | AnimationCleanup：释放帧、动作表及坐标，清链接并置槽索引-1；本次IDA核0x45B477／45B4BF／45B4F8／45B514。 |
| `Sub_45B650` | `ReleaseAnimationCacheSlotResources_45B650` | 同文件独立缓存表的帧、动作表及buffer清理；原45B650既有ACL01证据，不外推未知载荷。 |
| `Sub_45B630` | `ReleaseAllAnimationSlotResources_45B630` | 同文件遍历256主槽，返回末次清理值。 |
| `Sub_45B740` | `ReleaseAllAnimationCacheSlotResources_45B740` | 同文件遍历256独立缓存槽，返回末次清理值。 |
| `Sub_45BF20` | `CloseFlaggedAnimationFilesAndClearCache_45BF20` | 同文件按标志关闭文件、清标志，再调45B740；本次IDA核45BF23／45BF34／45BF3C／45BF53，当前已批准32槽及清FILE引用差异保留。 |
| `Sub_457670` | `FreePrimaryGraphicResourceTables_457670` | GraphicAlloc：仅释放主动画表、DX图形表和图形信息表，明确不含第四块动画缓存表。 |
| `Sub_45A420` | `LoadAutoMapColorCache_45A420` | ResourcePaths：检查版本2、三项文件时间及零标记，再读共享自动地图颜色表。 |
| `Sub_45A290` | `SaveAutoMapColorCache_45A290` | 同文件写版本、文件时间、零标记及颜色表；保留原忽略写入结果的行为。 |
| `Sub_45A190` | `RebuildAutoMapColors_45A190` | 同文件读取调色板，按图形信息mapNo和已证索引范围重建颜色表。 |

本组九份源／头完成九函数的全部引用迁移。完整正文反向替换后与提交前一致，旧符号及新名称冲突、UTF-8无BOM／LF、差异静态检查通过，未编译、测试或运行。

补查未知值：IDA对9A4A50的直接引用只有45751E初始化和45B380读取；9A4A48除45B385写入，另在45B752作为缓存槽循环的边界地址出现，不能据地址引用推断业务值消费。45B370／45B380及两个gValue继续保留。

## BSR01-C 输入编辑与校验

本组依据当前Input.cpp／InputCaret.cpp正文、InputKeyDispatch的键码分派和既有地址注释；当前UTF-16编辑及GBK保存边界保持，不用原端ANSI类型覆盖当前实现。

| 旧符号 | 新名称 | 证据 |
|---|---|---|
| `Sub_44A410` | `DrawAndAdvanceInputCaret_44A410` | 按相位绘制普通／IME光标后推进相位。 |
| `Sub_44A540` | `MoveInputCaretLeft_44A540` | 键码37；text::Previous移动光标，保留混合返回合同。 |
| `Sub_44A580` | `MoveInputCaretRight_44A580` | 键码39；text::Next移动光标。 |
| `Sub_44A5D0` | `MoveInputCaretUp_44A5D0` | 键码38；MoveCaretLine(-1)。 |
| `Sub_44A7D0` | `MoveInputCaretDown_44A7D0` | 键码40；MoveCaretLine(1)。 |
| `Sub_44A990` | `DeleteCharacterBeforeInputCaret_44A990` | 键码8；删除前一字符并将光标移到前边界。 |
| `Sub_44AA90` | `DeleteCharacterAtInputCaret_44AA90` | 键码46；删除光标处字符。 |
| `Sub_44AB60` | `HandleInputEnterKey_44AB60` | 键码13；按上下文分别提交聊天、插入换行或加入黑名单，不简化为仅发送聊天。 |
| `Sub_45D720` | `ConfigureInputLayoutAndResetScroll_45D720` | 清滚动字段，再写位置、绘制模式及颜色。 |
| `Sub_45D760` | `SetInputPositionAndStyle_45D760` | 写位置、颜色、绘制模式和字体，不清滚动。 |
| `Sub_476740` | `IsValidDelimitedFieldText_476740` | 拒绝逗号／竖线／反斜杠并要求存在非半角／全角空格字符；公会及对话字段消费。 |
| `Sub_45E770` | `ContainsPipeOrBackslash_45E770` | 仅检测竖线／反斜杠，命中返回1，不命名为校验通过。 |
| `gValue626C0C` | `gInputCaretBlinkPhase_626C0C` | InputCaret中0—39循环、20开始显示；编辑及IME写20。 |

十三份源／头全部引用同步，完整正文反向替换与提交前一致，旧符号零残留、新名无冲突、UTF-8无BOM／LF及差异静态检查通过。未编译、测试或运行；其他输入gValue和未证字段保持。

## BSR01-D 绘图队列与波纹

依据当前DrawGraphic、GraphicPendingQueue、SurfaceState及其地址注释，保留现有SDL实现。

| 旧符号 | 新名称 | 证据 |
|---|---|---|
| `Sub_45F090` | `QueueOriginalPaletteGraphic_45F090` | 设置OriginalPalette描述符并调用DrawGraphic3入队；返回队列索引。 |
| `Sub_521C80` | `QueueGraphicWithResourceOffset_521C80` | 大于999的图号查资源索引和XY偏移，加到入队坐标。 |
| `Sub_522210` | `DrawQueuedPaletteRect_522210` | 解包队列项四个S16矩形边界，按图号高位选择三种绘制模式。 |
| `Sub_451350` | `DrawPaletteRectPrimitive_451350` | 裁剪矩形，按调色板／扩展色绘制轮廓、填充或单行。 |
| `Sub_520910` | `AdvanceRipplePhase_520910` | 波纹相位加0.25f。 |
| `Sub_520930` | `WrapRipplePhase_520930` | 原位反复加减64.0f，保持循环与浮点行为。 |
| `Sub_521870` | `ApplyScreenRipple_521870` | 地图门禁后复制Back到Temp，按正弦表偏移回拷8行条带，并推进强度。 |
| `gFloat1020EF0` | `gRipplePhase_1020EF0` | 上述三根共享浮点相位，回拷前扣屏幕Y位移。 |

十五份源／头同步七函数和一个全局，包含登录、选人及环境动画调用。完整正文反向替换一致，旧符号零残留、新名无冲突、UTF-8无BOM／LF与差异静态检查通过；未编译、测试或运行。

## BSR01-E 音频生命周期、空间请求与渐隐

本组核当前LegacyAudio.cpp全文相关入口、请求生产／消费、LegacyAudioMath.cpp空间投影发布及调用方；不恢复MIDI或改动SDL后端。渐隐全局地址另经本次IDA的52B990核实。

| 旧符号 | 新名称 | 证据 |
|---|---|---|
| `Sub_52B8E0` | `StopAndReleaseStreamingBgm_52B8E0` | 停止流式BGM，等待工作线程通知，再清模式并释放。 |
| `Sub_52B940` | `ResetBgmPlayback_52B940` | 清理两路BGM资源并清渐隐、活动与阶段状态。 |
| `Sub_52B630` | `ShutdownAudio_52B630` | 禁用门禁后依次释放BGM、通道、样本及设备。 |
| `Sub_524230` | `InitializeAudioDeviceAndSamples_524230` | 打开设备、加载音效、准备通道并填BGM运行记录。 |
| `Sub_52CCE0` | `StopAndCloseMciDevice_52CCE0` | 原门禁后调用MCI_STOP／MCI_CLOSE，保留复合返回值。 |
| `Sub_52CD40` | `ResetSpatialSoundSources_52CD40` | 将50条空间记录的来源置-1、重新选择标志置1，不清其他字段。 |
| `Sub_52B9E0` | `InitializeSoundRuntimeState_52B9E0` | 初始化通道、请求、声像／音量表和空间来源记录。 |
| `Sub_52CDE0` | `UpdateNearestSpatialSoundSource_52CDE0` | 投影世界坐标，由Cg2dPublishSpatialProjectionX87比较屏幕中心距离并发布更近来源；保留重选标志分支。 |
| `Sub_52C9E0` | `AccumulateSpatialSoundRequest_52C9E0` | 250—299映射请求槽，累计请求并保留更近来源的距离／声像／音量。 |
| `Sub_52C680` | `ApplySpatialSoundRequest_52C680` | 将请求属性应用至通道，新请求准备、回绕并播放，旧请求更新参数。 |
| `Sub_52CB50` | `ProcessSpatialSoundRequests_52CB50` | 遍历请求表，无请求停通道，有请求应用后清请求数。 |
| `Sub_52CCA0` | `UpdateAudioPlayback_52CCA0` | 更新冷却、音效请求、BGM请求及渐隐。 |
| `Sub_52CD60` | `SubmitSpatialSoundSources_52CD60` | 扫描空间来源并提交请求，保首个有效距离不符时立即返回。 |
| `Sub_52B990` | `BeginBgmFadeOut_52B990` | 当前BGM有效时设置渐隐标志，按音乐音量初始化渐隐音量索引。 |
| `Sub_52C3D0` | `ApplyBgmVolume_52C3D0` | 依禁用／当前曲目／开关门禁，将音乐音量钳到0—15并应用。 |
| `Sub_454EE0` | `ShutdownFixedMusic_454EE0` | 停止／释放固定音乐；普通WAV初始化失败时关闭设备。 |
| `Sub_458AE0` | `CheckAudioAndInputAvailability_458AE0` | 探测真实SDL音频设备及DINPUT入口，保原返回位型。 |
| `gValue10507D4` | `gAudioDisabled_10507D4` | 5241A0在主动跳过或初始化失败时置1，成功置0；播放／更新／退出共用。不能只命名为初始化失败。 |
| `gFlag` | `gBgmFadeOutActive_105080C` | 52B990@52B9A3置1，当前52CCA0据此逐次降音量，清理／加载时清零。 |
| `gQuotient` | `gBgmFadeVolumeIndex_103CFB0` | 52B990@52B9D1写缩放结果，当前52CCA0每次减1并查音量表。 |

十二份源／头完成十七函数与三个全局的声明、定义和全部引用迁移。完整正文反向替换与提交前一致，新名无冲突、旧符号零残留、UTF-8无BOM／LF及差异静态检查通过；未编译、测试或运行。

## 本轮边界

A—E累计重命名49个函数、10个全局变量及1个结构成员；每组独立静态核对并提交。初始685个Sub代码标识符中尚余636个未重命名，主要世界、网络、UI和Yobi链尚未逐项完成分析；不能将其全部记为语义不明，更不能声称全项目命名完成。已分析但语义不足的45B370／45B380明确保留，后续仍以本SPEC的证据和命名规则推进。
