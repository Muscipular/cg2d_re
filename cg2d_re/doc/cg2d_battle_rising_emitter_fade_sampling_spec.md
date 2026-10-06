# BNX01 上升特效、屏幕发射与背景五态

<a id="battle-rising-emitter-fade-sampling-baseline"></a>
## 原端与实际缺口

[完整证据](cg2d_battle_rising_emitter_fade_sampling_evidence.json)保存七个独立单chunk原体：420D50..420F0E（106条446B）、41A200..41A269（39条105B）、421480..42155B（66条219B）、421570..42160F（49条159B）、421610..4216A1（37条145B）、4216B0..4216F3（25条67B）、421700..42174A（18条74B）；共340条1215B八页19call/19普通内部出口/14ret。背景callback另421496原五槽W32 switch表42155C、20B，0..4分别42149D/4214E3/42150C/42153F/42154F、default421558，实际表字节在证据内。

上升callback当前Play后Y取样晚于X发布，原41A259先FLD Y、41A25C才写X；本步修正此唯一源码差异，其余六根正确保持。两16项S32偏移原在构造器栈上逐项赋值，证据列全部32值/赋值地址/寄存器常量来源，当前两constexpr数组保持128B正常值合同，不模拟算法样例。

七ID原无独立语义/分类和战斗成员，历史直接定义已有，不在175定位队列。入向普通call共七处（上升创建2/2父、淡变创建3/3父、屏幕创建1/1父、发射创建1/1父）；callback地址绑定五处（上升3、背景1、发射1），类型/地址/父身份分列，不按列表宣全部父或ABI通过。421610已列入上轮[BNW01 callback绑定](cg2d_battle_effect_callbacks_sampling_spec.md)，本步补独立生产基线，不复验已过419FA0等consumer叶。

<a id="battle-rising-emitter-fade-sampling-decision"></a>
## 工程裁决与生产消费

先SPEC后实施，唯一补丁把上升callback的X截断结果与Y浮点取样分列，再发布X/Y。不改父deathFlag严格1早退、非死亡路径speed/Y未舍入double中间值及各F32存储、比较phase/父field58++和MarkDeath后仍Play(0)/投影。原FST保留浮点寄存器结果，不能重读已存F32 speed/Y替代；实际PC53环境/ABI仍待运行。__ftol沿[CPH01 host](cg2d_compiler_helper_spec.md#compiler-helper-decision)，MarkDeath沿既有只标死职责。

| 路径 | 保留的完整正常合同 |
|---|---|
| 上升创建 | RNG(0,15)先于分配，即使分配失败也消费；成功callback/设备真OR100000，假另RNG(0,1)再取flags选10000/8000。xOffset先flagspub；CC/两W8值4/100/image110004→yOffset→父x/F32 X→父y/F32 Y→另live父y→0.2/非拥有父/W16零→phase=yOffset+父y-100 |
| 上升更新 | 固定父deathFlag==1仅标死早退；其它speed减F32加速度/存F32但保double，再加offsetY/存F32但保double比较S32 phase；低于phase父计数+1/标死，之后Play(0)→X截断→Y取样→X/Y发布 |
| 背景创建 | New(2,0)失败空返回，flags/callback/image/index=-1；image非负才真实getMapNoIndex。原输入低W8 step承接S32 field58、100/F32零/priority3/phase0/publish/batch8/alpha0/X-Y/W16零保 |
| 背景0/1 | 原alpha逻辑U8与step S32相加比较255，未到阈值用另live低W8 step更新；达到写alpha255，可先发布background/phase4，再贯穿1清flags2/batch8，可再次发布background/phase4；该调用不立即标死 |
| 背景2/3/4/default | 2先alpha/step取样、batch8，差值S32>0才低W8扣值返回；否则alpha0贯穿3 batch0/flagsOR2再4标死。3直接同后段，4直接标死，default不增动作 |
| 屏幕创建 | callback one-shot/CC/100；无设备路径先W8 0→flags取样→W8 8→OR8000，之后各路径另live flagsOR40000/image110468/pub/两W8值4/direction0/X/W16零/Y；中间0/8虽后覆4也保原发布 |
| 屏幕发射 | 低W8 phase&7==0才两rand，先Y%480后X%640并创建；之后另live phase--/pub，负值才标死，不在原负phase或创建失败前插早退 |
| 发射创建 | 先原flags取样→callback→character0→OR2/pub→100/priority3/输入phase/W16零；不额外生成首个子特效 |

原AH位操作是寄存器，flags最终仍memW32；坐标/phase/step/计数S32，offset/加速度F32，priority/混合参数/batch/alpha逻辑W8，field270 W16。现alpha访问器读取color2高W8，写时保其它24位；这是现有native packed颜色的逻辑访问，不称当前生成指令为原memW8或ABI等价。三个访问器/两数组保持，未引入新型别、公共API、父判空、owner/free、保护/补零/钳制/溢出回绕或故障模型。

只已证地址采用获批native非拥有T*，既有void callback不消费返回分工保持，不宣原所有EAX返回路径等价。新增语义1360..1366绑定这七原ID、adapted分类和FUNC-BATTLE成员62→69；六正确源不计开发新增，完整枚举和所有完整门禁继续open。唯一取样问题本步解决，不留下已知代码问题待以后修。

<a id="battle-rising-emitter-fade-sampling-acceptance"></a>
## 可度量验收

| 项 | 本步限定证据 |
|---|---|
| 实现 | 七改前根/唯一上升callback补丁、整CPP从HEAD精确增量对照；其余六源与所有caller/声明-访问器保持，RNG/未舍入浮点/五态发布和失败返回定向静态 |
| 原端 | 340条1215B八页19call/19内部普通出口/14ret，五槽switch20B/全部目标，两本地32S32表128B值与实际赋值；五callback绑定/七直接call类型分列 |
| 登记 | 七独立ID/分类、成员69、各函数I/S共14E；七open/唯一进度同步，不代全部父和owner |
| 度量 | 语义1360→1367/有效ID1355→1362、分类466→473、关联1473→1480/3680=40.22%、E515→529；历史定义已有，175队列保持 |
| 完整门禁 | 不构建/测试/运行/摘要；完整consumer-owner/资源重进退出、SDK-ABI-PC53-实际RNG画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、五无ID片段/R7区间67/3061、60源码closed10/完整验收0/P01和七open、全部完整门禁保持。原正常业务零和失败路径保留，异常不专项处理或声称等价。

<a id="battle-rising-emitter-fade-sampling-source-state"></a>
## 当前推进

上升callback取样已修，七创建/消费入口补独立基线。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步实际局部状态。固定快照 `cg2d-re-battle-rising-emitter-fade-sampling-20261006`；网络原格式明文、音频暂停，执行限制与完整目标保留。
