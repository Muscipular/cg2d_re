# cg2d 输入上下文切换与初始化还原

## 函数边界

- `Input::SetContext@[0x44B330,0x44B33A)`，3 条指令、1 个基本块，原型为 `void(InputContext*)`。函数只把参数写入原 `gCurrentInputData@0x626C04`，允许写入空指针。
- `sub_449CF0@[0x449CF0,0x449D92)`，41 条指令、3 个基本块，原型为 `void(InputContext*, int, int, int, int, int, const char*, int, int, int, int)`。
- `sub_44A160` 是初始化器使用的私有文本装载辅助函数，不公开为新端接口。

## 固定字段写入

`sub_449CF0` 按以下顺序写入，未对顺序或目标字段作归并：

1. `field130@0x130`
2. `renderMode@0x138`，完整写入 32 位
3. `color@0x140`
4. `field134@0x134`
5. `maxCharPerLine@0x148`
6. `fontType@0x13C`
7. `encrypt@0x150`
8. `maxLine@0x144`
9. `field14C@0x14C`
10. `text[0]@0x000 = NUL`
11. `totalByteLength@0x12C = 0`
12. `caretByteOffset@0x128 = 0`
13. `field154@0x154 = 0`
14. `field158@0x158 = 0`

`field138` 在结构中保留为 4 字节原始存储；实现使用 `memcpy` 完整写入 `renderMode`，不通过不兼容指针别名访问。固定字段初始化阶段不清除 `text[1..287]`，也不写 `x@0x120`、`y@0x124` 或其他字节；非空初始文本造成的后续文本与计数变化只来自下述追加路径。

## 初始文本路径

- `initialText == nullptr` 时，初始化器在固定字段写入后直接结束，不进入文本装载路径，当前上下文指针保持原值。
- `initialText != nullptr` 时，初始化器先保存当前上下文，再通过 `SetContext_44B330` 切换到目标上下文，调用私有 `sub_44A160`，最后通过同一函数恢复保存值；保存值允许为空指针。
- `sub_44A160` 先检查当前上下文；为空时直接返回，非空才以 `strlen` 取得初始文本长度。长度大于等于 `0x120` 时截为 `0x11F`。随后逐字节调用现有 `AppendChar_44ADB0`，不直接复制文本，因此追加函数的过滤、行限制及伴随状态写入仍然生效。

本项不建立账号、密码或角色名上下文实例，不建立焦点表，不实现 `sub_45C7C0`、`sub_45CA20`、输入渲染或 IME。

## 完整回车提交44AB60实施 SPEC

2026-09-28，原会话只读核[44AB60,44ADAF)全部指令和四个原字节提示，
沿既有ProcessKeyInput_4596D0的detail::Sub_44AB60调用落实完整入口。
按工程委托，定义放在既有Input.cpp后部，直接复用同文件已有的
world::detail::gChatHistory621FF8及gCursor62683C；不搬owner、不新建
历史表或为本函数公开其结构。玩家名用既有BorrowPlayerDisplayText(1)
借用，输入上下文均为原生T*身份，不把原地址常量用作数值句柄。

### 普通聊天与历史

IME组合首字节非零立即退出，不写输入相位。IME为空才读取当前context。
context严格为626AA0时，检查+12C totalByteLength严格0立即退出；
不误用反编译的cursor名称，也不改为检查光标或text[0]。
读取历史尾槽完整W32，另保留其低S8符号扩展为旧比较索引；
完整值按S32加1、先写尾槽，结果>=64才写0，不额外处理负值。
原44ABB3为movsx ebp,al，旧比较索引不是完整尾槽或无符号U8。

以原字节strcmp比较固定输入与旧索引历史：
相同则把旧S8符号扩展值写回完整尾槽，不持久化且不重读context；
不同则把固定输入含NUL复制到新的历史槽，再取共享尾槽作为S32调用
449FA0，忽略写文件结果并重读gCurrentInputData。
不先验证历史文件尾槽、不截短复制、不以写文件失败阻断发送。
随后先读取626AA0.field140颜色，再把历史游标62683C=-1，
调用完整50C940(context,color)。无论发送是否被限流或失败，
逐次重读当前context并按totalByteLength、caretByteOffset、text[0]
顺序清零，再重读context进入公共后半段。

### 其他输入与黑名单

context分别等于D66CA8/CB03F0/CC46E0/CC2080/D2D828/CA0180之一，
则AppendChar(CR)，之后重读context；其他context不追加字符。
非上述对象且为空的context仍能进入公共相位写入，不新增空指针早退。

后半段context为C8B1B8才处理黑名单；原对静态对象地址再次非零判断
恒成立，不替换成检查文本是否为空。
先strcmp输入与同源玩家名，相同用原554938提示；
否则ContainsBlacklistRecord_4013D0非零用554928重复提示；
未命中才AddBlacklistRecord_401270，低S8非零选5532C4成功格式，
为0选5532B0失败格式。保留sprintf到未初始化256B局部区，
格式中的%s使用C8B1B8原文本，不转码、不复制名称或补长度门禁。
四个原字节格式（正文长度，不含NUL）为：

- 554938，26B：`B1 7A A1 4B A6 70 A6 B9 B9 BD B4 63 A6 DB A4 76 AA BA C1 6E AD B5 B6 DC A1 48`。
- 554928，14B：`25 73 A4 77 A6 62 B6 C2 A6 57 B3 E6 A4 A4`。
- 5532C4，16B：`A4 77 B1 4E 25 73 A5 5B A4 4A B6 C2 A6 57 B3 E6`。
- 5532B0，16B：`25 73 A5 5B A4 4A B6 C2 A6 57 B3 E6 A5 A2 B1 D1`。

生成提示后按text[0]、totalByteLength、caretByteOffset顺序清黑名单
输入；44AD7D实际地址C8B2E4(+12C)、44AD83为C8B2E0(+128)，
不信其反编译字段名。随后AppendMsg(buffer,4,3)、SetContext(626AA0)。
所有未早退路径最后将626C0C写20，不添加本函数没有的音效。

仅静态核对字段、索引解释、完整分支、原字节、持久化/发包与清空次序，
不构建、测试、运行或检查产物。接通已有回车调用不等于初始化/全部
编辑叶、窗口原型或完整聊天收发已验收；50CAE0回复链仍须继续。

实施状态：44AB60已在Input.cpp完整落位，现有InputKeyDispatch回车
调用解析到该本体，并接已实现50C940、449FA0及黑名单查询/插入。
同文件直接复用历史区，未新增公开存储或改变InputContext的15C布局。
四原字节提示、低S8/完整S32索引、长度字段及上下文重读/清空次序、
既有源登记与接口仅静态核对，未构建、测试或运行。

## 初始装载与字节分类数量宽度 SPEC（2026-10-02）

在追加本体修复后，沿其真实装载/分类依赖完整核五个入口：44A160共31条、
44B1D0共44条、44B190共21条、449CF0共41条、44B340共79条，合计216条。
按工程委托保留既有函数及调用，只收回两处被宿主size_t扩宽的原W32数量：

- 44A160在44A17E用signed比较长度与288，44A18D/44A19E用signed控制循环。
  strlen结果在边界转S32，length/index均保持S32；入口context为空直接返回，
  只取一次初始长度，>=288时取287，逐字节重新读取initialText并调用AppendChar。
  不提前复制源文本，也不按追加是否成功改变循环次数。
- 44B1D0的Count为W32；strncpy数量只在库调用边界按U32传递，不能把负S32
  直接转为x64 size_t。终止位置保持原S32偏移，扫描数量按S32比较。
  IED01已撤销旧288B局部预清，原复制填充/终止保持；负值、越界不专项模拟，不增加门禁或声称故障等价。

44B190完整高位双字节/孤立高位尾的2/3分类不变，不替换为系统代码页或UTF-8。
449CF0字段写序、仅text[0]清零及临时切换/恢复当前对象保持；不清整块上下文。
44B340仍优先Shift、其次Ctrl，依次读取F1至F8，最后一个命中决定0/8/16组索引，
先装载再调用原音效入口。原调用传51/320/240；现有音频窄接口消费前两项，
不在本步改造该暂停板块或把声音播放记为已验收。

44A160有七处原调用（历史浏览两处）；44B1D0有七处编辑调用，44B190有五处
输入/文本调用；44B340唯一caller473F80已接。源码关系仅登记真实现有消费者，
不以这五项齐备宣称整个输入板块、所有UI父链或全客户端完成。
只核本次数量修改、既有流程与映射，不构建、测试、运行或哈希校验。

## 2026-10-05：分类与编辑局部存储当前状态

[IED01](cg2d_input_edit_alignment_spec.md#input-edit-source-closure)已撤销44B1D0额外整块预清：真实StrNCpy_53BEC0低U32 Count、signed终止位置和S32扫描/0～3返回保持，不删原padding/NUL或加Count范围拒绝。七原点绑定与六编辑本体局部已对照，上下扫描预算/终行重新取长、退格钳制后对象重读按原序。此前日期保留预清是旧源码状态，固定历史见Git命名快照cg2d-re-input-render-source-20261005；完整输入/别名/SDK/运行仍未验。

## 2026-10-05：追加前置状态差异当前对齐

[IAP01](cg2d_input_append_alignment_spec.md#input-append-source-closure)撤销44ADB0非原有HasSafeLegacyState与两临时副本预清，原context为空/控制字节早退及当前对象逐阶段重取保持。三复制289/288/288、实际补零/NUL、低U32拼接余量和原截断/光标钳制/回滚不改；00仍按原接受，不能用可见串长/计数一致性补门禁。超行maxLine/626C20一次共同取样已正确；203条/实际直接引用仅局部静态，完整输入/别名/生命周期和运行未验。

## BPH01 预设键索引宽度与真实文本消费（2026-10-06）

[BPH01 SPEC](cg2d_key_index_preset_source_spec.md#key-index-preset-decision)/[原证据](cg2d_key_index_preset_source_evidence.json)。44B340只将0x70+index直接交完整S32查询，原Shift优先/否则Ctrl、F1..F8全扫/最后非零覆盖、未选返回最后查询保持。选中仍借同一24项15CH上下文文本，先44A160装载再52C450并透传；现有现代473F80唯一调用保持，不能当旧99路由已补。

原音效caller压51/320/240，164条551B支持体仅入口参数+4/+8读取，额外第三值未消费，现有两参调用保持。支持只参数消费，独立于三根87条分母、不纳音频验收或恢复播放。旧373全文映射归档后续记，正常旧输入结果不变。语义1422→1424条/有效ID1417→1419、关联1535→1537/3680（41.71%→41.77%）、分类543→546，INPUT14→17/TEXT22→23枚举未闭；E707→715仅8条正常局部I/S。60任务源码closed10/完整验收0、叶验收0/global_accepted=false及全部完整门禁保持。 所有其它输入正文/焦点/生命周期、SDK/ABI与实际输入画面运行未验。

## 旧分隔符扫描单BYTE取样更新（BSX01，2026-10-07）

按[BSX01裁决](cg2d_legacy_separator_byte_sampling_spec.md#legacy-separator-byte-decision)，45E770首BYTE一次取current、同样本供NUL和分类，末尾cursor[1]先取再++cursor。撤for门禁／循环体重复读取，不改0x81 unsigned两字节状态／尾字节跳过、非尾7C或5C返回完整1、空串／末lead遇NUL完整0，const文本不写且不建owner／新规则。

原25条57B／一页、无call／八jump／两RET，十实际CALL在七父；新增1602单真F、INPUT26→27及CHARACTER17→18真实消费绑定，两域各局部I-S。7B相邻NOP只用途。其它整个Input、所有消费者及接口保持；原姓名20 00／81 40 00字节正确保，不据编码猜测合并字节规则。全部原父／输入文本生产消费／正常alias-owner寿命与运行未验。
