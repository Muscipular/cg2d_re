# 动作16投射命中与退场 SPEC（BPHIT01）

2026-10-10；沿真实动作分发→投射命中→回调/标死/退场。现完整本体已存在，本步核完整原根、补独立身份及注释关联，不改业务算法或注册新路由。

2026-10-11逐项验收补充：[BMSA03](cg2d_battle_m_skill_item_acceptance.md#battle-m-skill-item-acceptance-projectile-byte)发现40731D/407AD8/407D79/407FED/408083五条直接BYTE测试，旧源码先读完整W32再掩码；现明确恢复五个低字节读取，保fresh游标/数值显示之后取样及全部其它分支。下文“本体/字段宽度保持、只改注释”指当时有界结果，由本节修正这五处遗漏；完整owner、资源、生成机器访问和运行仍待。

<a id="battle-projectile-hit-baseline"></a>
## 原基线与实际调用

原F:000072E0、4072E0–408169，一连续根1026条3721B，经当前已打开cg2d.exe会话17次分页只读取得。71个普通CALL/24目标、182跳/两个RET，零寄存器间接调用；[完整指令/字节、原常量与现源](cg2d_battle_projectile_hit_evidence.json)保留每条实读依据，没有摘要/算法运行。

唯一直接入向404E77属于404DB0的case16：压入一个action指针，调用后转共享尾部；现分发同case16调用UpdateBattleAction16_4072E0并依既有void裁决弃结果。父完整257槽/尾合同沿[BTS01](cg2d_battle_tail_sampling_spec.md#battle-tail-sampling-source-state)，本步邻窗仅三条11B，不重算父完成或推所有creator/清理已闭。公开参数为原生非拥有YobiAction*，载荷借用同BattleActorPayload；200项16B效果记录及全部非地址字段宽度保持，不按原偏移解引用x64载荷。

## 完整正常流程

| 阶段 | 已证原合同与现实现 |
|---|---|
| 入口 | 原action为空、payload为空、首effectRunning<0或>=200均直接共同尾退出；这些是原检查，不能误删或再加其它保护。首次flags bit8未置才转向，之后重新读取效果索引。 |
| 提前命中 | bit20000000、距离<70、frameEvent==0才重取目标，空目标退出；持有目标payload后MarkDeath(action)、phase17。mode2特效y减103；mode4保存F32 x/y/speed，临时speed20、移动/创建/整数坐标后依次恢复；其它mode特效y减68。创建空结果保原分支，随后目标特效及原双推进/清FCC、1024，直接RET。 |
| 普通命中 | 距离<20且frameEvent==0才重取目标；原目标为空退出。原4/800/400优先图号门禁、技能/技术级别/同阵营图号和direction保持，不增加默认图或合并随机消费。 |
| 随机图号 | 原95/3019/4002/3038四特定分支、259及普通fallback共六物理RAND点；新端四特定分支归同fallback表达式，仍每个路径仅RAND(0,2)一次加110001。75–80/2096–2101等已证固定图路径不调用它；不按六物理点与两源码表达式差认定漏调用或执行随机样例。 |
| 命中发布 | 先目标转向、battleOpponent、fresh效果flags→target effectFlags，再ApplyBattleEffectFlags；技术>=9且技能259才创建铺地特效，调用后fresh索引/flags进入原分类。 |
| bit8/100 | 特殊技能95/4002/3038按目标phase决定额外推进和两清字段，保共同推进；mode4与普通分支不同。bit100普通路径先flash1004=1、音效210、phase31，不调换发布/播放次序。 |
| HP扣减/复活 | 7E000未置且amount非零时按原顺序创建图/次图、复活效果、选音效141/142/255；amount先取，再重取目标扣CryptInt，后独立比较。负值构造零临时、fresh目标复制三个载荷字段再析构；bit20只沿原flagsFDC→59E570→aliveFF8次序发布，不新造owner或少消费RNG。 |
| 状态分类 | 48000/24000/12000优先保持，特殊技能/模式4清字段与减益创建的时点、重复推进不删。24000可10000000转图标，或加HP/比较上限、三字段复制/数字与属性；其余普通损伤数字type6与恢复type5保持。 |
| 附属清理/击退 | 普通尾100000先MarkDeath(targetPayload.effect1054)，后采flagsFDC、清同一T*槽、再清bit20；不提前free或造影子槽。特殊技能角度两次各加180/必要减360、speed8移动及推进保留；当前double/F32实现不改，F32 180/360原样本有据，不宣x87/CW或生成ABI已等价。 |
| 末提示/延迟 | 原259/3029独立减益门禁保；flags bit8未置且amount非零才按目标flags形成14/28延迟，267/3029覆盖2，随后frameEvent=1。 |
| 播放/退出 | delay>0先减后按两图号可播放一次；fresh delay==0再播放/移动，frameEvent==1时按flags108判断是否MarkDeath(action)，随后phase17。合法双播放、只标死不即时释放及负延迟路径不删改。 |

<a id="battle-projectile-hit-decision"></a>
## 本步决定与边界

1. 完整现函数正文、分发case16、声明签名、数据/图号、RNG/音频调用和callee源保持，不制造功能修复。仅补CPP字段effect1054的4072E0清理关联、函数原范围/合同注释和公开声明的原入口门禁说明；这些是注释，不记实现I。
2. 200项效果记录的索引/目标、amount/flags、CryptInt读写、F32坐标、S32阶段/计数与所有fresh读取/持样阶段保持。只已证指针按x64承载，不扩W32、不加目标payload保护/钳制/拒绝或省略原重复回调。
3. 新增一个独立原F语义/适配处置与BATTLE已知成员，仅一个限定S，枚举false、零新I/整功能通过；U08/U09/R01/R04/V04只追加进展/来源，全部门禁和六closure保持。
4. 71个局部调用不等于24callee所有消费者/SDK全体验收；数据生产、每个创建结果/失败、目标/附属借用和最终Yobi回收寿命及全战斗仍按各合同继续。原机器数值/生成ABI、实际声音/画面与运行未验，不能据完整本体源码关闭U08/U09或总SPEC§10。
5. RPLAN.24不专项溢出/越界/回绕/故障等价；CRT排序仍待既有选择，WAV-only不恢复MIDI。不构建、测试、运行客户端/算法、检查产物、计算摘要或引依赖。

<a id="battle-projectile-hit-acceptance"></a>
## 静态核对

核完整连续1026条/3721B、71直接call/24目标/182跳/两RET和实际case16三条11B；两F32常量按实读四字节。逐阶段核正常分支、原失败/入口检查、图号及随机路径、HP/CryptInt/清理/回调发布、延迟双播放与两种退场；不以表达式数量作机器调用数。现整CPP仅两注释，头仅一注释、所有函数正文/声明签名及其余源逐字保持，现root身份来自原库存。语义/成员/任务与一限定S只具名增量，原记录/执行政策/完整门禁保；显式UTF-8无BOM/LF、静态汇总/-Check一致，不执行算法或复用旧链接作当前证明。

<a id="battle-projectile-hit-source-state"></a>
## 当前源码状态

完整既有动作16按本合同保留，独立身份、完整基线与非拥有附属槽清理关联补齐；不是新增函数/调用或新播放实现。命名前态cg2d-re-battle-projectile-hit-history-20261010，提交后引用cg2d-re-battle-projectile-hit-20261010。完整创建—事件—回调—回收/失败与资源寿命、SDK/生成机器ABI、画面/声音及运行仍未闭，整体SPEC未完成。

### 本步实际限定结果

必要静态和汇总/-Check通过；完整根/现body与原入口失败、随机/状态/HP及清理/双播放/退场正确保持，仅三注释变更，全部声明签名/分发/其它源不动。新增一独立原F及一个限定S、零新I；语义2033（1985F＋48X）、合同2095/3680（56.93%）、F1458/E1345，BATTLE272且枚举false。184叶完整验收0、60任务关闭11及六closure保持，十二自有文件显式UTF-8无BOM/LF。未构建、测试、运行、检查产物或计算摘要；全战斗事件/数据/资源/owner、SDK/生成数值ABI及实际声音画面运行仍未验，整体SPEC未完成。
