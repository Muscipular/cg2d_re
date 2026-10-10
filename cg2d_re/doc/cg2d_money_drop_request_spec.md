# 丢钱数量回调与请求 SPEC（MDR01）

2026-10-11。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。沿子AGENTS第8条工程委托裁决，同一cg2d_re分支，不打tag。

<a id="money-drop-baseline"></a>
## 原端合同

MDR01仅丢钱数量回调488CF0完整18条61B/一页、按钮488D30完整91条291B/两页、woz发送510DA0完整44条142B/一页；合153条494B/四页，14E8九目标、15跳三RET。488CF0先原FDA88E单BYTE bit80测试，置位即短路不调场景；清位后真实440B50零才玩家Y后X两个CryptInt读取→当前socket/amount送510DA0。现只本处旧W16接口改既有ReadPlayerActionFlagsByte，不改共享W16接口或其它消费者，不加门槛/默认/下限/回滚。原回调最后残留EAX来自场景/发送或未定入口寄存器，现void只承副作用，既有窗口原生非拥有callback类型保持，不能宣完整机器返回/ABI等价。488D30完整正确正文保：入口held graphic先hover现GBK提示；click时先window42非空/sourceWindow14/sourceElement==index三短路匹配，有匹配原SE55，否则fresh player gold单次样本传14/index/244310/amount/amount/0及真实488CF0callback、flags1/mode0/actionFlags0到已完整48AC60；无论两支都consumed1。其后第二fresh三项匹配才再取gold→value10后value14；同held graphic默认244291→hover244293→第三fresh匹配244292，最后S32 consumed0/1，不把三次匹配合并缓存，也不跨创建/声效冻结金额/窗口或改图号目标。原488DCD持488CF0为data push，不伪造CALL；主元素562CD8(24项)slot1的+28绑488D30，回调仅data项/一槽，没有普通入CALL。woz原字节776F7A00/三S32字段x/y/amount，各当前capacity W32位模式S32→真实整数token→fresh输出buffer T* Append，三次独立容量/地址，末freshbuffer及传入socket→真实发送S32透传；当前原命令字段/LF明文沿已批准本机服务端，旧编码/外壳仅历史不恢复加密。发送两入CALL两个父，只E8及四有限后邻：本回调488D24返回前丢结果；另一父只定位，RET外align/下一函数不纳本父，全部外父/数值消费与ABI待。三新F三adapted，INPUT/UI各两成员、NETWORK两/RENDER一，四限定S零I八open；按钮/请求生产已源接线，但完整金额生产/请求回复、数量确认及共享窗口/input/Yobi/协议buf alias-owner寿命、SDK/生成读取和机器ABI、完整网络互通/画面声效及实际操作运行待。三类型十提供者/三旧声明、24元素表及六已有根保持，无新API/helper/字段/owner、索引/容量/空保护、补零/默认/钳制/回滚或异常数学模型。WAV-only/MSVC/明文及全部完整门禁保，整体未完成。

原指令/真实引用和woz命令见[证据](cg2d_money_drop_request_evidence.json)。数量创建/[QIP01](cg2d_quantity_window_creation_spec.md)、确认/[QVC01](cg2d_quantity_window_callbacks_spec.md)、键盘面板/[NPI01](cg2d_numeric_panel_input_spec.md)既有完整合同复用，不重计父/调用方机器ABI或运行验收。

<a id="money-drop-decision"></a>
## 方案及取舍

仅488CF0动作门槛使用已有ReadPlayerActionFlagsByte；保后续原场景短路/Y-X请求，原void callback边界不生成确定残留EAX。两其它正确正文保持，按钮三匹配各自新取、实时gold/value10/14和held graphic保持，woz三capacity→token→fresh输出及真实S32返回保持；原字段/LF明文承接沿已批合同。无新函数/API/字段/保护或回滚，只一处真实读取/六注释。

<a id="money-drop-acceptance"></a>
## 本步验收

完整153条494B/四页、14E8九目标/15跳三RET；488DCD callback data、24表slot1持址、两请求入CALL及有限四后邻分列。四整源精确回放/一处BYTE调用六注释，两个正确正文/三类型十提供者/三旧声明、元素表与六旧记录保持；三新F三adapted、四限定S零I八open。静态汇总/-Check必须实际终态0，其余派生和完整门禁保持；未构建/测试/运行/产物/摘要。

<a id="money-drop-source-state"></a>
## 当前状态

补齐丢钱回调/按钮和woz发送三完整原根153条494B/四页，14E8九目标/15跳三RET。仅动作标志改既有BYTE读取一处；按钮/发送正确正文保持，三次fresh窗口匹配、金额同步和三字段容量/token/输出原序保。六合同注释/三新F三adapted、四限定S零I八open；完整回调/共享数据owner、ABI与互通运行待。

语义2327=2279F关联+48有界代码，处置1785/证据1587；文档关联2389/3680=64.92%仅关联率。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

