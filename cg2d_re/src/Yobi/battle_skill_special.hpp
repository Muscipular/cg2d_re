#pragma once

#include "base.hpp"
#include "battle_base.hpp"

#include <cstdint>

namespace cg::yobi {

namespace detail {

// 原42B282..42B600，SKL技能4的完整状态0生产；公共字段已由父入口读取。
[[maybe_unused]] void ProcessBattleMSkill4(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42B600..42BEAC；61/62/63/83共用生产，反弹参与者与当前目标分别保留。
[[maybe_unused]] void ProcessBattleMSkill61Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42BEAC..42C053，SKL技能50/51/52/53/88；结果255只跳过自身效果。
[[maybe_unused]] void ProcessBattleMSkill50Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42C053..42C241，SKL技能12；数值必须在施法者判空之前消费。
[[maybe_unused]] void ProcessBattleMSkill12(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42C241..42C50C，技能82按对侧两位置中点移动，再进入目标循环。
[[maybe_unused]] void ProcessBattleMSkill82(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42C736..42C905，技能97的图号与动作准备独立于82。
[[maybe_unused]] void ProcessBattleMSkill97(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42CB5E..42CF66及432E99尾；技能93先发布替换角色，再消费其名称和属性。
[[maybe_unused]] void ProcessBattleMSkill93(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// SKL技能98的状态0内部分派；由ProcessBattleMSkillHeader调用，427470总入口统一回存。
[[maybe_unused]] void ProcessBattleMSkill98(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42EB32..42EC95，SKL107只消费目标结果，带效果哨兵而不调用RID。
[[maybe_unused]] void ProcessBattleMSkill107(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42EC95..42F035与436DE2尾；SKL108先读数值再读结果，特殊同侧不追加159。
[[maybe_unused]] void ProcessBattleMSkill108(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42F035..42F3F2及436E03尾；SKL109只读结果，200000分支与失败图号目标不同。
[[maybe_unused]] void ProcessBattleMSkill109(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42F3F2..42F739与42998C尾；SKL112的2000反馈作用于施法者，其余默认反馈作用于目标。
[[maybe_unused]] void ProcessBattleMSkill112(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42F739..42F9BF与436FA1尾；SKL114在157调用后确认结果，再发布两项状态字段。
[[maybe_unused]] void ProcessBattleMSkill114(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42F9BF..42FBF4与429999尾；SKL117先追加效果，目标反馈后再排施法者73/74。
[[maybe_unused]] void ProcessBattleMSkill117(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42FC39..42FDCF，SKL263直接读取施法位置，目标-1与非-1的生产结果相同。
[[maybe_unused]] void ProcessBattleMSkill263(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原42FDCF..430162，SKL400保留异位置两次动作0及正常255/空目标的不同出口。
[[maybe_unused]] void ProcessBattleMSkill400(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原430162..430463，SKL401正常255才追加结束效果，两种目标出口均执行完整211尾。
[[maybe_unused]] void ProcessBattleMSkill401(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原430463..43062E，SKL402先消费结果/位置/侧别，再遍历选定一侧的现有角色。
[[maybe_unused]] void ProcessBattleMSkill402(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原43062E..43097A，SKL405每目标九字段/四图号转换，全部消费后才追加五条效果。
[[maybe_unused]] void ProcessBattleMSkill405(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原43097A..431C9C；404/406共用双角色生产，预读游标不推进正式消息游标。
[[maybe_unused]] void ProcessBattleMSkill404Family(
    YobiAction* const scene, BattleScenePayload* const payload);

// 原431CBA..43222F及42CB37..42CB5E；106/111/2002共享生产，首整数写攻击模式。
[[maybe_unused]] void ProcessBattleMSkill106Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原43227D..43282A及42BD50..42BEAC；累计反弹参与者，保留149调用后的双位置重读。
[[maybe_unused]] void ProcessBattleMSkill2028(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原432867..432ECE，99..104/2019/3001共享状态生产；固定状态载荷跨144调用保留。
[[maybe_unused]] void ProcessBattleMSkill99Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

// 原4357BA..435B25及42CB44..42CB5E；2005/4003共用，首整数保留攻击模式，尾部递增子状态。
[[maybe_unused]] void ProcessBattleMSkill2005Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed);

} // namespace detail

} // namespace cg::yobi
