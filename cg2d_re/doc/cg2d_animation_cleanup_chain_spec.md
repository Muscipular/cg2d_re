# 动画主/P4槽清理、索引链与卸载 SPEC（ACL01）

2026-10-11。沿ATP01具名载荷与GAP01四块分配，补清理和实际退出链。依子AGENTS第8条工程委托先记方案，再对齐三个定向片段；未知P4生产仍开放。

<a id="animation-cleanup-baseline"></a>
## 原端证据与范围

| 原根 | 完整区间 | 指令/字节/页 | 取证方式 |
|---|---|---:|---|
| 45B440 主表槽清理 | 45B440–45B522 | 78/226B/2 | 复用ATP01完整原字节 |
| 45B650 P4槽清理 | 45B650–45B732 | 78/226B/2 | 复用ATP01完整原字节 |
| 45B630 主槽扫描 | 45B630–45B64C | 10/28B/1 | 新完整根 |
| 45B740 P4槽扫描 | 45B740–45B75C | 10/28B/1 | 新完整根 |
| 45BEF0 卸载 | 45BEF0–45BF1D | 15/45B/1 | 新完整根 |
| 45BF20 标志关闭 | 45BF20–45BF55 | 16/53B/1 | 新完整根 |

六根207条606B/八页、12E8六目标、18跳八RET，八CALL七父。六直接目标为MSVC free/Close、45B440/45B650/45B630/45B740；原入口、连续边界及各阶段合同在本SPEC，源码前后和取证细节沿Git历史保留。引用/当前退出父全源只支撑本族接线，不代所有完整父/全部callee或资源owner。

两个清理根先读取S32 *slotIndex，负值返回0；非负后取一次主/P4 table，动作数读取原+04的W16，0跳帧。每帧先从当次record取list，再一次取frames，非空free同值；free后按slotIndex→fresh table→fresh list清frames，再重取table。空帧路径保table，无额外全局读取。循环尾slotIndex取样、S32 action递增、held table取当次W16 count和record再比较。现for条件与帧读取每次重取全局表，且frames判空/free复读，P4按W32读再mask，与原取样阶段/宽度不同。

帧循环后先fresh slotIndex、持table取动作表，非空free同值后重取table；fresh slotIndex清actionList。随后fresh slotIndex→fresh table清主listNode/P4 values[4] W32。尾fresh slotIndex→fresh table取坐标/缓冲，非空free同快照，再fresh slotIndex→fresh table清尾引用，最后外部index=-1，返回S32 1。发布与条件重取不能统一提前缓存成tuple。

两个扫描均固定256槽，原16B步长；现MainSlot拓指针24B、CacheSlot16B沿已证类型，每次传真实slot.index地址，完整返回最后一次callee S32值。当前result初值0不被业务读取，固定数组非空保证逐项覆盖；正文保持，不改成总成功/短路扫描。

Unload逐九FILE一次取样，非空Close同值后清当次槽，结束才scan主槽，返回真实末值。现FILE判空与Close各取一次，恢复一次快照。BF20按九W32 flag逐项非零才取当次FILE Close、清flag，无新FILE判空；末scanP4返回。现额外清FILE owner引用属于用户MC01-B批准的MSVC适配，保该具名差异，不倒退到悬空宿主FILE、不据两遍关闭宣整个owner已验。

原P4直接global引用只见GAP分配/清零与清理读取，不据未见其它writer判恒空或排除功能。P4前24B、帧与缓冲正常生产/格式仍未证，继续既有独立类型及void*擦除，不借主表字段命名。

<a id="animation-cleanup-decision"></a>
## 方案与裁决

1. 两清理体各一个定向片段，显式index、held table/record、单次frames/list/尾指针读取；非空free后按原条件重取table，保所有清槽和index=-1发布。
2. P4 count用std::memcpy读取已有values[1]的低W16，沿当前Windows小端；新增标准cstring头，不拆未知高半/字段或新建helper。
3. Unload一个片段恢复FILE一次读取；两扫描/BF20正确当前正文保。三定位片段、六定义/三既有声明共九注释、三整源精确回放；没有既有声明的三私有根不新增接口。
4. 六类型/256+256槽、九FILE/flag声明、实际Close provider/当前Shutdown全源保持。现Shutdown顺序Unload→BF20→GraphicFree沿原接线，P4外层数组的真正释放责任仍未闭。
5. 两扫描新增F，四旧semantic只追加；两清理已有处置仅追加，其余四新增adapted。ANIMATION已有二清理仅加四，RESOURCE加六；两限定S覆盖六根、零I八open，不将成员新增数当根覆盖数。
6. 原负索引返回、动作数0、跳空释放、复读清槽、数组数量/非地址宽度与正常最后返回保；不增空/索引/容量保护、新默认/清零/钳制、回滚/异常模型、私有状态或owner。
7. ATP01/GAP01与直接MSVC、WAV-only、原命令字段LF明文保持。完整资源内容/所有父生命周期和SDK运行另验。

<a id="animation-cleanup-acceptance"></a>
## 本步静态验收

两ATP完整原根逐字节复用，四新根连续字节/指令、E8目标/分支/返回及256两扫描/九流有据；两清理原held表/条件重取与单W16、帧/list/尾引用发布、Unload一次FILE取样及BF20具名差异明确。

按用户最新要求只保存本SPEC＋Git，不另建重复evidence.json；原端依据/来源、合同、实际验证结果及未完成验收均保留。

三整源只三个定位片段/九注释与标准头，另三正确正文保持；六类型/四数组声明/当前Shutdown/Close及GAP两旧记录保持。旧记录和两处置仅追加、四新处置/两新F、六文档/八任务引用，派生仅六指标/六F/两功能/八任务；全门禁不升。

加入下述主索引链四根后，登记已落实：语义2347=2299F关联+48有界代码，处置1814/证据1614，ANIMATION19/RESOURCE55；原四旧记录/两旧处置只追加，六新F/八新处置、两限定S覆盖十根。184叶验收0、60任务关闭11开放49、六closure及全部门禁保持。关联率只由静态汇总派生，不代表功能完成率。

<a id="animation-cleanup-source-state"></a>
## 当前状态与剩余

真实源在AnimationCleanup.cpp，三既有声明沿AnimationRuntimeState.hpp/AnimationRuntime.hpp，唯一共享主/P4表和现MSVC FILE沿原退出父。原ATP01只类型责任历史保，本步追加完整取样合同；MC01-B批准额外清宿主FILE引用保持。

未知P4前缀/frames/buffer生产、外层表释放责任、完整主表借用/owner重进退出、全部父/callee与资源内容、生成读取/机器ABI/SDK和像素运行仍待，不因局部释放准确宣所有生命周期通过或整体完成。

本步将沿同一cg2d_re分支精确提交、不打tag，保七外部改动及原删除暂存。不构建、测试、运行、检查产物、计算摘要或引依赖；整体active/未完成。

## 2026-10-11 当前实施与实际验证

三个定位片段与九注释已落码。新增差异的三源文件精确回放、UTF-8无BOM/LF、两清理原位单W16读取/同frames快照与free后条件重取、Unload九FILE一次取样均静态核对通过。两扫描及MC01-B已批准的标志关闭正文保持，不重新核对已通过且未变化的函数，不生成临时或仓库JSON证据副本。

原调用邻窗进一步确认：45B530@45B557消费清理S32返回后才减少活动计数/字节；45B760@45BAC7调用清理后清EAX，弃结果。退出父457A40在457A63/457A68依次调用两卸载，457A6D先进入45A6C0关闭图形索引/数据流，457A72再释放前三块；当前父同序。邻窗不代完整父与全部借用寿命。

旁聊EDS01完成后已交回共享写入窗口，本步沿其当前引用追加登记和唯一进度，不覆盖已归档证据。用户随后要求优先补缺失、暂不核对差异：本步仅收尾此前已经完成的源码改动与静态结果，不重新核对已过函数；后续转向真实缺失的实现和生产链。

## 主索引链四根：原端依据、合同与实施范围

| 原根 | 完整范围 | 指令/字节 | 原调用来源 |
|---|---|---:|---|
| 45B530 领取最旧槽 | 45B530–45B583 | 26/83B | 45B760@45B7A8 |
| 45B590 挂到链尾 | 45B590–45B5AF | 8/31B | 45B5B0@45B5C1、45B5D0@45B60A、45B760@45B850 |
| 45B5B0 提升已用槽 | 45B5B0–45B5CC | 10/28B | 45B760@45B790 |
| 45B5D0 初始化链 | 45B5D0–45B622 | 22/82B | 45BB20@45BB36 |

四新完整根66条224B/四页，3E8两个目标、三跳五RET。连同前述清理/卸载，共十根273条830B/十二页、15E8七目标、21跳13RET。原源位置/读取与写入时点保留本文，源码前后版本由Git保存，不生成指令或源码副本JSON。

领取根一次取右哨兵next，等于左哨兵时返回空；否则oldest.next读两次，第一次向其previous写右哨兵，第二次发布到右哨兵next，两次读取之间原写入保持。清理传实际oldest.index地址，返回非零才更新计数。原45B563先取活动数量，45B569再取活动字节，随后数量减一并发布，45B575才读槽value04，以已持字节减该值再发布；返回仍原oldest。现先减数量才读字节，恢复两个值先读齐，不添空槽/容量门槛或改变失败返回。

挂尾根原45B594先保存左哨兵previous，45B59A才发布slot.next=左哨兵；之后slot.previous=已持前驱、同前驱next=slot、左哨兵previous=slot。现先发布next再读前驱，并经slot.previous复读写目标。用局部前驱承接，不改变四次链接发布顺序。

提升根原45B5B5读取slot.previous，45B5B8读取slot.next，45B5BB/45B5BE分别用两个持值写previous.next与next.previous，随后挂尾，返回S32零。现两复合赋值的C++求值与复读未明确原序，恢复前后指针先取齐再两次写；不用helper、不添链保护。

初始化原先左哨兵next空、主256槽清零，再左previous=右、右next=左、右previous空，逐槽index=-1并挂尾，返回零。现同序，沿已证native MainSlot数组承接原16B槽拓指针，不写新默认/重新布局，正文保持。

此扩展只有AnimationList.cpp的三个定位片段、四定义/四既有声明注释；计数W32、value04及真实MainSlot*保持。四根均新F/新adapted，ANIMATION/RESOURCE各加四，沿同一小步两限定S覆盖。原动画装载完整父、全部链别名/主表数据和owner寿命仍待，不以局部取样判整条装载或退出已完成。

## 本次登记收尾结果

登记增量及既有任务/功能门禁守恒检查通过，静态汇总与-Check均终态退出0；实际关联2409/3680（65.46%），仅合同关联率。此前六源码片段/17注释的限定静态结果未重复核对，本次无新增功能或运行验收。不创建源码/指令副本JSON；后续先补真实缺失。
