# 交易面板数量消费与滚动取样 SPEC（TQN01）

2026-10-11。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。沿子AGENTS第8条工程委托裁决，同一cg2d_re分支，不打tag。

<a id="trade-quantity-baseline"></a>
## 原端合同

TQN01仅共享交易面板4B1260完整524条1693B/九页、31E8十一目标、一个14参数间接callback、41跳一RET。入口window.elements持址→elemX低W16→elemY低W16→windowX低W16/x低W16相加→windowY低W16/y相加，结果按S16传递；现四个2B memcpy保证别名合法/原低宽读取，不改储存字段。state21H门槛先wheel S32→scrollPosition S32→保存元素地址/4743B0→写*scrollValue，随后fresh window.elements定位scroll、data flags只低BYTE bit1/真实474270/原写值。五个S16矩形依次hit，后命中覆盖先命中，不提前break；state bit2按S8旧tab/selected选SE51或55，声后发布tab BYTE再4B09E0。state400/tab1才按selected3/4及滚动S32边界0/2调整/原SE；声后先fresh *scrollValue后fresh window.elements持scrollTarget→4742C0，两分支不跨声效缓存、不合并读取。numericElement>=0/tab2才先descriptor colorW32→currentInput身份flags3/1→已完整NPI数值面板，numericResult只低BYTE。callback非空时既有-1哨兵nullptr适配沿旧已批不用该参数边界保持，正索引借同W32值槽；不扩到所有负索引保护。回调先借值槽→scrollSnapshot→mouseY后mouseX两个真实CryptInt→14参数，原itemCount/petCount/amount/displayAmount地址和非拥有descriptor保持；displayAmount原由callback写，不补零或为无回调分支捏值。回调后每次fresh tab绘制三页签，七DrawGraphic3包括背景，scroll/previous visible两槽分别fresh window.elements置0，tab1绘制previous时windowY→previousY→previousX→windowX/X，两个上下按钮按selected/state bit8映射，最后两个visible分别fresh置1。原四格式%9d/%3d/%3d/%10d、52B未清buf及四Text、背景按S8 tab索引不保护，S32返回state&2即0/2，三入CALL三父的各四后邻只有局部透传来源，两个RET后的align/下一函数不纳本父。五逻辑组六处/两注释，旧949格式历史保持追加，不新建F重复/改变签名与已有callback哨兵策略；一adapted、INPUT/UI/RENDER各一新成员、三限定S零I八open。所有共享数据生产/所有业务callback完整消费、窗口输入与descriptor alias-owner寿命、SDK/生成机器取样/ABI及完整交易画面声效和运行待。仅正常整数数学及原钳制/门槛保持，不新增低/高索引保护、补零/下限/回滚/默认值/异常溢出模型；WAV-only/MSVC/明文协议及全部完整门禁保，整体未完成。

完整原证、三入CALL、S16矩形表见[证据](cg2d_trade_quantity_panel_spec.md#evidence-archive-20261011)。复用[NPI01数值面板](cg2d_numeric_panel_input_spec.md)、[滚动位置合同](cg2d_window_scroll_position_spec.md)已有完整提供者；不重新计父/callee原根或已过格式历史。下列只本步正常源语义静态，非实际交易与完整owner/ABI验收。

<a id="trade-quantity-decision"></a>
## 方案及取舍

既有函数内五组：四低W16字段显式2B memcpy原序；scroll flags只低BYTE；初始wheel分支scrollPosition先元素地址；两个声效后滚动值先fresh元素地址；previous框windowY/previousY/previousX/windowX原序。共六片段两注释，无新函数/helper/API/字段/owner。保存原-1哨兵适配，不补无callback的displayAmount或其它保护，四格式MSVC和数量/页签/滚动业务门槛保持。

<a id="trade-quantity-acceptance"></a>
## 本步验收

完整524条1693B/九页、31E8十一目标/一间接14参callback、41跳一RET，三CALL三父/有限后邻和40B/20S16矩形表有据。两整源精确回放，三类型八提供者/三当前调用源及七已有记录保持；格式历史旧949只合同追加/一adapted，INPUT/UI/RENDER各一成员，三限定S零I八open。仅reviewed处置度量变化，其它计数和全部完整门禁保；汇总/-Check必须终态0，未构建/测试/运行/产物/摘要。

<a id="trade-quantity-source-state"></a>
## 当前状态

旧949格式历史追加共享交易面板4B1260完整524条1693B/九页，31E8十一目标/一个间接业务callback、41跳一RET。五逻辑组六处低宽坐标/滚动标志、滚动值先元素与前一滚动框坐标取样对齐；原数值确认结果、callback哨兵适配、四格式及S32 0/2返回保持。一处置、INPUT/UI/RENDER各一成员，三限定S零I八open；完整共享生产消费/业务callback、owner/ABI及交易运行待。

语义2323=2275F关联+48有界代码，关联2385/3680=64.81%仅关联率保持；处置1780/证据1580，INPUT151/UIRUNTIME192/RENDER109。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_trade_quantity_panel_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐交易面板数量消费与滚动取样合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4B1260 | 0x4B18FD | 524/1693 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
