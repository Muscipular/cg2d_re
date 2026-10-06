# BQJ01 战斗选择清理与命令入口取样SPEC

2026-10-07。沿[战斗尾段取样](cg2d_battle_tail_sampling_spec.md)、上一批命令目标及既有验收体系，根据已开IDA会话[三完整原根](cg2d_battle_selection_cleanup_entry_sampling_evidence.json)推进缺口。用户工程委托下先记SPEC/裁决再修改，原端事实按实际指令不猜。

<a id="battle-selection-cleanup-entry-sampling-baseline"></a>
## 原端基线与差异

| 原入口 | 新源 | 指令/实际B/页 | call/普通jump/ret | 交付 |
| --- | --- | --- | --- | --- |
| 0x0043C060 | src/YobiRuntime.cpp#FlushBattleEntries_43C060 | 20/56/1 | 0/2/1 | 修复1处 |
| 0x00496F10 | src/ui/WindowRuntime.cpp#Sub_496F10 | 46/149/1 | 3/6/1 | 修复3处 |
| 0x004972B0 | src/ui/WindowRuntime.cpp#Sub_4972B0 | 79/262/1 | 5/6/2 | 正确体保持 |

三根145条467B/三页、8call/5callee/14普通jump/四ret。Flush32、Close13共45代码入向，45邻窗各实际前指令/call/后指令，共135条693B；只捕实际接线邻域，不推完整UI父控制流。callback唯一数据xref位于56B710结构头，实际32B行的56B72C字段才是4972B0；56B728为56B5F8图形源，结构头是W32 type1。现窗口26原型/首元素及既有初始化、事件消费正确；该源无独立直接call，不虚构代码入向。原hover554138含NUL15B逐字节记录。

| 差异片段 | 原端依据 | 裁决 |
| --- | --- | --- |
| Flush首借用 | 43C06B MOV ECX→TEST及43C071/43C077首读写 | 同action判空与AND，保43C07A/43C084后两次fresh槽 |
| Close技能指针 | 496F50 MOV EAX→TEST→PUSH EAX | 当阶段skillWindow一次借用，MarkDeath后清原全局 |
| Close道具指针 | 496F6C同序 | 前一步结束后独立itemWindow借用，不齐取三槽 |
| Close宠物指针 | 496F88同序 | 前一步结束后独立petWindow借用，保空分支不清 |

四片段/两源体；正常旧值一致，不称数值/并发或机器故障。九个S32清零、窗口state W32缓存及寄存器CH修改、Flush AND/OR/cacheIndex S32、各标死清槽和callback正确体保持。

<a id="battle-selection-cleanup-entry-sampling-decision"></a>
## 完整正常源合同与裁决

1. Flush43C060：原59E980..59E9D0前20槽，源20槽span直接借22槽数组前20；每槽先一次共享T*读取、同对象判空，非空才同cached action.flags W32读→AND~8→W32写。之后独立重新从同共享槽取对象，flags再W32读→OR4→W32写；之后第三次重新同槽取对象cacheIndex S32=-2。原43C06B取ECX供43C06D判空及43C071/43C077首读写，43C07A/43C084才重新取槽；源新增action局部只恢复首借用，不将后两次都换成cached对象。无调用/新空值与边界门槛，不扩大到22槽，不把两W32写合成一次；唯一ret保。

2. CloseAllSubWindow496F10：九个S32选择模式清零（原ECX9/EAX0/REP STOSD，从5ABC7C起36B），源std::array<int32,9>.fill(0)正确体保。按顺序借窗口10、27、28 T*，每个非空都window.state W32读写|8000；原OR CH,80是W32缓存寄存器修改，不能窄成内存W8写。三个技能CC1A10/道具D2F790/宠物D2D60C槽分别当阶段一次借T*，同一个对象判空并MarkDeath弃返，然后只在该非空分支清相应全局槽null。第二/第三槽在前一个MarkDeath/清槽之后才独立读取，不能齐取三个、再次全局取对象或无条件清原空槽。原三个MOV EAX→TEST EAX→PUSH EAX，源分别skillWindow/itemWindow/petWindow恢复，均非拥有借用。全部真实空/非空分支、窗口顺序/九字段范围、标死/清槽次序及唯一ret保；完整销毁owner链不因此闭合。

3. Sub4972B0窗口26普通攻击callback：index/state S32。原先gCurrentWindow.elements[index].data借固定type1的WindowGraphicData T*（原runtime元素36B/+1C，prototype为32B/+18源/+1C回调，两布局不混）。窗口26.userState.value W32非0时graphic.imageNo=3B9C4，独立selectionModes[0] W32非0覆盖3B9C5，返回0，禁用路径不走hover或click。否则consumed0，cached state bit1有则复制原554138旧字节字符串b4b6b371aaabb27aa7f0c0bba14300含NUL15B到gHoverTextD68AE8，现strcpy字面量正确，不改编码/容量。state bit2有按原Sub496F10→selectionModes[0]=1→selectionMode5ABC70=0→Flush43C060→fresh currentPosition/flags475 CheckCommand弃返→fresh commandAction MarkDeath→commandAction=null→声51/x320（原第三Y240未消费）→CB054C W8=0→consumed1。graphic是入口固定借用，调用后不重取；最后graphic默认3B9C4，cached state bit1设覆盖3B9C6，再fresh selectionModes[0]非0覆盖3B9C5，返回consumed。原79条/五call/两ret，禁用与启用路径/图优先级/Click先关闭清理再选择/两旗值/原字节hover和声返回边界正确体保持，不改此源体。
原唯一数据xref从56B710元素结构头标记，实际32B行的callback字段是56B72C值4972B0，源字段56B728值56B5F8；不能把结构头W32类型1当函数指针。现gWindowElements56B710[14]首项graphic=&gWindowGraphic56B5F8、mouseCallback=&Sub4972B0，窗口26原型实际引用此数组。OpenWindow471440真实发布element.mouseCallback，Sub473560在visible==1时实际按(index,events)调用并消费非0；只是该已存在元素接线有据，不外推全部14元素、99窗口/57C368+04token、完整生命周期/鼠标运行或父流验收。

三完整原根145条467B/三页、8call/5callee、14普通内部jump/四ret。Flush32/Close13共45代码入向45窗135邻指令/693B；callback另1数据xref、实际32B行字段56B72C及15B原hover字节有据，32B prototype与36B runtime元素不混。四源码片段/两源体：Flush首借用供判空/AND、Close三个分别借用供判空/MarkDeath；正常旧值一致，不称数值/并发或机器故障。后两次fresh槽、各回收之后下一槽取样、原W32写/九S32清零/10-27-28窗口序、原空分支、callback正确体/图优先级/选择链/原字节/返回保持。只当前正常源及该prototype/发布/消费接线，不代45完整UI父/全部子依赖、其它生产与owner寿命、SDK/生成机器ABI/画面运行；无新R7/数据表分类，音频暂停、网络原格式明文、RPLAN.24不专项异常/保护/回绕/故障等价与适用标准保持。

<a id="battle-selection-cleanup-entry-sampling-acceptance"></a>
## 可度量的限定验收

- 三完整根145条/467B、三页、8call/5callee/14jump/四ret、45代码入向/45窗135条693B、1数据xref及实际32B行/15B旧字节可追。
- 整YobiRuntime与WindowRuntime分别只一/三片段；首次缓存及后两次fresh、三个阶段独立借用/清槽、九S32/三W32窗口请求及callback正确图优先级/调用链/原字节与返回保持。
- 新增1510..1512三原已有完整正常定义/equivalent，三冻结定位全文保。语义1510→1513/有效1505→1508、合同关联1623→1626/3680（44.18%）、BATTLE187→190枚举open、分类635→638/E770→772；两局部I/S各十七个实际来源。
- R7 260/3061/余2801、差队列172/待归属43及其它全部机器对象/完整门禁保持。U08/R01/R04/V04只当前/引用仍open，60源码closed10/完整验收0/叶0/global_accepted=false保持；窗口26/全部14元素、99窗口/57C368 token/全部owner寿命和完整战斗/R8未闭合。

<a id="battle-selection-cleanup-entry-sampling-source-state"></a>
## 当前交付

两源文件四借用阶段实修，三完整正常根与该既有callback原型/发布/消费纳管；命名快照 `cg2d-re-battle-selection-cleanup-entry-sampling-20261007`。定向静态及measure/-Check记录唯一进度；未编译/调试/测试/运行/摘要，音频暂停、原格式明文与原适用验收标准保持。
