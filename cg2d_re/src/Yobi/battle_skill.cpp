#include "battle_skill.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_message.hpp"
#include "battle_skill_attack.hpp"
#include "battle_skill_status.hpp"
#include "battle_skill_special.hpp"
#include "battle_skill_effect.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/render/RendererResources.hpp"

#include "battle_scene.hpp"

#include <cstdint>

namespace cg::yobi {

namespace detail {

// 原函数：0x004242B0。
std::int32_t AppendBattleRedirect_4242B0(
    YobiAction* const scene, const std::int32_t source, const std::int32_t target,
    const std::uint32_t flags, const std::int32_t value, const std::int32_t redirect)
{
    using world::detail::gBattleAllEntries59E980;
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[source], 74);
    AppendBattleEffectList_4393A0(gBattleAllEntries59E980[source], redirect, flags, value);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[source], 25);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[source], 73);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[source], 0);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[source], 25);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 0);
    AppendBattleEffectList_4393A0(gBattleAllEntries59E980[redirect], target, flags, value);
    gBattleAllEntries59E980[redirect]->payload.battleActor->targetPosition100C = target;
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 41);
    gBattleAllEntries59E980[redirect]->payload.battleActor->value1018 = 252;
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 80);
    gBattleAllEntries59E980[redirect]->payload.battleActor->reactionImage1010 = 0x5748;
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 69);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 36);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 75);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 7);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 4);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 44);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 73);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 10);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 76);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[redirect], 74);
    AppendBattleEffectList_4393A0(gBattleAllEntries59E980[redirect], target, flags, value);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[target], 0);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[target], 28);
    AppendBattleEffectList_4393A0(gBattleAllEntries59E980[target], source, flags, value);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[target], 73);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[target], 74);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[target], 64);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[target], 1);
    return redirect;
}

// 原函数：0x00424500。
void AppendBattleSkillTail_424500(
    YobiAction* const scene, const std::int32_t position, const std::uint32_t flags,
    const std::int32_t mode, const std::int32_t target)
{
    using world::detail::gBattleAllEntries59E980;
    if (mode == 0) {
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 37);
    }
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 73);
    AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 74);
    if ((flags & 0xE000u) != 0) {
        if (target < 0) {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 11);
        } else {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 3);
            AppendBattleEffect_439400(gBattleAllEntries59E980[position], {target, 0, 0, 0});
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 74);
        }
    }
    if ((flags & 0x20u) == 0) {
        AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 40);
    }
    // 原4245BF独立尾块，不能与前面的E000h分支合并去重。
    if (mode == 0) {
        if (target < 0) {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 11);
        } else {
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 3);
            AppendBattleEffect_439400(gBattleAllEntries59E980[position], {target, 0, 0, 0});
            AppendBattleCast_4392A0(scene, gBattleAllEntries59E980[position], 74);
        }
    }
}

namespace {

// 原427470/SKL的非零子状态；由ProcessBattleMSkillHeader调用，不单独注册消息回调。
[[maybe_unused]] void ProcessBattleMSkillWaitingState(
    YobiAction* const scene, BattleScenePayload* const payload, const std::int32_t subState)
{
    using namespace world::detail;
    const std::int32_t skill = gBattleMSkill5AAA78;
    const std::int32_t value88 = gBattleMValue5A4A34;
    if (subState == 2) {
        switch (skill) {
        case 19: case 20: case 21: case 22: case 23: case 24: case 25:
        case 26: case 27: case 28: case 29: case 30: case 31:
        case 61: case 62: case 63: case 83: case 400: case 401: case 402:
        case 2028: case 2055: case 2056: case 2057:
        case 2068: case 2069: case 2070: case 2071: case 2072: case 2073:
        case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
        case 3037:
            break;
        default:
            return;
        }
        const std::int32_t count = payload->entryCount04;
        if (count == 0) {
            payload->packetSubState58 = 1;
            return;
        }
        std::int32_t readyCount = 0;
        for (std::int32_t index = 0; index < count - 1; ++index) {
            if (gBattleAllEntries59E980[payload->entryPositions08[index]]->phase == 0) {
                ++readyCount;
            }
        }
        YobiAction* const last = gBattleAllEntries59E980[payload->entryPositions08[count - 1]];
        if (last->phase == 1) {
            ++readyCount;
        }
        if (readyCount < count) {
            return;
        }
        const std::int32_t casterPhase = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->phase;
        if (casterPhase == 0 || casterPhase == 1) {
            // 原42A930传最后参与者ECX，而不是用于phase门禁的施法者。
            AdvanceBattleCastState_439340(last);
            payload->packetSubState58 = 1;
        }
        return;
    }
    if (subState != 1) {
        return;
    }

    enum class FinishKind {
        Caster, LastCaster, OnlyFinish, ClearCaster, BothCasters, CasterFlash, EffectTargets,
    };
    FinishKind kind;
    switch (skill) {
    case 0: case 1: case 3: case 5: case 6: case 72: case 73:
    case 75: case 76: case 77: case 78: case 79: case 80: case 81:
    case 84: case 85: case 87: case 94: case 95: case 96: case 105: case 110: case 113:
    case 257: case 258: case 259: case 260: case 262: case 266:
    case 2007: case 2008: case 2016: case 2017: case 2021: case 2022:
    case 2096: case 2097: case 2098: case 2099: case 2100: case 2101:
    case 3019: case 3038: case 3100: case 3101: case 4002:
        kind = FinishKind::LastCaster;
        break;
    case 19: case 20: case 21: case 22: case 23: case 24: case 25:
    case 26: case 27: case 28: case 29: case 30: case 31: case 82:
    case 267: case 400: case 401: case 402:
    case 2055: case 2056: case 2057:
    case 2068: case 2069: case 2070: case 2071: case 2072: case 2073:
    case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
    case 3029: case 3037:
        kind = FinishKind::OnlyFinish;
        break;
    case 4: case 12: case 50: case 51: case 52: case 53:
    case 61: case 62: case 63: case 83: case 88: case 93: case 97:
    case 107: case 108: case 109: case 114: case 117: case 263: case 405:
    case 2005: case 4003:
    case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39:
    case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49:
    case 54: case 55: case 56: case 57: case 58: case 59: case 60:
    case 64: case 65: case 66: case 67: case 68: case 86:
    case 99: case 100: case 101: case 102: case 103: case 104:
    case 2006: case 2010: case 2011: case 2012: case 2013: case 2014: case 2015: case 2019: case 2027:
    case 2050: case 2051: case 2052:
    case 2058: case 2059: case 2060: case 2061: case 2062: case 2063: case 2064: case 2065: case 2066: case 2067:
    case 2089: case 2090: case 2091: case 2092: case 2093: case 2094: case 2095:
    case 3001: case 3002: case 3003: case 3004: case 3005: case 3006: case 3007: case 3008:
    case 3009: case 3010: case 3011: case 3012: case 3013: case 3014: case 3015: case 3016:
        kind = FinishKind::Caster;
        break;
    case 106: case 111: case 2002:
        kind = FinishKind::ClearCaster;
        break;
    case 404: case 406:
        kind = FinishKind::BothCasters;
        break;
    case 112:
        kind = FinishKind::CasterFlash;
        break;
    case 98:
        kind = value88 == 7 ? FinishKind::EffectTargets : FinishKind::Caster;
        break;
    default:
        return;
    }
    if (CheckBattleTurnReady_4394A0(scene, skill == 68 ? 1 : 0) != 1) {
        return;
    }
    switch (kind) {
    case FinishKind::Caster:
        Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    case FinishKind::LastCaster: {
        YobiAction* const last = gBattleAllEntries59E980[gBattleMValue5ABA7C];
        if (last != nullptr) {
            auto* const lastPayload = last->payload.battleActor;
            if ((*reinterpret_cast<const unsigned char*>(&lastPayload->flagsFDC) & 2u) == 0) {
                if (gBattleMSkill5AAA78 == 105) {
                    lastPayload->flash1004 = 0;
                }
                Sub_406260(gBattleAllEntries59E980[gBattleMValue5ABA7C]);
            }
        }
        break;
    }
    case FinishKind::OnlyFinish:
        break;
    case FinishKind::ClearCaster:
        Sub_406260(gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        FinishBattleEvent_43B6C0(scene);
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->frameEvent = 0;
        gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->flags &= ~2u;
        if (render::HasLegacy3DDevice_44B610() != 0) {
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->flags &= ~0x100000u;
        } else {
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->flags &= ~0x8000u;
        }
        return;
    case FinishKind::BothCasters: {
        YobiAction* const caster = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (caster != nullptr && (*reinterpret_cast<const unsigned char*>(
                &caster->payload.battleActor->flagsFDC) & 2u) == 0) {
            Sub_406260(caster);
        }
        YobiAction* const last = gBattleAllEntries59E980[gBattleMValue5ABA7C];
        if (last != nullptr && (*reinterpret_cast<const unsigned char*>(
                &last->payload.battleActor->flagsFDC) & 2u) == 0) {
            Sub_406260(last);
        }
        break;
    }
    case FinishKind::CasterFlash: {
        YobiAction* const caster = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (caster != nullptr && (*reinterpret_cast<const unsigned char*>(
                &caster->payload.battleActor->flagsFDC) & 2u) == 0) {
            Sub_406260(caster);
            gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->flash1004 = 0;
        }
        break;
    }
    case FinishKind::EffectTargets: {
        auto* const casterPayload = gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor;
        for (BattleEntryEffectRecord* effect = casterPayload->effects1C.data(); effect->position != -1; ++effect) {
            YobiAction* const target = gBattleAllEntries59E980[effect->position];
            if (target != nullptr && target->characterAction != 10 && target->phase != 34) {
                Sub_406260(target);
            }
        }
        break;
    }
    }
    FinishBattleEvent_43B6C0(scene);
}

// 原42ACFD..42AE37；返回ECX技能，FP输出承接caller局部ESI，不接具体技能生产。
[[maybe_unused]] std::int32_t ReadBattleMSkillHeader(
    YobiAction* const scene, std::int32_t& fpUsed)
{
    const std::int32_t initialTechnique = TakeBattleMInteger_438F70(scene);
    gBattleMTechnique5A4A38 = initialTechnique;
    fpUsed = TakeBattleMInteger_438F70(scene);
    gBattleMSkill5AAA78 = TakeBattleMInteger_438F70(scene);
    // E01／42AD06、42AD1A：在原技能／等级归一化前保留报文双 ID。
    CaptureBattleSkillMeta(initialTechnique, gBattleMSkill5AAA78);
    gBattleMSkillLevel5A4A2C = TakeBattleMInteger_438F70(scene);
    std::int32_t requestedLevel = TakeBattleMInteger_438F70(scene);
    if (gBattleMTechnique5A4A38 >= 205000) {
        const std::int32_t remainder = gBattleMSkillLevel5A4A2C % 30;
        const bool belowOne = remainder < 1;
        gBattleMSkillLevel5A4A2C = remainder;
        if (belowOne) {
            gBattleMSkillLevel5A4A2C = 1;
        } else if (remainder >= 15) {
            gBattleMSkillLevel5A4A2C = 15;
        }
        if (requestedLevel < 1) {
            requestedLevel = 1;
        } else if (requestedLevel >= 15) {
            requestedLevel = 15;
        }
        gBattleMSkillLevelIndex5A3A18 = requestedLevel - 1;
        return gBattleMSkill5AAA78;
    }
    const std::int32_t skill = initialTechnique / 100;
    std::int32_t technique = initialTechnique % 100;
    gBattleMSkill5AAA78 = skill;
    const bool reduceByFifty = technique >= 50;
    gBattleMTechnique5A4A38 = technique;
    gBattleMSkillLevelIndex5A3A18 = technique;
    if (reduceByFifty) {
        technique %= 50;
        gBattleMTechnique5A4A38 = technique;
        gBattleMSkillLevelIndex5A3A18 = technique;
    } else if (technique >= 30) {
        technique %= 30;
        gBattleMTechnique5A4A38 = technique;
        gBattleMSkillLevelIndex5A3A18 = technique;
    }
    if (technique >= 14) {
        gBattleMTechnique5A4A38 = 14;
        technique = 14;
        gBattleMSkillLevelIndex5A3A18 = 14;
    } else if (technique <= 0) {
        gBattleMTechnique5A4A38 = 0;
        technique = 0;
        gBattleMSkillLevelIndex5A3A18 = 0;
    }
    gBattleMSkillLevel5A4A2C = technique + 1;
    return skill;
}

} // namespace

void ProcessBattleMSkillHeader(
    YobiAction* const scene, BattleScenePayload* const payload,
    const std::uint32_t flags9C, const std::int32_t valueA0)
{
    const std::int32_t subState = payload->packetSubState58;
    if (subState != 0) {
        ProcessBattleMSkillWaitingState(scene, payload, subState);
        return;
    }
    std::int32_t fpUsed;
    const std::int32_t skill = ReadBattleMSkillHeader(scene, fpUsed);
    switch (skill) {
    case 4:
        ProcessBattleMSkill4(scene, payload, fpUsed);
        break;
    case 61: case 62: case 63: case 83:
        ProcessBattleMSkill61Family(scene, payload, fpUsed);
        break;
    case 50: case 51: case 52: case 53: case 88:
        ProcessBattleMSkill50Family(scene, payload, fpUsed);
        break;
    case 12:
        ProcessBattleMSkill12(scene, payload, fpUsed);
        break;
    case 82:
        ProcessBattleMSkill82(scene, payload, fpUsed);
        break;
    case 97:
        ProcessBattleMSkill97(scene, payload, fpUsed);
        break;
    case 93:
        ProcessBattleMSkill93(scene, payload, fpUsed);
        break;
    case 98:
        ProcessBattleMSkill98(scene, payload, fpUsed);
        break;
    case 107:
        ProcessBattleMSkill107(scene, payload, fpUsed);
        break;
    case 108:
        ProcessBattleMSkill108(scene, payload, fpUsed);
        break;
    case 109:
        ProcessBattleMSkill109(scene, payload, fpUsed);
        break;
    case 112:
        ProcessBattleMSkill112(scene, payload, fpUsed);
        break;
    case 114:
        ProcessBattleMSkill114(scene, payload, fpUsed);
        break;
    case 117:
        ProcessBattleMSkill117(scene, payload, fpUsed);
        break;
    case 106: case 111: case 2002:
        ProcessBattleMSkill106Family(scene, payload, fpUsed);
        break;
    case 99: case 100: case 101: case 102: case 103: case 104:
    case 2019: case 3001:
        ProcessBattleMSkill99Family(scene, payload, fpUsed);
        break;
    case 263:
        ProcessBattleMSkill263(scene, payload, fpUsed);
        break;
    case 400:
        ProcessBattleMSkill400(scene, payload, fpUsed);
        break;
    case 401:
        ProcessBattleMSkill401(scene, payload, fpUsed);
        break;
    case 402:
        ProcessBattleMSkill402(scene, payload, fpUsed);
        break;
    case 405:
        ProcessBattleMSkill405(scene, payload, fpUsed);
        break;
    case 404: case 406:
        ProcessBattleMSkill404Family(scene, payload);
        break;
    case 267: case 3029:
        ProcessBattleMSkill267(scene, payload, fpUsed, flags9C, valueA0);
        break;
    case 2005: case 4003:
        ProcessBattleMSkill2005Family(scene, payload, fpUsed);
        break;
    case 2028:
        ProcessBattleMSkill2028(scene, payload, fpUsed);
        break;
    case 3101:
        ProcessBattleMSkill3101(scene, payload, fpUsed);
        break;
    case 32: case 33: case 34: case 35: case 36: case 37: case 38:
    case 39: case 40: case 41: case 42: case 43: case 44: case 45:
    case 46: case 47: case 48: case 49:
    case 54: case 55: case 56: case 57: case 58: case 59: case 60:
    case 64: case 65: case 66: case 67: case 68: case 86:
    case 2006: case 2010: case 2011: case 2012: case 2013: case 2014: case 2015:
    case 2027: case 2050: case 2051: case 2052:
    case 2058: case 2059: case 2060: case 2061: case 2062:
    case 2063: case 2064: case 2065: case 2066: case 2067:
    case 2089: case 2090: case 2091: case 2092: case 2093: case 2094: case 2095:
    case 3002: case 3003: case 3004: case 3005: case 3006:
    case 3007: case 3008: case 3009: case 3010: case 3011:
    case 3012: case 3013: case 3014: case 3015: case 3016:
        ProcessBattleMSkill32Family(scene, payload, fpUsed);
        break;
    case 19: case 20: case 21: case 22: case 23: case 24: case 25:
    case 26: case 27: case 28: case 29: case 30: case 31:
    case 2055: case 2056: case 2057:
    case 2068: case 2069: case 2070: case 2071: case 2072: case 2073:
    case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
        ProcessBattleMSkill19Family(scene, payload, fpUsed, 0, flags9C, valueA0);
        break;
    case 3037:
        ProcessBattleMSkill19Family(scene, payload, fpUsed, 1, flags9C, valueA0);
        break;
    case 73: {
        const std::int32_t mode = TakeBattleMInteger_438F70(scene);
        if (mode == 0) {
            const std::int32_t attackSkillLevelIndex = gBattleMSkillLevelIndex5A3A18;
            const std::int32_t attackSkillLevel = gBattleMSkillLevel5A4A2C;
            const std::int32_t attackSkill = gBattleMSkill5AAA78;
            ProcessBattleMSkillAttackTargets(
                scene, payload, attackSkill, attackSkillLevel, attackSkillLevelIndex, fpUsed, false);
        } else {
            ProcessBattleMSkill73NonzeroMode(scene, payload, mode);
        }
        break;
    }
    case 3100: {
        TakeBattleMInteger_438F70(scene);
        const std::int32_t attackSkillLevelIndex = gBattleMSkillLevelIndex5A3A18;
        const std::int32_t attackSkillLevel = gBattleMSkillLevel5A4A2C;
        const std::int32_t attackSkill = gBattleMSkill5AAA78;
        ProcessBattleMSkillAttackTargets(
            scene, payload, attackSkill, attackSkillLevel, attackSkillLevelIndex, fpUsed, true);
        break;
    }
    case 0: case 1: case 3: case 5: case 6: case 72:
    case 75: case 76: case 77: case 78: case 79: case 80: case 81:
    case 84: case 85: case 87: case 94: case 95: case 96:
    case 105: case 110: case 113:
    case 257: case 258: case 259: case 260: case 262: case 266:
    case 2007: case 2008: case 2016: case 2017: case 2021: case 2022:
    case 2096: case 2097: case 2098: case 2099: case 2100: case 2101:
    case 3019: case 3038: case 4002: {
        const std::int32_t mode = TakeBattleMInteger_438F70(scene);
        const std::int32_t currentSkill = gBattleMSkill5AAA78;
        if (currentSkill == 72 || currentSkill == 113 || currentSkill == 2017 || mode == 0) {
            const std::int32_t attackSkillLevelIndex = gBattleMSkillLevelIndex5A3A18;
            const std::int32_t attackSkillLevel = gBattleMSkillLevel5A4A2C;
            ProcessBattleMSkillAttackTargets(
                scene, payload, currentSkill, attackSkillLevel, attackSkillLevelIndex, fpUsed, false);
        } else {
            ProcessBattleMSkillAttackNonzeroMode(scene, payload, fpUsed, mode);
        }
        break;
    }
    default:
        break;
    }
}

} // namespace detail

} // namespace cg::yobi
