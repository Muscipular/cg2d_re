# UUN 请求与原 CRC 表承接 SPEC（BUO01）

日期：2026-10-08。承接BUN01确认的510710基线缺口，沿原两个实际请求父及整数token/校验查表依赖推进。按AGENTS.md第8条先裁决再实施。[四完整根/原数据/引用/源前态](cg2d_map_request_original_tables_evidence.json)。

<a id="map-request-tables-baseline"></a>
## 完整原基线

| 原根 | 完整范围 | 原端量 | 源码处理 |
| --- | --- | --- | --- |
| 510710 | [510710,510807) | 77条247B/两页/十四call/一RET/两入向 | UUN正确构造正文保持 |
| 517120 | [517120,517160) | 19条64B/一页/三call/一RET/160引用来自68父 | 正确token正文保持，仅本链消费与引用身份 |
| 469290 | [469290,4693E9) | 106条345B/两页/六call/九jump/两RET/一入向 | 正确地图核验及失配请求保持 |
| 469240 | [469240,469284) | 27条68B/一页/两jump/一RET/三入向 | 正确算法体保持，两原表数据承接 |

合229条724B/六页四块、23directcall/11jump/五RET。记录全部原引用身份，不把token160点/68父称完整复核。BUN01已有46AF30全根，本步另469290完整核定UUN失配调用，509090与46AF30只补局部后窗。

## 原表数据与源码差异

| 原表 | 范围/元素 | 原指令引用 | 本步实施 |
| --- | --- | --- | --- |
| 55DAAC | [55DAAC,55DBAC)，256B/256个U8 | 469263原输入BYTE索引→零扩展W16 | 同名std::array<uint8_t,256>原值常量 |
| 55DBAC | [55DBAC,55DDAC)，512B/256个W16 | 46926B当前状态高BYTE索引/XOR W16 | 同名std::array<uint16_t,256>原值常量 |

此前两表由公式constexpr生成，仅少数static_assert，尚无整表原值对照。按原已加载768B直接列512元素，保持类型、符号、owner、现只读宿主视图、所有引用与既有断言；不运行生成或校验算法，不计算文件摘要，不声称旧生成数据曾错误。已核全表初值身份，不因此排除原所有间接写别名或关闭运行寿命。两数据初始化替换不改四函数正文，不新增表副本/函数/helper/接口或网络协议。

CRC体原S32 length<=0返0，正数按BYTE取样、U8表值零扩展、状态lowW16高BYTE索引、XOR lowByte<<8及零扩展返回保持。地图校验保持三2602 WORD局部数组/既有加载承接、原加载返回不加新门禁、held完整W32 mode==1选择hit WORD &1FF，否则&FFF。三次原1458B校验及tile/obj/hit次序、低W16结果/三S32 expected比较保持；匹配1，不匹配交原UUN后0。原mask实际是WORD，不误改BYTE；不造故障/越界或回绕模拟。

## 原格式字段与发送接线

原510715字符串已加载55554E00，命令确为UUN；原51714C字符串2000为一个空格和NUL。
UUN六S32字段依次各先采当前W32 capacity，再format token，之后fresh目标buffer追加；末fresh buffer与socket交既有SendProtocolBuffer并沿发送返回。字段、顺序、原命令转换/数字格式/空格/NUL/帧及明文约定不改，不恢复加解密。
token先第一capacity→format整数→fresh首destination copy，再第二capacity/fresh destination→空格append，最后fresh共享buffer返回。已有517730数值承接1220、517080复制/5170D0追加/517480明文父保持，不扩完整callee/CRT或互通结论。

469290请求实参沿原socket/first/floor/xStart/yStart/xEnd/yEnd；BUN01的46AF30已完整证明窗口参数。两个真实父与checksum接线、所有CPP/声明/网络缓冲保持。本步解决BUN01的510710原体缺口，未决的完整互通/owner-alias/全部父和运行验收继续。

三尾12+7+9=28B逐BYTE实际NN_NOP/90、无入向/后真入口，仅用途；token直接邻517160，没有额外区间。

<a id="map-request-tables-decision"></a>
## 具名裁决与实施

只改SceneState两已有表初始化，直接承接原768B常量；四函数正文和NetworkRuntime全文保持。原初值静态逐元素可核，避免以生成公式代原数据；不额外添加默认/保护或虚构先前错误。
新1708/UUN、1709/token、1710/地图核验、1711/CRC四adapted。LOOP四成员，MAP核验/CRC两成员，NETWORK UUN/token两成员；I仅MAP/LOOP两表开发，S三域，不把四新身份称四个新增函数实现。
网络原格式明文/401960与401AB0排除、内部LegacyCryptInt/音频暂停/RPLAN.24持续。U10/G02/R04/V03只current/引用，四任务仍open。

<a id="map-request-tables-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 边界 |
| --- | --- | --- |
| 源码 | 一cpp两数据初始化、四正确正文全保 | 512原值/类型/符号/owner与全文模型，不执行算法或称旧数据错误 |
| 原端 | 229条724B/六页，774B原表/字串数据、28B用途/两个后窗 | 原字节/参数/读写及返回，不代160全父或所有callee/运行 |
| 身份 | 四新adapted/三R7用途 | 旧1708条/冻结定位/pending18与ECX pending7保持 |
| 功能 | LOOP120→124/MAP117→119/NETWORK57→59 | 两I（LOOP/MAP）、三S各25来源，全部成员/寿命/互通门禁不伪闭 |
| 派生 | 语义1712/有效1705、合同1823（49.54%）、处置853/E999、R7已审418/余2643 | 汇总器生成，静态后记实值 |
| 任务 | U10/G02/R04/V03仅current/引用，四任务open | 原closed11/差队列和完整门禁保持 |

仅必要静态和文本汇总，不构建、测试、运行、执行生成/校验算法或计算文件摘要。完整正常生产消费/owner-alias寿命/数值/机器ABI/链接/像素/明文互通及R8继续，不降低标准。

<a id="map-request-tables-source-state"></a>
## 当前状态

四完整根/原两表/字串/尾区/后窗及源前态已取得。先记本SPEC，再替换两个数据初始化并同步唯一进度/三域台账/四任务，必要静态后汇总记实值。命名快照：cg2d-re-map-request-original-tables-20261008。

BUO01必要静态已通过：完整Scene只两数据初始化改原768B/512常量，类型/符号/owner/断言和四正确正文及NetworkRuntime/全部其它源保持；每个源常量与原实际字节对应，未执行生成/CRC或摘要，不称旧值错误。四根229条724B/六页、原表/字串774B、三28B用途及两个后窗/完整引用身份与证据和台账/文档模型一致。单引用检查曾因顶层单元素数组展开不匹配，已保留数组形状并重做该项通过。汇总及 -Check 通过，派生实值：语义1712/有效1705、合同1823（49.54%）、处置853/E999、R7已审418/余2643、LOOP124/MAP119/NETWORK59。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11与完整门禁保。未编译、测试或运行，原全部写别名/160全父/正常寿命/数值/机器ABI/像素/明文互通及R8继续。命名快照：cg2d-re-map-request-original-tables-20261008。
