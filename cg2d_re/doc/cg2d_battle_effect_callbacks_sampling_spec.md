# BNW01 两特效回调与三个关联创建入口

<a id="battle-effect-callbacks-sampling-baseline"></a>
## 缺口与原端基线

[完整证据](cg2d_battle_effect_callbacks_sampling_evidence.json)保存五个独立单chunk：419FA0..419FC0的13条32B、41E640..41E676的19条54B、41E720..41E7C0的43条160B、41E7C0..41E882的55条194B、41A0D0..41A158的38条136B；共168条576B五页17call、14内部分支/八ret、无switch。五原ID原无独立语义/分类和战斗成员，历史直接定义已有，不在175定位队列，不以源码定义代替独立基线。

当前位置回调Y取样晚于X发布，原41E666先FLD offset44，41E669再写X；本步修正这个实际源码取样差异。其余四根与原正常合同一致，保持原源。上轮[BNV01创建取样](cg2d_battle_effect_creation_sampling_spec.md)已过的随机/位置创建不重验，本步沿两个真实callback消费端及三个其它生产入口闭合追踪缺口。

两callback各三条绑定引用，不是普通call：one-shot由419FC0/41A0D0/421610写入action callback槽，position由41E680/41E720/41E7C0写入。三个创建入口分别2call/2父、7call/3父、2call/1父；引用类型/地址/父身份全部分列，不据列表宣所有父全文或ABI已验。

<a id="battle-effect-callbacks-sampling-decision"></a>
## 本线程裁决与完整合同

沿工程委托先SPEC后实施：仅位置回调一块补丁，将X截断结果与Y浮点取样分列后发布X/Y。两callback的PlayFrame(1)只在返回值严格等于1时MarkDeath；位置回调标死之后仍投影，不改早退或即时释放。MarkDeath既有F:00004D00只标死亡标记、不free/摘链；实际回收沿既有404C50/404D10职责，本步不重验这些叶或擅补owner/free。__ftol沿[CPH01 host](cg2d_compiler_helper_spec.md#compiler-helper-decision)，正常F32→S32仍向零，不新增异常转换器。

| 入口 | 原完整正常合同 |
|---|---|
| 419FA0 one-shot | PlayFrame(action,1)→严格1才MarkDeath→返回；其它结果不标死，无RNG或坐标写 |
| 41E640 position | 同一严格门禁→两路径均offsetX截断→offsetY取样→X发布→Y截断发布→返回；不能把标死视为对象已释放 |
| 41E720坐标创建 | New(2,0)失败空返回→position callback/CC/100→设备→flags一次取样两W32发布OR60000→两W8值4→offsetY/offsetX→image/direction0→固定输入X/Y截断发布→W16零 |
| 41E7C0轨迹创建 | New(2,0)失败空返回→position callback/低W8 priority/200→有设备只flags OR100000，无设备先RNG(0,1)再取flags选10000/8000→mode严格1才OR20000→另live flags OR40000→offsetX/Y→image/direction0→固定输入X/Y截断→W16零 |
| 41A0D0屏幕创建 | New(2,0)失败空返回→one-shot callback/CC/100→设备→flags一次取样两W32发布OR40000→固定S32 X→两W8值4→Y/direction/image→W16零；无RNG/浮点/父引用 |

创建两地址返回与callback action均沿获批native非拥有T*，新引擎既有void callback类型与不消费返回的分工保持；不称原EAX各路径或机器ABI等价。其余字段保S32/F32/W32/W16/W8，AH80是寄存器操作、最终flags仍memW32；RNG只无设备轨迹分支消费一次，不能移出分支或在有设备时消费。固定输入坐标不要改成重读action offsets，原业务0/4/100/200与失败返回保，不加判空/容量/补零/钳制/溢出回绕/故障模型。

新增语义1355..1359分别绑定F:00019FA0/F:0001E640/F:0001E720/F:0001E7C0/F:0001A0D0和adapted分类；只FUNC-BATTLE成员57→62，完整枚举open。四正确源保持，不计源码新开发；位置回调问题本步解决，不再留待办。其它完整创建/更新、Exec/owner资源重进退出、SDK/ABI/PC53-实际RNG画面仍按真实范围继续。

<a id="battle-effect-callbacks-sampling-acceptance"></a>
## 可度量验收

| 门禁 | 本步限定证明 |
|---|---|
| I | 一回调/一补丁，五根与整CPP按HEAD加唯一补丁精确对照；严格Play结果/标死后投影、设备-RNG-flag-live取样、固定输入坐标及字段宽度静态检查 |
| S | 五完整168条576B五页17call、14内部出口/八ret；六callback绑定与11创建直接call引用类型单列，全部父全文不计 |
| 追踪 | 五独立ID/分类与战斗成员62，各函数I/S共十E，七open和唯一进度同步；所有旧记录/其它域/完整门禁保持 |
| 度量 | 语义1355→1360/有效ID1350→1355、分类461→466、关联1468→1473/3680=40.03%、E505→515；175队列不减少 |
| 完整验收 | 不构建/测试/运行/摘要，不代所有consumer-owner/SDK-ABI-PC53-RNG画面或R1/R7/R8，整体未完成 |

YOBI55/RENDER26/LOOP32、五无ID片段/R7区间67/3061、60源码closed10/完整验收0/P01及七open、全部完整门禁保持。位移/数学越域不设故障或回绕模型，原正常业务和依赖合同不降低。

<a id="battle-effect-callbacks-sampling-source-state"></a>
## 当前源码与推进

一个回调取样已修，五入口独立基线登记；[唯一进度](../plan/cg2d_full_reconstruction_progress.md)和[看板](../plan/cg2d_measurement_dashboard.md)按实证更新。固定快照 `cg2d-re-battle-effect-callbacks-sampling-20261006`。网络原格式明文、音频暂停，禁止执行事项保持；不全面复验已过生产叶。
