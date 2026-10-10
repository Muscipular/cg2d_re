# 战斗附属特效开关与共享跟随 SPEC（BAS01）

2026-10-10。147/148与反转创建/共享回调/减益创建五完整正常根。只共享41F800回调Y转换后再取屏幕Y偏移一组，CPP/公头各五注释；其它主体保持。1913/CAC07减益创建此前仅数据角色关联，本步补完整正常根及处置，原数据子记录保历史；四新语义身份，不重复计1913。

<a id="battle-attached-effect-baseline"></a>
## 原根与复用边界

| 原身份 / 当前源 | 完整范围（尾不含） | 指令 / 字节 / CALL |
|---|---|---|
| F:00011320 / UpdateBattleAction147_411320 | 411320–411391 | 34 / 113B / 4 |
| F:0000FEF0 / UpdateBattleAction148_40FEF0 | 40FEF0–40FF35 | 26 / 69B / 2 |
| F:0001F860 / NewBattleReverseEffect_41F860 | 41F860–41F942 | 67 / 226B / 5 |
| F:0001F800 / UpdateBattleDebuffEffect_41F800 | 41F800–41F85F | 30 / 95B / 4 |
| F:0001F660 / NewBattleDebuffEffect_41F660，已有1913 | 41F660–41F7DD | 100 / 381B / 4 |

合257条884B/七页，19 E8/八唯一直接callee；十八跳含一真实间接switch JMP，十RET。147/148各唯一父404DB0实际CALL，本步取40573C表两4B项/两11B case，均push→CALL→JMP40563F。反转创建三CALL/三父411320、422370、438550；减益创建34 CALL/七父；共享回调仅两callback字段MOV绑定41F743/41F88D，不当CALL。总39 CALL入向与两MOV分列，不将xref的iscode/type代实际指令类别。

减益创建41F68D是真六项跳表，复用CAC07的X:0001F7DD-0001F800完整35B（3B对齐/24B六指针/8B NOP）与guard取用窗口；source switch映射六图号，既有X分类/区间不改或重计R7。原1.0 F32常量54D494四加载字节0000803F。完整根、数据与源差集见[证据](cg2d_battle_attached_effect_evidence.json)。

154–156、159/160/162、163分别已有BPS01/BPT01/BPU01完整正常源合同及1432–1438记录；保持、不重复取证/计新身份或S覆盖。尤其162已有W32 FDC门禁、fresh1024严格2及清通知→hidden→2D8→Advance顺序；163已有进入/退出四槽/借用/类型W8及原第三槽判断边界，仍不代完整运行寿命。

## 147开关与148等待

147固定入口payload，先一次W32读取FDC，原TEST AH08对应完整flags&800。未置位时NewReverse(source,110152)弃返，之后fresh完整FDC OR800发布，即使已有对象/分配失败返回空亦置位；最后AdvanceCast。不把失败改回滚或给空返回加新保护。

已置位时无空门禁MarkDeath(payload.effect105C)，返回后fresh完整FDC清800，先105C=null再发布保存flags，AdvanceCast。原AH F7只是保存W32样本的寄存器位运算，不将内存字段/读取缩成BYTE。现源两个分支与共同推进正确，主体保持。

148固定payload，先W32读FDC/AL60判断，清时PlayFrame(source,0)弃返；之后fresh1024==0才NewEffect(source,Y0,type33,0,1)弃返。所有路径phase162，不额外推进、清1024/hidden或保存+10；162合同复用，不重复登记。

## 反转创建41F860

入口父payload.105C非空即返空；否则NewYobi(priority2,payloadSize0)，失败返空。成功绑定完整41F800、priority BYTE C8/rate100→真实HasLegacy3DDevice查询，返回后才读新对象完整flags。按设备非零OR100000或零OR8000，先发布该值，随后再OR40000/第二次发布，不合并两次保存。现HasLegacy3DDevice实际提供者为RendererResources.cpp的textureInterface非空状态，沿已批准视频替换边界，未重验原COM/SDK/像素。

从父offset40取F32位值并发布，之后父offset44+原1.0以double中间/F32落值；图号参数、方向0、309/310两BYTE4保持。PlayFrame(new,1)弃返后fresh父payload→105C=new，再new.+10=父原生非拥有T*。X向零S32转换→屏幕X W32偏移→Y样本→屏幕X发布；Y向零转换→屏幕Y W32偏移→field270原W16清零→屏幕Y发布，返回新对象。入口payload不跨播放复用，已有对象分支不返已有T*。

## 减益创建41F660与1913推进

入口父payload.1054非空立即返空，不再更新状态。图号110075–110080六case分别保存原完整FDC OR10/20/40/80/100/200；之后重新读取父payload，先13D0=1–6，再13CC=2。原CL/CH OR只是寄存器操作，字段仍W32。其他图号按原默认直接创建，不更新这些状态；源局部type0仅控制原未匹配路径，不给载荷补默认值。

这些更新发生在NewYobi(priority2,payloadSize0)之前，失败返空不回滚。成功持新对象flags→绑定同一41F800→priority BYTE9/rate100→flags OR60000；父X→父Y+1.0、图号/方向0/两BYTE4/F32 Y沿原发布。PlayFrame(new,1)之后fresh父payload写1054，再绑定new.+10父T*；同序投影/清W16 270→返回新对象。现正确主体保留。

1913的CAC07只证明表/源角色，本步补上述全部100条381B正常创建/失败/发布范围。原range_fourth_batch_data完整保历史，旧原身份/入口不改，新增一F适配处置；BATTLE原已包含该成员，不再添加。其余四根新增四语义/F/成员；不把当前语义关联或原表目标当完整owner/机器ABI通过。

## 两创建器共享的完整回调41F800

入口取当前+10父借用，deathFlag严格1时MarkDeath(self)；否则PlayFrame(self,0)后重新取+10父，父X发布到self.offset40，然后同一父Y+1.0落F32 offset44。标死路径不更新跟随坐标，但仍执行共同投影；没有提前return、父槽清理、物理释放或通知写入。

共同尾X向零转换→屏幕X偏移→Y样本→X发布。原41F84F先完成Y转换，41F854才读屏幕Y偏移，41F85A发布；本步显式projectedY→offsetY→y一组，避免未定序加法表达式承接两个阶段。两创建者受同一callback影响，一并核真实绑定和正常/失败分支；不为1054/105C造私有影子owner。__ftol当前是本步源S32显式转换表达式，复用1252宿主边界，不引入新Ftoll函数或称原S64机器ABI。正常旧值一致，source读取顺序明确不称运行故障。

<a id="battle-attached-effect-decision"></a>
## 工程裁决

依子项目AGENTS第8条工程委托，先写本SPEC后恢复共享回调一组顺序，补五完整原根及两实际分派/两创建者共享关系；四正确主体/原类型/数据/失败/字段发布与所有其它源保持。五旧callee独立语义复用，反转创建是147 callee并同时本步根，不重复计身份；PlayFrame和设备查询仅定位当前提供者，不代完整独立合同。

当前WAV-only/SDL及具名数学/CRT宿主边界保持。原F32、S32/W32、BYTE/W16及已证原生T*不扩业务宽度；正常及失败/返回/随机消费沿原；RPLAN.24不专项异常/模拟、保护、回滚、钳制、默认值或索引限制。只读当前IDA，不重开/修改/保存库，无新依赖，不构建/测试/运行客户端或算法/检查产物/摘要。

<a id="battle-attached-effect-acceptance"></a>
## 限定静态验收

核五完整根/连续机器码/E8与实际一switch、两表项/两case、39入向CALL/两callback MOV、复用35B/六表target及4B常量；整CPP仅一组/五注释、公头五注释重放，四正确主体/已登记动作/共享器及CAC07旧原证保持。

四新语义、已有1913补完整元数据/当前来源及新局部结构，旧CAC07子记录保历史；五新增F处置、四已知成员，限定I只共享callback一组/S五正常根。U08/U09/R01/R04/V04仍open，BATTLE枚举false、全部完整gates/184叶/60任务与六closure保持。全部数据生产/所有父-callee/容量/机器ABI/alias/owner/SDK/声音画面/运行待，总SPEC§10不降低，整体未完成。

<a id="battle-attached-effect-source-state"></a>
## 当前状态

本步前Git提交说明“[#] 对齐战斗场地特效与状态清理”；完成提交说明“[#] 对齐战斗附属特效开关与共享跟随”。同一cg2d_re分支，不打tag。一共享回调组/十注释已落实；五完整原根/两分派/39CALL与两MOV及复用数据、整CPP/公头重放、1913十一当前属性/旧子记录和所有其它旧记录/政策守恒、引用/编码/差异及派生范围静态通过，文本汇总/-Check均退出0；七项用户外部改动保留并排除提交。

静态结果：语义2177→2181（2133F＋48X），合同2239→2243/3680=60.95%仅关联率，F1605→1610、E1409→1411、BATTLE417→421且枚举false。四新语义/成员及已有1913当前完整范围与F处置分列；派生仅六metrics/五F/一功能/五任务。导入16/218、X3060/3061余一、184叶完整验收0、60任务关闭11及六closure保持；全部输入/生产/父-callee/容量/机器ABI/alias/owner/SDK与声音画面/运行仍待，整体SPEC未完成。
