# 战斗连续动作、多效果执行与独立复制体生产完整合同 SPEC（BCPY01）

2026-10-10；动作99–103及创建4205F0六完整正常根。18个BYTE消费表达式（原19点，两数字支路合用一表达式）、100入口帧索引/102临时HP零复制/103效果四值三读取片段、100共享游标PE静态初值共22组；另十三注释。其它正文/签名/数值/调用/RNG、完整W32发布/高位、正确最大HP Copy、共享复制器/追加器与原表保持，无新函数/API/保护/默认值/循环上限或业务重构。

<a id="battle-combo-copy-baseline"></a>
## 完整原根与生产来源

| 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / CALL |
|---|---|---|
| F:0000D390 / UpdateBattleAction99_40D390 | 40D390–40D3B8 | 11 / 40B / 1 |
| F:0000D3C0 / UpdateBattleAction100_40D3C0 | 40D3C0–40D577 | 137 / 439B / 10 |
| F:0000D580 / UpdateBattleAction101_40D580 | 40D580–40D6B4 | 85 / 308B / 8 |
| F:0000D6C0 / UpdateBattleAction102_40D6C0 | 40D6C0–40DBD5 | 365 / 1301B / 29 |
| F:0000DBE0 / UpdateBattleAction103_40DBE0 | 40DBE0–40DC8A | 70 / 170B / 12 |
| F:000205F0 / NewBattleCastCopy_4205F0 | 4205F0–4206B8 | 54 / 200B / 4 |

共722条2458B/十五份最多64条分页、64E8/二十八唯一callee、61跳九RET，无间接CALL/JMP。五动作各唯一实际CALL/父404DB0及各11B push→CALL→JMP40563F；复制体四真实CALL/三父为40DBE0、40E4B0、40EB00（后者两点），合九入向/四不同父。callback404DB0由42060C字段+8 MOV地址生产，不混计CALL。99保0/S32返回，101末PlayFrame S32，其它void/创建真实非拥有T*。

## 99/100：共享S8动作序列与入口帧样本

99先rate25→Advance→frameIndex0→frameRemain0→return0；没有event清零/播放。100固定入口payload，先currentIndex/position，再frameIndex取样，然后全局target/targetPayload取值；只有该保存frameIndex0才fresh frameRemain0，按fresh level1044*20+共享游标取S8写characterAction，再重新读共享游标+1发布。当前入口帧读取原在目标查找之后，本步恢复position-frameIndex-target顺序，不缓存frameRemain或把target提前。

PlayFrame(1)严格非1直接返；正常1才fresh当前完整flags写固定targetPayload.FE4，再独立重取完整flags。保存样本的AH按20h→80h→40h优先调用原效果工厂类型7→9→8，现三flags高BYTE消费明确，完整FE4发布不截短。普通路径先target.phase26，再fresh currentIndex/flags，CL4图110000；否则CH4才FindTechnique(8,该index position)，<12图110098否则111048；其它110133。然后固定target随机效果、NewRebirth(source,target)，创建失败仍继续。低/高BYTE取自同一保存W32样本，不换成多次fresh内存。

共同尾先持源屏幕X→RAND(131,132)→WAV声(随机号,heldX)，现二参不消费原额外Y，不恢复MIDI；再frameIndex0→frameRemain0→frameEvent0，重新读level/共享游标取下一S8，严格负数才游标0→Advance source。无追加扣HP、RNG/播放或提前缓存结束哨兵。

## 共享游标：撤销误把IDA未加载FF当初值

当前IDA数据库59F9DC无加载值/无文件偏移，get_bytes返回FFFFFFFF不能证明原初值-1。只读数据库保存的40B .data节头：RVA153000h、VirtualSizeB34B08h、SizeOfRawData40000h、原映像base400000h；原文件承载到593000h，虚拟节尾1087B08h。59F9DC的完整4B位于593000h–1087B08h尾部，按[Microsoft PE节表的零填充规则](https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#section-table-section-headers)确定**映像静态初值0**。

据此把当前全局-1改0，并纠正表注释/旧shared_battle的初值说法；不改为每对象游标、不新加复位/保护/域限制，不把静态初值当运行值或证明间接批量写者不存在。原五个直接xref均100，完整运行初始化/技能level和索引生产域仍待。原错误前态保于Git命名快照，PE节头来自当前打开库，未重开/读取原文件、修改/保存IDA或运行客户端。

5538F4–5539BC完整200B/10×20 S8，首两行分别原九动作后-1、十个10后-1，其余160B零；与当前40显式值/其余值初始化零逐值核对。负值只作原哨兵，不添行内下标、默认动作或补零。

## 101：三次位置门禁与中心汇聚

固定payload，characterAction3/value54=8F/rate100，保存当前position只给首次TurnPosition：<10向356/276，否则278/278。调用后fresh当前position>=10才测距离278/278<=8；未抵达或门禁不符再fresh position<=9才测356/276<=8。抵达各先X后Y发布目标中心，面对98/128或542/407→Advance source→characterAction9→入口payload.FF0=0→PlayFrame(0)S32返回。未抵达Move→同Play返回。不能缓存后两位置或把三次门禁合并，数值与现几何依赖正确源保持。

## 102：完整逐效果伤害、回复与延迟生产

固定入口payload，characterAction5/value54=32F/rate25，fresh旧FF0先+1发布，旧<8才Move→PlayFrame(1)弃返后早返；不按新计数判断。其余至少一轮do，每轮固定current record的position/完整initialFlags及target/targetPayload；初始AL4、AH4选图与100普通路径相同。fresh sourcePosition18给目标TurnPosition：>9向356/276，否则278/278；方向后fresh完整effect.flags发布固定FE4，再ApplyFlags(fresh sourcePosition,source)。

之后持当前hitIndex/完整hitFlags：CL8只phase31；否则CH1才flash1004=1→声210/fresh sourceX→phase31；其它CH E0全清且该保存index.position3非0才普通伤害。保存CL2先尝试图110008，image>=0再选图创建，Rebirth→声134/fresh X→fresh当前effect/amount/position查动态目标扣HP，再fresh当前目标Less(HP,0)。负值构造零后原40D8BD取C9C、40D8C3取temp.encoded，再动态目标查找；encoded发布→fresh mask读取/发布→fresh BYTE count读取/发布→Destroy，保token/其它字节。现最大HP Copy及共享复制器均保持，仅这一临时Copy展开。原普通伤害没有10000000h免疫门禁，不套其它阶段逻辑。

之后fresh原BYTE20h才固定targetPayload完整FDC OR2→gateMode1→aliveFF8=1。再fresh保存完整flags，AH80类型先Advance target；否则AH40回复先Advance，再fresh当前index完整10000000h测试：有则fresh动态目标图22459，跳过加血；无则fresh amount/动态目标Add HP→fresh compareTarget Greater(HP,maxHP)→命中后再次fresh copyTarget，原正确encoded-mask-count Copy(maxHP)保持。回复数字先currentFlags、amount、固定targetY-80、固定targetX，Number(type5,2,fullFlags)。

AH20则Advance；普通先保存完整100000h才无父NULL门禁MarkDeath(effect1054)，fresh完整FDC先持清20h结果，再1054=null→FDC发布→Advance target。调用后fresh index/amount；amount0才fresh flags+3 BYTE10h，无论当前amount值不提前读取该高BYTE。该门禁命中动态目标图22459，否则fresh完整displayFlags→固定targetY-80→X，Number(type6,2,fullFlags)。两数字支路共用的后续fresh BYTE2对应原40DA5A/40DB20两点，命中固定target图22454，然后Attribute(source,target)；免疫图/类型80或20路径不新增属性调用。

每轮共同末fresh tailIndex，先原BYTE8全清、再该index.position3非0才取固定targetPayload完整FDC，按bit1产生FD0=14*(bit+1)，否则保旧FD0。AdvanceEffect(source)严格1才结束；末value54=8F→rate100→入口payload.FF0=0→Advance source，不额外Play/清event/循环cap。保全部固定视觉与动态HP目标、正常恢复/数值/失败/调用/RNG边界；18 BYTE表达式中102为13个（原14点），高位W32测试/数字完整flags不缩窄。

## 103与4205F0：真实复制体生产

103固定入口payload及caster1070，至少一轮do：每轮NewCastCopy(source)返回沿原callee边界使用，不添加父NULL失败绕行。对保存caster/新copy严格追加25、77、91、9、92、14、107七动作，再fresh当前effect。原按position4→flags→position3→position读齐（40DC4E/51/55/59），本步显式四样本，再构造既有按值record传AppendEffect，不改追加器/签名/字段发布。Advance(copy)→AdvanceEffect(source)，严格1结束后Advance(source)，不能把四值提前七次追加或复用上一轮记录/复制体。

创建器请求NewYobi(2,原载荷13E8h对应当前sizeof原生BattleActorPayload)，失败真实NULL返；成功callback404DB0→固定新payload→复制parent.displayPriority BYTE、imageNo W32，先持新对象完整flags OR20000h，再parent T*发布+10→flags发布→value54=0。随后parent angle→direction→offsetX→offsetY逐字段读取发布，field270 W16=0/phase0。新载荷只skill103C、attackModeFF4、position18三项，各次重新读取parent.payload；不是整载荷memcpy/浅克隆，也不新增技能level或队列复制。

PlayFrame(0)后正常S32转换新offsetX，再读取新offsetY保F32样本，X发布后才转换/发布Y，最后真实T*返回；两原__ftol沿现受限宿主边界，不模拟异常/溢出。新独立载荷、借用parent与最终scheduler物理释放/重进退出仍待，创建/标死/推进不代完整owner。

<a id="battle-combo-copy-decision"></a>
## 实施与登记边界

依第8条先SPEC后落实：一个CPP22组/七注释（六定义及一表），公头六注释；旧shared_battle仅纠正99/100初值/本轮状态与补当前来源。完整W32/真正高位、正确最大HP Copy、共享复制/追加、其它源/表/旧记录保持。六身份此前无独立语义/F处置/已知BATTLE成员，补六adapted记录/处置/成员，枚举false；一限定I仅100/102/103和100当前游标静态初值，一S六正常根/200B表/PE节头/实际依赖，不计新全实现。复用二十一既有语义，不重复登记新原函数/数据X。

U08/U09/R01/R04/V04补局部来源且open；所有原分母/政策/gates/六closure保持。MSVC CRT/WAV-only/原格式明文/RPLAN.24、七外部改动保留和排序此前待选保持；不编译/测试/运行/算法/产物/摘要，不引依赖，不降总SPEC§10。

<a id="battle-combo-copy-acceptance"></a>
## 必要静态与剩余

核六完整722条2458B/十五页/库存、64E8/二十八callee/61跳九RET、九实际入向/四父/五11Bcase与callback地址MOV；200B全S8和PE节头/原未加载状态/游标完整4B位于零尾部，当前游标0仅静态初值。核18 BYTE表达式/原19点、100入口帧样本、102临时HP零/回复上限及固定/动态目标、103四值读序/七动作序、创建器独立载荷/三父payload读取/末Y先X发布和正常T*；101三门禁/坐标/抵达先推进。

核整CPP仅22组/七注释、公头六注释、共享器/表/其它源/旧记录保持；六新语义/F/成员、限定I三F/S六F、五open任务、原政策/gates/UTF8无BOM/LF/单末LF、差异与文本汇总/-Check。完整level/序列/效果/HP/复制体生产、全部父-callee/ABI/alias/owner、SDK正常失败/容量/数值/RNG/资源与声音画面运行仍待，总SPEC§10保持。

<a id="battle-combo-copy-source-state"></a>
## 当前状态

六完整原根/200B表/case与原PE节头已取齐，18BYTE表达式/三读取片段及共享游标静态零初值共22组、十三注释落实；必要静态、文本汇总/-Check及精确派生范围通过。语义2158（2110F＋48X）、合同2220/3680=60.33%（仅关联率）、F1586、E1399限定I三F/S六F、BATTLE398且枚举false；五任务open、全部旧记录/原分母/政策/gates保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-combo-copy-history-20261010，提交引用cg2d-re-battle-combo-copy-20261010。整体SPEC未完成。
