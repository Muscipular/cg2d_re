# BOT01 其余图形入队与描述符生产 SPEC

2026-10-06：沿BOS01公共入队/逐W32描述符职责，补同一真实队列的521C80与521DD0两个源码缺口；先记裁决，本线程实施，不合并两根不同业务顺序。

<a id="graphic-secondary-enqueue-production-baseline"></a>
## 原基线

| 原入口 / 本步语义 / 原ID | DrawGraphic.cpp符号 | 指令 / 字节 / 页 | call / 内跳 / ret | 普通入向 / 父 |
| --- | --- | --- | --- | --- |
| 521C80 / 1405 / F:00121C80 | Sub_521C80 | 100 / 326 / 2 | 2 / 10 / 2 | 1 / 1 |
| 521DD0 / 1406 / F:00121DD0 | DrawGraphic2_521DD0 | 106 / 324 / 2 | 1 / 12 / 3 | 25 / 18 |
| 合计 | 两既有源码根，新机器映射 | 206 / 650 / 4 | 3 / 22 / 5 | 26调用点，父分根列 |

[证据](cg2d_graphic_secondary_enqueue_production_evidence.json)保两单chunk/四页全指令字节、26普通调用点、两个改前正文/旧定位快照、两补丁及BOS01保持的helper正文。只纳盘调用点及其父，不代完整caller或运行。

两根当前都用整块std::array赋值/整体清0后颜色Byte重写，偏离原四W32逐对消费和空参W16/W32/U8发布。原521C80在521D74读clip、521D7B才pub batch0；521DD0在521EC9先pub batch参数、521ECC才读clip，不能统一其先后。原记录地址在callee前保存，521DD0的y参数样本在521E5F/xpub521E65之前，descriptor样本521E67在ySum/value4/ypub之前；本步恢复各根阶段。

<a id="graphic-secondary-enqueue-production-decision"></a>
## 裁决与两个正文补丁

### 521C80：低图号允许/映射入队

S16入口pendingCount仅原>=4096返S32-2；正常业务容量内保存实际record目标在两callee之前。imageNo<=999全部直传/offsetXY0，不增加其它两入口的[-1,999]拒绝；>999先map至同localgraphicIndex再XY低W16，不查返回。初始count定位cache3，layerByte先live S16cacheIndexpub；正常S32 x+S16Xpub，y+S16Y先value4样本/ypub，再descriptor是否存在样本/mappedImagepub/value4pub/action native0。复用BOS01同文件helper逐原四W32读写或空参4W16/颜色W32FFFFFFFF/两U8/末W16发布；不整块快照/清色后重写。clipByte样本先batch0、clip1覆1。special条件mapped C18..C21或原imageNo1DF0..1DFF才Byte1；尾fresh U16count返回旧S16→S32后低W16+1pub。source/input原宽与native非拥有地址保，不新拒绝/保护或回绕策略。

### 521DD0：直接图形号/批次入队

S16入口pendingCount仅原>=4096返-2；正常容量内保存实际record目标在XY之前。graphicIndex[-1,999]原返-2；<-1直传XY0，>999仅callXY而不map，入参graphicIndex保存原值并最终pub。cache3layer先live S16cacheIndexpub；原y参数样本在x正常S32+S16pub前，随后descriptor地址样本→正常ySum→value4样本→descriptor存在比较→ypub→原graphicIndexpub→value4pub→action native0。同BOS helper原四W32每对读写/空参W16-W32-U8默认发布。batch参数Byte先pub，随后读取当前clipByte/等1覆batch1，不能套用其它入口clip先pub的顺序。special两范围均使用原graphicIndex C18..C21或1DF0..1DFF。尾fresh U16count返回旧S16→S32后低W16+1pub；原字段宽度/native引用、正常坐标/失败保，不扩大其它入口门槛或故障模拟。

只在本文件替换两个完整正文；复用[已落实的BOS01 helper](cg2d_graphic_enqueue_descriptor_production_spec.md#graphic-enqueue-descriptor-production-decision)，helper/另两根/所有其它正文、队列owner/布局及公开头保持。每对U32先读取局部再立即写目标，保正常描述符源目标别名；默认字段依原发布。没有新增同义helper/原函数或API，不把src格式或图号门槛借本步修改。

旧MAX4096/S16上限与各图号失败分支保，不新增负计数拒绝、callee结果门槛、容量/回滚/钳制。正常S32坐标+低S16偏移，只有已证指针native承载；队列低W16计数窄化保，RPLAN.24不模拟溢出/越界或回绕。原已决x64动作指针/记录布局沿[上位合同](cg2d_graphic_dx_cache_x64_spec.md)保。

<a id="graphic-secondary-enqueue-production-acceptance"></a>
## 可度量局部验收

| 项目 | 本步条件 |
| --- | --- |
| 原覆盖 | 全206条650B/四页3call22内跳5ret、26普通引用及单chunk，无switch |
| 开发 | 整DrawGraphic.cpp仅两正文补丁；BOS helper和另外两根/头/owner/provider保持 |
| 身份 | 1405/1406两新机器映射对应已有源码，历史定位各1/旧行号保 |
| 计量 | 语义1405→1407/有效ID1400→1402，关联1518→1520（41.25%→41.30%）、分类524→526 |
| 功能/局部证据 | RENDER36→38枚举open，两根独立I/S共4条static E655→659；RESOURCE41保持/runtime=false |
| 完整门禁 | 七open仅進展引用；60源码closed10/完整验收0、member_inventory_closed=false/gates空/global_accepted=false/accepted_leaf_features=0保持 |

只必要原/源码/别名与字段发布文本静态、两JSON完整增量、measure/-Check；不构建/调试/查产物、不新增执行测试、不运行客户端或算法、不计算hash。网络原格式明文/内存CryptInt保/音频暂停，整体SPEC范围不缩。

<a id="graphic-secondary-enqueue-production-source-state"></a>
## 当前状态与后续

本步落实两个入队根的实际目标/逐W32描述符与不同clip-batch顺序，保各业务门槛、取图调用和原范围。命名快照 `cg2d-re-graphic-secondary-enqueue-production-20261006`。继续沿实际caller、队列排序/绘制消费与owner-像素运行收口，局部static不关闭整体SPEC或所有正常别名运行。
