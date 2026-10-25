# Yobi战斗M消息内部模式及合击子阶段具名（YMN08）

日期：2026-10-25。用户要求继续分析剩余magicNumber，已证用途定义枚举并替换引用，证据不足保留说明，完成后执行build.bat Debug编译验证。

## 范围与裁决

按既有工程委托，先记SPEC，由本线程依据当前生产—消费关系及cg2d.exe原端证据裁决。新增四组共14项S32枚举：附加图形读取模式2项、PSN数值模式5项、ITM物品模式3项、CMB合击子阶段4项。附加图形模式供三个技能实现共用，置于既有battle_message.hpp的detail域；其余三组只在battle_message.cpp的detail域定义，不新增文件依赖或公开接口。

只替换同一解释域的数字。原整数参数、全局和载荷字段类型、布局、初始化、游标及读取数量、分支、失败路径、队列顺序、计数递增和RNG顺序保持。PSN具体状态名称、ITM模式7的物品身份未证，按已证资源变化、图号和动作命名；不从协议缩写猜业务。

## 枚举取值

| 枚举类型 | 值 | 成员后缀 | 已证用途 |
| --- | --- | --- | --- |
| BattleMAuxGraphicsReadMode | 0 | ReadObjectFields | 从对象消息游标读取8整数，其中6项图号经既有版本映射，另2项值原样输出。 |
| BattleMAuxGraphicsReadMode | 1 | SkipPreviewFields | 只消费预扫描游标8整数，不读取对象或解引用输出指针。 |
| BattleMStatusValueMode | 0 | DecreaseHpWithHit | 音效273、减HP并播放受击动画；原死亡处理顺序保持。 |
| BattleMStatusValueMode | 1 | IncreaseHpWithRecoveryEffect | 播放状态HP恢复效果，增加HP并等待。 |
| BattleMStatusValueMode | 2 | DecreaseFp | 音效274，减少FP并等待。 |
| BattleMStatusValueMode | 3 | DecreaseHpAndWait | 音效273、减少HP，按原死亡标记选择等待路径。 |
| BattleMStatusValueMode | 4 | DecreaseHpWithImage104216AndWait | 在模式3的公共路径中增加图104216的配置效果。 |
| BattleMItemMode | 0 | RecoverTargetHp | 按目标列表排入恢复效果及HP增加；255结束列表。 |
| BattleMItemMode | 6 | SealCard | 排入封印卡三阶段动作，按独立结果字段选择成功／失败表现。 |
| BattleMItemMode | 7 | PlayNodAndWait | 音效438、点头、等待120及60 tick；具体物品身份未证。 |
| BattleMComboPhase | 0 | ReadAndQueue | 解析目标、参与者及附加字段，建立合击动作队列，读RID尾后递增。 |
| BattleMComboPhase | 1 | WaitForEntryCompletions | 等待完成计数达到参与者数，归零该计数并递增子阶段。 |
| BattleMComboPhase | 2 | AdvanceEntriesAndShowSequence | 推进空闲参与者；达到原计数门槛后按原标志条件显示序列并递增。 |
| BattleMComboPhase | 3 | WaitForTurnReady | 等待回合就绪，按原存活条件恢复目标朝向并结束事件。 |

成员前缀分别为类型名加下划线，显式取值如表；不把全局零初始化改为业务默认模式，不把其他消息的packetSubState58套用CMB枚举。

## 原端及接线证据

- 当前cg2d.exe会话经MCP只读取证。43BF30在0x43BF38／0x43BF3B区分0和1：模式1在0x43BF41／0x43BF46先读取两次，0x43BF50／0x43BF55／0x43BF5A再循环两次；模式0在0x43BF6C起读取并映射六项图号，0x43BFCD读取两项值。当前函数和28处调用同域：预扫描8处、对象读取20处，三个技能实现已有battle_message.hpp依赖。
- PSN从0x429ADB读模式并在0x429AE1写5ABA88，0x429B45起依次判0、1、2、3／4；0x429BD9／0x429C83／0x429D0C／0x429DDD向角色+1034写6／5／3／6。当前battle_status.cpp的4107C0／410B00资源消费段分别执行减HP、增HP、减FP，符合既有BattleNumber枚举。
- 模式0在0x429B96／0x429BAB排入159／32，死亡分支0x429C01／0x429C17排46再47；模式3／4的0x429E01／0x429E17排47再46，随后等待38或36。两组死亡顺序不同，按原顺序保持。模式4在0x429D61单独判4，0x429D7B写图19718h（104216），0x429D94排252；不猜图对应状态名。
- ITM预扫描0x43ADB1和执行0x436A59均依次用减0、减6、递减分派到0／6／7；执行模式在0x436A62写59FA10。模式7在0x436AB1写音效1B6h（438），随后0x436AE2／0x436AF8／0x436B0D排205／38／37。模式6在0x436C65／0x436C7D／0x436C96排154／155／156，与已证封印卡效果握手一致；模式0的资源显示类型为既有IncreaseHp。预扫描字段消费及执行中的255／空目标分支分别保留，不统一二者字段布局。
- CMB入口0x42781E从载荷+58读取，0x427825比较3，0x42782E经0x437B38起四项W32分派：0→427835、1→427EC7、2→427EEA、3→427F5A。已核实际目标：0读消息排队后于427EBE递增；1在427ED0比较完成／参与计数、427ED8清计数、427EE1递增；2推进角色并在427F3A调用420100显示序列；3在427F5D判回合就绪，经42AAF5／42AAFF恢复朝向及结束事件。当前四个case与原表逐项对应。

## 保留数字及证据缺口

- PSN模式名称只表达资源变化及表现，未证明中毒、天气或具体技能身份。图104216、音效273／274／438及其他资源号仍为独立数字域；不额外创建资源枚举。
- ITM模式6的结果字段1和模式0另读字段value==7，均不是本轮物品模式。前者按成功／失败表现处理，后者仅决定恢复效果；其业务定义未充分闭合，保留数字，不因同值误替换。
- 全局模式变量的初始0只是静态初始化；其他消息的子状态、计数0、位置255、参数0及技能内部mode不套用本轮枚举。43BF30的未知mode仍不读取字段；ITM未知mode与CMB未知子阶段仍走原返回路径。
- 保留CMB空参与者列表上amount可能未初始化的路径，不补默认值。全局主场景phase8的同对象发布证据仍不足，沿YMN07保留，不按CMB消息头或本轮子阶段替换。

## 验证安排

实施后核四组14项S32取值／顺序／唯一性及引用闭合；排除新增声明及注释、保留完整字符串，将新标识符逆映射为原数字逐词比较五份源／头。核对原整数接口、分支、参数、读取数量及调用顺序，检查UTF-8、原换行、历史进度前缀及差异。随后执行build.bat Debug配置、编译和链接；不测试、运行客户端／算法、安装依赖、计算摘要或改动IDA／外部资料，实际结果随后追加。

### 实施结果

- 已在四份CPP及既有battle_message.hpp新增四组14项S32枚举，替换51处数字引用：附加图形模式30、PSN模式6、ITM模式11、CMB子阶段4；按文件为battle_message.cpp 32、battle_skill_attack.cpp 9、battle_skill_special.cpp 8、battle_skill_status.cpp 2。没有新增头依赖或改变整数接口。
- 排除新增枚举及注释、保留完整字符串后的五份源／头逆映射逐词一致；四组14项数值／顺序／唯一性、引用域／闭合、UTF-8、原LF／CRLF、历史进度前缀及差异检查通过。43BF30全部28处调用已具名，独立value==7、结果字段1、全局初值0及CMB的amount原路径保持，开始编译验证。
- build.bat Debug使用既有MSVC x64预设，配置、编译和链接成功，退出码0；脚本报告产物build/windows-msvc-x64/Debug/cg2d_re.exe。相关模块重编译仍有既有C4701：battle_message的amount、battle_skill_status的lastAmount／statusType、battle_skill_special的statusType／selfFlags可能未初始化，原路径保持；未测试、运行客户端／算法、安装依赖、计算摘要或改动IDA／外部资料。
- 本轮提交限定四份CPP、一份既有头、本文及唯一进度，共7份文件，保留原分支及既有未跟踪内容，没有创建临时补丁文件。PSN状态名、ITM模式7物品身份、独立结果／效果字段及全局主场景phase8继续保留说明；具名和编译通过不代表协议运行、全部战斗状态或剩余魔法数字已完成验收。
