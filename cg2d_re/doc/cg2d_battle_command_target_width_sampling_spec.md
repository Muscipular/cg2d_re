# BQI01 战斗命令目标读取宽度SPEC

2026-10-07。沿[战斗尾段取样](cg2d_battle_tail_sampling_spec.md)和当前验收体系，依据已开IDA会话[单根完整证据](cg2d_battle_command_target_width_sampling_evidence.json)推进缺口。先记SPEC和用户工程委托裁决，再修改；原端事实按实际指令，不猜。

<a id="battle-command-target-width-sampling-baseline"></a>
## 原端基线与差异

| 原入口 | 新符号 | 指令/实际B/页 | call/普通jump/ret | 源交付 |
| --- | --- | --- | --- | --- |
| 0x0043C560 | CheckBattleCommand_43C560 | 266/901/4 | 7/63/3 | 四片段/六低W8读取表达式 |

完整266条901B/四页、7call/2callee、63普通内部跳转/三ret；六UI父八入向，八个实际前3指令/call/后1指令邻窗共40条141B。完整参数生产/父函数未外推。原20项矩阵是20个栈写初始化（18立即数及EBP8/EDX0），没有物理switch表，不新增R7。

| 差异组 | 原端实际依据 | 裁决 |
| --- | --- | --- |
| 同阵营800筛选FDC | 43C640/43C69B TEST BYTE +FDC,4 | 同源表达式低W8读取，承接两个阵营物理路径 |
| 400玩家位80 | 43C7E0 TEST BYTE 5ABCAC,80 | 保flags400短路，实际低W8读取，不改全局W32存储 |
| row0前排选中 | 43C849/43C85D TEST BYTE +24,8 | 两低W8只读条件；43C870清标记仍W32 |
| row3前排选中 | 43C8A8/43C8BC TEST BYTE +24,8 | 两低W8只读条件；43C8CF清标记仍W32 |

四源码片段内六低W8表达式对应七实际内存点。正常旧值一致，不宣称数值/并发或机器故障；参数flags W32及原BL/BH寄存器测试、四源OR/两源AND选中标记W32写均保持。

<a id="battle-command-target-width-sampling-decision"></a>
## 完整正常源合同与裁决

CheckBattleCommand_43C560：position S32/flags W32，先发布gBattleTargetFlags5ABC44，再GetPet(position)，其返回缓存pet；flags bit1清才返回0，不能先拒绝而跳过发布/GetPet。bit4宠物槽、bit8本人槽分别共享T*非空才LessEqual(HP,0)，先实际比较/RNG消费，再取cached flags bit2决定HP<=0是否也选；通过后重新从共享槽取目标，W32 flags|8，原pet两物理OR路径由源一表达式承接。flags810设才同阵营十槽，position<10起0否则10，每槽先排除position/pet，再共享T*非空；cached flags10直接准入，否则flags800设才固定payload FDC低W8 bit4。两阵营原43C640/43C69B一共享低W8表达式承接，不宽化内存加载；准入后LessEqual HP0、cached flags2分支及共享目标重取W32 OR。flags20另遍历相反十槽，无本人/宠物排除和额外FDC门禁，LessEqual先于flags2，共享槽重取后W32 OR。六原LessEqual物理点分别pet/本人/同阵营0侧/10侧/对侧10/0，源四表达式每实际槽原消费保，无新HP读取/省略比较。
原临时rows矩阵20项S32：14,12,10,11,13；19,17,15,16,18；9,7,5,6,8；4,2,0,1,3。原20个栈写中值8来自EBP（43C5BF/43C5D4），值0来自43C73F EDX清零，余18个W32立即数；源现只读常量rows不向外传地址，正常语义/字段宽度正确体保，不承诺生成机器栈布局，不虚造数据switch/R7项。flags400清直接返回1而不读取玩家位；400设才gBattlePlayerFlags5ABCAC实际低W8 bit80测试，清且position==当前位置也返回1，否则扫描矩阵4×5，row/column初始都为position，匹配覆盖且继续全部20项，保持原不匹配初值、不新增拒绝/保护。
row0取rows1[column]前排索引，五列每轮重新从共享表取ownFront，非空时actual低W8 flags bit8；通过才取rows2[c]敌前排同低W8，非空/bit8通过才rows3[c]敌后排非空后W32 flags&~8。row3对称自身rows2[column]、敌前rows1[c]、敌后rows0[c]。四源前排读取对应43C849/43C85D/43C8A8/43C8BC，清除43C870/43C8CF是AND [EAX+24],EDI W32，不能一起窄化写。每列独立重取自身前排保别名影响，不把ownFront提前到循环外；row1/2不清后排，其余所有路径返回1。原有三ret，其中bit1拒绝0和row0/其它正常1出口，源两个return1表达式保实际结果；当前八UI消费/弃返接线只局部对应，不证明完整生产/生命周期/画面运行。

单完整命令选择原根266条901B/四页、7call/2callee、63普通内部jump/三ret，六UI父八入向、八实际五指令邻窗40条141B；局部父接线不代全部目标参数生产/完整父验收。四源片段恢复六低W8读取表达式对七原点（同阵营FDC一表达式承接两个物理路径）；正常旧值一致。flags参数/目标全局W32、四个源选中OR与两个清除AND的W32写、原HP先比较再bit2/调用后共享目标重取、两阵营各十槽与排除本人/宠物、20项S32矩阵、400玩家位短路/最后匹配覆盖、两前排每列fresh目标及原0/1返回保持。原矩阵由20个栈写初始化，不是物理跳转表，不新R7。完整目标其它生产/owner寿命/递归UI父、SDK/生成机器ABI/画面运行未验；音频暂停、网络原格式明文、RPLAN.24不专项异常/保护/回绕/故障等价及原验收标准保持。

<a id="battle-command-target-width-sampling-acceptance"></a>
## 可度量的限定验收

- 单根266条/901B、四页、7call/2callee/63jump/三ret、六UI父八入向/八窗40条141B及20项原矩阵发布有实际依据。
- 整YobiRuntime只此根四片段，六低W8源表达式对七原点；发布flags/GetPet/bit1拒绝顺序、六HP比较消费点/四源表达式、相邻/对侧十槽及排除、矩阵20项/400短路、每列fresh别名与原W32选中/清除写保持。
- 新增1509单个原已有正常定义/equivalent，原冻结定位全文保。语义1509→1510/有效1504→1505、合同关联1622→1623/3680（44.10%）、BATTLE186→187枚举open、分类634→635/E768→770；两局部I/S各十七个实际来源。
- R7 260/3061/余2801、差队列172/待归属43和其它全部机器对象/完整门禁保。U08/R01/R04/V04只当前描述/引用仍open，60任务源码closed10/完整验收0/叶0/global_accepted=false保持；完整战斗及R8不以本根静态替代。

<a id="battle-command-target-width-sampling-source-state"></a>
## 当前交付

四取样片段实修、单完整命令目标选择正常基线纳管；命名快照 `cg2d-re-battle-command-target-width-sampling-20261007`。定向静态及measure/-Check结果记唯一进度；未编译/调试/测试/运行/计算摘要，音频暂停、原格式明文与原适用验收标准保持。
