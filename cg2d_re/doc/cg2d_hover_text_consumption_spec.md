# 共享悬停文本消费与IME尾 SPEC（HWT01）

2026-10-10。沿窗口6真实回调完成输入/悬停文本消费合同，依子AGENTS第8条裁决。同一cg2d_re分支、不打tag。

<a id="hover-text-consumption-baseline"></a>
## 完整原根与身份

4731E0–473559（尾不含）完整230条889B/四页，四实际E8（两53BE40、459090、4590D0）、一GetKeyboardState IAT调用、十跳一RET。旧822此前只核34查找相邻指令，本步保存完整消费本体，保旧记录历史再追加。唯一数据xref指向原型54E330首部；真实main在+36/54E354，既有e0314700和窗口6源码绑定复用，不能把数组首部引用计为直接CALL。

4734F0属于同一4731E0根的共用尾。现BorrowWindowImeDescription_4734F0是窄平台helper，不赋新的原函数身份，也不重建回调调度。原窗口6为14元素，本文序号只沿实际记录，不新增容量门槛。

两IME叶459090–45909F五条15B、4590D0–4590D6两条6B均完整；合计237条910B/六页、五E8/四目标、两IAT调用、十跳三RET。459090先GetKeyboardLayout(0)，再经原ImmIsIME thunk返回完整Win32 BOOL；四实际CALL有据。4590D0仅返回73ED70描述地址，一实际CALL来自本回调。三个API进口仍unassessed，本步调用合同不当完整进口验收。

<a id="hover-text-consumption-decision"></a>
## 生产到消费及清理

gWindow6HoverD2EA70恰1或主串首BYTE为零时，元素1..7显示，8隐藏、10显示、11/12隐藏。否则先han，未命中才kok；命中且主串非空时在strlen-5处写NUL，再发布主值1/5。沿现真实查找和已批准MSVC CRT，不按匹配位置截串，不加长度/null或容量保护。

主串先借当前elements，再求含NUL长度，再借元素11的WindowTextData；复制到同一text后才读取主值低BYTE，写同一payload颜色。副串非空才显元素12，空时保原visible；随后同样先elements/长度/payload、含NUL复制、后读副值低BYTE。元素1..7逐项fresh取当前窗口隐藏，8/10隐藏、11显示。

共用尾保三个调用顺序：IME布局/ImmIsIME结果丢弃，256B未初始化键表由GetKeyboardState采样且返回/键表未使用，最后借唯一100B描述。不清零键表或添失败门槛。借描述后先当前elements，求含NUL长度，再借元素9 payload并复制；不复制私有描述存储。随后依次清主串首BYTE、主值W32、副串首BYTE、副值W32，返回S32 1。

源码已有上述分支、取样和owner关系，保正确正文。仅两颜色源由W32转BYTE改为const std::byte视图读取对象第零BYTE，S32/W32存储和上三BYTE不动。普通值相同，不称现有显示值错误。四定义/四真实声明共八注释，三个IME正文不改；无新API/helper、布局、类型、owner、私有状态、分配释放、保护、补零、默认值、钳制、截断、回滚或异常模型。

<a id="hover-text-consumption-acceptance"></a>
## 限定静态验收

核完整连续237条910B、各实际E8/IAT调用、十跳三RET、两叶五CALL点；确认4734F0根内尾与旧原型绑定。核三复制的elements/长度/payload取样、含NUL/颜色原低BYTE、各显示组、副标题空保visible和四槽清理/S32返回。四整源精确差异仅两BYTE源/八注释，三个IME正确body、三个真实结构、原型、五悬停槽储宽及唯一100B描述owner保持。

两新语义F/旧822历史追加，三adapted处置，INPUT/UIRUNTIME各三成员，两限定S零I，十任务仍open。原型回调、完整外父/间接路由、全部主副串及IME生产消费和容量/alias-owner寿命、SDK/机器ABI/生成BYTE访问/实际输入及悬停画面/运行仍待；进口枚举及48有界代码、完整gates/六closure/总SPEC§10保持。

仅必要文本静态和measure_cg2d.ps1/-Check。不构建、测试、运行客户端或算法、检查产物、计算摘要或引依赖；当前网络/文本、MSVC CRT与WAV-only边界保持。

<a id="hover-text-consumption-source-state"></a>
## 当前状态

前提交“[#] 补齐矩形命中与描边比较接线”；拟提交“[#] 补齐悬停文本消费并对齐颜色读取”。源码对齐、登记、静态汇总/-Check终态退出0及派生范围核对通过。语义2271=2223F关联+48有界代码、处置1714/证据1515，INPUT91/UIRUNTIME128；文档关联2333/3680=63.40%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持，整体active/未完成。
