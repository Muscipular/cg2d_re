# 命中特效创建与计时回调 SPEC（BHE01）

2026-10-10；沿258的真实HitEffect依赖补两完整原根，修创建/回调两段Y取样先于X发布。正确计数/上限持样、参数/初始化/播放及标死后投影保持，不判全部资源owner或SDK运行完成。

<a id="battle-hit-effect-baseline"></a>
## 完整原根与引用

| 原身份 / 当前源 | 完整根 | 原实际引用 |
|---|---|---|
| F:0001F5B0 / NewBattleHitEffect_41F5B0 | 41F5B0–41F65B，46条171B/一页 | 六实际CALL/五父 |
| F:0001F570 / UpdateBattleHitEffect_41F570 | 41F570–41F5A6，20条54B/一页 | 41F5C8写callback槽的唯一已知数据引用，零直接CALL |

合66条225B/两页，七直接CALL；创建三callee/一跳两RET，更新两callee/一跳一RET，无间接调用/分派表。回调引用是mov [action+8],41F570，不将operand元数据8当地址，不按零直接CALL判不可达；现通过真实Yobi callback调度执行。

六调用来自406E10两互斥等级路径、408170、40C120、4195F0及427470。六个后窗都在消费前覆盖EAX/AL或转共同尾不消费创建返回；当前五表达式均显式丢弃返回，406E10两原点由一个表达式按路径覆盖，不补重复创建。后窗只定位/证明该局部消费，不代五父或全部callee的完整合同。

## 创建参数与字段发布

五参数是非拥有parent T*及offsetX/offsetY/lifetime/imageNo四S32；New(priority2,payloadSize0)，现保相同零额外载荷请求，不据此断言宿主零尺寸分配指针必空。分配失败即返回空；成功读flags、发布callback41F570、priority低BYTE CC、flags|40000及rate100。

Xoffset正常S32转数学中间值加parent.offsetX F32，发布source.offsetX F32；Yoffset加parent.offsetY先保存未发布结果，随后value4C位型C1000000（-8F）、imageNo、两个rebirth值4、offsetY F32、deltaY=lifetime及direction0依原序发布。没有保存parent指针、额外载荷访问/寿命镜像或显式deltaX重置；新对象初始计数沿既有New初始化，不新增固定默认或拒绝参数。

PlayFrame(source,1)弃返之后，原读取offsetX并正常截S32，接着读取offsetY F32，才发布screenX，再将已取Y正常截S32并发布screenY；field270低W16清0，返回保存的action T*。现旧表达式先发布X再读Y，改为held screenX→held screenY F32→X发布→Y转换/发布。播放之前的父偏移/字段/数学和返回保持，不把坐标提前到播放前。

## 正确计时与标死后的投影

原先取deltaX旧S32到EDX、deltaY上限S32到ECX，旧值+1保存到EAX后发布deltaX，再用保存count与保存maximum比较；count>=maximum才MarkDeath(source)。当前previous/maximum/count局部与原一致，完整正文保持，不改为先发布后读上限/计数或添加lifetime下限、溢出/回绕模拟。

无论是否标死都继续投影尾，且MarkDeath后才fresh offsetX/offsetY；原Y取样先于screenX发布，同创建器恢复持样screenX→读取Y→发布X→转换/pubY。回调没有PlayFrame/Move/父跟随/当场free或清callback；不因为标死早返，void签名与既有调度不消费原投影余值的合同保持。后续删除沿YLC/YAS已证正常Yobi路径，完整调度门禁/间接alias/资源及实际SDK生命周期另验。

<a id="battle-hit-effect-decision"></a>
## 两段取样修正

一个CPP两根各一段投影取样，另CPP/公头各两注释；计数/初始化/播放、原宽度/输入/失败/返回与全部其它源、动作15/回旋创建/21—23保持。无新函数/类型/API/保护或字段；原数学/CRT差异沿已批合同，不恢复私有ftol/x87机器ABI或声称正常SDK数值已验。

新增两独立语义/F adapted/两个BATTLE已知成员，枚举false；限定I只两投影片段，S覆盖两完整现源，U08/U09/R01/R04/V04只补来源/局部描述，原分母/六closure/完整门禁保持。按已获工程委托依SPEC落实；RPLAN.24、WAV-only/原格式明文、MSVC与排序未决边界保持，不编译/测试/运行/算法/查产物/摘要或引依赖。

<a id="battle-hit-effect-acceptance"></a>
## 必要静态核对

核两连续根/库存、66条225B/两页、七directCALL/各callee/跳/RET，callback唯一数据生产及六实际调用/后窗；原四S32参数/字段发布/PlayFrame后投影、正确计数持样/发布/比较/标死后fresh投影均有据。核整CPP仅两段/两注释，公头两注释，其余全部正文/签名/源保持；首计数不模拟运行或执行计时/投影样例，引用的分配/标死/调度删除合同只复用不重计新根/全寿命验收。核两身份/处置/成员、限定I/S/五任务和政策/门禁/分母、UTF-8无BOM-LF及静态汇总/-Check。所有输入/父-callee/间接alias、SDK零尺寸/容量失败/正常数学/重进退出及资源owner/画面运行仍待。

<a id="battle-hit-effect-source-state"></a>
## 当前状态

两完整根与两段差异已实施并完成必要静态核对，静态汇总与-Check通过；前态cg2d-re-battle-hit-effect-history-20261010，提交引用cg2d-re-battle-hit-effect-20261010。语义2051（2003F＋48X）、合同2113/3680（57.42%仅合同关联率）、F处置1478、软件证据1368，BATTLE已知成员291且枚举false；全部完整门禁保持，未编译或运行。整体SPEC未完成。
