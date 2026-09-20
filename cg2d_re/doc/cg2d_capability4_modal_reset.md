# 能力 4 删除确认模态初始化锁复位证据

## 函数范围与行为

本文只记录当前 `ida-17524` 中 `D:\SOFTSTAR\cg2d.exe` 的
`sub_464440@[0x464440,0x46444B)`。函数大小为 `0x0B`，原型为 `void()`，共
2 条指令、1 个基本块，没有直接 callee：

```text
0x464440  mov dword ptr [0x9A57E4], 0
0x46444A  retn
```

因此唯一行为是把 W32 `0x9A57E4` 清零。新端在
`src/LoginCapability4Modal.cpp` 的匿名命名空间保留唯一私有状态
`gCapability4ModalInitialized`，`login::detail::ResetCapability4Modal_464440()`
只执行同一项清零。未来还原 `sub_464450` 时必须在同一源文件复用该私有状态，
不能建立另一份初始化锁。

## 引用与配对访问

函数唯一 caller 是 `sub_464840@0x464840` 的 state 7 分支：
`0x464C8E call sub_464440`。本步只提供复位入口，不修改或接入该 caller。

IDA 对 `0x9A57E4` 只确认三处直接引用：

| 地址 | 所在函数 | 行为 |
|---:|---|---|
| `0x464440` | `sub_464440` | 写 W32 0 |
| `0x46451D` | `sub_464450` | 读 W32 并判断是否已经初始化 |
| `0x464532` | `sub_464450` | 未初始化路径写 W32 1 |

`sub_464450` 的其余模态创建与更新逻辑不属于本项。本复位函数不销毁 action，
不清布局或结果，也不补写任何其他场景状态。

## 初值与状态归属

`0x9A57E4` 位于 PE `.data` 的 virtual 零填充区域；IDA 查询其文件 raw 偏移返回
`-1`，原始映像 W32 值为 0。因此私有状态以 `std::int32_t` 零初始化，既保留
W32 宽度，也保留启动初值。

该锁不能与 `LoginMessage.cpp` 的 `gMessageState@0x9A6E2C` 合并：后者是登录
消息模块私有的 W16 状态，地址、宽度、读写者和生命周期均不同。本项不修改
`LoginMessage`，也不为能力 4 模态提前建立 action、布局或结果存储。
