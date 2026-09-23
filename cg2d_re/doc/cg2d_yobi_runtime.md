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
| `sub_423640` | `[0x423640,0x4237D5)` | — / — | 更新随机屏幕偏移、幅度并将共享 Temp 裁剪回拷 Back |
| `sub_521870` | `[0x521870,0x5219B5)` | 98 / 10 | 已实现门禁、Back→共享 Temp 快照及 60 条带裁剪回拷 |
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
`Sub_520930` 已由 `Sub_521870` 按原位置调用；`Sub_520910` 的原 caller、
`DrawAllQueued` type 8 consumer 与主循环提交链仍未闭合。

## sub_423640 屏幕偏移回拷

### 原端 IDA 事实

`sub_423640@[0x423640,0x4237D5)` 以 `[0,0,640,480)` 为初始源矩形。其五个
W32 状态为升降标志 `0x59F9F4`、X/Y 偏移 `0x59F9FC/0x59FA00`、当前幅度
`0x59FA04` 和最大幅度 `0x553C58`，初值依次为 `0/0/0/0/4`。函数复用
Yobi 更新门禁 `dword_59E570/dword_59E574`：仅当门禁值为 1 且相位计数非零时，
本帧不消费 RAND，并复用旧 X/Y；其他路径先为 X、再为 Y 各重新读取一次当前幅度，
严格依次调用两次 `RAND(-A,+A)`，并按 X 后 Y 的顺序写回偏移。

`RAND@0x46FAF0` 的范围运算按 x86 W32/signed32 语义执行：`upper+1` 的 W32
结果为 0 时直接返回 0；否则以 `upper-lower+1` 的 W32 位模式作为 signed32
除数，对共享 `NextRandom_53B4C9` 的结果取余，再以 W32 加上 lower。对称下界用
无符号 `0-A` 形成，不引入 C++ signed overflow，也不对 `INT_MIN` 作有符号取负。

偏移确定后才更新幅度。升降标志精确等于 1 时，先读取最大幅度，形成当前幅度的
W32 递增值并完成 signed32 比较；随后首次写入递增值，超出时第二次写最大幅度。
其他标志值均走下降路径：
先按 W32 减 1 并写回，若 signed32 小于 0，再第二次写 0。全部 RAND、偏移和幅度
副作用完成后，渲染只取 X/Y 的低 16 位并按 S16 扩展，再同时裁剪源矩形和目标坐标。
完全不相交时不 blit；相交时尝试从共享 Temp 向 Back 回拷，且不检查底层返回值。

固定 640×480 边界下，`(-6,+6)` 得到目标 `(0,6)`、源
`[6,0,640,474)`；`(+6,-6)` 得到目标 `(6,0)`、源 `[0,6,634,480)`；
`x=±640` 或 `y=±480` 时完全不相交。主循环原有顺序保持为
`Sub_423640 → DrawAllQueued_520DD0 → CopyWorldProjectionState_468FF0 → Flip_44C560`。

### 用户批准的新端映射与当前实现

DX7 表面操作已按 2026-09-22 用户裁决映射为同步 CPU 像素复制；这属于新端替换，
不是新增 IDA 事实。`SurfaceState` 现有唯一、零初始化的共享 Back/Temp owner，二者
均为固定 `640×480` 的 `std::array<std::uint32_t>`，格式明确为自上而下、行主序的
`0x00RRGGBB`，pitch 为 640 像素/2560 字节。`BlitTempToBack` 不混合、不缩放、
不使用色键，只接受已经完全落在固定 Back/Temp 边界内的非空请求并逐行复制；非法
请求返回 false 且不修改 Temp。`Sub_423640` 已接入该共享 owner，调用后忽略 bool，
没有改动 `MainLoop`。

当前已闭合 owner、严格 blit、`sub_423640` 与波纹 `sub_521870` 本体。Back/Temp 的
真实内容生产、`DrawAllQueued` 的 case 6/7/8 触发以及 `Flip`/`Present` 转换与提交仍未
闭合；在这些生产者接入前，零初始化 owner 不代表已有可呈现画面。

## 波纹后缓冲函数、已批准替换边界与未接线项

2026-09-22 用户裁决：`sub_521870` 波纹后缓冲链采用 CPU 像素缓冲，不采用 GPU
render-target Texture；当前实现按用户指定保留原版后缓冲画面快照与逐条带回拷语义，
后续再通过既有最终呈现边界上传。该裁决只适用于此波纹链，不推广到其他渲染功能；
也不改变 SDL3 `SDL_Renderer`、仅 32 位色范围。`Sub_521870` 本体及其最窄
world helper、状态 owner、CRT 门禁依赖已经实现；caller/consumer 与最终提交链仍未接线。

同日用户另行批准：DX7 `Blt`、`BltFast` 等底层操作可直接映射为等价 SDL/CPU
行为，不要求逐项复刻 `HRESULT`、显存 caps、`DDBLTFAST_WAIT`、surface
lost/busy/restore、原失败码，也不要求复刻被替换 DX7 原生 API 边界的 COM 接口 ABI、
对象/vtable 布局及其调用约定。该批准是新端底层替换差异，不是新增 IDA 事实；ABI
豁免严格限于被 SDL/CPU 替换的 DX7 原生 API 边界，不适用于其上层函数或其他外部
ABI。上层触发时点、既有 `RAND` 消费、状态读写、`W32`/`S16` 规则、坐标与裁剪、
像素内容、共享 Back/Temp 生命周期及提交顺序仍是替换验收约束。既有共享 CPU
Back/Temp 裁决保持不变。

当前独立 IDA 只读分析核证 `sub_521870@[0x521870,0x5219B5)` 共 98 条指令：

`0x521895..0x5218A9` 门禁失败时跳到 `0x5219A9` 清强度并跳过 blit；楼层变化
在 `0x5218BE..0x5218C3` 于绘制前清强度。以下仅是通过门禁后的路径：

- `0x5218E1` 的 `BltFast` 先尝试将完整 Back 复制到共享 `gTempSurface`。
- `0x52192C..0x52198E` 以同一共享 Temp 为源，依序尝试向 Back 回拷 60 条、
  每条高 8 的条带；
  对第 `n` 条（`0<=n<60`），目标 `y=8n`，源
  `y=8n+12+trunc(strength×table[(phase+n)&63]/100)`。`table@0x58B6E0` 为
  64 项有符号表，值域 `[-12,12]`。
- 强度小于 100 时，函数末尾将强度加 2。该变化不改变本次先快照、再逐条回拷的
  顺序。

`0x5218E1` 和 `0x521981` 两处 blit 返回值均未检查，不能保证原 DX7 快照或条带实际
成功。这是原端 IDA 事实；按上述用户裁决，新端不需要仿真对应 `HRESULT` 或 surface
状态，也不得因等价 SDL/CPU 操作的后端结果新增原上层不存在的分支或状态写入。

新端 `Sub_521870` 先单次读取 `0x9A7FA8` 的私有 `LegacyCryptInt` 门禁；门禁非零或
楼层不在 `40000..41999` 与六个离散允许值内时只清强度并短路。正常路径独立再次读取
楼层，楼层变化时按缓存后强度的顺序写入，随后将 307200 个 `0x00RRGGBB` Back 像素
同步位复制到唯一共享 Temp。世界当前 X/Y、投影 reference X/Y、楼层、整数相位、
上帧 X/Y、强度及楼层缓存均由带原地址注释的唯一状态承载；投影 helper 保持 reference
X/Y 先读、输出 X 后 Y，坐标差 helper 保持旧 Y、旧 X、投影、当前 Y、当前 X、投影的
读取次序。坐标 X 差保持加宽直至乘 `0.125f` 后最终落 F32；Y 差先显式落 F32；两差
均完成计算后才按 X、Y 顺序写出，因此输出指针别名时最终仍为 Y。私有门禁由 CRT
顺序适配先于十个鼠标 `CryptInt` 构造；退出时鼠标按钮坐标严格按 `[7..0]`、再 mouseY、
mouseX 调用析构，最后才析构门禁。该门禁尚未扩展到其余 19 个 reader。

相位路径继续复用唯一 `gFloat1020EF0` 与 `Sub_520930`，在 `phase-yDelta` 的加宽中间值
后显式落回 F32，再以向零截断写整数相位。60 条带各自严格按 phase、表项、strength
顺序独立采样，使用唯一 64 项有符号近似表；乘积以 W32 低位计算，除以 100 采用
C++ signed32 向零截断。
所有条带完成后才重新读取强度，小于 100 时按 W32 加 2，因此本帧像素只使用更新前值。

`DrawAllQueued` 的 type 8 分支在 `0x52181E` 调用本函数。`SortTextures` 可选的
layer 2/type 8 与原有 layer 5/type 6、layer 6/type 7、layer 20/type 9 均属于
待提交队列；case 6/7 也可读写共享 Temp。因此波纹不能改为局部 scratch，也不能
延迟至 `Flip` 执行。主循环相关时序为
`DrawAllQueued` → `sub_468FF0@0x4572B9` → `Flip`；中间调用不可与
`sub_5208F0@0x457351` 混同。

IDA 核证共享 `gTempSurface` 的请求内容尺寸是 640×480：原端首选 `CreateSurface`，
失败后走 `MakeSurface`，后者的 3D/caps 路径可能放大物理表面尺寸。该 caps 相关物理
表面选择只保留为原端证据，新端获准不复刻，可按共享 CPU Back/Temp 的逻辑尺寸执行
等价 SDL/CPU 像素操作。

原 32bpp mask 在运行时由 Back 的 `GetPixelFormat` 取得，不能直接宣称与新端
`SdlPresenter` 的 `0x00RRGGBB` 布局相同。新端仍须明确 CPU Back/Temp 的 32 位像素
格式及到最终 `Present` 的转换边界，以保持像素内容；固定 Back/Temp owner 已按上节
落位，但内容生产、`DrawAllQueued`、`Flip` 未实现，`Present` 也尚未接入这条链。

强度为 0 时，最后一条带的源 `y` 区间为 `[484,492)`，超过首选 480 高度。原端不
检查 blit 返回值；新端不再复刻 caps 放大或未知驱动失败行为，但必须保留该源坐标
计算。当前实现采用用户批准的新端确定性 CPU 裁剪：对目标 `d=8n`、请求源顶
`s=8n+12+offset`、条带高 `h=8`、表面高 `H=480`，计算
`begin=max(0,-s,-d)`、`end=min(h,H-s,H-d)`；空区间跳过，否则立即同步调用既有
`BlitTempToBack(0,d+begin,0,s+begin,640,s+end)`。未覆盖目标保留入口 Back 像素，
不填充、不回绕、不重复边缘、不清色，也不修改 Temp。该裁剪是用户批准的新端等价映射，
不冒充原 DX7 对非法 RECT、驱动失败或 caps 放大表面的行为。

`BackupValues_5208F0` 已在 `MainLoop` 原调用点 `0x457351` 接线：严格按 current X、
current Y 顺序读取原始 F32 位模式，再按 previous X、previous Y 顺序复制到唯一
owner；函数返回缓存的 X 位模式，`MainLoop` 按原 caller 忽略该返回值。

以下仍是后续须闭合的接线边界，不再是 `Sub_521870` 本体或 DX7
caps/`HRESULT` 复刻待裁决项：

1. `DrawAllQueued` type 8 的原触发位置；case 6/7 的共享 Temp 生命周期也仍未闭合。
2. 同一 `0x9A7FA8` 门禁的其余 19 个 reader。
3. 共享 CPU Back 的真实内容生产，以及 Back/Temp 到 `Flip`/`Present` 的像素转换与提交。

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

## R6.1.3.154.D：Sub_448370 按 callback 查找存活 action

本节依据 Leibniz 的 READY 分析交接，记录本叶原函数事实与实现合同，状态为实现推进。
最终通过状态以[唯一执行进度](../plan/cg2d_full_reconstruction_progress.md)为准，
本节不预置通过结论，也不改写历史已复核段落。

### 地址与读取顺序

`Sub_448370@[0x448370,0x44839C)` 共 44 字节、17 条指令，无 callee。原接口为
cdecl，只有一个函数指针参数，返回 `YobiAction*` 或空；`0x44839B` 为 `retn`。

| 地址 | 原端行为 |
|---|---|
| `0x448370` | 读取首哨兵 `start@0x59E56C` |
| `0x448375` | 取得 callback 参数 |
| `0x448379` | 缓存尾哨兵 `end@0x59E568` |
| `0x448380` | 读取首哨兵的 `next(+0x04)`，作为首个候选 |
| `0x448383/0x448385` | 候选与缓存的 end 比较；到达尾哨兵即走返回空路径 |
| `0x448387/0x44838A` | 先比较候选的 `callback(+0x08)`；不匹配时不读取死亡标志 |
| `0x44838C/0x44838F/0x448391` | 仅 callback 匹配时读取完整 W32 `deathFlag(+0x1C)`；等于 0 即返回首个命中对象 |
| `0x448393/0x448396` | 仅未命中时读取候选的 `next(+0x04)` 并继续；命中不读取 next |
| `0x448398` | EAX 清零，形成空返回值 |
| `0x44839B` | 返回 |

遍历排除首尾两个哨兵，终点只使用入口缓存的 end，不以 `tail->next` 或 `nullptr`
作为终点。普通节点的 callback 可以为空，空 callback 参数仍可匹配，不能增加提前
拒绝空参数的分支。函数没有任何写入，也不调用 callback。

有效初始化后的空链满足 `start->next == end`，直接返回空。未初始化且首指针为空时，
原端会在 `0x448380` 非法解引用；坏链没有空指针或环保护。这些状态不等同于正常空链，
本节不声称异常域通过。

### 接口与生命周期合同

本叶在 `cg::login::detail` 下采用
`YobiAction* Sub_448370(void (*callback)(YobiAction*))`，声明与实现分别落在既有
`include/cg/YobiRuntime.hpp` 和 `src/YobiRuntime.cpp`（相对 `cg2d_re/`）。
复用现有私有 `gYobiActionStart/gYobiActionEnd` 及 `next/callback/deathFlag`，
不新增字段或 owner，不公开对象布局，不移动现有 `YobiCallback` 别名。

返回值是非拥有指针，不转移或延长 action 生命周期；所有权仍归既有 runtime/list，
延迟死亡、摘链及释放继续由既有流程负责。本叶只查找，不标死、不摘链、不分配或释放，
也不承诺返回后对象仍存活。本次仅还原原函数，不引入新设计，无需新 SPEC。

### 入向引用与未接线边界

以下仅记录 11 处入向引用及其 7 个 caller，不展开 caller 实现：

| caller | 引用地址 |
|---|---|
| `0x448290` | `0x4482CB` |
| `0x4483A0` | `0x448400` |
| `0x448810` | `0x44886B`、`0x44888D` |
| `0x448D90` | `0x448DDD`、`0x448E38` |
| `0x449700` | `0x44977C`、`0x44979E` |
| `0x449BC0` | `0x449BC6`、`0x449BD7` |
| `0x449C10` | `0x449C16` |

本叶不接入上述 caller，不代表 `0x449160/0x448290` 链闭合。
`R6.1.3.151/153` 涉及的 `0xFF02B4/0x73ED48` 及新增 Yobi 字段仍待批准，
本叶不承接或裁决这些事项。

## R6.1.3.181.D：Sub_446470 world 对象表重置证据

本节仅保存 `.181.A` 的原端本体、同源及调用证据。本体取证为 `READY`，
整项存储映射为 `NEEDS_CLARIFICATION`；未修改源码，亦不预判独立审查、
文件验证或最终实现状态。上文关于 world 私有类型尚未合并的陈述保留为
当时源码状态：现已证明原端槽内指针与 Yobi list 的 action 同源，但
新端源码仍未合并该声明。

### 范围、表边界与写序

`Sub_446470@[0x446470,0x4464C5)` 无参数、无直接 callee。
`0x446472` 取得首项 Yobi 槽地址 `0x5AC5DC`；`0x4464B2/0x4464B7`
按 `0x13C` 跨步并与 `0x5F983C` 比较，处理恰好 1000 项。
原对象表为 `[0x5AC4C0,0x5F9720)`，其中 `+0x11C` 为指向 Yobi action
的非拥有引用。函数不读取任何全局内容；两处全局地址仅作为立即数地址使用。
写入顺序如下，宽度以汇编核对为准：

| 阶段 | 指令地址 | 目标 | 写入 |
|---|---|---|---|
| 表前 | `0x446477` | `0x5AC4B4` | W32 零 |
| 表前 | `0x44647D` | `0x5F9738` | W32 零 |
| 表前 | `0x446483` | `0x5AC49C` | W32 零 |
| 每项第 1 写 | `0x44648C` | `+0x00` | W16 零 |
| 每项第 2 写 | `0x446493` | `+0x11C` | W32 零 |
| 每项第 3 写 | `0x446495` | `+0x42` | W16 零 |
| 每项第 4 写 | `0x44649C` | `+0x120` | W16 零 |
| 每项第 5 写 | `0x4464A0` | `+0x04` | W32 零 |
| 每项第 6 写 | `0x4464A6` | `+0x34` | W32 `-1` |
| 每项第 7 写 | `0x4464AC` | `+0x54` | W8 零 |
| 表后 | `0x4464BE` | `0x5AC498` | W32 零 |

反编译的 `YobiAction**` 仅是返回候选类型，不据此将对象表改作指针数组。
最终 EAX 为 `0x5F983C = 0x5F9720 + 0x11C`：这是槽地址迭代的
跨步终值，位于整个对象表之外，并非真实连续 `YobiAction*` 数组的
一过尾元素；新端不得伪造该 x86 地址常量。

### 同源与 caller

`0x441DB9` 调用 `YobiAction::new`，`0x441DB0` 入口返回所建 action；
`0x44683E/0x446846` 将其写入同一 world 对象的 `+0x11C` 槽。
`0x44631A/0x4463E6/0x4463EE` 从槽取指针、调用
`Yobi::MarkDeath`、清槽；`0x4462CC` 也从该槽返回指针。
这些生产与消费证实槽内对象与既有
[`cg::login::YobiAction`](../include/cg/YobiRuntime.hpp) 同源，
所有权仍归现有 [runtime/list](../src/YobiRuntime.cpp)。
邻近的 `0x4464D0` 会先标死 action 再清槽，`0x446470` 只清槽，
不标死、不释放，两者不能互换。

四处原端 caller 都未消费该返回值：`sub_45EBB0` 在 `0x45EBE4`
调用，随后 `0x45EBE9` 调用 `YobiClearUp`，直到 `0x45EBEE`
才由 `mov eax, gWorld__SceneState` 明确覆盖 EAX；
`World::Update@0x51C430` 的 `0x51C57C/0x51C674/0x51C6DB`
调用后分别紧接 `sub_4D3170`、`YobiClearUp`、`sub_5081A0`，
在后续调用前均不读取 EAX。不能说这些后续调用必然覆盖 EAX，
也不能因四处弃值就默认批准把新端接口改为 `void`。

### 待批共享存储 SPEC 与边界

当前仅有 [world 私有声明](../src/world/WorldUpdateDependencies.hpp)
及 [WorldUpdate 调用](../src/world/WorldUpdate.cpp)，尚无 world 对象表
及上述四个独立 W32 全局槽的新端 owner。待批建议是在 world 边界
设唯一 1000 项共享表与四槽，保留七字段写序及仅清槽行为；
Yobi action 继续由 runtime/list 拥有。须先批准完整共享布局和
返回接口（保留指针型合同或改为 `void`），不能据未知字段自造结构。
原 `+0x120` W16 紧邻 x86 的四字节指针槽，不能直接把这些偏移套在
原生 x64 指针上。

这些入口处是否仍有存活槽引用尚无证据，不擅加清理前提。
`.151/.164` 等既有门禁不因本节解除。后续静态验收需核对
1000 次七写、四个全局写、零全局内容读取、四处弃值及生产者与
标死消费者共用同一槽；本节只记录取证，不声明这些实现验收通过。

## R6.1.3.193.D：43F220 观察链清头证据

本节仅记录已完成 `.193.A` 的窄证据交接，不扩展函数范围或调用者清单。
`0x43F220` 只清 `0x5F9750` 链头，不标死对象。该槽属于独立的、
非拥有 `YobiAction*` 观察链，不是 Yobi runtime 主链的首尾哨兵；
因此清头不能等同于 `Yobi::MarkDeath`、主链摘链或释放 action。

原端观察链 payload 长 `0x30`，链接位于 payload `+0x2C`。
这些 x86 尺寸与偏移不直接规定新端 x64 表示；x64 表示及跨模块接口仍待批准，
本节不指定 owner、结构布局或实现方案。既有
[scene 文档](cg2d_scene_state.md) 已列出 `0x43F220` 位于父入口调用链；
此处不据此声称调用者已穷尽或接线已完成。
