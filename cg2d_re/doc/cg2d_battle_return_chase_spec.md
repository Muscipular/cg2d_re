# 战斗回位、追近与骑乘门禁完整合同 SPEC（BRCP01）

2026-10-10；沿真实动作分派补动作2/3/4/6/11五完整根。移动/抵达正常正文已正确；仅动作11三处低BYTE消费及十二合同注释、独立身份与验收登记本步落实，不新增函数/API/保护或移动/播放消费。

<a id="battle-return-chase-baseline"></a>
## 完整原根与分派

| 动作 / 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / 原调用 |
|---|---|---|
| 2 / F:00006760 / UpdateBattleAction2_406760 | 406760–4067EC | 45 / 140B / 4CALL四callee、一跳一RET |
| 3 / F:00006C30 / UpdateBattleAction3_406C30 | 406C30–406CC3 | 45 / 147B / 6CALL五callee、一跳两RET |
| 4 / F:00006D70 / UpdateBattleAction4_406D70 | 406D70–406E03 | 45 / 147B / 6CALL五callee、一跳两RET |
| 6 / F:00006CD0 / UpdateBattleAction6_406CD0 | 406CD0–406D65 | 44 / 149B / 5CALL五callee、一跳一RET |
| 11 / F:000067F0 / UpdateBattleAction11_4067F0 | 4067F0–406930 | 112 / 320B / 7CALL五callee、十八跳三RET |

五动作合291条903B/六份64条分页、28directCALL/七唯一callee、22跳九RET，无间接调用/跳转。五根各一个实际CALL，全部来自404DB0。case2/3/4/6/11各11B的push→CALL→JMP40563F与共同尾局部窗口取齐；复用BTS01已有总分派及B22C01已证真实do-loop接线，当前五分派均弃返回，不重计该父或擅改循环。四S32接口继续原末PlayFrame返回，动作11当前void/末播放弃返保持；局部窗口不扩为全部机器ABI验收。

## 动作2：回原位置并直接归零

固定action.payload；依次characterAction=1、value54 F32=4、framePlayingRate=100。从固定载荷先读y04 S32→F32，再读x00→F32，TurnPosition(action,x,y,1,0F)后Move；再fresh读Y/X，DistancePosition的S32结果严格<5才抵达。

抵达先fresh X转换/发布offset40，再fresh Y读取转中间double，phase=0后发布offset44 F32，最后fresh完整FDC读/低AL OR1/完整W32发布。当前flagsFDC |=1保完整其余位和正常顺序；不误改为推进队列、提前清phase或合并两个坐标取样。两路都末PlayFrame(action,0)，保实际S32返回。

## 动作3/4：追近效果目标

固定payload；动作3为characterAction=1、动作4为3，共同value54 F32=8、framePlayingRate=100。先读C9C→按16B effect记录position→全局gBattleAllEntries查T*，TurnToward(action,target,1,0F)；调用后重新读取C9C/对应position/全局目标供Distance，不能缓存首次目标或提前合并两次取样。

动作3严格距离<70、动作4严格<8时AdvanceCast；另一分支只Move。原两分支各一个PlayFrame(action,0)，当前合并在if尾但正常路径各只调用一次、原先推进或移动后才播放及实际返回保持。不得改为先移动再比较，不能同帧同时推进和移动，不加目标/索引拒绝或新保护。

## 动作6：回FE8/FEC坐标

固定payload；characterAction=3、value54 F32=8、framePlayingRate=100。先FEC Y S32→F32后FE8 X→F32作TurnPosition(action,x,y,1,0F)，Move后fresh再按Y/X取样作DistancePosition。S32距离严格<5时先fresh X/offset40发布，再fresh Y/offset44发布，然后AdvanceCast；两路末PlayFrame(action,0)，保返回。不改目标字段为00/04、不先发布Y、不添加phase0/FDC bit1。

## 动作11：骑乘死亡短路与回位

固定payload；先rider13C8非空，再effect1060非空，取该effect固定BattleRideCarrierPayload。顺序first00 T*→其payload T*（原已有两级空检查）→FDC低BYTE bit2；首项死亡就短路，不读取second04。否则读second04及其payload的原空检查/低BYTE bit2。任一死亡时AdvanceCast一次，再fresh action.phase；恰为46才再AdvanceCast一次，随后直接return，不执行后续转向/移动/播放。载荷first/second是真T*，不截W32或加新空保护。

非骑乘死亡分支先持完整W32 FDC，再仅用低BYTE测试bit2及0Ch；bit2置且0Ch全清、characterAction非10时仅phase=46并return；该组characterAction已经10则继续。其它flags组characterAction非10才发布1；随后fresh FC4-1，非负且CA4上一项==7时发布3（即便此前为10也按原覆盖），不钳制上一索引或把7改11。

再fresh characterAction决定转向updateDirection（非10为1，等10为0）；先固定payload Y/X S32→F32、TurnPosition(...,0F)，Move后fresh Y/X作Distance。S32结果严格<5才fresh X发布offset40→fresh Y发布offset44→AdvanceCast。共同尾重新读取characterAction，恰10才PlayFrame(action,1)，否则0；不复用转向前方向值或为骑乘早退加播放。两处死亡测试改显式U8、持W32 flags之后加U8 entryFlags供两门禁，共三片段，原业务条件/空检查/短路/持样与fresh边界保持。

## 共享距离叶4062A0：正常平方和/转换

F:000062A0 / GetBattleDistanceToPosition_4062A0，连续4062A0–4062F8，30条88B/一页、零CALL、四跳一RET；其中4062F0直接尾JMP53B498（__ftol）。原零比较常量54D420完整4B为00000000。16实际CALL/11父全部定位，本步五动作内三处消费及此前已证阶段13/21只复用，其他父完整数据/结果仍待。五动作与本叶合321条991B/七页、28directCALL与一库尾跳，不按尾跳造新业务函数。

原action为空直接S32零；否则按传入F32 x减action.offset40 F32，再按F32 y减offset44求差，先Y平方后X平方相加，显式存F32 squared。负/零按原返回0；正值FSQRT后尾__ftol作S32转换。现double中间、float平方和、标准sqrt和正常S32转换正文正确保留；当前!(squared>0F)沿原两个比较的正常门禁，不添异常模拟/保护/钳制。MC01宿主库与既有数学近似边界保持，不宣称x87机器状态/越界值已验。

此叶此前未独立纳管；初稿误将其及439340归为既有BOG01/BQD01，核实际语义后纠正：距离叶本步补整根，439340沿BEV01、总分派沿BTS01及B22C01局部同步生产。不是重计旧完整root或改写旧验收。新增该叶两注释，不改任何数学正文。

<a id="battle-return-chase-decision"></a>
## 实施边界与登记

按既有工程委托先SPEC后落实：一CPP仅动作11三片段和六定义注释，公头六合同注释。动作2/3/4/6完整正文/签名、动作11全部其余正文及所有其它源保持。零新函数/API、RNG、图标、保护、拒绝、钳制、输入限制或业务重构；S32转F32/D64沿已批准标准数学和现字段，不恢复x87机器ABI/异常模拟。

五动作及共享距离叶六原身份此前均无独立语义/F处置，新增六语义/adapted处置/已知BATTLE成员，成员枚举false。一限定I只覆盖动作11三处BYTE消费，一限定S覆盖六完整现源；不由源码存在宣布完整I/运行通过。复用BOG01真实转向/406300距离/移动、BEV01推进、BTS01总分派与B22C01同步回调接线，所有未知alias/owner/SDK和正常动画仍待。

U08/U09/R01/R04/V04补来源/局部状态，五任务open；全部原分母、六closure、政策/门禁和旧记录保持。CRT七组排序此前两方案仍未决，WAV-only、原格式明文及RPLAN.24保持；不编译/测试/运行/算法/产物/摘要或引依赖。

<a id="battle-return-chase-acceptance"></a>
## 必要静态与剩余

核六完整连续根/321条991B/七份64条分页、28E8目标和一CRT尾跳/各跳-RET与库存、五11Bcase及共同尾局部窗口；三原BYTE测试的字节编码、持完整FDC/低BYTE消费，全部S32/F32/指针和原门禁/分支/读取/发布/返回。核动作2直接phase0/OR1，3/4两次fresh目标及互斥推进-移动，6两组FE8/FEC、11骑乘双T*空检查/短路/推进后phase46再推进/上一项7和末fresh播放；正确坐标发布与其它全部本体保持。

核整CPP仅三片段/六注释、公头六注释，无其它source/signature差异；六语义/F/成员、一限定I/S及五open任务/政策/旧台账/分母、UTF-8无BOM/LF和静态汇总/-Check。全部输入/生产/间接alias、全部callee/SDK正常失败/资源/容量/重进退出/数值/owner及声音画面运行仍未验，总SPEC§10不降。

<a id="battle-return-chase-source-state"></a>
## 当前状态

六完整正常根纳管，动作11三BYTE与十二注释已落实；必要静态、汇总和-Check通过，不含编译或运行。前态cg2d-re-battle-return-chase-history-20261010，提交引用cg2d-re-battle-return-chase-20261010。整体SPEC未完成。
