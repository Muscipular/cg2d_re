# 战斗控制、附属清理与音效入口完整合同 SPEC（BCLC01）

2026-10-10；动作75–83九完整正常根。全部现正文/签名/数值/调用正确保持，仅十八合同注释；动作73/74已有BQH01旧1503/1504完整合同，本步保持，不重复登记。

<a id="battle-cleanup-controls-baseline"></a>
## 完整原根及局部分派

| 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / CALL |
|---|---|---|
| F:0000CB10 / UpdateBattleAction75_40CB10 | 40CB10–40CB29 | 7 / 25B / 1 |
| F:0000CB30 / UpdateBattleAction76_40CB30 | 40CB30–40CB49 | 7 / 25B / 1 |
| F:0000CB50 / UpdateBattleAction77_40CB50 | 40CB50–40CB7F | 17 / 47B / 3 |
| F:0000CB80 / UpdateBattleAction78_40CB80 | 40CB80–40CBAF | 17 / 47B / 3 |
| F:0000CBD0 / UpdateBattleAction79_40CBD0 | 40CBD0–40CC76 | 47 / 166B / 4 |
| F:0000CC80 / UpdateBattleAction80_40CC80 | 40CC80–40CCA7 | 15 / 39B / 2 |
| F:0000CCB0 / UpdateBattleAction81_40CCB0 | 40CCB0–40CCD7 | 15 / 39B / 2 |
| F:0000CCE0 / UpdateBattleAction82_40CCE0 | 40CCE0–40CD07 | 15 / 39B / 2 |
| F:0000CD10 / UpdateBattleAction83_40CD10 | 40CD10–40CD6C | 26 / 92B / 2 |

共166条519B/九份最多64条分页、20E8/五唯一callee、七跳十一RET；无间接CALL/JMP。九根各唯一实际入向CALL/同父404DB0，九case各11B push→CALL→JMP40563F，标签75–83完整取齐。77/78各两静态Advance点/互斥分支，每条正常路径仅一次推进，不伪记两次实际调用。83保末PlayFrame S32返回；其它当前void/真实弃返保持。局部分派不代全父/ABI/owner。

## 75–78：1004与设备绘制位

75固定payload，完整1004=1后Advance；76同槽=0后Advance，两根无Play/角色动作/速率发布。77先真实设备查询，再fresh完整flags，有设备OR100000h，无设备OR8000h，整字段发布后Advance；78依查询结果仅清相应位（~100000h或~8000h）、整字段发布后Advance。保其它位，不一次清两位/采用XOR；设备调用前不预取flags。现SDL3渲染设备边界沿既有豁免，原AH局部运算后的完整W32发布由现整字段掩码承载，不改提供者/新增DX机制或条件播放。

## 79：五槽清理、三项循环及两标志持样

固定一次payload，先fresh centeredEffect1064，非空MarkDeath后清1064；再fresh rebirthEffect106C，非空MarkDeath后清106C。随后按1074/1078/107C原三个非拥有T*槽顺序逐项读取，非空才MarkDeath并清当前槽，空槽继续后续；原三次循环/4B x86指针步长对应当前std::array<YobiAction*,3>按x64真指针迭代，不加减项数，不提前持五个值/重排或立即free。三个静态标死点中循环点最多正常调用三次，不能只按静态三CALL说只有三个对象。

清理后characterAction0，再完整W32 FDC与FE0分别读齐并各清bit2，按FDC→FE0整字段发布，最后Advance。不能在FDC发布后才读取FE0、缩成BYTE发布或清其它位。现正确两个局部持样和发布序保持；1064中心创建BDE01旧2104号、重生BRAE01旧2059号及其余生产/最终owner仅关联，清槽/标死不代借用/物理释放与重入退出全验收。

## 80/81/82：三个独立音效字段

均固定payload，再持当前X、取对应完整S32音效字段，PlaySe(sound,heldX)后Advance。80为1018，81为101C，82为1020，不能合并为1010/统一sound槽或重排X/音效读序。原Y先取但现二参WAV边界不消费额外Y，沿已有批准差异不恢复第三参/MIDI、播放门禁/RNG或私有音源；实际资源/声音结果和生命周期仍待。现声1899号及真实SDK源保持，不新增包装/API。

## 83：未舍入阈值、推进后fresh载荷

以54D49C原F32步长3BA3D70A（约0.005）加fresh scaleX，保存未舍入中间；然后characterAction9→F32 scaleX发布→fresh scaleY加步长/F32发布。比较仍用未舍入scaleX对54D498原F32阈值3FA66666（约1.3），正常>=才Advance；调用后重新读取action.payload，再fresh完整FDC OR1000h整字段发布，不预取/复用入口载荷或把flags发布提前推进。没有新增封顶/归零、rate写入或一次性门禁。

每条正常路径最后PlayFrame(action,0)，保S32返回；现double中间/float字段及正常宿主数值边界不变，不在本步扩大数值许可或模拟异常/溢出。两原常量合8B完整实读/位模式核对，未引新数据定义/重计X。原CH置10h前完整DWORD取数、后完整DWORD发布，现W32 OR1000h保持其它位。

<a id="battle-cleanup-controls-decision"></a>
## 实施与登记边界

依第8条既有委托先SPEC后落实：一CPP九定义注释、公头九注释；九完整正文/全部签名/数值/调用/其它源及原三槽声明、所有旧语义/处置保持，零消费/算法改动、新函数/API/保护/默认值/声/播放/RNG或业务重构。九原身份此前无独立语义/F处置/已知BATTLE成员，新增九adapted语义/处置/成员，枚举false。

一限定S九完整当前正常源/局部分派/原常量/清理与资源依赖，不新增I或计新全实现。U08/U09/R01/R04/V04补来源/局部状态且open，原分母/门禁/政策/旧记录/六closure保持。MSVC CRT、WAV-only、原格式明文和RPLAN.24保持；CRT排序待此前选择，当前CRT文档另有压缩修改及原六项用户改动均保留、不夹带。不构建/测试/运行/算法/产物/摘要或引依赖，不降低总SPEC§10。

<a id="battle-cleanup-controls-acceptance"></a>
## 必要静态及剩余

核九完整166条519B/九页/库存、20E8/五callee/七跳十一RET、九真实入向及九11Bcase，77/78两互斥静态点；79五槽/三项循环、五对象读取/标死/清槽顺序与两个W32持样/发布；三个独立音效字段/X取样顺序；83两4B常量、未舍入比较/角色动作及F32发布、推进后payload/FDC重取和共同PlayFrame(0)。

核整CPP/公头仅十八注释，其它所有本体/签名/数值/源/三槽声明与复用旧记录保持；九语义/F/成员、一限定S零I、五open任务及所有原政策门禁，UTF-8无BOM/LF/新SPEC单一末尾LF、差异和文本汇总/-Check。完整1004/1064/106C/1074/音效/scale生产、其它父/全部callee/ABI/alias/owner、SDK正常失败/资源/容量/数值/重进退出及声音画面运行仍待，总SPEC§10不降。

<a id="battle-cleanup-controls-source-state"></a>
## 当前状态

九完整原根/两常量/case及当前源已取齐，正文全保仅十八注释；必要静态、文本汇总/-Check与精确派生范围通过。语义2135（2087F＋48X）、合同2197/3680=59.70%（仅关联率）、F1563、E1393仅一限定S零I、BATTLE375且枚举false；五任务open、旧记录/原分母/全部政策门禁保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-cleanup-controls-history-20261010，提交引用cg2d-re-battle-cleanup-controls-20261010。整体SPEC未完成。
