# BQH01 战斗参战遍历、朝向与名字取样SPEC

2026-10-07。沿[战斗尾段取样](cg2d_battle_tail_sampling_spec.md)、既有语义/验收体系，依据已开IDA会话[六根定向完整证据](cg2d_battle_participant_turn_name_sampling_evidence.json)推进缺口。用户工程委托下先记裁决再实施，原事实不猜。

<a id="battle-participant-turn-name-sampling-baseline"></a>
## 原端基线与差异

| 原入口 | 新符号 | 指令/实际B/页 | call/普通jump/ret | 源交付 |
| --- | --- | --- | --- | --- |
| 0x0040CAC0 | AdvanceBattleLinkedCast_40CAC0 | 14/45/1 | 2/0/1 | 正确体保持 |
| 0x0040CAF0 | AdvanceBattleEffectAndCast_40CAF0 | 9/22/1 | 2/0/1 | 正确体保持 |
| 0x0043C910 | Sub_43C910 | 19/58/1 | 0/3/1 | 修复1处 |
| 0x0043C9A0 | Sub_43C9A0 | 62/171/1 | 6/6/1 | 修复3处 |
| 0x0043CA50 | Sub_43CA50 | 13/38/1 | 0/3/1 | 修复1处 |
| 0x0043CA80 | Sub_43CA80 | 54/133/1 | 3/7/1 | 正确体保持 |

六完整171条467B/六页、13call/8callee、19普通内部jump/六ret。八入向来自404DB0与43CBB0，各保存一个实际前指令、call和后指令，八窗24条107B；无参函数父前指令可能是其它call或jmp，不虚称push参数。两个返回比较窗口及局部父初始化另存，43CBEA MOV EBP,1供43CFF4/43CFF6比较；43D18D/43D190判断count<2跳过。仅六新根与真实接线，不重验完整父；父switch只邻近记录，不增R7表分类。

| 实际差异 | 原端依据 | 裁决 |
| --- | --- | --- |
| 43C910 FDC读取 | 43C921 TEST BYTE [EDI+FDC],1 | 非空actor固定payload低W8读取，保全20槽/返回 |
| 43CA50 FDC读取 | 43CA60 TEST BYTE [EDX+FDC],4 | 非空actor低W8读取，保全20槽计数 |
| 43C9A0第一阵营旗位 | 43C9A0 MOV AL全局，再TEST AL,1 | 单独W8 firstFlags，不改变全局W32存储 |
| 43C9A0第二阵营旗位 | 43C9F5 TEST BYTE全局,2 | 第一阵营全部调用完后重新W8 secondFlags |
| 43C9A0投影操作数 | 两物理路径FILD y0C→FISUB y04→F32 Y，再FILD x08→FISUB x00→F32 X | 共享lambda显式按序四S32读，正常差值仍double承接并两次F32发布 |

共五源片段/三源体；四低W8源表达式对四原点、投影一共享片段承接两阵营实际路径。正常旧值一致，不称数值、并发或机器故障。其余三个完整源体已正确，保原名字绘制Y前X后/颜色继承和两推进顺序。

<a id="battle-participant-turn-name-sampling-decision"></a>
## 完整正常源合同与裁决

1. 40CAC0：借用固定source.payload，当前C9C→同record.position→共享22槽目标T*，439340(target)之后439340(source)。原唯一父404DB0 case73直接调用并弃返，没有新增空目标/边界门槛；两原调用及位置/别名顺序和源正确体保持。局部承接父实际未消费的返回，不承诺生成机器ABI。

2. 40CAF0：439470(source)弃返，随后439340(source)，顺序和源正确体保持。原唯一父404DB0 case74弃返，没有按第一个返回新增条件，也没有提前缓存施法状态；局部正常源不承诺生成机器ABI。

3. 43C910：遍历原59E980..59E9D0前20槽（新端span直接借用22槽数组前20，不另建影子表），每槽T*一次取样，空跳过，非空借payload；FDC实际低W8 bit1清才写固定actor.phase2并记pending。即使已找到对象仍遍历剩余槽，pending返回0、全部无pending返回1；不改为只检查第一个、全22槽或W32载入。原43C921 TEST BYTE由显式std::uint8_t内存读承接。父43CBB0的43CFEF调用后43CFF4 CMP EAX,EBP/43CFF6 JNZ，父43CBEA MOV EBP,1有据，与当前源==1消费相符；不计父整体闭合。

4. 43C9A0：原43C9A0先独立读取玩家旗位低W8，bit1设才按10..19槽顺序处理；全部调用完到43C9F5再重新读同全局低W8，bit2设才按0..9槽处理。不能齐取两位或复用第一次值；全局存储仍W32供其它消费者。每槽先T*非空再固定payload，原两物理路径均y0C S32先读→y04 S32后读/FILD-FISUB→F32发布Y，再x08 S32先读→x00 S32后读→F32发布X，然后Sub46F970(X,Y)。源共享lambda以double准确承接正常S32差，显式四操作数次序，保Y/X两个独立F32发布；不新异常/S32回绕或实际x87等价门禁。角度返回F64降F32之后重新从共享槽取对象写angle50，之后再次共享槽取对象Sub405CB0；调用后又重新取共享槽PlayFrame(object,1)弃返。两阵营各三原call共六物理点共享三源表达式，保每实际路径消费、调用后独立指针/返回及前后旗位取样，不保存旧actor到后续调用；源五阶段中的三修复在本根（投影、firstFlags、secondFlags）。

5. 43CA50：同原前20槽，每槽共享T*一次取样，空跳过，非空固定payload的FDC实际低W8 bit4设才count++；遍历全部20后返回count。43CA60 TEST BYTE用std::uint8_t内存读承接，不宽读FDC或扩到22槽。原唯一父43D188返回CMP EAX,2/43D190 JL跳过，当前源>=2消费相符；不因此声明父其它门禁已验。

6. 43CA80：原颜色寄存器初值0，W16 gValue5F974C为0返回；非零遍历前20槽，仅非空槽继续。每个非空槽fresh W32当前position<20才按index==position短路或GetPet(position)更新color为4/0；position>=20保留前一非空槽color，不能每轮无条件重置或钳制position。GetPet调用后重新共享槽取actor，name C4地址先、actor.y-24后，TextMeasure(name,1)后再次共享槽取actor.x，宽度按原正常S32向零除2，X=x-width/2。DrawText2(x,y,0,1,color,name,0)原七参/源公开S16 X/Y、S8颜色边界保持，不去掉现有原W16存储边界也不补溢出模拟。原DrawText前Y/name从同缓存actor，测量后X从新共享槽，原正确体保持。43CBB0三个实际call43CFD1/43D03C/43D100分别对应源三个Sub43CA80调用；只新邻窗接线，不声称父完整运行/寿命闭合。

六完整原根171条467B/六页、13call/8callee、19普通内部jump/六ret；八原入向来自404DB0/43CBB0，两推进父与六场景消费父各捕一个实际前指令/call/后指令，八窗24条107B。另存场景局部返回比较/EBP=1初始化窗，父switch仅邻近观察、不新增R7分类或父完整验收。只三个源体五片段：43C910/43CA50低W8 FDC、43C9A0两独立低W8全局读取与共享两阵营投影的四S32操作数原序/F32 Y后X发布；四新W8表达式对四原点，投影一源片段承接两物理路径，正常旧值一致。20槽共享span/前后fresh指针/两阵营顺序/返回消费/正确名字Y前X后和逐槽颜色继承、两个正确推进体保。不外推生成机器ABI/实际x87/其它全部生产与owner寿命/递归父/画面运行；R7/任务完整门禁保，音频暂停、网络原格式明文、RPLAN.24不专项异常/保护/回绕/故障等价及原验收标准保持。

<a id="battle-participant-turn-name-sampling-acceptance"></a>
## 可度量的限定验收

- 六完整根171条/467B、六页、13call/8callee、19jump/六ret，八原入向/八窗24条107B和原返回比较/EBP=1局部依据可追。
- 整YobiRuntime只43C910一处、43C9A0三处、43CA50一处；共享span固定前20槽、四W8原点及两阵营投影顺序/F32发布、调用后fresh共享目标、名字测量前Y/后X与颜色继承和两个推进正确体保持。
- 新增1503..1508六原已有完整正常定义/equivalent，六冻结定位全文保。语义1503→1509/有效1498→1504、合同关联1616→1622/3680（44.08%）、BATTLE180→186枚举open、分类628→634/E766→768；两局部I/S各十七个实际来源，包括两个父、共享槽/声明及名字公开边界。
- R7 260/3061/余2801、差队列172/待归属43和其它全部机器对象/完整门禁保。U08/R01/R04/V04只当前/引用仍open，60任务源码closed10/完整验收0/叶0/global_accepted=false保；不以局部源码条目代完整战斗或R8。

<a id="battle-participant-turn-name-sampling-source-state"></a>
## 当前交付

五源片段实修、六完整正常基线纳管；命名快照 `cg2d-re-battle-participant-turn-name-sampling-20261007`。定向静态和measure/-Check结果记唯一进度；未编译/调试/测试/运行/摘要，音频暂停、原格式明文、原适用验收标准保持。
