# 战斗克隆/图像还原取样SPEC（BPZ01）

日期：2026-10-07。按工程委托先记裁决再实施，上承战斗收尾、字段/别名与追踪验收体系。

<a id="battle-clone-restore-sampling-baseline"></a>
## 原端完整基线与差异

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret |
| --- | --- | --- | --- |
| 0x00413550 | UpdateBattleAction218_413550 | 134/507/2 | 9/14/3 |
| 0x00413750 | UpdateBattleAction219_413750 | 147/553/2 | 13/15/3 |
| 0x00413980 | UpdateBattleAction220_413980 | 116/448/2 | 9/13/2 |

三根合397条1508B/六页、31call/八入口/42普通内部jump/八ret，无switch；三唯一404DB0入向九条实参-弃返邻窗。RestoreBattleBaseImage仅现有源帮助体，219/220两实际使用者覆盖两完整原内联阶段，不将helper当原callee/新F。

218克隆初次图发布：4135B9先currentIndex，4135BF先1060指针，4135C9后position4，4135CD写该指针image；源复合赋值在字段值后取目标指针，恢复显式借用指针/值顺序，后续原每操作fresh槽保。共同Restore创建阶段219 413777先1060指针/41377D后13AC基础图号/413786发布；220 4139A7/39AD/39B6同序，两个完整父都证明此源阶段。源RHS先图号/LHS后指针，恢复先指针后值，再发布。

219结束还原后的413905是真实TEST BYTE FDC低0C，源W32表达式恢复unsigned byte对象表示；字段仍W32、所有写者及stateFE0 W32不改。旧正常指针/位掩码结果相同，不称正常数值错误或机器验证通过。

<a id="battle-clone-restore-sampling-decision"></a>
## 实施裁决与正常完整合同

只三个源阶段：218创建分支具名imageCopy先于position4；共享Restore创建分支具名imageCopy先于baseImage；219尾FDC低W8。220包装与其余源码不改，保原两次同步终帧循环和槽空检查，不限迭代/不重查图号；按源帮助阶段完成bool接线，不外推原寄存器返回ABI。

1. 218：fresh current.position W32==-1时characterAction0否则6；1060为空才fresh current.position!= -1且PlayFrame1不等1就返回，否则Clone(action)借用T*写1060、返回指针callback清0，先fresh currentIndex/1060指针、后该index.position4读并写克隆image；随后每操作fresh1060写character0/alpha W8=0→PlayFrame0弃返立即返回。非空1060先RandomEffect(action,110471,0)弃返、Has3D0时主体flags8000和fresh副体flags10000置位；主体308=8→fresh副体308=8→fresh1060指针读alpha+7，未达255时主体alpha-7正常低于等0归0、随后fresh1060指针alpha低W8+7发布返回。达255时MarkDeath该缓存指针，fresh currentIndex先取后1060清空，同cached记录.position!= -1才fresh FDC W32先捕/13B0=1，flags10000000清则旧主体image保存13AC后读position4换图；-1分支13B0=0/同flag清则直接position4换图。共同character0/3080/alpha255/303255→PlayFrame0→Has3D0清主体8000→frameEvent0→439340。源与副体别名时按原重取/发布序，不加创建失败保护或提前推进。

2. 219：源RestoreBattleBaseImage承接413750..413905原内联阶段；1060为空时Clone/action返回指针写槽/该指针callback清0，fresh1060指针先捕后基础13AC图号W32读/写该副体，随后fresh1060 character10/alpha0，再一次fresh1060与其image>999才PlayFrame1；不等1时原同步循环只fresh1060继续PlayFrame1直到等1，随后原确实fresh检查1060为空则返回。非空或同步完成则RandomEffect110471/Has3D和双3088，同218透明度每次-7/+7未到255返回false承接原早退；到255 MarkDeath缓存副体后先捕13AC图号再清1060、主体image还原/character10/3080/alpha255/303255；cached基础图号>999才PlayFrame1，不等1同步推进本体到终帧，不新查图号/不设循环限。之后13B0和frameEvent清0、Has3D0清主体8000，bool true只源内联阶段完成标识而非原callee/返回ABI。219后续实际FDC低W8/0C清时一次借1058非空MarkDeath/条件清槽→stateFE0 W32|2→phase48；有0C时CenteredEffect110169→声215(x)（原第三Y未消费，音频暂停）→fresh stateFE0 W32|2→439340。保原循环、失败/早返、业务下限、借用别名及状态顺序。

3. 220：413980..413B35原完整还原内联与219帮助体对应，初次Clone后1060指针4139A7先取、13AC图号4139AD后读，字段写4139B6；后续fresh1060/character10/alpha0、image>999同步终帧、原槽空早退和每次-7/+7、回收先13AC后清1060/恢复主体字段与同步终帧、13B0/frameEvent清0/Has3D清8000全部同源帮助体。Restore源bool true后只phase214，false不推进；原无Centered/sound/439340尾。两实际源调用者均完整捕获并纳管，共同源helper不新造原函数F或跨模块owner。220包装本体正确不改。

三完整原根397条1508B/六页、31call/八入口/42普通内部jump/八ret、三唯一父404DB0九邻窗与219/220原内联帮助阶段。实际源只有218一指针取样、219一低W8条件及共同Restore一指针取样三阶段；220包装保，两真实帮助体使用者全部对应完整原根，helper不新造原call/F/owner。正常旧值一致，不称数值/并发或机器故障；1060/1058借用、每操作fresh/条件回收、同对象别名、同步终帧循环及原业务alpha下限/宽度/返回/状态保。只当前完整正常源接线静态，不外推递归父/全部子依赖和其它生产/owner寿命、SDK/生成机器ABI/画面运行；音频暂停，RPLAN.24不专项异常/保护/回绕/故障等价保。

<a id="battle-clone-restore-sampling-acceptance"></a>
## 限定验收

- 三完整原根、实际字节连续/边界、31实际call/八入口/42内部jump/八ret、三父九邻窗及两个完整Restore内联阶段可追。
- 整个YobiRuntime只有218/219及共享Restore三个片段，220包装及其它所有源保持；两个原先指针后值的借用阶段/一个真实W8、原同步循环/每操作重取与条件回收/字段宽度和alpha业务下限/状态一致。helper两实际使用者完整纳管且不增原F。
- 新增1469..1471三原已有定义正常语义/equivalent：语义1469→1472/有效1464→1467、关联1582→1585/3680（43.07%）、BATTLE146→149枚举open、分类594→597、E750→752仅两局部I/S，每项十独立实来源。R7 260/3061/余2801、差队列172/待归属43与其它全部完整门禁保持。
- U08/R01/R04/V04只当前/引用仍open，原60任务关闭条件不改，完整战斗/生命周期/运行及R8未关闭。

<a id="battle-clone-restore-sampling-source-state"></a>
## 当前交付

218/219/Restore三个读取阶段恢复，三完整克隆/还原状态及帮助体所有实际父纳管；快照 `cg2d-re-battle-clone-restore-sampling-20261007`。未编译/调试/测试/运行/摘要，音频暂停、原格式明文和原验收标准保持。
