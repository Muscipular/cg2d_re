# 战斗装备与背包空槽 SPEC（BEI01）

2026-10-10。沿IER01及战斗装备实际依赖推进，依子AGENTS第8条工程委托裁决。同一cg2d_re分支、不打tag。

<a id="battle-equipment-baseline"></a>
## 原端证据

49AC40完整755条2502B/十二页，518C60完整九条29B/一页；合764条2531B/十三页、56E8十八目标/103跳五RET。两装备data引用是4C6EC0持址及原型槽20，不计CALL；空槽十九入CALL/后邻来自十六父，仅登记来源，本父49AF38零判和49B1D7与武器数比较全S32，其余十七调用点/十五外父完整合同待。原FE0B84..FF02B4按C5C步长为唯一玩家slot8..27，共20项used W16，现统计正文正确。旧924已有两Q格式证，完整历史保追加，不重复F。

<a id="battle-equipment-decision"></a>
## 实施合同与边界

BEI01战斗装备49AC40完整755条2502B/十二页与空槽518C60完整九条29B/一页；合764条2531B/十三页，56E8十八目标/103跳五RET。装备两data引用：4C6EC0原持址及原型槽20，不造普通CALL；空槽十九入CALL/后邻仅登记来源，本父两完整消费分别全S32零判/与武器数比较，其余十七调用点/十五外父完整合同待。49AC40原items/count低AL失败返S32零及两16B白/半透明descriptor/byte13=4保持；S8mode正时一次pet/slot数，原动态slot循环先x/y后used W16，低BYTE事件21/occupancy门槛、命中GBK禁用提示/bit1框、原used-slot禁图22738和占用图/暗descriptor，原跳过路径/consumed与返回保，不新增slot上限或新保护。玩家event60原dragkind!=2或source>=100过滤、drop40来源>=count、来源used==1及itemType0..7/70..79原门槛；双手used==1计数、真实空槽零及battleUseFlag低BYTE bit8才容量提示，否则未鉴定提示或成功窗口8000→选择槽5清0→未清256B Q|%X|来源U32→当前socket发送→动作1/声77→drag reset，发送结果原不消费，不补失败回滚/flush。hover21动态count命中/consumed1，固定item和命中索引；bit2窗口2000后无hover且回复used非零声51→fresh窗口/3FC1/槽20属性回调49B610早返，提示按原used/type/hasDescription Word及battleUseFlag低BYTE bit1/已批GBK。双击只手位2/3统计非type7武器，真实空槽<weapons容量提示、weapons0无武器提示，否则窗口8000→选择槽5清0→原255u Q命令→发送→动作1/声57；focus和reset/consumed及失败提示原序保。单击48BF10/声51与右键原页递增/pages业务归零、拖框/hover框和末逐槽used/禁图及未鉴定独立重复22738、flags低BYTE bit80后22739/occupancy/暗或普通image均保持；不合并原两次禁图。区域描边一次当前window→hoverColor U32→elements T*→windowY/X→elementX/height→elementY/width，再算原四边/低U16/层223/heldcolor；不缓存其它描边或改绘制入口。四组四处：battleUseFlag两BYTE、flags一BYTE及区域描边持样/读取序；Word字段储宽、GBK/MSVC两格式点/原command字节、其余正文/owner保持。518C60现正确正文保：唯一玩家item区原FDA8A4/strideC5C，原FE0B84从slot8到FF02B4排除slot28，共20项，逐used W16==0累加S32，忽略occupancy/其它字段，不复用宠物item区、不增加钳制/默认。两根定义/两声明四合同注释、四源全文限定diff及正确空槽/类型/既有callee/原型保持；旧924格式历史追加、一新F两adapted/五限定S零I九open。全输入/物品/宠物生产/19外父/动态callback分发、命令与callee/SDK/alias-owner寿命、生成BYTE/机器ABI/实际战斗操作画面声音与运行待；无新公开API/helper/结构/字段/owner/保护/补零/默认值/新业务钳制/回滚/异常模型，MSVC/WAV-only/当前原命令字段LF明文网络、48有界代码/完整枚举gates/六closure/总SPEC§10保持。

两处battleUseFlag和一处flags保既有W16存储，unsigned char别名读原低BYTE；不重构字段或迁移其它消费者。区域描边保一次借window与U32颜色，元素表和八字段按原序读取，仅本区域保持四边低U16。其余框/descriptor/逐槽和全部已批GBK/MSVC格式保持，不缓存整回调。

空槽依赖完整正常根独立登记，现有正确S32循环保持。原20槽不受当前占用BYTE影响；装备调用只按真实结果处理，不以固定值补依赖。卸装备命令保原255u，发送后动作/声与原reset不按返回回滚；两个禁图条件独立，可重复绘制同图，不擅合并。

<a id="battle-equipment-acceptance"></a>
## 限定静态验收

核两连续完整原根/56E8、三BYTE/区域八字段取序、原20槽S32生产和两完整本父消费、十九CALL来源/两data绑定。四整源精确差异及四注释、正确空槽正文/类型/原型/既有callee保持。旧924仅追加、一新F/两adapted、五限定S零I/九open；十九调用点的完整外父/数据生产/callback/命令/callee/alias-owner寿命、SDK/生成取样/实际战斗交互像素声效与运行待。完整枚举/六closure/总SPEC§10不提升。

只做必要静态文本及measure_cg2d.ps1/-Check，不构建/测试/运行/检查产物/摘要/新依赖。

<a id="battle-equipment-source-state"></a>
## 当前状态

前提交“[#] 补齐装备交互与属性回复合同”；拟提交“[#] 补齐战斗装备与背包空槽合同”。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。

语义2306=2258F关联+48有界代码记录，函数处置1760/证据1553，INPUT135/UIRUNTIME173/RENDER99/NETWORK91/BATTLE427；文档关联预期2368/3680=64.35%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_battle_equipment_interaction_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐战斗装备与背包空槽合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x49AC40 | 0x49B606 | 755/2502 |
| 0x518C60 | 0x518C7D | 9/29 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
