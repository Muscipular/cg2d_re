# cg2d 保存状态第二输入写入证据

## 函数边界

- 原函数：`sub_5335B0@[0x5335B0,0x5335EA)`，共 `0x3A`（58）字节、
  22 条指令、5 个基本块，无真实 callee，返回严格为 void。
- 两个 caller 分别是 `sub_464840@0x464B14` 与 `sub_465740@0x46575C`。
- 源码只在既有 `cg::login::detail` 中定义已声明的
  `Sub_5335B0(const char*,std::int32_t)`；不修改 caller，也不实现相邻函数、
  reset、load、映射构建或 persistence。

## 空指针与逐轮读取

`0x5335B5` 检查 source。source 为 `nullptr` 时直接返回，不读取 logicalSlot、
map，也不写任何保存状态。

非空路径循环覆盖 `index=0..50`，每轮顺序严格为：

1. `0x5335C0` 重新读取 `logicalToPhysical[logicalSlot]@0x105C36C`；
2. `0x5335C7` 第一次独立读取 `source[index]`；
3. `0x5335D3` 把该次读取结果写入目标字节；
4. `0x5335DA` 以第二个独立表达式再次读取 `source[index]`；
5. `0x5335DD` 检查第二次读取结果是否为 NUL。

源码没有把 physical 缓存在循环外，也没有缓存源值用于 NUL 判断。若 NUL 位于
索引 `k`，则执行 `k+1` 次 map 读取和目标写入、`2*(k+1)` 次源字节读取。

## 精确寻址、区间与 51 字节规则

每轮目标为：

```text
records[logicalToPhysical[logicalSlot]].parts[0].text[0x33 + index]
```

物理记录大小是 `0x510`，part0 的 text 位于记录偏移 `+0x04`，所以目标地址为
`records + physical*0x510 + 0x37 + index`。51 字节写入区间是该物理记录内的
`[+0x37,+0x6A)`。函数不检查 map 是否为 `-1`，也不把异常映射值归一化。

NUL 字节先写入目标，随后函数返回。前 51 字节全部非零时恰好写满该区间，
共读取 map 51 次、源字节 102 次；不读取第 52 字节，不追加 NUL，也不清理
目标尾部。

正常调用前提是 source 在遇到 NUL 前可读且最多需读取 51 字节、logicalSlot 位于
逻辑表范围、map 指向有效物理槽 `0..9`。本函数不增加 logical/physical 边界保护；
越出该正常域的行为未决，留给后续上层门禁裁决。
