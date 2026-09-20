# 能力 3 Yobi 字段与 FrameMove 共享边界

## 证据范围

本项依据当前 IDA 实例 `ida-17524` 中的 `cg2d.exe`、PE 映像和当前
`cg2d_re` 源码核对。STONAGE 不作为行为证据。

`sub_45F190@[0x45F190,0x45F93D)` 的状态 2 在 `0x45F3D3`、
`0x45F458` 创建两个 `YobiAction`，并在 `0x45F44D`、`0x45F4D0` 调用
`Yobi::FrameMove(action,100,0,0)`。状态 3、4、5 继续移动、修改颜色高
字节并标记这些对象死亡。

## 唯一 owner 与原生布局

原 x86 `YobiAction` 大小为 `0x330`。新端不复制该物理布局；唯一完整
`cg::login::YobiAction` 定义只存在于 `YobiRuntime.cpp`，公开头仅暴露 opaque
指针和窄访问器。Yobi intrusive list 是对象 owner，能力 3 和角色选择保存的
`YobiAction*` 均为非拥有观察者。对象被 `MarkDeath` 标记后，场景仍须清空自己
的观察者槽。

能力 3 所需字段映射如下：

| 原偏移 | 原宽度 | IDA 语义 | 新端语义字段 |
|---:|---:|---|---|
| `+0x15` | W8 | `dispPrio` | `displayPriority` |
| `+0x30` | W32 | `gX` | `x` |
| `+0x34` | W32 | `gY` | `y` |
| `+0x2A8` | W32 | `imageNo` | `imageNo` |
| `+0x2B0` | W32 | `charAction` | `characterAction` |
| `+0x2B8` | W32 | `dir` | `direction` |
| `+0x308` | W8 | `batchType` | `batchType` |
| `+0x30C` | W32 | `color2` | `color2` |

新端字段 `x/y` 对应原 `gX/gY@+0x30/+0x34`，不是原结构中
`+0x40/+0x44` 的两个 float 字段。

状态 2 在 `0x45F433/0x45F4B7` 将 `batchType` 写为 8，并在
`0x45F440/0x45F4C3` 将 `color2` 写为 `0x00FFFFFF`。状态 3 在
`0x45F54E` 写左对象 `+0x30F`，在 `0x45F560` 重新读取该字节并于
`0x45F566` 写入右对象；到达中心后又在 `0x45F58F` 清零。状态 4 在
`0x45F603` 将同一字节写为 `0xFF`。

原平台为小端，`+0x30F` 是 `color2` 的 bits 31..24，而不是独立字段。
新端访问器以 `bit_cast<uint32_t>` 和掩码修改该高字节，严格保留低 24 位，
不依赖原 x86 对象的物理偏移。

所有普通字段访问器保持非空对象前提。原状态 3 对两个对象直接解引用，不能
在访问器中新增空指针吞并；`MarkDeath` 自身仍保持既有空指针安全行为。

## 共享 FrameMove 依赖

`Yobi::FrameMove` 的完整边界为 `[0x517880,0x517D9D)`，原型为
`int __cdecl(YobiAction*, int, int, uint32_t)`。能力 3 的两个调用均传入
`(action,100,0,0)`。该函数共有 37 个唯一 caller 和 64 个调用交叉引用，属于
Yobi runtime 的共享行为，不属于能力 3 或角色选择场景私有实现。

直接调用点为：

| 调用点 | 目标 |
|---:|---|
| `0x517974` | `Animate::getIndex@0x45B400` |
| `0x517990` | `AnimeLoadAction@0x45B760` |
| `0x517AC5` | `World::PlaySe@0x52C450` |
| `0x517B08`、`0x517CAF` | `Graphic::getXY@0x45A710` |
| `0x517B64`、`0x517D2E` | `Graphic::LoadGraphic@0x522570` |
| `0x517C9B` | `Graphic::getMapNoIndex@0x45A8B0` |

其直接全局依赖还包括 `gMAX_ANIMATE_COUNT@0x9A4A4C`、
`gAnimate@0x9A4A58`、`gWorld::SceneType@0xFF0D40` 和
`gGraphicInfoDxCache@0x103CFA4`。完整实现还需要 action 内部的
`lastImageMapNo`、`lastAction`、`lastDir`、帧索引、帧剩余时间、宽高、
隐藏状态、翻转状态和帧偏移等字段；这些依赖当前尚未共同闭合。

图号 245150 会进入 FrameMove 的静态图路径，且第四参数为 0 时只需要
`getMapNoIndex/getXY`。但现有角色预览还会向同一函数传入 `100xxx` 动画图号，
因此不能用能力 3 的 245150 特例冒充完整 `FrameMove`，否则会破坏共享调用者。

本项只在 `cg/YobiRuntime.hpp` 的 `cg::login::detail` 中保留唯一真实未解析声明
`Sub_517880`。角色选择和能力 3 必须共同引用该声明，不得在不同 namespace
复制同地址符号；完整实现留给动画、图形缓存及 action 帧状态同时闭合后的独立任务。
