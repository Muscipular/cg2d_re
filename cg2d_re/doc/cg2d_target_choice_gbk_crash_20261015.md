# 目标选择列表GBK误入UTF-8解码：离线诊断与修复

日期：2026-10-15。用户提供cg2d_crash_20261014_235648_279_94228_94340_1.dmp并要求修复；只读取该转储和匹配符号，不启动／附加客户端。

## 转储证据

- 异常线程94340，MSVC C++异常码0xE06D7363；对象错误码1113（0x459），诊断为“文字解码失败”。
- 主模块基址0x7FF7C53D0000，大小0x15AA000，时间戳0x6ACFA33B。转储RSDS与PDB流1的GUID为9A7B423D-AD22-4B3D-9739-F3A959B7449A、Age7，精确匹配；未计算文件摘要。
- 已有DbgHelp解析返回地址候选，结合源码直接调用核对；未将栈中所有模块地址都当作有效帧。

| 返回地址RVA | 符号及源码行 |
| --- | --- |
| 0x3C1F30 | Decode，text/Encoding.cpp:37 |
| 0x3C0F37 | ToUtf16，text/Encoding.cpp:70 |
| 0xA70C1 | Sub_49F0A0，ui/WindowRuntime.cpp:39482 |
| 0xA6F91 | Sub_49F060，ui/WindowRuntime.cpp:39466 |
| 0x8206A | OpenWindow_471440，ui/WindowRuntime.cpp:45169 |
| 0x24FE2B | Sub_4758D0，YobiRuntime.cpp:1932 |

Decode的string_view位于0x54402FDE10，data=0x7FF7C5A18C30、size=4；实际字节B0 A1 B0 A1，随后NUL。代码页65001（CP_UTF8），标志8（MB_ERR_INVALID_CHARS），已确认GBK输入导致UTF-8解码失败。异常对象地址0x54402FDD50；错误码和诊断来自转储，不推测未保存的异常类型元数据。

PDB确认0x7FF7C5A18C30为gSkillTargetNamesCB0550起始地址，0x7FF7C5A18D10为gSkillTargetNameViewsD45F98；后者首项指向前者，确定本次输入来自技能目标名单。

## 生产与消费合同

- 4F0200从旧玩家名称、Yobi宠物名称及已解转义网络字段生成GBK技能名单，4871B0发布借用指针表。
- 4E4860从同类GBK来源生成物品目标名单，48AF00发布借用指针表。
- 两者是49F000的两个直接调用者。窗口45初始化49F060及翻页／滚动共用49F0A0。
- 49F680将原名称交给回调，487020复制至后续GBK目标状态。因此本步明确保留GBK借用合同，避免单独改回调编码而破坏后续读取。
- LegacyText593850读取gLegacyInput593850前缀并可能延续至相邻LegacyInputStorage，是可变GBK字节owner，不能当作恒定空串。窗口46名称本身已按GBK转换，本次只修其三个未用行填充值。

## 修复与静态核对

49F0A0名称直接FromGbkWide写入wstring；另修40处LegacyText593850消费（WindowRuntime.cpp 38处、LegacyUiRecord.cpp 2处），合计41处、27个函数／辅助。邮件时间栏原有条件式两种编码混用：slash后正文来自FromGbk生成的UTF-8，仍用ToUtf16；仅无slash时的原GBK前缀改用FromGbkWide。三处注释明确名单及回调合同。

逐文件文本比对确认只有上述转换和注释变动，源码行数及原地址引用全部保持，原函数表无需迁行。其余源码、两名单生产者、回调、通用编码器、条件／索引／颜色与原读取发布序不变。三个源码／头词法括号通过；词法解析逐个ToUtf16实参核对，不再接收LegacyText593850。同源扫描不等于全工程编码或C++编译验证。

范围见[TUE02](cg2d_text_encoding_spec.md#target-choice-gbk-boundary-20261015)。本次是尚存GBK记录进入宽控件的过渡适配；NIO01完整网络／文件数据模型迁移继续分模块推进，未宣称全部完成。

未构建、测试、运行、部署或计算摘要；实际重编及相同操作的运行复现仍待。
