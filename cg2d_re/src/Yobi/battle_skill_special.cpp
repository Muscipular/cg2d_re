#include "battle_skill_special.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_message.hpp"
#include "battle_skill.hpp"

#include "battle_actor.hpp"
#include "battle_scene.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::yobi {

namespace detail {

static std::int32_t gBattleMSelectedPosition5ABA98 = 0; // 原全局：0x005ABA98。 SKL98额外操作位置，独立于主施法者/目标，不属于13项快照。

// 原42B282..42B600，SKL技能4的完整状态0生产；公共字段已由父入口读取。
[[maybe_unused]] void ProcessBattleMSkill4(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    const std::int32_t mode = TakeBattleMInteger_438F70(scene);
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[casterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const skillPayload = caster->payload.battleActor;
    skillPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = skillLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->attackModeFF4 = mode;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 70);
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        if ((gBattleMResultFlags5A4A20 & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        const std::int32_t effectImage = firstImage;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const BattleEntryEffectRecord effectRecord {effectTargetPosition, effectFlags, amount, effectImage};
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, effectRecord);
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxiliaryImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxiliaryTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t auxiliaryCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const auxiliaryCaster = cg::world::gBattleAllEntries59E980[auxiliaryCasterPosition];
        AppendBattleAuxEffects_43BC10(auxiliaryCaster, auxiliaryTargetPosition, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, auxiliaryImage);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x108u) == 0) {
            if ((flags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                const std::int32_t playerTargetPosition = gBattleMTargetPosition5AAA40;
                if (cg::world::gBattleCurrentPosition5ABCA4 == playerTargetPosition) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
                flags = gBattleMResultFlags5A4A20;
            } else if (amount != 0 && (flags & 3u) != 0) {
                if ((flags & 0x8000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 123);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x4000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 122);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x2000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 121);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x20000000u) == 0) {
                    const std::int32_t reactionKind = (flags & 2u) != 0 ? 165 : 27;
                    const std::int32_t reactionTargetPosition = gBattleMTargetPosition5AAA40;
                    YobiAction* const reactionTarget = cg::world::gBattleAllEntries59E980[reactionTargetPosition];
                    AppendBattleCast_4392A0(scene, reactionTarget, reactionKind);
                    flags = gBattleMResultFlags5A4A20;
                }
            }
        }
        if ((amount != 0 || (flags & 0x108u) != 0) && (flags & 0x2000E000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
            if ((flags & 0x100u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原42B600..42BEAC；61/62/63/83共用生产，反弹参与者与当前目标分别保留。
[[maybe_unused]] void ProcessBattleMSkill61Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    payload->completedEntryCount00 = 0;
    payload->entryCount04 = 0;
    gBattleMMessageValue59FA14 = 0;
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[casterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const skillPayload = caster->payload.battleActor;
    skillPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = skillLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    if (gBattleMSkill5AAA78 == 83) {
        const std::int32_t pointIndexX15 = cg::world::gBattlePointIndices5536FC[15];
        const std::int32_t pointIndexX5 = cg::world::gBattlePointIndices5536FC[5];
        const std::int32_t x15 = cg::world::gBattleGraphicPoints553758[pointIndexX15][2];
        const std::int32_t x5 = cg::world::gBattleGraphicPoints553758[pointIndexX5][2];
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->moveTargetXFE8 = x15 + (x5 - x15) / 2;
        const std::int32_t pointIndexY15 = cg::world::gBattlePointIndices5536FC[15];
        const std::int32_t pointIndexY5 = cg::world::gBattlePointIndices5536FC[5];
        const std::int32_t y15 = cg::world::gBattleGraphicPoints553758[pointIndexY15][3];
        const std::int32_t y5 = cg::world::gBattleGraphicPoints553758[pointIndexY5][3];
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->moveTargetYFEC = y15 + (y5 - y15) / 2;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 8);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 6);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 137);
        const std::int32_t position = gBattleMCasterPosition5AAA64;
        const std::int32_t kind = position < 20 ? 90 : 198;
        YobiAction* const positionActor = cg::world::gBattleAllEntries59E980[position];
        AppendBattleCast_4392A0(scene, positionActor, kind);
        const std::int32_t playerCasterPosition = gBattleMCasterPosition5AAA64;
        if (cg::world::gBattleCurrentPosition5ABCA4 == playerCasterPosition) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerCasterPosition], 256);
        } else {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerCasterPosition], 52);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
        }
    } else {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
        const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
        const bool useLevelOffset = level < 12;
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t image = useLevelOffset ? kBattleMSkillEffectOffsets553E58[level] + 110081 : 111014;
        cg::world::gBattleAllEntries59E980[imageCasterPosition]->payload.battleActor->image1028 = image;
    }
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x10000u) != 0) {
            const std::int32_t accumulatedValue = gBattleMMessageValue59FA14;
            const std::int32_t target = gBattleMTargetPosition5AAA40;
            gBattleMFlags5ABA84 = flags;
            const std::int32_t total = accumulatedValue + amount;
            flags &= ~0x20u;
            gBattleMFeedbackSourcePosition5ABA90 = target;
            gBattleMMessageValue59FA14 = total;
            gBattleMResultFlags5A4A20 = flags;
            payload->entryPositions08[payload->entryCount04] = target;
            ++payload->entryCount04;
            flags = gBattleMResultFlags5A4A20;
        }
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, flags, amount);
        const std::int32_t targetSkillPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const targetSkillActor = cg::world::gBattleAllEntries59E980[targetSkillPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        targetSkillActor->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t targetLevelPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const targetLevelActor = cg::world::gBattleAllEntries59E980[targetLevelPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        targetLevelActor->payload.battleActor->castSkillLevel1044 = targetLevel;
        std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
        const bool sourceBelowTwenty = sourcePosition < 20;
        std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
        if (sourceBelowTwenty) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 149);
            sourcePosition = gBattleMCasterPosition5AAA64;
            targetPosition = gBattleMTargetPosition5AAA40;
        } else {
            if (targetPosition < 10 && sourcePosition != 20) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 149);
                sourcePosition = gBattleMCasterPosition5AAA64;
                targetPosition = gBattleMTargetPosition5AAA40;
            }
            if (targetPosition >= 10 && sourcePosition != 21) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 149);
                sourcePosition = gBattleMCasterPosition5AAA64;
                targetPosition = gBattleMTargetPosition5AAA40;
            }
        }
        flags = gBattleMResultFlags5A4A20;
        const bool sameSide = (sourcePosition == 20 && targetPosition < 10)
            || (sourcePosition == 21 && targetPosition >= 10);
        const std::uint32_t response = (flags & 0x40000u) != 0 ? 0x40000u
            : (flags & 0x20000u) != 0 ? 0x20000u : (flags & 0x10000u);
        if (sameSide) {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[targetPosition], sourcePosition, flags, amount);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 184);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 74);
            targetPosition = gBattleMTargetPosition5AAA40;
        }
        if (response == 0x40000u) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 126);
            continue;
        }
        if (response == 0x10000u) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 124);
            continue;
        }
        if (response == 0x20000u) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 125);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 159);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 32);
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->value1030 = amount;
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->numberDisplayType1034 = 6;
            if ((gBattleMResultFlags5A4A20 & 0x10000000u) != 0) {
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                    ->numberDisplayType1034 = 0;
            }
            if ((*reinterpret_cast<const unsigned char*>(&gBattleMResultFlags5A4A20) & 0x20u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 47);
            }
            continue;
        }
        if (!sameSide) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 160);
        }
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->value1030 = amount;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->numberDisplayType1034 = 5;
        if ((gBattleMResultFlags5A4A20 & 0x10000000u) != 0) {
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->numberDisplayType1034 = 0;
        }
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    if (gBattleMMessageValue59FA14 != 0) {
        const std::int32_t lastSkillPosition = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const lastSkillActor = cg::world::gBattleAllEntries59E980[lastSkillPosition];
        const std::int32_t lastSkill = gBattleMSkill5AAA78;
        lastSkillActor->payload.battleActor->castSkill103C = lastSkill;
        const std::int32_t lastLevelPosition = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const lastLevelActor = cg::world::gBattleAllEntries59E980[lastLevelPosition];
        const std::int32_t lastLevel = gBattleMSkillLevelIndex5A3A18;
        lastLevelActor->payload.battleActor->castSkillLevel1044 = lastLevel;
        gBattleMFlags5ABA84 &= ~0x10000u;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 1);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 140);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 0);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 149);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 160);
        const std::int32_t valueCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const valueCaster = cg::world::gBattleAllEntries59E980[valueCasterPosition];
        const std::int32_t casterAmount = gBattleMMessageValue59FA14;
        valueCaster->payload.battleActor->value1030 = casterAmount;
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->numberDisplayType1034 = 5;
        const std::int32_t accumulatedAmount = gBattleMMessageValue59FA14;
        const std::uint32_t accumulatedFlags = gBattleMFlags5ABA84;
        const std::int32_t accumulatedCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t accumulatedTargetPosition = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const accumulatedTarget = cg::world::gBattleAllEntries59E980[accumulatedTargetPosition];
        AppendBattleEffectList_4393A0(
            accumulatedTarget, accumulatedCasterPosition, accumulatedFlags, accumulatedAmount);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 74);
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    payload->packetSubState58 = 2;
}

// 原42BEAC..42C053，SKL技能50/51/52/53/88；结果255只跳过自身效果。
[[maybe_unused]] void ProcessBattleMSkill50Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[position];
    const bool casterUnavailable = caster == nullptr;
    gBattleMCasterPosition5AAA64 = position;
    if (casterUnavailable) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
    const std::int32_t skillCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const skillCaster = cg::world::gBattleAllEntries59E980[skillCasterPosition];
    const std::int32_t skill = gBattleMSkill5AAA78;
    skillCaster->payload.battleActor->castSkill103C = skill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = skillLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    const std::uint32_t flags = gBattleMResultFlags5A4A20;
    if (flags != 255u) {
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectCasterPosition, flags, 0);
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    const bool useLevelOffset = level < 12;
    const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
    const std::int32_t image = useLevelOffset ? kBattleMSkillEffectOffsets553E58[level] + 110056 : 111018;
    cg::world::gBattleAllEntries59E980[imageCasterPosition]->payload.battleActor->image1028 = image;
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原42C053..42C241，SKL技能12；数值必须在施法者判空之前消费。
[[maybe_unused]] void ProcessBattleMSkill12(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t amount = TakeBattleMInteger_438F70(scene);
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const skillPayload = caster->payload.battleActor;
    skillPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = skillLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    const bool useLevelOffset = level < 12;
    const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
    const std::int32_t image = useLevelOffset ? kBattleMSkillEffectOffsets553E58[level] + 110180 : 111017;
    cg::world::gBattleAllEntries59E980[imageCasterPosition]->payload.battleActor->image1028 = image;
    const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
    AppendBattleEffectList_4393A0(effectCaster, effectCasterPosition, 0, amount);
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->soundId1018 = 256;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 160);
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->value1030 = amount;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->numberDisplayType1034 = 5;
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原82/97两段目标循环的161条指令一致，共用42CB2F..42CB5E收尾。
static void ProcessBattleMSkill82And97Targets(YobiAction* const scene, BattleScenePayload* const payload)
{
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        if ((gBattleMResultFlags5A4A20 & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        const std::int32_t effectImage = firstImage;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const BattleEntryEffectRecord effectRecord {effectTargetPosition, effectFlags, amount, effectImage};
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, effectRecord);
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxiliaryImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxiliaryTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t auxiliaryCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const auxiliaryCaster = cg::world::gBattleAllEntries59E980[auxiliaryCasterPosition];
        AppendBattleAuxEffects_43BC10(auxiliaryCaster, auxiliaryTargetPosition, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, auxiliaryImage);
        const std::int32_t targetSkillPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const targetSkillActor = cg::world::gBattleAllEntries59E980[targetSkillPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        targetSkillActor->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t targetLevelPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const targetLevelActor = cg::world::gBattleAllEntries59E980[targetLevelPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        targetLevelActor->payload.battleActor->castSkillLevel1044 = targetLevel;
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x108u) == 0) {
            if ((flags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                const std::int32_t playerTargetPosition = gBattleMTargetPosition5AAA40;
                if (cg::world::gBattleCurrentPosition5ABCA4 == playerTargetPosition) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
                flags = gBattleMResultFlags5A4A20;
            } else if (amount != 0 && (flags & 3u) != 0) {
                const std::int32_t reactionKind = (flags & 2u) != 0 ? 165 : 27;
                const std::int32_t reactionTargetPosition = gBattleMTargetPosition5AAA40;
                YobiAction* const reactionTarget = cg::world::gBattleAllEntries59E980[reactionTargetPosition];
                AppendBattleCast_4392A0(scene, reactionTarget, reactionKind);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if (amount != 0 || (flags & 0x108u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & 0x20u) != 0 && (flags & 0x10000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原42C241..42C50C，技能82按对侧两位置中点移动，再进入目标循环。
[[maybe_unused]] void ProcessBattleMSkill82(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[casterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const skillPayload = caster->payload.battleActor;
    skillPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = skillLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 8);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 6);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
    const std::int32_t position = gBattleMCasterPosition5AAA64;
    const bool firstSide = position < 10;
    const std::int32_t basePosition = firstSide ? 10 : 5;
    const std::int32_t otherPosition = firstSide ? 15 : 0;
    const std::int32_t basePointIndexX = cg::world::gBattlePointIndices5536FC[basePosition];
    const std::int32_t otherPointIndexX = cg::world::gBattlePointIndices5536FC[otherPosition];
    const std::int32_t baseX = cg::world::gBattleGraphicPoints553758[basePointIndexX][2];
    YobiAction* const midpointXActor = cg::world::gBattleAllEntries59E980[position];
    const std::int32_t otherX = cg::world::gBattleGraphicPoints553758[otherPointIndexX][2];
    const std::int32_t deltaX = otherX - baseX;
    auto* const midpointXPayload = midpointXActor->payload.battleActor;
    midpointXPayload->moveTargetXFE8 = baseX + deltaX / 2;
    const std::int32_t basePointIndexY = cg::world::gBattlePointIndices5536FC[basePosition];
    const std::int32_t otherPointIndexY = cg::world::gBattlePointIndices5536FC[otherPosition];
    const std::int32_t baseY = cg::world::gBattleGraphicPoints553758[basePointIndexY][3];
    const std::int32_t otherY = cg::world::gBattleGraphicPoints553758[otherPointIndexY][3];
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->moveTargetYFEC = baseY + (otherY - baseY) / 2;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->soundId1018 = 288;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 85);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 36);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 91);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 86);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 37);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 89);
    const std::int32_t playerCasterPosition = gBattleMCasterPosition5AAA64;
    if (cg::world::gBattleCurrentPosition5ABCA4 == playerCasterPosition) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerCasterPosition], 256);
    } else {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerCasterPosition], 52);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
    }
    ProcessBattleMSkill82And97Targets(scene, payload);
}

// 原42C736..42C905，技能97的图号与动作准备独立于82。
[[maybe_unused]] void ProcessBattleMSkill97(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[casterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const skillPayload = caster->payload.battleActor;
    skillPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = skillLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    std::int32_t image;
    std::int32_t imageCasterPosition;
    if (level < 12) {
        const std::int32_t offset = kBattleMSkillEffectOffsets553E58[level];
        imageCasterPosition = gBattleMCasterPosition5AAA64;
        image = offset + 110052;
    } else {
        imageCasterPosition = gBattleMCasterPosition5AAA64;
        image = 111001;
    }
    cg::world::gBattleAllEntries59E980[imageCasterPosition]->payload.battleActor->image1028 = image;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->soundId1018 = 5;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 80);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 88);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 87);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 86);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 89);
    ProcessBattleMSkill82And97Targets(scene, payload);
}

// 原42CB5E..42CF66及432E99尾；技能93先发布替换角色，再消费其名称和属性。
[[maybe_unused]] void ProcessBattleMSkill93(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[casterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const skillPayload = caster->payload.battleActor;
    skillPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = skillLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->reactionImage1010 = 22338;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 67);
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        static_cast<void>(MarkDeath(cg::world::gBattleAllEntries59E980[targetPosition]));
        YobiAction* const created = NewBattleActor_438290(gBattleMTargetPosition5AAA40);
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40] = created;
        auto* const actorPayload = created->payload.battleActor;
        {
            char nameBytes[29]{};
            TakeBattleMString_438FE0(scene, nameBytes, 29);
            created->nameC4 = cg::text::FromGbkWide(nameBytes);
        }
        created->imageNo = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        created->field1D0 = TakeBattleMInteger_438F70(scene);
        {
            created->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            created->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            created->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            created->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        actorPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        actorPayload->stateFE0 = 0;
        created->fields288[3] = 0;
        created->fields288[2] = 0;
        created->fields288[1] = 0;
        created->fields288[0] = 0;
        if ((actorPayload->flagsFDC & BattleActorFlag_Earth) != 0) {
            created->fields288[0] = 1;
        }
        if ((actorPayload->flagsFDC & BattleActorFlag_Water) != 0) {
            created->fields288[1] = 1;
        }
        if ((actorPayload->flagsFDC & BattleActorFlag_Fire) != 0) {
            created->fields288[2] = 1;
        }
        if ((actorPayload->flagsFDC & BattleActorFlag_Wind) != 0) {
            created->fields288[3] = 1;
        }
        actorPayload->castPlayIndexFC4 = 0;
        actorPayload->castListIndexFC8 = -1;
        actorPayload->castListCA4[0] = 0;
        actorPayload->castListCA4[1] = -1;
        created->offset40 =
            static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[gBattleMTargetPosition5AAA40]][2]);
        created->offset44 =
            static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[gBattleMTargetPosition5AAA40]][3]);
        created->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[gBattleMTargetPosition5AAA40]][2];
        created->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[gBattleMTargetPosition5AAA40]][3];
        created->scaleX = 0.0F;
        created->scaleY = 0.0F;
        actorPayload->battlePosition18 = gBattleMTargetPosition5AAA40;
        UpdateBattleAlbumFlag_4238F0(created);
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, 0, 0);
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor->soundId1018 = 204;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 80);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 54);
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原42CF66..42D0D0；成功后由完整父分派重读5A4A34，空施法者不进入模式生产。
static bool PrepareBattleMSkill98(YobiAction* const scene, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    gBattleMSkill98Mode5A4A34 = TakeBattleMInteger_438F70(scene);
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[casterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return false;
    }
    auto* const skillPayload = caster->payload.battleActor;
    skillPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = skillLevel;
    const std::int32_t modeCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const modeCaster = cg::world::gBattleAllEntries59E980[modeCasterPosition];
    const std::int32_t mode = gBattleMSkill98Mode5A4A34;
    modeCaster->payload.battleActor->skill98Mode1048 = mode;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 39);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    const bool useLevelOffset = level < 12;
    const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
    const std::int32_t image = useLevelOffset ? kBattleMSkillEffectOffsets553E58[level] + 110056 : 111018;
    cg::world::gBattleAllEntries59E980[imageCasterPosition]->payload.battleActor->image1028 = image;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    return true;
}

// 原42D696/42EAEF..42EB32，模式目标退出后仍追加哨兵、执行RID并递增固定载荷。
static void CompleteBattleMSkill98Targets(YobiAction* const scene, BattleScenePayload* const payload)
{
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原42D142..42D285，技能98模式6只消费目标位置列表。
static void ProcessBattleMSkill98Mode6(YobiAction* const scene, BattleScenePayload* const payload)
{
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, targetPosition, 0, 0);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t skill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = skill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = skillLevel;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor->soundId1018 = 288;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 80);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 83);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 68);
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->reactionImage1010 = 22404;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor->soundId101C = 168;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 81);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 56);
    }
    CompleteBattleMSkill98Targets(scene, payload);
}

// 原42D28E..42D68D，技能98模式7交换目标与配对槽，固定原槽和当前目标分别保留。
static void ProcessBattleMSkill98Mode7(YobiAction* const scene, BattleScenePayload* const payload)
{
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::int32_t otherPosition = cg::world::GetBattlePetPosition_405B40(targetPosition);
        const std::int32_t currentPosition = cg::world::gBattleCurrentPosition5ABCA4;
        const std::int32_t oldPosition = gBattleMTargetPosition5AAA40;
        const bool isCurrentPosition = currentPosition == oldPosition;
        YobiAction* const moving = cg::world::gBattleAllEntries59E980[oldPosition];
        YobiAction** const oldSlot = &cg::world::gBattleAllEntries59E980[oldPosition];
        if (isCurrentPosition) {
            cg::world::gBattleCurrentPosition5ABCA4 = otherPosition;
        }
        AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], otherPosition, 0, 0);
        const std::int32_t coordinateTargetPosition = gBattleMTargetPosition5AAA40;
        if (gBattleMCasterPosition5AAA64 == coordinateTargetPosition) {
            gBattleMCasterPosition5AAA64 = otherPosition;
        }
        const std::int32_t movingXIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
        auto* const movingXPayload =
            cg::world::gBattleAllEntries59E980[coordinateTargetPosition]->payload.battleActor;
        movingXPayload->x00 = cg::world::gBattleGraphicPoints553758[movingXIndex][2];
        const std::int32_t movingYTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t movingYIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
        auto* const movingYPayload =
            cg::world::gBattleAllEntries59E980[movingYTargetPosition]->payload.battleActor;
        movingYPayload->y04 = cg::world::gBattleGraphicPoints553758[movingYIndex][3];
        const std::int32_t movingX08TargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t movingX08Index = cg::world::gBattlePointIndices5536FC[otherPosition];
        auto* const movingX08Payload =
            cg::world::gBattleAllEntries59E980[movingX08TargetPosition]->payload.battleActor;
        movingX08Payload->x08 = cg::world::gBattleGraphicPoints553758[movingX08Index][0];
        const std::int32_t movingY0CTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t movingY0CIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
        auto* const movingY0CPayload =
            cg::world::gBattleAllEntries59E980[movingY0CTargetPosition]->payload.battleActor;
        movingY0CPayload->y0C = cg::world::gBattleGraphicPoints553758[movingY0CIndex][1];
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->battlePosition18 = otherPosition;
        YobiAction* const initialOther = cg::world::gBattleAllEntries59E980[otherPosition];
        if (initialOther != nullptr) {
            auto* const initialOtherPayload = initialOther->payload.battleActor;
            initialOtherPayload->battlePosition18 = gBattleMTargetPosition5AAA40;
            YobiAction* const other = cg::world::gBattleAllEntries59E980[otherPosition];
            if (other != nullptr && other->phase != 34) {
                const std::int32_t otherXTargetPosition = gBattleMTargetPosition5AAA40;
                auto* const otherXPayload = other->payload.battleActor;
                const std::int32_t otherXIndex = cg::world::gBattlePointIndices5536FC[otherXTargetPosition];
                otherXPayload->x00 = cg::world::gBattleGraphicPoints553758[otherXIndex][2];
                const std::int32_t otherYTargetPosition = gBattleMTargetPosition5AAA40;
                YobiAction* const otherYActor = cg::world::gBattleAllEntries59E980[otherPosition];
                const std::int32_t otherYIndex = cg::world::gBattlePointIndices5536FC[otherYTargetPosition];
                auto* const otherYPayload = otherYActor->payload.battleActor;
                otherYPayload->y04 = cg::world::gBattleGraphicPoints553758[otherYIndex][3];
                const std::int32_t otherX08TargetPosition = gBattleMTargetPosition5AAA40;
                YobiAction* const otherX08Actor = cg::world::gBattleAllEntries59E980[otherPosition];
                const std::int32_t otherX08Index = cg::world::gBattlePointIndices5536FC[otherX08TargetPosition];
                auto* const otherX08Payload = otherX08Actor->payload.battleActor;
                otherX08Payload->x08 = cg::world::gBattleGraphicPoints553758[otherX08Index][0];
                const std::int32_t otherY0CTargetPosition = gBattleMTargetPosition5AAA40;
                YobiAction* const otherY0CActor = cg::world::gBattleAllEntries59E980[otherPosition];
                const std::int32_t otherY0CIndex = cg::world::gBattlePointIndices5536FC[otherY0CTargetPosition];
                auto* const otherY0CPayload = otherY0CActor->payload.battleActor;
                otherY0CPayload->y0C = cg::world::gBattleGraphicPoints553758[otherY0CIndex][1];
            }
        }
        const std::int32_t sourcePosition = gBattleMTargetPosition5AAA40;
        YobiAction* const replacement = cg::world::gBattleAllEntries59E980[otherPosition];
        gBattleMTargetPosition5AAA40 = otherPosition;
        cg::world::gBattleAllEntries59E980[sourcePosition] = replacement;
        cg::world::gBattleAllEntries59E980[otherPosition] = moving;
        if (moving != nullptr) {
            YobiAction* const rider = moving->payload.battleActor->rider13C8;
            if (rider != nullptr) {
                rider->battleAttachmentParent10 = moving;
                const std::int32_t riderTargetPosition = gBattleMTargetPosition5AAA40;
                cg::world::gBattleAllEntries59E980[riderTargetPosition]->payload.battleActor
                    ->rider13C8->value1D4 = riderTargetPosition;
            }
        }
        YobiAction* const oldActor = *oldSlot;
        if (oldActor != nullptr) {
            YobiAction* const rider = oldActor->payload.battleActor->rider13C8;
            if (rider != nullptr) {
                rider->battleAttachmentParent10 = oldActor;
                (*oldSlot)->payload.battleActor->rider13C8->value1D4 = oldPosition;
            }
        }
        // 原42D4E5检查施法者槽，却重绑当前目标附件；不替换为目标判空。
        if (cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64] != nullptr) {
            YobiAction* const actor = cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40];
            YobiAction* const gear = actor->payload.battleActor->gear13B4;
            if (gear != nullptr) {
                gear->battleAttachmentParent10 = actor;
                const std::int32_t gearTargetPosition = gBattleMTargetPosition5AAA40;
                cg::world::gBattleAllEntries59E980[gearTargetPosition]->payload.battleActor
                    ->gear13B4->value1D4 = gearTargetPosition;
            }
            for (std::size_t index = 0; index < 2; ++index) {
                YobiAction* const target = cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40];
                YobiAction* const auxiliary = target->payload.battleActor->auxiliaryGear13B8[index];
                if (auxiliary != nullptr) {
                    auxiliary->battleAttachmentParent10 = target;
                    const std::int32_t auxiliaryTargetPosition = gBattleMTargetPosition5AAA40;
                    cg::world::gBattleAllEntries59E980[auxiliaryTargetPosition]->payload.battleActor
                        ->auxiliaryGear13B8[index]->value1D4 = auxiliaryTargetPosition;
                }
            }
        }
        YobiAction* const remaining = *oldSlot;
        if (remaining != nullptr) {
            YobiAction* const gear = remaining->payload.battleActor->gear13B4;
            if (gear != nullptr) {
                gear->battleAttachmentParent10 = remaining;
                (*oldSlot)->payload.battleActor->gear13B4->value1D4 = oldPosition;
            }
        }
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor->soundId1018 = 210;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 80);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->motionMagnitude54 = 3.0F;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->framePlayingRate = 100;
        YobiAction* const other = *oldSlot;
        if (other != nullptr && other->phase != 34) {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], oldPosition, 0, 0);
            AppendBattleCast_4392A0(scene, *oldSlot, 11);
            (*oldSlot)->motionMagnitude54 = 3.0F;
            (*oldSlot)->framePlayingRate = 100;
        }
    }
    CompleteBattleMSkill98Targets(scene, payload);
}

// 原42D6BC..42D7D1，技能98模式8的宠物标记和恢复队列读取次序。
static void ProcessBattleMSkill98Mode8(YobiAction* const scene, BattleScenePayload* const payload)
{
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, parsedTargetPosition, 0, 0);
        const std::int32_t flagsTargetPosition = gBattleMTargetPosition5AAA40;
        const auto* const flagsTargetPayload =
            cg::world::gBattleAllEntries59E980[flagsTargetPosition]->payload.battleActor;
        if ((reinterpret_cast<const unsigned char*>(&flagsTargetPayload->flagsFDC)[0] & BattleActorFlag_Dead) != 0) {
            ResetBattleCastList_4391E0(flagsTargetPosition);
        }
        const std::int32_t petPosition = cg::world::GetBattlePetPosition_405B40(cg::world::gBattleCurrentPosition5ABCA4);
        std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
        if (petPosition == targetPosition) {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[targetPosition], -2, 0, 0);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 223);
            targetPosition = gBattleMTargetPosition5AAA40;
        }
        cg::world::gBattleAllEntries59E980[targetPosition]->payload.battleActor->soundId1018 = 205;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 80);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 53);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
    }
    CompleteBattleMSkill98Targets(scene, payload);
}

// 原42DCDC与42EB0A尾：操作对象收哨兵/74，但推进的是主施法者。
static void CompleteBattleMSkill98SelectedTargets(YobiAction* const scene, BattleScenePayload* const payload)
{
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], -1, 0, 0);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 74);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原42D7DA..42DD12，SKL98模式12的操作对象准备与附图/受击目标循环。
static void ProcessBattleMSkill98Mode12(YobiAction* const scene, BattleScenePayload* const payload)
{
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    const std::int32_t selected = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const bool selectedUnavailable = selected == 255;
    gBattleMSelectedPosition5ABA98 = selected;
    if (selectedUnavailable) {
        return;
    }
    if (cg::world::gBattleAllEntries59E980[selected] == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->targetPosition100C = selected;
    const std::int32_t skillSelectedPosition = gBattleMSelectedPosition5ABA98;
    YobiAction* const skillSelected = cg::world::gBattleAllEntries59E980[skillSelectedPosition];
    const std::int32_t skill = gBattleMSkill5AAA78;
    skillSelected->payload.battleActor->castSkill103C = skill;
    const std::int32_t levelSelectedPosition = gBattleMSelectedPosition5ABA98;
    YobiAction* const levelSelected = cg::world::gBattleAllEntries59E980[levelSelectedPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelSelected->payload.battleActor->castSkillLevel1044 = skillLevel;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 151);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 8);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 6);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 10);
    const std::int32_t position = gBattleMSelectedPosition5ABA98;
    const std::int32_t basePosition = position < 10 ? 10 : 5;
    const std::int32_t otherPosition = position < 10 ? 15 : 0;
    const std::int32_t baseXIndex = cg::world::gBattlePointIndices5536FC[basePosition];
    const std::int32_t otherXIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
    const std::int32_t baseX = cg::world::gBattleGraphicPoints553758[baseXIndex][2];
    YobiAction* const midpointXActor = cg::world::gBattleAllEntries59E980[position];
    const std::int32_t otherX = cg::world::gBattleGraphicPoints553758[otherXIndex][2];
    const std::int32_t deltaX = otherX - baseX;
    auto* const midpointXPayload = midpointXActor->payload.battleActor;
    midpointXPayload->moveTargetXFE8 = baseX + deltaX / 2;
    const std::int32_t baseYIndex = cg::world::gBattlePointIndices5536FC[basePosition];
    const std::int32_t otherYIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
    const std::int32_t baseY = cg::world::gBattleGraphicPoints553758[baseYIndex][3];
    const std::int32_t otherY = cg::world::gBattleGraphicPoints553758[otherYIndex][3];
    const std::int32_t midpointY = baseY + (otherY - baseY) / 2;
    cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98]->payload.battleActor
        ->moveTargetYFEC = midpointY;
    cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98]->payload.battleActor->soundId1018 = 288;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 80);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 85);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 36);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 91);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 86);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 37);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 89);
    const std::int32_t playerSelectedPosition = gBattleMSelectedPosition5ABA98;
    if (cg::world::gBattleCurrentPosition5ABCA4 == playerSelectedPosition) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerSelectedPosition], 256);
    } else {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerSelectedPosition], 52);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 51);
    }
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        if ((gBattleMResultFlags5A4A20 & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        const std::int32_t effectImage = firstImage;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t effectSelectedPosition = gBattleMSelectedPosition5ABA98;
        YobiAction* const effectSelected = cg::world::gBattleAllEntries59E980[effectSelectedPosition];
        AppendBattleEffect_439400(effectSelected, {effectTargetPosition, effectFlags, amount, effectImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxiliaryImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxiliaryTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t auxiliarySelectedPosition = gBattleMSelectedPosition5ABA98;
        YobiAction* const auxiliarySelected = cg::world::gBattleAllEntries59E980[auxiliarySelectedPosition];
        AppendBattleAuxEffects_43BC10(auxiliarySelected, auxiliaryTargetPosition, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, auxiliaryImage);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetSkillLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetSkillLevel;
        const std::int32_t zeroTargetPosition = gBattleMTargetPosition5AAA40;
        if (gBattleMCasterPosition5AAA64 == zeroTargetPosition) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[zeroTargetPosition], 0);
        }
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x108u) == 0) {
            if ((flags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                const std::int32_t playerTargetPosition = gBattleMTargetPosition5AAA40;
                if (cg::world::gBattleCurrentPosition5ABCA4 == playerTargetPosition) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
                flags = gBattleMResultFlags5A4A20;
            } else if (amount != 0 && (flags & 3u) != 0) {
                const std::int32_t damageTargetPosition = gBattleMTargetPosition5AAA40;
                const bool critical = (flags & 2u) != 0;
                YobiAction* const damageTarget = cg::world::gBattleAllEntries59E980[damageTargetPosition];
                const std::int32_t kind = critical ? 165 : 27;
                AppendBattleCast_4392A0(scene, damageTarget, kind);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if (amount != 0 || (flags & 0x108u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & 0x20u) != 0 && (flags & 0x10000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    CompleteBattleMSkill98SelectedTargets(scene, payload);
}

// 原42DD1B..42E015，SKL98模式13的中央移动与直接数值反馈，不消费附图。
static void ProcessBattleMSkill98Mode13(YobiAction* const scene, BattleScenePayload* const payload)
{
    const std::int32_t selected = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const bool selectedUnavailable = selected == 255;
    gBattleMSelectedPosition5ABA98 = selected;
    if (selectedUnavailable) {
        return;
    }
    if (cg::world::gBattleAllEntries59E980[selected] == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->targetPosition100C = selected;
    const std::int32_t skillSelectedPosition = gBattleMSelectedPosition5ABA98;
    YobiAction* const skillSelected = cg::world::gBattleAllEntries59E980[skillSelectedPosition];
    const std::int32_t skill = gBattleMSkill5AAA78;
    skillSelected->payload.battleActor->castSkill103C = skill;
    const std::int32_t levelSelectedPosition = gBattleMSelectedPosition5ABA98;
    YobiAction* const levelSelected = cg::world::gBattleAllEntries59E980[levelSelectedPosition];
    const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
    levelSelected->payload.battleActor->castSkillLevel1044 = skillLevel;
    const std::int32_t x15Index = cg::world::gBattlePointIndices5536FC[15];
    const std::int32_t x5Index = cg::world::gBattlePointIndices5536FC[5];
    const std::int32_t x15 = cg::world::gBattleGraphicPoints553758[x15Index][2];
    const std::int32_t x5 = cg::world::gBattleGraphicPoints553758[x5Index][2];
    const std::int32_t midpointX = x15 + (x5 - x15) / 2;
    cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98]->payload.battleActor
        ->moveTargetXFE8 = midpointX;
    const std::int32_t y15Index = cg::world::gBattlePointIndices5536FC[15];
    const std::int32_t y5Index = cg::world::gBattlePointIndices5536FC[5];
    const std::int32_t y15 = cg::world::gBattleGraphicPoints553758[y15Index][3];
    const std::int32_t y5 = cg::world::gBattleGraphicPoints553758[y5Index][3];
    const std::int32_t midpointY = y15 + (y5 - y15) / 2;
    cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98]->payload.battleActor
        ->moveTargetYFEC = midpointY;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 8);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 6);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 10);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 64);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 137);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 90);
    const std::int32_t playerSelectedPosition = gBattleMSelectedPosition5ABA98;
    if (cg::world::gBattleCurrentPosition5ABCA4 == playerSelectedPosition) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerSelectedPosition], 256);
    } else {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerSelectedPosition], 52);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 51);
    }
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        const std::int32_t effectSelectedPosition = gBattleMSelectedPosition5ABA98;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        YobiAction* const effectSelected = cg::world::gBattleAllEntries59E980[effectSelectedPosition];
        AppendBattleEffect_439400(effectSelected, {effectTargetPosition, effectFlags, amount, 0});
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetSkillLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetSkillLevel;
        std::int32_t castTargetPosition = gBattleMTargetPosition5AAA40;
        if (gBattleMCasterPosition5AAA64 == castTargetPosition) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[castTargetPosition], 0);
            castTargetPosition = gBattleMTargetPosition5AAA40;
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[castTargetPosition], 149);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 160);
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->value1030 = amount;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor->numberDisplayType1034 = 5;
    }
    CompleteBattleMSkill98SelectedTargets(scene, payload);
}

// 原42E01E..42E1E4，SKL98模式14成对读取；第二位置255不撤销第一项。
static void ProcessBattleMSkill98Mode14(YobiAction* const scene, BattleScenePayload* const payload)
{
    const std::int32_t kind = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    for (;;) {
        const std::int32_t selectedPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool selectedPositionUnavailable = selectedPosition == 255;
        gBattleMSelectedPosition5ABA98 = selectedPosition;
        if (selectedPositionUnavailable) {
            break;
        }
        const std::int32_t firstAmount = TakeBattleMInteger_438F70(scene);
        const std::int32_t firstAmountPosition = gBattleMSelectedPosition5ABA98;
        const bool firstKindIsZero = kind == 0;
        cg::world::gBattleAllEntries59E980[firstAmountPosition]->payload.battleActor
            ->value1030 = firstAmount;
        auto* const firstKindPayload =
            cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98]->payload.battleActor;
        firstKindPayload->numberDisplayType1034 = firstKindIsZero ? 9 : 8;
        cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98]->payload.battleActor
            ->image102C = 111015;
        const std::int32_t firstEffectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t firstEffectTargetPosition = gBattleMSelectedPosition5ABA98;
        YobiAction* const firstEffectCaster = cg::world::gBattleAllEntries59E980[firstEffectCasterPosition];
        AppendBattleEffect_439400(firstEffectCaster, {firstEffectTargetPosition, 0, 0, 0});
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetPositionUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetPositionUnavailable) {
            break;
        }
        const std::int32_t secondAmount = TakeBattleMInteger_438F70(scene);
        const std::int32_t secondAmountPosition = gBattleMTargetPosition5AAA40;
        const bool secondKindIsZero = kind == 0;
        cg::world::gBattleAllEntries59E980[secondAmountPosition]->payload.battleActor
            ->value1030 = secondAmount;
        auto* const secondKindPayload =
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor;
        secondKindPayload->numberDisplayType1034 = secondKindIsZero ? 9 : 8;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->image102C = 111015;
        const std::int32_t secondEffectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t secondEffectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const secondEffectCaster = cg::world::gBattleAllEntries59E980[secondEffectCasterPosition];
        AppendBattleEffect_439400(secondEffectCaster, {secondEffectTargetPosition, 0, 0, 0});
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 250);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 160);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 250);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMSelectedPosition5ABA98], 160);
    }
    CompleteBattleMSkill98Targets(scene, payload);
}

// 原42E1ED/42E444起的15/16两模式；固定数值类型分别为6/3，其他消费和反馈相同。
static void ProcessBattleMSkill98DamageTargets(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t numberType)
{
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        if ((gBattleMResultFlags5A4A20 & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        const std::int32_t effectImage = firstImage;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, {effectTargetPosition, effectFlags, amount, effectImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxiliaryImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxiliaryTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t auxiliaryCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const auxiliaryCaster = cg::world::gBattleAllEntries59E980[auxiliaryCasterPosition];
        AppendBattleAuxEffects_43BC10(auxiliaryCaster, auxiliaryTargetPosition, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, auxiliaryImage);
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->image102C = 111011;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 250);
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->value1030 = amount;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->numberDisplayType1034 = numberType;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 159);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x40u) != 0 && (flags & 0x10000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
            const std::int32_t playerTargetPosition = gBattleMTargetPosition5AAA40;
            if (cg::world::gBattleCurrentPosition5ABCA4 == playerTargetPosition) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], 256);
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], 52);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
            }
            if (amount == 0) {
                flags = gBattleMResultFlags5A4A20;
            }
        } else if (amount != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 165);
        }
        if (amount != 0 || (flags & 0x108u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 47);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    CompleteBattleMSkill98Targets(scene, payload);
}

// 原42E6A7..42E80D，模式17无附图时只清首图号，保留其余跨目标输出。
static void ProcessBattleMSkill98Mode17(YobiAction* const scene, BattleScenePayload* const payload)
{
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        std::uint32_t flags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        gBattleMResultFlags5A4A20 = flags;
        if ((flags & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
            flags = gBattleMResultFlags5A4A20;
        } else {
            firstImage = 0;
        }
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxiliaryImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::int32_t auxiliaryCasterPosition = gBattleMCasterPosition5AAA64;
        const std::uint32_t auxiliaryFlags = flags;
        const std::int32_t auxiliaryTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const auxiliaryCaster = cg::world::gBattleAllEntries59E980[auxiliaryCasterPosition];
        AppendBattleAuxEffects_43BC10(auxiliaryCaster, auxiliaryTargetPosition, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, auxiliaryImage);
        const std::int32_t effectImage = firstImage;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, {effectTargetPosition, effectFlags, 0, effectImage});
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->image102C = 111015;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 250);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 251);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 58);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 36);
    }
    CompleteBattleMSkill98Targets(scene, payload);
}

// 原42E80D..42EB32，进入目标族后仍逐目标重读模式，不固定为首次分派值。
static void ProcessBattleMSkill98StatusTargets(YobiAction* const scene, BattleScenePayload* const payload)
{
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        gBattleMResultFlags5A4A20 = resultFlags;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, resultFlags, 0);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t skill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = skill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t skillLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = skillLevel;
        const std::int32_t mode = gBattleMSkill98Mode5A4A34;
        if (mode >= 0 && mode <= 5) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 119);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
                const std::int32_t reapplyTargetPosition = gBattleMTargetPosition5AAA40;
                YobiAction* const reapplyTarget = cg::world::gBattleAllEntries59E980[reapplyTargetPosition];
                const std::int32_t reapplySkill = gBattleMSkill5AAA78;
                reapplyTarget->payload.battleActor->castSkill103C = reapplySkill;
                const std::int32_t modeTargetPosition = gBattleMTargetPosition5AAA40;
                YobiAction* const modeTarget = cg::world::gBattleAllEntries59E980[modeTargetPosition];
                const std::int32_t targetMode = gBattleMSkill98Mode5A4A34;
                modeTarget->payload.battleActor->skill98Mode1048 = targetMode;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 141);
            }
        } else if (mode == 9) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 134);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 47);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 45);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
            }
        } else if (mode == 10) {
            const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
            const bool sameSide = (casterPosition == 20 && targetPosition < 10)
                || (casterPosition == 21 && targetPosition >= 10);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], sameSide ? 183 : 133);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 142);
            }
        } else if (mode == 11) {
            const std::int32_t amount = TakeBattleMInteger_438F70(scene);
            ResetBattleCastList_4391E0(gBattleMTargetPosition5AAA40);
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->phase = 0;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 148);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->motionMagnitude54 = 8.0F;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 79);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 160);
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                    ->value1030 = amount;
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                    ->numberDisplayType1034 = 5;
            }
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 40);
        }
    }
    CompleteBattleMSkill98Targets(scene, payload);
}

// SKL技能98的状态0内部分派；由ProcessBattleMSkillHeader调用，427470总入口统一回存。
[[maybe_unused]] void ProcessBattleMSkill98(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    if (!PrepareBattleMSkill98(scene, fpUsed)) {
        return;
    }
    switch (gBattleMSkill98Mode5A4A34) {
    case 0: case 1: case 2: case 3: case 4: case 5:
    case 9: case 10: case 11: case 18:
        ProcessBattleMSkill98StatusTargets(scene, payload);
        break;
    case 6:
        ProcessBattleMSkill98Mode6(scene, payload);
        break;
    case 7:
        ProcessBattleMSkill98Mode7(scene, payload);
        break;
    case 8:
        ProcessBattleMSkill98Mode8(scene, payload);
        break;
    case 12:
        ProcessBattleMSkill98Mode12(scene, payload);
        break;
    case 13:
        ProcessBattleMSkill98Mode13(scene, payload);
        break;
    case 14:
        ProcessBattleMSkill98Mode14(scene, payload);
        break;
    case 15:
        ProcessBattleMSkill98DamageTargets(scene, payload, 6);
        break;
    case 16:
        ProcessBattleMSkill98DamageTargets(scene, payload, 3);
        break;
    case 17:
        ProcessBattleMSkill98Mode17(scene, payload);
        break;
    default:
        MoveBattleCastState_439380(world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        ParseBattleMRide_4239D0(scene);
        ++payload->packetSubState58;
        break;
    }
}

// 原42EB32..42EC95，SKL107只消费目标结果，带效果哨兵而不调用RID。
[[maybe_unused]] void ProcessBattleMSkill107(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[casterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 104);
    for (;;) {
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        gBattleMResultFlags5A4A20 = resultFlags;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, resultFlags, 0);
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原42EC95..42F035与436DE2尾；SKL108先读数值再读结果，特殊同侧不追加159。
[[maybe_unused]] void ProcessBattleMSkill108(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMResultFlags5A4A20 = resultFlags;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        const std::int32_t comparisonTargetPosition = gBattleMTargetPosition5AAA40;
        if (gBattleMCasterPosition5AAA64 != comparisonTargetPosition) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 0);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 7);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 9);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 105);
        if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
            const std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
            const std::int32_t sourceKind = sourcePosition == gBattleMTargetPosition5AAA40 ? 106 : 57;
            YobiAction* const source = cg::world::gBattleAllEntries59E980[sourcePosition];
            AppendBattleCast_4392A0(scene, source, sourceKind);
            const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
            const bool sameSide = (casterPosition == 20 && targetPosition < 10)
                || (casterPosition == 21 && targetPosition >= 10);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], sameSide ? 184 : 149);
            const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
            if (level < 12) {
                const std::int32_t imageTargetPosition = gBattleMTargetPosition5AAA40;
                const std::int32_t image = kBattleMSkillEffectOffsets553E58[level] + 110081;
                YobiAction* const imageTarget = cg::world::gBattleAllEntries59E980[imageTargetPosition];
                imageTarget->payload.battleActor->image1028 = image;
            } else {
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                    ->image1028 = 111014;
            }
            if (!sameSide) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 159);
            }
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->value1030 = amount;
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->numberDisplayType1034 = 5;
        } else {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->reactionImage1010 = 22331;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 59);
        }
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, effectFlags, amount);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原42F035..42F3F2及436E03尾；SKL109只读结果，200000分支与失败图号目标不同。
[[maybe_unused]] void ProcessBattleMSkill109(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMResultFlags5A4A20 = resultFlags;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        const std::int32_t comparisonTargetPosition = gBattleMTargetPosition5AAA40;
        if (gBattleMCasterPosition5AAA64 != comparisonTargetPosition) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 0);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 7);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 9);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 105);
        const std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x80000u) != 0) {
            const std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
            const std::int32_t sourceKind = sourcePosition == gBattleMTargetPosition5AAA40 ? 106 : 57;
            YobiAction* const source = cg::world::gBattleAllEntries59E980[sourcePosition];
            AppendBattleCast_4392A0(scene, source, sourceKind);
            const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
            const bool sameSide = (casterPosition == 20 && targetPosition < 10)
                || (casterPosition == 21 && targetPosition >= 10);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], sameSide ? 183 : 133);
        } else if ((flags & 0x200000u) != 0) {
            if ((flags & 2u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 119);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 68);
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                    ->reactionImage1010 = 22329;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 50);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 59);
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 68);
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                    ->reactionImage1010 = 22329;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 32);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 59);
            }
        } else {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 68);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->reactionImage1010 = 22331;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 59);
        }
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, effectFlags, 0);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原42F3F2..42F739与42998C尾；SKL112的2000反馈作用于施法者，其余默认反馈作用于目标。
[[maybe_unused]] void ProcessBattleMSkill112(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 161);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 75);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 109);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 110);
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        const std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x80000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
        } else if ((flags & 0x2000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 121);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 111);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 37);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 89);
            const std::int32_t switchCasterPosition = gBattleMCasterPosition5AAA64;
            if (cg::world::gBattleCurrentPosition5ABCA4 == switchCasterPosition) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[switchCasterPosition], 256);
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[switchCasterPosition], 52);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
            }
        } else if ((flags & 0x4000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 122);
        } else if ((flags & 0x8000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 123);
        } else {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 111);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 89);
            const std::int32_t switchTargetPosition = gBattleMTargetPosition5AAA40;
            if (cg::world::gBattleCurrentPosition5ABCA4 == switchTargetPosition) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[switchTargetPosition], 256);
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[switchTargetPosition], 52);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
            }
        }
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, effectFlags, 0);
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原42F739..42F9BF与436FA1尾；SKL114在157调用后确认结果，再发布两项状态字段。
[[maybe_unused]] void ProcessBattleMSkill114(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    if (level < 12) {
        const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t image = imageOffset + 110056;
        YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
        imageCaster->payload.battleActor->image1028 = image;
    } else {
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image1028 = 111018;
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMResultFlags5A4A20 = resultFlags;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, resultFlags, 0);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 157);
        if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
            auto* const targetPayload =
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor;
            targetPayload->statusEffectType13DC = 7;
            targetPayload->statusEffectMode13D8 = 1;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 143);
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->soundId1018 = 253;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 80);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
        }
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原42F9BF..42FBF4与429999尾；SKL117先追加效果，目标反馈后再排施法者73/74。
[[maybe_unused]] void ProcessBattleMSkill117(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMResultFlags5A4A20 = resultFlags;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, resultFlags, 0);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 56);
        } else {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 68);
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->reactionImage1010 = 22331;
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 73);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原42FC39..42FDCF，SKL263直接读取施法位置，目标-1与非-1的生产结果相同。
[[maybe_unused]] void ProcessBattleMSkill263(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    gBattleMTargetPosition5AAA40 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t image = TakeBattleMInteger_438F70(scene);
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
    const std::int32_t effectImage = image;
    const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
    AppendBattleEffect_439400(effectCaster, {effectTargetPosition, 0, 0, effectImage});
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 218);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    payload->packetSubState58 = 1;
}

// 原42FDCF..430162，SKL400保留异位置两次动作0及正常255/空目标的不同出口。
[[maybe_unused]] void ProcessBattleMSkill400(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    const std::int32_t initialEffectPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const initialEffectCaster = cg::world::gBattleAllEntries59E980[initialEffectPosition];
    AppendBattleEffect_439400(initialEffectCaster, {initialEffectPosition, 0, 400, 0});
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 246);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 216);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    const std::int32_t firstLevel = gBattleMSkillLevelIndex5A3A18;
    if (firstLevel < 12) {
        const std::int32_t firstImageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t firstImageOffset = kBattleMSkillEffectOffsets553E58[firstLevel];
        const std::int32_t firstImage = firstImageOffset + 110052;
        YobiAction* const firstImageCaster = cg::world::gBattleAllEntries59E980[firstImageCasterPosition];
        firstImageCaster->payload.battleActor->image1028 = firstImage;
    } else {
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image1028 = 111001;
    }
    const std::int32_t secondLevel = gBattleMSkillLevelIndex5A3A18;
    if (secondLevel < 12) {
        const std::int32_t secondImageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t secondImageOffset = kBattleMSkillEffectOffsets553E58[secondLevel];
        const std::int32_t secondImage = secondImageOffset + 110044;
        YobiAction* const secondImageCaster = cg::world::gBattleAllEntries59E980[secondImageCasterPosition];
        secondImageCaster->payload.battleActor->image102C = secondImage;
    } else {
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image102C = 111011;
    }
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], {255, 0, 0, 0});
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMResultFlags5A4A20 = resultFlags;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, {effectTargetPosition, resultFlags, 0, 0});
        std::int32_t zeroTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t zeroCasterPosition = gBattleMCasterPosition5AAA64;
        if (zeroTargetPosition != zeroCasterPosition) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[zeroTargetPosition], 0);
            zeroTargetPosition = gBattleMTargetPosition5AAA40;
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[zeroTargetPosition], 0);
        std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
        if (targetPosition != gBattleMCasterPosition5AAA64) {
            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[targetPosition], {targetPosition, 0, 400, 0});
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 246);
            targetPosition = gBattleMTargetPosition5AAA40;
        }
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 8u) == 0 && (flags & 0x10000100u) == 0) {
            if ((flags & 0x80000u) != 0) {
                if (cg::world::gBattleCurrentPosition5ABCA4 == targetPosition) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 11);
            }
            targetPosition = gBattleMTargetPosition5AAA40;
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & 0x108u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 11);
            if ((gBattleMResultFlags5A4A20 & 0x100u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
            }
        }
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    payload->packetSubState58 = 2;
}

// 原430162..430463，SKL401正常255才追加结束效果，两种目标出口均执行完整211尾。
[[maybe_unused]] void ProcessBattleMSkill401(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 216);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    if (level < 12) {
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
        const std::int32_t image = imageOffset + 110081;
        YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
        imageCaster->payload.battleActor->image1028 = image;
    } else {
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image1028 = 111014;
    }
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], {255, 0, 0, 0});
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, {effectTargetPosition, effectFlags, 0, 0});
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 212);
            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], {110074, 30, 0, 23});
        }
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 211);
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 0, 1, 3);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 37);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 211);
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 0, 1, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    payload->packetSubState58 = 2;
}

// 原430463..43062E，SKL402先消费结果/位置/侧别，再遍历选定一侧的现有角色。
[[maybe_unused]] void ProcessBattleMSkill402(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedSide = TakeBattleMInteger_438F70(scene);
    const std::int32_t initialCasterPosition = gBattleMCasterPosition5AAA64;
    const std::int32_t side = parsedSide;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[initialCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    if (level < 12) {
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
        const std::int32_t image = imageOffset + 110052;
        YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
        imageCaster->payload.battleActor->image1028 = image;
    } else {
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image1028 = 111001;
    }
    std::int32_t position = side == 0 ? 0 : 10;
    const std::int32_t end = position + 10;
    gBattleMTargetPosition5AAA40 = position;
    bool hasTarget = position < end;
    while (hasTarget) {
        if (cg::world::gBattleAllEntries59E980[position] != nullptr) {
            const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t effectTargetPosition = position;
            YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
            AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, 0, 0);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 217);
            // 原4305F7在调用后重取全局位置，再执行共同的递增/发布。
            position = gBattleMTargetPosition5AAA40;
        }
        ++position;
        hasTarget = position < end;
        gBattleMTargetPosition5AAA40 = position;
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    payload->packetSubState58 = 2;
}

// 原43062E..43097A，SKL405每目标九字段/四图号转换，全部消费后才追加五条效果。
[[maybe_unused]] void ProcessBattleMSkill405(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    const std::int32_t initialValue = TakeBattleMInteger_438F70(scene);
    gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(initialValue);
    const std::int32_t initialEffectImage = initialValue;
    const std::int32_t initialEffectPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const initialEffectCaster = cg::world::gBattleAllEntries59E980[initialEffectPosition];
    AppendBattleEffect_439400(initialEffectCaster, {initialEffectPosition, 0, 0, initialEffectImage});
    const std::int32_t firstLevel = gBattleMSkillLevelIndex5A3A18;
    if (firstLevel < 12) {
        const std::int32_t firstImageOffset = kBattleMSkillEffectOffsets553E58[firstLevel];
        const std::int32_t firstImageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t firstEffectImage = firstImageOffset + 110052;
        YobiAction* const firstImageCaster = cg::world::gBattleAllEntries59E980[firstImageCasterPosition];
        firstImageCaster->payload.battleActor->image1028 = firstEffectImage;
    } else {
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image1028 = 111001;
    }
    const std::int32_t secondLevel = gBattleMSkillLevelIndex5A3A18;
    if (secondLevel < 12) {
        const std::int32_t secondImageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t secondImageOffset = kBattleMSkillEffectOffsets553E58[secondLevel];
        const std::int32_t secondEffectImage = secondImageOffset + 110044;
        YobiAction* const secondImageCaster = cg::world::gBattleAllEntries59E980[secondImageCasterPosition];
        secondImageCaster->payload.battleActor->image102C = secondEffectImage;
    } else {
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image102C = 111011;
    }
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        const std::int32_t firstImage = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        const std::int32_t secondImage = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        std::array<std::int32_t, 2> images;
        for (std::int32_t& image : images) {
            image = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        }
        const std::int32_t firstValue = TakeBattleMInteger_438F70(scene);
        const std::int32_t secondValue = TakeBattleMInteger_438F70(scene);
        const std::int32_t thirdValue = TakeBattleMInteger_438F70(scene);
        const std::int32_t parsedFourthValue = TakeBattleMInteger_438F70(scene);
        const std::int32_t primaryEffectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t primaryEffectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t fourthValue = parsedFourthValue;
        const std::int32_t primaryEffectImage = firstImage;
        YobiAction* const primaryEffectCaster = cg::world::gBattleAllEntries59E980[primaryEffectCasterPosition];
        AppendBattleEffect_439400(primaryEffectCaster, {primaryEffectTargetPosition, 0, amount, primaryEffectImage});
        const std::int32_t secondaryEffectImage = secondImage;
        const std::int32_t secondaryEffectValue = firstImage;
        const std::int32_t secondaryEffectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t secondaryEffectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const secondaryEffectCaster = cg::world::gBattleAllEntries59E980[secondaryEffectCasterPosition];
        AppendBattleEffect_439400(secondaryEffectCaster,
            {secondaryEffectTargetPosition, 0, secondaryEffectValue, secondaryEffectImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (images[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t arrayEffectValue = images[imageIndex];
        const std::int32_t arrayEffectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t arrayEffectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const arrayEffectCaster = cg::world::gBattleAllEntries59E980[arrayEffectCasterPosition];
        AppendBattleEffect_439400(arrayEffectCaster, {arrayEffectTargetPosition, 0, arrayEffectValue, 0});
        const std::int32_t pairEffectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t pairEffectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t pairEffectImage = secondValue;
        const std::int32_t pairEffectValue = firstValue;
        YobiAction* const pairEffectCaster = cg::world::gBattleAllEntries59E980[pairEffectCasterPosition];
        AppendBattleEffect_439400(pairEffectCaster, {pairEffectTargetPosition, 0, pairEffectValue, pairEffectImage});
        const std::int32_t lastEffectImage = fourthValue;
        const std::int32_t lastEffectValue = thirdValue;
        const std::int32_t lastEffectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t lastEffectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const lastEffectCaster = cg::world::gBattleAllEntries59E980[lastEffectCasterPosition];
        AppendBattleEffect_439400(lastEffectCaster, {lastEffectTargetPosition, 0, lastEffectValue, lastEffectImage});
    }
    AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], {-1, 0, 0, 0});
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    payload->packetSubState58 = 1;
}

// 原43097A..431C9C；404/406共用双角色生产，预读游标不推进正式消息游标。
[[maybe_unused]] void ProcessBattleMSkill404Family(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    casterPayload->castSkill103C = gBattleMSkill5AAA78;
    casterPayload->castSkillLevel1044 = gBattleMSkillLevelIndex5A3A18;
    const bool singlePosition = gBattleMSkill5AAA78 == 406;
    std::int32_t previewOffset = payload->packetOffset64;
    std::int32_t previewPosition;
    std::int32_t previewCasterPosition;
    std::array<char, 256> previewName;
    for (;;) {
        previewPosition = GetBattlePosition_4237E0(TakeBattleMIntegerAt_43B940(&previewOffset));
        previewCasterPosition = gBattleMCasterPosition5AAA64;
        if (previewPosition == 255 || previewPosition != previewCasterPosition) {
            break;
        }
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        TakeBattleMStringAt_43B8C0(&previewOffset, previewName.data(), 29);
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        for (std::int32_t index = 0; index < 2; ++index) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        }
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        TakeBattleMStringAt_43B8C0(&previewOffset, previewName.data(), 29);
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
    }
    YobiAction* const previewCaster = cg::world::gBattleAllEntries59E980[previewCasterPosition];
    AppendBattleEffectList_4393A0(previewCaster, previewPosition, 0, 0);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    if (previewPosition < 20) {
        const std::int32_t pointPosition = singlePosition
            ? (previewPosition < 5 || (previewPosition >= 10 && previewPosition < 15)
                ? previewPosition + 5 : previewPosition)
            : (previewPosition < 5 ? 4 : previewPosition < 10 ? 9 : previewPosition < 15 ? 13 : 18);
        const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[pointPosition];
        const std::int32_t pointY = cg::world::gBattleGraphicPoints553758[pointIndex][3];
        const std::int32_t pointX = cg::world::gBattleGraphicPoints553758[pointIndex][2];
        const bool sampleCasterBeforeOffset = previewPosition < 5
            || (previewPosition >= 10 && previewPosition < 15);
        std::int32_t effectCasterPosition;
        std::int32_t x;
        std::int32_t y;
        if (sampleCasterBeforeOffset) {
            effectCasterPosition = gBattleMCasterPosition5AAA64;
            y = pointY + (singlePosition ? (previewPosition < 10 ? -110 : 110) : (previewPosition < 10 ? 35 : -35));
            x = pointX + (singlePosition ? (previewPosition < 10 ? -121 : 121) : (previewPosition < 10 ? -155 : 155));
        } else {
            y = pointY + (singlePosition ? (previewPosition < 10 ? -110 : 110) : (previewPosition < 10 ? 35 : -35));
            x = pointX + (singlePosition ? (previewPosition < 10 ? -121 : 121) : (previewPosition < 10 ? -155 : 155));
            effectCasterPosition = gBattleMCasterPosition5AAA64;
        }
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, x, static_cast<std::uint32_t>(y), 0);
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 207);
    if (previewPosition < 20) {
        const std::int32_t pointPosition = singlePosition
            ? (previewPosition < 5 || (previewPosition >= 10 && previewPosition < 15)
                ? previewPosition + 5 : previewPosition)
            : (previewPosition < 5 ? 4 : previewPosition < 10 ? 9 : previewPosition < 15 ? 13 : 18);
        const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[pointPosition];
        const bool sampleCasterBeforePoint = previewPosition < 5
            || (previewPosition >= 10 && previewPosition < 15);
        std::int32_t effectCasterPosition = 0; // 互补取样分支均在使用前覆盖。
        if (sampleCasterBeforePoint) {
            effectCasterPosition = gBattleMCasterPosition5AAA64;
        }
        const std::int32_t pointY = cg::world::gBattleGraphicPoints553758[pointIndex][3];
        const std::int32_t pointX = cg::world::gBattleGraphicPoints553758[pointIndex][2];
        std::int32_t x;
        std::int32_t y;
        if (singlePosition) {
            y = pointY + (previewPosition < 10 ? -50 : 50);
            x = pointX + (previewPosition < 10 ? -55 : 55);
            if (!sampleCasterBeforePoint) {
                effectCasterPosition = gBattleMCasterPosition5AAA64;
            }
        } else {
            if (!sampleCasterBeforePoint) {
                effectCasterPosition = gBattleMCasterPosition5AAA64;
            }
            y = pointY + (previewPosition < 10 ? 35 : -35);
            x = pointX + (previewPosition < 10 ? -55 : 55);
        }
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, x, static_cast<std::uint32_t>(y), 0);
        casterPayload->targetPosition100C = singlePosition ? pointPosition
            : (previewPosition < 5 ? 3 : previewPosition < 10 ? 8 : previewPosition < 15 ? 13 : 18);
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 230);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 41);
    std::int32_t firstPosition = 0;
    YobiAction* const first = NewBattleActor_438290(0);
    auto* const firstPayload = first->payload.battleActor;
    std::int32_t secondPosition = 0;
    firstPayload->stateFE0 = 0;
    first->fields288[3] = 0;
    first->fields288[2] = 0;
    first->fields288[1] = 0;
    first->fields288[0] = 0;
    firstPayload->castPlayIndexFC4 = 0;
    firstPayload->castListIndexFC8 = -1;
    firstPayload->castListCA4[0] = 0;
    firstPayload->castListCA4[1] = -1;
    first->scaleX = 1.0F;
    first->scaleY = 1.0F;
    firstPayload->battlePosition18 = 0;
    first->phase = 0;
    first->flags |= 2u;
    first->callback = nullptr;
    YobiAction* const second = NewBattleActor_438290(0);
    auto* const secondPayload = second->payload.battleActor;
    secondPayload->stateFE0 = 0;
    second->fields288[3] = 0;
    second->fields288[2] = 0;
    second->fields288[1] = 0;
    second->fields288[0] = 0;
    secondPayload->castPlayIndexFC4 = 0;
    secondPayload->castListIndexFC8 = -1;
    secondPayload->castListCA4[0] = 0;
    secondPayload->castListCA4[1] = -1;
    second->scaleX = 1.0F;
    second->scaleY = 1.0F;
    secondPayload->battlePosition18 = 0;
    second->phase = 0;
    second->flags |= 2u;
    second->callback = nullptr;
    YobiAction* const carrierAction = NewYobiAction_404AE0(2, sizeof(BattleRideCarrierPayload), YobiPayloadKind::BattleRideCarrier);
    casterPayload->effect1060 = carrierAction;
    auto* const carrier = carrierAction->payload.battleRideCarrier;
    carrier->first00 = first;
    carrier->second04 = second;
    carrier->gearImage0C = 0;
    carrier->bytes10.fill(std::byte{0});
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 227);
    const std::int32_t initialEffectCasterPosition = gBattleMCasterPosition5AAA64;
    const std::int32_t effectIndex = casterPayload->effectCountCA0;
    YobiAction* const initialEffectCaster = cg::world::gBattleAllEntries59E980[initialEffectCasterPosition];
    AppendBattleEffect_439400(initialEffectCaster, {initialEffectCasterPosition, 1u, 0, 0});
    std::uint32_t selfFlags;
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        if (parsedTargetPosition == gBattleMCasterPosition5AAA64) {
            firstPosition = parsedTargetPosition;
            secondPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            selfFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
            {
                char nameBytes[29]{};
                TakeBattleMString_438FE0(scene, nameBytes, 29);
                first->nameC4 = cg::text::FromGbkWide(nameBytes);
            }
            first->imageNo = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
            for (std::int32_t& image : firstImages) {
                image = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
            }
            first->field1D0 = TakeBattleMInteger_438F70(scene);
            {
                first->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                first->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                first->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                first->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            firstPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
            firstPayload->stateFE0 = 0;
            first->fields288[3] = 0;
            first->fields288[2] = 0;
            first->fields288[1] = 0;
            first->fields288[0] = 0;
            if ((firstPayload->flagsFDC & BattleActorFlag_Earth) != 0) {
                first->fields288[0] = 1;
            }
            if ((firstPayload->flagsFDC & BattleActorFlag_Water) != 0) {
                first->fields288[1] = 1;
            }
            if ((firstPayload->flagsFDC & BattleActorFlag_Fire) != 0) {
                first->fields288[2] = 1;
            }
            if ((firstPayload->flagsFDC & BattleActorFlag_Wind) != 0) {
                first->fields288[3] = 1;
            }
            first->priority = static_cast<std::uint8_t>(static_cast<std::uint8_t>(firstPosition) * 10 + 2);
            first->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][2]);
            first->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][3]);
            first->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][2];
            first->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][3];
            firstPayload->x00 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][2];
            firstPayload->y04 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][3];
            firstPayload->x08 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][0];
            firstPayload->y0C = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][1];
            firstPayload->battlePosition18 = firstPosition;
            UpdateBattleAlbumFlag_4238F0(first);
            casterPayload->rider13C8->nameC4 = first->nameC4;
            YobiAction* const fieldRider = casterPayload->rider13C8;
            const std::int32_t riderField1D0 = first->field1D0;
            fieldRider->field1D0 = riderField1D0;
            casterPayload->rider13C8->readiness = first->readiness;
            casterPayload->rider13C8->maxHp = first->maxHp;
            casterPayload->rider13C8->barValue1B0 = first->barValue1B0;
            casterPayload->rider13C8->barMaximum1C0 = first->barMaximum1C0;
            const std::int32_t riderXSourcePosition = gBattleMCasterPosition5AAA64;
            YobiAction* const riderX = casterPayload->rider13C8;
            YobiAction* const sourceX = cg::world::gBattleAllEntries59E980[riderXSourcePosition];
            riderX->offset40 = static_cast<float>(static_cast<double>(sourceX->frameX) + sourceX->offset40);
            const std::int32_t riderYSourcePosition = gBattleMCasterPosition5AAA64;
            YobiAction* const riderY = casterPayload->rider13C8;
            YobiAction* const sourceY = cg::world::gBattleAllEntries59E980[riderYSourcePosition];
            riderY->offset44 = sourceY->offset44;

            {
                char nameBytes[29]{};
                TakeBattleMString_438FE0(scene, nameBytes, 29);
                second->nameC4 = cg::text::FromGbkWide(nameBytes);
            }
            second->imageNo = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
            second->field1D0 = TakeBattleMInteger_438F70(scene);
            {
                second->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                second->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                second->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                second->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            secondPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
            secondPayload->stateFE0 = 0;
            second->fields288[3] = 0;
            second->fields288[2] = 0;
            second->fields288[1] = 0;
            second->fields288[0] = 0;
            if ((secondPayload->flagsFDC & BattleActorFlag_Earth) != 0) {
                second->fields288[0] = 1;
            }
            if ((secondPayload->flagsFDC & BattleActorFlag_Water) != 0) {
                second->fields288[1] = 1;
            }
            if ((secondPayload->flagsFDC & BattleActorFlag_Fire) != 0) {
                second->fields288[2] = 1;
            }
            if ((secondPayload->flagsFDC & BattleActorFlag_Wind) != 0) {
                second->fields288[3] = 1;
            }
            second->priority = static_cast<std::uint8_t>(static_cast<std::uint8_t>(secondPosition) * 10 + 2);
            second->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][2]);
            second->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][3]);
            second->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][2];
            second->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][3];
            secondPayload->x00 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][2];
            secondPayload->y04 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][3];
            secondPayload->x08 = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][0];
            secondPayload->y0C = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][1];
            secondPayload->battlePosition18 = secondPosition;
            UpdateBattleAlbumFlag_4238F0(second);
            std::int32_t imageIndex = 0;
            for (std::int32_t index = 0; index < 2; ++index) {
                if (firstImages[index] > 0) {
                    imageIndex = index;
                    break;
                }
            }
            carrier->gearImage0C = firstImages[imageIndex];
            casterPayload->effects1C[effectIndex].position3 = firstPosition;
            casterPayload->effects1C[effectIndex].position4 = secondPosition;
            firstPayload->castSkill103C = gBattleMSkill5AAA78;
            firstPayload->castSkillLevel1044 = gBattleMSkillLevelIndex5A3A18;
            secondPayload->castSkill103C = gBattleMSkill5AAA78;
            secondPayload->castSkillLevel1044 = gBattleMSkillLevelIndex5A3A18;
            continue;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        if ((gBattleMResultFlags5A4A20 & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        AppendBattleCast_4392A0(scene, second, 25);
        AppendBattleCast_4392A0(scene, second, 232);
        const std::int32_t targetEffectImage = firstImage;
        const std::int32_t targetEffectAmount = amount;
        const std::uint32_t targetEffectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t targetEffectPosition = gBattleMTargetPosition5AAA40;
        AppendBattleEffect_439400(second,
            {targetEffectPosition, targetEffectFlags, targetEffectAmount, targetEffectImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxImage = firstImages[imageIndex];
        const std::int32_t auxSecondImage = secondImage;
        const std::int32_t auxFirstImage = firstImage;
        const std::uint32_t auxFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxTargetPosition = gBattleMTargetPosition5AAA40;
        AppendBattleAuxEffects_43BC10(second, auxTargetPosition,
            auxFlags, auxFirstImage, auxSecondImage, auxImage);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x108u) == 0) {
            if ((flags & 0x40u) != 0 && (flags & 0x2000u) == 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                const std::int32_t comparisonTargetPosition = gBattleMTargetPosition5AAA40;
                if (cg::world::gBattleCurrentPosition5ABCA4 == comparisonTargetPosition) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
                flags = gBattleMResultFlags5A4A20;
            } else if (amount != 0 && (flags & 3u) != 0) {
                if ((flags & 0x8000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 40);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 123);
                } else if ((flags & 0x4000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 40);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 122);
                } else if ((flags & 0x2000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 40);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 121);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 233);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 40);
                }
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & 0xE000u) == 0 && (flags & 0x108u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
            if ((flags & 0x100u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & 0x2000u) == 0 && (flags & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    const std::int32_t restoredFirstPosition = firstPosition;
    const std::int32_t restoredSecondPosition = secondPosition;
    gBattleMCasterPosition5AAA64 = restoredFirstPosition;
    const std::int32_t finalPreviewPosition = previewPosition;
    gBattleMValue5ABA7C = restoredSecondPosition;
    if (finalPreviewPosition < 20) {
        const std::int32_t pointPosition = singlePosition
            ? (finalPreviewPosition < 5 || (finalPreviewPosition >= 10 && finalPreviewPosition < 15)
                ? finalPreviewPosition : finalPreviewPosition - 5)
            : (finalPreviewPosition < 5 ? 3 : finalPreviewPosition < 10 ? 8 : finalPreviewPosition < 15 ? 14 : 19);
        const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[pointPosition];
        const std::int32_t y = cg::world::gBattleGraphicPoints553758[pointIndex][3];
        const std::int32_t x = cg::world::gBattleGraphicPoints553758[pointIndex][2];
        AppendBattleEffectList_4393A0(second, x, static_cast<std::uint32_t>(y), 0);
    }
    AppendBattleCast_4392A0(scene, second, 207);
    const std::uint8_t selfActionFlags = *reinterpret_cast<const std::uint8_t*>(&selfFlags);
    if ((selfActionFlags & 0x40u) != 0) {
        AppendBattleCast_4392A0(scene, second, 234);
        AppendBattleCast_4392A0(scene, second, 52);
        AppendBattleCast_4392A0(scene, second, 51);
    } else {
        AppendBattleCast_4392A0(scene, second, 231);
        AppendBattleCast_4392A0(scene, second, 36);
        AppendBattleCast_4392A0(scene, second, 0);
        AppendBattleEffectList_4393A0(first, secondPosition, 0, 0);
        AppendBattleCast_4392A0(scene, first, 25);
        AppendBattleCast_4392A0(scene, first, 73);
        AppendBattleCast_4392A0(scene, first, 74);
        AppendBattleCast_4392A0(scene, first, 7);
        AppendBattleCast_4392A0(scene, first, 11);
        AppendBattleCast_4392A0(scene, first, 10);
        AppendBattleCast_4392A0(scene, second, 7);
        AppendBattleCast_4392A0(scene, second, 11);
        AppendBattleCast_4392A0(scene, second, 10);
    }
    if ((selfFlags & 0x200000u) != 0) {
        const std::int32_t selfEffectImage = firstImage;
        const std::uint32_t selfEffectFlags = selfFlags;
        const std::int32_t selfEffectPosition = secondPosition;
        AppendBattleEffect_439400(second, {selfEffectPosition, selfEffectFlags, 0, selfEffectImage});
        const std::uint32_t selfAuxFlags = selfFlags;
        const std::int32_t selfAuxPosition = secondPosition;
        AppendBattleAuxEffects_43BC10(second, selfAuxPosition, selfAuxFlags, 0, 0, 0);
        AppendBattleCast_4392A0(scene, second, 235);
        AppendBattleCast_4392A0(scene, second, 74);
    }
    const std::uint8_t secondActionFlags = *reinterpret_cast<const std::uint8_t*>(&secondPayload->flagsFDC);
    if ((secondActionFlags & BattleActorFlag_Dead) != 0) {
        AppendBattleCast_4392A0(scene, second, 47);
        AppendBattleCast_4392A0(scene, second, 46);
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    payload->packetSubState58 = 1;
}

// 原431CBA..43222F及42CB37..42CB5E；106/111/2002共享生产，首整数写攻击模式。
[[maybe_unused]] void ProcessBattleMSkill106Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    const std::int32_t attackMode = TakeBattleMInteger_438F70(scene);
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->attackModeFF4 = attackMode;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 161);
    const std::int32_t initialSkill = gBattleMSkill5AAA78;
    if (initialSkill == 106) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 77);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 36);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 91);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 103);
    } else if (initialSkill == 111) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 7);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 9);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 75);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 108);
    } else if (initialSkill == 2002 && gBattleMSkillLevelIndex5A3A18 == 90) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 112);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    }
    std::array<char, 29> ignoredName;
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        if (gBattleMSkill5AAA78 == 2002) {
            static_cast<void>(TakeBattleMInteger_438F70(scene));
            static_cast<void>(TakeBattleMInteger_438F70(scene));
            TakeBattleMString_438FE0(scene, ignoredName.data(), 29);
            static_cast<void>(TakeBattleMInteger_438F70(scene));
        }
        if ((gBattleMResultFlags5A4A20 & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        const std::int32_t effectFirstImage = firstImage;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, {effectTargetPosition, effectFlags, amount, effectFirstImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxImage = firstImages[imageIndex];
        const std::int32_t auxSecondImage = secondImage;
        const std::int32_t auxFirstImage = firstImage;
        const std::uint32_t auxFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t auxCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const auxCaster = cg::world::gBattleAllEntries59E980[auxCasterPosition];
        AppendBattleAuxEffects_43BC10(
            auxCaster, auxTargetPosition, auxFlags, auxFirstImage, auxSecondImage, auxImage);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        if (gBattleMSkill5AAA78 == 2002 && gBattleMSkillLevelIndex5A3A18 < 90) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 112);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
        }
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x108u) == 0) {
            if ((flags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                const std::int32_t comparisonTargetPosition = gBattleMTargetPosition5AAA40;
                if (cg::world::gBattleCurrentPosition5ABCA4 == comparisonTargetPosition) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
                flags = gBattleMResultFlags5A4A20;
            } else if (amount != 0 && (flags & 3u) != 0) {
                if ((flags & 0x8000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 123);
                } else if ((flags & 0x4000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 122);
                } else if ((flags & 0x2000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 121);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                        (flags & 2u) != 0 ? 165 : 27);
                }
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((amount != 0 || (flags & 0x108u) != 0) && (flags & 0xE000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
            if ((flags & 0x100u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    const std::int32_t finalSkill = gBattleMSkill5AAA78;
    if (finalSkill == 111) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 76);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 7);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 11);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
    } else if (finalSkill == 2002) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原54D6FC..54D710，2028按等级有符号除3选择前四项，第五项用于等级>=12。
static constexpr std::array<std::int32_t, 5> kBattleMSkill2028Images54D6FC {111147, 111148, 111149, 111150, 111151}; // 原常量参考：0x0054D6FC。

// 原43227D..43282A及42BD50..42BEAC；累计反弹参与者，保留149调用后的双位置重读。
[[maybe_unused]] void ProcessBattleMSkill2028(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    payload->completedEntryCount00 = 0;
    payload->entryCount04 = 0;
    gBattleMMessageValue59FA14 = 0;
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    if (level < 12) {
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t imageIndex = level / 3;
        YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
        auto* const imagePayload = imageCaster->payload.battleActor;
        const std::int32_t image = kBattleMSkill2028Images54D6FC[imageIndex];
        imagePayload->image1028 = image;
    } else {
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
        const std::int32_t image = kBattleMSkill2028Images54D6FC[4];
        auto* const imagePayload = imageCaster->payload.battleActor;
        imagePayload->image1028 = image;
    }
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x10000u) != 0) {
            const std::int32_t previousTotal = gBattleMMessageValue59FA14;
            const std::int32_t target = gBattleMTargetPosition5AAA40;
            gBattleMFlags5ABA84 = flags;
            const std::int32_t total = previousTotal + amount;
            flags &= ~0x20u;
            gBattleMFeedbackSourcePosition5ABA90 = target;
            gBattleMMessageValue59FA14 = total;
            gBattleMResultFlags5A4A20 = flags;
            payload->entryPositions08[payload->entryCount04] = target;
            ++payload->entryCount04;
            flags = gBattleMResultFlags5A4A20;
        }
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, flags, amount);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
        std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
        if (!((sourcePosition == 20 && targetPosition < 10) || (sourcePosition == 21 && targetPosition >= 10))) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 149);
            sourcePosition = gBattleMCasterPosition5AAA64;
            targetPosition = gBattleMTargetPosition5AAA40;
        }
        flags = gBattleMResultFlags5A4A20;
        const bool sameSide = (sourcePosition == 20 && targetPosition < 10)
            || (sourcePosition == 21 && targetPosition >= 10);
        const std::uint32_t response = (flags & 0x40000u) != 0 ? 0x40000u
            : (flags & 0x20000u) != 0 ? 0x20000u : (flags & 0x10000u);
        if (sameSide) {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[targetPosition], sourcePosition, flags, amount);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 184);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 74);
            targetPosition = gBattleMTargetPosition5AAA40;
        }
        if (response == 0x40000u) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 126);
            continue;
        }
        if (response == 0x10000u) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 124);
            continue;
        }
        if (response == 0x20000u) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 125);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 159);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 32);
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->value1030 = amount;
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->numberDisplayType1034 = 6;
            if ((gBattleMResultFlags5A4A20 & 0x10000000u) != 0) {
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                    ->numberDisplayType1034 = 0;
            }
            const std::uint8_t responseFlags = *reinterpret_cast<const std::uint8_t*>(&gBattleMResultFlags5A4A20);
            if ((responseFlags & 0x20u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 47);
            }
            continue;
        }
        if (!sameSide) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 160);
        }
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->value1030 = amount;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->numberDisplayType1034 = 5;
        if ((gBattleMResultFlags5A4A20 & 0x10000000u) != 0) {
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->numberDisplayType1034 = 0;
        }
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    if (gBattleMMessageValue59FA14 != 0) {
        const std::int32_t reflectedSkillPosition = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const reflectedSkillActor = cg::world::gBattleAllEntries59E980[reflectedSkillPosition];
        const std::int32_t reflectedSkill = gBattleMSkill5AAA78;
        reflectedSkillActor->payload.battleActor->castSkill103C = reflectedSkill;
        const std::int32_t reflectedLevelPosition = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const reflectedLevelActor = cg::world::gBattleAllEntries59E980[reflectedLevelPosition];
        const std::int32_t reflectedLevel = gBattleMSkillLevelIndex5A3A18;
        reflectedLevelActor->payload.battleActor->castSkillLevel1044 = reflectedLevel;
        gBattleMFlags5ABA84 &= ~0x10000u;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 1);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 140);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 0);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 149);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 160);
        const std::int32_t feedbackCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const feedbackCaster = cg::world::gBattleAllEntries59E980[feedbackCasterPosition];
        const std::int32_t feedbackTotal = gBattleMMessageValue59FA14;
        feedbackCaster->payload.battleActor->value1030 = feedbackTotal;
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->numberDisplayType1034 = 5;
        const std::int32_t feedbackAmount = gBattleMMessageValue59FA14;
        const std::uint32_t feedbackFlags = gBattleMFlags5ABA84;
        const std::int32_t feedbackTargetPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t feedbackSourcePosition = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const feedbackSource = cg::world::gBattleAllEntries59E980[feedbackSourcePosition];
        AppendBattleEffectList_4393A0(feedbackSource, feedbackTargetPosition, feedbackFlags, feedbackAmount);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 74);
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    payload->packetSubState58 = 2;
}

// 原54D680按有效档1..5索引；只保存54D684开始的五个实际图号。
static constexpr std::array<std::int32_t, 5> kBattleMSkill2019Images54D684 {111126, 111127, 111128, 111129, 111130}; // 原常量参考：0x0054D684。

// 原432867..432ECE，99..104/2019/3001共享状态生产；固定状态载荷跨144调用保留。
[[maybe_unused]] void ProcessBattleMSkill99Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[casterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    if (gBattleMSkill5AAA78 == 2019) {
        const std::int32_t band = GetBattleValueBand_43BF00(gBattleMSkillLevelIndex5A3A18 + 1);
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t image = kBattleMSkill2019Images54D684[band - 1];
        YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
        imageCaster->payload.battleActor->image13E4 = image;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 253);
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 39);
    if (gBattleMSkill5AAA78 == 2019) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 254);
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    if (level < 12) {
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t image = kBattleMSkillEffectOffsets553E58[level] + 110056;
        YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
        imageCaster->payload.battleActor->image1028 = image;
    } else {
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image1028 = 111018;
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    // 原432A3E承接未初始化的a4；正常99..104先赋1..6，switch默认不覆盖跨目标值。
    std::int32_t statusType;
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        std::uint32_t flags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        gBattleMResultFlags5A4A20 = flags;
        if ((flags & 0x80000u) != 0) {
            const std::int32_t appendTargetPosition = gBattleMTargetPosition5AAA40;
            const std::int32_t appendLevel = gBattleMSkillLevel5A4A2C;
            const std::int32_t appendSkill = gBattleMSkill5AAA78;
            AddBattleAppendEffect_43BE00(appendSkill, appendLevel, appendTargetPosition);
            flags = gBattleMResultFlags5A4A20;
        }
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, flags, 0);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        const std::int32_t targetSkill = gBattleMSkill5AAA78;
        skillTarget->payload.battleActor->castSkill103C = targetSkill;
        const std::int32_t levelTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelTargetPosition];
        const std::int32_t targetLevel = gBattleMSkillLevelIndex5A3A18;
        levelTarget->payload.battleActor->castSkillLevel1044 = targetLevel;
        const std::int32_t skill = gBattleMSkill5AAA78;
        if (skill >= 99 && skill <= 104) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 157);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) == 0) {
                continue;
            }
            const std::int32_t reapplyTargetPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const reapplyTarget = cg::world::gBattleAllEntries59E980[reapplyTargetPosition];
            const std::int32_t reapplySkill = gBattleMSkill5AAA78;
            reapplyTarget->payload.battleActor->castSkill103C = reapplySkill;
            const std::int32_t statusTargetPosition = gBattleMTargetPosition5AAA40;
            const std::int32_t statusSkill = gBattleMSkill5AAA78;
            YobiAction* const statusTarget = cg::world::gBattleAllEntries59E980[statusTargetPosition];
            auto* const targetPayload = statusTarget->payload.battleActor;
            switch (statusSkill) {
            case 99: statusType = 1; break;
            case 100: statusType = 2; break;
            case 101: statusType = 3; break;
            case 102: statusType = 4; break;
            case 103: statusType = 5; break;
            case 104: statusType = 6; break;
            default: break;
            }
            if (targetPayload->statusEffectMode13D8 != 1 || targetPayload->statusEffectType13DC != statusType) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 144);
            }
            targetPayload->statusEffectType13DC = statusType;
            targetPayload->statusEffectMode13D8 = 1;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 143);
            const std::int32_t currentSkill = gBattleMSkill5AAA78;
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->soundId1018 = currentSkill == 99 || currentSkill == 101 || currentSkill == 103 ? 253 : 274;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 80);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
        } else if (skill == 3001) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 158);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) == 0) {
                continue;
            }
            const std::int32_t reapplyTargetPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const reapplyTarget = cg::world::gBattleAllEntries59E980[reapplyTargetPosition];
            const std::int32_t reapplySkill = gBattleMSkill5AAA78;
            reapplyTarget->payload.battleActor->castSkill103C = reapplySkill;
            auto* const targetPayload =
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor;
            if (targetPayload->statusEffectMode13D8 != 1 || targetPayload->statusEffectType13DC != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 144);
            }
            targetPayload->statusEffectType13DC = 0;
            targetPayload->statusEffectMode13D8 = 1;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 143);
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->soundId1018 = 274;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 80);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
        } else if (skill == 2019) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 157);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) == 0) {
                continue;
            }
            const std::int32_t statusTargetPosition = gBattleMTargetPosition5AAA40;
            const std::int32_t statusLevel = gBattleMSkillLevel5A4A2C;
            YobiAction* const statusTarget = cg::world::gBattleAllEntries59E980[statusTargetPosition];
            const std::int32_t remainderDividend = statusLevel - 1;
            auto* const targetPayload = statusTarget->payload.battleActor;
            const std::int32_t remainder = remainderDividend % 3;
            if (targetPayload->statusEffectMode13D8 != 3 || targetPayload->statusEffectType13DC != 7 || targetPayload->statusEffectImageIndex13E0 != remainder) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 144);
            }
            targetPayload->statusEffectType13DC = 7;
            targetPayload->statusEffectMode13D8 = 3;
            targetPayload->statusEffectImageIndex13E0 = (gBattleMSkillLevel5A4A2C - 1) % 3;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 143);
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->soundId1018 = 253;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 80);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
        }
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原4357BA..435B25及42CB44..42CB5E；2005/4003共用，首整数保留攻击模式，尾部递增子状态。
[[maybe_unused]] void ProcessBattleMSkill2005Family(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    const std::int32_t attackMode = TakeBattleMInteger_438F70(scene);
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->attackModeFF4 = attackMode;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 249);
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t parsedTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = parsedTargetPosition == 255;
        gBattleMTargetPosition5AAA40 = parsedTargetPosition;
        if (targetUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[parsedTargetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t amount = TakeBattleMInteger_438F70(scene);
        if ((gBattleMResultFlags5A4A20 & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        const std::int32_t effectImage = firstImage;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, {effectTargetPosition, effectFlags, amount, effectImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t auxImage = firstImages[imageIndex];
        const std::int32_t auxSecondImage = secondImage;
        const std::int32_t auxFirstImage = firstImage;
        const std::uint32_t auxFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t auxCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const auxCaster = cg::world::gBattleAllEntries59E980[auxCasterPosition];
        AppendBattleAuxEffects_43BC10(auxCaster, auxTargetPosition,
            auxFlags, auxFirstImage, auxSecondImage, auxImage);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x108u) == 0) {
            if ((flags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                const std::int32_t comparisonTargetPosition = gBattleMTargetPosition5AAA40;
                if (cg::world::gBattleCurrentPosition5ABCA4 == comparisonTargetPosition) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
                flags = gBattleMResultFlags5A4A20;
            } else if (amount != 0 && (flags & 3u) != 0) {
                if ((flags & 0x8000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 123);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x4000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 122);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x2000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 121);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x20000000u) == 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                        (flags & 2u) != 0 ? 165 : 27);
                    flags = gBattleMResultFlags5A4A20;
                }
            }
        }
        if ((amount != 0 || (flags & 0x108u) != 0) && (flags & 0x2000E000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
            if ((flags & 0x100u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

} // namespace detail

} // namespace cg::yobi
