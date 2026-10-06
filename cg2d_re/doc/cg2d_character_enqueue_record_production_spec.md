# BPC01 角色入队的实际记录与字节发布

<a id="character-enqueue-production-baseline"></a>
## 当前差异与独立原身份

既有RenderChara和[旧合同](cg2d_yobi_runtime.md)保正常最终值，但槽位重定位/容量取样合并、描述符整体复制、颜色及两个flags位按W32读取，以及mix/clip/image取样未逐原阶段承接。该原根只有冻结源码定位，机器记录/逐ID处置和两域成员为空，本步直接修正文并补独立可追踪基线。

[完整原证据](cg2d_character_enqueue_record_production_evidence.json)纳盘F:00121F20@[521F20,522173)160条595B/两页、3绝对call/28内跳/1ret；18普通调用点来自3个父43CBB0/51C430/51C7D0，仅入向库存，不代全部父正文或执行。三个call为名称456670一次、__ftol53B498两次，不把本函数视为直接调用普通图形入队。

<a id="character-enqueue-production-decision"></a>
## 工程裁决与单正文实施

依既有委托，只修改YobiRuntime.cpp的RenderChara完整正文，不新增helper/文件/公开字段、影子状态或owner，不改已批action/48B queue/4B cache3布局。保全原正常/失败门槛和调用语义，不以最终数组值一致代生产阶段对应。

1. **两实际游标与链**：入口先S16 count、真实start、order地址、start.next、record地址、end。正向遍历；成功入队尾才在W16 count递增后推进order/record。所有continue仍走action.next→fresh end公共尾；不复制/改/释放对象链或把入口end冻结。
2. **parity/名称**：flags两正常分支仍只cacheIndex=-2跳当前。嵌入名地址的正常有效对象按旧合同直接调用，包括空名；不测试首字符/长度、补NUL或新增名称门槛。先name地址，随后W32value、S8font、原W8layer、S8color、S32Y/X，沿既有callee声明缩低S16/S8，不改业务宽度。
3. **计数两次取样**：名称调用后S16 count重新定位实际order/record，再独立fresh S16 count判断原>=4096容量。图号-1、death严格1和flags低U8bit2各跳当前，不重置cacheIndex；不新增负count拒绝或图号限制。
4. **坐标/描述符**：先layer W8/pub live W16 index；X偏移S32、scale F32按原序读，F64乘/S64向零/低S32承接后才加frame X并pub；Y同样但取得实际descriptor目标在Y pub前，image样本/源descriptor地址在image pub前。保持原PC53默认正常域承接，不改CPH/浮点框架。四对W32实际源读→实际目标写依0/4/8/C进行，不整数组复制或私有快照。
5. **颜色仅字节**：首次mask W8只用于nonzero与bit1，bits2/4/8分别重取mask；每次命中只取实际color2的+2/+1/+0/+3一个U8，按red/green/blue/alpha发布descriptor10/9/8/11，不保存整体color2或引入统一缓存。
6. **mix两次pub**：初W8 mask非零才取旧descriptor13，沿初mask清位并pub；随后fresh mask→replacement W8，基于保存的cleared值合并并第二次pub，不重读已发布目标。后续flags100000仍最后覆alpha80。
7. **对象/尾**：pub同一action原生非拥有指针后freshflags按bit8优先value4=2、bit4为1、否则0；freshS16count符号扩展写actionCache S32。clip U8先取，再batch0/严格1改1；重取queuedAction引用后仅低U8 flags20覆FF，图号一次S32样本保3096..3105范围，最后W16count递增及两native游标推进。

无介入正常最终坐标/颜色等旧值一致，不宣称修复已有像素错误；本次修读取宽度/时点、真实目标和逐对发布。原角色名称等完整业务生产、所有动态别名/资源寿命仍独立推进，不预建未知状态。越界、溢出、异常不专项模拟或宣称故障等价，也不新增保护/钳制/补零/拒绝。

<a id="character-enqueue-production-acceptance"></a>
## 可度量的定向验收

静态核160条595B连续覆盖/真实稳定ID、3call/28内跳/1ret及18点3父库存；整CPP仅一个完整正文，原类型/存储/头/父与callee-provider保持。核名称/两计数、实际X/Y-target与四W32复制、各U8颜色/flags、mix/clip/image样本和公共链尾，对应原正常阶段；不新增或执行测试/算法样例。

新增机器1418关联已有原根；语义1418→1419/有效ID1413→1414、文档关联1531→1532（41.60%→41.63%）、分类539→540；RENDER51→52、YOBI55→56均枚举未闭，两域各正常局部I/S共4条E685→689。冻结定位1/旧行号不刷新，两域局部证据不是两个原函数/完整功能。

RESOURCE41/AUDIO32暂停、pending18、队列175/需核46/暂停25、R7区间67/3061与全部完整门禁保持；八open U08/U09/G01/G02/G04/R01/R04/V04仅进展/引用。60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false不变，不闭C09/完整角色绘制/Yobi或R1/R7/R8。

<a id="character-enqueue-production-source-state"></a>
## 当前交付

交付单角色生产正文、独立原证据/SPEC、机器身份1418/两域成员与分类、四局部static I/S、八open任务/唯一进度/看板对照。命名快照：`cg2d-re-character-enqueue-record-production-20261006`；14文件精确提交，用户既有六项排除。

所有18调用点之父、实际名称/坐标/动画业务上游及全部callee/正常别名、action-队列-图形-纹理-owner与清理/场景/失效/退出、SDK/机器FP ABI和像素运行仍未验；仅必要静态与measure/-Check，未构建、测试、运行或计算摘要。网络原格式只明文、内存CryptInt保持、音频暂停，整体SPEC继续。
