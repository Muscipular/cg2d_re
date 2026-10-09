# 战斗图号与配对位置依赖 SPEC（BDI01）

2026-10-10；核既有完整本体及独立原身份。本步不修改客户端源码，不把已有函数称新增实现。

<a id="battle-scalar-dependencies-baseline"></a>
## 原基线

| 原身份 / 当前提供者 | 完整原根 | 本步范围 |
|---|---|---|
| F:00004D90 / SelectIntValue_404D90 | 404D90–404DAF，13条31B | 三有符号门禁、只在有效索引读取一个S32、两个返回 |
| F:00005B40 / GetBattlePetPosition_405B40 | 405B40–405B5F，13条31B | 两区间判断及加/减5、两个返回 |

[完整指令与字节、入向和当前源码](cg2d_battle_scalar_dependencies_evidence.json)保存26条62B、七跳/四RET、零callee。前者四普通CALL/两父，后者29普通CALL/16父；这些引用只证明局部调用事实，不代33调用处全部父或完整战斗/UI流程验收。

404D90在[BNT01](cg2d_battle_skill_targets_core_spec.md#battle-skill-targets-core-baseline)已有局部基线，现补独立身份，不重复计算原实现。新增核54D450五项及复用54D710/54D724两表；三表均S32五项、合60B，当前constexpr内容逐项吻合。409680的3100技能、426FC0首次图号与target<10两分支使用原表/数量5；四原CALL不按新端三个调用表达式计成缺一调用，条件选表承接原两个互斥分支。父完整生产/消费仍沿其各自合同验收，邻窗不是完整父。

<a id="battle-scalar-dependencies-decision"></a>
## 实施边界

1. 两现正文、公开/内部声明及所有调用保持。图号选择严格count<=0、index<0、index>=count任一成立返回0，先门禁再读values[index]；0是原业务返回，不是新增保护或默认成功，不加null检查/钳制/重排。
2. 配对位置严格[0,5)或[10,15)加5，其余减5；参数/结果是S32，不擅收窄到0..19、拒绝负值或按指针扩宽。RPLAN.24不新增溢出/越界/回绕专项处理或故障等价。
3. values仅按已证指针拓宽为非拥有const S32*，数量/索引/表项均保持S32；两函数不创建或释放owner、不读写共享状态或消费RNG。实际表生产、载荷/场景借用寿命及调用者结果仍按完整标准，不据叶函数正确宣全链通过。
4. 仅新增两个现有原F的语义及adapted处置；BATTLE补两已知成员，UIRUNTIME只补配对位置共享成员，枚举仍false。各域仅一个限定S，零新I；不更改完整门禁、原分母、任务状态或六closure。
5. 旧CRT排序待决、WAV-only及全部执行限制保持。只做静态指令/文本/台账核对，不构建、测试、运行客户端或算法、检查产物、计算摘要或引入依赖。

<a id="battle-scalar-dependencies-acceptance"></a>
## 静态核对

核两原完整根的13条/31B及每条取值、条件/返回，与现S32签名、正文/声明对应；核三表15项/60B和指定调用的参数、原两互斥表分支。核两身份来自原库存，BNT01的旧基线和父记录不重写；33原入向仅定位，全部父/callee、业务结果/数据/owner、SDK生成ABI及画面运行不记通过。客户端src/include/CMake零差异，语义/登记仅具名增量、政策/全部完整门禁保持；显式UTF-8无BOM/LF和静态汇总/-Check一致。

<a id="battle-scalar-dependencies-source-state"></a>
## 当前状态

两正确函数及所有业务源码逐字保持，独立身份补到语义/验收台账；完整原根、三个实际常量表及现源片段可追。命名前态cg2d-re-battle-scalar-dependencies-history-20261010，提交后引用cg2d-re-battle-scalar-dependencies-20261010。U08/R01/R04/V04保持open，未新增功能、编译/运行或完整验收；整体SPEC未完成。

### 本步实际限定结果

必要静态和汇总/-Check通过；两完整原根/现正文及三表一致，源实现保持。新增两语义/两F处置与两个限定S，语义2032（1984F＋48X）、合同2094/3680（56.90%）、F1457/E1344；BATTLE271/UIRUNTIME67且枚举false，184叶完整验收0/60任务关闭11及六closure保持。十一自有文档/台账及派生文本显式UTF-8无BOM/LF，客户端源码/头/项目及总SPEC零改动；未构建、测试、运行、查产物或计算摘要，整体SPEC未完成。
