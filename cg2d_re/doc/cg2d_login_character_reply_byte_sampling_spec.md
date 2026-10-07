# 登录与人物回复字节取样 SPEC（BRW01）

2026-10-07，聚焦登录／创建／删除三条既有真实回复的取样顺序，先SPEC后按既有工程委托实施。已有正常比较值逻辑正确，当前循环先读成功标记、后读回复，原端相反；本步恢复原读序，不声称已观察到运行错误。

<a id="reply-byte-sampling-baseline"></a>
## 完整原端基线

| 原根 | 完整体 | 真实入向／覆盖返回 | 成功发布 |
|---|---|---|---|
| 508BC0 | [508BC0,508C39)，40条121B | 512E40@51630E／516316 XOR EAX | W16回复1→S32 outcome1→GetTickCount→512DF0(socket,tick) |
| 508EE0 | [508EE0,508F61)，41条129B | 512E40@5161C4／5161CC XOR EAX | S16选槽→W16回复1→S32预览取样→record+4C注册号／+50预览 |
| 509020 | [509020,50907B)，33条91B | 512E40@516269／516271 XOR EAX | W16删除回复1，不在本父清记录 |

三体合114条341B／三完整页、21jump／三RET；只有登录根两个call：IAT GetTickCount与direct512DF0。其余两个无callee，三唯一code入向均有据；只绑定三回复，不提升整个512E40或SDK／ABI。

三根先检查共享状态==1，否则不写；状态1先发布S32状态2，再内联匹配58AFB8的successful\0原11B。每个byte先L8回复、后L8标记，比较相等后以同一次回复字节判NUL。原每轮展开两次，但无插入发布或外调，当前按byte循环可保持同一正常读序和短路；没有原strcmp调用，不能因名字新增共享helper或改成宿主比较。

创建成功后读原S16选槽，W16回复1发布后再取9A6E14的S32预览，随后原同148B记录+4C／+50两个W32顺序写入；现源码这部分已有正确顺序，保持。三回复当前分别通过hPUd／DZ／oy接既有dispatcher；正常void业务声明及caller覆盖EAX保持，不公开原寄存器残值。

相邻三个真实X为7／15／5B共27B：全90、每BYTE canonical nop、无入向、前完整根RET／后真函数入口，仅填充用途，不混业务或排除身份。

<a id="reply-byte-sampling-decision"></a>
## 最小开发裁决

只改LoginRequest.cpp、CharacterCreationRequest.cpp、CharacterDeletionRequest.cpp的三同形循环，每体两精确片段：

1. 每轮以unsigned char显式取result[index]，再以unsigned char取kSuccessfulResult[index]；随后比较两个持有值。
2. 相等后用同一个received==0判终止，非零仍原index++；不再借expected或重读回复值判NUL。

unsigned char表达原L8／无符号比较；仅相等谓词，不新增差值返回、保护／补零／严格解析／转码或回绕模型。成功后所有原共享字段、计时调用、角色记录、W16／W32／S16、发布顺序及原失败保持。其余启动／轮询／超时、登出和名称查询、本体调用及公共核心全文保持。

三真实F纳管：LOGIN仅登录父、CHARACTER仅创建／删除父、NETWORK三回复父绑定；不据同文档候选新增其它成员。两个功能各局部I／S，完整枚举和门禁不提升。U10／V03仅同步当前／引用，保持open；音频暂停、原格式明文与RPLAN.24保持。

<a id="reply-byte-sampling-acceptance"></a>
## 可度量验收

| 项目 | 前态 | 本步条件 |
|---|---|---|
| 三循环取样 | 标记→回复，判标记NUL | 回复U8→标记U8，判同回复NUL；三体六精确片段 |
| 三成功／失败发布 | 已有 | 状态2先发布，完全匹配才原成功链；全部宽度／顺序保持 |
| 基线／语义 | 三仅冻结定位 | 三完整F补基线，1573→1576／有效1566→1569 |
| 文档对照 | 1684／3680 | 1687／3680（45.84%），不代表功能完成率 |
| 正常处置 | 703 | 706，仅三F；ABI／运行不通过 |
| R7 | 277／3061 | 280／3061余2781，三个27B真NOP用途 |
| 功能成员／局部证据 | LOGIN1／CHARACTER3／NETWORK51；E808 | 实际LOGIN2／CHARACTER5／NETWORK54；E812，四局部I-S |
| 完整门禁／任务 | 完整0／源码closed11 | 不变；U10／V03仅引用和当前、open |

验收集中核三源整文件六精确替换、原已有成功尾／Start-Poll和唯一owner／当前dispatcher保持；三完整原体连续字节、114／341／21jump／三RET／两call、原11B及三真实X；机器台账和完整派生快照仅对应增量、编码／引用／精确diff。仅定向文本静态，不构建、测试、执行或摘要。

<a id="reply-byte-sampling-source-state"></a>
## 当前边界

开发恢复三条已接回复的原字节读取与持有值判定，原正确正常发布和失败保持；只局部正常源静态验收。完整启动／服务器／登录／选择或创建人物／入主地图、正常owner和全部回复／资源／生命周期、真实SDK／ABI及明文互通／运行继续按整体SPEC推进。
