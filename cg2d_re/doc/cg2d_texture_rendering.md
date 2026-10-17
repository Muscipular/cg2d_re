# 纹理绘制入口与底层边界

## 战斗蒙版缩放采样试用（2026-10-17）

**当前结论：**用户在预乘Alpha修正后反馈“目前效果好一些，按照目前的版本来”，确认保留线性采样配合预乘Alpha的现有实现，解除本项确认前不提交限制。以下试用过程保留为依据；该反馈仅确认当前画面效果，不外推为所有资源、分辨率和设备的完整验收。

用户提供缩放前后蒙版与阴影截图，确认对照图为未缩放状态，并同意尝试仅对缩放战斗蒙版启用线性采样；确认效果之前不提交。

DrawBattleBackgroundScaled使用独立战斗背景批次，仍取完整源图铺满画布，仅当源／目标尺寸不同时向GPU提交线性采样。普通绘制默认最近邻，每次提交显式设置本次采样模式，防止共享纹理复用时影响人物、地图、文字或登录背景。普通战斗快照与renderer重建均走同一入口，保持图号、层序、调色板、颜色和Alpha混合；不裁战斗蒙版透明区域，不平滑整帧，不修改阴影资源或索引图上传规则。

预期减轻放大网点的硬边及不均匀感，代价是蒙版略柔和；线性采样并不消除素材网点，也不保证恢复未缩放观感。先静态检查接线与状态隔离，不编译、测试或运行，实际蒙版颜色、颗粒及阴影叠加效果等待用户试用确认；本次不暂存、不提交。

### 试用反馈后的Alpha过滤修正

用户反馈效果不好，截图仍有网点且蒙版偏厚。源码确认上一版将普通ARGB纹理直接线性过滤后按SRC_ALPHA混合：透明黑与有色像素的RGB先插值一次，混合时又乘插值后的Alpha，会使边缘额外压暗。例如不透明颜色C与透明黑各占一半时，原方案源颜色贡献为C/4，正确覆盖率贡献应为C/2。这个确定的计算差异不代表截图全部视觉差异已定位。

在已同意的战斗蒙版线性采样范围内修正：仅linearSampling且普通Alpha混合时上传预乘Alpha像素，采用SDL_BLENDMODE_BLEND_PREMULTIPLIED；顶点颜色调制的RGB同时乘全局Alpha。CPU原始像素不变，既有GPU缓存记录当前预乘状态，模式切换强制完整重传，内容更新沿现有脏矩形转换上传。其他最近邻／加法／不混合绘制仍用原像素和混合方式。使用本地SDL_PremultiplyAlpha，按既有sRGB数值空间计算，不改色彩空间；临时缓冲只在实际上传时分配，失败沿既有上传失败路径返回。

这一步只纠正线性插值与Alpha的配合，不新增网点识别、模糊或重建算法，不声称消除网点；修正后的颜色与网点观感仍待试用。继续不编译、不运行、不暂存、不提交。

> 后续接线：完整 `DrawAllQueued_520DD0` 已接三条纹理 caller、
> FillDirtyArea 及文本前后的 native Begin/End；真实设备状态参与四顶点绘制。
> 见[父队列实施 SPEC](cg2d_sdl_presenter.md)。旧阶段缺口仅为当时快照，
> 不能再据此称父函数无本体；当前仍未构建、测试或运行。

2026-09-27，原 `ida-17524 / D:\SOFTSTAR\cg2d.exe.i64` 只读核
`44D520–44DA8F`、`44DA90–44E0B1` 全部指令及反编译，
并核 `5211BE/521393/52169F` 三处实参和返回消费。
以下为工程委托下先行 SPEC，不把底层声明当作像素实现。

## 类型与返回

- `44D520`：x/y 保留完整 S32 实参；交集/位移取低 S16，负值门禁
  按分支保原宽度（BUX01 原数字指令纠正早期 S16 声明）。矩形为四项 S32 按值传递；
  surface 是非拥有 `TextureSurface*`，图号 S32、调色板 S16、
  mixMode 取低 W8、color 保留 W32。
- `44DA90`：x/y 仅消费低 S16；surface、图号、调色板、mixMode/color 同前；
  矩形以指针同步只读借用，两个比例为 F32，不回写 caller。
- 两函数早拒绝及无 3D 时 bit3 置位均返回 0；实际绘制透传完整
  W32 状态，不把返回固定成成功。三个 caller 后续比较
  `0x887601AE`，其表面忙状态消费不能由后端任意非零值替代。
- x64 私有矩形沿已有变换缓存使用 `array<int32_t,4>`，
  顺序 left/top/right/bottom；完整指针不编码成 W32。
  不引入 Win32 RECT 到上层业务，也不改变原图号与调色板宽度。

## 44D520 不缩放路径

1. 保存低 S16 的 lowX/lowY，按序拒绝 lowX>=renderer逻辑宽、
   right+lowX-left<=0、lowY>=renderer逻辑高、bottom+lowY-top<=0。
   +94/+98 在各短路阶段取样；640/480 只是初始化值。
   先检查交集，再按 bit0/bit1 裁剪，不额外添加空矩形或源面范围校验。
2. 横向正向以完整 S32 x<0 判断，left-=lowX、目标x=0；
   翻转以低 S16 lowX<0 判断，right+=lowX、目标x=0。
   两分支负值裁剪之后各重新读逻辑宽，再按原式裁剪右/左边。
   目标x平时为lowX；不得在caller先窄化完整实参。
3. 纵向两分支都以完整 S32 y<0 判断，用lowY改top或bottom，
   目标y=0；否则目标y=lowY。负值裁剪之后分别重新读逻辑高，
   正向再改bottom、翻转再改top。原门禁宽度与横向不同，不作对称化。
   这是整数裁剪，不借缩放入口替代。
4. `44D6DB` 查询 `44B610`，真才读取 renderer `+BC`；
   两者均非零才构造纹理顶点。目标四角为裁剪后的 x/y 与原矩形
   宽高相加，z=0、rhw=1、color 四点一致。
5. u 起终点：未翻转为 `(left+bias)/ww`、`right/ww`；
   翻转为 `(right-1+bias)/ww`、`(left-1)/ww`。
   v 同理。ww/hh 来自 renderer `+B8/+BA` 的 S16，
   **不能拿节点面宽高或请求 64/48 直接替代**。
6. 无该纹理路径时，bit3 置位不绘制；bit0/1 都零则原 BltFast
   按源色键写 Back。否则先构造目标矩形；场景 10 以目标矩形为
   变换缓存键，其余场景以源矩形为键，调用真实 `44CD80` 后
   把返回面按原源/目标矩形 Blt。不能绕过有地址身份的缓存。

## 44DA90 缩放路径

1. 依 top、left、right、bottom 次序乘各自比例，每项先 F64
   运算再存回 F32；低 S16 x/y 转 F32。先保存逻辑宽并拒绝x>=宽，
   再比较横向宽度+x，通过后才保存逻辑高、比较y>=高与纵向高度+y。
   四裁剪使用这两个保存尺寸。两次浮点检查对应
   `fnstsw/test ah,41h`，有序严格大于零才继续，unordered 也拒绝。
2. clipping 的中间算式按原 x87 PC53 用 F64，只有原 `fstp`
   到 F32 局部的位置才收窄；禁止合并或代数重排。翻转横向超界
   是 `left=(extent-width)+left`；**翻转纵向超界是
   `top=top-(extent-height)`**，指令 `44DC5D–44DC65` 如此，
   不按对称性修正成加法。
3. 先 bottom/scaleY、right/scaleX、top/scaleY、left/scaleX
   各自 F64 除法，按既有 CRT PC53/`__ftol` 合同转 S64、
   取低 W32，形成源矩形。RPLAN.24 下不额外处理越界或数学异常。
4. 源矩形形成后重新读取+94，当前x+right-left小于该逻辑宽才给
   right的F64临时值加1；随后重新读取+98，当前y+bottom-top小于
   该逻辑高才给bottom临时值加1。不沿用初始保存尺寸或固定640/480。
   随后依 bottom、right、top、left 顺序转换形成目标矩形，
   不一律补 1，不把该 F64 临时值先收窄 F32。
5. 同样查询 `44B610` 和 `+BC`。四顶点来自转换后的目标矩形；
   u/v 使用转换后的源矩形。翻转时起点取 right/bottom、
   终点取 left/top，**不做不缩放入口的 -1**。
6. 非纹理分支 bit3 置位返回 0；无翻转直接 Blt；
   有翻转依场景选同一变换缓存、再 Blt。不丢失缩放目标矩形。

## 顶点与混合

原四顶点每项为 x/y/z/rhw 四个 F32、W32 color、u/v 两个 F32，
按左上、右上、右下、左下排列；原 FVF=0x144、primitive=6、
vertexCount=4、flags=0。新端 render 私有值类型按相同字段承接。
`54DD80` 字节 `CD CC 4C 3D` 为精确 F32 bias，
先扩为 F64 参与除法，再将 u/v 存为 F32；
不把十进制近似 `0.05` 当作原双精度常量。

两入口末段相同：

| 条件 | SRCBLEND | DESTBLEND |
|---|---:|---:|
| mixMode bit3 置位 | 2 | 2 |
| 其他且 color 高 W8 为 FF | 2 | 1 |
| 其他 | 5 | 6 |

原顺序为写 source blend、写 destination blend、按共享 dwStage
绑定 surface、提交四顶点；前三次结果不作门禁，透传最后绘制状态。
不只处理 alpha 而省掉 color 的 RGB 分量，也不把加法路径按普通透明混合。
实际纹理过滤、寻址、色键、混合及像素舍入必须在底层完成，不能仅由
本节顶点数值宣称像素一致。

## 3D 查询的纠正

`44B618` 实际读取 renderer `+9C`。
`44BD3E–44BD4D` 把这个槽地址交 QueryInterface；
`44BE5A–44BE73` 则从 `+9C` 调 CreateDevice，将输出交 `+A0`。
因此 `44B610` 检查的是 **IDirect3D7 接口，不是 IDirect3DDevice7**。
名称 `HasLegacy3DDevice_44B610` 为现存接口名暂保留，
注释改正，不以 mode>1、device 非空或 SDL renderer 非空替代。
原初始化失败清理先释放/清 `+A0`，再释放/清 `+9C`，两者不能合槽。
初始化 `44BCA6/44BCAC/44BD33` 对模式低 W8 用有符号跳转，
不是无符号模式比较；现有 U8 存储在未来该消费者须按 S8 解释。

## 实施决定与剩余边界

两个完整上层入口放在 startup 的 `LegacyTextureDrawing.cpp`，
沿用该目标的 `/fp:strict` 与已有 S64 截断方式。私有头只向
未来队列消费者和后端公开矩形、顶点、类型化 surface 及必要接口。
不建立新的全局 renderer 或 texture owner。

共享查询 `GetLegacyTextureState` 必须借用现有 renderer owner 的
`+BC/+B8/+BA` 对应字段，不能返回私有快照或固定能力；
原 `+94/+98` 的逻辑尺寸沿已批准的固定 640×480 CPU Back。
native 绘制段收敛为两个有明确合同的底层调用：

- `SubmitLegacyTextureFan`：执行上述 blend→bind→draw，
  同步消费四顶点，不持有栈地址，返回真实提交状态。
- `BlitTextureToBack`：原源色键、无额外颜色调制的 Back 写入；
  源/目标宽高相等时对应 BltFast，其余对应 Blt 的缩放。

**本步仅完整实现两个上层函数及相同顶点/混合准备段。**
当时上述底层两函数、共享查询以及既有 `44B610` 仍只声明，
不写空函数、不返回固定成功、不把缺失 3D 路径改走普通 Blt。
当前 `DrawAllQueued` 尚无本体，故本步不伪称三个父调用已接通。
RendererInit、native 模式承载、像素提交和整链验收仍须继续。
不构建、不新增或执行测试、不运行客户端、不用 subAgent。

## 当前落实

[两个完整入口](../src/render/LegacyTextureDrawing.cpp)及
[私有接口](../src/render/LegacyTextureDrawing.hpp)已落位，
两条翻转回退路径调用现有 `TransformTextureSurface_44CD80`；
其中色键 Blt、共享查询及四顶点 native 提交后续已按各节落位；
设备创建父调用与 +9C 接口状态随后按[Init SPEC](cg2d_sdl_presenter.md)
接通，没有以占位实现补齐；最终像素与父绘制仍未验收。
本线程逐支核对整数/F32 裁剪、八次 S64 转换的顺序、两个入口不同的
翻转 UV、三组混合值与真实状态返回。CMake startup 登记与
`/fp:strict`、bias 位模式、十文件严格 UTF-8 无 BOM/LF、
相对文件链接及限定差异静态检查通过；不代表编译、链接或像素验收。

## CPU 色键 Blt 实施 SPEC

2026-09-27，续 `9c69110`。原函数控制流沿前节证据；
本步实施共同的 `BlitTextureToBack` native 替换和必要表面生命周期，
并在唯一 renderer owner 落位已证三字段的只读查询；
不以此补造纹理设备或四顶点混合能力。
按工程委托选已有 SDL3 的软件 surface blit，不另写拉伸/色键算法，
不引入、下载或升级依赖，最终 SDL_Renderer 自动 driver 策略不变。

### 依赖与生命周期

本机 `third_party/sdl/SDL3-3.4.8/include/SDL3/SDL_surface.h`
明确给出 CreateSurfaceFrom 的零复制借用和先销毁 wrapper 再释放像素要求，
Unchecked/UncheckedScaled 的已裁剪矩形前提，及 NONE 混合下的源色键复制。
实现保留上层已完成的裁剪，不再次裁剪、修正异常矩形或添加源边界门禁；
越界/数学异常仍按 RPLAN.24，不据 low-level blit 宣称异常输入通过。

- 节点面及变换面仍由 TextureSurfaceOwner 唯一持有原 `uint32_t[]`，
  新增随各面存活的 SDL_Surface 包装，只借用该数组。每次 Create 在
  原尺寸像素区成功后建立 XRGB8888 包装，pitch=width×4，
  设置 `SDL_BLENDMODE_NONE` 和该面的源色键；全部成功才链接 owner。
  包装或设置失败释放此次对象，不改变已有节点前缀/变换分配失败回退。
- CpuBackStorage 仍只有一块原大小像素存储，包括既有额外尾像素；
  SDL 包装仅暴露 640×480，pitch=2560，不把尾像素算入可见画面。
  创建失败保留空 Back，成功后清面、手工像素、快照、Blt 和 Flip
  均操作同一存储。后续按[文本 DIB SPEC](cg2d_sdl_presenter.md)
  将 Back 分配承载换为匿名 mapping，使 CPU、SDL 和 GDI 借用
  同一后备内存；逻辑容量不变、不复制画面。释放 DC 后先销毁
  SDL 包装及 DIB，再解除像素映射，不另建一份 Back。
- SDL_Surface 只出现在 render 私有头/实现；内部借用 getter
  返回原生非拥有 `SDL_Surface*`，不能跨 Back 销毁/重建长期缓存。
  公开 SurfaceState 接口不增加 SDL 类型或 mutable 像素视图。
- core 目标的既有 render 源文件使用已有 `SDL3::SDL3`，
  增加该目标的私有链接依赖即可；不更改第三方文件或构建配置版本。
- `LegacyTextureState` 直接成为现有 RendererResources 的值成员，
  原 GPTR 零初始化对应三个字段的初值零；GetLegacyTextureState
  返回该成员的 const 引用，不保存第二份状态，不根据请求模式计算能力，
  不补空 renderer 时的默认对象。RendererInit 对 +B8/+BA/+BC 的
  后续生产仍缺，本步不能让三个字段的零初值冒充初始化已完成。

### 绘制与状态映射

1. 以传入的四项 S32 形成 x/y/width/height，不转换地址或截断指针。
   源目标尺寸都相等时用 `SDL_BlitSurfaceUnchecked`，
   否则用 `SDL_BlitSurfaceUncheckedScaled(..., SDL_SCALEMODE_NEAREST)`。
   nearest 是本次获委托的 CPU 拉伸选择，不新增滤镜或把缩放忽略；
   本步没有运行对比，不能宣称不同 DX 驱动的取样舍入逐像素一致。
2. 全部同步写入同一 Back；不清屏、不改变描述符、不再调制颜色、
   不执行 alpha/additive，也不代替另一条四顶点路径。
3. 真实 SDL 源或目标的 `SDL_SURFACE_LOCKED` 标志置位时返回
   W32 `0x887601AE`；包装缺失或其他 SDL 失败返回通用失败
   `0x80004005`；成功返回 0。不把所有失败当作表面忙，
   不以 flag/模式常量模拟忙状态，不新增等待或隐式解锁。
4. 错误数值已静态核本机 Windows SDK 10.0.26100.0 的 `um/ddraw.h`：
   `_FACDD=0x876`，`DDERR_SURFACEBUSY=MAKE_DDHRESULT(430)`；
   `DDERR_SURFACELOST=MAKE_DDHRESULT(450)` 是另一值 `0x887601C2`。
   `DDERR_GENERIC=E_FAIL`。纠正前节旧“丢失状态”措辞，
   不改 caller 的数值比较或把 SDL 通用错误伪装为原特定 HRESULT。

验收限于源码和接口静态核对：表面/数组身份、失败发布/释放、
两个 blit 分支、色键/混合设置、三类返回及私有 SDL 边界。
不构建、不新增或执行测试、不运行客户端；SDL 像素结果及最终画面仍未验收。

### 本步落实

`BlitTextureToBack` 已在 GraphicDxCache 定义，两个原上层入口的现有
调用直接落到该实现；SurfaceState 私有 getter 借用同一 Back 包装，
RendererResources 查询借用唯一纹理状态成员。
节点及变换面在包装/设置成功后才加入原 owner 链，失败仍让既有
BuildSurfaceCache/TransformTextureSurface 处理其各自回退；
原 reset/invalidate 不销毁或另建包装。

十文件限定差异、接口/既有 CMake 登记与 SDL 私有依赖、Back 全部
读写点迁移、两包装成员析构顺序、SDK 状态码、严格 UTF-8 无 BOM/LF
及文档链接静态检查通过。未编译/测试/运行，未调用 IDA；
不以此结论代替最近邻取样结果、异步锁定情形或完整生命周期的运行验收。

## 纹理实际尺寸与 MakeSurface SPEC

2026-09-27，续 `6dc6fa7`，原 `ida-17524` 只读核
`MakeSurface@[44C6D0,44C8A5)` 全部指令，以及 `44C8B0`、
`450400/44FF40/450190` 反编译和变换面创建调用。
此前 CPU factory 直接采用请求 S16 尺寸，只能代表无 3D 的尺寸路径；
启用模式前必须补上以下区别，不能用 64×48 推断实际 pitch。

### 原端合同

1. 两个尺寸参数实际取低 S16，`44C6ED` 查询 3D 接口；
   非零时各加 2，再取低 S16、符号扩展，写入 DDS 的 W32 宽高。
   无 3D 时沿原请求的符号扩展结果。
2. `44C76A` 再查接口，非零才处理原 renderer `+B4` 的能力：
   bit1 为 1 时，宽高分别从 W32 1 开始倍增，按**无符号**
   比较达到加 2 后的目标；bit5 为 1 时，将较小的一边改为较大边。
   这不是 IDA 自动标签所说的 surface ALPHA/FRONTBUFFER 标志，
   也不是把块尺寸常量改大。
3. 该接口路径在 `44C849–44C853` 检查原 renderer `+68`
   pixel-format bit count，零则返回空。无接口路径不做此检查。
   原两个中间 `44B610` 查询只用于 native DDS flags/caps；
   新 CPU 后端不保留已替换的 COM 描述符和显存类别元数据。
4. 创建失败返回空；成功后设源色键为第三 W32 参数，
   原 SetColorKey 返回不影响 surface 返回。
   新端沿上一节已选的 SDL wrapper 创建/设置失败合同，不冒称
   SDL 与原 COM 错误点或 HRESULT 逐项相同。
5. `44D09B` 的变换面也调 MakeSurface，使用源面的实际宽高低 S16
   作为新请求。因此有接口时可能再次加 2；不得为避免再次扩展
   而直接绕过 factory。变换键仍为原 `TextureSurface*`，
   像素消费者用各自实际 width 推进，不改变源矩形原 right/bottom 循环合同。
6. `44FF40/450190` 的 32 位上传从目标 `lpSurface` 起始处写入，
   行距来自实际 Lock pitch，没有因加 2 而整体平移到 `(1,1)`。
   当前上传代码已按 surface width 前进，不增加边缘复制或偏移。

### 工程决定

- 新私有 `CreateTextureSurface_44C6D0` 承接实际尺寸、像素格式门禁、
  源色键与真实 CPU 分配；第四个原 memory caps 参数保留但未使用，
  已批准 CPU 后端不区分显存/系统内存。
- `LegacyTextureState` 的唯一 renderer 成员增加 `textureCaps@+B4`
  和 `pixelBitCount@+68`，保持 W32；原零初值不变。
  现有 `PublishRendererPixelFormat32_44BB1E` 在掩码发布前写 32，
  将来初始化仍须生产真实能力。不能为通过尺寸分支固定 has3D=false。
- TextureSurface 的 width/height 改为实际 S32 尺寸；
  此处由原 DDS 的 W32 宽高及 SDL int 尺寸证明，并非地址拓宽的推广。
  请求参数仍为 S16，块坐标、图号、调色板和节点布局均不扩大。
- 先接 BuildSurfaceCache 两个节点创建点及变换创建点；
  两个节点调用继续保留 `0x4000/0x800` 实参、原计数/发布/失败回退。
  变换创建保留 `44D08C` 的第四参数 `0x4000`，不误作节点回退类别。
  AutoMap/Temp 的首创建 `44C8B0` 不加 2；Temp 失败回退、
  初始化探测和其他原调用者仍须后续接对应 factory，不能本步统一记完成。
- 不加溢出、负尺寸或循环保护；底层既有分配适配器继续返回真实失败，
  RPLAN.24 的异常域不专项模拟或声称通过。

### 四顶点实现的后续条件

本次查阅 SDL 官方 `release-3.4.8` 的
`src/render/software/SDL_render_sw.c`：`SW_QueueGeometry` 先把
`uv*textureSize` 和目标坐标转为整数；`SDL_triangle.c`
按像素中点计算插值。直接把前节保留的 F32 顶点交给该软件路径，
会提前丢掉原 bias 的小数，不能仅因 SDL_RenderGeometry 接口存在就接上。
固定版本源码入口：

```text
https://raw.githubusercontent.com/libsdl-org/SDL/release-3.4.8/src/render/software/SDL_render_sw.c
https://raw.githubusercontent.com/libsdl-org/SDL/release-3.4.8/src/render/software/SDL_triangle.c
```

需继续落实保留采样坐标精度的 native 替换及原纹理 stage 默认值证据，
不先放行模式、强降模式 1 或采用已知丢精度的路径。
本步不编译、不测试、不运行客户端，不用 subAgent。

### 尺寸链落实

`CreateTextureSurface_44C6D0` 已在原 GraphicDxCache 源定义，
两个节点创建点和变换面创建点已接入；原底层 owner 只有该入口调用。
实际宽高使用 S32、SDL pitch 按实际宽计算，32 位上传仍从数组首项起笔。
GetLegacyTextureState 借用同一 renderer 成员；格式发布写入 bit count，
没有补造 textureCaps 的非零值或 3D 接口实例。

本线程按指令核对无接口、有接口、两能力位及格式为零的分支，
核对节点类别 `0x4000/0x800`、变换类别 `0x4000` 和失败回退。
十文件限定差异、接口/三处调用/既有源登记、SDK 能力位、
严格 UTF-8 无 BOM/LF 与文档文件链接静态检查通过；
没有配置、编译、测试或运行，不把其他 factory caller 或四顶点路径记为完成。

## AutoMap/Temp 创建与实际行距 SPEC

2026-09-27，续 `4bbef9e`，原会话只读核
`5231A0–523254`、`466C95–466CAC`、`5217FF–521816`、
`51D68A–51D6A7` 和 `51D750–51D76A` 的指令。

### 创建与拥有关系

- AutoMap：`44C8B0(640,480,0,0x800)`，返回先写独立槽，零才失败提示。
- Temp 首次：`44C8B0(640,480,0,0x4000)`；零也先写 Temp 槽。
  仅失败时再次调用 **44C6D0** `(640,480,0,0x800)`，
  再覆盖同槽。不能保持此前两次相同 CreateCpuTempSurface 的调用。
- `44C8B0` 对有/无 3D 两路只改变被替换的 native 描述符/内存类别，
  请求尺寸都按 S16 原样使用，不加 2。新私有
  `CreatePlainTextureSurface_44C8B0` 复用真实 CPU/SDL 分配；
  memory caps 参数保留，不在 CPU 后端模拟显存。
- 为避免复制 MakeSurface 规则或为回退再建一套缓存，AutoMap/Temp
  改为 SurfaceState 中唯一的非拥有 `TextureSurface*` 槽，
  和节点/变换面使用同一实际表面类型及分配登记 owner。
  SurfaceState 仍决定两槽的创建、发布与释放；不把它们加入节点数组，
  不影响节点 count，也不改变它们与节点的不同生命周期。
- 原 CpuCacheOwner 的第二套固定 640×480 数组/分配链移除，
  实际像素仍各只有一份。覆盖旧槽不主动释放旧对象，
  保留现有分配登记处理残留的规则；销毁先释放/清 Temp，
  再释放/清 AutoMap。对 Back 的 owner、额外尾像素和最终提交无改动。

### 拷贝语义

上述四个 Back→Temp 入口均调用 vtable `+1C`，flags=`0x10`，
源矩形空、目标 `(0,0)`。这是等待型 BltFast，不使用色键或拉伸；
Temp 回退面变大时只覆盖左上 640×480，其他像素不补写。
现有 Temp→Back 窄接口也保持既有逻辑矩形、无色键/无混合语义，
不因实际面变大擅自拓宽其上层调用范围。

采用同一 SDL_Surface 的真实 pitch 完成两向复制，不再把 Temp 行距
写死为 640。对 Temp 作源时，它的 factory 已设置色键，但这些
操作的原 flags 不启用色键；因此在同步软件 blit 范围内暂禁源色键，
复制成功或失败后都调用恢复原键值/启用状态；恢复失败也返回 false，
不宣称该次状态已恢复。
不因像素为黑色而跳过复制，不改混合模式，也不长期创建第二个包装。
仅在真实锁定、查询/设置或 blit 失败时返回 false；
调用者既有忽略返回的行为不变。SDL 私有借用 getter 不进入公开头。

首创建与回退用独立的 CreateCpuTempSurface /
CreateCpuTempSurfaceFallback 入口，原 BuildSurfaceCache 完整父流程
沿原位置接入。原未使用的 BuildCpuSurfaceBuffers 辅助接口保持
其已有首创建行为，不将它误作原完整父函数。

本步不编译、不测试、不运行客户端；只静态核对三个创建类别、
两槽发布/释放、无色键复制、真实行距与失败返回。
四顶点、模式/接口生产和初始化探测仍不计完成。

### 缓存面接线落实

`44C8B0` 对应普通 factory、同一登记 owner 的释放与 SDL 借用接口
已在 GraphicDxCache 定义；SurfaceState 两槽只持有原生 T*。
BuildSurfaceCache 的失败分支已改调独立 Temp fallback，不再重复首创建。
两个快照接口复用私有无色键复制段，传入相同源/目标尺寸；
不存在 Temp 固定数组或固定 640 源行距的残留。
原销毁 `44C443→44C446→44C458→44C45B` 的释放/清槽顺序已核对。

九文件限定差异、四个接口与源内接线、SDL 函数声明、
原等待/无色键 flags、唯一槽及旧 owner 移除、严格 UTF-8 无 BOM/LF
和文档文件链接静态检查通过。没有编译、测试或运行。
外部旧纹理 stage 文档读取遇 TLS 失败，未修改 TLS 验证设置；
不据检索摘要冻结未核实的原默认状态，四顶点链继续保留缺口。

## 纹理绑定生命周期 SPEC

2026-09-27，按工程决策委托，先补绑定引用的资源基础；
本节不提前生产设备、不固定 HasLegacy3DDevice，不把绑定工具计为绘制完成。

### 证据与边界

- 原 `44D95A–44D964` 读取共享 dwStage 后 SetTexture，
  `44D96A–44D988` 随后 DrawPrimitive；绑定不是仅存于该调用栈的参数。
- 原销毁先释放节点，再于 `44C443/44C458` 释放 Temp/AutoMap，
  `44C474→44C47D` 才释放/清设备，随后 `44C495→44C49E`
  释放/清接口。因此新端不能让节点释放无条件删除仍被绑定的像素。
- 初始化 `44C284` SetTexture、`44C2A3` GetTexture 后，
  `44C2B8/44C2DC` 存在返回接口释放；读取接口和同步借用不是同一合同。
  `44C2EE` 的 dwStage 来自探测结果，不能仅因当前推荐后端采用单纹理
  就将原共享 stage 的生产与读取删除。
- 辅助参考为 Wine 自身 `wine-11.0` 实现：
  `dlls/ddraw/device.c` 的 `d3d_device7_GetTexture` 增加返回接口引用，
  `d3d_device7_SetTexture` 转交状态块；
  `dlls/wined3d/stateblock.c` 的 `wined3d_stateblock_set_texture`
  先增加新纹理的内部引用、再减少旧引用。只读取得源码，未安装或引入依赖。
  这是兼容实现的交叉参考，不将其内部计数或返回值当作 cg2d 原驱动证据。
  固定来源：
  `https://raw.githubusercontent.com/wine-mirror/wine/wine-11.0/dlls/ddraw/device.c`
  和
  `https://raw.githubusercontent.com/wine-mirror/wine/wine-11.0/dlls/wined3d/stateblock.c`。

### 实施裁决

1. 仍只有 GraphicDxCache 的 TextureSurfaceOwner 登记分配，
   不建立共享指针控制块、第二份像素或按地址的引用表。
   每个 TextureSurface 增加私有 U32 内部引用数，创建完成时为 1；
   此数是新端生命周期实现，不是原 COM 可观察计数。
2. 现有节点、变换缓存、Temp/AutoMap 的 `T*` 槽和键不变，
   不因引用数加入而改为智能指针或扩大非地址字段。
   原分配/释放流程仍消费创建时的那份引用；
   释放仅在最后一份引用消失时摘登记链并析构 SDL 包装及像素。
3. render 私有 `RetainTextureSurface` 返回同一地址并增加引用；
   null 返回 null。既有 `ReleaseTextureSurface` 配对减少引用，
   null 无操作；SDL getter 始终借用，不增加引用。
4. 私有 `TextureSurfaceBinding` 负责一份独立持有：
   Bind 先保留新面，再释放旧面，最后写槽；重复绑定同一面安全，
   绑定 null 解绑定，析构释放最后绑定。禁止隐式复制/移动，
   Get 仅借用；需要 GetTexture 式持有时显式 Retain(Get())。
   该类型不管理缓存、像素格式、混合状态或设备存在性。
5. 不增加全局绑定对象，不在无设备时伪造绑定；后续真实设备 owner
   承接此 RAII 成员，按原设备释放时点销毁。
   本步只完成引用与绑定叶，设备创建、共享 stage、采样和绘制仍未接线。
6. 不引入线程同步、计数溢出处理或异常输入门禁。
   现有渲染线程域与 RPLAN.24 不变；登记 owner 的进程结束兜底清理
   不是设备正常销毁的替代，也不作为原端故障等价证据。

静态检查限于新增引用路径、同面换绑顺序、现有释放调用、
布局未变、UTF-8/LF 和限定差异；不构建、不新增或运行测试。

### 引用叶落实

GraphicDxCache 的创建、节点释放、变换缓存清理和两缓存面释放
已经使用同一内部计数；失败创建仍直接清理未发布对象。
TextureSurfaceBinding 的保留、借用、换绑和析构均有定义，
尚无设备实例持有它，也未改变 RendererDestroy 的现存调用。
因此不能把本步记为设备绑定接线或悬空路径的运行验收。

## CPU 四顶点设备与提交 SPEC

2026-09-27，继引用叶后按工程委托实施真实 CPU native 提交。
范围仍为两个已证入口产生的轴对齐矩形、相同顶点颜色、
z=0/rhw=1、单套 UV；不扩展成任意三角形或通用 3D 引擎。

### 补充证据

- 原 Init 的 `44C066/44C088/44C0A9` 分别写 stage 0 的
  ALPHAOP=MODULATE、ALPHAARG1=TEXTURE、ALPHAARG2=DIFFUSE。
  已核 47 个直接引用 renderer 的函数，未见另一个
  SetTextureStageState 调用；此覆盖不宣称排除了所有间接别名。
- 微软 `DirectX-Specs/d3d/archive/D3D11_3_FunctionalSpec.htm`
  明确 D3D9 及更早版本采用整数像素中心。
  MicrosoftDocs 的 `direct3d9/d3dtexturestagestatetype.md`
  给出 stage 0 RGB 默认 TEXTURE×CURRENT、后续 stage 默认禁用；
  这是文档的版本边界，不伪称它直接描述某张原 DX7 显卡。
- Wine `wine-11.0` 的 `wined3d/stateblock.c`
  `init_default_texture_state/init_default_sampler_states` 与上述
  RGB 默认值一致，采样默认 POINT、寻址 WRAP、无 mip；
  ddraw/device.c 的 DX7 stage 映射使用这些颜色/采样状态。
  按两组资料和原端只写 alpha 的证据，采用此 native 固定管线配置。
  不将兼容实现的内部精度当原驱动逐像素结果。
- SDL `release-3.4.8` 的 `SDL_surface.c` 实际支持
  NONE、BLEND 和 ADD_PREMULTIPLIED；`SDL_blit_slow.c`
  在源色键判断后做 RGB/alpha 调制，再进行混合。
  加法必须使用 ADD_PREMULTIPLIED，普通 ADD 会额外乘源 alpha。
- 原 `dwStage@62A648` 只有 `44C2EE` 已见写入，以及
  `44CBCE/44D95A/44DFBA` 三处读取。其所在 PE `.data`
  RVA=153000、VirtualSize=B34B08、RawSize=40000，
  槽相对节偏移 D7648 位于零填充尾部。原输入文件只读核节头，
  没有重新加载/保存 IDA；不能把 IDA 未加载字节返回的 FF 当初值。
  新端使用零初值共享 U32，销毁不擅自重置，后续 Init 按原位写入。

固定资料入口（均只读，不下载依赖）：

- `https://raw.githubusercontent.com/microsoft/DirectX-Specs/master/d3d/archive/D3D11_3_FunctionalSpec.htm`
- `https://raw.githubusercontent.com/MicrosoftDocs/win32/docs/desktop-src/direct3d9/d3dtexturestagestatetype.md`
- `https://raw.githubusercontent.com/wine-mirror/wine/wine-11.0/dlls/wined3d/stateblock.c`
- `https://raw.githubusercontent.com/libsdl-org/SDL/release-3.4.8/src/video/SDL_surface.c`
- `https://raw.githubusercontent.com/libsdl-org/SDL/release-3.4.8/src/video/SDL_blit_slow.c`

### 存储与调用合同

1. `LegacyTextureDevice` 为 render 私有真实资源对象，由已有
   RendererResources 独占持有；创建分配 640×1 XRGB8888 scratch，
   借用唯一 Back，不新增整帧缓冲。分配失败不发布设备。
   原 +9C 接口仍独立待实现，HasLegacy3DDevice 不改为设备存在性。
2. 设备拥有八个 RAII 绑定槽，对应原 stage 范围，保留独立引用身份。
   绑定/查询检查 native stage 范围；查询返回同一面的一份持有引用。
   stage 0 参与上述固定管线，其他 stage 默认禁用而非覆盖 stage 0。
   不生成全局影子绑定，也不把无纹理绘制固定返回成功。
3. Submit 依次写 source blend、destination blend，按共享 stage
   换绑，再执行绘制；绑定失败不成为提前返回门禁，绘制读取仍保留的
   实际 stage 0 绑定。后续绘制失败不回滚混合或绑定。
4. 正常释放仍是节点→Temp/AutoMap→设备→接口→palette→Back。
   本步将实际设备释放接至现有 Destroy 对应位置，释放全部绑定和
   scratch，再允许 Back 析构；接口创建/释放及父 Init 仍未完成。

### 采样与混合

1. 四角的覆盖域取各轴两端 min/max，支持反向轴；按整数像素中心
   做左上包含、右下排除和 640×480 viewport 裁剪。
   零覆盖自然不写像素，不把负向轴一律当空矩形。
2. 保留输入 F32 UV，以 F64 对原始端点插值，使用原未裁剪四角
   计算权重；不先取整端点、丢失 bias 或在 viewport 裁剪后重置 UV。
   POINT 按 `floor(uv * 实际物理尺寸)` 取 texel，WRAP 用有符号
   余数纠正负值，不用可能把微小负数舍入到 1 的 `uv-floor(uv)`。
3. 从当前绑定面按实际 pitch 读取，每次只填一行 scratch；
   SDL 源色键使用原像素值，RGB 调制读取 color 的各 W8，
   alpha 调制读取高 W8。XRGB 纹理自身 alpha 按不透明格式处理。
   stage 0 无纹理时按固定管线禁用该纹理阶段，直接使用顶点色。
4. One/Zero→NONE；SourceAlpha/InverseSourceAlpha→BLEND；
   One/One→ADD_PREMULTIPLIED。其他未被两 caller 产生的组合
   不伪造已有实现；native 绘制返回失败。
5. 逐行 unchecked SDL blit 写同一 Back，失败保留已写前缀。
   实际源或 Back 被锁时返回 `0x887601AE`；
   其他 native 失败返回 `0x80004005`，成功返回 0。
   所有返回均来自实际状态或操作，不把故障统一伪装成 surface-busy。

### 验收边界

本步只核对新增源码、真实 SDL 声明、引用/销毁顺序、UV/行距使用
及接口接线，不编译、不新增或执行测试、不运行客户端。
原驱动插值精度、SDL 通道舍入、纹理边缘与最终像素仍须真实验收；
不能由上述公式或静态检查宣称逐像素一致。
完整 RendererInit、接口能力/模式生产、探测和 DrawAllQueued
仍是后续必需任务，不因本步设备 factory 可用而记为已接通。

### CPU 提交落实

`SubmitLegacyTextureFan` 已定义并由两个上层入口共用；
私有设备的分配、八槽绑定/持有查询、单行采样/混合和释放均已定义。
RendererResources 为唯一设备 owner，Destroy 已在缓存面之后、
palette/Back 之前释放设备；绑定叶由真实设备类型持有，
不是独立全局对象。此时创建函数尚无 RendererInit caller；
后续按[完整 Init SPEC](cg2d_sdl_presenter.md)接入独立格式接口、
原 4/3/2 尝试、实际设备配置、尺寸与 Set/GetTexture 探测。
共享 stage 在成功探测之后写入，不把零初值当探测通过。

本步源码逐支静态核对空覆盖、正反向轴、WRAP 负余数、
真实 pitch、三组混合、色键先于调制、失败前缀和引用释放顺序。
现有 startup 源登记、`/fp:strict`、SDL 私有依赖无需修改。
只做静态检查；本次没有编译、链接、测试或实际像素验收。

## FillRect_44C9A0 完整入口 SPEC

2026-09-27，原会话只读核 `[44C9A0,44CC11)` 全指令与唯一直接
caller `520AEF`。沿工程委托复用已有四顶点 native 提交，不另建
填色器、不借普通 SDL FillRect 省略混合状态或解绑副作用。

1. 参数为 left/right/top/bottom 四个 S16、完整 W32 color、
   低 W8 mixMode；返回保留完整 W32，现有 dirty 包装只认
   `0x887601AE`。不将返回改为 bool。
2. 先调用 `HasLegacy3DDevice_44B610`，零立即返回 0；
   随后保存逻辑宽，依序检查left>=保存width、right<=0、
   top>=此阶段读取的逻辑高、bottom<=0；640/480只是初始化值。
   只按原条件拒绝，不新增 left>=right 或 top>=bottom 门禁。
3. left<0改零，right>入口保存width才fresh读逻辑宽低W16写right；
   top<0改零后fresh保存逻辑高，bottom>该height才将其低W16写bottom。
   两者仍承接S16局部，不能统一用固定640/480或同一次宽高取样。
   随后再次查询接口，
   非零才读 renderer +BC；任一门禁为零返回 0。
4. 将裁剪后的四角按左上、右上、右下、左下提交，
   每点 z=0、rhw=1、color 相同，全部 UV=0。
   这些 S16 坐标转 F32 没有额外舍入策略。
5. bit3、alpha=FF、其余三条混合选择与两纹理入口一致。
   先 source blend、再 destination blend，然后按共享 stage
   绑定 null，最后 DrawPrimitive；不保留上一张绑定纹理，
   也不因为裁剪后空覆盖而提前跳过这些状态写入。
6. 复用现有 SubmitTexturedRectangle 与 SubmitLegacyTextureFan，
   native 状态以 W32 位型透传。FillDirtyArea 的读取顺序、
   surface-busy 才置 dirty 以及其他返回不清 dirty 均不变。
   mode 1 没有该接口时仍不绘制，不擅自补普通填色回退。

函数放在既有 LegacyTextureDrawing.cpp；私有头统一声明，
GraphicPendingQueue 删除旧局部候选声明并包含该头。
仅静态核对门禁、混合/解绑/返回和现有 caller；
不编译、不新增或运行测试，不把本体接线当矩形像素验收。

## 逻辑尺寸与整数实参恢复（BUX01，2026-10-08）

[本步原证](cg2d_texture_viewport_sampling_spec.md#texture-viewport-sampling-baseline)、[裁决](cg2d_texture_viewport_sampling_spec.md#texture-viewport-sampling-decision)与[结构化证据](cg2d_texture_viewport_sampling_evidence.json)纠正本文早期固定尺寸/整数S16声明。两完整入口812条2960B/13页、三实际caller134条415B，完整S32坐标仅整数caller恢复，缩放低S16保持。整数初始与各负值裁剪后重取尺寸，缩放初始保存供裁剪/源RECT后两轴fresh决定+1；沿同源RendererResources既有接口，不新建owner或getter。

两cpp三片段、一私有hpp一片段；原UV/混合/缓存/返回尾、两个正确helper、八ftol/两SetRect/原纵flip减法全文保。纳管1745/1746、纠正1201调用合同，原三busy/next消费保。12B常量及两16B NN_NOP用途有实际原证；既有SDK替换不豁免这些上层差异，完整像素/寿命/ABI/运行未验。

### BUX01 当前修复限定静态结果

集中静态已通过：两完整根812条2960B/13页、三caller134条415B/原12B/两16B用途，三个源仅四具名片段；完整语义/登记簿及派生快照只有具名模型差异，七对应文档/三个任务变更和十七文件UTF8无BOM/LF、四限定证据各25真实来源均通过。检查脚本中的比较参数空格和旧字段层级已纠正后通过，不留下失败模型。

文本汇总与Check均退出0；实际语义1747/有效1740、合同1858（50.49%）、F888/E1043/R7已审450余2611、LOOP160/RENDER61。pending18/未知ECX、closed11/原差队列与完整门禁保持，四旧BUU过宽通过仍fail；仅本步开发修复/静态证据通过，完整所有父/callee/COM/owner-alias寿命、机器ABI/链接/像素/运行及R8未验，整体目标继续。

## 回退变换缓存的下游基线（BUY01，2026-10-08）

[本步完整基线](cg2d_surface_transform_cache_baseline_spec.md#surface-transform-cache-baseline)、[保留裁决](cg2d_surface_transform_cache_baseline_spec.md#surface-transform-cache-decision)与[结构化证据](cg2d_surface_transform_cache_baseline_evidence.json)将44CD80与44D490两已有正确入口纳管。两绘制回退原局部50条159B沿BUX01证据和已提交源码，不重取完整父；场景10以目标RECT为键，其余以源RECT，surface地址/图号/S16调色板与两翻转低W8传递、返回面继续Blt保持。

原两根526条1769B/八页与两场景新窗22条93B，源空/创建失败返原面、像素生成/尾fresh键场景及清理release→清值→erase正确保持。源码0改动、无新I，两23B用途；既有native SDK与host树仅原批准范围，不替代上层像素/寿命/完整父和运行验收。

### BUY01 当前基线限定静态结果

集中静态已通过：两完整根526条1769B/八页、四局部72条252B（两绘制窗准确复用BUX01）/两23B用途，两正确正文及六相关生产/构析源与HEAD保持；完整语义/登记簿和派生快照只有具名模型差异，七对应文档/四任务和十四文件UTF8无BOM/LF、四锚点/三限定S各25真实来源通过。源码/头文件改动0、无新I。

文本汇总与Check均退出0；实际语义1749/有效1742、合同1860（50.54%）、F890/E1046、R7已审452余2609、LOOP162/RENDER63/RESOURCE43。旧未接线误述已纠正；pending18/未知ECX、closed11/原差队列与完整门禁保持，四旧BUU错误通过仍fail。完整所有父/callee、owner/alias寿命、机器ABI/链接/像素/运行与R8未验，整体目标继续。

## 填充入口的逻辑尺寸取样实修（BUZ01，2026-10-08）

[本步原基线](cg2d_fill_rectangle_dimension_sampling_spec.md#fill-rectangle-dimension-baseline)、[裁决](cg2d_fill_rectangle_dimension_sampling_spec.md#fill-rectangle-dimension-decision)与[证据](cg2d_fill_rectangle_dimension_sampling_evidence.json)纠正早期FillRect固定640/480描述。完整44C9A0原162条625B/三页，两Has与六indirect/15跳转/两返回，唯一caller原19条57B复用BOZ01；尾15B实际NN_NOP用途。

一cpp一个片段恢复：第一次Has后保存width用于left/right比较，right超界才fresh宽低W16发布；初始top门禁按短路读高，top负值裁剪后fresh保存高用于bottom比较及低W16发布。四S16坐标/两Has/+BC、顶点/原混合/共享stage解绑null/Draw/完整W32与原caller busy消费保持，零头/新helper/owner，其他函数全文保。仅本步局部静态，完整像素/所有父/寿命/ABI/运行未验。

### BUZ01 当前修复限定静态结果

集中静态已通过：完整162条625B/三页、BOZ01既有19条57B原caller准确复用和15B NN_NOP用途；整cpp仅一个具名片段，正确caller整源/四其他函数及原混合解绑/返回尾保持。完整语义/登记簿与派生快照只有具名模型差异，七对应文档/三任务与十五文件UTF8无BOM/LF、四锚点/四限定I-S各25真实来源通过。

文本汇总与Check均退出0；实际语义1750/有效1743、合同1861（50.57%）、F891/E1050、R7已审453余2608、LOOP163/RENDER64。既有1412/BOZ证据及四旧BUU错误证据fail保持，pending18/未知ECX、closed11/原差队列与全部闭包保持；只有本步尺寸取样修复/静态对照通过，完整父/callee、owner-alias寿命、机器ABI/链接/像素/运行及R8未验，整体目标继续。

## BVA01描边/填色原逻辑裁剪取样（2026-10-08）

[完整原基线](cg2d_outline_rectangle_dimension_sampling_spec.md#outline-rectangle-dimension-baseline)、[裁决](cg2d_outline_rectangle_dimension_sampling_spec.md#outline-rectangle-dimension-decision)与[证据](cg2d_outline_rectangle_dimension_sampling_evidence.json)补case2真实Sub451350下游，原完整783条2600B/12页、三caller同BOZ原53条131B。初始left/top与负值裁剪后right/bottom四逻辑尺寸读点恢复，一cpp一个prefix/零头文件；同源RendererResources接口保，不新建owner或helper。

原mode2仅width非0、其余宽高>2、三个32位模式/left+1起笔/边角行列、AutoMap与颜色256的3B、物理pitch/native Back门禁/void正确保。R1一mixed只排除8/16/24目标支，不整体排除；8B尾用途实际解码，LOOP164/RENDER65四限定I-S，完整像素/父/alias寿命/ABI运行未验。

### BVA01 当前修复与混合范围限定静态结果

集中静态已通过：完整783条2600B/12页、BOZ53条131B准确复用、3B RGB与8B实际NN_NOP；整cpp只有一个prefix，原三32位像素/模式拒绝/physical pitch与正确caller整源/私有声明保持。三完整机器只有具名差异，R1原11记录和1411/旧证据保持；完整派生快照只新mixed身份、两域/候选、四任务/一X、R1分类来源和对应指标变化，八对应文档/十七文件UTF8无BOM/LF、四锚点/四限定I-S各25真实来源通过。

文本汇总与Check均退出0；实际语义1751/有效1744、合同1862（50.60%）、F892/E1054、R7已审454余2607、LOOP164/RENDER65。R1清单12/mixed3、具名分类120、原完整入口排除113；不扣整个混合F或计低色深为实现。pending18/未知ECX、closed11/原差队列和全闭包、旧四BUU错误证据fail保持；只有本步尺寸取样/适用32位局部静态通过，完整所有父/callee、owner-alias寿命、机器ABI/链接/像素/运行及R8未验，整体目标继续。

## BVB01可变源RECT复制的正确合同（2026-10-08）

[基线](cg2d_quadrant_transition_blit_baseline_spec.md#quadrant-transition-blit-baseline)、[裁决](cg2d_quadrant_transition_blit_baseline_spec.md#quadrant-transition-blit-decision)及[原证](cg2d_quadrant_transition_blit_baseline_evidence.json)固定4535B0完整92条278B/两页。原尾明确低S16目的坐标，现接口/caller转换不扩宽；离屏返0、负位移及右/底原式回写同源四S32 RECT、fresh逻辑尺寸与native借用复制保。

51EC10四独立RECT分别被callee原位裁剪，四块Temp借用和Y/X采样不合并。五相关源/两完整正文保持，零代码/无新I；两X合21B实际NN_NOP用途，LOOP/RENDER各两新成员与限定S，完整复制/生命周期/像素运行门禁未闭。

### BVB01 当前基线限定静态结果

集中静态通过：两完整根308条1147B/六页、十五局部累计210条697B含重叠邻指令、两21B实际NN_NOP；五相关源与HEAD全文相同，两完整正确正文/低S16声明/case18-19接线及原非对称写0/三字段发布保持。源码0改动/无新I，完整机器与派生快照只有具名差异；十四文件UTF8无BOM/LF、四锚点/两限定S各25真实来源通过。

文本汇总与Check均退出0；实际语义1753/有效1746、合同1864（50.65%）、F894/E1056、R7已审456余2605、LOOP166/RENDER67。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有旧记录与完整闭包保持；完整五父/其他callee、owner-alias寿命、机器ABI/链接/像素/正常失败/运行及R8未验，整体目标继续。

## BVC01扫描线原读点与直接提交（2026-10-08）

[完整原基线](cg2d_scanline_trigger_sampling_spec.md#scanline-trigger-baseline)、[裁决](cg2d_scanline_trigger_sampling_spec.md#scanline-trigger-decision)与[原证](cg2d_scanline_trigger_sampling_evidence.json)保存两扫描线完整根306条1062B/六页、七局部累计98条251B含重叠邻指令、两10B NN_NOP用途。原51F863/51FA73保存触发早于各四S32 RECT生产，当前共享助手已恢复此阶段/同一持样判定。Top/Bottom原除法源Y、全S32目的坐标、逐行fresh比例/循环条件、绘制后低S8零取8及真实返回保持；两wrapper/声明不改，只有一cpp一个prefix，LOOP/RENDER四限定I-S，完整像素/alias寿命/ABI运行继续。

### BVC01 当前修复限定静态结果

集中静态通过：两完整根306条1062B/六页、七局部累计98条251B含重叠邻指令、两10B实际NN_NOP；整cpp仅既有助手一个prefix，完整W32触发先持样后RECT/按持样判定，两wrapper与其余四相关源全文保持，原采样/方向三字段齐采发布/native正常失败保。两完整机器及完整派生快照只有具名差异，十五文件UTF8无BOM/LF、四锚点/四限定I-S各25真实来源通过。

文本汇总与Check均退出0；实际语义1755/有效1748、合同1866（50.71%）、F896/E1060、R7已审458余2603、LOOP168/RENDER69。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有旧记录和全部闭包保持；完整控制父/其他callee、owner-alias寿命、机器ABI/链接/像素/正常失败/运行及R8未验，整体目标继续。

## BVD01随机画块与网格/半幅真实消费基线（2026-10-08）

[完整基线](cg2d_transition_dispatch_grid_baseline_spec.md#transition-dispatch-grid-baseline)、[保留裁决](cg2d_transition_dispatch_grid_baseline_spec.md#transition-dispatch-grid-decision)及[原证](cg2d_transition_dispatch_grid_baseline_evidence.json)补四正确根979条3326B/17页、21局部累计293条953B含重叠邻指令；九窗准确复用本轮完整控制父，末窗13条不补邻函数。64块先一次RAND再原float/ftol，后只原拒绝路径重抽；两遍有不同RECT生产/Temp借用/低S16坐标序。960块960个x→y偏移与逆序源、每块比例及两S32乘积/向零除64/232-310偏移；半幅上半持样/下半独立fresh分母、第二借用再矩形生产与严格0/240阈值均正确保。五相关源/五完整正文保持，完整像素/寿命运行未闭。

### BVD01 当前正确基线限定静态结果

集中静态通过：四完整根979条3326B/17页、21局部累计293条953B/九准确复用与末窗真实13条，四区间178B的两表40项160B/15B NN_NOP及3B同ECX零位移LEA按实际解码/索引入向区分；五相关源/五完整正文与HEAD一致、真实36分派与各原生产/采样/native失败及发布保持。源码0改动/无新I，两完整机器与完整派生只有具名差集；十四文件UTF8无BOM/LF、四锚点/两限定S各25真实来源通过。

文本汇总与Check均退出0；实际语义1759/有效1752、合同1870（50.82%）、F900/E1062、R7已审462余2599、LOOP172/RENDER73。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有旧记录与完整闭包保持；完整51D790/51E0D0及其余父/callee、owner-alias寿命、机器ABI/链接/像素/正常失败/运行及R8未验，整体目标继续。

## BVE01FX原触发持样与复制消费（2026-10-08）

[完整原基线](cg2d_fx_trigger_sampling_spec.md#fx-trigger-baseline)、[取样裁决](cg2d_fx_trigger_sampling_spec.md#fx-trigger-decision)及[原证](cg2d_fx_trigger_sampling_evidence.json)承接BVD两大FX：两完整根1252条5152B/19页，完整控制父285条752B/18局部252条707B准确复用，两9项表72B/24B实际NN_NOP与两X96B用途。正向51D793、反向51E0D3都先保存完整W32，再生产四S32 RECT；既有两根一cpp两个prefix已恢复。两完整余体/声明与低S16矩形复制接口保，不新helper/owner/保护。取样实修/现消费及用途限定I-S通过前必须完成集中静态；数值分支/精度/全像素/寿命和运行门禁仍独立继续。

### BVE01 当前触发取样实修限定静态结果

集中静态通过：两完整原根1252条5152B/19页，BVD完整285条752B父及18局部252条707B准确复用、两9项表72B/24B NN_NOP与两X96B用途；整cpp仅两根各一个prefix，W32触发先持样后RECT/按同持样判定，完整余体/四其它源/声明与实际mode/value/payload消费保持。两完整机器和完整派生只具名差集；十五文件UTF8无BOM/LF、四锚点/四限定I-S各25真实来源通过。

文本汇总与Check均退出0；实际语义1761/有效1754、合同1872（50.87%）、F902/E1066、R7已审464余2597、LOOP174/RENDER75。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/1758与所有旧记录/完整闭包保持。仅取样prefix/接口及用途局部通过，完整数值分支/FPU精度、像素/其他callee/owner-alias寿命、机器ABI/链接/正常失败/运行与R8仍未验；完整原证不代这些门禁，整体目标继续。

## BVF01整源面快照与准备返回（2026-10-08）

[原基线](cg2d_back_temp_full_source_snapshot_spec.md#back-temp-baseline)、[实施决定](cg2d_back_temp_full_source_snapshot_spec.md#back-temp-decision)及[原证](cg2d_back_temp_full_source_snapshot_evidence.json)：准备根完整18条49B/一页，六调用局部84条418B/四父均弃返回；九共享复制窗200条712B含邻指令/重叠与场景9完整40条163B，GQD01完整769条2658B及case7准确复用，三零生产窗口58条219B；一X15B逐字节NN_NOP用途。CopyBackToTemp现按真实Back面尺寸而非固定640×480复制，源/目标native不可用或目标容纳不了整源时返回原正常资源失败；不剪源框、不缩放或用色键，CPU Back/Temp所有权和非拥有借用不改。51D750返回与FF48E4写入沿同完整W32 result，不窄S8、不多抽RNG。两cpp两hpp共四片段、零新helper/owner/接口函数；其余五源/调用方和完整函数保持，原native正常资源失败保，不增异常保护。四域八限定I-S各28真实来源，G02/R04/V04仍open；全像素/其它父-callee/owner-alias寿命/机器ABI链接运行未验，完整门禁保持。

### BVF01 当前准备与整源复制实修限定静态结果

集中静态通过：准备完整18条49B/六调用局部84条418B/四父、九共享复制窗200条712B含邻指令及重叠，GQD01完整769条2658B及case7精确复用/三零生产窗口58条219B，场景9既有1719保持、一X15B逐字节NN_NOP用途。两cpp两hpp仅四片段：完整W32同值写入/返回，真实Back整源面及原native正常资源失败，不部分裁剪/缩放/色键；六准备/九复制消费均弃值、失败后仍一次RNG，其他五源/所有余体和调用方控制流保。完整机器与完整派生仅具名差集，十八文件UTF8无BOM/LF、四锚点/八限定I-S各28真实来源通过。

文本汇总及Check均退出0；实际语义1762/有效1755、合同1873（50.90%）、F903/E1074、R7已审465余2596、LOOP175/RENDER76/MAP129/BATTLE264。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有旧记录及完整闭包保持；仅准备/整源复制合同、接口接线与用途静态通过，全像素/其它父-callee/owner-alias寿命、机器ABI链接/正常失败实测/运行和R8仍未验，整体目标继续。

## BVG01FX条带生产与实际复制接线（2026-10-08）

[原基线](cg2d_fx_strip_rectangle_publication_spec.md#fx-strip-rectangle-baseline)、[发布决定](cg2d_fx_strip_rectangle_publication_spec.md#fx-strip-rectangle-decision)与[证据](cg2d_fx_strip_rectangle_publication_evidence.json)：BVE两完整FX根1252条5152B/19页准确复用；八实际条带生产窗170条571B逐条一致，其中七实修151条506B、一反向7/8正确窗19条65B。七RECT片段恢复原top=row、bottom=row+1、left=0、right=640发布序；模式4奇偶X在字段后读、5在字段后借用Temp/读Y、6与正向7/8先持样数组坐标再写字段保持。5仍先截断再加row，原4535B0低S16及正常资源失败沿既有复制基线。仅一cpp七字段发布片段，原坐标持样/__ftol低S32/低S16、行循环、所有余体和四相关源/声明保持；LOOP/RENDER四限定I-S各25来源，1759/1760已有记录承接、零新F/X或成员，G02/R04/V04仍open。完整数值/FP精度、像素/其它父-callee/owner-alias寿命/机器ABI链接运行仍未验，完整门禁保。

### BVG01 当前条带字段发布实修限定静态结果

集中静态通过：BVE两完整原根1252条5152B/19页元数据/边界精确复用，八实际生产窗170条571B逐条一致，七实修151条506B及一正确19条65B、八原坐标/__ftol/4535B0接线准确承接。一cpp七字段片段恢复top→bottom→left→right四S32写入；入口/反向7-8正确穿插及全部数值/所有余体、坐标持样/截断/低S16、行范围、四相关源/声明和原native正常失败保持。完整机器/派生只两已有原新/两域四限定I-S与三任务差集；十五文件UTF8无BOM/LF、四锚点/四证据各25真实来源通过。

文本汇总及Check均退出0；实际语义1762/有效1755、合同1873（50.90%）、F903、R7已审465余2596、LOOP175/RENDER76均保持，E1078。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有其他旧记录及完整闭包保持；零新F/X/成员，只七RECT生产/实际消费及一正确片段保持的静态通过，完整数值/FP精度/全像素、其它父-callee/owner-alias寿命、机器ABI链接/正常失败实测/运行及R8未验，整体目标继续。
