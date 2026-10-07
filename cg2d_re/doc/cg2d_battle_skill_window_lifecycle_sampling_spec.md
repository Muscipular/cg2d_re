# BQO01 战斗技能窗口收尾与初始化取样 SPEC

日期：2026-10-07。沿BQN01命令消费496AE0收尾及47A000滚动助手、BQM01窗口10初始化/返回依赖推进。按工程委托先记方案后实施，不扩展业务。

<a id="battle-skill-window-lifecycle-sampling-baseline"></a>
## 原版基线

[完整证据](cg2d_battle_skill_window_lifecycle_sampling_evidence.json)：四根260条837B/六页，17直接call/十callee、40普通jump/一实际七项switch/五ret。十二收尾调用邻窗60条305B仅局部调用与相邻消费，83助手代码入向只登记；两地址数据入向均由原型/数组头给出，不能读头值0/1当回调指针或把数据xref虚计为代码call。

实际窗口10原48B原型54E420..54E450：elements字段54E440→561B78，初始化字段54E444→47A620；原30元素中返回按钮index4的32B原型561BF8..561C18：data561C10→561AD0、callback561C14→47B110，type1/x16/y28/width50/height15/visible1和源第4行完全对应。原型32B与运行元素36B不能混；源原生T*/函数指针字段现已拓宽，非地址字段W32保持。初始化通用消费链以前步证据承接，不重新全面核对。

42B未归属区间 `X:0007A776-0007A7A0`：首2B 8BFF实际只读解码为MOV同一寄存器的对齐；47A778..47A794七W32目标mode0..6：47A64A/47A6A4/47A6A4/47A65C/47A66F/47A687/47A695；尾47A794..47A7A0十二90 NOP。唯一47A643→47A778数据入向。mixed_table_data/mapped只证明字节用途，不计新函数/排除或运行验收。

<a id="battle-skill-window-lifecycle-sampling-decision"></a>
## 工程裁决

496AE0四片段恢复window10取样先于userState写，以及三个action槽各阶段一次借用，非空MarkDeath/清当前槽后才取下一槽。不能把三个槽齐取、再次重读槽当实参，或把关闭请求/MarkDeath改成立即释放。

47B110四片段：先elements T*→入口hover S32缓存→graphic T*；尾用cached hover；点击2和pressed8仅按各原TEST BYTE取形参state低W8，S32参数不改，两个掩码不合并。4742C0一片段明确height后previousHeight W32读取，保正常S32算术、写previous.y后fresh baseY及+8别名写。共九片段/三体，正常旧值一致，不称数值/并发或故障；初始化47A620完整正确体保。原回退/负maximum分支、W8减计数和W32状态写保持。RPLAN.24不加异常拒绝/补零/钳制/保护、故障或回绕模拟，不引入新工具函数/外部依赖/RTTI。

## 四函数完整正常合同

### Sub_496AE0（0x00496AE0）

496AE0无参void收尾：window26槽一次取非空才继续；原value10 S32>0清0、value14 S32<0清0，各条件独立，空窗不写任何后段字段。496B0C原window10 T*读取先于userState W32=2发布，再依次10/27/28各fresh T*，非空只W32 state|8000关闭请求；OR CH是原W32寄存器缓存，不窄化写、不即时释放窗口。之后CC1A10/D2F790/D2D60C三展开action槽按阶段各一次借T*判空并MarkDeath，非空清该共享槽后才取下一槽，不齐取或借后再次读槽为实参，不把MarkDeath变为delete。原53条176B/一页、三call同404D00/九jump/一ret；源入口early return与尾void同结果。四片段：window10读取/状态写序及三个阶段一次指针取样，正常旧值一致；12代码入向五父/十二邻窗60条305B只局部调用/相邻写序，不推完整父或owner。

### Sub_47A620（0x0047A620）

47A620窗口10初始化/刷新：value==-1才先mode W32缓存、scroll=0，原无符号>6默认及七表项mode0→479B80(fresh currentWindow,-1)，3→479EA0(fresh currentWindow,-1)，4先fresh selectedSkill再fresh currentWindow→47A000(skill,1)，5→47A1A0(fresh window)，6→47A320(fresh window)，1/2/其它不调用。随后fresh mode W32缓存、hover=-1，仅mode1/2/4且fresh selectedSkill.name[0] W8为NUL回退；mode2先4796E0→D67AFD W8--，mode1/2再479660，三种都fresh currentWindow→479B80(0)。之后每写独立fresh currentWindow/elements，6.visible=0，fresh mode1才1；8.visible=0，fresh mode5才1，再独立fresh mode6才1，始终返回1。原92条342B/两页、八call七callee/17普通jump/一七项switch/一ret；现完整正确体保，不合并新mode判断、两更新元素或回退。原54E420原型头不是callback字段，实际54E444=47A620/elements54E440=561B78，源码窗口10对应原型row有据。

### Sub_47B110（0x0047B110）

47B110返回按钮：入口currentWindow T*缓存、elements T*先取，再hover=state S32&1缓存寄存器，之后元素index.data借graphic T*并跨调用持有，consumed0。cached hover时fresh mode1/4复制586488原21B、mode2复制586470原21B，均含首NUL；其它不写hover。点击原47B16C TEST BYTE形参state低W8 mask2，fresh mode1→479B80(借窗,fresh selectedSkill)→479660，2→479C80(借窗,-1)→4796E0→D67AFD W8--，4→479EA0(借窗,fresh skill)，其它不调用子分支；任何点击都声51/x320→consumed1。固定graphic先图3B571，入口cached hover覆3B573，原47B1FA形参低W8 mask8再覆3B572，返回consumed。不让callee改变currentWindow后重借graphic/旧window，不重读hover，不把mask8当mask2。原85条250B/两页、六call六callee/13普通jump/一ret；四片段借element/hover缓存、两W8和尾hover消费。唯一数据xref561B78是30元素数组头，真实index4/32B记录561BF8..561C18/data561C10→561AD0/callback561C14→47B110，源第4项完整值逐字节匹配。

### Sub_4742C0（0x004742C0）

4742C0滚动位置助手：入口借element T*及一次data T*，maximum S32<=0取element.y W32；正时先element.height W32再previous element.height W32，原差×position S32/maximum向零、后fresh element.y加得到y。先写previous element.y，随后fresh读element.y得delta，写入口借data的+8 W32并返回delta。source WindowMouseData.deltaY与WindowGraphicData.offsetY同+8有既有offsetof断言/四字节memcpy，type1调用不能错转仅Mouse结构或新拒绝。原30条69B/一页、无call/一jump/两ret；源归并共同写/返回同正常结果。一片段只两height W32取序，最大值0/负分支、原S32计算与写后再取baseY/别名及返回保，不补钳制/异常或回绕模拟。83代码入向只完整登记，本步仅既有47A000父依赖承接，不全面复查83父。

<a id="battle-skill-window-lifecycle-sampling-acceptance"></a>
## 可验收范围

四原根全部指令字节/边界、原新17直接call/分支与返回、原七表项/42B用途、两真实原型和42B首NUL文本及十二邻窗可追。源码整文件只三体九片段，第四体/既有共享声明及原型保持。语义1530→1534/有效1525→1529、文档关联1643→1647/3680（44.76%）、分类656→660、BATTLE208→212枚举open/E780→782；两局部I/S各十六真实来源。R7 261→262/3061余2800→2799；U08/R01/R03/R04/V04仅当前/引用仍open，差队列172/待归属43和60任务源码closed10/完整验收0/叶0/global=false保。台账后文本汇总器和-Check生成看板/快照，不手改生成件。

不构建、调试、测试、运行或计算摘要。局部关闭/初始化正常源不代表全部生产、子依赖/父与owner寿命、窗口10/26/57C368 token、SDK/生成机器ABI/画面运行及R8完成。音频暂停/网络原格式明文维持。

<a id="battle-skill-window-lifecycle-sampling-source-state"></a>
## 当前交付

四完整260条837B/六页、17直接call/十callee、40普通jump/一实际七项switch/五ret。三体九片段恢复window10 T*取样先userState发布、三action槽按阶段一次T*、返回按钮elements/hover/data顺序和入口hover缓存/两形参W8、助手两height W32取序；正常旧值一致，47A620完整正确体与W32关闭/正常S32计算/别名和旧42B提示保。原型头/数组头数据xref不等callback字段，实际54E444与数组第4项561C14有完整48/32B和源行值；12收尾父邻窗60条305B仅调用相邻序，助手83代码入向只登记。42B X有原七W32目标与2B MOV同寄存器/12NOP对齐，mapped仅字节用途；R7 262/3061余2799。完整生产/其它子依赖/全部父、窗口10/26与57C368 token及owner寿命、SDK/生成机器ABI/画面运行/R8未验。原格式明文、音频暂停与RPLAN.24不新增保护/故障/回绕模拟保持。

命名快照：`cg2d-re-battle-skill-window-lifecycle-sampling-20261007`；唯一进度记实际检查和提交。
