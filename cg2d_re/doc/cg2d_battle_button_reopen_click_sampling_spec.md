# BQL01 战斗按钮再操作点击取样SPEC

2026-10-07。依据[战斗尾段取样](cg2d_battle_tail_sampling_spec.md)、前批快捷入口和选择清理及已开IDA会话[三完整按钮原根](cg2d_battle_button_reopen_click_sampling_evidence.json)，按用户工程委托先记SPEC/裁决再实施。只推进已证点击读取差异及真实按钮分支/基线，原端事实不猜。

<a id="battle-button-reopen-click-sampling-baseline"></a>
## 原端基线与差异

| 原入口 | 新源 | 指令/实际B/页 | call/普通jump/ret | 交付 |
| --- | --- | --- | --- | --- |
| 0x004975A0 | src/ui/WindowRuntime.cpp#Sub_4975A0 | 129/450/2 | 9/18/4 | 一已选分支低W8读取修复 |
| 0x00497770 | src/ui/WindowRuntime.cpp#Sub_497770 | 121/395/2 | 7/14/4 | 一已选分支低W8读取修复 |
| 0x00497900 | src/ui/WindowRuntime.cpp#Sub_497900 | 109/346/2 | 5/13/4 | 一已选分支低W8读取修复 |

三根359条1191B/六页、21call/8callee/45普通jump/十二ret。各唯一原数据xref位于56B710数组头，实际是索引2/3/4的32B原型行56B750/56B770/56B790，callback字段56B76C/56B78C/56B7AC分别指向三个按钮；没有独立代码入向。三份原hover11/11/21共43B含NUL保持。通用构造/事件链已在前批通过，本步只新三个绑定，不重复全链核对，不新R7分类。

| 差异 | 原端实际依据 | 裁决 |
| --- | --- | --- |
| 技能已选分支 | 4976A9 TEST BYTE D68D9C,2 | 原阶段低W8读取，保技能重选/关闭结果消费 |
| 物品已选分支 | 497873同TEST BYTE | 低W8，保短路后W8待选标记/创建存槽/清标记/Flush |
| 宠物已选分支 | 4979F7同TEST BYTE | 低W8，只按原请求window28 W32关闭 |

三个源片段/三源体，正常旧值一致，不称数值/并发或机器故障；edgeButtons共享存储仍uint32，两待选标记原本uint8读写保持。原OR CH的window.state读写是W32寄存器消费，不一起窄化；不将初次state参数点击同已选全局点击读取混合。

<a id="battle-button-reopen-click-sampling-decision"></a>
## 完整正常源合同与裁决

1. 4975A0技能按钮：入口固定graphic T*，先skillUsed W32非0就图3B9C8/返回0；然后窗口26.userState W32非0，默认3B9C7/专属mode[2]非0图3B9C8/返回0。启用consumed0、一次缓存hover=state&1，非0复制原5540F0含NUL11B a8cfa5cea7deafe0a14300。mode[2]0且cached state掩码2设：Close→主mode2先写→mode[2]1后写（与快捷入口先后不同，不合并）→Flush→fresh commandAction MarkDeath→全局null→5ABCC0=0→声51/x320→CB054C W8=0→Sub47A4C0(&Sub4974C0)返回保存CC1A10→consumed1。mode[2]已设则原4976A9实际低W8全局点击掩码2：skillTargetPending D303F8原W8非0时fresh selectedSkill W32、&4974C0传Sub47A500，返回先存CC1A10，pending W8=0后Flush；pending0且window10非空时Sub47A5C0返回非0才mode[2]=0；window10为空则Sub47A4C0创建保存，包括空结果。原六页中的本根两页/9call及三分支结果保存保持，两个47A4C0物理点源两个表达式不合并；没有新增consumed1给原再操作路径。
共同图默认3B9C7、入口cached hover非0覆3B9C9、fresh mode[2]非0覆3B9C8，图写后fresh window10存在则CC1A10=null；否则fresh该槽空且fresh pending W8=0才mode[2]=0，最后返回consumed。原首两拒绝与window存在/普通尾四ret，源归并尾返回保同0/1结果。原pending读写本已uint8，既有W8正确体保；只点击全局edgeButtons原W8恢复，不将共享字段或其它pending扩宽/窄化，不加创建失败成功/保护，不外推完整owner寿命。

2. 497770物品按钮：入口固定graphic，先skillUsed W32非0图3B9D4/返回0，再userState W32非0默认3B9D3、mode[3]非0图3B9D4/返回0。启用consumed0；cached state掩码1复制原5540E4含NUL11B a8cfa5ceaaabab7ea14300。mode[3]0且state掩码2设：Close→主mode3→mode[3]1→Flush→fresh commandAction MarkDeath→清全局→5ABCC4=0→声51/x320→CB054C W8=0→CreateWindowAnimation(27,1,0,null)返回存D2F790→consumed1。mode[3]已设，原497873实际低W8全局点击掩码2设且itemTargetFlag CC4D40原W8非0才创建同27返回先存D2F790，flag W8=0后Flush；该原重开路径不新增consumed1。已有flag uint8读写正确体保，源短路次序同原，不宽读鼠标点击位。
共同图默认3B9D3、cached state1覆3B9D5、fresh mode[3]非0覆3B9D4；之后fresh window27存在则D2F790=null，否则fresh槽空且fresh flag W8=0才mode[3]=0，返回consumed。图先于尾清mode，不重新计算图；四原ret/7call、实际空结果和标记读写/Flush前后/别名次序保。只有点击全局edgeButtons原W8实修。

3. 497900宠物按钮：入口固定graphic，先skillUsed W32非0图3B9CB/返回0；再userState W32非0默认3B9CA、mode[4]非0图3B9CB/返回0。启用consumed0，state1复制原5540CC含NUL21B a56cb3eac364aaaba9ceabcaa64cc364aaaba14300。mode[4]0且state2设：Close→主mode4→mode[4]1→Flush→fresh commandAction MarkDeath→全局null→声51/x320→CreateWindowAnimation(28,1,0,null)返回存D2D60C→consumed1；原不清5ABCC0/5ABCC4/CB054C，也不套入物品待选标记分支。mode[4]已设时4979F7实际低W8全局点击掩码2设，借当前window28一次判空，非空window.state W32缓存|8000写回（原OR CH寄存器，不窄为内存W8），只请求关闭，没有新创建/标死/consumed1。
共同图默认3B9CA、cached state1覆3B9CC、fresh mode[4]非0覆3B9CB；图写后fresh window28存在则D2D60C=null，否则fresh槽空就mode[4]=0，无额外pending条件，返回consumed。原四ret/5call、空分支/创建结果/关闭请求W32及图优先级/尾次序保持，只全局点击位原W8恢复。

三完整按钮原根359条1191B/六页、21call/8callee、45普通内部jump/十二ret。各唯一原数据xref来自56B710数组头，实际索引2/3/4三条32B prototype及各data/callback字段、11/11/21共43B旧hover含NUL可追；没有直接代码入向，不虚构call。只三个已选分支全局鼠标掩码2读取恢复W8，三个表达式对三个原点，共享edgeButtons存储仍W32，正常旧值一致，不称数值/并发或机器故障。原W8 pending/标记、skillUsed与userState门禁、主mode先专属mode后写/初始点击创建、技能重选/重建/关闭结果消费、物品待选重开/宠物W32关闭请求差别、各真实创建结果/图先尾清/返回与旧字节保持。既有窗口26三个元素绑定有据，通用初始化/事件链前步已过不重验，不代全部14元素/99窗口/57C368 token/生产与owner寿命、SDK/生成机器ABI/画面运行。无新R7/原型表分类，音频暂停、网络原格式明文、RPLAN.24不专项异常/保护/回绕/故障等价与原标准保持。

<a id="battle-button-reopen-click-sampling-acceptance"></a>
## 可度量的限定验收

- 三完整根359条/1191B、六页、21call/8callee/45jump/十二ret、三数据xref与三32B行/三份旧hover共43B可追，数据入向不当代码call。
- 整WindowRuntime只三个原全局点击掩码2低W8片段，保共享W32/原W8待选标志、技能重选/重建/关闭返回、物品重开flag/Flush、宠物W32关闭请求、初始点击/模式写序/创建空结果和图先尾清/返回差别。
- 新增1517..1519三原已有完整正常定义/equivalent，三冻结定位全文保。语义1517→1520/有效1512→1515、合同关联1630→1633/3680（44.38%）、BATTLE194→197枚举open、分类642→645/E774→776；两局部I/S各十五实际来源。
- R7 260/3061/余2801、差队列172/待归属43及其它全部机器对象/完整门禁保持。U08/R01/R04/V04只当前/引用仍open，60源码closed10/完整验收0/叶0/global_accepted=false保持；完整UI父/窗口26/owner寿命/运行及完整战斗/R8未完成。

<a id="battle-button-reopen-click-sampling-source-state"></a>
## 当前交付

三个已选点击读取实修，三完整按钮正常基线/专属再操作分支及原型绑定纳管；命名快照 `cg2d-re-battle-button-reopen-click-sampling-20261007`。定向静态与measure/-Check结果记唯一进度；未编译/调试/测试/运行/摘要，音频暂停、网络原格式明文与原适用验收标准保持。
