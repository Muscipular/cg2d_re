# BQU01 战斗命令状态分发、倒计时与目标提示SPEC

2026-10-07。沿[BQT01宠物按钮与两命令回调](cg2d_battle_pet_command_button_sampling_spec.md#battle-pet-command-button-sampling-source-state)，补齐另一真实父498520及其倒计时、目标提示依赖。依AGENTS.md第8条先写裁决再实施，[完整原指令与源前后态](cg2d_battle_command_state_dispatch_sampling_evidence.json)为依据。此前“keyboard”是未核完整本体时的泛称；本根实际处理命令状态与鼠标，没有键盘读取，当前名称与验收范围在本批纠正。

<a id="battle-command-state-dispatch-sampling-baseline"></a>
## 完整基线与修复范围

| 原根 | 符号 | 指令/实际B/页 | CALL/普通jump/switch/RET | 语义 |
| --- | --- | --- | --- | --- |
| 498520 | Sub_498520 | 761/2882/10 | 74/110/3/1 | 899既有格式记录完整续记 |
| 43C3E0 | Sub_43C3E0 | 56/163/1 | 2/5/0/2 | 1559新正常F，正确正文保持 |
| 4982C0 | Sub_4982C0 | 171/593/3 | 16/22/0/1 | 922既有格式记录完整续记 |

合计988条3638B/14页、92物理CALL（91直接/24callee、1实际GetTickCount IAT）、137普通jump/三switch/四RET，各一连续chunk。三条实际入向：主根43D05A来自43CBB0，两叶分别49856C/498B6E来自主根；上游43CBB0仅登记，不能据主根完整推其通过。

| 差异 | 原点 | 裁决 |
| --- | --- | --- |
| 战斗标志BYTE观察 | 主4985AF/4988C7/498ACA/498B7A/498B59，计时49833E/4983B9 | 七实际内存低W8读，用既有unsigned byte视图恢复，共享字段仍W32 |
| 鼠标BYTE观察 | 498928/49896B/498CDD/498D4E/498D9D/498DFD/498E5A/498EB4 | 八实际低W8读；短路门禁、提示前/中间取样、held click保持 |
| FP cost-first原桥 | 49869F/498C1C原401F80 | 复用IsCostGreaterThanYobiActionFp(cost, action)，两阶段动作T*与费用获取序保 |
| held flags谓词与计数发布 | 4985D2 W32读→4985DB TEST20000→4985E0 pub | 原W32 flags cache先计算skipPlayer，随后发布旧petCount+1，再据该谓词分支 |

两体12种替换/18实际位置，其中15个真实W8读、两FP桥、一个谓词阶段。普通旧值一致，FP单Read与RNG消费不变，不虚称数值、并发或故障差。498767实际MOV W32后TEST AL以及49861D W32读/AND AL/写W32保持；不把所有AL测试改成内存BYTE，不扩/窄共享非地址字段。

<a id="battle-command-state-dispatch-sampling-decision"></a>
## 状态、选择与原发送裁决

1. 主入口held hover扫描原59E980至59E9D0的前20槽，非空action的cacheIndex相等且flags低W8 bit8才RequestCursor(2,0)，随后目标提示。既有span20正确，不增加边界、补空或扩到其它槽。
2. command0先当前位置<20及player内层骑乘T*；BYTE flags8时原N→旧petCount/held W32 flags→skipPlayer谓词→count发布。skipPlayer为真再N→skillCount/scene5/count发布，否则fresh W32 flags清8/byteRebirth0；两路Close后playerAction0。原W32高bit20000不能窄化。
3. 骑乘正常选择保skill=-1/selectionMode/skill mask、cost-first FP、fresh骑宠/skill target S16和Check0各门禁。需要打开才phase1/index-1/47A580(&497FF0)，本父弃真实返回，不仿按钮写CC423C。graphicIndex=-2→CloseSub→mode>0标槽/Flush、mode0只标0、negative不标→command4/SetCommandType(4,341,0)/声56/playerAction0。
4. command0普通分支保W32 flags cache及寄存器AL位4/20/10消费、N后fresh位置/flags、原计数/scene与clear4写序。初始化按count选择两个模式，byteRebirth0各阶段不合并；Reset9/CloseSub→非负mode标槽/Flush/fresh mode→0 Check475、2技能callback4974C0、3窗27、4窗28，默认不新动作。最后BYTE modeFlags→command1→原commandType0/1/2与声56/playerAction0。
5. command1原右键BYTE2仅usedSlot0==1才清槽/Flush/声57。CB054C本来就是W8存储，保持；非0才GetSelected，!=-1且左BYTE1时mode0 H|%X、2 S|%X|%X|%X、3 I|%X|%X，默认不发。target先held，技能分支technique再skill取样/实参skill-technique-target保持，原未清256B/各U32→fresh socket发送弃返→Flush/playerAction1/声57。完成时旧skillCount→command2→count+1→Flush/Close，fresh CB054C零才pub1。
6. command2仅window26.userState W32==3，旧count→playerAction0；count1实际BYTE singleAction区分command0/3、lastMode及mode6归零/skillUsed/secondMode写序，mode2..6与mode8 rebirth分支保持。count非1第二模式取样、mode6归零/scene5保。
7. command3入口BYTE flags20无则scene5；bit8有且位置<20才W|FF→petCount+1/scene5。否则普通宠物skill/mask、映射位置/action T*→fresh活宠/skill/cost-first FP、fresh活宠/target S16/映射位置Check各阶段保。需打开才phase1/index-1/47A540(&497E30)弃返回；最后graphicIndex=-2/command4/SetCommandType(3,341,0)/声56，不凭对称性加playerAction写入。
8. command4 phase3先目标!=-1/左BYTE1→fresh technique构W|%X|%X→fresh socket/Flush/playerAction1/声57，再fresh phase；phase2发布3后按原switch0..3。phase0右BYTE2先held ridePet→phase1→按符号选selector，hover相等时BYTEclick在打开提示15B复制前取，左键只phase1；phase1先复制关闭提示12B→BYTEclick→尾3B，左键借window10 T*存在才W32 state|8000→phase0/声57，不能移动点击取样或合并15B复制。
9. phase2/3右BYTE2→Flush→held ridePet→phase1/index-1→selector弃返回；hover返回提示时BYTEclick在15B复制前取，左键Flush/phase1/index-1，跳过名称/费用但仍图5899。否则fresh活宠/nameTechnique/%-16s256B→Y后X实际W32→低S16 Text2(x-114,y-15)，另一次fresh活宠/costTechnique/%4d→Y后XW32→Text2(x+90,y-12)。图589B/589C/5899按原W32 Y/X→DrawGraphic真实返回pubgraphicIndex，默认phase不绘制。playerAction1旧petCount→command5→count+1→fresh活宠last→Close。
10. command5仅userState3；fresh player内层骑乘存在且petCount<2才playerAction0/command0，否则scene5。全部分支最后fresh flags BYTE，4/8/10全置才跳倒计时；其余调用完整4982C0，不增加键盘处理。
11. 目标提示43C3E0正确正文保：CommandValue非0立即返；20槽首非空动作flags低W8 bit8才绘制，command4 y88否则91，图58B1(502,y+20,212,0,null)、Text2(383,y+12,2,0,0,原11B“請選擇目標”,0)，随后19B“按滑鼠右鍵可取消。”复制到hover。不以无目标“改善”行为。
12. 倒计时先GetTickCount32→W32deadline差的原S32判定→active W32门禁；到期清active/remaining0→Flush/Close→fresh player骑宠判断。骑乘发W|FF；否则实际BYTE flags→旧skillCount：bit20有时count0且无bit4发N，再fresh petCount0发W|FF；bit20无时count0且bit4有不发，其余count0连续两N（每次fresh socket）后fresh BYTE flags，最后fresh skillCount1且无bit4再N。不能合并重复N、把finish强置或提前结束尾绘制。
13. 仅finish才声55/x320→held commandAction T*→scene5→MarkDeath→清槽。正常remaining<10000使用569A、seconds变时声202→pub秒，否则5690；原%2d未清16B显示只看两S8字符、空格跳图、(304/336,240,209,base+digit-'0')。modeFlag==1原turn W32+1/11B回合格式→DrawText3(288,180,0,4,buffer,0)。真实栈20B局部含4B临时指针和16B buffer；不新越界/回绕模型、保护或补零。音效本线实际两参数，第三原240已证不消费，音频继续暂停。
14. 原四命令格式H5B/S11B/I8B/W8B，N2B/W|FF5B，显示%-16s6B/%4d4B/%2d4B/回合11B；三阶段提示各15B以及目标11B/取消19B都含首NUL，按证据保原字节/容量/实参，不改协议，不加解密。已过Flush/Close/Check/selector/FP桥不全面重核。

## R7与机器台账

X:00099062-000990A0全文62B：499062两字节8BFF只读decode为MOV EDI,EDI对齐；499064六项24B、49907C五项20B、499090四项16B全是本根三个W32 switch表，唯一实际入向为498583/498887/498D47。三表目标逐项在完整根内、低case0/default原目标保持；不是新函数或业务排除，mapped仅原字节用途。

899/922两旧记录保business_*、原边界/指令数/引用/格式与历史全文，更新当前status/source关系并追加五引用和完整子基线；1559新提示正常F。三equivalent/三成员，语义1559→1560/有效1552→1553、文档关联1670→1671/3680（45.41%）、分类687→690/BATTLE239→242枚举open/E792→794，R7 264→265/3061余2796。无F/有效区间各7、差队列172/缺定位171/43待归属及原完整门禁保持；五任务U08/R01/R03/R04/V04只本步当前和引用，60源码closed10/完整验收0/叶0/global=false。

<a id="battle-command-state-dispatch-sampling-acceptance"></a>
## 可度量验收

- 三完整逐指令连续字节/单chunk、988/3638B/14页、92CALL分型/24directcallee、137jump/三switch/四RET/三实际入向；62B X用途、全15个W32表项与实际入向、2B对齐decode。
- 三源前后体冻结；整UI仅两体18位置授权差，13+2实际W8、两cost-first桥、held W32谓词pub序；正确目标提示体和共享W32/正常S32、S16坐标target、数据/回调T*与原返回保持。
- state0..5/模式0..4/phase0..3/default、短路/取样/提示12B→点击→3B、原256B组包/16B倒计时显示/fresh socket/重复N和门禁/finish/MarkDeath清槽、图尾与所有失败/返回/写序静态对应；不据已核本根推全部生产和上游/owner生命周期。
- 一新F/两旧完整续记与历史业务保、三处置/242open成员、一R7/两局部I/S和五任务、完整派生快照/冻结定位/编码/引用/定向diff；measure/-Check仅文本汇总。

<a id="battle-command-state-dispatch-sampling-source-state"></a>
## 本步交付

先落SPEC与证据，修两体18位置、纳管正确提示，再同步台账/五任务/唯一进度/派生看板，精确提交并固定快照`cg2d-re-battle-command-state-dispatch-sampling-20261007`。此前BQT01的keyboard泛称纠正为本真实状态/鼠标根，旧证据和历史不改。

目前完成本根及两依赖的正常源码静态映射。43CBB0上游、全部技能/宠物数据生产、其它窗口/owner寿命、SDK/机器ABI/画面运行和R8继续推进；运行门禁仍受现行不构建测试运行摘要限制，音频暂停，不降低验收标准，不声明整体完成。
