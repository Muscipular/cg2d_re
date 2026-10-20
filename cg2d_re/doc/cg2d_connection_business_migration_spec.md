# CgConnection 全业务网络入口迁移 SPEC

日期：2026-10-20。编号：CTO02。状态：XCNY已移至登录业务并清理前两次多余改动，本轮Debug编译与链接通过；运行互通/竞态待验。

目标：在[CTO01对象封装](cg2d_connection_transport_spec.md)基础上，将登录、Lua、主循环及其他业务模块的网络请求全部交由CgConnection管理，移除旧连接适配入口、协议工作区兼容桥和gConnectionSocket镜像。用户补充的“其他业务点也要迁移”属于本轮必需范围。

## 1. 范围与源码基线

当前CgConnection已拥有会话、协议状态和CgTransport，但业务仍经NetworkRuntime自由函数及ConnectionLegacyAccess使用这些资源。源码与头文件共57个文件出现gConnectionSocket，204处文本匹配包含定义、声明、注释和使用；该数字仅用于说明影响面，不作为完成率。

本轮覆盖以下全部网络入口，不能只替换登录和Lua后宣布完成：

| 业务范围 | 主要落点 |
|---|---|
| 建连、登录、账号及人物操作 | scene/login、scene/character_select，包括创建、删除、选择、版本与登出 |
| 主循环、保活、退出 | platform/MainLoop、MainLoopSupport、ClientShutdown及相关依赖声明 |
| Lua与自动战斗 | script/LuaUI、AutoBattle，通用发包及编码接口、收发事件 |
| 世界操作 | world/SceneState、WorldUpdate、MailState及其他协议生产/消费模块 |
| 所有UI网络动作 | ui/WindowRuntime和ui/windows，包括背包、银行、宠物、技能、战斗、交易、摆摊、聊天、邮件、公会、人物资料和对话窗口 |
| 协议与回调 | network内全部业务发送入口、状态访问、拆包/派发和接收handler依赖声明 |

当前NetworkRuntime有81个业务组包函数，另有连接探测、Lua通用发包、自动战斗直接拼接报文，以及背包/银行直接构造SWITMP、SWITM、rh的路径。实施须按符号查全引用，不能只按gConnectionSocket或network::前缀替换。

“业务迁移”指网络请求、编码工作区、收发门禁、会话校验和派发由连接对象统一管理；人物数据、UI状态、世界更新、动作选择和业务处理仍由原模块拥有。CgTransport继续只负责字节传输。

## 2. 对象与调用关系

```text
登录 / Lua / 世界 / UI
    → CgConnection的业务方法或完整明文入口
    → 对象私有组包、回调和会话校验
    → CgTransport有界字节队列 / I/O线程

主循环 → CgConnection::Pump
    → 私有拆帧、解压、参数解码和派发
    → 原业务handler，显式接收CgConnection上下文
```

GetGameConnection仍提供进程内唯一对象，声明移至CgConnection.hpp，唯一对象及访问器实现归CgConnection.cpp。实例寿命覆盖初始化、重连及退出，Shutdown/Initialize不重建对象、不重置ConnectionId序列。已有显式对象上下文的函数直接使用传入引用，不再重新查全局连接。

业务侧不持有Socket、不传递uintptr_t连接参数、不操作协议工作区或待发队列。Id保留用于诊断和内部重入检查，业务发送由对象内部捕获身份。此次不引入连接池、多连接服务定位器、虚接口层或新的全局镜像。

## 3. 公开接口与全部业务发送

已有Initialize、Shutdown、BeginConnect、PollConnection、Close、StopIo、Active、Error、Pump、RegisterReceive、RegisterSend继续使用。

81个业务发送函数全部迁为CgConnection成员：**保留语义名称，去掉地址后缀和连接首参，其余参数类型、顺序及返回合同保持**。完整映射见附录。原地址注释保留在实现附近，不进入新公开名称。旧自由函数与声明在调用者完成迁移后删除，不再作为长期转发层。

例如：

```cpp
auto& connection = network::GetGameConnection();
connection.SendAccountLoginRequest(digest, password, account, adapterId);
connection.SendChatMessage(first, second, text, fourth, fifth, sixth);
connection.SendTradeOffer(items, pets, gold);
```

上例用于说明取消连接首参，真实形参和字段继续以对应原函数为准，不借机重命名未知业务字段或修改协议。

| 补充接口 | 合同 |
|---|---|
| IoEnabled() | 只读当前业务收发门禁，供登录请求轮询和诊断使用 |
| SetIoEnabled(bool) | 唯一门禁发布入口，同时更新对象与传输后端 |
| ResetLoginReply / LoginAccepted | 承接当前登录前清零及回复后判断的W16状态，不公开旧地址字段访问器，不改变原状态重置时机 |
| SendPacket(data, size, replacePending=false) | 完整明文入口，不添加LF、不识别登录命令；身份在对象内捕获，业务显式选择是否替换旧待发字节 |
| EncodeInteger / EscapeText / UnescapeText | 供现有Lua编码API调用，返回拥有式字节字符串；保留Base62、GBK、转义、容量及错误合同，不返回私有共享缓冲指针 |
| MoveItemToBagPage / SelectBagPage / SelectInventoryBankPage | 承接现有UI直接构造的SWITMP、SWITM、rh路径，具体字段编码见第6节 |

普通业务使用具名方法。Lua开放发包和已有自动战斗完整报文使用SendPacket；不得将所有具名业务接口退化成业务侧拼字符串后统一SendPacket。发送仍返回0、-1、-100等既有结果；不得将成功入队当作服务端已处理。

连接探测留作对象私有操作；60秒保活由现有调度器调用对象SendKeepAliveRequest。收发队列、分词、分配、释放及旧NetworkCallback不作为公开业务接口。

## 4. 门禁、身份与回调重入

按已确认方案，将gRequestPollingEnabled一并退休，由CgConnection::IoEnabled作为唯一事实来源。这是对CTO01“登录模块保留门禁变量”的具名修订。

- Initialize、BeginConnect、Close、StopIo、Shutdown及正常终止后的门禁关闭语义保持；成功连接由登录流程在原71阶段入口前调用SetIoEnabled(true)。
- LoginRequest中的轮询判断和POLLING日志改读IoEnabled，保留0/1语义。普通发送、Lua发送和主循环不再用旧全局值反向覆盖对象门禁。
- 每个业务发送从开始组包起捕获会话身份；回调后仍检查该身份。无会话、已关闭或身份变化返回原失败，不在发送末尾重新读取当前Id来替换旧身份。
- 对原调用者先读连接编号再执行其他操作的路径，逐项保留身份采样位置与作用范围；不能机械替换成每次发送时重新选连接。需要跨多次调用保持的会话上下文由连接对象管理，不让业务恢复保存编号。
- 接收派发期间由对象保存私有会话上下文；原handler或Hook重入关闭/重连后，旧派发栈发起的后续发送不得流入新会话。上下文随同步调用栈恢复，不通过新的全局变量或UI字段保存。
- 实施采用CaptureSession返回的SessionScope保存/恢复同步上下文；嵌套作用域继承入口身份，Close/Shutdown不清除尚未退出的调用栈上下文。它不暴露编号，不能跨帧、跨线程或跨Lua异步任务保存；内部连接探测也沿同一作用域运行。
- 具名业务组包在未初始化或已Shutdown时返回-100，避免旧回调栈继续借用已释放工作区；已初始化但收发门禁关闭的路径保留原组包/RNG顺序，仍由唯一发送入口拒绝。
- 回调执行时不持传输锁。按2026-10-20用户最终明确，登录业务显式先发XCNY、再发账号请求，并通过通用replacePending选择前导替换旧待发内容。通用SendPacket移除登录识别、自动注入及两包总容量预检；撤除前两次新增的底层消息边界和余量状态，恢复既有字节发送循环，详见NTM01§5.2。

CgConnection::SendPacket现有带expectedId的公开形态转为内部入队操作；不再对业务暴露“0表示改用当前连接”的隐式替换规则。Lua及原生回调必须基于当前派发对象校验身份，不通过GetGameConnection重新选择接收者。

## 5. 登录、主循环和退出

### 登录及人物选择

LoginState直接调用Initialize、BeginConnect、PollConnection、SetIoEnabled、Close和Shutdown；账号/人物请求改调对应成员。保留原步骤0～99、600000ms期限、错误阶段映射、提示文本和账号请求内容。状态快照仍在Close之前取得，错误提示使用该次结果。

71阶段由LoginState显式发送XCNY，成功后才调用SendAccountLoginRequest。两个请求共用同一会话作用域，分别走通用发送Hook和日志；CgConnection只提供通用追加/替换能力，SendAccountLoginRequest只组装账号请求，不隐式补发前导。前导失败沿原queued结果日志及阶段发布路径处理，不新增重试或超时分支。

原Begin/Initialize/Shutdown适配器对gNetworkDisconnectState的清零移到对应登录、返回服务器选择和场景调用位置，保留原成功/失败条件和顺序；不能因删除包装函数漏掉副作用，也不能把场景全局重新塞入CgConnection。

### 主循环及断线

在原NetworkLoop调用位置直接消费connection.Pump，位置仍在消息泵、KeepAlive、时间及图形缓存更新之后，音频/world::Update/Lua场景更新之前。

原断线状态由主循环管理：状态1时切场景11、置2并跳过本轮Pump；其他非零状态仍不派发。Pump报告terminated时记录原阶段/错误/端点日志并置状态1，保持后续一轮处理切场景的流程，不提前或重复跳转。Active、Error直接读对象。

KeepAlive保留60000ms、零哨兵和原采样时点；连接内部30000ms探测仍保持八相位采样，实际收发与派发不恢复八次节流。

### 退出及返回登录

ClientShutdown在ShutdownLua前直接StopIo，后续原网络关闭位置直接Shutdown。LoginScene、CharacterSelectScene、SceneState、WorldUpdate等全部关闭路径迁移，保留已有存档、清世界、场景变更及断线标志处理顺序。重复关闭和初始化中途失败继续安全，不能仅替换最终退出而遗漏登出、超时或返回选择页。

## 6. Lua和特殊报文路径

Lua可见函数名、参数、返回值和事件注册形式保持。本轮不新增Lua连接对象或Lua建连API。

- WinMgr_sendPacket单字符串模式：已有LF的输入保持，缺LF时仍只补一次；字节长度、含NUL处理沿当前Lua读取路径。
- 多参数模式：保持当前逐项解析顺序、整数编码、文本从现有GBK边界转换和失败位置；调用对象编码操作并立即保存拥有式结果，最后交SendPacket。不得先批量验证所有参数再编码而改变既有错误顺序。
- nrEncode62、nrEncodeString、nrDecodeString改调用对象操作，保持Lua输出字节。无连接状态的nrDecode62/16等算法可继续用无状态工具，不强制附加会话或联网条件。
- AutoBattle的所有SendLuaPacket调用改走当前对象完整明文入口；其目标选择、RNG、动作计数与原始Rg报文保持。该路径不能改走会再次转义或补LF的接口。
- Lua收发桥接显式取得当前CgConnection上下文，事件集合/参数快照与回调前后身份校验保持；不借此次迁移改变Lua注册顺序或生命周期。

必须单列的UI报文合同：

| 当前路径 | 迁移约束 |
|---|---|
| SWITMP | 物品来源按原Base62编码，页码按原十进制，保留末项-1及唯一LF |
| SWITM | 页码按原十进制，保留原UI发布顺序 |
| 背包界面银行页rh | 保留page+1及十进制编码 |
| 既有SelectBankPageRequest | 保留原协议整数编码；不能仅因同为rh就与上一路径合并 |

上述具名方法复用相应旧发送流水线，保留日志、发送观察者、Lua/原生Hook的先后关系；不能直接绕到CgTransport。

## 7. 接收业务与内部桥清理

CgConnection继续负责接收字节、LF/压缩解析、参数快照和分发。业务handler仍落在原世界/UI/登录模块，但连接首参由旧uintptr_t改为显式CgConnection&；有反向网络请求时直接使用该引用。仅作回调适配的忽略参数分支也同步调整，覆盖完整派发表及跨文件声明。

原生RegisterReceive/RegisterSend的上下文改为明确的CgConnection引用，参数借用只持续至同步回调结束。Lua脚本的事件参数不变。接收Hook、Lua事件、内建业务分发的次序保持，不新增异步业务队列。

全部组包和状态访问完成迁移后，删除CgConnectionLegacy.hpp、ConnectionLegacyAccess友元及定义、ProtocolOutputAccess和NetworkCallback兼容安装路径。分配/释放、发送观察者、登录回复状态和字段工作区仅由对象私有实现操作。

无状态字段解析函数仍服务大量原业务handler；例如ExtractDelimitedField、ParseDelimitedInteger、Base62Decode不持有连接资源，可继续在既有NetworkRuntime文件内集中维护。本阶段不为这些小工具另建文件，不把纯文本解析强行变成有会话要求的对象调用。NetworkRuntime不得再承担连接拥有者、业务发送或全局状态适配职责。

## 8. 删除清单与文件落点

| 删除或退休项 | 替代 |
|---|---|
| gConnectionSocket定义、声明、写入及全部读取 | 对象内部会话身份；业务方法取消连接首参 |
| gRequestPollingEnabled及同步代码 | IoEnabled / SetIoEnabled及对象生命周期 |
| BeginServerConnection、PollServerConnection、CloseServerConnection、StopNetworkIo | 对应CgConnection成员 |
| InitializeNetworkRuntime、ShutdownNetworkRuntime、NetworkLoop及其using声明 | 调用位置直接使用对象，场景副作用留在调用模块 |
| ReadNetworkRuntimeActive、ReadNetworkError、Read/WriteProtocolMode旧访问器 | Active、Error及登录回复操作 |
| SendLuaPacket、RegisterProtocolReceive、RegisterProtocolSend | 对象发包/注册接口及显式上下文 |
| 81个业务发送自由函数及连接探测外部入口 | 附录中的对象成员，探测仅私有调度 |
| 公开的SendProtocolBuffer、共享token访问、分配/接收队列/分词/派发适配 | 对象内部实现或拥有式编码结果 |
| CgConnectionLegacy内部桥、ProtocolOutputAccess、旧NetworkCallback | 对象私有操作，无转发外壳 |

主要修改CgConnection.hpp/.cpp、NetworkRuntime.hpp/.cpp、网络依赖声明及第1节各业务目录；MainLoop.hpp/.cpp删除镜像，MainLoopDependencies删除旧using，LoginRequest.hpp/.cpp删除门禁全局。CgTransport的线程、DNS、候选、队列和SDK操作保持本轮基线。

新公开业务方法按功能分组声明；首轮沿现有网络文件归拢实现，不为每个命令新增文件或管理器。原地址证据注释保留，不更新历史函数表或JSON台账。

## 9. 实施步骤与验收

1. 建立完整调用清单，涵盖旧自由函数、未加命名空间前缀的调用、函数指针、头文件和using；明确每个旧入口的副作用及迁移位置。
2. 补齐对象公开方法及私有派发上下文，迁入81个业务组包和特殊报文路径。过渡兼容仅存在于实施过程，不能成为最终结果。
3. 迁移登录、人物选择、Lua/AutoBattle、主循环/保活/退出，再逐组迁移世界和全部UI网络动作；每组检查协议字段与原调用顺序。
4. 完成接收handler和原生Hook上下文迁移，删除全部镜像、门禁全局、适配入口及内部桥。
5. 集中静态核对、按授权构建，更新SPEC和唯一进度并精确提交。

验收必须同时满足：

- src/include中无gConnectionSocket、gRequestPollingEnabled及已退休生命周期入口；无宏、类型别名或同义全局替代它们。
- 81个业务发送全部成员化，调用者不传连接编号；所有普通、Lua、自动战斗和特殊UI报文到对象唯一队列的路径完整，无旧自由函数兜底。
- 对象公开API不返回可变工作区指针；CgConnectionLegacy物理删除，网络原生资源仍只在CgTransport。
- 原命令/字段/编码/LF、容量、返回值、日志/Hook顺序、RNG和业务发布时点逐项核对；特别覆盖rh两种编码、登录业务前导调用及队列替换和Lua两种发包形式。XCNY业务抽离按NTM01§5.2的最新具名修订执行。
- 登录成功/失败、人物操作、登出、主动关闭、正常EOF、错误断线、返回服务器选择和最终退出的副作用完整；回调内重连不将旧消息或后续旧派发请求送入新会话。
- NTM01的IPv4/IPv6、scope、策略DNS、候选次序、取消、部分发送、背压、计时及两阶段关闭合同继续适用。

用户已确认实施本SPEC，并授权完成后执行build.bat；运行互通/竞态保持单独验收，不由静态核对或构建代替。不新增外部依赖、不启用RTTI、不使用子代理，不运行客户端或新增/执行测试。

## 10. 确认范围

用户已整体确认全部网络业务点迁移；包含清理gRequestPollingEnabled、改造接收/原生Hook上下文、迁移81个业务组包及特殊报文，并删除CTO01过渡桥。场景与UI业务状态保留原所属模块，纯字段解析工具保留无状态形式。

## 11. 本轮实施与验证结果

- 81个业务组包及194处调用已迁入对象；三个特殊UI方法、Lua完整明文发送、AutoBattle、登录/人物、主循环/保活和两阶段退出均已接线。
- 108条接收派发路径保持命令、目标、字段类型及读取掩码顺序；业务handler、原生Hook、Lua收发桥和对话内部路由均承接连接上下文。
- src/include中已清除两份旧全局、退休自由入口及兼容桥；CgConnectionLegacy.hpp已物理删除。CTO02基线未修改CgTransport和构建配置，后续XCNY业务抽离及前导接口清理见NTM01§5.2。
- 静态文本比较确认81个组包本体除对象/上下文替换外相同，194处调用保留业务参数与顺序；三条特殊UI报文、主循环日志及生命周期副作用单列核对。UTF-8、换行一致性和差异空白检查通过。
- build.bat Debug首轮定位并修正退出模块缺少声明、对话内部路由首参未迁移两项接线问题；第二轮配置、编译和链接成功，退出码0。既有C4701/C4244等业务警告保留，未新增依赖、运行客户端或执行测试；互通和重入竞态仍需运行验收。

## 附录 业务发送成员映射

下表各项均去除原连接首参，其余参数及返回合同保持；成员属于CgConnection。连接探测、特殊UI报文和Lua完整明文入口见正文，不混入81项计数。

| 原自由函数 | 新成员 |
|---|---|
| SendCharacterListRequest_512B90 | SendCharacterListRequest |
| SendLogoutRequest_512BD0 | SendLogoutRequest |
| SendKeepAliveRequest_512C60 | SendKeepAliveRequest |
| RequestItemTargetAttributes_511CD0 | RequestItemTargetAttributes |
| SendTradeRequest_511FB0 | SendTradeRequest |
| WithdrawTradeConfirmation_512230 | WithdrawTradeConfirmation |
| ToggleBattleRowRequest_512570 | ToggleBattleRowRequest |
| RequestLogoutToTown_512CF0 | RequestLogoutToTown |
| SendLoginTimestamp_512DF0 | SendLoginTimestamp |
| SendVersionUpgradeRequest_512D30 | SendVersionUpgradeRequest |
| SendCraftDuration_512DA0 | SendCraftDuration |
| SendPlayerMovementPath_510570 | SendPlayerMovementPath |
| SendCharacterLoginRequest_512B00 | SendCharacterLoginRequest |
| SendCharacterDeletionRequest_512AB0 | SendCharacterDeletionRequest |
| SendAccountLoginRequest_512800 | SendAccountLoginRequest |
| SendCharacterCreationRequest_5128C0 | SendCharacterCreationRequest |
| SendMapTransitionRequest_510630 | SendMapTransitionRequest |
| RequestMapRectangle_510710 | RequestMapRectangle |
| SendItemUseRequest_510AC0 | SendItemUseRequest |
| SendPositionDirectionInteraction_510B80 | SendPositionDirectionInteraction |
| SendPlayerCoordinates_510810 | SendPlayerCoordinates |
| RequestAlbumEntries_511030 | RequestAlbumEntries |
| RequestContactDeletion_511190 | RequestContactDeletion |
| SendGuildTextUpdate_511250 | SendGuildTextUpdate |
| SendGuildRankDefinition_5112C0 | SendGuildRankDefinition |
| SendGuildMemberRankChange_511350 | SendGuildMemberRankChange |
| SendDuelRequest_510880 | SendDuelRequest |
| SendSpectateRequest_510940 | SendSpectateRequest |
| SendContactCardRequest_5111E0 | SendContactCardRequest |
| SendGuildInvitation_5113C0 | SendGuildInvitation |
| RequestGuildMemberDeletion_511430 | RequestGuildMemberDeletion |
| SendContactMail_510FA0 | SendContactMail |
| SendGuildMail_511480 | SendGuildMail |
| SendContactPetMail_5110B0 | SendContactPetMail |
| SendGuildPetMail_511510 | SendGuildPetMail |
| RequestProfileCategoryUpdates_5115F0 | RequestProfileCategoryUpdates |
| SendPlayerProfile_511640 | SendPlayerProfile |
| RequestPlayerProfile_511760 | RequestPlayerProfile |
| RequestMiniMailContactDeletion_5117D0 | RequestMiniMailContactDeletion |
| SendMiniMail_511840 | SendMiniMail |
| SetProfileVisibilityRequest_5118B0 | SetProfileVisibilityRequest |
| SendChatMessage_511900 | SendChatMessage |
| SendPlayerOptions_511A00 | SendPlayerOptions |
| SendTeamJoinLeaveRequest_511A50 | SendTeamJoinLeaveRequest |
| SendPetStates_511AE0 | SendPetStates |
| MovePetSlotRequest_511BC0 | MovePetSlotRequest |
| RequestSkillTargetAttributes_511C30 | RequestSkillTargetAttributes |
| SelectSkillTextTarget_511C80 | SelectSkillTextTarget |
| SelectItemTextTarget_511D10 | SelectItemTextTarget |
| SendItemTargetPair_511D60 | SendItemTargetPair |
| SendWorldActionRequest_511DD0 | SendWorldActionRequest |
| SendPlayerMotionRequest_511E60 | SendPlayerMotionRequest |
| SendSkillUseRequest_511EF0 | SendSkillUseRequest |
| SelectTradePartnerRequest_511FF0 | SelectTradePartnerRequest |
| SendTradeOffer_512040 | SendTradeOffer |
| PublishStallRequest_5120D0 | PublishStallRequest |
| CloseStallRequest_512160 | CloseStallRequest |
| RequestStallContents_512190 | RequestStallContents |
| PurchaseStallOffer_5121E0 | PurchaseStallOffer |
| SelectBankPageRequest_5127B0 | SelectBankPageRequest |
| SendTradeDecision_512270 | SendTradeDecision |
| SelectPlayerTitleRequest_5122C0 | SelectPlayerTitleRequest |
| DeletePlayerTitleRequest_512310 | DeletePlayerTitleRequest |
| SetCustomPlayerTitleRequest_512360 | SetCustomPlayerTitleRequest |
| IncreasePlayerAttributeRequest_5123B0 | IncreasePlayerAttributeRequest |
| IncreasePetAttributeRequest_512400 | IncreasePetAttributeRequest |
| SwapPlayerSkillOrderRequest_512470 | SwapPlayerSkillOrderRequest |
| DropPetRequest_510E30 | DropPetRequest |
| SwapPetSkillOrderRequest_5124E0 | SwapPetSkillOrderRequest |
| RenamePetRequest_5125B0 | RenamePetRequest |
| SendDialogResponse_512620 | SendDialogResponse |
| DropItemRequest_510CE0 | DropItemRequest |
| BeginItemUseRequest_510A00 | BeginItemUseRequest |
| MoveItemBetweenOwnersRequest_516C90 | MoveItemBetweenOwnersRequest |
| SendIuEPacket_516D70 | SendIuEPacket |
| SendBattlePacket_5109B0 | SendBattlePacket |
| DropGoldRequest_510DA0 | DropGoldRequest |
| MoveItemStackRequest_510EC0 | MoveItemStackRequest |
| SendSkillRebirthSelection_510F50 | SendSkillRebirthSelection |
| AcknowledgeBattleExit_5108F0 | AcknowledgeBattleExit |
| SendPlayerPositionAndDirection_512720 | SendPlayerPositionAndDirection |
