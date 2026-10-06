# BPF01 调色板帧末老化的正常计数

<a id="palette-age-normal-baseline"></a>
## 原证据与具体缺口

AgeGraphicPaletteCache_44E230已真实接在绘制公共尾，原池基址取样阶段由GCL01修正；当前计数加一仍经过两次bit_cast和U32模回绕。依现行RPLAN.24，本步撤额外回绕模型，恢复正常S32加一，保正常可计算结果，不宣称溢出已修复或故障等价。

[原证据及全文前态](cg2d_palette_age_normal_counter_evidence.json)归档既有F:0004E230/[44E230,44E286)全部21条86B/一页、3个内部跳转、1个ret、无callee及唯一普通caller521428/父520DD0。父仅采[52140E,521436)11条40B的实际公共尾窗口，其中COM+18H是间接操作数；不把它当独立callee或完整父验收。

原池字节步长410H、扫描末界1A680H对应104项；新端已有原生PaletteCacheItem数组和104常量，字段dirtyCount仍S32/原+0C。x64只已证地址承载拓宽，计数不扩宽；本步不重建原x86整体结构布局。

<a id="palette-age-normal-decision"></a>
## 实施裁决

依工程委托先定SPEC后实施。只改GraphicPaletteCache.cpp一个计数表达式并删除仅供该回绕表达式使用的bit头，不新增接口、helper、资源或依赖。

1. 入口保存唯一动态池，104项按原顺序扫描。S32图号负数直接前进，不读取计数或重读池。
2. 非负沿保存池判S32 dirtyCount>=1；满足才取DX基址清对应palette、重读共享池写图号-1、再重读池供后续计数。保原回收门禁与各次基址读取。
3. 两非负路线均沿当前保存池读完整S32 count，正常count+1，沿同一基址发布dirtyCount，随后才重读池进入下一项。刚写图号-1也仍加一，不添加跳过或清整项。
4. 撤额外U32模回绕和双bit_cast，不增加饱和、拒绝、负计数保护、索引门禁或故障模拟。继续保S32正常算式/原业务上下限；原可计算值一致，不以本步声明overflow结果或原机器ABI通过。
5. 无free、RAII清理、影子表、默认资源或额外字段写入；池/表/节点owner和借用寿命保持。实际DrawAllQueued公共尾位置不改，上传计数/版本发布先于Age，特殊旗清零在后。

本步只更新GCL01老化根的当前算术表达，不重验或修改其Reset/Build两正确体及其他纹理路径、13批次和获批SDL/CPU机制边界。网络原格式只明文、两加解密叶排除、内存CryptInt及音频暂停保留。

<a id="palette-age-normal-acceptance"></a>
## 可度量验收

必要静态限定原21条86B/一页、字节连续/真实head/104项、3内跳1ret和唯一普通点/11条40B父窗口；源整CPP仅一个计数表达式与无用bit头退休，正确分支、池重读顺序、字段/API和全部其它源头保持。

续记既有机器1204/F:0004E230，全文旧映射及旧处置归档，equivalent分类和RENDER/RESOURCE原成员数保持。语义1422条/1417原ID、合同关联1535/3680（41.71%）、分类543、RENDER52/RESOURCE41枚举未闭；E699→703仅两域各I/S四条正常局部证据，不是新增四个功能。

G01/G02/G04/R01/R04/V04增加进展与证据引用且仍open；60任务源码closed10/完整验收0、叶验收0/global_accepted=false及全部完整门禁保持。不因一计数表达式收口整个绘制父/缓存寿命/像素或运行。

<a id="palette-age-normal-source-state"></a>
## 当前交付

交付一个老化计数表达式修正、无用bit头退休、真实原根与唯一父窗口、旧1204映射续记和四正常局部I/S。命名快照：`cg2d-re-palette-age-normal-counter-20261006`。

全部上游/完整520DD0、动态池/节点/表正常alias-owner与失效/退出寿命、SDK机器ABI/链接和同资源全模式像素运行仍独立推进。只必要静态和measure/-Check，未构建、测试、运行或计算摘要，整体SPEC继续。
