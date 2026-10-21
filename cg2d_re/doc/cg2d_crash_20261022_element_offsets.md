# 2026-10-22 商店滚轮崩溃与元素指针偏移修正

## 结论与范围

用户先要求定位转储，随后要求检查其他位置并修复同类问题。本次确认：窗口实例元素已迁入 `SortedElementMap` 的独立 `std::map` 节点，部分调用仍把查询结果当连续数组指针进行加减。商店滚轮因此取得伪元素，读取错误的 owner 后崩溃。

本次修正7份窗口源码中的33处元素定位，不修改容器、窗口生命周期、滚动算法或业务分支。沿现有原生槽号与固定ID合同，在槽号上计算目标，再调用 `FindElementById(NativeElementKey(...))`；目标不存在时沿现有空值分支跳过该元素操作。

## 转储证据

- 文件：`cg2d_crash_20261022_001141_724_36888_31800_1.dmp`，异常线程31800。
- 异常码 `0xC0000005`，读取地址 `0x71`；故障指令 `mov rax,qword ptr [rax+30h]`，现场 RAX 为 `0x41`。
- 使用同目录 `cg2d_re.exe` 与 `cg2d_re.pdb`。转储、PE CodeView与PDB信息流的标识均为 `66dca620-b6b9-4572-9b60-6db414387602`、age 84；转储与PE的映像时间戳均为 `0x6AD8E3D2`。未计算文件摘要。
- 模块基址 `0x7FF7C9F60000`，故障地址 `0x7FF7CA08B0EE`，模块偏移 `0x12B0EE`；匹配到MSVC `xhash:1585` 的 `_Find_last<unsigned int>`。
- 使用匹配PE的异常展开表、转储内存与本机DbgHelp展开应用调用栈，未将栈扫描候选地址冒充调用帧。

关键调用链：

```text
WindowRuntime_DispatchElementMouse
  → WindowShop_Scroll                  WindowShop.cpp:2546
  → WindowScroll_ApplyWheel            WindowRuntime.cpp:3749
  → WindowInstance::FindElementById
  → SortedElementMap::FindElementById   WindowRuntimeState.hpp:218
  → unordered_map::find → _Find_last
```

该转储的商店窗口ID为53，包含66个原生元素。滚轮为120，条目数20，当前位置0。现场有效元素指针为 `0x1F26689B7D8`，elementId为17，nativeSlot为16，owner为有效窗口 `0x1F260C99F30`。

崩溃时的代码执行 `indexedElement - 5`。PDB确认 `sizeof(WindowElement) == 152`，因此实际减去 `5 × 152 = 760` 字节，得到 `0x1F26689B4E0`。匹配EXE中调用前的实际指令也为 `sub rcx,2F8h`，与栈参数一致。

错误地址按元素解释后，owner字段为 `1`，relatedElementId为14。随后窗口的sortedMap偏移48、byId偏移16使哈希容器this成为 `1 + 48 + 16 = 0x41`，再读取其偏移48的字段得到故障地址 `0x71`。这条链直接证明本次故障由错误的元素地址运算造成；无需假设当前战斗修改、哈希容器内部损坏或随机堆覆盖。

## 同类排查与修正

| 文件 | 处数 | 涉及路径与目标 |
| --- | ---: | --- |
| WindowShop.cpp | 5 | 前后行、前后页、滚轮；目标原生槽11，即固定ID12 |
| WindowTradeTarget.cpp | 5 | 前后行、前后页、滚轮；目标原生槽1，即固定ID2 |
| WindowTextTarget.cpp | 5 | 前后行、前后页、滚轮；目标原生槽1，即固定ID2 |
| WindowPetSkillChoice.cpp | 15 | 多模式及两列的前后行、前后页、滚轮，保留每处分支原有槽偏移 |
| WindowCrystalShop.cpp | 1 | 滚轮；目标原生槽10，即固定ID11 |
| WindowCrystalExchangeShop.cpp | 1 | 滚轮；目标原生槽10，即固定ID11 |
| WindowDialog.cpp | 1 | 添加图片；直接查找count+8槽，即固定ID9至18的动画元素 |

32处滚动调用统一将 `FindElementById(NativeElementKey(index))` 后的指针减法，移到查询前的 `NativeElementKey(index - n)`；被保护和传入的指针都是目标元素本身。对话图片直接查询 `NativeElementKey(count + 8)`，去除对无关前置元素的查询与指针加法。

宠物技能下一行函数有一处既有注释明确记录原 `0x004C0314` 使用 `index - 1`，模式12／13也不切换到另一业务分支。本次只改为按该目标槽查询，保留该特殊偏移，不将15处全部强制改成某一滚动条。普通两列滚动目标为固定ID11／19；这处特殊路径仍按原偏移选择前一元素。

原生回调index来自 `element->nativeSlot`；实例创建记录该槽号。`NativeElementKey` 对非负槽映射为slot+1；本次七份原型全部核对了固定枚举ID与槽号的一致性，未根据节点地址、遍历顺序或seqNo推断目标。

## 验证与边界

- 在src范围检索显式WindowElement指针、查询结果及其加减、自增、自减、下标、跨行表达式，补查相关强转、大小计算及内存复制；人工区分原型数组、稳定引用数组、同名整数和坐标字段。未发现其他已确认的同类元素指针偏移残留。
- 对32处滚动修改做反向文本还原，对话图片单独还原；七份源码完整文本均与修改前一致，证明没有夹带其余逻辑修改。七份原型共370项的固定ID与原生槽一致；特殊宠物技能偏移保留。
- 源码保持UTF-8、无BOM及LF；进度文件保持既有CRLF。Git差异检查通过。
- 未构建、启动客户端、新增或执行测试，也未调用IDA。转储分析使用现有产物只读取证，不代表新源码已通过运行验收。
- 待运行回归：商店及两类目标列表的滚轮与翻页、宠物技能各模式与两列、水晶商店滚轮、打开对话窗口后添加图片；还需覆盖目标元素被删除时的空值路径。
