# Yobi地图及模态阶段魔法数字具名（YMN03）

日期：2026-10-25。继续按用户要求分析剩余magicNumber，已证用途定义枚举并替换引用，证据不足保留说明，完成后执行build.bat Debug编译验证。

## 范围与裁决

按既有委托先记SPEC，由本线程依当前生产—消费关系和原端指令裁决。本轮新增三组17项枚举：地图过渡模式10项、地图角色过渡phase四项、模态动作phase三项。仅具名已证数字、补充枚举解释及必要头引用，不改函数名、布局、字段／参数类型、算式、默认分支、初始化、调用或RNG顺序。

MapTransitionMode以S16为底层类型，放在既有map_transition.hpp的cg::yobi；MapActorPhase以S32为底层类型，放在既有map_actor.hpp的cg::yobi。内部network及world生产方沿现有整数接口替换同源赋值；network补已有map_transition.hpp引用。仅由5046D0消费的ModalActionPhase以S32定义在window.cpp的detail域，不扩公开接口。

## 已证枚举

| 枚举 | 值与成员后缀 | 用途及命名边界 |
| --- | --- | --- |
| MapTransitionMode | Wandering=0 | 按候选方向逐格游走，定时安排两路18902特效，按原距离／时间条件结束；不是普通地图角色phase0。 |
| 同上 | PairedEffects=1 | 依次生产两次18902特效，过1200时间阈值后重建原对象并标死过渡对象。 |
| 同上 | ExpandingOrbit=2 | 三点圆周表现，半径从0向外增加；过阈值后结束。 |
| 同上 | ContractingOrbit=3 | 半径由800向内减少，到零后结束。 |
| 同上 | AcceleratingBlink=4 | 原地闪烁，step递减使间隔缩短；不表示移动速度加快。 |
| 同上 | DeceleratingBlink=5 | 原地闪烁，step递增使间隔增长；不表示移动速度减慢。 |
| 同上 | StraightBlink=6 | 直线步进并闪烁，保持原结束距离／时间条件。 |
| 同上 | ApproachingBlink=7 | 靠近玩家逐格步进并闪烁，保持原结束条件。 |
| 同上 | TrailUsingYOrigin=8 | 抛落及十槽残影，主图和残影绘制Y偏移取轴1的资源原点。 |
| 同上 | TrailUsingXOrigin=9 | 同一残影逻辑，但绘制Y偏移按原端取轴0；不将可疑跨轴行为修正为轴1。 |
| MapActorPhase | Normal=0 | 不阻断4417E0的普通移动／绘制，不命名为Idle；普通角色可以继续行走。 |
| 同上 | Mounting=1 | 上骑过渡，使用rideActor44和1AE10效果，结束后回0；无骑乘对象也沿原路径回0。 |
| 同上 | Dismounting=2 | 下骑过渡，清骑乘对象、恢复原图后回0；协议撤除骑乘也安排此阶段。 |
| 同上 | Spiral=3 | 旋转方向、按进度生成螺旋位移，结束播放收步动画后回0；具体业务招式名未证。 |
| ModalActionPhase | Opening=0 | 绘制并扩张边框；达到原计数后按winId转1或2。 |
| 同上 | ReadyOnly=1 | 仅将readiness置1；当前由winId=-1／-2进入，不据此猜两个负编号的业务名称。 |
| 同上 | DrawingBody=2 | 绘制九宫格／窗口底图并将readiness置1；不表示另行打开WindowRuntime窗口。 |

成员前缀分别为MapTransitionMode_、MapActorPhase_、ModalActionPhase_。所有原字段继续为整数；特别是MapTransitionPayload.mode与创建参数继续S16，角色／窗口phase继续S32，未改为枚举字段。

## 原端及接线证据

- 当前cg2d.exe会话经MCP只读取证。0x447E69以MOVSX读取载荷首S16，0x447E6C比较9，0x447E75经实际W32表0x448014分派。读取全部10项并核目标处case注释：0～9分别落447E7C、447E92、447EA8、447EBE、447ED1、447EE4、447EF7、447F0A、447F1D、447F30；随后分别调用446D80、446FA0、447070、447230、4473B0、4474E0、447610、447820、447A20、447C40。
- 447070在0x447083初始化，0x4470DE增加半径4、按三点圆周绘制；447230的相反半径更新与当前实现对应。4473B0的0x447422切换显隐，0x44742E取step/2，0x447437递减step；命名加速仅指闪烁。当前其余模式的具体步进、时间、轨迹及返回路径保留原实现。
- 447A20的0x447B43／447B8C为主图／残影Y偏移读取轴1；447C40的0x447D62／447DAA读取轴0。只据此区分两种Trail模式，保留同一既有helper的轴参数及原次序。
- 当前网络50DD80无记录路径将协议mode0／1／2／3映射为过渡2／8／4／0；有记录路径先生产世界记录，再映射为3／9／5／1。只替换actionMode赋值，协议mode的case0～3、有记录标志及value02保持。创建后mode0／1使用union附件臂，其他仍使用初始化数值臂，不改选择或读取。
- 角色0x43E4E4读取动作+2C的完整W32，0x43E4E7比较3，0x43E4F0经实际W32表0x43EFAC分派。四项实际目标0～3为43EF9F、43E4F7、43EA02、43EDA2，目标case注释一致。1／2执行路径移动和上下骑，3执行旋转位移；0直接返回0，未知值原43EF98回写0并返回0。归零写入可见43E78D、43ECE4、43ED3C、43EF29，语义与当前生产消费吻合。
- 地图角色生产包括CreateMapActorAction初始化、骑乘撤除、玩家70／80类回复及WorldActor对应回复。DrawMapMainGear406明确读取MapActor载荷，其parent->phase!=0只与这套角色阶段关联；保留4.06挂件既有判断，不据cg2d证据新增渲染行为。无callback的NewYobiGearPet初始化phase0缺少相同消费，不混用该枚举。
- 5046D0在0x5046F8判phase0，0x5046FE～504706区分1／2；0x50498D写1，0x50497A写2，0x5048EA起在阶段1发布readiness=1，阶段2绘制后同样发布。创建仍沿原对象初始phase0，不增加显式写入；其他windowOpening载荷的phase可能透传存放窗口参数，不按此模态枚举替换。

## 保留数字与未证项

- map_observation.cpp的phase1～5属于观察角色／编队状态机，不能与普通MapActorPhase合并；本轮未完成该组生产消费的原端核对，保留数字。
- 模态winId的-1／-2、负场景类型和environment中的资源选择type／characterAction模式属于其他解释域；本轮没有完整业务名称证据，保留原值。frameCount>=10、readiness=1为计数与就绪数值，不当作模态phase替换。
- 地图过渡initialized18的0／1只证实一次初始化，union.state1C目前只有写零，仍保留；图号、音效号、方向、轨迹索引、时间阈值、显隐位和数值零不机械枚举化。
- 过渡8／9的跨轴差异及角色phase3只按已证行为命名，不猜传送、技能或剧情名称。既有BeginMapActorPhase2／ApplyPlayerPhase3Reply函数名本轮不扩展重命名。

## 验证与实施记录

实现后排除新增枚举声明、注释及必要已有头引用，将新标识符还原为原数字，保留全部字符串逐词比较。检查17项取值／顺序／唯一性和同域引用，未知值、union分支、协议映射、其余实参及所有原类型／控制流保持，UTF-8与原换行保持。静态核对后按本轮授权执行build.bat Debug配置、编译、链接；不运行测试、客户端或算法，实际结果随后追加。

### 实施结果

- 已在8份源／头新增三组17项枚举，替换46处数字引用：地图过渡20处、普通地图角色phase21处、模态phase5处。network补充既有地图过渡头引用，其余依赖沿原头文件；公开YobiRuntime接口及现有函数名保持。
- 排除上述新增声明、注释及一处头引用后，8份源／头将新枚举引用还原为原数字，与实施前保留全部字符串逐词一致。S16／S32原类型、枚举取值／顺序／唯一性、引用闭合、协议映射、未知值及union选择保持；UTF-8、各文件原LF／CRLF、历史进度前缀和差异检查通过。
- build.bat Debug使用既有MSVC x64预设，配置、编译及链接成功，退出码0；脚本报告产物build/windows-msvc-x64/Debug/cg2d_re.exe。未测试、运行客户端或算法，未安装依赖、计算文件摘要或改动IDA数据库及外部资料。
- 观察状态机、窗口负编号、union.state1C及资源／计数等数字仍按上文保留。本轮构建只验证具名与引用可构建，不代替未知业务命名或图像表现验收。
