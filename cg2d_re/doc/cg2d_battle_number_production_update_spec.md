# BOI01 战斗数字创建与更新 SPEC

> 2026-10-06，按既有工程委托先登记再实施。沿已纳管命中回调对NewBattleNumber41F1A0的真实调用，开发字符对象布局与Update41F110正常更新，不全面复验已通过的格式化、CryptInt或命中回调。网络原格式仅明文；内存CryptInt保，音频暂停。

<a id="battle-number-production-update-baseline"></a>
## 1. 原端与现有基线差集

已打开cg2d.exe只读实际指令为权威。已有语义1071/F:0001F1A0及[stdio合同](cg2d_stdio_contract.md)仅证明入口格式callee53B304/原%d/S32第三参数和符号前缀59条；其余132条对象、资源/布局及更新未全验。本步保留该接线与历史证据，补完整创建原体及更新根，不另造重复ID记录。

| 原入口 / 库存ID | 源符号 | 全指令/代码B | call/分支/ret | 本步补丁/语义 |
| --- | --- | --- | --- | --- |
| 0x41F1A0 / F:0001F1A0 | NewBattleNumber_41F1A0 | 191/687 | 11/14/1 | 4 / 1071续原记录 |
| 0x41F110 / F:0001F110 | UpdateBattleNumber_41F110 | 44/144 | 4/7/1 | 5 / 1397新增 |

两根单chunk，共235条831B/四页、15call/21普通内部分支/两ret，无switch；源码switch为三字符普通分支，不伪报IDA跳表。完整字节、出口、改前根和九补丁见[证据](cg2d_battle_number_production_update_evidence.json)。创建46普通call/23父；更新有两type1 callback地址绑定41F28C/420792（父41F1A0/4206C0），不计两普通call或完整父验收。

原54D4E8十行四列40×W32图号160B，两F32速度常量8B，格式%d NUL三字节及+/-各含NUL两字节共7B，总175B数据证据。原立即数速度18/270与90角/B8初速度-8由实际指令分列。既有[CryptInt固定字段](cg2d_battle_impact_callbacks_sampling_spec.md)和[正常__ftol边界](cg2d_compiler_helper_spec.md)保；不据此宣宿主PC53/ABI通过。TextMeasure44B260、图号与XY资源callee全部原正常/失败及运行门禁仍随真实依赖继续，不能由本步代验。

## 2. 完整正常合同

### 1. NewBattleNumber_41F1A0

未清256B buffer先Format53B304 %d/S32 value，随后mode5ABFCC==0时flags40000000追加+、80000000再追加-独立；font0 TextMeasure/2正常S32向零截断后固定strlen，不随字刷新长度。逐字NewYobiAction(2,0)，失败break保已建对象/所有路径原S32返回0；成功callback41F110/W8 CF，buffer字节S8构造临时CryptInt，readCount/encoded/mask三个先取样再按W8/W32/W32 pub，token/reserved保/析构。image0/speedF32 18、x<320角270否则90、flagsW32=2；字符+/-/:取十行四列的1/2/3，其它行0+S8字符-0，无新过滤。MapNoIndex→GetGraphicXY(两个S16输出)都不增成功门禁；S16 graphicX先F64→live指定fontType.sWidth S32*index→依序+x-halfWidth+advance落F32X，后S16graphicY+y落F32Y。X截断→Y样本→Xpub→Y截断→flags样本→Ypub→nameX样本→flagsOR40000→B8=-8/flagspub/58=2index/两W8=4→nameX截断→nameY样本→nameXpub→nameY截断pub→W16field270零；不PlayFrame、不整体回滚或增输入限制。

### 2. UpdateBattleNumber_41F110

单次入口phase0→单次oldDelay，正值仅减一且本次不运动；零/负值speedB8 F32+0.8F形成未舍入和，读flags清2→与7.2F比较→速度F32pub→flagspub→达到峰值phase1→同未舍入speed正常S32截断并加到S32 y，不把Y写offset44或取刚存F32。入口phase1旧deltaX+1pub，>=50 MarkDeath仍尾；其它phase只尾。所有路径nameOffsetAC正常S32截断→F32 nameOffsetB0样本→nameXpub→nameY截断pub，无对象XY投影、PlayFrame、parent/deathFlag或新空门禁。

数字创建的固定font0测宽与逐字fontType.sWidth不是同一参数职责，按原体保，不擅改为指定fontType测宽。长度在测宽之后固定；每字S8两次消费保持，不过滤+/-/:或改符号顺序。资源返回仍原忽略，不新增失败拒绝、输出补零或改图号；NewYobiAction失败仅停止后续字，已建对象保，返回值仍0，不能改成成功数量/固定假成功。

创建给58=2index、B8=-8与隐藏flags2；phase0正旧延迟只减一，即便本次变为0也不运动。只有旧延迟零/负才清flags2并向上/下推进S32 y；它使用未舍入speed正常S32截断，同浮动图标41F450的F64 speed加F32 offset44职责不同，不合并。峰值达7.2转phase1后下次才计停留，50帧标死仍更新名字坐标；不是浮动图标的60帧或对象XY投影。

更新另有NewBattleTimeNumber4206C0的真实绑定。本步登记该入向，不宣其完整构造者、23父/46调用点、全部draw consumer/字符载荷读取及回收/重入owner闭包；MarkDeath仍既有标记而非立即free。原回调void/原创建S32返回0与非拥有nativeT*保持。

<a id="battle-number-production-update-decision"></a>
## 3. 九处差异与本线程裁决

| 差异 | 原证据 | 决策/实施 |
| --- | --- | --- |
| 临时CryptInt三字段边读边写，原三个值先取样后写目标 | 41F2A6/AA/AE→41F2B2/BC/C2 | 固定W8 readCount→W32 encoded→W32 mask，再按同序pub，token/reserved/析构保 |
| 指定字体宽度先于S16 graphicX转换读取 | 41F380/38C→41F390/39A/39D/3A4/3AC/3B0 | 先graphicX转F64，再live宽度*index，X依原算式落F32；固定font0测宽保 |
| 创建Xpub先Y取样、Ypub先flags读取、flagsOR先nameX样本 | 41F3CD/3D2/3D5/3D8/3DD/3E0/3E3/3E9 | 显式X截断→Y样本→Xpub→Y截断→flags样本→Ypub→nameX样本→flagsOR |
| 创建与更新先nameXpub后读nameY | 41F410/415/41B/421/426；41F182/187/18D/193/198 | 各一补丁正常nameX截断→F32 nameY样本→nameXpub→nameY截断/pub |
| 更新重复phase判断与正延迟二次读取 | 41F115/11D/11E；41F139/140/141 | 两phase补丁固定入口样本，另单次oldDelay样本后减一，不改变0/1/其它与零/负旧延迟分流 |
| 更新先存F32速度/flags后才比较峰值 | 41F152/155/157/15D/163/16B/174/179 | flags清2样本后先比较未舍入speed，再速度F32/flagspub与phase；y增加仍截断同未舍入speed，不换成刚存F32 |

九精确补丁覆盖创建四处/更新五处；源表达/取样还原不等同静态证明旧画面差异。只落实原正常S32/F32/F64/W32/W8/W16和字节，不新设异常/回绕/拒绝/钳制/父owner、三角函数或新工具，不改原文本/网络格式、不引入外部依赖/RTTI。

<a id="battle-number-production-update-acceptance"></a>
## 4. 可度量局部验收

| 项 | 局部证据/增量 | 不外推范围 |
| --- | --- | --- |
| 原根S | 两完整235条831B/四页15call-21普通分支/两ret，48入向分列；图号表40项/速度/格式175B逐字节 | 59条格式接线历史保持；不以本步证明全部callee/23父/46调用点/时间数字构造者 |
| 实施I | 整CPP精确重放九补丁；格式、符号/返回0/失败保旧对象、S8临时/表图号/资源序、三坐标pub、原phase/延迟/50计数-标死继续按完整原体定向核 | 未编译/链接/运行，资源像素/宿主PC53/全consumer-owner/重进退出未验 |
| 语义/分类 | 1071原记录补完整基线与当前状态、历史格式事实不删；新增1397/F:0001F110，两新adapted分类/BATTLE99→101成员，局部I/S各根两条E589→593 | member_inventory_closed=false，全功能gates不变，R1/R7/R8继续 |
| 计量/进度 | 语义1397→1398/有效ID1392→1393、分类503→505；关联1510→1511/3680=41.06%，七open追加和六文档/唯一进度同步，measure/-Check | 175队列/R7区间67/3061、60源码closed10/完整验收0/P01、全验收仍false；关联率不代完成率 |

按现行限制仅定向文本静态和获准计量汇总器；不构建/调试/新增或执行测试/运行客户端或算法/hash。原标准不降。

<a id="battle-number-production-update-source-state"></a>
## 5. 实施及接续

本SPEC先落盘，再实现两函数九还原、原证据/语义1071续记及新增1397、两分类/成员-四局部I/S、任务/唯一进度和看板同步，精确提交到命名快照 `cg2d-re-battle-number-production-update-20261006`。

本步两完整正常根纳管，旧格式事实保留且当前验证范围明确扩大；时间数字构造者、TextMeasure-资源读取-实际draw consumer、所有caller/回收-重入owner/ABI-PC53-画面与R1/R7/R8沿真实依赖继续。

## BOJ01 对本SPEC测宽接线与旧I结论的更正（2026-10-06）

[后续具体SPEC](cg2d_battle_time_default_measure_spec.md#battle-time-default-measure-baseline)/[证据](cg2d_battle_time_default_measure_evidence.json)核实原41F242目标44B260。BOI01实施快照的TextMeasure(buffer,0)实际对应44B2A0通用逐字读取宽度，不能代原默认非空入口缓存字体0双宽/单宽合同；本线程本步已改MeasureDefaultText_44B260(buffer)/2。故旧E-BATTLE-NUMBER-PRODUCTION-UPDATE-0001F1A0-I-20261006改fail，原snapshot/artifacts/expected保持且附冲突原因，新BOJ01局部接线I/S独立通过；旧原指令S/格式事实与其它九还原保持。

Time420718同样恢复44B260，Default正确体独立纳管。当前来源与状态以BOJ01接线更正为准；本历史SPEC的其它完整原根事实和更新回调50帧整数Y合同不变，不凭新局部I/S宣布全资源/consumer-owner/运行验收。
