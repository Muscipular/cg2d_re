# YSS01 Yobi 哨兵初始化与清场/终止 SPEC

<a id="yobi-sentinel-sampling-baseline"></a>
## 原端基线与差异

沿[YLC01摘链/回收合同](cg2d_yobi_cleanup_sampling_spec.md#yobi-cleanup-sampling-source-state)推进初始化及清场/终止的真实两端关系。原会话只读保存：
| 根/完整区间 | 指令/字节 | 入向 |
|---|---:|---|
| 404A40=[404A40,404AD1) | 36/145B | 4576B0@4577F5一type17 call |
| 404D30=[404D30,404D5B) | 13/43B | 20 type17 call，11父 |
| 404D60=[404D60,404D85) | 10/37B | 457A40@457A81一type17 call |

[三完整59条225B](cg2d_yobi_sentinel_sampling_evidence.json)共22直接call/13不同父，各一chunk。三入口已有语义与本体，本轮仅初始化四片段实修、相关生命周期来源/分工追加，不重复计新函数实现或复验全部父。

初始化原事实：
- 两次独立cAlloc(1,原0x330)，首次返回发布start后调用第二次；第二次返回仍在EAX时404A5E先读start，404A64再发布end，404A6E用先取start写prev=0。源现先发布end，再从共享start取目标。
- 404A70读start→404A76读end→404A7C写start.next；404A9D读end→404AA3读start→404AA9写end.prev。现两个复合赋值按C++先右值再左值，各颠倒这两个读取点。
- 404ABE取得返回用的end快照，404AC3写该对象priority低W8=FF；404AC7重新读end、404ACD向当次对象写graphicIndex=-1，返回仍前一个快照。源现末return再次读共享end，未保持原返回取样。

清场/终止已有源码符合原顺序：404D30入口先取start.next，再调用46DD40，之后逐节点写deathFlag=1并取next，每次与当次end比较；不立即摘链/free。404D60依次ClearUp→UpdateAll，之后读start并free，首释放后再读end并free；不一次缓存两个全局指针，不清共享start/end或重置阶段计数。

<a id="yobi-sentinel-sampling-decision"></a>
## 工程裁决

依AGENTS.md第8条委托采用：
1. Sub_404A40仅四具名片段。第二次calloc返回保存allocatedEnd，读取initialStart后发布end，再向initialStart写prev空；前向链接明确forwardStart→forwardEnd读取后发布，反向链接明确backwardEnd→backwardStart读取后发布。
2. 尾priority写前保存原生result=当次end，向result写U8 FF；graphicIndex仍重新读取共享end写S32 -1，返回保存result，不以最近共享槽值替代。其他每次start/end写入目标仍按原当次取样。
3. 两次分配仍沿已有批准的应用std::calloc(1,sizeof实际YobiAction)/std::free域，只拓宽原生指针字段；不固化新端为原0x330，不用一次双对象分配或私有堆替代。保首次start发布时点、prev/next/callback、U8 priority=0/FF与S32 graphicIndex=-1顺序。
4. 原初始化无分配失败门禁，此处不新增判空、回滚、重试或清旧指针，不初始化未知字段/阶段计数。不清哨兵payload或添加同所有节点一样的Delete。
5. YobiClearUp_404D30及Sub_404D60源码保持。原46DD40的职责、清场标死和YLC01 UpdateAll实际摘链/free分开；四相门禁/原counter保持，不为保证一次清空而新增循环、关闭门禁或默认释放孩子。原终止不清共享槽，完整返回寄存器在现void调用中无已证业务合同，不构造返回值。
6. 所有其它源/头/汇编、YLC01/ACP01/AAS01不改；不新增异常保护、回绕/故障模拟或称等价。

<a id="yobi-sentinel-sampling-acceptance"></a>
## 可度量验收

- 三完整59条225B、22type17call/13父、库存/chunk与真实初始化/终止父保持；完整指令保存只支撑局部取样/分工，不代全部父实参、46DD40/所有callback/callee、运行或owner重进退出验收。
- 本文件全文只精确重放四初始化片段；initialStart取样在end发布前，两链接目标/值按原读取，result在priority写前取得、graphicIndex写仍复读end/返回保存result；两独立分配/原字段顺序和宽度、无额外失败保护、sizeof原生承载保持。
- 清场入口start.next在46DD40前取样，原逐节点标死/next和终止ClearUp→UpdateAll→首free→再读尾free保持；不缓存两全局槽、不清共享槽或重置counter，不擅解四相门禁。
- 三既有语义仅追加源状态/证据，全部旧字段/来源保留；三adapted和MAP/LOOP同三ID关联，四局部I/S共用一份实修/原证据，不增加语义条数/有效ID或计作三个新实现。
- 六open任务与全部完整门禁/功能/R1/R7/R8和整体不升级；获准文本汇总/-Check与链接/UTF-8无BOM/LF/精确提交只支持静态，不编译/运行。

<a id="yobi-sentinel-sampling-source-state"></a>
## 源状态与剩余

真实来源为YobiRuntime.cpp中的Sub_404A40/YobiClearUp_404D30/Sub_404D60及既有include/cg/YobiRuntime.hpp声明；YLC01真实UpdateAll删除与本组终止调用保持，单一start/end owner继续，不创建额外生命周期控制器或应用入口。

本组静态结果记唯一进度和登记簿。所有父/callback/callee和数据/资源生产、实际清场跨门禁行为与所有链表别名/owner重进退出/借用失效、真实ABI/SDK/PC53/时钟RNG/完整场景画面运行仍未闭。音频暂停、原格式明文及不构建/测试/运行/摘要限制保持。固定快照`cg2d-re-yobi-sentinel-sampling-20261005`。
