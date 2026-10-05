# BML01 战斗骑乘与群体消息取样

日期：2026-10-06。沿[BMK01位置/怪物目标](cg2d_battle_m_position_monster_sampling_spec.md)继续原427470的RID22/GMN21，按已有工程委托先记录再实施。两根九组正常取样差异、十一补丁定义/十二处替换；其它目标、核心与载体保持。

<a id="battle-m-ride-gmn-sampling-baseline"></a>
## 原端有界基线

| 目标 | 原区间 | 指令 / 字节 | call |
| --- | --- | ---: | ---: |
| RID case22主块 | 428EF3..42998C | 623 / 2713 | 89 |
| GMN case21主块 | 437203..43797B | 552 / 1912 | 78 |
| RID非零substate就绪/完成尾 | 427FA4..427FCD | 12 / 41 | 2 |
| RID Move/58递增尾 | 42998C..4299AD | 8 / 33 | 1 |
| GMN mode0的58递增尾 | 436E0B..436E17 | 3 / 12 | 0 |

五区间1198条4711B十八指令页/170call，见[连续实际字节、分支及源码前态](cg2d_battle_m_ride_gmn_sampling_evidence.json)。两主块1175条4625B十五页/167call，加三个实际共用尾才完整覆盖本组跳转；GMN内437248就绪已在主块，不重复计数。所有本组跳转落入本组指令或已证437A29父回存。父17298含两chunk仍不是完整原正文复核。

GMN4378A2确证调用401EA0；该完整叶只有一条5B尾跳401EC0，沿[既有CryptInt合同](cg2d_legacy_cryptint.md)承接，当前Read_401EC0单次读取正确保持，不造接口差异或新F/X/分类。原父4274F3初始化entrance临时W8为0沿BMG既有字节，GMN当前I8局部0及mode1=2、mode2一次Rand低位+1正确；仅mode2有421700(150)调用，不改变RNG/发射器消费。CopyPayload438270沿BMH既有9条27B逐读/pub W32/W32/W8合同；本组实际RID四call及GMN五call均保持，原inline也由该正确现代核心承载，不伪造原call。

## RID三条真实路径

入口固定payload及一次S32 substate58，0解析，1只ready0命中才Finish，其他非零回存；没有额外406260。mode→caster位置/pub→second位置，借当前caster；空Finish后live58增加。非空固定existingPayload，再Take resultFlags先pub全局，原以同返回W32位80000分派，不重新读全局。

非零mode/80000命中先保存当前firstPosition，second空则Finish/58增量；正常NewYobiAction(2,原24B骑乘载体)，新端按已证两非拥有T*拓宽及原16B非地址部分承载，不改共享布局。固定carrier对象/payload解析TakeStr29、图号映射、gear图号映射、field1D0，四次Take/构造/逐字段copy/析构及flags08。没有Unescape调用，不补入。以固定firstPosition取点写carrier40/44，旧existingPayload.effect1060最后pubcarrier，当前caster追加224；效果先一次caster，原{firstPosition,1,caster,secondPosition}16B值语义及同caster借actor，再live追加40。

非零mode/80000未命中保存firstPosition，NewYobiAction(2,0)结果先pub旧payload.effect1060，固定工厂结果写40；44原先点索引→从旧payload读当前effect指针→点值/float pub。当前caster追加225及同一次caster的效果，然后68、reaction22331、两次58必须保留。借second及固定其payload，W8 flagsFDC bit2为0才写reaction22334，再每次借second追加68/65/Move；为空或bit2命中只进入公共Move/58尾，不加新默认动作。

mode0先创建载体、取其payload、pub旧existingPayload.effect1060，再当前firstPosition创建第一角色，固定对象/payload依次消费名称/图号/gear图号/field1D0/四载体/flags；原fields288 W16依3/2/1/0零写、四次live W32高位设1，FC4=0/FC8=-1/CA4[0]=0/[1]=-1，各点写float40/44/S32 x/y、scale1.0、位置、Album、phase0/flags|2/callback空，再pubcarrier.first00。复制到旧payload每次当前rider：名称、field1D0、四载体；坐标每次先当前caster→当前rider→该caster actor，X正常S32 frameX+float40再写float，Y写当前44。第二角色按secondPosition创建、名称/图号/field1D0/四载体及同样状态/坐标/1.0/Album/phase/flags/callback，pubcarrier.second04。随后live caster224，以同一次caster的actor追加{caster,0,firstPosition,secondPosition}；分别借carrier首/次角色，原W8 bit2命中才各追加46。三个分支共同live caster Move/58增量，固定旧payload和之后当前对象不得统一缓存或新加空保护。工厂失败、原零写、全部取包/载体/RNG和已有224/225消费者合同保持。

## GMN两条真实路径

入口一次substate58：0解析，1先检查槽20再槽21的busyFD8 W32是否等于1，命中返回；否则ready0→live caster406260→Finish，其他非零返回。解析首position、mode0不改变caster：固定该slot，Read mapX1E0一次含计数/RNG副作用，调用后重借slot追加效果(palette,0,40)、174/176/172/168/51；position21范围10..19，其余0..9，S32循环内每个非空成员171/168/Move，各次重借slot，末本slot Move/58增量，mode0不pub5ABA7C。

非零mode先按入口I8生成方式决定entrance，随后Take caster/pub、value1018、effectValue；原effectValue==0映射-1保持。固定首position槽，旧对象存在才MarkDeath，不立即free；新Actor(position)结果先pub同槽，再固定created对象/payload解析名称/图号/field1D0/四载体/flags、FE0/FC4/FC8/CA4/C9C/CA0和两effects.position=-1。四点各次写40/44/x/y，payload位置/created.phase0/两rebirth W8=4，当前palette第五载体构造/copy/析构；PlayFrame重借slot，Text使用固定created.name，current slot载荷写value1018，80/效果{13,0,40,effectValue}/175/174/0/37/167逐次重借。

entrance0固定created scale0/0后current slot54；1 current slot W8 field30F=0→flags|2→406260→171；2四次分别点索引→current slot actor→点值写float40/44，及点索引→actor→payload→点值写FE8/FEC；之后各自重借actor，原__ftol正常返回低S32写x/y，406260/6/10。现float→int64→S32承载正常合同保持，不新异常/PC53模型。

后续范围仍position21为10..19其余0..9，linked初值当时caster；position20/21按序找首非空，找不到保该caster。成员存在才37/167，再当前对象W32 drawMode+8复制color2；原先借field30F对象、形成index==linked条件，再W8 field30F=255/pub并消费同条件。命中追加{position,I8 entrance扩S32再U32,0,effectValue}/73/74；下一动作原先比较index==live caster再借该memberSlot，173或172。调用后再次index==linked才追加0，随后当前member Move。末本slot效果{linked,0,0,0}/25/73/74/Move，非零mode才live caster pub5ABA7C，live58增量。fixed created/slot/payload与current members、两次linked条件和live caster条件各自保留，不跨调用缓存整组角色。

<a id="battle-m-ride-gmn-sampling-decision"></a>
## 九组取样差异裁决

| 组 | 修复 |
| --- | --- |
| RID resultFlags | 428F5F Take→F67全局pub→F74同EAX位80000，保解析W32局部resultFlags；不pub后重读全局。 |
| RID非零效果 | 429173/429206各一次caster用于效果记录caster和actor索引，显式effectCasterPosition/16B effectRecord，两处。 |
| RID空载荷效果Y | 4291D9点索引→1E0旧payload当前effect→1E9点值→1EF写44，不先点值。 |
| RID三byte门禁 | 42928F/42995C/429977均W8 bit2，三处改unsigned char对象字节别名，不W32读后截。 |
| RID骑手坐标 | 429668及429686均caster→旧payload当前rider→当前actor，各自S32 caster位置，X原正常S32/float计算和Y字段保持。 |
| RID零mode效果 | 42993D一次caster用于记录position及actor索引，显式同值，不改{caster,0,first,second}字段序。 |
| GMN entrance2坐标 | 437622/637/64C/667点索引先current slot，后两项再payload后点值，四次独立取样。 |
| GMN linked门禁 | 4377B6借对象→7BC比较linked→7BE W8 field30F pub→7C5同条件，显式flagActor/linkedMember。 |
| GMN caster动作 | 4377FA先比较live caster，再802/80C借memberSlot，显式casterMember/castMember，调用后第二linked判断保持。 |

九组差异十一补丁定义/十二处，单CPP两根；全部其它源/头、正确CryptInt核心、共享布局/owner和其他目标保持。不新增原身份、接口、保护/默认、异常回绕或故障模型。网络仍原格式仅明文，音频暂停。

<a id="battle-m-ride-gmn-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 五区间1198条4711B十八页170call连续字节及原跳转闭合，三个共用尾分列；401EA0独立1条5B尾跳不混入父计数。 |
| 实现 | HEAD整CPP九组十一补丁定义/十二处精确差异，两根正常/失败/返回与原每次取样/三W8/效果记录原序有限静态。 |
| 消费/依赖 | 保RID三路径/十二构造析构和四骑手copy、固定旧payload与当前角色；GMN两路径/五载体及RNG/readCount/markdeath、入场/成员/Move/5ABA7C/58，正确依赖保持。 |
| 台账 | 旧812只追加两目标，不新增F/X/adapted或完整成员；仅双域四局部I/S、七任务open，所有其它指标/完整门禁保持。 |
| 验证 | 必要静态及获准文本汇总/Check；不构建/测试/运行/摘要，不宣编译访问指令或异常等价。 |

<a id="battle-m-ride-gmn-sampling-source-state"></a>
## 当前状态与后续

九组正常差异十二处落码，RID/GMN与三个共用尾有界纳管。完整427470其他消息/技能目标、全部列表状态消费者、资源owner重进退出/借用失效、SDK/SEH/ABI/PC53/时钟RNG/画面及整体R1/R7/R8仍按真实缺口开发验收，不以正确401EA0尾跳或局部取样外推完整通过。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务及已闭10保持；E365→369仅双域四局部I/S。关联率不是完成率，本组14文件，固定命名快照：`cg2d-re-battle-m-ride-gmn-sampling-20261006`。
