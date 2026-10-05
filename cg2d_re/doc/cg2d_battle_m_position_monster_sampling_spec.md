# BMK01 战斗位置交换与怪物消息取样

日期：2026-10-06。沿[BMJ01装备/状态目标](cg2d_battle_m_status_sampling_spec.md)继续原427470的POS/MON两个目标，按既有工程委托先记录本方案再实施。两根九项正常取样差异九处替换；旧槽、当前角色、工厂与固定载荷的区别及所有其它消息保持。

<a id="battle-m-position-monster-sampling-baseline"></a>
## 原端有界基线

| 目标 | 原区间 | 指令 / 字节 | call |
| --- | --- | ---: | ---: |
| case4 / POS位置交换 | 428441..4288F7 | 280 / 1206 | 12 |
| case5 / MON怪物消息 | 4288F7..428EF3 | 353 / 1532 | 46 |
| POS就绪共用406260/Finish尾 | 42AAF5..42AB0C | 7 / 23 | 2 |
| MON失败空目标完成尾 | 43703E..43704C | 4 / 14 | 1 |
| POS旧槽空/phase34增量尾 | 436E0B..436E17 | 3 / 12 | 0 |

五个同属427470的有界区间647条2787B、十二指令页/61call，见[实际指令、分支及源码前态](cg2d_battle_m_position_monster_sampling_evidence.json)。两个主目标633条2738B九页/58call；必须纳入三个实际共用尾。所有本组跳转落在本组指令或已证437A29父回存。最后增量尾只到436E17，后面的下一目标不在本范围。25头表、真实callback绑定、父恢复/回存及正确依赖沿既有合同；17298仍为含两chunk的父统计，不冒充全文复核。

## POS与MON的真实合同

POS入口一次S32 substate58，只有0解析、1进入ready0，其他非零返回后父回存。就绪先GetPet(caster)，再live caster的406260，再借pet；pet为空、characterAction10或phase34只Finish，其他先406260再Finish。初次解析先取位置，形成255条件再发布caster；255直接回存，不消费后续/不Finish。空角色Finish后live58增加，非空才GetPet。

GetPet返回后先caster、player及相等条件，再借moving、固定非拥有旧槽T**及旧位置；仅该条件命中发布player为pet。moving四坐标分别取样：首x00先pet点索引、moving.payload、点值；随后y04/x08/y0C各自caster、pet点索引、当前actor、payload、点值/pub。随后当前caster载荷写battlePosition18=pet。pet非空时先该payload、当前caster再写battlePosition18；再次借pet/phase非34才交换其坐标，首x00为caster→已有pet.payload→点索引→点值，后面三次为caster→当前pet actor→点索引→payload→点值/pub。不能把全部点值或载荷提前缓存。

交换依次取当前caster和pet替代对象、发布caster=pet、旧索引槽=replacement、pet槽=moving。moving的rider parent绑定后按当前caster再次借rider写位置；旧槽固定，旧rider位置写旧位置。原4286E8在旧槽非空门禁中重绑新槽两辅助附件，随后新槽非空块又重绑同两附件，并处理新主gear；旧槽末尾只处理主gear。这是原非对称重复，不能改成对称重绑或去重。随后新槽value1018=210、80/11→Move→live value54=3.0F/rate100；旧槽存在且phase非34再11→Move→各次重借旧槽写3.0F/100，最后live58增加，旧槽空/phase34经436E0B也增加。两现代遍历只承载原两展开附件，不新增原循环或身份。

MON入口沿原恢复目标及一次substate58：0解析并先形成actor存在条件再pubcaster，空Finish且不增58；正常GetPet同解析position并pubdefender。2沿恢复defender重试，其他非0/1/2返回。pending存在且busyFD8 W32等于1时只写58=2，不继续耗包；其他继续41，caster→actor→defender→payload/pub target100C，Take resultFlags/pub后再Take value。

第二整数返回后先取caster再player比较，玩家条件且(flags==0或value!=-1)才为该caster追加效果(-2,0,0)，接着live caster追加223、调用后再取caster。未命中则保该已取caster；两路共同先defender再用相应caster借actor，追加目标效果。flags==0失败路追加68、写reaction22339、追加67；再固定defender位置/借对象，空经43703E Finish返回，不推进。非空flagsFDC原W8位2命中才以同位置Reset，调用后重读defender；没调用则保先前位置，两路再借actor写205、live追加80/53/51。

成功路固定monsterPosition和非拥有defenderSlot，槽空才NewBattleActor(same position)，返回值包括null先发布同槽；再借该槽对象及固定payload。保原工厂失败路径，不新增空保护/默认对象。顺序为TakeStr29→Unescape52D150→Take/图号映射→field1D0；四次Take→CryptInt构造→逐字段CopyPayload→析构，分别readiness/maxHp/barValue/barMaximum。内存载体保持原合同，不扩大网络加解密排除。随后flagsFDC W32 pub、stateFE0=0，四fields288 W16依3/2/1/0写零，四W32高位条件依次写对应W16为1，每次live读flags。固定payload依次写FC4=0/FC8=-1/CA4[0]=0/[1]=-1；四坐标各次live取defender点，S32→float写40/44，再S32写x/y，scaleX/Y正常零，固定payload位置写defender→Album4238F0→固定对象phase0，随后live68、reaction22338、67、当前defender写204、80/54。两路共同live caster效果(-1,0,0)→Move→58=1。四载体/字段宽度/正常零写、生命周期与随机等既有正确依赖保持。

<a id="battle-m-position-monster-sampling-decision"></a>
## 九项差异裁决

| 项 | 原取样及修复 |
| --- | --- |
| POS位置门禁 | 4284C5比较255→4CA caster发布，保同解析位置的角色判断/GetPet参数；保存条件再发布，不先重读已发布全局。 |
| POS玩家与moving | 4284FB caster→501 player→50A比较→50C moving→513旧槽→51A旧位置→条件/pub，显式初始位置与movePlayer。 |
| POS moving坐标 | 恢复四次点索引/角色/payload/点值顺序，首只借moving，其余各次live角色，四写不统一缓存。 |
| POS pet位置/坐标 | 原5C3 payload先5C6 caster再pub；首坐标caster→pet.payload→点索引，其余caster→pet actor→点索引→payload→点值，显式局部阶段。 |
| MON空角色条件 | 4289B6 test actor→9B8 caster pub→9BD分支，保存actorPresent，不提前Finish或增58。 |
| MON target100C | 428A0A caster→A11 actor→A18 defender→A1E payload→A21 pub，显式三个局部，避免赋值右侧先defender。 |
| MON效果两路径 | A37 caster先player，命中用该caster的actor；A84只在223调用后重新取caster，共同A8D defender→A93 actor；保存该跨分支S32位置，不跨调用固定actor。 |
| MON失败W8/位置 | B14是byte flagsFDC位2，不W32读后截；Reset用已读defender位置，只在调用后B23重读；没调用沿原位置再借对象写205。 |
| MON工厂固定槽 | B89目标位置只读一次、B95形成槽、工厂同位置、返回pub同槽；显式monsterPosition，失败合同/固定对象和payload保持。 |

单CPP两既有根九处替换，其它正文、声明与共享布局保持。只现代非拥有T*/T**与S32局部，不引新原F/X、保护、业务默认、owner或异常模型。原格式仅明文/音频暂停持续。

<a id="battle-m-position-monster-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 五区间647条2787B十二页61call，实际连续字节/页界/本组跳转及三个共用尾；不含下一目标。 |
| 源码 | HEAD整份指定九处精确差异、两根正常/失败/返回、取样及字节宽度有限静态；旧槽/重复附件/当前对象与固定payload/四载体/列表/58保持。 |
| 接线 | 既有427470真实case4/5调用及父回存保持；不创建新helper身份或宣完整父已验。 |
| 台账 | 旧812仅追加两目标，不新分类/完整成员；双域四局部I/S、七任务仍open，全部其它计数/完整门禁保持。 |
| 验证边界 | 只必要静态和获准文本汇总/Check，不构建、测试、运行或摘要；不宣编译后访问指令或异常等价。 |

<a id="battle-m-position-monster-sampling-source-state"></a>
## 当前状态与后续

两根九项取样差异九处落码，两个主目标与三个共用尾有界纳管。完整427470其余消息/技能目标、所有列表状态消费者与资源owner重进退出/借用失效、SDK/SEH/ABI/PC53/时钟RNG/画面及整体R1/R7/R8继续按实际缺口开发验收，不因本步缩减。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、文档关联1463/3680=39.76%、R7区间67/3061/队列175及60任务已闭10保持，E361→365仅双域四局部I/S。关联率不是完成率。本组14文件；固定命名快照：`cg2d-re-battle-m-position-monster-sampling-20261006`。
