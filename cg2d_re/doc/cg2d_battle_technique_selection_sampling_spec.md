# BQN01 战斗技术选择取样与命令回调 SPEC

日期：2026-10-07。沿BQM01列表回调的47A000→47C700→CC4230真命令目标4974C0推进，依工程委托先记裁决再实施。

<a id="battle-technique-selection-sampling-baseline"></a>
## 原版基线

[完整原证据](cg2d_battle_technique_selection_sampling_evidence.json)：三根313条1108B/六页，16物理call中15直接/十callee、一CALL EAX槽消费，26普通jump/五ret，无物理switch。47A000两个代码入向、47C700一个地址数据入向、4974C0五地址数据入向有据；外47A620一个邻窗六条13B，仅局部调用，BQM01已过47C570正文不重验。三提示586588/586568/586518首NUL各31/31/21B共83B，格式553FF0含NUL11B，旧字节共94B保持。

CC4230所有四原地址引用：47A499清零、47A4CE/47A508发布参数、47C7A3读；47C7B2/7B3压technique/skill，47C7B4 CALL EAX，真实二S32/void借用函数指针，操作数值0只是间接寄存器标识，不是函数地址。两发布者完整正常源已有BQM01证据，沿用并限定当前三根消费，47A490只清零点登记，不推完整函数/父已验。

<a id="battle-technique-selection-sampling-decision"></a>
## 工程裁决

47A000只恢复selectedSkill一次缓存、canRebirth非0显示6后才重读，0路径保持原缓存；47C700明确先value7C后fpReduce，正常S32乘法/100不改。hover仅原TEST BYTE读取state低W8，输入形参S32保；绘制坐标四次低W16顺序元素Y/X→窗口Y/X，每次两字节memcpy避免窄指针别名，共享坐标W32保持。共四片段/两体，正常旧值一致，不称数值/并发或故障修复。

两函数现有单NUL字面量按实际字符核实正确，不因工具JSON转义显示误改。现有S16配对、byte directSkill、原负maximum处理与void/S32接口保。4974C0正文原正确，989局部格式历史字段全文保持，只追加完整合同/证据和一处置；不重复增语义ID，不合并FP比较/回调后窗口/两Flush/两音效，不加null成功或发送返回门槛。原格式明文，不加解密/协议/转义；音频继续暂停。RPLAN.24不新增异常拒绝/补零/钳制或故障/回绕模拟，保持正常S32算术。

## 三根完整正常合同

### Sub_47A000（0x0047A000）

47A000技术列表生成：mode W32=4、scroll W32=0；directSkill低W8非0直接存index，否则index S32非负才取配对索引S16保存，负保持旧技能。读oldMaximum W32后以元素11 T*传4742C0(oldMaximum,0)弃返；随后元素1..16隐藏、2/4/5/9显示。原47A093一次取selectedSkill W32→canRebirth W32，非0显示6后47A0B4才再读selectedSkill；0路径重用缓存，不无条件重读。15技术逐个name[0] W8直到NUL计数，原count-10负时0存maximum；10..16显示后14/15隐藏，27.data图3BA26，17..26逐次fresh elements写47C700真回调。原86条315B/两页、一call/十jump/一ret，第三低W8形参/原S16配对/NUL判断与void保；仅一个条件重读片段实修。

### Sub_47C700（0x0047C700）

47C700技术回调：scroll+index-17 S32 technique，一次selectedSkill取record，技术name[0] W8为NUL返回0。原47C747先技术value7C W32、47C753后fpReduce W32，正常S32乘法再/100向零得cached cost，不加宽/故障或钳制。value8C W32非0才fresh position/entry T*调用FP>=cost，原AL结果用bool；点击state S32 mask2在允许和拒绝两原路径缓存为同源clicked。允许点击一次借CC4230真实void(S32 skill,S32 technique)函数指针，非空才fresh skill/technique调用；即使null回调也fresh currentWindow并W32 state|8000→声51/x320→consumed1；拒绝点击声55不置消费。原47C804 TEST BYTE形参state低W8 bit1才hover：fresh selectedSkill取record，visual2/hoverTechnique发布后fresh canUse，0原586588；非0fresh position/entry FP<cached cost，真586568假586518，三旧NUL串逐字节复制。最后cached click且fresh selectedSkill.canUse非0才fresh position/entry FP>=cached cost，真覆visual1；不沿用入口allowed或合并三比较。fresh currentWindow/element T*，四低W16按元素Y→元素X→窗口Y→窗口X取、各S16和取S16结果，memcpy只两字节/共享坐标W32保。47B9B0(x,y,S8 visual,technique+100,3,223,FFFFFFFF)，返consumed0/1。原175条579B/三页、七call其中六直接/五目标及一个CALL EAX槽消费、十五jump/两ret；不是call地址0，不造function ID。仅两读取序/低W8/四低W16共三片段，正常旧值一致。

### Sub_4974C0（0x004974C0）

4974C0双S32/void命令消费：先selectedSkill W32 pub、再selectedTechnique W32 pub，按原两个输入读technique.value84 W32 target，再fresh position W32调用43C560(position,target)。非0只pending D303F8 W8=1/返回，不发送；零路径fresh position→selectedTechnique→selectedSkill依序重读，未清256B以原553FF0 S|%X|%X|%X、三个U32技能/技术/位置格式化。格式后fresh socket原生uintptr取样真实SendBattlePacket弃返，随后Flush→声57/x320→fresh skillCount W32取→commandState W32=2→count+1→再次Flush→声51/x320→496AE0。两次刷新/两次音效及写序不合并；不因发送返回拒绝后写、不加转义/协议或加解密。原52条214B/一页、八call/六callee、一jump/两ret。正文原正确保持；既有989格式局部记录全文保，增加完整正常源合同/证据及一equivalent处置，不重复新增语义ID。五地址数据入向不虚计代码调用；本步CALL EAX真实CC4230消费与BQM01已过47A4C0/47A500生产承接，47A490清零只登记，不据此宣称其完整父已验。

<a id="battle-technique-selection-sampling-acceptance"></a>
## 可验收范围

四源片段/两完整体、第三正确体与整个文件其余内容/共享声明保；三原根每条字节与边界、原新15直接call和CALL EAX消费/三FP分支与返回、四槽引用/首NUL94B/局部六条13B有据。语义1528→1530/有效1523→1525、文档关联1641→1643/3680（44.65%）、分类653→656、BATTLE205→208枚举open/E778→780；两局部I/S各十六真实来源，不代表完整生产/owner/运行。R7 261/3061余2800、差队列172/待归属43与60任务源码closed10/完整验收0/叶0/global=false保。U08/R01/R04/V04只当前和引用仍open；新源码/台账后文本汇总器和-Check生成看板，不手改生成件。

不构建、调试、新增/执行测试、运行客户端/算法样例或计算摘要；完整输入/其它模式/全部父、所有权与重入寿命、窗口10/26与57C368 token、SDK/生成机器ABI/画面运行及R8不据本步验收。

<a id="battle-technique-selection-sampling-source-state"></a>
## 当前交付

三个完整313条1108B/六页、16物理call（15直接/十callee与一CALL EAX槽消费）、26普通jump/五ret；无物理switch/新R7。只两体四片段：条件selectedSkill W32重读、cost两个W32取序、hover实际state形参低W8及四坐标低W16取序，正常旧值一致。NUL名称首字节判断、S32状态/计算、W16配对、W32共享坐标/状态、三FP比较/三旧提示83B和S格式11B、回调null仍关闭/consume1及消息/命令写序保持。CC4230仅三原写点/一读点：47A490清零、BQM01已过47A4C0/47A500发布、47C700借用后双S32/void消费；不造地址0函数，不扩大已过生产复核。一个外父六条13B只局部call；两新语义与989完整追加、三equivalent，所有历史989字段保。完整数据生产/其它模式/全部父、owner寿命/窗口10-26与57C368 token、SDK/生成机器ABI/画面运行与R8未验；音频暂停、网络原格式明文和RPLAN.24不专项异常/保护/回绕/故障等价保持。

命名快照：`cg2d-re-battle-technique-selection-sampling-20261007`；唯一进度记实际检查与提交。
