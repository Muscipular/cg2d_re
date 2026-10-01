# 外部屏幕键盘包装层与对话框回调

## 范围与证据

R6.1.3.96 还原 `sub_534AB0@[0x534AB0,0x534B7E)`，大小 `0xCE`，
共 79 条指令；R6.1.3.97 接续还原
`DialogFunc@[0x534B80,0x534EEA)`，主范围共 260 条指令、22 个基本块，
IDA 计入异地 EH 节点时为 267 条指令、25 个基本块。
R6.1.3.98 接续实现 `sub_537DC0@[0x537DC0,0x537DD8)` 的 6 条指令和
`sub_5354C0@[0x5354C0,0x5354C7)` 的 2 条指令所对应的两个叶语义；构造、
析构及 UI 写者仍未实现。R6.1.3.99 接续实现
`sub_535290@[0x535290,0x5352AE)` 的 11 条指令、3 个基本块所对应的删除外层；
`sub_535310` 析构主体继续保留为真实未解析依赖。
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

## 真实对象依赖、删除外层与两个叶实现

`src/platform/LegacyExternalKeyboardDependencies.hpp` 只声明最窄对象边界，
不提供空实现、固定值或 fallback：

| 私有接口 | 地址依据与实现状态 |
| --- | --- |
| `CreateExternalKeyboard_535090(instance,window,initialText)` | 未实现。组合 `0x534D56` raw 分配、`0x535090` 构造及 `0x54BC30` 构造异常释放；按真实原生布局分配，并保留空分配与异常传播语义 |
| `DeleteExternalKeyboardVirtual_535290(object,flags)` | 外层已实现。`0x535180` 安装的虚表 `0x54F85C` 首项为 `0x535290`；adapter 真实虚调用唯一 native owner 的 `Delete`，先显式调用尚未定义的析构，flags 低字节 bit 0 决定释放，返回原对象指针 |
| `QueryExternalKeyboardSizeVirtual_537DC0(object,output)` | 叶语义已实现。同虚表 `+0x20` 即 `0x54F87C` 项为 `0x537DC0`；adapter 真实虚调用唯一 native owner 的 `QuerySize`，保留实际返回指针 |
| `GetExternalKeyboardResult_5354C0(object)` | 叶语义已实现。返回同一 native owner 的真实结果存储；原 x86 为 `this+0x190`，新端不套用旧偏移 |

依赖头仍只前向声明 `ExternalKeyboard`。私有
`src/platform/LegacyExternalKeyboardObject.hpp` 定义唯一原生 owner，仅持有
`ExternalKeyboardSize size` 的两个 W32 和 `char result[0x104]`；对应 `.cpp`
实现删除外层与两个叶语义并加入 `cg2d_re_startup`。未来 Create、析构及 UI 写者必须共用
该定义，不另建状态副本。原三参构造器和 `sub_535310` 对应的非虚析构器仅声明，
没有可用默认构造或空析构；字段均不设默认初值。原生虚方法不使用 RTTI，
也不把原虚表 `+0x20` 当作 x64 字节偏移。
该私有 native owner 只承载具体 `JoyparkKeyboard`，不提供派生扩展，因此声明为
`final`；保留虚方法调用语义，不为消除析构警告添加空虚析构。MSVC 的
[C4265 默认关闭](https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-3-c4265?view=msvc-170)，
仅现有 `/W4` 不启用该警告；`final` 也排除了把非虚析构类型作为可继承基类的
[警告条件](https://devblogs.microsoft.com/oldnewthing/20200618-00/?p=103874)。
此处为源码和官方文档静态核对，没有执行编译确认。

删除外层在 `0x535293` 无条件调用析构；`0x535298` 的原字节
`F6 44 24 08 01` 只测试参数低字节 bit 0，其他位不参与判断。
`0x5352A0` 仅在该位非零时调用 `delete@0x53B4E7` 释放存储；
`0x5352A8` 在两条正常路径均返回保存的原 this。新端对应为显式析构调用、
`flags & 1u`、单参数全局 `::operator delete(object)` 和返回原指针；
不使用再次调用析构的 delete 表达式，不套用旧对象大小或虚表字节偏移。
未来 Create 必须提供与该全局释放函数匹配的原生分配。

原外层没有捕获或异常释放路径，析构抛出时不会执行存储释放。
析构主体的 `FuncInfo@0x550DD8` 为 4 个 unwind state、零 try/catch；
`0x550DF8` 的清理表依次关联基 Window、第一组指针容器、第二组指针容器和树。
因此析构声明使用 `noexcept(false)`，Delete 方法及 adapter 不添加 `noexcept`，
保留异常向外传播。当前不定义析构主体，不提前添加或初始化其资源字段。
主体涉及 `0x535347` 的窗口销毁、`0x53537B/0x5353D1` 的两组子对象虚删、
`0x53542B` 的树清理、`0x535447..0x53545D` 的共享计数与哨兵释放、
两组 backing storage 释放及 `0x5354A8` 的基 Window 析构，均仍待还原。

尺寸叶函数的全部指令为：

```asm
537DC0 mov eax, [esp+4]
537DC4 mov edx, [ecx+17Ch]
537DCA mov [eax], edx
537DCC mov ecx, [ecx+180h]
537DD2 mov [eax+4], ecx
537DD5 retn 4
```

原叶函数严格按读宽、写宽、读高、写高的顺序执行，返回同一 output。新方法保留
该顺序，不提前复制整个尺寸；adapter 仍返回虚调用实际结果，不把所有虚实现的
返回值硬编码为 output。函数无直接调用引用，四个虚表引用为
`0x54F87C/0x54F8B0/0x54F8E4/0x54F918`；Dialog 两次调用仍独立保留。

基构造 `sub_537E60` 的两次清零在 `+0x174` 前结束，派生构造 `sub_535090`
也不直接初始化 `+0x17C/+0x180`。实际来源是 `WM_CREATE` 处理尾部
`0x5368E7` 的虚表 `+0x24` 调用：`sub_537DE0` 在 `0x537E06` 调 MoveWindow，
仅成功时在 `0x537E20/0x537E26` 写两项 W32。窗口注册、创建或移动失败留下的
未初始化尺寸仍属未闭合异常域，不能以默认零尺寸替代。

结果叶函数的全部指令为：

```asm
5354C0 lea eax, [ecx+190h]
5354C6 retn
```

唯一直接 caller 为 `DialogFunc:0x534CB8`。原构造 `0x5351A3..0x5351BF`
只把空串复制到结果起点，不清整个区；`+0x294` 在 `0x5350CF` 独立初始化，
并在析构 `0x535351` 及 UI 构造局部作为容器成员基址。因此新端保留
`[+0x190,+0x294)` 的 `0x104` 字节物理跨度；它不是已证逻辑输入上限，
也不据此声称恢复了原源语言数组声明。

`sub_536970` 普通字符路径在 `0x536B0F` 比较长度 `0x40`，
`sub_536C30` 的字母、数字和小键盘数字路径也执行相同门禁；但前者的 F9 空格
和 FD Tab 路径绕过该检查。已核对跳表实际指针
`0x536C08→0x536AD0`、`0x536C18→0x536A85`；分别从
`0x55C9BC` 的 `20 00`、`0x58F7D4` 的 `09 00` 无界追加到结果末尾。
因此不能宣称结果始终至多 64 字节，或缩成 65 字节存储；持续追加还能越过
`+0x294`。本项只实现结果地址获取，不改写尚未实现的消息写者或裁决溢出策略。

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
两个叶实现也不追加空对象或空输出门禁。Getter 原 x86 LEA 对空 this 只形成
地址 `0x190`，native C++ 的空对象成员访问属于非法域，不宣称保留该机器级
偶然行为。未初始化尺寸、失效对象、缺少终止 NUL 和越界追加均不因叶实现而闭合。

## 未闭合项

包装层和 `DialogFunc@0x534B80` 已按地址证据补齐源码，`JoyparkKeyboard`
对象闭包及外部 `idguardlibr.dll` 的真实资源和行为仍未闭合。R6.1.3.98 仅为
四个对象边界中的 Query/Get 提供了真实字段叶实现；R6.1.3.99 补齐 Delete 外层，
以真实未解析的 `ExternalKeyboard::~ExternalKeyboard` 取代未解析 Delete adapter，
未解析符号净数量不变。Create 仍未实现，原生构造和析构也仍只有声明。
尺寸初始化、结果 UI 写者、逻辑容量及异常域策略均未闭合，
最终链接仍未闭合，不代表屏幕键盘可用。
本项仅进行源码与 IDA 的静态自查，未执行编译、运行、测试、lint、格式化、
代码生成或编译产物检查；尚未据此声称独立验收通过。

<a id="excluded-keyboard-production-wiring"></a>
## 已批准排除的生产接线

用户已明确屏幕键盘不在实现范围，旧包装层、DialogFunc及对象叶
仍作为历史还原结果保留，不继续构造/析构/控件/DLL资源闭包。
本轮检查发现该排除尚未落实到生产路径：ReadSetupIni仍先写
gMonitorKeyboard=1，能力3/4输入仍调用534AB0，两份历史cpp仍
列入startup静态库，因而仍携带未定义对象构造/析构依赖。
此处按既有工程委托落实原批准范围，不扩大排除到普通键盘输入、
IME、登录校验、保存记录或其他辅助窗口。

### 局部实施方案

- ReadSetupIni将gMonitorKeyboard固定初始化为0；保留原
  MonitorKeyboard配置读取及CLOSE判定，不允许配置重新启用。
  StartupState原零初值、字段类型及其他配置处理均不变。
- 能力4的464E30在focus1走原关闭屏幕键盘分支，回到focus0；
  能力3的45FA70在focus0请求focus1。移除两处外部对话框调用及
  其专属返回/文本复制段，保留普通登录提交、校验、鼠标与焦点尾。
  原键盘成功路径的goto专用标签一并移除，不新增假返回函数。
- 从生产共用依赖头移除534AB0声明，CMake取消
  LegacyExternalKeyboard.cpp/LegacyExternalKeyboardObject.cpp的
  源文件条目；两个cpp及两个私有对象/依赖头逐字保留为历史源码，
  不再作为当前构建目标的可达对象。

不以默认0但保留真实未解析调用的方式声称接线已闭合，
也不以固定成功/失败的534AB0替身冒充旧键盘实现。
本步只声称移除已排除链的生产入口及构建依赖，完整客户端仍未链接，
不是屏幕键盘“实现完成”或运行验收。

### 共享窗口范围

537A30/537AE0的辅助窗口表和临界区仍不自动注册。
537B80直接caller为535090、538D40、53A220；
前者属于本次屏幕键盘链，后两者已见535540调用来源，
不能仅因使用相同窗口基类就把它们纳入屏幕键盘排除。
已批准newest五函数为50FC30/50FE30/50FF10/50FF70/510120，
不能仅按“登录检查”名称把535540等额外函数也排除。
后续继续核实际调用及网络加密范围，不把本轮移除键盘对象等同于
整个共享窗口/CRT范围已确认。总SPEC不例行对齐，日志不压缩。

### 实施与静态核对

上述生产排除已落位。将旧两控制器仅按gMonitorKeyboard==0
分支裁剪，去掉已无用途的专属标签/include后，除注释和空白外
与新控制器逐字一致；未更改普通提交、文本校验、IME绘制和焦点尾。
SetupIni仍按原序读取四项配置，键盘标志当前三处赋值均为0
（StartupState一处、SetupIni两处），没有重新启用的生产写者。

静态解析CMake源清单仅减少两份键盘cpp，其余101项源不新增、
不重排；活动cpp不再调用534AB0/键盘对象边界或引用键盘私有头。
两份历史cpp及两个私有头与本步前逐字一致，生产共用声明已移除，
未添加同名替身、固定成功值或演示对象。
此检查不执行CMake、不编译/链接、不读产物，也不运行配置读取、
登录交互、DLL加载或测试；仅声称已落实指定范围的生产接线。

### 另外两个窗口caller归属补证

继续只读追踪后，535540并非网络或newest检查函数，而是键盘创建消息
处理函数。535180把535090所构造对象的虚表写为54F85C；
该表+2Ch的实际项54F888为536FD0。537BB5在创建窗口前通过+2Ch
调用注册函数，536FD0在537028/537030把消息1与535540组成记录，
537038交给537090注册。535540的两条53A220调用创建lower/upper容器，
十六处538D40调用生产按键对象；全部直接caller均在该消息处理函数。
两组按键数据来自58F750/58F760，不以共享593850空文本判成网络调用。

因此537B80已发现的三个创建caller均归已排除的键盘对象链，
不是上一节所保留疑问中的额外普通窗口需求。此证据更新其归属，
不改变上一步已落实的生产排除，不扩大到非加密组包、网络分派或普通输入。
537A30/537AE0不为这些键盘对象另行加入新端CRT；未把直接xref分析
提升为所有辅助函数全局不可达，也不宣称全工程CRT已闭合。
