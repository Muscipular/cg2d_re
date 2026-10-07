# BQS01 宠物技能选择消费与费用行取样SPEC

2026-10-07。沿[BQR02两个selector发布CC4D14](cg2d_battle_skill_state_producer_sampling_spec.md#battle-skill-state-producer-sampling-source-state)及[BQP01两列表/FP桥](cg2d_battle_skill_lists_rollback_sampling_spec.md#battle-skill-lists-rollback-sampling-source-state)推进三个consumer与所需费用行。依AGENTS.md第8条工程委托先记裁决再实现；[完整指令与冻结前后源体](cg2d_battle_pet_skill_selection_sampling_evidence.json)为实际证据，不猜原事实。

<a id="battle-pet-skill-selection-sampling-baseline"></a>
## 完整基线与差异

| 原入口 | 新源（cpp#符号） | 指令/实际B/页 | 物理call/jump/ret | 台账 |
| --- | --- | --- | --- | --- |
| 47C950 | WindowRuntime#Sub_47C950 | 136/430/2 | 5/16/2 | 新正常身份 |
| 47CC80 | WindowRuntime#Sub_47CC80 | 145/487/2 | 11/9/2 | 新正常身份 |
| 47CFE0 | WindowRuntime#Sub_47CFE0 | 127/393/2 | 5/9/2 | 新正常身份 |
| 47CB00 | WindowRuntime#Sub_47CB00 | 123/374/2 | 10/3/1 | 983既有格式记录补完整正常源 |
| 47CE70 | WindowRuntime#Sub_47CE70 | 119/361/2 | 7/3/1 | 984既有格式记录补完整正常源 |
| 401D70 | LegacyCryptInt#GreaterEqual_401D70 | 4/15/1 | 1/0/1 | 新原正确单读比较叶 |

六根654条2060B/十一页、39物理CALL（36直接/13callee和三间接）、40普通jump/九RET，无switch/新R7。28入向中25代码、三数据、20有IDA父；只登记，不把入向数当完整父验收。

| 差异 | 原实际依据 | 裁决 |
| --- | --- | --- |
| skip循环界/hover | 47C9C8初W32界，47CA34尾取界后47CA3A增row；47CA68..6A缓存hover，47CADB尾只测该cache | 初/尾界显式保序、缓存hover并跨callback/音效用于尾图 |
| 两选择回调点击/尾状态 | 两启用与两禁用TEST BYTE [ESP+位移],2；47CDE1/47D0E2各一次MOV DL后尾两判断 | 四实际低W8点击读；各尾一次visualFlags W8缓存，pressed优先 |
| 两回调坐标 | 各MOV DX elementY、MOV CX elementX、ADD DX windowY、ADD CX windowX | 四低W16各读，原序；2B memcpy免宽指针别名，原共享S32字段保 |
| 三cost-first父 | 47CA11/47CBC2/47CF25实际401F80 | 复用既有typed cost-first桥；旧反向Less正常值/单读/RNG一致，不造新decoder |

五源体13片段，正确>=叶保持；并无普通数值或RNG消费错误，不称故障/并发差。三间接CALL分别47CAA4寄存器EAX、47CD94实际[CC4D14]、47D09B寄存器EDX；其operand value 0/CC4D14/2不是函数入口，不创建F0/F2或把slot地址当函数。

<a id="battle-pet-skill-selection-sampling-decision"></a>
## 正常消费、接线与裁决

1. skip先借graphic T*后mode。mode5活宠→映射当前位置；mode6骑宠→原当前位置。有效名称才cost-first FP单读，FP不足短路不读flags；否则按row低五位mask判enabled，分别增加unavailable/count。初capacity及每轮尾fresh capacity先row++保持。若count>unavailable，fresh window.elements[8].visible=0/返回0；否则原hover旧字节→clicked callback一次held T*、非空传-1→fresh window.state W32|8000→声音51/consumed1→借graphic default/held hover/pressed图→原返回。
2. 普通选择pairIndex=index-17为S32；五次活宠获取、三次S16 pair读取按name/FP/调用参数阶段分别保留。名称首NUL返0；flags禁用且clicked才声55，FP不足不补声音。enabled先FP>=cost再实际低W8 clicked门禁；callback第一次读只判非空，之后活宠/pair重取，再fresh同slot CALL。不能把该回调缓存跨43C4B0。
3. 骑乘选择先held骑宠/pair/record；启用和FP检查同上，callback这次须缓存T*，随后fresh骑宠/pair生成参数但CALL仍用同T*。普通/骑乘callback=null都继续fresh window W32请求关闭、声51/consumed1，不能引新返回或跳过收尾。两个尾状态只一次低W8取样，hover2后pressed1优先；两窗口/元素坐标字段W32储存保、低W16消费原序。
4. 原IDA把两尾MOV DL标成arg8，合流get_spd为-20/-12也与实际指令不符；原SUB/PUSH保frame -24/-16，FP的401D70 RET4、caller参数清理和音效三个C3 RET0不净改变该frame，32/24位移实际回到entry ESP+8，仍第二参数state。至47CE1D才POP EDI；不能据IDA错误元数据造第三参数，不改数据库。skip初state是W32取样后寄存器DL消费，保其W32观察，不照搬另两回调的内存W8读取。
5. 47CB00两次活宠分开held record及pair，47CE70入口held骑宠；完整descriptor零初始化/32位color/byte13=4有原写入依据。空名称返；清文本尾/空字绘制→位置/action/费用→cost-first缺FP色6，disabled色7覆之→name绘制→fresh费用到原20B/%3d→数字→图，不能缓存费用跨名称绘制或移动FP读。两原常量格式同553FCC，含NUL 25336400；保既有FormatLegacyString接口/格式字段和整个buffer使用。
6. helpers的x/y现S16边界有效：原456780分别456794/45679E只取输入低W16；数字/图前原也显式MOVSX DI/BX。visual原helper两个0FBE字节读取对应现S8，三图W32表54F454为3B5B7/3B5B8/3B5B9，保持原索引，不新钳制。此处只核所需坐标消费点，不重新验收整个DrawText/renderer。
7. 401D70完整原四指令：Read401EC0一次→S32 CMP/SETNL→RET4，现GreaterEqual及Yobi typed桥正确。cost-first另三父只借原FP对象，桥内单右读/计数RNG与原401F80一致；不新增接口/底层对象公开或删FP检查。
8. skip真实原型是index8的32B [561C78,561C98)，callback字段561C94→47C950/data561B00；数据xref从561B78数组头不等callback字段。两constructor的47A303/47A459只是MOV ECX取函数地址，实际循环store47A313/47A47A发布到元素17..26共十槽，两原36B runtime元素字段+20，现原生mouseCallback一致。普通/骑乘列表构造及通用事件分发已过，只核该发布尾/来源，不复验全部父。
9. 已过47A620只有mode5/6使element8可见；其它mode原skip不写position的事实仍保，不造默认position/拒绝分支或故障模拟。两旧tooltip分别5865A8 13B/586518 21B含首NUL共34B，以原字节保持，不猜转码。
10. 原音效第三坐标未消费，保持现51/55、x320的实际两参数桥；音频继续暂停。本步不改网络、协议、加解密或资源格式。

## 台账更新与历史保留

三个consumer和>=叶新增1554..1557四正常原F。983/984两既有费用格式记录保全部business_*、原引用/格式证据、地址/指令数；把其当前status/source关系同步到本批完整源，旧状态/关系和完整记录存入本批证据，追加五引用及完整子基线，不重复原F。

六equivalent及六BATTLE成员，仍枚举open：语义1554→1558/有效1547→1551、关联1665→1669/3680（45.35%）、分类678→684/BATTLE230→236/E788→790。无F/有效区间各7、R7 264/3061余2797、开发队列172/待归属43及其余完整门禁不变。

<a id="battle-pet-skill-selection-sampling-acceptance"></a>
## 可度量验收

- 六完整原根/单chunk/每条连续字节、654/2060B/十一页、39call与36直接/13callee/三间接、40jump/九ret。
- 六冻结源体，整WindowRuntime仅五体13片段；正确FP>=平台叶/Yobi桥、共享字段/同slot/构造/网络/两helper费用格式和绘制正文保持。正常值/单读/RNG/返回与失败分支不变。
- loop初尾界与held hover、四实际点击W8/两尾一次W8 cache、八W16坐标序、三cost-first父/两旧tooltip和表项/接口实际绑定相互对照。不要把register operand值当F。
- 两旧983/984历史全文/业务证据保持，新当前状态与关系、四新语义及六处置/成员、两局部I/S/四任务U08/R01/R04/V04只本批增量。定向静态、measure/-Check/完整快照增量和编码/引用/diff通过后精确提交12文件。

仅当前正常源和所需原字节/局部消费点通过；全部pet/技能/FP生产、全部父/窗口/owner寿命、SDK/机器ABI/画面运行与R8仍须分别完成。音频及不构建测试运行摘要/RPLAN.24限制保持，不降低适用验收门禁。

<a id="battle-pet-skill-selection-sampling-source-state"></a>
## 当前交付

先落本SPEC与证据再修源码/台账/四任务/唯一进度。固定快照`cg2d-re-battle-pet-skill-selection-sampling-20261007`；实现三个consumer/两费用行的原取样合同，纳管正确>=叶，不以236成员、文档覆盖率或局部I/S声明完整战斗或整体完成。
