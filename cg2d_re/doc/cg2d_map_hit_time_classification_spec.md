# 地图命中与时段分类 SPEC（MHT01）

2026-10-10。沿PDH01真实检测依赖补46AE60/46AD00，并完整纳管实际时段叶458EC0。依子AGENTS第8条先记本SPEC，再恢复原边界读取/索引表达序；时段正确body、命中低字节/原分类发布与业务门槛保持。

<a id="map-hit-time-baseline"></a>
## 三完整原根与实际来源

| 原身份 / 当前符号 | 完整范围（尾不含） | 指令 / 字节 / E8 / 跳 / RET | 入向 |
|---|---|---|---|
| F:0006AE60 / Sub_46AE60 | 46AE60–46AE94 | 13 / 52B / 0 / 0 / 1 | 一点，父46AA00 |
| F:0006AD00 / Sub_46AD00 | 46AD00–46AD97 | 47 / 151B / 1 / 12 / 2 | 一点，父46AA00 |
| F:00058EC0 / GetCate_458EC0 | 458EC0–458EF6 | 21 / 54B / 0 / 5 / 3 | 三点，父458F00/46AD00/50C870 |

合81条257B/三页、一个实际E8、十七跳六RET，无switch/间接CALL；五入向中AD00→GetCate为内部，四外点来自三个外父。当前真实提供者为SceneState.cpp两检测和GameTime.cpp时段；原PDH父2208保持，两未独立登记时段父仅绑定现Sub_458F00/50C870，局部窗不代完整父/调色板/协议链验收。

地图边界/跨度原S16取样后符号扩展S32；命中表每项步长2，原只读低BYTE，AD零扩展到W16后发布9DAD4A。现gMapXStart/YStart/SpanX为S16、gMapHitArray为U16表、gLastDetectedCode为U16，逻辑消费低U8保，不新增裸字节表或复制缓存；不宣现优化器读取宽度/机器布局已验。GameTime原六W32共24B，hours偏移0Ch由现头断言承载；参数为原生非拥有const GameTime*，仅读取hours一次，U32位型转S32分类，不采用CRT tm/SDK布局或改字段。

<a id="map-hit-time-decision"></a>
## 完整合同与局部裁决

46AE60原先读Y边界→跨度→计算y-top→读X边界，随后跨度乘Y差、减left、加x，取命中低BYTE与2比较，原XOR EAX/SETZ AL产生完整S32 0/1。当前先齐取三个边界，再以x-left+span*(y-top)计算，提前X边界读取且换了表达序；只将localY放回left取样前，索引改为span*localY-left+x，原参数/低字节/返回保持。

46AD00原先读X边界，再读Y边界，之后形成X/Y差，调用GetCate；返回后才fresh读取跨度，先乘Y差再加X差。当前x-left计算插在两个边界读取之间，索引表达以localX在前；恢复两个边界样本先齐取，再形成两差，GetCate之后显式fresh span，索引span*localY+localX。不把跨度缓存到time调用前，不合并检测/时段调用或新建通用索引工具。

随后读低BYTE→零扩展，先比较3，再将原码发布U16 gLastDetectedCode。完整比较短路序3/9/11/13/10任一返回1；6仅category3命中；7仅category0或1；8仅category2；其余0。现isThree比较在发布前，所有其它比较在发布后，正确保持；不能把发布放到返回分支、改为仅命中时发布或使用表项高BYTE。参数和返回均S32，原完整EAX的0/1与PDH调用消费保持。

458EC0只读hours一次并按原S32比较：先19<=h<24，再0<=h<4，任一类2；之后h<7类3；余下h>=16类1，否则类0。所有有符号门槛及原未归一化路径保持，不改无符号/模24/默认小时，不加null/范围拒绝或补值。现完整body正确，只关联注释/独立登记，六W32布局/共享gCgTime及全部时间生产者保持；不借时段叶新增本地时间或更改服务端时间。

两检测共三逻辑组（AE边界/索引一组，AD先取齐两边界/整数差一组、time后fresh跨度/索引一组），时段body零改；两CPP/两头只这些组与三定义/三声明注释。RPLAN.24不新增保护、默认值、补零、钳制、索引/容量限制、回滚或异常模型；已批现普通整数、ACP01/MC01数学及转换边界保，不以源序称优化后独立读取/并发或ABI一致。

<a id="map-hit-time-acceptance"></a>
## 限定静态验收

核三根连续原码/边界、唯一实际E8/十七跳六RET、五调用窗/四外点三父、边界S16符号扩展与跨度时点、表步长2/低BYTE/分类W16发布、完整EAX 0/1及hours一次W32/signed门槛。整SceneState.cpp/GameTime.cpp与WorldUpdateDependencies.hpp/GameTime.hpp精确重放三组和六关联注释；时段body、PDH父/全部其它body、表/类型/共享源/原时间生产者保持。

新增三语义/F，MAP/Yobi各三成员、两限定S零I；U09/U10/G01/R01/R04/V04六任务保持open。原成员枚举、完整gates、六closure及所有其它台账/总SPEC§10守恒，不以数量/文档关联率/现字段类型称实际全功能完成。

只做必要文本静态与measure_cg2d.ps1/-Check，不构建/测试/运行客户端或算法/产物/摘要/依赖。完整三个外父、全部输入/地图表生产消费与alias-owner、时间写入/gCgTime寿命、调色板与PudO网络接线、SDK/数值/机器ABI/画面/完整生命周期仍待。WAV-only、原格式明文、MSVC直接CRT及现文本字节边界保持。

<a id="map-hit-time-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐路径方向生产并恢复分量取样顺序”；本步拟提交说明“[#] 补齐地图命中与时段分类并恢复边界取样”。统一cg2d_re分支、不打tag。先记SPEC，三原根取证/源码三采样组、核心静态/登记、文本汇总/Check及派生范围核对通过。语义2213=2165F+48X，文档关联2275/3680=61.82%仅关联率，F处置1644/证据1446，MAP164/Yobi94仍未验收；派生仅六metrics/三F/两功能/六任务变化，184叶验收0、60任务关闭11、六closure开放/全部完整gates及其它字段守恒，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_map_hit_time_classification_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐地图命中与时段分类并恢复边界取样」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x46ae60 | 0x46ae94 | 13/52 |
| 0x46ad00 | 0x46ad97 | 47/151 |
| 0x458ec0 | 0x458ef6 | 21/54 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`generated_machine_reads_verified`=false；`runtime_verified`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
