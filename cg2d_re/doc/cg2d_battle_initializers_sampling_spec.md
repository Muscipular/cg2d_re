# BOB01 五个战斗特效初始化助手

<a id="battle-initializers-sampling-baseline"></a>
## 原端与实际差异

[完整证据](cg2d_battle_initializers_sampling_evidence.json)覆盖五独立单chunk：422E50..422FF7的138条423B、423040..4231CE的120条398B、4231D0..42334F的115条383B、423350..4234AF的112条351B、4234B0..423631的123条385B，共608条1940B十页、30call/35普通内部分支/五ret。Target另有十五W32槽switch60B，100/200两个F32常量8B；完整表与实际字节均归档。

五原ID此前无独立语义/分类/战斗成员，历史定义已有，不在175定位队列。六普通入向全部来自已过[BNZ01共同创建](cg2d_battle_effect_factory_sampling_spec.md)：Target两处，其余各一处；这里只核对实际助手边界，不重新全面复验父。

现五根将数组记录直接按值传入Append，未显式保持原四字段取样次序。Target/Lower是position4→flags→position3→position，另三根flags→position4→position3→position；不得把共同父的flags先读外推给所有助手。另有Center的当前游标应先于新payload读取，以及Offset的targetX应先于position值读取。本步六补丁解决全部上述取样差异，保所有其它源码。

<a id="battle-initializers-sampling-decision"></a>
## 本线程裁决与完整合同

按既有用户授权先SPEC后实施：五Append处固定一次S32 effectIndex，按各原顺序独立保存U32 flags及三个S32值，再沿既有16B数值结构按值传送。Center在effectIndex之后固定新payload；Offset在Load之后固定fresh positionIndex、取targetX，再取position值进行原正常double加减。Append之后辅助门禁继续重读live C9C/flags，不复用旧记录样本；Aux消费三个后续项，四输出传值保持。

| 根 | 完整正常行为 |
|---|---|
| 422E50 | 固定source/action payload与初始target；callback/C9优先级→固定C9C按4/flags/3/position传值；Append后重读当前flags，800000才四输出Aux；rate100、liveC9C非零才delay RNG；复制原image后十五槽音效分派，复制技能/等级，Load固定target后取X/Y、W16清270 |
| 423040 | 固定source/action payload与初始target，callback后OR40000；效果flags/4/3/position传值，Append后liveflags/Aux；rate100/liveC9C条件delay后无条件图RNG0..2；复制技能/等级并Load固定target，liveC9C→targetX→position判>=10后±100；graphic→targetY→DX的S16半高，Y-200-halfHeight，W16清270后Y发布 |
| 4231D0 | 先action flags、source payload，再callback/C9/OR60000；固定C9C先新payload，再flags/4/3/position传值；Append后liveflags/Aux；rate100/条件delay/image→两W8值4→speed RNG4..8、复制技能/等级→320/260；转向时fresh当前目标/TurnToward→angle RNG-90..90→W16清270→double加live角度后F32发布 |
| 423350 | 固定source payload与target，callback/C9后真实设备查询；单blendFlags两次发布再OR40000；固定C9C按4/flags/3/position传值，Append后liveflags/Aux；rate100/条件delay/无条件图RNG0..2→两W8值4→固定targetX、对象2CC S32半偏移加targetY，W16清270后F32 Y；不复制技能 |
| 4234B0 | 固定source payload/target后取初flags，callback/CC/OR60000；固定C9C按flags/4/3/position传值、Append后liveflags/Aux；rate100/条件delay；skill20/24/28直接图号否则图RNG0..2后liveimage；两W8值4→固定targetX发布→side非空才fresh当前position写S32±1→target屏幕Y+1→W16清270→F32 Y |

五根真实action/source空门禁全部保持，ScreenY的side空门禁也有真实原指令4235F0/4235F5，保持可空输出；没有新增payload/target判空、容量保护或异常模型。原所有RNG条件与先后、callback绑定、设备查询、目标/载荷固定与重取边界、正常S32加减和F32/F64中间值、W8值4/W16零都保留；这些零是原业务值，不是补零或钳制。原800000门禁以实际operand/字节为准，不把IDA渲染的地址标签当字段语义。

原100/200字节分别0000C842/00004843。Offset使用DX +6有符号S16高/2，Lower使用对象2CC有符号S32偏移/2；两者不能混用。Target图号111177..111191中111180..111182走默认，其余九项音效263、末三项269，原额外Y栈字沿既有52C450接口说明未被本体读取，音频暂停，不修改接口或宣音频验收。

action/source/target/payload/真实callback及可空S32* side地址沿获准native非拥有接口，其余数值原宽，不新增owner/free。当前语言级取样恢复不代编译器实际指令顺序、x87 PC53或ABI运行证明。

新增语义1373..1377分别登记五原ID、adapted分类及FUNC-BATTLE成员75→80，完整枚举仍open。六补丁已解决本步发现的源码问题，所有父/callee-owner/资源重进退出、SDK与运行继续沿实际依赖推进；网络原格式明文，音频暂停。

<a id="battle-initializers-sampling-acceptance"></a>
## 可度量验收

| 项 | 限定依据 |
|---|---|
| I | 五根六补丁/整CPP从HEAD精确重放；各四值顺序、Center游标先payload、Offset游标/X/position、Append后live辅助消费与完整条件RNG、坐标和真实空门禁定向静态 |
| S | 五完整608条1940B十页/全字节、30call/35内部分支/五ret、switch60B与常量8B、六普通入向分列 |
| 追踪 | 五独立ID/分类、成员80，各函数局部I/S共十E；七open/唯一进度同步，所有旧记录和完整门禁保持 |
| 度量 | 语义1373→1378/有效ID1368→1373、分类479→484、关联1486→1491/3680=40.52%、E541→551；175定位队列保持 |
| 整体 | 不构建/测试/运行/摘要；全部父/callee-owner/资源-ABI-SDK-PC53-RNG画面与R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open/全部完整门禁保持。局部I/S不能替代功能整体验收，不外推异常输入、原EAX或生成机器ABI。

<a id="battle-initializers-sampling-source-state"></a>
## 当前收尾

五助手全部取样差异本步修复，目标/回调/附加记录/条件随机与坐标基线可独立追踪。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)/[看板](../plan/cg2d_measurement_dashboard.md)同步，固定快照 `cg2d-re-battle-initializers-sampling-20261006`。继续实际依赖的更新回调开发。
