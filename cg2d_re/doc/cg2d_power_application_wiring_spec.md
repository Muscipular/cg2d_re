# 完整幂接通两地图原调用点SPEC（BWL01）

> 2026-10-09最新数值实现：[ACP01-M](cg2d_asm_to_cpp_migration_spec.md#asm-cpp-map-source-state)将地图13数学/投影入口迁CPP/double/std数学，覆盖本组旧x87/PC53/ST0数值保证；原字段、F32落存与读取/发布序保持。原ASM实现/机器验收留作历史，不代当前CPP编译/运行。

<a id="power-app-baseline"></a>
## 实际接线缺口

BWK01已有完整CI/core及真实依赖，但SceneState.cpp的Sub_46F1D0两次spawn仍经MapEffectMath.asm的Cg2dMapEffectDivisor内联固定1.25域算法。该入口的原46F38D/46F43A调用点必须接真实完整CI，不以算法已存在代应用接线。

本轮当前cg2d会话只读核两个从参数生产到截断消费的窗口：46F37B..46F3C5及46F428..46F472。每窗先FLD F64 1.25，再将signedLevel−1 FILD，CALL53D000，之后FMUL F64幅值/FCOM F32零/状态和回退、除法及原ftol/低S32消费；两窗均21条74B，共42条148B、四CALL（两CI/两ftol）。完整原父/幂原体复用既有证据，不全面重核未变内容。

现Cpp只有一个除数调用表达式，spawn先kind6/levelC8A752/min1000，再kind7/levelC8A754/min500，两次实例对应两原窗口。它的signed16位解释、level−1、原零divisor→1及每次后续RNG消费均已实现，本步保持。

<a id="power-app-decision"></a>
## 实施决定

- 只改MapEffectMath.asm/.hpp五处：一已有除数本体替换、一个真实CI EXTERN及现注释/合同更新，不新增本体/原F/依赖/CMake项。不改SceneState.cpp和LegacyFloatEnvironment两组真实提供者。
- 除数改正常72B FRAME/ALLOCSTACK/ENDPROLOG，保32B调用区；本帧指数S32在32、原保存CW W16在36、027F和0E7F分别38/40，借用幅值指针在48，原I64截断槽在56。只拓已证指针，不扩大数值字段，不填padding/未生产字节。
- 保现D01/MFA PC53阶段与外层CW保存/恢复；把旧FILD指数/FLD底数/内联FYL2X与TwoToTOS撤掉，按原FLD底数→FILD指数→真实Cg2dLegacyCIPowerX87顺序调用。CI/core/原异常/范围链已经有完整本体，不加包装/默认成功或输入限制。
- 完整CI/core会返回原W32控制字载体到EDX，因此调用前把原生幅值指针独立保存、返回后重读；不截地址/拿载体当指针。幂结果不落F64，原FMUL/零比较与状态/回退、除法及I64 FISTP/低S32返回连续保持，外层原CW尾恢复不移到别处。
- 两spawn原调用方的顺序/门槛/字段/signedLevel−1、零divisor原回退、RNG和记录生产/后消费均保持；当前两个已证直接CI窗口已接，不外推未知间接alias或其他共享CRT调用父全闭。
- 更新1265/1266/1267/1424/1426/1427六已有记录的当前MapEffectMath关系：旧“本次不改/只内联/通用未闭”等当前描述改为真实CI/core传递调用；旧source前态及MFA/BPJ/BPM/BWF/BWK历史保留。各既有F身份不重计，FUNC-MAP/CRT限定I/S同步，不用来源修正削减历史定位队列。
- RPLAN.24/原格式明文/音频暂停及不执行门禁保持；这是原调用接线及正常帧适配，不加越界/溢出保护、专项故障测试或等价声明。共享543185等CRT分派、全应用/FPU生命周期与机器链接运行继续原依赖。

<a id="power-app-acceptance"></a>
## 可度量验收

| 项目 | 标准 |
| --- | --- |
| 原窗口 | 两个已证CI生产消费窗口/四原CALL，参数顺序及S32截断消费有据 |
| 实施 | 两现文件五改动，一已有ASM本体实际替换，零新本体/原F/owner |
| 接线 | 两spawn→现除数接口→完整CI→core/所有依赖，幅值指针跨CALL保存/重读 |
| 数值 | ST0连续幂消费、原PC53/CW恢复、F64乘法/F32比较/回退/I64截断低S32不变 |
| 保持 | Cpp/RNG/记录/后消费及通用提供者/CMake完整不改 |
| 台账 | 六已有映射/F当前关系纠正，原前态/历史可追，FUNC-MAP/CRT成员129/176保持 |
| 限定证据 | 两域I/S共四，E1158→1162；F999/语义1837/有效1830保持 |
| 差集 | 定位150/CRT未定位22/总队列151保持；不把接线数当新原身份 |
| 任务 | C01/R01/V02/R04同步；完整枚举/gates/pending18保持open |

只做必要静态/完整差集和文本汇总/-Check，不编译、测试、运行样例或计算hash。现两个直接原点的源码接通不代机器调用/ABI、未知调用父/alias、全应用/FPU环境生命周期或真实运行。

<a id="power-app-source-state"></a>
## 状态

先记录本SPEC，再依工程委托接两真实调用点与当前关系；实际静态结果随后追加。

### BWL01本步实际静态结果

两个已证原窗口46F38D/46F43A已由现两spawn/除数入口接完整CI/core/真实依赖。原两生产消费窗各21条74B，共42条148B/四CALL；原FLD1.25、signed16减一/FILD、CI及幅值/零回退/除法/ftol低S32后消费有据。只核新增关联窗口，完整原父与幂原体复用原证。

两现文件五改动、一已有ASM本体实际替换，零新本体/原F/owner。除数72B正常展开帧保32B调用区、S32指数/三W16控制字、原生幅值指针与I64截断槽；原底数后指数真调CI，返回后重读幅值指针，不拿原EDX控制字载体当地址。旧固定域FYL2X/TwoToTOS内联撤除，幂ST0连续到FMUL/零比较与回退、除法和原截断低S32，外层PC53/CW阶段及尾恢复保持。既有startup构建同时纳管两ASM，未添目标或依赖。

SceneState.cpp全文保持，现两spawn的kind/level/最低速度顺序、门槛/signedLevel−1、原零divisor→1/RNG/记录及后消费不改；通用幂三提供者文件/CMake全文保持。两个源完整前后态与六映射完整前态已保存；1265/1266/1267/1424/1426/1427当前MapEffectMath.asm/.hpp关系改为真实CI/core传递调用，旧内联关系不再冒当前实现，MFA/BPJ/BPM/BWF/BWK历史仍可追。

集中源码、完整机器/派生差集与文本汇总/-Check通过。MAP/CRT各新增限定I/S共四，E1158→1162；既有MAP129/CRT176、语义1837/有效1830、文档1947/3680（52.91%）与F999保持。源码定位150/范围内未定位CRT22、总差队列151和暂停25/排除103/元数据1不变：本步真实应用接线及六当前绑定纠正，不重复计原身份或把已定位功能再削队列。

C01/R01/V02/R04与两域完整枚举/gates仍open；pending18、商店mixed-pending6、创建pending7、R7已审467余2594、closed11、完整叶0/globalfalse保持。当前只对两个已证直接CI窗口源码接通有据，不外推未知父/alias、共享CRT分派或全应用/FPU生命周期已闭；下一步沿这些真实依赖继续。机器展开/ABI/链接及运行待，未编译、测试、运行样例或计算hash；无新保护/故障样例、宿主pow或输入限制，RPLAN.24/原格式明文/音频暂停保持，整体SPEC目标继续。
