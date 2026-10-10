# cg2d 静态资源来源与启动绑定证据

> 日期：2026-09-14；任务：R0.1.2.1。
> 状态：已通过独立验证；本文件记录当前 IDA 与磁盘的静态证据。
> 范围：静态文件名、启动参数改写、资源入口及有前提的候选路径。
> 不代表完整解码实现、相关函数逐项还原完成或实际运行资源集已冻结。

关联：[IDA 基线](cg2d_reconstruction_baseline.md)；[独立执行进度](../plan/cg2d_full_reconstruction_progress.md)。

## 1. 权威与地址口径

- 实例为 `ida-30904`，客户端标识 `509d5660`，模块为 `cg2d.exe`，数据库为 `D:\SOFTSTAR\cg2d.exe.i64`。
- IDB 原始输入 SHA-256：`dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08`，大小为 `1670816` 字节。
- 原端为 `metapc`、32 位、小端；本文地址均为当前 IDA 中的 VA，映像基址为 `0x400000`，不是磁盘偏移。
- 当前同名磁盘 EXE 与 IDB 原始输入不同，不能代替 IDA；匹配副本及活动数据库持久化边界见基线。
- 函数名、全局名仅作定位；数据角色以实参、指令、实际字节及消费者为准。

## 2. 启动命令来源

`WinMain@0x465790` 在 `0x4657C3` 将 `lpCmdLine` 写入命令文本全局 `0x9A6EC4`。
`0x4657C8` 以 `strstr` 检查大小写敏感的 `CGstart` 子串；命中时改从相对路径 `.\data\argument.ini` 取得命令文本。
打开调用位于 `0x465814`，模式为 `r`；缓冲区先清零为 4096 字节，`0x46583B` 调用 `fgets` 时传入容量 1024，最多读取 1023 字节内容并另写终止符（计数预减见 `0x53B23A`）。
累计长度达到 4096 时停止追加；`0x465890` 关闭文件，`0x4658A7` 将命令来源改为该缓冲区。打开失败也会改指向空缓冲区。
`0x4658CD` 检查最终文本是否含 `updated` 子串；不含时提示并返回 0，不能把配置文件存在等同于启动门禁通过。

| 顺序 | 调用指令 VA | 被调用函数 |
|---:|---|---|
| 1 | `0x4659B7` | `ReadSetupIni` |
| 2 | `0x4659C1` | `sub_532E70@0x532E70`，切目录并创建目录 |
| 3 | `0x4659CB` | `ParseCmdOpt@0x465F00` |
| 4 | `0x4659D0` | `sub_456050`，构造资源路径 |
| 5 | `0x4659D5` | `WndInit` |
| 6 | `0x4659DA` | `MainLoop` |

表中只列与本步相关的顺序，中间仍有其他初始化调用；`sub_533300` 属于存档链，本步不展开，也不声明没有其他间接目录修改。
`sub_532E70` 在 `0x532E77` 压入 `data`（`0x58F598` 字节 `64 61 74 61 00`），`0x532E7C` 调用 `_chdir`。
随后 `0x532E84` 清零索引，`0x532E91` 用 `%d` 格式化，`0x532E9B` 调用 `_mkdir`；`0x532EA4` 比较 `0x0A`，循环创建名称为 `0`–`9` 的目录。
最后 `0x532EA9` 压入 `..`（`0x58F594` 字节 `2E 2E 00`），`0x532EAE` 调用 `_chdir`；两次切目录及创建目录均不检查返回值，函数最终返回 1。
`_chdir@0x53DD2F` 在 `0x53DD3B` 调用 `SetCurrentDirectoryA`；当前已见该 API 的唯一直接引用入口为此包装，包装的已见调用为上述两处，不据此排除间接调用。
因此应区分读取 `argument.ini` 时的初始 CWD 与后续资源打开时的 CWD；若首次切入 `data` 失败，末尾 `..` 仍可能切到父目录，不能保证工作目录不变或等于 EXE 所在目录。

## 3. 六张静态文件名表

每张表均为 `9 × 0x80 = 0x480` 字节，实际读取范围全部已初始化；下列 SHA-256 覆盖各表完整原始字节，包含槽内填充。

| 语义 | 起始 VA | SHA-256 |
|---|---|---|
| Graphic 数据名 | `0x556F84` | `0b8b5208670efed3b45a6c81b57acee8a84e731a5a3827a540353bcbc948e858` |
| GraphicInfo 名 | `0x557404` | `318cc0618c52ede9e7bfa4552d1aa49ca492c00d217a5550a1376254152e9775` |
| Anime 数据名 | `0x557884` | `1a1648c65b5f2d2f6e440ee3d29d793c6fc3411e2751232e382d8981790a1ee9` |
| AnimeInfo 名 | `0x557D04` | `0085f7743e5cefd453dbf746286e0ebdf26d0e556dfde08668d5710cba0b475f` |
| Coordinate 数据名 | `0x558184` | `1d4bdf72dc5bd504bb8385283025512625c99e46852c942f3bbd1046463963eb` |
| Coordinate Info 名 | `0x558604` | `af331e70688ce893efb11ca696de3778d4fef59382a72814bb85fdeb5e5fca06` |

`gBinPath@0x55CF04` 的静态字符串为 `Bin`。下表列原始相对文件名；不改写时各项完整路径为 `Bin\` 加该项，不能省略 PUK 子目录。

| 槽 | Graphic | GraphicInfo | Anime | AnimeInfo |
|---:|---|---|---|---|
| 0 | `Graphic.bin` | `GraphicInfo.bin` | `Anime.bin` | `AnimeInfo.bin` |
| 1 | `GraphicEx.bin` | `GraphicInfoEx.bin` | `AnimeEx.bin` | `AnimeInfoEx.bin` |
| 2 | `GraphicV2.bin` | `GraphicInfoV2.bin` | `AnimeV2.bin` | `AnimeInfoV2.bin` |
| 3 | `GraphicV3.bin` | `GraphicInfoV3.bin` | `AnimeV3.bin` | `AnimeInfoV3.bin` |
| 4 | `PUK2\Graphic_PUK2.bin` | `PUK2\GraphicInfo_PUK2.bin` | `PUK2\Anime_PUK2.bin` | `PUK2\AnimeInfo_PUK2.bin` |
| 5 | `PUK3\Graphic_PUK3.bin` | `PUK3\GraphicInfo_PUK3.bin` | `PUK3\Anime_PUK3.bin` | `PUK3\AnimeInfo_PUK3.bin` |
| 6 | `Graphic_Joy.bin` | `GraphicInfo_Joy.bin` | `Anime_Joy.bin` | `AnimeInfo_Joy.bin` |
| 7 | `Graphic_Joy_Ex.bin` | `GraphicInfo_Joy_Ex.bin` | `Anime_Joy_Ex.bin` | `AnimeInfo_Joy_Ex.bin` |
| 8 | `Graphic_Joy_CH1.bin` | `GraphicInfo_Joy_CH1.bin` | `Anime_Joy_CH1.bin` | `AnimeInfo_Joy_CH1.bin` |

Coordinate 数据表原始九槽为 `*3,*3,*3,Crd.bin,*3,*3,*3,*3,*3`；Info 表为 `*3,*3,*3,CrdInfo.bin,*3,*3,*3,*3,*3`。
`*3` 是后述名称别名，不是要打开的文件；无版本改写且别名解析完成后，九槽分别都指向 `Bin\Crd.bin` 和 `Bin\CrdInfo.bin`。

`sub_456050@0x456050` 为每类建立九个 256 字节目标缓存及九个 4 字节指针槽；目标缓存与静态文件名槽不是同一布局。

| 来源表 | 目标缓存首址 | 指针表定位 |
|---|---|---|
| `0x556F84` | `0x62DA98` | `0x62D108` |
| `0x557404` | `0x62D174` | `0x62DA74` |
| `0x557884` | `0x62B5E4` | `0x62BEE4` |
| `0x557D04` | `0x62C808` | `0x62D150` |
| `0x558184` | `0x62E398` | IDA 名 `coordinateInfoNames`，实际为数据路径 |
| `0x558604` | `0x62BF08` | IDA 名 `coordinateFileNames`，实际为 Info 路径 |

`0x45609C/0x4560A1` 分别递增目标偏移 `0x100`、指针偏移 4，`0x4560A4` 比较 `0x900`。
随后六次 `sprintf` 以 `%s\%s` 拼接 base 与源名称，调用位于 `0x4560C9/0x4560E9/0x456109/0x456129/0x456149/0x456169`。
源槽步长为 `0x80`（`0x45616E`），指针步长为 4（`0x456177`），源偏移终界为 `0x480`（`0x45617A`）。

## 4. 参数改写与别名时序

`ParseCmdOpt` 用 `strstr` 定位 `binfolder:`，在 `0x466017` 以无宽度限定的 `%s` 写入 `gBinPath`，不是目录扫描或外部注册表。
资源版本同样由 `strstr` 定位关键字、`sscanf("%d")` 取数，再以各槽模板 `sprintf` 改写原静态名称；已见路径不检查该次解析返回值，不能补入验证规则。
Graphic、GraphicInfo、Anime 的九槽键后缀依次为无后缀、`ex`、`v2`、`v3`、`_puk2`、`_puk3`、`_joy`、`_joy_ex`、`_joy_ch1`，完整键分别以 `graphicbin`、`graphicinfobin`、`animebin` 开头并以 `:` 结尾。
第一轮资源循环在 `0x46669A/0x46669D` 递增指针偏移 4、名称偏移 `0x80`，`0x4666A3` 比较指针偏移 `0x24`，共九槽。

- Graphic 有第二次覆盖解析；槽 2 的实际键为 `graphicbiv2x:`，与首轮 `graphicbinv2:` 不同。两者同时命中时后轮覆盖前轮（`0x4664FD/0x466552` 调用 `strstr`，`0x46653B/0x466590` 写名）。
- AnimeInfo 槽 0–6 使用对应 `animeinfobin...:` 键，但槽 7/8 实际使用 `animebin_joy_ex:`、`animebin_joy_ch1:`，不能改成看似对称的 Info 键。
- `0x4660AF/0x4660B4` 分别将上述两个 Anime 键装入 ECX/EAX，`0x466274/0x466278` 放入 Anime 选择表，`0x4662B4/0x4662BB` 再放入 AnimeInfo 选择表；Info 改名调用为 `0x466692`。
- Coordinate 数字键存在于槽 0–5，槽 6–8 的键为空；每槽先检查当前名称是否含 `*`（`0x4666B9/0x466720`），含星号则跳过该槽数字参数。
- 因原始默认仅槽 3 不含星号，所以此默认状态下 `coordinatebinv3:`、`coordinateinfobinv3:` 可分别更新槽 3。
- 此后另一次循环才解析星号别名（`0x46679A/0x4667EC`）；`0x55D1A0` 实际字节 `2A 25 64 00` 是 `*%d` 字符串，不是指针，不能被 IDA 名 `off_55D1A0` 误导。
- 解析所得槽号左移 7（`0x4667BD/0x46680F`），从同一张表复制名称；因此原始 `*3` 引用的是已经过数字改写的槽 3，而非预先固定的 `Crd.bin`。

## 5. 资源入口与先后

`MainLoop` 在 `0x457023` 调用 `sub_4576B0@0x4576B0`；其中资源相关调用依次为：
`Renderer::InitPalette`（`0x457775`）、`GraphicAlloc`（`0x457782`）、`GraphicInit`（`0x4577B9`）、`AnimeInit`（`0x4578EF`）、`CoordLoadInit`（`0x457925`）。
这些调用之间仍有状态检查及其他初始化；此顺序不是所有失败分支都会继续执行的保证，也不是完整主循环还原。

| 入口 | 本步确认的文件行为 | 证据 |
|---|---|---|
| `GraphicInit@0x459960` | 先遍历九槽打开 Graphic 数据，再遍历九槽打开 GraphicInfo；两类必需槽均为 0、3、6、7、8，缺失进入失败分支 | 数据 `fopen`：`0x4599CF`；Info：`0x459A70`；必需槽失败目标包括 `0x459B5B/0x459B88/0x459B9D/0x459BC7` |
| `AnimeInit@0x45BB20` | 先打开 Info，必需槽为 0、8；后续只有局部标志为 1 才打开对应数据，数据缺失检查的必需槽也为 0、8 | Info 真实调用：`0x45BC05`，检查 `0x45BC13/0x45BC1B`；数据条件 `0x45BD2F`，调用 `0x45BD3C`，检查 `0x45BD4E/0x45BD56` |
| `CoordLoadInit@0x45C040` | 九槽逐项处理；相同数据路径可复用先前槽的文件句柄，只比较第一条数据路径，不同时比较 Info 路径 | `CoordLoad` 调用 `0x45C08E`；`_strcmpi` 比较 `0x45C0B4`；共享句柄 `0x45C111` |

GraphicInit 内部复制名称的缓存步长为 `0xFF`（`0x459A1D/0x459AF2`），不同于上一节路径构造缓存的 `0x100`，不能统一改写成同一布局。
坐标实参压栈 `0x45791B/0x457920` 及调用 `0x457925` 确认：来源 `0x558184` 是数据，`0x558604` 是 Info；两个 IDA 全局名称与该语义相反。
`CoordLoad@0x45BF60` 先打开数据（`0x45BF6E`），将其保存为共享 `FILE*`（`0x45BF7C`）；再打开 Info（`0x45BF8E`），遍历后关闭 Info。
本步未还原这些完整函数的记录布局、排序、覆盖规则、失败清理和后续索引使用，不把入口证据当作实现验收。

## 6. 调色板来源

记录公式为 `A(i)=0x554B78+i×0x82`；前 2 字节为有符号门槛，`+2` 为 128 字节路径。
实际字节范围 `[0x554B78,0x556252)` 有 45 个连续且已初始化的物理记录，之后 2 字节填充，`0x556254` 起为颜色数据。
但选择函数 `sub_450B70` 在 `0x450B74` 仅检查 `i<39`，没有负数下界检查；正常非负可选槽为 0–38，逻辑末界为 `0x555F46`，不能把物理槽 39–44 认定为可选资源。
下表名称均添加前缀 `pal\palet_`、后缀 `.cgp`，索引、名称和门槛按行内顺序对应。

| 索引 | 首记录 VA | 名称 | 门槛 |
|---|---|---|---|
| 0–15 | `0x554B78` | `00`–`15` | 全部 0 |
| 16–25 | `0x555398` | `opA,opB,opC,black,white,logo00,logo01,logo02J,idpass,opDJ` | 全部 0 |
| 26–29 | `0x5558AC` | `logo02,opAex,opBex,ld2` | 全部 1 |
| 30–34 | `0x555AB4` | `ssv2,tiex,tiv2_1,tiv2_2,e_s` | `2,1,2,2,0` |
| 35–37 | `0x555D3E` | `logo02cn,logo36cn,logo37cn` | 全部 2 |
| 38 | `0x555EC4` | `10` | 0 |
| 39–44 | `0x555F46` | `06,08,04,14,15,e_s` | 全部 0；超出正常选择范围 |

三处均以 `%s\%s` 拼接 `gBinPath` 与记录路径，默认 0 号实参是 `Bin\pal\palet_00.cgp`；base 同样可由 `binfolder:` 改写。

| 读取入口 | 索引与条件 | 拼接／打开 VA |
|---|---|---|
| `Renderer::InitPalette@0x450470` | 索引 0；仅 `tPaletteInited@0x628FD8==0` | `0x4507AB/0x4507BD` |
| `sub_450A80` | 参数索引；已见 `0x45A1B4` 传 0 | `0x450AA9/0x450AB8` |
| `sub_450BB0` | 活动索引不同于缓存索引，且门槛允许 | `0x450C24/0x450C33` |

三路均以 `rb` 从文件头读取 `224 × 3 = 672` 字节，无头部跳过或短读校验（`0x4507F5/0x450AF6/0x450C47`）。
磁盘已见文件大小为 708 字节，不是上述路径的消费长度；剩余字节用途不能由大小差额推断。
启动先初始化 0 号；`InitLoading` 首次请求 19 号黑盘（`0x457BC8→0x457C08`），其后按状态切换，不遍历整表加载。
选择与实际加载均将记录门槛与全局 `0x9A7F64` 比较（`0x450B88/0x450C06`）；该全局在 `0x457B2E` 接收 `sub_45C970` 的结果，本文不补定义其业务含义。
默认初始化打开失败返回 0；动态路径在 `0x450BCE/0x450BD4` 比较活动索引与缓存索引 `0x5562F4`，相等才跳过打开。动态打开失败在 `0x450C41` 直接返回；缓存索引到读取、关闭后的 `0x450D73` 才更新，因此失败后即使活动索引未变，后续调用仍可能再次打开。
读取后的 32 位转换路径有 `0x4509E1` 与 `0x451004→0x451220` 证据；仅排除低色深渲染，不排除索引资源、调色板读取及转 32 位色。
表区已见交叉引用为读取或取址，尚未证实运行期文件名替换；不能据此证明运行期绝无改写。
按 `D:\SOFTSTAR\bin` 核对，该表 38 个唯一文件名中 35 个存在且均为 708 字节；缺槽 35–37 对应 `pal\palet_logo02cn.cgp`、`pal\palet_logo36cn.cgp`、`pal\palet_logo37cn.cgp`。

## 7. 有前提的磁盘样例

以下结果**仅若当前 IDA 逻辑被执行、`CGstart` 选中该配置、读取配置时初始 CWD 为 `D:\SOFTSTAR`、上述两次切目录均成功，且后续 CWD 与资源参数无运行期改写**才成立；不是实际启动记录。
候选配置 `D:\SOFTSTAR\data\argument.ini` 大小为 1602 字节，已核对为 ASCII 内容；SHA-256 为 `60be5de597b59bb04c8630b286705b5d15a8548e4a0d1bf8fb3453e9e1c39cd7`。
配置中存在 `updated`，没有 `binfolder:`；以下文件名均相对于 `D:\SOFTSTAR\Bin`。每格依次列数据与 Info，版本数字也按此顺序列出。

| 槽 | Graphic／Info 版本 | 对应候选文件 | Anime／Info 版本 | 对应候选文件 |
|---:|---|---|---|---|
| 0 | `66/66` | `graphic_66.bin`、`graphicInfo_66.bin` | `4/4` | `anime_4.bin`、`animeinfo_4.bin` |
| 1 | `5/5` | `graphicex_5.bin`、`graphicinfoex_5.bin` | `1/1` | `animeex_1.bin`、`animeinfoex_1.bin` |
| 2 | 未指定 | `GraphicV2.bin`、`GraphicInfoV2.bin` | 未指定 | `AnimeV2.bin`、`AnimeInfoV2.bin` |
| 3 | `19/19` | `graphicv3_19.bin`、`graphicinfov3_19.bin` | `8/8` | `animev3_8.bin`、`animeinfov3_8.bin` |
| 4 | `2/2` | `PUK2\graphic_puk2_2.bin`、`PUK2\graphicinfo_puk2_2.bin` | `4/4` | `PUK2\anime_puk2_4.bin`、`PUK2\animeinfo_puk2_4.bin` |
| 5 | `1/1` | `PUK3\graphic_puk3_1.bin`、`PUK3\graphicinfo_puk3_1.bin` | `2/2` | `PUK3\anime_puk3_2.bin`、`PUK3\animeinfo_puk3_2.bin` |
| 6 | `125/125` | `graphic_joy_125.bin`、`graphicinfo_joy_125.bin` | `91/91` | `anime_joy_91.bin`、`animeinfo_joy_91.bin` |
| 7 | `195/195` | `graphic_joy_ex_195.bin`、`graphicinfo_joy_ex_195.bin` | `188/188` | `anime_joy_ex_188.bin`、`animeinfo_joy_ex_188.bin` |
| 8 | 保留默认 | `Graphic_Joy_CH1.bin`、`GraphicInfo_Joy_CH1.bin` | 保留默认 | `Anime_Joy_CH1.bin`、`AnimeInfo_Joy_CH1.bin` |

- 槽 7 的 AnimeInfo 版本由 `animebin_joy_ex:188` 生效；配置另有的 `animeinfobin_joy_ex:188` 不被这一槽的选择键使用。
- 配置末尾相关键原文为 `_joy_ch:` 而非 `_joy_ch1:`，不能匹配当前解析键；槽 8 四个文件因此保留静态默认，磁盘均存在。
- 坐标配置为 `coordinatebinv3:11`、`coordinateinfobinv3:11`，槽 3 变为 `coordinatev3_11.bin`、`coordinateinfov3_11.bin`；其余八槽随后解析 `*3`，最终九槽均使用这两条路径，两文件均存在。
- 四类九槽加两条去重坐标路径共 38 个候选路径，不包括上一节调色板；其中缺失 6 个，其余 32 个存在。
- 缺失槽 2：`D:\SOFTSTAR\Bin\GraphicV2.bin`、`D:\SOFTSTAR\Bin\GraphicInfoV2.bin`、`D:\SOFTSTAR\Bin\AnimeV2.bin`、`D:\SOFTSTAR\Bin\AnimeInfoV2.bin`。
- 缺失槽 7：`D:\SOFTSTAR\Bin\anime_joy_ex_188.bin`、`D:\SOFTSTAR\Bin\animeinfo_joy_ex_188.bin`；缺失事实不等于已经验证运行失败。
- 以上候选只做存在性与大小核对，未做候选资源全量哈希、完整记录解码或原端运行；基线中的少量样本哈希不能替代全量核验。
- `D:\SOFTSTAR\GlobalPalettes` 仅已知存在；当前读取链指向上述 base 下的 `pal`，没有证据将该相邻目录认定为实际来源或后备搜索路径。

## 8. 未决与验收边界

1. 实际启动器、命令行、是否经过 `CGstart`、初始及资源打开时的 CWD、两次切目录结果和运行期改写尚未核实；磁盘配置不证明运行时采用它。
2. 非启动入口、其他配置对资源的影响，以及完整加载失败、重试、清理和覆盖关系尚未闭合。
3. Graphic、Anime、Coordinate 的记录结构、索引、解码和跨资源关联尚未还原；调色板剩余字节与完整转换算法也不在本步验收范围。
4. 调色板门槛的业务语义、负索引可达性、物理槽 39–44 的其他引用，以及缺失文件对具体运行阶段的影响仍待证据。
5. 本步静态取证与文档已通过独立验证；未编译、运行原端或修改 IDA，验证结果统一记录于独立执行进度。

## `45A130` 自动地图颜色缓存包装入口

2026-09-24。当前 IDA `sub_45A130@[45A130,45A184)` 的唯一直接 caller 是 `InitializeClient@4578D6`，由 `4578D1` 压入 `62DA74` 九项图形信息路径表；返回值被丢弃。入口用 128 字节栈缓冲按原 `"%s\\%s"` 拼接共享 `gDataPath@55CF84` 与 `"auto.dat"`，调用 `45A420(fileName,paths)`；非零直接返回原结果，零时才依序调用 `45A190()` 和 `45A290(fileName,paths)`，透传末次完整 EAX32。无论缓存读取失败原因为何，本体都走同一回退，不另加文件存在性门禁。

新端 `cg::resource::LoadAutoMapColor_45A130` 复用已有数据路径及图形信息路径表，不复制路径数组。`45A420` 取路径表的第 0/1/6 项及缓存文件，成功时读取原 500,000 字节 `oldMapRes`；`45A190` 重建该表，`45A290` 写回缓存。当前共享表、重建与取色、完整缓存读写及旧 ctime 窄边界均已落位，见末节两个实施 SPEC；未进行旧缓存文件互通、真实文件时间或整个初始化链、完整链接/运行验收。

## `auto.dat` 缓存布局与地图颜色表别名

2026-09-24，当前 `ida-17524/cg2d.exe.i64`。IDA 数据项 `oldMapRes@927F48` 大小为 500,000 字节，排他尾 `9A2068`；`newMapRes@8F7208` 的数据项大小为 199,912 字节，排他尾 `927EF0`。两基址相差恰好 200,000 字节，但 IDA 数据项大小不是下标访问的边界。`45A190@45A1CB–45A1D2` 仅在 `450A80(0,palette)` 返回非零后，以 `rep stosd` 清 `oldMapRes` 全部 500,000 字节；返回零则保留旧表。之后它遍历图形记录，按文件号和图号条件在 `45A205/45A211/45A23A` 写 `oldMapRes` 的 W8，或在 `45A263` 写 `newMapRes[mapNo]` 的 W8。后一路仅对已证图号 `220000..265535` 执行，实际对应同一 `oldMapRes[20000..65535]`；不允许把这一路写入独立新表。`46C840@46CC75/46CCEA` 又以无符号 W16 地图图号索引读取 `oldMapRes` 的 W8，证明这段重叠内容进入地图绘制。

`45A420@[45A420,45A6B3)` 首先以 `"rb"` 打开 `paths[0]` 并取 `_fstat` 的 32 位 `st_ctime`；首文件打开或 stat 失败直接返回零。`paths[1]` 与 `paths[6]` 的打开或 stat 失败各保留对应时间零值。再以 `"rb"` 打开调用者给的缓存文件：依次读取 W16 版本 `2`、三个 W32 时间（分别对应路径 0/1/6）、W32 零哨兵及 `oldMapRes` 的一个 500,000 字节项；版本、时间、哨兵或任一项读取不满足条件即关闭返回零。整块读取只在 `fread(size=500000,count=1)==1` 时返回一；短读可能已改写缓冲前缀，不能把失败路径改成原子无副作用。`45A290@[45A290,45A415)` 同样先取三个路径时间，首路径失败返回零，另外两项失败保留零；以 `"wb"` 成功打开缓存后顺序写版本、三个时间、零哨兵和同一 500,000 字节表，忽略 `fwrite/fclose` 结果后返回一。写端请求的定长前缀共 500,018 字节；读端不检查额外尾部，不能以 x64 原生 `time_t` 宽度直接序列化四个 W32。

**请用户确认的实施 SPEC：**由单一 `cg::resource` 状态 owner 持有 500,000 字节、零初值的 `oldMapRes` 原字节表；`45A420/45A190/45A290` 与地图消费者均复用同一存储。已证 `newMapRes[220000..265535]` 以 `oldMapRes[mapNo-200000]` 表达原地址别名，不另建新表；这仅覆盖已证访问，不宣布 `8F7208` 前置区域或未知间接引用均可忽略。旧 `auto.dat` 使用定宽小端 W16/W32 头和原 500,000 字节正文，文件时间比较需保持原 32 位字段与取样次序；现代文件时间转为原 `st_ctime` 值的策略若不能直接对应，须另行核定。读取失败时保留已读字节、重建仅在调色板读取成功时清表、写入忽略写数的分支均须按证据保留。未经批准不建 owner 或编码三个下游；`450A80/451D80` 与图形/地图整链也不会随本 SPEC 自动视为闭合。

## 地图颜色表重建实施 SPEC

2026-09-28，按用户工程决策委托采用上节的唯一表与地址别名方案；
“待确认”只保留历史含义，不再等待逐项批准。现有原 IDA 会话只读核
`45A190/451D80/451E90` 全指令，并核 `45A420/45A290` 缓存读写。
本步完整实现重建及两个取色依赖，不将时间边界尚缺证据的缓存
读写或 `46C840` 地图窗生产接成空实现。

1. ResourcePaths 的既有自动地图缓存模块唯一持有 W8[500000]，
   零初值按已证 PE 零填充合同。通过 `const uint8_t*`
   借用查询，不复制状态、不以 W32 令牌代替地址。
   后续缓存读写须在同一模块使用本表，world 仅借用。
2. `45A190` 先快照 `gMaxGraphicCount`，清 1024B 局部调色板，
   调 `ReadPalette(0)`；失败不清表。成功先清整表，再按旧计数
   遍历图形记录，保持 fileIndex W16 与 mapNo S32。
   文件号仅 0/1/6/8 合格；其他文件号对原 mapNo 直接写零，
   不新增下标门禁。合格的 mapNo=0 写表首零；1000..19999
   写直接下标；220000..265535 写 `mapNo-200000` 别名；
   其他值跳过。仅在取色调用后重取图形表指针，不复制整条记录。
3. `451D80` 使用现有 `ReadGraphicDataByIndex` 和共享
   `gGraphicDataPtr/gCurrentGraphicWidth/Height`，最后参数为空。
   失败写 `gGraphicDataBuffer=-1` 并透传返回；
   成功写本次图号，S32 高度非正返回零。初始源指针只读取一次，
   行数快照；每像素后重读 S32 宽度，行尾按最后读取的宽度前进。
   索引零不查表/计数，其他项按 palette 的 +0/+1/+2
   累加 U32 红/绿/蓝并递增 U32 个数；全透明返回零。
   三个 U32 商各取低 W8，组成 `0x00BBGGRR` 后调 `451E90(...,256)`。
4. `451E90` 先计算表第 0 项 RGB 平方距离并以索引 0 为初值，
   再只扫描 `[16,count-16)`，不是从 1 或全 256 项中选择。
   仅严格更近时替换，等距保留先前索引；不因距离零提前退出。
   三通道差值来自 W8，平方和最大 195075，转 F64 可精确表示；
   保持 F64 比较，不引入感知色差、alpha 权重或浮点近似。
5. 取色叶分别落既有 GraphicRead/Palette，重建落既有
   ResourcePaths；不新增源目标或外部依赖，不修改原图解码策略。
   文件时间仍需核原 `_fstat` 的实际 32 位产生方式；
   不把现代 time_t 或文件修改时间当作旧创建时间。

本步按源码/全指令、共享状态、范围与别名核对；不构建、
不新增或运行测试、不读取运行图像。重建完整本体不等于
auto.dat 缓存、地图窗、case 5 或整体启动已通过。

实施结果：ResourcePaths 已建立唯一颜色表与完整 `45A190`，
GraphicRead/Palette 已实现 `451D80/451E90`，原 `45A130`
回退调用解析至同一定义。类型化借用不延长或转移所有权；
八文件差异及接口、已有 CMake 源登记、UTF-8/LF 和文档链接
静态核对通过。未执行实际读图、文件缓存或调色板比较样本。

## auto.dat 文件时间与完整读写实施 SPEC

本轮原会话只读追到 `__fstat@[53CCD0,53CF48)`，
核完整指令及错误映射 `542F44@[542F44,542FAB)`，
并与已有 `ConvertLocalTime_541160`、SDK 原生声明交叉核对。
按工程委托采用“保留旧时间计算、仅适配真实 FILE 的句柄取得”
方案，不调用现代 `_fstat/time_t` 代替原 32 位结果。

### 时间窄边界

1. 两个缓存函数只读取原 stat 的 +20 W32 ctime 和函数返回，
   不使用 mode、uid、size 等其他局部字段；这些字段不进入
   新公开接口。platform 提供 `ReadLegacyFileCreationTime32`，
   借用调用方仍打开的 FILE，返回 0/-1、只发布 W32 创建时间；
   不声称它是已实现的通用 `_fstat`，其他 CRT caller 不改接。
2. 用 `_fileno→_get_osfhandle` 取得真实本机借用句柄，
   不读取现代 FILE 的私有布局，不重造 x86 CRT descriptor 表。
   非法 descriptor/handle 保留本链的 errno=9、返回 -1；
   不把 HANDLE 截成 W32，不关闭句柄或接管 FILE 生命周期。
3. `GetFileType` 后清 bit 0x8000，原 type 2/3 返回 ctime=0，
   type 3 仍执行 `PeekNamedPipe`，忽略它的结果后成功；
   type 0 写 errno=9 后失败。其他非磁盘类型调用原错误映射，
   磁盘 `GetFileInformationByHandle` 失败也映射当次 GetLastError。
4. 磁盘依次处理 last-write、非零 last-access、creation：
   每次先 `FileTimeToLocalFileTime`，成功才 `FileTimeToSystemTime`，
   成功后调用同一 `ConvertLocalTime_541160(...,-1)`，
   保留已有时区/DST 共享缓存副作用，不能略掉不用于最终 ctime
   的访问时间转换。任一 OS 转换失败直接 -1，不新增 errno 映射；
   旧时间转换本身返回 -1 则仍作为正常 W32 时间，不改判失败。
   creation 的两个 W32 都为零时，取之前的修改时间值；
   不重做修改时间转换，也不使用访问时间回退。
5. 错误槽原 `1085FE0/1085FE4` 由 platform 唯一持有，
   使用原 `[591E80,591FE8)` 45 项表顺序，首个匹配决定 errno。
   保留重复 nativeCode=6 的顺序，不能字典化后覆盖首项。
   无匹配时 19..36→13，188..202→8，否则→22；
   188 以上且超过 202 的分支先写 8 再写 22，保留写序。
   原混合返回仅在映射边界转完整 uintptr_t，未匹配为表尾地址，
   匹配为 errno 数值。其他尚缺 CRT 生产/消费者不据此算完成。

### 定宽缓存读写

- `45A290/45A420` 共用本模块的文件时间采样 helper，
  逐次读取 paths[0]、paths[1]、paths[6] 后打开各文件；
  每次先取时间后关闭。首文件打开/查询失败终止，
  后两文件失败各保留时间 0；不能以文件是否存在代替查询。
- 读端先打开缓存，再顺序读版本 W16=2、三 W32 时间、
  W32 零哨兵、500000B 正文，每个 fread 的 size/count
  保持原值，每个比较/读失败都关闭返回 0。
  正文直接写唯一颜色表，短读已写前缀保留，不能改成原子替换。
  不检查额外文件尾，关闭结果不改变成功 1。
- 写端在时间采样完成后才以 `"wb"` 打开缓存，依次单项写
  W16 版本、三个 W32 时间、W32 零哨兵及正文；忽略所有
  fwrite/fclose 返回，成功打开即在结束返回 1。
  保持 500018B 定长请求，Windows x64 小端以断言明确，
  不序列化本机 struct padding 或 64 位 time_t。
- `LoadAutoMapColor_45A130` 原 caller 不改：
  读取非零直接返回，否则完整重建后写回。即使重建时调色板失败，
  仍尝试写当时颜色表，不增加重建结果或落盘成功门禁。

本步只适配当前两个 caller 的已证可观察合同；不扩展其他文件 API，
不改时间算法，不引入外部依赖，不执行实际文件写回。
完整函数落位仍不等于磁盘缓存互通、DST 环境或客户端运行验收。

实施状态：ResourcePaths 两函数完整实现并接原包装，
LegacyTime 提供窄句柄/时间边界及原错误映射，复用已有七字段
转换而不另存时区。原同一 500000B 表参与读、重建、写及 world
窗口查询；没有第二份缓存或失败回滚副本。
已核 API 声明、各失败分支、调用/发布顺序、45 项原表字面量、
格式长度与接口/既有源登记、UTF-8/LF/链接和限定差异；
仅静态检查，不构建/测试/运行，新断言未编译执行。

## 目录创建与共享错误槽收口 SPEC（2026-10-02）

完整核53C654..53C680的17条指令和六处调用，发现现有宿主_mkdir写宿主errno，
而auto.dat文件时间边界已使用原1085FE0/1085FE4唯一错误槽；
截图453797也明确读取1085FE0与17比较。按工程委托，本步将创建目录接回原
CreateDirectoryA及既有542F44映射，不创建第二份错误状态。

### 原入口合同

53C65A调用CreateDirectoryA(path,nullptr)。成功把本地错误值置0；
失败才调用GetLastError。随后只在该值非0时执行542F44并返回-1，
否则返回0。因此保留API返回失败但错误值为0时的原返回0，不擅自统一成-1。
成功不清共享errno/DOS错误，也不新增路径预检查、Unicode转换或重试。
原路径为NUL终止旧字节，继续经ANSI API；不推定为UTF-8。

542F44继续使用既有原表，先写DOS错误，再按首匹配或原范围规则写errno。
不改表顺序、重复项、失败映射或其已有返回语义，不调用现代_dosmaperr。

### 全部直接调用点和实施

| 原caller及调用点 | 当前消费者 | 消费方式 |
|---|---|---|
| 449DA0@449E34 | Input日志 | 忽略创建结果，随后继续原时间/文件链 |
| 453760@45378B | SdlPresenter截图 | 结果0继续；非0时读取原errno，17继续、其他返回 |
| 468570@4685C8/4685EA/468611 | SceneState地图缓存 | 按原顺序创建map、地图子目录和条件子目录，忽略结果 |
| 532E70@532E9B | StartupCommon存档目录 | 原0至9循环逐次调用并忽略结果 |

在既有LegacyTime.cpp添加CreateLegacyDirectory_53C654，复用本文件错误映射；
在LegacyTime.hpp只增加该字节路径接口和原S32错误槽extern供截图读取。
不为少量接口拆新文件，不公开Win32类型或私有映射函数。
Input/SceneState移除无其他用途的direct.h；截图移除无其他用途的direct.h/cerrno。
StartupCommon仍使用_chdir，保留其direct.h。六处源码调用全部替换，截图
保持短路顺序，不在创建成功后额外读取错误槽。截图先前现代_mkdir/errno适配
由本节收紧为原API/共享状态，不改GDI捕获、文件探测、像素或BMP写入。

### 未验边界

本次同时定位53DD2F切换目录的45条指令：SetCurrentDirectoryA成功后查询当前目录，
非双同斜杠路径还更新驱动器环境项；其mbctoupper和错误状态链未在此实现。
现有_chdir不因本轮创建目录修复记为原切换目录已等价，也不扩展为全部文件CRT完成。

旧交易CB1D1C名称区仍无已证生产/容量，D30134/CF46D0缺完整生产，保持未决；
不从172字节的IDA数据项标注猜十条数组。本轮未补交易空数据。

集中静态核对17条入口、六处接线、同一错误owner和截图短路条件；
不实际创建目录、改变CWD、写截图、运行客户端、编译、测试或进行哈希校验。

## 存档目录切换接线（2026-10-02）

前节未完成的53DD2F现已按[多字节生产SPEC](cg2d_legacy_multibyte.md#启动生产与目录接线实施-spec2026-10-02)
实现并替换CreateSaveDirectories两处_chdir。仍先进入data、依次创建0至9、
最后切到..，全部返回仍忽略；不新增工作目录保存/恢复或失败门禁。

新端调用原SetCurrentDirectoryA/GetCurrentDirectoryA(261)/SetEnvironmentVariableA，
用CRT553178实际初始化的原多字节表转换驱动器首字节；
目录改变后查询或环境更新失败均经既有542F44，已发生的CWD改变不回滚。
该修复不代表实际改过目录。NLS失败未写局部内容、长路径异常、全启动次序和运行
仍未验；旧环境/参数入口本身不因初始化职责接线算已实现。

## 自动地图缓存、资源路径与原文件状态整链接线实施 SPEC（2026-10-03）

按第8条工程委托，本线程采用补齐53CCD0完整文件状态本体、
保留创建时间投影接口并迁移全部缓存文件流的方案。
不再借宿主_fileno/_get_osfhandle；LegacyFile的S32 descriptor直接
进入已有唯一原描述符表。只拓宽FILE/HANDLE指针，不扩大stat或缓存字段。

| 原父范围 | 完整指令/直接入向 | 接线范围 |
|---|---:|---|
| 456050..456189 | 77/1 | 4560C9/4560E9/456109/456129/456149/456169六格式→53B304 |
| 45A130..45A184 | 25/1 | 45A14B格式→53B304；原读取→重建→写回调用保持 |
| 45A290..45A415 | 138/1 | 四打开、五关闭、六写入及三次53CCD0 |
| 45A420..45A6B3 | 281/1 | 四打开、十六关闭、六读取及三次53CCD0 |
| 53CCD0..53CF48 | 210/6 | 描述符验证、设备/管道/磁盘状态与全部时间/错误依赖 |

五父完整731条、十直接入向与全部调用取证。
应用层48个文件/格式调用及六个stat点，共54点对应现有25个源码
表达式；时间采样helper和三个时间的读写循环保持动态顺序，不拆为重复代码。
原53CCD0十四个调用包括三次541160、一次542F44及十个Win32 API点，
全部由真实本体与既有依赖承接，不沿用窄时间实现宣布通用stat完成。

内部LegacyLowIo.hpp新增无指针LegacyFileStatus32，固定24H/36B：
+00/+10设备W32位型，+04 inode/+06 mode/+08 linkCount/+0A user/
+0C group为W16，+0E两未写字节，+14 size低W32位型，
+18/+1C/+20为S32访问/修改/创建时间。仅已证宽度命名，未取证的
数值符号含义不另作推断。尺寸/每字段偏移及4B对齐以静态断言表达；
不预清对象，不把原未写+0E补零，不序列化该结构到auto.dat。

ReadLegacyDescriptorStatus32_53CCD0直接以U32 descriptor/capacity比较，
越界或同表flags/1未置只写共享errno9并返-1，不新增句柄值门槛。
保存的是同一块槽的引用，GetFileType及后续查询重新从槽取HANDLE；
只清type的8000H位。type0 errno9/-1，其他非1/2/3类型只调用
GetLastError→542F44返-1，输出不提前清理，DOS错误槽沿既有映射。

type2/3依原写序发布mode2000H/1000H、两个设备字段=descriptor、
linkCount1、inode/group/user0、创建/修改/访问时间0。
type2 size0成功；type3仍重取同表HANDLE执行PeekNamedPipe，
成功用available低W32，失败size0但仍成功、不新增错误映射。
disk仅先写mode/group/user/inode0及linkCount1，再查询文件信息；
失败先映射当次错误，保留已写前缀与未写字段。

disk按attributes低位1在mode或124H/1B6H，不增加目录类型判断。
顺序仍last-write→非零last-access→creation；每阶段先
FileTimeToLocalFileTime，再FileTimeToSystemTime，成功后才
调用既有ConvertLocalTime_541160(...,-1)，保留原TZ/DST副作用。
修改转换后立即写+1C；零访问时间复用上次修改转换值并写+18，
非零才新转换。零创建时间在最后重新读输出+1C，不能拿访问时间
或冻结的局部替代；非零才新转换。任一OS转换失败直接-1，不新增
errno映射，541160返回-1仍作为已产生S32时间。最后依序mode或8000H、
写创建时间、设备0、sizeLow、另一设备0；不检查或合并sizeHigh。

ReadLegacyFileCreationTime32改为LegacyFile*投影：采样其descriptor，
调用同一完整53CCD0，成功才把+20创建时间发布到输出并透传S32结果；
失败不写输出，不关闭句柄/流。原六个应用点只消费ctime/返回的
接口保持，其余stat字段由完整本体实际产生，不扩大上层接口。
LegacyTime.hpp仅前置声明LegacyFile，不暴露Win32或宿主FILE；
完整stat声明和定宽结构只放已有内部Lowio头，不新增文件/依赖。

缓存仍逐次采样paths[0]/[1]/[6]，首缺文件/查询失败终止，后两项
失败保留0；成功先写相应时间再关闭。所有流统一LegacyFile，
现有合循环/短路及各动态路径恰关闭一次保持。
read按(2,1)版本2、三(4,1)时间、(4,1)零哨兵、(7A120H,1)
正文顺序读取/逐项比较；任一失败关闭返0，正文短读已写前缀保留，
不加额外文件尾检查。write同序忽略写入/关闭结果，打开成功结束返1；
请求500018B不变，正文尺寸改为显式U32 7A120H而不是size_t。

456050先绑定六组九路径指针，逐槽按既有顺序格式化，返回最后
coordinateInfo的S32格式结果；45A130保留128B未清路径、共享gDataPath、
auto.dat及读取非零直返、失败重建后仍写回的原流程。七格式全部
接53B304，不展开资源名、不重建路径owner或新增落盘成功门槛。
含NUL实际字节：553514=25735c257300、554920=726200、
55491C=776200、5598E4=6175746f2e64617400。
原资源名/路径编码、共享500000B颜色表、地图别名/重建/取色与其他
时间算法/目录/CRT正文逐字保持。旧窄接口借宿主描述符的历史实施
由本节覆盖，不以过去验证宣称新完整stat已运行。
只核新增本体、字段/失败/调用/别名/文件差异；完整CRT启动/模式3、
日志等其他业务FILE链与内容/运行验收仍待，音频暂停、待批/排除保持。
不构建、链接、查产物、新增/执行测试、读写缓存样例、运行客户端或哈希校验。

## BOQ01 调色板DBF索引生产实施续记（2026-10-06）

[BOQ01 SPEC](cg2d_palette_dbf_index_production_spec.md#palette-dbf-index-production-decision)/[原证据](cg2d_palette_dbf_index_production_evidence.json)。522900完整170条553B/三页、16call-11内跳/1ret，唯一普通入向522B8E/父522B30。五段原数据31B；58D360在.data，初值44424600、唯一直接引用5229C5为W32读。新增私有U32承载1、headerMagic→共享比较字和stride→fresh payload→S32偏移→native record地址两正文补丁。原正常值、两清零path/第255Byte判断、短读共享头/魔数不符不关闭、两分配及解码/旧块与键/重复覆盖/count-stride后pub/关闭后迭代入队保。

本步沿PGM01原两池初始化与BOM01独立色表consumer补DBF生产；auto.dat旧0/1/6时间、定宽500018B写入及读取正文未改、不重验。当前工程委托的实施依据见具名SPEC，历史“请确认”文字不新增审批门槛。

724已有ID完整历史深拷贝续记，语义1404/1399有效ID及1517关联41.22%不变；分类521→522、RESOURCE40→41枚举open，独立局部I/S E647→649。历史直接定义1/旧FILE属性保持；七open仅引用/进展、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它门禁保持。

仅全根正常局部static；W32比较字非地址不扩宽，不据无直接写引用宣只读或所有alias闭合。私有vector/map沿已有批准宿主承接，不以本根代原容器机制/53AF90或SDK-owner运行。未构建/测试/运行/hash；网络原格式明文、音频暂停，不新加异常/越界/故障或关闭策略。

## 2026-10-11 PCR01：调色板门槛实际来源

[PCR01](cg2d_palette_capability_selection_spec.md#palette-capability-source-state)补齐45C970探测及450B70选择，两完整原根78条232B/两页、2E8一个目标/九跳五RET。移除probe原无的出参初值，Select恢复steps判定先于发布，两组两处/五注释；五固定图号两轮同index重查、原-1/4-index、S16门槛/39上界及拒绝index-接受steps返回保持。一新F/旧171追加、两adapted、MAP193/RENDER120，两限定S零I八open；主表生产/全部父及资源owner/SDK/运行未闭。
