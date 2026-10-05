# BMM01 战斗道具与连击消息取样

日期：2026-10-06，沿[BML01骑乘/群体目标](cg2d_battle_m_ride_gmn_sampling_spec.md)继续427470的ITM7/COM8。按既有工程委托先记具体方案再实施，单CPP两根二十四组差异/二十七处替换；原正常/失败/返回和调用后live重读保持。

<a id="battle-m-item-combo-sampling-baseline"></a>
## 原端有界基线

| 区间 | 指令 / 字节 | call |
| --- | ---: | ---: |
| ITM7：4369F9..436FD8 | 391 / 1503 | 59 |
| COM8：42781E..427F98 | 542 / 1914 | 52 |
| ITM mode0就绪：436FEB..43701F | 14 / 52 | 3 |
| ITM mode6就绪：43710F..437144 | 14 / 53 | 3 |
| COM就绪406260/Finish：42AAF5..42AB0C | 7 / 23 | 2 |

五区间968条3545B十五指令页/119call，见[实际字节/分支及源码前态](cg2d_battle_m_item_combo_sampling_evidence.json)。两主块933条3417B十二页/111call，三个实际共用尾另外列出。ITM自身436E0B增量尾已在主块，不重复计数。COM42782E为间接四态分派，其表437B38四W32/16B实际目标0→427835、1→427EC7、2→427EEA、3→427F5A/default437A29；437B38是表地址，不伪作代码目标。所有直接跳转及表目标落入本组或已证437A29父回存。

父25头表/真实callback/15恢复及13回存沿BMG合同，未据17298含两chunk父统计称全文已验。三个共用尾复用已证BMJ/BMK实际字节，保持各调用次数/宽度。

## ITM的三个正常模式

入口一次substate58 S32，0解析，1仅恢复mode0/6/7允许就绪；其他非零返回。mode0走436FEB、6走43710F、7走本块ready0→live caster406260→Finish，三路不新增动作。解析mode后pub59FA10，仅0/6/7继续解析caster/pub及借角色，空Finish而不增58。mode7固定已借actor载荷写1018=438，live 80/205/38/37→Move→58增量，不消费目标和值。

mode6和0先已借actor25→live40/138。6再live73/0→Take target，先比较255再pubtarget，255直接回存，不Finish/增58。目标空Finish并直接返回，正常Take value→先target→pub59FA14→该target当前actor/payload写1030=value；caster→target→caster actor→效果(target,0,0)，live target0/154/155/156。随后target→caster→target actor→反向效果(caster,0,0)，live target73/74，再**live59FA14==1**决定caster68/reaction28880/三次57及target51，其他caster68/reaction22333/两次58；两路caster74→Move→58增量。不得把调用后的消息值判断改成最初value或合并重复动作。

mode0再live71、Take固定kind value，循环Take target，比较255先于pubtarget；255结束循环，目标空Finish后仍走公共效果(-1,0,0)/Move/58增量，不改成return。每个正常目标Take parsedMessageValue，先caster、target再pub59FA14，用同caster借actor追加目标效果。其后target→kind==7条件→actor/payload→castSkillLevel1044正常零写，再消费同条件追加150/149，live target160，随后target→actor→live59FA14→payload写1030，再live target载荷写1034=5。所有目标结束后live caster效果-1→Move→58增量；不为哨兵/空目标补额外取包、保护或默认值。

## COM的四个真实子状态

0保留原初始化和两图号/两数组/values跨caster消费，value00/entryCount04/59FA14先清零，取target/pub并借初始target，空Finish返回；初始target追加0。caster循环先比较255再pubcaster；255结束，空caster Finish后仍走末目标处理；正常Take resultFlags/pub再Take amount。bit800000才TakeAuxGraphics43BF30，未命中保原图号和数组。current caster载荷castSkill103C=-1，再取W32 flags，E000未命中以同S32累计值pub59FA14并作effectAmount，命中用原amount。效果依caster→target→actor，保持16B {target,flags,amount,image}值语义。参与者数组先读entryCount04再caster，pub对应entryPositions08，然后live count增量；不加容量保护/默认空列表amount。amount仍只在每个非空caster解析时写，不擅补初始化。

当前caster25/7/9/10/72/0；两图搜索原S32两个索引，首个firstImages>0命中选该index，否则0。辅助参数按selectedImage→secondImage→firstImage→live flags→target→caster→actor取样，43BC10一次；再live14。flagsE000未命中写当前value1038=1并live重取flags。原bit40/低2/bit2000各分支及调用后flags刷新保持：40命中/低2/2000先caster34、target127/0，调用后再判2000，caster→player比较决定同caster256或同caster52后live caster51；40未命中且amount非0/低2按8000→target129/0、4000→128/0、2000→caster165/27及target127/0，各对应调用后flags重读。原循环165/27先bit2条件→caster→动作号→actor；末target165/27则先动作号→target→actor，两处不统一。flags2020齐时caster11/46，否则caster7/11/10，Move后live caster pub5ABA7C，再下caster。

循环结束按当时5ABA7C当前actor载荷写1038=0，取flags；E000命中对当前target同对象FC8-1作AppendAt(scene,target,-1,index)。其他末处理保40/低2、player/caster/target比较和256/52/51、amount/flags8/4000等原门禁及调用后live flags。末11/46存在跨门禁目标位置寿命：11允许时先target和lastCaster，条件命中以该target借actor追加11，调用后**先重读target再flags**；门禁失败时也原读取target，11位置条件失败则保该已读target。46门禁flags20，原先形成bit2000条件再读lastCaster，比较同已缓存target并以该target追加46，不重新取全局target。末ParseRide4239D0→live58增量，原异常/未初始化输入不另设确定值或故障模型。

1只value00>=entryCount04才value00=0/58增量，随后返回。2借entryPositions08[value00]当前actor，phase0才Advance439340；调用后count→value00门禁，value00>=count-1才检查flags404未命中且同count>=2。满足则先live5ABA7C，再entryCount04原W8对象字节读取/作为I8给420100，其余不调用420100；所有满足外层门禁路径最后58增加。3 ready0后借target，空或flagsFDC原W8 bit2命中只Finish，否则先406260(target)再Finish。未知状态直接回存，不能因为切片包含表就宣全父完成。

<a id="battle-m-item-combo-sampling-decision"></a>
## 二十四组差异裁决

| 组 | 恢复 |
| --- | --- |
| ITM两目标门禁 | 436BD5/436EB0先255比较，之后target pub，保存同条件，两个源码点。 |
| ITM mode6值 | 436C03 target读取先C09消息值pub，用该target查当前actor/payload写1030。 |
| ITM mode6正向效果 | 436C22 caster→C27 target→C2D actor，再EffectList。 |
| ITM mode6反向效果 | 436C9B target→CA1 caster→CAA actor，再EffectList。 |
| ITM mode0消息/效果 | 436ED5 caster→EDB target→EE1消息pub→EE7 actor，显式各值和非拥有actor。 |
| ITM kind条件 | 436EF6 target→EFF kind==7条件→F02 actor/payload→F0C零pub→F12同条件。 |
| ITM当前消息写入 | 436F5D target→F66 actor→F6D消息值→F73 payload/pub，不能赋值右侧先消息值。 |
| COM序列参数 | 427F30先lastValue，F36 entryCount原W8读/I8参数；不W32读后截。 |
| COM就绪flags | 427F86 flagsFDC W8 bit2，保空/命中仅Finish和未命中406260。 |
| COM caster门禁 | 427888比较255先88D caster pub，同条件控制结束。 |
| COM累计值 | 42791B同S32加法值pub后作效果量，不再读取发布后的全局。 |
| COM效果参数 | 427935 caster→93C target→941 actor，16B记录字段原序保持。 |
| COM参与者写 | 42794F count→952 caster→95B数组pub；随后原live count读取/增加保持。 |
| COM两图索引 | 4279EA/9EC两个非地址索引保S32，两轮/默认index0保持，不拓size_t。 |
| COM辅助图号参数 | 427A03选图→A07第二图→A0B首图→A10flags→A17target→A1Dcaster→A25actor。 |
| COM两caster玩家门禁 | 427AD9/427D12 caster先player，首个256/52沿同caster，调用后的51仍live，两点。 |
| COM target玩家门禁 | 427D5D target先player，首个256/52沿同target，后51仍live。 |
| COM循环165/27 | 427BB9 bit2→BBB caster→BCA动作号→BCE actor。 |
| COM末165/27 | 427DBE动作号→DC2 target→DC8 actor，不与循环统一。 |
| COM末11/46目标 | 427DEF..427E84缓存target，11调用后先target再flags，其余路径target也读取；46位2000条件先lastCaster再用同target，保各分支/读取寿命。 |
| ITM同解析target借用 | 436BE5/436EC0按同解析target查actor，不在pub后重读全局，两点。 |
| COM同解析caster借用 | 427898按同解析caster查actor，原255/空结束合同保持。 |
| ITM初始caster借用 | mode0/6/7同解析caster用于pub与actor索引，三原点由同现代入口承载。 |
| COM初始target借用 | 427852 pubtarget/857索引actor共用同解析S32位置。 |

二十四补丁定义/二十七处替换，两根不新增接口/原F/X、保护/默认、owner、异常回绕/故障模型；所有其它源、头和正确核心/布局保持。网络原格式仅明文、音频暂停持续。

<a id="battle-m-item-combo-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 五区间968条3545B十五页119call、三个共用尾与原直接跳转闭合；间接42782E单列实际四W32/16B表及目标，不按表地址作代码跳转。 |
| 实现 | HEAD整份单CPP二十四组二十七处精确差异，原各取样、S32/W8/正常零写、列表重复和调用后live重读有限静态。 |
| 消费 | ITM0/6/7不同结束与后续消费、COM四state/四图数组/累计值/参与者/辅助效果/末目标位置寿命、RID/58保持；不补empty amount默认或新增边界。 |
| 台账 | 旧812追加两个目标，不新身份/adapted/完整成员；仅双域四局部I/S、七open任务，全部其它指标/完整门禁保持。 |
| 限制 | 只必要静态与获准文本汇总/Check，不构建/测试/运行/摘要，不宣编译访问指令或异常等价。 |

<a id="battle-m-item-combo-sampling-source-state"></a>
## 当前状态与后续

二十四组正常差异二十七处落码，ITM/COM及三个共用尾有界纳管。至此24个非技能消息头已按BMG/BMH/BMI/BMJ/BMK/BML/BMM相关分组记录有界基线；仍含技能头0的完整427470及SEH、全部列表状态消费者/资源owner重进退出/借用失效、SDK/ABI/PC53/时钟RNG/画面和R1/R7/R8需继续开发验收，不将24目标或正确接线代完整父/运行完成。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务及已闭10保持；E369→373只双域四局部I/S。关联率不是完成率，本组14文件；固定命名快照：`cg2d-re-battle-m-item-combo-sampling-20261006`。
