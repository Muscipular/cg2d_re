# 创建初始化递增／版本发布与对象取样子SPEC（BST01）

2026-10-07。沿创建controller两个真实初始化入口推进，依AGENTS第8条委托先裁决再实施。controller分派保持，不改已批准的D01固定1入口或制造未变父的语义身份。

<a id="creation-initialization-baseline"></a>
## 原端基线与实际缺口

| 原根 | 完整范围／指令 | 调用／实际caller |
| --- | --- | --- |
| 462870清理后递增 | [462870,46287C)，3条12B／一页 | 404D30后INC同S32场景；唯一4626A0@4626AA |
| 462880创建两个对象 | [462880,462926)，38条166B／一页、四跳转 | 两404AE0(2,0)后44B330(NULL)；4626A0@4626C1／4626DC |

两根41条178B／两页、4直接／三唯一目标、四跳转／两RET，无间接。原controller另完整130条464B／两页、18直接／16目标、30跳转／一RET作上下文；不据此改D01原未知事实、pending7／P08或替代完整父／依赖验收。

462875在YobiClearUp返回后INC当前S32场景，源却位型转U32加1再转回；依RPLAN.24撤普通递增回绕模拟，用既有场景S32的++，不提前读场景跨清理调用。

reset低W8非零时，462888取Puk U8，46288D先发布9A57E0=0，462894／462896 unsigned比较5，满足才462898写1，最后清两其它BYTE。现三目表达式只写最终0／1，省掉>=5时原初始0发布；恢复单Puk样本及原写序，不把版本取样提前到reset为0分支。

两创建返回分别4628B9／4628F1发布共享槽，4628BE／4628F6判返回EAX、4628C2／4628FA仍借EAX写+30。源发布后重读共享槽用于判空和+30；各以非拥有firstAction／secondAction复用返回值，保发布位置与空返回。

关键保持：原+34与+2B8确实再次读取共享槽，分别4628C9／4628D5和462901／46290E；不能把三个字段都缓存成创建返回。原517／243／396／5、两个slot先后和失败后仍继续第二对象／InputSetContext(NULL)保持。当前普通值正确，本步修数据流和写序，不称运行故障。

双X:0006287C-00062880四字节／X:00062926-00062930十字节，共14B全90逐BYTE真NOP，无入向、各前RET后真入口，只原对齐用途。

<a id="creation-initialization-decision"></a>
## 实施裁决与保持项

单源四精确片段／两既有体：清理后S32 ++scene；Puk样本→先0／条件1→两BYTE清零；各NewYobiAction返回值保共享槽发布，判空和第一字段沿该返回样本，后两字段仍现场重读共享槽。使用既有强类型T*与字段helper，无新owner／全局／句柄／接口。

原两槽均native非拥有，三选择字段／reset／Puk均W8，scene与三动作字段均S32，非地址不扩宽。保持两创建失败、+30／+34／+2B8值和写序、最终输入清空、controller整个正文及D01已批准接线、所有头／场景状态／Yobi／输入边界／业务。完整原门禁寄存器仍未知，不改称原常量1，不将此局部修复关闭pending7或完整CHARACTER门禁。

RPLAN.24不专项模拟或处理异常／越界／溢出／回绕，不新增拒绝／保护／补零／钳制／输入限制。网络兼容原格式且只明文，音频暂停、无RTTI／外依赖和既定排除／执行限制保。

<a id="creation-initialization-acceptance"></a>
## 可度量验收

1. HEAD整源按四唯一from→to构expected逐字相等；两个return样本只供各判空／+30，原后四共享槽重读和字段值／pub位置不删，Puk先0／条件1和清理后++对应原点。
2. 两原根41条178B／两页、4直接／三目标／四jump／两RET，分别真实入向及双14B对齐用途归档；controller上下文单列，不混入子原语数量或据此宣完整门禁。
3. 新1600／1601分别对应F:00062870／F:00062880与两adapted，CHARACTER15→17；语义1600→1602／有效1593→1595，文档1711→1713／3680（46.55%）、处置735→737、E856→858；R7 306→308／剩2753；两局部I-S各19实来源。
4. U11／V03／R04只current／引用仍open；P08原closed、pending7整个原记录与pending18、P05原closed／closed11／完整叶0／globalfalse及全部完整门禁、冻结定位／差队列保持。四集中静态、measure／-Check同句柄终态、完整派生快照和12文件UTF-8无BOM-LF／四锚点／真实引用／差异通过后记结果。

完整创建controller／三callee及字段／输入消费接线、所有正常alias-owner寿命／请求回复／图像文本、CRT-SDK／机器ABI／链接、人物创建-主地图／明文往返／画面／运行R8未验；局部不代整体完成，不构建／测试／运行或计算摘要。

<a id="creation-initialization-source-state"></a>
## 实施追踪

预定Git命名快照：cg2d-re-creation-initialization-sampling-20261007。对应[原证据](cg2d_creation_initialization_sampling_evidence.json)、[人物场景](cg2d_character_select_scene.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。集中静态与派生终态后追加实际结果。

实际结果（2026-10-07）：单源四片段／两初始化原语已实施，清理后普通递增、版本先零／条件一、两个factory返回样本供判空和+30；原后四共享槽重读／失败／输入尾与整个controller保持。41条178B／两页、三个实际controller调用点和双14B用途证据落地；controller130条464B仅上下文，D01固定1／pending7整个原记录与P08原closed保持。四集中静态、measure与-Check同句柄终态0、完整派生快照和12文件编码／四锚点／两局部I-S各19实来源引用／差异通过。新1600／1601两F／adapted／CHARACTER两成员、双用途与局部I-S落实；语义1602有效1595／文档1713（46.55%）／737处置／E858／R7 308余2753。U11／V03／R04仍open，P05原closed及全部完整门禁保持；完整创建controller／三callee／字段输入消费与全部正常alias-owner寿命、人物创建-主地图／链接／明文互通／画面／运行及整体验收未达成。
