# 战斗受击后退、恢复与活动计数完整合同 SPEC（BHRC01）

2026-10-10；补动作26–31及实际依赖406360活动计数七完整正常根。现阶段/计时/创建/播放流程正确，仅十处显式BYTE/WORD消费表达和十四合同注释、身份/验收登记本步落实，不新增函数/API/保护/RNG/移动/播放或数值算法。

<a id="battle-hit-reaction-baseline"></a>
## 完整原根与接线

| 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / 原CALL |
|---|---|---|
| F:0000A9E0 / UpdateBattleAction26_40A9E0 | 40A9E0–40AA66 | 38 / 134B / 四CALL两callee、三跳两RET |
| F:0000AA70 / UpdateBattleAction27_40AA70 | 40AA70–40ACE6 | 186 / 630B / 十二CALL五callee、二十跳五RET |
| F:0000ACF0 / UpdateBattleAction28_40ACF0 | 40ACF0–40AE4B | 109 / 347B / 十CALL五callee、九跳四RET |
| F:0000AE50 / UpdateBattleAction29_40AE50 | 40AE50–40AF25 | 59 / 213B / 六CALL三callee、五跳三RET |
| F:0000AF30 / UpdateBattleAction30_40AF30 | 40AF30–40B01C | 68 / 236B / 六CALL三callee、五跳三RET |
| F:0000B020 / UpdateBattleAction31_40B020 | 40B020–40B130 | 80 / 272B / 六CALL四callee、十二跳三RET |
| F:00006360 / CountBattleActiveEntries_406360 | 406360–406386 | 13 / 38B / 零CALL、三跳一RET |

合553条1870B/十二份最多64条分页、44directCALL/七唯一callee、57跳二十一RET，无间接调用/跳转。六动作各一个实际CALL来自404DB0；六case各11B push→CALL→JMP40563F取齐。计数叶九实际CALL/九父，合本步十五入向/去重十父；其余父完整生产/SDK/owner未验。复用BTS01已有总分派，不重计父或据局部分派宣称全部ABI通过；26当前void/播放弃返，其余五动作保实际S32末播放返回，计数叶返回S32。

## 26/27/29/30入口标志和角度反转

固定payload后先characterAction=8；完整FE4读取的AL bit4或AH bit0Ch任一置时发布9。当前一次U16消费0xC04，原完整字段及其它位不改。28始终8，31按自己的阶段发布，不给它们新增该入口门禁。

26先持angle50 F32并加原F32常量180，依次rate=50、value54 F32=5、发布angle50；持加法中间值>=360才再发布减360后的F32，不重读已经舍入的角度或改多次模360。之后Move再PlayFrame(action,1)，无FCC计数/推进。本逻辑也在27早期与28/29/30各自早期分支使用；double中间/标准数学差异沿既有许可，F32字段和原发布/正常判断保持，不新增x87机器ABI/故障模拟。

原常量54D420零、54D424=360F、54D438=180F、54D464为3E99999Ah（约0.3F），四处4B共16B实读；该数值区不重计为新X或图号表。

## 27：入口持样时长、fresh阶段门禁与活动发布

先8/9和rate50；持完整W32 initialFlags=FDC，取其bit1为initialBit；fresh旧FCC+1为count并发布。count<14*(initialBit+1)才反转角度/速度5，之后fresh FDC低BYTE bit2置时依次全局gYobiUpdateGateMode=1、payload.aliveFF8=1，然后Move/PlayFrame(1)返回。不能把fresh测试改为initialFlags，不能交换两状态发布。

下一段count<14*initialBit+34：fresh FE4低BYTE bit2置才direction先发布旧值+1，保存该值>=8时再减8发布（一次，不改%8）；随后fresh完整W32 FDC移位bit1生成14*(bit+1)阈值，再fresh FCC比较相等。相等先速度5，随后fresh FDC低BYTE bit2置才调用CountBattleActiveEntries；结果<=1先全局gate=0，再aliveFF8=0；无论该BYTE是否置，该边界最后FF0=0。计数调用先于本对象alive清零，不能把返回当BOOL/只==1或缓存全局计数。

随后fresh FF0==0才NewBattlePositionEffect(action,110006)弃返；创建后fresh FF0旧previous→发布previous+1→previous>=2再0，保三周期/原创建失败消费，不提前取previous或重试。fresh value54先减原0.3F，先F32发布该speed，持中间speed按原负值业务门禁才再速度0；Move再PlayFrame(1)返回，不把原归零改为新保护或改数字常量。

剩余段count>=14*initialBit+54才FCC=0；使用入口持样initialFlags低BYTE bit2决定收尾（不fresh FDC）：清时速度6/rate100→AdvanceCast→fresh phase恰11才速度4/rate100；置时fresh完整FE4的低WORD 0x3010非0给速度6/rate100，否则速度1/rate250，之后AdvanceCast。无论是否到阈值，共同尾PlayFrame(1)保返回，中间等待段不移动/创建或重设速度。

## 28：音效后计数与14/34/54三段

固定payload、characterAction8；若入口FCC==0才PlaySe(132,action.x)，原读取未消费的Y沿已确认音频边界不恢复第三参数。随后rate50，fresh FCC+1→count发布。count<14反转角度/速度5→Move→PlayFrame(1)返回。

count<34的第二段：仅保存count==14时速度5/FF0=0；随后FF0==0创建110006，再fresh旧FF0增量/旧值>=2归零，按同一0.3F减速/原负值归零、Move/PlayFrame。count>=54才FCC=0→速度4→rate100→AdvanceCast；34..53不移动/创建/推进但仍尾PlayFrame(1)，正常S32返回保。创建与音效当前正确接线，不提前FCC取样，不增音效/Play消费。

## 29/30：计数阈值及后期fresh死亡位

两根8/9→rate50，fresh FCC+1→count发布；count<14才角度反转/速度5→Move→PlayFrame(1)。29之后count>=44才FCC=0→rate100→速度4→AdvanceCast，14..43仅尾播放。30在count>=14之后才fresh完整W32 FDC移位/bit1计算14*(bit+1)，count达该阈值才按同一发布序推进；不将该FDC读取提前为入口缓存，原14/28门禁和末PlayFrame(1)/S32返回保持。

## 31：FCC重取、偶数旋转与等待对手14

固定payload、rate100；先持旧FCC和完整W32 FE4，再count=旧FCC+1并发布；持FE4的08000000h置且保存新count为偶数才direction先+1发布、保存值>=8再一次减8发布。随后fresh FCC为current决定三段，不复用之前count或截FE4高位；当前正常S32 %2表达保原偶数关系，不专项模拟异常。

current<14时characterAction1→速度-4→Move；14<=current<28时characterAction0，无Move；current>=28才取非拥有battleOpponent10。其为空或fresh other.phase!=14时继续恢复，否则不归零FCC/不推进，仍尾播放且后续帧计数继续。

恢复分支fresh完整FE4 bit08000000h置才Sub_405CB0恢复方向；调用后characterAction0，再fresh完整FE4的低WORD 0x1010决定速度6或4，然后FCC=0→AdvanceCast。不能复用入口flags、在对手仍14时提前归零/切动作，或把两个方向相关FE4读取合并。各路径尾PlayFrame(1)保实际S32返回。

## 活动计数406360：全部22槽/FF8严格等1

原按59E980至59E9D8不含尾、4B指针步长顺序扫描，明确22槽（含20/21特殊槽）；当前world::detail唯一std::array<YobiAction*,22>按原生指针拓宽，非地址字段不拓宽。每槽fresh T*，为空跳过；非空取其payload，对aliveFF8完整S32严格==1才计数加1。原没有payload为空保护，当前不新增；不是非零/低BYTE或仅扫描20玩家。

现正文正确保持，只两注释/独立登记。它在27边界调用时包括当前尚未清alive的对象；真实表/所有生产/其它八父/借用owner仍待，不把九CALL定位代替全consumer合同。

<a id="battle-hit-reaction-decision"></a>
## 实施边界

按既有工程委托先SPEC后落实：一CPP仅十处消费表达（六WORD、四BYTE）及七定义注释，公头七合同注释。26/27/29/30入口C04、27末3010、31末1010为六U16；27早期fresh FDC、减速fresh FE4、边界fresh FDC及收尾cached initialFlags为四U8。完整W32初始/阈值移位和08000000h测试保，不误统一截为BYTE。

所有其它完整本体/签名/源、标准数学/业务零归/角度与速度发布序、计数/门禁/创建/音效/移动/播放/返回保持。零新函数/API/保护/拒绝/钳制/异常模拟/RNG或新数值算法。BNV01既有1354号41E680位置创建器、BOG01移动/方向、BEV01推进和现WAV音效仅复用，旧语义/身份/处置不重计。

七根此前均无独立语义/F处置/已知BATTLE成员，新增七语义/adapted处置/成员，枚举false。限定I仅五动作的十WORD/BYTE消费，S覆盖七完整正常现源，28与计数叶不计新增实现；U08/U09/R01/R04/V04补来源/局部状态且open，原分母、六closure及全部门禁/政策保。CRT排序此前选择未答复，WAV-only、原格式明文与RPLAN.24保，不构建/测试/运行/算法/产物/摘要或引依赖。

<a id="battle-hit-reaction-acceptance"></a>
## 必要静态与剩余

核七完整连续根/553条1870B/十二页/库存、44E8/七callee/57跳二十一RET、六11Bcase与计数九CALL/九父、22槽边界/完整FF8严格等1；四处F32常量16B，十原AL-AH/CL-CH/byte ptr消费与完整W32移位/高位门禁、所有持样/fresh/发布及分支返回有据。

核整CPP仅十表达片段/七注释、公头七注释；全部其它源/签名/数值正文及BNV01旧1354/位置创建处置保持；七语义/F/成员、限定I十表达/S七根、五open任务与全部旧台账/政策/门禁、UTF-8无BOM/LF和汇总/-Check精确。不执行计时/角度/减速/样例，完整生产/全部父-callee/ABI/alias/owner、SDK正常失败/容量/重进退出/数值和声音画面运行仍未验，总SPEC§10不降低。

<a id="battle-hit-reaction-source-state"></a>
## 当前状态

七完整正常根/十消费表达和十四注释已落实；必要静态、汇总与-Check通过，不含编译或运行。前态cg2d-re-battle-hit-reaction-history-20261010，提交引用cg2d-re-battle-hit-reaction-20261010。整体SPEC未完成。
