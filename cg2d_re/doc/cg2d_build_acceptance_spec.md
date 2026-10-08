# 当前项目编译与链接（BWZ01 / CAA01）

## 授权与范围

2026-10-08 用户明确要求“编译当前项目”。本次对当前工作线 `cg2d_re` 执行现有 Windows/MSVC/x64 配置及 Debug 构建，覆盖原禁止构建条款；不执行测试、客户端、算法样例或文件摘要。仅使用已有本机工具链和 SDL3，不下载或引入依赖。

## 构建修复决议

首次配置成功，构建在 `LegacyFloatEnvironment.asm` 的 MASM 对象输出失败。同名 CPP/ASM 使 CMake 生成 `$(IntDir)/src/platform/LegacyFloatEnvironment.asm.obj`，而 MASM 执行时目录不存在。通过源文件 `VS_SETTINGS` 将该 ASM 的 `ObjectFileName` 设为 `$(IntDir)%(Filename).asm.obj`，保持独立对象名和原汇编正文；不修改生成项目，不依赖手工创建目录。

后续以实际编译诊断为依据修复必要的声明、类型或汇编语法，保留现有算法、字段宽度、调用顺序及已批准差异；每项修复与最终结果在本文件和唯一进度记录。

第二轮确认 MASM 输出路径已修复，随后 `mov ch, byte ptr [r9+5]` 报 A2218：REX 寻址不能与高字节寄存器 CH 同用。改为一次 `mov al, byte ptr [r9+5]` 后 `mov ch, al`，两条 MOV 均不改标志，保持原单 U8 取样和后续 CH/CL 分类运算。AL 在分类计算前本来即被 CH 覆盖，不增加读取或改变发布字段。

第三轮的必要声明和接线修复采用现有提供者：

- `LegacyException.cpp` 引入已有 `LegacyRuntimeError.hpp`；`SceneState.cpp` 引入已有 `LoginSceneDependencies.hpp`，不复制声明或增加实现。
- `MapLegacyString_543AE8`、`MapLegacyFileError_542F44` 的现正文移出匿名命名空间，匹配已有公开声明；各自私有工具/表及其余正文保留。
- 旧 `Stringify_52D2D0` 引用对齐已有 `EscapeLegacyTextAndNotify_52D2D0`；网络空叶对齐已有 `cg::NullSub_509080`，不生成假体或改协议。
- 战斗代码的 `SprimaryGraphic` 更正为已有生产变量 `primaryGraphic`；动作52的两宠物位置调用明确借用现 `world::detail::GetBattlePetPosition_405B40`。
- 窗口读取 `gValue9A6E0C` 对齐现 `world` 唯一存储；两处动画图号指针显式视为对应 S32 类型，保持现 U32 owner、W32 位样本和原消费者，不新建影子状态。

第四轮和一次全量重建确认 C++/MASM 已过，客户端还有10个链接符号缺口。必要的链接符号诊断证明其中两个已有正文：core导出的 `MeasureDefaultText_44B260` 指针参数为 `QEBD`、`Sub_451350` 为 `QEAUtagRECT`，调用声明要求 `PEBD`/`PEAUtagRECT`。这两个定义没有同签名前置声明，现 MSVC 对顶层 const 指针形参生成了不同修饰名。将两定义的指针形参去掉顶层 const，与现声明一致；不改变指向对象的 const、算法、读取宽度或调用。

其余八项中，五根（458AE0、454C30、454EE0、50E680、52C3D0）属于具名暂停音频；553000旧C++90槽表没有启用定义，CA0478目录和CB1D1C名单的总容量/生产尚无已证事实。必须按原有 SPEC 完成实际提供者后才可链接；不能凭编译授权解除音频暂停、猜数组长度、补零表或用固定返回假体填缺口。

## 验收条件

现有 `windows-msvc-x64` 配置返回零；`windows-msvc-x64-debug` 完成 core、startup、render 三库及真实 `cg2d_re_client` 的编译和链接，构建返回零。保留完整构建日志和仍存在的诊断，不将编译成功当作原新行为、机器 ABI、运行或整体 SPEC 验收。

## BWZ01 阶段结果（历史）

配置成功，当前目标列入的全部C++/MASM源已编译；含一次全量重建。最终单独构建 core、startup、render 三库返回0，客户端链接返回1（LNK1120，八个未解析符号）。两已有正文的修饰名缺口已实际消除，全部编译错误已修复；客户端尚未生成可交付EXE，本次整项目验收未通过。

| 剩余符号 | 现有范围/合同 | 必要动作 |
|---|---|---|
| `Sub_458AE0`、`InitializeMusic_454C30`、`Sub_454EE0`、`Sub_50E680`、`Sub_52C3D0` | [音频暂停根](cg2d_audio_paused_identity_spec.md)，含能力/初始化/关闭、murD回复、音量应用 | 待用户恢复必要音频范围后实现实际提供者/依赖；原网络格式和明文保持 |
| `gLegacyCppInitializationTable553000` | [标准流宿主提供者](cg2d_sdk_standard_stream_owners_spec.md)：原90槽分为70源投影、15 SDK、null1、音频暂停2、键盘排除2；旧实际表未启用 | 沿宿主生命周期合同完成旧入口边界，不补零表或重复宿主构造 |
| `gLegacyPetSkillCatalogCA0478` | [旧目录生产](cg2d_pet_catalog_production_spec.md)：总容量/运行生产未证 | 核真实原生产和容量后定义唯一存储；不可猜长度或按消费补零 |
| `gLegacyTradeTargetNamesCB1D1C` | [旧交易名单生产](cg2d_trade_target_production_spec.md)：总容量/运行生产未证 | 核真实原生产和容量后定义唯一存储；不可借现代十项名单 |

[构建记录](cg2d_build_acceptance_evidence.json)保存环境、命令/返回码、全量重建诊断、最终未解析符号及本步源码前后差异。原始完整日志仅保存在本地构建目录，不作为已验收运行证据。

## CAA01 当前结果（2026-10-08）

用户随后要求“先完成客户端链接”“用sdl播音频”，已恢复必要五根音频开发和构建。[CAA01 SPEC](cg2d_client_link_sdl_audio_spec.md)明确SDL实际提供者与独立归档成员边界，替代上节“待恢复”的当前状态，BWZ01失败记录仍保留为历史。

现有Windows/MSVC/x64 Debug配置、core/startup/render三库及真实客户端链接均返回0，八个缺口不再出现在当前客户端链接。第5次全量重建和最后第7次增量构建通过，产物为 `build/windows-msvc-x64/Debug/cg2d_re_client.exe`。保留131条全量编译警告，未新增音乐后端/提供者警告；不以零链接错误代音频效果、完整启动退出或整体SPEC通过。

原553000表与CA0478/CB1D1C生产容量仍按各合同保留：旧入口与八个窗口正文/状态迁入独立静态库成员，正常客户端未抽取它们，未定义假数据或取消原功能。实际源码/构建/map证据见[CAA01记录](cg2d_client_link_sdl_audio_evidence.json)。未运行客户端、测试或样例，未计算摘要；V02完整任务仍open。

<a id="gbk-songti-debug-build"></a>

## CAB04 GBK、宋体与ASCII光标编译复验（2026-10-09）

用户要求“编译项目”，本次复用现有Windows/MSVC/x64配置，执行 `cmake --build --preset windows-msvc-x64-debug --parallel 8`。源码为提交 `b27173e4` / 快照 `cg2d-re-big5-to-gbk-20261009`，core/startup/render三库与真实客户端编译链接全部通过，退出码0；无需新增源码修复。产物 `build/windows-msvc-x64/Debug/cg2d_re_client.exe` 为7980032B，完整日志 `build/windows-msvc-x64/build-gbk-songti-20261009.log`。

本次增量构建记录0错误、100条既有源码警告（C4244/C4701/C4996/C4456/C4703），没有文字转码、Font.cpp或InputCaret.cpp相关诊断；增量警告数不作为全量警告总数。构建证据记入[文字更新记录](cg2d_big5_to_gbk_evidence.json)及登记簿build_validation；未重新配置、运行客户端、执行测试/算法样例或计算摘要，V02、文字画面及整体SPEC验收保持未关闭。
