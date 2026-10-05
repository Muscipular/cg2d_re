# BMN01 战斗技能入口与等待态取样

日期：2026-10-06。沿[BMM01道具/连击](cg2d_battle_m_item_combo_sampling_spec.md)推进剩余SKL头0，按既有工程委托先记录后实施。三既有根四组正常差异、七补丁定义/八处替换；原各技能主体不在本步冒充验收。

<a id="battle-m-skill-entry-waiting-sampling-baseline"></a>
## 原端有界基线

| 区间 | 指令 / 字节 | call |
| --- | ---: | ---: |
| SKL入口/等待态：42A768..42ACFD | 357 / 1429 | 21 |
| 五整数/等级归一化：42ACFD..42AE37 | 91 / 314 | 5 |
| Caster就绪尾：429E8A..429EBF | 14 / 53 | 3 |
| Caster就绪尾：437248..43727D | 14 / 53 | 3 |
| Caster就绪尾：4379D9..437A06 | 14 / 45 | 3 |

五区间490条1894B十指令页/35call，见[实际字节、分支/表和源码前态](cg2d_battle_m_skill_entry_waiting_sampling_evidence.json)。两主块448条1743B七页/26call，三共用尾另外列出；入口内42AAF5/Finish已在等待主块，不重复计数。五个等待态压缩表实际**28跳槽/112B+400索引槽/400B=512B**，逐表保存真实元素宽度/低case/默认与case映射，不按ncases误读跳表长度或把数据地址作代码目标。所有等待直接/表目标落本组或已证437A29回存；state0从入口进入本组五整数，归一化末42AE37是正常技能主体分派边界，该边界后的表/各主体另验。

## 入口和state2

原42A76C packetSubState58只读取一次，以同S32值分0/1/2，其余返回后父回存。现代ProcessBattleMSkillHeader保存subState，再把同值传入private ProcessBattleMSkillWaitingState，不第二次读payload。仅现代内部参数调整，不新增原函数/公开接口或改原callback/25头表和父回存。

state2只允许19..31、61..63、83、400..402、2028、2055..2057、2068..2079、3037，共37个技能。count04==0仅写58=1/回存。其他按S32 count，除最后参与者外phase W32==0各计一，最后参与者phase==1计一；readyCount<count返回。满足时再live caster phase，0或1才调用439340推进**已借最后参与者**，不是用于phase门禁的caster，调用后固定scene payload58=1。原三段相同循环由既有统一源承载，count-1/各次position/actor读取和最后对象区别保持，不改成caster Advance或增空/容量保护。

## state1的七类结束

低段和高段稀疏比较/五压缩表确定七类，现代既有技能集合保持。所有匹配类只ready命中1才推进；未知技能/state不添加默认动作。skill68的Caster ready参数1，其它0，原68先push1跳43724A，不能在共用尾437248重新push0。

Caster经各不同原共用尾live caster406260→Finish。LastCaster经42AC5F ready0→live last位置/actor/payload，flagsFDC原W8 bit2；空或位命中只Finish，位未命中后**live skill==105**才固定lastPayload.flash1004=0，再live last位置/actor406260→Finish。OnlyFinish仅ready0→Finish。ClearCaster原ready0→live caster406260→Finish，然后每次live caster分别frameEvent W32=0/flags W32清bit2；HasLegacy3DDevice44B610之后再live actor，存在清100000，否则W32 flags读/寄存器CH清位/原W32 pub等价清8000，立刻返回，不再第二次Finish；原后续live读取/正常零写和批准视频边界保持。

BothCasters ready0，分别live caster和last，存在且各flagsFDC原W8 bit2未命中才406260对应已借actor；第一调用后last重新借，不跨调用统一缓存，末Finish。CasterFlash ready0/live caster/W8 bit2门禁，406260之后再live caster/actor/payload写flash1004=0，末Finish。skill98原恢复value88==7才EffectTargets，否则Caster；EffectTargets固定当时casterPayload及effects1C，以原16B记录步长按position=-1结束，每个目标存在/characterAction非10/phase非34才406260，调用后沿固定记录序列继续，最后Finish。不扩音频、owner、目标数量或异常模型。

四FDC字节读取为42A9A1 CasterFlash、42AAC6/42AAEC BothCasters两点、42AC8A LastCaster；原AL/AH寄存器上的运算和其他W32 flags/phase读取保持，不误改所有flags为byte。

## state0五整数与正常等级归一化

依次Take initialTechnique/pub technique、Take fpUsed到S32输出、Take skill/pub、Take skillLevel/pub、Take requestedLevel。五Take顺序及初始technique和live technique区别保持；第五返回后**live technique>=205000**决定高段，低段仍使用最初initialTechnique，不能统一成同一变量。

高段live skillLevel%30，原先形成remainder<1条件再pub remainder，同条件控制业务范围下限1；不命中再remainder>=15上限15。requestedLevel独立原1..15范围归一化，pub levelIndex=requestedLevel-1，再live skill返回。低段最初technique/100为S32 skill、%100为technique，先pub skill，然后**比较technique>=50，再pub technique和index**，同条件命中%50并依次pub两个字段；否则>=30才%30/pub二者。然后technique>=14取14、<=0取0，pub technique/index的原次序保持，最后skillLevel=technique+1、返回同已计算skill。原Magic乘积/有符号idiv由正常S32 /和%承载，原业务范围归一化/零写保持，不设数学回绕或异常确定值、输入拒绝、额外钳制。

<a id="battle-m-skill-entry-waiting-sampling-decision"></a>
## 四组差异裁决

| 组 | 恢复 |
| --- | --- |
| 一次subState | Header一次S32取样，private Waiting增加同值参数并移除第二次payload58读取；两个源码点，原0/1/2和其它返回保持。 |
| 四FDC W8 | LastCaster一处、BothCasters两处、CasterFlash一处均unsigned char对象字节别名，不W32读取后截断。 |
| 高段remainder条件 | 42AD57比较<1先AD59 pub remainder，保存belowOne再pub/消费条件；原后续>=15及requestedLevel处理保持。 |
| 低段50条件 | 42ADC9 pub skill→ADCF比较>=50→ADD2/ADD8 pub technique/index，保存reduceByFifty后再pub二字段/同条件消费，30与14/0后续原序保持。 |

三根七补丁定义/八处，所有其它源/头/布局/正确核心和正常主体路由正文保持。原辅助函数不新增F/X，源private参数不外露SDL/Win32对象，不改变flags9C/valueA0/packet布局或owner；网络原格式仅明文、音频暂停持续。

<a id="battle-m-skill-entry-waiting-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 五区间490条1894B十页35call、三共用尾和本组直接/五压缩表目标闭合；512B表按真实长度/宽度核；42AE37明确作为后续分派边界。 |
| 实现 | HEAD整CPP三根四组七定义八处精确差异，唯一private Waiting定义/调用同签名，无第二58读取、四W8和两条件/pub时序有限静态；其余正文和头保持。 |
| 消费 | 原37技能state2/最后参与者Advance、七finish路径及68参数1、调用后live和Clear先Finish后清字段、五Take与原业务等级范围/RID之外不新消费。 |
| 台账 | 旧812仅追加入口/等待态/五整数有界合同，不新身份/adapted/完整成员；双域四局部I/S、七任务open和所有完整门禁/其它指标保持。 |
| 限制 | 必要静态/获准文本汇总和Check，不构建/测试/运行/摘要，不宣编译访问指令/SDK/SEH/ABI或异常等价。 |

<a id="battle-m-skill-entry-waiting-sampling-source-state"></a>
## 当前状态与后续

四组八处落码，完整有界等待段、五整数归一化及三个共用尾纳管。正常技能主体分派从42AE37开始，各技能主体/所有列表消费者及资源owner重进退出/借用失效、SDK/SEH/ABI/PC53/时钟RNG/画面和整体R1/R7/R8继续实际开发验收；不以512B表、局部490或父17298统计宣完整SKL/427470完成。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务及已闭10保持，E373→377只双域四局部I/S。关联率不是完成率；本组14文件，固定快照：`cg2d-re-battle-m-skill-entry-waiting-sampling-20261006`。
