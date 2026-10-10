# 地图回复文本与调色板合同 SPEC（PTR01）

2026-10-11。沿已核普通场景控制器BSA01和时段生产GTP01补实际TeD回复生产、调色板消费。依子AGENTS第8条工程委托先记方案，再修两组顺序；消费者正确正文保持。

<a id="map-reply-palette-baseline"></a>
## 原端证据与范围

| 原根 | 完整区间 | 指令/字节/页 | 原来源 |
|---|---|---:|---|
| 509550 / Sub_509550 | 509550–509642 | 82/242B/2 | 512E40@515F28一CALL |
| 51D6F0 / Sub_51D6F0 | 51D6F0–51D743 | 20/83B/1 | 已完整51C7D0的十三CALL |

两根102条325B/三页，7个E8五目标、七跳三RET，详见[完整机器指令及源差异](cg2d_map_reply_palette_evidence.json)。原网络父只核调用邻窗：按socket与提取文本调用，返回后清EAX而不消费本根固定1；不以此验完整512E40。当前NetworkRuntime实际TeD一T路由、首socket未读的原参数、既有LF明文接线保持。

原回复先原位Unescape(payload)，提取field1，分隔ASCII |、容量255、本地256B；不检查提取返回、不预清本地。再Unescape2(text)，strlen大于24才选原???；原name/NUL保持。原5095A0..5095DC与备用分支先REP MOVSD整词拷贝，5095DC发布FF0D4C=-2，5095E6才REP MOVSB余0..3字节。现一次memcpy全部字节再发状态，尾字节时点不同。

field2提取容量31、本地32B，首BYTE空则-1。非空原atoi返回EAX，先发布FF0D4C再test同EAX负值，负时原覆盖-1；现发布后复读共享槽判负。原C82210==1才C82218=1，返回S32 1；不增提取失败门禁或局部补零。

消费一次读取回复S32：-1先取当时时段category，SelectPalette(category,0)后mode=1、回复=-2；非负持值SelectPalette(value,0)后mode=0、回复=-2；其它负值直接返回。两有效分支弃调色板结果，原provider拒绝也不回滚这两个发布。现正文与完整原根一致。

GTP01完整458F00在category变化时先读mode，再发布category，仅mode1进入3600步SelectPalette；尾总458F40。复用旧根，不再完整取证。现SelectPalette真实资源表门槛/active/steps发布全文保持，不据本两根验完整provider原根、调色板资源/转换和画面。

<a id="map-reply-palette-decision"></a>
## 方案与裁决

1. name复制计算strlen+1与整词字节数，先memcpy整词，发布S32=-2，再memcpy原余字节；源、目的、NUL和容量保持，不新造helper、类型、缓冲或保护。
2. atoi当次S32结果保存value，先原位发布，再按value负值判断，原负时-1覆盖保持；采用已批MSVC atoi，不造新解析/溢出模型。
3. 51D6F0正文全部保持；两定义及两既有声明四注释，首socket接口/返回/其它全源码、六实际提供者/已核父/TeD路由/26B地图名与三S32槽保持。
4. 两新F两adapted；NETWORK增加生产1、MAP增加两、RENDER增加消费1，三限定S零I，八任务仍open。原协议/字段/ASCII |/LF明文、WAV-only、MSVC边界保持。
5. 不新增空/索引/容量保护、默认/初始化、失败回滚或所有权机制；正常清零及负值-1属于原业务门槛。生成读取/完整ABI/资源owner/SDK及画面运行仍待。

<a id="map-reply-palette-acceptance"></a>
## 本步静态验收

完整两根指令/字节连续、E8目标、两生产分支的REP整词→S32发布→REP尾、atoi持EAX判断及消费者发布序可追；两整源精确回放仅两组两处四注释，正确消费体、六provider/父/route/标量定义保持。两新语义/处置及三限定证据、六文档严格追加和八任务引用、派生仅六指标/两F/三功能/八任务，完整门禁不升级。

语义2339=2291F关联+48有界代码，关联2401/3680=65.24%仅关联率，处置1803/证据1608；NETWORK106/MAP191/RENDER118。184叶验收0、60任务关闭11开放49、六closure保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

<a id="map-reply-palette-source-state"></a>
## 当前状态与剩余

本步源码在world/SceneState.cpp，私有声明在WorldUpdateDependencies.hpp，NetworkRuntime真实TeD生产路由及已核普通场景十三消费调用、GTP01时段副作用保持。两个原根没有新指针字段或owner，不以此完成所有数据来源/调色板表和转换、图形资源寿命、全SDK/机器ABI/生成代码读取/像素或客户端运行。

沿同一cg2d_re分支精确提交、不打tag，保七项用户外部改动和原暂存删除。不构建、测试、运行、检查产物、计算摘要或引依赖；整体active/未完成。
