# 战斗播放、计时与目标等待完整合同 SPEC（BPAW01）

2026-10-10；补动作5/7/8/9/10/24/25七完整正常根及真实分派。仅7/8/10/25四处显式低BYTE消费和十四合同注释需本步落实，其余正确完整正文/签名/源保持；零新函数/API/移动/播放/保护或RNG。

<a id="battle-play-wait-baseline"></a>
## 完整原根与真实调用

| 动作 / 原身份 / 当前源 | 完整连续根 | 指令 / 字节 / 原直接调用 |
|---|---|---|
| 5 / F:0000CFA0 / UpdateBattleAction5_40CFA0 | 40CFA0–40D00F | 32 / 111B / 四CALL三callee、一跳两RET |
| 7 / F:00009410 / UpdateBattleAction7_409410 | 409410–40949B | 43 / 139B / 四CALL四callee、四跳一RET |
| 8 / F:000094A0 / UpdateBattleAction8_4094A0 | 4094A0–40950A | 31 / 106B / 三CALL三callee、两跳一RET |
| 9 / F:00009510 / UpdateBattleAction9_409510 | 409510–409605 | 82 / 245B / 十CALL七callee、四跳三RET |
| 10 / F:00009610 / UpdateBattleAction10_409610 | 409610–40967C | 31 / 108B / 三CALL三callee、四跳一RET |
| 24 / F:00009380 / UpdateBattleAction24_409380 | 409380–4093AB | 12 / 43B / 一CALL一callee、一跳一RET |
| 25 / F:00006BA0 / UpdateBattleAction25_406BA0 | 406BA0–406C28 | 44 / 136B / 两CALL两callee、五跳两RET |

合275条888B/八份64条分页、27directCALL/八唯一callee、21跳十一RET，无间接调用/跳转。各根一个实际入向CALL，全部来自404DB0；七个对应case各11B push→CALL→JMP40563F取齐。本步只核七分派窗口，沿BTS01既有总分派/既有回调接线，不重计该父或外推其它case全部ABI/owner已验。动作5/9保S32实际末PlayFrame返回，其余当前void/弃返保持，分派本身没有额外返回消费。

## 动作5与24：计数持样和推进顺序

5先固定payload，依次characterAction=3、value54 F32=8、framePlayingRate=100；持旧FF0 S32，发布旧值+1，再以旧值严格<16决定Move，另一支AdvanceCast后才FF0=0。原CMP旧值在增量发布前、分支在发布后，当前纯本地比较表达恢复同一正常控制关系；不把比较改为新FF0、不交换推进/清零。两支原各末PlayFrame(action,0)，当前共同尾每路一次并保S32返回。

24持FF0+1为S32新count，先发布count，再严格count>20才FF0=0后AdvanceCast。与5的持样和归零/推进序不同；24原无characterAction/rate发布、Move或PlayFrame，当前不补这些消费。不加入越界/溢出模拟、范围保护或新计数限制，RPLAN.24保持。

## 动作7/8：转向后严格终帧推进

两根先固定payload、characterAction=2、framePlayingRate=100。7先读一次C9C对应16B effects记录的position；等-1才按固定payload先Y04后X00的S32→F32取样调用TurnPosition(action,x,y,1,0F)，否则按保存position查一次全局目标作TurnToward(action,target,1,0F)，不调用后额外重取目标/位置。8始终先FEC Y后FE8 X S32→F32，TurnPosition(action,x,y,1,0F)。

随后各一次PlayFrame(action,1)，仅严格返回==1才fresh固定payload.FDC低BYTE bit2；置时framePlayingRate=200，再AdvanceCast，不置也AdvanceCast。非1直接尾，不读该FDC测试、不提升速率、不推进；不缓存播放前flags、不改为非零播放成功。两处现显式U8匹配原byte ptr TEST，其余流程保持。

## 动作9：两项前的14切换距离路径

固定payload；characterAction=3、value54 F32=8、framePlayingRate=100。先读FC4 S32-2为previous，previous>=0且CA4[previous]==14才走回00/04：先Y/X取样作TurnPosition(...,1,0F)，调用后fresh Y/X作DistancePosition；距离S32严格<5则AdvanceCast后PlayFrame(action,0)并保返回。

其它previous关系走效果目标：C9C→position→全局T*供TurnToward，调用后fresh重新取C9C/position/全局T*供Distance；严格<70则AdvanceCast后PlayFrame(action,0)返回。两距离分支未抵达共只Move后PlayFrame(action,0)，不先移动再比较、不同时推进和移动。不钳制负previous以外索引、不添加目标空拒绝，也不把CA4位置误读成固定C9C字段。所有已有正确正文保持，不缓存两次目标/坐标或合并取样。

## 动作10：收跑终帧恢复方向

固定payload；characterAction=4、framePlayingRate=100，然后PlayFrame(action,1)。仅==1才fresh FC4-1，非负且CA4[previous]==11才fresh FDC低BYTE bit2；清时fresh battlePosition18查全局T*调用Sub_406260。该方向叶原完整合同沿BOH01语义1392/F:00006260复用，只本步局部call，不改正文/身份/处置或重计成员。

不论上述previous/11/bit2门禁是否符合，已进入终帧路径就最后AdvanceCast；非1不取这些字段/不推进。目标为空仍交真实方向叶的原空处理，不在父新加保护。低BYTE表达修正不改变短路取样或条件，不能使用Play前缓存FC4/FDC或把上一项11替换动作9的前两项14。

## 动作25：等待目标与X/Y重新查找

固定payload；持C9C对应position，position==-1直接AdvanceCast/return；否则查全局target T*，为空同样AdvanceCast/return。正常target存在时先battlePosition18==position，成立不读target.phase；不成立才读target.phase==0。

符合任一关系时先固定该target.payload，读target.x S32/发布targetPayload.x10；随后fresh从固定sourcePayload重新读取C9C/对应position/全局currentTarget T*及其payload，读currentTarget.y S32/发布currentPayload.y14，再AdvanceCast/return。不把两个发布合并、不复用第一次target或把X/Y都写第一次payload；原后半无额外空保护，当前不新加。

其它目标非0阶段路径先action.characterAction=0，然后fresh固定payload.FDC低BYTE，0x60全清才PlayFrame(action,0)弃返回，不推进。原MOV CL/TEST CL 60h，现显式U8消费；不缓存归零前flags、恢复速率或为position/target早退加播放。

<a id="battle-play-wait-decision"></a>
## 本步实施和登记

按既有工程委托先SPEC后落实：一CPP四低BYTE表达片段与七定义注释，公头七合同注释。零新函数/API/保护/输入限制/默认值/稳定序；所有其它本体/签名/源和原固定/fresh指针、S32/F32字段、计时/分派/播放/返回保持。BOG01转向/移动/406300距离、BRCP01共享4062A0、BEV01推进和BOH01方向叶仅复用，不重核已过整根或重复纳管。

七身份此前无独立语义/F处置/已知BATTLE成员，本步新增七语义/adapted处置/成员，成员枚举false。一限定I只覆盖7/8/10/25四BYTE片段，一限定S覆盖七完整正常现源；不据源码存在计新增全实现、所有数据/父/callee/alias/owner、SDK正常失败/容量/数值/声音画面运行通过。

U08/U09/R01/R04/V04补来源/局部状态，五任务保持open；原分母、六closure/政策/所有门禁和旧记录保持。CRT排序此前选择仍未答复，不作为本步阻塞；WAV-only、原格式明文/RPLAN.24保，不构建/测试/运行/算法/产物/摘要或引依赖。

<a id="battle-play-wait-acceptance"></a>
## 必要静态和剩余

核七完整连续根/库存、275条888B/八页、27E8真实目标/八callee/各跳-RET、七11B分派和共同尾准确；四原BYTE编码/播放后fresh字段、计数旧/新样及不同归零/推进序、9两次目标/坐标与互斥推进-移动、10短路门禁和实际方向提供者、25X发布后重新查目标作Y发布。

核整CPP仅四表达片段/七注释、公头七注释，其余源/签名以及BOH01旧1392记录/方向正文保持；七语义/F/成员、限定I四BYTE/S七完整根、五open任务/全部政策门禁及UTF-8无BOM/LF、汇总/-Check准确。完整输入/生产/全部父-callee/未知alias、SDK正常失败/容量/重进退出/数值及资源owner/声音画面运行仍未验，总SPEC§10不降低。

<a id="battle-play-wait-source-state"></a>
## 当前状态

七完整正常根/四BYTE消费和十四注释已落实；必要静态、汇总与-Check通过，不含编译或运行。前态cg2d-re-battle-play-wait-history-20261010，提交引用cg2d-re-battle-play-wait-20261010；整体SPEC未完成。
