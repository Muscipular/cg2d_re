# 战斗缩放恢复、范围处理与震动生产完整合同 SPEC（BRAF01）

2026-10-10；动作84–92九完整正常根。仅84/85尾门禁和86两效果门禁四BYTE消费、86负HP临时复制一取样片段及十八注释；现缩放/命名选图/固定与动态目标分工/十位置RNG/延迟和越界退出均保，不新增函数/API/保护/默认值、循环上限/声/播放/RNG或业务重构。

<a id="battle-restore-area-baseline"></a>
## 完整原根与分派

| 原身份 / 当前源 | 连续完整根 | 指令 / 字节 / CALL |
|---|---|---|
| F:0000CD70 / UpdateBattleAction84_40CD70 | 40CD70–40CDDB | 33 / 107B / 2 |
| F:0000CDE0 / UpdateBattleAction85_40CDE0 | 40CDE0–40CF30 | 104 / 336B / 4 |
| F:0000C520 / UpdateBattleAction86_40C520 | 40C520–40C779 | 165 / 601B / 16 |
| F:0000C780 / UpdateBattleAction87_40C780 | 40C780–40C8C6 | 106 / 326B / 16 |
| F:0000C8D0 / UpdateBattleAction88_40C8D0 | 40C8D0–40C8F0 | 7 / 32B / 1 |
| F:0000C8F0 / UpdateBattleAction89_40C8F0 | 40C8F0–40C906 | 6 / 22B / 1 |
| F:0000C910 / UpdateBattleAction90_40C910 | 40C910–40CA98 | 109 / 392B / 9 |
| F:0000CF30 / UpdateBattleAction91_40CF30 | 40CF30–40CF4C | 11 / 28B / 2 |
| F:0000CF50 / UpdateBattleAction92_40CF50 | 40CF50–40CF6C | 11 / 28B / 2 |

共552条1872B/十四份最多64条分页、53E8/二十一唯一callee、47跳九RET，无间接CALL/JMP。九根各唯一实际入向CALL/同父404DB0，九case各11B push→CALL→JMP40563F。87/91/92保真实S32末播放返回，其它当前void/真实弃返保持，局部分派不代全父/ABI/owner验收。

## 84/85：原未舍入数学与两种载荷读取

84先fresh scaleX减54D4A0原F32步长3C23D70A（约0.01），保存未舍入中间，再固定入口payload；scaleX F32发布后fresh scaleY减同值/F32发布。正常未舍入X<=1时先两scale=1→Advance，再fresh action.payload/FDC清1000h整W32发布。共同末播放门禁仍从入口payload fresh BYTE FDC取60h，全清才PlayFrame(action,0)，不能改用刚重取的payload或提前持FDC。

85先fresh scaleX加同一步长并保存中间，再持旧direction和入口payload；newDirection=旧+1，characterAction9→scaleX F32→fresh scaleY中间→direction新值→scaleY F32发布。保存direction>=8只减8发布一次；未舍入scaleX正常>=2才创建随机效果/Advance，不按已舍入值判断、不新加两轴上限。共同末从入口payload fresh BYTE60h，全清才PlayFrame(0)。两门禁只明确U8，保持数学double正常中间/float字段及既有数值许可，不专项模拟异常/溢出。

普通技能直接NewRandomEffect(action,110154,0)；castSkill103C==2002才从gBattleNamedEffects5ABCE0逐36B原name寻找与action.nameC4相等记录，再沿57E174三S32记录按第一列key逐项匹配该记录value，命中第二列image，否则遇key0用原默认110154；之后创建/Advance。正常名称匹配预条件不造默认name/额外边界或失败绕行，std::strcmp沿MC01真实MSVC承接，仅相等结果消费，字节/字段/读序保持。

57E174–57E42C完整696B/58×3 S32实读，含57条和全零哨兵，全部174值与当前constexpr一致；85只消费前两列，第三列供其它既有消费者保留，不删或重计新X。当前动态名字目录20×36B/720B的真实定义/声明沿既有源，完整name/value生产仍待；不把当前清零定义当原初值/完整输入验收。

## 86：逐效果处理、固定目标与动态HP/phase目标

固定入口source payload，原do至少一轮，每轮以fresh C9C/position查一次target T*、固定其payload，target.hidden=0→TurnToward(target,source,1,0F)→Sub405CB0(target)。方向调用后fresh当前效果flags整W32发布固定targetPayload.FE4，再ApplyEffectFlags(source.battlePosition18,source)；调用后fresh原BYTE flags20h置位才固定targetPayload完整FDC OR2→全局updateGateMode1→aliveFF8=1。

随后fresh完整当前效果flags100000h，置位才无父NULL门禁MarkDeath(targetPayload.effect1054)，调用后fresh完整FDC先持清20h结果，再effect1054=null→整FDC发布。接着fresh持C9C和完整flags；10000000h置位则用保存index重新读position/查全局target，NewFloatingIcon(target,22459,0,0)，跳过普通数字/属性/HP路径。

普通路径固定target X先读→保存index的position3 amount→固定target Y-80，NewNumber(x,y,amount,6,2,heldFlags)；之后fresh C9C/原BYTE flags2才对固定target创建22454图标，再NewAttributeEffects(source,target)。属性调用后fresh effectIndex/damage/position，查动态target扣readiness；扣后再fresh当前position/全局target，Less(readiness,0)真实读取，不用固定target代替。

负值才构造临时CryptInt零；原先fresh C9C，再读取临时encoded，之后读position/查动态copyTarget。随后encoded发布→fresh临时mask读取/发布→fresh临时BYTE readCount读取/发布→Destroy临时对象，保target token/未复制字节。现共享CopyPayload本身encoded-mask-count写序正确，但其encoded读取发生在目标查找之后；本步只展开此调用段恢复原40C6CC/D2/后续取目标的先后，不改共享复制器或其它caller、不把mask/count提前读齐。

两视觉分支合流后分别fresh当前position/target调用MoveState，再fresh position/target/phase，严格phase0才Advance该目标；每轮AdvanceEffect(source)，严格返回1才结束，不新加哨兵/上限/目标NULL保护。完成后振幅6→震动启用1→Advance source。所有原完整高位/固定与动态目标、创建失败后继续/读取RNG和调用顺序保；只修两个低BYTE门禁和临时复制读序，不能合并两HP/phase目标或将标死当free。

## 87/88/89：十位置随机与同源震动

87固定payload，但每个index0..9重新读取battlePosition18分阵营。<10仅gate RAND(0,2)==0才依次image RAND(-1,0)+110007→RAND(-60,0)后读point[index+10][3]求Y→RAND(-30,30)后读point[index+10][2]求X→NewScreenEffect(x,y,image,0)→RAND(0,30)==0音效门禁。另一侧同序，point[index]、Y范围(-80,0)、X范围(-40,40)。创建返回无论成功都执行后声门禁，失败不跳RNG；gate未中只推进index，不消费image/Y/X/声RNG。两支互斥、十个父静态RAND点保，不预取阵营/坐标、不把Y/X随机交换或缓存成一轮。

声命中播放WAV263/X320，原额外Y240不由现二参边界消费。十轮后fresh FF0+1发布，新值>=270才FF0=0→Advance→再次声263/X320；每一正常路径最后PlayFrame(0)S32返回。BWT01两静态坐标表源/既有全值证据复用，原常量不新建/重计；BNW01屏幕创建/回调与BVU01真实RAND及其内部消费保持，不在本步宣全RNG结果/资源/声音通过。

88振幅6→启用1→Advance；89只启用0→Advance，不清振幅/坐标、不新增播放。86/88/89沿同一当前震动全局，不造影子状态，真正更新/重进退出仍待验。

## 90：事件重取、目标启动、同帧延迟与正常越界退出

入口先持frameEvent，再固定payload，value54=10F。入口event0才固定当前effect引用，position非-1才flash1004=1，再从该记录fresh读position/全局目标，TurnToward(source,target,0,20F)。转向后fresh frameEvent0才fresh C9C/目标Distance，正常S32距离<20才又fresh目标MoveState，再fresh目标/phase0才Advance target。然后FD0=4→AdvanceEffect(source)，严格返回1才frameEvent=1，不能缓存三次目标、把入口事件复用到转向后或合并阶段判断。

共同尾fresh FD0，>0先减1并发布，同帧再fresh FD0==0才direction旧+1先发布，新值>=8改0（原不是减8），PlayFrame(0)→MoveByAngle。之后按原正常X<-200、X>840、Y<-200、Y>680顺序短路读取；越界才额外AdvanceEffect(source)→AdvanceCast(source)，保至少一次目标与额外推进，不添新边界/延迟门禁。现正确源和原值10F/20F、四帧设置当帧减1保持，不模拟异常浮点。

## 91/92：对象flags与播放返回

91完整对象flags OR2、92清2，各先Advance后PlayFrame(action,0)S32返回；不写载荷FDC、不把播放提前推进或新增完帧门禁/角色动作/速率发布。现正文保持。

<a id="battle-restore-area-decision"></a>
## 实施与登记边界

按第8条工程委托先SPEC后落实：一CPP四BYTE消费及86临时复制一取样片段（共五组）/九定义注释，公头九注释；其它六完整正文/全部签名/数值/算法/实际调用/RNG参数与顺序、原表/共享源/复制器及全部旧语义/处置保持。九身份此前无独立语义/F处置/已知BATTLE成员，新增九adapted语义/处置/成员，枚举false。一限定I仅84/85/86五片段，一S九完整正常源/局部分派/原常量/696B表及依赖接线，不计新全实现。

U08/U09/R01/R04/V04补来源/局部状态且open；全部原分母/门禁政策/旧记录/六closure保持。MSVC CRT、WAV-only、原格式明文、RPLAN.24保持；CRT排序待此前选择，原六项用户改动/额外CRT文档压缩保留、不夹带。不构建/测试/运行/算法/产物/摘要或引依赖，不降总SPEC§10。

<a id="battle-restore-area-acceptance"></a>
## 必要静态与剩余

核九完整552条1872B/十四页/库存、53E8/二十一callee/47跳九RET、九实际CALL/九11Bcase。核84入口/后置payload分工和两缩放门禁；85原名字36B/值偏移20与12B表步进/第二列/默认110154、696B全174值及目录实际定义；86固定与动态目标、两BYTE/完整高位、临时index-encoded先于查找/encoded-mask-count各读各发布、真实do/严格1/震动源；87十位置/逐次阵营与父十RAND静态点/所有参数序、88/89差异；90事件/目标fresh及延迟当帧递减/方向归0/四边界/额外推进、91/92对象flags与返回。

核整CPP仅五组/九注释、公头九注释，所有其它源/本体/签名/数值/调用/RNG/原表/复制器/旧记录保持；九语义/F/成员、限定I五片段/S九正常根、五open任务及所有原政策门禁，UTF-8无BOM/LF/新SPEC单一末尾LF、差异及文本汇总/-Check。完整名字/技能/效果/HP/延迟/震动生产、其它父/全部callee/ABI/alias/owner、SDK正常失败/资源/容量/数值/RNG结果/重进退出与声音画面运行仍待，总SPEC§10不降。

<a id="battle-restore-area-source-state"></a>
## 当前状态

九完整原根/表/常量/case已取齐，四BYTE与86临时复制一取样共五片段/十八注释落实；必要静态、文本汇总/-Check及精确派生范围通过。语义2144（2096F＋48X）、合同2206/3680=59.95%（仅关联率）、F1572、E1395仅限定I三F/S九F、BATTLE384且枚举false；五任务open、旧记录/原分母/全部政策门禁保持。未构建/测试/运行/算法/产物/摘要。前态cg2d-re-battle-restore-area-history-20261010，提交引用cg2d-re-battle-restore-area-20261010。整体SPEC未完成。
