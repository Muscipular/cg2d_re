# 重生与属性附属效果完整合同 SPEC（BRAE01）

2026-10-10；沿阶段14真实依赖补4208F0/4209C0完整本体。两当前正文已符合原合同，只补四条合同注释及身份/语义/验收登记，不称新增实现；原失败消费、字段宽度、取样顺序及未决owner/SDK门禁保持。

<a id="battle-attached-effects-baseline"></a>
## 完整原根与调用

| 原身份 / 当前源 | 连续完整根 | 指令与调用 |
|---|---|---|
| F:000208F0 / NewBattleRebirthEffects_4208F0 | 4208F0–4209BD，52条205B/一页 | 两directCALL/一callee、四跳一RET；27实际CALL/21父 |
| F:000209C0 / NewBattleAttributeEffects_4209C0 | 4209C0–420CF6，236条822B/四页 | 八directCALL/一callee、50跳两RET，其中两JMP取表；32实际CALL/20父 |

合288条1027B/五页、十directCALL；八阶段14窗口仅证明三重生/五属性调用均不消费返回，不能代两调用域全部父；二域去重后共22父。属性原只发布AL，不承诺EAX高位；现U8返回与原低八位掩码保持，所有其它父/ABI仍待。重生现void接口和已证局部弃返保持，不猜原未消费余值。

## 重生类型与两段创建

原先source空返回，再target空返回；随后固定两payload指针。八个S32图号由原八条立即数写本地表：sourceType-1使用110431/110443/110455/110467，targetType+3使用110430/110442/110454/110466。实际字段都是1080低S8，不读106C；零时跳过对应创建，非零按有符号索引计算，原没有类型范围拒绝，不增钳制/补零/故障模拟。

先取sourceType，非0则NewRandomEffect(target,image,0)，对保存返回T*依次drawMode第11 BYTE=96h、rebirthValue1 BYTE=4、rebirthValue2 BYTE=4。原无分配返回空检查，当前不新增保护。第一段结束后才fresh取已经固定targetPayload的targetType，再按同样顺序创建/发布；不提前合并两类型、不fresh改取target.payload或给source创建。两原调用表达式与当前两处对应，所有正常RNG留在真实BNV01创建器，不重复创建或新增RNG。

## 属性入口门禁与四W16

原先取source.payload并测试stateFE0低BYTE bit1，再取target.payload并同样测试；之后读取一次全局U8 flags59F9E0与S32 battleType。依次按原条件返回0：flags bit1清且type非5/6；flags bit2清且fresh mode5ABFCC==1；flags bit4清且type为5/6。随后才检查source/target及两个payload为空。原门禁位置保持，不把后置空检查提前或写成覆盖前置解引用的安全保证。

source/target四属性从原288/28A/28C/28E的W16非零取得本地0/1。source特殊技能19/23/27与2068..2070只置属性0；20/24/28与2071..2073只置1；21/25/29与2074..2076只置2；22/26/30与2077..2079只置3。2055..2057保持四项0；其它技能按四W16逐项读取，source FDC bit800置时对换0↔2、1↔3。特殊技能不补source四属性或bit800读取。

target始终按四W16顺序取0/1，随后按固定targetPayload的FDC bit800作同样对换；这些本地向量在全部创建前已取齐，不因前一图标创建去重读属性/技能/门禁。两组switch完整76B数据沿CAC07的90B原X记录复用：420CF8十二W32目标、420D28四W32目标、420D38十二BYTE选择器，所有target仍属4209C0。90B中的对齐和尾padding保历史，不重计X/新函数。

## 八配对顺序、纵偏移与U8掩码

起始y=-20；按下表顺序，source/target对应本地属性都非0才调用NewFloatingIcon(target,image,0,y)，弃返回后置相应掩码位，再将后续y减22。未匹配不创建、不消耗纵位置；原最后一槽不再修改无消费者的y，当前循环的末次局部减22不发布/消费，正常结果保持，不造新逻辑或稳定性保证。

| 位 | source属性→target属性 | 图号 |
|---|---|---|
| 1 / 2 / 4 / 8 | 0→1 / 1→2 / 2→3 / 3→0 | 22455 / 22456 / 22457 / 22458 |
| 10h / 20h / 40h / 80h | 0→3 / 1→0 / 2→1 / 3→2 | 22450 / 22451 / 22452 / 22453 |

掩码仅表示匹配并调用：原八CALL后未测分配返回，失败也置位并占后续y，不将掩码当成功分配数或回退y/重试。早退低AL=0，完整路径AL=本地掩码；U8返回不扩大为原全部EAX/机器ABI已验。BOH01真实图标创建器只复用，全部owner/资源和画面仍待。

<a id="battle-attached-effects-decision"></a>
## 当前实施与旧记录

一个CPP/一个公头各两条合同注释，两个完整正文/签名及其它全部源保持，零新I/函数/API/保护。新增重生一独立语义，属性1915号CAC07记录仅追加BRAE01完整合同/当前状态，保旧原角色/来源关系/机器身份及range_fourth_batch_data完整前态；旧关联证据不冒称当时已有完整本体。两F adapted处置、一限定S覆盖两完整现源；只重生新增已知成员，属性已在BATTLE，枚举false。

U08/U09/R01/R04/V04只补来源/局部描述，原分母、六closure及全部门禁保持。按既有工程委托先SPEC后落实，不改阶段14/16/20/22或其它父，不重计CAC07/X、BNV01创建器与BOH01图标。RPLAN.24、MSVC排序待决、WAV-only和原格式明文不扩；未编译/测试/运行/查产物/计算摘要或引依赖。

<a id="battle-attached-effects-acceptance"></a>
## 必要静态核对

核两连续原根/库存、288条1027B/五页、十directCALL/各callee-跳-RET、两个真实取表JMP/选择器和76B当前数据逐项对照CAC07完整90B前态；八立即数、S8/W16/BYTE及门禁/指针持样/目标类型重读/字段发布有据。核59实际CALL及分域21/20父定位、阶段14八后窗局部弃返，逐项八配对/图号/bit/y与创建失败消费、U8返回保持。

整CPP/公头仅四注释，两正文/签名和所有其它源保持；属性1915旧记录前态按固定Git引用复读，CAC07机器/原X/归属不改。核两处置/一语义/一成员/限定S零I、五任务/政策/门禁/分母、UTF-8无BOM/LF及静态汇总/-Check。完整所有父/输入/间接alias、SDK类型域/正常失败/容量/重进退出、全部资源owner/RNG/画面运行仍未验，完整验收不降低。

<a id="battle-attached-effects-source-state"></a>
## 当前状态

两完整正常源正确保持，四注释/独立登记已落实；必要静态、汇总及-Check通过，一限定S零I，不含编译或运行。前态cg2d-re-battle-attached-effects-history-20261010，提交引用cg2d-re-battle-attached-effects-20261010。整体SPEC未完成。
