# 战斗目标移位、HP归零与技能百格生产完整合同 SPEC（BSTL01）

2026-10-10；动作93–98、角度归一化405E60和共享绘制41E890八完整正常根。仅95临时HP零复制、405E60角度两次持样共两片段及十六注释；其它完整正文/签名/数值/调用/表/全局和共享复制器保持。无新函数/API、保护/默认值、循环上限、声/播放/RNG或业务重构。

<a id="battle-skill-tiles-baseline"></a>
## 原根与局部分派

| 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / CALL |
|---|---|---|
| F:0000CF70 / UpdateBattleAction93_40CF70 | 40CF70–40CF9D | 16 / 45B / 2 |
| F:0000D010 / UpdateBattleAction94_40D010 | 40D010–40D0AB | 43 / 155B / 4 |
| F:0000D0B0 / UpdateBattleAction95_40D0B0 | 40D0B0–40D162 | 49 / 178B / 5 |
| F:0000D170 / UpdateBattleAction96_40D170 | 40D170–40D19D | 11 / 45B / 1 |
| F:0000D1A0 / UpdateBattleAction97_40D1A0 | 40D1A0–40D1CD | 11 / 45B / 1 |
| F:0000D1D0 / UpdateBattleAction98_40D1D0 | 40D1D0–40D38E | 128 / 446B / 6 |
| F:0001E890 / DrawBattleTiledEffect_41E890 | 41E890–41E915 | 47 / 133B / 1 |
| F:00005E60 / NormalizeBattleAngle_405E60 | 405E60–405E9B | 18 / 59B / 0 |

共323条1106B/九份最多64条分页、20E8/十一唯一callee、52跳十RET，无间接CALL/JMP。六动作各唯一实际CALL/父404DB0，六case各11B push→CALL→JMP40563F。角度叶四实际CALL/四父；百格绘制七CALL/四父，98保两个独立技能读取门禁，七个原点对应现有七个调用表达式，413400的两个点保原互斥分支。全部十七实际入向/九不同父仅关联，不代其它父/ABI/owner全验收。八当前签名保持，百格返回最后DrawGraphic真实S32结果，其余void弃返。

## 93–97：活动计数、目标移位与HP归零

93固定入口payload，先CountBattleActiveEntries()<=1才updateGateMode=0，然后aliveFF8=0→Advance。不能先清alive再计数。BHRC01完整22槽计数与BEV01推进复用，不重计依赖/改变Count门禁。

94每次从固定payload当前C9C/position/全局表查目标：X先发布，再fresh目标Y发布，再fresh目标角度减原F32 180/F32发布；Normalize单圈→value54=50F→Move，之后第四次fresh当前目标TurnToward(source,target,1,0F)→Advance。不能缓存同一target或先读两坐标再一起写，数值与BOG01移动/转向保持。

95入口固定payload，fresh FF0==0才characterAction3/PlayFrame(0)；随后value54=6F，再fresh旧FF0、先发布旧+1，用旧值<16才Move。旧>=16则先Advance source→仍从入口payload清FF0→Construct临时CryptInt零。原40D114取当前C9C、40D11A读临时encoded，然后位置/全局target查找；encoded发布→fresh mask读写→fresh BYTE readCount读写→Destroy。当前共享CopyPayload写序正确但encoded读在目标查找之后，本步仅展开这一调用段；保token/未复制字节、零归正常业务行为与原两条互斥Move/Advance路径，不加负HP门禁/新播放/保护，也不改共享复制器或其它caller。

96/97各fresh当前effect位置/全局target，仅target.hidden整字段置0/1→Advance source；不推进target、不加目标/位置保护或播放。正文正确保持。

## 405E60：两个角度样本与一次归一化

原首angle50 F32取值用于360比较；原405E6D在读取状态位/分支前再次加载angle50。首样正常>=360，则用第二样减360/F32发布后直接返回；否则用第二样比较0，正常负值才fresh第三次angle50加360/F32发布。当前源码把第二读取放在正分支、负比较另读；本步明确upperAngle/currentAngle先后持样，正分支用currentAngle、负分支保fresh第三读。不是循环/fmod、不更新direction，也不新增钳制或数学异常模拟。原0/180/360三个F32共12B位模式实读，当前float字段/double中间正常数值保持。

## 98：双图号门禁与两次技能读取

入口先持frameEvent再固定payload，characterAction6；入口event>=10000（没有20000上界）才fresh level1044<12选110074否则111056，再fresh C9C/目标NewRandom(target,image,0)，无论创建成功都清event。然后PlayFrame(1)严格返回1，再独立fresh level/当前目标创建相同两档图，随后Advance source；两个门禁并不互斥，不能共用入口level/目标或删一次创建。

后续fresh battleCastEffect10非空才取固定payload当前skill103C。类型0对应19/23/27/2068–2070；1对应20/24/28/2071–2073；2对应21/25/29/2074–2076；3对应22/26/30/2077–2079；4对应31/3037；其它不调用第一处百格绘制。第一处调用后重新读取skill103C，86/2055/2056/2057才调用类型5；不能将第二次skill改为入口缓存或添加空图/默认调用。现switch与两个独立读取、创建失败后的清event/推进正确保持。

## 41E890：共享相位与百格提交

借唯一59F9EC相位/59F9F0方向。reverse0则fresh phase+1先发布，新值>=15再reverse=1；其它reverse则phase−1先发布，新值<=0再reverse=0。先推进后绘制，无hold端点帧、额外重置/越界保护或按技能另造相位。

type*16在循环前持样；X外层0..576步64，Y内层0..432步48，共100个正常DrawGraphic点，坐标X+32/Y+24、priority3、图号来自553A74[type*16+每格fresh phase]、第五参数0/第六NULL。每次提交前重新读取共享phase，不能缓存整帧/交换XY循环、减少格数或失败早停；保最后提交的S32返回。原一个静态CALL正常消费100次，不把静态次数误记动态格数。

553A74–553BF4完整384B/6×16 S32表、96值与当前constexpr核对；其它表消费者保持，不重计新X。两个原全局在IDA中无加载值/文件映射，读取接口返回FFFFFFFF不能当运行-1或原初值；当前零初始化两定义保持。实际初态/外层生产与全局寿命仍待，不用BSS展示值/现定义冒充运行验收。

<a id="battle-skill-tiles-decision"></a>
## 实施与登记

按第8条工程委托先SPEC后实施：一个CPP两片段/八定义注释、公头八注释；其它六正文、表/全局/共享CopyPayload/全部旧源与旧语义保持。八原身份此前无独立语义/F处置/已知BATTLE成员，补八adapted记录/处置/成员，枚举false。限定I只95与405E60两F/两片段，S八完整正常源/384B表/12B常量/实际局部分派及依赖，不计新全实现。

U08/U09/R01/R04/V04补局部来源且open，全部原分母/门禁/政策/六closure保持。MSVC CRT/WAV-only/原格式明文/RPLAN.24保持，排序仍待此前用户选择；原六项用户改动及额外CRT文档压缩保留、不夹带。不编译/测试/运行/算法/产物/摘要，不引依赖、不降低总SPEC§10。

<a id="battle-skill-tiles-acceptance"></a>
## 必要静态与剩余

核八完整323条1106B/九页/库存，20E8/十一callee/52跳十RET，十七真实入向/九父与六11Bcase；93 Count先alive后、94四目标重取/X先Y/180-50/Move后转向、95旧16及HP零临时读取顺序、96/97hidden目标与source推进。核405E60原三次读取/两个样本、一次加减360及12B常量，98两独立level/目标/创建与事件/终帧门禁、26+4技能映射/两次skill，百格两方向/先相位发布/每格fresh相位/XY次序/真实六参/末返回及384B全96值。

核整CPP仅两片段/八注释、公头八注释，其它全部正文/共享源/表/全局/旧记录保持；八新语义/F/成员与限定I两F/S八F、五open任务、原政策门禁/UTF-8无BOM/LF/单末尾LF、差异与文本汇总/-Check。完整效果/HP/技能/phase输入生产、全部父-callee/ABI/alias/owner、SDK正常失败/容量/数值/RNG/资源与声音画面运行待，总SPEC§10保持。

<a id="battle-skill-tiles-source-state"></a>
## 当前状态

八完整原根/表/常量/case已取齐，95临时HP零复制与405E60角度持样两片段/十六注释落实；必要静态、文本汇总/-Check及精确派生范围通过。语义2152（2104F＋48X）、合同2214/3680=60.16%（仅关联率）、F1580、E1397仅限定I两F/S八F、BATTLE392且枚举false；五任务open、旧记录/原分母/全部政策门禁保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-skill-tiles-history-20261010，提交引用cg2d-re-battle-skill-tiles-20261010。整体SPEC未完成。
