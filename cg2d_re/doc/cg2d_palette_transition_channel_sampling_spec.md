# BON01 调色板过渡、通道与查色表取样 SPEC

2026-10-06：沿BOM01两张色表的真实通道生产补取样差异；按已授权先SPEC再本线程裁决实施。仅已批准共享和32位渲染，现有FILE链/CPH正常转换与原上限255业务保持。

<a id="palette-transition-channel-sampling-baseline"></a>
## 原基线

| 原入口 / 已有语义 / 原ID | 当前符号 | 指令 / 字节 / 页 | call / 内部跳 / ret | 普通入向 / 父 |
| --- | --- | --- | --- | --- |
| 450BB0 / 723 / F:00050BB0 | UpdatePalette_450BB0 | 357 / 1499 / 5 | 15 / 38 / 2 | 6 / 5 |
| 451220 / 172 / F:00051220 | RebuildPalette32 | 23 / 80 / 1 | 0 / 2 / 1 | 1 / 1 |
| 合计 | 两已有原ID、单chunk、无switch | 380 / 1579 / 6 | 15 / 40 / 3 | 7引用，父按根分列 |

[证据](cg2d_palette_transition_channel_sampling_evidence.json)保六页、chunk、入向、三个改前正文(含原内联通道段的当前私有helper)、两个完整旧记录与12补丁。原6个FILE/format调用及6个ftol调用分列；32位callee451220与已排除16/24callee451190/451270不混。低色深原调用分流区[450FF0,451004)/[451018,45102C)，各20B，共40B仅纳盘。

原格式553514为6B、rb554920为3B、45组三Byte ToneTargets556254为135B，共144B；当前ToneTargets逐项一致，保持正确可写表，不重算或改值。723旧FILE105条局部及CPH六转换/五正常加法历史完整保，不追认为本步完整过渡已验。

<a id="palette-transition-channel-sampling-decision"></a>
## 12处正常取样恢复

### 过渡主根11处

入口先保存S32 frameVersion再active；已加载同active且transitionActive0原返0，活动不同仅threshold/capability门槛、不新增负索引处理，返原S32130*active或rb失败0/不发状态。512B未清path/%s\\%s/rb、16..239每色三getc低U8 B/G/R，168颜色3Byte清黑后原RGB浮点步长，close后fresh active→transitionActive/frameVersion1；defaultTone判定先throttle0/loaded号pub。每色当前B/G/R先样本，B F32pub→targetB差/G F32pub→targetG差/R F32pub→S32 steps-F64→targetR差，三F32 step按共同steps；默认Tone三Byte先样本，B Current pub→targetB差→targetIndex pub→G Current pub→targetG差→R Current pub→steps→targetR差→三F32步长，foreign原六F32清0与旧targetIndex保。共同steps先counter+1/比较/Frame pub，frame>steps完成：逐色targetB/G/R原bytepub，默认tone targetB/G先B pub、targetR先G pub；旧version先transition0清→S32+10→changing0清→version pub。未完分支每色B未舍入F64 sum在F32pub后转换低U8，G/R按F32样本转换，原六ftol正常S64→U8职责沿CPH；tone三Curr F32 pub后B转换、G F32先B Bytepub、G转换/R F32先G Bytepub/R转换，changing=1。仅已批准32路径call451220，callee后active样本保存，throttle/steps样本在counterpub前；steps<=120原counter>=2清0且每次Frame+1，>120用同S32 steps/20/counter达门槛才清0+Frame1。尾入口active与callee后样本相等才读frame，未变返入口version；否则依序R/G/B三256U8表，默认Tone[2]/[1]/[0]分别128走恒等；非中性每项fresh Tone pointer/byte、S32 i*coef>>7/原业务上限255、fresh通道指针/W8写，最终返回原B路径256或最后计算值。完整原357条含16/24调用区40B，仅共享/32位实现，不扩完整父/PC53/数组初始化-owner/画面运行。

具体补丁：初始化当前B/G/R和目标差的取样阶段；defaultTone判断先节流/loaded发布；Tone初始化三样本/目标差/索引及浮点发布；共同steps先frame发布；完成tone目标B/G与R取样先各前一写；完成版本先状态清；未完成tone G/R浮点先B/G字节写；32查色callee后活动号保存、counter/steps先节流发布；长短周期使用同counter/steps样本；尾比较保存的post-rebuild活动号。

private RebuildToneMap沿原内联段保持：每通道入口读Tone指针/相应字节，128恒等逐项重取实际通道指针；非中性每项重取Tone指针/byte再计算S32乘/>>7、原业务上限255，再重取实际通道指针写U8。顺序R/G/B，实际Tone字段[2]/[1]/[0]，最后B路径的原返回值保；不新增全参数快照、统一色调或安全钳制。

六转换继续保蓝未舍入F64、绿红F32、Tone三F32正常S64→U8，五加法按原S32正常域表达；不模拟或宣异常/回绕、PC53或编译器私有ABI等价。当前FILE模式/三getc低U8/168黑色/flags不改，零steps及异常域不新处理。

### 32位查色根1处

原S32四字节游标/256色，先U8 R再G取样/整数(R<<8)+G，再U8 B取样/pack00RRGGBB；每项完整W32写色，纯黑再W32=1，最终仅索引0 W32清0，返原游标400H。flags不读/不作alpha，不增加条件拒绝/回滚/钳制；当前仅RGB取样恢复，原两W32黑色/透明0及return400H保持，全部caller/像素/owner运行另验。

RGB样本和中间整数显式按原顺序表达；已正确的完整颜色W32→纯黑W321、最终透明0和400H返回保持。BOM01通道承载B-G-R/具名数组绑定、其它父与声明保持。

<a id="palette-transition-channel-sampling-acceptance"></a>
## 可度量的局部验收

| 项目 | 局部通过条件 |
| --- | --- |
| 原覆盖 | 两完整380条1579B/六页、15call/40内部跳/3ret，七入向分父、144B数据；40B低色深调用区分列 |
| 实现 | 整Palette.cpp仅11主根+1查色补丁；私有Tone helper/其余正文和公开头保持，135B表逐项与原一致 |
| 映射历史 | 723/172两个已有ID续记，完整旧记录深拷贝；723旧stdio/CPH属性保持，历史源码定义1/0不伪造 |
| 台账 | 语义1401/1396有效ID及1514关联(41.14%)不变，分类514→516；RESOURCE33→35、RENDER28→30枚举open |
| 两域局部证据 | 两根各域独立I/S共8条static E621→629；full_runtime_verified=false，仍两个原函数 |
| 完整门禁 | member_inventory_closed=false/gates空；七open仅进展/引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0 |

仅正常共享/32位局部静态；不代全部六caller/初始化/数组和色调指针owner、实际FILE/渲染接线、PC53/ABI、像素与画面、R1/R7/R8或运行。只复用已过stdio和CPH相应边界，不全面复验其它链。

<a id="palette-transition-channel-sampling-source-state"></a>
## 当前状态与继续推进

命名快照：`cg2d-re-palette-transition-channel-sampling-20261006`。两个原根已有正文，本步补完整正常取样/原初值和当前映射分类；[原资源合同](cg2d_resource_implementation.md)与[DX合同](cg2d_graphic_dx_cache.md)同步。正确Tone helper、135B可写目标表、六正常转换/五加法及已批准低色深范围保持。

继续沿真实通道初始化、上层触发/绘制与owner推进；不构建/调试/新增或执行测试/客户端或算法样例/hash。RPLAN.24不增溢出/越界/拒绝/补零/故障模型，原业务255上限不删；网络原格式明文、内存CryptInt保、音频暂停。
