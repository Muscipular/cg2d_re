# 战斗复制体、击倒、离场与抛物线合同 SPEC（BKE01）

2026-10-10；109–113五完整正常根。七BYTE消费、两HP零复制、三效果四值传送、一次事件持样、三转向Y/X取样、两运动参数读取与113两借用/入口读取共二十组，另十注释。其它正文/签名/数值/真实调用/RNG、完整W32、共享追加/复制及当前696B图号表保持，无新API、默认值、保护或循环上限。

<a id="battle-knockdown-exit-baseline"></a>
## 原根与已有身份

| 原身份 / 当前源 | 完整范围（尾不含） | 指令 / 字节 / CALL |
|---|---|---|
| F:0000E4B0 / UpdateBattleAction109_40E4B0 | 40E4B0–40E55F | 62 / 175B / 8 |
| F:0000E560 / UpdateBattleAction110_40E560 | 40E560–40E970 | 276 / 1040B / 24 |
| F:0000E970 / UpdateBattleAction111_40E970 | 40E970–40EAFB | 108 / 395B / 6 |
| F:0000EB00 / UpdateBattleAction112_40EB00 | 40EB00–40EFD4 | 365 / 1236B / 26 |
| F:0000EFE0 / UpdateBattleAction113_40EFE0 | 40EFE0–40F093 | 53 / 179B / 3 |

共864条3025B/十五页（每页至多64条）、67 E8/二十四唯一直接callee、73跳六RET，无间接CALL/JMP。五动作各唯一实际入向CALL/父404DB0；沿BIE01已证phase/实际40573C表基址，109–113实际表项4058F0起20B，各落五11B push→CALL→JMP40563F。111返回末PlayFrame S32，113原EAX含旧计数/标死指针但caller忽略，现void保持。

113已有BPS01语义1431及equivalent处置/BATTLE成员；本步仅补当前两读取组与来源，保历史结构及分类，不重复新身份。另四根补四独立语义/处置/成员。二十个已有关联callee身份复用，其余四个当前提供者仅按本步真实调用定位，不代独立语义或整依赖验收。

## 109：独立复制体与单效果

固定入口payload/caster1070→NewCastCopy(source)，原返回沿创建器边界使用，无新增空保护；固定copyPayload→image101031→真实设备查询后fresh完整copy.flags，按设备OR100000h或8000h→flash1004=1→position18=-1。为保存caster/copy依次Append25、110、107，再fresh源C9C/当前effect，原40E531先position4→40E534 flags→40E538 position3→40E53C position读齐后AppendEffect。随后Advance(copy)→Advance(source)，不推进效果列表或另做清理。当前只显式四值取样，现独立创建器/追加器和借用关系保持。

## 110：蓄力、接近与两侧HP零

固定payload，角色动作3/speed16F/rate290，面向fresh动态目标；持旧FF0为previous，count=previous+1发布，只有旧previous>=60且fresh1038==0才rate50→测fresh动态目标距离。距离>=70仅Move；较近则sourcePosition>=0才至少一轮do。每轮固定视觉target/targetPayload，Turn(target,source)后fresh完整flags写FE4，ApplyFlags之后fresh保存flags。

保存AL8只phase31；否则保存完整80000h置位才进入两侧命中。保存AH E0全清时RAND(0,2)+110001→RandomEffect(target)→Rebirth(source,target)，从RandomEffect返回的原effect写两scale=2F，无新增失败回滚/保护；声132/fresh sourceX→ConstructZero。原40E6D7先C9C、40E6DD先temp.encoded，再当前记录position/动态目标查找；encoded发布后fresh mask读/发布，fresh BYTE count读/发布→Destroy。随后fresh BYTE20才固定targetPayload FDC OR2→共享gate1→alive1。

第二侧条件是**fresh当前flags的CH20**，不复用前保存flags，且在完整80000h分支内：独立RAND/RandomEffect(source)→Rebirth(source,target)→返回effect两scale2F→声132→ConstructZero。原40E7AD sourcePosition先于40E7B0 temp.encoded，之后动态源对象查找/逐项encoded-mask-BYTEcount发布→Destroy。fresh BYTE20只设置入口源payload FDC/gate/alive，然后Advance(source)。这两HP临时复制展开，目标token/reserved及共享复制器保持。

每轮仍无条件Advance固定target，之后fresh完整当前flags bit19形成FD0=14*((bit)+1)，AdvanceEffect(source)严格1结束。较近路径即使sourcePosition<0也继续Move→FF0=0→1038=1→flash1004=0→char5→event0，真实循环PlayFrame(0)直到一次事件样本进入[10000,20000)，然后event0；不改成固定成功或添次数上限。

另一分支fresh1038：为0且新count<60才rate减4*count；非零则持旧FD0、先发布remaining-1，旧remaining非0才置局部suppressPlayback、speed2/rate100/Move，旧0仍产生-1而不移动。共同末fresh1038非0且FD0==0才speed8/rate100/char3→FF0/1038/FD0清零→Advance(source)；否则仅未抑制时PlayFrame(0)。五BYTE消费和两HP读序之外的正确正文/完整高位、旧/新计数与条件RNG全部保持。

## 111：阈值散射、反向离场与结束震动

char8/rate100，先持完整FDC后FCC+1发布，由保存FDC bit1生成14/28阈值。未达到时speed=-2F，fresh AL2置gate1/alive1。恰好达到时声207后再次fresh AL2，若置位先Count22活动对象<=1才gate0，再fresh完整FE0取样→alive0→FE0 OR2发布，顺序保持；两个AL消费明确为BYTE，不把FDC/FE0字段缩窄。

然后40次Scatter(source,110004)，清FF0/FD0，角度+180的未落F32中间值比较360，先speed32/F32角度发布，>=360仅一次减360。达到及后续离场帧按四次独立原F32读取检查X[-200,840]/Y[-200,680]；超过边界则Advance(source)→入口FD0=0→震动振幅6/启用1。所有路径最后Move→PlayFrame(0)S32返回。40次真实创建/RNG、角度单次归一化和原负值/边界业务处理保持，不专项模拟异常。

## 112：双复制体、名称表选图与运动参数

固定payload/caster，在PlayFrame(1)严格1时Advance(source)；之后fresh image按原四闭区间选char7，否则char5；fresh level<90才面向fresh当前目标。转向之后原40EBAE一次读取事件，两比较复用样本，本步恢复一次持样；事件不在[10000,20000)真实return。

先NewCastCopy(source)得first，image98003/char0/direction0→PlayFrame(first,0)，再独立NewCastCopy(source)得copy。固定copyPayload/image26220，沿36B命名记录原byte比较到name相等；再沿57E174三S32表第一列key匹配该记录+20h的value，命中选**第三列**图号，第一列0终止/未中保默认图。与85用第二列不同；复用BRAF01已证全58行696B/174值和当前表，命名输入/生产仍待，不增加搜索cap、默认记录或强制命中。

原inline strcmp/strlen+REP字节复制沿现std::strcmp/std::strcpy，当前原名按字节/NUL借用复制，不解码、不恢复Big5文本。copy char0/direction0→PlayFrame(copy,0)→借用first写原+10，再fresh源level复制及第二次fresh level==90门禁。90按fresh position>9置目标188/203，否则449/336，再向源转向时**先Y后X**；其它level分别fresh当前目标取X正常S32转换/FE8发布，重新取当前目标Y转换/FEC发布，不统一为一次目标快照。

copy.Y先加source graphicOffsetY的S32向零除2，再X转换、Y样本、X发布、Y转换/发布。对copy和first分别转向：每次独立先目标Y后X，前次回调后再重取下一次字段；三处Y/X读取组本步显式保存，数值与参数保持。

运动参数原40EE1D先copy屏幕X，之后294=60发布→目标X差绝对值→角度样本→乘原F32 3C888889h落F32 xStep→Cos。Cos后先copy屏幕Y再value54发布；随后目标Y差绝对值→角度样本→fresh294读作分母→F32 yStep→Sin。Sin后再次fresh294→copy.Y样本→field58=100→加live value54→worldY发布→speed发布，294/2的S32平方用于100F32除后worldX发布。两运动读取组恢复这些样本次序；现double数学/标准abs、三真实转向、四__ftol宿主正常转换及F32落值保持，不加域限制或异常保护。

严格Append113；之后fresh level==90才Append85、36，再Append114、107。fresh level<90只当前一effect：原40EF3A **flags先position4**，之后position3/position；其它level至少一轮do：原40EF68 **position4先flags**，之后position3/position，AdvanceEffect(source)严格1才退出。两处显式四值，不能套成同顺序或整体提前取样。末copyPayload.C9C=0→Advance(copy)→source.frameEvent0，fresh当前**下一条**position>=0且fresh level<90才Advance(source)；不补源游标复位或推进first。

## 113：入口Y与两轴父引用

原40EFE7先worldY取样、40EFED再固定payload、40EFF0发布offset44后Move。Move后原40EFF8先parent+10，再40EFFB ownX、40F001 parent.X发布；40F004再次fresh parent，再graphicOffsetY/ownY读算写parent.Y。当前赋值表达式不能代这两次独立父取样，本步将入口和双轴两组显式展开，不跨Move/轴发布缓存同一parent。

之后fresh ownY→worldY回存，294按S32保持一次样本；FF0-duration/2的正常S32平方乘worldX、减field58、加live ownY落F32。再次fresh旧FF0先比较旧duration，再FF0+1发布；旧值>=duration才FE8/FEC转F32定位→Advance(source)→fresh parent MarkDeath→入口FF0=0。原混合EAX未被唯一caller消费，void/借用union槽及所有数学/尾调用保持；不称原正常无介入值已发现运行故障。

<a id="battle-knockdown-exit-decision"></a>
## 实施与登记裁决

依第8条先SPEC后落实：一个CPP二十组/五定义注释、公头五注释；旧shared_battle对应109/112原“仅取证”状态及当前来源、BPS01当前113状态追加限定说明。五root引用现有结构和真实提供者，无新业务/接口/owner。WAV-only/32位色与MSVC库边界、RPLAN.24保持，BPST01四块未知载荷区域不猜。

仅新增109–112四独立语义/adapted处置/已知BATTLE成员。1431/113的旧BPS01记录/正常equivalent类别保留，追加当前来源和两读取组状态，不重写旧完整evidence.json或再计新F。两限定I/S覆盖五根当前改动和正常源，枚举false；U08/U09/R01/R04/V04追加来源仍open，全部原政策/门禁/六closure保持。归属取证不代所有callee、输入、创建失败后正常寿命或机器运行。

<a id="battle-knockdown-exit-acceptance"></a>
## 必要静态与剩余

核五完整864条3025B/十五页/库存、67E8/24直接callee、73跳六RET、五实际入向/一父/五11Bcase与实际20B表项；28B七F32常量及复用696B表全174值。核二十组/七BYTE、两HP零的不同索引或源位置/encoded/动态查找及逐项pub、三Append四值差异、112一次事件/三Y-X与两运动读取、113入口和双轴独立父引用。

核全CPP/公头精确重放二十组/十注释，数值/调用/所有其它源与共享器/图号表、旧BPS113原证/结构状态和二十callee语义保持；四新身份/成员及1431当前追加来源、两限定I/S和五open任务，UTF8无BOM/LF、差异及文本汇总/-Check。完整名称/等级/当前与下一effect/HP/复制体输入生产、全父-callee/容量/alias/owner/SDK正常失败与声音画面运行仍待；不构建/测试/运行/算法/产物/摘要或引依赖，总SPEC§10不降低。

<a id="battle-knockdown-exit-source-state"></a>
## 当前状态

五完整原根/实际表项/28B常量及复用696B表已取齐，二十组/十注释落实；必要静态、文本汇总/-Check与派生精确范围通过。语义2167（2119F＋48X）、合同2229/3680=60.57%仅关联率、F1595、E1405限定I/S五F、BATTLE407且枚举false；四新身份，113原1431/equivalent与历史结构保持仅补当前来源，五任务open/原政策门禁保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-knockdown-exit-history-20261010，提交引用cg2d-re-battle-knockdown-exit-20261010；整体目标active/未完成。
