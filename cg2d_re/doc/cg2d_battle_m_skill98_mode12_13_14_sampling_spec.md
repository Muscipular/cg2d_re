# BMY01 战斗技能98 mode12/13/14选中角色与双位置取样

日期：2026-10-06。沿[BMX01 mode7/8](cg2d_battle_m_skill98_mode7_8_sampling_spec.md)推进12/13/14。按既有工程委托先SPEC后实施，单CPP三个既有根32组32定义32处（12十五/13十一/14六），不扩业务或降低验收。

<a id="battle-m-skill98-mode12-13-14-sampling-baseline"></a>
## 原端有界基线

| 范围 | 指令/字节 | call |
| --- | ---: | ---: |
| mode12及其选中终结尾42D7DA..42DD12 | 363/1336 | 34 |
| mode13目标主体42DD1B..42E015 | 188/762 | 20 |
| mode14双位置主体42E01E..42E1E4 | 111/454 | 15 |
| mode14施法者终结参数尾42D696..42D6AF，沿BMW片段 | 7/25 | 0 |
| 共用消费尾42EB02..42EB32，沿BMW范围 | 12/48 | 3 |

五范围去重681条2625B十二页72call，见[实际字节、分支及源码前态](cg2d_battle_m_skill98_mode12_13_14_sampling_evidence.json)。全部直接目标落本组或已证437A29父回存；mode13循环哨兵/空失败进mode12实际42DCDC/42DCD4选中角色尾，12/13终结/Append74后跳42EB0A。mode14两哨兵进42D696施法者参数尾/42EB02消费。两个既有尾19条73B/3call沿BMW当前证据，不称本步新增覆盖；新取证662条2552B/69call分列。

三个模式选择比较各两条9B，合六条27B/0call，仅入口上下文；12/13/14相等分别落42D7DA/42DD1B/42E01E，不等沿42DD12/42E015/42E1E4继续，后续未验。先前比较/一次mode读取沿BMX/BMW已证前缀，EBX=8及EDI=6的原正常常数有此前比较链来源；SKL98入口沿BMO202B表。不是原跳表或完整98/父17298验收。

## 原正常消费保持

12/13 Take选中位置/GetPosition，255比较先pub，同解析位置actor空门禁，初始255直接父回存，初始空Finish直接回存；不走终结尾。固定解析selected写caster.targetPosition100C；选中skill/level各另live selected→actor→live值→payload/pub，不复用初始actor。

mode12 Append151/8/6/10，每次live selected；一次位置比较<10确定基准10/15或5/0。X两S32 indices先读，baseX→固定比较位置actor→otherX→delta→payload→正常baseX+delta/2；Y另两indices→baseY/otherY→正常中点→live selected/actor/payload，不能跨轴固定角色/索引。288与80/85/36/91/86/37/89保持。mode13固定15/5各轴两indices→两坐标→正常中点→live selected/actor/payload，8/6/10/64/137/90保持。两模式selected先currentPosition比较，256或52沿比较所得位置查actor，51调用后再live selected，不反置读取或复用角色跨调用。

目标循环Take/GetPosition→255比较先pub/同解析位置查actor；目标空Finish break仍走选中终结尾。Take/pub resultFlags→Take amount；mode12仅800000条件进原辅助图取包，两个图号与三组两S32数组沿BMO原八W32正常零一次初始化，跨目标保持，不逐目标补零。原结果标志从内存读W32到EAX后AL/AH测试，保持现代W32局部flags，不误改成内存W8。

mode12首次效果参数为firstImage→live flags→target→selected→actor；两图选择计数恢复原S32，首次正firstImages索引/默认0规则保持。辅助参数在首次效果调用之后取firstImages[选中index]→secondImage→firstImage→live flags→target→selected→actor。两目标字段各target→actor→live skill或level→payload；target先caster比较，0动作沿该次位置。34之后target先currentPosition，256/52沿比较位置，51再live target。165/27先target→局部flags低位2条件→actor→kind，原amount/108/40/3/2/20和非10000门禁、flags调用后重读及11/46保持，不新字节读取或异常模型。

mode13首次效果为live selected→target→flags→actor，最后图号正常0；两目标字段同原序。0/149分支先target→caster比较，相等沿该位置Append0并调用后重新取target给149；不相等149沿比较所得位置，不额外读全局。后160、amount写1030/原5写1034各次live目标保持。12/13共用选中尾按live selected效果(-1,0,0)→另live selected Append74→共用live caster Move→RID→固定scene payload58递增。

mode14首Take/GetPosition所得kind固定、Append25；循环首selected/次target分别255比较先pub，**原没有actor判空/Finish**，不新增门禁。各金额Take后live该位置→kind==0条件→actor/payload金额1030，另live位置→actor/payload→沿该条件写1034=9或8；不能复用Take前解析位置或在金额字段发布后才取kind条件。原111015图号、两效果均caster位置→selected或target位置→actor，四250/160动作按target两次、selected两次逐次live，双金额及双哨兵次序保持。任一哨兵都走施法者终结参数尾/共用消费，不改为选中尾或提前返回。

<a id="battle-m-skill98-mode12-13-14-sampling-decision"></a>
## 三根32处裁决

| 根 | 处数 | 落码 |
| --- | ---: | --- |
| mode12 | 15 | 初始selected比较pub/两字段；两轴取样；选中player；循环目标；效果参数/两S32选择计数/辅助参数；两目标字段；0动作；34后player；165/27参数。 |
| mode13 | 11 | 初始selected比较pub/两字段；两轴索引与中点；选中player；循环目标/效果参数；两目标字段；0/149条件缓存与调用后重读。 |
| mode14 | 6 | 两位置比较pub、两个金额/类型字段条件与payload顺序、两个效果参数。 |

仅32处修改，wrapper/准备/mode6/7/8/两个正确尾helper/其它根、布局与声明保持；不新F/X/adapted/分类/完整成员、owner或公开接口。非拥有native T*仅局部借用，只有已证地址拓宽，非地址保原S32/W32；网络原格式仅明文/音频暂停。不新增越界/数学溢出/输入拒绝/异常补零/钳制/回绕/确定值模型，正常中点保持原业务范围，不宣异常等价。

<a id="battle-m-skill98-mode12-13-14-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 五范围681条2625B十二页72call连续实际字节/直接目标闭合，两个尾与BMW当前证据片段一致；六条27B选择比较及BMO初始化/202B表只作上下文。 |
| 实现 | HEAD整CPP三根32组32定义32处精确差异；两类中点/效果参数/字段-player原序与S32计数、0/149条件取样、14金额类型和双位置有限静态，其它根/头保持。 |
| 消费 | 12/13初始失败直接返回、循环失败走selected终结/74；14没有actor空门禁且两哨兵走caster终结；原动作-金额/flags和图值跨目标及末Move-RID-增58保持。 |
| 台账 | 旧812仅有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S/七open任务、完整门禁及其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI、完整98或owner运行。 |

<a id="battle-m-skill98-mode12-13-14-sampling-source-state"></a>
## 当前状态与后续

12/13/14三根32处实修，有界三主体与两个尾681条2625B纳管。两个既有尾19条73B非新增，三个比较仅入口，不据父计数或现代switch验完整98。状态目标、mode15/16、17/默认继续按真实原端依赖子SPEC开发，所有owner/消费者运行与其它技能-全路由/完整父-SEH、SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E409→413仅双域四局部I/S。关联率不是完成率，本组14文件；快照`cg2d-re-battle-m-skill98-mode12-13-14-sampling-20261006`。
