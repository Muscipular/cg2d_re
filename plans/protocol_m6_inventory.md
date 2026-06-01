# M6.0 协议资料盘点

## 成功标准
- 从 `plans/CG封包.xlsx` 主表 `封包` 读取 209 行并生成统计与清单。
- format 非空的来源标 `xlsx`；format 为空的来源标 `待 C/IDA 补全`，不猜测字段。
- 比对现有 `src/packet/` 原型定义，记录已覆盖封包和对 M6 后续计划的影响。
- 本步只写入文档，不编译、不提交、不修改 C++ 源码。

## 资料来源优先级
1. `plans/CG封包.xlsx` 主表 `封包`：主来源，统计和清单字段均以该表为准。
2. `D:\Source\cgmsv\cgmsv_server\nrproto_serv.c` 与 `D:\Source\cgmsv\cgmsv_server\include\nrproto_serv.h`：仅在 format 为空时辅助判断 C send/recv 参数是否含 `char*` 或 `array`。
3. `src/packet/`：当前仓库已有 C++ 原型，仅用于确定 M6 实现边界和已存在的封包结构，不覆盖 xlsx 字段事实。
4. IDA：本步未使用；format 为空的封包仍统一标 `待 C/IDA 补全`。

## 统计总数
| 项 | 数量 |
|---|---:|
| 封包总数 | 209 |
| 疑似内嵌 payload | 88 |
| 非疑似内嵌 payload | 121 |

## 方向统计
| 方向 | 数量 |
|---|---:|
| 客户端->服务端 | 100 |
| 服务端->客户端 | 109 |

## 状态统计
| 状态 | 数量 |
|---|---:|
| 完成 | 161 |
| 进行中 | 25 |
| 暂缓 | 18 |
| 待办 | 5 |

## 封包格式空/非空统计
| format 状态 | 数量 |
|---|---:|
| 非空 | 156 |
| 空 | 53 |

## 疑似内嵌 payload 判定规则
- format 或说明包含 `data/message/text/name/items/pets/gold/buff/cdata` 时标记为是，大小写不敏感。
- format 为空且对应 C send/recv 参数含 `char*` 或 `array` 时标记为是。
- 上述规则只用于盘点风险提示，不补全字段格式。

## 现有 `src/packet/` 覆盖
| 文件 | 命名空间 | 方向语义 | 当前内容 | M6 影响 |
|---|---|---|---|---|
| `src/packet/send.hpp` | `cg::packet::send` | 客户端发送给服务端，对应旧 C `RECV_*` | `ClientLogin`，code `JFVf`，字段 `{clientHash:es} {password:es} {account:es}`，`Encode()` 未实现 | M6 应保留客户端视角命名，schema 字段顺序以注释/xlsx 为准，不以构造函数参数顺序推断 |
| `src/packet/receive.hpp` | `cg::packet::receive` | 客户端接收服务端封包，对应旧 C `SEND_*` | `ClientLoginResult`，code `whV`，字段 `{result:i62} {data:es}`，含登录结果枚举，`Parse()` 未实现；`data` 注释出现空值、`fail`、`block`、`busy` | M6.15 登录/角色管理封包以该结构为第一批迁移对象 |
| `src/packet/utils.hpp` | `cg::packet` | 基础 codec | base62 与字符串 encode/decode 为 TODO | M6.2/M6.3 应先落地到独立 codec，再由具体封包复用 |
| `src/packet/packet.hpp` | `cg::packet` | 聚合头 | 引入 `utils.hpp`、`receive.hpp`、`send.hpp` | M6 继续使用 `src/packet` 作为协议模块目录，M9 再接入 `src/net` 连接层 |

当前 `src/packet` 只覆盖 2 个草稿封包，未形成 209 个封包注册表；`rg` 只找到 `packet.hpp` 内部 include，未发现 CMake 或业务代码接入。实现 M6 时不得把该目录视为完整协议事实来源。

已知草稿风险：`receive.hpp` 使用 `std::string_view` 但未包含 `<string_view>`；`Parse()` 返回 `{}`，当前类型无默认构造。该风险只记录到 M6 计划，不在本步修改 C++。

## 现有 `src/packet/` 封包清单
| code | 方向 | 现有类型 | format | 当前状态 |
|---|---|---|---|---|
| `JFVf` | 客户端->服务端 | `cg::packet::send::ClientLogin` | `{clientHash:es} {password:es} {account:es}` | 结构已存在，Encode 待实现 |
| `whV` | 服务端->客户端 | `cg::packet::receive::ClientLoginResult` | `{result:i62} {data:es}` | 结构与结果枚举已存在，Parse 待实现；`data` 可能为空或为 `fail/block/busy` |

## 209 个封包清单
| 序号 | code | 方向 | 状态 | 说明 | format 是否为空 | 是否疑似内嵌 payload | 来源 |
|---:|---|---|---|---|---|---|---|
| 1 | `JFVf` | 客户端->服务端 | 完成 | 账号密码 | 否 | 否 | xlsx + src/packet |
| 2 | `Gp` | 客户端->服务端 | 完成 | RECV_CharList 获取角色列表 | 否 | 否 | xlsx |
| 3 | `emRV` | 客户端->服务端 | 完成 | RECV_CharLogin 登录 | 否 | 否 | xlsx |
| 4 | `CCL` | 客户端->服务端 | 完成 | RECV_CheckCommand 发送客户端时间 | 否 | 否 | xlsx |
| 5 | `lz` | 客户端->服务端 | 完成 | RECV_PRO CHAR_setProfileStatus 简介开关 | 否 | 否 | xlsx |
| 6 | `WA` | 客户端->服务端 | 完成 | RECV_CharLogout 登出 | 否 | 否 | xlsx |
| 7 | `whV` | 服务端->客户端 | 完成 | SEND_ClientLogin 登录角色表结果 | 否 | 是 | xlsx + src/packet |
| 8 | `PfM` | 服务端->客户端 | 完成 | SEND_PV 版本信息？ | 否 | 否 | xlsx |
| 9 | `Daj` | 服务端->客户端 | 完成 | SEND_CharList 登录角色表 | 否 | 是 | xlsx |
| 10 | `HNP` | 服务端->客户端 | 完成 | SEND_PRV <br>CHAR_sendProfileVersion简介版本内容 | 否 | 是 | xlsx |
| 11 | `Zd` | 服务端->客户端 | 完成 | SEND_STK  SYSTEMMSG 系统信息 | 否 | 是 | xlsx |
| 12 | `hPUd` | 服务端->客户端 | 完成 | SEND_CharLogin 登录结果 | 否 | 否 | xlsx |
| 13 | `PudO` | 服务端->客户端 | 完成 | SEND_ClientIndex | 否 | 否 | xlsx |
| 14 | `GmNM` | 服务端->客户端 | 完成 | SEND_CC CHAR_sendCoordinate | 否 | 否 | xlsx |
| 15 | `TeD` | 服务端->客户端 | 完成 | SEND_MN SendMapName | 否 | 是 | xlsx |
| 16 | `aPc` | 服务端->客户端 | 完成 | SEND_CP CharData_1 | 否 | 是 | xlsx |
| 17 | `UU` | 服务端->客户端 | 完成 | SEND_CP2 CharData_2 | 否 | 是 | xlsx |
| 18 | `jv` | 服务端->客户端 | 完成 | SEND_CJ SendJobName | 否 | 是 | xlsx |
| 19 | `cNE` | 服务端->客户端 | 完成 | SEND_CS CHAR_sendSkillData | 否 | 是 | xlsx |
| 20 | `Sc` | 服务端->客户端 | 完成 | SEND_TITLE | 否 | 是 | xlsx |
| 21 | `rU` | 服务端->客户端 | 完成 | SEND_I CHAR_sendItemData | 否 | 是 | xlsx |
| 22 | `OaHi` | 服务端->客户端 | 完成 | SEND_EP 遇敌率 | 否 | 否 | xlsx |
| 23 | `SkBD` | 服务端->客户端 | 完成 | SEND_FS 发送玩家状态开关 | 否 | 否 | xlsx |
| 24 | `jmb` | 服务端->客户端 | 完成 | SEND_MC sendMapHash | 否 | 否 | xlsx |
| 25 | `gnxd` | 服务端->客户端 | 完成 | SEND_AB SendAddressbook | 否 | 是 | xlsx |
| 26 | `PhdQ` | 服务端->客户端 | 完成 | SEND_ABG SendAddressbookGuildInfo | 否 | 是 | xlsx |
| 27 | `nsY` | 服务端->客户端 | 完成 | SEND_C ObjectList | 否 | 是 | xlsx |
| 28 | `MTg` | 服务端->客户端 | 完成 | SEND_BT CHAR_sendFeverStatus | 否 | 否 | xlsx |
| 29 | `Ov` | 服务端->客户端 | 完成 | SEND_POS 前后排 | 否 | 否 | xlsx |
| 30 | `Ilz` | 服务端->客户端 | 完成 | SEND_AL 图鉴？ | 否 | 是 | xlsx |
| 31 | `lSZ` | 服务端->客户端 | 完成 | SEND_IP | 否 | 否 | xlsx |
| 32 | `tq` | 服务端->客户端 | 完成 | SEND_MAC Mouse? | 否 | 否 | xlsx |
| 33 | `zzd` | 服务端->客户端 | 完成 | SEND_EF | 否 | 是 | xlsx |
| 34 | `NI` | 服务端->客户端 | 完成 | SEND_CA 发送头顶图标 | 否 | 是 | xlsx |
| 35 | `HMNy` | 服务端->客户端 | 完成 | SEND_CharLogout | 否 | 否 | xlsx |
| 36 | `Or` | 客户端->服务端 | 完成 | Ping | 否 | 否 | xlsx |
| 37 | `DE` | 客户端->服务端 | 完成 | RECV_FCTime, 定时心跳包，视服务器setup.cf配置而定 {fctime}秒内心跳{fccount}次 | 否 | 否 | xlsx |
| 38 | `LOh` | 服务端->客户端 | 完成 | Pong | 否 | 否 | xlsx |
| 39 | `Ot` | 客户端->服务端 | 完成 | RECV_PRS SetProfileData 简介内容 | 否 | 是 | xlsx |
| 40 | `aBS` | 服务端->客户端 | 完成 | SEND_MAP_DATA | 否 | 是 | xlsx |
| 41 | `UUN` | 客户端->服务端 | 完成 | RECV_M MAP_getdataFromRECT 获取地图信息 | 否 | 是 | xlsx |
| 42 | `zA` | 客户端->服务端 | 完成 | RECV_W 移动 | 否 | 否 | xlsx |
| 43 | `uSB` | 服务端->客户端 | 完成 | SEND_CD 发送删除ObjectIndex列表 | 否 | 是 | xlsx |
| 44 | `QBt` | 服务端->客户端 | 完成 | SEND_XYD | 否 | 否 | xlsx |
| 45 | `sz` | 客户端->服务端 | 完成 | RECV_w 移动 | 否 | 否 | xlsx |
| 46 | `pg` | 客户端->服务端 | 完成 | RECV_EV NPC事件 | 否 | 否 | xlsx |
| 47 | `KeOX` | 客户端->服务端 | 完成 | RECV_EN 客户端遇敌 | 否 | 否 | xlsx |
| 48 | `ik` | 客户端->服务端 | 完成 | RECV_DU PK请求 | 否 | 否 | xlsx |
| 49 | `eL` | 客户端->服务端 | 完成 | RECV_EO 离开战斗界面 | 否 | 否 | xlsx |
| 50 | `NgMy` | 客户端->服务端 | 完成 | RECV_BU 停止观战 | 否 | 否 | xlsx |
| 51 | `EZ` | 服务端->客户端 | 完成 | SEND_EV Revc_EV的返回 | 否 | 否 | xlsx |
| 52 | `dAgg` | 服务端->客户端 | 完成 | SEND_EN 进入战斗 | 否 | 否 | xlsx |
| 53 | `Gn` | 服务端->客户端 | 完成 | SEND_RS 战斗获得物品 | 是 | 是 | 待 C/IDA 补全 |
| 54 | `hB` | 服务端->客户端 | 完成 | SEND_RD BATTLE_GetDuelPoint | 是 | 是 | 待 C/IDA 补全 |
| 55 | `LYIm` | 服务端->客户端 | 完成 | SEND_B | 是 | 是 | 待 C/IDA 补全 |
| 56 | `ELoy` | 服务端->客户端 | 暂缓 | SEND_IA 道具信息相关 | 是 | 是 | 待 C/IDA 补全 |
| 57 | `vWpn` | 服务端->客户端 | 完成 | SEND_SI moveitem | 是 | 否 | 待 C/IDA 补全 |
| 58 | `xmgQ` | 服务端->客户端 | 完成 | SEND_IR CHAR_sendItemRecipe | 是 | 是 | 待 C/IDA 补全 |
| 59 | `Mhbt` | 服务端->客户端 | 完成 | SEND_MSG recvmsg | 是 | 是 | 待 C/IDA 补全 |
| 60 | `dM` | 服务端->客户端 | 完成 | SEND_ALI | 是 | 是 | 待 C/IDA 补全 |
| 61 | `Dn` | 服务端->客户端 | 完成 | SEND_ALN | 是 | 是 | 待 C/IDA 补全 |
| 62 | `IYo` | 服务端->客户端 | 完成 | SEND_ALO | 是 | 否 | 待 C/IDA 补全 |
| 63 | `lDuJ` | 服务端->客户端 | 完成 | SEND_PME 宠邮动画 | 否 | 是 | xlsx |
| 64 | `vI` | 服务端->客户端 | 完成 | SEND_ABI ADDRESSBOOK_sendAddressbookTableOne | 否 | 是 | xlsx |
| 65 | `GC` | 服务端->客户端 | 进行中 | SEND_GC addGuildMember 加入家族 | 否 | 否 | xlsx |
| 66 | `aDn` | 服务端->客户端 | 进行中 | SEND_GI guildinfo | 否 | 是 | xlsx |
| 67 | `znv` | 服务端->客户端 | 进行中 | SEND_GT guildtitle | 否 | 是 | xlsx |
| 68 | `yuD` | 服务端->客户端 | 进行中 | SEND_GM guildmember | 否 | 是 | xlsx |
| 69 | `Kka` | 服务端->客户端 | 进行中 | SEND_GMI guildmemberinfo | 否 | 是 | xlsx |
| 70 | `oRut` | 服务端->客户端 | 进行中 | SEND_RGM 开除回执 | 否 | 否 | xlsx |
| 71 | `Rll` | 服务端->客户端 | 进行中 | SEND_GML 家族邮件 | 否 | 是 | xlsx |
| 72 | `frQp` | 服务端->客户端 | 进行中 | SEND_GD 退出或解散家族 | 否 | 否 | xlsx |
| 73 | `eZT` | 服务端->客户端 | 进行中 | SEND_PRL 收到profile list | 否 | 是 | xlsx |
| 74 | `IUn` | 服务端->客户端 | 进行中 | SEND_PRA 收到about list | 否 | 是 | xlsx |
| 75 | `DpSY` | 服务端->客户端 | 进行中 | SEND_PRD profile data | 否 | 是 | xlsx |
| 76 | `cNB` | 服务端->客户端 | 进行中 | SEND_PRE 删除迷你邮件成员 | 否 | 是 | xlsx |
| 77 | `ow` | 服务端->客户端 | 进行中 | SEND_PRM 迷你信件回执? | 否 | 是 | xlsx |
| 78 | `DyAG` | 服务端->客户端 | 进行中 | SEND_PRW 交流版 | 否 | 是 | xlsx |
| 79 | `KyN` | 服务端->客户端 | 进行中 | SEND_PRAD 收到迷你信件 | 否 | 是 | xlsx |
| 80 | `zdr` | 服务端->客户端 | 完成 | SEND_TK | 是 | 是 | 待 C/IDA 补全 |
| 81 | `IsSy` | 服务端->客户端 | 完成 | SEND_KP | 是 | 是 | 待 C/IDA 补全 |
| 82 | `AU` | 服务端->客户端 | 完成 | SEND_KP2 | 是 | 是 | 待 C/IDA 补全 |
| 83 | `TNT` | 服务端->客户端 | 完成 | SEND_PP | 是 | 是 | 待 C/IDA 补全 |
| 84 | `JJQj` | 服务端->客户端 | 完成 | SEND_CN | 是 | 是 | 待 C/IDA 补全 |
| 85 | `Rspn` | 服务端->客户端 | 完成 | SEND_CT | 是 | 是 | 待 C/IDA 补全 |
| 86 | `OBee` | 服务端->客户端 | 完成 | SEND_PT | 是 | 是 | 待 C/IDA 补全 |
| 87 | `Yo` | 服务端->客户端 | 暂缓 | SEND_S | 是 | 是 | 待 C/IDA 补全 |
| 88 | `sZWu` | 服务端->客户端 | 暂缓 | SEND_HL | 是 | 否 | 待 C/IDA 补全 |
| 89 | `bNY` | 服务端->客户端 | 完成 | SEND_PR | 是 | 否 | 待 C/IDA 补全 |
| 90 | `ysVc` | 服务端->客户端 | 完成 | SEND_GFL CHAR_sendFrontCharList | 是 | 是 | 待 C/IDA 补全 |
| 91 | `ND` | 服务端->客户端 | 完成 | SEND_GPD CHAR_sendCharAndPetList | 是 | 是 | 待 C/IDA 补全 |
| 92 | `LY` | 服务端->客户端 | 完成 | SEND_GFLI CHAR_sendFrontCharList | 是 | 是 | 待 C/IDA 补全 |
| 93 | `nFV` | 服务端->客户端 | 完成 | SEND_GPDI CHAR_sendCharAndPetList | 是 | 是 | 待 C/IDA 补全 |
| 94 | `JgO` | 服务端->客户端 | 完成 | SEND_TU techuse | 是 | 是 | 待 C/IDA 补全 |
| 95 | `Cxo` | 服务端->客户端 | 完成 | SEND_TRPL CHAR_Trade_getPlayerList 获得可交易人员列表 | 是 | 是 | 待 C/IDA 补全 |
| 96 | `ms` | 服务端->客户端 | 完成 | SEND_TRS CHAR_Trade_startTrade 被交易 | 是 | 是 | 待 C/IDA 补全 |
| 97 | `NdM` | 服务端->客户端 | 完成 | SEND_TROP CHAR_Trade_setTradeList 对方取出物品 | 是 | 否 | 待 C/IDA 补全 |
| 98 | `nUbJ` | 服务端->客户端 | 完成 | SEND_TRLI CHAR_Trade_setTradeList 对方物品信息 | 是 | 是 | 待 C/IDA 补全 |
| 99 | `hJq` | 服务端->客户端 | 完成 | SEND_TRLG CHAR_Trade_setTradeList 对方金币信息 | 是 | 否 | 待 C/IDA 补全 |
| 100 | `RwK` | 服务端->客户端 | 完成 | SEND_TRLP CHAR_Trade_setTradeList 对法宠物信息 | 是 | 是 | 待 C/IDA 补全 |
| 101 | `Yq` | 服务端->客户端 | 完成 | SEND_TRLPS CHAR_Trade_setTradeList 对方宠物技能信息 | 是 | 是 | 待 C/IDA 补全 |
| 102 | `xEg` | 服务端->客户端 | 暂缓 | SEND_TRCL CHAR_Trade_closeItem | 是 | 否 | 待 C/IDA 补全 |
| 103 | `We` | 服务端->客户端 | 完成 | SEND_TROC | 否 | 否 | xlsx |
| 104 | `BHe` | 服务端->客户端 | 暂缓 | SEND_PS | 是 | 否 | 待 C/IDA 补全 |
| 105 | `kg` | 服务端->客户端 | 完成 | SEND_LVUP | 是 | 否 | 待 C/IDA 补全 |
| 106 | `sA` | 服务端->客户端 | 完成 | SEND_PLVUP | 是 | 否 | 待 C/IDA 补全 |
| 107 | `BZ` | 服务端->客户端 | 完成 | SEND_WN | 否 | 是 | xlsx |
| 108 | `TFCu` | 服务端->客户端 | 完成 | SEND_SE | 是 | 否 | 待 C/IDA 补全 |
| 109 | `wiB` | 服务端->客户端 | 完成 | SEND_BGMW | 是 | 否 | 待 C/IDA 补全 |
| 110 | `bJg` | 服务端->客户端 | 完成 | SEND_PC send_palette 变换调色板 | 否 | 否 | xlsx |
| 111 | `PVfz` | 服务端->客户端 | 完成 | SEND_SH 可移动动画（船，鲸鱼） | 否 | 否 | xlsx |
| 112 | `murD` | 服务端->客户端 | 完成 | SEND_PLAYSE | 是 | 否 | 待 C/IDA 补全 |
| 113 | `ftaN` | 服务端->客户端 | 完成 | SEND_ES NPC_sePlayMySePlay | 否 | 否 | xlsx |
| 114 | `DZ` | 服务端->客户端 | 完成 | SEND_CreateNewChar | 是 | 是 | 待 C/IDA 补全 |
| 115 | `oy` | 服务端->客户端 | 完成 | SEND_CharDelete | 是 | 是 | 待 C/IDA 补全 |
| 116 | `cJzI` | 服务端->客户端 | 暂缓 | SEND_ProcGet | 是 | 是 | 待 C/IDA 补全 |
| 117 | `Bi` | 服务端->客户端 | 暂缓 | SEND_PlayerNumGet | 是 | 否 | 待 C/IDA 补全 |
| 118 | `ed` | 服务端->客户端 | 暂缓 | SEND_PVUP 历史迁移封包 | 是 | 否 | 待 C/IDA 补全 |
| 119 | `Expire` | 服务端->客户端 | 暂缓 | SEND_Expire | 是 | 否 | 待 C/IDA 补全 |
| 120 | `Tmrx` | 服务端->客户端 | 完成 | SEND_StallStart | 否 | 否 | xlsx |
| 121 | `KXhH` | 服务端->客户端 | 完成 | SEND_StallEnd | 否 | 否 | xlsx |
| 122 | `ft` | 服务端->客户端 | 完成 | SEND_StallBrowse | 否 | 是 | xlsx |
| 123 | `Yx` | 服务端->客户端 | 完成 | SEND_StallBuy | 否 | 否 | xlsx |
| 124 | `iKe` | 服务端->客户端 | 完成 | SEND_PET_ITEM | 是 | 是 | 待 C/IDA 补全 |
| 125 | `kiG` | 服务端->客户端 | 待办 | SEND_PET_ITEM_SWAP 在人或宠物间移动装备 | 否 | 否 | xlsx |
| 126 | `TgX` | 服务端->客户端 | 待办 | SEND_EQUIP_SUIT | 否 | 是 | xlsx |
| 127 | `cEj` | 客户端->服务端 | 完成 | RECV_JB | 否 | 否 | xlsx |
| 128 | `IMv` | 客户端->服务端 | 完成 | RECV_LB WatchTry 触发一次Watch？会重置玩家位置到x, y, Δx,Δy>1时会发送XYD封包重置玩家位置 | 否 | 否 | xlsx |
| 129 | `Rg` | 客户端->服务端 | 完成 | RECV_B BattleCommand 战斗指令 | 否 | 否 | xlsx |
| 130 | `Ak` | 客户端->服务端 | 完成 | RECV_ID ITEM_USE 使用物品 | 否 | 否 | xlsx |
| 131 | `iVfo` | 客户端->服务端 | 完成 | RECV_IDF ITEM_useRecovery_Field 使用回复物品 | 否 | 否 | xlsx |
| 132 | `fIM` | 客户端->服务端 | 完成 | RECV_PI PickUpItem 捡起物品 | 否 | 否 | xlsx |
| 133 | `QpfE` | 客户端->服务端 | 完成 | RECV_DI DropItem 丢物品 | 否 | 否 | xlsx |
| 134 | `woz` | 客户端->服务端 | 完成 | RECV_DG DropGold 丢金币 | 否 | 是 | xlsx |
| 135 | `FvWm` | 客户端->服务端 | 完成 | RECV_DP DropPet 丢宠物 | 否 | 否 | xlsx |
| 136 | `yi` | 客户端->服务端 | 完成 | RECV_MI MoveItem 移动物品 | 否 | 否 | xlsx |
| 137 | `fS` | 客户端->服务端 | 完成 | RECV_IR RequestItemRecipe 获取配方 | 否 | 否 | xlsx |
| 138 | `CV` | 客户端->服务端 | 完成 | RECV_MSG 发送邮件信息 触发SEND_MSG | 否 | 是 | xlsx |
| 139 | `kza` | 客户端->服务端 | 完成 | RECV_ALI 请求图鉴数据 触发SEND_ALI | 否 | 否 | xlsx |
| 140 | `LaR` | 客户端->服务端 | 完成 | RECV_ALN 请求图鉴名字 触发 SEND_ALN | 否 | 否 | xlsx |
| 141 | `rMsv` | 客户端->服务端 | 完成 | RECV_PMSG 发送宠邮 | 否 | 是 | xlsx |
| 142 | `FPkR` | 客户端->服务端 | 完成 | RECV_DAB 删除名片 | 否 | 否 | xlsx |
| 143 | `vcAz` | 客户端->服务端 | 完成 | RECV_AAB 交互名片 | 否 | 否 | xlsx |
| 144 | `Ufm` | 客户端->服务端 | 进行中 | RECV_GI 变更家族信息 | 否 | 是 | xlsx |
| 145 | `GY` | 客户端->服务端 | 进行中 | RECV_GT 设置家族职位信息 | 否 | 是 | xlsx |
| 146 | `CZ` | 客户端->服务端 | 进行中 | RECV_GMR 请求家族成员列表 | 否 | 否 | xlsx |
| 147 | `uHb` | 客户端->服务端 | 进行中 | RECV_BGT  变更家族成员职位 | 否 | 否 | xlsx |
| 148 | `npzi` | 客户端->服务端 | 完成 | RECV_AGM 家族邀请 | 否 | 否 | xlsx |
| 149 | `XIsR` | 客户端->服务端 | 进行中 | RECV_LG 离开家族 | 否 | 否 | xlsx |
| 150 | `dbQC` | 客户端->服务端 | 进行中 | RECV_RGM 移除家族成员 | 否 | 否 | xlsx |
| 151 | `uId` | 客户端->服务端 | 进行中 | RECV_GML 家族邮件 | 否 | 是 | xlsx |
| 152 | `PfqV` | 客户端->服务端 | 进行中 | RECV_PGML 宠邮 | 否 | 是 | xlsx |
| 153 | `nkda` | 客户端->服务端 | 进行中 | RECV_GD 解散家族 | 否 | 否 | xlsx |
| 154 | `HoW` | 客户端->服务端 | 待办 | RECV_PRV 获取profile | 否 | 否 | xlsx |
| 155 | `TLKH` | 客户端->服务端 | 进行中 | RECV_PRD 关闭stall后发送 | 否 | 否 | xlsx |
| 156 | `wD` | 客户端->服务端 | 待办 | RECV_PRE 删除迷你邮件好友 | 否 | 是 | xlsx |
| 157 | `GH` | 客户端->服务端 | 待办 | RECV_PRM 发送迷你邮件 | 否 | 是 | xlsx |
| 158 | `nB` | 客户端->服务端 | 暂缓 | RECV_L | 是 | 否 | 待 C/IDA 补全 |
| 159 | `uSr` | 客户端->服务端 | 完成 | RECV_TK 发送聊天 | 否 | 是 | xlsx |
| 160 | `dcMv` | 客户端->服务端 | 完成 | RECV_FS 战易聊队状态 | 否 | 否 | xlsx |
| 161 | `FVm` | 客户端->服务端 | 暂缓 | RECV_HL | 是 | 否 | 待 C/IDA 补全 |
| 162 | `zn` | 客户端->服务端 | 完成 | RECV_PR 进出队伍 | 否 | 否 | xlsx |
| 163 | `LKQy` | 客户端->服务端 | 完成 | RECV_KS PET_SelectBattleEntryPet 宠物战斗状态 | 否 | 否 | xlsx |
| 164 | `Ukuy` | 客户端->服务端 | 完成 | RECV_MP PET_MovePet 移动宠物 | 否 | 否 | xlsx |
| 165 | `iET` | 客户端->服务端 | 完成 | RECV_GFL CHAR_sendFrontCharList 技能角色列表 | 否 | 否 | xlsx |
| 166 | `McVa` | 客户端->服务端 | 完成 | RECV_GPD CHAR_sendCharAndPetList 技能角色宠物列表 | 否 | 否 | xlsx |
| 167 | `as` | 客户端->服务端 | 完成 | RECV_GFLI CHAR_sendFrontCharList 物品角色列表 | 否 | 否 | xlsx |
| 168 | `mjCv` | 客户端->服务端 | 完成 | RECV_GPDI CHAR_sendCharAndPetList 物品角色宠物列表 | 否 | 否 | xlsx |
| 169 | `BG` | 客户端->服务端 | 完成 | RECV_IH Item_useHanko 盖章 | 否 | 否 | xlsx |
| 170 | `MIS` | 客户端->服务端 | 完成 | RECV_AC 角色动作 | 否 | 否 | xlsx |
| 171 | `klF` | 客户端->服务端 | 完成 | RECV_ACS 生产技能图标 | 否 | 否 | xlsx |
| 172 | `RXr` | 客户端->服务端 | 暂缓 | RECV_MU | 是 | 是 | 待 C/IDA 补全 |
| 173 | `sM` | 客户端->服务端 | 完成 | RECV_TU tech_use 使用技能 | 否 | 否 | xlsx |
| 174 | `ixH` | 客户端->服务端 | 完成 | RECV_TRPL 请求交易 | 否 | 否 | xlsx |
| 175 | `QeuC` | 客户端->服务端 | 完成 | RECV_TRS CHAR_Trade_startTrade 从交易列表选择交易对象 | 否 | 否 | xlsx |
| 176 | `aiAb` | 客户端->服务端 | 完成 | RECV_TROP CHAR_Trade_setTradeList 取出物品 | 否 | 是 | xlsx |
| 177 | `suI` | 客户端->服务端 | 完成 | RECV_TRCL CHAR_Trade_closeItem 取消交易 | 否 | 否 | xlsx |
| 178 | `OU` | 客户端->服务端 | 完成 | RECV_TROC CHAR_Trade_closeTrade 确认物品 | 否 | 否 | xlsx |
| 179 | `Flvp` | 客户端->服务端 | 暂缓 | RECV_PS | 是 | 是 | 待 C/IDA 补全 |
| 180 | `zGp` | 客户端->服务端 | 完成 | RECV_ST CHAR_selectTitle 设置称号 | 否 | 否 | xlsx |
| 181 | `tj` | 客户端->服务端 | 完成 | RECV_DT CHAR_deleteTitle 删除称号 | 否 | 否 | xlsx |
| 182 | `ACwB` | 客户端->服务端 | 完成 | RECV_FT CHAR_inputOwnTitle 自定义称号 | 否 | 否 | xlsx |
| 183 | `IHw` | 客户端->服务端 | 完成 | RECV_LVUP 人物加点 | 否 | 否 | xlsx |
| 184 | `kjSK` | 客户端->服务端 | 完成 | RECV_PLVUP 宠物加点 | 否 | 是 | xlsx |
| 185 | `Noeb` | 客户端->服务端 | 完成 | RECV_SKSW SKILL_swap 人物技能移动 | 否 | 否 | xlsx |
| 186 | `HJQp` | 客户端->服务端 | 完成 | RECV_PSSW SKILL_swapPetSkill 宠物技能移动 | 否 | 是 | xlsx |
| 187 | `ih` | 客户端->服务端 | 完成 | RECV_POS CHAR_Position 战斗位置 | 否 | 否 | xlsx |
| 188 | `ycX` | 客户端->服务端 | 完成 | RECV_KN CHAR_inputUserPetName 宠物姓名 | 否 | 是 | xlsx |
| 189 | `xD` | 客户端->服务端 | 完成 | RECV_WN 打开系统窗口 | 否 | 是 | xlsx |
| 190 | `cZt` | 客户端->服务端 | 完成 | RECV_SP 离队时发送 | 否 | 否 | xlsx |
| 191 | `aSEl` | 客户端->服务端 | 完成 | RECV_CreateNewChar 创建角色 | 否 | 是 | xlsx |
| 192 | `Godf` | 客户端->服务端 | 完成 | RECV_CharDelete 删除角色 | 否 | 否 | xlsx |
| 193 | `yp` | 客户端->服务端 | 暂缓 | RECV_ProcGet gm命令？ | 是 | 否 | 待 C/IDA 补全 |
| 194 | `rvb` | 客户端->服务端 | 暂缓 | RECV_PlayerNumGet | 是 | 否 | 待 C/IDA 补全 |
| 195 | `IcE` | 客户端->服务端 | 暂缓 | RECV_Shutdown | 是 | 是 | 待 C/IDA 补全 |
| 196 | `PGXE` | 客户端->服务端 | 完成 | RECV_CH cheat 检测作弊 | 否 | 否 | xlsx |
| 197 | `lO` | 客户端->服务端 | 完成 | RECV_CharLoginGate 回登陆点 | 否 | 否 | xlsx |
| 198 | `Xoee` | 客户端->服务端 | 暂缓 | RECV_PVUP 历史迁移封包 | 否 | 否 | xlsx |
| 199 | `ylGN` | 客户端->服务端 | 完成 | RECV_StallStart 开始摆摊请求 | 否 | 是 | xlsx |
| 200 | `JMh` | 客户端->服务端 | 完成 | RECV_StallEnd 结束摆摊请求 | 否 | 否 | xlsx |
| 201 | `ElVN` | 客户端->服务端 | 完成 | RECV_StallBrowse 摆摊浏览请求 | 否 | 否 | xlsx |
| 202 | `wAo` | 客户端->服务端 | 完成 | RECV_StallBuy 摆摊购买请求 | 否 | 否 | xlsx |
| 203 | `IPy` | 客户端->服务端 | 完成 | RECV_ProduceTime 生产时间 | 否 | 否 | xlsx |
| 204 | `rh` | 客户端->服务端 | 完成 | RECV_BankPage 银行翻页 | 否 | 否 | xlsx |
| 205 | `Cpa` | 客户端->服务端 | 完成 | RECV_PET_ITEM_MOVE 移动宠物装备 | 否 | 否 | xlsx |
| 206 | `SWITM` | 客户端->服务端 | 完成 | RECV_BagPage 背包翻页 | 否 | 否 | xlsx |
| 207 | `SWITMP` | 客户端->服务端 | 完成 | RECV_BagPageMove 移动物品 | 否 | 否 | xlsx |
| 208 | `SWITMM` | 客户端->服务端 | 暂缓 | RECV_BagPageMgr 菜单型背包窗口 | 否 | 否 | xlsx |
| 209 | `SWITMP2` | 客户端->服务端 | 完成 | RECV_BagPageMoveEx 移动物品 | 否 | 否 | xlsx |
