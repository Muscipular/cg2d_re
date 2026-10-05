# BMZ01 战斗技能98 mode15/16伤害目标取样

日期：2026-10-06。沿[BMY01三模式](cg2d_battle_m_skill98_mode12_13_14_sampling_spec.md)继续15/16。按既有工程委托先SPEC后实施；两条原主体分别完整取证后保留单既有ProcessBattleMSkill98DamageTargets共用根，五组五定义五处，不扩业务或降低验收。

<a id="battle-m-skill98-mode15-16-sampling-baseline"></a>
## 原端有界基线

| 范围 | 指令/字节 | call |
| --- | ---: | ---: |
| mode15主体42E1ED..42E43B | 168/590 | 16 |
| mode16主体含哨兵压零42E444..42E69E | 172/602 | 16 |
| mode16真实施法者参数尾42E7F8..42E80D | 5/21 | 0 |
| mode15参数尾42EAEF..42EB02，沿BMX尾后缀 | 6/19 | 0 |
| 共用消费尾42EB02..42EB32，沿BMX范围 | 12/48 | 3 |

五范围去重363条1280B九页35call，见[完整实际字节、分支及源码前态](cg2d_battle_m_skill98_mode15_16_sampling_evidence.json)。所有直接目标落本组或已证437A29父回存。mode15哨兵进42EAEF参数尾，正常落42EB02；mode16哨兵进42E695先压零/零，再跳42E7F8读caster/压-1/actor→42EB02，不能只见跳到另mode地址就当尾相同。定位探针42E80D以后状态目标内容不计本步。两既有尾18条67B/3call沿BMX当前证据非新增；新取证345条1213B/32call分列。

两个选择比较各两条9B，合四条18B/0call，只作入口上下文；相等落42E1ED/42E444，不等42E43B/42E69E后续分派。先前原比较链沿BMY/BMX/BMW，SKL入口沿BMO202B表，不以现代switch造原跳表或验其它mode/父17298全文。

## 两条原路径与共用根裁决

两路径逐项核实：目标255比较pub→flags/amount Take→800000条件辅助图取包→效果和辅助参数→111011/250→金额1030/类型1034→159→结果分支→循环/施法者终结。原参数/读取和正常分支相同；不同为15沿BMW原EDI=6、16入口42E449显式EDI=3以及哨兵参数尾位置。既有wrapper仅两实际调用传6/3和既有共用根正确保留，不新模式函数/假体/类型裁决或业务默认。

目标Take/GetPosition→255比较先pub同target，原**没有actor判空/Finish**；不新增空门禁。flags原W32取包/pub→amount原S32固定；800000条件取原辅助图。两个图值和三组两S32数组沿BMO八W32正常零一次初始化、跨目标保持，不在每个目标补零。

首次效果按firstImage→live flags→target→caster→actor，amount固定；辅助参数在效果调用后读firstImages[选中index]→secondImage→firstImage→live flags→target→caster→actor，不能在第一效果之前取未来参数或跨调用固定actor。两图选择原EDX/EAX为S32计数，首正firstImages索引/默认0保持，非地址不拓宽。

另live target/actor/payload写image102C=111011→live target Append250→另live target/payload写固定amount1030→另live target/payload写numberType1034（6/3）→live target Append159。调用后读resultFlags内存W32到EAX，AL/AH只是寄存器位测试，保持现代W32 flags，不误改内存W8。

flags40且非10000→live target Append34→**live target先currentPosition比较**；256或52沿比较所得位置actor，51调用后再live target。此分支amount==0才在player调用后重新读flags；否则amount!=0进入11，不能无条件新增flags重读。条件不满足且amount!=0才Append165；接着amount!=0或flags108→Append11后再读flags。最后局部flags20同时控制47/46，两调用各次live target，47后不重新读flags决定46；原分支/常数与正负S32金额保持。

两个哨兵均是live caster/actor效果(-1,0,0)→另live caster Move→RID(scene)→固定scene payload58递增→437A29。正确CompleteBattleMSkill98Targets核心保持，不将哨兵改为提前return、选中尾74或目标空失败，不补caster保护/owner或新异常模型。

<a id="battle-m-skill98-mode15-16-sampling-decision"></a>
## 五组差异

| 组 | 恢复 |
| --- | --- |
| 目标 | 同解析S32 target的255比较先pub。 |
| 效果参数 | firstImage→flags→target→caster→actor。 |
| 图号选择 | 两个原S32计数，不用size_t非地址拓宽。 |
| 辅助参数 | selectedImage→secondImage→firstImage→flags→target→caster→actor，保持效果调用后取样。 |
| player | Append34后target先currentPosition，256/52沿该次位置，51再live target。 |

单根五处之外的wrapper/准备/其它mode/两个正确尾helper、布局与声明保持；非拥有native T*仅局部借用，不新增原F/X/adapted/分类/完整成员、公开接口或owner。网络原格式仅明文/音频暂停，保正常S32/W32及原业务范围/常数，不新越界/数学溢出/输入拒绝/异常补零/钳制/回绕或确定值模型。

<a id="battle-m-skill98-mode15-16-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 五范围363条1280B九页35call连续字节/直接目标闭合，两条主体/真实不同参数尾均核，既有两尾18条67B与BMX当前证据/后缀一致；四条18B比较与原初始化/202B仅上下文。 |
| 实现 | HEAD整CPP单根五组五定义五处精确差异，两个原路径的目标-参数/图选择/player原序有限静态，真实wrapper6/3及其它根头保持。 |
| 消费 | 三处静态Take循环/无actor空门禁，原图值跨目标与111011/250/金额6或3/159、flags条件重读-34或165-11/47/46与施法者终结Move-RID-增58保持。 |
| 台账 | 旧812有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S/七open、完整门禁和其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI、其它mode或完整98/owner运行。 |

<a id="battle-m-skill98-mode15-16-sampling-source-state"></a>
## 当前状态与后续

15/16两条原主体和真实尾均纳管，单共用根五处实修；两既有尾18条67B非新增，不据相似表面省略第二路径。状态目标、17/默认仍按真实依赖子SPEC开发，其它技能-全路由/完整父-SEH/全部消费者-owner重进退出借用失效/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E413→417仅双域四局部I/S；关联率不是完成率，本组14文件，快照`cg2d-re-battle-m-skill98-mode15-16-sampling-20261006`。
