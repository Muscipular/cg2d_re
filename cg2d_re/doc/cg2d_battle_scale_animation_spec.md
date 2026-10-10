# 战斗放大、FP扣减与动画完成完整合同 SPEC（BSA01）

2026-10-10；动作54–63及55实际调用的405B60扣FP叶共十一完整正常根。所有现正文/签名/数值/调用正确保持，仅二十二合同注释；独立原身份和必要静态登记不计新实现I。

<a id="battle-scale-animation-baseline"></a>
## 完整原根与真实分派

| 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / CALL |
|---|---|---|
| F:0000BFE0 / UpdateBattleAction54_40BFE0 | 40BFE0–40C0B8 | 67 / 216B / 7 |
| F:0000C0C0 / UpdateBattleAction55_40C0C0 | 40C0C0–40C0DE | 12 / 30B / 3 |
| F:0000C160 / UpdateBattleAction56_40C160 | 40C160–40C19E | 18 / 62B / 2 |
| F:0000C1A0 / UpdateBattleAction57_40C1A0 | 40C1A0–40C1D8 | 17 / 56B / 2 |
| F:0000C1E0 / UpdateBattleAction58_40C1E0 | 40C1E0–40C218 | 17 / 56B / 2 |
| F:0000C240 / UpdateBattleAction59_40C240 | 40C240–40C278 | 17 / 56B / 2 |
| F:0000C220 / UpdateBattleAction60_40C220 | 40C220–40C236 | 6 / 22B / 1 |
| F:0000C280 / UpdateBattleAction61_40C280 | 40C280–40C2AA | 14 / 42B / 2 |
| F:0000C2B0 / UpdateBattleAction62_40C2B0 | 40C2B0–40C2DA | 14 / 42B / 2 |
| F:0000C2E0 / UpdateBattleAction63_40C2E0 | 40C2E0–40C379 | 44 / 153B / 3 |
| F:00005B60 / ConsumeBattleFpCost_405B60 | 405B60–405BCA | 33 / 106B / 4 |

共259条841B/十二份最多64条分页、30E8/十一唯一callee、21跳十二RET；无间接CALL/JMP。十动作各唯一实际入向CALL/父404DB0，十case各11B push→CALL→JMP40563F；扣FP另有40C0CE唯一CALL/父40C0C0，合十一入向/两父。54/63保真实S32末播放返回；其它当前void及case真实弃返保持。局部分派不代全部父/所有callee/ABI/owner验收。

## 54：放大与两次独立设备查询

固定payload→查询设备→fresh完整flags，设备有则OR100000h、无则OR8000h并整字段发布，再Sub_406260(action)。之后fresh scaleX＋54D490原F32步长3CA3D70A（约0.02）持未舍入中间，F32发布scaleX；再fresh scaleY加相同步长后F32发布。只在未舍入scaleX>原54D494的1.0时scaleX=1、scaleY=1；不提前用已舍入值作上限门禁、不改为统一两轴上限/新输入钳制。现double正常中间和float字段保持，既有数值/宿主许可不外推新异常模型。

fresh完整1024==0才创建type23：NewBattleEffect(action,0,offsetY0,23,0,1)，实际返回发布+10；无新失败保护/仅首帧缓存。创建后fresh scaleX按W32位模式严格==3F800000h（原整数CMP），不能换近似epsilon或一般>=1条件。完成时取+10，非空MarkDeath后清槽；随后再次调用实际设备边界，按这次结果及fresh完整flags清100000h或8000h、整字段发布→1024=0→Advance。不能缓存入口设备结果、不跳第二次设备查询或把MarkDeath当free。每一正常路径最后PlayFrame(action,1)，保S32返回。

原两常量54D490/54D494完整8B实读，step/one分别3CA3D70A/3F800000；前者已在BDE01缩小合同使用，只复用不重计X。SDL3设备边界和BNZ01 type工厂、BOH01方向恢复/BEV01推进仅沿真实既有提供者，不在本步更改。

## 55/405B60：播放后实际扣FP与业务负值零归

55按PlayFrame(action,1)弃返→ConsumeFp(action)→Advance，不根据播放返回早退/等待终帧。405B60重新固定此时payload，持完整S32 fpUsed1008为cost，仅cost>0才SubtractAssign(&barValue1B0,cost)；之后Less(&barValue1B0,0)是真实CryptInt读取/比较，不提前读取旧FP或按明文额外读取替代，不改变读取计数/库内原业务消费。

扣后负值才ConstructFromInt(临时对象,0)，按mask/encoded/BYTE readCount全部读齐，再mask(+8)→encoded(+4)→BYTE count(+C)发布到既有FP对象，保token/未复制字节，Destroy临时对象；不改CopyPayload/整对象复制或提前销毁。无论是否负值，cost>0路径末对固定payload发布fpUsed1008=-1；cost<=0不扣/比较/构造，也不发布-1。原业务负值零归保持，不新增最大FP封顶、cost限制、保护或异常模拟。现真实CryptInt及BSC01已纳管比较叶（旧语义1584号）复用，不把本叶当全输入/加密对象owner/RNG/机器ABI验收。

## 56–62：严格终帧和不同帧状态清理

56先characterAction5，再PlayFrame(action,1)；严格S32返回1才frameIndex(+2C0)=0→frameRemain(+2C4)=0→frameEvent(+2D0)=0→Advance。57/58/59分别动作13/14/15，同样严格返回1才frameIndex=0→frameRemain=0→Advance，原不清frameEvent。不统一公共清帧助手或把事件清零扩到三根。

60只characterAction15→Advance，不调用Play、不清帧。61/62分别动作12/18→PlayFrame(action,1)，严格返回1才Advance，原不清index/remain/event。所有根没有新增rate/方向写入，不用非零/>=1代严格终帧，也不把当前不同职责合成同一循环；现全部正确正文保持。

## 63：旧计数门禁与两次方向读取

固定payload，先characterAction0，再持旧FF0为previous、新count=previous+1并发布FF0。旧previous>=70才Advance后FF0=0，并进入公共PlayFrame(action,0)返回；不改新count>=70、提前归零/按count限制输入，也不在推进后早退漏播放。

旧previous<70时保存的新count==20才direction减2；随后fresh payload.FF0==50才direction加4，不能复用之前count或改互斥else-if。然后独立读取direction，>=8仅减8发布一次；再fresh读取修正后direction，<0才加8发布并从该支PlayFrame(action,0)返回，否则公共尾同样PlayFrame(0)返回。原每条正常路径仅一次播放，两个静态播放点/两RET都保持；不改%8/循环纠正或追加播放。RPLAN.24不专项处理溢出/回绕。

<a id="battle-scale-animation-decision"></a>
## 实施及登记边界

依第8条既有委托先SPEC后落实：一CPP十一定义注释、公头十一注释；十一完整正文/签名、所有数值/业务正常上限/调用/其它源与复用旧语义/处置保持，零消费/算法改动、新函数/API/保护/默认值/播放/RNG或业务重构。十一原身份此前无独立语义/F处置/已知BATTLE成员，新增十一adapted语义/处置/成员，枚举false。

一限定S仅十一完整当前正常源/十局部分派/扣FP依赖接线/8B原常量，不新增I、不记新全实现。U08/U09/R01/R04/V04追加来源与局部状态且open；原分母、所有政策/门禁、旧记录及六closure保持。MSVC CRT、WAV-only、原格式明文、RPLAN.24和当前执行禁令保持；CRT排序此前取舍未答，不外推批准，不构建/测试/运行/算法/产物/摘要或引依赖。

<a id="battle-scale-animation-acceptance"></a>
## 必要静态及剩余

核十一完整259条841B/十二页/库存、30E8/十一callee/21跳十二RET、十一真实入向/两父、十11Bcase和8B两F32位模式。核54入口/完成两次设备、方向后fresh缩放/未舍入上限/创建后W32精确1与清理发布/共同播放；55播放→真实扣FP→推进及cost正门禁/读取消费/三字段临时复制/末-1；56–62不同角色动作/严格终帧/清帧职责和60无Play；63旧70/新20/fresh50、方向两独立读取/一次wrap和互斥两播放返回。

核整CPP/公头仅二十二注释、所有其它全文/签名/数值/真实调用/提供者与旧记录保持；十一语义/F/成员、仅限定S零I、五open任务及全部原政策门禁，UTF-8无BOM/LF、必要差异与文本汇总/-Check。完整1008/1024/动画状态生产、其它父/全部callee/ABI/alias/owner、SDK正常失败/容量/数值/重进退出与声音画面运行仍待，总SPEC§10不降。

<a id="battle-scale-animation-source-state"></a>
## 当前状态

完整原根/常量和当前源已取齐，正文全保持仅二十二注释；必要静态、文本汇总/-Check及精确派生范围通过。语义2117（2069F＋48X）、合同2179/3680=59.21%（仅关联率）、F1545、E1390仅一限定S零I、BATTLE357且枚举false；五任务open、旧记录/原分母/全部政策门禁保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-scale-animation-history-20261010，提交引用cg2d-re-battle-scale-animation-20261010。整体SPEC未完成。
