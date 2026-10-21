# 战斗角色 flagsFDC 语义与枚举方案（BAF01）

日期：2026-10-21。状态：用户确认后已实施，静态核对完成；未编译或运行验证。

## 1. 范围与依据

用户要求分析 flagsFDC，尝试使用 enum，并允许结合 `cgmsv_server/battle`。本项只读核对客户端当前源码和本机服务器源码；服务器提供当前协议语义参考，不替代原端机器指令证据，不据此修正客户端历史行为。未调用 IDA、编译、运行或计算摘要。

- 客户端字段：`src/Yobi/battle_base.hpp`，`BattleActorPayload::flagsFDC` 是32位位集合；独立于 `stateFE0`、`effectFlagsFE4`、`YobiAction::flags` 和 `BattleEntryEffectRecord::flags`。
- 服务器定义：[include/battle.h](../../../cgmsv_server/include/battle.h)，480–515行的 `BC_FLAG_*`。
- 服务器生产：[battle/battle_command.c](../../../cgmsv_server/battle/battle_command.c)，`BATTLE_MakeCharaString` 的203–372行由角色类型、状态及属性生产标志，434–445行放入角色记录的 HP/FP 之后。
- 另一路生产：[battle/battle_event.c](../../../cgmsv_server/battle/battle_event.c)，`BATTLE_makeCharFlag` 的19710–19830行供骑乘等消息使用。
- 客户端 BC 消费：`src/Yobi/battle_message.cpp:3376` 读取 `inputFlags`，3392行起逐项更新；部分 BM 角色记录直接将读取值赋给 flagsFDC，例如2057、2557、2796、2866、3096、3170行。骑乘还通过 `BattleRideCarrierPayload::flags08` 保存并在 `battle_ride.cpp:243` 恢复。

服务器注释存在历史乱码，因此以下结论以宏定义数值、实际条件与客户端生产／消费为依据，不据乱码补译语义。服务器的条件编译声明也不等于当前运行版本一定启用。

## 2. 单位映射

下表给出候选 `BattleActorFlag_` 后缀。数值保留原值；协议常量采用独立的 `BattleCharacterFlag_` 前缀，避免将两个域无差别混用。

| 位值 | 服务器 BC 定义 | 客户端候选名称／说明 |
| --- | --- | --- |
| `0x00000001` | `APPEAR` | `EntryComplete`：客户端入场／归位流程完成标记，不能直接命名为 Appear，见下一节 |
| `0x00000002` | `DEATH` | `Dead`：倒地／死亡；待机选择动作10，复活路径清除此位 |
| `0x00000004` | `PLAYER` | `Player`：协议中的玩家类别；骑乘宠物也可能按此类别编码，不等于实体一定是玩家 |
| `0x00000008` | `MON` | `Pet`：服务器只在 `CHAR_TYPEPET` 分支设置，不应翻成普通怪物 |
| `0x00000010` | `ABNORMAL_POISON` | `Poison`：中毒，特效110075 |
| `0x00000020` | `ABNORMAL_SLEEP` | `Sleep`：睡眠，特效110076，受击唤醒路径清除此位 |
| `0x00000040` | `ABNORMAL_STONE` | `Stone`：石化，特效110077 |
| `0x00000080` | `ABNORMAL_INEBRIETY` | `Inebriety`：醉酒，对应服务器 `ModDrunk`、特效110078；Lua现名isDrug保持 |
| `0x00000100` | `ABNORMAL_CONFUSION` | `Confusion`：混乱，特效110079 |
| `0x00000200` | `ABNORMAL_FORGET` | `Forget`：遗忘，对应 `ModAmnesia`、特效110080 |
| `0x00000400` | `2ACT` | `TwoActions`：双动，服务器玩家无出战宠时设置；BC消费不直接写入本字段，见下一节 |
| `0x00000800` | `REVERSE_TYPE` | `ReverseType`：属性反转，特效110152 |
| `0x00001000` | `DAMAGE_UP` | `DamageUp`：伤害增强；客户端以1.3倍放大表现，并在放大／缩小动作完成时置位／清位 |
| `0x00002000` | `MORPH` | `Morph`：变身，对应服务器玩家 `Metamo_Count`；客户端用于绘制和挂件处理 |
| `0x00004000` | `DEF_UP` | `DefenseUp`：防御上升，特效103018 |
| `0x00008000` | `DEF_DOWN` | `DefenseDown`：防御下降，特效103019 |
| `0x00010000` | `ATK_UP` | `AttackUp`：攻击上升，特效103020 |
| `0x00020000` | `ATK_DOWN` | `AttackDown`：攻击下降，特效103021 |
| `0x00040000` | `AGL_UP` | `AgilityUp`：敏捷上升，特效103022 |
| `0x00080000` | `AGL_DOWN` | `AgilityDown`：敏捷下降，特效103023 |
| `0x00100000` | `RCV_UP`，条件编译 | `RecoveryUp`：回复能力上升；客户端103033分支置位，沿用103018显示资源 |
| `0x01000000` | `ATTR_EARTH`，条件编译 | `Earth`：地属性标记；客户端映射到 `fields288[0]` |
| `0x02000000` | `ATTR_WATER`，条件编译 | `Water`：水属性标记，映射到 `[1]` |
| `0x04000000` | `ATTR_FIRE`，条件编译 | `Fire`：火属性标记，映射到 `[2]` |
| `0x08000000` | `ATTR_WIND`，条件编译 | `Wind`：风属性标记，映射到 `[3]` |
| `0x10000000` | `REBIRTH`，条件编译 | `Rebirth`：REB形态状态；客户端切换图像、附加持续特效并置位，退出形态清位，不应命名为死亡后复活 |

证据定位：六种异常的客户端置位在 `battle_effect.cpp:2056`；属性增减／回复能力的置位在2460行；状态恢复在2656行；Lua只读属性在 `script/LuaData.cpp:268` 起；Rebirth形态进入／退出分别见 `battle_status.cpp:462`、597行。

暂不加入运行状态枚举：`0x20000000` 仅在服务器头文件找到 `BC_FLAG_LEVELDOWN` 条件定义，尚未找到本次所查battle目录的生产使用和客户端消费。`0x00200000`、`0x00400000`、`0x00800000`、`0x40000000`、`0x80000000` 本次未建立 flagsFDC 的语义证据；保持原32位传递，不命名、不过滤未知位。

## 3. 不可直接按服务器宏替换的地方

1. **最低位反向承接。** 服务器 INIT／RESCUE 且非突袭分支设置 `BC_FLAG_APPEAR`。客户端 BC 分支收到该位后清 flagsFDC 的bit0并放到入场起点；未收到则置bit0并就位（`battle_message.cpp:3392`）。`ReturnBattleActorsToFormation_43C910` 对该位为0的角色派发phase2（`battle_scene.cpp:414`），角色抵达目标后置位（`battle_actor.cpp:885`）。因此协议侧使用 `BattleCharacterFlag_Appear`，运行侧使用 `BattleActorFlag_EntryComplete`。其他消息的整值赋值仍保持原样，不额外套用取反。
2. **BC不完整复制所有位。** `0x400` 在BC中只选择附着特效（`battle_message.cpp:3461`）；`0x1000` 在BC中只发布scale（3458行），动作83／84另行置位／清位（`battle_cast.cpp:785`、804行）。不能因为枚举定义齐全就把这些分支改为整值赋值。Lua的isTwoAction读取FDC bit10与这条BC更新路径之间存在一致性核查点；本轮不据此修改数据生产。
3. **不同字段的同值不代表同义。** 服务器 `BM_FLAG_DEATH=0x20`，但 BC／flagsFDC 的 `0x20` 是Sleep；`CHAR_BATTLEFLG_*` 又是另一套服务器内部状态。替换范围必须沿 flagsFDC、其局部快照、对应协议输入追踪，不能全局替换数值。`stateFE0` 即使同步bit1也不自动使用本枚举。
4. **成对清位保留。** `battle_message.cpp:3480` 与 `battle_effect.cpp:2713` 的现有代码明确保留每对共用清位掩码：判断DefenseDown未置位时清DefenseUp，AttackDown／AgilityDown同理。这与直觉对称更新不同，但枚举化只将真实被清除的掩码命名，不顺带改算法；要纠正须另行核原端证据。
5. **缓存与重新读取时点保持。** 已有低8位、低16位读取、局部快照和调用后重新读取不合并；不把 `reinterpret_cast`、截断、移位提取统一重写为辅助函数。

## 4. 枚举落地方案

建议在既有 `src/Yobi/battle_base.hpp` 的 `cg::yobi::detail` 内定义两套固定 `std::uint32_t` 底层类型的普通enum，枚举项带类型前缀，沿现有项目的位掩码风格。协议枚举描述BC／相关角色消息中的位；运行枚举描述flagsFDC。除bit0命名不同外，已证共享位保持相同数值，分别在对应语义边界使用。

字段继续保存 `std::uint32_t flagsFDC`。这是允许多个位并存及未知位原样传递的位集合，不是只能选一个枚举项的状态机；本轮建议先命名常量，不改字段类型或布局，不引入通用位运算模板／operator重载。暂不重命名字段，原偏移后缀继续帮助证据定位。

以下仅示范边界和用法；实施时按上表补齐全部已证项，不能将示例当成只有三个位的完整声明：

```cpp
enum BattleCharacterFlag : std::uint32_t {
    BattleCharacterFlag_Appear = 0x00000001u, // 协议：需要入场。
    BattleCharacterFlag_Dead = 0x00000002u,
    BattleCharacterFlag_Sleep = 0x00000020u,
};

enum BattleActorFlag : std::uint32_t {
    BattleActorFlag_EntryComplete = 0x00000001u, // 客户端：入场／归位完成。
    BattleActorFlag_Dead = 0x00000002u,
    BattleActorFlag_Sleep = 0x00000020u,
};

if ((inputFlags & BattleCharacterFlag_Appear) != 0) {
    payload->flagsFDC &= ~BattleActorFlag_EntryComplete;
}
payload->flagsFDC &= ~BattleActorFlag_Sleep;
```

组合掩码显式列成员，建议定义为本域的 `inline constexpr std::uint32_t`：

- `SleepOrStoneMask = Sleep | Stone`，对应 `0x60`，用于特定动画播放门禁，不泛称全部不可行动状态。
- `PlayerOrPetMask = Player | Pet`，对应 `0x0C`。
- `AbnormalMask = Poison | Sleep | Stone | Inebriety | Confusion | Forget`，对应 `0x3F0`；原清位常量 `0xFFFFFC0F` 等价于其32位补码。
- `ParameterChangeMask = DefenseUp | DefenseDown | AttackUp | AttackDown | AgilityUp | AgilityDown`，对应 `0xFC000`；原 `0xFFF03FFF` 只清这六位，不包含 `RecoveryUp`，不得扩大为所有增益。
- `ElementMask = Earth | Water | Fire | Wind`，对应 `0x0F000000`，仅在已有同组合位置使用，不新增规范化行为。

## 5. 实施范围和验收

后续替换须逐处确认来源：battle_actor、battle_scene、battle_message、battle_effect、battle_cast、battle_attack、battle_impact、battle_status、battle_attachment、battle_base、battle_skill、battle_skill_special、battle_ride、battle_lua_skill、map_render及LuaData。局部变量名相同不代表同一域，不进行全文数字替换。

- 静态验收：逐项数值一致；所有替换表达式保持原32位掩码／低位截断、取样次序与赋值次数；结构大小、协议序列化、Lua键名和返回值不变；未知位、非对称清位及纯快照／整值复制保持。
- 初次分析仅提交方案及进度；用户随后明确“先实现上一个SPEC”，确认上述枚举边界。未确认处不猜命名，不将静态分析记为运行验证。
- 不修改服务器，不编译、运行或新增测试。

## 6. 实施结果（2026-10-21）

- 在既有battle_base.hpp内增加两套各26项的普通enum及四个已用组合掩码，替换16个CPP中的304处掩码常量；ElementMask没有现有同组合用点，未增加闲置声明。flagsFDC继续使用uint32_t，字段布局不变。
- 按用户补充要求，将22处 `(flagsFDC >> 1) & 1u` 改为 `(flagsFDC & BattleActorFlag_Dead) != 0`。该位固定为0x2，判断结果在原算术／显式转换中仍为0或1；保留原先加1、乘14、除4等运算、读取次数和时点，不新增辅助函数。
- 协议bit0与运行bit0分别命名，BC逐位更新、BM整值复制、骑乘恢复、低位读取、非对称清位、Lua公开键及未知位保持；未处理Yobi其他模糊字段重命名。
- 静态核对：两套枚举逐项与本表一致；304处新掩码按实际声明求值得到原值；16个CPP除这304处及22处死亡位判断外，全文与修改前一致，UTF-8及既有LF保持。头文件除新增声明和字段注释外一致。差异检查通过；不代表编译、链接或运行验收。
