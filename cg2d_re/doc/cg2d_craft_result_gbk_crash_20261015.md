# 制造结果GBK与UTF-8混拼崩溃：诊断与修复

日期：2026-10-15。用户提供cg2d_crash_20261015_012555_693_94168_94516_1.dmp并要求修复；仅离线读取转储及匹配符号，不启动或附加客户端。

## 转储证据

- 异常线程94516，MSVC C++异常码0xE06D7363；异常对象地址0xBCC58FBD10，错误码1113（0x459），诊断为“文字解码失败”。
- 主模块基址0x7FF77B600000；转储CodeView与本地PDB的GUID为9A7B423D-AD22-4B3D-9739-F3A959B7449A、Age8，精确匹配，未计算摘要。
- DbgHelp解析栈内返回地址候选，结合源码直接调用关系核对：NetworkDispatch_512E40的JgO回复 → Sub_50C7F0的kind=1分支 → Sub_4F39F0 → ToUtf16 → Decode。栈内其它候选不一概作为有效帧。

| 返回地址RVA | 符号及转储对应源码行 |
| --- | --- |
| 0x3C22C0 | Decode，text/Encoding.cpp:37 |
| 0x3C12C7 | ToUtf16，text/Encoding.cpp:70 |
| 0x9E1E8 | Sub_4F39F0，ui/WindowRuntime.cpp:33863 |
| 0x83E10 | Sub_50C7F0，ui/WindowRuntime.cpp:45289 |
| 0x1C1237 | NetworkDispatch_512E40，network/NetworkRuntime.cpp:4471 |

Decode实参保存槽中代码页65001、标志8；string_view位于0xBCC58FBDD0，data=0xBCC58FC170、size=24。其正文为C6 BB B9 FB B1 A1 BA C9 31 30 20 E5 A4 B1 E5 8E BB E4 BA 86 EF BC 81 0A：前10字节是GBK“苹果薄荷10”，后14字节是UTF-8“ 失去了！”及LF。原字段暂存0xBCC58FBE50包含同样10字节和NUL。该混合字节串不能整体按UTF-8或GBK解释。

## TUE03局部方案与裁决

沿用户已批准的动态UTF-16及网络解析边界迁移方案，只修本次制造结果文本支路。原函数0x004F39F0在提取并解转义字段9..14后，用FromGbkWide解码字段，再以std::format和宽字面量拼接结果；lostItem采用动态wstring，不再向256B固定数组拼接名称和提示。原0x00589F9C常量地址注释保留，文字及显示消费者不变。

依据[窗口11既有原合同](ui_windows/window_11.md)，补LF条件仍为完整原GBK行小于28字节。当前后缀一个空格加四个双字节字符共9字节，因此以原解转义字段的strlen加9判断；正文仍全程使用UTF-16，不为判宽重新编码。该条件修正迁移后误按UTF-8后缀长度计数的问题，保原28门槛，不能改为wstring码元数。

结果状态、字段1..8解析顺序、最多六个丢失项、八段门槛、空字段终止、声音及尾部窗口更新保持。共用JgO的另三分支和四处appendValue已核来源：共享数值格式化仅合并UTF-8常量与整数，不混入此GBK字段，本步不扩大重构。不改通用ToUtf16、不吞异常、不新增猜码或截断回退。

## 验证范围

源码只变更六行；反向应用六处替换后与改前逐字一致，原地址序列、总行数及函数定位保持。宽格式常量仅有一个消费者，std::format已有直接头文件，GBK字段只在进入宽正文时解码；调用、条件、循环与发布顺序静态核对通过。未重新编译、测试、运行或部署；完整Unicode／制造窗口验收不升级。

台账汇总仍因已有LegacyExternalKeyboard.cpp／LegacyExternalKeyboardObject.cpp两项历史引用缺失退出，未生成派生文件。局部修复的源码与JSON核对通过，不记录完整台账或运行验收通过。
