# cg2d 保存状态运行期默认值 owner 证据

## 本项范围

本项只为未来 `sub_5331A0` 所读取的运行期默认状态建立唯一 owner，不实现
`sub_5331A0`、恢复路径、UI、音频、Flip 或保存行为，也不把这些 live state
复制到 `SavedStateStorage`。按原读取单元计为三个独立 W32、前缀内四个 W32
和一个 W16，共八项；源码以五个唯一 owner 符号承载。

## 地址、宽度与初值

| 源码状态 | 原地址/区间 | 宽度 | 映像初值 | `sub_5331A0` 证据 |
|---|---:|---:|---:|---|
| `gUseStereo` | `0x58D3C4` | W32 | 1 | `0x533244` 读取低字节 |
| `gEffectVolume` | `0x58D3C8` | W32 | 15 | `0x53323A` 读取低字节 |
| `gMusicVolume` | `0x58D3CC` | W32 | 15 | `0x533250` 读取低字节 |
| `gBgmFrequencySelectionPrefix` | `[0x1050840,0x1050850)` | 16 字节 | 全零 | `0x53325C/0x533267/0x533273/0x53327E` 读取四个 W32 |
| `gScanlineSyncMode` | `0x554B74` | W16 | 1 | `0x53328A` 读取低字节 |

IDA 对前三个 W32 和同步模式 W16 的静态值分别读得 `1/15/15/1`。BGM 区间
没有 PE raw backing；运行时全零来自 PE loader 对虚拟区的零填充。当前证据只
闭合 `sub_5331A0` 连续读取的 16 字节 `[0x1050840,0x1050850)`，因此源码名称
明确为 `Prefix`，不宣称后续地址属于同一张完整 BGM 表。

`gScanlineSyncMode` 保留原 W16 宽度：值 0 表示同步，值 1 表示非同步。本项不
增加枚举、钳制或 getter。

## owner 选择与边界

这些值是客户端运行期的声道、音量、BGM 选择和显示同步状态，与既有
`gEnableSound`、`gEnableMusic`、`gDeviceBitsPerPixel` 等 legacy live state 具有
相同生命周期。故唯一声明和定义收敛在 `cg::platform::CommandOptions`，而未来
保存记录函数只是读取者；若把它们复制到 `SavedStateStorage`，会形成第二份运行
状态并破坏 owner 唯一性。

本项不接任何消费者或恢复入口，不修改 UI、音频、Flip、保存函数和 CMake。
以上结论只来自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与行为、地址、类型或
验收证据。
