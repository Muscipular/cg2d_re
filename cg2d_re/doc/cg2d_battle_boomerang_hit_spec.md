# 战斗阶段22回旋命中与五档选图 SPEC（B22H01）

2026-10-10；继续追加效果查找的真实消费端，覆盖回旋队列命中整根和2022技能所用分档依赖。保留阶段22独立伤害/恢复/显示及延迟合同，不外推阶段20或其它动作的分支。

<a id="battle-boomerang-hit-baseline"></a>
## 完整原根与入口

| 原身份 / 当前源 | 完整根 | 本次实际引用 |
|---|---|---|
| F:000087C0 / UpdateBattleAction22_4087C0 | 4087C0–4090E9，645条2345B/十一页 | 唯一404EAE实际CALL，404DB0 case22 |
| F:0003BF00 / GetBattleValueBand_43BF00 | 43BF00–43BF2E，17条46B/一页 | 16实际CALL/三父；本根408BCF为一个具体消费 |

合662条2391B/十二页。主根42直接CALL/28目标、99跳一RET；分档零CALL/五跳一RET；没有间接调用或switch表。case22三条11B为push对象→CALL→JMP40563F；公共尾原EAX消费前覆盖沿BTS01/BPEX01原证据复用，当前S32接口与源码实际丢弃返回保持。16分档引用只定位，不据此验全部三父/机器ABI。

主根先读取frameEvent S32，再固定入口payload；入口事件非零直接计时尾。事件为0且field1D0为0才读取当前索引/position/共享对象，GetAngleDifference正常截S32发布field1D0。初距严格<60才以原54D464 F32 0.3（9A99993E）加速/发布F32并转向一次；之后无条件第二次转向，顺序为fresh currentIndex→读取已发布acceleration48→position→共享目标。Normalize后读取battleOpponent10，目标phase非15才Turn(other,source,1,0)。之后重新读frameEvent，仍为0且重新取目标距离<20才命中，不补空对象/索引或距离保护。

命中缓存target及targetPayload，发布acceleration48=10F（原41200000），然后重新取imageIndex/flags W32。其它对象表仍为59E980的真实非拥有T*，原非地址字段保S32/W32/BYTE；固定payload与独立currentIndex/共享对象取样不合并成一份快照。

## 主图、第二图及方向

初始main/secondary=-1、direction=0。imageFlags W32 bit4直接main110000，bit400调用FindTechnique(8,同imageIndex.position)，S32结果<12为110098、否则111048；剩余读取castSkill103C S32，沿以下已证关系选图：

| 技能 | 主图/第二图/方向 |
|---|---|
| 0、2007、2008、2021 | level<12主110133，否则111029 |
| 1 / 3 / 5 / 6 | level<12分别110134/110135/110139/110137，否则111047/111046/111052/111050 |
| 75–80、2096–2101、3019、3038 | 主110074 |
| 81 / 84 / 85 / 87 | 主110174/110175/110159/110160 |
| 257 | 第二图110476；level>=12第二图111051，9..11主110014，其余主-1 |
| 258 | 第二图110477，主-1 |
| 259 | 一次RAND(0,2)+110001为主，第二图110470 |
| 260 | level<12主110473，否则111054 |
| 262 | level<12主110473且方向1，否则主111049、方向0 |
| 266 | 第二图110476；level>=12第二图111028，9..11主110014，其余主-1 |
| 2022 | 43BF00(level+1)档1..5读取五图111136..111140 |
| 其余 | 一次RAND(0,2)+110001为主 |

原两RAND点在259/默认互斥路径，每路径一次；不按物理点数增加消费，不改技能比较、等级取样或给主-1补图。分档读取五S32阈值54D93C={1,4,7,10,13}；入参1..15按阈值顺序计数返回1..5，范围外或结果<1返回-1。43BF1D把54D950只当阈值表排他末址，不合并其后独立表。2022原54D438+档*4在现54D43C五项数组用档-1，完整20B图号匹配；不额外校验/钳制分档返回或执行无效输入样例。分档正文/原五阈值和图表保持。

## 应用、伤害与恢复分支

图号确定后Turn(target,source,1,0)；fresh ownPosition共享对象发布target.battleOpponent10，fresh current.flags发布targetPayload.FE4，再fresh ownPosition调用ApplyFlags。之后level>=9且技能259才TiledEffect(0,15)。重新取appliedIndex/flags W32：bit8目标phase31；否则bit100音效210/source.x→flash1→目标phase31，两支路直接进入命中结束段。

普通分支E000清且同appliedIndex.amount非零才依次可选110008、非负主图/direction、非负第二图，Rebirth(fresh ownPosition共享对象,target)，音效143/source.x，然后fresh currentIndex的amount→position→共享对象SubtractHP。**这里没有阶段20的10000000免疫扣血门禁**，不新增。负值比较另取currentIndex/position/共享目标；负时Construct(temporary,0)，原currentIndex→temporary.encoded→position→copyTarget→pub encoded→读取/pub mask→读取/pub BYTE count→Destroy。恢复该阶段，不改全局CopyPayload、虚表token或reserved。随后408DDC独立读取current.flags低BYTE bit20，才targetFDC|2→全局更新门1→alive1。

接着fresh hitFlags W32决定显示路径：8000只Advance(target)；4000先Advance，再fresh current.flags，10000000置位时fresh对应共享对象22459，否则同索引amount→position→共享对象AddHP，另取比较目标并Greater(HP,maxHp)，超上限才fresh位置/共享对象，按encoded/mask/BYTE count交替CopyPayload(maxHp→HP)，原正确体保持。恢复数显fresh numberFlags→targetY→amount→targetX，type5/style2；408F2F再独立读current.flags低BYTE bit2，可22454，随后Attribute(fresh ownPosition共享对象,target)。

2000只Advance；其它hitFlags的100000先MarkDeath(targetPayload.effect1054)，再取targetFDC W32、清1054、清20；然后Advance。fresh numberFlags的10000000置位同样22459，否则同读取序type6/style2数显，408FED独立低BYTE bit2可22454，随后Attribute。两数显分支与8000/2000路径共同：fresh skill259且fresh current.flags W32 bit80000才Debuff(target,110075)。不为特殊8/100路径加该Debuff，不新增source标死。

原音效额外压Y不被已有PlaySe本体消费，现两参保持；RNG/正常失败、真实接收者和返回消费、字段发布与各调用后的fresh取样保持。所有callee、间接alias与资源最终owner另验。

## 命中结束与计时尾

409056以fresh finishIndex读取flags低BYTE bit8；清且同索引amount非零时，按cached targetPayload.FDC W32 bit1计算14*(bit+1)/4，写payload.FD0正常3或7。不改其它flags W32缓存，不加延迟下界或异常回绕。AdvanceEffect(source)严格返回1才frameEvent=1；无新的while批量消费或隐含成功。

所有入口/命中路径共同计时：FD0>0减1后发布，再fresh读取FD0；严格0才PlayFrame(source,0)弃返→Move(source)→fresh frameEvent，等1时source.phase=23。原当帧写3/7也进入此尾减1，不能延后或立即强制phase23。返回计时值，计时为0走播放移动时返回fresh frameEvent；当前S32接口正文保持，不将非1返回当成功，不模拟原库残留寄存器。

<a id="battle-boomerang-hit-decision"></a>
## 七组修正与实施边界

主根一个CPP七组：入口frameEvent先取再固定payload；第二次转向先currentIndex再step；HP归零原encoded/位置-目标持样与三字段交替pub；activation20、恢复icon2、普通icon2、finish8四处原低BYTE读取。W32图号/应用后/显示/免疫与寄存器缓存保持；最大HP的正确CopyPayload和普通无免疫扣血、原3/7延迟/返回/调用序、其它源不改。主根/分档各补CPP及公开声明注释，分档正文/数据保持，无新函数/类型/API/保护或功能。

新增两独立语义/F adapted和两个BATTLE已知成员，枚举false；I仅主根七组，S覆盖主根与正确分档，不代全部父-callee/业务或SDK运行。U08/U09/R01/R04/V04只补来源/局部描述，所有分母/任务开放、六closure/总SPEC§10保持。按已获工程委托依本SPEC实施；RPLAN.24不专项故障/回绕/钳制，MSVC数值和WAV-only/原格式明文合同保持，不恢复CRT排序未决设计，不编译/测试/运行/算法/查产物/摘要或引依赖。

<a id="battle-boomerang-hit-acceptance"></a>
## 必要静态核对

核两个完整连续根/库存、662条2391B/十二页、主42直接CALL/28目标/99跳一RET及分档五跳一RET零CALL，唯一case22三条11B；四低BYTE原点、归零复制窗口、入口/第二转向读取序与全部技能、伤害/恢复/显示/延迟/返回分支有据。核0.3F、五阈值/五图20B与current const源一致，实际2022索引关系及16分档CALL只定位；不运行分档/数值/命中样例。公共尾、真实查找/正常CryptInt、转向和绘制/音效沿已有合同复用，不计新原根/全callee验收。核整CPP只有主根七组及两注释，公头仅两注释，分档/阈值/图表、最大HP复制、阶段20和所有其它源/签名保持；两身份/处置/成员、限定I/S、五任务与全部政策/门禁/分母、UTF-8无BOM-LF及静态汇总/-Check。全生产数据、所有父-callee/间接alias、owner/SDK正常失败/生命周期/声音画面运行仍待。

<a id="battle-boomerang-hit-source-state"></a>
## 当前状态

完整原根与七组差异已落实，必要静态与统一汇总检查通过；分档正确正文保持。前态cg2d-re-battle-boomerang-hit-history-20261010，提交引用cg2d-re-battle-boomerang-hit-20261010。整体SPEC未完成。

### 实际限定结果

必要静态、汇总与-Check通过：两完整662条2391B/十二页、主42直接CALL/28目标/99跳一RET与分档五跳一RET零CALL、case22 11B及分档16实际CALL/三父定位有据；四BYTE/归零复制、入口/第二转向取样与0.3F/五阈值/五图/2022索引匹配。整CPP仅主根七组/两注释，公头两注释，分档正文/数据、正确最大HP Copy与全部其它源和原独立普通无免疫扣血/恢复/图号/RNG/3-7当帧计时及真实S32返回保持；既有依赖/公共尾复用不重计新原根或全生命周期。语义2043→2045（1997F＋48X）、合同2105→2107/3680（57.26%仅合同关联率）、F1470→1472、E1359→1361仅限定I/S各一，I只主根/S两根，BATTLE283→285且枚举false。导入16/218、X3060/3061余1、184叶完整验收0、60任务关闭11与六closure保持；十一自有文件显式UTF-8无BOM/LF，精确提交固定引用cg2d-re-battle-boomerang-hit-20261010。未构建/测试/运行/算法/查产物/计算摘要，无新依赖；用户六项改动保留，整体目标active。全部生产数据/父-callee/间接alias、SDK数值/容量失败/重进退出与所有资源owner/声音画面运行仍未验，整体SPEC未完成。
