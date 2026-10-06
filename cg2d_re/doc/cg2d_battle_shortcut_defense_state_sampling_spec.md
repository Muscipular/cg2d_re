# BQK01 战斗快捷入口与防御状态取样SPEC

2026-10-07。依据[战斗尾段取样](cg2d_battle_tail_sampling_spec.md)、前批选择清理与已开IDA会话[四完整原根](cg2d_battle_shortcut_defense_state_sampling_evidence.json)，推进三个快捷输入状态的读取宽度缺口及正确防御入口基线。按用户工程委托先记SPEC/裁决再实施，原事实按实际指令不猜。

<a id="battle-shortcut-defense-state-sampling-baseline"></a>
## 原端基线与差异

| 原入口 | 新源 | 指令/实际B/页 | call/普通jump/ret | 交付 |
| --- | --- | --- | --- | --- |
| 0x00497000 | src/ui/WindowRuntime.cpp#Sub_497000 | 38/174/1 | 5/7/1 | 一低W8→S8修复 |
| 0x00497100 | src/ui/WindowRuntime.cpp#Sub_497100 | 41/181/1 | 5/7/1 | 一低W8→S8修复 |
| 0x00497210 | src/ui/WindowRuntime.cpp#Sub_497210 | 39/156/1 | 5/7/1 | 一低W8→S8修复 |
| 0x004973C0 | src/ui/WindowRuntime.cpp#Sub_4973C0 | 79/256/1 | 5/6/2 | 正确体保持 |

四根197条767B/四页、20call/7callee/27普通jump/五ret。快捷九代码入向来自473840和4C7AE0，九实际前/call/后邻窗27条119B；新源每父各一表达式承接每快捷入口的三物理调用路径，不按字面数虚增调用或重验完整父。4C7AE0当前源名ProcessWindowShortcuts，以冻结定位为准。防御另1数据xref指向56B710数组头，实际是第二行56B730..56B750中的callback56B74C，不读取数组头类型值当函数地址；15B旧hover和2B原G含NUL保存。

| 读取差异 | 原端实际依据 | 裁决 |
| --- | --- | --- |
| 技能快捷 | 497027 MOV AL，TEST AL/与2比较 | 低W8一次读→bit_cast S8，同值两次比较 |
| 物品快捷 | 497127 MOV AL及比较 | 同上，不先W32截断 |
| 宠物快捷 | 497233 MOV AL及比较 | 同上，保原专属尾差别 |

三个源片段/三源体、三个低W8表达式对应三个原点。actionFlags字段当前存储为uint32，保留供其它消费者；正常旧值一致，不称数值/并发或机器故障。防御源正确体不改，网络依已批准原格式明文G，不新增或改命令、协议、加解密。

<a id="battle-shortcut-defense-state-sampling-decision"></a>
## 完整正常源合同与裁决

1. 497000技能快捷：三个快捷入口均先窗口26.window非空，再userState.value W32恰好0，再currentPosition S32<20，按原短路次序；不加负位置拒绝。其后一次实际W8取actionFlags低字节并按S8解释，同一值先<0、再>2拒绝。现字段存储为uint32，旧static_cast<uint8>会先宽读；改为unsigned byte内存读取→bit_cast<int8>，保同一值两个比较以及共享W32字段供其它消费者。不读取signed char别名，不加入新状态/异常门槛。再依次gBattleSkillUsed W32为0、当前专属selectionModes[index] W32为0才处理，物品/宠物也保原技能已用门槛。CloseAllSubWindow→独立写对应mode槽1及主mode编号→Flush20槽→fresh commandAction MarkDeath→清全局槽，原失败返回/弃返/所有权与调用先后保持。声51/x320，原第三Y240未消费，音频暂停。所有路径唯一ret，当前两UI父六源表达式分别承接原九物理入向，每实际路径调用次数保持，未因此验收完整父流。 专属selectionModes[2]/主mode2；MarkDeath后清commandAction→5ABCC0=0→声51→CB054C W8=0→Sub47A4C0(&Sub4974C0)返回保存技能面板CC1A10，包括真实空结果，不硬造成功或new窗口。原字段/正常流程正确体保，只497027低字节读实修。

2. 497100物品快捷：三个快捷入口均先窗口26.window非空，再userState.value W32恰好0，再currentPosition S32<20，按原短路次序；不加负位置拒绝。其后一次实际W8取actionFlags低字节并按S8解释，同一值先<0、再>2拒绝。现字段存储为uint32，旧static_cast<uint8>会先宽读；改为unsigned byte内存读取→bit_cast<int8>，保同一值两个比较以及共享W32字段供其它消费者。不读取signed char别名，不加入新状态/异常门槛。再依次gBattleSkillUsed W32为0、当前专属selectionModes[index] W32为0才处理，物品/宠物也保原技能已用门槛。CloseAllSubWindow→独立写对应mode槽1及主mode编号→Flush20槽→fresh commandAction MarkDeath→清全局槽，原失败返回/弃返/所有权与调用先后保持。声51/x320，原第三Y240未消费，音频暂停。所有路径唯一ret，当前两UI父六源表达式分别承接原九物理入向，每实际路径调用次数保持，未因此验收完整父流。 专属selectionModes[3]/主mode3；MarkDeath后清commandAction→itemTargetPending5ABCC4=0→声51→CB054C W8=0→CreateWindowAnimation(27,1,0,null)返回保存D2F790，包括空结果；原4971A0 W8清零发生创建调用之前，保正确顺序。只497127低字节读实修，不把物品目标清零移到其它入口。

3. 497210宠物快捷：三个快捷入口均先窗口26.window非空，再userState.value W32恰好0，再currentPosition S32<20，按原短路次序；不加负位置拒绝。其后一次实际W8取actionFlags低字节并按S8解释，同一值先<0、再>2拒绝。现字段存储为uint32，旧static_cast<uint8>会先宽读；改为unsigned byte内存读取→bit_cast<int8>，保同一值两个比较以及共享W32字段供其它消费者。不读取signed char别名，不加入新状态/异常门槛。再依次gBattleSkillUsed W32为0、当前专属selectionModes[index] W32为0才处理，物品/宠物也保原技能已用门槛。CloseAllSubWindow→独立写对应mode槽1及主mode编号→Flush20槽→fresh commandAction MarkDeath→清全局槽，原失败返回/弃返/所有权与调用先后保持。声51/x320，原第三Y240未消费，音频暂停。所有路径唯一ret，当前两UI父六源表达式分别承接原九物理入向，每实际路径调用次数保持，未因此验收完整父流。 专属selectionModes[4]/主mode4；MarkDeath后清commandAction→声51→CreateWindowAnimation(28,1,0,null)返回保存D2D60C，包括空结果。原没有5ABCC0/5ABCC4或CB054C清零，不能照搬技能/物品尾；只497233低字节读实修。

4. 4973C0防御callback：index/state S32，入口固定graphic T*来自currentWindow.elements[index].data。窗口26.userState.value非0，graphic默认3B9D0，fresh selectionModes[1]非0覆3B9D1，返回0；不处理hover/click。启用consumed0，cached state bit1复制原554128含NUL15B b4b6b371aaabb27aa8bebf6da14300（现strcpy转义字面正确）；bit2先Close→selectionModes[1]=1→主mode0→Flush→fresh connectionSocket及原554124字符串4700即G调用SendBattlePacket弃返→fresh commandAction MarkDeath→全局null→playerActionState5ABC74 W32=1→声57/x320→consumed1。原发送失败也不添加早退，按已批准原格式明文网络边界承接G，不自创命令/改格式或加解密；该UI源接线不外推完整组包/连接/发送父验收。入口固定graphic始终保留，最后默认3B9D0、cached state bit1覆3B9D2、fresh selectionModes[1]非0覆3B9D1，再返回consumed。原五call/两ret、禁用与悬停/点击/图优先级/尾正确体保持，无CB054C清零，不能照搬攻击入口。唯一数据xref在56B710数组头，实际第二个32B原型行56B730..56B750，data56B748→56B610、callback56B74C→4973C0；源窗口26的gWindowElements56B710[14]第二项graphic=&gWindowGraphic56B610、callback=&Sub4973C0有据。原型32B与runtime36B/+20 callback不混，既有OpenWindow471440发布/473560消费保持，不重验前步已过的通用链，也不推全部14元素/99窗口/57C368 token/owner寿命或鼠标运行。

四完整原根197条767B/四页、20call/7callee、27普通内部jump/五ret；三个快捷入口九代码入向来自473840/4C7AE0，九实际邻窗27条119B，防御另1数据xref及第二个32B原型行/15B原hover/2B命令G有据。只三个快捷源体各一输入状态低W8→S8片段，三个表达式对三个原点，正常旧值一致，不称数值/并发或机器故障。三入口原门槛/侧别模式2-3-4、各自清零差别/真实创建结果、关闭清理/回收/声/返回及防御正确体/图优先级/原字节/网络原格式明文G保持。共享actionFlags存储W32及其它已证接线保持，不新R7/原型表分类。只当前完整正常源与局部父/既有callback，完整UI父/全部生产与owner寿命、SDK/生成机器ABI/组包/连接发送/画面运行未验；音频暂停、RPLAN.24不专项异常/保护/回绕/故障等价与原适用标准保持。

<a id="battle-shortcut-defense-state-sampling-acceptance"></a>
## 可度量的限定验收

- 四原根197条/767B、四页、20call/7callee/27jump/五ret，快捷九入向/九窗27条119B，防御1数据xref/第二个32B行/15B旧hover/2B G可追。
- 整WindowRuntime仅三个输入状态低W8→S8片段，共享W32存储/原门槛/模式2-3-4、各专属清零差别与创建结果、关闭清理回收/声/返回、防御图优先级/原字节/网络原格式明文G保持。
- 新增1513..1516四原已有完整正常定义/equivalent，四冻结定位全文保。语义1513→1517/有效1508→1512、合同关联1626→1630/3680（44.29%）、BATTLE190→194枚举open、分类638→642/E772→774；两局部I/S各十七实际来源。
- R7 260/3061/余2801、差队列172/待归属43及其它全部机器对象/完整门禁保持。U08/R01/R04/V04只当前/引用仍open，60源码closed10/完整验收0/叶0/global_accepted=false保持；完整UI/owner寿命/组包连接/画面运行与完整战斗/R8未闭合。

<a id="battle-shortcut-defense-state-sampling-source-state"></a>
## 当前交付

三个快捷状态读取实修，四完整正常根及防御正确体/局部接线纳管；命名快照 `cg2d-re-battle-shortcut-defense-state-sampling-20261007`。定向静态与measure/-Check结果记唯一进度；未编译/调试/测试/运行/摘要，音频暂停、原格式明文与原验收标准保持。
