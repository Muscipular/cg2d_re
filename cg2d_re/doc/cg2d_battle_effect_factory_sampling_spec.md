# BNZ01 战斗特效共同创建入口

<a id="battle-effect-factory-sampling-baseline"></a>
## 原函数、实际差异与数据

F:0001CD00/41CD00..41E3C1是单chunk完整1583条5825B、20页66call/267普通内部转移/21ret，四实际switch及其数据在[证据](cg2d_battle_effect_factory_sampling_evidence.json)。原0..47由48个U8索引映13个目标组；type>47直接取第三输入的既有action，再到公共尾部。层级38表八W32、原source image区间84个U8/三目标、末类型位置复制27个U8/两目标，总分派数据41E3C4..41E4CB为263B。另553A38十二S32层级值48B及54D5A8原F32 1.5的4B，合315B实际数据；这不是新增R7区间关闭。

当前入口缺独立语义/基线、分类和战斗成员。25个普通入向/20父含41D3A6自递归，仅列原身份，不代全部父或owner生命周期。已有历史直接定义、不在175定位队列。

发现flags取样晚于callback/priority等写入，type13/35固定target二次发布又重读flags，以及type35两个Append直接传整记录未明确原四独立取样序。17个补丁/18处文本替换：11取样定义/12处实修，另六处scope支撑固定局部、避免跨case初始化。原末尾action payload与source payload两判空确实存在，应保留；34/43的全局发布/先写deathFlag再走null分支也保持，RPLAN.24不擅改安全失败。

原三音效call仍指52C450，既有[接口合同](../include/cg/LegacyAudio.hpp)明确额外压栈Y未被原本体读取；现二参声像接口保持，音频本体/SDK和运行不复验或恢复。这里不将未消费栈字新增成业务参数，不因IDA名字猜另一个音频API。

<a id="battle-effect-factory-sampling-decision"></a>
## 本线程裁决与原正常合同

先SPEC后实施，仅本函数17定义/18段，无接口/全局或依赖新增。source、returned action及type>47 third.existingAction采用获准native非拥有T*；third.offsetY仍S32，type/flags W32、其余偏移/图号/level/位置S32、坐标/scale F32、priority/混合W8、field270 W16。第三参现有union和六参声明保持，不把既有action地址截W32，也不扩大非地址字段。

| 原路径 | 保留与本步修正 |
|---|---|
| 默认0..47族 | flags先callback/priority/rate/两混合值取样，随后固定值OR40000发布。两固定整数偏移加live source F32坐标/正常精度保持；全部type图号、tier层级、高图、deviceBlend/centerY/childAura门禁及回调原分工保 |
| 47 | 两W8值4后flags取样，priority/offsetX/rate后同值OR40000发布；third.offsetY/W16零、两scale原1.5及image102C保持 |
| 13/35固定target | 设备返回后flags一次取样、分支值先pub/同值OR40000再pub；level/图号/固定target采样与真实后续调用保持 |
| 30/31/32、36、34/43、17/19 | flags在各原callback/priority写前取样，发布用原固定值。三图110012/110014/110013、36低W8位置优先级/S16图宽与X-Y投影、34/43 factory槽及真实前置death写、17/19父引用/图号与级别原分支保持 |
| 25/33 | 原52C450后先取flags，再priority/OR40000/pub；原ring载荷/索引或reverse绑定、图宽/偏移及声像调用保持，不推进暂停音频 |
| 35/skill400 | flags先新payload取样再callback/priority与OR60000；payload固定跨后续调用。原初effect/aux、条件RNG(0,30)、speed RNG(4,8)、skill/level复制、320/260、TurnToward后RNG(-90,90)与angle相加/业务W16零保 |
| 35两Effect追加 | 各先固定C9C index，再flags→position4→position3→position四独立S32/W32样本，传原16B值记录。Append后另live index/flags判800000，再TakeAux/AppendAux；不能缓存前flags贯穿aux门禁或将记录引用延迟到callee |
| 35其它skill | 31/3037/2013的固定target在分配前取；19/23/27、22/26/30、21/25/29、2068..2079、2055..2057与default各真实Init/不同callback/side临时输出保持，不造新dispatch框架 |
| 公共尾部 | 每次独立设备查询/blend1或2与对应flags，link严格1才父=source/另live source payload1024=1；X取样→direction0→X截断→Y取样→X/Y发布；17/19/34/35/43才按原两个载荷判空复制live battlePosition，返回实际action |

原12项tier值为3/3/3/2/2/2/1/1/1/0/0/0，所有图选择/层级12/9阈值、type21对3100/3101的level1040原减1、type14/16递归19、type40两次live S16图宽/两次RNG-X/Y、source image映射与tail受限位置复制保持。四flags操作中的AH只改寄存器，实际flags写仍W32；两aux test原立即数确证800000，IDA地址标签不是另一业务掩码。

MarkDeath/分配/已过发射callback等职责沿已有基线，本步不重验所有consumer叶或父全文。没有新增capacity/parent/null保护、默认成功/补零/钳制、异常回绕/故障模型、owner/free或协议改变；保实际业务零/失败和返回。scope六处仅支持局部变量，不改变case范围/贯穿或分派。

新增语义1369/F:0001CD00与adapted分类，FUNC-BATTLE成员71→72，完整枚举open。所有旧语义/其它域、网络原格式明文/音频暂停和执行限制保持；以上已发现源码差异当步解决，不留下代码问题待以后处理。

<a id="battle-effect-factory-sampling-acceptance"></a>
## 可度量验收

| 项 | 本步限定证明 |
|---|---|
| 实现 | 一个完整根/整CPP从HEAD仅17指定补丁18段；C++ case scope成对、flags取样/发布序、两固定四字段记录及Append后live flags，真实其它字段/分支/音频和接口保持 |
| 原端 | 完整1583条5825B20页66call/267普通转移与21ret、四switch所有目标/四组压缩索引263B、tier48B/scale4B共315B，实际所有出口连续，入向25/20父分列 |
| 追踪 | 一个独立ID/分类、成员72、各I/S两E；七open/唯一进度同步，不代全战斗/所有callee-owner或运行 |
| 度量 | 语义1369→1370/有效ID1364→1365、分类475→476、关联1482→1483/3680=40.30%、E533→535；原历史定义已在，175队列不减少 |
| 完整门禁 | 不构建/测试/运行/摘要；所有父/consumer-owner/资源重进退出、SDK-ABI-PC53-RNG画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open/完整门禁保持。完整原体已取证/审查，不把原21机器ret/物理call数量机械等同合并后的C++表达式数或宣运行等价。

<a id="battle-effect-factory-sampling-source-state"></a>
## 当前推进

共同创建入口已修取样及记录传值，独立完整基线纳管；[唯一进度](../plan/cg2d_full_reconstruction_progress.md)/[看板](../plan/cg2d_measurement_dashboard.md)同步实际局部状态。固定快照 `cg2d-re-battle-effect-factory-sampling-20261006`；全部callee/上游合同与owner-资源寿命仍沿真实依赖继续。
