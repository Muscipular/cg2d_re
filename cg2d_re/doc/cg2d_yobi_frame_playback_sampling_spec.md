# 对象帧播放与共享帧取样 SPEC（YFPS01）

2026-10-10。沿既有动画运行时 §10，补两原完整正常根的独立语义身份及当前共享取样合同。主体此前已实现，本步不记作首次实现；两私有辅助函数是当前代码组织，不虚构原独立函数身份。

<a id="yobi-frame-playback-baseline"></a>
## 原证与当前来源

| 原身份 / 当前源 | 完整范围（尾不含） | 指令 / 字节 / CALL | 入向 CALL / 父 |
|---|---|---|---|---|
| F:00117880 / Sub_517880 | 517880–517D9D | 386 / 1309B / 8 | 64 / 37 |
| F:00117DA0 / PlayFrame_517DA0 | 517DA0–5182A1 | 380 / 1281B / 8 | 265 / 178 |

合766条2590B/十三页、十六E8/六唯一callee、九十跳/二十RET。329入向均由实际E8目标核对，两个父集合合214，不以xref类型代CALL。完整原根、入向及前后源码差集见[证据](cg2d_yobi_frame_playback_sampling_evidence.json)。不称329完整父、所有动态输入或画面已验。

六已有callee身份160/165/738/1199/1261/1899与提供者保持；分别为GraphicInfo的GetGraphicXY/GetGraphicMapNoIndex、AnimationRuntime的AnimeLoadAction/GetIndex、GraphicDxCache的LoadGraphic和LegacyAudio的PlaySe。已有S16宽高窄读取及BorrowGraphicDxCacheEntries/FromEntries接口保持；不新增公开底层接口、SDK对象或影子资源。

<a id="yobi-frame-playback-decision"></a>
## 工程裁决与十六源组

依子AGENTS第8条先写本SPEC并按委托裁决：两入口共享片段一起修正，入口差异独立保留。正常旧数值通常一致，本步明确源码消费/发布阶段，不将静态差异称已发生运行故障。

- 共享翻转三组：私有helper改收已确定的加载图号，调用方先按原模式门禁决定是否调用；加载成功后重新读取BYTE模式一次控制两轴。X按当前图号→当前缓存基址→当前X偏移→S16宽→发布；Y按当前图号→当前缓存基址→S16高→当前Y偏移→发布，借用现有接口。
- 共享帧应用五组：先固定当前帧引用再清模式，图号发布及GetXY参数均来自同一引用；GetXY后重新取帧引用，X按输出X→帧X→发布，Y按帧Y→输出Y→发布。revert字段仍U16，但消费仅BYTE memcpy，先发布BYTE、取value14、保存低两位门禁、再发布颜色；只有门禁通过才读取当前图号并加载，加载后模式重读。
- 坐标检索包含在上述五组：有坐标且原count大于零时才持样一次当前图号，循环全部用该样本；坐标引用保持，实际叠加仍重新读取当前count。先坐标X→当前frameX→生成X→保存frameY→发布X，随后坐标Y→发布Y。最后模式、帧下标与remain读写沿原。
- 两入口各四组：重置时先写上次动作、保存当前方向、再写上次图号/方向；回绕时先保存旧循环数、清帧下标、再写循环数+1。静图按各自GetXY输出取样/字段发布阶段保存局部图号，LoadGraphic用该局部；Sub额外在Y发布后取第四参数低BYTE开关，并在其非零路径先持样方向再清模式。两静图调用均在模式门禁通过后调用helper，不能改用对象图号重读。

私有helper两个consumer与其三处直接调用一并更新；不新增原F身份。公开函数类型不变，Sub声明参数名明确rate/stopAtEnd/flipStatic，两声明补本合同说明，ActionEntry与YobiAction布局不改。

## 完整正常入口与批准边界

空指针或hidden严格1返回0；当前动作先读、完整停播参数发布后比较上次动作/方向/图号；改变时按原清remain/frameIndex/completedLoops。remain非零减1并返回0，负值亦不另恢复。图号≤999仅清graphicIndex返回0。

Sub静图为[1000,100000)及≥200000，只第四参数低BYTE非零才按方向2..4横翻；Play静图仅[1000,100000)，始终按方向2..4横翻。两者各自偏移、图号、palette W16、模式BYTE、color W32及末尾frameX/Y清零顺序保持；remain=7FFFFFFF。

动画上界、负索引、动作数零或选择失败返回1，LoadAction失败返回-1。action direction/action W16零扩展后比较S32，计数低U16。Sub用参数速率；Play选中动作后才读对象速率。duration乘积W32及无符号除100后发布remain，原最小1业务限制保；frames先保存，U32 frameIndex与frameCount比较。严格stopAtEnd==1才remain255/completedLoops1返回1，其他值回绕。

flag W16零保持旧事件，≥10000写事件；1..9999沿原scene9/flags80200门禁调用既有双参PlaySe(flag,x)，未被消费的原y压栈不造新状态或音频API。帧/坐标表与借用/调用链沿原，所有输出字段宽度保持。现WAV-only/SDL、MSVC CRT与已批准数学差异保持，RPLAN.24不专项异常、模拟、保护、默认值、钳制、补零或新范围限制。

<a id="yobi-frame-playback-acceptance"></a>
## 限定静态验收

核两完整机器码连续边界/十六E8/九十跳/二十RET、329实际入向CALL及214父，十六源码组/整CPP和公头精确重放，六旧callee及全部其它原身份保持。FUNC-ANIMATION、FUNC-YOBI各加两已知成员；FUNC-BATTLE仅加PlayFrame（已有162个BATTLE父的直接调用关系），不将FrameMove猜成战斗成员或添加UIRUNTIME成员。

四条限定I/S分别覆盖ANIMATION与YOBI两根及共享八组；BATTLE仅记录依赖引用，不新增完整gate通过。G01/U08/U09/R01/R04/V04仍open。所有member_inventory_closed、完整gates、184叶验收0、60任务关闭11/开放49和六closure不变。所有数据生产、329完整父及传递callee、容量/机器ABI/alias/owner/SDK/声音画面仍待；不构建/测试/运行客户端或算法/检查产物/摘要，总SPEC§10不降低。

<a id="yobi-frame-playback-source-state"></a>
## 当前状态

本步前Git提交说明“[#] 对齐战斗附属特效开关与共享跟随”，完成提交说明“[#] 对齐对象帧播放与共享帧取样”。统一cg2d_re分支，不打tag。十六组与公头参数名/两注释已落实，整CPP/公头精确重放、两完整原根/329入向/六旧callee、登记守恒及必要源静态通过；文本汇总/-Check均退出0。派生只六metrics/两F/三功能/六任务变化；语义2183（2135F＋48X）、合同2245/3680=61.01%仅关联率、F处置1612/E1415，动画11/Yobi62/BATTLE422均枚举false。七项外部改动保留排除。全部完整门禁/184叶验收0/任务关闭11/六closure及运行仍待，整体未完成。
