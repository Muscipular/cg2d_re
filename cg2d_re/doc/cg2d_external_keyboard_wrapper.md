# 外部屏幕键盘包装层与对话框回调

## 范围与证据

R6.1.3.96 还原 `sub_534AB0@[0x534AB0,0x534B7E)`，大小 `0xCE`，
共 79 条指令；R6.1.3.97 接续还原
`DialogFunc@[0x534B80,0x534EEA)`，主范围共 260 条指令、22 个基本块，
IDA 计入异地 EH 节点时为 267 条指令、25 个基本块。
证据取自当前只读 IDA 实例 `ida-17524` 的
`D:\SOFTSTAR\cg2d.exe`（x86），SHA-256 为
`e7c2790f479862635932e043b2d7e93b9e791a1edc276a960af05f1454c54010`。
未修改 IDA，未使用 STONAGE 作为行为证据。

唯一两个直接调用点为能力 3 的 `0x45FCCA` 和能力 4 的 `0x465077`。
新端在 `src/platform/LegacyExternalKeyboard.cpp` 实现既有内部入口
`cg::login::detail::Sub_534AB0(std::uintptr_t window,char* output)`，并加入
`cg2d_re_startup`；业务头不引入 Win32 类型。
包装层在 `0x534B02` 取回调地址，`DialogFunc_534B80` 也定义在该实现单元的
`cg::platform`，与包装层复用原有实例句柄和结果数组。

## 包装层调用顺序与返回域

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

## 回调消息与返回域

| 消息与分支地址 | 原行为及新端对应 | 回调返回 |
| --- | --- | ---: |
| `WM_DESTROY(2)`，`0x534C44..0x534C66` | 对象非空才执行虚删，`0x534C50` 传 flags `1`，`0x534C52` 调虚表首项；忽略返回，不清对象指针 | `0` |
| `WM_PAINT(15)`，`0x534C0E..0x534C41` | `0x534C18` 调 `BeginPaint`，`0x534C24` 保存 HDC，`0x534C29` 调 `EndPaint` | `0` |
| `WM_CLOSE(16)`，`0x534BC4..0x534C0B` | 把 `byte_593850` 空串复制到共享结果，`0x534BF3` 调 `EndDialog(window,200)` | `0` |
| `WM_GETDLGCODE(0x87)`，`0x534C69..0x534C7E` | `0x534C6A` 写返回值 `4` | `4` |
| `WM_INITDIALOG(0x110)`，`0x534D51..0x534EEA` | 严格按下节顺序构造对象、计算尺寸、随机定位并请求重绘 | `1` |
| `WM_USER+1(0x401)`，`0x534CAB..0x534D4E` | 完整原生 `LPARAM==1` 时读取对象真实结果并无界复制，`0x534CE6` 调 `EndDialog(window,100)`；否则只复制空串，`0x534D33` 调 `EndDialog(window,200)` | `1` |
| `WM_COMMAND(0x111)` 与其他，`0x534C8D/0x534C96` | 无对象或结果处理；`WM_COMMAND` 明确落入默认返回 | `0` |

回调不读取 `WPARAM`。关闭或取消仅覆盖结果首串的终止 NUL，不清整段
`0x108` 字节；确认路径保留无界复制和旧字节串，不增加截断或编码转换。
`WM_DESTROY` 保留悬空全局指针这一原行为，不加二次释放门禁。

## 初始化顺序与 W32 算术

1. `0x534D56` 原 `operator new(0x2C8)` 与 `0x534D81` 的 `sub_535090`
   合并到待实现的 factory 边界；传入共享实例句柄、当前对话框和空串。
   原构造异常由 `0x54BC30..0x54BC3A` 释放 raw 分配，异常继续传播。
   `0x534D9F` 将 factory 结果写入唯一对象指针；返回空值也继续下列步骤。
2. `0x534DA4` 调 `GetWindowRect(dialog,rectDD8)`，`0x534DAC` 调
   `GetClientRect(dialog,rectDB8)`。先以 W32 回绕计算
   `borderHeight=client.top-client.bottom-window.top+window.bottom`，再计算
   `borderWidth=client.left-client.right-window.left+window.right`。
3. `0x534E04`、`0x534E23` 分别执行一次尺寸虚调用，使用独立输出局部变量。
   第一次读取实际返回指针 `[0]` 加 `borderWidth`，写 `width@0x1085DAC`；
   第二次读取实际返回指针 `[1]` 加 `borderHeight`，写 `height@0x1085DD0`。
   不合并查询，也不把返回指针假定为传入的输出指针。
4. `0x534E37/0x534E43` 调并保存 `GetParent(dialog)`，随后依次在
   `0x534E48`、`0x534E50` 调 `GetWindowRect(parent,rectDE8)` 和
   `GetWindowRect(dialog,rectDF8)`。第二个对话框矩形仍保存，不因后续未读取而省略。
5. `0x534E54` 调既有 `ReadLegacyTime32_53BA26(nullptr)`，低 32 位在
   `0x534E5A` 传给既有 `SeedRandom_53B4BF`。在 `0x534E62`、`0x534E88`
   各调一次既有 `NextRandom_53B4C9`，不调用宿主 CRT 的 `time/rand/srand`。
6. 第一次随机数取 signed W32 余数，分母为
   `parent.right-parent.left-width`，再加 `parent.left` 写 X；第二次对应
   `parent.bottom-parent.top-height`，再加 `parent.top` 写 Y。各次加减先按
   `uint32_t` 位型回绕再 `bit_cast<int32_t>`，`%` 的两侧保持 signed W32。
   不为原 `0x534E7E/0x534EA6` 的零分母异常域增加门禁。
7. `0x534EC0` 调 `MoveWindow(dialog,X,Y,width,height,TRUE)`，随后
   `0x534ECB` 调 `InvalidateRect(dialog,nullptr,TRUE)`，返回 `1`。

全部 Win32 成败返回均按原行为忽略；有状态含义的 `BeginPaint/GetParent`
返回仅保存。对象空值、尺寸返回指针和字符串结果不追加安全返回，异常不吞。

## 共享状态与 x64 边界

- 原 `dword_1085F5C@0x1085F5C` 由本实现单元唯一的原生 `HINSTANCE`
  `gExternalKeyboardInstance1085F5C` 持有并零初始化。当前 IDA 仅有包装层
  `0x534AB0` 与回调 `0x534D72` 两处读取引用；不把窗口查询返回值写回该状态。
- 原 `byte_1085E08@0x1085E08` 使用唯一零初始化
  `gExternalKeyboardResult1085E08[0x108]`，容量保留至下一对象 `0x1085F10`。
  包装层在 `0x534B47` 读取结果，回调另有 `0x534BDD`、`0x534CD0`、
  `0x534D21` 三处引用；本次回调在同一实现单元复用该存储。

以下回调专属状态均在同一实现单元匿名 namespace 中唯一持有，并零初始化：

| 原地址 | 新端状态及类型 |
| --- | --- |
| `0x1085DA8` | X，`int32_t` |
| `0x1085DAC` | width，`int32_t` |
| `0x1085DB0` | Y，`int32_t` |
| `0x1085DB8` | 客户区矩形，原生 `RECT` |
| `0x1085DCC` | borderWidth，`int32_t` |
| `0x1085DD0` | height，`int32_t` |
| `0x1085DD4` | borderHeight，`int32_t` |
| `0x1085DD8` | 第一次对话框矩形，原生 `RECT` |
| `0x1085DE8` | 父窗口矩形，原生 `RECT` |
| `0x1085DF8` | 第二次对话框矩形，原生 `RECT` |
| `0x1085F10` | 绘制状态，原生 `PAINTSTRUCT` |
| `0x1085F50` | 绘制 DC，原生 `HDC` |
| `0x1085F54` | `ExternalKeyboard*` 不透明对象指针 |
| `0x1085F58` | 原生父窗口 `HWND` |

x64 将包装层窗口查询适配为 `GetWindowLongPtrA(parent,GWLP_HINSTANCE)`，
仍丢弃返回值。窗口、实例、模块、DC、对象指针、回调参数及 dialog 返回值均使用
原生宽度，不压缩为 W32；`PAINTSTRUCT` 不硬套原 x86 的 64 字节布局。
`DialogFunc_534B80` 使用真实 `INT_PTR CALLBACK(HWND,UINT,WPARAM,LPARAM)`。
旧对象的 `0x2C8` 分配大小、字段偏移和虚表字节偏移仅为证据，不用于访问原生对象；
Win32 类型仅位于平台实现，私有对象依赖头使用 `uintptr_t` 传递句柄。

## 四个真实对象依赖

`src/platform/LegacyExternalKeyboardDependencies.hpp` 只声明最窄对象边界，
不提供空实现、固定值或 fallback：

| 私有接口 | 地址依据与待实现契约 |
| --- | --- |
| `CreateExternalKeyboard_535090(instance,window,initialText)` | 组合 `0x534D56` raw 分配、`0x535090` 构造及 `0x54BC30` 构造异常释放；按真实原生布局分配，并保留空分配与异常传播语义 |
| `DeleteExternalKeyboardVirtual_535290(object,flags)` | `0x535180` 安装的虚表 `0x54F85C` 首项为 `0x535290`；`0x535293` 析构，`0x535298` 检查 flags bit 0，`0x5352A0` 释放，返回原对象指针；边界必须执行真实虚调用 |
| `QueryExternalKeyboardSizeVirtual_537DC0(object,output)` | 同虚表 `+0x20` 即 `0x54F87C` 项为 `0x537DC0`；真实实现从对象 `+0x17C/+0x180` 取两项 W32 并返回输出指针；边界保留真实虚调用及实际返回指针 |
| `GetExternalKeyboardResult_5354C0(object)` | `0x5354C0` 返回对象真实结果串，原 x86 为 `this+0x190`；原生对象层需映射真实存储，不在回调内套用旧偏移 |

`ExternalKeyboard` 仅作前向声明；`ExternalKeyboardSize{width,height}` 仅表达
尺寸输出的两个 W32，不虚构 `JoyparkKeyboard` 完整布局或资源状态。

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
68 字节局部缓冲是否足够仍取决于真实对象结果契约，本项不追加截断或输入校验。
窗口查询、游标计数、释放模块及错误框的返回值均按原行为忽略。

## 未闭合项

包装层和 `DialogFunc@0x534B80` 已按地址证据补齐源码，`JoyparkKeyboard`
对象闭包及外部 `idguardlibr.dll` 的真实资源和行为仍未闭合。原来的一个未解析
回调符号由定义消除，但引入上述四个真实未解析对象符号；就本次替换边界而言，
未解析符号净增加三个，最终链接仍未闭合，不代表屏幕键盘可用。
本项仅进行源码与 IDA 的静态自查，未执行编译、运行、测试、lint、格式化或
编译产物检查；独立复核结论另记进度。
