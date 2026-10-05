# WRS01 现代窗口循环判定取样与真实场景调用 SPEC

<a id="window-routing-sampling-baseline"></a>
## 证据与实际缺口

沿[现代窗口/旧表调用边界](ui_windows/shared_legacy_panels.md)及[原指令/差异](cg2d_window_routing_sampling_evidence.json)推进。现有证据只证明57C368的编号/分类/对象/脏标记访问与调用者给503160的初始化回调，没有+04 handler分派来源；不得用猜测的99项轮询填台账，旧实际路由仍待取证。

| 原入口 | 完整范围/数量 | 入向 |
|---|---|---|
| 473F80现代窗口循环 | [473F80,4740F0)，103条368B，一chunk | 504170一个type19尾跳，父504120 |
| 504120转生到期及窗口入口 | [504120,504175)，23条85B，一chunk | 16个type17 call，43CBB0/51C7D0各8个 |

合计126条453B；16普通调用与1尾跳分列，3个已知父含504120，不能混计17个普通call。源src/world/SceneState.cpp既有DrawBattleScene_43CBB0/DrawNormalScene_51C7D0各8个调用表达式保持，仅相关接线事实登记，不宣两完整场景/全部分支与数据已验。

473F80旧语义1233/WMB01、已adapted处置和公共UI成员存在，本步保其全部历史字段/原W8修复，只追加；504120当前真实正文和声明存在、独立语义缺失，新增该原身份而非假成功入口。

三源差异：
1. 原473FA6先判排序返回count>0，473FA8再发布CC4844，473FB1据先前标志跳转；源先发布count再比较。
2. 原474045先判focused!=0，474047发布当前元素索引=-1，474051分流；源先发布-1再判焦点。
3. 原474053取phase，474058写鼠标低W8=212，47405F先判phase==4，474062再发布focus=1，47406C分流；源先发布focus再比较phase。

<a id="window-routing-sampling-decision"></a>
## 裁决与实施边界

依AGENTS.md第8条工程委托采用，只UpdateWindows_473F80三个片段：
- count返回后先保存hasWindows=count>0，再发布共享count，用同判定进入原循环。
- focused保存后、元素处理完成，先保存hasFocus=focused!=0，再发布索引=-1，用同判定走原焦点块。
- 原phase读取和低W8=212发布后，先保存releaseDrag=phase==4，再发布focus=1，用同判定进入原可选拖动/重置/当前窗口复读块。

三个局部bool只表达已证判定取样，不添新状态/门槛；count与focused/phase仍完整S32。保原低W8鼠标两写/低U8比较、高三字节不动，元素visible严格1/共享索引与窗口复读、原update→元素→focus/drag→draw/carry和每轮count复读、快捷键→预设热键及原末尾phase读取→清当前ID→条件低字节写/返回1。

不统一window.id两次读取：原473FC7和473FE3确为两次，现源两次保持。不能把callback后的窗口/共享索引和每轮count贯穿缓存，不能加空窗口/回调保护或新旧表遍历。三处源码判定发布顺序静态成立，不外推本机生成EFLAGS/指令顺序、回调全部签名目标或ABI已经通过。

RenderWindows_504120全文保持，既有gValue5F9754低U8门禁→非零才读取同源timer S32<=0→玩家Y后X→socket明文62→发送后读occupancy+7低U8并减1→先清标志再写该字节→47A140→完整窗口更新S32返回；不复用不同479660计数门禁，不新增失败回滚/音效/默认或运行期owner。不将业务窗口入口因名字或LegacyAudio表owner归入暂停音频；现行音频暂停/网络原格式明文与既定叶排除保持。W8既有原字段承载不是异常回绕模拟；RPLAN.24不添保护/钳制/拒绝或故障模拟。

<a id="window-routing-sampling-acceptance"></a>
## 可度量验收

1. 两原126条453B/chunk/末界及16普通call+1尾跳、3已知父可追；真实现代窗口调用不当旧99表分派完成。
2. 全CPP只精确重放三个单根片段；三判定早于对应共享发布/同判定分流，原W8宽度和窗口/元素/drag/callback及所有其它正文保持。504120/SceneState两实际消费者与已有声明全文保持。
3. 既有1233仅追加，新504120一独立语义：1304→1305/有效ID1299→1300，非函数片段5保持；处置404→405仅新F，旧F处置追加证据/保旧原因，R7区间67/队列保持。公共UI14→15、LOOP29→31同两F不双计；E273→277仅两域局部I/S共用同三源片段/两原证据，不提升完整门禁。
4. U04/U07/U11/R01/R04/V04以及全R1/R7/R8/整体保持未闭；旧实际路由、全部间接目标/输入与场景分支、数据资源/owner重进退出/借用失效、SDK/ABI/PC53/时钟RNG及完整画面运行仍需对应证据。
5. 仅必要集中静态/获准文本汇总/-Check/引用/UTF-8无BOM/LF与精确提交；不构建/测试/算法样例/客户端/摘要，不全面复查或例行刷新总SPEC/索引和压历史。

<a id="window-routing-sampling-source-state"></a>
## 源状态与剩余

真实src/ui/WindowRuntime.cpp两入口、include/cg/ui/WindowRuntime.hpp声明及src/world/SceneState.cpp两场景的16表达式保持，单一窗口/输入/玩家状态owner继续。本步只UpdateWindows三判定源取样修正和504120原身份/真实调用纳管，不新建循环或抽象接口。

旧99表实际调用来源继续待证；现代窗口全部间接callback/元素和输入、两完整场景/所有数据资源、真实生命周期/ABI/SDK/PC53/时钟RNG及实际画面仍未闭，局部静态不代整体。固定快照`cg2d-re-window-routing-sampling-20261006`。
