# cg2d 保存输入上下文恢复证据

## 函数边界与调用关系

- 原函数：`sub_520690@[0x520690,0x5206EE)`，共 94 字节、36 条指令、
  3 个基本块，无 callee。
- 两个 caller/callsite 分别是 `sub_464840@0x464BE6` 与
  `sub_5201B0@0x5201D2`；两处均传入当前记录并忽略返回值。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] RestoreSavedInputContexts_520690(...)`；本项不接 caller。

## 循环与逐项顺序

函数固定执行 24 轮。第 `i` 轮的源文本从
`source+0xB2+i*0x19` 开始，目标为
`gInputContexts5F9758[i].text`，目标项步长由完整 `InputContext` 布局固定为
`0x15C`。每轮严格保持以下三个阶段：

1. 从该轮源地址开始执行第一次无界 NUL 扫描，得到包含终止 NUL 的复制字节数；
2. 从源地址重新读取并复制该文本及终止 NUL 到目标 `text`；
3. 从目标 `text` 开始再次执行无界 NUL 扫描，把所得长度先写
   `context+0x12C` 的 `totalByteLength`，再把同值写入 `context+0x128` 的
   `caretByteOffset`。

函数不读取独立长度字段；复制字节数只来自首轮扫描。它不清理目标终止 NUL 后的
旧尾部，24 轮结束后固定返回 0。

## 连续 owner 与别名边界

原目标完整覆盖 `[0x5F9758,0x5FB7F8)`，恰好是 24 个连续
`InputContext`：`24*0x15C=0x20A0`。该区间位于 PE virtual-only 区域，原运行时
初值来自 loader 零填充；新端只在 `Input.hpp/Input.cpp` 建立唯一、静态零初始化的
`input::gInputContexts5F9758[24]` owner，并以静态断言锁定总大小 `0x20A0`。

最后一个对象的 `field158` 位于 `0x5FB7F4`，其末端正好是区间上界
`0x5FB7F8`。后续 chat companion 逻辑若以其他名字引用 `0x5FB7F4`，必须别名到
这份数组 owner 的第 24 项 `field158`，不得建立第二份字段存储。

另有函数可能以 78 项索引触及该区域，但当前证据只支持本函数写入的连续 24 项；
因此不把 owner 推定或扩张为 78 项，也不为尚未还原的索引范围建立额外对象。

## 异常与实现边界

源名义槽宽为 `0x19`，但第一次扫描和随后复制都不受槽边界限制：缺少 NUL 时可
越过当前源槽继续读。目标 `text` 名义容量为 `0x120`，复制阶段没有容量门禁，长
文本可越过 `text` 写入同一 `InputContext` 的后续字段乃至相邻对象；随后目标扫描
同样无界。函数也不检查空指针、记录长度，不清零目标，不钳制长度，不提供回滚。

本项不修改当前记录 owner、CMake 或两个 caller。行为与布局证据只取自当前
`ida-17524 / cg2d.exe`；STONAGE 仅可作为架构参考，不参与本项行为、地址、语义
或验收结论。
