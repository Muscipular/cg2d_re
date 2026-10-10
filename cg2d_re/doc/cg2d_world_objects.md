# World 对象表实施 SPEC

继承[Yobi对象表证据](cg2d_yobi_runtime.md)的1000项、原0x13C步长、
字段与原位CryptInt合同。本节按用户工程委托裁决唯一owner与本步接口，
不把未消费字段或未实现生产/清理入口记成最终映射或运行验收。

## 存储与生命周期

唯一表归`cg::world::detail`，位于私有`WorldObjectState.hpp/.cpp`。
原[5AC4C0,5F9720)映射为1000条原生记录；5AC498查找游标、
5AC49C空槽游标、5AC4B4占用数、5F9738遍历上界各为独立S32。
沿既有PE零填充初值，不将IDA未定义字节的FF显示当作初值。
不建位置索引、影子查询表、额外句柄或缓存当前坐标。

当前直接消费字段为+00/+02两个W16、+04对象编号S32、
+0C/+1C两个独立LegacyCryptInt、+34 S32、+42 W16、
+54首字节、+11C非拥有`login::YobiAction*`及+120完整W16。
原+08仅保存W32图号承载，不扩大为地址；其余未消费区保留原字节容量。
不能据此宣称+38/+3C/+48/+4C/+50/+10C/+124等属性已穷尽。
原生action在+120、原+120标志在+128，总长0x148；原x86偏移
仅作来源，不再用于新端地址计算或0x13C整记录复制。
action仍由唯一Yobi链拥有，表引用不自行delete。

完整只读核43D760/43D770/43D7A0/43D7C0/43D810及向量迭代器
53C45E/53C4E0。构造按0..999，每项先+0C后+1C调用401C70，
共2000次RNG；不懒构造、不以静态零值替代，不在重置时重新构造。
析构按999..0，每项先+1C后+0C调用401CF0；不释放action。
既有LegacyMouse.cpp统一构造接线增加本表组，放在玩家坐标组之前：
原CRT 553014先于553064及后续世界/Ripple/鼠标组。
只承诺该已证相对顺序，不宣称尚未落位的其他CRT项已经齐全，
不声明原SEH异常展开或完整进程RNG序列已运行验收。

## 编号查找与空槽领取

### 444FA0

完整[444FA0,445026)。捕获查找游标、遍历上界；先扫描
[游标,上界)，再在游标>0时扫描[0,游标)，不重新采上界。
每项先检查+00 W16非零，再比较完整+04编号；命中发布同一查找游标，
返回槽下标。未命中返回-1，不清游标，不以对象编号作为数组下标。
不增加越界保护或把两轮扫描改成只扫描有效上界。

### 446130

完整[446130,4461D1)。占用数S32>=1000立即返回-1，不改其他状态。
否则重读占用数，捕获上界、空槽游标；先增加占用数，返回候选为旧游标。
若旧游标+1大于所采上界则发布新上界，随后从旧游标+1扫描至999，
找第一个+00为零的槽更新空槽游标。未找到且旧游标>0才回绕扫描
[0,旧游标)；仍无空槽则保留游标。返回始终为进入时旧游标，
不验证/标记这个返回槽，不补满表情况下的回滚或重新查找。
真正的槽初始化与发布由后续生产者完成。

## 三种位置查询

完整只读核445030/4450D0/445160；函数在YobiRuntime已有world::detail段
访问唯一表与opaque action，避免暴露对象布局或复制CryptInt。
三者入口先取上界，S32<=0分别返回-1/0/-1。
从索引0起，每项依序检查占用W16、借用action非空、该action当前位置
+1E0与X的Equal；X不匹配不读Y。X匹配后**重读记录action**，
用其+1F0与Y作Equal。两次均读原位CryptInt，保留readCount/RNG。
坐标通过后才检查记录+02类别掩码交集；445030再查+120 W16与第四参数。
所有掩码实际只消费低W16，新端接口保持U16，不解释高位为地址。

445030/445160命中时从同一槽重新读取对象编号S32返回；
4450D0命中返回完整S32 1，不能经返回对象编号的函数判是否非零，
因为编号0本身可能命中。每次未命中后先重读共享上界，再递增索引比较，
不把入口上界快照用到整轮，也不把类别检查提前以省掉坐标读取副作用。

## 446470 重置

完整[446470,4464C5)，旧证据四个caller均弃返回；当前三个已实现caller
亦不消费。原返回是表外的跨步槽地址，不是真实指针数组一过尾。
受托裁决改为void，删除world内旧异类YobiAction前置声明，
不伪造x86常量、非法表外T**或额外返回令牌。

先清占用数、上界、空槽游标，再对全部1000项按序清：
占用W16→action引用→+42 W16→+120 W16→对象编号W32→
将+34设为-1→+54首字节清零；表后才清查找游标。
不标死、不释放、不清类别/图号/其余文本，也不触碰两组CryptInt。
不能与4464D0或单项删除合并，不要求进入时所有action已经死亡。

本步补齐共享表、真实构造/析构、编号查找、空槽领取、三查询和重置。
四类生产、单项/批量清理、记录其余映射、51B3E0/469C90和完整世界场景
仍须沿原链完成。不构建、测试、运行、检查产物或使用subAgent。

## 实施状态

唯一1000项表、四独立S32槽、完整构造/逆序析构、六个查找/领取/重置入口
已落位。446470已接现有WorldUpdate三处调用，构造组接在现有玩家坐标
组之前；其他生产者尚缺，不声明当前已有有效世界对象或完整RNG序列。
接口、原指令、0x148布局算式、原位读取/重读/写序、源登记、限定差异、
UTF-8编码与文件链接仅静态检查；未编译、链接或运行。

## 单项删除与批量清理 SPEC

2026-09-28 按工程委托裁决。现有原会话只读核完整
4462E0@[4462E0,446465)、4464D0@[4464D0,446604)及调用关系。
实现仍归WorldObjectState.cpp，复用唯一表、玩家编号读取、
519D50及Yobi的ClearMapActorAttachments/MarkDeath，不暴露载荷布局，
不增加清理影子表、句柄或新依赖。所有action保持非拥有T*。

### 接口与返回裁决

4462E0的两处直接引用为441CAC与50BF49。后者在50BF53覆盖EAX；
前者沿441CBB返回至Yobi回调调用者，4417E0唯一地址引用在441DEA，
由404C94间接调用，404C96–404C99不消费返回。
因此新端采用`void Sub_4462E0(std::int32_t objectIndex)`，
不模拟原玩家分支低字节、查找失败-1和普通路径上界等寄存器残留。
4464D0保持既有void声明，三个caller在5094AB/50D162/51D2CD
均不消费返回；本步为已落位SceneState调用提供真实定义，
不以空回调或替代场景提前接入仍未实现的上游。

### 4462E0

先比较玩家对象编号完整W32位型；命中只调用519D50后返回。
否则调用444FA0，负下标返回。捕获该槽action，非空时调用既有附件
清理：允许空payload；依次标死并清原+4/+8/+C/+10/+2C/+4C/+30/+44，
最后free非空+1C文本并清引用。随后重新读取记录action，MarkDeath后
清主引用；不立即释放主action或payload。

4463F5先捕获占用数，再按原顺序清used、flags42、flags120，
将value34置-1；仅所采占用数>0才减一。空槽游标大于被删下标时
才收窄。读取上界，从上界-1倒查used；找到非零项才发布其下标+1，
退到零仍保留旧上界，不额外清查找游标。
对象编号、类别、文本首字节、图号和CryptInt均保留。

### 4464D0

先清占用数、上界、空槽游标，再按0..999遍历全部记录。
used为零直接跳过，即使action非空也不处理。已用槽的action非空
时使用同一附件清理合同，随后重读主action、标死、清引用。
已用槽依次清used、flags42、flags120、objectIndex，将value34置-1；
不清文本首字节，不重构CryptInt，不整条清零。
全部遍历完才清查找游标。与446470仅清引用的合同保持独立。

本步验收仅限原指令、全部分支、调用/读写顺序、接口、差异与编码的
静态核对；不构建、测试、运行或检查产物，不宣称完整生命周期验收。

两入口完整本体已落位；4464D0已满足SceneState现有真实调用，
4462E0的4417E0回调与50B380协议上游尚未实现。
World记录生产、其余字段映射、普通/战斗场景与整体SPEC仍未闭合。
前文“单项/批量清理仍缺”是前一步状态，以本节为准。

## 类别2、4生产与更新 SPEC

原会话只读核445D20@[445D20,445F3E)、445F40@[445F40,44612E)
完整指令、栈位移和唯一caller 50BB16/50BCA7。参数分别为：
`(objectIndex,imageNo,mapX,mapY,direction,value48,name,value50)`、
`(objectIndex,imageNo,mapX,mapY,direction,value4C,name)`。
除name为借用`const char*`外均为S32；图号写入记录时保留W32位型。
两caller均弃用返回：50BB1B直接覆盖EAX，50BCAF递增后回到50B3B5，
50B3C0覆盖EAX。裁决两入口为void，不模拟寄存器残留。

### 增量映射与归属

沿既有三种查询的方式，完整入口位于YobiRuntime.cpp的world::detail段，
直接访问唯一World表和本源私有action/payload；不增加字段访问器群，
不将Yobi布局公开到WorldObjectState.hpp，不合并两种不同写序的生产者。

- 记录原+30定为S32 direction；+48/+4C/+50分别为S32 value48/value4C/value50。
  50BABA/50BAFD经52CFA0解析十进制数，空项为-1，非空调用atoi，
  445DCA/445E86写入+48/+50；50BC16–50BC98对数值作有符号阈值比较，
  445FED写入+4C，均不是指针。
- 原+C9文本存61字节；不是截断缓冲，源长度>60时保留旧内容。
  原+114为U32 flags114，+118为S32 value118；445755写1，两入口写0。
  bytes54缩为[54,C9)，新增nameC9和[106,114)保留区，其余未知字段不改。
  原action仍映射+120、flags120仍+128，总长0x148不变。
- MapActorPayload原+18由value50写入，定为S32 value18，新端+2C；
  原+14仍保留4字节。后续字段及总长0x88不变，仍不解释未知字段。
- Yobi原+C4由445EB0允许28字节加NUL，4460C2对应入口允许16字节
  加NUL；扩为29字节承载，不把先前玩家入口<=16误作全局容量。
  nameC4与titleE1仍独立，原+C4并非绘制文本+64；现有玩家写入门槛不变。
  Yobi是原生重排对象，分配仍按sizeof，不承诺原0x330或固定新偏移。

### 领取、创建与共同尾

先444FA0按编号查找。未找到才446130领取；领取失败返回。
调用441DB0(imageNo,mapX,mapY,direction)后先发布返回T*，为空直接返回，
不回滚占用数、空槽游标或上界，不补used。成功写used=2，
重读action并写payload.index=槽下标，再写category=2/4、
objectIndex和value34=-1。已找到记录跳过上述初始化，
即使其action为空也不补建、不改category或value34。

随后写imageNo及各自value48/value4C，构造mapX临时量写coordinate0C，
再构造mapY写coordinate1C；两次独立RNG，不复用action坐标或搬整块CryptInt。
445D20的每组载荷是mask→encoded→readCount，
445F40是encoded→mask→readCount；每次随后析构临时量。
两者再写direction，长度<=60才复制name到记录+C9（含NUL）；
保留长度判定与实际复制前的独立strlen，不解码或转义原字节。

### 各自尾部与失败残留

445D20写flags114=0x400、value118=0、value50。
action为空则返回，记录更新仍保留。非空先写action.imageNo，
源name长度<=28才原样复制到重读action的nameC4，否则写554590的
实际四字节`???\0`。随后再次重读action，将其flags按位或0x400，
再重读写field1D8=value48，最后重读payload写value18=value50。

445F40写flags114=0x800、value118=0；action为空即返回。
非空先更新imageNo，name长度<=16才复制到重读action的nameC4，
超限同样写`???\0`；最后重读action并将flags按位或0x800。
不写value50、field1D8或payload.value18。

两入口不更新已有action当前位置、目标、朝向或世界像素；这些更新
仍由真实后继回调处理。不清其他旧字段、不添加空文本/payload保护，
不新增测试或模拟异常。验收仅作接口、指令、写序/RNG、布局算式、
限定差异与UTF-8静态核对，不编译、运行或检查产物。

两生产入口完整本体与增量类型映射已落位，使用真实441DB0创建，
不增加替代循环或空回调。五文件接口、分支、字段写序/数值宽度、
原生布局算式、sizeof分配和源登记仅作静态核对；
445350/445A60另外两类生产、50B380协议caller、
4417E0/442F70/43E490回调与完整场景仍缺，未声明生产消费链运行通过。

## 445A60无action记录生产 SPEC

原会话只读核445A60@[445A60,445D1E)、4461E0@[4461E0,446203)、
446210@[446210,446247)及唯一caller 50E036。
本步放入WorldObjectState.cpp：只操作唯一记录，不接触Yobi内部布局，
文本解析/反转义分别复用既有network和input边界。

### 参数、返回与转换叶

按实际栈位移，445A60参数依序为objectIndex、imageNo、mapX、mapY、
direction、name、titles、level、value40、flag1、flag2、type。
name/titles为借用const char*；value40只消费原栈槽低W16，
其余数值为S32。原50DF62的atoi产出level，写入记录原+2C；
不将其视为地址。返回完整S32 0/1，50E03E确实test EAX后选择后继。

4461E0返回完整W32：type为1或32返回0x100，3返回0x200，其余0x1000。
446210采用直接switch，已核44625C的71个索引字节及446248五个目标：
1→446229→8；6/13/27→44622F→0x11；32→446235→0x21；
71→44623B→0x101；其他→446241→1。新端同样返回完整W32，
仅记录category的写入截为低W16；不按反编译注释猜位含义，
不为原输入减一的异常回绕建立专项模拟。

### 原序与残留

444FA0找到编号立即返回0，保留查找游标的原副作用，不更新记录。
未找到才446130领取，负下标返回0。成功先将action置空、used置2，
调用446210并写category，然后写编号、图号、value40和level2C。
不分配、标死或释放旧action，也不置value34=-1或清其他旧字段。

两组CryptInt分别由mapX/mapY独立构造临时量。coordinate0C依次写
readCount→encoded→mask；coordinate1C写mask→encoded→readCount，
各自随后析构；不直接复用上一生产入口的统一写序。
随后写direction；flag1非零才将flags42低W8或0x40，
flag2非零才将同一低W8或0x80，零参数不清位，高W8不写。
以unsigned char别名执行原字节访问，保留整W16清理和高字节别名。

name源长度<=16才复制含NUL到原+54，否则留旧文本；不额外反转义。
再分别将titles按'|'取第1/2/3段至三个256B临时区，容量参数255：
第1段不调用反转义，长度<=16时复制到+65；第2、3段各自先调用
52D2F0，然后长度<=32才复制到+76/+97。每段按原顺序独立处理，
不合并解码、不提取第4段、不初始化原端未初始化的临时区，
不清超限文本残留。最后调用4461E0(type)写flags114，返回1。

### 增量布局与验收边界

原+2C改为S32 level2C；[54,C9)拆为name54[17]、title65[17]、
text76[33]、text97[33]及bytesB8[17]。所有后续偏移和0x148总长不变。
446470原+54首字节清零改为name54[0]='\0'，不改变清理集合。
沿既有UTF-8源码/原字节协议合同，不增加长度保护或外部依赖。
只作指令、接口、跳表、分支、字节宽度、RNG及文本处理顺序、
布局算式与差异静态检查，不构建、测试、运行或检查产物。

三完整本体已落位，文本边界复用现有真实实现；五文件接口、完整分支、
跳表/常量、调用与字段/RNG写序、0x148布局、源登记、差异和UTF-8
仅静态核对。唯一caller位于50DD80@[50DD80,50E0E7)，该协议父函数
尚未落位；445350生产、实体回调和普通/战斗场景仍缺。
当前完成不代表存在可运行协议生产链或有效场景验收。

## 445350实体记录生产更新 SPEC

原会话只读核445350@[445350,445A55)、5183B0@[5183B0,5183CB)
完整指令及唯一caller 50B894。13参数按栈位移依序为objectIndex、
imageNo、mapX、mapY、direction、name、titles、extraText、level、
value40、flag1、flag2、type。三个文本为借用const char*，数值均S32。
value40在记录只写低W16，但action.field1D8写完整W32，接口不得缩为U16。
50B899直接压入另一参数并调用World::FindObjectYobi，未消费此返回；
裁决445350为void，不搬寄存器残留或伪造成功返回。

### 创建、方向与共享状态

先444FA0查编号，未找到才446130领取；失败即返回。
仅新建分支按type>3、同源gValue9DAD68==0、5183B0(imageNo)==0
逐层短路，将本次direction置0。5183B0只作S32范围判断，
100000<=imageNo<200000返回完整1，否则0；不加载动画或读取资源表。
gValue9DAD68复用SceneState已有46DA80写入槽，不复制地图模式。
记录已存在直接使用原direction，不能把门槛移到公共尾。

随后真实441DB0创建，先将返回T*发布到槽，空值返回且不回滚领取。
成功写used=2、重读action写payload.index=槽下标、经446210写category，
再写objectIndex及value34=-1。旧槽不补建空action，不改category、
objectIndex或value34。

公共尾依次写imageNo、value40低W16、level2C；mapX临时CryptInt写入
coordinate0C时按readCount→encoded→mask，mapY临时量写coordinate1C
按mask→encoded→readCount，每组构造/析构独立；随后写本次direction。
flag1/flag2非零分别仅将flags42低W8或0x40/0x80，零不清位。
name长度<=16才复制含NUL到name54，超限留旧值。

### 两轮文本与标志

记录阶段，type!=3时按顺序取titles第1/2/4段，各自独立
52CED0(capacity=255)→52D2F0；长度<=16/32/16时分别复制到
title65/text76/text128。此轮**不处理第3段，也不写text97**。
extraText长度<=16才复制到textB8，原字节不另行反转义。
type==3仅取第1段、反转义、按<=16写title65，之后只清textB8首W8，
不清剩余16字节或其他称号。445717的C6字节写已核，不能清整个W32。

调用4461E0(type)，先将完整结果写flags114；只有该结果含0x1000，
且level同时含0x100与0x40时，才另写结果|0x400000。
接着写value118=1；action为空直接返回，记录更新仍保留。
action非空先写imageNo，重读写field1D0=level，再先重读action、
后读取同一record.flags114，按位或到action.flags。
name长度<=16才复制到重读action的nameC4，超限保留原值，不写???。

action阶段不能复用上轮文本。type!=3按1/2/3/4顺序重新提取和反转义，
长度<=16/32/32/16时分别写titleE1/text102/text123/text31C，
每次通过长度门槛后才重读action、取得目的地址、重新求复制长度。
extraText长度<=16才复制到重读action的text144。
type==3仅重新提取第1段、反转义、按<=16写titleE1，
不清action.text144或其他称号。公共尾最后重读action写完整value40到
field1D8。所有文本复制含NUL，保留原字节及超限残留，不增加保护。

### 增量映射与实施边界

完整入口和5183B0落在现有YobiRuntime.cpp的world::detail段，
不公开Yobi布局或建立重复状态。记录bytesB8改为textB8[17]；
bytes122拆为6B未知区、text128[17]和bytes139[3]。
原+128文本在新端+130，记录总长仍0x148，其余既有偏移不变。
Yobi新增原+144对应text144[17]，其容量只覆盖本入口已证<=16加NUL；
原生重排分配继续按sizeof，不据此冻结未知字段或原0x330映射。
本步不改其他生产入口、玩家发布或动画播放的同值范围判断。

四种生产入口均须保留各自合同，不收敛成统一初始化/文本函数。
只作指令、全部分支、同源状态、返回合同、布局、RNG/文本/字段写序、
差异和UTF-8静态核对；不编译、测试、运行或检查产物。

445350和5183B0完整本体、同源模式读取及增量文本映射已落位。
至此四生产入口445350/445A60/445D20/445F40均已有实现，
不代表它们的协议caller 50B380/50DD80或实体回调已完成。
五文件接口/分支/文本与RNG顺序/原生布局和sizeof分配/源登记、
差异、UTF-8及文件链接仅静态核对，整体SPEC和运行验收继续。

## 50DD80协议记录与专用动作父链 SPEC

原会话只读核50DD80@[50DD80,50E0E7)完整指令、两张四项跳表
50E0E8/50E0F8及唯一caller513C41。新端位于network::detail，
复用已有版本转换、解析、门禁、World生产和[Yobi专用创建](cg2d_yobi_runtime.md)。
九参数为未使用socket、关联编号、外层图号、mapX、mapY、direction、
withRecord、mode、payload。socket保留uintptr_t，payload为借用const char*，
其余S32。513C49直接xor EAX，不消费原分支混合返回；裁决void。

### 门禁与无记录路径

gValue9A6E0C非零直接返回；仅零时对唯一9A7FA8 CryptInt读取一次，
读数非零返回。之后才51B740转换外层图号，保留共享readCount/RNG。
withRecord==0时不读payload，根据mode调用448040：
mode0/1/2分别用专用模式2/8/4、value02=0；
mode3用专用模式0、value02为direction低S16，同时action方向仍为完整S32。
均使用入口mapX/mapY、转换后的外层图号、关联编号-1。
其他mode直接返回，但不能把mode检查提到门禁或图号转换之前。

### 带记录路径

withRecord任何非零值均进入。使用三个未预初始化2048B栈缓冲，
按'|'逐项执行下表；单独Extract的容量参数均为2047。
不将atoi的空串0替换成ParseDelimitedInteger的空串-1。

| 段 | 原处理 | 用途 |
|---|---|---|
| 1 | 52CFA0，空串-1 | 记录type |
| 2 | Extract→Base62Decode | 记录对象编号 |
| 3/4/5 | 各自Extract→atoi | 记录/动作mapX、mapY、direction |
| 6 | Extract→atoi→51B740 | 记录图号，与外层转换独立调用 |
| 7 | Extract→atoi | level |
| 8 | 52CFA0，空串-1 | value40，传生产入口时截低W16 |
| 9 | Extract→52D2F0 | name |
| 10 | Extract→52D2F0 | titles整串，后续生产入口仍执行自身分段合同 |
| 11/12 | 各自Extract→atoi | flag1/flag2 |

按上表值调用445A60；返回0立即结束，不创建动作、不回滚解析副作用。
成功后mode0/1/2/3分别使用专用模式3/9/5/1、value02=0调用448040。
动作图号仍为转换后的**外层图号**，坐标/方向使用解析后的第3/4/5段，
关联编号使用原第二参数，不能换成解析出的记录对象编号。
其他mode此时才返回，已经完成的记录生产必须保留。
动作创建失败也不回滚已生产记录，不添加空payload、类型范围或协议长度保护。

两跳表实际目标依序为50DDE9/50DDF1/50DE22/50DE53和
50E05E/50E06C/50E095/50E0BE；新端用switch保留对应默认分支。
全部直接下游已有真实定义；447E60及其回调子链仍缺，dispatcher512E40
尚未接入本函数。本步不因此声称完整协议或动作生命周期闭合。
只作参数/返回、门禁、跳表、解析/调用顺序、同源状态、字段宽度和差异
静态核对，不构建、测试、运行、实际收发或检查产物。

实施状态：完整50DD80两路本体已落位，445A60、51B740及448040已由此
真实父函数调用；不新增影子门禁、对象句柄或协议缓存。
四文件接口/分支/两跳表/低W16与完整S32/原解析和RNG顺序/源登记、
差异、UTF-8及文件链接仅静态核对。dispatcher512E40未实现，
447E60及十种回调、50B380和完整场景仍未闭合，不声明收包或动作运行通过。

## 完整对象批量回复50B380实施 SPEC

按工程委托，先核50B380@[50B380,50BCC3)全指令、唯一caller514BC6、
4462B0/51A4B0/51A5E0/449C90/449CC0/442C40/442BD0与相关返回弃用链。
复用[附件生产](cg2d_yobi_runtime.md)的43DD00/441130、四类对象生产、
玩家唯一owner与既有FF02E0地图五槽，不将F633F8宠物数据表混为同一存储。

### 归属与窄边界

50B380主体放既有SceneState，以便直接操作玩家/地图五槽；
YobiRuntime只承接opaque对象字段写入及原公共附件尾。
PetMapCharaSlot原+8改S32 level，+18改17B名称，其他间隔保持原字节，
非拥有action仍新+30，整槽仍0x38。World原+44只拆出U8标记，
后3B未解释，World记录仍0x148。不新增副本、句柄表或底层对象公开布局。
所有对象引用/回写槽用YobiAction*/YobiAction*&，数值不扩宽。

4462B0只调用444FA0查槽，负值返回空，否则返回同一World.action；
不增加used或空action检查，查找游标副作用仍由原叶处理。
51A4B0入口玩家空直接返回；姓名和ownTitle分别strlen<=16才复制，
每次成功复制后重读共享action，再写等级、重读后复制HP、
重读后复制最大HP（encoded→mask→count）。不解析称号、不复制FP或补其他字段。
通过内部原生指针借用同一玩家来源，不把level提前快照到另一存储。
51A5E0仅OR玩家flags18A的200；51A5A0仍复用已有完整实现。
51A4B0仅caller不消费返回，442BD0的50A3E0尾返回最终也被网络caller丢弃；
本步这些状态叶采用void，不编造旧EAX的现代地址。

四个World标志叶均仅入口action空时返回，不增加payload/索引/used门禁：
449C90：flags42不含200才OR100；449CC0：marker44为0才清flags42的100；
442C40：OR flags42的200；442BD0：先OR flags42的100，再marker44=1。
新端W16组合写保留原高W8操作的其他位，U8标记不扩大。

### 记录解析与玩家分支

保留未用socket；先读9A6E0C，零才读一次9A7FA8 CryptInt，非零均返回。
从序号1以TAB提取每行（容量2047）；只有strlen(row)==0且提取返回1才结束，
空行与其他返回值仍按原探测后递增，不加数量或字段合法性限制。
每行先探测第11字段是否非空，非空为实体；否则再探测第6字段，
仍空才探测第4字段。各探测和随后重新提取不得合并。

实体按1..16顺序解析：1用52CFA0；2提取后base62；
3/4/5/6/7各提取后atoi（X/Y/方向/图号/等级）；8用52CFA0；
9容量255提取后52D2F0；10容量255提取后52D150；
11/12/13各提取atoi，**13结果弃用但调用保留**；
14容量255提取52D2F0；15/16提取atoi为模式/附件图号。
两种反转义不能统一，atoi空串0不能改成52CFA0空串-1。

原对象编号按W32位型等于玩家编号时：
先51B740转换图号；当前方向负值才发布请求方向。
玩家action空走519CD0(转换图,X,Y,请求方向)→51A4B0，
非空走519E50(转换图,当前方向)，不在创建失败后补保护。
保存入口所获action作为本行公共尾对象，后续共享action按原重读。
模式1/2比较旧2E8与**请求图号**，不同才置2EC=1，再写2E4模式、2E8图号；
其他模式也先作同一请求图号比较，再写2E4/2E8=0，不无条件置或清刷新位。
模式2仅flags18A不含200时OR100；模式1或其他模式只有flags18C低U8为0才清100。
随后9A4B94清bit1→468200→51A090，保持坐标读取/RNG与文本写入先后。
若此时flags18A含100且地图第0槽占用，先更新其level，再无界复制玩家名称。
按槽0..4找首个占用且编号相同者，写当次共享玩家action→51A5E0；
仅槽0再51A5A0，然后停止扫描。

### 非玩家与公共附件尾

非玩家先51B740→445350全部13参数→4462B0，得到本行固定action。
非空才更新模式/图号/刷新位，与玩家相同；模式2调449C90，
模式1及其他模式调449CC0。随后找地图五槽首匹配并发布固定action，
调用442C40；仅槽0追加442BD0，不写玩家标志。
**查找返回空仍进入公共尾**，保留原无空保护，不额外continue。

公共尾先从本行action捕获payload，再提取字段17并strlen；
非空调用43DD00(action,row,17)，忽略返回，空时仅标死/清payload.action2C，
不清其余挂件、骑乘或kind。此段为Yobi内部窄边界以保留私有载荷，
不能因helper可接受指针就增加入口空判断。

### 另外两类记录

第11空、第6非空：1提取base62，2/3/4提取atoi为X/Y/图号，
5用52CFA0，6容量1023提取52D2F0，7用52CFA0；
调用445D20(编号,图号,X,Y,0,第五值,名称,第七值)，不作51B740。
第11/6空、第4非空：1 base62、2/3/4 atoi为X/Y/数量；
先按58AFE4的原字节`25 64 20 AA F7 B9 F4 00`格式化到1024B标签，
再按S32数量门槛50/100/1000/5000/10000/50000/100000/500000/1000000
选择连续图号6B08..6B11，调用445F40(编号,图号,X,Y,0,数量,标签)。
三探测均空则仅递增行号，不新增删除对象业务。

全部直接下游已有或本步完整落位，无空实现；网络512E40仍未分发到本体，
不能宣称收到对象数据或完成场景。异常按RPLAN.24不专项模拟，也不补保护。
仅静态核对接口/原分支/字段宽度/同源状态/读写与RNG顺序/源登记及差异，
不构建、测试、运行或检查产物。

完整50B380和上述七个状态/查找叶已落位，公共附件尾已接43DD00；
玩家、非玩家和两类地面对象的直接下游均有真实定义。
实体名称/称号/附加文本仍各256B，两地面分支共用1024B标签，
不把容量不同的原栈区合并；两个图号转换均调用既有角色模块入口。
八文件接口/原分支/字段宽度/同源T*重读/解析与RNG顺序/源登记、
差异/UTF-8和文件链接仅静态核对。512E40网络分发仍缺，
未声明真实收包、对象画面或整体场景通过。

## 4467D0动作结束后恢复实体 SPEC

原会话只读核[4467D0,4468EC)全指令及十个直接caller，
全部随后将原动作传给MarkDeath，不消费4467D0返回；
裁决采用void，不合并不同分支的EAX残留。
完整入口位于YobiRuntime.cpp的world::detail段，直接复用唯一World表、
MapActorPayload与真实441DB0，不暴露对象布局或新增记录。

1. S32对象编号<0立即返回，不调用查找。否则444FA0查槽，负下标返回。
   对同一槽再检查used非零、action为空，任一不满足返回。
2. 先读取record.direction，再对原+1C的coordinate1C作Read，
   后对原+0C的coordinate0C作Read，最后读取imageNo，
   用所得Y/X按441DB0(imageNo,X,Y,direction)调用。IDA的gameX/gameY
   自动名称不覆盖实际地址5AC4DC/5AC4CC的对应关系。
3. 将返回T*先写回record.action，空值返回；不释放或回滚记录。
   成功才写used=2，再重读action、写payload.index=槽下标；
   重读action后读取level2C并写field1D0。
4. 重读action取得nameC4目的地址，再求record.name54长度并复制含NUL；
   独立重读action取得titleE1目的地址，复制record.title65含NUL。
   此入口没有长度门槛，不添加截断、补零或重复反转义。
5. 最后先重读action、后读取record.flags114，按位或到action.flags。
   不写field1D8、其他称号/文本、category、value34、flags42/120或value118，
   不重新领取槽、不改变占用数/上界；查找游标仅由444FA0按原规则更新。

十种动作回调继续各自决定何时恢复及标死，即使本函数创建失败也不改变
它们的后继顺序。本步只核接口、分支、同源引用、CryptInt/RNG及字段写序、
差异/编码，不构建、测试、运行或检查产物。

完整本体已落位。四文件接口、十条返回弃用链、全部门禁、坐标地址、
T*重读、文本集合、字段写序及既有源登记、差异/UTF-8仅静态核对；
十种动作回调仍待接入，未声明实际恢复、回收或画面验收。

## 地图五槽回复与移除续接 SPEC

沿已接50B380的同一FF02E0五槽，原会话只读核
50A3E0@[50A3E0,50A6A4)、51B080@[51B080,51B0EE)、
46AFE0@[46AFE0,46B1A3)、442C70及51A5F0完整指令与直接caller。
工程裁决继续由SceneState拥有五槽，三个原+0C/+10/+14数值
从既有12B区拆为S32，不猜HP语义、不改变0x38布局或数值宽度。
YobiRuntime只提供目标坐标读取、清路径计数、到格门禁、
相邻距离判断四个内部窄边界，不公开私有载荷、不复制五槽。

### 回复50A3E0

保留未用socket、完整S32槽下标及文本参数；9A6E0C非零返回。
首段52D0D0解析完整W32掩码；不读RippleGate、不新增槽下标门禁。
掩码为零按以下顺序移除：

1. 当前槽占用且编号不同于玩家，先4462B0查询，非空调用442C70。
2. 无论占用与否，若槽action非空，先读原+200的CryptInt，
   再重读同一槽action读+210；空时X/Y分别为-1。
3. 仅清当前槽flags/action，编号和数值文本残留不动。
4. 按0..4计占用槽，并记录首个占用且下标大于被移除下标者，
   不因action为空跳过。剩余<=1先清FF0D36，再51B080并结束。
5. 否则后继下标非负且X/Y至少一个非负时才46AFE0；
   不能改为两坐标都非负，也不要求后继action非空。

完整掩码非零：先清FF0D32，再置FF0D36=1、槽flags=1。
从字段2按低U8掩码0x03/0x05/0x09/0x11/0x21/0x41逐项消费：
编号、等级、+10、+0C、+14各用52CFA0，编号只作W32位型转换；
名称以255容量提取后52D2F0，长度<=16原串，否则写原???。
不把高位非零而低位全零的掩码改为移除或全量更新。
最后按当次槽编号分支：非玩家4462B0查找，非空先发布T*再442C40，
仅第0槽再442BD0，空则清槽action；玩家发布共享action（可空），
总是51A5E0，仅第0槽再51A5A0。未更新字段继续保留旧值。
唯一网络caller514C55随即清EAX，采用void，不编造原分支的残留返回。

### 解散清理51B080与标记叶

遍历全部五槽：占用玩家槽若action非空只清其+270的W16，
随后无条件51A5F0清玩家flags18A的200；
占用非玩家槽仅玩家flags18A含100且action非空时清+270，
随后无论action空否调用442C70。每槽末尾总是依序清flags、编号、action，
不清等级、其他数值、名称或两组路径内容。
全部完成后尾调已有51A5B0，保留其非拥有YobiAction*返回。
442C70先440B50场景门禁，命中即返回，之后才检查action空；
非空通过payload.index清同一World.flags42的200，不添加其他门禁。
两条442C70 caller均弃返回，采用void；51A5F0也仅保留状态写。

### 移除后的路径续接46AFE0

先固定第0槽action；空或其+270按S16大于0则返回。
依次读+200/+210的CryptInt，分别以F64乘原F32常量64并与worldX/Y比较，
保持x87 C3门禁的既有表达：小于或大于才返回，不额外加入NaN分类。
首轴未到不读次轴，不重读第0槽指针。

从输入下标-1倒找首个占用且action非空的前驱。
找到后比较前驱与输入槽目标：X轴先求差判符号，再重读两个共享T*求差取幅值；
X幅值>=2就不读Y；否则Y同样两次求差，幅值<2直接返回。
两轴均保持401E70的右操作数先读、左操作数后读及重复RNG消费；
不将输入槽action提前快照或加入空保护，不用一次abs替代重复求差。

需要续接时先将9DAD78写为F32 1，再从输入下标递增到4：
仅占用且action非空才处理。先固定本槽action，再读其+200，
重读共享槽action后读+210；以固定指针调用441F60(传入X,Y)，
然后把刚读出的X/Y作为下一有效槽的目标。空洞不更新目标。
输入下标>=5仍先发布倍率而不遍历。唯一caller的返回最终由网络丢弃，采用void。

仅静态核对原分支、同源T*、字段宽度、CryptInt/RNG及写入顺序，
不构建、测试、运行或检查产物。网络512E40、其他动作回复和普通场景
仍需接入；不将五槽生产完整等同于完整协议或跟随画面验收。

完整50A3E0、51B080、46AFE0与两个标记叶已落位，四个对象窄边界
只借用同一T*槽；五槽三个新增数值字段及原生action偏移/总长未变。
六文件接口、分支/返回弃用链、掩码/偏移、CryptInt/RNG读取次数与次序、
原64.0F常量及既有源登记、限定差异/UTF-8/文件链接仅静态核对。
网络分发、50BCD0动作回复、50D000另一解散caller及普通场景仍未闭合，
不声明实际协议收发或跟随移动通过。

## 跟随状态回复50D000 SPEC

原会话只读核[50D000,50D07F)全部指令及唯一caller5151B2，
后者立即清EAX，采用void；主体放同一SceneState，不搬移玩家或五槽状态。
保留未使用socket、两个完整S32参数；9A6E0C非零立即返回且不清FF0D32。
第一个参数为1且第二个参数为1时先51A5E0，再清FF0D32并结束。
第一个参数为0且第二个参数为1时依次51A5F0→FF0D36=0→51B080，
再采玩家方向低S8并符号扩展、读C751B0快照Y、读C751C0快照X，
最后读取共享连接，调用已有512720(socket,X,Y,direction)，忽略返回，
随后清FF0D32。其他参数组合只清FF0D32。
不读RippleGate，不把传入socket用于发送，不新增错误提示或重试。
仅静态核对接口、分支、共享状态、参数宽度及CryptInt/RNG读序；
网络分发未接，不声明实际发出cZt或跟随状态运行通过。

## 完整动作回复50BCD0 SPEC

原会话只读核50BCD0@[50BCD0,50BF14)全指令、局部栈区与唯一caller514D4D；
caller立刻清EAX，采用void并保留未使用socket。放同一SceneState，
沿现有51A840/444410及完整440920，不新建玩家/World状态或动作dispatcher。

9A6E0C非零立即返回；否则只读一次9A7FA8 CryptInt，非零返回。
从第1段起按逗号取行，缓冲2048B、传capacity2047；
只按strlen(row)==0结束，不检查52CED0返回，不清缓冲或跳过空行。
每行先以'|'取第1段到另一2048B缓冲并base62Decode，再按2/3/4/5顺序
用52CFA0取X/Y/动作/方向。调用440920(object,X,Y,direction,action,row)；
非零便直接取下一行，不再解析6..9，也不查普通动作对象。

未拦截才依序解析6..9四个选项，之后再读玩家对象编号作比较。
玩家分支固定本次action T*，空或该action的imageNo==0则跳本行；
其他情况调用51A840(X,Y,direction,action,四选项)，该函数仍按自身时点
重读共享玩家，不把外层检查指针偷偷传入替代共享对象。
非玩家分支4462B0查找；非空调用444410(固定T*,X,Y,direction,action,四选项)。

查找为空时按原序对本地缺失编号表去重：扫描已写项，有重复不写；
无重复且U32 count<0x1000才先count++、写原游标、游标前移一个S32。
原局部区为0x1000字节（1024个S32），原门槛却为4096项；
依RPLAN.24不专项模拟邻栈、不扩大容量、不新增1024门禁或异常恢复，
保留源容量与原比较，不声称异常访问等价。局部编号不是地址，保持S32；
游标使用原生S32*。尾部没有消费该表或发补取请求，不擅自增加网络发送。
无论普通处理、忽略玩家、重复缺失或成功拦截，均按行号+1继续取行。

仅静态核参数/门禁、解析和CryptInt调用序、对象身份/缺失表及返回弃用链；
512E40分发与窗口open仍未完成，不宣称实际收包、动作或观察画面验收。

实施状态：50BCD0完整本体已落位，按原次序接440920、51A840与444410，
新增缺失编号数组/游标仅为本次调用局部，不发布共享表或补发请求。
本次与观察拦截共十文件接口/门禁/容量与比较差异/解析顺序/共享引用/
源登记、限定差异/严格UTF-8无BOM/LF和文档文件链接仅静态核对。
不构建、测试、运行或检查产物；512E40网络分发、窗口open、完整场景
及实际收包/画面仍缺，不能用三个分派目标已有本体代替整链验收。

## 对象属性回复50AEF0与移除回复50BF20 SPEC

2026-09-28，原会话只读核50AEF0@[50AEF0,50B0CD)、
50BF20@[50BF20,50BF98)完整指令及唯一caller514B4A/514DC9。
caller均弃返回，采用void，保留未用socket。按用户工程委托复用同一
World查找/删除、Yobi原生对象和地图五槽，不新增副本或地址令牌。

### 50AEF0：四个原位CryptInt

没有9A6E0C或9A7FA8门禁。栈文本区2048B，不预清零；
以field=3起，至field<28，每轮加5，共最多五组。
先52CED0按竖线取field-2，capacity2047，忽略返回；
只按文本长度为0提前退出。非空编号用52D050 base62解析，
之后依序提field-1、field、field+1、field+2到同一2048B区，
每次均用atoi取S32；不用52CFA0的127B中间区或空串-1规则替代。
全部解析后才4462B0查同一对象；空则直接推进下一组，不消费RNG。

非空对象固定本次非拥有YobiAction*，依原序构造四个临时CryptInt，
每个均401CC0(value)后复制已证载荷、立即401CF0销毁再构造下一个。
目标槽不重构，不覆盖token/reserved，不以通用CopyPayload统一写序：

| 原目标 | 现有字段 | 目标写序 |
|---|---|---|
| +170 | readiness，地图HP | readCount→encoded→mask |
| +180 | maxHp | mask→encoded→readCount |
| +1B0 | barValue1B0，地图FP | encoded→mask→readCount |
| +1C0 | barMaximum1C0 | readCount→encoded→mask |

完整本体归world::detail，实现在已有YobiRuntime.cpp的world部分，
直接访问已映射私有字段；不公开对象布局、不为单处赋值新增四个访问器，
不写玩家/宠物主记录或改变对象生命周期。

### 50BF20：删除后只清首个匹配引用

先9A6E0C非0退出，否则按逗号以52D0D0从第1项解析编号；
严格等于-1结束。每项先4462E0删除，再从地图五槽0..4顺序查找：
flags W16非0且objectIndex完整W32位型相等，才清该槽action并停止扫描。
保持flags、编号及其他字段，不直接释放action，不重新连接跟随队列，
不调用51B080或其他解散同步；没有匹配也照常解析下一编号。
选择仍使用原生T*字段，不搬原30H槽步长到x64。
本体放已有SceneState，能访问同一私有五槽，不新增跨模块状态访问器。

两个回复直接接既有512E40路由；其他生产/场景消费者和完整协议未据此
完成。仅静态核对，不构建、测试、运行或检查产物。

实施状态：50AEF0/50BF20完整本体已接TNT/uSB，Yobi与五槽布局未改，
没有新共享状态或对象访问器。九文件接口、全部分支/参数容量/解析顺序/
四次CryptInt构造与各自发布序、删除后首匹配/既有源登记、限定差异、
严格UTF-8无BOM/LF及文档文件链接仅静态核对；剩余回复33项。
未构建、测试、运行、检查产物，不计完整对象生命周期或收发验收。

<a id="recreate-world-actors"></a>

## 普通场景全表补建446640

2026-10-01核446640..4467C4完整124条原指令；仅51C82A/51CF8E两个caller，
均属于普通场景51C7D0。工程方案沿YobiRuntime.cpp已有world::detail实现，
复用唯一1000项World表、441DB0工厂和MapActorPayload，不公开Yobi私有布局，
不建立新对象表或缓存。调用方不消费返回，采用void完整副作用接口。

原指针从5AC4DC开始，每轮加13CH至5F973C，共1000条；
不是按gWorldObjectUpperBound或usedCount截断。每条先used W16非零，
再action空才创建；未用或已有action跳下一条，不刷新名称或标志。
创建参数依序采direction、CryptInt读coordinate1C(Y)、再读coordinate0C(X)，
然后读W32 imageNo并调用441DB0。工厂返回先发布record.action，
空则保留used及其他记录，继续下一条，不补回滚或重试。

成功先used=2，再重取action和payload写当前槽下标，之后重读category W16：

| category | 名称与附加发布 |
|---|---|
| 恰1或8 | 重取action，取level2C写action.field1D0，再重取action，将name54含NUL复制到nameC4 |
| 恰2 | nameC9字节长<=28时再测一次长度并复制含NUL，否则复制原554590的`???`含NUL |
| 恰4 | 同上，但长度门槛16 |
| 其他 | 直接下一槽，不复制名称、不写field1D0，也不合并flags114 |

上述1/8/2/4路径复制后均再重取record.action，随后读record.flags114，
将完整W32或入action.flags。类别是精确值比较，不能将类别3等复合位当作1/2；
只有1/8写等级。该入口不复制title65，与单项补建4467D0不同，不能复用其本体。
字符按原字节容量处理，不新增截断/Unicode转换；`???`来源4B为3F3F3F00。
已证短名称门槛保留；异常索引、文本与分配沿RPLAN.24，不加防御性分支。

完整场景51C7D0含736条代码指令及多状态路径，本步只补该真实依赖，
不注册部分场景回调。音频仍暂缓，渲染快照/转场与其他状态依赖继续单列。

实施状态：446640完整本体已落位；124条原指令、1000槽终点算式、
门禁与工厂前后字段读序、精确类别、两次长度读取及4B回退串已静态核对。
新增实现直接借用既有记录/action/payload，布局、工厂、单项4467D0和其他
生产/释放函数未改。类别1/8不补title复制，其他类别不额外合并flags，
没有将单项与全表行为统一化。仅静态核对，不构建、测试或运行地图对象链。

<a id="map-object-production-current-20261010"></a>
## 2026-10-10 MOP01：445350完整正常对象生产

[原根/差集](cg2d_map_object_production_sampling_evidence.json)与[当前合同](cg2d_map_object_production_sampling_spec.md#map-object-production-source-state)补已有445350：543条1797B/28E8、十callee/唯一十三参入向有据。这里只恢复两坐标取样和四256B文本槽复用，不改变1000项表、WorldObjectRecord/LegacyCryptInt布局、action借用、类别/flags提供者或真实SceneState接线。

第一坐标先BYTE count/encoded/mask齐取，第二先mask/encoded/BYTE count齐取，之后各自同序发布/原同一临时析构；不复制token/reserved。原DB gameY/gameX名称不作换轴依据。记录与action九解析点按四原槽身份恢复，原段号/容量255/16及32字节门槛、type3只记录textB8首字节清零、action尾完整value40保持，仍各自重新解析/借目标。领取/创建失败及旧记录不补action规则保持；唯一原父只调用窗，不代整个接收/生产/寿命。MAP138/Yobi67/TEXT29均枚举false、七任务open/完整门禁保持，全部生产/传递callee/alias/owner/容量/SDK/ABI及画面声音/运行待。

<a id="world-object-lookup-current-20261010"></a>
## 2026-10-10 WOL01：查槽、领取与完整类型转换

[五完整合同](cg2d_world_object_lookup_allocation_spec.md#world-object-lookup-source-state)与[原根/数据/差集](cg2d_world_object_lookup_allocation_evidence.json)纳管MOP01依赖：五144条421B/五页、一E8/28跳含类别switch/十五RET，31实际入向/十九父、105B五target/71selector与1000项地址界有据。五正确body/表/字段/存储保持，仅CPP/真实声明各五关联注释。

查找固定start/end两轮，used WORD非零才编号W32比较、命中发布查找游标；领取先计数/高水位/前扫和条件回绕，返回旧游标，不写被领槽、used/category/action/文本或分配/RNG，失败不改状态也不加回滚。类型提供者返回完整W32，category由各父写入时才W16截取；FindObjectYobi查槽一次并借原生T*、负返空，保游标副作用，不移owner。

四新语义、1920从数据角色关联补55B完整正常范围/来源及新F处置，旧CAC07子记录/入口/原MAP成员保持；五新F、MAP四/Yobi五成员和两限定S五根，零I/新函数/接口/逻辑。MOP01正常生产与无tag历史附件规则保，六任务open/全部完整gates/枚举/closure及政策保。全部原输入/生产/31完整父/传递callee/容量/alias/owner/SDK/机器ABI/画面声音和运行待，总SPEC§10不降。

<a id="world-object-lifecycle-current-20261010"></a>
## 2026-10-10 WLR01：删除、整表清理与空action恢复

[四完整合同](cg2d_world_object_lifecycle_spec.md#world-object-lifecycle-source-state)与[原指令/绑定/差集](cg2d_world_object_lifecycle_evidence.json)纳管四正确正常根：328条1066B/八页、26E8六callee/37跳六RET，无switch，19实际入向/十七父与19短窗有据；只五CPP/五头关联注释、零I/新函数/逻辑，主体/类型/存储/全其它源保持。

删除先玩家专用分流，世界action八附件/text清理后fresh主标死清槽，计数/游标及逆扫上界原序、全空保旧上界；无释放重置全1000槽清原七字段含name首BYTE、无标死/free；占用槽清理只used非零、同一八槽/free/fresh主后清原字段、不清name或未占用槽。空action恢复先direction→1C Read→0C Read→image创建，返回先pub空/失败不回滚，成功index/level/原名称title字节与fresh flags保持。401EA0按旧219唯一jmp同一Read，不加读取/RNG；共享八槽不含34/38、逐槽取样标死清槽，现std::free沿MSVC域，不当主对象立即free/完整owner闭环。

四新语义/F及MAP/Yobi各四成员、两限定S四根，六旧callee/第三玩家1300及MOP01/WOL01保；七任务open、全部完整gates/成员枚举与六closure保。所有数据/19完整父/传递callee/容量/alias/owner/SDK/机器ABI/正常失败与声音画面运行待，总SPEC§10不降。

<a id="world-object-record-producers-current-20261010"></a>
## 2026-10-10 WOP01：记录生产与更新

[三完整根](cg2d_world_object_record_producers_spec.md#world-object-record-producers-source-state)/[原指令与差集](cg2d_world_object_record_producers_evidence.json)：445A60无action生产与445D20/445F40类别2/4更新合516条1738B/十页，27E8九callee、22跳五RET，三入向/两父短窗有据。六原坐标临时三值先取齐后发布，分别count/encoded/mask、mask/encoded/count与encoded/mask/count，保构造/销毁/RNG及字段宽度。445A60三个独立未初始化256B槽明确为100/200/300，ordinal1/2/3借100/300/200，原解析/通知/容量/门槛保持。

保重复/领取/创建失败及pub、已有记录不补action、名字60/28/16和原四字节"???"、低BYTE flags42与完整flags114、类别2独立附加字段。共享CryptInt工具、其它根、两个当前父和六旧身份保持；三新语义/F与限定S，MAP/Yobi各三成员、TEXT一成员，七任务/完整gates/成员枚举和六closure仍开放。完整输入/两整父/传递callee/正常失败/容量/alias/owner/SDK/ABI/画面声音运行待，总SPEC§10不降。

<a id="world-object-bulk-restore-current-20261010"></a>
## 2026-10-10 WBR01：整表清引用与批量补建

[两个完整根](cg2d_world_object_bulk_restore_spec.md#world-object-bulk-restore-source-state)/[原指令与差集](cg2d_world_object_bulk_restore_evidence.json)合134条431B/三页、三E8两callee/15跳两RET，五实际入向/三父短窗与五当前接线有据。原游标相差13Ch×1000：清引用只used W16非零的action/value34，不标死/free或改其它字段/计数；批量补建先direction/1C Read/0C Read/image与创建pub，成功used/index之后才四类别名字/flags尾，未知类仍保已创建action，不能提前筛选或回滚。

两个正确body及三个清引用点先404D30、两个普通场景补建点先519A80/455C60的接线保持，仅两CPP/两声明注释。两新语义/F/MAP-Yobi各两成员、两限定S零I，六旧身份/共享工具/其它根全保；七任务/完整gates/成员枚举与六closure开放，所有输入/三完整父/传递callee/容量/正常失败/alias/owner/SDK/ABI与画面声音运行待。

<a id="world-object-grid-query-current-20261010"></a>
## 2026-10-10 WGQ01：动作交互格与标志查询

[五完整查询根](cg2d_world_object_grid_query_spec.md#world-object-grid-query-source-state)/[原指令与差集](cg2d_world_object_grid_query_evidence.json)合280条765B/五页、十E8唯一401D00/42跳十RET，十入向/八父短窗与返回补窗有据。先used W16/action、X比较、fresh action/Y比较，再W16类别与各掩码；不能以record坐标、预取动作或提前筛选类别/标志替代，实际Read/RNG保持。所有未命中路径尾fresh S32上界，不固定count/cap或动查找游标。

445030必需flags120，mask0失败，返编号或-1；4450D0只类别返0/1；445160只类别返编号或-1；4451F0包含mask0分别跳字段；4452A0排除mask0仍读字段且42失败不取120，两者返完整0/1。五body/十现接线全保，只五CPP/五头注释；比较叶四条15B/一次Read/本组十AL消费成立，原48入向其余38/高EAX ABI及独立身份不外推，旧219/八父八身份保持。五新语义/F/两限定S零I，MAP/Yobi各五成员，六任务/完整gates/枚举及六closure开放，全交互输入/生产/八完整父/正常失败/容量/alias/owner/SDK/ABI/画面声音运行待。

<a id="neighbor-fim-current-20261010"></a>
## 2026-10-10 NFI01：邻格请求完整生产与发送

[两完整根](cg2d_neighbor_fim_request_plaintext_spec.md#neighbor-fim-source-state)/[原指令/提供者/server判据](cg2d_neighbor_fim_request_plaintext_evidence.json)纳管51B3E0/510B80，合227条676B/五页、25E8十三callee/两SDK tick/20跳三RET，唯一外父469C90弃返窗有据。父正确body保持：cached原位型、两轴各两次减法/fresh target、类别7查询、两F32差/方向、unsigned500门禁、先Y后X发/两tick与发送失败仍更新时间；不合并abs/读取或回滚。

子按CAB05现fIM/Base62字段/普通空格/LF只删原17写入，原time/seed/rand、扫描/fresh buffer和发送结果保持；server分词对17不终止、整数遇17返回0为静态判据，不试包/改server或恢复外壳。补两独立身份（WGQ01当时缺51B3E0的历史结论保），MAP/Yobi仅父、NETWORK仅子三S零I，十二旧身份/所有其它源保持；七任务/完整gates/枚举/六closure开放，完整交互与网络/owner/ABI/画面声音运行待。
