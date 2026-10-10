# 窗口拖动、焦点命中与原读取宽度 SPEC（WDF01）

2026-10-10。沿已确认鼠标分发/矩形命中补完整窗口父，依子AGENTS第8条记录原事实并实施对齐。同一cg2d_re分支，不打tag；当前文本边界、MSVC CRT与WAV-only保持。

<a id="window-drag-focus-baseline"></a>
## 完整原根与返回

472D20–472EF2（尾不含）完整152条466B/三页，九实际E8/三目标（401E60两次、401EA0四次、472F00三次）、十八跳两RET；唯一实际CALL在473FEB，属于旧1233/473F80更新父。原401EA0由现Read_401EC0承接（旧219），名字不等于原CALL地址；旧1234/MIC01矩形命中完整原根复用，不重复采集/登记或修改其正确body。

本函数返回S32两位处理掩码：拖动处理置bit1（值2），可焦点窗口命中置bit0（值1），保真实0..3结果。现有公开内部声明和父消费者不改，不把它改为bool或丢返回。原窗口state和持久化WindowState.flags不是同一字段。

## 原拖动入口和坐标

先读取phase，再当前窗口指针；phase==1跳过拖动状态读取。其它phase只读一次实例W32 state，同一值先判bit1禁止拖动，再判bit11已拖动。现body两次读state，改为phase门槛内一个样本作两判定，不增加默认值或phase==1时的读取。

已拖动时按heldButtons低BYTE bit0决定继续：x目标地址在401E60前持有，x数值在调用后读取、加差值、写原目标；重新取得当前窗口，y目标也按原持地址/调用后数值读写。随后mouseX读后新窗口写dragMouseX，mouseY读后新窗口写dragMouseY。现坐标增量和新指针正文正确，保持，不误改为调用前冻结坐标值。

按钮未按住时，用原保存state只清800位；两分支都处理为2并重新取当前窗口。非拖动状态只有edgeButtons低BYTE bit0命中才扫描拖动矩形。

<a id="window-drag-focus-decision"></a>
## 四组对齐

1. 将phase/state两层门槛按原展开，phase!=1时一次W32 state供bit1/bit11，phase==1不提前取state。
2. heldButtons、两处edgeButtons消费和后段实例state bit0均显式按对象低BYTE读取。既有按钮/state存储仍W32，采用const unsigned char视图，只读第零BYTE；不改变生产者、布局、上三字节或字段类型。当前static_cast<uint8_t>(W32)和整state掩码改为真实BYTE源访问。
3. 矩形循环先取S32 secondaryCount；每轮按rect.x→window.x→rect.y→window.y→rect.height→focus==0判定→rect.width顺序取样，焦点判定跨宽度读取保持。focus允许时才矩形命中；命中后原mouseX/Y及drag800/result2写保持。允许焦点路径结束重新取当前窗口，否则保原窗口；每轮末先新取count，再index递增/比较，不新增十项容量限制。
4. 后段用state低BYTE bit0决定可焦点；phase1/4与其它phase/focus零两路分别按height→width→y→x四S32样本调用既有矩形函数。命中置结果1，普通phase下再读edgeButtons低BYTE bit0，才新窗口state置2000。两路和原门槛/调用/返回保，不合并取样或新增helper。

四组源码和两条定义/真实声明注释。实例/矩形/LegacyMouse真实类型保持，不新增API、字段、类型、owner、分配释放或协议，不补null、尺寸、边界、容量保护，默认值、补零、钳制、回滚或异常/溢出模型。原secondaryCount及业务门槛保。

原拖动矩形是实例内十项存储，但原count没有本步新增上限；不得因为std::array<10>就更改原循环数量。Native只沿已批地址字段拓宽，原S32/W32/W8合同保持。

<a id="window-drag-focus-acceptance"></a>
## 限定静态验收

核连续原字节、两RET/十八跳/九真实E8及唯一入向；核原phase跳过state读取、同一W32状态样本、四BYTE消费、矩形字段/focus/count顺序和两全窗参数逆序取样、原坐标读写及结果1/2。两整源按四组/两注释精确重放，两真实结构和原按钮/state储宽及其它源保持。

新增一语义F/一adapted处置、INPUT/UIRUNTIME各一成员、两限定S零I；九任务open。旧命中/更新父及既有CryptInt接线保持，本步不代所有密码整数叶/输入生产独立验收。全部其它台账、48有界代码/完整枚举与门禁/六closure/总SPEC§10保持。

只做必要文本静态及measure_cg2d.ps1/-Check，不编译、测试、运行客户端或算法，不查看产物、计算摘要或安装依赖。全部按钮/phase/焦点/窗口state/矩形count与坐标生产消费、Crypto/所有父/间接路由和alias-owner寿命、原BYTE源表达式的实际生成读取/SDK/机器ABI、真实拖动焦点和画面/全生命周期待，整体未完成。

<a id="window-drag-focus-source-state"></a>
## 当前状态

本步前提交“[#] 补齐滚轮滚动并恢复坐标持样”；拟提交“[#] 补齐窗口拖动焦点并恢复取样宽度”。源码对齐、登记、静态汇总/-Check终态退出0和派生范围核对通过。语义2267=2219F关联+48有界代码记录，F处置1709/证据1510，INPUT86/UIRUNTIME124；文档关联2329/3680=63.29%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持；未构建、测试或运行，整体active/未完成。
