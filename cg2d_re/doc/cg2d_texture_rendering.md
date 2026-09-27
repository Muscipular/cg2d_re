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
  `0x887601AE`，其丢失状态消费不能由后端任意非零值替代。
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
上述底层两函数、共享查询以及既有 `44B610` 仍只声明，
不写空函数、不返回固定成功、不把缺失 3D 路径改走普通 Blt。
当前 `DrawAllQueued` 尚无本体，故本步不伪称三个父调用已接通。
RendererInit、native 模式承载、像素提交和整链验收仍须继续。
不构建、不新增或执行测试、不运行客户端、不用 subAgent。

## 当前落实

[两个完整入口](../src/render/LegacyTextureDrawing.cpp)及
[私有接口](../src/render/LegacyTextureDrawing.hpp)已落位，
两条翻转回退路径调用现有 `TransformTextureSurface_44CD80`；
三个新底层依赖仍只有声明，没有占位实现。
本线程逐支核对整数/F32 裁剪、八次 S64 转换的顺序、两个入口不同的
翻转 UV、三组混合值与真实状态返回。CMake startup 登记与
`/fp:strict`、bias 位模式、十文件严格 UTF-8 无 BOM/LF、
相对文件链接及限定差异静态检查通过；不代表编译、链接或像素验收。
