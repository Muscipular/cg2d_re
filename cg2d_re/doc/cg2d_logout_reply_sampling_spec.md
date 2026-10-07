# 登出回复字节取样与返回合同 SPEC（BTE01）

日期：2026-10-07。沿BTD01退出请求的实际发送/轮询/回复依赖推进；按工程委托先记裁决再实施。原格式仅明文、音频暂停、RPLAN.24和执行限制保持。

<a id="logout-reply-baseline"></a>
## 原基线与开发缺口

| 原根 | 连续完整原范围/指令/字节/页 | 真实消费 |
|---|---|---|
| 508C40 启动 | [508C40,508C81)／11／65B／一页 | 45ED90@45EE22唯一caller；一direct和一Tick导入 |
| 508C90 轮询 | [508C90,508CF0)／26／96B／一页 | 45ED90@45EEAA唯一caller；一direct和一Tick导入 |
| 508CF0 回复 | [508CF0,508DF4)／84／260B／70+14 | 512E40@516442唯一caller；四direct |

合121条421B/四页，六direct六唯一目标、两Tick导入、16jump七RET。508C90已有1065语义及超时格式局部，全部原字段保后追加；另两根已有正文/冻结定位而未登记，仅补两个语义，不造新原函数。

启动当前正确：先一次socket，再清独立W16 DA3B2E，弃512BD0返回后仍清D9F2D0、DA3B30，置DA3B34=1，取U32 tick写DA3B38并返回同tick；不清登录DA3B2C/消息、不额外按发送失败跳过后续。现WA正文/593850文本/协议发送已有实接，保持原格式及仅明文；本步不重新定义命令/字段或证明全部发送层运行。

轮询当前正确：状态2时508C99先取W16回复，再508C9F清状态；码1→1、2→2、其它→S32 -2。非完成态一次U32 tick减原起始tick，原无符号严格>600000才53B304写15B无占位超时文案/含NUL并返-1，不清共享1024B余部/状态/回复；否则返0。不能按登录轮询的不同顺序误改登出轮询。

回复入口508CF0一次取DA3B34并分支。原状态1先写状态2，再按result BYTE→successful BYTE比较；其它状态先比较，成功才发布状态2与回复2。两个原比较循环均依序读取各字节，不匹配立即停止；unsigned byte小于返回-1、大于返回1，NUL全等为0。当前两路宿主strcmp替原BYTE读取，且void声明丢失原W32返回合同；恢复共享逐BYTE比较骨架/入口样本及S32返回，不加NULL、长度、字符清洗或未知状态拒绝。

状态1全等原508D48写独立回复1并返0，不构造数值/转场/音频；不匹配保旧回复，状态2仍发布。非1不匹配不改状态/回复、返回比较±1。非1全等才原顺序发布状态2、回复W16=2，构造一次零值LegacyCryptInt、发布同世界73ECD0，销毁一次、SetPendingSceneType(7)、末次PlaySe返回完整S32。现callback省略最后返回；恢复末次callee返回，保既有两参数音频桥与暂停，不扩大音频实现或重新接额外压栈值。

原508DAD/508DB1/508DB5先读局部+8 mask W32、+4 encoded W32、+C readCount W8，再508DB9/508DC3/508DC8按同序写共享目的。当前边读边写，改三局部样本先读齐再发布；目的+0原vft token和+0D..0F保留字节不写。构造0/销毁和RNG次数保持；LegacyCryptInt是既有内存数值机制，不是网络加解密，不删叶或代固定零载荷。

原唯一network caller局部已证：5163B2匹配HMNy，按参数1/2先解码两文本，51643F/516440/516441压text2/result/socket并调用；51644A清EAX为0，明确弃callback返回。新端既有TryDispatchReply<Sub_508CF0,T,T>(head,"HMNy",socket,4)及静态弃返回保持，借用同解码文本/参数序和旧capacity-mask，不另造私有缓冲或修改协议。只此局部取证，不称整512E40/全部分派及明文互通通过。

成功正文58AFB8十一B为7375636365737366756c00；超时55CA18十五B为b342b27ab56ea558b94faec9a14300，原常量/NUL、1024B共享余部及唯一owner保。原socket/result/unused text接口仅已证地址在新端native承载，非地址字段宽度不扩。

相邻两X共27B：[508C81,508C90)十五B及[508DF4,508E00)十二B，各BYTE实际NN_NOP/90，无入向、前RET后真入口；轮询末尾紧邻callback无额外X。只原对齐用途，不造F/helper/范围排除或门禁。

<a id="logout-reply-decision"></a>
## 实施裁决

LoginRequest.cpp三个唯一片段改一个既有callback：S32声明/入口状态样本及逐BYTE比较的原分支返回；三tuple先取齐后写；末次callee直接返回。私有LoginRequestDependencies.hpp一片段对齐声明及合同注释。其它函数/包含/接口类型保持；cstring仍供既有名称比较与字段复制，不删除。

用单次入口waitingForReply复用原分支，仅合并两相同字节比较骨架，不重复读取共享状态，不新外调/helper/业务条件。返回比较保原unsigned CMP对应±1，不返回字节差值，不把异步成功改为固定0/1。原network模板caller继续弃S32返回，声明与实际定义同类型；不推为已验证机器ABI或链接。

1065原格式字段/入向/源/role/end/count保后追加完整根；新1610启动、新1611回复，共三处置（启动/轮询equivalent、回复adapted），FUNC-LOGIN10→13及两局部I-S；现NETWORK54和其它域保持。源码局部合同不代完整请求往返/所有callee/正常owner-alias寿命/运行。

<a id="logout-reply-acceptance"></a>
## 可度量验收

| 项 | 标准 |
|---|---|
| 基线 | 三完整根121条421B/四页，六direct两Tick/16jump七RET与三真实入向 |
| callback | 声明/定义S32；入口状态只取一次；result→successful逐BYTE短路/unsigned ±1，全等等待0、异步末次原callee结果 |
| tuple | mask/encoded/readCount三样本先取齐，按W32/W32/W8发布同owner；构造零/销毁/RNG一次与目的vft/保留区保持 |
| 其它源 | 整cpp=HEAD+三片段／一callback、整hpp=HEAD+一片段；启动/轮询、其它源头/网络模板/WA-HMNy字段/明文与音频调用保持 |
| 消费 | 原512E40两文本/result/socket与弃callback返回局部；新端现模板两T与弃返回，无新发送格式 |
| 台账 | 1065所有原格式字段保、两新语义/三处置/LOGIN10→13/两局部I-S、两X27B用途、三open任务仅进展/引用 |
| 派生 | measure/-Check终态0，完整快照增量、UTF-8无BOM/LF/真实引用及差异核对 |
| 完整门禁 | 全父/callee/间接别名/正常寿命/ABI/链接/画面/明文互通/运行R8仍未验；原P05-P08 closed/pending7保 |

<a id="logout-reply-source-state"></a>
## 实施状态

工程委托裁决已记录；实施/集中静态/派生结果在本节和唯一进度追加。旧文档中2026-09-24的缺声明、owner未决等是历史状态，当前接线在本SPEC分层记录；不因旧段落撤销已实现正文，也不以当前源码存在宣称完整运行。音频暂停/执行限制保，不做异常回绕模拟或摘要，不手改派生看板/快照。

### 集中静态与派生完成记录

2026-10-07：callback cpp三片段/私有hpp一片段已实施。整两源、原121条421B/四页/比较SBB及tuple取样/原HMNy生产与弃返回、完整机器增量/1065原字段、四文档及三任务六列四组静态全部通过。measure/-Check同会话终态0；完整派生快照仅两新语义及1065追加/三个LOGIN成员与候选、三任务/两R7和六指标增量。13文件UTF-8无BOM/LF、四锚点及两局部I-S各28真实引用/差异文本通过。

真实结果：语义1612有效1605、文档1723（46.82%）、LOGIN13/处置751/E876/R7 321余2740。其它五功能域/冻结定位/pending18/差队列/P05-P08原closed/closed11/完整叶0/globalfalse保持。本步确认的返回丢失、BYTE读取与tuple取样差异已修复；完整512E40/发送层/全部正常寿命/ABI/链接/互通/运行门禁仍待后续证据，静态不替代。
