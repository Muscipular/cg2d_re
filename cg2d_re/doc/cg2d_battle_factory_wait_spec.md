# 战斗工厂、施法等待与身份更新 SPEC（BFW01）

2026-10-10。四完整正常根115、127–129共体、138、139；复用已登记1497/40FC10工厂共体。只139十组源码调整、CPP/公头各四注释；其它三主体与40FC10主体保持。无新API、字段、默认业务值、保护、循环上限或依赖。

<a id="battle-factory-wait-baseline"></a>
## 原证范围

| 原身份 / 当前函数 | 完整范围（尾不含） | 指令 / 字节 / CALL |
|---|---|---|
| F:0000FB80 / UpdateBattleAction115_40FB80 | 40FB80–40FC05 | 38 / 133B / 2 |
| F:0000FF90 / UpdateBattleActions127To129_40FF90 | 40FF90–40FFFD | 43 / 109B / 3 |
| F:0000FF40 / UpdateBattleAction138_40FF40 | 40FF40–40FF85 | 26 / 69B / 2 |
| F:00010000 / UpdateBattleAction139_410000 | 410000–4106AC | 458 / 1708B / 41 |

合565条2019B/十一页，48 E8/十五唯一直接callee、98跳四RET；无间接CALL/JMP。四实际入向CALL均属于404DB0。沿BIE01已证phase输入和40573C表基址，本步取115/127/128/129/138/139六实际4B表项及四物理11B case；127–129共享同一根，不能计三新函数。四case均push→CALL→JMP40563F，原caller不消费尾EAX，既有void签名保持。

40FC10已有1497/BQG01的完整203条561B合同；当前源与旧证据source_bodies_after一致，仅引用，不重取整根、改写旧记录或重复计身份/I/S覆盖。原根及当前源差集见[证据](cg2d_battle_factory_wait_evidence.json)。

## 工厂115、127–129、138

115固定入口payload→角色动作5→PlayFrame(source,1)弃返；之后读1024，为0才读取当前效果完整W32 flags。原MOV ECX后TEST CH10，当前flags&1000保持一次W32读取及相同门禁，不缩字段或改BYTE内存读。工厂实参(source,0,Y=-32,type3或0,0,1)，返回不保存+10；调用后fresh C9C/position查目标，先target.hidden=1再source.hidden=1。所有路径phase162，不额外推进、清1024或以播放返回值门禁。

127–129与138先W32读取FDC、在AL寄存器判断60h，清时PlayFrame(source,0)弃返，随后fresh读取1024。127–129为0才一次读播放后phase，127/128/129分别创建type7/8/9、Y0、末参0；其它phase不创建，全部路径AdvanceCast(source)，不改phase162。138为0创建type20、Y0、末参1，全部路径phase162。三完整正确主体保持。

## 139：施法事件、完成段及平铺尾

固定payload→characterAction6；1024==0才NewEffect(type34,Y0,0,1)，返回原生非拥有T*覆盖+10，包括创建失败返空。随后一次读event，区间[10000,20000)进入事件段。在该段入口一次借用+10；非空且fresh skill!=402才MarkDeath同一指针→清+10→fresh skill。事件段所有路径末清event。

只有保存skill==98时读取一次1048，12/13两比较复用S32样本；命中只MoveCast(100C动态目标)，不执行效果公共推进。局部moveSpecialTarget初值false只表达skill!=98的已证分支结果，不向载荷/全局补默认值；非98不读1048。97先震动幅度6/上升1，依次声5/262、位置320，亦无公共效果推进；原第三声参数240沿既有未消费边界，不另造状态。

50–53/88当前position!=-1才声275/fresh X，声后fresh skill分别选110092/110089/110090/110091/110155，命中才NewFieldEffect，接着AdvanceEffect。此技能路径无论position是否哨兵均另执行一次公共AdvanceEffect，不能统一次数或缓存声后skill。

405先声275→fresh当前position4→NewBackgroundFade(image,1,1)→AdvanceEffect。逐轮直到position==-1，尾后仍公共AdvanceEffect：
1. 每轮C9C/记录/position取样，哨兵和目标查找复用同一position；先查target，再取position3并发布target.field1D0。AdvanceEffect。
2. fresh记录，先position→position3图号→查target→发布imageNo。随后fresh记录/position查riderTarget及其payload，13C8非空才读position4；非零时先持该值再取target.imageNo→SetRider，真实门禁和读序保持。AdvanceEffect。
3. fresh记录，先position3装备值→position→查target→SetGear。AdvanceEffect。
4. 当前项position3/4分别ConstructFromInt产生HP/maxHP两临时值；每次构造后fresh C9C→position→temporary.encoded→查当前target，先发布encoded，再即时读取/发布mask，再BYTE readCount→Destroy。AdvanceEffect。
5. 当前项position3/4同序构造FP/maxFP、独立重取对象/复制/析构，AdvanceEffect后回循环。原四次构造/析构/RNG及推进数量保持；不复制token/reserved，不以共享CopyPayload调用把encoded读取推迟到查对象之后，共享复制器本体及其它调用不改。

19–30、2068–2079、2055–2057、31、3037进入特效分支；2070/2076/2079先对当前目标创建type44/45/46、末参1，无哨兵预判。然后逐项position!=-1时Seed(position+源position18)→NewEffect(source,type35,末参0)→AdvanceEffect。其它技能逐项Seed后fresh C9C/position查目标MoveCast→AdvanceEffect。两循环结束均公共AdvanceEffect，不加上限或用Seed前目标替代之后目标。

事件后PlayFrame(source,1)严格1才进入完成段；此时重新一次借用+10，非空才MarkDeath同一指针→清槽→至少一轮do。每轮fresh skill==402才MoveCast当前动态目标，AdvanceEffect严格1退出。无论槽是否空，1024=0→AdvanceCast；没有提前return。

末尾fresh+10非空才一次读skill选择平铺0–4，之后再fresh skill选择独立type5。0为19/23/27/2068–2070，1为20/24/28/2071–2073，2为21/25/29/2074–2076，3为22/26/30/2077–2079，4为31/3037；type5为86/2055/2056/2057。原CMP链不是物理跳转表，不新增R7表分类；真实Draw调用和共享相位保持。

<a id="battle-factory-wait-decision"></a>
## 裁决与实现范围

依据子项目AGENTS第8条工程委托，恢复139上述十组读取/发布：两阶段T*、98一次1048、等级循环持position并先target、图号position/value/target、装备value/position/target、四HP/FP临时复制。只使用已证类型和当前原生结构；不改变正常旧值、业务门禁/失败/哨兵/随机消费，异常沿RPLAN.24，不设故障模拟或添加保护。

115/127–129/138原W32读后寄存器局部位门禁与当前表达相符，主体保留。1497共体及十五真实callee当前提供者保持；其中十个已关联独立语义身份复用，其余五个仅定位当前源，不据关联声称独立完整callee合同。音频采用当前WAV-only/SDL合同，数学/CRT沿已批准宿主边界；本步不恢复MIDI、构建、测试、运行、产物或摘要权限。

<a id="battle-factory-wait-acceptance"></a>
## 限定静态验收

必要静态核四完整原根/连续机器码/E8目标、六实际表项/四case、十组取样与全部四源精确重放；核四次临时encoded/mask/BYTEcount及目标token/reserved保持、98条件读取、两次+10样本时点、正确其它分支/共享器保持。登记四新语义/F适配处置/已知BATTLE成员。局部I仅139十组；局部S覆盖四正常源及本步局部分派/十五调用关联，不代完整父-callee/所有生产/容量/alias/owner/机器ABI/SDK/声音画面或运行。

U08/U09/R01/R04/V04继续open，BATTLE枚举false；全部完整功能gates、叶验收、closures及总SPEC§10保持。静态汇总器仅核文本登记，不代构建或运行，整体SPEC未完成。

<a id="battle-factory-wait-source-state"></a>
## 当前状态

十组及八注释已落实；四完整原根/实际分派、整CPP/公头精确重放、全部旧语义及政策守恒、引用/UTF-8/LF/差异与派生范围均通过。文本汇总和-Check均退出0。本步前固定引用Git提交说明：[#] 对齐战斗复制体与离场动作的取样顺序；完成引用Git提交说明：[#] 对齐战斗工厂等待与施法身份更新。已有用户改动七项保留且排除提交。

静态结果：语义2171（2123F＋48X），合同关联2233/3680=60.68%仅关联率，F处置1599、软件E1407、BATTLE已知成员411且枚举false。派生仅六metrics/四F/一功能/五任务变化；导入16/218、X3060/3061余一、184叶完整验收0、60任务关闭11与六closure保持。I只139十组，S四正常源，不以原数量或登记增量称整体完成。
