# 聊天接收文本比较字节取样 SPEC（BTH01）

日期：2026-10-07。沿回车发送与原zdr接收的实际依赖推进；按工程委托先记裁决再实施，网络原格式明文、音频暂停、RPLAN.24和执行限制保持。

<a id="chat-receive-byte-baseline"></a>
## 原基线与差异

50CAE0@[50CAE0,50CE01)完整278条801B/四页70+70+70+68，13direct十一目标含原栈探针、无间接、31jump两RET。唯一512E40@51483C调用；现NetworkDispatch的zdr入口仍I/T/I/I、原0x4解码掩码、同五参数和返回弃。仅该消费点不代5779条分派父全根。809已登记全部限长复制及两strncmp接线字段，保持后追加；没有新原函数或新语义。

50CD27先读取解码source BYTE，50CD2B再读取593850共享BYTE；相等后50CD2F复用同source样本判NUL，不等立即停止。原两字节展开只消费全等谓词，相同跳过日志，不同44B050(source,color,font)，随后都进公共音效/对象文本尾。现宿主strcmp替代原读取，改为显式逐BYTE短路，不公开SBB内部差值，不称此前正常比较结果错误或运行故障。

593850保现Input owner的8B前缀与紧邻context、LegacyText593850借用getter，不能改为空串常量。当前IDA该八字节均is_loaded=false/has_value=false，工具读出的FF不是有效原文本；不以该值重写初始化，保现唯一对象和实际共享读取，不推定恒空、不可变或全部生产消费/寿命已闭合。

原9A6E0C非零早返；类型只按原抽取首BYTE为P继续，源2024B、抽取容量2023。反转义后S32长度大于0才复制恰好length字节至2048B snapshot，不补NUL、不预清源或快照。626C14非零跳过名称/门禁/名单/日志并保原颜色字体，进入公共尾。

626C14为0时预清64B name，再现场strlen(source)，才清DA3B54；source非空查首个冒号空格，每步递增后重测source长度，命中用原U32 cursor限长复制后caller补NUL。保两真实StrNCmp_53BFC0与MIC/GP各原strlen计数、32B剥前缀临时区每步重测名字长度、临时NUL→清name→含NUL复制、MIC gate1及名单AL非零早返。不新增容量、字符、空名或名单规则。

原负color只在名称路径归0/font1；否则原color/font。日志比较后先原SE74/X320调用（多余Y240仍不消费、音频继续暂停），再对象号负数早返；同源玩家action非空/对象号命中交51A6A0，其余FindObjectYobi非空交442DC0，均duration1000/同一未补NUL快照/S32 length及分支颜色字体。所有调用与生产消费保持。

发送50C940完整106条406B/两页、九direct六目标/两Tick导入、14jump三RET，现色4→5、禁言门禁/计时发布、1024B两个未清局部/P|%s、参数读序与内部CryptInt/511900保持，只作为上下文取证，不造发送修改/语义/处置或完整验收。

X:0010CE01-0010CE10为15B逐BYTE90/NN_NOP，无入向，前50CE00 RET/后50CE10真入口；仅对齐用途，不造F/helper/范围排除。

<a id="chat-receive-byte-decision"></a>
## 实施裁决

SceneState.cpp一个原Sub_50CAE0业务体一唯一片段：先借共享地址一次；source BYTE后shared BYTE，相等后判同source NUL，失配短路。局部bool只表达全等；非拥有地址和局部索引不引入状态/helper/外调，不改原void声明、共享owner/初始化或其它正文。

809旧复制、strncmp、入向/source/end/count和文档全部保后追加；NETWORK54→55、单adapted、两局部I-S及单15B用途，三open任务仅current/引用。完整父/provider、正常owner-alias寿命、SDK-ABI/链接/画面/明文互通与运行仍未验，不以局部静态闭门禁。

<a id="chat-receive-byte-acceptance"></a>
## 可度量验收

| 项 | 标准 |
|---|---|
| 原根/消费 | 278连续指令801B/四页、13direct十一目标/31jump两RET，唯一51483C及现zdr五参数/0x4掩码保持 |
| 比较 | source BYTE→shared BYTE，失配短路、相等后同source样本NUL、仅全等谓词/无helper |
| 保持 | 原容量/非NUL快照、626C14门禁、动态长度与64/32B局部、原复制/strncmp字段、名单AL早返、颜色分支、日志/音效/对象文本尾 |
| 共享地址 | 八字节未初始化FF不视作内容；8B前缀/相邻context与借用getter/初始化保持，不固定空文本 |
| 差异 | 整SceneState.cpp=HEAD+一唯一片段，发送正文及所有关联源头保持 |
| 登记 | 809完整旧字段后追加、无新语义/F、NETWORK55/单处置/两I-S、15B用途；旧完整门禁和三任务状态保持 |
| 派生 | measure/-Check终态0、完整快照增量/UTF-8无BOM/LF/真实引用/文本差异静态 |
| 完整门禁 | 全父/provider/全部正常寿命/ABI/链接/画面/原格式明文互通/运行R8不代验；原P05-P08 closed/pending7保 |

<a id="chat-receive-byte-source-state"></a>
## 实施状态

方案依工程委托裁决；实施及集中静态/派生结果在本节和唯一进度追加。只修已证读取合同，不构建/测试/运行或计算摘要，不补异常回绕/保护/拒绝/钳制/限长，不手改派生看板/快照。

### 集中静态与派生完成记录

2026-10-07：单比较片段已实施。整SceneState.cpp=HEAD+一原业务体片段、source→共享BYTE/同source NUL、所有原容量/快照/过滤/公共尾与共享初始化/全部关联源头保持；278完整原指令801B/四页、唯一实际51483C/zdr消费、15B逐BYTE用途、未初始化FF有效性及正确发送上下文、整个机器增量、四文档/三任务六列，四组集中静态全部通过。measure/-Check同会话终态0；完整派生快照仅809追加、单NETWORK成员/候选、三任务/单15B和两指标增量。12文件UTF-8无BOM/LF、四锚点、两局部I-S各21实际来源/差异文本通过。

真实结果：语义1614有效1607/文档1725（46.88%）保持，NETWORK55/处置755/E882/R7 325余2736。809旧复制/strncmp/入向字段、所有其它语义与源头/五域/冻结定位/pending18/差队列/原任务和完整门禁保。已确认的比较读取差异已修；共享FF问题已按实际有效性消解，未作伪文本改写。全接收父/provider/全部正常寿命、SDK-ABI/链接/画面/原格式明文互通/运行R8继续，未构建/测试/运行/算法样例或计算摘要。
