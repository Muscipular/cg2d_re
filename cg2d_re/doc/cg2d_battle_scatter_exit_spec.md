# 战斗终帧、散射创建与退场完整合同 SPEC（BSEX01）

2026-10-10；补动作32–36及41E510创建/41E4D0回调七完整正常根。现40次尝试、碰壁/直线退场、三RNG与回调投影正确；仅七消费表达（两WORD/五BYTE）和十四合同注释、独立登记需本步落实，不新增函数/API/保护/创建/移动/播放或RNG。

<a id="battle-scatter-exit-baseline"></a>
## 完整原根及生产接线

| 原身份 / 当前源 | 完整连续根 | 指令 / 字节 / 原CALL |
|---|---|---|
| F:0000B410 / UpdateBattleAction32_40B410 | 40B410–40B444 | 15 / 52B / 两CALL两callee、一跳一RET |
| F:0000B450 / UpdateBattleAction33_40B450 | 40B450–40B484 | 15 / 52B / 两CALL两callee、一跳一RET |
| F:0000B5E0 / UpdateBattleAction34_40B5E0 | 40B5E0–40B921 | 244 / 833B / 十一CALL七callee、三十一跳一RET |
| F:0000B930 / UpdateBattleAction35_40B930 | 40B930–40BACF | 124 / 415B / 八CALL六callee、十五跳两RET |
| F:0000BAD0 / UpdateBattleAction36_40BAD0 | 40BAD0–40BB18 | 24 / 72B / 两CALL两callee、两跳一RET |
| F:0001E510 / NewBattleScatterEffect_41E510 | 41E510–41E5C0 | 50 / 176B / 四CALL两callee、一跳两RET |
| F:0001E4D0 / UpdateBattleScatterEffect_41E4D0 | 41E4D0–41E50C | 21 / 60B / 五CALL四callee、一跳一RET |

合493条1660B/十一份最多64条分页、34directCALL/十二唯一callee、52跳九RET，无间接调用/跳转。五动作各一个入向CALL/404DB0，case32–36各11B push→CALL→JMP40563F取齐；创建器四实际CALL/四父（34、35及另外两父），回调一个MOV数据生产/零directCALL，不能据无直接CALL当未接线。九实际入向CALL/去重五父，另一个回调生产MOV单列，不代全部父/ABI/owner验收。

## 32/33与36：严格终帧和等待计数

32依次characterAction8/rate100，33为9/rate100；各PlayFrame(action,1)严格==1才AdvanceCast，不改非零为成功，不重取payload或加计时/移动。两正确正文只注释纳管。

36固定payload，完整FDC读后低AL 60h全清才PlayFrame(action,0)弃返，随后fresh FF0+1为count并发布。count>=30才FF0=0后AdvanceCast；禁止提前计数读取或在flags不符时跳过计数，不恢复速率/改变角色动作。仅显式U8消费；原S32正常计数/归零及所有门禁保持。

## 34/35：时长、活动发布与40次散射

两根固定payload、characterAction8；完整FE4读取后AL bit4或AH 0Ch任一置才9，当前低WORD C04一次消费；然后rate100。34先FCC+1发布后fresh完整W32 FDC移位bit1取14/28 duration；35先持旧FCC、完整W32 FDC flags再发布FCC+1，duration沿该flags计算。不能合并两根时长取样。

count<duration都先角度持样加180→速度5→角度F32发布→持中间>=360才一次减360再发布；fresh FDC低BYTE bit2置时依次全局gate=1、aliveFF8=1，再Move。34进入共同播放重试尾，35直接一次PlayFrame(1)返回。

count==duration时34先PlaySe(207,action.x)再fresh FDC低BYTE bit2；35使用入口持样flags低BYTE bit2，无207声。符合时CountBattleActiveEntries<=1先全局gate=0，再fresh完整stateFE0持样、aliveFF8=0、持样state低AL OR2/完整W32发布。计数先于当前alive清零，FE0取样先于alive发布且不截其它位。

随后两根都固定尝试40次NewBattleScatterEffect(action,110004)，每次弃返/失败仍继续全部次数；原仅一个静态CALL位点，各真实循环40次，不加成功计数、失败break、额外创建/RNG或重试。34之后FF0=0→FD0=0→速度15；35只FF0=0，不新增34的速度/FD0发布。

## 34：四边反弹和播放至非零

晚期（count>=duration）先Move，再direction旧值+1发布、保存值>=8再一次减8发布；fresh FF0>=3时只检查offset X/Y是否超原[-200,840]/[-200,680]，超出AdvanceCast后FD0=0，不在此父MarkDeath/立刻释放。原范围/判断保，不新增钳制或异常处理。

FF0<3时LoadGraphic(action.graphicIndex)弃返，再fresh graphicIndex取DX width低S16，一次halfWidth=width*0.5供两横边。现真实ReadGraphicDxWidth返回int16_t，源在GraphicDxCache.cpp，宽度与本步加载/读取顺序保持，不用旧字节步长裁原生指针或替换为无符号宽度。

按原else-if顺序最多一次碰边：左halfWidth>X且FD0!=1；右X>640-halfWidth且FD0!=2；上-negative graphicOffsetY>Y且FD0!=3；下Y>480且FD0!=4。分别原角度-90/-270/-180/零作反射，两次角度读取与原FSubR内存重读保持；左/右/上先delta=角度-常量，随后fresh角度-(delta+delta)发布，下边一次持角度-(角度+角度)。不强行合并取样或归一化角度。

碰边之后fresh FF0加1/发布，PlaySe(208,action.x)，再FD0=相应1..4；音效先于最后FD0发布，仍走共同尾。共同尾每次只PlayFrame(action,1)，至少一次，返回0持续再次播放，首次非零（不是严格1）才返回S32；不加循环上限/Move/重新做碰边。原循环/两静态CALL位置与现do-while对应。

## 35：每帧尾迹与直线退场

count>=duration时每帧NewBattlePositionEffect(action,110005)弃返（不加FF0==0门禁），创建后fresh FF0旧previousCycle→发布+1→旧值>=2归零；随后速度15→Move，fresh offset出同一原范围才AdvanceCast，末PlayFrame(1)一次返回。无需等40次散射成功，不复用FF0创建前持样，不添加反射、FD0或原没有的声207/208。

## 41E510：分配、真实回调和三RNG

NewYobiAction(priority2,payloadRequest0)，空直接返空、不消耗RNG。成功固定新T*，先callback=41E4D0、displayPriority低BYTE CCh，再依序真实RandomRange参数(10,200)/(0,359)/(5,20)：第一返回后读完整action.flags，发布rate→flags OR40000h→imageNo→direction0，第二返回转F32/发布angle50后才第三，第三转F32/发布value54。BVU01随机源只复用，参数/消费次数顺序不扩。

接着复制parent.offset40 F32发布新X，之后fresh parent.graphicOffsetY S32按原向零/2，再fresh parent.offset44 F32作中间double加法；field270低W16=0后才offset44 F32发布，返回新T*。原无parent保护/新父链接，不新增；请求0不据此保证宿主payload指针为空，也不以原S32指针运算承载x64地址。

回调唯一MOV生产在41E52C，左operand元数据8是callback字段偏移，右41E4D0才实际函数地址；现真实函数指针赋值，不是假体或同名stub。其余两个创建父仅实际调用定位，完整数据/owner仍待。

## 41E4D0：移动、标死后仍投影

Move→PlayFrame(action,1)，严格==1才MarkDeath弃返；标死后继续投影，不当场释放/return。先offset40 F32正常S32转换持X，再读offset44 F32持Y，发布action.x，再正常Y转换/发布action.y。现正文正确，不把两坐标读取顺序改为X发布后才取Y，不改严格终帧或为失败补值。原两__ftol职责沿CPH01宿主正常转换，全部异常/ABI/SDK数值与正常生命周期运行未验。

## 原数值/复用范围

十F32常量40B：360/180、680/840/-200/480/270/640/90/0.5完整实读；不重计新X/函数或新增数学算法。当前double中间、F32字段及已批准数学近似、RPLAN.24保；原正常负值/范围及源码发布序不降低。

YAS01分配、现222号404D00五条标死体、BVU01 RandomRange、CPH01 __ftol、GDL01加载、BNV01位置创建、BHRC01活动计数和BOG01移动仅复用。原七已有语义记录/身份/处置及所有数值/其余源保持，不再重复取整个未改根或把局部接线当完整owner/SDK验收。

<a id="battle-scatter-exit-decision"></a>
## 本步实施和登记

按既有工程委托先SPEC后落实：34/35各入口C04 U16/早期fresh FDC U8/边界34 fresh与35 cached FDC U8共六表达，36完整FDC后AL60h U8一表达，总七（两WORD/五BYTE）。完整W32时长移位和FE0持样/整字段发布保；只另CPP七定义注释/公头七注释。32/33与两散射完整正文/签名保持，所有其它源、数值、几何与调用不改，零新函数/API/保护/额外RNG/声/移动/播放。

七原身份此前无独立语义/F处置/已知BATTLE成员，新增七语义/adapted处置/成员，枚举false。I只34/35/36七表达，S七完整现源/当前创建—回调—标死投影正常合同，不计新全实现或所有owner/资源/SDK/运行已验；U08/U09/R01/R04/V04补来源/局部状态且open，全部原分母、六closure/政策/门禁保。CRT排序此前选择未答复，WAV-only/原格式明文/RPLAN.24保，不构建/测试/运行/算法/产物/摘要或引依赖。

<a id="battle-scatter-exit-acceptance"></a>
## 必要静态与剩余

核七完整493条1660B/十一页/库存、34E8/十二callee/52跳九RET、五11Bcase与创建四CALL/回调MOV生产分列；三RNG参数/顺序/空早退、40次尝试循环、投影读齐/标死后继续、七WORD/BYTE、34/35不同持样/FE0持样与发布、Load后fresh索引/S16一次半宽、四边原优先/两次角度读/音效后FD0发布及34直到非零真循环。

核整CPP仅七表达/七注释、公头七注释，全部其它本体/签名/数值/几何/调用及已复用七记录精确守恒；七语义/F/成员、限定I七表达/S七根、五open任务/原分母/所有政策门禁、UTF-8无BOM/LF与静态汇总/-Check准确。全部生产/其它父/完整callee/ABI/alias/owner、SDK正常失败/容量/数值/重进退出和声音画面运行仍待，总SPEC§10不降低。

<a id="battle-scatter-exit-source-state"></a>
## 当前状态

七完整正常根/七消费表达和十四注释已落实；必要静态、汇总与-Check通过，不含编译或运行。前态cg2d-re-battle-scatter-exit-history-20261010，提交引用cg2d-re-battle-scatter-exit-20261010。整体SPEC未完成。
