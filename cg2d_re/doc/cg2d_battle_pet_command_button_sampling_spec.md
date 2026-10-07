# BQT01 宠物命令按钮、普通与骑乘回调SPEC

2026-10-07。沿[BQR02 selector发布](cg2d_battle_skill_state_producer_sampling_spec.md#battle-skill-state-producer-sampling-source-state)和[BQS01三个选择consumer](cg2d_battle_pet_skill_selection_sampling_spec.md#battle-pet-skill-selection-sampling-source-state)，推进window26宠物按钮→普通/骑乘命令回调→发送与关闭。依AGENTS.md第8条工程委托先写裁决再实施，[三完整原根与源前后态](cg2d_battle_pet_command_button_sampling_evidence.json)为实际依据。

<a id="battle-pet-command-button-sampling-baseline"></a>
## 完整原基线与差异

| 原入口 | src/ui/WindowRuntime.cpp符号 | 指令/实际B/页 | call/jump/ret | 台账 |
| --- | --- | --- | --- | --- |
| 498160 | Sub_498160 | 109/352/2 | 3/16/2 | 新正常身份 |
| 497E30 | Sub_497E30 | 107/436/2 | 19/5/2 | 既有990完整续记 |
| 497FF0 | Sub_497FF0 | 88/364/2 | 12/3/2 | 既有991完整续记 |

三根304条1152B/六页、34直接CALL/十callee、24普通jump/六RET，无switch/新R7。九入向都是数据关系：按钮一原型头，两个回调各四PUSH地址指令，来源两个父498160/498520；不是九次直接调用。两个命令回调全文正常源已对应，本批不改其正确正文。

| 按钮差异 | 原实际点 | 修复裁决 |
| --- | --- | --- |
| state/window/待展开槽序 | 498190初W32 state→498198借window→hover；关闭提示后4981D1取slot或window空时4981FF取slot，打开提示后跳过slot重取 | state先取、借window后hover；关闭提示→一次pendingAction→打开提示，点击gate用该阶段cache |
| 两W8观察/条件发布序 | 4981E3 TEST BYTE D68D9C,2；498238 MOV AL窗口动作字节；phase1→498247 CMP AL4→index-1→selector | global mouse低W8仅短路需要时取；动作低W8、held riding谓词后pub -1和创建结果 |

一体两源片段，普通值同旧源；不称并发/数值/故障差。入口state是W32取样后寄存器DL测试，仍保W32，不与global BYTE观察混合。窗口状态原OR AH是W32读/写，现state|8000保；共享edgeButtons/actionFlags仍W32存储、只本点W8观察。

<a id="battle-pet-command-button-sampling-decision"></a>
## 正常开关、选择、命令与裁决

1. 按钮先借当前element.graphic T*，window26.userState W32非0直接返0。入口state W32→window10 T*→held hover；hover且window有值先拷关闭提示，再一次pendingAction T*；hover且window空/pending空才拷打开提示。该cache控制click gate，不在提示后fresh slot补读。
2. pending空且入口click2或短路global鼠标W8 click2，已有window只用该held T*请求关闭。window空：Flush→动作state低W8→phase1→held riding=(state==4)→skillIndex=-1→模式4选47A580(&497FF0)，其它选47A540(&497E30)，实际Yobi T*返回存CC423C，真空结果也保，consumed=1。
3. 尾先graphic default3B9C7/held hover3B9C9，再fresh window10或fresh pending槽存在就3B9C8；然后再次fresh window10非空才清CC423C。不能以入口window/pending cache替代尾，也不把clear变为MarkDeath/delete。该槽与前三战斗子窗动作槽独立，整体owner寿命另验。
4. 普通negative：phase0/声57→fresh位置<20才fresh socket发送原W|FF（不扩成无条件）→Flush/第二声57→count取样/command5/count+1→fresh活宠/lastPet→Close/return。普通不清selectionMode，不照搬骑乘。
5. 普通nonnegative：先skill pub→fresh活宠/skill/target S16符号扩展→fresh位置映射→CheckCommand。非0只走尾；0才fresh活宠/skill/techniqueId，>=200600取fresh位置映射，否则fresh活宠；之后fresh skill和held目标构原W|%X|%X/未清256B buffer→fresh socket发送弃返→Flush/声57→count/command5/count+1→fresh活宠last/Close。保所有重读阶段，不把两个活宠结果合成同一个，target掩码S16→W32原位型保。
6. 骑乘negative：先phase0/selectionMode0，再相同负请求发送门禁/Flush/重复声57；写序是旧count→held骑宠→count+1发布→command5→lastPet held→Close/return，与普通不同。
7. 骑乘nonnegative：selectionMode0→held骑宠→skill pub→target S16→fresh当前位置直接Check（不映射）。非0不发送，0才fresh骑宠先fresh skill→原256B/原格式→fresh socket→发送/Flush/声57→旧count/held fresh骑宠→count+1/command5/lastPet→Close。两回调所有nonnegative路径最后都声51/phase2，包括Check0已Close路径；不能用业务直觉加早退、改phase或假成功。
8. 旧命令字节553FB8 W|FF含NUL5B、554060 W|%X|%X含NUL8B共13B唯一字节；每个回调各一格式真实53B304原点，保既有FormatLegacyString接口/两个U32参数次序与256B栈buffer、不补零。网络格式/明文发送不改、不加解密，不据本步验收完整网络互通。
9. 两旧tooltip554078/5871D4各15B含首NUL共30B，保源原字节。window26元素8 [56B810,56B830)32B，data56B628/callback字段56B82C→498160，原数据xref56B710是数组头；现源行/typed声明一致。两command的八PUSH指令为数据引用，传给两个已过selector后由已过consumer调用，不造直接CALL记录。
10. 实际另一父498520已定位761条2882B、74个物理CALL元信息；本步仅登记其六个回调地址PUSH和六个selector CALL，不核全keyboard、本体及倒计时/其它命令。后续沿该真实剩余父推进，不能据元信息计完整父通过。音效保实际两参数57/51与x320，音频暂停；已过Flush/Close/Check/selector不全面重验。

## 当前台账与历史

1558新增按钮正常F；990/991两旧格式记录补完整正常源，保business_*、原引用/格式和边界/指令数，历史全文及旧状态关系存证，只同步当前status/source关系、追加五新引用和完整子基线。三equivalent/三BATTLE成员：语义1559/有效1552、关联1670/3680（45.38%）、分类687/BATTLE239枚举open/E792；无F/有效区间各7、R7 264/3061余2797/差队列172与43待归属保持。

<a id="battle-pet-command-button-sampling-acceptance"></a>
## 可度量验收

- 三完整逐指令连续字节/确切单chunk、304/1152B/六页、34call十callee/24jump六ret；九数据入向/八callback PUSH与真实32B字段。
- 三源体前后冻结，整UI仅按钮两授权片段；两正确命令正文、两个W8观察/原W32参数和槽存储、held/fresh阶段、selector参数/返回、图尾清按原对应。
- 原负/正/Check非0分支、target S16/阈值/位置映射差别、未清256B/13B唯一旧命令字节和原格式实接、socket采样/弃返与重复声/不同计数pub/lastPet/Close/最后phase2对照；两15B旧提示保持。
- 一新F/两旧完整续记与历史业务保、三处置/成员、两局部I/S及四任务U08/R01/R04/V04只本批增量。定向静态/measure/-Check、完整派生快照与编码/引用/diff通过后精确提交12文件。

当前限正常源与本步原字节静态对应。完整keyboard/target后发送阶段、技能/pet数据生产、全部父/窗口/owner寿命、SDK/机器ABI/画面运行与R8另验；音频及不构建测试运行摘要/RPLAN.24保持，不降低原失败分支和适用完整门禁。

<a id="battle-pet-command-button-sampling-source-state"></a>
## 本步交付

先落本SPEC和证据，修一个按钮体两片段、纳管两正确command体，再同步台账/四任务/唯一进度/看板。快照`cg2d-re-battle-pet-command-button-sampling-20261007`；整体SPEC目标保持，不以三静态处置或文档数代完整流程和运行验收。
