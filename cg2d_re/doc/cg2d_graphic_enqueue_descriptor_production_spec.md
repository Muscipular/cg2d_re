# BOS01 图形入队与描述符生产 SPEC

2026-10-06：沿真实绘制链上游推进两公共入队根；已委托本线程先SPEC后裁决实施。撤已证额外拒绝与S32回绕策略，保原正常/失败和W16字段窄化，不运行异常样例。

<a id="graphic-enqueue-descriptor-production-baseline"></a>
## 原基线与实现缺口

| 原入口 / 语义 / 原ID | 源符号 | 指令 / 字节 / 页 | call / 内跳 / ret | 普通入向 / 父 |
| --- | --- | --- | --- | --- |
| 521B10 / 已有7 / F:00121B10 | cg::login::detail::DrawGraphic | 113 / 354 / 2 | 2 / 12 / 3 | 328 / 181 |
| 520970 / 新1404 / F:00120970 | cg::render::DrawGraphic3_520970 | 114 / 350 / 2 | 2 / 12 / 3 | 94 / 30 |
| 合计 | 两已有源码根，单chunk/无switch | 227 / 704 / 4 | 4 / 24 / 6 | 422引用，父分根列 |

[原证据](cg2d_graphic_enqueue_descriptor_production_evidence.json)保四页/连续字节、全部422普通入向、两个旧源码正文/7完整旧记录与两定位快照、四文件补丁。入向仅纳盘，不能外推所有328/94caller正常时序或运行已验。

原容量检查均只S16比较1000H/JL，没有负数拒绝；非空描述符均四W32读写（521BE3..521BFA/520A40..520A55），空参按四W16零、W32颜色-1、两U8零、末W16零。当前多增负计数拒绝；公共入口用了额外S32回绕helper；描述符整块赋值/预存mode偏离原逐W32与修正发布。各问题本步解决；不擅自将异常域或所有正常别名模拟计作通过。

<a id="graphic-enqueue-descriptor-production-decision"></a>
## 本线程裁决与实现

### 公共图形入队

入口S16 pendingCount只保原>=4096返S32-2上限，不新增负数拒绝或异常域演练。正常容量内保存当前真实record目标在map/XY调用前；imageNo[-1,999]返-2，<-1直传/offset0，>999 call45A8B0至同localfinalImageNo再call45A710低W16 XY，不新查callee结果。初始count定位cache3，layerU8先live count S16 cacheIndex；x正常S32+S16 pub，y正常S32+S16先value4样本/y pub，再descriptor存在样本/最终图号pub→value4pub→原action native pointer清0。非空descriptor依原0/4/8/12四W32每次先读再写，不整块预存，保正常源目标别名；空参4W16零/颜色W32FFFFFFFF/clip0U8/mix0U8/paletteIndex0W16顺序。clip先batchType0/clip1覆1；final图号C18..C21或原imageNo1DF0..1DFF时原Byte special1。尾fresh counter U16，返回旧S16符号扩展S32，然后原低W16+1pub。x/y撤S32回绕helper只正常加，W16计数窄化不改，source参数/records非拥有地址native，其它非地址宽度保。

### 可选映射/批次图形入队

入口容量仅原S16>=4096返-2，不增负计数拒绝。正常容量内先保存record目标；图号[-1,999]返-2，<-1直接图号/XY0；>999只按isGraphicNo低U8是否非零决定map调用，同参数槽graphicIndex初值isGraphicNo保持，零低Byte直接图号，再XY低W16，不新查callee返回。cache3 layer pub→live count样本→y参数样本→cacheIndexpub，recordx原S32直接pub→value4样本→ypub→mappedIndexpub→descriptor存在样本→value4pub→action native pointer0。描述符四W32每对读写或空参4W16/颜色W32/两U8/末W16原默认；随后读当前descriptor.sourceX W16减offsetX/低W16pub，再重读当前sourceY W16减offsetY/pub，不在私有mode整体快照内先减。现有W16减法helper仅原字段自然窄化保，不新增故障策略。clip样本先batchType参数U8pub，clip1覆1；只有mappedIndex C18..C21 specialByte1；尾fresh U16count返回旧S16→S32并低W16+1pub。

### 同文件描述符写入职责

新增私有CopyGraphicDescriptorWords只承接两根原内联段：四次以unaligned-safe memcpy读U32局部、立即写本词，再读下一词；不能一次取完整16B临时值。空参依原4个W16→一个W32→U8/U8→W16发布，不先整体清零颜色再重写。两根调用同helper，无新增原函数ID或公开API。普通入队正文与DrawGraphic3各一个补丁、同文件helper插入一个、AddSigned16Wrapping原私有块退休一个，共四文件补丁；既有W16减法helper与其它入口保持。

target在有效业务容量路径、map/XY调用前保存为native record T*；不对异常计数额外建故障模型。排序槽索引继续取live counter，参数与clip按原阶段取样，不把原saved record改为重新定位。原[队列x64布局](cg2d_graphic_dx_cache_x64_spec.md)保，action已证地址拓宽、非地址字段原宽；不改队列owner/producerABI、消费/排序或其它业务根。

<a id="graphic-enqueue-descriptor-production-acceptance"></a>
## 可度量局部验收

| 项目 | 本步条件 |
| --- | --- |
| 原 | 完整227条704B/四页、4call24内跳6ret，422普通入向分根纳盘 |
| 开发 | 整DrawGraphic.cpp仅四补丁；两根无额外负拒绝/S32回绕helper；描述符/默认/W16修正/clip和队列尾原阶段 |
| 映射 | 7续记完整历史＋1404一新机器映射，历史定义0/1保；shared helper不增原函数数 |
| 计量 | 语义1404→1405/有效ID1399→1400，关联1517→1518（41.22%→41.25%）、分类522→524 |
| 功能/证据 | RENDER34→36枚举open；两根独立I/S共4条static E651→655，RESOURCE41保持，runtime=false |
| 完整门禁 | 七open只进展引用，60源码closed10/完整验收0、member_inventory_closed=false/gates空/global_accepted=false/accepted_leaf_features=0保持 |

仅定向静态完整源码重放、原全根/入向与宽度/发布/两JSON完整增量，执行measure/-Check。未构建/调试/查产物、不新增或执行测试、不运行客户端/算法、不计算hash。RPLAN.24不新增拒绝/越界/溢出或故障模拟；网络原格式明文、内存CryptInt保、音频暂停。

<a id="graphic-enqueue-descriptor-production-source-state"></a>
## 当前状态与后续

本步解决两入队根已证额外门槛/描述符消费与阶段偏差，保原map/XY provider、normal S32坐标、W16修正/计数及真实队列存储。命名快照 `cg2d-re-graphic-enqueue-descriptor-production-20261006`。全部caller、排序/绘制consumer和owner-真实像素运行继续按任务推进，局部static不代整体SPEC验收。
