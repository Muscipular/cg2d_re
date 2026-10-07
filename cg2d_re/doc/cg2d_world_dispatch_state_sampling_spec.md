# 场景分派状态取样与正常递增 SPEC（BTB01）

日期：2026-10-07。沿人物登录／地图入口实际分派推进，按工程委托先记裁决再实施。原格式明文、音频暂停、D01、RPLAN.24及执行限制保持。

<a id="world-dispatch-state-baseline"></a>
## 完整原基线与差异

原 `World::Update@[51C430,51C726)` 完整188条／758B／三页70+70+48，无缺页。73直接CALL／48唯一目标，另有51C6A6的GetTickCount导入间接CALL；17跳转、十一RET。唯一实际入向为MainLoop457020@457117，当前MainLoop.cpp直接调用void world::Update()。本步不外推完整主循环／48callee／导入SDK。

原先取pending S32 type，非负才写current type、取pending state、清pending type=-1、写current state；pending state不清。随后51C458取current type S32，再51C45E清C82204 W8。现源两者倒序，本步恢复原现场样本，当前13主场景分派及capability五项次分派保。

原scene11先取当前状态，0执行原初始化／清对象／窗口／58B6D0=1／效果／73ECC0=2／既有音频调用，一次GetTickCount U32以高零QWORD→F64写gLastRenderDt；1只在51FDE0(2,0,3F19999A)完整返回1时清73ECC0。两路汇合51C6BE INC共享当前sceneState，必须在外调／tick发布后取当场状态，不递增入口快照。现两源码路径均显式用U32位模式+1再bit_cast回S32模拟回绕；按RPLAN.24改普通S32 ++。

scene11其它状态仍进入独立455C90；返回0回YobiUpdateAll／RenderChara尾，非零按原清理／断网／ProbePalette、先算paletteIndex再发布capability、SelectPalette、转scene2／清Yobi／清D9F2CA W8，原顺序不改。普通值原正确，不称实测运行故障，不建立回绕／异常门禁。

完整主分派保：1按capability0/1、2、3、4分子场景，再Yobi／Chara／Input／44A410；2/3分别选服／人物后Yobi与Chara；4输入后创建，再四项尾；5原地图进入初始化与各S32/W8发布，转6／既有音频调用后只清七DWORD（D67B08共28B），不增清零容量；6登录状态后Yobi／Chara；7断线提示后仅Yobi；8/12和未知return；9正常地图、10战斗；13输入后按capability<3调用旧版本场景，否则普通版本，随后四项尾。11按前段合同。

创建场景4当前显式ProcessCreationScene_4626A0(1)沿已批准D01；P08接线已完成，原ECX各退出路径未知事实仍保。185旧boundary仍写“待确认”，与已批准／实施状态冲突。本步只更新此陈旧句，保原pending合同，不关闭原4626A0 pending7或把固定1称原常量。

头字段证据：gCurrentSceneState为S32，C82204与9DAD30为W8，D67B08严格std::array<U32,7>；原stosl计数7吻合。bit头仍供本文件后段真实低W8→S8位型边界，不因撤递增模型删掉或拓宽其他字段。

相邻X:0011C726-0011C770完整74B：2B 8BFF单条MOV EDI,EDI中性对齐、52B十三主目标、20B五capability目标。两同父数据入向51C470／51C48C，无code入向，前51C725 RET／后51C770真入口。仅归原用途，不造F／x64 W32地址表，不将自MOV记成90 NOP。

<a id="world-dispatch-state-decision"></a>
## 实施裁决

在既有WorldUpdate.cpp内Update一原业务体实施三个唯一片段：交换入口两语句，tick发布后的状态推进和过渡完成后的状态推进各用++gCurrentSceneState。其它整源／头与调用方保持；真实W8 bit_cast及bit头保。

既有185身份、原role与source保，不新增重复语义。追加完整根与本步合同，更新旧boundary的门禁待决句为D01已批／P08已接及原ECX未知；不重写原SPEC历史、不例行刷新总SPEC／AGENTS。主循环域FUNC-LOOP增加此真实分派成员与单adapted处置；完整依赖和运行门禁保持。

普通S32算术不模拟回绕或加异常保护／钳制／限长／固定成功，不改scene切换、资源／对象生命周期、网络、音频接线或D01适配范围。

<a id="world-dispatch-state-acceptance"></a>
## 可度量验收

| 项 | 本步标准 |
|---|---|
| 原基线 | 188连续指令／758B／三页，73direct四十八目标＋一GetTick导入，17jump十一RET，唯一MainLoop消费 |
| 入口 | 原pending顺序保；current type先取，再清C82204 W8，再依同样本分派 |
| 算术 | 两源码路径同原共用INC，外调／tick发布后普通S32 ++当场共享状态；无回绕模拟 |
| 保持 | 全主/次分派、原子场景／渲染尾与失败、28B七DWORD、U32 tick→F64、字节别名和真实W8边界保 |
| D01 | 固定gate1/P08已实施/原ECX未知明确区分；只更新185陈旧句，pending7保持 |
| R7 | 单74B完整2B自MOV＋52/20B两表，18同父目标／两数据读取／前RET后入口 |
| 源差异 | 整源=HEAD+三片段／一原业务体；所有其它源头／接口保持 |
| 登记 | 185原记录保／无新语义，LOOP32→33／单处置／两局部I-S，三任务仍open |
| 汇总 | measure／-Check终态0，完整快照只具名增量，编码／引用／差异通过 |
| 完整验收 | 不代全部MainLoop／48callee／SDK／正常alias-owner资源寿命／ABI／链接／运行／R8 |

<a id="world-dispatch-state-source-state"></a>
## 实施状态

方案按工程委托裁决；实施与集中静态／派生结果在本节和唯一进度追加。P05／P08原closed、pending18、冻结定位／差队列及全部完整门禁保，不构建测试运行或计算摘要，不手改生成看板／快照。

集中静态已通过：整World源仅三片段、真实S32/W8／七DWORD与D01、bit头及低W8位型保；完整188连续指令／758B、73direct四十八目标＋一GetTick导入、原type→清BYTE／共用INC／tick F64及七DWORD时点、唯一MainLoop实际入向；74B完整自MOV／两13-5表及两同父数据引用；整个机器185仅具名追加与陈旧句更新／原role和sources保，四文档／三任务整文本。measure及-Check同一会话13643已取得终态0；完整派生快照／原pending7和D01边界、12文件UTF-8无BOM-LF、四锚点、两I-S各21实际来源及差异检查通过。语义／文档覆盖数保持，未编译、链接或运行，整体目标继续active。
