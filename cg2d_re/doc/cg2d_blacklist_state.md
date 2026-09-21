# cg2d 黑名单共享状态与 `sub_401000` 证据

## 权威范围

本文只依据当前只读 IDA 实例 `ida-17524` 中的 `D:\SOFTSTAR\cg2d.exe`；
IDB 原始输入 SHA-256 为
`dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08`。
STONAGE 不参与本项行为、地址、语义或验收结论。

本项只还原 `sub_401000` 的外层语义和四项共享状态 owner，不实现其持久化
callee，也不实现独立的加载函数 `sub_401030`。

## `sub_401000` 边界与行为

- 精确范围：`sub_401000@[0x401000,0x401029)`，共 41 字节、11 条指令、
  3 个基本块。
- 原型：`char __cdecl(int32_t fileId)`；新端以
  `std::int8_t PersistAndReset_401000(std::int32_t fileId)` 保持 W8 返回值和
  signed W32 参数。
- 唯一 callee：`sub_4011A0@0x4011A0`。
- 唯一 caller：`sub_45D990` 中的 `call` 指令 `0x45DAE1`；调用者传入
  `gValue55A054`，并忽略返回值。

控制流严格为：

1. 以原参数调用一次 `sub_4011A0`。
2. 返回 AL 为 0 时立即返回 0，不写任何共享状态。
3. 返回 AL 为任意非零值时，先把 `0x5531A0` 以 W32 写为 -1，再把
   `0x59384C` 以 W32 写为 0，最后返回 AL=1。

该函数不清空记录表或路径，不附加文件 ID、路径、游标或空指针门禁。登录连接
控制器保持 `sub_5081A0`、本入口、`sub_508140` 的原调用顺序。

## 四项唯一 owner

| 原地址 | 公开布局 | PE 映像初值 | 新端唯一 owner |
|---|---|---:|---|
| `0x592FA8` | `100 * 17` 字节，合计 `0x6A4` | 全零 | `cg::blacklist::gRecords` |
| `0x59364C` | `char[512]`，合计 `0x200` | 全零 | `cg::blacklist::gPath` |
| `0x59384C` | signed W32 游标 | 0 | `cg::blacklist::gCursor` |
| `0x5531A0` | signed W32 当前文件 ID | -1 | `cg::blacklist::gCurrentFileId` |

公开头使用 `std::array` 固定两项数组布局；实现对单条记录 17 字节、记录表
`0x6A4` 字节及路径 `0x200` 字节作静态断言。`gValue55A054` 只是
`sub_45D990` 传给本入口的另一项 W32 owner，不是 `gCurrentFileId`；R110 的
两个状态也都是不同存储，均不得并入上述四项。

## 未解析依赖与独立加载边界

`sub_4011A0` 的原端语义已经确认：它格式化
`data\blacklist_%d.txt`，仅当该结果与共享路径相等时才以 `w` 模式持久化共享
记录。新端本项只保留有地址依据的
`std::int8_t PersistBlacklistFile_4011A0(std::int32_t)` 外部声明，不提供空体、
固定返回值或替代实现。因此最终链接和真实磁盘持久化仍未闭合。

`sub_401030` 与本模块共享状态 owner，但属于独立的加载任务；本项不修改其真实
声明，不把加载逻辑合并到 `PersistAndReset_401000`。因此本项也不表示服务器
连接链已经完成。
