# 登录负分类宽字符串断言诊断（2026-10-14）

用户提供转储 cg2d_assert_20261014_222354_757_85408_13608_1.dmp。本次只离线定位；未启动或附加客户端，未构建、部署、修改业务源码或计算文件摘要。

## 结论

本次Unicode迁移遗漏了原固定文字区的字节别名观察。LegacyUiRecord.cpp:1907 的 ReadNegativeSelectionAvailability 在 page=0、slot=3 时执行 gHoverTextD68AE8[159]，但该对象已经是 std::wstring，现场长度为0。MSVC xstring:2176 的 _Off <= _Mysize 检查失败，报告 string subscript out of range。

相邻 case 4 的 gHoverTitleD68BF4[84] 属于同类遗留；本次断言实际发生在 case 3，不能把 case 4 写成已发生故障。159和84原为GBK固定缓冲区的字节偏移，不是UTF-16码元索引。单纯更换转换函数、扩大wstring容量或把越界返回固定0，均不能证明恢复原读取合同。

## 转储及符号证据

- 转储位于 build/windows-msvc-x64-debug；断言线程13608，主模块基址0x7FF6856F0000。
- 文件没有ExceptionStream；当前线程RIP位于等待转储写入的系统调用，不是断言触发指令。CRT回调栈内保留的CaptureStackBackTrace数组及其下方实际返回槽相互印证。
- 同目录PDB的CodeView GUID为9A7B423D-AD22-4B3D-9739-F3A959B7449A、Age=5，与转储记录完全一致。使用已有DbgHelp和Python标准库离线解析，没有下载工具或符号。
- 调用链：wstring::operator[] → ReadNegativeSelectionAvailability → ReadServerAvailability → ProcessScene_45FEA0 → world::Update → MainLoop → WinMain。

| 返回地址RVA | 对应源码（返回地址减1定位） |
| --- | --- |
| 0x6CA02 | MSVC xstring:2176，std::wstring::operator[] |
| 0x350278 | src/LegacyUiRecord.cpp:1907 |
| 0x1B2906 | src/LoginScene.cpp:405 |
| 0x1B1E52 | src/LoginScene.cpp:736 |
| 0x2E5908 | src/world/WorldUpdate.cpp:70 |
| 0x3B95D | src/platform/MainLoop.cpp:85 |
| 0x2A6A4 | src/platform/ClientEntry.cpp:347 |

operator[]返回槽0x737850D548的后两槽分别保存this=0x7FF685DA90A0、_Off=0x9F（159）。本地匹配EXE的函数入口指令先把RDX存入[rsp+10h]、RCX存入[rsp+8]，与这两个实参槽一致；其比较实际读取对象+0x18的_Mysize。DbgHelp确认this就是gHoverTextD68AE8，大小40字节；转储中+0x18为0、+0x20为7，正文为空。

ReadServerAvailability实际参数槽0x737850D5E0／D5E8／D5F0低S32依次为category=-1、page=0、slot=3；分类全局gValue55A054也为-1。不是用户输入159个字符导致溢出。

## 源码合同与修复边界

原两次负分类读取的证据已记录在[BRU01](cg2d_login_negative_selection_byte_owner_spec.md#login-negative-byte-baseline)：原D68B87=HoverText+159、D68C48=HoverTitle+84，分别依赖原256／260字节文字owner。UTL02将两个owner迁为动态UTF-16，但本辅助函数保留了旧索引。

当前两个登录尾部都调用ReadServerAvailability：第一处先读取、随后才检查category>=0；第二处直接按读取结果决定绘制。因此，仅调整第一处条件的短路顺序可以绕过本次栈，却不能处理另一处消费者。后续修复须明确退休负分类旧别名观察，或为仍需保留的原字节观察定义与UTF-16生产者一致的边界；不能假定动态宽字符串尾部仍承载原固定区内容。

本次定位已完成；业务修复与用户复现尚未执行，完整登录／Unicode运行验收保持未通过。

<a id="login-wstring-assert-source-fix"></a>
## 用户要求修复后的源码状态

按[LNW01决议](cg2d_login_negative_selection_byte_owner_spec.md#login-unselected-unicode-20261014)，未选择分类时两处绘制的共同查询均返回不可用；非负分类仍读取原服务器表。旧负分类适配器、声明和专用零字节退休，HoverText[159]／HoverTitle[84]不再作为可用性来源。当前源码修复覆盖两个消费者；本节不覆盖上述转储对应的旧源码行号和原故障证据。

局部具名变化为：未选择分类时不再根据旧邻接内存显示服务器按钮。未扩容或填充wstring、未引入GBK影子owner；实际鼠标追踪和原服务器协议记录保持。只做静态差异、调用与台账检查，未重新编译或运行，用户复现尚待。

静态核对确认：两个尾部调用仍存在，共享查询的非负分支逐字保持；整个ProcessScene_45FEA0及LegacyUiRecord其它函数正文不变。退休名称和两个hover下标模式在当前源码／头文件中无残留，函数表仅更新283处行号，3680个原身份／状态及完整门禁保持。
