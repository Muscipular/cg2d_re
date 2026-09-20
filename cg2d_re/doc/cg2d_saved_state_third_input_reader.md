# cg2d 保存状态第三输入读取证据

## 函数边界

- 原函数：`sub_533680@[0x533680,0x5336C6)`，共 `0x46`（70）字节、
  26 条指令、8 个基本块，无 callee。
- 唯一 caller 是能力 4 输入初始化 `sub_465320@0x465362`。
- 源码在既有 `cg::login::detail` 中定义已声明的
  `Sub_533680(char*,std::int32_t)`。本项不实现 writer、reset、load、映射构建
  或其他 reader，也不抽取共享 helper。

## 空指针、映射与寻址

`0x533680` 先保存原 output，`0x533685` 检查空指针。output 为 `nullptr` 时
直接返回同一空指针，不读取 map。

非空时，`0x53368F` 首次读取
`logicalToPhysical[logicalSlot]@0x105C36C`。值严格等于 `-1` 时，
`0x533699` 只写 `output[0]='\\0'` 后返回；其他非法值不按 `-1` 处理。

非 `-1` 路径读取：

```text
records[logicalToPhysical[logicalSlot]]
    .parts[0].text[0x66 + index]
```

物理记录大小为 `0x510`，part0 的 text 位于记录偏移 `+0x4`，因此首字节原
地址表达式为 `records + physical*0x510 + 0x6A`；反汇编在
`0x5336A7..0x5336B0` 以 `physical*0x510 + 0x6A + index` 完成同一寻址。

正常调用前提是 `logicalSlot` 位于逻辑表范围，映射值为 `-1` 或有效物理槽
`0..9`。本函数不新增逻辑或物理索引保护；越出该正常域的行为未决，留给后续
上层门禁裁决。

## 重读次数与 NUL 规则

循环覆盖 `index=0..50`。循环头 `0x5336A0` 每轮都重新读取一次 map，源码没有
缓存循环外 physical：

- output 为空：map 读取 0 次；
- 首次检查命中 `-1`：map 读取 1 次；
- 非 `-1` 且第 `index` 个源字节为 NUL：总读取 `1 + index + 1` 次；
- 51 个源字节全部非零：初检 1 次、循环重读 51 次，共 52 次。

每轮把源字节写入 `output[index]` 后检查该字节是否为零。因此 NUL 本身会被
复制并立即返回；51 字节均非零时不追加 NUL，也不清理输出尾部。所有路径均
返回传入的原 output。
