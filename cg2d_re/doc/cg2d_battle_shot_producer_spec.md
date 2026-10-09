# 阶段18/19射击生产与攻击事件探测 SPEC（BSP01）

2026-10-10；沿动作15及命中特效的真实依赖补4065E0、408170、4082F0三完整原根。只恢复两个射击父的单次事件门禁和音效事件→X取样，探测循环、技能/等级分支、播放与队列推进保持；不判所有父-callee或资源owner已验。

<a id="battle-shot-baseline"></a>
## 原根、分派与返回消费

| 原身份 / 当前源 | 连续完整根 | 调用与分支 |
|---|---|---|
| F:000065E0 / ProbeBattleAttackEvent_4065E0 | 4065E0–40662A，21条74B/一页 | 一directCALL/一callee、五跳一RET；15/19两个真实caller消费S32零判定 |
| F:00008170 / UpdateBattleAction18_408170 | 408170–4082E1，98条369B/两页 | 八directCALL/七callee、16跳一RET；404E82为case18唯一CALL |
| F:000082F0 / UpdateBattleAction19_4082F0 | 4082F0–40840D，77条285B/两页 | 八directCALL/七callee、九跳一RET；404E8D为case19唯一CALL |

合196条728B/五页、17个directCALL，无间接调用/分派表。18/19的原case窗口各三条11B，调用后跳共尾，不消费本根EAX；现总分派case18/19和void签名保持。探测在406E3B和408318后test EAX，仅零时将characterAction改5，两个现调用正确，不以AL或bool缩窄其S32返回。

## 探测循环与共同退出

反复PlayFrame(action,1)，严格返回1时result=0并退出，不再读取该次frameEvent；否则取一次S32事件，10000<=event<20000时result=1，其他值继续同一原循环。共同退出先lastCharacterAction=-1，再frameEvent=0，返回保存result。现正文正确保持；不加次数上限、超时、空对象门禁、固定成功、播放缓存或额外队列推进。

## 阶段18：射击与258命中特效

入口固定payload；先characterAction=5，再取一次castSkill决定95/3019速率33、其余100。按当时effectRunning取position；-1时PlayFrame(action,1)==1才AdvanceCast，其余先真实共享目标TurnToward(action,target,1,0F)再PlayFrame(action,0)，弃播放返回。没有额外Move或父跟随。

播放/可选推进后fresh取一次frameEvent用于两个门禁比较，未进入[10000,20000)就返回，且不清事件。进入后fresh检查skill258；仅该技能取一次S32 level，<9用29105、9..11用29106、>=12用104217，当前持样level正确保持。再按fresh effectRunning/position取共享目标，HitEffect(target,0,0,30,image)弃返；其他技能不创建这项特效。

随后按fresh effectRunning取真实效果地址，RangeHit(action,effect,22050)弃返，创建失败也继续音效与队列；音效fresh取frameEvent，再取screenX，计算event-10000并PlaySe(soundNo,X)。原额外压栈Y不被52C450本体读取，当前两参合同保持，不增加第三参或无消费者读。AdvanceEffect(action)弃返之后fresh skill，仅95/3019且fresh当前position非-1才依次frameRemain=0、frameIndex=0；最后frameEvent=0。入口技能/位置不得跨播放、创建或队列推进复用。

## 阶段19：探测后的范围射击

入口固定payload；FD0==0才先FD0=1、characterAction=7，执行上述探测；result==0才characterAction=5。无论初始门禁均继续framePlayingRate=33；position=-1/非-1的播放/推进/转向合同与18相同，但没有入口95/3019分速或258 HitEffect。

同样只取一次fresh事件用于[10000,20000)门禁；进入后RangeHit(action,fresh effect,22090)弃返，随后fresh soundEvent→soundX→event-10000→PlaySe→AdvanceEffect，全部实际返回消费保持。推进后只skill95且fresh position非-1才依次清frameRemain/frameIndex，3019没有此重播分支；最后清frameEvent。原名含RandomShot，但本根没有RAND调用，不新增随机数或套用阶段18技能差异。

<a id="battle-shot-decision"></a>
## 实施边界

一个CPP在18/19各改两段：gateEvent持样用于两次比较，soundEvent和soundX按原顺序取齐后传真实音效。CPP三条、公头三条合同注释；探测完整正文、两个射击其余正文/签名、总分派与动作15/命中特效/RangeHit/音效全部保持。无新函数/类型/API/字段/保护、输入限制、RNG或库算法重建。

新增三独立语义/F adapted/三个BATTLE已知成员，枚举false；限定I仅18/19四段，S覆盖三个完整现源。U08/U09/R01/R04/V04只补来源和当前局部描述，原分母/六closure/完整门禁保持。依已获工程委托先SPEC后实施；WAV-only、原格式明文、MSVC排序未决边界、RPLAN.24及double/标准数学许可不扩。

<a id="battle-shot-acceptance"></a>
## 必要静态核对

核三根连续区间/库存/196条728B/五页、17直接CALL/各callee-跳-RET、两个case窗口与两个探测返回消费窗；单次事件、播放严格==1、固定payload/fresh索引与共享目标、原两范围图号/三258图号/level持样、95/3019与仅95的差别、推进后重播清零序及共同事件清零有据。整CPP仅四段/三个注释，公头三个注释，其余全部源守恒；依赖复用不重计完整原根或全owner。核三语义/处置/成员、限定I/S/五任务与政策/分母/门禁、UTF-8无BOM/LF和静态汇总/-Check；不执行循环/射击/排序或数值样例。

所有输入/完整父-callee、原未知间接alias、资源借用与最终owner、SDK正常失败/容量/重进退出/数值、声音画面和实际运行仍待；不以静态核对降低总SPEC§10。

<a id="battle-shot-source-state"></a>
## 当前状态

三完整根与四段差异已实施并完成必要静态核对，静态汇总与-Check通过；前态cg2d-re-battle-shot-producer-history-20261010，提交引用cg2d-re-battle-shot-producer-20261010。语义2054（2006F＋48X）、合同2116/3680（57.50%仅合同关联率）、F处置1481、软件证据1370，BATTLE已知成员294且枚举false；全部完整门禁保持，未编译/测试/运行/查产物/计算摘要或引依赖，整体SPEC未完成。
