# 数量确认与增减输入 SPEC（QVC01）

2026-10-11。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。依子AGENTS第8条工程委托裁决，同一cg2d_re分支、不打tag。

<a id="quantity-callback-baseline"></a>
## 原端合同

QVC01仅窗口42初始化48A8C0/输入激活48A900/数量面板48A950/增加48AA60/减少48AB10五完整原根230条714B/六页；原八个E8六目标、一个间接callback、十九跳五RET（按证据统计校验）。窗口初始化仅value=-1：先currentWindow持址→上限999999999/参数0/callback空→elem2状态0→输入激活(0,2)，所有路径返回1。输入激活bit2调用既有476570(57DA70,0,-100,0)→SetContext(CB0878)→窗口快照→第一次elem0 data发布焦点→第二次同窗口elem0 data发布context，返回0。数量面板currentInput先于actionFlags低BYTE；value14先于currentWindow，调用474550只消费低BYTE，先S32上限钳回后bit1第二比较不补下限；bit2快照非拥有callback，非空调用当前elem[index] S32后重取窗口，随后关闭bit15/声57再重取窗口，无条件用原%9d/MSVC写12B文本返回0/1。增加按钮先窗口/graphic持址→原悬停提示→elem2指针来源，上限S32先当前数量S32，严格小于才+1/声51，否则声55；减少原数量S32>0才-1/声51否则声55；两者原默认→hover→pressed图号顺序，始终返回0。三逻辑组（低BYTE/参数-窗口次序/上限-数量次序）三处源码对齐，初始化/输入激活/减少正确正文保持；不新增保护、下限、补零、回滚、默认值/类型/helper/owner。元素0/2/3/4四callback槽与主原型update只data引用；唯一入CALL从初始化调用输入激活，第三后邻RET外align不纳父代码。关闭按钮/释放仅既有绑定和未变提供者，不据本步记完整原父/取消按键/释放owner闭合；原void业务callback适配仅保副作用，完整机器ABI/动态调度/所有上游与资源寿命、真实画面/声效及运行待。WAV-only/MSVC/明文协议与全部完整门禁保持，整体未完成。

完整指令、来源与元素表见[证据](cg2d_quantity_window_callbacks_evidence.json)。旧937仅26条格式邻窗历史，本步追加83条完整原根及局部合同，历史原样保持，不重复F。

<a id="quantity-callback-decision"></a>
## 方案及取舍

低BYTE actionFlags采用unsigned char别名，value14先于当前窗口持址；增加按钮上限取样先于当前元素2数量。保留既有两次上限钳回/回调非空门槛及窗口复读取样，不补下限/空指针/索引保护或重置。三个其余正确正文保持，仅具名注释，不将取证说成新增功能。复用既有输入/数值面板/关闭按钮和释放适配，不新增现代弹窗或状态机；完整关闭/释放及callback ABI另验。

<a id="quantity-callback-acceptance"></a>
## 本步验收

完整五根230条714B/六页、8E8六目标/19跳五RET及一个间接callback；四元素callback槽/主update持址，唯一入CALL/有限后邻边界明确。必要静态核原字节/字段宽度/读取序、两整源精确回放、三类型/五提供者及原型表保持。三读取合同三处、十注释；旧937合同追加、四新F五adapted，INPUT/UI各五成员/RENDER四，三限定S零I八open。静态汇总/-Check须终态0、其它派生与全门禁保持，未构建/测试/运行/摘要。

<a id="quantity-callback-source-state"></a>
## 当前状态

[QVC01](cg2d_quantity_window_callbacks_spec.md#quantity-callback-source-state)补齐数量初始化、输入激活、确认面板和增减按钮五完整原根230条714B/六页；三读取合同对齐，三正确正文保持，五定义/五声明十注释。原callback非空调用后重取窗口，关闭/声效后再次重取窗口并格式化；保两次原上限钳回及增减门槛。旧937格式历史追加、四新F五adapted，INPUT/UI各五成员/RENDER四成员，三限定S零I八open；完整取消/关闭/释放owner及全callback/ABI/运行待。

语义预期2319=2271F关联+48有界代码，处置1775/证据1571；文档关联2381/3680=64.70%仅关联率。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

