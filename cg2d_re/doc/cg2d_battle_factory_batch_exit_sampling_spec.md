# BQG01 战斗工厂、批次显示与退出取样SPEC

2026-10-07。依据[战斗尾段取样](cg2d_battle_tail_sampling_spec.md)、[定向原端证据](cg2d_battle_factory_batch_exit_sampling_evidence.json)、原新函数台账和既有验收体系，只推进本批缺口。未决工程选择按用户授权裁决，原端事实以已开IDA会话只读取证为准；先写本SPEC再修改。

<a id="battle-factory-batch-exit-sampling-baseline"></a>
## 原端基线与差异

| 原入口 | 新符号 | 指令/实际B/页 | call/普通jump/ret | 源交付 |
| --- | --- | --- | --- | --- |
| 0x004071C0 | UpdateBattleAction249_4071C0 | 79/288/1 | 6/11/1 | 修复1处 |
| 0x0040FC10 | UpdateBattleFactoryAction_40FC10 | 203/561/3 | 2/61/1 | 正确体保持 |
| 0x0040B180 | UpdateBattleAction251_40B180 | 31/122/1 | 5/0/1 | 正确体保持 |
| 0x0040FE50 | UpdateBattleAction253_40FE50 | 31/83/1 | 3/3/1 | 正确体保持 |
| 0x0040FEB0 | UpdateBattleAction254_40FEB0 | 18/54/1 | 2/1/1 | 正确体保持 |
| 0x004186B0 | UpdateBattleAction255_4186B0 | 339/1176/5 | 29/44/1 | 修复4处 |
| 0x00418B50 | UpdateBattleAction256_418B50 | 3/21/1 | 0/0/1 | 正确体保持 |

七原根完整704条2305B/十三页，47个call/22个callee、120个普通内部跳转/七ret，七个唯一父404DB0调用。四原窗口实际20条69B；256调用后直接落入40563F，无jmp，不统一虚计七组push/call/jmp。原工厂为30个CMP创建分支，没有物理switch表；父31个逻辑case共用一个F身份，118无创建分支，250/252各类型41/42。既有父表118/250/252三项分别405914/405B24/405B2C、字节0f534000、同目标40530F，落在同一次call405310；不新增R7分类或多个原函数。

发现的五处读取表达差异：

| 片段 | 原端依据 | 决议 |
| --- | --- | --- |
| 249事件区间 | 407231先MOV EAX，407237/407242共用EAX比较 | 位置哨兵之后仅取一次frameEvent，用event做两次比较 |
| 255归零复制 | 4188B4位置→4188B6临时encoded→4188BA共享目标→encoded写→mask读写→count读写 | 保固定effect，显式按原序取样发布，不调用先取目标的普通CopyPayload |
| 255激活 | 4188E3 TEST BYTE [ESI+4],20 | 独立低W8 activationFlags；目标FDC仍W32 |
| 255属性 | 4189E6/418A85 MOV AL,[ESI+4]，各TEST AL,2 | 恢复和普通两原路径共用一个低W8 attributeFlags表达式 |
| 255共同尾 | 418AAB TEST BYTE [ESI+4],8 | 独立低W8 tailFlags后按原amount/FDC计算时长 |

正常旧值一致；这是原取样次数、宽度和发布阶段恢复，不宣称数值、并发或机器故障。FDC以及原初始/命中/显示W32加载再AL/AH寄存器测试保持。其余五个完整正确源体保持。

<a id="battle-factory-batch-exit-sampling-decision"></a>
## 完整正常源合同及开发裁决

1. 249：固定payload，characterAction=5/framePlayingRate=100；当前effect位置非-1才Turn(source,共享目标,1,0)，PlayFrame(source,1)恰好返回1才推进施法。调用后重新取得当前effect位置，-1则frameEvent=0返回；其后一次W32读取frameEvent，同一值比较[10000,20000)。原407231 MOV EAX配407237/407242两个CMP，只新增event局部，不提前到位置哨兵之前。区间内原do循环每次先读level：>=12生成图111030，否则skill2005图104222/4003图104238；其它skill不生成仍Advance。每次创建取得当前C9C及record，直到AdvanceBattleEffect恰好返回1，原无新循环限制。最后声268/source.x→frameEvent0；图值、调用后独立取样、返回消费和正确业务保。

2. 40FC10工厂共体：固定payload，FDC为W32读后AL寄存器bit60判断，清时PlayFrame(source,0)弃返；之后重新读value1024，非0不创建仍phase162。为0才一次读取phase按原30个CMP/类型分支创建，原不是物理跳转表：130→13、116→1、117→2、133→25、149→16、150→18、151→21、152→22、153→23、119→4、120→5、121→7、122→8、123→9、124→10、125→11、126→12、134→26、135→27、136→28、137→29、131→14、132→15、146→6、157→37、158→38、161→39、217→40、250→41、252→42。匹配后NewEffect(source,0,offsetY0,type,0,1)，返回弃置、不保存+10；不匹配无创建，所有路径phase162。父31个逻辑case复用这一个原F身份；118实际表项405914同40530F但没有创建分支，走默认仍phase162。250/252表项405B24/405B2C同目标，既有父表不重新R7分类。源switch只是既有CMP语义承接，完整正确共体保持。

3. 251：ConstructFromInt(temporary,1)后先齐读mask W32、encoded W32、readCount W8，再按同序发布source.readiness→Destroy；同temporary再次Construct1、三字段齐读后同序发布source.barValue1B0→Destroy，最后439340(source)。两个构造/RNG消费和三字段取样发布已正确，不替换为交替读取的普通CopyPayload，不改正确体。

4. 253：固定const payload，FDC W32取样后低寄存器bit60判断，清时PlayFrame(source,0)弃返；调用后独立读取source.phase恰好253且独立payload.value1024为0才NewEffect(source,0,offsetY0,43,0,1)，返回保存source.battleCastEffect10。全部路径439340(source)。不复用播放前phase/value，不窄化原W32 FDC，不改正确体。

5. 254：source.battleCastEffect10非拥有指针先取，payload后取；前者非空才MarkDeath(cached effect)→source.+10=null→固定payload.value1024=0，所有路径439340(source)。原空分支不清value1024，调用后的清槽与取样顺序已正确，源体保持。

6. 255入口/创建：frameEvent先读、payload后固定，characterAction5/rate100；事件[10000,20000)进入原无限循环。每轮取当前C9C并固定effect引用（原ESI），fixed.position=-1或shared target=null均按原break到批次尾；缓存target与targetPayload。仅入口index0创建type47，position<10坐标500/450，否则150/50，NewEffect(source,x,offsetYy,47,0,1)弃返。创建后fixed effect.flags W32取样选主图：4→110000，400→FindTechnique(8,fixed.position)<12为110098否则111048，200→110135，否则重新读parent.C9C==0为110135，余路径一次RAND(0,2)+110001。固定effect.flags再W32读写cached target.FE4，之后fresh parent.battlePosition调用ApplyFlags(source)。固定引用不替换成每次fresh C9C。
255命中/扣减：独立fixed.flags W32命中缓存，8目标phase31；100无旧flash门禁，flash1→声210/source.x→phase31。其它E000清且fixed.amount非0，cached bit2目标110008、主图非负目标主图、Rebirth(source,target)→声134/source.x。扣HP参数严格fixed.position先、fixed.amount后、shared target T*最后；Less0另读fixed.position/shared目标。负值Construct0后fixed.position→临时encoded→shared目标指针，先写目标encoded，再读/写mask，再读/写readCount，Destroy；不是先取目标后CopyPayload，也不是三字段齐读。随后fixed.flags实际低W8 bit20，cached targetPayload.FDC保W32|2、全局更新门1、alive1；此处原4188E3是TEST BYTE，新增独立activationFlags。
255显示/恢复：fresh fixed.flags W32主缓存，8000仅439340(target)；4000先439340(target)，fixed.flags10000000当前fixed.position共享目标图22459后跳属性；否则shared[fixed.position]作为healTarget，恢复量确实读取healTarget.counter1DC（原41896C MOV [EAX+1DC]），AddHP，再fresh fixed.position共享目标GreaterHP/max，超过原上限才fresh同位置目标普通CopyPayload。不改为effect.amount；恢复数显fixed.flags W32→cached target.Y→fixed.amount→target.X，type5/style2。2000只439340(target)。其它100000回收cached targetPayload.effect1054，调用后FDC先读、槽null后FDC&~20；439340(target)后fixed.flags W32，10000000使用fresh parent.C9C的current.position共享目标图22459并跳属性，此原独立重新定位保；否则target.Y→fixed.amount→target.X/type6/style2。恢复和普通数显后原4189E6/418A85各fixed.flags低W8 bit2，cached target图22454，Attribute(source,cached target)；源共享一attributeFlags低W8表达式承接两个实际路径，不计为两个源片段。
255共同尾：fixed.flags实际低W8 bit8清且fixed.amount非0，按cached targetPayload当前FDC W32 bit1求14*(bit+1)写fixed parent.FD0；此处原418AAB TEST BYTE，新增tailFlags。不改变W32初始/命中/主显示/恢复/回收缓存及AL/AH寄存器消费。Advance(source)恰好1结束批次，否则下一轮fresh C9C。原位置/空目标break及正常结束共同声268/source.x→speed8/rate100→parent.FF0=0→frameEvent0；共同PlayFrame(source,1)返回1才439340(source)。接收者、HP来源/上限、独立取样、原失败/哨兵、返回/RNG与调用顺序保持，无新异常、边界或迭代保护。

7. 256：原三条/21B写共享gValue5ABB1C=0→gValue5ABB20=1→ret，忽略父传入action。源无参数名保持；唯一父40563A调用后直接落入共同尾40563F，没有额外跳转。只纳管该原完整身份，不扩展为独立生命周期或父完整验收。

七完整原根704条2305B/十三页、47call/22入口/120普通内部jump/七ret，七唯一404DB0入向四窗口20邻指令/69B（256父直接落尾）及118/250/252三原表项。只249一次事件W32取样和255归零encoded先目标、三个独立低W8源表达式承接四原点，共五源片段/两源体；正常旧值一致，不称数值/并发或机器故障。厂共体30创建类型/31父case（118默认无创建仍phase162）、251两临时齐读、253原W32 FDC/后取phase与保存指针、254先借指针再回收、256双全局发布正确体保。255固定effect与普通免疫fresh C9C差别、实际healTarget.counter1DC恢复量、W32缓存、原接收者/HP上限copy/RNG/播放尾保持。工厂原无物理switch表，不新R7/父表分类。不外推递归父/全部子依赖/其它生产与owner寿命、SDK/生成机器ABI/画面运行；音频暂停、原格式明文，RPLAN.24不专项异常/保护/回绕/故障等价及原验收标准保持。

<a id="battle-factory-batch-exit-sampling-acceptance"></a>
## 可度量的限定验收

- 七原根逐指令连续字节、完整十三页、704条/2305B、47call/22callee、120jump/七ret、七父四窗20条/69B和三既有表项可追。
- 整YobiRuntime只249一处及255四处，共五源片段；255三个低W8源表达式对四真实原消费点。249一次event区间、255归零字段交替发布及恢复counter1DC/固定effect与fresh C9C差别有实际原端依据；厂30创建/31父case和118默认、其余五正确体/共享helper保持。
- 新增1496..1502七原已有正常定义/equivalent，七冻结定位全文保存。语义1496→1503/有效1491→1498、关联1609→1616/3680（43.91%）、BATTLE173→180枚举open、分类621→628/E764→766；只有两项局部I/S，各十五个独立实际来源。
- R7 260/3061/余2801、差队列172/待归属43和其它全部机器对象/完整门禁保持。U08/R01/R04/V04只当前描述/引用仍open，60任务源码closed10/完整验收0/叶0/global_accepted=false保持；完整战斗及R8不以本批静态替代。

<a id="battle-factory-batch-exit-sampling-source-state"></a>
## 当前交付

249事件一次取样、255归零encoded先目标及三个低W8读取实修；249/40FC10/251/253/254/255/256七完整正常基线纳管。命名快照 `cg2d-re-battle-factory-batch-exit-sampling-20261007`；定向静态及measure/-Check记录唯一进度。未编译/调试/测试/运行/计算摘要，音频暂停、网络原格式明文及原验收标准保持。
