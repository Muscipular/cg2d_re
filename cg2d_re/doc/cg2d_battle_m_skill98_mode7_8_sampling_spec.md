# BMX01 战斗技能98 mode7/8换位与重置取样

日期：2026-10-06。沿[BMW01 mode6](cg2d_battle_m_skill98_mode6_sampling_spec.md)继续mode7/8。按既有工程委托先SPEC后实施，单CPP两既有根18组18定义18处，mode7十五处、mode8三处；不扩业务或降低验收。

<a id="battle-m-skill98-mode7-8-sampling-baseline"></a>
## 原端有界基线

| 范围 | 指令/字节 | call |
| --- | ---: | ---: |
| mode7目标/换位42D28E..42D68D | 238/1023 | 8 |
| mode8目标/重置42D6BC..42D7D1 | 72/277 | 10 |
| mode7空失败/终结参数尾42D68D..42D6AF，沿BMW已证尾 | 10/34 | 1 |
| mode8空失败/终结参数尾42EAE6..42EB02 | 9/28 | 1 |
| 两模式共用消费尾42EB02..42EB32，沿BMW已证尾 | 12/48 | 3 |

五范围去重341条1410B七页23call，见[实际字节/分支和源码前态](cg2d_battle_m_skill98_mode7_8_sampling_evidence.json)。所有直接目标落本组或已证437A29父回存；已有两个尾22条82B/4call只沿当前BMW证据保持，不称本步新增原范围或重复计入跨步总完成率。

mode7比较42D285..42D28E两条9B、mode8比较42D6AF..42D6BC三条13B，另列五条22B/0call入口上下文，不计正文。mode7相等正常落42D28E、不等进mode8比较；mode8相等落42D6BC、不等42D7D1后续分派，后续主体未验。一次mode读取及先前比较沿BMW26条114B已证上下文，SKL98入口沿BMO202B表。全为比较链，不据现代switch造原跳表、代其它模式或完整父17298验收。

<a id="battle-m-skill98-mode7-8-sampling-decision"></a>
## mode7十五处裁决

| 组/处数 | 原序及恢复 |
| --- | --- |
| 目标/1 | Take/GetPosition→255比较→pub同target，非哨兵沿同解析位置查actor/空门禁及GetPetPosition，不pub后重读位置。 |
| 当前位置/1 | GetPetPosition调用后live currentPosition先live target/oldPosition，比较先固定moving和oldSlot，匹配才pub otherPosition。 |
| caster比较/1 | 首次效果调用后live target先caster比较，保该target用于第一坐标actor；不要交换全局读取次序或额外取target。 |
| moving四坐标/4 | x00先otherIndex→固定比较所得target的actor/payload→point[2]；y04/x08/y0C各次live target→otherIndex→actor/payload→point[3/0/1]，不先读完整point后角色。 |
| initialOther字段/1 | 初次other非空时固定initialOther payload先live target/pub battlePosition18；后再次查other/非空且phase!=34门禁保持。 |
| other四坐标/4 | x00 live target→固定已检查other payload→targetIndex→point[2]；其余三次各live target→live other actor→targetIndex→payload→point[3/0/1]，不跨坐标缓存角色或index。 |
| rider/gear/两aux/3 | parent10重绑后另一次live target供actor/payload/附件链和value1D4发布共同使用；两aux实际13B8/13BC均核后沿既有两次循环，不重复读取全局目标，不复用先前附件指针。 |

GetPetPosition返回otherPosition固定，调用之后另取oldPosition；moving非拥有T*及oldSlot非拥有native T**固定跨首次效果和换位，oldSlot保持原地址，不能以当前目标/重新查槽替代。moving四坐标和position18写入后，初次other判空/写position18/再次查other及phase!=34四坐标保持。

换位前另live sourcePosition→other槽replacement→pub target=otherPosition→source槽replacement→other槽moving，不能交换发布时序或省略原含null槽写。移动rider、旧槽rider、**caster槽判空却重绑当前target gear/两aux**、最后旧槽gear的非对称门禁/parent10和值1D4保持；原42D4E5注释与旧槽后续借用不改成目标判空或对称遍历，不新增立即释放/owner或null保护。

最后当前target value1018=210、Append80/11、value54原F32 3.0、framePlayingRate原S32 100。固定oldSlot再判非空且phase!=34，符合才caster效果oldPosition/0/0→oldSlot Append11→旧槽value54/rate各次重读。继续目标循环；哨兵或空Finish break均走原共用结束尾。

## mode8三处裁决

目标Take/GetPosition→255比较先pub、同解析位置actor空门禁；首次效果沿解析target，之后live caster→actor用于4393A0(target,0,0)，不改读live target参数。效果调用后另live flagsTargetPosition→actor→payload，原42D719为**flagsFDC对象低W8**测试2；通过后Reset4391E0沿该次固定flagsTargetPosition，不再读全局目标。使用既有unsigned char对象字节访问模式，不以W32读取后截断代原W8。

Reset后live currentPosition→GetPetPosition→再live target比较，匹配才以该target actor AppendEffect(-2,0,0)，后live target Append223，调用之后再live target供205字段；不匹配沿比较所得target直接写value1018=205。随后80/53/51各次live目标，回下一Take。原条件和常数、单Take循环/可选重置与223保持，不补新业务或哨兵保护。

两个模式的空目标Finish后仍进入各自终结参数尾，255直接走终结；共用消费效果(-1,0,0)→另live caster Move→RID(scene)→固定scene payload58递增→437A29。正确CompleteBattleMSkill98Targets核心保持，不将失败break改return、不新caster保护或默认返回。

两根18处之外的wrapper/准备/mode6/共用尾helper/其它根、布局和声明保持；保原正常S32、W32/W8、原业务常数/范围；native只拓宽已证地址，非地址不拓宽，不引入全局类型或SDL/Win32上层对象。网络原格式仅明文/音频暂停；不新增越界/数学溢出/输入拒绝/异常补零/钳制/回绕或确定值模型。

<a id="battle-m-skill98-mode7-8-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 五范围341条1410B七页23call连续字节/直接目标闭合，两个既有尾与BMW当前证据一致；两选择比较五条22B另列，仅上下文。 |
| 实现 | HEAD整CPP两根18组18定义18处精确差异，mode7目标/位置/八坐标/三附件路径及mode8 W8/固定reset目标有限静态，其它根和头保持。 |
| 消费 | 两单Take循环/目标255或空Finish均仍终结-Move-RID-增58；固定oldSlot/moving与逐次live区别、原非对称门禁/槽交换顺序/210或205/动作和phase34保持。 |
| 台账 | 旧812仅有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S/七open、完整门禁和其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI、全部98/owner运行或异常等价。 |

<a id="battle-m-skill98-mode7-8-sampling-source-state"></a>
## 当前状态与后续

mode7/8十八处实修，完整有界两主体和三个实际尾341条1410B纳管；比较链只验证两个模式入口。状态目标、mode12/13/14、15/16、17/默认继续按真实原端依赖形成子SPEC开发，不以正确共用尾或现代路由代全部模式及owner运行。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E405→409仅双域四局部I/S。其它技能-全路由/完整父-SEH/全部消费者-owner重进退出借用失效/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续，关联率不是完成率；本组14文件，快照`cg2d-re-battle-m-skill98-mode7-8-sampling-20261006`。
