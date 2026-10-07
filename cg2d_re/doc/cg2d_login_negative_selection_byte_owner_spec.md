# 登录负分类20字节读取与owner SPEC（BRU01）

2026-10-07，沿工程委托解决 ReadNegativeSelectionAvailability 只有声明的真实缺口。只补已有登录尾部调用所需数据读取，不重写登录状态机或鼠标追踪器。

<a id="login-negative-byte-baseline"></a>
## 原读取与存储

45FEA0完整旧基线继承[登录证据](cg2d_login_scene.md)。本步只取两负分类可达尾读460B1D/460BD7的实际指令：每项计算193×(category×20+page×10+slot)+D69798+C0H，先读取BYTE；第一尾才判断已读分类是否非负，另一尾按BYTE决定绘制。4600A5是已有非负选择输入路径，单独记录、不外推负分类消费。正常负分类为-1，page0/1与slot0..9，对应下面20点。当前源码调用和上述先读后判顺序保持。

| page×10+slot | 原BYTE地址 | 唯一真实存储对应 |
|---:|---|---|
| 0 | D68944 | 底部栏[0][3] byte0 |
| 1 | D68A05 | 底部栏[5][1] byte1 |
| 2 | D68AC6 | 未装载/无正常写入的独立零字节 |
| 3 | D68B87 | HoverText+159 |
| 4 | D68C48 | HoverTitle+84 |
| 5 | D68D09 | mouseY.legacyVftToken byte1 |
| 6 | D68DCA | gValueD68DC8 byte2 |
| 7 | D68E8B | fixedPool+187 / node7.next byte3 |
| 8 | D68F4C | fixedPool+380 / node15.prev byte0 |
| 9 | D6900D | fixedPool+573 / node23.prev byte1 |
| 10 | D690CE | fixedPool+766 / node31.prev byte2 |
| 11 | D6918F | fixedPool+959 / node39.prev byte3 |
| 12 | D69250 | fixedPool+1152 / node48.x byte0 |
| 13 | D69311 | fixedPool+1345 / node56.x byte1 |
| 14 | D693D2 | fixedPool+1538 / node64.x byte2 |
| 15 | D69493 | fixedPool+1731 / node72.x byte3 |
| 16 | D69554 | fixedPool+1924 / node80.y byte0 |
| 17 | D69615 | fixedPool+2117 / node88.y byte1 |
| 18 | D696D6 | fixedPool+2310 / node96.y byte2 |
| 19 | D69797 | gValueD69794 byte3 |

底部栏现存每组9个S32，不套原40B步长跨组；hover文本/标题分别有256/260B既有owner。鼠标Y token是既有原W32位型，会被原构造/析构及Reset真实写入，不能固定D3或调用CryptInt::Read额外消费RNG。12个池点始终属于固定池，不能跟随当前nativePool切到动态分配。

D68AC6：未装载/文件偏移-1；389569条代码操作数目标区D68AC0..C8无命中、全部非代码加载W32候选无此区域地址别名。邻近D68ABC已证仅独立W32计时值，D68AC8为另一个W32伴随值；前方20B物品标记清理为五个W32并按既有20项消费，不能把它的地址形成当作向C6写入。该独立字节正常内容由原静态区加载零初始化生产，不从相邻现代对象越界取值；只承接此已证单BYTE，不猜整个填充区大小。

<a id="login-negative-byte-decision"></a>
## 工程裁决与实施

1. 在既有LegacyUiRecord.cpp实现现有声明，直接读真实底部栏、hover、mouseY token、左锚和服务器伴随值；单BYTE采用现对象字节视图/char位型转换。D68AC6由该模块唯一私有const uint8_t零字节承接原加载生产，不增加运行写者或业务字段。
2. 追踪器原24B节点和已批准W32 handle布局保持。为登录原BYTE观察增加既有模块内只读ReadLegacyMouseTrackerFixedByte桥：坐标等前16B直接读fixedPool；链接读其实际handle并投影为原固定节点/哨兵身份的W32字节，再取所需BYTE。0映射0、1..100映射D68DD0+(handle-1)×24、四哨兵分别D68AD0/D69730/D69750/D69770。
3. 上述投影是原只读观察的标量身份位型；不能解引用或作为新端指针。不截断本机地址，不写回token，不增加影子池/链接镜像/地址反查表，不更改旧handle解析、重置、追踪、申请释放、计数或保护。固定池在切入动态池后仍保旧内容，链接投影仍按固定池身份，不能用ResolveNode/current nativePool反算它。
4. 只在该BYTE观察边界扩展原handle适配；不是把handle编号或新进程指针低字节当作原字节。旧登录文档“暂不添加／暂不反算”保留其历史定位，本次已证身份投影由新SPEC裁决，不能读成永久禁止补真实缺口。
5. 只支持现已证正常page0/1、slot0..9；不增输入检查、拒绝、钳制或故障模拟。20点均有实际owner/生产合同，原server记录与正分类逻辑、两调用位置和RNG/音频/网络保持；不外推其他负分类/地址域。

<a id="login-negative-byte-acceptance"></a>
## 可度量验收

| 项目 | 本步前 | 本步源码条件 |
|---|---|---|
| 已有登录适配器定义 | 0 | 1，现声明与两尾consumer接同一实现 |
| 正常20读取点 | 存储定位，C6/五链接字节未闭 | 20完整owner对应；mutable现场取样，无副本或统一固定可用 |
| 固定池读取 | 无观察桥 | 12点，七坐标BYTE直接读取、五链接BYTE原身份投影 |
| 附加生产/副作用 | 无 | 无CryptInt::Read/RNG、申请释放/重置/新影子状态 |
| 原追踪器和登录父 | 已有 | 整原函数体保持；仅增加必要只读桥和既有缺provider |
| 语义F/R7/源码任务 | 1571/275/11closed | 不变，派生适配器不造原函数ID；V03仍open |
| 局部证据 | 804 | 806，仅LOGIN局部I/S；完整门禁不变 |

集中静态检查四文件精确差异、接口唯一与域映射、20原地址/12池偏移及五标量身份、现owner声明/布局与原重置只清最终pool的事实、原父及追踪函数全文保持；同步V03引用/机器登记和完整派生快照，不以静态或原位型投影代链接/运行。

<a id="login-negative-byte-source-state"></a>
## 当前源码结果与边界

本步补齐缺定义后，登录负分类尾部不再依赖悬空声明。两页20点、C6加载生产、固定池切换后的存储归属与链接身份对应已落实；没有统一固定返回或补可用服务器。整体登录连接/人物/主地图、全部资源/生命周期、原格式互通及V03仍待推进。保持音频暂停、RPLAN.24和不配置/构建/测试/运行/摘要限制。
