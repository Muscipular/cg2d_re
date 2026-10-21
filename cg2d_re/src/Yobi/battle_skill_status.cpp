#include "battle_skill_status.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_message.hpp"
#include "battle_skill.hpp"

#include "battle_scene.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"

#include <Windows.h>

#include <array>
#include <format>
#include <bit>
#include <cstdint>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::yobi {

namespace detail {

// 原2006/2010/2011/2012/2013/2015/2014/2027有效档1..5；各行原地址见共享战斗明细。
static constexpr std::array<std::array<std::int32_t, 5>, 8> kBattleMStatusBandImages {{
    {111063, 111064, 111065, 111066, 111067},
    {111078, 111079, 111080, 111081, 111082},
    {111088, 111089, 111090, 111091, 111092},
    {111083, 111084, 111085, 111086, 111087},
    {111093, 111094, 111095, 111096, 111097},
    {111111, 111112, 111113, 111114, 111115},
    {111106, 111107, 111108, 111109, 111110},
    {111142, 111143, 111144, 111145, 111146},
}};

// 原54D738..54D864，按真实等级直接索引，重复图号不取消列。
// BSI01：各行对应独立技能；仅凭图号不能确认名称，2058～2067／2089～2095仍保留技能编号。
// 2091～2095当前使用111083～111087；用户资料为111183～111187，尚未消解差异，不改表值。
static constexpr std::array<std::array<std::int32_t, 5>, 15> kBattleMStatusImages54D738 {{ // 原常量参考：0x0054D738。
    {111167, 111167, 111167, 111167, 111167},
    {111168, 111168, 111168, 111168, 111168},
    {111169, 111169, 111169, 111169, 111169},
    {111170, 111170, 111170, 111170, 111170},
    {111171, 111171, 111171, 111171, 111171},
    {111172, 111172, 111172, 111172, 111172},
    {111173, 111173, 111173, 111173, 111173},
    {111174, 111174, 111174, 111174, 111174},
    {111175, 111175, 111175, 111175, 111175},
    {111176, 111176, 111176, 111176, 111176},
    {111083, 111084, 111085, 111086, 111087},
    {111083, 111084, 111085, 111086, 111087},
    {111083, 111084, 111085, 111086, 111087},
    {111078, 111079, 111080, 111081, 111082},
    {111078, 111079, 111080, 111081, 111082},
}};

// 原54D864..54D93C，前三行2050..2052，后十五行3002..3016。
// BSI01：仅有独立技能的播放图号依据，业务名称未确认；不得按共享常驻状态图猜名。
static constexpr std::array<std::array<std::int32_t, 3>, 18> kBattleMStatusImages54D864 {{ // 原常量参考：0x0054D864。
    {111153, 111154, 111155},
    {111156, 111157, 111158},
    {111159, 111160, 111161},
    {111247, 111248, 111249},
    {111197, 111198, 111199},
    {111200, 111201, 111202},
    {111203, 111204, 111205},
    {111206, 111207, 111208},
    {111209, 111210, 111211},
    {111212, 111213, 111214},
    {111215, 111216, 111217},
    {111218, 111219, 111220},
    {111221, 111222, 111223},
    {111224, 111225, 111226},
    {111227, 111228, 111229},
    {111230, 111231, 111232},
    {111233, 111234, 111235},
    {111236, 111237, 111238},
}};

static_assert(sizeof(kBattleMStatusBandImages) + sizeof(kBattleMStatusImages54D738)
    + sizeof(kBattleMStatusImages54D864) == 676);

// 原432ECE..4340FE通用状态生产；不同成功门禁、替换动作和动态技能路径分别保留。
[[maybe_unused]] void ProcessBattleMSkill32Family(
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
    const std::int32_t initialSkill = gBattleMSkill5AAA78;
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    if (initialSkill == 86) {
        if (level < 12) {
            const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
            const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t initialImage = imageOffset + 110052;
            YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
            imageCaster->payload.battleActor->image1028 = initialImage;
        } else {
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->image1028 = 111001;
        }
    } else if ((initialSkill >= BattleSkill_LpRecovery && initialSkill <= BattleSkill_LpRecoverySuper)
        || initialSkill == BattleSkill_Revive) {
        if (level < 12) {
            const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
            const std::int32_t initialImage = imageOffset + 110081;
            YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
            imageCaster->payload.battleActor->image1028 = initialImage;
        } else {
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->image1028 = 111014;
        }
    } else {
        if (level < 12) {
            const std::int32_t imageCasterPosition = gBattleMCasterPosition5AAA64;
            const std::int32_t imageOffset = kBattleMSkillEffectOffsets553E58[level];
            const std::int32_t initialImage = imageOffset + 110056;
            YobiAction* const imageCaster = cg::world::gBattleAllEntries59E980[imageCasterPosition];
            imageCaster->payload.battleActor->image1028 = initialImage;
        } else {
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->image1028 = 111018;
        }
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
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
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x80000u) != 0) {
            const std::int32_t appendTargetPosition = gBattleMTargetPosition5AAA40;
            const std::int32_t appendLevel = gBattleMSkillLevel5A4A2C;
            const std::int32_t appendSkill = gBattleMSkill5AAA78;
            AddBattleAppendEffect_43BE00(appendSkill, appendLevel, appendTargetPosition);
            flags = gBattleMResultFlags5A4A20;
        }
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
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
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->numberDisplayType1034 = -1;
        const std::int32_t skill = gBattleMSkill5AAA78;
        if ((skill >= BattleSkill_ReflectionPhysics && skill <= BattleSkill_IneffectiveMagic)
            || (skill >= BattleSkill_LpRecovery && skill <= BattleSkill_StatusRecover)) {
            const std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
            const std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
            const bool sameSide = (sourcePosition == 20 && targetPosition < 10)
                || (sourcePosition == 21 && targetPosition >= 10);
            const std::int32_t state = skill <= BattleSkill_IneffectiveMagic ? (sameSide ? 190 : 120)
                : skill <= BattleSkill_LpRecoverySuper ? (sameSide ? 180 : 130) : (sameSide ? 183 : 133);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], state);
            if (skill == BattleSkill_StatusRecover && (gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 142);
            }
        } else if (skill >= BattleSkill_StatusChangePoison && skill <= BattleSkill_StatusChangeForgetSuper) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 119);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) == 0) {
                continue;
            }
            const std::int32_t successfulSkillPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const successfulSkillActor = cg::world::gBattleAllEntries59E980[successfulSkillPosition];
            const std::int32_t successfulSkill = gBattleMSkill5AAA78;
            successfulSkillActor->payload.battleActor->castSkill103C = successfulSkill;
            const std::int32_t currentSkill = gBattleMSkill5AAA78;
            const std::int32_t type = currentSkill >= BattleSkill_StatusChangePoison
                && currentSkill <= BattleSkill_StatusChangeForgetSuper
                ? (currentSkill - BattleSkill_StatusChangePoison) % CG_BATTLE_DEBUFF_TYPE_COUNT + CG_BATTLE_DEBUFF_POISON
                : CG_BATTLE_DEBUFF_NONE;
            YobiAction* const target = cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40];
            auto* const targetPayload = target->payload.battleActor;
            if (targetPayload->debuffState13CC != 2 || targetPayload->debuffType13D0 != type) {
                AppendBattleCast_4392A0(scene, target, 142);
            }
            targetPayload->debuffType13D0 = type;
            targetPayload->debuffState13CC = 2;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 141);
        } else if (skill == BattleSkill_ReverseType) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 146);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 147);
            }
        } else if (skill == BattleSkill_Revive) {
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
        } else if (skill >= BattleSkill_DefUp && skill <= BattleSkill_AglDown) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 157);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) == 0) {
                continue;
            }
            const std::int32_t successfulSkillPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const successfulSkillActor = cg::world::gBattleAllEntries59E980[successfulSkillPosition];
            const std::int32_t successfulSkill = gBattleMSkill5AAA78;
            successfulSkillActor->payload.battleActor->castSkill103C = successfulSkill;
            switch (gBattleMSkill5AAA78) {
            case BattleSkill_DefUp: statusType = CG_BATTLE_PARAMETER_EFFECT_DEFENSE_UP; break;
            case BattleSkill_DefDown: statusType = CG_BATTLE_PARAMETER_EFFECT_DEFENSE_DOWN; break;
            case BattleSkill_AtkUp: statusType = CG_BATTLE_PARAMETER_EFFECT_ATTACK_UP; break;
            case BattleSkill_AtkDown: statusType = CG_BATTLE_PARAMETER_EFFECT_ATTACK_DOWN; break;
            case BattleSkill_AglUp: statusType = CG_BATTLE_PARAMETER_EFFECT_AGILITY_UP; break;
            case BattleSkill_AglDown: statusType = CG_BATTLE_PARAMETER_EFFECT_AGILITY_DOWN; break;
            case BattleSkill_RecoveryUp: statusType = CG_BATTLE_PARAMETER_EFFECT_RECOVERY_UP; break;
            default: break;
            }
            auto* const targetPayload =
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor;
            if (targetPayload->statusEffectMode13D8 != CG_BATTLE_STATUS_MODE_PARAMETER || targetPayload->statusEffectType13DC != statusType) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 144);
            }
            targetPayload->statusEffectType13DC = statusType;
            targetPayload->statusEffectMode13D8 = CG_BATTLE_STATUS_MODE_PARAMETER;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 143);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
        } else if (skill == 86) {
            const std::int32_t technique = gBattleMTechnique5A4A38 / 100;
            const std::int32_t state = technique == 2053 ? 135 : technique == 2054 ? 136 : 134;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], state);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 47);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 45);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
            }
        } else if (skill == BattleSkill_Concentration || (skill >= BattleSkill_ElementalAmplification && skill <= BattleSkill_BideTime)
            || skill == BattleSkill_Adrenaline) {
            if (skill == BattleSkill_VoodooCurse) {
                const auto output = std::format(L"performIndex = {}", gBattleMSkillLevelIndex5A3A18);
                ::OutputDebugStringW(output.c_str());
                if ((gBattleMResultFlags5A4A20 & 0x80000u) == 0) {
                    continue;
                }
            }
            std::int32_t row = 0;
            switch (skill) {
            case BattleSkill_ElementalAmplification: row = 1; break;
            case BattleSkill_Blessing: row = 2; break;
            case BattleSkill_DefensiveBarrier: row = 3; break;
            case BattleSkill_VoodooCurse: row = 4; break;
            case BattleSkill_BideTime: row = 5; break;
            case BattleSkill_BattleCommand: row = 6; break;
            case BattleSkill_Adrenaline: row = 7; break;
            default: break;
            }
            const std::int32_t band = GetBattleValueBand_43BF00(gBattleMSkillLevelIndex5A3A18 + 1);
            const std::int32_t bandTargetPosition = gBattleMTargetPosition5AAA40;
            if (skill == BattleSkill_Concentration || skill == BattleSkill_Blessing || skill == BattleSkill_VoodooCurse
                || skill == BattleSkill_Adrenaline) {
                const std::int32_t image = kBattleMStatusBandImages[row][band - 1];
                YobiAction* const bandTarget = cg::world::gBattleAllEntries59E980[bandTargetPosition];
                bandTarget->payload.battleActor->image102C = image;
            } else {
                YobiAction* const bandTarget = cg::world::gBattleAllEntries59E980[bandTargetPosition];
                auto* const bandPayload = bandTarget->payload.battleActor;
                const std::int32_t image = kBattleMStatusBandImages[row][band - 1];
                bandPayload->image102C = image;
            }
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 252);
            if (skill == BattleSkill_Blessing || skill == BattleSkill_VoodooCurse || (gBattleMResultFlags5A4A20 & 0x80000u) == 0) {
                continue;
            }
            const std::int32_t type = skill == BattleSkill_Concentration ? CG_BATTLE_EXT_EFFECT_SKILL_2006
                : skill == BattleSkill_ElementalAmplification ? CG_BATTLE_EXT_EFFECT_SKILL_2010
                : skill == BattleSkill_DefensiveBarrier ? CG_BATTLE_EXT_EFFECT_SKILL_2012
                : skill == BattleSkill_BattleCommand ? CG_BATTLE_EXT_EFFECT_SKILL_2014
                : skill == BattleSkill_BideTime ? CG_BATTLE_EXT_EFFECT_SKILL_2015 : CG_BATTLE_EXT_EFFECT_SKILL_2027;
            auto* const targetPayload =
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor;
            const bool replaceState = skill != BattleSkill_BideTime && skill != BattleSkill_Adrenaline;
            if (replaceState && (targetPayload->statusEffectMode13D8 != CG_BATTLE_STATUS_MODE_EXTENDED || targetPayload->statusEffectType13DC != type)) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 144);
            }
            targetPayload->statusEffectType13DC = type;
            targetPayload->statusEffectMode13D8 = CG_BATTLE_STATUS_MODE_EXTENDED;
            if (replaceState) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 143);
            }
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
        } else if ((skill >= 2050 && skill <= 2052) || (skill >= 2058 && skill <= 2067)
            || (skill >= 2089 && skill <= 2095) || (skill >= 3002 && skill <= 3016)) {
            std::int32_t image;
            std::int32_t type;
            bool useDebuffActions = false;
            if (skill >= 2050 && skill <= 2052) {
                image = kBattleMStatusImages54D864[skill - 2050][gBattleMSkillLevelIndex5A3A18];
                type = skill - 2050 + CG_BATTLE_EXT_EFFECT_SKILL_2050;
            } else if (skill >= 2058 && skill <= 2067) {
                image = kBattleMStatusImages54D738[skill - 2058][gBattleMSkillLevelIndex5A3A18];
                type = (skill - 2058) / 2 + CG_BATTLE_EXT_EFFECT_SKILL_2058_2059;
            } else if (skill == 2089) {
                image = kBattleMStatusImages54D738[13][gBattleMSkillLevelIndex5A3A18];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2089;
                useDebuffActions = true;
            } else if (skill == 2090) {
                image = kBattleMStatusImages54D738[14][gBattleMSkillLevelIndex5A3A18];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2090;
                useDebuffActions = true;
            } else if (skill == 2091) {
                image = kBattleMStatusImages54D738[10][gBattleMSkillLevelIndex5A3A18];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2091;
            } else if (skill == 2092 || skill == 2093) {
                image = kBattleMStatusImages54D738[11][gBattleMSkillLevelIndex5A3A18];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2092_2093;
            } else if (skill == 2094 || skill == 2095) {
                image = kBattleMStatusImages54D738[12][gBattleMSkillLevelIndex5A3A18];
                type = CG_BATTLE_EXT_EFFECT_SKILL_2094_2095;
            } else {
                image = kBattleMStatusImages54D864[skill - 3002 + 3][gBattleMSkillLevelIndex5A3A18];
                type = skill - 3002 + CG_BATTLE_EXT_EFFECT_SKILL_3002;
            }
            cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
                ->image102C = image;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 252);
            if ((gBattleMResultFlags5A4A20 & 0x80000u) == 0) {
                continue;
            }
            auto* const targetPayload =
                cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor;
            if (targetPayload->statusEffectMode13D8 != CG_BATTLE_STATUS_MODE_EXTENDED || targetPayload->statusEffectType13DC != type) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                    useDebuffActions ? 142 : 144);
            }
            targetPayload->statusEffectType13DC = type;
            targetPayload->statusEffectMode13D8 = CG_BATTLE_STATUS_MODE_EXTENDED;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                useDebuffActions ? 141 : 143);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 37);
        }
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原5A4A30仅本生产段六处读写；入口覆盖，不属于场景13项恢复/回存。
static std::int32_t gBattleMHalfValue5A4A30 = 0;

// 原43412E/434130..434EDF；常规入口传entryValue=0，3037保留原EDI=1。
[[maybe_unused]] void ProcessBattleMSkill19Family(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::int32_t fpUsed, const std::int32_t entryValue,
    const std::uint32_t flags9C, const std::int32_t feedbackAlternateImageA0)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    std::int32_t halfApplied = entryValue;
    payload->completedEntryCount00 = entryValue;
    payload->entryCount04 = entryValue;
    gBattleMMessageValue59FA14 = entryValue;
    gBattleMHalfValue5A4A30 = entryValue;
    const std::int32_t parsedCaster = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCaster;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCaster];
    if (reinterpret_cast<std::uintptr_t>(caster) == static_cast<std::uintptr_t>(entryValue)) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    casterPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t casterLevelPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const casterLevelActor = cg::world::gBattleAllEntries59E980[casterLevelPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    casterLevelActor->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 139);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    if (level < 12) {
        const std::int32_t casterImagePosition = gBattleMCasterPosition5AAA64;
        const std::int32_t offset = kBattleMSkillEffectOffsets553E58[level];
        const std::int32_t image = offset + 110052;
        YobiAction* const casterImageActor = cg::world::gBattleAllEntries59E980[casterImagePosition];
        casterImageActor->payload.battleActor->image1028 = image;
    } else {
        YobiAction* const casterImageActor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        casterImageActor->payload.battleActor->image1028 = 111001;
    }
    std::int32_t* const effectImage =
        &cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->image102C;
    switch (gBattleMSkill5AAA78) {
    case BattleSkill_AttackMagicEarth: case BattleSkill_AttackMagicEarthStrong: case BattleSkill_AttackMagicEarthSuper: {
        const std::int32_t index = gBattleMSkillLevelIndex5A3A18;
        *effectImage = index < 12 ? 3 * kBattleMSkillEffectOffsets553E58[index] + 110032 : 111002;
        break;
    }
    case BattleSkill_AttackMagicWater: case BattleSkill_AttackMagicWaterStrong: case BattleSkill_AttackMagicWaterSuper: {
        const std::int32_t index = gBattleMSkillLevelIndex5A3A18;
        *effectImage = index < 12 ? 3 * kBattleMSkillEffectOffsets553E58[index] + 110020 : 111005;
        break;
    }
    case BattleSkill_AttackMagicFire: case BattleSkill_AttackMagicFireStrong: case BattleSkill_AttackMagicFireSuper: {
        const std::int32_t index = gBattleMSkillLevelIndex5A3A18;
        *effectImage = index < 12 ? 3 * kBattleMSkillEffectOffsets553E58[index] + 110140 : 111008;
        break;
    }
    case BattleSkill_AttackMagicWind: case BattleSkill_AttackMagicWindStrong: case BattleSkill_AttackMagicWindSuper: {
        const std::int32_t index = gBattleMSkillLevelIndex5A3A18;
        *effectImage = index < 12 ? kBattleMSkillEffectOffsets553E58[index] + 110044 : 111011;
        break;
    }
    case BattleSkill_Dorain: case 3037: {
        const std::int32_t index = gBattleMSkillLevelIndex5A3A18;
        *effectImage = index < 12 ? kBattleMSkillEffectOffsets553E58[index] + 110063 : 111012;
        break;
    }
    case 2055: *effectImage = 111164; break;
    case 2056: *effectImage = 111165; break;
    case 2057: *effectImage = 111166; break;
    case 2068: case 2070: *effectImage = 111177; break;
    case 2069: *effectImage = 111178; break;
    case 2071: *effectImage = 111183; break;
    case 2072: *effectImage = 111184; break;
    case 2073: *effectImage = 111185; break;
    case 2074: case 2076: *effectImage = 111186; break;
    case 2075: *effectImage = 111187; break;
    case 2077: case 2079: *effectImage = 111189; break;
    case 2078: *effectImage = 111190; break;
    default: break;
    }
    bool seenCaster = false;
    std::int32_t lastAmount;
    std::int32_t reflectedSecondImage = std::bit_cast<std::int32_t>(flags9C);
    std::int32_t reflectedAlternateImage = feedbackAlternateImageA0;
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t parsedTarget = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetFinished = parsedTarget == 255;
        gBattleMTargetPosition5AAA40 = parsedTarget;
        if (targetFinished) {
            break;
        }
        if (parsedTarget == gBattleMCasterPosition5AAA64) {
            seenCaster = true;
        }
        YobiAction* const parsedTargetActor = cg::world::gBattleAllEntries59E980[parsedTarget];
        if (parsedTargetActor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        lastAmount = TakeBattleMInteger_438F70(scene);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & 0x10000u) != 0) {
            const std::int32_t previousTotal = gBattleMMessageValue59FA14;
            const std::int32_t target = gBattleMTargetPosition5AAA40;
            gBattleMMessageValue59FA14 = previousTotal + lastAmount;
            gBattleMFeedbackEffectImage5A4A28 = firstImage;
            reflectedSecondImage = secondImage;
            gBattleMFeedbackSourcePosition5ABA90 = target;
            gBattleMFlags5ABA84 = flags;
            std::int32_t imageIndex = 0;
            for (std::int32_t index = 0; index < 2; ++index) {
                if (firstImages[index] > 0) {
                    imageIndex = index;
                    break;
                }
            }
            reflectedAlternateImage = firstImages[imageIndex];
            flags &= 0xFC1FFFDFu;
            gBattleMResultFlags5A4A20 = flags;
            payload->entryPositions08[payload->entryCount04] = target;
            ++payload->entryCount04;
            flags = gBattleMResultFlags5A4A20;
        }
        const std::int32_t effectTarget = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCaster = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCasterActor = cg::world::gBattleAllEntries59E980[effectCaster];
        AppendBattleEffect_439400(effectCasterActor, {effectTarget, flags, lastAmount, firstImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t selectedImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxiliaryTarget = gBattleMTargetPosition5AAA40;
        const std::int32_t auxiliaryCaster = gBattleMCasterPosition5AAA64;
        YobiAction* const auxiliaryActor = cg::world::gBattleAllEntries59E980[auxiliaryCaster];
        AppendBattleAuxEffects_43BC10(auxiliaryActor, auxiliaryTarget, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, selectedImage);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x108u) == 0) {
            if ((flags & 0x40u) != 0 && (flags & 0x10000u) == 0) {
                const std::int32_t skill = gBattleMSkill5AAA78;
                std::int32_t targetPosition;
                if (skill == BattleSkill_Dorain || skill == 3037) {
                    if ((flags & 0xC00u) != 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 33);
                    }
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                    std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
                    targetPosition = gBattleMTargetPosition5AAA40;
                    if (sourcePosition != targetPosition) {
                        const std::uint32_t effectListFlags = gBattleMResultFlags5A4A20;
                        YobiAction* const effectListActor = cg::world::gBattleAllEntries59E980[targetPosition];
                        AppendBattleEffectList_4393A0(
                            effectListActor, sourcePosition, effectListFlags, lastAmount);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 25);
                        targetPosition = gBattleMTargetPosition5AAA40;
                    }
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 73);
                    sourcePosition = gBattleMCasterPosition5AAA64;
                    targetPosition = gBattleMTargetPosition5AAA40;
                    if (sourcePosition != targetPosition) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 74);
                        sourcePosition = gBattleMCasterPosition5AAA64;
                        targetPosition = gBattleMTargetPosition5AAA40;
                    }
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[sourcePosition], 0);
                    const std::uint8_t halfFlags =
                        *reinterpret_cast<const std::uint8_t*>(&gBattleMResultFlags5A4A20);
                    targetPosition = gBattleMTargetPosition5AAA40;
                    sourcePosition = gBattleMCasterPosition5AAA64;
                    if ((halfFlags & 0x20u) == 0 || sourcePosition != targetPosition) {
                        if ((sourcePosition == 20 && targetPosition < 10)
                            || (sourcePosition == 21 && targetPosition >= 10)) {
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[sourcePosition], 182);
                            targetPosition = gBattleMTargetPosition5AAA40;
                        }
                        halfApplied = 1;
                        gBattleMHalfValue5A4A30 += lastAmount / 2;
                    }
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                    targetPosition = gBattleMTargetPosition5AAA40;
                }
                const bool currentTarget = targetPosition == cg::world::gBattleCurrentPosition5ABCA4;
                if (currentTarget) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
                flags = gBattleMResultFlags5A4A20;
            } else if (lastAmount != 0 && (flags & 3u) != 0) {
                if ((flags & 0x40000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 126);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x20000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 125);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x10000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 124);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x20000000u) == 0) {
                    const std::int32_t skill = gBattleMSkill5AAA78;
                    if (skill != BattleSkill_Dorain && skill != 3037) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                            (flags & 2u) != 0 ? 165 : 27);
                        flags = gBattleMResultFlags5A4A20;
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                            (flags & 0xC00u) != 0 ? 33 : 32);
                        std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
                        std::int32_t targetPosition = gBattleMTargetPosition5AAA40;
                        if (sourcePosition != targetPosition) {
                            const std::uint32_t effectListFlags = gBattleMResultFlags5A4A20;
                            YobiAction* const effectListActor = cg::world::gBattleAllEntries59E980[targetPosition];
                            AppendBattleEffectList_4393A0(
                                effectListActor, sourcePosition, effectListFlags, lastAmount);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 25);
                            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 73);
                            sourcePosition = gBattleMCasterPosition5AAA64;
                            targetPosition = gBattleMTargetPosition5AAA40;
                            if (sourcePosition != targetPosition) {
                                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[targetPosition], 74);
                                sourcePosition = gBattleMCasterPosition5AAA64;
                                targetPosition = gBattleMTargetPosition5AAA40;
                            }
                        }
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[sourcePosition], 0);
                        flags = gBattleMResultFlags5A4A20;
                        sourcePosition = gBattleMCasterPosition5AAA64;
                        targetPosition = gBattleMTargetPosition5AAA40;
                        if ((flags & 0x20u) == 0 || sourcePosition != targetPosition) {
                            if ((sourcePosition == 20 && targetPosition < 10)
                                || (sourcePosition == 21 && targetPosition >= 10)) {
                                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[sourcePosition], 182);
                                flags = gBattleMResultFlags5A4A20;
                            }
                            halfApplied = 1;
                            gBattleMHalfValue5A4A30 += lastAmount / 2;
                        }
                    }
                }
            }
        }
        if ((lastAmount != 0 || (flags & 0x108u) != 0) && (flags & 0x20070000u) == 0) {
            const std::int32_t skill = gBattleMSkill5AAA78;
            if (skill != BattleSkill_Dorain && skill != 3037) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
                flags = gBattleMResultFlags5A4A20;
                if ((flags & 0x100u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
                    flags = gBattleMResultFlags5A4A20;
                }
            }
        }
        if ((flags & 0x20u) != 0 && (flags & 0x10000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
        }
    }
    if (halfApplied == 1) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 117);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 160);
        const std::int32_t halfCaster = gBattleMCasterPosition5AAA64;
        YobiAction* const halfCasterActor = cg::world::gBattleAllEntries59E980[halfCaster];
        const std::int32_t halfAmount = gBattleMHalfValue5A4A30;
        halfCasterActor->payload.battleActor->value1030 = halfAmount;
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->numberDisplayType1034 = 5;
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
        const std::int32_t imageSourcePosition = gBattleMCasterPosition5AAA64;
        YobiAction* const imageSourceActor = cg::world::gBattleAllEntries59E980[imageSourcePosition];
        const std::int32_t imageReflectedPosition = gBattleMFeedbackSourcePosition5ABA90;
        auto* const sourcePayload = imageSourceActor->payload.battleActor;
        YobiAction* const imageReflectedActor = cg::world::gBattleAllEntries59E980[imageReflectedPosition];
        auto* const reflectedPayload = imageReflectedActor->payload.battleActor;
        reflectedPayload->image102C = sourcePayload->image102C;
        gBattleMFlags5ABA84 &= ~0x10000u;
        if (seenCaster) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 37);
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 1);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 140);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 0);
        const std::uint8_t reflectedFlags =
            *reinterpret_cast<const std::uint8_t*>(&gBattleMFlags5ABA84);
        if ((reflectedFlags & 8u) == 0) {
            if ((reflectedFlags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 34);
                const std::int32_t reflectedCasterPosition = gBattleMCasterPosition5AAA64;
                const bool currentCaster = reflectedCasterPosition == cg::world::gBattleCurrentPosition5ABCA4;
                if (currentCaster) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[reflectedCasterPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[reflectedCasterPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
                }
            } else if (lastAmount != 0) {
                const std::int32_t skill = gBattleMSkill5AAA78;
                if (skill != BattleSkill_Dorain && skill != 3037) {
                    if ((reflectedFlags & 3u) != 0) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64],
                            (gBattleMResultFlags5A4A20 & 2u) != 0 ? 165 : 27);
                    }
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64],
                        (gBattleMResultFlags5A4A20 & 0xC00u) != 0 ? 33 : 32);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
                    const std::uint8_t reflectedHalfFlags =
                        *reinterpret_cast<const std::uint8_t*>(&gBattleMFlags5ABA84);
                    if ((reflectedHalfFlags & 0x20u) == 0) {
                        const std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
                        const bool sameSide = (sourcePosition == 20 && gBattleMTargetPosition5AAA40 < 10)
                            || (sourcePosition == 21 && gBattleMTargetPosition5AAA40 >= 10);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[sourcePosition], sameSide ? 182 : 117);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 160);
                        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                            ->value1030 = lastAmount / 2;
                        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                            ->numberDisplayType1034 = 5;
                    }
                }
            }
        }
        const std::int32_t reflectedEffectImage = gBattleMFeedbackEffectImage5A4A28;
        const std::int32_t reflectedEffectAmount = gBattleMMessageValue59FA14;
        const std::uint32_t reflectedEffectFlags = gBattleMFlags5ABA84;
        const std::int32_t reflectedEffectCaster = gBattleMCasterPosition5AAA64;
        const std::int32_t reflectedEffectSource = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const reflectedEffectActor = cg::world::gBattleAllEntries59E980[reflectedEffectSource];
        AppendBattleEffect_439400(reflectedEffectActor,
            {reflectedEffectCaster, reflectedEffectFlags, reflectedEffectAmount, reflectedEffectImage});
        const std::int32_t reflectedAuxSecondImage = reflectedSecondImage;
        const std::int32_t reflectedAuxFirstImage = gBattleMFeedbackEffectImage5A4A28;
        const std::uint32_t reflectedAuxFlags = gBattleMFlags5ABA84;
        const std::int32_t reflectedAuxCaster = gBattleMCasterPosition5AAA64;
        const std::int32_t reflectedAuxSource = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const reflectedAuxActor = cg::world::gBattleAllEntries59E980[reflectedAuxSource];
        AppendBattleAuxEffects_43BC10(reflectedAuxActor, reflectedAuxCaster, reflectedAuxFlags,
            reflectedAuxFirstImage, reflectedAuxSecondImage, reflectedAlternateImage);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 74);
        if (lastAmount != 0 || (*reinterpret_cast<const std::uint8_t*>(&gBattleMFlags5ABA84) & 8u) != 0
            || (gBattleMResultFlags5A4A20 & 0x100u) != 0) {
            const std::int32_t skill = gBattleMSkill5AAA78;
            if (skill != BattleSkill_Dorain && skill != 3037) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 11);
                if ((gBattleMResultFlags5A4A20 & 0x100u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
                }
            }
        }
        if ((*reinterpret_cast<const std::uint8_t*>(&gBattleMFlags5ABA84) & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 46);
        }
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    payload->packetSubState58 = 2;
}

// 原434EDF..43573F；267保留继承累计值，每目标累计效果与最终累计尾分别生产。
[[maybe_unused]] void ProcessBattleMSkill267(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t fpUsed,
    const std::uint32_t flags9C, const std::int32_t feedbackAlternateImageA0)
{
    static_cast<void>(TakeBattleMInteger_438F70(scene));
    const std::int32_t parsedCaster = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = parsedCaster;
    YobiAction* const caster = cg::world::gBattleAllEntries59E980[parsedCaster];
    if (caster == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    auto* const casterPayload = caster->payload.battleActor;
    casterPayload->castSkill103C = gBattleMSkill5AAA78;
    const std::int32_t casterLevelPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const casterLevelActor = cg::world::gBattleAllEntries59E980[casterLevelPosition];
    const std::int32_t casterLevel = gBattleMSkillLevelIndex5A3A18;
    casterLevelActor->payload.battleActor->castSkillLevel1044 = casterLevel;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor->fpUsed1008 = fpUsed;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 25);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 55);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 40);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 240);
    const std::int32_t effectCountLevel = gBattleMSkillLevelIndex5A3A18;
    const std::int32_t effectCountPosition = gBattleMCasterPosition5AAA64;
    const std::int32_t effectOffset = kBattleMSkillEffectOffsets553E58[effectCountLevel];
    YobiAction* const effectCountActor = cg::world::gBattleAllEntries59E980[effectCountPosition];
    const std::int32_t effectCount = 4 - effectOffset;
    effectCountActor->payload.battleActor->image102C = effectCount;
    const std::int32_t level = gBattleMSkillLevelIndex5A3A18;
    if (level < 12) {
        const std::int32_t casterImagePosition = gBattleMCasterPosition5AAA64;
        const std::int32_t offset = kBattleMSkillEffectOffsets553E58[level];
        const std::int32_t image = offset + 110052;
        YobiAction* const casterImageActor = cg::world::gBattleAllEntries59E980[casterImagePosition];
        casterImageActor->payload.battleActor->image1028 = image;
    } else {
        YobiAction* const casterImageActor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        casterImageActor->payload.battleActor->image1028 = 111001;
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 64);
    gBattleMValue5ABA7C = 0;
    gBattleMFlags5ABA84 = 0;
    std::int32_t lastAmount;
    std::int32_t accumulatedSecondImage = std::bit_cast<std::int32_t>(flags9C);
    std::int32_t accumulatedAlternateImage = feedbackAlternateImageA0;
    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    for (;;) {
        const std::int32_t parsedTarget = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetFinished = parsedTarget == 255;
        gBattleMTargetPosition5AAA40 = parsedTarget;
        if (targetFinished) {
            break;
        }
        YobiAction* const parsedTargetActor = cg::world::gBattleAllEntries59E980[parsedTarget];
        if (parsedTargetActor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        lastAmount = TakeBattleMInteger_438F70(scene);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x800000u) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, 0, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & 0x12000u) != 0) {
            const std::uint32_t savedFlags = gBattleMFlags5ABA84;
            const std::int32_t previousTotal = gBattleMMessageValue59FA14;
            const std::int32_t target = gBattleMTargetPosition5AAA40;
            accumulatedSecondImage = secondImage;
            const std::uint32_t combinedFlags = savedFlags | flags;
            const std::int32_t total = previousTotal + lastAmount;
            gBattleMFlags5ABA84 = combinedFlags;
            gBattleMMessageValue59FA14 = total;
            gBattleMFeedbackEffectImage5A4A28 = firstImage;
            gBattleMFeedbackSourcePosition5ABA90 = target;
            std::int32_t imageIndex = 0;
            for (std::int32_t index = 0; index < 2; ++index) {
                if (firstImages[index] > 0) {
                    imageIndex = index;
                    break;
                }
            }
            accumulatedAlternateImage = firstImages[imageIndex];
            flags &= 0xFC1FFFDFu;
            gBattleMResultFlags5A4A20 = flags;
            payload->entryPositions08[payload->entryCount04] = target;
            ++payload->entryCount04;
            flags = gBattleMResultFlags5A4A20;
        }
        const std::int32_t effectTarget = gBattleMTargetPosition5AAA40;
        const std::int32_t effectCaster = gBattleMCasterPosition5AAA64;
        YobiAction* const effectCasterActor = cg::world::gBattleAllEntries59E980[effectCaster];
        AppendBattleEffect_439400(effectCasterActor, {effectTarget, flags, lastAmount, firstImage});
        std::int32_t imageIndex = 0;
        for (std::int32_t index = 0; index < 2; ++index) {
            if (firstImages[index] > 0) {
                imageIndex = index;
                break;
            }
        }
        const std::int32_t selectedImage = firstImages[imageIndex];
        const std::int32_t auxiliarySecondImage = secondImage;
        const std::int32_t auxiliaryFirstImage = firstImage;
        const std::uint32_t auxiliaryFlags = gBattleMResultFlags5A4A20;
        const std::int32_t auxiliaryTarget = gBattleMTargetPosition5AAA40;
        const std::int32_t auxiliaryCaster = gBattleMCasterPosition5AAA64;
        YobiAction* const auxiliaryActor = cg::world::gBattleAllEntries59E980[auxiliaryCaster];
        AppendBattleAuxEffects_43BC10(auxiliaryActor, auxiliaryTarget, auxiliaryFlags,
            auxiliaryFirstImage, auxiliarySecondImage, selectedImage);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 0);
        flags = gBattleMResultFlags5A4A20;
        if ((flags & 0x108u) == 0) {
            if ((flags & 0x40u) != 0 && (flags & 0x12000u) == 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 34);
                const std::int32_t responseTarget = gBattleMTargetPosition5AAA40;
                const bool currentTarget = responseTarget == cg::world::gBattleCurrentPosition5ABCA4;
                if (currentTarget) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[responseTarget], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[responseTarget], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 51);
                }
                flags = gBattleMResultFlags5A4A20;
            } else if (lastAmount != 0 && (flags & 3u) != 0) {
                if ((flags & 0x8000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 123);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x40000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 126);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x4000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 122);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x20000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 125);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x2000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 121);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x10000u) != 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 124);
                    flags = gBattleMResultFlags5A4A20;
                } else if ((flags & 0x20000000u) == 0) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40],
                        (flags & 2u) != 0 ? 165 : 27);
                    flags = gBattleMResultFlags5A4A20;
                }
            }
        }
        if ((lastAmount != 0 || (flags & 0x108u) != 0) && (flags & 0x2007E000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 11);
            flags = gBattleMResultFlags5A4A20;
            if ((flags & 0x100u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 46);
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & 0x12000u) != 0) {
            const std::int32_t reflectedSkillPosition = gBattleMFeedbackSourcePosition5ABA90;
            YobiAction* const reflectedSkillActor = cg::world::gBattleAllEntries59E980[reflectedSkillPosition];
            const std::int32_t reflectedSkill = gBattleMSkill5AAA78;
            reflectedSkillActor->payload.battleActor->castSkill103C = reflectedSkill;
            const std::int32_t reflectedLevelPosition = gBattleMFeedbackSourcePosition5ABA90;
            YobiAction* const reflectedLevelActor = cg::world::gBattleAllEntries59E980[reflectedLevelPosition];
            const std::int32_t reflectedLevel = gBattleMSkillLevelIndex5A3A18;
            reflectedLevelActor->payload.battleActor->castSkillLevel1044 = reflectedLevel;
            const std::int32_t imageSourcePosition = gBattleMCasterPosition5AAA64;
            YobiAction* const imageSourceActor = cg::world::gBattleAllEntries59E980[imageSourcePosition];
            const std::int32_t imageAccumulatedPosition = gBattleMFeedbackSourcePosition5ABA90;
            auto* const sourcePayload = imageSourceActor->payload.battleActor;
            YobiAction* const imageAccumulatedActor = cg::world::gBattleAllEntries59E980[imageAccumulatedPosition];
            auto* const accumulatedPayload = imageAccumulatedActor->payload.battleActor;
            accumulatedPayload->image102C = sourcePayload->image102C;
            const std::uint32_t accumulatedEffectFlags = gBattleMFlags5ABA84;
            const std::int32_t accumulatedEffectImage = gBattleMFeedbackEffectImage5A4A28;
            const std::int32_t accumulatedEffectAmount = gBattleMMessageValue59FA14;
            const std::uint32_t clearedEffectFlags = accumulatedEffectFlags & ~0x12000u;
            const std::int32_t accumulatedEffectSource = gBattleMFeedbackSourcePosition5ABA90;
            gBattleMFlags5ABA84 = clearedEffectFlags;
            const std::int32_t accumulatedEffectCaster = gBattleMCasterPosition5AAA64;
            YobiAction* const accumulatedEffectActor = cg::world::gBattleAllEntries59E980[accumulatedEffectSource];
            AppendBattleEffect_439400(accumulatedEffectActor,
                {accumulatedEffectCaster, clearedEffectFlags, accumulatedEffectAmount, accumulatedEffectImage});
            const std::int32_t accumulatedAuxImage = gBattleMFeedbackEffectImage5A4A28;
            const std::uint32_t accumulatedAuxFlags = gBattleMFlags5ABA84;
            const std::int32_t accumulatedAuxCaster = gBattleMCasterPosition5AAA64;
            const std::int32_t accumulatedAuxSource = gBattleMFeedbackSourcePosition5ABA90;
            YobiAction* const accumulatedAuxActor = cg::world::gBattleAllEntries59E980[accumulatedAuxSource];
            AppendBattleAuxEffects_43BC10(accumulatedAuxActor, accumulatedAuxCaster,
                accumulatedAuxFlags, accumulatedAuxImage, accumulatedSecondImage, accumulatedAlternateImage);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 242);
            AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], -1, 0, 0);
            const bool repeatedAccumulatedCast = gBattleMValue5ABA7C != 0;
            const std::int32_t accumulatedCastPosition = gBattleMCasterPosition5AAA64;
            YobiAction* const accumulatedCastActor = cg::world::gBattleAllEntries59E980[accumulatedCastPosition];
            AppendBattleCast_4392A0(scene, accumulatedCastActor, repeatedAccumulatedCast ? 26 : 1);
            gBattleMValue5ABA7C = 1;
        }
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    if (gBattleMMessageValue59FA14 != 0) {
        if ((*reinterpret_cast<const std::uint8_t*>(&gBattleMFlags5ABA84) & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 244);
        }
        const std::int32_t amountCaster = gBattleMCasterPosition5AAA64;
        YobiAction* const amountCasterActor = cg::world::gBattleAllEntries59E980[amountCaster];
        const std::int32_t accumulatedAmount = gBattleMMessageValue59FA14;
        amountCasterActor->payload.battleActor->value1030 = accumulatedAmount;
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->numberDisplayType1034 = 6;
        std::uint32_t flags = gBattleMFlags5ABA84;
        if ((flags & 0x10000000u) == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 159);
            flags = gBattleMFlags5ABA84;
        }
        if ((flags & 8u) == 0) {
            if ((flags & 0x40u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 34);
                const std::int32_t reflectedCasterPosition = gBattleMCasterPosition5AAA64;
                const bool currentCaster = reflectedCasterPosition == cg::world::gBattleCurrentPosition5ABCA4;
                if (currentCaster) {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[reflectedCasterPosition], 256);
                } else {
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[reflectedCasterPosition], 52);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 51);
                }
                flags = gBattleMFlags5ABA84;
            } else if ((flags & 0x10000000u) == 0 && lastAmount != 0 && (flags & 3u) != 0) {
                const std::uint32_t responseFlags = gBattleMResultFlags5A4A20;
                const std::int32_t responseCaster = gBattleMCasterPosition5AAA64;
                const std::int32_t responseState = (responseFlags & 2u) != 0 ? 165 : 27;
                YobiAction* const responseCasterActor = cg::world::gBattleAllEntries59E980[responseCaster];
                AppendBattleCast_4392A0(scene, responseCasterActor, responseState);
                flags = gBattleMFlags5ABA84;
            }
        }
        const std::int32_t finalEffectImage = gBattleMFeedbackEffectImage5A4A28;
        const std::int32_t finalEffectAmount = gBattleMMessageValue59FA14;
        const std::int32_t finalEffectCaster = gBattleMCasterPosition5AAA64;
        const std::int32_t finalEffectSource = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const finalEffectActor = cg::world::gBattleAllEntries59E980[finalEffectSource];
        AppendBattleEffect_439400(finalEffectActor,
            {finalEffectCaster, flags, finalEffectAmount, finalEffectImage});
        const std::int32_t finalAuxImage = gBattleMFeedbackEffectImage5A4A28;
        const std::uint32_t finalAuxFlags = gBattleMFlags5ABA84;
        const std::int32_t finalAuxCaster = gBattleMCasterPosition5AAA64;
        const std::int32_t finalAuxSource = gBattleMFeedbackSourcePosition5ABA90;
        YobiAction* const finalAuxActor = cg::world::gBattleAllEntries59E980[finalAuxSource];
        AppendBattleAuxEffects_43BC10(finalAuxActor, finalAuxCaster, finalAuxFlags,
            finalAuxImage, accumulatedSecondImage, accumulatedAlternateImage);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMFeedbackSourcePosition5ABA90], 74);
        flags = gBattleMFlags5ABA84;
        if ((flags & 0x10000000u) == 0
            && (lastAmount != 0 || (flags & 8u) != 0 || (gBattleMResultFlags5A4A20 & 0x100u) != 0)) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 11);
            if ((gBattleMResultFlags5A4A20 & 0x100u) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], 76);
            }
        }
        if ((*reinterpret_cast<const std::uint8_t*>(&gBattleMFlags5ABA84) & 0x20u) != 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], 46);
        }
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ParseBattleMRide_4239D0(scene);
    payload->packetSubState58 = 1;
}

} // namespace detail

} // namespace cg::yobi
