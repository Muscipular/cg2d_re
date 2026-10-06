# 战斗转段、反弹与反伤取样SPEC（BQD01）

日期：2026-10-07。按工程委托先记裁决再实施，聚焦233/234/236/237基线与实际读取缺口，不复检已过232或骑乘状态。

<a id="battle-bounce-reflection-sampling-baseline"></a>
## 原端完整基线与具体差异

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret | 源调整 |
| --- | --- | --- | --- | --- |
| 0x00416860 | UpdateBattleAction233_416860 | 168/577/3 | 10/18/5 | 3阶段 |
| 0x00416AB0 | UpdateBattleAction234_416AB0 | 223/756/3 | 10/26/1 | 3阶段 |
| 0x00406630 | UpdateBattleAction236_406630 | 9/26/1 | 1/0/1 | 0阶段 |
| 0x00416DD0 | UpdateBattleAction237_416DD0 | 434/1588/6 | 41/42/1 | 5阶段 |

四完整根834条2947B/十三页、62原call/26callee、86普通内部jump/八ret，无switch；四唯一404DB0入向12邻窗合35B。233/234/236/237已有真实定义，冻结定位有而明确语义/文档/处置空，本步纳管。

| 差异 | 原证据 | 裁决 |
| --- | --- | --- |
| 233初始FDC/FCC | 416898/4168A0 | 两字段先取，再bit/计时计算 |
| 233两个实际低W8 | 4168F1、41697A | 转角后bit2、中段阈值退活bit2分别取当前低字节 |
| 234两个实际低W8 | 416AFB、416B55 | 声207后bit2、四十Scatter后FCC奇偶分别重取低字节 |
| 234下边界反射 | 416D4E/416D51/416D53 | 角度先读倍增，再真实重读角度/减/发布 |
| 237三个数显 | 41711E、417211、417322 | index先Y、同index.amount后、X最后 |
| 237反伤HP归零 | 4171EA/1EE/1F2→4171F6/200/206 | encoded/mask/count三齐读后写source，局部展开AssignFromInt |
| 237普通扣减归零 | 4172E8/2EE/2F5/2F9 | index→encoded→position→共享目标，encoded发布后两字段交替；顺序与232不同 |

233初始和阈值FDC及FE4均W32，后AL/AH/BL是寄存器消费，保持；四新读取仅真实内存W8。234原DX宽度+04是MOVSX S16，既有ReadGraphicDxWidth返回int16已正确，LoadGraphic后graphicIndex重读和共享缓存保持，不造新原身份/call。十原F32常数按[原字节](./cg2d_battle_bounce_reflection_sampling_evidence.json)冻结；原业务边界、同步帧循环保。

<a id="battle-bounce-reflection-sampling-decision"></a>
## 工程裁决与完整正常合同

依既有委托决定：只十一读取/载荷阶段，四读宽度各当前独立读取；归零两路径按本身原序展开，普通CopyPayload/AssignFromInt共享helper与结构/公开头不改。236正确正文和已证父弃返边界保，不声明原返回ABI。正常旧值一致，不称并发/运行数值错误；无新增空检查/钳制/迭代限制/异常专项模型。

1. 233：固定payload，characterAction8后fresh FE4 W32判断C04→character9；frameRate50，先初始FDC W32、FCC S32取样，再bit1与++count发布。count<14*(initialBit+1)：angle F32+原F32 180、speed5、比较原F32 360且按F32发布/必要减360，未落位中间保；此后fresh FDC低W8 bit2才gate1/alive1，PlayFrame(action,1)返回。中段count<14*initialBit+34：direction S32++/原>=8减8，fresh FDC W32 bit1求阈值后fresh FCC比较；恰阈speed5，fresh FDC低W8 bit2才CountActive<=1清gate并alive0，FF0=0。FF0==0创建110006，调用后fresh previous计时++/previous>=2清0，PlayFrame1返回。末段count>=14*initialBit+54清FCC，初始缓存FDC寄存器bit2清时speed6/frameRate100→439340，fresh phase11才speed4/frameRate100；bit2置时fresh FE4 W32&3010非0同6/100，否则1/250→439340。其它中间区间只共同PlayFrame1返回，原五ret/十物理call点与正常业务环转保。初始和阈值FDC W32、FE4 W32及后续AL/AH/BL寄存器测试不改成内存W8。

2. 234：固定payload，characterAction8/frameRate100，FCC S32++发布，原count==1声207(x)后fresh FDC低W8 bit2，CountActive<=1才清gate，先stateFE0 W32后alive0/state|2发布。连续40次Scatter(action,110004)，调用后FF0/FD0=0/speed15，fresh FCC低W8奇偶偶数才原F32 angle+180发布；之后另fresh角比较原360/减360。共同Move→fresh direction++/>=8减8。FF0>=3时分别fresh X<-200/X>840/Y<-200/Y>680才439340后FD0=0；这些是真实业务边界，不新增保护。 234碰撞与帧尾：FF0<3才LoadGraphic(fresh graphicIndex)弃返，调用后再fresh graphicIndex→既有共享DX缓存的+04 S16宽度，ReadGraphicDxWidth已是int16返回，无新原F/call；halfWidth=原F32 0.5×S16，normal数学/未落位边界保。按原排他优先级：halfWidth>X且FD0!=1左边界，X>640-halfWidth且FD0!=2右边界，-graphicOffsetY S32>Y且FD0!=3上边界，Y>480且FD0!=4下边界。左/右/上用delta=(freshAngle-90/270/180)，double delta后另freshAngle反射F32发布；下边界原416D4E读取angle、倍增、416D53再次读angle再减/发布，必须两读而非cached复用。每命中FF0++→声208(x)→FD0=1/2/3/4，原声两共享物理点，不虚造四原call。末尾PlayFrame(action,1)至少一次、返回0同步继续至非0才返回；保原真实循环/缓存寿命及正常角度/边界，不增迭代限制。十实际原F32常数/原8位资源和SDL获批边界保持，完整图形生命周期/机器x87未验。

3. 236：固定payload先借，source.phase0→payload.busyFD8=0→真实call406650(action)，唯一404DB0父弃用返回。源void仅承接该已证弃返消费，不声明原EAX/机器ABI等价；现正确正文不改，不把这九指令外推为完整状态0/递归父验收。

4. 237入口/预效果：固定payload，characterAction3/speed10/frameRate100；initial FF0<16才Move，调用后FF0再<16才fresh current.position/共享目标Distance<20，否则直接处理。近距或计时已达16才fresh同index.position先借共享目标T*、flags W32后、targetPayload后缓存；Turn(target,action,1,0)→targetSkill=-1，fresh ownPosition/共享attacker写target+10，cached flags发布target FE4，fresh ownPosition ApplyFlags(position,action)。cached bit8 target.phase31；否则100/flash0声210(x)→flash1/phase31，flash非0只共同尾。其它C000清且fresh current.amount!=0：target x10/y14 S32→offsets F32；2000反伤bit2给source110008，RAND(0,2)+110001给target图→Rebirth(action,target)后source FE4=2000，cached bit20才sourceFDC|2/gate1/alive1（无232的skill404/406条件）；非2000路径100000回收target.effect1054，调用后目标FDC先读、槽null后FDC&~20，再bit2给target110008，另RAND(0,2)+110001和Rebirth，bit20激活target。所有flags初始W32快照及BL/BH寄存器消费保持，不能照相似体新增条件。 237恢复/反伤/扣减：8000只439340(target)；4000先439340(target)，10000000 fresh当前共享目标图22459；否则amount先position/共享目标后AddHP，fresh共享目标GreaterHP/max，超过原上限再fresh目标普通CopyPayload encoded/mask/count交替；恢复数显index→cached targetY→sameindex.amount→targetX/type5/style2/cached flags，bit2 target图22454→Attribute(action,target)。2000分支先439340(action)再439340(target)，10000000当前图22459；否则fresh amount扣source HP，<0 Construct0后encoded W32/mask W32/count W8三齐读，再按同序写source→Destroy；反伤数显index→sourceY→amount→sourceX/type6/style2/flags，bit2 source图22454，Attribute仍(action,target)，41724A/41723A跳原共享417153/41715B物理点，不新call。普通扣减10000000当前图22459；否则fresh current.amount=0给当前目标22331，非0先amount后position/共享目标扣HP，fresh共享目标Less0才Construct0消费RNG。原4172E8读index后4172EE先读temporary.encoded，再4172F5 position/4172F9共享目标，encoded发布后mask/count交替，再Destroy；顺序与232不同。普通数显index→cached targetY→amount→targetX/type6/style2/flags，bit2 target22454→Attribute(action,target)，最终439340(target)。HP/图标/Attribute接收者、独立RNG/普通上限复制/共享调用尾保持。 237共同尾：处理分支cached bit8清且fresh current.amount!=0才按cached targetPayload当前FDC W32的bit1求14*(bit+1)写source FD0。cached bit2000清且fresh source FF0<16才phase5；随后439470(action)弃返。共同尾fresh previousFF0后++发布，previous>=16才439340(action)→FF0=0，最后PlayFrame(action,0)返回。未近路径同样进入共同计时/帧尾，保原两次FF0阈值读/处理前后调用消费、状态与别名，不新增RNG或数学异常/回绕模型。

四完整原根834条2947B/十三页、62call/26入口/86普通内部jump/八ret，四404DB0唯一入向12邻窗/35B和十实际F32常数。只233三阶段/234三阶段/237五阶段共十一源码片段：初始FDC/FCC先取后算、四实际内存低W8、下边界两次角度读、三数显index先Y、两归零各自原序。正常旧值一致，不称数值/并发或机器故障；236正确体/父弃返边界、S16宽度共享getter/原业务边界、原W32寄存器旗位/HP图标接收者/独立RNG/普通上限CopyPayload、真实同步帧循环/原调用尾和返回保持。只当前完整正常源接线静态，不外推递归父/全部子依赖/其它生产与owner寿命、SDK/生成机器ABI/实际x87/画面运行；音频暂停，RPLAN.24不专项异常/保护/回绕/故障等价保。

<a id="battle-bounce-reflection-sampling-acceptance"></a>
## 可度量的限定验收

- 四根834条/2947B、十三页、62call/26callee、86jump/八ret，四父12邻窗/35B、十F32常数及十一源码片段逐项可追。
- 整YobiRuntime只233三片段/234三片段/237五片段，236及其它源保持；四真实W8、两角度读取、三数显索引、反伤encoded/mask/count齐读和普通扣减encoded先position/目标分别落实。普通helper、S16width getter/公开头及原业务/计时/HP图标接收者、W32寄存器旗位/独立RNG/真实同步帧循环/共享尾/返回保。
- 新增1481..1484四原已有正常定义/equivalent，四冻结定位全文保持。语义1481→1485/有效1476→1480、关联1594→1598/3680（43.42%）、BATTLE158→162枚举open、分类606→610/E758→760。仅两局部I/S、每项十四独立实际来源。
- R7 260/3061/余2801、差队列172/待归属43及其它全部机器对象/完整门禁保持；U08/R01/R04/V04只当前描述/引用仍open，原60任务关闭条件保。完整战斗/生产与owner寿命/图形生命周期/机器ABI/x87/运行及R8仍open。

<a id="battle-bounce-reflection-sampling-source-state"></a>
## 当前交付

233/234/237十一阶段实修，233/234/236/237完整正常基线纳管；快照 `cg2d-re-battle-bounce-reflection-sampling-20261007`。定向静态及measure/-Check记录唯一进度；未编译/调试/测试/运行/摘要，音频暂停、网络原格式明文及原验收标准保持。
