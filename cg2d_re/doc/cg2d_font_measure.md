# 字体字节度量

## IDA 证据

本项只依据当前 `ida-17524 / D:\SOFTSTAR\cg2d.exe`：

- `TextMeasure@[0x44B2A0,0x44B2DC)` 共 `0x3C`（60）字节、24 条指令、8 个基本块，没有 callee；IDA 原型为 `int32 __cdecl(const char* text, int32 fontType)`。
- 该函数共有 147 个代码引用，逐项核对均为直接 `call`，归属 85 个唯一 caller；无需在本文抄录全部地址。
- `0x44B2A7..0x44B2AB` 先读取首字节并检查 NUL；空串由此直接返回 0，尚未读取 `fontType` 或字体表。非空后，`0x44B2AD` 才按完整 W32 读取 `fontType`。
- `0x44B2B2..0x44B2CA` 只检查当前首字节 bit 7：bit 7 为 0 时游标加 1 并读取 `sWidth`，bit 7 为 1 时游标加 2 并读取 `doubleWidth`，完全不读取或校验第二字节。
- `0x44B2D1..0x44B2D7` 在推进后读取新边界首字节，以 W32 加法累计宽度，再按新边界是否为 NUL 决定继续。
- `[0x44B2DC,0x44B2E0)` 是 4 个 NOP；其后的三参数 `Font::measure@0x44B2E0` 是独立函数，不是 `TextMeasure` 的延续或包装。
- `Font::measure@[0x44B2E0,0x44B32A)` 共 `0x4A` 字节、33 条指令、8 个基本块，没有 callee；IDA 原型为 `int32 __cdecl(const char* text, int32 fontType, int32 maxBytes)`。
- 六个直接调用点为 `0x44A4E1`、`0x456B8B`、`0x456C8A`、`0x456CFB`、`0x4A7603`、`0x4A76AA`。
- `0x44B2EC..0x44B2EE` 对 `maxBytes` 作有符号 `<= 0` 判断并直接返回 0；此路径尚未读取 `text` 或字体表。
- `0x44B2F8..0x44B301` 每轮只读取当前首字节：NUL 结束，bit 7 为 0 走单字节，bit 7 为 1 走双字节。
- `0x44B303..0x44B31E` 的双字节分支直接把游标和计数各加 2，并读取 `doubleWidth`；不读取或校验第二字节，预算只剩 1 字节时仍加入完整双字节宽度。单字节分支对应各加 1 并读取 `sWidth`。
- `0x44B31E..0x44B323` 以 W32 加法累计宽度和已消费字节，尾部用有符号 `consumed < maxBytes` 决定是否继续。

## 两参数 TextMeasure 算法

1. `text == nullptr` 时按新端最窄安全行为返回 0；否则必须先读取首字节。
2. 首字节为 NUL 时立即返回 0，不访问字体表；因此空串即使携带域外 `fontType` 也返回 0。
3. 仅在非空首字节之后检查 `fontType`，有效域为 0..13，域外按新端最窄安全行为返回 0。
4. 每个非 NUL 字符边界只看当前 W8 首字节：`0x01..0x7F` 推进 1 字节并取 `gFontMetrics[fontType].sWidth`，`0x80..0xFF` 推进 2 字节并取 `doubleWidth`。
5. 推进后读取新边界首字节，再以无符号 W32 累加宽度；新边界非 NUL 时继续，最终按有符号 W32 返回。

它与三参数 `Font::measure` 的关键差异是没有 `maxBytes`、消费计数和预算停止条件；不得通过传入 `INT32_MAX` 复用三参数函数。有效输入域内，两者都沿用 bit 7 判定的旧双字节规则，均不执行 UTF-8、GBK 或 Big5 解码。

## 当前源码 owner 与调用拓扑

当前唯一函数符号和 owner 是 `cg::login::detail::TextMeasure`。唯一声明位于 `src/LoginStateDependencies.hpp`，唯一实现物理位于 `src/render/Font.cpp`；物理目录不等于 C++ namespace，该定义明确处于 `cg::login::detail`。当前不存在 `cg::render::TextMeasure`，也不存在 `TextMeasure` adapter。`src/render/Font.cpp` 中的实现对原函数有效输入域内的相同输入字节保持算法等价；这不代表调用方改变编码后仍保持链级布局等价。`text == nullptr` 与域外 `fontType` 返回 0 是既有的最窄安全适配，不改变原有效域算法。

当前源码共有 9 个翻译单元的 27 个调用点。它们与原 `TextMeasure@0x44B2A0` 直接调用点的映射如下；这些消费者均直接调用上述唯一 owner，本项不移动或改写声明、定义与调用：

| 翻译单元 | 数量 | 原调用地址 |
|---|---:|---|
| `CharacterDeletionConfirmation.cpp` | 5 | `0x45E055`、`0x45E264`、`0x45E2C5`、`0x45E320`、`0x45E381` |
| `CharacterSelectModal.cpp` | 4 | `0x4621D0`、`0x46222E`、`0x462289`、`0x4622E7` |
| `CharacterVersionUpgrade.cpp` | 2 | `0x4623ED`、`0x462472` |
| `LoginCapability3InputController.cpp` | 1 | `0x45FB93` |
| `LoginCapability4Modal.cpp` | 2 | `0x4644D9`、`0x464509` |
| `LoginInputController.cpp` | 1 | `0x464F50` |
| `LoginMessage.cpp` | 8 | `0x45CE94`、`0x45CEBE`、`0x45CF2B`、`0x45D011`、`0x45D11F`、`0x45D1D8`、`0x45D2BB`、`0x45D4C8` |
| `LoginState.cpp` | 2 | `0x45D9B7`、`0x45DB99` |
| `world/RenderInput.cpp` | 2 | `0x459248`、`0x4594C2` |

## 地址中性度量表

原 `gFontList@0x558F68` 有 14 项、每项 `0x18` 字节。首个 W32 是原生 `HFONT`，本函数不读取；新端不把它压入固定 W32，而以独立的 `FontMetrics` 保存其后的 `0x14` 字节度量与配置。未来原生字体句柄须由独立资源类型承载。

| fontType | sWidth | sHeight | doubleWidth | doubleHeight | familyIndex | fontHeight | field16 | field17 |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | 9 | 17 | 17 | 17 | 0 | 0 | 0 | 0 |
| 1 | 8 | 15 | 15 | 15 | 0 | 0 | 0 | 0 |
| 2 | 11 | 21 | 21 | 21 | 0 | 0 | 0 | 0 |
| 3 | 9 | 17 | 17 | 17 | 0 | 0 | 0 | 0 |
| 4 | 8 | 15 | 15 | 15 | 0 | 0 | 0 | 0 |
| 5 | 11 | 21 | 21 | 21 | 0 | 0 | 0 | 0 |
| 6 | 11 | 21 | 21 | 21 | 1 | 20 | 0 | 0 |
| 7 | 25 | 49 | 49 | 49 | -1 | 48 | 0 | 0 |
| 8 | 5 | 11 | 10 | 11 | 0 | 11 | 0 | 0 |
| 9 | 6 | 12 | 12 | 12 | -1 | 12 | 0 | 0 |
| 10 | 8 | 16 | 16 | 16 | -1 | 16 | 0 | 0 |
| 11 | 10 | 20 | 20 | 20 | -1 | 20 | 0 | 0 |
| 12 | 6 | 12 | 12 | 12 | 0 | 12 | 0 | 0 |
| 13 | 7 | 13 | 13 | 13 | 0 | 12 | 0 | 0 |

`render::gFontMetrics` 是上述度量的唯一状态。既有 `login::detail::gValue558F78` 改为对 `gFontMetrics[0].doubleHeight` 的引用别名，不再保留第二份整数。

## 编码边界裁决与待修复项

2026-09-22，文档任务 `RPLAN.16.B`（所属 `RPLAN.16`）同步用户已批准的[文本编码边界 SPEC](cg2d_text_encoding_spec.md)：`cg2d_re` 忠实还原核心按原编码字节处理，保留原字节容量、索引单位和处理顺序；Unicode 仅在确有需求的现代边界集中转换。这是对“内部文本统一 UTF-8”的局部例外，源码与文档文件仍为 UTF-8，不将测宽算法改成 UTF-8 解码。

当前源码尚未按此裁决修复。首个待实施任务 `R6.1.2.8` 为连接提示的“常量 → 复制 → 测宽 → 布局 → 文字入队”链。在 font 0 初始 `doubleWidth = 17` 下，原正文 12 字节按 6 个双字节单元测得 102，当前 UTF-8 正文 18 字节被按 9 个单元测得 153，导致列数 `3 → 4`、对象 X `224 → 192`、对象宽度 `192 → 256`；原字节、公式及限定范围见[登录状态控制器证据](cg2d_login_state_controller.md)。该差异是当前待修复问题，不是用户批准的等价替换。

本轮未重新查询 IDA，仅使用既有原端证据与当前源码静态推导，未验证运行画面。其他文本链留待逐项盘点，不能由此推定所有文本采用同一编码，也不能将本轮文档同步记为实现修复。

## 新端边界

- 两参数 `TextMeasure` 对空指针采用最窄安全行为返回 0；非空时仍先读取首字节，空串在检查 `fontType` 前返回。只有非空首字节之后才检查 0..13，有效域外返回 0。原函数对空指针或域外字体索引没有安全保证。
- 三参数 `Font::measure` 在 `maxBytes <= 0` 时严格先返回 0，不读取 `text` 或度量表；`maxBytes > 0` 时，空 `text` 或越界 `fontType` 采用最窄安全行为返回 0。
- 任意 `0x80..0xFF` 首字节均按双字节处理；不进行 UTF-8、GBK 或 Big5 转换，也不增加尾字节校验。
- 当前源码中仍使用 UTF-8 字符串的调用方，会把其 UTF-8 原始字节直接传入；每个边界上的高位字节仍与后一字节盲目配对，多字节 UTF-8 可能被跨码点或跨 NUL 配对。这是历史方案的当前实现事实，待按新 SPEC 调整；“不保证 UTF-8 的视觉测宽正确”不是获准差异，不能据此豁免编码与布局契约。
- 两参数有效域不得增加尾字节检查或长度预算；孤立高位字节、尾字节为 NUL 或缺少可访问后续字节均保留为原函数的未定义/越界输入前提，不在本项修正。但上述正常连接提示在当前 UTF-8 输入中按偏移 `0, 2, ..., 16, 18` 访问，恰在 18 处读到 NUL，不跨 NUL 或越界，不能援引这些异常域豁免其布局差异。
- 宽度和三参数函数的消费计数均以无符号 W32 执行回绕，再按有符号 W32 做循环比较或返回，避免宿主 C++ 有符号溢出。
