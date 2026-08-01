# M6R：协议 209 包显式强类型补完计划

> 基线日期：2026-07-29
> 本计划是 M6 的独立补完线，以当前代码、schema 和协议清单重新盘点，不沿用历史“88 个 payload 目标完成即 M6 完成”的宽松口径。

## 1. 范围与完成定义

M6R 只补协议编解码、协议证据和测试，不实现 TCP session、重连、网络线程或业务事件分发；网络运行链路仍属于 M10。

以 `tests/fixtures/protocol/schema/packet_coverage_schema.json` 的 209 个大小写敏感唯一 code 为全集 `U`。原计划要求 `U` 全部进入 typed/fixture；用户现明确增加 3 个执行例外：

- `contractStatus=no-effective-wire-contract`：`sz`、`Expire`，表示当前无有效封包协议；
- `executionStatus=user-skipped`：`yp`，仅表示用户明确跳过本轮执行，不表示不存在有效封包协议、exact0、已 resolved 或证据终止；
- 有效完成集合 `E = U - terminal - user-skipped = 209 - 2 - 1 = 206`。

schema 全集和方向基线仍为 209、100/109，不删除例外 code。M6R 有效完成必须同时满足：

1. `E` 中 98 个客户端→服务端封包有显式 `cg::packet::send::*` 类型和生产 `Encode()`。
2. `E` 中 108 个服务端→客户端封包有显式 `cg::packet::receive::*` 类型和生产 `Parse()`。
3. 每个类型显式声明 `Code`、`Direction`、`Fields`、`Spec`，字段数量、顺序、名称和类型均有可追溯来源。
4. 不允许完成态 `Spec` 使用 `PacketFieldType::Unknown`，不允许用 `unknownN`、异常 token 或猜测字段冒充完成。
5. 每个 `es/char*` 字段都完成普通字符串、结构化 payload 或有证据的不透明文本三选一判定；结构化 payload 必须完成 parse/build 和无损回写。
6. `E` 中每个 code 至少有 1 条独立明文 fixture；最终 fixture 恰好覆盖 206 个 code、98/108 两个方向。
7. 测试侧 typed manifest 显式引用 206 个具体类型，并验证 98 个 `Encode`、108 个 `Parse`；不得把 schema 的 209 行等同于 typed 覆盖。
8. 禁用诊断文本和 test helper 后，生产协议表面仍可编译并运行最小 smoke。
9. 不恢复生产侧 `PacketRegistry`、无 `Spec` 的 Parse/Build、按 code 自动分发或 schema 驱动生产类型。
10. 不引入 RTTI、SDL 类型、外部依赖或新的协议目录。
11. `sz`、`Expire`、`yp` 保留在 schema，但不得进入 typed manifest、fixture inventory 或 payload audit；三者的状态字段必须保持精确且互斥。

## 2. 已核实基线

下表“当前”保留 M6R 建立时的初始基线；完成目标按有效集合 `E` 修订，最新状态见 2.3。

| 指标 | 当前 | 完成目标 | 缺口 |
|---|---:|---:|---:|
| schema 唯一 code | 209 | 209 | 0 |
| 客户端→服务端 | 100 | 100 | 0 |
| 服务端→客户端 | 109 | 109 | 0 |
| 显式 typed struct | 91 | 206 | 115 |
| `send` typed / `Encode` | 25 | 98 | 73 |
| `receive` typed / `Parse` | 66 | 108 | 42 |
| packet 测试目标 | 95 | 非完成计数 | — |
| 已有 typed code 的直接测试字面量 | 91/91 | 206/206 | 115 |
| typed manifest `Spec` 与 schema effective 显式交叉核对 | 91/91 | 206/206 | 115 |
| 独立 `.pkt` 明文 fixture | 0/209 | 206/209 | 206 |

历史 88 个“疑似内嵌 payload”code 已全部进入现有 91 个 typed 类型；另外 3 个是 `JFVf`、`Gp`、`CCL`。剩余 118 个均未被旧关键词规则标为 payload，但这只是启发式结果，不能替代逐包 `es/char*` 审计。

### 2.1 剩余 118 包的证据状态

| 队列 | 客户端→服务端 | 服务端→客户端 | 合计 | 当前结论 |
|---|---:|---:|---:|---|
| Q0：外层定义可进入逐包审计 | 14 | 16 | 30 | 外层字段为明确 `i62/es`，或 xlsx 完成态明确零字段 |
| Q1：字段类型为 `unknown` | 51 | 9 | 60 | 共 119 个字段缺少有效类型 |
| Q2：format 为空 | 4 | 16 | 20 | 无 `resolvedFormat`，必须先查 Server C/IDA |
| Q3：异常定义 | 6 | 2 | 8 | 原格式未转成字段、非完成态零字段或 malformed token |
| 合计 | 75 | 43 | 118 |  |

因此，严格门禁下只有 Q0 的 30 包可以开始逐包 DoR 审计；Q1～Q3 共 88 包必须先完成协议证据闭环。

### 2.2 现有 91 包的遗留债务

- 当前 91 个 typed code 均能在 packet 测试中找到直接 code 字面量，但不能据此证明 schema 字段形状和外部 fixture 已覆盖。
- `a205b34` 历史时点的 45 个唯一 code 只是 `PacketCoverageSchemaTest` 手写 `FindPacket` 分支数；M6R.0.1a-k 与 `uSB` 补证后该数为 57，二者均不是 manifest。本步从 0 建立 91 项显式 typed manifest，并完成 91/91 schema effective 交叉核对。
- fixture 目录只有 `schema/packet_coverage_schema.json`，不存在 `.pkt` 明文样本。
- M6R.0.1a-k 已逐包完成 `aPc, UU, Sc, NI, eZT, IUn, DpSY, cNB, ow, DyAG, KyN` 的证据闭环、resolved 元数据和必要 typed 修正；这 11 包不再属于遗留债务。

### 2.3 当前有效范围快照

截至 `LYIm` 完成，schema 仍为 209 个 code、100/109 两个方向；typed/manifest 为 206/209（send 98/100、receive 108/109），fixture/audit 为 181/209（97 C→S、84 S→C），历史 fixture 债务为 25，M6R.5 完成 66/91。

- terminal：`sz`、`Expire`，共 2；
- user-skipped：`yp`，共 1；
- active Q0/Q1/Q2/Q3：`0/18/17/0`，其中 Q2 从原始 20 中扣除已完成的 `ed`、terminal `Expire` 和 user-skipped `yp`；
- 原始 typed 缺口：`209 - 171 = 38`；
- 有效 typed 缺口：`206 - 171 = 35`；
- 来源未决 12、payload 未审计 22，均沿用现有历史账，暂不作为可独立重算门禁。

## 3. 协议证据规则

### 3.1 来源优先级

1. `plans/CG封包.xlsx`：code、方向，以及明确写出的 `i62/es` 外层字段。
2. `D:\Source\cgmsv\cgmsv_server\nrproto_serv.c` 和 `include\nrproto_serv.h`：收发函数参数数量、顺序与 `int/char*` 类型。
3. `D:\Source\cgmsv\cgmsv_server\callfromcli.c`：业务调用语义和实参来源。
4. IDA MCP 的 `cg2d.exe`：客户端 token 读取顺序、条件分支、payload 分隔和字段用途。
5. 可定位的真实日志/抓包：用于交叉验证，不单独覆盖 C/IDA 的结构证据。
6. 当前 `src/packet`：只作为实现风格参考，不能反向证明协议格式。

`xlsxStatus` 的“完成/进行中/暂缓/待办”是来源表元数据，不是 C++ 实现完成度。

### 3.2 单包 Definition of Ready

一个 code 进入实现前，必须全部满足：

- code、方向、legacy 函数和 typed 类型名已确定；
- 外层字段数量、顺序、名称、`i62/es` 类型已确定；
- 每条字段结论能定位到 xlsx 行、Server C 函数或 IDA 地址/函数；
- 每个 `es/char*` 已完成 payload 判定；
- 零字段包有明确的无参数调用或客户端不取 token 证据；
- 来源冲突已记录并解决；
- schema 的原始字段保持不变，补充 `resolvedFormat`、`resolvedSource`、`resolvedFields` 表达最终证据。

证据不足时，该 code 保持阻塞并直接询问用户，不以 opaque string、`unknownN` 或经验类型推进。

## 4. 实现边界

### 4.1 必须复用

- 生产类型继续放在 `src/packet/Send.hpp`、`src/packet/Receive.hpp`，沿用当前 per-packet struct。
- 外层编解码复用 `PacketBuilder.hpp`、`PacketParser.hpp`、`PacketTypes.hpp`。
- 小颗粒协议工具优先收敛到现有 `src/packet` 文件；不得为少量 helper 新增独立 hpp/cpp。
- payload 复用 `BuildPayload`、`SplitPayload`、`SplitPayloadLegacyAware`；只有旧双字节尾字节会与分隔符冲突时才使用 legacy-aware 版本。
- 错误统一使用 `PacketResult`、`PacketErrorCode`，并补齐 `packetCode`、`fieldIndex`。
- 外层 `i62` 使用 `PacketFieldType::Int62`，typed 值使用 `int`；普通外层 `es` 使用 `EscapedString`，只有确需保留二级 `\z/\c/\y/\n` raw escape 的字段才使用 `SubPayload`。

### 4.2 客户端→服务端

- 显式 `Code/Direction/Fields/Spec`、typed 成员和构造。
- `ToPacket()` 只装配外层字段。
- 生产 `Encode()` 复用 `BuildPacket(ToPacket(), Spec)`。
- 复杂 payload 先由 typed 字段构造 `BuildData()`；不得要求调用方直接提供未经验证的 raw blob。
- 反向 `Parse/FromPacket` 只在 roundtrip 测试确有需要时放在 `CG_PACKET_ENABLE_TEST_HELPERS` 下。

### 4.3 服务端→客户端

- 显式 `Code/Direction/Fields/Spec` 和 typed 成员。
- 生产 `Parse(view)` 复用 `ParsePacket(view, Spec)`，再检查 code、direction、字段数量/名称/类型并构造 typed 值。
- payload 包继续使用私有嵌套 Codec 或同组既有逻辑，不新增全局特例。
- `BuildData()/ToPacket()` 仅用于测试回写时放在 `CG_PACKET_ENABLE_TEST_HELPERS` 下；生产解析依赖的逻辑不得被条件编译掉。

### 4.4 明确禁止

- 生产 `PacketRegistry`、code 自动分发表、无 `Spec` Parse/Build。
- 在通用 Parser/Builder 中增加特定 code 分支。
- schema 生成生产类型或在运行时读取 schema。
- 把 payload 内十进制数误当作外层 i62。
- 顺手拆分巨型头、重构既有 48 份 decimal helper 或迁移旧测试框架。

## 5. 执行阶段

| 阶段 | 工作 | 结束计数 | 阶段门禁 |
|---|---|---:|---|
| M6R.0 | 建立 typed/fixture/release 自动门禁；复核现有 91 包基线 | typed 91/209 | 测试侧 manifest=91；现有 91 的 schema effective 字段可核对；fixture grammar 固定 |
| M6R.1 | 完成 Q0 外层定义可审计的 30 包 | typed 121/209 | 30 包逐包 DoR/DoD 完成，无 payload 漏判 |
| M6R.2 | 查证并完成 Q3 异常定义 8 包 | typed 129/209 | 8 个异常均有 resolved 证据和 typed 实现 |
| M6R.3 | 查证并完成 Q1 unknown 类型 60 包 | typed 189/209 | 119 个 unknown 字段全部消除 |
| M6R.4 | 逆向并完成 active Q2；保留 3 个显式例外 | typed 206/209 | 有效集合达到 send 98、receive 108；`sz/Expire/yp` 保持无 typed/fixture |
| M6R.5 | 补齐剩余历史 fixture 债务与完整 typed/schema 核对 | fixture 206/209 | 历史 fixture 债务归零，实际 fixture 与有效 typed 集合一致 |
| M6R.6 | Debug/Release 全量一致性审计 | 有效 206/209 + 3 例外 | 98 Encode、108 Parse、206 fixture；`sz/Expire/yp` 状态和排除关系精确 |

M6R.5 可在 M6R.1～M6R.4 等待 C/IDA 证据时穿插推进，但不能晚于 M6R.6。每个 packet 始终是独立完成单位，不以分组通过代替单包通过。

### 5.1 M6R.0 基础门禁

1. 新增测试侧 typed manifest，初始显式引用当前 91 个 `Type::Spec`。
2. 用 C++20 `requires/static_assert` 检查：
   - `send` 类型有返回 `PacketResult<std::string>` 的 `Encode()`；
   - `receive` 类型有精确返回 `PacketResult<T>` 的 `Parse(std::string_view)`。
3. manifest code 必须唯一、方向正确，并与 schema 的 effective 字段形状交叉核对。
4. 每完成一个新包，同一提交增加 typed 类型、manifest 条目、schema resolved 元数据、fixture 和单包测试。
5. 新增不匹配 `^cg-packet-.*-test$` 的 production-surface smoke target，显式设置：
   - `CG_PACKET_ENABLE_DIAGNOSTIC_TEXT=0`
   - `CG_PACKET_ENABLE_TEST_HELPERS=0`
   - MSVC 使用 `/GR-`，其他编译器使用 `-fno-rtti`
6. smoke 必须实际编译 send `Encode` 和 receive `Parse`，不能通过暴露 test helper 迁就测试。

typed manifest 只存在于测试侧，不导出 API、不按 code 解析，也不进入 `src/packet`。

### 5.2 fixture 门禁

M6R.0 先固定 `.pkt` v1 grammar，再开始批量添加。文件严格按以下顺序书写，不允许
Trim、空行、注释、未知 key、乱序、尾随空白或缺少最终 LF：

```text
pkt_fixture=1
code=<[A-Za-z0-9]+>
direction=<client_to_server|server_to_client>
sample=<[a-z0-9][a-z0-9_-]*>
plain_hex=<非空大写两位 hex，单空格分隔>
canonical_hex=<非空大写两位 hex，单空格分隔>
[payload_field=<非负十进制，无前导 0，0 合法>]
[payload_hex=<大写两位 hex，单空格分隔，可空>]
end_sample
```

- 每个文件至少 1 个 sample；`sample` 在文件内唯一。`payload_field/payload_hex` 只能成对出现，
  且只能位于 `canonical_hex` 与 `end_sample` 之间；同一文件可继续追加下一完整 sample 块。
- 物理字节只允许 ASCII `0x20..0x7E` 与 LF；BOM、CR、tab、空行和非 ASCII 字节均拒绝。
- `code` 必须精确等于大小写敏感文件 stem，`direction` 必须精确等于父目录名。
  code 不限制历史长度，`SWITMP2` 等字母数字 code 合法。
- `canonical_hex` 还原后必须以 `0x20` 结尾，且全串禁止 `0x00/0x0A/0x0D`。
  `plain_hex` 中这三种控制字节如存在，只能合计出现一次且位于最后一个字节。

- 目录按方向分为：
  - `tests/fixtures/protocol/plain/client_to_server/`
  - `tests/fixtures/protocol/plain/server_to_client/`
- 一 code 一 `.pkt` 文件；文件名精确使用大小写敏感 code。
- `.pkt` 文件物理内容只使用 ASCII 元数据、十六进制数字和 LF 换行，不直接写入非 ASCII 原始字节；ASCII 在 UTF-8 与 GBK 下的字节表示一致，文件本身不存在转码歧义。
- `plain_hex`、`canonical_hex` 表达的旧协议明文字节统一按 GBK 解释；所有非 ASCII 文本样本均先编码为 GBK 字节，再写为 hex。
- fixture reader 只把 hex 还原为精确字节，不做隐式转码；需要 UTF-8/GBK 转换时，只能在既有 `cg::text` 或协议边界显式执行。
- `0x17`、包尾空格、反斜杠、换行和旧双字节内容均按原始字节写入 hex，避免编辑器改写。
- typed 语义断言仍放在单包 C++ 测试，不把业务字段期望塞进通用 fixture 解析器。
- 每条实际 sample 均执行 generic `ParsePacket(input, Spec) -> BuildPacket == canonical`，
  再执行 `ParsePacket(canonical, Spec) -> BuildPacket == canonical`；两次 `Packet` 的
  code、direction 及 fields 的 name/type/value 必须完全一致。此门禁只证明 generic
  Spec roundtrip，不计入 typed `Encode/Parse` 运行覆盖。

fixture inventory 以集合恒等式分阶段收紧。令 `H` 为冻结的 91 个历史 typed code，`D`
为显式 fixture 债务，`A` 为实际 fixture code，`T` 为 typed manifest code，则始终要求：

```text
D ⊆ H
A ∩ D = ∅
A ∪ D = T
payload audit codes = A
```

初始 `H=D=T=91`、`A=0`，因此 plain root 不存在仅在预期 `A=0` 时合法；新 typed code
不能加入历史债务。最终必须达到 `D=0`、`A=T=E=206`，且方向精确为客户端→服务端 98、
服务端→客户端 108；`sz`、`Expire`、`yp` 均不得进入 `A/T`。

payload 状态只以 inventory audit 为权威，不从 `SubPayload` 类型推断：

1. 债务 code 隐含为 `Unaudited`，没有实际 fixture 和 audit。
2. 每个实际 fixture 必须恰有一个 `StructuredPayload(fieldIndex)` 或
   `NoStructuredPayload` audit。
3. `StructuredPayload` 要求 `fieldIndex` 对应 Spec 的 `EscapedString/SubPayload`，
   至少一个 sample 使用相同 `payload_field` 且 `payload_hex` 非空；generic Parse 后该字段
   value 必须逐字节等于 `payload_hex`。
4. `NoStructuredPayload` 的所有 sample 禁止携带 payload 元数据。

fixture 测试读取的是精确字节，不得对包尾空格、空字段或控制字节做 `Trim()`。

## 6. 初始任务队列

以下 code 均为当前缺失的 typed 类型，四个队列合计 118，无重复、无遗漏。

### Q0：外层定义可进入逐包审计，30

- 客户端→服务端，14：
  `emRV, lz, WA, Or, DE, sz, pg, Rg, as, ixH, suI, ih, lO, JMh`
- 服务端→客户端，16：
  `PfM, hPUd, PudO, GmNM, OaHi, SkBD, jmb, MTg, Ov, lSZ, tq, HMNy, LOh, QBt, EZ, dAgg`

推荐先用三个最小纵切验证门禁：`WA`（零字段 send）、`PfM`（单字段 receive）、`emRV`（多字段 send）。三包仍分别验证和提交。

### Q1：字段类型为 unknown，60

- 客户端→服务端，51：
  `KeOX, ik, cEj, IMv, Ak, iVfo, fIM, QpfE, FvWm, yi, fS, kza, LaR, FPkR, vcAz, CZ, uHb, npzi, XIsR, dbQC, HoW, TLKH, dcMv, zn, LKQy, Ukuy, iET, McVa, mjCv, BG, MIS, klF, sM, QeuC, OU, zGp, tj, ACwB, IHw, Noeb, Godf, PGXE, Xoee, ElVN, wAo, IPy, rh, Cpa, SWITM, SWITMP, SWITMP2`
- 服务端→客户端，9：
  `oRut, frQp, bJg, PVfz, ftaN, Tmrx, KXhH, Yx, kiG`

### Q2：format 为空，原始 20，当前 active 17

- 客户端→服务端，4：
  `nB, FVm, rvb`；`yp` 为 `executionStatus=user-skipped`，封包协议状态仍未裁决，不进入 active Q
- 服务端→客户端，16：
  `vWpn, IYo, sZWu, bNY, NdM, hJq, xEg, BHe, kg, sA, TFCu, wiB, murD, Bi, ed, Expire`

其中 `ed` 已完成，`Expire` 已按 `contractStatus=no-effective-wire-contract` 终止；二者与 `yp` 一同从当前 active Q2 扣除，但保留在原始 20 包历史清单中。

### Q3：异常定义，8

- malformed type，1：`zA` 的 `{warpnum::i62}` 当前被解析为 `typeToken=":i62"`，必须查证，不能直接修成 i62。
- 原格式含内容但 schema 未形成字段，4：
  - 客户端→服务端：`eL, NgMy, cZt`
  - 服务端→客户端：`We`
- xlsx 非完成态且当前仅显示零字段，3：
  - 客户端→服务端：`nkda, SWITMM`
  - 服务端→客户端：`GC`

### 现有 91 typed 的 fixture 补债清单

- 客户端→服务端，25：
  `JFVf, Gp, CCL, aSEl, UUN, Ot, RXr, Flvp, kjSK, HJQp, ycX, CV, uSr, xD, IcE, rMsv, PfqV, woz, Ufm, GY, uId, wD, GH, aiAb, ylGN`
- 服务端→客户端，66：
  `whV, Daj, DZ, oy, cJzI, HNP, aPc, UU, Sc, eZT, IUn, DpSY, cNB, ow, KyN, TNT, JJQj, Rspn, cNE, rU, iKe, TgX, ELoy, xmgQ, Ilz, dM, Dn, lDuJ, IsSy, AU, JgO, OBee, Yo, ysVc, LY, ND, nFV, DyAG, Zd, zzd, Gn, hB, LYIm, Mhbt, zdr, BZ, ft, jv, TeD, gnxd, vI, aDn, znv, yuD, Kka, Rll, Cxo, ms, nUbJ, RwK, Yq, PhdQ, nsY, NI, uSB, aBS`

## 7. 单包完成模板

### 7.1 证据小步

- 只处理一个 code。
- 记录 xlsx、Server C、IDA 的精确位置和冲突结论。
- 更新 schema resolved 元数据和协议清单，不改原始 `rawFormat/fields`。
- 验证 subAgent 只读复核来源、方向、字段顺序和类型。
- Q1～Q3 的证据小步独立记录并提交；证据未通过不得进入实现小步。

### 7.2 实现小步

- 新增一个 typed 类型及其 `Spec`。
- 新增一个外部 `.pkt` fixture。
- 新增或更新该 code 的聚焦测试。
- 增加 typed manifest 条目。
- 更新 `plans/progess.md` 累计计数。
- 由独立验证 subAgent 复核后提交。

### 7.3 最低测试矩阵

所有包：

- Spec/schema 的 code、方向、字段顺序/名称/有效类型一致。
- 错 code、缺字段、多字段。
- 主方向：send 必测生产 `Encode`；receive 必测生产 `Parse`。
- generic `ParsePacket → BuildPacket` canonical roundtrip。
- fixture 精确字节输入和 canonical 输出。

按字段追加：

- 每个 i62：正常、负数或边界（协议允许时）、非法字符、溢出。
- 每个 es：空值、空格、反斜杠、换行，以及适用时旧双字节截断。
- 手工 `Packet` API 存在时：错 direction、字段名、字段类型、字段数量。
- payload：空/最小/最大记录、字段缺/多、尾随分隔符、raw escape、旧双字节分隔冲突、十进制非法/溢出、unknown raw 字段无损回写。

不添加没有协议证据的数值范围、枚举或业务校验。

### 7.4 单包 Definition of Done

- DoR 全部满足。
- typed 类型和主方向实现完成。
- payload 判定与所需 codec 完成。
- schema resolved、inventory、fixture、manifest、测试同步。
- `git diff --check` 通过。
- 聚焦目标重新构建并运行通过，不能只运行旧 exe。
- 独立验证 subAgent 通过。
- `plans/progess.md` 已记录并形成该包的独立提交。

## 8. 验证节奏

- M6 全阶段固定复用仓库既有 `build/` 作为 CMake configure/build 目录，不得按 code、sample 或 case 新建独立构建目录。
- 逐包 case 只增加 target/CTest 条目；typed manifest、fixture inventory/roundtrip 与 production-surface smoke 使用共享目标。

### 每包

1. CMake 变更后 configure。
2. 构建该包目标，以及 parser、builder、coverage、fixture 门禁目标。
3. 运行聚焦 CTest。
4. 运行 production-surface smoke。
5. `git diff --check`。

### 每 4～8 包检查点

- Debug 全量构建后运行全部 `^cg-packet-` CTest。
- 从 `--parallel 4` 起步；`Receive.hpp` 已很大，避免过高并行度造成内存尖峰。
- 重新统计 typed、方向、fixture、resolved source 和未决 payload 数量。

### 每阶段

- Debug 全量构建。
- 全部 packet CTest。
- schema/manifest/fixture 三方集合差异必须为空。
- 独立验证 subAgent 复核阶段门禁。

### 最终 M6R.6

- schema：209 唯一 code，100/109 方向。
- typed manifest：有效集合 206 个唯一 `Spec`，98 个 Encode，108 个 Parse。
- fixture：有效集合 206 个 code，每包至少 1 条；payload 包有二级样本。
- 有效集合的 schema effective format/source 无空缺、无 `Unknown`、无 malformed token。
- 有效集合的全部 payload 结构化，或有证据证明为不透明文本并能无损回写。
- terminal 集合精确为 `sz/Expire` 且状态为 `no-effective-wire-contract`；user-skipped 集合精确为 `yp` 且状态为 `user-skipped`。
- terminal/user-skipped code 均无 typed manifest、fixture 和 payload audit；不要求其 resolved format/source/fields 完成。
- Debug 全量 packet CTest 通过。
- Release 构建 `cg-client` 和 production-surface smoke；诊断/test helper 均关闭。
- typed packet 无 `TODO/NotImplemented` 残留。
- 不存在生产 registry、无 Spec API、schema 驱动代码或 RTTI。

只有以上项目全部通过，才把 M6 严格口径标记为完成。

## 9. 进度与提交规则

每个可验证小步都向 `plans/progess.md` 追加一行，固定记录：

```text
typed X/209；Encode X/100；Parse X/109；
fixture X/209；Q0/Q1/Q2/Q3 剩余 X/X/X/X；
terminal X；user-skipped X；有效 typed 缺口 X；原始 typed 缺口 X；
来源未决 X；payload 未审计 X。
```

Q0～Q3 统计对象是初始队列中的“未完成协议 code”，不是提交或子步骤数量。证据小步为某个 code 补齐 `resolvedFormat` 后，该队列剩余数只减少一次；后续 typed/fixture 小步不得再次扣减。每次记录均应以本节初始队列和当前 schema 的 `resolvedFormat` 实际状态重算，不能沿用上一行做手工递减。

`contractStatus=no-effective-wire-contract` 表示该 code 已由用户裁决为无当前有效 wire 合同：它是证据终止状态，从 active Q 队列扣除，但不等价于 exact0，且不要求 `resolvedFormat` 或 `resolvedFields`。

`executionStatus=user-skipped` 只表示用户明确跳过本轮执行：它从 active Q 队列和有效完成集合扣除，但不等价于 exact0、terminal、resolved 或无有效封包协议；除 `yp` 外不得使用该状态。

`来源未决` 与 `payload 未审计` 的历史账当前没有对应的固定 code 集合，暂时不能仅从 schema/fixture 独立重算；在补齐可执行门禁前，进度行必须注明为沿用值，不得把它们作为自动完成判据。

提交粒度：

- M6R.0 的每个基础门禁各自提交。
- Q0 通常一个 code 一个“实现+测试+fixture”提交。
- Q1～Q3 每个 code 最多拆为：
  1. `M6R <code> 协议证据与 schema 确认`
  2. `M6R <code> typed 实现与验证`
- 现有 91 包按一个 code 一个 fixture 补债提交。
- 不把多个未独立验证的 code 合成一个提交。

## 10. 主要风险与阻塞策略

1. 旧 payload 清单来自关键词规则；例如 `Rg {cmd:es}` 仍必须审计，不能因“不疑似 payload”直接按普通字符串关闭。
2. Q1 含 119 个 unknown 字段；按字段名推断 int/string 会制造不可追溯协议。
3. Q2 包含历史迁移、GM、人数、音效等低频 code；“暂缓/废弃”不能替代字段证据。
4. `Send.hpp/Receive.hpp` 已很大；本轮继续现有组织方式，不顺带拆文件。只有实际构建成为阻塞时，另开计划决策。
5. 当前 95 个 packet target 依赖大量手工 CMake；M6R 不批量重构旧目标。若新增目标的 warning 配置开始漂移，只做局部、可验证的 CMake 收敛。
6. Server C 和 IDA 仍无法确定格式时，该 code 明确保持 BLOCKED 并向用户报告证据缺口，不降级完成标准。

## 11. 首个执行序列

1. `M6R.0.1a-k`：逐包复核 `aPc/UU/Sc/NI/eZT/IUn/DpSY/cNB/ow/DyAG/KyN` 的非标准或 unknown schema token，并补 resolved 来源；每个 code 分别验证、提交。
2. `M6R.0.2`：从 0 建立当前 91 类型的测试侧显式 typed manifest，并完成 91/91 schema effective 交叉核对；45/57 仅是历史手写 `FindPacket` 分支数，不是 manifest 基线。
3. `M6R.0.3`：确定 `.pkt` grammar，建立 fixture inventory/roundtrip 门禁。
4. `M6R.0.4`：建立 diagnostics=0、test helpers=0、RTTI=off 的 production-surface smoke。
5. `M6R.1.01`：`WA` 零字段 send。
6. `M6R.1.02`：`PfM` 单字段 receive。
7. `M6R.1.03`：`emRV` 多字段 send。

前三个新包分别验证零字段、receive Parse、send Encode 和多字段 fixture；通过后再按 Q0 清单顺序推进。
