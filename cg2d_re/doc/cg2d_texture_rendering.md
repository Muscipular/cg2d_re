# 纹理绘制入口与底层边界

2026-09-27，原 `ida-17524 / D:\SOFTSTAR\cg2d.exe.i64` 只读核
`44D520–44DA8F`、`44DA90–44E0B1` 全部指令及反编译，
并核 `5211BE/521393/52169F` 三处实参和返回消费。
以下为工程委托下先行 SPEC，不把底层声明当作像素实现。

## 类型与返回

- `44D520`：x/y 为 S16，矩形为四项 S32 按值传递；
  surface 是非拥有 `TextureSurface*`，图号 S32、调色板 S16、
  mixMode 取低 W8、color 保留 W32。
- `44DA90`：同样的 x/y、surface、图号、调色板、mixMode/color；
  矩形以指针同步只读借用，两个比例为 F32，不回写 caller。
- 两函数早拒绝及无 3D 时 bit3 置位均返回 0；实际绘制透传完整
  W32 状态，不把返回固定成成功。三个 caller 后续比较
  `0x887601AE`，其表面忙状态消费不能由后端任意非零值替代。
- x64 私有矩形沿已有变换缓存使用 `array<int32_t,4>`，
  顺序 left/top/right/bottom；完整指针不编码成 W32。
  不引入 Win32 RECT 到上层业务，也不改变原图号与调色板宽度。

## 44D520 不缩放路径

1. 按序拒绝 x>=640、right+x-left<=0、y>=480、
   bottom+y-top<=0。先检查交集，再按 bit0/bit1 裁剪，不额外
   添加空矩形或源面范围校验。
2. 横向正向：x<0 时 left-=x、x=0；超过右边时
   right=left+640-x。翻转时 x<0 改 right+=x、x=0；
   超过右边时 left=right+x-640。
3. 纵向同理：正向改 top/bottom；翻转改 bottom/top。
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
   运算再存回 F32；x/y 转 F32。先拒绝 x>=640，再比较横向
   宽度+x，之后 y>=480、纵向高度+y。两次浮点检查对应
   `fnstsw/test ah,41h`，有序严格大于零才继续，unordered 也拒绝。
2. clipping 的中间算式按原 x87 PC53 用 F64，只有原 `fstp`
   到 F32 局部的位置才收窄；禁止合并或代数重排。翻转横向超界
   是 `left=(extent-width)+left`；**翻转纵向超界是
   `top=top-(extent-height)`**，指令 `44DC5D–44DC65` 如此，
   不按对称性修正成加法。
3. 先 bottom/scaleY、right/scaleX、top/scaleY、left/scaleX
   各自 F64 除法，按既有 CRT PC53/`__ftol` 合同转 S64、
   取低 W32，形成源矩形。RPLAN.24 下不额外处理越界或数学异常。
4. 当前 x+right-left 小于 640 才给 right 的 F64 临时值加 1；
   当前 y+bottom-top 小于 480 才给 bottom 的临时值加 1。
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
其中色键 Blt 与共享查询后续已按下节落位；四顶点提交仍只有声明，
没有占位实现。
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
- CpuBackStorage 仍只有一块原大小像素数组，包括既有额外尾像素；
  SDL 包装仅暴露 640×480，pitch=2560，不把尾像素算入可见画面。
  创建失败保留空 Back，成功后清面、手工像素、快照、Blt 和 Flip
  均操作同一数组。释放顺序为 SDL 包装、像素，不另建一份 Back。
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
