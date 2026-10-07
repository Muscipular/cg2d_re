# 速度入口坐标三字段取样 SPEC（BTU01）

日期：2026-10-07。承接BTT01真实运动链，按AGENTS.md第8条工程委托先记后实施；[完整原指令/常数与前态](cg2d_actor_velocity_coordinate_sampling_evidence.json)。

<a id="actor-velocity-coordinate-baseline"></a>
## 原端基线

当前cg2d.exe完整442110 126条466B/4422F0 92条357B，共218条823B/四归档页两块、10call/17jump/3RET及5入向四父。两入口均先发布27C/280步进，再两坐标X200/Y210构造/三字段发布/销毁，最后按双轴非零条件调用46CDF0/写2B8与W16 field272=1。但临时取样和发布顺序不同：442110原每次先encoded W32、mask W32、readCount W8全部取齐，再encoded→mask→readCount发布；4422F0先mask W32、encoded W32、readCount W8全部取齐，再mask→encoded→readCount。现前者通用AssignFromInt经CopyPayload边读边pub，后者直接边读边pub，不能只看字段结果或套同一取样顺序。

442110原override非零直接用原F32位，否则仅一次S16 path270选择>5/4/2阶梯，speed普通S32转浮点/百分比落F32 rate；坐标原X→Y读取/world差/规范化/方向及step F32落点，当前PC53数值叶承接保持。七原F32常数1/0/2/1.6/1.2/0.01/4共28B已逐字节loaded/完整flags及原字节证明，1.6/1.2/0.01保实际F32位值，不改数学实数近似。4422F0普通S32目标放大及现差值/方向数值叶与表达式保持，不增回绕模型或故障门槛。

442110末FLD原F32 rate，NULL原F32+0，以现Double结果承接；四调用点中442549真实FSTP F32消费，其余44257E/46A698/46B206弹栈弃浮点，不能记四点全弃返。4422F0局部寄存器观察保现void合同；唯一46A28D后下一call，完整父/机器ABI和数值运行未验。尾两区14/11共25B逐BYTE90/NN_NOP无入向/后真入口，只对齐用途。

<a id="actor-velocity-coordinate-decision"></a>
## 具名裁决及实现

只修YobiRuntime两既有正文三片段。442110用本入口局部assignCoordinate lambda，Construct后先const encoded/mask/readCount三值再按原同序pub/Destroy，调用X后Y；不全局修改通用CopyPayload/AssignFromInt或复用另入口mask-first语义。4422F0两轴各增加mask/encoded/readCount先取齐再同序pub。新增局部lambda1、全局命名helper0，无新文件/声明/owner/布局或业务。

两原Construct/Destroy次数、RNG序及27C→280→X→Y→条件方向/标志、所有NULL/路径取样/现数值模块与表达式和原速率结果保。ASM/数值头/LegacyCryptInt/所有其它源头保持，不为此项改精度模块或谎称浮点运行等价。原普通S32/实际W32和F32落点保，不添拒绝/补零/钳制/限长/数学异常或回绕模型。网络原格式明文、内部LegacyCryptInt、音频暂停/RPLAN.24保持。

<a id="actor-velocity-coordinate-acceptance"></a>
## 本步验收

1. 完整cpp仅三具名片段，两入口三值先取/按各自顺序pub，两个Constructor/Destroy与字段/方向/返回序保持；全数值模块/头/通用工具与其它源保持。
2. 四页218条823B/10call/17jump/3RET/5四父局部路径，七真实F32数据及25B用途完整核对；速率真实消费与pop区别有据，不以局部未改数值叶代全数值/机器ABI验收。
3. 新1664-1665/adapted/两个实际F处置，LOOP76→78/MAP73→75各两局部I-S真实来源，三任务current/引用保open，全部旧对象/完整门禁保。
4. 必要静态及纯文本measure/-Check后追加实际终态，不编译、测试、运行或摘要；全正常owner-alias寿命、全部父/callee/机器ABI/链接/运行R8继续。

<a id="actor-velocity-coordinate-source-state"></a>
## 完成边界

BTU01两完整速度/移动入口442110/4422F0共218条823B/四归档页两块、10直接call/17jump/3RET、5入向四父局部路径；YobiRuntime两正文三片段实修：442110以本入口局部lambda在每次Construct后先encoded W32→mask W32→readCount W8全部取齐再同序pub/Destroy；4422F0两轴各先mask W32→encoded W32→readCount W8取齐再同序pub/Destroy，不能套通用读取/发布交错或另一入口顺序。新局部lambda1/全局helper0、两原Construct/两Destroy/RNG消费次数及27C→280→X200→Y210→条件方向2B8/W16 field272序保持。原override/仅零或无序才S16 pathCount选择>5/4/2阶梯、world单读和现PC53数值叶/表达式、原W32/S32坐标放大及规范化/F32落点不改、不添故障/回绕模型；442110原F32 rate返回以Double承接/NULL0及真实一处F32消费三处pop保持，4422F0当前void寄存器观察适配。原七F32常数已加载28B/完整flags及25B NN_NOP只用途。ASM/头/通用LegacyCryptInt/其它源码保持，无新owner/布局/上层SDK/业务或边界保护；网络原格式明文/内部LegacyCryptInt/音频暂停/RPLAN.24保持，完整四父/全部正常寿命/机器ABI/数值运行R8未验。

预期语义1666/有效1659、文档1777（48.29%）、处置807/E920、LOOP78/MAP75、R7 375余2686；原pending18/pending7、差队列172/缺定位171/待归属43、closed11与完整叶0/globalfalse保。集中静态/汇总后追加实值，不据局部关联率或字段恢复宣告整体完成。

## 本步静态终态（2026-10-07）

完整cpp两根三片段、两入口原不同三值预取/pub次序及Ctor/dtor/rate消费、218条823B四页全部原MCP字段/5局部路径、七已加载F32/现PC53数值叶冻结与25B NN_NOP用途、完整机器台账两域具名增量及五文档/三任务定向静态通过。检查器动态拼接数组受逗号优先级影响而合并字符串，已核实各模式并改逐变量确认三预取/pub顺序后通过，源码/原证据未为检查器降标准。纯文本measure/-Check终态0；完整派生快照仅两新语义/两域成员和候选/两X/三任务及六指标，十三文件UTF-8无BOM/LF、四局部I-S各25真实来源通过。

实际语义1666/有效1659、文档关联1777（48.29%）、处置807/E920、LOOP78/MAP75、R7 375余2686；完整叶验收0、整体未通过。未编译、测试、运行或摘要；完整四父/全部正常生产消费及owner-alias寿命、数值/SDK-ABI/链接/画面/明文互通及运行R8按原门禁继续，不以局部数值叶未改或字段恢复代整体验收。
