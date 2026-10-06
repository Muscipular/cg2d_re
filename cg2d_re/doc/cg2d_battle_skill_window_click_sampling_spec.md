# BQM01 战斗技能窗口与列表取样 SPEC

日期：2026-10-07。依现行工程委托先记方案后实施；只沿BQL01技能按钮的创建、返回、列表回调与动画依赖推进。

<a id="battle-skill-window-click-sampling-baseline"></a>
## 原版基线

[完整证据](cg2d_battle_skill_window_click_sampling_evidence.json)保留八函数573条1739B、十二页、22物理调用/十三目标、50普通跳转、一实际七项跳转表、十三返回及所有原字节。八根均已有源码定义，原语义台账/文档/处置为空；新增索引1520..1527，冻结定位全文保持，不把数据回调入向计代码调用。

六外部父邻窗36条115B仅证明所列调用/相邻取值；475AA0的107代码入向只登记，未据此宣称所有父闭合。原586548消息29B、5864C8提示25B按首NUL收束，含NUL共54B，与源既有显式字节常量一致；不能把IDA字符串项的对齐/后续文本计进C串。

37B区间 `X:0007A5FB-0007A620`：47A5FB一个90 NOP；47A5FC..47A618七个W32目标按mode0..6为47A5DB/47A5F8/47A5F8/47A5DB/47A5E8/47A5DB/47A5DB；47A618..47A620八个90 NOP。唯一入向47A5D4→47A5FC。登记mixed_table_data、mapped仅字节用途，R7 260→261/3061，未知2801→2800，不计新函数/范围排除。

<a id="battle-skill-window-click-sampling-decision"></a>
## 工程裁决

只修47C570三片段：两次配对索引读取按原MOV CL memory取低W8；绘制坐标按原元素Y→元素X→窗口Y→窗口X四次取低W16，再按S16参与正常加法/结果S16。索引W16、共享坐标/flags W32存储不改。四窄坐标用每次两字节memcpy承接，避免用窄指针别名读取；既有cstring可用，不增加工具函数或外部依赖。点击用S16索引以及两阶段W32 flags缓存保持，正常旧值一致，不称数值/并发或机器故障。

其余七完整正确体保留。创建失败/null、RNG顺序、原负滚动/最大值处理、固定末尾行gate、回调和完成槽均按原。RPLAN.24下不新增异常拒绝、补零、钳制、故障或回绕模拟；原滚动/最大值的正常负值处理不是本步新保护。475950/4758D0完成槽为已证非拥有T**，只拓宽地址，不造句柄表或影子状态，不把MarkDeath改成立即释放。

## 八函数完整正常合同

### Sub_47A4C0（0x0047A4C0）

47A4C0列表创建：先D672AC W32=3，再发布双S32/void callback CC4230，随后一次借window10 T*，非空W32 state&~8000，始终调用475AA0(10,1,0,null)，真实返回含null交调用者，原16条52B/一call/一jump/一ret。

### Sub_47A500（0x0047A500）

47A500技术创建：callback先发布，window10 T*先取样，然后mode W32=4/skillIndex W32存D2EA60，借窗非空才W32 state&~8000；始终475AA0(10,1,0,null)返回，不重取窗口或伪造成功。原18条62B/一call/一jump/一ret。

### Sub_47A5C0（0x0047A5C0）

47A5C0返回：一次借window10，空返回0；mode W32按原无符号>6默认0与七表项0..6分派，0/3/5/6只W32 state|8000并返回1，4调用479EA0(借窗,fresh D2EA60)弃返后0，1/2及其它0。原19条59B/一call/两普通jump/一物理switch/两ret；OR CH为W32缓存，不窄化写；37B相邻X内七W32表及九NOP有完整用途。

### Sub_479EA0（0x00479EA0）

479EA0列表重建：15配对索引按S16查输入skillIndex，首匹配或无匹配回row0；mode3、scroll=row-9后原负值和负skillIndex各清0。元素1..16先隐藏；1/3与10..16显示；27.data图3BA24。15对used W16非0且canRebirth W32为0或5才原序写S8配对行，剩余到15填-1。原479FA9只一次读取rows[end]的S8 gate，end固定15，不能改成逐行非负门禁或补保护；15行在cached gate非负时按各S8 row取used W16，count-10原负时0存maximum。元素17..26逐次fresh elements写47C570 callback，保T*及真正函数指针；原110条351B/无call/18jump/一ret。

### CreateWindowAnimation_475AA0（0x00475AA0）

475AA0窗口动画包装：原型T*先取width/height W32；仅10/11/12/14/27原序消耗RAND(-15,15)后读/写x，RAND(-15,15)后读/写y；x负、y负各RAND(10,20)，x+width>640和y+height>480各按原620-width..640-width及460-height..480-height采样写。六物理RAND，条件独立且保顺序；随后一次借现window，非空只W32关闭请求，之后fresh重读window槽取其x再y；空则状态x再y。原八形参475950(x,y,width,height,id,flags,mode,T**)真实返回，完成槽仅已证非拥有T**拓宽。113条355B/七call两callee/十一jump/一ret，107代码入向只完整登记，未全面验证全部父。

### Sub_475950（0x00475950）

475950动画动作创建：声56/x320后NewYobiAction(3,24H payload)，空时完成槽非空清null→OpenWindow(id)弃返→null。成功一次payload T*；callback4758D0、priority W8 DF、flags W32|6，value1C W32=0/flag20 W16=0后flags形参低W8 bit1才flag20=1。x/y/height/width按原低S16拓为S32，id低W16、counter W16=0、width/height值和非拥有T**槽保存。mode形参低W8：0中心为各尺寸/2向零+坐标、deltaX=0/deltaY=(centerY-lowY)/10、halfWidth=centerX-lowX；1两中心/两delta依原写序；其它不补写中心/delta。123条334B/三call三callee/五jump/三ret；音效原未消费第三Y不造shim，音频继续暂停。

### Sub_4758D0（0x004758D0）

4758D0动画更新：一次payload T*，顺序halfHeight/centerY/halfWidth/centerX W32快照生成矩形DrawGraphicHover。绘制后fresh action.deltaX→payload.halfWidth→halfHeight，先写新halfWidth，fresh deltaY，原counter低S16>=10比较先于halfHeight写；完成则OpenWindow(id低S16)弃返→MarkDeath(action)弃返→fresh action非拥有完成T**槽，非空清null，最终counter W16++。不把MarkDeath变成立即释放、不先取完成槽或用增后counter决定完成；49条126B/三call三callee/两jump/一ret，只有一个callback地址数据入向。

### Sub_47C570（0x0047C570）

47C570技能列表点击：scroll+index-17读S8配对行，行负或used W16=0返回0；点击state W32掩码2取一次。点击用索引S16及flags W32 cache，mask1<<原低5位：使能则fresh当前窗/配对/0调用47A000→声51→consumed1；禁用仅skillId72且518C60返回0发586548原29B消息(4,3)，总声55，不consumed1。点击分支后fresh flags W32取一次并缓存给hover及绘制；hover state低bit1仅寄存器TEST，不新增byte内存要求；原47C653实际索引低W8读取→visual2/hoverPair→mask使能才复制5864C8原25B含NUL。cached click非0覆visual1；之后fresh currentWindow T*与其elements T*，原47C6C2独立索引低W8与cached flags算enabled0/1。原坐标四低W16读取顺序元素Y、元素X、窗口Y、窗口X；源用每次memcpy两字节并按S16和正常加法取S16 x/y，共享坐标W32不改。47B6C0(x,y,S8 visual,pair,enabled,3,223,FFFFFFFF)，最后返回consumed。三片段仅两W8及四W16取样/顺序，正常旧值一致；125条400B/六call五callee/十jump/三ret，不造shift拒绝/新保护/故障等价。

<a id="battle-skill-window-click-sampling-acceptance"></a>
## 可验收范围

源码只一体三片段，两个W8、四个W16读取及顺序实修；整文件其它正文与YobiRuntime、既有共享类型/原字节保。八根全部原指令/调用/分支/返回、实际switch/X所有37字节、六邻窗和首NUL文本核对；原新每调用对应、各完整合同、台账其它对象与完整任务条件保持。两局部I/S各十七真实来源；执行文本汇总器及-Check，生成看板/快照不得手改。

不构建、调试、测试、运行客户端/样例或计算摘要。新增基线关联1641/3680（44.59%），BATTLE205成员枚举仍open、分类653、E778；R7 261/3061/余2800。U08/R01/R03/R04/V04只当前/引用更新，任务保持open，60任务源码closed10/完整验收0/叶0/global=false。完整生产/别名和owner寿命、其它子依赖/全部父、窗口10/26与57C368 token、SDK/生成机器ABI/画面运行及R8均未据本步验收，原格式明文/音频暂停保持。

<a id="battle-skill-window-click-sampling-source-state"></a>
## 当前交付

八完整原根573条1739B/十二页、22物理call/十三callee、50普通jump/一物理七项switch/十三ret。仅47C570三片段/一体，两个实际W8原点和四低W16坐标取样及其顺序恢复，正常旧值一致；配对索引W16、flags/坐标W32与其余七正确体及旧字节保持。真实创建失败/null、原RNG消费、模式及回调发布/借窗关闭、counter比较前后、MarkDeath后取T**完成槽和返回保持。六外父邻窗36条/115B只局部call/消费者，不代完整父；107创建包装代码入向只登记。37B X七W32表/九NOP用途mapped，不等新函数或范围排除。完整技能内容生产、其它子依赖/全部父、owner寿命、窗口10/26与57C368 token、SDK/生成机器ABI/画面运行及R8未验；原格式明文、音频暂停、RPLAN.24不专项异常/保护/模拟/故障等价保持。

提交快照：`cg2d-re-battle-skill-window-click-sampling-20261007`。唯一进度同步实际检查与提交结果。
