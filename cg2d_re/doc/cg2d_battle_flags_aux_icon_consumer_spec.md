# BOH01 战斗标志、附加记录与浮动图标消费 SPEC

> 2026-10-06，按既有工程委托先登记方案再实施。沿已纳管命中回调中的406260/406390真实调用，收口本步五个未独立登记助手与图标创建—更新链，不全面重验已通过的创建、骑乘和装备生产者。网络原格式仅明文，音频继续暂停。

<a id="battle-flags-aux-icon-consumer-baseline"></a>
## 1. 完整原端基线

已打开cg2d.exe会话只读实际指令为权威。五根均单chunk，共**286条952B/六页/26call-22普通内部分支/六ret，无switch**；175处普通入向call与一处41F4D8回调地址写入共176引用。后者type1并非普通call，来自41F4C0的绑定。每条指令/字节、全部出口、入向、改前原根与六补丁归档在[证据](cg2d_battle_flags_aux_icon_consumer_evidence.json)。

| 原入口 / ID | 现有符号 | 指令/代码B | call/分支/ret | 本步补丁 |
| --- | --- | --- | --- | --- |
| 0x406260 / F:00006260 | Sub_406260 | 20/50 | 2/1/1 | 0 |
| 0x406390 / F:00006390 | ApplyBattleEffectFlags_406390 | 153/526 | 14/15/1 | 1 |
| 0x43BC90 / F:0003BC90 | TakeBattleAuxEffects_43BC90 | 34/109 | 3/0/1 | 0 |
| 0x41F4C0 / F:0001F4C0 | NewBattleFloatingIcon_41F4C0 | 44/164 | 4/1/2 | 1 |
| 0x41F450 / F:0001F450 | UpdateBattleFloatingIcon_41F450 | 35/103 | 3/5/1 | 4 |

原两个F32常量：54D5B0=cdcc4c3f（0.8F）、54D5AC=6666e640（7.2F），共8B；初速度位型C1000000=-8F为原指令立即数。方向载荷x00/y04/x08/y0C/C9C均既有S32，flags/U32、offset/F32、速度位型/W32、方向/W32、CC与两重生/W8、field270/W16不扩宽。真实地址按既有x64承载保native非拥有T*/S32*，不宣x86/x64内存布局一致。

[BOG01方向/原表角度](cg2d_battle_motion_angle_dependency_spec.md)、[骑乘/装备生产者](cg2d_battle_attachment_producer_spec.md)与[Advance439470事件消费](cg2d_battle_event_production_sampling_spec.md)已存在；本步只定向核接线和Advance正文保持，不复验其全部原体。正常__ftol转换继续按[编译器辅助边界](cg2d_compiler_helper_spec.md)，不能据此宣宿主ABI/PC53通过。

## 2. 完整正常合同与生产消费

### 1. Sub_406260

action原空返回，非空固定payload且无payload空门禁；先S32 y04-y0C以F64运算/F32参数，后x00-x08同样落F32，46F970结果F32写angle50后405CB0一次归一/更新方向；不回绕S32差或以屏幕坐标替代。

### 2. ApplyBattleEffectFlags_406390

原无action/payload空门禁，固定payload/C9C后flags先position入口快照；五bit独立依序200000→图22329、400000→22330、800000→三次Advance的Aux四输出后22328/音效214/骑乘/装备、1000000→22330、2000000→22328/音效214。初flags2000选sourcePosition，否则普通块固定position、Aux块输出effectPosition；Aux推进游标后末两块仍用入口flags/position。所有icon/audio/rider/gear间fresh查全对象表；audio后riding再image先查骑乘对象，骑乘返回gear先查装备对象；不复用旧指针，不把位条件改互斥。

### 3. TakeBattleAuxEffects_43BC90

原无action/payload/输出指针空门禁，固定payload先Advance439470；每次输出单独freshC9C：position→position3(value3)→position4(value4)，输出可能别名游标，不能固定同一record。第二Advance后freshC9C-position3(value5)，第三Advance原S32结果返回；不在sentinel位置==-1时自加早退。

### 4. NewBattleFloatingIcon_41F4C0

NewYobiAction(2,0)失败原nullptr返回，成功固定原flags→callback41F450/优先级W8 CC/flagsOR40000/播放率W32 100；S32 offsetX+parent F32X落F32后S32 offsetY+parent F32Y暂存F64，速度W32位型C1000000(-8F)/imageW32/方向0/两重生W8=4/原YF32与W16field270=0按序；PlayFrame(1)后X正常S32截断→fresh F32Y→Xpub→Y截断pub，返回native action。parent仅本次取坐标，无父指针保存/持有，不补空门禁、屏幕偏移或析构。

### 5. UpdateBattleFloatingIcon_41F450

固定入口phase：0读F32速度+原0.8F为未舍入和，先与7.2F比较再FST存F32；达阈值phase1，Y仍用未舍入速度和相加落F32，不在本次继续phase1计数。入口1将旧deltaX+1正常S32发布，>=60才MarkDeath，仍投影；其它phase不速度/计数。所有路径X正常S32截断→F32Y样本→Xpub→Y截断pub；原不PlayFrame/不parent/deathFlag门禁、MarkDeath不即刻free。

Apply406390的三个Aux输出图号和effectPosition按原四指针合同传入43BC90，后者每次Advance后使用已固定payload的fresh C9C；四输出写入之间允许与C9C别名。输出顺序及第三Advance结果保，不固定一个record、不添加sentinel早退。Apply自身固定入口flags/position，因此三次推进、图标、音效、骑乘/装备调用后末两位仍消费初记录，不能刷新为已经移动后的记录。

Apply中各位独立顺序200000→400000→800000→1000000→2000000，不合并或互斥；五块均由初flags2000选对象，Aux块非2000时用effectPosition，其它块仍用初position。可写全对象表每次调用重新取目标：图标后音效fresh、音效后骑乘fresh、骑乘后装备fresh；不能长期持有或复用旧对象。既有音效214二参数接口按当前暂停边界保留；原三栈字中的Y归档且不消费，不改音频API或宣音频运行已验。

浮动图标parent仅在创建成功后读取两坐标，原无父指针保存、同步父坐标或deathFlag跟随。NewYobiAction失败返回空保，成功后PlayFrame(1)返回才投影；更新无PlayFrame也无立即free，phase1达60只MarkDeath仍执行X/Y投影。真实回收由既有YOBI生命周期处理，不能把标死等同回收或凭本步宣布退出/重入owner闭包。

<a id="battle-flags-aux-icon-consumer-decision"></a>
## 3. 差异、裁决和实施

| 差异 | 原EA证据 | 本线程裁决 |
| --- | --- | --- |
| 浮动更新phase在两个判断中重复读取 | 41F455/41F458/41F45D/41F45E | 两补丁固定单次入口phase，0/1/其它职责保持 |
| 更新先存回速度F32再做阈值比较，原先比较未舍入和后FST | 41F47C/41F482/41F488/41F490/41F499 | 先固定reachedPeak，再存F32/更新phase；Y仍用未舍入和，不读刚存回F32 |
| 图标创建及更新先Xpub后读Y，原X截断后先取F32Y才Xpub | 41F54D/41F552/41F555/41F558/41F55D；41F4A2/41F4A7/41F4AA/41F4AD/41F4B2 | 各一补丁：X正常S32截断→F32Y样本→Xpub→Y截断/pub，创建不缓存播放前坐标，更新标死后仍投影 |
| Apply骑乘/装备实参的图号和对象查表顺序由C++求值决定 | 40647D/406481/406485/40648F/406494/406498；4064D0/4064D8/4064DD/4064E6/4064EB/4064EF/4064F4/4064FC | 一补丁显式audio后riding/image→fresh骑乘对象；Rider后gear→fresh装备对象，借用指针只覆盖对应调用 |
| Reset和Aux正文正确、五原ID缺独立记录 | 原库存、全根和当前派生前态 | 不改正确正文，新增五独立语义/分类/BATTLE成员、每根局部I/S，不代176引用全部父完成 |

共六个精确补丁，三个函数改动、两个正确函数保持。它们恢复源表达/取样，不凭静态推断旧画面差异，不引入新原子/volatile/通用包装、异常拒绝/保护/回绕、父owner或音频实现。正常S32加减/截断以及F32/F64落点保，越界/数学溢出按RPLAN.24不专项处理，也不宣异常等价。

<a id="battle-flags-aux-icon-consumer-acceptance"></a>
## 4. 可度量局部验收

| 验收项 | 本步证据 | 限制 |
| --- | --- | --- |
| S 原函数/引用 | 五完整原根286条952B/六页、26call/22内部分支/六ret与8B常量；175call和单callback地址引用分列 | 不以地址引用数量代全部父验收 |
| I 还原与接线 | 完整CPP仅六补丁精确重放；原Reset、Aux/Advance以及骑乘/装备正文/公开声明保持；游标四fresh读/三Advance、flags五独立块、fresh对象查表及创建/更新F32投影序定向核 | 未构建/链接/运行；owner回收/重入、全数据生产消费/共享表修改者与宿主PC53继续 |
| 映射/登记 | 五独立ID/adapted分类、五组局部I/S10条；BATTLE94→99，语义1392→1397/有效ID1387→1392、分类498→503、关联1505→1510/3680=41.03%、E579→589 | 成员枚举仍open，全部功能gates保持；关联比例不是整体完成率 |
| 进度/门禁 | 六文档追加、七open任务追加本步进展/引用及唯一进度；measure/-Check派生文本 | 175队列、R7区间67/3061、60任务源码closed10/完整验收0/P01及R1/R7/R8不变；global_accepted仍false |

仅必要定向文本静态与获准汇总器；不构建、调试、新增/执行测试、运行客户端/算法或计算任何hash。完整功能验收标准不降。

<a id="battle-flags-aux-icon-consumer-source-state"></a>
## 5. 状态与后续

本SPEC先落盘，本线程随后实施六处还原、原证据归档、五独立基线与局部登记、看板/唯一进度同步，并精确提交。固定命名快照：`cg2d-re-battle-flags-aux-icon-consumer-20261006`。

本步五正常函数纳管；正确的Reset/Aux及已过Advance-方向-附件生产者保持。沿176引用中未完成的真实父/消费者和owner继续开发，全部闭包、原音频/资源重进退出、ABI-PC53-画面与R1/R7/R8仍待，不以本步局部I/S声明整体完成。
