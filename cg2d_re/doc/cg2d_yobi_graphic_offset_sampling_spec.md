# 对象图形偏移、挂件绘制与动画判定 SPEC（YGOS01）

2026-10-10。沿帧播放后的绘制消费，纳管两图形偏移查询、动画图号判定和已有1919/CAC07挂件绘制四完整正常根。主体均已存在；只5182B0两轴查询显式恢复S16输出先读、对象S32偏移后读，两公头注释更新/四CPP关联注释，不称旧正常数值错误或新建整条地图链。

<a id="yobi-graphic-offset-baseline"></a>
## 原证与已有来源

| 原身份 / 当前源 | 完整范围（尾不含） | 指令 / 字节 / CALL | 入向 CALL / 父 |
|---|---|---|---|---|
| F:001182B0 / login::detail::Sub_5182B0 | 5182B0–5182FD | 32 / 77B / 1 | 86 / 21 |
| F:00118300 / login::detail::Sub_518300 | 518300–5183A6 | 55 / 166B / 3 | 6 / 1 |
| F:001183B0 / world::detail::Sub_5183B0 | 5183B0–5183CB | 9 / 27B / 0 | 1 / 1 |
| F:00041570 / login::detail::DrawGear_441570，已有1919 | 441570–44177D | 196 / 525B / 13 | 1 / 1 |

合292条795B/七页、十七E8/五唯一callee、二十五跳（含一真实switch JMP）/九RET；94实际入向E8、父集合合22。完整指令、入向、当前定义与差集见[证据](cg2d_yobi_graphic_offset_sampling_evidence.json)。两根在本步也是挂件父的callee，身份只计一次；三个新原身份、已有1919補完整正常范围与F适配处置，原CAC07子记录/入口保历史。

原4415DD六项表复用CAC07的X:0004177D-000417A0完整35B：3B对齐、24B六W32目标、8B NOP；guard为kind-1无符号>5进默认，表六项对应kind1..6。无新R7/X或表重分类。1252 __ftol、160 GetGraphicXY、1256 DrawGearNormal三已有callee身份保持；真实提供者分别为本步明确S64→S32转换、GraphicInfo.cpp和world/MapDrawCache.cpp，不把旧1252样例当本步函数体。

<a id="yobi-graphic-offset-decision"></a>
## 工程裁决与两轴读取

依子AGENTS第8条工程委托：先写本合同，5182B0在GetGraphicXY之后各轴先将S16输出符号扩展为S32，再读当前对象graphicOffsetX/Y W32，返回差值。用两个局部变量明确原5182D4→5182D9及5182E8→5182ED，避免未定序减法表达式承担阶段读取；正常旧值一致，不造保护/回退。

两入口参数axis现U8，承接原低BYTE观察，类型不变。即使axis不是0/1，两者仍先调用GetGraphicXY，再返回0；不提前拒绝或清绘制偏移。对象graphicIndex按原W32图号交唯一资源查询，输出各S16，查询返回弃用；原无action空门禁，未新增。

## 缩放查询518300与注释纠正

原518301先读kind、建立F32系数，3/4/5为1.75/1.5/1.25，其他为原0；这在GraphicXY调用之前，当前正确源码保持。公头原“先查询，再按kind系数”更新为先确定系数、再查询与按低BYTE axis消费。不是把当前switch后移或改变默认业务。

GetXY后axis0用S16 x、axis1用S16 y：经正常double乘F32系数、S64向零转换再取S32，之后fresh对象对应偏移减结果。两原__ftol物理点对应两个已有显式表达式，沿1252宿主转换边界；不新增Ftoll函数或承诺原x87控制字/EDX:EAX/private ABI、非有限/越域行为。现字段、F32系数、业务返回及RPLAN.24保持。

## 挂件绘制441570的完整正常消费

入口固定父payload，slot0先kind3C再借action34，slot1先kind40再借action38；空child跳过，原严格两轮，payload不跨轮重新获取。kind不在1..6默认不绘制。

- kind3/4/5：先借child内嵌drawMode，分别用518300(child,1,kind)取Y→当前父Y→参数offsetY，再取X→当前父X→参数offsetX，之后当前child图号。交既有DrawGearNormal(primaryIndex,before低BYTE,imageNo,X低W16,Y低W16,同descriptor,batch10/11/12)。
- kind1/2/6：同样先descriptor、Y查询/当前父Y、X查询/当前父X，再child图号；不加offsetX/Y参数。kind1的before固定0，kind2/6固定1，batch固定0。
- 六物理case的六5182B0 CALL和六518300 CALL由现两条语义路径各Y/X调用覆盖，唯一DrawGearNormal公共尾保持。现两表达式共四查询点不当少八调用或新缺口；同一child引用、descriptor与参数宽度/借用/原正常默认保持。

当前正确父正文不改；1919此前只有分派数据角色关联，本步補上述完整196条525B正常body及新F处置，保旧range_fourth_batch_data，不重复新身份或既有MAP成员。全父RenderChara442F70只取443F65–443F95局部调用窗/当前实际调用定位，不重验完整父或其余图形owner。

## 动画判定5183B0与创建入口

原S32 imageNo在[100000,200000)返回完整1，其余返回完整0，不新增图形资源查询/上界或动画装载。这是已存在world::detail局部声明范围的真函数，不增公头接口。

唯一445350父的445383–4453CE局部窗确认type>3、当前gMapPukMode==0时才调用；非动画返回0才选方向0，否则使用传入方向，再创建441DB0。现Sub_445350完整源码保持，只此方向门禁/真实callee接线有据，不称全543条原父或记录/文本/CryptInt/寿命已验。

<a id="yobi-graphic-offset-acceptance"></a>
## 限定静态验收

核四完整机器码/十七E8/二十五跳九RET、94实际入向及22父、两个父局部窗、复用35B全表/六target/guard，整CPP两轴两组/四注释与公头两注释精确重放；其余三正确body、所有其它源码/结构/帧播放及六表数据保持。三旧callee身份/当前提供者保持，不外推完整SDK/生成ABI。

三个新语义、已有1919当前元数据/来源完整化及新F处置；MAP三新成员、YOBI四新成员，均枚举false。四限定I/S分别归MAP/YOBI：I仅5182B0两组，S四正常根；G01/G02/U09/U11/R01/R04/V04仍open。全部完整gates/184叶验收0/60任务关闭11、六closure及范围政策不改。

原字段/参数宽度、正常返回/失败/借用和现WAV-only/SDL、MSVC CRT及批准数学边界保持；RPLAN.24不专项模拟/异常恢复，不加action空保护、索引/容量限制、默认值、补零、钳制或回滚。只读当前IDA，不重开/修改库，不构建/测试/运行/算法/产物/摘要或引依赖。全部生产、94完整入向父及传递callee、owner/alias/容量/SDK/机器ABI/画面声音/运行待，总SPEC§10不降低。

<a id="yobi-graphic-offset-source-state"></a>
## 当前状态

本步前Git提交说明“[#] 对齐对象帧播放与共享帧取样”，完成提交说明“[#] 对齐图形偏移查询与挂件绘制合同”。统一cg2d_re分支、不打tag。两轴两组/六注释已落实，四完整原根/94实际入向、35B六表与两父局部窗、整CPP/公头精确重放、1919十一当前属性/旧CAC07子记录及所有其它语义/政策守恒通过；静态文本汇总/-Check均退出0。派生仅六metrics/四F/两功能/七任务。结果语义2186（2138F＋48X）、合同2248/3680=61.09%仅关联率、F处置1616/E1419、MAP137/Yobi66枚举均false，184叶完整验收0/任务关闭11/六closure保持。七外部改动保留排除；完整输入/生产/94全部父/传递callee/alias/owner/容量/SDK/机器ABI与声音画面/运行仍待，整体SPEC未完成。
