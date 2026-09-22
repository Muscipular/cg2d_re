# Yobi action/runtime、modal 与九宫格还原证据

## 范围与证据源

本次只依据当前 IDA 实例 `ida-17524` 中的
`D:\SOFTSTAR\cg2d.exe.i64` / `D:\SOFTSTAR\cg2d.exe`（32 位 x86，映像基址
`0x400000`）及当前 `cg2d_re` 源码。STONAGE 未作为行为证据。

| 原函数 | 范围 | 指令/基本块 | 本次结论 |
|---|---:|---:|---|
| `sub_404A40` | `[0x404A40,0x404AD1)` | 36 / 1 | 分配并连接首尾两个 `0x330` 哨兵 |
| `YobiAction::new` | `[0x404AE0,0x404C42)` | 118 / 12 | action/payload 分配、默认值、稳定优先级插入 |
| `Yobi::UpdateAll` | `[0x404C50,0x404CF7)` | 60 / 17 | 四相门控、callback、延迟死亡和释放 |
| `Yobi::MarkDeath` | `[0x404D00,0x404D10)` | 5 / 3 | 仅写 `+0x1C=1`，返回原指针 |
| `sub_404D10` | `[0x404D10,0x404D30)` | 13 / 3 | 先释放 `+0x0C` payload，再释放 action |
| `YobiClearUp` | `[0x404D30,0x404D5B)` | 13 / 4 | 先调用 `sub_46DD40`，再标死整链 |
| `sub_404D60` | `[0x404D60,0x404D85)` | 10 / 1 | `ClearUp -> UpdateAll -> free(head) -> free(tail)` |
| modal creator | `[0x5044E0,0x5046C6)` | 179 / 36 | 建立 `0x24` payload、窗口映射与展开增量 |
| modal callback | `[0x5046D0,0x5049A0)` | 250 / 39 | 展开、整块/九宫格绘制、readiness 赋值 |
| 九宫格 | `[0x49BE00,0x49C4C7)` | 525 / 35 | 角、边、中心及尾块 source rect 裁剪 |
| `Renderer::DrawGraphic3` | `[0x520970,0x520ACE)` | 114 / 20 | descriptor 版本的唯一待提交队列写入 |
| 注册表清理 | `[0x46DD40,0x46DD82)` | 24 / 7 | 399×399 标志/非拥有指针表按行清理已实现 |

关键指令证据如下：

- `0x404A41/0x404A4D` 两次申请 `0x330`；`0x404A6E..0x404ACD` 设置哨兵
  `prev/next/callback/priority/graphicIndex`。
- `0x404B96..0x404BFF` 写 action 默认值；`0x404C1A..0x404C39` 以
  `existing.priority <= new.priority` 向后扫描并在首个更高优先级前插入，因此同优先级
  保持 FIFO。尾哨兵优先级为 `0xFF`，有效业务 priority 必须小于 `0xFF`。
- `0x404C7C..0x404C94` 与 `0x404C9E..0x404CCC` 分别核证 callback/死亡的
  `flags&0x40000 && dword_59E570==1 && dword_59E574!=0` 三条件门控；
  `0x404C99` 在 callback 返回后重新读取当前 action 的 `next`。
- `0x404CD8..0x404CF6` 先递增四相计数，返回值依次为 `1,2,3,4`，达到 4 后存储
  回到 0。
- `0x404D36` 在调用 `sub_46DD40` 前保存首 action；`0x404D4B..0x404D57`
  随后逐项标死。
- 原初始化入口 `sub_4576B0` 在 `0x4577F5` 调用 `sub_404A40`。当前
  `InitializeClient_4576B0` 仍是未实现真实依赖，因此本步公开 `Sub_404A40`，不在
  `MainLoop` 再添加一次可能重复的初始化。

## 原生 x64 所有权与语义字段

新端没有复制 x86 `0x330` blob，也没有保存 32 位指针位模式。`YobiAction` 是无
虚函数、无 RTTI 的原生 x64 standard-layout/trivially-copyable 类型；公开头只暴露
opaque 指针和窄访问函数，不断言 `sizeof(YobiAction)==0x330`。

- intrusive `prev/next` 使用原生指针；runtime/list 是 action 的唯一 owner。
- action 独占 `payload`；摘链时先释放 payload，再释放 action。
- 登录、消息、角色场景中的 `YobiAction*` 只是不拥有的观察者，调用
  `MarkDeath` 后仍须由场景清空自己的槽。
- 当前闭包保留 callback、priority/display priority、cache、death、graphic、flags、
  phase、坐标/增量、scale、readiness、creator param6、walk/frame、预览图号/动作/
  方向及 16 字节 draw mode 默认值。原 `+0x2C` phase 与 `+0x170` readiness 是两个
  独立字段。
- `NewYobiAction_404AE0` 用 `calloc` 建立 action；非零 payload 尺寸另行
  `calloc` 并按原端再次清零。action 失败保留原两次消息，payload 失败先释放 action
  再显示原消息。未添加静态析构或进程退出清理。

角色预览只通过窄接口访问原 `+0x2A8/+0x2B0/+0x15/+0x30/+0x34/+0x2B8` 的
语义字段。`sub_460D00` 的三个 caller 位于 `0x46116D`、`0x461173`、
`0x4618FD`，调用后均不读取 EAX；因此新端接口改为 `void`，删除会截断 x64 指针的
`GetYobiActionLegacyPointerBits32`，保留全部副作用和末尾 `sub_517880` 调用。

## modal payload 与 creator

`YobiUIData` 是纯标量 `0x24` 字节类型：

| 偏移 | 类型 | 含义 |
|---:|---|---|
| `+0x00` | `int16` | `winId` |
| `+0x02` | `int16` | `frameCount` |
| `+0x04/+0x08` | `int32` | `width/height` |
| `+0x0C/+0x10` | `int32` | `centerX/centerY` |
| `+0x14/+0x18` | `int32` | 当前半宽/半高 |
| `+0x1C` | `int32` | 九宫格基础图号 |
| `+0x20` | `int16` | 整块绘制 style |
| `+0x22/+0x23` | `uint8` | calloc 保持为 0 的尾字节 |

creator 在 `0x5044E5` 固定以 priority 3、payload `0x24` 新建 action；
`0x504502..0x504515` 设置 `flags|=6`、callback 和 display priority `0xD4`。
`winId>=0 && param6!=0` 时，宽、高只要 remainder 非零，就分别执行
`(value / 64 + 1) * 64`、`(value / 48 + 1) * 48`；除法为 signed
trunc-to-zero。正数表现为向上取整，但负数也执行该公式，例如宽度 `-1 -> 64`、
`-65 -> 0`；0 或整倍数保持不变。常用调用 `(130,300,340,52,4,1)` 保存为
`384x96`。

| `winId` | `tileStyle` | `baseImageNo` |
|---:|---:|---:|
| 0 | 1 | 22620 |
| 1 | 1 | 22630 |
| 2 | 0 | 22620 |
| 3 | 0 | 22630 |
| 4 | 1 | 245300 |
| 5 | 0 | 245300 |
| 6 | 1 | 245310 |
| 7 | 0 | 245310 |
| 8 | 1 | 245380 |
| 9 | 0 | 245380 |

`winId<0` 的 style/base 图号保持 0，且不写 creator param6。`winId==-1` 从半宽和
半高均为 0 展开，两个 action 增量分别为 `halfWidth/10`、`halfHeight/10`；
`winId==-2` 或非负时预置完整半宽、半高从 0 展开，横向增量为 0。`winId<-2`
不进入这两个几何初始化分支。

## callback 时序

`0x5046E0` 读取 action phase，`0x50495C` 独立读取 payload frameCount；两者不可
合并。按 scene 先执行、`World::Update` 尾部再执行 `UpdateAll` 的现有顺序：

| callback 次数 | 入口 phase/frame | 本次行为 | 退出状态 |
|---:|---|---|---|
| 1..10 | `0 / 0..9` | 绘制展开框并累加半宽/半高 | `phase=0`，frame 为 `1..10` |
| 11 | `0 / 10` | 仍先绘制、累加，再切 phase | 非负 winId 到 2；-1/-2 到 1；frame=11 |
| 12 | `1` 或 `2` | phase 1 直接完成；phase 2 完成窗口绘制 | `AssignFromInt(readiness,1)` |
| 最早第 13 次 scene | readiness 已为 1 | 上层比较可观察到就绪 | — |

`IsYobiActionReady/StateEqual/StateLessOrEqual` 各自只调用一次 LegacyCryptInt 比较，
因此每次判定只解码一次。

## 整块、九宫格与 DrawGraphic3

phase 2 的分支由 `0x50470C..0x5048A2` 核证：

- `winId<4 && param6!=0`：按 64x48 整块调用既有 `DrawGraphic`，图号偏移为
  四角 `0/2/6/8`、左右 `3/5`、上下 `1/7`、中心 `4`。
- 其余路径调用 `sub_49BE00`；`winId>=4` 使用 74x54，否则 64x48；边框色
  `-1`，中心色 `0x80FFFFFF`。绘制完成后才写 readiness。

九宫格在 `0x49BF37/0x49BFA1/0x49C009/0x49C048` 先提交四角，
`0x49C0F4..0x49C2BC` 提交左右和上下边，`0x49C35F..0x49C4B7` 最后提交
中心。宽或高不足两个边框尺寸时，左右/上下分别按前半和剩余量拆分；边和中心的
末尾不足一块时只缩短 descriptor 的 `size.w/size.h`，source 起点保持对应边的原值。
descriptor 固定为 `clip=1,mixMode=4,paletteCacheIndex=0`。

`DrawGraphic3_520970` 与普通 `DrawGraphic` 的关键差异已保留：

- `imageNo<-1` 时直接作为 graphic index，图偏移为 0；`-1..999` 返回 `-2`。
- `imageNo>999` 且 `isGraphicNo` 低字节非零时先查 map index，再调用
  `GetGraphicXY`。
- 待提交记录的 `x/y` 保存调用坐标，不加图偏移；图偏移从 descriptor 的
  `pos.x/pos.y` 作 16 位回绕减法。
- descriptor 为空时写零位置/尺寸、颜色 `-1`、其余 0；非空时复制完整 16 字节。
- 先写传入 batch type，`clip==1` 再强制为 1；最终 graphic index 在
  `[0xC18,0xC21]` 时置唯一 `gSpecialGraphicPending`。
- 记录与并行 layer/cacheIndex 均复用唯一 `gLegacyPendingStorage`，计数复用
  `platform::gPendingGraphicCount`，没有建立第二条队列。

原函数对负的 signed16 队列计数会在容量比较前形成负索引；该状态在现有主循环
“每帧清零、最多递增到 4096”的有效域不可达。新端与既有 `DrawGraphic` 一致，对负值
返回 `-2`，避免 x64 容器越界；有效域行为保持一致。这是本次唯一额外安全门槛。

## 浮点相位叶函数

原 `sub_520910@[0x520910,0x520923)` 共 19 字节，依次执行
`fld dword ptr [0x1020EF0]`（`0x520910`）、`fadd dword ptr [0x54F684]`
（`0x520916`）、`fstp dword ptr [0x1020EF0]`（`0x52091C`）及 `retn`。
常量 `0x54F684` 的位模式为 `0x3E800000`，即单精度 `0.25f`；
`0x1020EF0` 位于 PE `.data` 的虚拟零填充区，初值为 0。
本地唯一 `gFloat1020EF0` 承载该地址，`Sub_520910()` 只读一次、加一次、写一次，
不归一化或重置。默认舍入下的普通有限值数值路径对应原 x87 运算；非默认控制字、
浮点异常及特殊值环境尚未验证。

该地址全部 6 个直接 xref 分别为本叶的 `0x520910/0x52091C` 和
`sub_521870` 的 `0x5218F3/0x5218FD/0x521902/0x52190D`。本叶唯一 caller 是
`0x449176`；`sub_521870` 在减去局部浮点值后调用 `sub_520930@0x521908`
归一化，并在后续后缓冲路径使用相位。

原 `sub_520930@[0x520930,0x52096B)` 共 59 字节、20 条指令、6 个基本块，
唯一直接 caller 为 `sub_521870` 的 `0x521908`。入口从参数取得 `float*`；
`0x520934/0x520936` 读取当前值并与 `dbl_54F688=64.0` 比较，
`0x52093E` 检查 x87 状态字 C0：值大于或等于 64 时，`0x520943..0x52094D`
重新读取、减去 `flt_54DD30=64.0f`、原位写回，再回到比较处。
否则 `0x52094F/0x520951` 重新读取并与 `dbl_54D5D0=0.0` 比较；
`0x520959/0x52095C` 再查 C0，值大于或等于 0 时直接返回，余下路径在
`0x52095E..0x520968` 重新读取、加 `64.0f`、原位写回并重试。本地
`Sub_520930(float*)` 保留每轮重读与写回顺序，不改写负零。

默认屏蔽浮点异常下，NaN 在两次比较中均进入加 64 路径并无限循环；
正无穷在减法路径、负无穷在加法路径无限循环；极大有限值也可能因减去 64
后数值不变而不进展。未添加 `fmod`、钳位、保护、超时或额外状态。
这两个相位叶均未接入原 caller、consumer 或后缓冲路径，主循环波纹链仍未闭合。

## 波纹后缓冲链方案裁决（未实现）

2026-09-22 用户裁决：`sub_521870` 波纹后缓冲链采用 CPU 像素缓冲，不采用 GPU
render-target Texture；后续实现按用户指定保留原版后缓冲画面快照与逐条带回拷语义，
再通过既有最终呈现边界上传。此为方案与实现目标，不是本轮新增 IDA 取证；
`sub_521870`、caller/consumer 及后缓冲链仍未实现。

后缓冲与临时缓冲的 owner（所有权）、像素格式、尺寸、映射及上传时序仍须逐项取证。
该裁决不改变 SDL3 `SDL_Renderer`、仅 32 位色范围，也不推广到其他渲染功能。

## 注册表清理与当前未闭合项

`sub_46DD40` 在 `0x46DD43/0x46DD48` 取得两张全局表。标志表
`[0xB65A80,0xB8C861)` 共 `159201=399×399` 字节；指针表
`[0xB8C864,0xC27FE8)` 共 `636804=399×399×4` 字节。`0x46DD4F` 只接受标志
精确等于 1 的项，`0x46DD55` 先将该标志清零，`0x46DD59` 才读取指针并在
`0x46DD5C` 调用空指针安全的 `Yobi::MarkDeath@0x404D00`。`0x46DD64/0x46DD65`
按列推进，`0x46DD70` 按 399 字节进入下一行，直到指针到达 `0xC27FE8`；原函数
不清指针表。

当前 Yobi runtime 以两份唯一、零初始化的平坦 `std::array` 承载上述 399×399
行优先表，并对元素总数作静态断言。标志元素保持 `std::uint8_t`，不 bool 化；
指针元素使用原生 `YobiAction*`，不保存 32 位指针位模式。该表只是非拥有索引，
action 的 owner 仍是既有 intrusive list；清理后的残留指针由已清零的标志门控，
不会在本函数中释放或改写。

本步没有公开注册/查找接口，也没有实现 `sub_46CF60`、`sub_46DE50`、
`sub_46DD90`、`sub_46DA80` 或 `InitializeClient_4576B0` 接线；这些相关路径仍是
后续边界。`YobiClearUp_404D30` 已按原顺序调用本地 `Sub_46DD40()`，完整退出/全清
路径不再含该未定义符号。

`cg::world::detail::YobiAction** Sub_446470()` 保持原 world 私有类型声明，未与本次
`cg::login::YobiAction` 合并。当前未执行编译、测试、客户端运行、lint、格式化或
任何构建产物检查。
