# Yobi动画与特效枚举语义命名 SPEC（YAN01）

2026-10-25。用户要求再整理按动画名命名的enum及效果。本项承接YCA01、BAN01和BFTN01，用新核实的客户端情绪窗口映射完善动画名称，并对齐相关战斗动作及已有用途依据的特效类型。

## 裁决与边界

依既有工程委托，纯重命名8个CharacterAnimationId、13个BattleActionId和4个BattleEffectType成员，替换源码／头文件引用。三个枚举的类型、数值、成员顺序、字段／参数整数承载及所有分支不变；不增加别名、校验、资源回退或业务处理。

| 动画号 | 原枚举后缀 | 新后缀 | 已证名称 |
|---|---|---|---|
| 12 | Animation12 | Wave | 挥手 |
| 13 | Animation13 | Joy | 高兴 |
| 14 | Animation14 | Angry | 愤怒 |
| 15 | Animation15 | Sad | 悲伤 |
| 16 | Animation16 | Nod | 点头 |
| 17 | Animation17 | Rock | 猜拳出石头 |
| 18 | Animation18 | Scissors | 猜拳出剪刀 |
| 19 | Animation19 | Paper | 猜拳出布 |

这些名称由原客户端提示文字与实际动画表建立联系，不再仅依赖服务端HAND／PLEASURE／ANGRY／SAD／NOD／GUU／CHOKI／PAA声明。名称表示动画动作身份，不保证每份角色资源都有相应帧，特效对象使用同编号时也不意味着画面一定呈现角色手势。

BattleActionId保留原动作号及播放／仅设置／清帧区别：

| 动作号 | 旧后缀 → 新后缀 |
|---|---|
| 7 | PlayAnimation2FacingEffectTarget → PlayBeginRunFacingEffectTarget |
| 8 | PlayAnimation2FacingMoveTarget → PlayBeginRunFacingMoveTarget |
| 10 | PlayAnimation4AndRestoreFacing → PlayEndRunAndRestoreFacing |
| 56 | PlayAnimation5AndResetFrames → PlayAttackAnimationAndResetFrames |
| 57 | PlayAnimation13AndResetFrames → PlayJoyAnimationAndResetFrames |
| 58 | PlayAnimation14AndResetFrames → PlayAngryAnimationAndResetFrames |
| 59 | PlayAnimation15AndResetFrames → PlaySadAnimationAndResetFrames |
| 60 | SetAnimation15 → SetSadAnimation |
| 61 | PlayAnimation12 → PlayWaveAnimation |
| 62 | PlayAnimation18 → PlayScissorsAnimation |
| 106 | SetAnimation13 → SetJoyAnimation |
| 179 | PlayAnimation6 → PlayCastAnimation |
| 205 | PlayAnimation16AndResetFrames → PlayNodAnimationAndResetFrames |

| 特效type | 旧后缀 → 新后缀 | 依据与限制 |
|---|---|---|
| 0 | FixedImage110009 → AttackClash | 动作115普通交锋分支；反击分支仍独立用type3。 |
| 20 | FixedImage110104 → ItemUse | 物品报文安排动作138，该动作创建此效果并等待；名称不表示此工厂实际消费物品。 |
| 29 | CenteredImage110157 → HealingCast | 补血施放侧动作137／198使用的效果；其他技能可共用，不声称为某单技能独占。 |
| 33 | ReverseEmitter → ReviveEmitter | 动作148的气绝回复／复活受术效果，原反向环形发射行为保持；清死亡位和恢复HP由后续队列处理。 |

同时把world/SceneState.cpp的kEmoteLocalActions584B6C原18个动画数字改用现有具名动画枚举，保持std::array<int32_t,18>及顺序。相邻协议表kEmoteProtocolActions584BB4、按钮索引与报文30／31不改；不能把协议命令、按钮序号和动画号混成同一枚举。

## 原端证据

- 当前cg2d.exe会话只读核对495FE0的18路按钮分派。0x49602D比较输入edx与17，0x496036经0x496528起18个W32目标分派，逐项目标与原case注释对应；未凭伪代码猜表首地址。
- 挥手提示0x587198经按钮1调用RequestEmote(2)；点头0x587180经按钮2调用RequestEmote(4)；高兴0x587168、愤怒0x587150、悲伤0x587138分别调用索引6／8／10。剪刀0x587108、布0x5870F0、石头0x587018分别调用14／16／17。
- 0x584B6C的18项S32实际为11,0,12,1,16,10,13,5,14,9,15,8,7,6,18,3,19,17；0x4FAA7B按索引取表，0x4FAA83调用51A060。0x51A076／0x51A083进入43EFC0；未骑乘路径0x43F114写对象+2B0，骑乘路径另按原条件映射。由此把提示文字明确关联到动画编号。
- 0x584BB4的独立18项S32实际为11,31,12,30,16,10,13,5,14,9,15,8,7,6,18,3,19,17；0x4FAA51取此表，0x4FAA76发送。站立／行走协议值31／30与本地0／1不同，原限制条件、发送与本地更新顺序均保持。
- 石头提示所在0x587018未被IDA定义为字符串，get_string不能直接读取；只读原字节与“做「出石頭」這個動作。”的编码逐字节匹配，并对同段已识别挥手提示做相同匹配，均一致。没有创建字符串定义、修改数据库、容错解码或改变来源字节。服务端include/char_base.h的17=GUU仅作独立编号旁证。
- 角色动作7／8／10／56／179已分别使用动画BeginRun／EndRun／Attack／Cast；其余高号动作的实际赋值、播放及清帧合同沿YCA01与BAN01。只把枚举名中的动画号换成已证动画名，不把高兴姿态命名为某个独占战斗技能或改变推进时机。
- 0x40FB80的交锋动作在0x40FBC0压入type3、0x40FBC4压入type0，两路共同在0x40FBCB调用工厂；0x40FF73在物品动作138创建type20；0x40FF23在复活动作148创建type33。type29的137／198映射沿既有40FC10／412310合同及补血队列生产方核对；固定图110157与居中／混合行为保持。工厂跳转表及type0～47边界沿BFTN01，不改既有分类。

## 仍保留的编号与验证

BattleEffectType中其余FixedImage／LevelScaledImage／CenteredImage／PayloadImage及BattleAction中PlayEffectType等项，部分只能证实图号、缩放或输入字段，尚无足够依据另起具体业务名。AnimationPlayback_LoopMode3仍只证实循环行为，不据3猜新业务模式。原名字及说明保留。

实施后对源码／头排除注释、保留字符串，逆向映射名称及唯一18项本地动画表后逐词核对；检查枚举值集合、旧引用清零、UTF-8、逐文件原换行和差异。沿用户此前完成后编译的授权使用既有build.bat Debug；不执行测试或客户端，不安装依赖、计算摘要或改动外部参考资料。

## 实施与静态核对

- 19份源／头替换25个枚举成员的161处标识符：角色动画49处、战斗动作98处、特效类型14处；另将kEmoteLocalActions584B6C的18项本地动画数字改为现有枚举引用。协议表、按钮索引和所有资源图号不变。
- 19份源／头保留字符串、排除注释并逆向映射名称及唯一18项本地表后，与修改前逐词一致。20／257／48个枚举成员的值集合、声明顺序和唯一性保持；旧名称引用清零，本地表18项逐项还原且协议表全文不变，UTF-8及逐文件原LF／CRLF、历史进度前缀检查通过。
- 按已获授权开始build.bat Debug，实际配置／编译／链接结果另记；不运行测试或客户端。

## 编译结果与交付范围

build.bat Debug使用既有MSVC x64预设，配置、编译和链接成功，退出码0；脚本报告产物build/windows-msvc-x64/Debug/cg2d_re.exe。本次输出仍有既有C4701可能未初始化变量告警及WindowShop的C4244窄化告警，未为消除告警增加初始化或修改数值转换规则。没有执行测试、客户端或算法，没有安装依赖、计算摘要、修改IDA或外部服务端／游戏数据。

本项范围为19份源／头、YAN01与唯一进度，共21份文件，沿当前分支精确提交并保留既有未跟踪内容。本轮临时补丁记录核对归属后清理。动画语义名称及四类特效用途已落实；其他仅有图号／字段依据的特效继续保留，画面和全部资源动作覆盖仍未运行验收。
