# 战斗等待、交替动作与转向完整合同 SPEC（BWT01）

2026-10-10；补动作37–44八完整正常根及静态目标表。现交替音效/两次方向增量/三次播放和目标转向正确，仅37/38两低BYTE表达及十六合同注释、独立登记本步落实，不新增函数/API/保护/音效/移动/播放/RNG。

<a id="battle-wait-turn-baseline"></a>
## 完整原根与分派

| 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / 原CALL |
|---|---|---|
| F:0000BB20 / UpdateBattleAction37_40BB20 | 40BB20–40BB68 | 24 / 72B / 两CALL两callee、两跳一RET |
| F:0000BB70 / UpdateBattleAction38_40BB70 | 40BB70–40BBB8 | 24 / 72B / 两CALL两callee、两跳一RET |
| F:0000BBC0 / UpdateBattleAction39_40BBC0 | 40BBC0–40BD3A | 110 / 378B / 十一CALL四callee、八跳一RET |
| F:0000BD40 / UpdateBattleAction40_40BD40 | 40BD40–40BD56 | 9 / 22B / 两CALL两callee、零跳一RET |
| F:0000BD60 / UpdateBattleAction41_40BD60 | 40BD60–40BDA1 | 21 / 65B / 两CALL两callee、零跳一RET |
| F:0000BDB0 / UpdateBattleAction42_40BDB0 | 40BDB0–40BDE2 | 18 / 50B / 两CALL两callee、零跳一RET |
| F:0000BE30 / UpdateBattleAction43_40BE30 | 40BE30–40BE6B | 19 / 59B / 两CALL两callee、一跳一RET |
| F:0000BDF0 / UpdateBattleAction44_40BDF0 | 40BDF0–40BE22 | 17 / 50B / 两CALL两callee、零跳一RET |

合242条768B/九份最多64条分页、25directCALL/六唯一callee、十三跳八RET，无间接调用/跳转。八根各一个实际入向CALL/全部404DB0，八case各11B push→CALL→JMP40563F取齐；本步仅这些分派，不重复纳管父/擅改循环或外推其它case全部ABI/owner已验。39/43保实际S32末播放返回，其它当前void/真实弃返保持。

## 37/38：条件播放后的fresh等待计数

固定payload，先完整W32 FDC后低AL 60h全清才PlayFrame(action,0)弃返；随后fresh FF0+1为count并发布，不论是否播放都执行计数。37新count>=60、38>=120才FF0=0后AdvanceCast；不提前取FF0、不改严格阈值或给原没有的rate/characterAction/Move增加发布。两处U8明确原低BYTE消费，其余原S32正常算法保，RPLAN.24不新增计数限制/异常模拟。

## 39：音效取样、交替动作和三次播放

nameX==0时先持action.x S32，再characterAction8→PlaySe(163,heldX)→fresh action.x供PlaySe(171)。原各声调用前Y取样但实际音频边界不消费第三参数，按现WAV-only二参保，不恢复额外音频/RNG。

两声后fresh持旧nameX和nameY（interval），count=旧nameX+1并发布；interval==36原业务短路，跳过除法直接进入切换；其它仅S32 count%(36-interval)==0才进入，不补默认除数、钳制interval或范围拒绝。原IDIV的正常整数/返回关系保，不专项模拟溢出。

进入后fresh characterAction==9才持X→发布8→PlaySe163→fresh X/PlaySe171，然后两次独立direction读取/增量/发布：每次旧方向+1先发布，保存值>=8才减8再发布；第二次从第一次最终值重新读，不改直接+2/%8。其它characterAction则持X→发布9→PlaySe163→fresh X/PlaySe174，不增方向。

声/方向后fresh nameY+1为cycle并发布，保存cycle>9时依次nameX=0、nameY=0、nameValueC0=0、characterAction9，Sub_406260(action)再AdvanceCast。每一正常路径共同尾连续三次PlayFrame(action,1)，前两弃返，第三真实S32返回；不根据前两返回早退/合并调用，也不在推进后跳过播放。现全部正确正文/签名保持，仅两注释/独立合同登记。

## 40–44：不同转向对象和发布顺序

40仅Sub_406260(action)→AdvanceCast，无Play/计数/rate等新增逻辑。

41固定payload，targetPosition100C S32→gBattlePointIndices5536FC映射一次→gBattleGraphicPoints553758固定point引用；先point[3] S32转F32 Y，后point[2]转F32 X，TurnPosition(action,x,y,1,0F)后AdvanceCast。两原只读表完整440B：5536FC–553754共22 S32索引，553758–5538B8共22×4 S32坐标；88/352B及全部值实读/对照现SceneState常量，索引皆0..21。不据这项静态表验收批准额外输入边界/猜-1或加保护，数据区不重计新X/函数。既有BATTACK/M404表用途关联复用，不改当前定义。

42固定payload，targetPosition100C取一次全局target T*，先offset44 F32 Y再offset40 F32 X，TurnPosition(action,x,y,1,0F)后AdvanceCast；没有父NULL保护，当前不新增或替换为41的静态坐标。

43先持非拥有battleOpponent10，非空才TurnToward(action,other,1,0F)；之后无论是否转向都rate100→characterAction0→PlayFrame(action,0)，保实际S32返回。不能交换两个发布或改变播放方向参数。

44固定payload，C9C→16B effects记录position→全局target T*，TurnToward(target,action,1,0F)（接收转向的是target）后AdvanceCast(action)。原不在父加target空门禁，真实BOG01助手处理沿原，不能互换两对象或推进target。

BOH01方向叶、BOG01转向叶、BEV01推进以及现播放/二参WAV音效仅复用，全部旧原身份/语义/处置与本体保持，不把局部接口/分派当全producer/owner/SDK验收。

<a id="battle-wait-turn-decision"></a>
## 实施与登记边界

按既有工程委托先SPEC后落实：一CPP仅37/38各一低BYTE表达与八定义注释，公头八合同注释；其它六完整正文/全部签名及所有其它源/表/算法/真实调用保持。零新函数/API/保护/拒绝/默认值、声/播放/RNG或业务重构；44转向对象与39全部持样/fresh、原短路36和两方向/三播放保。

八身份此前无独立语义/F处置/已知BATTLE成员，新增八语义/adapted处置/成员，枚举false。I仅两BYTE表达，S八完整当前正常源及440B静态数据/局部分派，不计新全实现；U08/U09/R01/R04/V04补来源/局部状态且open，全部原分母、六closure/政策/门禁和旧记录保。CRT排序此前选择未答复，WAV-only/原格式明文/RPLAN.24保持，不构建/测试/运行/算法/产物/摘要或引依赖。

<a id="battle-wait-turn-acceptance"></a>
## 必要静态与剩余

核八完整242条768B/九页/库存、25E8/六callee/十三跳八RET及八11Bcase；两原AL60h消费与播放后fresh FF0/不同阈值、39各声前X持样/声后X重取、nameX/Y持样/36短路IDIV/切换后fresh cycle、两方向增量/三个连续播放、五转向不同数据和对象/发布序。

核两只读表440B/全部110 S32值及真实常量/声明/无改动；整CPP仅两表达/八注释、公头八注释，所有其它本体/签名/调用和旧记录不变；八语义/F/成员、限定I两BYTE/S八根、五open任务/所有政策门禁/UTF-8无BOM/LF与静态汇总/-Check准确。完整nameX/Y/目标生产、其它父/全部callee/ABI/alias/owner、SDK正常失败/容量/重进退出/数值与声音画面运行仍待，总SPEC§10不降。

<a id="battle-wait-turn-source-state"></a>
## 当前状态

八完整根及两表证据已取，两BYTE与十六注释已落实；必要静态、文本汇总/-Check及精确派生差异通过。语义2095（2047F＋48X）、合同2157/3680=58.61%（仅关联率）、F1523、软件E1387、BATTLE335且枚举false；五任务仍open，旧记录/原分母/门禁保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-wait-turn-history-20261010，提交引用cg2d-re-battle-wait-turn-20261010。整体SPEC未完成。
