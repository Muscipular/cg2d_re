# 登录场景控制器证据

## 范围与调用

本文只记录当前 `ida-17524/cg2d.exe` 中 `LoginScene::Process@0x45FEA0` 的证据。

- 精确边界：`[0x45FEA0,0x460C98)`，大小 `0xDF8`。
- 规模：1135 条指令、132 个基本块、53 个直接调用点。
- 原型：无参数，返回 32 位整数；最终返回
  `Renderer::DrawText3(540, 424, 0, 0, "7.1.00", 0)` 的原值。
- 唯一调用者：`World::Update@0x51C430` 的场景类型 2 分支。
- 23 个唯一直接目标：`sprintf`、`SetWindowTextA`、`SelectPalette@0x450B70`、
  `World::LoadAndPlayBGM`、`sub_45F100`、`World::PlaySe`、`PostMessageA`、
  `SetPendingSceneType@0x51C7B0`、`sub_52B990`、`sub_5202E0`、
  `sub_5203E0`、`sub_520440`、`sub_45D980`、`sub_45D990`、
  `SetCurrentSceneType@0x51C770`、`sub_5081A0`、`sub_45CE00`、
  `sub_45CE10`、`ShellExecuteA`、`sub_45EFD0`、`hasIDirect3D7`、
  `sub_45F090`、`Renderer::DrawText3`。其中 `SetWindowTextA/PostMessageA` 已由
  `LegacyWindow` 的唯一原生边界 `SetLegacyWindowText/PostLegacyWindowMessage`
  承接，`ShellExecuteA` 已由同模块的地址中性边界 `ExecuteLegacyShell` 承接；
  `sub_5202E0` 已由平台模块的唯一摘要边界
  `ComputeExecutableDigest_5202E0` 承接，`sub_5203E0/sub_520440` 已由
  `ReadInputValueFF494F_5203E0/ReadInputValueFF4982_520440` 的唯一公共实现承接，
  `SelectPalette` 与 `Renderer::DrawText3` 也已分别由唯一调色板选择和文本提交实现
  承接，登录场景不再保留这些重复占位声明。模块摘要的文件读取、MD5、输出契约
  与非法域风险见 [登录模块摘要证据](cg2d_login_module_digest.md)；新端只把无界
  `sprintf` 改为 256 字节标题缓冲上的 `snprintf`，除上述已闭合目标外，其余
  未实现目标只保留真实声明。

`SelectPalette` 的直接证据边界为 `[0x450B70,0x450BAD)`：16 条指令、5 个
基本块。登录场景仅有两处 caller：`0x45FF0A` 传 `(27,0)`，`0x45FF7D`
传 `(20,1)`；两处返回值均未消费。新端均直接调用公开
`cg::render::SelectPalette`，不在登录依赖层保留重复声明。

## 顺序状态处理

状态检查严格按 `0 → 1 → 2 → 3 → 100 → 101 → 102` 排列，不是互斥
`switch`。因此一次调用最长可从 0 同帧贯穿到 101；101 或 102 写回 1 后
不会反向重入本帧已经执行过的状态 1。

1. 状态 0：先写状态 1，再更新基础标题；依次清 `0xD9F2CA`、写
   `0x58B6D0=1`、清 `0x9A5B04`，调用 `SelectPalette(27,0)` 和
   `LoadAndPlayBGM(17)`，随后同帧进入状态 1。
2. 状态 1：把本帧尾部标志写为 1。输入检测严格依次为关闭、切换场景、
   返回分类、60 项分类扫描、10 项服务器扫描；切换场景时先调用
   `SetPendingSceneType(1)`，再调用 `SelectPalette(20,1)`。接受服务器后把状态
   增至 2，同帧继续。
3. 状态 2：调用 `ResetValues_45D980` 后把状态增至 3，同帧继续。
4. 状态 3：调用 `UpdateServerConnection`。返回 1 时立即
   `SetCurrentSceneType(3)`；返回 -1/-2/-3/-4 时写状态 100、生成消息并
   调用 `sub_5081A0`；其他返回值保持等待。
5. 状态 100：调用 `ResetLoginMessage` 后把状态增至 101，同帧继续。
6. 状态 101：`UpdateLoginMessage` 返回 0 时保持 101；任意非零时，若 URL
   标志非零则依次 `ShellExecuteA`、清标志、`SetPendingSceneType(1)`，随后
   写状态 1 并更新基础标题。
7. 状态 102：每次都调用 `UpdateLoginMessage`；任意非零时写状态 1，随后
   无条件更新基础标题。

`ShellExecuteA` 的 IAT 槽为 `0x54D224`，导入模块为 `SHELL32`；唯一代码
调用点 `0x460939` 位于 `LoginScene::Process`。`0x46092B..0x460938` 从右到左
压入六个参数，依次对应
`(hWnd, nullptr, 0x55CAA4, nullptr, nullptr, 1)`，其中 `0x55CAA4` 是密码
过期 URL。调用后的 `0x46093F` 直接为后续 `sub_51C7B0(1)` 压参，未读取
`EAX`，因此新端保留 `HINSTANCE` 的完整指针位型但调用方仍丢弃返回值；边界
直接调用 `ShellExecuteA`，不解释 `<= 32` 的返回值。

## 状态 1 的数据与 UI

固定 UI 记录均为 7 个 32 位整数、大小 `0x1C`；x64 代码只传原生指针，
不把指针截断为 32 位。

| 原地址或范围 | 宽度、初值与行为 |
|---|---|
| `0x55BD88` | 切换场景控件 `{540,360,64,16,245045,245047,245046}` |
| `0x55BDA8` | 关闭控件 `{540,392,64,16,245153,245155,245154}` |
| `0x55BDC8` / `0x55BDE8` | 上一页/下一页控件，位置分别为 `(130,310)`、`(254,310)` |
| `[0x55BE08,0x55C038)` | 20 个服务器控件，两页各 10 项；第二页偏移 `0x118` |
| `[0x55C038,0x55C2F4)` | 25 个真实分类控件 |
| `0x55C2F4` / `0x55C2F8` | 4 字节填充，随后是独立的返回/已选控件 |
| `[0x55C038,0x55C6C8)` | 原循环按 `<0x55C6C8` 做 60 次、步长 `0x1C` 的扫描视图；第 25 项后会穿入相邻控件、表和旧编码文本，因此不能声明为 `UiRecord[60]`。新端以单一 `uint32_t[420]` 原始映像和显式偏移视图保留。 |
| `[0xD9F2D4,0xD9F3C4)` | 唯一真实 60 项数组，每项 W32、映像零填充；`sub_507C80@[0x507C80,0x507D88)` 在 `0x507D6D` 读取并于 `0x507D7C` 写回计数 |
| `0xD69798` | 800 项、每项严格 `0xC1` 字节的零填充服务器表；索引 `category*20+page*10+slot`，本函数只读 `+0xC0` 的 W8 可用标志 |
| `0x55A054` | W32，映像初值 -1，跨登录函数共享的分类选择 |
| `0x58AD94` | 有符号 W16，映像初值 -1；接受服务器时写绝对服务器索引 |
| `0x9A5B04` / `0x9A6EA4` | 本函数私有 W32，分别为槽位与页号，映像初值均为 0 |

逐字节核对值：`[0x55C038,0x55C6C8)` 的 SHA-256 为
`d095d53c88a46d6b79c4dd21f3e4e9c595b37ca4c93dfdbc06d6ebf32be3e3a4`；
`[0x55BE08,0x55C038)` 的 SHA-256 为
`94b04568cc884dc273b956ea9c8b20be521b5748acbf3d2b6387b5d4066ae7b3`。

分类扫描每项都先调用 `sub_45F100(raw+0x1C*i)`，之后才判断输入 bit 0、
`count[i] > 0`、当前分类小于 0。选择分类后，令源 `A=record.+0x14`、
`B=record.+0x10`，第 `j` 个服务器按钮的 `+0x10` 写为 `B+2+2*j`，
`+0x14` 写为 `A+2+2*j`。服务器输入只检测第一页的 10 个同位置记录；
低字节 bit 6 先更新槽位，低字节 bit 7 再检测服务器表可用标志。接受后依次
更新标题、写 W16 服务器索引、写槽位、播放 `(53,320,240)`，再调用
`ComputeExecutableDigest_5202E0(0xDA3A1C)`、
`ReadInputValueFF494F_5203E0(0xDA38DC)`、
`ReadInputValueFF4982_520440(0xDA3A5C)`；后两者分别承接原
`sub_5203E0/sub_520440`。摘要调用返回值仍被丢弃，随后才递增场景状态；
因此 `PlaySe → 摘要 → 5203E0 → 520440 → 状态递增` 的原顺序不变。

分类标题原表从 `0x559958` 起，每项 64 字节，共 25 项；服务器标号原表从
`0x559F98` 起，每项 5 字节，共 20 项。新端标题文本使用 UTF-8，但索引、
大小写和特殊槽位 9 的 `"%s %s [逆 %s]"` 分支不变。

25 个分类标题的原字节正文依次为：

```text
A5 64 BD AC | AA F7 A4 FB AE 79 | C2 F9 A4 6C AE 79 | AD BB B4 E4
B7 E0 A4 6C AE 79 | B3 42 A4 6B AE 79 | A4 D1 AF AF AE 79 | BA 71 AE 56
C4 E5 A4 A7 BB 52 | BC AF BD 7E AE 79 | A4 F4 B2 7E AE 79 | C2 F9 B3 BD AE 79
AD BB B4 E4 | B4 FA B8 D5 | A4 45 C0 73 | A5 64 BD AC | AA F7 A4 FB AE 79
C5 53 A4 F1 | AD BB B4 E4 | B7 E0 A4 6C AE 79 | B3 42 A4 6B AE 79
C5 53 A4 F1 | A4 DF AC FC | C5 53 A4 F1 | A4 DF AC FC
```

对应 IDA 文本为：卡蓮、金牛座、雙子座、香港、獅子座、處女座、天秤座、
歌姬、櫻之舞、摩羯座、水瓶座、雙魚座、香港、測試、九龍、卡蓮、金牛座、
露比、香港、獅子座、處女座、露比、心美、露比、心美。服务器标号 20 个
5 字节槽依次为 `I..X` 与 `11..20`，大小写严格保留。

解析函数 `sub_507C80` 接受服务器编号 0..799，因此可令计数组 25..39 为正；
原 `0x4600F0` 会继续按 `0x559958 + group*64` 读取真实标题表之后的相邻映像。
新端不执行跨对象指针运算，而是显式保留各 64 字节槽首个 NUL 前的实际字节：

```text
25 "I"；26 ""；27 "Z"；28 "\x07V"；29..33 "d"；34..39 "x"
```

这些值只是原越界读取结果，不是正式分类标题。计数组 40..59 没有已知写者，
保持零，不能通过分类选择条件。

## 状态 3 返回与错误矩阵

`0xDA3B24` 是有符号 W16、映像零填充。返回 -2 时先按有符号值判断
`>1234`，再判断 `==1234`，其余值进入 1..23 跳转表；17 以及范围外值走
默认“無法連上伺服器。 ”路径。普通路径在复制后清 W16；表中标为“先清”
的路径保留先清再复制的顺序。

原 `LoginScene::Process` 对 `UpdateServerConnection` 返回 -3/-4 的两个分支继续保留：分别生成“無法連上更新伺服器”和“版本錯誤，請重新啟動魔力寶貝。”，仍属于原端调用契约与历史行为证据。2026-09-21 用户批准排除 `newest` HTTP/raw-socket 五函数检查链后，新端主阶段 1 直接旁路到主阶段 2，当前不再有该检查链产出 -3/-4；这项旁路差异不删除、合并或改写 `LoginScene.cpp` 的 -3/-4 分支，也不把五函数链记为已实现。

| 值 | 消息或动作 |
|---:|---|
| 1 | `版本錯誤` |
| 2、5 | `Multiline`；写第 6..11 行的账号说明，清值发生在写第 10、11 行之间 |
| 3 | 先写 `Multiline` 再覆盖为 `Multiline_New`；写第 0..11 行，清值发生在写第 9、10 行之间 |
| 4 | `點數不足` |
| 6 | `此帳號已在線上` |
| 7 | 先清；`此帳號已在線上` |
| 8 | `登入伺服器發生錯誤` |
| 9 | 先清；`儲值時間已到` |
| 10 | `計費方式有問題` |
| 11、13 | 先清；`目前停權中` |
| 12、14 | `目前停權中` |
| 15 | 先清；`未知的錯誤` |
| 16 | `伺服器負荷過重，請稍候再試。` |
| 17、默认 | `無法連上伺服器。 `，保留句尾空格 |
| 18 | 先清；`請休息一下滑鼠，數秒後再嘗試。` |
| 19 | `此伺服器不提供試玩服務` |
| 20 | 先清；`此電腦沒有權限登入。（行動鎖上鎖）` |
| 21 | `Multiline`；第 6..9 行为机码说明，第 10、11 行都是一个空格，最后才清值 |
| 22 | 先清；`此帳號已使用行動鎖凍結。` |
| 23 | 写 URL 标志、清值，再复制含 5 个 LF 的密码过期说明 |
| 1234 | 先清；`此伺服器很擁擠。` |

返回 -1、-3、-4 分别复制 `NETWORK還沒準備好。`、
`無法連上更新伺服器`、`版本錯誤，請重新啟動魔力寶貝。`；这些路径不清
`0xDA3B24`。消息主缓冲 `[0x9A60F0,0x9A61F4)` 为本函数私有 260 字节；
多行内容复用公开唯一 `[0x9A620C,0x9A6E0C)` 的 `12*256` 缓冲。“新端只复制
UTF-8 正文与 NUL”仅记录 R6.1.2.9 实施前的历史表示，不作为本次四行的编码约定；
机码停用四行的源码对齐按文末合同保留原正文与 NUL，仍不清缓冲余部，不扩展到其他分支。

## 原字符串字节

下表是上述状态分支使用的原端字节，均包含末尾 NUL；“新端不直接使用这些旧
编码字节”仅为本次实施前的历史描述，不再约束 R6.1.2.9 的四常量原字节对齐。
原字节表保持不变，其他文本的源码状态不由本项推定。

| 地址 | IDA 显示 | 原字节 |
|---|---|---|
| `0x559384` | `魔力寶貝` | `C5 5D A4 4F C4 5F A8 A9 00` |
| `0x55C5BC` | `7.1.00` | `37 2E 31 2E 30 30 00` |
| `0x55C7E8` | `%s %s` | `25 73 20 25 73 00` |
| `0x55C7D8` | `%s %s [%s %s]` | `25 73 20 25 73 20 5B 25 73 20 25 73 5D 00` |
| `0x55CD18` | `%s %s [逆 %s]` | `25 73 20 25 73 20 5B B0 66 20 25 73 5D 00` |
| `0x55C7C4` | `NETWORK還沒準備好。` | `4E 45 54 57 4F 52 4B C1 D9 A8 53 B7 C7 B3 C6 A6 6E A1 43 00` |
| `0x55C62C` | `無法連上伺服器。 ` | `B5 4C AA 6B B3 73 A4 57 A6 F8 AA 41 BE B9 A1 43 20 00` |
| `0x55CAD0` | `此伺服器很擁擠。` | `A6 B9 A6 F8 AA 41 BE B9 AB DC BE D6 C0 BD A1 43 00` |
| `0x55C7B8` | `版本錯誤` | `AA A9 A5 BB BF F9 BB 7E 00` |
| `0x55C5DC` | `Multiline` | `4D 75 6C 74 69 6C 69 6E 65 00` |
| `0x55C5C4` | `Multiline_New` | `4D 75 6C 74 69 6C 69 6E 65 5F 4E 65 77 00` |
| `0x55C6F4` | `點數不足` | `C2 49 BC C6 A4 A3 A8 AC 00` |
| `0x55C6E4` | `此帳號已在線上` | `A6 B9 B1 62 B8 B9 A4 77 A6 62 BD 75 A4 57 00` |
| `0x55C6D0` | `登入伺服器發生錯誤` | `B5 6E A4 4A A6 F8 AA 41 BE B9 B5 6F A5 CD BF F9 BB 7E 00` |
| `0x55C6C0` | `儲值時間已到` | `C0 78 AD C8 AE C9 B6 A1 A4 77 A8 EC 00` |
| `0x55C6B0` | `計費方式有問題` | `AD 70 B6 4F A4 E8 A6 A1 A6 B3 B0 DD C3 44 00` |
| `0x55C6A4` | `目前停權中` | `A5 D8 AB 65 B0 B1 C5 76 A4 A4 00` |
| `0x55C698` | `未知的錯誤` | `A5 BC AA BE AA BA BF F9 BB 7E 00` |
| `0x55C678` | `伺服器負荷過重，請稍候再試。` | `A6 F8 AA 41 BE B9 AD 74 B2 FC B9 4C AD AB A1 41 BD D0 B5 79 AD D4 A6 41 B8 D5 A1 43 00` |
| `0x55C658` | `請休息一下滑鼠，數秒後再嘗試。` | `BD D0 A5 F0 AE A7 A4 40 A4 55 B7 C6 B9 AB A1 41 BC C6 AC ED AB E1 A6 41 B9 C1 B8 D5 A1 43 00` |
| `0x55C640` | `此伺服器不提供試玩服務` | `A6 B9 A6 F8 AA 41 BE B9 A4 A3 B4 A3 A8 D1 B8 D5 AA B1 AA 41 B0 C8 00` |
| `0x55CC00` | `此電腦沒有權限登入。（行動鎖上鎖）` | `A6 B9 B9 71 B8 A3 A8 53 A6 B3 C5 76 AD AD B5 6E A4 4A A1 43 A1 5D A6 E6 B0 CA C2 EA A4 57 C2 EA A1 5E 00` |
| `0x55CB70` | `此帳號已使用行動鎖凍結。` | `A6 B9 B1 62 B8 B9 A4 77 A8 CF A5 CE A6 E6 B0 CA C2 EA AD E1 B5 B2 A1 43 00` |
| `0x55C618` | `無法連上更新伺服器` | `B5 4C AA 6B B3 73 A4 57 A7 F3 B7 73 A6 F8 AA 41 BE B9 00` |
| `0x55C5F8` | `版本錯誤，請重新啟動魔力寶貝。` | `AA A9 A5 BB BF F9 BB 7E A1 41 BD D0 AD AB B7 73 B1 D2 B0 CA C5 5D A4 4F C4 5F A8 A9 A1 43 00` |
| `0x55CAA4` | 密码过期 URL | `68 74 74 70 3A 2F 2F 77 77 77 2E 73 6F 66 74 73 74 61 72 2E 63 6F 6D 2E 74 77 2F 47 61 6D 65 2F 49 6E 64 65 78 2E 61 73 70 78 00` |

多行原文本地址为 `0x55C798/778/754/738/71C/700`、
`0x55CCEC/CCC0/CC94/CC6C/CC48/CC24`、
`0x55CBE8/CBD4/CBB0/CB8C/C9BC`、`0x55CAE4`。其中：

- `0x55CC24` 的前导原字节严格为四个 `A1 40`，即四个全角空格；
- `0x55C9BC` 严格为 `20 00`；
- `0x55CAE4` 中五处分隔符和结尾分别为单字节 LF `0A`，末尾为 `0A 00`；
- 账号说明前缀 `0x55C798` 原字节为
  `B1 7A A5 69 AF E0 B9 4A A8 EC A5 48 A4 55 20 33 20 BA D8 B1 A1 A7 CE A4 A7 A4 40 A1 47 00`。

其余多行行文本原字节如下：

```text
55C778: 31 2E B1 4B BD 58 BF F9 BB 7E A1 41 BD D0 AC 64 A9 FA AB E1 AD AB B7 73 BF E9 A4 4A 00
55C754: 32 2E A5 BC C0 78 AD C8 A1 41 BD D0 A6 DC 20 53 4F 46 54 53 54 41 52 20 BA F4 AF B8 C0 78 AD C8 00
55C738: 33 2E B7 73 B7 7C AD FB A8 74 B2 CE A4 A4 AC 64 B5 4C A6 B9 B1 62 B8 B9 00
55C71C: 2D 2D 2D 3E BD D0 B3 74 A6 5E 20 53 4F 46 54 53 54 41 52 20 BA F4 AF B8 00
55C700: 2D 2D 2D 3E B5 F9 A5 55 B7 73 B1 62 B8 B9 A8 C3 B2 BE C2 E0 C2 C2 B1 62 B8 B9 00
55CCEC: A1 D7 A1 D7 A1 D7 A1 D7 20 AC B0 A4 46 BA FB C5 40 AA B1 AE 61 B1 62 B8 B9 A6 77 A5 FE A9 CA 20 A1 D7 A1 D7 A1 D7 A1 D7 00
55CCC0: A4 6A A6 74 B8 EA B0 54 A4 77 A9 F3 A2 B1 A2 AF A2 AF A2 B4 A1 FE A2 AF A2 B4 A1 FE A2 B0 A2 B2 A5 FE AD B1 C5 DC A7 F3 00
55CC94: A9 D2 A6 B3 AA B1 AE 61 B7 7C AD FB B1 4B BD 58 A1 42 A5 48 A4 CE C5 5D A4 4F C4 5F A8 A9 B9 43 C0 B8 B1 4B BD 58 A1 41 00
55CC6C: BD D0 AA B1 AE 61 A6 DC 53 4F 46 54 53 54 41 52 B7 7C AD FB A4 A4 A4 DF BB E2 A8 FA B7 73 AA BA B1 4B BD 58 A1 41 00
55CC48: B8 D4 B1 A1 BD D0 B0 D1 BE 5C 53 4F 46 54 53 54 41 52 A9 78 A4 E8 BA F4 AF B8 B8 EA B0 54 A1 47 00
55CC24: A1 40 A1 40 A1 40 A1 40 68 74 74 70 3A 2F 2F 77 77 77 2E 73 6F 66 74 73 74 61 72 2E 63 6F 6D 2E 74 77 00
55CBE8: A5 BB A5 78 BE F7 BE B9 A4 A7 BE F7 BD 58 A4 77 B0 B1 A5 CE A1 43 00
55CBD4: BD D0 C0 CB AC 64 B9 43 C0 B8 C2 49 BC C6 A1 43 00
55CBB0: A5 69 AF E0 AC 4F C2 49 BC C6 A4 A3 A8 AC BE C9 AD 50 B5 6E BF FD A4 A7 BE F7 BD 58 B0 B1 A5 CE A1 43 00
55CB8C: C0 78 AD C8 AB E1 BD D0 B5 79 AB DD BC C6 A4 C0 C4 C1 A8 74 B2 CE B3 42 B2 7A AE C9 B6 A1 A1 43 00
55C9BC: 20 00
```

`0x55CAE4` 的完整原字节为：

```text
BF CB B7 52 AA BA AA B1 AE 61 B1 7A A6 6E A1 47 0A A5 D1 A9 F3 B1 7A
AA BA B9 43 C0 B8 B1 4B BD 58 A4 77 B8 67 B9 4C B4 C1 A1 41 0A BD D0
B5 6E A4 4A A4 6A A6 74 B7 7C AD FB A8 74 B2 CE A7 F3 B4 AB B9 43
C0 B8 B1 4B BD 58 A1 41 0A BD D0 A9 77 B4 C1 A7 F3 B4 AB B1 4B BD
58 A5 48 BD 54 AB 4F B1 62 B8 B9 A6 77 A5 FE A1 43 0A C2 49 C0 BB
BD 54 A9 77 A1 41 B1 4E AB 65 A9 B9 53 4F 46 54 53 54 41 52 BA F4
AD B6 A1 43 0A 00
```

## 公共绘制尾部

关闭与切换控件始终先绘制。分类循环的 60 次上界及 `0x1C` 步长不变；状态
1 本帧还会绘制分页控件并按“下一页后上一页”的顺序检测输入。两次点击在页号
已达边界时仍播放 `(51,320,240)`。服务器绘制使用当前页记录；状态 1 本帧样式
为 0，其他状态按当前槽位选择样式 2 或 1。

最后严格依次调用：

```text
hasIDirect3D7()
sub_45F090(49, 67, 3, 0x3BEA3, 0, -1)
sub_45F090(0, 0, 3, 0x3BEA2, 0, -1)
DrawText3(540, 424, 0, 0, "7.1.00", 0)
```

原机器码在服务器可用标志判断中先形成并读取地址，后判断分类选择是否非负。
选择值为 -1 时，两页 20 个潜在读取地址位于
`0xD68944,0xD68A05,...,0xD69797`，不属于 `0xD69798` 的 800 项表；其中部分
地址有其他运行期写者。因此新端不能仅凭映像零值把这次读取折叠成常量，也不能
在独立 `D69798` 数组前构造越界指针；新端以唯一窄读取适配器声明承接这些真实
宿主字节，适配器不提供实现或固定返回。20 个地址按 `page*10+slot` 依次为：

```text
D68944 D68A05 D68AC6 D68B87 D68C48 D68D09 D68DCA D68E8B D68F4C D6900D
D690CE D6918F D69250 D69311 D693D2 D69493 D69554 D69615 D696D6 D69797
```

状态 1 本帧尾部仍先调用该适配器取得可用值，再判断选择值非负；其他状态尾部
直接使用采样值决定是否绘制。该适配器仍未实现，负索引兼容路径不计行为闭合。

## R6.1.2.9 机码停用四行原字节合同

### 来源与源码范围

本节继承 Mendel 已 READY 并关闭的分析交接及本文[原字符串字节](#原字符串字节)，
不重新查询 IDA 或复审旧消费者算法。主线程按已批准
[RPLAN.16 文本编码边界 SPEC](cg2d_text_encoding_spec.md) 实施门禁，无新增设计批准。
2026-09-22 原批准轮仅更新方案、规则、规划与记录，未修改生产代码、构建或测试；
本次四常量源码对齐是后续实施，不倒改原批准轮事实。

源码对齐范围仅为 [LoginScene.cpp](../src/LoginScene.cpp) 的
`kMachineCode0..3`：用连续 `\xNN` 保存已证正文，字面量仅隐式追加一个 NUL，
不另写 `\x00`；四个 `sizeof` 断言分别为 `23/17/35/33`。源码、文档仍为 UTF-8，
运行期兼容数据不由中文源码重新编码；来源编码名称未定，不猜作 Big5/CP950。
本节记录源码对齐工作及静态预期，不提前给出独立 review 或文件验证通过结论。

| 原地址 | 源码常量 | 目标槽 / 原地址 | 原正文 / 含 NUL（字节） | 槽内保留余部（字节） | 修复前 UTF-8 正文（字节） |
|---|---|---|---:|---:|---:|
| `0x55CBE8` | `kMachineCode0` | 6 / `0x9A680C` | 22 / 23 | 233 | 33 |
| `0x55CBD4` | `kMachineCode1` | 7 / `0x9A690C` | 16 / 17 | 239 | 24 |
| `0x55CBB0` | `kMachineCode2` | 8 / `0x9A6A0C` | 34 / 35 | 221 | 51 |
| `0x55CB8C` | `kMachineCode3` | 9 / `0x9A6B0C` | 32 / 33 | 223 | 48 |

原正文及末尾 `00` 直接继承上文已有字节表，不另复制或重新取证。槽号与复制长度均以
字节缓冲为边界，不改作 Unicode 字符或码元索引。

### 生产顺序与缓冲

调用入口继承 [WorldUpdate.cpp](../src/world/WorldUpdate.cpp) 第 61 行的场景类型 2 分支，
调用 `ProcessScene_45FEA0`。登录子状态 3 调用
[UpdateServerConnection](../src/LoginState.cpp) 返回 `-2`，且有符号 W16
`gValueDA3B24@0xDA3B24 == 21` 时，`LoginScene.cpp` 的生产顺序严格为：

```text
登录子状态置 100 → 主缓冲写 Multiline → 依次写槽 6、7、8、9
→ 槽 10、11 各写 20 00 → 错误值清零 → ShutdownNetworkRuntime_5081A0
```

这是登录子状态 100，不是把场景类型改为 100。槽 `0..5` 不写。`CopyText` 向每个
256 字节槽分别复制 `23/17/35/33` 字节，包含 NUL；表中余部保留既有内容，
不清尾、不转码。槽 10、11 各只覆盖 2 字节，其余 254 字节同样保留。
260 字节主缓冲 `gLoginSceneMessage@0x9A60F0` 仅写入含 NUL 的 `Multiline`
10 字节，余部不清；它不承载这四行正文。

随后子状态 100 调用 [ResetLoginMessage](../src/LoginMessage.cpp) 只重置内部状态，
再进入 101 调用 `UpdateLoginMessage`。后者将主缓冲标记的 10 字节复制到本地
256 字节栈数组；四行正文仍来自共享槽，不进入该栈副本。

### 消费与所有权

[LoginMessage.cpp](../src/LoginMessage.cpp) 第 94 行的 `Multiline` 分支首次将内部状态
置 1、结果置 `-2`，测槽 8 建窗。对象非空且 `IsYobiActionReady(action, 1)` 时才绘制；
六行共用以槽 6 测宽求得的居中 X，逐行调用 `DrawText(X,Y,2,0,0,slot,0,0)`。
`FindClickedTextIndex_45D7A0(&gMessageResult, 1)` 的调用及结果处理不改。

四常量为静态存储；正文复制进由 `LoginMessage.cpp` 唯一定义的共享
`gValue9A620C[12][256]`，槽内容有效至下次覆写，不以主缓冲或栈副本替代共享状态。
测宽仅临时借用槽指针，复用[字体字节度量](cg2d_font_measure.md)的既有契约。
`login::detail::DrawText` 是到 `DrawText_456780` 的薄适配，复用
[文本待提交队列](cg2d_text_pending_queue.md)的正文加 NUL 复制契约：
复制进入独立记录的 `text[255]`，不长期持有源指针；队列记录可被后续覆写，
本项不新增分配或释放责任。正常队列条件下，六次分别复制
`23/17/35/33/2/2` 字节，均含 NUL 且容于 255 字节，排队前后均无转码。

### 静态预期输出

以下仅继承既有算法并按交接数值记录预期，不硬编码布局、不复审算法或宣称画面已验。
原四行分别按 `11/8/17/16` 次双字节推进，准确落到各自末尾 NUL；
在 `font0.doubleWidth = 17` 初值下，测宽分别为 `187/136/289/272` 像素。
实际布局只测槽 8 与槽 6：

| 项目 | 静态预期 |
|---|---|
| 窗宽 | `((289 + 95) / 64) * 64 = 384` 像素，沿用整数除法 |
| 窗口位置 / 高度 | `(128,144)` / `192` 像素 |
| 六行共用 X | `128 + (384 - 187) / 2 = 226` |
| 槽 6..11 的 Y | `174/194/214/234/254/274` |
| 第三行右边 / 窗口右边 | `226 + 289 = 515` / `128 + 384 = 512`，多 3 像素 |

保留原共用 X 与窗口公式，不为第三行的 3 像素差值纠偏，最终裁剪行为未验证。
修复前 UTF-8 槽 6、8 的正文分别为奇数长度 `33/51`，旧双字节推进可能越过 NUL
读取保留尾部，不能为修复前窗口及共用 X 给出固定数值；不得把上述原字节布局写成
已验证的运行结果。

### 边界与后续门禁

- `DrawAllQueued@0x520DD0` 仍无实现，`sub_507140` 文本命中生产者未闭合；
  [LoginMessage.cpp](../src/LoginMessage.cpp) 第 16 行 `kConfirmText` 仍为 UTF-8。
  本项只覆盖四行及其共享槽消费合同，不代表整体编码、确认交互、弹窗或最终呈现完成。
- `RPLAN.20` 不专项检查溢出、回绕、令牌耗尽，正常位宽、掩码与取模行为保留；
  其他异常域、编码或布局差异不自动豁免，不把既有限制当作新增设计批准。
- 本文档任务 `R6.1.2.9.D` 仅自查文档转录与链接定位；后续由独立角色进行增量 review
  和文件验证。本轮不编译，不新增或执行测试，不运行客户端，不查询 IDA、不使用图谱，
  不计算或复核哈希，也不复检既有内容；不写唯一进度、不暂存或提交。

## R6.1.2.11 账号帮助与强制改密十二常量原字节合同

### 来源与常量

本节对应文档任务 `R6.1.2.11.D`，继承 Planck 的 READY 分析、主线程已完成的门禁及
[RPLAN.16 文本编码边界 SPEC](cg2d_text_encoding_spec.md)，无新增设计。交接基线为
`259c179`；下述源码行号均指交接基线。原字节直接引用本文第 217–233 行，不重复抄表、
重新取证或从汉字转码；具体编码名称仍未知，不猜作 Big5/CP950。

另行源码任务仅对齐 [LoginScene.cpp](../src/LoginScene.cpp) 的 `kAccountHelp0..5`、
`kPasswordChange0..5`：正文全部用连续 `\xNN`，仅隐式追加一个 NUL，保留原址注释，
逐项以 `sizeof` 断言下表数组字节数；其余源码不动。源码、文档仍为 UTF-8。

| 常量 | 原地址 | 正文字节 | 数组字节（含唯一 NUL） | 目标槽 | 宽度（px） |
|---|---|---:|---:|---:|---:|
| `kAccountHelp0` | `0x55C798` | 29 | 30 | 6 | 248 |
| `kAccountHelp1` | `0x55C778` | 28 | 29 | 7 | 239 |
| `kAccountHelp2` | `0x55C754` | 32 | 33 | 8 | 278 |
| `kAccountHelp3` | `0x55C738` | 24 | 25 | 9 | 205 |
| `kAccountHelp4` | `0x55C71C` | 24 | 25 | 10 | 211 |
| `kAccountHelp5` | `0x55C700` | 26 | 27 | 11 | 223 |
| `kPasswordChange0` | `0x55CCEC` | 40 | 41 | 0 | 341 |
| `kPasswordChange1` | `0x55CCC0` | 40 | 41 | 1 | 340 |
| `kPasswordChange2` | `0x55CC94` | 40 | 41 | 2 | 340 |
| `kPasswordChange3` | `0x55CC6C` | 38 | 39 | 3 | 327 |
| `kPasswordChange4` | `0x55CC48` | 32 | 33 | 4 | 276 |
| `kPasswordChange5` | `0x55CC24` | 34 | 35 | 5 | 302 |

宽度沿用 `font0` 单字节 9、双字节 17 的初值。`0x55CC24` 前导严格为四个 `A1 40`，
随后是小写 `http://www.softstar.com.tw`；其他行原有大写 `SOFTSTAR` 不变。

### 触发与生产顺序

场景子状态 3 中 `UpdateServerConnection` 返回 `-2`，按有符号 W16
`gValueDA3B24@0xDA3B24` 区分：错误 2/5 为六行 `Multiline`，错误 3 为十二行
`Multiline_New`；错误 4 的“點數不足”不动。`LoginState` 阶段 4 将
`Sub_508360` 的负返回映射为 `-2`，但该函数仍缺，错误生产链不能称为运行闭合。

```text
2/5：子状态 100 → 主缓冲写 Multiline → helper 写槽 6..9 → 槽 10 → 清错误 → 槽 11
3：子状态 100 → 主缓冲先写 Multiline 再写 Multiline_New → 槽 0..5 → helper 写槽 6..9 → 清错误 → 槽 10、11
```

helper 原第 290–293 行复制前四账号常量，仅有原第 455、469 行两个 caller；
后两账号行在原第 456/458、471/472 行复制，密码前缀在原第 463–468 行复制。
各行分别进入独立槽并逐行绘制，不拼成长串。随后两路均关闭网络
`ShutdownNetworkRuntime_5081A0`；同帧子状态 100 仅调用 `ResetLoginMessage`
重置消息内部状态，再进入 101。101 完成后写回子状态 1 并继续公共尾部，不反向重入
本帧状态 1；102 也有消息消费者，但这两路不生产 102，既有尾部顺序不改。

### 消费与静态输出

[LoginMessage.cpp](../src/LoginMessage.cpp) 六行路原第 100 行测槽 8 建窗，第 128 行
测槽 6 求共用 X，第 139 行绘槽 6..11；十二行路原第 166/194 行测槽 0，
第 205 行绘槽 0..5，第 219 行绘槽 6..11，后六行沿用同一 X。
首次消息内部状态 `0 → 1`、结果置 `-2` 并创建对象。点击匹配先于 readiness：
匹配则播放音效，非空对象标死并清非拥有指针，返回 1；未匹配且对象为空或未 ready
则返回 0、不绘正文及“確認”，通过后才提交正文与“確認”。

以下静态数值仅在十二常量全部对齐、消息初始状态 0、`font0` 单 9/双 17、
未点击完成、对象 ready、队列参数合法且容量足够时成立。建窗保留整数公式
`W = ((指定槽宽 + 95) / 64) * 64`，仍测指定槽，不改为最大行宽，不硬编码推导结果。

| 静态项 | 错误 2/5：六行 | 错误 3：十二行 |
|---|---|---|
| 建窗依据 / 窗宽 | 槽 8：278 / 320 px | 槽 0：341 / 384 px |
| 窗口 `(x,y,w,h)` | `(160,144,320,192)` | `(128,72,384,336)` |
| 共用 X | `160 + (320 - 248) / 2 = 196` | `128 + (384 - 341) / 2 = 149` |
| 密码行 Y（槽 0..5） | 不读写这些槽 | `102/122/142/162/182/202` |
| 账号行 Y（槽 6..11） | `174/194/214/234/254/274` | `232/252/272/292/312/332`，仍共用 X149 |
| “確認” `(x,y)` | `(303,300)` | `(303,368)` |
| 仅消息文本提交数 / “確認”结果 | 7 / `n+6` | 13 / `n+12` |

正文调用为 `DrawText(x,y,2,0,color,slot,0,0)`，密码 `color=4`、账号 `color=0`；
“確認”为 `DrawText(303,y,2,0,4,kConfirmText,2,int8(uint8(gValueD69768)))`，保留
低 8 位及有符号解释。表中 `n` 为消息入口文本索引，仅在所有提交连续且成功时成立，
不是整个登录场景输出或最终绘制结果。`R6.1.2.10` 的“確認”已对齐为原 5 字节
（含 NUL），前节相关 UTF-8 表述为历史状态；`.9/.10` 已通过，本项不重审。

### 缓冲、所有权与边界

十二常量均为静态存储；`CopyText` 按含 NUL 的 `SourceSize` 分别复制到
`LoginMessage` 唯一 `char[12][256]`，槽地址为 `0x9A620C + i*0x100`，余部保留至
覆写，六行路不读写槽 0..5。场景私有主缓冲为 260 字节；`Multiline` 正文/含 NUL
为 9/10 字节，`Multiline_New` 为 13/14 字节，仅标记进入 256 字节分行栈数组，
无 LF 故产生一行；十二行正文不进该栈副本，以上链路不做 Unicode 转换。

`TextMeasure` 同步借用槽指针；[LoginStateDependencies.hpp](../src/LoginStateDependencies.hpp)
的薄适配通向 `DrawText_456780`，继承既有独立记录 `text[255]` 的含 NUL 复制，
本项最长复制 41 字节。仅覆盖正文及终止 NUL，`text[255]` 未覆盖尾部保留至后续覆写。队列持 record token，不持槽指针，记录内容有效至覆写；
不新增释放责任。Yobi runtime/list 拥有对象，消息只持观察指针，标死不即时释放。

`Sub_508360`、`DrawAllQueued@0x520DD0`、`sub_507140` 缺口保留，不宣称完整消息、
渲染或客户端完成。正常位宽、掩码与取模不变；overflow 专项回绕、令牌耗尽不做，
一般内存、栈、除零问题不因此豁免。`include/cg/LoginMessage.hpp` 的 UTF-8 注释属于
遗留表述，兼容数据以已批 SPEC 为准，本项不改该文件。

本文仅追加转录并自查，不新增证据、复审旧算法或提前给出独立 review/文件验证 PASS；
不编译、测试或运行，不查询 IDA、计算哈希或使用图谱，不派生代理或新任务。
原 377 行/24217 字节前缀保持不变；进度仅提供草稿，不写进度、不暂存、不提交。

## R6.1.2.12 十八条单行登录提示原字节合同

### 基线与范围

本文档任务为 `R6.1.2.12.D`，基线为 `2720cd6`，继承 Heisenberg READY 交接及主线程
已通过的门禁。[RPLAN.16 文本编码边界 SPEC](cg2d_text_encoding_spec.md) 已覆盖本项，
无新增设计；`.8` 至 `.11` 及旧算法不重审。下述“原第几行”均为交接基线快照，
不是修改后的实时源码行号。原第 188–207 行字节表完整保留，仅引用，不重取、不转码。

另行源码任务仅在 [LoginScene.cpp](../src/LoginScene.cpp) 原第 27–44 行的十八个唯一定义
落位原字节、唯一末尾 NUL 及长度断言；本项不代替源码代理报告落位结果。
源码、文档仍为 UTF-8，兼容正文按原字节保存；来源编码名称未知，不猜作 Big5/CP950。
`NETWORK` 大写字节为 `4E 45 54 57 4F 52 4B`；`kCannotConnect` 末尾严格为
`A1 43 20 00`，句尾单字节空格不删除。窗口、标题、URL、密码过期正文、两种
`Multiline` 标记及其他静态数据不在本项范围，另行 RAND 文档任务也不在本项范围。

### 十八条生产与初值矩阵

记 `R = UpdateServerConnection` 的返回值，`E` 为 `gValueDA3B24@0xDA3B24` 的有符号
W16 值；`C` 为直接调用 `CopyText(gLoginSceneMessage, 常量)`，`Z` 为清零该错误值。
除表中特别列出的 `R` 外，均要求 `R=-2`。正文与 `N` 的单位为字节，`N` 含唯一
末尾 NUL；布局列依次为测宽 `w`、窗宽 `W`、窗口 `X`、正文 `textX`，单位为像素。
表内十八个唯一定义共对应二十一处直接使用，不把多个错误值误计为多个源码使用点。

| 常量 | 原地址 | 正文 / N | 全部旧源码使用点、条件与写序 | w / W / X / textX |
|---|---|---:|---|---|
| `kNetworkNotReady` | `0x55C7C4` | 19 / 20 | 原 472：`R=-1`，仅 C，不清 E | 165 / 256 / 192 / 237 |
| `kCannotConnect` | `0x55C62C` | 17 / 18 | 原 479：`E>1234`，C→Z；原 586：default，C→Z | 145 / 192 / 224 / 247 |
| `kServerCrowded` | `0x55CAD0` | 16 / 17 | 原 483：`E=1234`，Z→C | 136 / 192 / 224 / 252 |
| `kVersionError` | `0x55C7B8` | 8 / 9 | 原 487：`E=1`，C→Z | 68 / 128 / 256 / 286 |
| `kInsufficientPoints` | `0x55C6F4` | 8 / 9 | 原 513：`E=4`，C→Z | 68 / 128 / 256 / 286 |
| `kAlreadyOnline` | `0x55C6E4` | 14 / 15 | 原 517：`E=6`，C→Z；原 522：`E=7`，Z→C | 119 / 192 / 224 / 260 |
| `kLoginServerError` | `0x55C6D0` | 18 / 19 | 原 525：`E=8`，C→Z | 153 / 192 / 224 / 243 |
| `kStoredTimeExpired` | `0x55C6C0` | 12 / 13 | 原 530：`E=9`，Z→C | 102 / 192 / 224 / 269 |
| `kBillingError` | `0x55C6B0` | 14 / 15 | 原 533：`E=10`，C→Z | 119 / 192 / 224 / 260 |
| `kSuspended` | `0x55C6A4` | 10 / 11 | 原 539：`E=11/13`，Z→C；原 543：`E=12/14`，C→Z | 85 / 128 / 256 / 277 |
| `kUnknownError` | `0x55C698` | 10 / 11 | 原 548：`E=15`，Z→C | 85 / 128 / 256 / 277 |
| `kServerBusy` | `0x55C678` | 28 / 29 | 原 551：`E=16`，C→Z | 238 / 320 / 160 / 201 |
| `kWaitBeforeRetry` | `0x55C658` | 30 / 31 | 原 556：`E=18`，Z→C | 255 / 320 / 160 / 192 |
| `kNoTrialService` | `0x55C640` | 22 / 23 | 原 559：`E=19`，C→Z | 187 / 256 / 192 / 226 |
| `kMachineLocked` | `0x55CC00` | 34 / 35 | 原 564：`E=20`，Z→C | 289 / 384 / 128 / 175 |
| `kAccountFrozen` | `0x55CB70` | 24 / 25 | 原 578：`E=22`，Z→C | 204 / 256 / 192 / 218 |
| `kCannotConnectUpdate` | `0x55C618` | 18 / 19 | 原 596：`R=-3`，仅 C，不清 E | 153 / 192 / 224 / 243 |
| `kRestartForVersion` | `0x55C5F8` | 30 / 31 | 原 601：`R=-4`，仅 C，不清 E | 255 / 320 / 160 / 192 |

原 586 的 default 精确为 `E<=0`、`E=17` 或 `24<=E<=1233`；`E>1234` 已在原 479
处理，`E=1234` 单独处理。`E=2/3/5/21/23` 有其他分支，不属于本表，也不落入 default。

### 连接结果与场景时序

[LoginState.cpp](../src/LoginState.cpp) 连接阶段 2 依次关闭网络、保存并重置黑名单、
初始化网络；初始化返回 0 才产出 `R=-1`。阶段 4 将 `Sub_508360<0` 映射为 `R=-2`，
将 `Sub_508360==1` 映射为 `R=1`，其余保持等待。非零结果先标死并清连接对象、
加载黑名单，再返回；`Sub_508360` 仍缺，具体 `E` 的生产尚未闭合。
当前连接阶段 1 已旁路，不产出 `R=-3/-4`，但场景内这两个历史分支仍保留。

场景收到表内结果后，先置登录子状态 100，再按各行执行 C/Z，之后才关闭网络：
`R=-1/-2/-3/-4` 的关闭点分别为 `LoginScene.cpp` 原第 `473/591/597/602` 行。
同帧继续 `100 → ResetLoginMessage → 101 → UpdateLoginMessage`。消息返回 0 时等待；
非零时仍按既有 URL 标记条件处理，随后写回子状态 1 并更新标题，不反向重入本帧状态 1。
十八条生产路径均不写 URL 标记，也不生产子状态 102；原第 631 行的 102 消费者保留。

### 三层复制、尾部与所有权

| 层次 | 容量、覆盖与尾部 | 所有者、借用与寿命 |
|---|---|---|
| 静态常量 → 场景主缓冲 | 唯一 `gLoginSceneMessage` 为 `char[260]@0x9A60F0`；`SourceSize=N`，`memcpy` 只覆盖 `[0,N)`，余 `260-N` 字节保留 | 常量为静态存储，主缓冲由场景持有；复制后正文至覆写有效，不转码、不新增分配 |
| 主缓冲 → 分行栈副本 | 每次 `UpdateLoginMessage` 先零初始化 `splitText[256]` 与 `lines[256]` 指针表，再原样复制 `strlen+1` 字节；这是本次零初始化的栈数组，不继承主缓冲旧尾部 | 栈副本及行指针仅活到本次调用结束；测宽同步借用，不保存这些指针 |
| 栈中正文 → 独立文字记录 | `text[255]` 精确复制 `N` 字节含 NUL，只覆盖 `[0,N)`，余 `255-N` 字节保留 | 队列记录拥有字节副本，至记录覆写有效，不转码、不持有栈指针；队列以记录 token 关联 |

十八条均无 LF、无内嵌 NUL，且不等于 `Multiline` 或 `Multiline_New` 两个 ASCII 标记，
故 `lineCount=1`、`lines[0]` 指向 `splitText`，仅测这一行，不读取共享
`gValue9A620C` 多行槽。最长正文为 34 字节、`N=35`：栈输入正文上限是 255 字节
（含 NUL 最多 256），队列正文上限则是 254 字节（含 NUL 最多 255），两者不可混同。

消息状态、结果、布局与对象观察指针属于消息模块共享状态，并非每种输入的私有状态；
`ResetLoginMessage` 只清 W16 内部状态，不同时清结果、布局或观察指针。
Yobi runtime/list 拥有 action，action 拥有 payload；消息模块只持非拥有观察指针，
`MarkDeath` 只标死，不即时释放。鼠标边沿复用
[LegacyMouse.cpp](../src/platform/LegacyMouse.cpp) 原第 225 行的唯一所有者，不新增副本。

### 初次布局与通用按钮时序

表中布局仅在十八常量已按原字节替换、消息内部状态初值为 0、后续正文不变，
且 `font0` 单字节宽 9、双字节宽 17、双字节高 17 的初值条件下成立。
`NETWORK` 路按 7 次单字节、6 次双字节推进；`kCannotConnect` 按 1 次单字节、
8 次双字节推进；其余为正文字节数除以 2 次双字节推进，均准确在 NUL 停止。

沿用整数公式 `W=((w+95)/64)*64`、`X=(640-W)/2`、`textX=X+(W-w)/2`。
单行窗口 `H=96`、`Y=192`，正文 `textY=222`；creator 为
`Sub_5044E0(X,192,W,96,4,1)`，传入尺寸已为整倍数，不再二次改变尺寸。
这些数值是静态条件推导，不是运行输出，也不硬编码进源码。

[LoginMessage.cpp](../src/LoginMessage.cpp) 原第 241 行通用分支首次将内部状态置 1，
随后算高度、置结果 `-2`、确定布局并创建对象。每次调用都先以 layer `-1` 检测半开
按钮区域 `[287,353) × [253,270)`，命中后才读取鼠标值低 8 位的 bit 0。
点击先调用 `PlaySe(79,320,240)`，对象非空则标死并清观察指针，点击路径返回 1；
此路径早于 ready 检查，对象尚未 ready 或为空也不把点击延后。
未点击时，仅对象非空且 ready 值至少为 1 才提交正文与按钮，最后返回 0；
内部状态已为 1 而对象为空时不会自动重建。

### 条件化排队输出

以下仅描述未点击、对象非空且 ready、队列参数合法且容量足够时，消息本次调用的贡献；
不计 callback 或场景公共绘制尾部，不由提交成功推定最终可见。

正文调用为 `DrawText(textX,222,2,0,0,lines[0],0,0)`，
[LoginStateDependencies.hpp](../src/LoginStateDependencies.hpp) 原第 19 行薄适配到
`DrawText_456780`，继承[文本待提交队列合同](cg2d_text_pending_queue.md)：

- 独立 text record 保存 S16 坐标、`mode=2`、`font=0`、`color=0`、`value7=0`、
  `marker=0`；`text[255]` 按上文精确复制含 NUL 的 `N` 字节，未覆盖尾部保留。
  `field108` 不写，`nextToken=0`，不存栈指针、不转码，副本至覆写有效。
- 文字索引 `n` 为 W32 零基索引，记录 token 为 `n+1`，连接 mode 尾槽 2。
  空链使用字体层初值 `0xD8` 提交 graphic：`imageNo=-1`、`value4=0`、
  `batchType=4`、`dataBits=n+1`；非空链连接旧尾。
- 正常文字计数从 `n` 增至 `n+1`；调用返回值被丢弃，不写 `gMessageResult`。
  图形计数另以 W16 维护；首链 graphic 排队失败不回滚 text chain，因此不能从返回值
  推出正文可见，也不能将正常容量条件下的计数描述推广到失败路径。

随后按钮调用为 `DrawGraphic(320,261,0xDF,hover?243050:243048,0,descriptor)`；
descriptor 严格为以下 16 字节：

```text
00 00 00 00 00 00 00 00 FF FF FF FF 00 04 00 00
```

该调用继承[角色选择场景图形合同](cg2d_character_select_scene.md) 原第 752 行：
成功记录的坐标为调用坐标加资源中的 S16 offset，图号为资源映射后值，
`value4=0`、`dataBits=0`、`batchType=0`，descriptor 独立复制。
本项未读取资源，不能把 `(320,261)` 调用坐标写成最终贴图坐标。
本单行分支不提交 `.10` 的“確認”文字，也不消费 `.9/.11` 的共享多行槽。

### 依赖现状与验收边界

[旧登录控制器文档](cg2d_login_state_controller.md) 将 `Sub_5049A0`、`DrawGraphic`、
部分共享值仅列为声明的记录已滞后；当前交接已定位
[LegacyHitTest.cpp](../src/LegacyHitTest.cpp) 原第 7 行、
[DrawGraphic.cpp](../src/DrawGraphic.cpp) 原第 169 行，以及
[Font.cpp](../src/render/Font.cpp) 的字体高度别名和上述鼠标唯一所有者。
本项纯继承这些定位，不重审实现，也不回写旧文档。

`Sub_508360`、`DrawAllQueued@0x520DD0`、`sub_507140` 仍缺，`PlaySe` 仍为声明。
通用按钮不直接使用 `sub_507140` 的文字命中索引，不代表整体交互已经通过；
具体错误生产、音频和最终呈现的缺口仍在，不能宣称完整客户端或画面已验。
正常位宽、掩码、取模行为保留，不做 overflow、专项回绕或耗尽检查，但一般内存、
栈和除零问题不因此豁免；`RPLAN.15` 不授权文本或音频差异。

本项验收覆盖十八条表、二十一处使用、全部 C/Z 写序差异、三层复制尾部与寿命、
条件化布局及排队输出、实际依赖缺口。本轮仅追加交接转录并自查，不给出独立 review
或文件验证 PASS；原 478 行/30972 字节前缀须保持不变，交由后续独立文件验证确认。
不查询 IDA、不计算或复核哈希、不使用图谱，不编译、测试或运行，不暂存、提交或委派
新任务；不写源码、其他文档、进度及用户已有改动，进度只向主线程提供草稿。

## GetServerAddressInfo_507D90 地址查询合同

`LoginScene.cpp` 的 `cg::login::detail::GetServerAddressInfo_507D90` 借用同文件唯一
`gValueD69798` 存储，不生产或初始化服务器记录。原函数 `0x507D94–0x507D9D`
对负索引及 `>=800` 返回 `-1`，不写地址和端口；合法索引按
`0x507D9F–0x507DA6` 的 `0xC1` 字节步长定位表项。

原 `0x507DAE–0x507DCC` 从记录 `+0` 复制地址至输出，包含结束 NUL；
`0x507DCE–0x507DE1` 从 `+0x80` 以 `_atoi` 解析十进制文本，仅将低 16 位
写入端口；`0x507DE5–0x507DE8` 返回 `0`。查询不读取 `+0xC0` 的
`available` 标志；零填充空记录也返回成功，地址为空串、端口为 0。
地址与端口均写入调用者缓冲，不保留记录指针；调用者
`LoginState.cpp` 的 `Sub_508360` 将负返回映射为 `-2`，不由本函数发起连接。
本项不改变记录生产者；输出容量、源字符串终止与 `_atoi` 溢出等异常域
不另加保护。

## 登录侧未命名依赖的接线 SPEC

2026-10-02，声明/本体定点检查发现login::detail的PlaySe、HasIDirect3D7和ReadNegativeSelectionAvailability仍只有声明，不能由此前地址命名函数扫描认定已闭合。

本轮按既有工程委托先完成两项证据充分的接线：

1. PlaySe保留三参数窄接口，在既有LoginMessage.cpp定义一次。第一参数为S32音效编号，第二参数为S32屏幕X；第三个原压栈值由52C450忽略，不给它新增含义。适配器只明确弃用第三参数、调用已存在audio::PlaySe_52C450一次并原样返回S32。所有既有caller的参数求值和调用位置保留；不改LegacyAudio/SDL后端，不恢复暂缓的音乐、murD或52C3D0。
2. HasIDirect3D7只有LoginScene尾部一处caller，直接改为调用既有render::HasLegacy3DDevice_44B610并显式弃返回，删除私有悬空声明。查询仍先于两次Sub_45F090及版本文字绘制，读取同一renderer接口槽，不补常量能力。

ReadNegativeSelectionAvailability仍须对20个真实宿主字节逐项确认owner与偏移，不以零值、相邻分配或现代服务器表越界读取代替。本轮两项接线不覆盖此第三项，也不代表整个登录或音频完成。

完成后集中核声明/定义唯一性、调用次数与顺序、完整返回、未改caller参数及差异编码；继续不构建、不运行、不测试、不做哈希校验。

两项接线已按上述边界落地。必要静态核对确认：PlaySe保持三参声明和单次两参转发、直接返回完整S32；原caller实参未改；3D查询仍在两次绘制之前。此结论仅覆盖新增源码差异，不是链接或运行验证。

### 负分类读取的宿主定位（尚未实现）

同轮在已打开cg2d.exe会话只读定位20个地址，再与现有源码对照。下表的存储定位不等于全部字节语义已闭合；IDA自动item边界也不单独证明owner。

| 原读取地址 | 目前定位及边界 |
|---|---|
| D68944 | 原第0组底部栏+0C，对应LegacyUiRecord.cpp的gLegacyBottomInitWordsD68938[0][3]；原502CB0生产 |
| D68A05 | 原第5组底部栏+05，对应同一owner的[5][1]第1字节；原502F30生产。新数组只存每组9个S32，不能套原40B步长直接跨组寻址 |
| D68AC6 | 尚未确认真实owner及生产；不能以无直接xref当作恒零 |
| D68B87 | gHoverTextD68AE8的+9F字节 |
| D68C48 | gHoverTitleD68BF4的+54字节 |
| D68D09 | mouseY的原对象+1；现有platform::gLegacyMouseState.mouseY.legacyVftToken保存原W32位型，可直接提取其第1字节，不用Read()，不额外消费RNG |
| D68DCA | gValueD68DC8的第2字节 |
| D68E8B、D68F4C、D6900D、D690CE、D6918F | 固定鼠标池D68DD0的+187、380、573、766、959字节，依24B记录布局分别落在节点7的next第3字节、节点15/23/31/39的prev第0/1/2/3字节 |
| D69250、D69311、D693D2、D69493、D69554、D69615、D696D6 | 同一固定池+1152、1345、1538、1731、1924、2117、2310字节，分别为节点48/56/64/72的x第0/1/2/3字节、节点80/88/96的y第0/1/2字节 |
| D69797 | gValueD69794的第3字节 |

现有LegacyMouseTracker.cpp的fixedPool确为100个24B节点，但next/prev已按[追踪器局部适配](cg2d_legacy_mouse_tracker.md#x64-handle-适配与安全边界)保存节点编号和哨兵值，原值则为进程指针。因此，上表五个链接字节不能直接读取当前handle并声称原端等价；也不能改读可能已切换的动态nativePool。暂不添加raw-byte接口、原地址反算表或第二份状态，不借本项重构已实现的追踪器。

负分类读取继续保留独立缺口：D68AC6的owner和链接表示差异尚需闭合；不以补零、固定可用或当前链接编号替代。

2026-10-02后续定位确认LegacyCryptInt.hpp将legacyVftToken显式保留为原W32数值，LegacyMouseState的mouseY偏移为10H，构造/析构写54D3A0，CopyPayload只写载荷，而ResetLegacyMouseState_506870会清零整个鼠标状态（包含token）。因此D68D09读取应取现有token第1字节，不应直接硬编码D3；本项仅闭合存储定位，暂不新增孤立借用接口。D68AC6位于D68ABC计时值之后、D68AC8伴随值之前；对D68A78..D68AE8的当前item及直接xref定位未发现其owner，不将邻近变量或无xref当作归属/恒零证明。

## 登录标题与密码过期说明原字节 SPEC（2026-10-03）

按已批准的[文本编码SPEC](cg2d_text_encoding_spec.md)，本轮补齐LoginScene中仍以
UTF-8字面量保存的标题和密码过期说明。读取当前已打开IDA的已加载原始字节，
不使用IDA解码后的字符串显示作为字节证据，不按代码页名称重新编码中文。

### 标题生产与消费者

- 原WindowName@559384正文为C5 5D A4 4F C4 5F A8 A9，含NUL共9B。
  45FEC9、4600FD/46011F、460958、4609AF均把该串送入标题格式化。
- 559958起25个64B分类槽的正文与本文既有“25个分类标题的原字节正文”表逐项一致；
  新端保留25项指针表，只把各正文改为明确字节。原64B索引已由CategoryTitle转换，
  不恢复跨对象取址，不改group25..39既有相邻字节视图，也不改20个ASCII服务器标号。
- 55CD18特殊格式正文为25 73 20 25 73 20 5B B0 66 20 25 73 5D；
  槽9仍先格式化普通标题，再覆盖为该特殊标题。“逆”保留原B0 66两个字节。
  基础和普通服务器格式仍为ASCII，版本7.1.00保持。
- SetBaseWindowTitle/SetServerWindowTitle仍使用同一256B调用方缓冲和既有snprintf，
  随后原样交SetLegacyWindowText→SetWindowTextA；不新增UTF-8转换、不迁移API，
  不指定猜测的代码页。各已证标题常量在现有容量内，不借本轮改变历史格式化适配。

### 密码过期回复链

原case23在4607F1读取55CAE4，4607FB先置URL标志，46080B清W16回复码，
460815..460824将含NUL正文复制到9A60F0。原正文139B、含唯一NUL共140B，
五个LF位置保持；六段长度依次16/26/32/30/30/0，最后一段为空。

原始正文（不含末尾NUL）：

```text
BF CB B7 52 AA BA AA B1 AE 61 B1 7A A6 6E A1 47 0A A5 D1 A9 F3 B1 7A AA BA B9 43 C0 B8 B1 4B BD 58 A4 77 B8 67 B9 4C B4 C1 A1 41 0A BD D0 B5 6E A4 4A A4 6A A6 74 B7 7C AD FB A8 74 B2 CE A7 F3 B4 AB B9 43 C0 B8 B1 4B BD 58 A1 41 0A BD D0 A9 77 B4 C1 A7 F3 B4 AB B1 4B BD 58 A5 48 BD 54 AB 4F B1 62 B8 B9 A6 77 A5 FE A1 43 0A C2 49 C0 BB BD 54 A9 77 A1 41 B1 4E AB 65 A9 B9 53 4F 46 54 53 54 41 52 BA F4 AD B6 A1 43 0A
```

新端kPasswordExpiredMessage改为上述字节，以sizeof==140断言固定合同；既有CopyText
仍只复制正文及NUL到260B共享消息缓冲，不清剩余120B，不改变URL标志/回复码写序。
后续UpdateLoginMessage仍复制到256B局部区并按LF分行，再沿既有TextMeasure和
文字入队路径消费原字节；本轮不改测宽、分行、布局、绘制或关闭/打开URL流程。
该静态文本不等同Multiline标记，不改账号说明和机码说明的其他共享行。

### 实施与核对边界

只修改上述常量及特殊格式中的两字节，保留全部业务控制流。集中静态核对IDA原字节、
NUL/长度/LF、25项顺序、实际使用位置和源码限定差异；不执行客户端或格式化/布局样例。
本文早期“新端标题文本使用UTF-8”描述的是修复前状态，此增量覆盖该状态描述。
本轮不证明目标系统ANSI显示一致，也不将标题与说明修复算完整登录运行验收。
不构建、检查产物、新增或执行测试、计算或比对哈希；总SPEC和总索引不例行刷新。

## 登录分支与删除角色提示原字节 SPEC（2026-10-03）

沿已批准文本编码合同，补齐五份现有源码的12处常量定义（11个唯一原地址）。
本轮用已打开IDA读取原始字节及父函数数据引用，不从解码显示或源码中文反向转码。
两种能力页的凭据提示同源55CA8C；保留两份既有静态常量，不为了合并常量重构模块。

| 源文件 / 常量 | 原地址 / 引用指令 | 含NUL字节数 | 原正文 |
|---|---|---:|---|
| CharacterDeletionConfirmation.cpp / kConfirmText | 55C8D4 / 45E134 | 10 | 20 20 BD 54 20 20 A9 77 20 |
| CharacterDeletionConfirmation.cpp / kCancelText | 55C8C8 / 45E148 | 10 | 20 20 A8 FA 20 20 AE F8 20 |
| CharacterDeletionConfirmation.cpp / kWarningTitle | 55C8B0 / 45E234 | 21 | A1 AF A1 AF A1 AF 20 C4 B5 20 20 A7 69 20 A1 AF A1 AF A1 AF |
| CharacterDeletionConfirmation.cpp / kDeletionCannotRecoverLine1 | 55C898 / 45E295 | 21 | A9 D2 BF EF BE DC AA BA A4 48 AA AB A4 40 B8 67 A7 52 B0 A3 |
| CharacterDeletionConfirmation.cpp / kDeletionCannotRecoverLine2 | 55C88C / 45E2F3 | 11 | B1 4E B5 4C AA 6B AB EC B4 5F |
| CharacterDeletionConfirmation.cpp / kDeletionQuestion | 55C87C / 45E351 | 15 | BD 54 A9 77 AD 6E A7 52 B0 A3 B6 DC A1 48 |
| LoginCapability3Scene.cpp / kCredentialsPrompt | 55CA8C / 45F84D | 21 | BD D0 B3 5D A9 77 49 44 A1 42 50 41 53 53 57 4F 52 44 A1 43 |
| LoginCapability4Scene.cpp / kCapability4CredentialsPrompt | 55CA8C / 464C4F | 21 | BD D0 B3 5D A9 77 49 44 A1 42 50 41 53 53 57 4F 52 44 A1 43 |
| LoginCapability4Scene.cpp / kCapability4DeletePrompt | 55CE4C / 464CC8 | 25 | BD 54 A9 77 AD 6E A7 52 B0 A3 B3 6F B1 62 B8 B9 B0 4F BF FD B6 DC A1 48 |
| LoginCapability4Scene.cpp / kCapability4DeleteSucceeded | 55CE34 / 464D64 | 21 | B1 62 B8 B9 B0 4F BF FD A4 77 A6 A8 A5 5C A7 52 B0 A3 A1 43 |
| LoginModeScene.cpp / kMissingCredentials | 55C59C / 45C78B | 29 | BD D0 BF E9 A4 4A A2 D7 A2 D2 A9 4D A2 DE A2 CF A2 E1 A2 E1 A2 E5 A2 DD A2 E0 A2 D2 |
| LoginOuterScene.cpp / kIncorrectAccountOrPassword | 55C584 / 45C534 | 21 | A4 A3 A5 BF BD 54 AA BA B1 62 B8 B9 A9 4D B1 4B BD 58 A1 43 |

### 原缓冲与消费者

1. 45E0C0删除角色警告的确认/取消静态指针仍进入两个CharacterModalUiRecord，
   分别沿503990命中和503A20绘制；保留文字中的空格、全角星号及标点。
   其余四段仍依序复制到同一256B局部缓冲，TextMeasure再DrawText；保持384像素窗口宽、
   原坐标和四次测宽/绘制顺序，不硬编码修复后的测宽结果，不清复制以外的尾部。
2. 45F190的提示按sizeof复制到既有9A50E8消息区；464840的三段按各自sizeof复制
   到既有9A4A78消息区。两区仍各256B，状态/写序不改；删除确认继续交464450，
   其他提示继续交UpdateLoginMessage，不混用两个模态入口。
3. 45C5A0的全角ID/PASSWORD提示保留原全角字节，复制到既有256B的9A5FE0消息区，
   随后沿UpdateLoginMessage分行、测宽和入队；不替换成半角文字或另定编码。
4. 45C2D0的错误提示继续只覆盖正文与NUL。9A4BA0已有“按唯一已知正文建立最窄缓冲”
   的局部适配保持，其sizeof来源改为原21B文本，并纠正源码中的UTF-8注释；
   这不是证明原整个缓冲容量为21B，也不扩大到未知间接写者或周边地址布局。

所有常量均以十六进制转义明确正文，由字面量追加唯一NUL，并添加精确sizeof断言。
现有memcpy/strcpy、动态strlen、界面/状态/焦点/返回行为原样保持，不改编码消费者算法。

本步集中核对12处常量的原字节、唯一NUL、长度、原地址/实际使用位置及源码限定差异；
不运行测宽或格式化样例，不把源码断言写成已编译通过。各文件历史UTF-8描述仅代表
修复前状态；本增量覆盖本表常量的状态，不重审已过文本和其他业务实现。
不构建、检查产物、新增或执行测试、运行客户端或哈希校验；真实显示/交互仍未验收。


## 登录标题五真实格式板块 SPEC（2026-10-04）

依工程委托裁决，现SetBaseWindowTitle一snprintf与SetServerWindowTitle两个snprintf恢复53B304、撤sizeof容量参数，复用既有256B title与两个helper签名；登录源码只引已有内部LegacyFormat.hpp。三个源码格式callee承接五原call，不造helper原地址、不保留新增截断。

| 原call | 格式 | 源绑定与条件 |
| --- | --- | --- |
| 45FEDE | 55C7E8 %s %s | Base helper第1调用，原state0先写1后格式/窗口标题。 |
| 46010C | 55C7D8 %s %s [%s %s] | 选服原低S8输入/available门禁后：窗口名、版本、category、server四T*，包含slot9。 |
| 46012E | 55CD18 %s %s [旧B066字节 %s] | 普通格式后slot9再三T*覆盖同title；保持两次原调用，窗口只在两格式之后发布。 |
| 460971 | 55C7E8 %s %s | 消息101尾先恢复state1再Base helper第2调用。 |
| 4609BE | 55C7E8 %s %s | state102消息轮询后无条件Base helper第3调用，不以轮询失败跳过。 |

窗口名559384旧8B正文及NUL、版本55C5BC的7.1.00及NUL沿现原字节常量；category按serverIndex/20、server按page*10+slot借现表，不重建或加界限。未清256B同一title、原状态写序/窗口文本边界/选服发布/错误文案、音效调用和newest排除均保持；模块摘要相关正文不改、不执行任何摘要算法。

原[45FEA0,460C98)1135条仅读140五格式邻指令，余995条/原51C430 caller和其它状态不重验。三新含NUL格式/两个原参数常量、五真实call和slot9双格式/现三次Base helper调用与源局部变更静态核对。未构建、测试、运行或hash，不把标题恢复算完整登录/窗口容量全链/CRT或总PLAN通过。

<a id="login-negative-byte-current"></a>
## 负分类读取定义与20个owner（BRU01，2026-10-07）

[20BYTE SPEC](cg2d_login_negative_selection_byte_owner_spec.md#login-negative-byte-source-state)已落实已有ReadNegativeSelectionAvailability声明：LegacyUiRecord.cpp直接借现底部栏/hover/mouseY原token/左锚/服务器伴随值，独立D68AC6加载零BYTE有目标代码地址形成/无加载静态别名及邻近正常对象边界证据；固定池12点由既有tracker只读桥提供，七坐标直接读BYTE、五链接按实际handle投影原固定节点/哨兵身份BYTE。整体adapter不是固定可用，不建影子状态，不调用CryptInt::Read额外消费RNG。

原handle布局/解析/全部重置与追踪体保持；动态nativePool切换不改变固定池观察归属，不把当前节点编号或本机指针低字节当原BYTE。两个尾先读后判原位置和整个LoginScene.cpp保持。此当前裁决覆盖此前“仍未实现／暂不添加”的历史定位；完整登录／网络／人物／主地图与V03仍未验，运行限制保持。

## 选服分类、图号阶段与点击页号更新（BSJ01，2026-10-07）

按[BSJ01裁决](cg2d_server_category_click_sampling_spec.md#server-category-click-decision)，分类60轮各在45F100调用后先读取selected，供本轮门禁与末轮选服；成功后写selected／Play51，再按字段5→4、B-A与A+2、20对字段4／5写入及每轮+2（含末轮），成功完成后才再次读取分类。普通S32算式恢复，真实CategoryField位型和原420W32分类映像保持，不设置默认分类。

低W8输入先处理40组选中、再80点击，点击成立才单读page，同值用于available、索引与既有两标题；slot9先普通再特殊、旧字节／同256B缓冲、S16发布、声音／摘要／账号／密码／现场状态读取及break保。私有状态helper改普通S32 ++，三调用对应460188／46019C／4608FD；其余完整源码保持。摘要调用只是保留接线，本步没有执行摘要。

原父完整1135条3576B／17页、47直接／20目标及6间接、95跳转／1RET，唯一51C430@51C4CC；旧映射1080补全，不重复身份。104B邻接只登记92B／23目标表与12B真实NOP原用途。正常值原正确，单源三片段的局部静态不代完整父／20依赖、全部消费／寿命、机器ABI或运行。

2026-10-14 LNW01：动态UTF-16迁移后的负分类旧旁读已退休，当前未选择分类不显示服务器按钮；两个尾部仍共用ReadServerAvailability，非负分类读取与绘制流程保持。原460B1D／460BD7及20字节映射保留为历史，当前规则见[具名修复决议](cg2d_login_negative_selection_byte_owner_spec.md#login-unselected-unicode-20261014)，不再按旧记录要求索引动态字符串尾部。
