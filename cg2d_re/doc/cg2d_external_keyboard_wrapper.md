# 外部屏幕键盘包装层

## 范围与证据

R6.1.3.96 只还原 `sub_534AB0@[0x534AB0,0x534B7E)`，大小 `0xCE`，
共 79 条指令。证据取自当前只读 IDA 实例 `ida-17524` 的
`D:\SOFTSTAR\cg2d.exe`（x86），SHA-256 为
`e7c2790f479862635932e043b2d7e93b9e791a1edc276a960af05f1454c54010`。
未修改 IDA，未使用 STONAGE 作为行为证据。

唯一两个直接调用点为能力 3 的 `0x45FCCA` 和能力 4 的 `0x465077`。
新端在 `src/platform/LegacyExternalKeyboard.cpp` 实现既有内部入口
`cg::login::detail::Sub_534AB0(std::uintptr_t window,char* output)`，并加入
`cg2d_re_startup`；业务头不引入 Win32 类型。

## 调用顺序与返回域

| 地址 | 原行为及新端对应 |
| --- | --- |
| `0x534AB0..0x534AC3` | 读取 `dword_1085F5C`；为零才调用 `GetWindowLongA(parent,-6)`，丢弃返回值 |
| `0x534ACE` | `LoadLibraryA("idguardlibr.dll")` |
| `0x534AE6..0x534AF2` | 加载失败时以传入 parent 弹出加载错误框，返回 `-1` |
| `0x534AFE` | `ShowCursor(TRUE)` |
| `0x534B0E` | `DialogBoxParamA(module,"KeyboardDlg",parent,DialogFunc,0)` |
| `0x534B18..0x534B1B` | 严格先 `ShowCursor(FALSE)`，再 `FreeLibrary(module)` |
| `0x534B24..0x534B3F` | dialog 结果 `<=0` 时以传入 parent 弹出启动错误框，返回 `-1` |
| `0x534B42..0x534B73` | dialog 结果 `==100` 时无界复制 `byte_1085E08`，包含终止 NUL，返回 `1` |
| `0x534B76..0x534B7B` | 其他正结果返回 `0`，包括取消值 `200` |

包装层返回域为 `{-1,0,1}`。两类错误框均使用 `MB_OK`；不改用全局窗口作为
parent。游标复用 `platform::ShowLegacyCursor`，不增减原有调用次数。

## 共享状态与 x64 边界

- 原 `dword_1085F5C@0x1085F5C` 由本实现单元唯一的原生 `HINSTANCE`
  `gExternalKeyboardInstance1085F5C` 持有并零初始化。当前 IDA 仅有包装层
  `0x534AB0` 与回调 `0x534D72` 两处读取引用；不把窗口查询返回值写回该状态。
- 原 `byte_1085E08@0x1085E08` 使用唯一零初始化
  `gExternalKeyboardResult1085E08[0x108]`，容量保留至下一对象 `0x1085F10`。
  包装层在 `0x534B47` 读取结果，回调另有 `0x534BDD`、`0x534CD0`、
  `0x534D21` 三处引用；后续真实回调在同一实现单元复用该存储。
- x64 仅把窗口查询适配为 `GetWindowLongPtrA(parent,GWLP_HINSTANCE)`，仍丢弃
  返回值。窗口、模块、回调参数及 dialog 返回值均使用原生宽度，不压缩为 W32。
- `DialogFunc_534B80` 只保留外部声明
  `INT_PTR CALLBACK(HWND,UINT,WPARAM,LPARAM)`；没有空回调、固定返回或替代 UI。

## CP950 与异常域

四段提示沿用原 CP950 字节常量，直接传给 `MessageBoxA`；UTF-8 源文件里的
中文仅用于注释。原始字节（含终止零）如下：

| 地址与文本 | 原始十六进制字节 |
| --- | --- |
| `0x58F6F8`：載入 idguardlibr.dll 失敗! | `B8 FC A4 4A 20 69 64 67 75 61 72 64 6C 69 62 72 2E 64 6C 6C 20 A5 A2 B1 D1 21 00` |
| `0x58F714`：無法載入函式庫 | `B5 4C AA 6B B8 FC A4 4A A8 E7 A6 A1 AE 77 00` |
| `0x58F6CC`：螢幕小鍵盤啟動失敗! | `BF C3 B9 F5 A4 70 C1 E4 BD 4C B1 D2 B0 CA A5 A2 B1 D1 21 00` |
| `0x58F6E0`：螢幕小鍵盤 | `BF C3 B9 F5 A4 70 C1 E4 BD 4C 00` |

结果缓冲作为旧字节串原样复制，不进行 UTF-8 转码。`strcpy` 不检查 output
空值、目标容量、重叠或源终止零；错误及其他正返回路径不清空 output。调用者的
68 字节局部缓冲是否足够仍取决于真实回调契约，本项不追加截断或输入校验。
窗口查询、游标计数、释放模块及错误框的返回值均按原行为忽略。

## 未闭合项

本项只完成包装层，`DialogFunc@0x534B80`、`JoyparkKeyboard` 与外部
`idguardlibr.dll` 的真实资源及行为仍未闭合。真实回调保留未解析外部依赖，因此
包装层实现不减少最终链接闭合的剩余边界，也不代表屏幕键盘可用。
本项未执行编译、运行、测试、lint、格式化或编译产物检查。
