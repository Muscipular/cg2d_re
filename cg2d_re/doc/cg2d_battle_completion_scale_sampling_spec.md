# BOA01 完成通知、父关联与缩放回调

<a id="battle-completion-scale-sampling-baseline"></a>
## 原端与实际差异

[完整证据](cg2d_battle_completion_scale_sampling_evidence.json)三独立单chunk：41A160..41A1B4的27条84B、41A1C0..41A1FE的22条62B、41A270..41A310的48条160B，共97条306B三页11call/十内部分支/三ret，无switch。三原ID此前无独立语义/分类/战斗成员，历史定义已有、不在175定位队列。

三当前回调均在X发布之后才取Y，原各X截断→FLD Y→X发布→Y截断发布，本步各一补丁解决。原阶段/通知/标死/正常精度与其它所有源保持，已过[BNZ01共同创建](cg2d_battle_effect_factory_sampling_spec.md)不重验。

原目标地址引用6/6/1共13处均来自41CD00，不是普通call；其中41CD47只是将完成callback地址载入EBP，完整父后续多处复用该值写callback，故引用数不等于实际绑定字段写入数。证据保存引用类型/地址/实际指令，不以13推所有回调接线或owner寿命已验。

<a id="battle-completion-scale-sampling-decision"></a>
## 本线程裁决与完整合同

先SPEC后实现，仅三个末坐标片段显式保存S32 screenX/F32 screenY后发布，接口不改。action/parent沿获准native非拥有T*，业务phase/1024/坐标仍S32、offset/scale F32；原__ftol沿既有CPH01 host，不新增异常转换帮助器。MarkDeath仍只标死，不立即free/摘链。

| 根 | 保留的完整正常行为 |
|---|---|
| 41A160完成 | PlayFrame(action,1)严格返回1才取parent；parent非空才取其payload，1024非零写2，随后MarkDeath。parent空仍标死，原无新payload判空；未完成和已标死都继续坐标投影 |
| 41A1C0父关联 | 初parent可空；只有非空且deathFlag严格1才标死自身，之后所有路径仍PlayFrame(action,0)和投影。不能改死亡后早退、立即释放或父任意非零都死亡 |
| 41A270缩放 | phase0加原F32 step、phase2减；X FST到F32但保未舍入double，Y计算存F32后才比较该X。上界严格>1，写phase1再X/Y=1；下界小于0，写X/Y=0再phase3；其它phase不改scale，仍Play(0)/投影，没有MarkDeath |

原常量54D490的4B为0AD7A33C（step位型3CA3D70A），54D494为0000803F（1），54D420为00000000（0），共12B。现bit_cast step/正常double中间值和比较保持，不改成重读已存F32 X。阶段终点0/1是原业务值，不是新增钳制、异常补值或故障模型；实际PC53/浮点环境和ABI仍未运行。

三补丁各恢复X截断→Y取样→X发布→Y截断发布，保通知和播放先于坐标。没有新增判空/容量保护、owner/free、RNG或其它状态/协议；网络仍原格式明文，音频暂停。

新增语义1370..1372分别绑定F:0001A160/F:0001A1C0/F:0001A270及adapted分类、FUNC-BATTLE成员72→75；完整枚举继续open。三源码问题本步解决，不再留待办；所有父/callee-owner/资源重进退出与运行仍待。

<a id="battle-completion-scale-sampling-acceptance"></a>
## 可度量验收

| 项 | 限定依据 |
|---|---|
| I | 三根三补丁/整CPP从HEAD精确重放，完成1024非零门禁/可空parent-严格死亡/标死后继续、未舍入缩放与业务阶段、X/Y序列定向静态 |
| S | 三完整97条306B三页11call/十内部出口-三ret、三个真实F32常量12B和13地址引用分列，EBP加载不计作直接callback写 |
| 追踪 | 三独立ID/分类和成员75、各函数I/S共六E；七open/唯一进度同步，所有旧记录-其它功能域/完整门禁保持 |
| 度量 | 语义1370→1373/有效ID1365→1368、分类476→479、关联1483→1486/3680=40.38%、E535→541；175定位队列保持 |
| 整体 | 不构建/测试/运行/摘要，全部上游/callee-owner/资源-ABI-SDK-PC53-RNG画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open/全部完整门禁保持。仅静态源码与完整原根局部证明，不外推原EAX全部返回/机器ABI或实际画面。

<a id="battle-completion-scale-sampling-source-state"></a>
## 当前收尾

三回调坐标取样已修，完成通知、父引用与缩放基线可独立追踪。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)/[看板](../plan/cg2d_measurement_dashboard.md)同步，固定快照 `cg2d-re-battle-completion-scale-sampling-20261006`。沿原真实依赖推进，不扩展暂停范围或全面复验。
