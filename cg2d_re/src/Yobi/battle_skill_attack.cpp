#include "battle_skill_attack.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_message.hpp"

#include "battle_scene.hpp"
#include "battle_skill.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"

#include <array>
#include <bit>
#include <cstdint>

namespace cg::yobi {

namespace detail {

static std::int32_t gBattleMRedirectPosition5AAA70 = 0; // 原全局：0x005AAA70。 426FC0/424620的独立转向输出，非13项快照或5A4A28。

// 426FC0的两组五项原表；由404D90保留S32索引门禁。
static constexpr std::array<std::int32_t, 5> kBattleSkillImages54D710 { // 原常量参考：0x0054D710。
    0x1B293, 0x1B294, 0x1B295, 0x1B296, 0x1B297,
};

static constexpr std::array<std::int32_t, 5> kBattleSkillImages54D724 { // 原常量参考：0x0054D724。
    0x1B29D, 0x1B29E, 0x1B29F, 0x1B2A0, 0x1B2A1,
};

// 原[424620,426F9C)：完整九参攻击目标生产；D01仅次级图号入口置0一次。
// 共享尾/字段与分支见doc/cg2d_battle_attack_targets_spec.md，保留原条件与追加次序。
void TakeBattleMSkillAttackTargets_424620(
    YobiAction* const scene, std::int32_t skill, std::int32_t level,
    const std::int32_t levelIndex, const std::int32_t fpUsed,
    std::int32_t* const sourcePosition, std::int32_t* const initialPosition,
    std::int32_t* const targetPosition, std::int32_t* const redirectedPosition)
{
    constexpr std::array<std::int32_t, 5> kAttackImages54D6D0 {
        0x1B210, 0x1B211, 0x1B212, 0x1B213, 0x1B214
    };
    constexpr std::array<std::int32_t, 5> kAttackImages54D5F4 {
        0x1B1DB, 0x1B1DC, 0x1B1DD, 0x1B1DE, 0x1B1DF
    };
    constexpr std::array<std::int32_t, 5> kAttackImages54D608 {
        0x1B1E0, 0x1B1E1, 0x1B1E2, 0x1B1E3, 0x1B1E4
    };
    constexpr std::array<std::int32_t, 5> kAttackImages54D694 {
        0x1B21A, 0x1B21B, 0x1B21C, 0x1B21D, 0x1B21E
    };
    constexpr std::array<std::int32_t, 5> kAttackImages54D6BC {
        0x1B206, 0x1B20C, 0x1B20D, 0x1B20E, 0x1B20F
    };
    std::int32_t sourceToken;
    std::int32_t firstSource;
    YobiAction* firstCaster;
    std::int32_t targetToken;
    std::int32_t firstTarget;
    std::int32_t* currentTarget;
    std::int32_t clearedMainFlags;
    std::int32_t secondaryPosition;
    std::int32_t redirectToken;
    std::int32_t mainRedirect;
    std::int32_t secondaryToken;
    std::int32_t secondaryConverted;
    std::int32_t secondaryRedirectToken;
    std::int32_t secondaryRedirect;
    std::int32_t movingSource;
    std::int32_t mainAuxIndex;
    std::int32_t mainScanIndex94;
    std::int32_t* mainScan94;
    std::int32_t* initialReactionPosition;
    std::int32_t skill2017SecondaryToken;
    std::int32_t skill2017RedirectToken;
    std::int32_t skill2017Redirect;
    std::int32_t mainScanIndex105;
    std::int32_t* mainScan105;
    std::int32_t mainScanIndexDefault;
    std::int32_t* mainScanDefault;
    std::int32_t skill72SecondaryToken;
    std::int32_t skill72RedirectToken;
    std::int32_t skill72Redirect;
    std::int32_t currentMainFlags;
    std::int32_t criticalPosition;
    std::int32_t secondaryNextTarget;
    std::int32_t originalTargetForSecondary;
    std::int32_t secondaryAuxIndex;
    std::int32_t secondaryScanIndex;
    std::int32_t* secondaryScan;
    std::int32_t secondaryCriticalPosition;
    std::int32_t currentSecondaryFlags;
    std::int32_t mainNextTarget;
    YobiAction* mainCompletedActor;
    std::int32_t mainCompletedTarget;
    std::int32_t skill72CriticalPosition;
    std::int32_t skill72NextTarget;
    std::uint8_t secondaryFlagByte1;
    std::int32_t* initialPositionForSkill72;
    std::int32_t initialSkill72Value;
    std::int32_t skill2017Flags; // 426A7B/426B8B完整W32，426BD2不窄化。
    std::int32_t skill2017Position;
    std::int32_t skill2017CriticalPosition;
    std::int32_t skill2017NextTarget;
    std::int32_t nextSourceToken;
    std::int32_t nextSource;
    std::int32_t* initialPositionAtTail;
    YobiAction* nextInitialActor;
    std::int32_t nextTargetToken;
    std::int32_t nextTarget;
    std::int32_t initialAtTail;
    YobiAction* tailInitialActor;
    BattleEntryEffectRecord effect2017Secondary;
    BattleEntryEffectRecord effect260Main;
    BattleEntryEffectRecord effect2017Redirect;
    BattleEntryEffectRecord effect72Redirect;
    BattleEntryEffectRecord effect72Main;
    BattleEntryEffectRecord effectHighTechnique;
    BattleEntryEffectRecord effect260Secondary;
    BattleEntryEffectRecord effectPrimary;
    BattleEntryEffectRecord effect2017Main;
    BattleEntryEffectRecord effect72Secondary;
    YobiAction* failureScene;
    std::int32_t primaryValue;
    std::int32_t primaryGraphic;
    std::int32_t deferredInitialDeath;
    std::int32_t secondaryGraphic = 0; // D01：原var_50仅入口初始化一次。
    std::int32_t mainFlag8;
    std::int32_t mainSecondGraphic;
    std::int32_t secondarySecondGraphic;
    std::int32_t nextTargetOrDeathFlag;
    std::int32_t roundCount;
    std::int32_t positionOrFlag;
    std::int32_t skill81Value = 0; // 仅skill==81读取；进入目标循环前已解析同一技能的值。
    std::int32_t mainFirstImages[2];
    std::int32_t secondaryFirstImages[2];
    std::int32_t secondaryValues[2];
    std::int32_t secondarySecondImages[2];
    std::int32_t mainValues[2];
    std::int32_t mainSecondImages[2];
    std::int32_t secondaryFlags;
    std::int32_t lastSecondaryPosition;
    std::int32_t mainFlags;
    // 424931..424999：两次独立索引/坐标采样，S32均值与源位置分别承接。
    std::int32_t coordinateIndex15;
    std::int32_t coordinateIndex5;
    std::int32_t coordinate15;
    std::int32_t coordinate5;
    std::int32_t coordinateSum;
    std::int32_t coordinateSource;

    primaryGraphic = 0;
    roundCount = 0;
    deferredInitialDeath = 0;
    mainSecondGraphic = 0;
    secondarySecondGraphic = 0;
    mainFirstImages[0] = 0;
    mainFirstImages[1] = 0;
    secondaryFirstImages[0] = 0;
    secondaryFirstImages[1] = 0;
    mainSecondImages[0] = 0;
    mainSecondImages[1] = 0;
    secondarySecondImages[0] = 0;
    secondarySecondImages[1] = 0;
    mainValues[0] = 0;
    mainValues[1] = 0;
    secondaryValues[0] = 0;
    secondaryValues[1] = 0;
    sourceToken = TakeBattleMInteger_438F70(scene);
    firstSource = GetBattlePosition_4237E0(sourceToken);
    *initialPosition = firstSource;
    positionOrFlag = firstSource;
    (*sourcePosition) = firstSource;
    if (firstSource == 255)
        return;
    firstCaster = cg::world::gBattleAllEntries59E980[firstSource];
    if (!firstCaster) {
// 原函数：0x0043B6C0。
FinishFirstTargetFailure:
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    firstCaster->payload.battleActor->castLevel1040 = level;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 40);
    if (static_cast<std::uint32_t>(skill) < 2
        || skill == BattleSkill_Parameter_AllOrNothing
        || skill == BattleSkill_Forcecut
        || skill == 72
        || skill == BattleSkill_StatusAttackPoison
        || skill == BattleSkill_StatusAttackSleep
        || skill == BattleSkill_StatusAttackStone
        || skill == BattleSkill_StatusAttackInebriety
        || skill == BattleSkill_StatusAttackConfusion
        || skill == BattleSkill_StatusAttackForget
        || skill == BattleSkill_StatusAttackPoison2096
        || skill == BattleSkill_StatusAttackSleep2097
        || skill == BattleSkill_StatusAttackStone2098
        || skill == BattleSkill_StatusAttackInebriety2099
        || skill == BattleSkill_StatusAttackConfusion2100
        || skill == BattleSkill_StatusAttackForget2101
        || skill == 81
        || skill == 84
        || skill == 85
        || skill == 87
        || skill == BattleSkill_Assassin
        || skill == BattleSkill_PanicAttack
        || skill == BattleSkill_GuardBreak
        || skill == BattleSkill_ShortStrike
        || skill == BattleSkill_Taunt
        || skill == BattleSkill_MercyStrike
        || skill == BattleSkill_SwallowSlash) {
        goto AppendInitialGenericCast;
    }
    if (skill == BattleSkill_ShadowHand) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 151);
        cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->image102C = kAttackImages54D6D0[GetBattleValueBand_43BF00(levelIndex + 1)];
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 252);
        goto ReadFirstTarget;
    }
    if (skill == BattleSkill_Recklessness) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPosition], 151);
        cg::world::gBattleAllEntries59E980[*initialPosition]->payload.battleActor->image102C = kAttackImages54D5F4[GetBattleValueBand_43BF00(levelIndex + 1)];
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPosition], 252);
        goto ReadFirstTarget;
    }
    if (skill < BattleSkill_DelayAttack257 || skill >= 4004) {
        if (skill == BattleSkill_Ranbu) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPosition], 161);
            cg::world::gBattleAllEntries59E980[*initialPosition]->payload.battleActor->flash1004 = 1;
            goto ReadFirstTarget;
        }
        if (skill != 113)
            goto ReadFirstTarget;
// 原函数：0x004392A0。
AppendInitialGenericCast:
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPosition], 151);
        goto ReadFirstTarget;
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPosition], 151);
    if (skill != BattleSkill_DelayAttack257) {
        if (skill == BattleSkill_DelayAttack258) {
            coordinateIndex15 = cg::world::gBattlePointIndices5536FC[15];
            coordinateIndex5 = cg::world::gBattlePointIndices5536FC[5];
            coordinate15 = cg::world::gBattleGraphicPoints553758[coordinateIndex15][2];
            coordinate5 = cg::world::gBattleGraphicPoints553758[coordinateIndex5][2];
            coordinateSum = coordinate15 + coordinate5;
            coordinateSource = *sourcePosition;
            cg::world::gBattleAllEntries59E980[coordinateSource]->payload.battleActor
                ->moveTargetXFE8 = coordinateSum / 2;
            coordinateIndex15 = cg::world::gBattlePointIndices5536FC[15];
            coordinateIndex5 = cg::world::gBattlePointIndices5536FC[5];
            coordinate15 = cg::world::gBattleGraphicPoints553758[coordinateIndex15][3];
            coordinate5 = cg::world::gBattleGraphicPoints553758[coordinateIndex5][3];
            coordinateSum = coordinate15 + coordinate5;
            coordinateSource = *sourcePosition;
            cg::world::gBattleAllEntries59E980[coordinateSource]->payload.battleActor
                ->moveTargetYFEC = coordinateSum / 2;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 8);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 6);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 10);
            goto ReadFirstTarget;
        }
        if (skill != BattleSkill_FirstAttack || levelIndex < 9)
            goto ReadFirstTarget;
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPosition], 75);
ReadFirstTarget:
    targetToken = TakeBattleMInteger_438F70(scene);
    firstTarget = GetBattlePosition_4237E0(targetToken);
    currentTarget = targetPosition;
    (*targetPosition) = firstTarget;
    if (!cg::world::gBattleAllEntries59E980[firstTarget])
        goto FinishFirstTargetFailure;
    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->castSkill103C = skill;
    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->castSkillLevel1044 = levelIndex;
    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 55);
    mainFlags = TakeBattleMInteger_438F70(scene);
    primaryValue = TakeBattleMInteger_438F70(scene);
    if (skill == 81)
        skill81Value = TakeBattleMInteger_438F70(scene);
    if ((mainFlags & 0x800000) != 0)
        TakeBattleMAuxGraphics_43BF30(scene, 0, &primaryGraphic, &mainSecondGraphic, mainFirstImages, mainSecondImages, mainValues);
    if (skill == BattleSkill_GuardBreak || skill == BattleSkill_ShortStrike) {
        clearedMainFlags = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(mainFlags) & ~0x4U);
        mainFlags = clearedMainFlags;
    }
    secondaryPosition = -1;
    secondaryFlags = 0;
    for (lastSecondaryPosition = -1; ; lastSecondaryPosition = -1) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 25);
        if ((mainFlags & 0x10) != 0 || (mainFlags & 0x1000) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 3);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 115);
            goto SelectSkill260MoveTail;
        }
        if (skill == BattleSkill_Assassin)
            goto SelectSkill260MoveTail;
        if (skill == BattleSkill_Wattack) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 208);
// 原函数：0x004392A0。
AppendSkill260Move206:
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 206);
            goto AppendPrimaryEffect;
        }
        if (roundCount > 0) {
            movingSource = (*sourcePosition);
            if ((*sourcePosition) == positionOrFlag) {
                if (skill == BattleSkill_FirstAttack) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[movingSource], 210);
                    goto AppendPrimaryEffect;
                }
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[movingSource], 3);
SelectSkill260MoveTail:
                if (skill != BattleSkill_Wattack)
                    goto AppendPrimaryEffect;
                goto AppendSkill260Move206;
            }
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 7);
        if (skill == BattleSkill_FirstAttack)
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 210);
        else
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 9);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 10);
        if (skill == BattleSkill_Taunt) {
            cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->image102C = kAttackImages54D608[GetBattleValueBand_43BF00(levelIndex + 1)];
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 252);
        } else {
            if (skill != BattleSkill_MercyStrike)
                goto SelectSkill260MoveTail;
            cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->image102C = kAttackImages54D694[GetBattleValueBand_43BF00(levelIndex + 1)];
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 252);
        }
// 原函数：0x004392A0。
AppendPrimaryEffect:
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 247);
        effectPrimary.position3 = primaryValue;
        effectPrimary.position4 = primaryGraphic;
        effectPrimary.flags = static_cast<std::uint32_t>(mainFlags);
        effectPrimary.position = (*currentTarget);
        AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effectPrimary);
        positionOrFlag = mainFlags & 0x200;
        if ((mainFlags & 0x200) != 0) {
            redirectToken = TakeBattleMInteger_438F70(scene);
            mainRedirect = GetBattlePosition_4237E0(redirectToken);
            *redirectedPosition = mainRedirect;
            AppendBattleRedirect_4242B0(scene, (*sourcePosition), (*currentTarget), mainFlags, primaryValue, mainRedirect);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], 11);
            (*currentTarget) = *redirectedPosition;
        }
        switch (skill) {
            case BattleSkill_Wattack:
                if ((mainFlags & 0x80000) != 0) {
                    secondaryToken = TakeBattleMInteger_438F70(scene);
                    secondaryConverted = GetBattlePosition_4237E0(secondaryToken);
                    secondaryPosition = secondaryConverted;
                    lastSecondaryPosition = secondaryConverted;
                    if (secondaryConverted != 255) {
                        failureScene = scene;
                        if (!cg::world::gBattleAllEntries59E980[secondaryConverted])
                            goto FinishMidRoundFailure;
                        secondaryFlags = TakeBattleMInteger_438F70(scene);
                        level = TakeBattleMInteger_438F70(scene);
                        if ((secondaryFlags & 0x800000) != 0)
                            TakeBattleMAuxGraphics_43BF30(scene, 0, &secondaryGraphic, &secondarySecondGraphic, secondaryFirstImages, secondarySecondImages, secondaryValues);
                        if ((secondaryFlags & 0x200) != 0) {
                            secondaryRedirectToken = TakeBattleMInteger_438F70(scene);
                            secondaryRedirect = GetBattlePosition_4237E0(secondaryRedirectToken);
                            AppendBattleRedirect_4242B0(scene, (*sourcePosition), secondaryPosition, secondaryFlags, level, secondaryRedirect);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 11);
                            effect260Main.position3 = primaryValue;
                            effect260Main.position4 = primaryGraphic;
                            effect260Main.flags = static_cast<std::uint32_t>(mainFlags);
                            secondaryPosition = secondaryRedirect;
                            effect260Main.position = (*targetPosition);
                            lastSecondaryPosition = secondaryRedirect;
                            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effect260Main);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                            currentTarget = targetPosition;
                        }
                    }
                }
                goto AppendTechniqueEffects;
            case BattleSkill_PanicAttack:
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->attackDebuffType1050 = TakeBattleMInteger_438F70(scene);
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId1018 = 260;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 98);
                mainAuxIndex = 0;
                mainScanIndex94 = 0;
                mainScan94 = mainFirstImages;
                while (*mainScan94 <= 0) {
                    ++mainScanIndex94;
                    ++mainScan94;
                    if (mainScanIndex94 >= 2)
                        goto AppendMainAuxiliaryEffects;
                }
                mainAuxIndex = mainScanIndex94;
                goto AppendMainAuxiliaryEffects;
            case BattleSkill_GuardBreak:
            case BattleSkill_ShortStrike:
                initialReactionPosition = initialPosition;
                cg::world::gBattleAllEntries59E980[*initialPosition]->payload.battleActor->reactionImage1010 = 22341;
                goto AppendInitialReaction;
            case 84:
                initialReactionPosition = initialPosition;
                cg::world::gBattleAllEntries59E980[*initialPosition]->payload.battleActor->reactionImage1010 = 22343;
                goto AppendInitialReaction;
            case 87:
                initialReactionPosition = initialPosition;
                cg::world::gBattleAllEntries59E980[*initialPosition]->payload.battleActor->reactionImage1010 = 22342;
// 原函数：0x004392A0。
AppendInitialReaction:
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialReactionPosition], 68);
AppendTechniqueEffects:
                if (levelIndex >= 9) {
                    switch (skill) {
                        case BattleSkill_DelayAttack257:
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[(*sourcePosition)], 2, 0, 15);
                            goto AppendTechniqueMainEffect;
                        case BattleSkill_StatusAttackPoisonStrike:
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[(*sourcePosition)], 0, 0, 15);
                            goto AppendTechniqueMainEffect;
                        case BattleSkill_Wattack:
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[(*sourcePosition)], 1, 0, 15);
                            goto AppendTechniqueMainEffect;
                        case BattleSkill_FirstAttack:
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[(*sourcePosition)], 3, 0, 15);
// 原函数：0x004392A0。
AppendTechniqueMainEffect:
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 245);
                            effectHighTechnique.position3 = primaryValue;
                            effectHighTechnique.position4 = primaryGraphic;
                            effectHighTechnique.flags = static_cast<std::uint32_t>(mainFlags);
                            effectHighTechnique.position = (*currentTarget);
                            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effectHighTechnique);
                            break;
                        default:
                            break;
                    }
                }
                mainAuxIndex = 0;
                mainScanIndexDefault = 0;
                mainScanDefault = mainFirstImages;
                while (*mainScanDefault <= 0) {
                    ++mainScanIndexDefault;
                    ++mainScanDefault;
                    if (mainScanIndexDefault >= 2)
                        goto AppendMainAuxiliaryEffects;
                }
                mainAuxIndex = mainScanIndexDefault;
// 原函数：0x0043BC10。
AppendMainAuxiliaryEffects:
                AppendBattleAuxEffects_43BC10(cg::world::gBattleAllEntries59E980[(*sourcePosition)], (*currentTarget), mainFlags, primaryGraphic, mainSecondGraphic, mainFirstImages[mainAuxIndex]);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 14);
                break;
            case 72:
            case 113:
                secondaryPosition = 0;
                lastSecondaryPosition = 0;
                secondaryFlags = 0;
                level = 0;
                if ((mainFlags & 1) != 0) {
                    skill72SecondaryToken = TakeBattleMInteger_438F70(scene);
                    secondaryPosition = GetBattlePosition_4237E0(skill72SecondaryToken);
                    lastSecondaryPosition = secondaryPosition;
                    secondaryFlags = TakeBattleMInteger_438F70(scene);
                    level = TakeBattleMInteger_438F70(scene);
                    if ((secondaryFlags & 0x800000) != 0)
                        TakeBattleMAuxGraphics_43BF30(scene, 0, &secondaryGraphic, &secondarySecondGraphic, secondaryFirstImages, secondarySecondImages, secondaryValues);
                    if ((secondaryFlags & 0x200) != 0) {
                        skill72RedirectToken = TakeBattleMInteger_438F70(scene);
                        skill72Redirect = GetBattlePosition_4237E0(skill72RedirectToken);
                        secondaryPosition = AppendBattleRedirect_4242B0(scene, (*sourcePosition), secondaryPosition, secondaryFlags, level, skill72Redirect);
                        lastSecondaryPosition = secondaryPosition;
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                        effect72Redirect.position3 = primaryValue;
                        effect72Redirect.position4 = primaryGraphic;
                        effect72Redirect.flags = static_cast<std::uint32_t>(mainFlags);
                        effect72Redirect.position = (*targetPosition);
                        AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effect72Redirect);
                    }
                }
                if ((mainFlags & 1) != 0) {
                    if ((secondaryFlags & 0x200) == 0)
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 73);
                    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->targetPosition100C = (*targetPosition);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 41);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 75);
                    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId1018 = 110;
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                    if ((secondaryFlags & 0x200) != 0)
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 73);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                    effect72Secondary.position3 = level;
                    effect72Secondary.position4 = secondaryGraphic;
                    effect72Secondary.position = secondaryPosition;
                    effect72Secondary.flags = static_cast<std::uint32_t>(secondaryFlags);
                    AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effect72Secondary);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 237);
                    effect72Main.position3 = primaryValue;
                    effect72Main.position4 = primaryGraphic;
                    effect72Main.flags = static_cast<std::uint32_t>(mainFlags);
                    effect72Main.position = (*targetPosition);
                    AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effect72Main);
                    if ((secondaryFlags & 0x40) == 0 && (secondaryFlags & 0x2000) == 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 41);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 5);
                    }
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 63);
                    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->targetPosition100C = (*targetPosition);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 41);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 75);
                    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId1018 = 110;
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 73);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 5);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 41);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 5);
                }
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 76);
                currentTarget = targetPosition;
                break;
            case BattleSkill_ShadowHand:
                secondaryPosition = 0;
                lastSecondaryPosition = 0;
                secondaryFlags = 0;
                level = 0;
                if ((mainFlags & 1) != 0) {
                    skill2017SecondaryToken = TakeBattleMInteger_438F70(scene);
                    secondaryPosition = GetBattlePosition_4237E0(skill2017SecondaryToken);
                    lastSecondaryPosition = secondaryPosition;
                    secondaryFlags = TakeBattleMInteger_438F70(scene);
                    level = TakeBattleMInteger_438F70(scene);
                    if ((secondaryFlags & 0x800000) != 0)
                        TakeBattleMAuxGraphics_43BF30(scene, 0, &secondaryGraphic, &secondarySecondGraphic, secondaryFirstImages, secondarySecondImages, secondaryValues);
                    if ((secondaryFlags & 0x200) != 0) {
                        skill2017RedirectToken = TakeBattleMInteger_438F70(scene);
                        skill2017Redirect = GetBattlePosition_4237E0(skill2017RedirectToken);
                        secondaryPosition = AppendBattleRedirect_4242B0(scene, (*sourcePosition), secondaryPosition, secondaryFlags, level, skill2017Redirect);
                        lastSecondaryPosition = secondaryPosition;
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                        effect2017Redirect.position3 = primaryValue;
                        effect2017Redirect.position4 = primaryGraphic;
                        effect2017Redirect.flags = static_cast<std::uint32_t>(mainFlags);
                        effect2017Redirect.position = (*currentTarget);
                        AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effect2017Redirect);
                    }
                    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->targetPosition100C = (*currentTarget);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 41);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 75);
                    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId1018 = 110;
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 73);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                    effect2017Secondary.position3 = level;
                    effect2017Secondary.position4 = secondaryGraphic;
                    effect2017Secondary.position = secondaryPosition;
                    effect2017Secondary.flags = static_cast<std::uint32_t>(secondaryFlags);
                    AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effect2017Secondary);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 237);
                    effect2017Main.position3 = primaryValue;
                    effect2017Main.position4 = primaryGraphic;
                    effect2017Main.flags = static_cast<std::uint32_t>(mainFlags);
                    effect2017Main.position = (*currentTarget);
                    AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*sourcePosition)], effect2017Main);
                    if ((secondaryFlags & 0x40) == 0 && (secondaryFlags & 0x2000) == 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 41);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 5);
                    }
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 63);
                    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->targetPosition100C = (*currentTarget);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 41);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 75);
                    cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId1018 = 110;
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 73);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 5);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 41);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 5);
                }
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 76);
                break;
            case BattleSkill_Assassin:
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId1018 = 62;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 77);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 36);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 91);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 36);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 94);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 92);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 36);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 78);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 36);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 75);
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId101C = 210;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 81);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 95);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 76);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 64);
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId1020 = 133;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 82);
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->reactionImage1010 = 22403;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 68);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 97);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 37);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 96);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 73);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
                break;
            case BattleSkill_Ranbu:
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 99);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 100);
                mainAuxIndex = 0;
                mainScanIndex105 = 0;
                mainScan105 = mainFirstImages;
                while (*mainScan105 <= 0) {
                    ++mainScanIndex105;
                    ++mainScan105;
                    if (mainScanIndex105 >= 2)
                        goto AppendMainAuxiliaryEffects;
                }
                mainAuxIndex = mainScanIndex105;
                goto AppendMainAuxiliaryEffects;
            default:
                goto AppendTechniqueEffects;
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], 0);
        if (skill == BattleSkill_ShortStrike) {
            cg::world::gBattleAllEntries59E980[(*currentTarget)]->payload.battleActor->image102C = kAttackImages54D6BC[GetBattleValueBand_43BF00(levelIndex + 1)];
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], 252);
        }
        currentMainFlags = mainFlags;
        mainFlag8 = mainFlags & 8;
        if ((mainFlags & 8) == 0 && (mainFlags & 0x100) == 0) {
            if ((mainFlags & 0x40) != 0) {
                if ((mainFlags & 3) == 0)
                    goto AppendSkill2008ReactionTail;
                if ((mainFlags & 0x2000) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 34);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 121);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 34);
                    if (positionOrFlag) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 73);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 74);
                    }
                }
                if ((mainFlags & 0x2000) != 0) {
                    criticalPosition = (*sourcePosition);
                    if (cg::world::gBattleCurrentPosition5ABCA4 == (*sourcePosition)) {
// 原函数：0x004392A0。
AppendLocalCriticalCast:
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[criticalPosition], 256);
                        currentTarget = targetPosition;
                        goto ReloadPrimaryFlags;
                    }
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[criticalPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 51);
                } else {
                    criticalPosition = (*targetPosition);
                    if (cg::world::gBattleCurrentPosition5ABCA4 == (*targetPosition))
                        goto AppendLocalCriticalCast;
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[criticalPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 51);
                }
                currentTarget = targetPosition;
                goto ReloadPrimaryFlags;
            }
            if ((mainFlags & 0x10000000) != 0 || !primaryValue && skill != BattleSkill_Forcecut || (mainFlags & 3) == 0)
                goto AppendSkill2008ReactionTail;
            if ((mainFlags & 0x8000) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], 123);
                goto ReloadPrimaryFlags;
            }
            if ((mainFlags & 0x4000) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], 122);
                goto ReloadPrimaryFlags;
            }
            if ((mainFlags & 0x2000) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], (mainFlags & 2) != 0 ? 165 : 27);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], 121);
                goto ReloadPrimaryFlags;
            }
            if (skill != BattleSkill_Wattack) {
                if (skill != 72 && skill != BattleSkill_ShadowHand && skill != 113) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], (mainFlags & 2) != 0 ? 165 : 27);
ReloadPrimaryFlags:
                    currentMainFlags = mainFlags;
                }
AppendSkill2008ReactionTail:
                if (skill == BattleSkill_Taunt && (currentMainFlags & 0x20) == 0 && (currentMainFlags & 0x80000) == 0)
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], 65);
                goto FindMainNextTarget;
            }
            if ((mainFlags & 0x80000) == 0 || secondaryPosition == 255) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*currentTarget)], (mainFlags & 2) != 0 ? 165 : 27);
            } else {
                secondaryNextTarget = FindBattleMTarget_43B9B0(scene, secondaryPosition, 0);
                originalTargetForSecondary = (*currentTarget);
                nextTargetOrDeathFlag = secondaryNextTarget;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[originalTargetForSecondary], 209);
                effect260Secondary.position3 = level;
                effect260Secondary.position4 = secondaryGraphic;
                effect260Secondary.position = secondaryPosition;
                    effect260Secondary.flags = static_cast<std::uint32_t>(secondaryFlags);
                AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[(*currentTarget)], effect260Secondary);
                secondaryAuxIndex = 0;
                secondaryScanIndex = 0;
                secondaryScan = secondaryFirstImages;
                while (*secondaryScan <= 0) {
                    ++secondaryScanIndex;
                    ++secondaryScan;
                    if (secondaryScanIndex >= 2)
                        goto AppendSecondaryAuxiliaryEffects;
                }
                secondaryAuxIndex = secondaryScanIndex;
// 原函数：0x0043BC10。
AppendSecondaryAuxiliaryEffects:
                AppendBattleAuxEffects_43BC10(cg::world::gBattleAllEntries59E980[(*currentTarget)], secondaryPosition, secondaryFlags, secondaryGraphic, secondarySecondGraphic, secondaryFirstImages[secondaryAuxIndex]);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 0);
                if ((secondaryFlags & 0x40) != 0) {
                    if ((secondaryFlags & 3) != 0) {
                        if ((secondaryFlags & 0x2000) != 0) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 206);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 34);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 121);
                        } else {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 34);
                            if ((secondaryFlags & 0x200) != 0) {
                                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 73);
                                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 74);
                            }
                        }
                        if ((secondaryFlags & 0x2000) != 0) {
                            secondaryCriticalPosition = (*targetPosition);
                            if (cg::world::gBattleCurrentPosition5ABCA4 == (*targetPosition)) {
                                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryCriticalPosition], 256);
                            } else {
                                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryCriticalPosition], 52);
                                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 51);
                            }
                        } else if (cg::world::gBattleCurrentPosition5ABCA4 == secondaryPosition) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 256);
                        } else {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 52);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 51);
                        }
                    }
                } else if ((secondaryFlags & 0x8000) == 0) {
                    if ((secondaryFlags & 0x4000) != 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 122);
                    } else if ((secondaryFlags & 0x2000) != 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 121);
                    } else if ((secondaryFlags & 8) == 0 && (secondaryFlags & 0x10000100) == 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], (secondaryFlags & 2) != 0 ? 165 : 27);
                    }
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 123);
                }
                currentSecondaryFlags = secondaryFlags;
                if ((secondaryFlags & 0x200) != 0) {
                    AppendBattleSkillTail_424500(scene, secondaryPosition, secondaryFlags, level, nextTargetOrDeathFlag);
                    currentSecondaryFlags = secondaryFlags;
                }
                if (level || (currentSecondaryFlags & 8) != 0 || (currentSecondaryFlags & 0x100) != 0) {
                    if ((currentSecondaryFlags & 0x10000000) == 0) {
                        if ((secondaryFlags & 0x200) != 0 && nextTargetOrDeathFlag >= 0) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 3);
                            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[secondaryPosition], {nextTargetOrDeathFlag, 0U, 0, 0});
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 74);
                        } else {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 11);
                        }
                        currentSecondaryFlags = secondaryFlags; // 426050/426069实际重读完整W32。
                    }
                    if ((currentSecondaryFlags & 0x100) != 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 76);
                        currentSecondaryFlags = secondaryFlags; // 426050/426069实际重读完整W32。
                    }
                }
                if ((currentSecondaryFlags & 0x20) != 0) {
                    if ((currentSecondaryFlags & 0x2000) != 0) {
                        if (secondaryPosition != *initialPosition)
                            goto DeferSecondaryInitialDeath;
                    } else if (secondaryPosition == *initialPosition) {
DeferSecondaryInitialDeath:
                        deferredInitialDeath = 1;
                        goto FindMainNextTarget;
                    }
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 46);
                }
            }
        }
FindMainNextTarget:
        mainNextTarget = FindBattleMTarget_43B9B0(scene, (*targetPosition), 0);
        if (positionOrFlag)
            AppendBattleSkillTail_424500(scene, (*targetPosition), mainFlags, primaryValue, mainNextTarget);
        if ((mainFlags & 0x10000000) == 0 && (primaryValue || skill == BattleSkill_Forcecut || mainFlag8
            || (mainFlags & 0x100) != 0) && (mainFlags & 0xC000) == 0) {
            if ((mainFlags & 0x2000) != 0) {
                if ((*targetPosition) == *initialPosition && (*sourcePosition) != mainNextTarget)
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 11);
                if (skill == BattleSkill_DelayAttack257) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPosition], 76);
                    goto HandleMainDeath;
                }
                if (skill == BattleSkill_FirstAttack) {
                    mainCompletedActor = cg::world::gBattleAllEntries59E980[*initialPosition];
                    if (mainCompletedActor->payload.battleActor->castSkillLevel1044 >= 9)
                        goto AppendMainCompleted76;
                }
            } else {
                mainCompletedTarget = (*targetPosition);
                if ((*targetPosition) != *initialPosition && skill != 72 && skill != BattleSkill_ShadowHand && skill != BattleSkill_Assassin && skill != 113) {
                    if (mainNextTarget < 0)
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[mainCompletedTarget], 11);
                    else
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[mainCompletedTarget], 248);
                }
                if ((mainFlags & 0x100) != 0) {
                    mainCompletedActor = cg::world::gBattleAllEntries59E980[(*targetPosition)];
// 原函数：0x004392A0。
AppendMainCompleted76:
                    AppendBattleCast_4392A0(scene, mainCompletedActor, 76);
                }
            }
        }
HandleMainDeath:
        nextTargetOrDeathFlag = mainFlags & 0x20;
        if ((mainFlags & 0x20) == 0)
            goto HandleSkill72PostReaction;
        if ((mainFlags & 0x2000) != 0) {
            if ((*targetPosition) == *initialPosition) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 46);
                goto HandleSkill72PostReaction;
            }
DeferMainInitialDeath:
            deferredInitialDeath = 1;
            goto HandleSkill72PostReaction;
        }
        if ((*targetPosition) == *initialPosition)
            goto DeferMainInitialDeath;
        if (skill == BattleSkill_Assassin)
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 47);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 46);
HandleSkill72PostReaction:
        if (skill == 72 || skill == 113) {
            primaryValue = mainFlags & 1; // 426376：局部W32后段条件快照。
            if ((mainFlags & 1) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 0);
                if (!mainFlag8 && (mainFlags & 0x100) == 0 && level && (secondaryFlags & 0x40) == 0 && (secondaryFlags & 3) != 0) {
                    if ((secondaryFlags & 0x8000) == 0) {
                        if ((secondaryFlags & 0x4000) != 0) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 122);
                        } else if ((secondaryFlags & 0x2000) != 0) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], (mainFlags & 2) != 0 ? 165 : 27);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 121);
                        } else {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], (secondaryFlags & 2) != 0 ? 165 : 27);
                        }
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 123);
                    }
                }
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 32);
            }
            if ((mainFlags & 0x80000) != 0) {
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId101C = 257;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 81);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 68);
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->reactionImage1010 = 22332;
            } else {
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId101C = 258;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 81);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 68);
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->reactionImage1010 = 22333;
            }
            positionOrFlag = secondaryFlags & 0x40;
            if ((secondaryFlags & 0x40) == 0 || (secondaryFlags & 0x2000) == 0) {
                if ((mainFlags & 0x80000) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 57);
                } else if ((mainFlags & 1) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 58);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 58);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 60);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 37);
                }
            }
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
            if ((mainFlags & 1) != 0 && !mainFlag8 && (mainFlags & 0x100) == 0) {
                if (positionOrFlag && (secondaryFlags & 3) != 0) {
                    if ((secondaryFlags & 0x2000) != 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 34);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 121);
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 34);
                        if ((secondaryFlags & 0x200) != 0) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 73);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 74);
                        }
                    }
                    if ((secondaryFlags & 0x2000) != 0) {
                        skill72CriticalPosition = (*sourcePosition);
                        if (cg::world::gBattleCurrentPosition5ABCA4 == (*sourcePosition)) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[skill72CriticalPosition], 256);
                        } else {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[skill72CriticalPosition], 52);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 51);
                        }
                    } else if (cg::world::gBattleCurrentPosition5ABCA4 == (*targetPosition)) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 256);
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 52);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 51);
                    }
                }
                skill72NextTarget = FindBattleMTarget_43B9B0(scene, secondaryPosition, 0);
                if ((secondaryFlags & 0x200) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 11);
                    AppendBattleSkillTail_424500(scene, secondaryPosition, secondaryFlags, level, skill72NextTarget);
                }
                if (((secondaryFlagByte1 = static_cast<std::uint8_t>(static_cast<std::uint32_t>(secondaryFlags) >> 8U), level) || (secondaryFlags & 8) != 0 || (secondaryFlags & 0x100) != 0) && (secondaryFlags & 0xC000) == 0) {
                    initialPositionForSkill72 = initialPosition;
                    if ((secondaryFlags & 0x2000) != 0) {
                        if (secondaryPosition == *initialPosition)
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 11);
                    } else {
                        if (secondaryPosition != *initialPosition) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 11);
                            secondaryFlagByte1 = static_cast<std::uint8_t>(static_cast<std::uint32_t>(secondaryFlags) >> 8U);
                        }
                        if ((secondaryFlagByte1 & 1) != 0)
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 76);
                    }
                } else {
                    initialPositionForSkill72 = initialPosition;
                }
                if (nextTargetOrDeathFlag) {
                    initialSkill72Value = *initialPositionForSkill72;
                    if ((mainFlags & 0x2000) != 0) {
                        if (secondaryPosition == initialSkill72Value) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 46);
                            goto HandleSkill2017PostReaction;
                        }
                    } else if (secondaryPosition != initialSkill72Value) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 46);
                        goto HandleSkill2017PostReaction;
                    }
                    deferredInitialDeath = 1;
                }
            }
        }
HandleSkill2017PostReaction:
        if (skill == BattleSkill_ShadowHand) {
            primaryValue = mainFlags & 1; // 426810：同一W32局部的2017后段快照。
            if ((mainFlags & 1) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 0);
                if (!mainFlag8 && (mainFlags & 0x100) == 0 && level && (secondaryFlags & 0x40) == 0 && (secondaryFlags & 3) != 0) {
                    if ((secondaryFlags & 0x8000) == 0) {
                        if ((secondaryFlags & 0x4000) != 0) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 122);
                        } else if ((secondaryFlags & 0x2000) != 0) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], (mainFlags & 2) != 0 ? 165 : 27);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 121);
                        } else {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], (secondaryFlags & 2) != 0 ? 165 : 27);
                        }
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondaryPosition], 123);
                    }
                }
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 32);
            }
            if ((mainFlags & 0x80000) != 0) {
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId101C = 257;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 81);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 68);
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->reactionImage1010 = 22332;
            } else {
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId101C = 258;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 81);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 68);
                cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->reactionImage1010 = 22333;
            }
            if ((secondaryFlags & 0x40) == 0 || (secondaryFlags & 0x2000) == 0) {
                if ((mainFlags & 0x80000) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 57);
                } else if ((mainFlags & 1) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 58);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 58);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 60);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 37);
                }
            }
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 74);
            if ((mainFlags & 1) != 0 && !mainFlag8 && (mainFlags & 0x100) == 0) {
                skill2017Flags = secondaryFlags;
                if ((secondaryFlags & 0x40) != 0 && (secondaryFlags & 3) != 0) {
                    if ((secondaryFlags & 0x2000) != 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 34);
                        skill2017Position = lastSecondaryPosition;
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[lastSecondaryPosition], 121);
                    } else {
                        skill2017Position = lastSecondaryPosition;
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[lastSecondaryPosition], 34);
                        if ((secondaryFlags & 0x200) != 0) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[lastSecondaryPosition], 73);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[lastSecondaryPosition], 74);
                        }
                    }
                    if ((secondaryFlags & 0x2000) != 0) {
                        skill2017CriticalPosition = (*sourcePosition);
                        if (cg::world::gBattleCurrentPosition5ABCA4 == (*sourcePosition)) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[skill2017CriticalPosition], 256);
                        } else {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[skill2017CriticalPosition], 52);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 51);
                        }
                    } else if (cg::world::gBattleCurrentPosition5ABCA4 == (*targetPosition)) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[skill2017Position], 256);
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[skill2017Position], 52);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[skill2017Position], 51);
                    }
                    skill2017Flags = secondaryFlags;
                }
                skill2017NextTarget = FindBattleMTarget_43B9B0(scene, lastSecondaryPosition, 0);
                if ((skill2017Flags & 0x200) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 11);
                    AppendBattleSkillTail_424500(scene, lastSecondaryPosition, skill2017Flags, level, skill2017NextTarget);
                }
                if ((level || (skill2017Flags & 8) != 0 || (skill2017Flags & 0x100) != 0) && (skill2017Flags & 0xC000u) == 0) {
                    if ((skill2017Flags & 0x2000) != 0) {
                        if (lastSecondaryPosition == *initialPosition)
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 11);
                    } else {
                        if (lastSecondaryPosition != *initialPosition)
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[lastSecondaryPosition], 11);
                        if ((skill2017Flags & 0x100) != 0)
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[lastSecondaryPosition], 76);
                    }
                }
                if (nextTargetOrDeathFlag) {
                    if ((mainFlags & 0x2000) != 0) {
                        if (lastSecondaryPosition == *initialPosition) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 46);
                            goto ReadNextAttackRound;
                        }
                    } else if (lastSecondaryPosition != *initialPosition) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*targetPosition)], 46);
                        goto ReadNextAttackRound;
                    }
                    deferredInitialDeath = 1;
                }
            }
        } else if (skill == 81 && !mainFlag8 && (mainFlags & 0xC100) == 0 && ((mainFlags & 0x2000) == 0 || !nextTargetOrDeathFlag)) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 64);
            cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->soundId1018 = 271;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 80);
            cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->value1030 = skill81Value;
            cg::world::gBattleAllEntries59E980[(*sourcePosition)]->payload.battleActor->numberDisplayType1034 = 5;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[(*sourcePosition)], 160);
        }
ReadNextAttackRound:
        if (lastSecondaryPosition == 255)
            goto PrepareInitialTail;
        nextSourceToken = TakeBattleMInteger_438F70(scene);
        nextSource = GetBattlePosition_4237E0(nextSourceToken);
        positionOrFlag = nextSource;
        (*sourcePosition) = nextSource;
        if (nextSource == 255)
            goto PrepareInitialTail;
        if (!cg::world::gBattleAllEntries59E980[nextSource]) {
            failureScene = scene;
// 原函数：0x0043B6C0。
FinishMidRoundFailure:
            FinishBattleEvent_43B6C0(failureScene);
PrepareInitialTail:
            initialPositionAtTail = initialPosition;
            goto FinalizeInitialActor;
        }
        if (skill == BattleSkill_DelayAttack257) {
            initialPositionAtTail = initialPosition;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPosition], 76);
        } else {
            initialPositionAtTail = initialPosition;
            if (skill == BattleSkill_FirstAttack) {
                nextInitialActor = cg::world::gBattleAllEntries59E980[*initialPosition];
                if (nextInitialActor->payload.battleActor->castSkillLevel1044 >= 9)
                    AppendBattleCast_4392A0(scene, nextInitialActor, 76);
            }
        }
        nextTargetToken = TakeBattleMInteger_438F70(scene);
        nextTarget = GetBattlePosition_4237E0(nextTargetToken);
        (*targetPosition) = nextTarget;
        if (!cg::world::gBattleAllEntries59E980[nextTarget])
            break;
        mainFlags = TakeBattleMInteger_438F70(scene);
        primaryValue = TakeBattleMInteger_438F70(scene);
        if ((mainFlags & 0x800000) != 0)
            TakeBattleMAuxGraphics_43BF30(scene, 0, &primaryGraphic, &mainSecondGraphic, mainFirstImages, mainSecondImages, mainValues);
        currentTarget = targetPosition;
        secondaryPosition = -1;
        ++roundCount;
        skill = -1;
        secondaryFlags = 0;
    }
    FinishBattleEvent_43B6C0(scene);
FinalizeInitialActor:
    initialAtTail = *initialPositionAtTail;
    if (deferredInitialDeath == 1) {
        if (initialAtTail < 20) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[initialAtTail], 11);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPositionAtTail], 46);
        }
    } else {
        if (initialAtTail < 20) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[initialAtTail], 7);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPositionAtTail], 11);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPositionAtTail], 10);
        }
        if (skill == BattleSkill_DelayAttack257) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*initialPositionAtTail], 76);
        } else if (skill == BattleSkill_FirstAttack) {
            tailInitialActor = cg::world::gBattleAllEntries59E980[*initialPositionAtTail];
            if (tailInitialActor->payload.battleActor->castSkillLevel1044 >= 9)
                AppendBattleCast_4392A0(scene, tailInitialActor, 76);
        }
    }
}

// 原函数：0x00426FC0。
void TakeBattleMSkillTargets_426FC0(
    YobiAction* const scene, const std::int32_t skill, const std::int32_t level,
    const std::int32_t technique, const std::int32_t fpUsed, const std::int32_t attackMode,
    std::int32_t* const sourcePosition, std::int32_t* const initialPosition,
    std::int32_t* const targetPosition, std::int32_t* const redirectedPosition)
{
    const std::int32_t source = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    *initialPosition = source;
    *sourcePosition = source;
    YobiAction* const first = cg::world::gBattleAllEntries59E980[source];
    if (first == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    first->payload.battleActor->castSkill103C = skill;
    cg::world::gBattleAllEntries59E980[*sourcePosition]->payload.battleActor->castLevel1040 = level;
    cg::world::gBattleAllEntries59E980[*sourcePosition]->payload.battleActor->castSkillLevel1044 = technique;
    cg::world::gBattleAllEntries59E980[*sourcePosition]->payload.battleActor->fpUsed1008 = fpUsed;
    cg::world::gBattleAllEntries59E980[*sourcePosition]->payload.battleActor->attackModeFF4 = attackMode;
    const std::int32_t initialImage = SelectIntValue_404D90(kBattleSkillImages54D710.data(), 5, technique);
    cg::world::gBattleAllEntries59E980[*sourcePosition]->payload.battleActor->image102C = initialImage;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*sourcePosition], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*sourcePosition], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*sourcePosition], 151);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*sourcePosition], 64);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*sourcePosition], 255);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*sourcePosition], 36);
    for (;;) {
        const std::int32_t target = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetFinished = target == 255;
        *targetPosition = target;
        if (targetFinished) {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[*sourcePosition], -1, 0, 0);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*sourcePosition], 74);
            return;
        }
        if (cg::world::gBattleAllEntries59E980[target] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::uint32_t flags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t value = TakeBattleMInteger_438F70(scene);
        std::int32_t firstImage = 0;
        std::int32_t secondImage = 0;
        std::array<std::int32_t, 2> firstImages {};
        std::array<std::int32_t, 2> secondImages {};
        std::array<std::int32_t, 2> values {};
        if ((flags & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        const std::int32_t effectTarget = *targetPosition;
        const std::int32_t effectSource = *sourcePosition;
        YobiAction* const effectActor = cg::world::gBattleAllEntries59E980[effectSource];
        AppendBattleEffect_439400(effectActor, {effectTarget, flags, value, firstImage});
        // 原4271B7逐槽扫描：取最后一个零；两项均非零时仍用槽0。
        std::int32_t selected = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (values[index] == 0) {
                selected = index;
            }
        }
        const std::int32_t selectedImage = firstImages[selected];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::int32_t auxiliaryTarget = *targetPosition;
        const std::int32_t auxiliarySource = *sourcePosition;
        YobiAction* const auxiliaryActor = cg::world::gBattleAllEntries59E980[auxiliarySource];
        AppendBattleAuxEffects_43BC10(auxiliaryActor, auxiliaryTarget, flags,
            auxiliaryFirstImage, auxiliarySecondImage, selectedImage);
        const std::int32_t* const images = *targetPosition < 10
            ? kBattleSkillImages54D724.data() : kBattleSkillImages54D710.data();
        const std::int32_t image = SelectIntValue_404D90(images, 5, technique);
        cg::world::gBattleAllEntries59E980[*sourcePosition]->payload.battleActor->image102C = image;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 25);
        if ((flags & 0x200u) != 0) {
            const std::int32_t redirect = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            *redirectedPosition = redirect;
            const std::int32_t redirectTarget = *targetPosition;
            const std::int32_t redirectSource = *sourcePosition;
            static_cast<void>(AppendBattleRedirect_4242B0(
                scene, redirectSource, redirectTarget, flags, value, redirect));
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 11);
            *targetPosition = *redirectedPosition;
        }
        if ((flags & (0x8u | 0x100u)) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 11);
        } else if ((flags & (0x40u | 0x80u)) != 0) {
            if ((flags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 34);
            }
            if ((flags & 0x80u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 35);
            }
            const std::int32_t currentTarget = *targetPosition;
            if (cg::world::gBattleCurrentPosition5ABCA4 == currentTarget) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[currentTarget], 256);
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[currentTarget], 52);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 51);
            }
        } else if ((flags & 0x8000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 129);
        } else if ((flags & 0x4000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 128);
        } else if ((flags & 0x2000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 127);
        } else if ((flags & 0x40000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 126);
        } else if ((flags & 0x20000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 125);
        } else if ((flags & 0x10000u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 124);
        } else if ((flags & 3u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], (flags & 2u) != 0 ? 165 : 27);
        }
        if ((flags & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[*targetPosition], 46);
        }
    }
}

static constexpr std::array<std::int32_t, 6> kBattleMSkill2007Images54D5F4 { // 原常量参考：0x0054D5F4。
    111067, 111068, 111069, 111070, 111071, 111072,
};

// 原435B63..43695E及42B26D..42B282；caller先分流mode0和72/113/2017至完整424620攻击生产。
[[maybe_unused]] void ProcessBattleMSkillAttackNonzeroMode(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::int32_t fpUsed, const std::int32_t mode)
{
    if (mode < 0 || mode > 4) {
        ParseBattleMRide_4239D0(scene);
        ++payload->packetSubState58;
        return;
    }
    const std::int32_t parsedCasterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCasterPosition;
    gBattleMValue5ABA7C = parsedCasterPosition;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCasterPosition];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        ParseBattleMRide_4239D0(scene);
        ++payload->packetSubState58;
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    const std::int32_t casterSkill = gBattleMSkill5AAA78;
    casterPayload->castSkill103C = casterSkill;
    // E09／435BB5：第二等级使用当前技能等级，1044仍保存技术等级索引。
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->castLevel1040 = gBattleMSkillLevel5A4A2C;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->attackModeFF4 = mode;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    std::int32_t skill = gBattleMSkill5AAA78;
    if (skill == BattleSkill_ContinuationAttack || skill == BattleSkill_Parameter_DoubleEdgedBlade
        || skill == BattleSkill_Parameter_AllOrNothing || skill == BattleSkill_Forcecut || skill == 81 || skill == 84
        || skill == 85 || skill == 87 || skill == BattleSkill_GuardBreak || skill == BattleSkill_ShortStrike
            || skill == BattleSkill_RandomShot || skill == 3019
        || skill == 4002 || skill == 3038 || skill == 110 || skill == BattleSkill_Taunt || skill == BattleSkill_MercyStrike
        || (skill >= BattleSkill_StatusAttackPoison && skill <= BattleSkill_StatusAttackForget)
            || (skill >= BattleSkill_StatusAttackPoison2096 && skill <= BattleSkill_StatusAttackForget2101)) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
        skill = gBattleMSkill5AAA78;
    } else if (skill == BattleSkill_Ranbu) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->flash1004 = 1;
        skill = gBattleMSkill5AAA78;
    } else if (skill == BattleSkill_Recklessness) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
        const std::int32_t band = GetBattleValueBand_43BF00(gBattleMSkillLevelIndex5A3A18 + 1);
        const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
        auto* const imageCasterPayload = imageCaster->payload.battleActor;
        const std::int32_t image = kBattleMSkill2007Images54D5F4[band];
        imageCasterPayload->image102C = image;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 252);
        skill = gBattleMSkill5AAA78;
    } else if (skill >= BattleSkill_DelayAttack257 && skill < 4004) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 151);
        skill = gBattleMSkill5AAA78;
        if (skill == BattleSkill_DelayAttack258) {
            const std::int32_t firstXIndex = cg::world::gBattlePointIndices5536FC[15];
            const std::int32_t secondXIndex = cg::world::gBattlePointIndices5536FC[5];
            const std::int32_t firstX = cg::world::gBattleGraphicPoints553758[firstXIndex][2];
            const std::int32_t middleXCasterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t secondX = cg::world::gBattleGraphicPoints553758[secondXIndex][2];
            const std::int32_t middleXSum = firstX + secondX;
            YobiAction* const middleXCaster = cg::world::gBattleAllEntries59E980[middleXCasterPosition];
            const std::int32_t middleX = middleXSum / 2;
            middleXCaster->payload.battleActor->moveTargetXFE8 = middleX;
            const std::int32_t firstYIndex = cg::world::gBattlePointIndices5536FC[15];
            const std::int32_t secondYIndex = cg::world::gBattlePointIndices5536FC[5];
            const std::int32_t firstY = cg::world::gBattleGraphicPoints553758[firstYIndex][3];
            const std::int32_t secondY = cg::world::gBattleGraphicPoints553758[secondYIndex][3];
            const std::int32_t middleYSum = firstY + secondY;
            const std::int32_t middleYCasterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t middleY = middleYSum / 2;
            YobiAction* const middleYCaster = cg::world::gBattleAllEntries59E980[middleYCasterPosition];
            middleYCaster->payload.battleActor->moveTargetYFEC = middleY;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 8);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 6);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 10);
            skill = gBattleMSkill5AAA78;
        }
    }
    if (gBattleMSkillLevelIndex5A3A18 >= 9) {
        if (skill == BattleSkill_Madmax) {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 0, 0, 15);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 245);
        } else if (skill == BattleSkill_StatusAttackPoisonStrike) {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 5, 0, 15);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 245);
        } else if (skill == BattleSkill_DelayAttack258) {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 3, 0, 15);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 245);
        }
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    if (mode == 4) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 18);
    } else if (mode == 3) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 15);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 43);
    } else if (mode == 2) {
        const std::int32_t modeSkill = gBattleMSkill5AAA78;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64],
            modeSkill == 4002 || modeSkill == 3038 ? 19 : 15);
    }
    std::int32_t selfPosition = -1;
    std::uint32_t selfFlags = 0;
    std::int32_t selfAmount = 0;
    std::int32_t selfFirstImage = 0;
    std::int32_t selfSecondImage;
    std::int32_t redirectPosition = 0; // selfFlags的0x200分支先解析，尾部恢复同一标志后才读取。
    std::array<std::int32_t, 2> selfFirstImages {};
    std::array<std::int32_t, 2> selfSecondImages {};
    std::array<std::int32_t, 2> selfValues {};
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
        if (parsedTargetPosition == gBattleMCasterPosition5AAA64) {
            selfPosition = parsedTargetPosition;
            selfFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
            selfAmount = TakeBattleMInteger_438F70(scene);
            if ((selfFlags & 0x800000u) != 0) {
                TakeBattleMAuxGraphics_43BF30(scene, 0, &selfFirstImage, &selfSecondImage,
                    selfFirstImages.data(), selfSecondImages.data(), selfValues.data());
            }
            if ((selfFlags & 0x200u) != 0) {
                redirectPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            }
            continue;
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
                    const std::int32_t feedbackSkill = gBattleMSkill5AAA78;
                    if ((feedbackSkill == BattleSkill_RandomShot || feedbackSkill == 3019) && (flags & 0x20u) == 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 29);
                    } else if ((feedbackSkill == 4002 || feedbackSkill == 3038) && (flags & 0x20u) == 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 30);
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                            (flags & 2u) != 0 ? 165 : 27);
                    }
                    flags = gBattleMResultFlags5A4A20;
                }
            }
        }
        if ((amount != 0 || (flags & 0x108u) != 0) && (flags & 0x2000E000u) == 0) {
            const std::int32_t feedbackSkill = gBattleMSkill5AAA78;
            const bool specialFeedback = feedbackSkill == BattleSkill_RandomShot || feedbackSkill == 3019
                || feedbackSkill == 4002 || feedbackSkill == 3038;
            if (specialFeedback && (flags & 0x20u) == 0) {
                continue;
            }
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
            if (!specialFeedback && (flags & 0x100u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    if (mode == 3) {
        const std::int32_t finalSkill = gBattleMSkill5AAA78;
        if ((finalSkill == BattleSkill_Madmax || finalSkill == BattleSkill_KarmaCycle) && selfPosition >= 0) {
            const std::int32_t selfCasterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t restoredFirstImage = selfFirstImage;
            YobiAction* const selfCaster = cg::world::gBattleAllEntries59E980[selfCasterPosition];
            gBattleMTargetPosition5AAA40 = selfCasterPosition;
            gBattleMResultFlags5A4A20 = selfFlags;
            firstImage = restoredFirstImage;
            AppendBattleEffect_439400(selfCaster, {-1, selfFlags, 0, 0});
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
            std::int32_t tailAmount;
            if ((gBattleMResultFlags5A4A20 & 0x200u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 0);
                tailAmount = selfAmount;
                const std::uint32_t redirectFlags = gBattleMResultFlags5A4A20;
                const std::int32_t redirectTargetPosition = gBattleMTargetPosition5AAA40;
                YobiAction* const redirectActor = cg::world::gBattleAllEntries59E980[redirectPosition];
                AppendBattleEffectList_4393A0(redirectActor, redirectTargetPosition, redirectFlags, tailAmount);
                YobiAction* const redirectFieldActor = cg::world::gBattleAllEntries59E980[redirectPosition];
                const std::int32_t redirectFieldTargetPosition = gBattleMTargetPosition5AAA40;
                redirectFieldActor->payload.battleActor->targetPosition100C
                    = redirectFieldTargetPosition;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 41);
                cg::world::gBattleAllEntries59E980[redirectPosition]->payload.battleActor->soundId1018 = 252;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 80);
                cg::world::gBattleAllEntries59E980[redirectPosition]->payload.battleActor
                    ->reactionImage1010 = 22344;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 69);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 36);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 75);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 7);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 4);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 44);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 73);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 10);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 76);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 64);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 25);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 73);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 74);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[redirectPosition], 0);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 28);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 64);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 74);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 43);
                std::uint32_t redirectEffectFlags = gBattleMResultFlags5A4A20;
                const std::int32_t redirectEffectImage = firstImage;
                redirectEffectFlags &= ~0x200u;
                gBattleMResultFlags5A4A20 = redirectEffectFlags;
                const std::int32_t redirectEffectCasterPosition = gBattleMCasterPosition5AAA64;
                YobiAction* const redirectEffectCaster = cg::world::gBattleAllEntries59E980[redirectEffectCasterPosition];
                AppendBattleEffect_439400(
                    redirectEffectCaster, {redirectPosition, redirectEffectFlags, 0, redirectEffectImage});
                gBattleMTargetPosition5AAA40 = redirectPosition;
            } else {
                tailAmount = selfAmount;
            }
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 24);
            const std::int32_t casterEffectImage = firstImage;
            const std::uint32_t casterEffectFlags = gBattleMResultFlags5A4A20;
            const std::int32_t casterEffectSourcePosition = gBattleMCasterPosition5AAA64;
            const std::int32_t casterEffectTargetPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const casterEffectTarget = cg::world::gBattleAllEntries59E980[casterEffectTargetPosition];
            AppendBattleEffect_439400(
                casterEffectTarget, {casterEffectSourcePosition, casterEffectFlags, tailAmount, casterEffectImage});
            const std::int32_t targetEffectImage = firstImage;
            const std::uint32_t targetEffectFlags = gBattleMResultFlags5A4A20;
            const std::int32_t targetEffectPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const targetEffectActor = cg::world::gBattleAllEntries59E980[targetEffectPosition];
            AppendBattleEffect_439400(
                targetEffectActor, {targetEffectPosition, targetEffectFlags, tailAmount, targetEffectImage});
            // 原436719读取普通目标附图数组，并非刚保存的自身数组。
            std::int32_t imageIndex = 0;
            for (std::int32_t index = 0; index < 2; ++index) {
                if (firstImages[index] > 0) {
                    imageIndex = index;
                    break;
                }
            }
            const std::int32_t finalAuxImage = firstImages[imageIndex];
            const std::int32_t finalAuxSecondImage = secondImage;
            const std::int32_t finalAuxFirstImage = firstImage;
            const std::uint32_t finalAuxFlags = gBattleMResultFlags5A4A20;
            const std::int32_t finalAuxTargetPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const finalAuxTarget = cg::world::gBattleAllEntries59E980[finalAuxTargetPosition];
            AppendBattleAuxEffects_43BC10(finalAuxTarget, finalAuxTargetPosition,
                finalAuxFlags, finalAuxFirstImage, finalAuxSecondImage, finalAuxImage);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 221);
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
            const std::uint32_t flags = gBattleMResultFlags5A4A20;
            if ((flags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                const std::int32_t comparisonTargetPosition = gBattleMTargetPosition5AAA40;
                if (cg::world::gBattleCurrentPosition5ABCA4 == comparisonTargetPosition) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[comparisonTargetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
            } else if ((flags & 0x8000u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 123);
            } else if ((flags & 0x4000u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 122);
            } else if ((flags & 0x2000u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 121);
            } else if (tailAmount != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                    (flags & 2u) != 0 ? 165 : 27);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            }
            // 原4368A2门禁只读flags最低W8，其余位视图仍沿完整W32样本。
            const std::uint8_t finalFlags = *reinterpret_cast<const std::uint8_t*>(&gBattleMResultFlags5A4A20);
            if ((finalFlags & 0x20u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
            }
        } else {
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 24);
        }
    } else {
        AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 74);
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原43699B..4369F9；3101沿426FC0生产，收尾重取initial输出而非source。
[[maybe_unused]] void ProcessBattleMSkill3101(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed)
{
    const std::int32_t attackMode = TakeBattleMInteger_438F70(scene);
    const std::int32_t levelIndex = gBattleMSkillLevelIndex5A3A18;
    const std::int32_t level = gBattleMSkillLevel5A4A2C;
    const std::int32_t skill = gBattleMSkill5AAA78;
    TakeBattleMSkillTargets_426FC0(scene, skill, level, levelIndex, fpUsed, attackMode,
        &gBattleMCasterPosition5AAA64, &gBattleMValue5ABA7C, &gBattleMTargetPosition5AAA40, &gBattleMRedirectPosition5AAA70);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMValue5ABA7C]);
    ParseBattleMRide_4239D0(scene);
    payload->packetSubState58 = 1;
}

// 原42AE7A..42B229及42B26D..42B282；caller已读mode并分流0，不提前首对象的后置空检查。
[[maybe_unused]] void ProcessBattleMSkill73NonzeroMode(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t mode)
{
    if (mode < 0 || mode > 4) {
        ParseBattleMRide_4239D0(scene);
        ++payload->packetSubState58;
        return;
    }
    const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t skill = gBattleMSkill5AAA78;
    gBattleMCasterPosition5AAA64 = position;
    gBattleMValue5ABA7C = position;
    cg::world::gBattleAllEntries59E980[position]->payload.battleActor->castSkill103C = skill;
    const std::int32_t levelCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const levelCaster = cg::world::gBattleAllEntries59E980[levelCasterPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    levelCaster->payload.battleActor->castSkillLevel1044 = casterLevel;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        ParseBattleMRide_4239D0(scene);
        ++payload->packetSubState58;
        return;
    }
    caster->payload.battleActor->attackModeFF4 = mode;
    if (mode == 2) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 15);
    } else if (mode == 3) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 15);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 43);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 24);
    } else if (mode == 4) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 18);
    }
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
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 42B253/43576F/436983共用九参生产尾；3100写1，其余两路递增。
void ProcessBattleMSkillAttackTargets(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::int32_t skill, const std::int32_t skillLevel, const std::int32_t skillLevelIndex,
    const std::int32_t fpUsed, const bool fixedState)
{
    TakeBattleMSkillAttackTargets_424620(
        scene, skill, skillLevel, skillLevelIndex, fpUsed,
        &gBattleMCasterPosition5AAA64, &gBattleMValue5ABA7C,
        &gBattleMTargetPosition5AAA40, &gBattleMRedirectPosition5AAA70);
    MoveBattleCastState_439380(world::gBattleAllEntries59E980[gBattleMValue5ABA7C]);
    ParseBattleMRide_4239D0(scene);
    if (fixedState) {
        payload->packetSubState58 = 1;
    } else {
        ++payload->packetSubState58;
    }
}

} // namespace detail

} // namespace cg::yobi
