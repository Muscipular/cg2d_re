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

新端 `cg::resource::LoadAutoMapColor_45A130` 复用已有数据路径及图形信息路径表，不复制路径数组。`45A420` 取路径表的第 0/1/6 项及缓存文件，成功时读取原 500,000 字节 `oldMapRes`；`45A190` 重建该表，`45A290` 写回缓存。2026-09-28 当前状态：共享表与重建及其两个取色叶已完整落位，见末节；缓存读写仍仅有真实声明，`ctime` 的原 CRT 产生边界尚待核对。不计旧缓存格式兼容、整个初始化链、完整链接或运行完成。

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
