# 物品提示GBK解码异常与修复（2026-10-14）

用户提供cg2d_crash_20261014_230019_185_75836_85872_1.dmp并要求继续修复。本次使用已有DbgHelp与标准库离线读取转储、匹配PDB及本地EXE元数据；未启动／附加客户端、构建、测试、部署或计算摘要。

## 根因与现场

线程85872发生未处理的MSVC C++异常0xE06D7363。异常类型描述符为std::system_error（继承_System_error／runtime_error／exception），错误码1113；异常文本为“文字解码失败: No mapping for the Unicode character exists in the target multi-byte code page.”。

主模块基址0x7FF70E620000。同目录PDB的GUID为9A7B423D-AD22-4B3D-9739-F3A959B7449A、Age=6，与转储CodeView记录一致。异常类型元数据不在dump内的部分按该匹配EXE的RVA离线读取，未加载执行客户端。

关键调用返回地址（按返回地址减1查PDB行号，行号属于转储构建）：

| 返回地址RVA | 函数与源码 |
| --- | --- |
| 0x3BBD00 | cg::text::Decode，src/text/Encoding.cpp:37 |
| 0x3BAD07 | cg::text::ToUtf16，src/text/Encoding.cpp:70 |
| 0x2AF0C8 | 窄DrawText_456780，src/render/TextPendingQueue.cpp:237 |
| 0x1011CB | Sub_48B810，src/ui/WindowRuntime.cpp:7976 |
| 0x101F44 | Sub_48B1A0，src/ui/WindowRuntime.cpp:12240 |

Decode参数保存槽0x6741F1DCE0低W32为65001（CP_UTF8），0x6741F1DCE8低W32为8（MB_ERR_INVALID_CHARS）。输入string_view位于0x6741F1DCF0，保存指针0x7FF70F2577D2和长度6；窄DrawText调用参数槽0x6741F1DE58也为同一正文地址。六字节为C4FBC3CAB2DD，按GBK为“柠檬草”，不是合法UTF-8。

Sub_48B1A0的名称／备注来自原PlayerItemRecord GBK字段，Sub_48B810却将其直接交给契约为UTF-8的窄绘制重载。故障属于调用边界漏转，不是文字不可用，也不是ToUtf16应该自动猜编码。通用编码函数与异常策略保持。

## 修复范围

按[TUE01](cg2d_text_encoding_spec.md#tooltip-gbk-wide-boundary-20261014)将48B810可选标题／页脚改为const wchar_t*，各生产者使用局部wstring拥有者，c_str只借用到本次调用结束；固定描述行仍在原GBK布局按stride推进，每条非空行直接FromGbkWide后进入宽色码绘制。共九处调用覆盖摆摊、玩家／宠物装备、交易、存放、普通商店、独立详情和晶石两个窗口。

普通商店名称与数量改为宽format组合；名称／备注／提示字段仍读取原记录，原flags／count／key重取和titleColor顺序保留。48B930唯一静态帮助调用改为四项wstring_view行表，保空行位置。4FBA30仅保留原宽正文实现，其九处GBK调用点直接转宽（八处旧UI、一处窗口技能），其它宽动态行保持。退休窄重载及一个无消费者的页脚UTF-8视图。

不改变DrawText窄重载的UTF-8合同、ToUtf16校验、协议／资源／存档记录、96B行步长及页码；不增加编码猜测／宽窄万能重载或吞异常。保留清队列、标题、逐行色码、页脚、背景与CryptInt读取顺序。原地址注释及历史函数身份保持。

当前仅源码修正及静态核对；实际重编和用户复现尚未执行，完整文字、物品提示与端到端运行验收仍待。

## 其它绘制／测宽入口审查

按用户追加要求盘点三种文字入队、TextMeasure／MeasureDefaultText及局部drawAt/drawText包装，并追踪字体分行接口。修正范围：

- 旧504BA0六处调用改为宽名称／备注，包含原来直接传GBK的修理、鉴定、存放和交易详情。商店29B名称加数量直接宽format；有效字段表示不触及原84B格式预算。
- 宠物48D540／49B670名称、4B4070／4B45A0技能说明、496D50战斗目标和4C75A0物品属性标题，在原GBK源边界转宽；静态提示仍直接使用已知编码。
- 旧4F0460／4E4A50目标名单、4DF730留言名称、4E1B10／4E7380宠物详情、52E650家族成员、4F6220图鉴和4ED020战斗结果名称同步修正。相应测宽也直接使用宽字符串，保各自取样时点。
- 4217F0的唯一当前调用来自438FE0读取的战斗M包名称；创建测宽与421750逐行绘制显式从GBK转宽，原字节分行／最长行选择／释放及坐标保持。
- 另外33处LegacyText593850直接文字入队，以及4206C0带该原前缀的测宽补GBK边界；原字节图号循环保持。

未误转的路径：InputContext、对话行、IME、光标及命中检测已使用宽字符串；ExtractGbkTextLine窄重载明确输出UTF-8；聊天分行先FromGbk，结果摘要／摆摊状态来自UTF-8字面量或宽常量视图；日志文件名和邮件题头当前生产为ASCII。RenderChara的displayName目前只有零初始化与整字段复制，未找到非空生产，不据此声称其未来数据编码或新增生产逻辑。本次是已确认来源的静态编码接线修复，不代表全部网络／文件模型完成UTF-16迁移，亦不代表运行回归通过。
