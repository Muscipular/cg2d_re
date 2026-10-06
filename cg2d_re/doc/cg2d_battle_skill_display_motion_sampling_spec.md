# 战斗技能显示与移动参数取样SPEC（BQF01）

日期：2026-10-07。按工程委托先记裁决再实施；聚焦243技能图/显示/退场与246..248移动参数未纳管基线。

<a id="battle-skill-display-motion-sampling-baseline"></a>
## 原端完整基线与具体差异

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret | 源调整 |
| --- | --- | --- | --- | --- |
| 0x00417D80 | UpdateBattleAction243_417D80 | 558/1986/7 | 35/96/1 | 3阶段 |
| 0x004185C0 | UpdateBattleAction246_4185C0 | 15/47/1 | 2/0/1 | 0阶段 |
| 0x004185F0 | UpdateBattleAction247_4185F0 | 16/43/1 | 3/0/1 | 0阶段 |
| 0x00418620 | UpdateBattleAction248_418620 | 44/131/1 | 5/1/1 | 0阶段 |

四完整根633条2207B/十页、45原call/22callee、97普通内部jump/四ret；四唯一404DB0入向12邻窗合32B。无物理switch表，243技能选择是CMP链，源switch承接语义，不造R7表分类。246系数原54D4E4 F32字节0ad7233c，源bit_cast<float>(0x3C23D70A)已正确，不换成double 0.01。

| 243读取阶段 | 原实际低W8 | 裁决 |
| --- | --- | --- |
| 首次转向 | 417D98 | 同initial record.flags bit8先，position后 |
| 恢复/其它属性图标 | 41836A、418424 | 两原路径，一共用源表达式，各在本路径推进/显示调用后fresh |
| 时长条件 | 41845E | 同tailIndex.flags bit8先、position3后 |

三W8源表达式承接四实际原点，不重复计共用属性表达式；初始/命中/主显示/数显/最后回收flags W32缓存和寄存器AL/AH/CL/CH保持。技能选择/RNG、正确显示/回收/移动和普通上限CopyPayload保持。

<a id="battle-skill-display-motion-sampling-decision"></a>
## 工程裁决与完整正常合同

依既有委托决定：只243三个读取表达式，不改共享结构/头/其它正文。保原CMP选择/六RAND物理点的每路径消费、不从238/239照搬扣HP或显示门禁；原其它分支图22459后仍Attribute，恢复10000000分支则跳共同尾。246..248正确体保持，正常S32/F32转换不新__ftol身份或异常模型。正常旧值一致，不宣称并发/运行数值错。

1. 243入口/技能图：fixed payload，fresh initial record.flags低W8 bit8清才同index.position/共享目标Turn(source,target,0,0)。另fresh current.position/共享目标Distance<20且fresh source.frameEvent0才处理。fresh current同index.position→initialFlags W32→共享target T*→targetPayload T*，image/second=-1/mode0。cached initial bit4图110000，800图110099，400为FindTechnique(8,position)<12图110098否则111048；其余skill W32按实际CMP链选择（原无物理switch表，源switch仅承接语义）。0/2007/2008/2021 level<12主110133否则111029；1为110134/111047，3为110135/111046，5为110139/111052，6为110137/111050；75..80、2096..2101、110主110074；81/84/85/87主110174/110175/110159/110160。257 second110476，level>=12 second111051且主仍-1；否则>=9才主110014。258只second110477；259一次RAND(0,2)+110001主且second110470；260 level<12主110473否则111054；262 level<12主110473/mode1，否则111049/mode0。95/3019/4002/3038原各独立RAND点，及其它default同RAND(0,2)+110001主；源default合并等价分支但每实际路径只一次RNG，六原RAND物理点不虚计六源表达式/新身份。所有图值/级别/模式和返回消费正确体保。 243命中/显示：Turn(cached target,source,1,0)后fresh ownPosition/共享attacker写target+10，fresh current.flags W32写targetPayload.FE4→fresh ownPosition ApplyFlags(position,source)。fresh level>=9且fresh skill259才NewTiled(0,15)。fresh hitIndex/flags W32，bit8 phase31；100先target.flash1→声210(x)→phase31，无新增flash旧值条件；其它hitFlags7E000清且sameindex.amount!=0，bit2 target110008，primary>=0按imageMode生成target主图、second>=0独立target次图/0；Rebirth(fresh共享ownPosition attacker,target)→声255(x)。该原分支没有扣HP，不从相似共体加Subtract/归零。 243恢复/属性/时长：fresh current.flags W32，48000只439340(target)；24000先439340(target)，fresh healIndex.flags W32 10000000当前共享目标图22459后跳共同尾，不显示Attribute；否则same healIndex.amount先position/共享目标后AddHP，fresh shared target GreaterHP/max超原上限才另fresh目标普通CopyPayload encoded/mask/count交替。数显fresh display.flags W32→targetY→same display.amount→targetX/type5/style2。12000只439340(target)。其它100000原回收target.effect1054，调用后targetFDC先读、槽null后FDC&~20；439340(target)后fresh displayIndex.flags W32 10000000当前图22459，随后仍走属性图/Attribute（与238/239不同）；即使无10000000也走Attribute，没有新的伤害数显。恢复数显/其它分支原41836A/418424各fresh current.flags低W8 bit2才cached target22454，Attribute(fresh共享ownPosition attacker,target)；源共用一W8表达式承接两原路径，不重复源片段。共同fresh tailIndex.flags低W8 bit8清且sameindex.amount非0才按cached targetPayload当前FDC W32 bit1求14*(bit+1)；取fresh skill先，FD0=duration后skill267/3029才覆盖2，frameEvent1。初始/命中/主显示/恢复/最后回收flags W32缓存及寄存器AL/AH/CL/CH保持。 243计时/退场：fresh remainingFD0>0减1，fresh source.imageNo只有110016/111055才PlayFrame0弃返；随后独立fresh FD0==0再PlayFrame0→Move，允许归零当帧按原图两次播放。fresh frameEvent1才fresh current.flags W32，两寄存器bit8/100均清MarkDeath(source)，phase17；其它事件仅返回，没有238/239的再转向分支。保原唯一ret、接收者/缓存别名/真实失败/原上限/调用返回/RNG，无新动画/迭代或异常限制。

2. 246：fixed payload/fresh currentIndex，same record.position3 S32先，FILD正常整数乘原54D4E4 F32字节0ad7233c（0.009999999776482582），F32发布source.value54→439470→439340。现bit_cast<float>(0x3C23D70A)正确，不把系数换成double 0.01，不改正确体/实际x87未验。

3. 247：source.offset40 F32先取，再fixed payload；原__ftol正常S32边界承接x→payload.x10，之后fresh source.offset44→S32→payload.y14，再439340。源int32转换/取样和正确体保，不新原__ftol身份或异常/回绕/机器ABI等价。

4. 248：fixed payload，characterAction1/speed8/frameRate100；payload.y14 S32转F32 Y先、x10 S32转F32 X后，Turn(source,X,Y,1,0)→Move。调用后fresh y14转F32、x10转F32，Distance(source,X,Y)<5才fresh x10→offset40 F32、fresh y14→offset44 F32→439340，所有路径PlayFrame(source,0)返回。保Y/X读序、独立前后参数、S32/F32存储、阈值与返回，正确体不改。

四完整原根633条2207B/十页、45call/22入口/97普通内部jump/四ret，四唯一404DB0入向12邻窗/32B与一真实F32系数。只243三W8源表达式承接四真实原点：首次转向、属性共用两路径、时长；正常旧值一致，不称并发/数值或机器故障。原技能CMP链/级别/图值/模式与六RAND物理点/各路径消费、W32寄存器旗位、无扣HP事实、恢复数显/其它图标后仍Attribute区别、上限普通CopyPayload、两条件播放/退场及其它三正确移动体/正常转换/返回保持。原无物理switch表，不新R7或原__ftol身份。只当前完整正常源接线静态，不外推递归父/全部子依赖/其它生产与owner寿命、SDK/生成机器ABI/实际x87/画面运行；音频暂停，RPLAN.24不专项异常/保护/回绕/故障等价保。

<a id="battle-skill-display-motion-sampling-acceptance"></a>
## 可度量的限定验收

- 四根633条/2207B、十页、45call/22callee、97jump/四ret，四父12邻窗/32B、一原F32系数与三源/四原W8可追。
- 整YobiRuntime仅243三读取片段；原技能图值/级别/模式/RNG、W32缓存、无扣HP、恢复数显和其它图标后仍属性的差别、普通上限copy/两条件播放/退场、三正确移动体及参数/转换/返回保持。
- 新增1492..1495四原已有正常定义/equivalent，四冻结定位全文保持。语义1492→1496/有效1487→1491、关联1605→1609/3680（43.72%）、BATTLE169→173枚举open、分类617→621/E762→764。仅两局部I/S，每项十二独立实际来源。
- R7 260/3061/余2801、差队列172/待归属43及其它全部机器对象/完整门禁保持；U08/R01/R04/V04只当前描述/引用仍open，原60任务关闭条件保。完整战斗/生产与owner寿命/生成机器/运行及R8仍open。

<a id="battle-skill-display-motion-sampling-source-state"></a>
## 当前交付

243三W8读取阶段实修，243/246/247/248四完整正常基线纳管；快照 `cg2d-re-battle-skill-display-motion-sampling-20261007`。定向静态及measure/-Check记录唯一进度；未编译/调试/测试/运行/摘要，音频暂停、网络原格式明文及原验收标准保持。
