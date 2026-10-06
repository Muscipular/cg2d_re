# 战斗资源变化临时载荷取样 SPEC（BPT01）

2026-10-07；沿后续动作159/160/162补完整本体与共享处理器的实际消费。工程委托有效，原事实以已打开IDA只读会话为准。

<a id="battle-resource-copy-baseline"></a>
## 完整原根与具体差异

| 动作/原根 | 完整原指令 | 实际出向/返回 | 唯一入向 |
|---|---|---|---|
| 159 / 4107C0..410AF5 | 266条821B/四页 | 34call/17普通jump/7ret | 405373 |
| 160 / 410B00..410E27 | 252条807B/四页 | 27call/17普通jump/7ret | 40537E |
| 162 / 410E30..410E81 | 22条81B/一页 | 2call/2普通jump/1ret | 405389 |

合540条1709B/九页、63call/11被调入口、36普通内部jump/15ret，三普通入向同父404DB0，只九条压action/弃返/公共尾邻窗。[完整证据](cg2d_battle_resource_copy_evidence.json)保全部实际指令、十二临时复制点与当前源前后态。

两个资源原根各有六次临时量复制，均先读齐三个载荷字段，再写目标；现共享处理器交替读写，六源片段恢复完整取样。正常旧复制值相同，差异是源读取/发布阶段，不冒称数值、并发或生成机器错误。原6/3的encoded→mask→readCount、9的readCount→encoded→mask、8的mask→encoded→readCount读/写序逐点保，字段宽度分别W32/W32/W8，目标token/reserved不覆盖。

<a id="battle-resource-copy-decision"></a>
## 实施裁决与完整正常合同

动作159/4107C0：flagsFDC MOV W32/TEST AL60清才PlayFrame(action,0)弃返；fresh type非零则先Y/X/amount和type数显(mode2/flags0)，其后fresh type按6扣readiness/最低0、5加readiness/最高maxHp、4加barValue/最高barMaximum、3扣barValue/最低0、0图22459、9先构造值赋readiness再限制0/maxHp、8先构造值赋barValue再限制0/barMaximum，default无资源更新但仍公共推进。各零临时与8/9初始临时先完整取三载荷字段再按原顺序写；token/reserved保，Constructor/Destroy和实际业务上下限保。原七439340点的所有正常路径归现有入口一次439340，不新增helper原身份。

动作160/410B00：与159相同数显/fresh type/七case/default及内存CryptInt业务合同，六临时复制阶段同原顺序；尾是phase=37，包括default，原七独立写点归现有入口一次发布。共享既有ApplyBattleResourceChange_4107C0_410B00正常处理，不把159推进尾混入160或加helper返回门禁。

动作162/410E30：W32 flags0x60清才PlayFrame(action,0)弃返；fresh value1024==2才依次value1024=0→hidden S32=0→field2D8 W32=0→439340(action)，其它只返回。当前入口完整正确，不改字段或新增守卫。

只在既有共享helper的六临时复制处，以两个U32和一个U8局部值先取载荷再写原目标；6/3原CopyPayload展开改局部对应写序，8/9各原初始/零载荷的三直接写用局部取样。一般CopyPayload实现及所有上限比较后的直接逐字段复制保，不全局改帮助体、不新造函数或影子状态。

三个入口及helper签名保持，159继续Advance、160继续phase37，default真实跳往各公共尾；不因七个独立尾点误加“仅已识别类型推进”的门禁。162现有复位顺序保持。原W32加载后测AL不改低W8，内存CryptInt继续原构造/析构及0/最大值业务限制，不把网络明文要求扩大到内存编码取消。

<a id="battle-resource-copy-acceptance"></a>
## 限定验收

- 三完整根540条1709B/九页、63call/11入口、36普通内部jump/15ret与三唯一父九条邻窗，以及两原根各六完整取样/复制阶段可追；shared helper无独立原身份。
- 源整个YobiRuntime只六helper片段，两个资源入口和162全文/其它所有源保持；十二原三字段读取都早于第一目标写，宽度与读写序保，构造/析构及业务上下限不变。
- 新增1435..1437三明确原身份/正常equivalent：语义1435→1438/有效1430→1433、关联1548→1551/3680（42.15%）、BATTLE112→115仍枚举open、分类560→563、E738→740仅两局部I/S。R7 259/3061/余2802、差队列172/待归属43与全部完整门禁保。
- U08/R01/R04/V04只当前/引用仍open，原60任务关闭条件保。三完整原根540条1709B/九页、63实际call/36普通内部jump/15ret、三唯一普通入向同父404DB0的九条邻窗及十二原临时复制阶段，只当前完整正常源合同/读写阶段与接线静态。源码一既有共享helper内六片段恢复全载荷取样，三个入口与其它源保持，正常旧载荷值一致，不声称数值/并发或生成机器bug。全部子依赖/其它生产/完整父与间接alias-owner寿命、SDK/生成机器ABI/时钟RNG/画面运行未新验；内存CryptInt与原业务上下限保，网络仍原格式明文，RPLAN.24不专项异常/保护/模拟或故障等价。

<a id="battle-resource-copy-source-state"></a>
## 当前交付

共享资源处理器六复制阶段已有完整取样，两个原资源体与复位体完整正常源/接线纳管；快照 `cg2d-re-battle-resource-copy-sampling-20261007`。未编译/调试/测试/运行/摘要，完整战斗、生命周期与R8未关闭。
