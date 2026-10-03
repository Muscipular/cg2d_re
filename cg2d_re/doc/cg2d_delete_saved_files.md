# cg2d 物理槽保存文件删除证据

## 函数边界与调用关系

- 原函数：`sub_533860@[0x533860,0x5338E1)`，共 129 字节、41 条指令、
  1 个基本块。
- 唯一 caller/callsite 是 `sub_464840@0x464D25`。
- 二进制共有 6 个调用点、2 个唯一目标：
  - 唯一 `sprintf` 目标 `0x53B304`：callsite 为 `0x533885`、`0x5338AA`、
    `0x5338C9`；
  - `DeleteFileA` 的 IAT 项为 `0x54D114`，`0x53388A` 把函数地址载入 EDI，
    三个寄存器间接 callsite 为 `0x533898`、`0x5338B7`、`0x5338D6`。
- 因三次删除均为寄存器间接调用，IDA 的直接 callee 列表只列出 `sprintf`，不能据此
  漏算 `DeleteFileA`。
- 源码只在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] DeleteSavedFiles_533860(std::int32_t)`；本项不接 caller。

## 三条路径与调用顺序

参数直接作为物理槽下标，从 `gSavedState.paths[physicalSlot].text` 取得目录。函数
只复用同一个未初始化的 `char path[0x400]`，依次执行：

1. 无界 `sprintf(path,"%s\\%s",directory,"save.dat")`，紧接
   `DeleteFileA(path)`；
2. 无界 `sprintf(path,"%s\\%s",directory,"mail.dat")`，紧接
   `DeleteFileA(path)`；
3. 无界 `sprintf(path,"%s\\%s",directory,"guildmail.dat")`，紧接
   `DeleteFileA(path)`。

前两次 BOOL 结果完全忽略，失败不会短路或改变第三次调用。函数直接把第三次
`DeleteFileA` 的 BOOL 按 `std::int32_t` 原值返回，因此返回值只表示
`guildmail.dat` 的删除结果，不汇总前三份文件的总体成功状态。

## 状态与错误边界

函数不读取 selected、`logicalToPhysical`、active、records 或任何 count，也不修改
`gSavedState`。它不检查物理槽下标、路径长度、文件存在性或目录状态，不调用
`GetLastError`，不重试，也不为缺失文件提供专用分支。

唯一 caller 在 `0x464D18..0x464D24` 自行把 selected 经 map 转成物理槽并传入；
`0x464D25` 调用删除函数后完全不检查 EAX，`0x464D2A` 只调用 `sub_45CE00`，
随后播放音效并绘图，最终在 `0x464D57` 无条件把 scene state 写为 10。进入状态 10
后，`0x464D64` 才取得成功文本 `asc_55CE34`，于 `0x464D85/0x464D8C` 完成复制，
并在 `0x464D8E` 交给 `sub_45CE10` 显示。因此删除结果被忽略后会无条件进入状态
10，随后显示成功文本；本函数不能擅自把失败升级为流程门禁。

本项不修改头文件、CMake 或 caller。行为证据只取自当前
`ida-17524 / cg2d.exe`；STONAGE 不参与结论。


## 三条删除路径接真实格式核心（2026-10-03）

局部 SPEC 与裁决见cg2d_stdio_contract.md的存档复位/删除四格式节。
本轮完整只读41条/唯一入向，533885/5338AA/5338C9的三格式全部
改接真实53B304；553514的%s反斜杠%s及三个原文件名含NUL字节保持。
同一共享物理槽目录及未清1024B path仍按save.dat→mail.dat→
guildmail.dat逐次生成并立即DeleteFileA；IAT54D114由53388A加载，
三间接调用533898/5338B7/5338D6已核。前两BOOL忽略，失败不短路，
第三完整BOOL32透传，不改为总体成功或新增业务门禁。
当前LoginCapability4Scene的464840流程已通过Sub_533860转发本体，
原“本项不接caller”为历史步骤记录；本轮保留caller对返回的消费。
不转码、限长、补零、清尾、校验下标或存在性；只替换格式，其他删除/
共享状态及全部保存正文逐字保持，不实际删除文件。
原字节/借用/次序/返回、整份源码限定差异与文档/JSON静态核对；
不构建、查看产物、新增/执行测试、运行格式/删除样例、读写或删除存档、
运行客户端或校验哈希，完整删除交互仍未运行验收。
