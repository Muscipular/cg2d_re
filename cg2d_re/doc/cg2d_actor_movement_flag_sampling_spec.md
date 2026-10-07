# 角色移动标志真实取样与返回 SPEC（BTP01）

日期：2026-10-07。承接BTN01/BTO01，按AGENTS.md第8条既有委托先裁决后实施；[完整原指令/前态](cg2d_actor_movement_flag_sampling_evidence.json)。

<a id="actor-movement-flag-baseline"></a>
## 原端基线

当前cg2d.exe只读：十三完整根164条550B/十三页十三块，唯一442C70→440B50调用、17jump/13RET，18入向五父。五置位仅flags42高BYTE OR1/2/40/80/04；449C90不同，原先一次MOV AX读取W16、TEST AH2，未设时OR AH1再写回同AX，应保held W16读改写，不盲改BYTE。低W16始终为该flags结果，NULL EAX=0，高W16沿旧index残余。

七取消真W16：CC0/C00 FEFF、C70 FDFF、CD0 BFFF、D30 7FFF、D90 EBFF、DB30 FF7F。BD0原高BYTE OR1→marker44 BYTE1；C00先marker44 BYTE0，再完整S32 field2E4!=2才取消100；CA0/D00/D60高BYTE置位→完整W32 value10C发布。门禁C70先完整EAX检查440B50再NULL action，不重排。

五纯地址C40/CD0/D30/D90/DB30非NULL尾LEA同flag字段、NULL为0。C70另有门禁非零整数原返回与地址混合，不能概括为纯指针。其余void叶原EAX为数组偏移或其它残值，原事实分别保留。

18点局部路径已取。BD0@50A650原在50A3E0尾RET透传数组偏移，不能记为直接弃返；其唯一上游512E40@514C55调用后XOR EAX清0再RET，残值不被业务消费。现父JJQj的I/T模板与void接线正确，保持；其它路径覆盖或后续call覆盖，不新增残值消费。路径只证局部观察，不验五完整父。

十一相邻区共90B逐BYTE90/NN_NOP，无入向/后真入口；449C90→CC0和C70→CA0原无间隙，不造X。

<a id="actor-movement-flag-decision"></a>
## 具名裁决及实现

修一cpp十正文和一头六声明，共16唯一片段。五OR只高BYTE，后续marker/value顺序保。449C90用同field引用、一次W16 sampledFlags取样，条件更新同held值并返std::uint16_t低W16，NULL返原0；原EAX高残余不升级成业务返回或模拟x86寄存器，适配只本叶。

五地址叶用同一次flags引用、原操作后返&flags/NULL返nullptr，非拥有、不截W32、不新增持有或owner。七偏移/混合寄存器叶明确维持void观察合同；其中BD0残值直到512E40清0链有据，C70混合返回不造伪指针/统一整数。正确CC0/C00/C70三正文完整保持，原取消操作与条件不改。

网络原格式、NI/JJQj原接线/明文和内部LegacyCryptInt及音频暂停保持。无新helper/影子状态、额外NULL保护、容量/范围、拒绝/补零/钳制/限长、异常/回绕模型。具名裁决依既有委托实施，不重复请求批准。

<a id="actor-movement-flag-acceptance"></a>
## 本步验收

1. 完整cpp仅十具名片段，完整头仅六声明；五真实地址及一低W16声明/定义/NULL结果对应；其余所有正文、接口输入/调用/owner保持。
2. 十三完整根164条550B/十三页、1call/17jump/13RET，18点五父、低Word与五LEA和七取消/五BYTE/90B用途完整对照。BD0透传至上游清0事实不隐藏、不误称全部直接弃返。
3. 十三既有F新1645-1657/十adapted三equivalent、LOOP57→70、两局部I-S各27真实来源、三任务仅current/引用保open，旧对象和完整门禁保。
4. 仅必要静态与纯文本measure/-Check，不编译测试运行/摘要；预期语义1658有效1651/文档1769（48.07%）/处置799/E902、R7 367余2694，终态追加真实结果。

<a id="actor-movement-flag-source-state"></a>
## 完成边界

BTP01十三完整移动/跟随/观察清除叶164条550B/十三页十三块，唯一call442C70→440B50、17jump/13RET、18direct点五父50B380/444410/50A3E0/51B080/440440。实修一cpp十既有正文十片段及一头六声明，共16唯一片段：五flags42高BYTE OR1/2/40/80/04依原序发布marker44 BYTE或value10C W32；449C90原一次W16取样→TEST AH2→仅未设时同held W16 OR100写回，恢复有定义低W16结果/NULL0，以std::uint16_t承接，原EAX高W16旧index残余不扩作业务值/不模拟；七原W16取消及marker清→完整field2E4模式2短路/门禁优先顺序保。五纯LEA地址叶C40/CD0/D30/D90/DB30恢复同flags42或flags120非拥有std::uint16_t*返回/原NULL返nullptr。七void偏移/混合寄存器叶观察适配具名批准，C70原门禁非零EAX或flag ptr混合不冒充纯指针；BD0@50A650残余偏移透传50A3E0父RET、最终512E40@514C5D XOR EAX清0已证，不误称18点全部直接弃返，父JJQj模板接线正确保持。其余所有源码/接口输入/owner/值与清序保持，无新helper/状态/保护/范围扩大。十三既有F新语义1645-1657/十adapted三equivalent、LOOP57→70/两局部I-S各27真实来源、十一NOP区90B只用途；原无间隙两个边界不造X。全部旧1645语义/其它成员/冻结定位/原pending18和任务/完整门禁保，网络原格式明文/内部LegacyCryptInt/音频暂停/RPLAN.24保持。五完整父/状态生产消费及正常owner-alias寿命、SDK-ABI/链接/画面/明文互通/运行R8未验，局部返回裁决不外推机器ABI。

完整五父/全部状态生产消费与正常owner-alias寿命、SDK-ABI/链接/画面/明文互通/运行R8继续；局部具名观察/低W16适配不代逐机器ABI验收，不降低整体门禁。

## 本步静态终态（2026-10-07）

十正文/六声明的完整源码差异、十三原根164条550B及18条调用路径/唯一上游清0链、十一NOP区90B、完整机器台账与五文档/三任务的定向静态均通过；逐页原字段核对及纯文本measure/-Check终态为0。完整派生快照只包含具名增量，十四文件UTF-8无BOM/LF及两局部I-S各27条真实来源通过。

实际语义1658、有效1651、文档关联1769（48.07%）、处置799/E902、LOOP70；R7已复核367/余2694。完整叶验收0、整体未通过。未编译、测试或运行；五完整父链、全部生产消费/正常寿命与机器ABI及运行R8仍按原门禁推进，不以本步静态结果代替。

## 元数据描述更正（BTQ01，2026-10-07）

[后续子SPEC](cg2d_pet_slot_shutdown_sampling_spec.md#pet-slot-shutdown-decision)将十三语义1645-1657的original_all_actual_callers_discard_return_kept更名为original_local_caller_observation_and_upstream_return_clear_kept、值保true。原字段误称全部直接弃返；本SPEC与已取原证据始终明确BD0尾透传50A3E0至512E40清0。只修机器描述，原BTP01源码、证据、处置、数量及局部验收边界不变。
