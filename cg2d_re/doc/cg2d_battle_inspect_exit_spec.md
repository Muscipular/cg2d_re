# 战斗调查、绕行、标死及逐效果命中合同 SPEC（BIE01）

2026-10-10；104–108五完整正常根。104全局/源位置与105游标/源位置四读取组、108十三BYTE消费表达式（原十四点）及临时HP零复制一组，共十八组；十注释。其它正文/签名/数值/调用/RNG、完整W32发布/高位、正确最大HP Copy及共享器保持，无新API、保护、默认值、循环上限或业务重构。

<a id="battle-inspect-exit-baseline"></a>
## 原根、真实分派与依赖

| 原身份 / 当前源 | 完整范围（尾不含） | 指令 / 字节 / CALL |
|---|---|---|
| F:0000DC90 / UpdateBattleAction104_40DC90 | 40DC90–40DE3C | 133 / 428B / 11 |
| F:0000DE40 / UpdateBattleAction105_40DE40 | 40DE40–40DF4D | 77 / 269B / 4 |
| F:0000DF50 / UpdateBattleAction106_40DF50 | 40DF50–40DF66 | 6 / 22B / 1 |
| F:0000DF70 / UpdateBattleAction107_40DF70 | 40DF70–40DF7C | 5 / 12B / 1 |
| F:0000DF80 / UpdateBattleAction108_40DF80 | 40DF80–40E4A2 | 367 / 1314B / 30 |

共588条2045B/十三页（每页至多64条）、47 CALL（44 E8/二十一唯一直接callee，另三CALL EBX）、57跳五RET，五根无间接JMP。五动作各唯一实际入向CALL/父404DB0，五11B case均push→CALL→JMP40563F。父404DC7读action.phase，404DCA/DCF保257项边界，404DD5实际按40573Ch+phase×4跳转；104–108对应4058DCh起20B五表项，逐项实际落到40526A/275/280/28B/296。107是五条独立根并保T*返回，不把函数内部40E060捏造为入口。

104的40DCD0从IAT54D258读取EBX，该槽数据库名称/类型为wsprintfA及int(*)(LPSTR,LPCSTR,...)；三FFD3为寄存器调用，操作数值3是寄存器编号，不是地址/新F。未加载IAT的FFFFFFFF不作运行函数地址，也不新计导入审查或宿主实现身份。五根原指令/入向/case、真实格式字节及70F32见[证据](cg2d_battle_inspect_exit_spec.md#evidence-archive-20261011)。

## 104：调查消息、原始结构字与结束推进

入口原40DC96先读取全局玩家位，之后固定payload/读源position18比较。本步明确保存该全局样本。入口失配只沿当前effect.position==-1哨兵循环AdvanceEffect，弃返回；入口匹配则每轮fresh当前index/position，-1结束，否则固定该轮target。完整flags80000h全清输出失败；置位则40DD08先fresh全局，再40DD0D读源position比较，仍匹配才两条成功消息。这个重取独立于入口样本，失配不输出。每条消息追加参数4、3；每轮推进，结束后**额外AdvanceEffect一次，再AdvanceCast**。无播放、数值解码或清帧。

40DD18–63把maxHP/HP各16B按值压入原可变参数区；格式%s/%d/%d只消费固定nameC4及当前HP结构的前两W32（legacyVftToken、encoded），不消费解码HP或最大值。第一条输出返回后重新取maxFP/FP快照并按同name地址输出第二条；原没有Read/构造/析构CryptInt。当前四memcpy保各16B原字节及未消费上限快照，两个S32用bit_cast承载，保持token/reserved且不消费RNG或readCount，不擅自修“显示正常HP/FP”。

三格式原Big5字节连NUL分别21/20/20B（共61B），原证据保留；当前静态文字按[已批准GBK合同](cg2d_text_encoding_spec.md#big5-to-gbk-source-state)承载，格式符%s/%d/%d保持。256B栈缓冲/目标名借用及三现std::sprintf保持；本步只恢复两global-position取样，库内格式算法/原x86栈ABI不作为新机器等价声明。

## 105：四区段绕行与旧计数门禁

入口固定payload及初始effect.position对应target，characterAction16/rate50。原40DE72先fresh C9C，40DE78再读sourcePosition，之后读该index.position比较；本步显式保存两样本。两位置不同才按旧FF0<=30、<=60、<=90、其它分别定位到target上/右/下/左70：上/下先X发布再fresh Y计算/发布，右/左先Y再fresh X计算/发布；沿既有double中间/F32落值后Turn(source,target,1,0)。70原F32位模式00008C42，数值实现保持。

随后重新读旧FF0为previous，+1发布，用**旧previous>=120**判断：FF0=0→rate100→Advance(source)，再按40DF20游标/40DF26源位置/40DF2F该记录取样比较，不同才Advance入口固定target；本步恢复这个第二独立读取组，不缓存初始record或换成当前动态target。末PlayFrame(source,0)返回S32。所有已正确阈值、计数/坐标发布及返回保持。

## 106/107：角色动作与逻辑标死

106仅characterAction13→Advance(source)，无Play/rate/frame修改，void保持。107原40DF75调用404D00 MarkDeath后返回原EAX对应的真实YobiAction*；不改为物理释放或省去返回，调度器摘链/释放沿既有独立合同。两正确正文全部保持，仅补完整根来源。

## 108：持事件触发、固定视觉目标与动态HP目标

原40DF89读取frameEvent再固定payload；characterAction5/rate200，只有保存事件>=10000且<20000才至少一轮do。每轮保存initialIndex/position，固定target/targetPayload后保存完整initialFlags：DL4图110000；DH4走FindTechnique(8,position)<12图110098否则111048；DH2或initialIndex0图110135；其它唯一RAND(0,2)+110001。目标Turn(target,source,1,0)后fresh C9C==0才图110014；随后fresh完整flags写固定FE4，再ApplyFlags(fresh sourcePosition,source)。不把原低/高BYTE测试缩成字段类型或提前持后续样本。

ApplyFlags后保存当前hitIndex与完整hitFlags，CL8仅target.phase31；否则CH1才flash1004=1→声210/fresh sourceX→phase31。其余保存CH E0全清且该hitIndex.amount非0才伤害：CL2可选图110008→非负image创建→Rebirth→声134/fresh X→fresh当前amount/position动态目标Subtract HP→fresh compareTarget Less(HP,0)。原没有10000000h伤害免疫门禁。

负值ConstructZero后原40E195先C9C、40E19B先temp.encoded，再40E1A2/1A6取position/动态target；encoded发布后才fresh mask读取/发布，再fresh BYTE readCount读取/发布→Destroy。本步只展开这一临时Copy，保目标token/reserved，正确最大HP Copy和共享器保持。调用后fresh BYTE20才固定targetPayload完整FDC OR2→共享gate1→aliveFF8=1；AL位操作结果仍完整W32发布。

然后fresh完整flags持样：AH80仅Advance(target)；AH40先Advance，fresh healIndex的完整10000000h命中则动态图22459，不Add/Number/Attribute。否则fresh amount/动态target Add HP→fresh compareTarget Greater(HP,maxHP)→命中后fresh copyTarget做现正确最大HP Copy。数字按flags→amount→固定targetY-80→X，Number(style5,2,fullFlags)，随后fresh BYTE2命中图22454，然后Attribute(source,target)。

AH20仅Advance；普通按保存完整100000h先MarkDeath(effect1054)，fresh完整FDC持清20h结果，effect1054=null→FDC发布→Advance(target)。其后fresh displayIndex/完整displayFlags，**仅凭10000000h**决定动态图22459；否则固定targetY-80→fresh amount→X，Number(style6,2,fullFlags)，再fresh BYTE2及属性。与102不同，不加amount0免疫门禁；两数字尾原40E332/40E3F0对应现同一BYTE表达式。

每轮共同末fresh tailIndex，先BYTE8全清，再该index.amount非0才固定targetPayload完整FDC bit1生成FD0=14*(bit+1)，其余保旧FD0。AdvanceEffect(source)严格返回1结束；末speed8F→rate100→入口payload.FF0=0→frameEvent0。无论是否处理事件，公共PlayFrame(source,1)严格1才Advance(source)。十三BYTE表达式/原十四点均从正确持样或fresh字段消费，FE4/数字完整flags、100000h/10000000h与全部调用/RNG保持。

<a id="battle-inspect-exit-decision"></a>
## 实施与承载裁决

依AGENTS第8条先SPEC后实施：一个CPP十八读取/消费组和五定义注释，公头五注释；旧shared_battle仅对应104/105编码/取样说明和当前来源对齐。104/105/108限定I；S限五完整正常根/局部分派、21直接callee关系和wsprintfA导入来源/61B原格式及70F32。106/107正文保持，不把补原身份算新代码实现。

沿既有x64委托明确104的**有限格式宿主边界**：保原实际使用的%s/%d及未消费返回，继续用现std::sprintf与两个S32显式实参适配原结构按值ABI；仅这三处当前格式，不扩大为通用wsprintfA替代或引新Win32/CRT框架。原库内算法/栈ABI不复刻，业务参数字/读取或消息顺序、目标名借用及已批准GBK输出保持。完整机器ABI/所有同名格式调用/未知alias和真实画面仍未验。

五根此前无独立语义/处置/已知BATTLE成员，补五adapted记录/成员，枚举false；两限定I/S软件证据、五任务U08/U09/R01/R04/V04来源且open。完整原分母/政策/所有gates/六closure保持；不把依赖只定位当全部callee/输入生产/共享寿命已验。BPST01的四块未知载荷区域不猜、不补。本步不编译/测试/运行/算法/产物/摘要或引依赖，WAV-only与MSVC方向保持，总SPEC§10不降低。

<a id="battle-inspect-exit-acceptance"></a>
## 必要静态与剩余

核五完整588条2045B/十三页/库存、44E8/21直接callee和三FFD3真实IAT来源、57跳五RET、五实际入向/一父及五11Bcase、父phase来源/实际表基址与20B五目标。原字符串用有界raw bytes直到00读取；IDA字符串接口返回已转码/拆项视图，不作为原字节。核十八组/十三BYTE原十四点、104两全局顺序/16B原始字实参/额外推进、105两游标先位置/旧120与固定target、107调用后T*、108临时HP零发布序/固定与动态目标/完整免疫区别/真do及公共严格终帧。

核整CPP仅十八组/五注释、公头五注释，其它源/正确最大HP Copy/共享器/静态文本保持；五新身份/处置/成员、两限定证据与五任务open、政策/gates/UTF8无BOM/LF、差异及文本汇总/-Check。完整所有输入生产/父-callee/容量/alias/owner/SDK正常失败/声音与画面运行仍待，当前静态不代机器执行或整体完成。

<a id="battle-inspect-exit-source-state"></a>
## 当前状态

五完整原根及实际20B分派表/61B格式/70F32已取齐，十八组/十注释落实；必要静态、文本汇总/-Check及精确派生范围通过。语义2163（2115F＋48X）、合同2225/3680=60.46%仅关联率、F1591、E1403限定I三F/S五F、BATTLE403且枚举false；十五旧语义/所有其它源与原门禁政策、五任务open保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-inspect-exit-history-20261010，提交引用cg2d-re-battle-inspect-exit-20261010；整体目标active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_battle_inspect_exit_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 对齐战斗调查与逐效果命中的读取顺序」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x40dc90 | 0x40de3c | 133/428 |
| 0x40de40 | 0x40df4d | 77/269 |
| 0x40df50 | 0x40df66 | 6/22 |
| 0x40df70 | 0x40df7c | 5/12 |
| 0x40df80 | 0x40e4a2 | 367/1314 |

原验证记录：`necessary_static`=pass（历史限定检查）；`measure`=pass（历史限定检查）；`measure_check`=pass（历史限定检查）；`derived_snapshot`=pass（历史限定检查）。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
