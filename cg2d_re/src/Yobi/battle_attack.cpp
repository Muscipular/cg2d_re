#include "battle_attack.hpp"
#include "cg/platform/Startup.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_impact.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/GraphicDxCache.hpp"

#include <array>
#include <bit>
#include <cstdint>
#include <optional>

namespace cg::yobi {

namespace detail {

namespace {

// 408BD4以54D438加1..5档索引；此表从首个有效S32图号54D43C开始。
constexpr std::array<std::int32_t, 5> kBattleBoomerangHitImages54D43C {111136, 111137, 111138, 111139, 111140}; // 原常量参考：0x0054D43C。

} // namespace

// B14H01：完整帧命中/四类HP-FP及六复制、十二BYTE和音效取样见doc/cg2d_battle_frame_hit_spec.md。
// 原函数：0x00409680。
void UpdateBattleAction14_409680(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    YobiAction* const caster = payload->caster1070;
    action->characterAction = 5;
    const std::int32_t event = action->frameEvent;
    action->framePlayingRate = 100;
    if (event >= 10000) {
        std::int32_t image = -1;
        std::int32_t secondaryImage = -1;
        std::int32_t direction = 0;
        const std::int32_t initialIndex = payload->effectRunningC9C;
        const std::uint32_t initialFlags = payload->effects1C[initialIndex].flags;
        if ((initialFlags & 4u) != 0) {
            image = 110000;
        } else if ((initialFlags & 0x400u) != 0) {
            image = FindBattleAppendEffectTechnique_43BEA0(8, payload->effects1C[initialIndex].position) < 12
                ? 110098 : 111048;
        } else {
            const std::int32_t skill = payload->castSkill103C;
            switch (skill) {
            case 0: case 2007: case 2008: case 2021:
                image = payload->castSkillLevel1044 < 12 ? 110133 : 111029;
                break;
            case 1: image = payload->castSkillLevel1044 < 12 ? 110134 : 111047; break;
            case 3: image = payload->castSkillLevel1044 < 12 ? 110135 : 111046; break;
            case 5: image = payload->castSkillLevel1044 < 12 ? 110139 : 111052; break;
            case 6: image = payload->castSkillLevel1044 < 12 ? 110137 : 111050; break;
            case 3019: case 3038: image = 110074; break;
            case 81: image = 110174; break;
            case 84: image = 110175; break;
            case 85: image = 110159; break;
            case 87: image = 110160; break;
            case 105: image = 110135; break;
            case 257: {
                const std::int32_t level = payload->castSkillLevel1044;
                secondaryImage = 110476;
                if (level >= 12) {
                    secondaryImage = 111051;
                } else if (level >= 9) {
                    image = 110014;
                }
                break;
            }
            case 258: secondaryImage = 110477; break;
            case 259:
                image = platform::RandomRange_46FAF0(0, 2) + 110001;
                secondaryImage = 110470;
                break;
            case 260: image = payload->castSkillLevel1044 < 12 ? 110473 : 111054; break;
            case 262:
                if (payload->castSkillLevel1044 < 12) {
                    image = 110473;
                    direction = 1;
                } else {
                    image = 111049;
                }
                break;
            case 3100:
                image = SelectIntValue_404D90(kBattleHitImages54D450.data(), 5, payload->castSkillLevel1044);
                break;
            default:
                if ((skill >= 75 && skill <= 80) || (skill >= 2096 && skill <= 2101)) {
                    image = 110074;
                } else {
                    image = platform::RandomRange_46FAF0(0, 2) + 110001;
                }
                break;
            }
        }
        YobiAction* const target = gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position];
        if (target != nullptr) {
            auto* const targetPayload = target->payload.battleActor;
            target->hidden = 0;
            TurnBattleToward_406190(target, action, 1, 0.0F);
            targetPayload->castSkill103C = -1;
            target->battleOpponent10 = gBattleAllEntries59E980[payload->battlePosition18];
            targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
                payload->effects1C[payload->effectRunningC9C].flags);
            if (action->frameEvent >= 20000 || payload->value1038 == 1) {
                const std::int32_t index = payload->effectRunningC9C;
                const std::uint32_t flags = payload->effects1C[index].flags;
                if ((flags & 0x2000u) == 0) {
                    if ((flags & 8u) != 0) {
                        target->phase = 31;
                    } else if ((flags & 0x100u) != 0) {
                        static_cast<void>(audio::PlaySe_52C450(210, action->x));
                        targetPayload->flash1004 = 1;
                        target->phase = 31;
                    } else if ((flags & 0x1000C000u) == 0
                        && (payload->effects1C[index].position3 != 0 || payload->castSkill103C == 6)) {
                        target->offset40 = static_cast<float>(targetPayload->x10);
                        target->offset44 = static_cast<float>(targetPayload->y14);
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x100000u) != 0) {
                            static_cast<void>(MarkDeath(targetPayload->effect1054));
                            const std::uint32_t targetFlags = targetPayload->flagsFDC;
                            targetPayload->effect1054 = nullptr;
                            targetPayload->flagsFDC = targetFlags & ~0x20u;
                        }
                        if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                        }
                        if (image >= 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(target, image, direction));
                        }
                        if (secondaryImage >= 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(target, secondaryImage, 0));
                        }
                        NewBattleRebirthEffects_4208F0(action, target);
                        target->phase = 26;
                        const std::int32_t currentEvent = action->frameEvent;
                        if (currentEvent >= 20000) {
                            static_cast<void>(audio::PlaySe_52C450(currentEvent - 20000, action->x));
                        } else {
                            static_cast<void>(audio::PlaySe_52C450(currentEvent - 10000, action->x));
                            ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
                        }
                    }
                    if (action->frameEvent < 20000) {
                        ++caster->payload.battleScene->value00;
                        payload->value1038 = 0;
                    }
                    goto finishHit;
                }
            }
            // 原40A910公共命中尾与40A96B播放尾不同；各早退只跳过尚未发生的命中处理。
            {
                ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
                if (payload->castSkillLevel1044 >= 9 && payload->castSkill103C == 259) {
                    static_cast<void>(NewBattleTiledEffect_41E9D0(0, 15));
                }
                const std::int32_t index = payload->effectRunningC9C;
                const std::uint32_t flags = payload->effects1C[index].flags;
                if ((flags & 8u) != 0) {
                    target->phase = 31;
                    goto finishHit;
                }
                if ((flags & 0x100u) != 0) {
                    if (targetPayload->flash1004 == 0) {
                        static_cast<void>(audio::PlaySe_52C450(210, action->x));
                        targetPayload->flash1004 = 1;
                        target->phase = 31;
                    }
                    goto finishHit;
                }
                if ((flags & 0x1000C000u) == 0
                    && (payload->effects1C[index].position3 != 0 || payload->castSkill103C == 6)) {
                    target->offset40 = static_cast<float>(targetPayload->x10);
                    target->offset44 = static_cast<float>(targetPayload->y14);
                    const std::uint32_t visualFlags = payload->effects1C[payload->effectRunningC9C].flags;
                    if ((visualFlags & 0x2000u) != 0) {
                        if ((visualFlags & 2u) != 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(action, 110008, 0));
                        }
                        if (image >= 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(target, image, direction));
                        }
                        if (secondaryImage >= 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(target, secondaryImage, 0));
                        }
                        NewBattleRebirthEffects_4208F0(action, target);
                        const std::int32_t currentIndex = payload->effectRunningC9C;
                        payload->effectFlagsFE4 = 0x2000;
                        if ((static_cast<std::uint8_t>(payload->effects1C[currentIndex].flags) & 0x20u) != 0) {
                            payload->flagsFDC |= 2u;
                            gYobiUpdateGateMode = 1;
                            payload->aliveFF8 = 1;
                        }
                        const std::int32_t currentEvent = action->frameEvent;
                        static_cast<void>(audio::PlaySe_52C450(
                            currentEvent >= 20000 ? currentEvent - 20000 : currentEvent - 10000, action->x));
                    } else {
                        if ((visualFlags & 0x100000u) != 0) {
                            static_cast<void>(MarkDeath(targetPayload->effect1054));
                            const std::uint32_t targetFlags = targetPayload->flagsFDC;
                            targetPayload->effect1054 = nullptr;
                            targetPayload->flagsFDC = targetFlags & ~0x20u;
                        }
                        if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                        }
                        if (image >= 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(target, image, direction));
                        }
                        if (secondaryImage >= 0) {
                            static_cast<void>(NewBattleRandomEffect_419FC0(target, secondaryImage, 0));
                        }
                        NewBattleRebirthEffects_4208F0(action, target);
                        if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 0x20u) != 0) {
                            targetPayload->flagsFDC |= 2u;
                            gYobiUpdateGateMode = 1;
                            targetPayload->aliveFF8 = 1;
                        }
                        const std::int32_t soundEvent = action->frameEvent;
                        const std::int32_t soundX = action->x;
                        static_cast<void>(audio::PlaySe_52C450(soundEvent - 10000, soundX));
                    }
                }
                const std::int32_t hitIndex = payload->effectRunningC9C;
                const std::uint32_t hitFlags = payload->effects1C[hitIndex].flags;
                if ((hitFlags & 0x8000u) != 0) {
                    AdvanceBattleCastState_439340(target);
                    if (caster->phase == 8) {
                        ++caster->payload.battleScene->value00;
                    }
                    const std::int32_t skill = payload->castSkill103C;
                    if (skill >= 75 && skill <= 80) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, skill + 110000));
                        }
                    } else if (skill >= 2096 && skill <= 2101) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, skill + 107979));
                        }
                    } else if (skill == 259) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                        }
                    } else if (skill == 94) {
                        const std::int32_t value = payload->value1050;
                        if (value != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, value + 110074));
                        }
                    } else if (skill == 6) {
                        const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                        if ((effect.flags & 0x10000000u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(
                                gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                        } else {
                            const std::int32_t amount = effect.position3;
                            (gBattleAllEntries59E980[effect.position]->barValue1B0 -= static_cast<std::int32_t>(amount));
                            if ((gBattleAllEntries59E980[
                                    payload->effects1C[payload->effectRunningC9C].position]->barValue1B0 < 0)) {
                                std::optional<platform::LegacyCryptInt> temporary;
                                temporary.emplace(static_cast<std::int32_t>(0));
                                const std::int32_t currentIndex = payload->effectRunningC9C;
                                const std::uint32_t temporaryEncoded = temporary->encoded;
                                YobiAction* const copyTarget = gBattleAllEntries59E980[
                                    payload->effects1C[currentIndex].position];
                                copyTarget->barValue1B0.encoded = temporaryEncoded;
                                const std::uint32_t temporaryMask = temporary->mask;
                                copyTarget->barValue1B0.mask = temporaryMask;
                                const std::uint8_t temporaryCount = temporary->readCount;
                                copyTarget->barValue1B0.readCount = temporaryCount;
                                temporary.reset();
                            }
                            const std::int32_t numberIndex = payload->effectRunningC9C;
                            const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                            const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                            const std::int32_t y = target->y - 80;
                            static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 3, 2, numberFlags));
                            if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                                static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                            }
                            static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                        }
                    }
                    goto finishHit;
                }
                if ((hitFlags & 0x4000u) != 0) {
                    AdvanceBattleCastState_439340(target);
                    const std::int32_t skill = payload->castSkill103C;
                    if (skill == 6) {
                        const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                        if ((effect.flags & 0x10000000u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(
                                gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                        } else {
                            const std::int32_t amount = effect.position3;
                            (gBattleAllEntries59E980[effect.position]->barValue1B0 -= static_cast<std::int32_t>(amount));
                            if ((gBattleAllEntries59E980[
                                    payload->effects1C[payload->effectRunningC9C].position]->barValue1B0 < 0)) {
                                std::optional<platform::LegacyCryptInt> temporary;
                                temporary.emplace(static_cast<std::int32_t>(0));
                                const std::int32_t currentIndex = payload->effectRunningC9C;
                                const std::uint32_t temporaryEncoded = temporary->encoded;
                                YobiAction* const copyTarget = gBattleAllEntries59E980[
                                    payload->effects1C[currentIndex].position];
                                copyTarget->barValue1B0.encoded = temporaryEncoded;
                                const std::uint32_t temporaryMask = temporary->mask;
                                copyTarget->barValue1B0.mask = temporaryMask;
                                const std::uint8_t temporaryCount = temporary->readCount;
                                copyTarget->barValue1B0.readCount = temporaryCount;
                                temporary.reset();
                            }
                            const std::int32_t numberIndex = payload->effectRunningC9C;
                            const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                            const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                            const std::int32_t y = target->y - 80;
                            static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 3, 2, numberFlags));
                            if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                                static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                            }
                            static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                        }
                        goto finishHit;
                    }
                    if (skill >= 75 && skill <= 80) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, skill + 110000));
                        }
                    } else if (skill >= 2096 && skill <= 2101) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, skill + 107979));
                        }
                    } else if (skill == 259) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                        }
                    } else if (skill == 94) {
                        const std::int32_t value = payload->value1050;
                        if (value != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, value + 110074));
                        }
                    }
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    if ((effect.flags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                    } else {
                        const std::int32_t amount = effect.position3;
                        (gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                        YobiAction* const compareTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        if ((compareTarget->readiness > compareTarget->maxHp)) {
                            YobiAction* const copyTarget = gBattleAllEntries59E980[
                                payload->effects1C[payload->effectRunningC9C].position];
                            copyTarget->readiness = copyTarget->maxHp;
                        }
                        const std::int32_t numberIndex = payload->effectRunningC9C;
                        const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                        const std::int32_t y = target->y - 80;
                        const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                        static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 5, 2, numberFlags));
                        if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                    }
                    if (caster->phase == 8) {
                        ++caster->payload.battleScene->value00;
                    }
                    goto finishHit;
                }
                if ((hitFlags & 0x2000u) != 0) {
                    AdvanceBattleCastState_439340(action);
                    AdvanceBattleCastState_439340(target);
                    const std::int32_t skill = payload->castSkill103C;
                    if (skill == 6) {
                        const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                        if ((effect.flags & 0x10000000u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(
                                gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                        } else {
                            auto* const fp = &action->barValue1B0;
                            ((*fp) -= static_cast<std::int32_t>(effect.position3));
                            if (((*fp) < 0)) {
                                *fp = std::int32_t{0};
                            }
                            const std::int32_t numberIndex = payload->effectRunningC9C;
                            const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                            const std::int32_t y = action->y - 80;
                            const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                            static_cast<void>(NewBattleNumber_41F1A0(action->x, y, displayed, 3, 2, numberFlags));
                            if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                                static_cast<void>(NewBattleFloatingIcon_41F4C0(action, 22454, 0, 0));
                            }
                            static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                        }
                        AdvanceBattleEffect_439470(action);
                        goto finishHit;
                    }
                    if (skill >= 75 && skill <= 80) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(action, skill + 110000));
                        }
                    } else if (skill >= 2096 && skill <= 2101) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, skill + 107979));
                        }
                    } else if (skill == 259) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(action, 110075));
                        }
                    } else if (skill == 94) {
                        const std::int32_t value = payload->value1050;
                        if (value != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(action, value + 110074));
                        }
                    }
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    if ((effect.flags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                    } else {
                        auto* const hp = &action->readiness;
                        ((*hp) -= static_cast<std::int32_t>(effect.position3));
                        if (((*hp) < 0)) {
                            *hp = std::int32_t{0};
                        }
                        const std::int32_t numberIndex = payload->effectRunningC9C;
                        const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                        const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                        const std::int32_t y = action->y - 80;
                        static_cast<void>(NewBattleNumber_41F1A0(action->x, y, displayed, 6, 2, numberFlags));
                        if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(action, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                    }
                    if (caster->phase == 8) {
                        ++caster->payload.battleScene->value00;
                    }
                    AdvanceBattleEffect_439470(action);
                    goto finishHit;
                }
                const std::int32_t skill = payload->castSkill103C;
                if (skill == 6) {
                    const BattleEntryEffectRecord& effect = payload->effects1C[hitIndex];
                    if ((hitFlags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                    } else {
                        const std::int32_t amount = effect.position3;
                        (gBattleAllEntries59E980[effect.position]->barValue1B0 -= static_cast<std::int32_t>(amount));
                        if ((gBattleAllEntries59E980[
                                payload->effects1C[payload->effectRunningC9C].position]->barValue1B0 < 0)) {
                            std::optional<platform::LegacyCryptInt> temporary;
                            temporary.emplace(static_cast<std::int32_t>(0));
                            const std::int32_t currentIndex = payload->effectRunningC9C;
                            const std::uint32_t temporaryEncoded = temporary->encoded;
                            YobiAction* const copyTarget = gBattleAllEntries59E980[
                                payload->effects1C[currentIndex].position];
                            copyTarget->barValue1B0.encoded = temporaryEncoded;
                            const std::uint32_t temporaryMask = temporary->mask;
                            copyTarget->barValue1B0.mask = temporaryMask;
                            const std::uint8_t temporaryCount = temporary->readCount;
                            copyTarget->barValue1B0.readCount = temporaryCount;
                            temporary.reset();
                        }
                        const std::int32_t numberIndex = payload->effectRunningC9C;
                        const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                        const std::int32_t y = target->y - 80;
                        const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                        static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 3, 2, numberFlags));
                        if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                    }
                } else {
                    if (skill >= 75 && skill <= 80) {
                        if ((hitFlags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, skill + 110000));
                        }
                    } else if (skill >= 2096 && skill <= 2101) {
                        if ((hitFlags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, skill + 107979));
                        }
                    }
                    const std::int32_t currentSkill = payload->castSkill103C;
                    if (currentSkill == 259) {
                        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                        }
                    } else if (currentSkill == 94) {
                        const std::int32_t value = payload->value1050;
                        if (value != 0) {
                            static_cast<void>(NewBattleDebuffEffect_41F660(target, value + 110074));
                        }
                    }
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    if ((effect.flags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                    } else {
                        const std::int32_t amount = effect.position3;
                        if (amount == 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(
                                gBattleAllEntries59E980[effect.position], 22331, 0, 0));
                        } else {
                            const std::int32_t damageSkill = payload->castSkill103C;
                            if (damageSkill != 84 && damageSkill != 85 && damageSkill != 87) {
                                (gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                                if ((gBattleAllEntries59E980[
                                        payload->effects1C[payload->effectRunningC9C].position]->readiness < 0)) {
                                    std::optional<platform::LegacyCryptInt> temporary;
                                    temporary.emplace(static_cast<std::int32_t>(0));
                                    const std::int32_t currentIndex = payload->effectRunningC9C;
                                    const std::uint32_t temporaryEncoded = temporary->encoded;
                                    YobiAction* const copyTarget = gBattleAllEntries59E980[
                                        payload->effects1C[currentIndex].position];
                                    copyTarget->readiness.encoded = temporaryEncoded;
                                    const std::uint32_t temporaryMask = temporary->mask;
                                    copyTarget->readiness.mask = temporaryMask;
                                    const std::uint8_t temporaryCount = temporary->readCount;
                                    copyTarget->readiness.readCount = temporaryCount;
                                    temporary.reset();
                                }
                                const std::int32_t numberIndex = payload->effectRunningC9C;
                                const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                                const std::int32_t y = target->y - 80;
                                const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                                static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 6, 2, numberFlags));
                                if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                                    static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                                }
                                static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                            }
                        }
                    }
                }
                AdvanceBattleCastState_439340(target);
            }
        finishHit:
            const std::int32_t finishIndex = payload->effectRunningC9C;
            if (((static_cast<std::uint8_t>(payload->effects1C[finishIndex].flags) & 8u) == 0
                    && payload->effects1C[finishIndex].position3 != 0) || payload->castSkill103C == 6) {
                payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
            }
            action->frameEvent = 0;
        }
    }
    const std::int32_t delay = payload->valueFD0;
    if (delay > 0) {
        payload->valueFD0 = delay - 1;
    }
    if (payload->valueFD0 == 0) {
        std::int32_t frames = payload->valueFD4;
        if (frames != 0) {
            if (frames >= 11) {
                AdvanceBattleCastState_439340(action);
                AdvanceBattleEffect_439470(action);
                payload->valueFD0 = 0;
                payload->valueFD4 = 0;
                return;
            }
        } else {
            if (PlayFrame_517DA0(action, 1) != 1) {
                return;
            }
            frames = payload->valueFD4;
        }
        payload->valueFD4 = frames + 1;
    }
}

// BSP01：严格终帧优先、单次事件与共同清理原循环保持，见射击生产SPEC。
// 原函数：0x004065E0。
std::int32_t ProbeBattleAttackEvent_4065E0(YobiAction* const action)
{
    std::int32_t result;
    for (;;) {
        if (PlayFrame_517DA0(action, 1) == 1) {
            result = 0;
            break;
        }
        const std::int32_t event = action->frameEvent;
        if (event >= 10000 && event < 20000) {
            result = 1;
            break;
        }
    }
    action->lastCharacterAction = -1;
    action->frameEvent = 0;
    return result;
}

// B15P01：两次独立事件门禁、child/X发布和普通音效取样见doc/cg2d_battle_boomerang_cast_producer_spec.md。
// 原函数：0x00406E10。
void UpdateBattleAction15_406E10(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    if (payload->valueFD0 == 0) {
        payload->valueFD0 = 1;
        action->characterAction = 7;
        if (ProbeBattleAttackEvent_4065E0(action) == 0) {
            action->characterAction = 5;
        }
    }
    action->framePlayingRate = 100;
    if (payload->attackModeFF4 == 3) {
        const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
        if (position != -1) {
            YobiAction* const target = gBattleAllEntries59E980[position];
            const std::int32_t sourcePosition = payload->battlePosition18;
            std::int32_t x = target->x;
            const std::int32_t y = target->y;
            if (position >= 10) {
                if (sourcePosition >= 10 && sourcePosition < 15) {
                    if (action->y <= y) {
                        x -= 100;
                    }
                } else if (action->y > y) {
                    x += 100;
                }
            } else if (sourcePosition >= 0 && sourcePosition < 5) {
                if (action->y >= y) {
                    x += 100;
                }
            } else if (action->y < y) {
                x -= 100;
            }
            TurnBattleTowardPosition_4060D0(action, static_cast<float>(x), static_cast<float>(y), 1, 0.0F);
        }
        if (PlayFrame_517DA0(action, 1) == 1) {
            AdvanceBattleCastState_439340(action);
            payload->valueFD0 = 0;
        }
        const std::int32_t gateEvent = action->frameEvent;
        if (gateEvent >= 10000 && gateEvent < 20000) {
            YobiAction* const created = NewBattleBoomerang_4195F0(action);
            const std::int32_t soundX = action->x;
            action->battleCastEffect10 = created;
            const std::int32_t soundEvent = action->frameEvent;
            static_cast<void>(audio::PlaySe_52C450(soundEvent - 10000, soundX));
            action->frameEvent = 0;
        }
        return;
    }
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position != -1) {
        TurnBattleToward_406190(action, gBattleAllEntries59E980[position], 1, 0.0F);
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
        payload->valueFD0 = 0;
    }
    const std::int32_t gateEvent = action->frameEvent;
    if (gateEvent < 10000 || gateEvent >= 20000) {
        return;
    }
    if (payload->castSkill103C == 258) {
        const bool lowLevel = payload->castSkillLevel1044 < 9;
        do {
            const std::int32_t image = lowLevel ? 29105 : (payload->castSkillLevel1044 < 12 ? 29106 : 104217);
            static_cast<void>(NewBattleHitEffect_41F5B0(
                gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 0, 0, 30, image));
            static_cast<void>(NewBattleRangeHit_419390(action, &payload->effects1C[payload->effectRunningC9C], 22090));
        } while (AdvanceBattleEffect_439470(action) != 1);
    } else {
        do {
            static_cast<void>(NewBattleRangeHit_419390(action, &payload->effects1C[payload->effectRunningC9C], 22090));
        } while (AdvanceBattleEffect_439470(action) != 1);
    }
    const std::int32_t soundEvent = action->frameEvent;
    const std::int32_t soundX = action->x;
    static_cast<void>(audio::PlaySe_52C450(soundEvent - 10000, soundX));
    action->frameEvent = 0;
}

// BSP01：阶段18单次事件/音效event-X，258与95/3019发布见doc/cg2d_battle_shot_producer_spec.md。
// 原函数：0x00408170。
void UpdateBattleAction18_408170(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 5;
    const std::int32_t skill = payload->castSkill103C;
    action->framePlayingRate = skill == 95 || skill == 3019 ? 33 : 100;
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position == -1) {
        if (PlayFrame_517DA0(action, 1) == 1) {
            AdvanceBattleCastState_439340(action);
        }
    } else {
        TurnBattleToward_406190(action, gBattleAllEntries59E980[position], 1, 0.0F);
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    const std::int32_t gateEvent = action->frameEvent;
    if (gateEvent < 10000 || gateEvent >= 20000) {
        return;
    }
    if (payload->castSkill103C == 258) {
        const std::int32_t level = payload->castSkillLevel1044;
        const std::int32_t image = level < 9 ? 29105 : (level < 12 ? 29106 : 104217);
        static_cast<void>(NewBattleHitEffect_41F5B0(
            gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 0, 0, 30, image));
    }
    static_cast<void>(NewBattleRangeHit_419390(action, &payload->effects1C[payload->effectRunningC9C], 22050));
    const std::int32_t soundEvent = action->frameEvent;
    const std::int32_t soundX = action->x;
    static_cast<void>(audio::PlaySe_52C450(soundEvent - 10000, soundX));
    static_cast<void>(AdvanceBattleEffect_439470(action));
    const std::int32_t currentSkill = payload->castSkill103C;
    if ((currentSkill == 95 || currentSkill == 3019)
        && payload->effects1C[payload->effectRunningC9C].position != -1) {
        action->frameRemain = 0;
        action->frameIndex = 0;
    }
    action->frameEvent = 0;
}

// BSP01：阶段19探测/单次事件/音效event-X，仅95重播，见射击生产SPEC。
// 原函数：0x004082F0。
void UpdateBattleAction19_4082F0(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    if (payload->valueFD0 == 0) {
        payload->valueFD0 = 1;
        action->characterAction = 7;
        if (ProbeBattleAttackEvent_4065E0(action) == 0) {
            action->characterAction = 5;
        }
    }
    action->framePlayingRate = 33;
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position == -1) {
        if (PlayFrame_517DA0(action, 1) == 1) {
            AdvanceBattleCastState_439340(action);
        }
    } else {
        TurnBattleToward_406190(action, gBattleAllEntries59E980[position], 1, 0.0F);
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    const std::int32_t gateEvent = action->frameEvent;
    if (gateEvent < 10000 || gateEvent >= 20000) {
        return;
    }
    static_cast<void>(NewBattleRangeHit_419390(action, &payload->effects1C[payload->effectRunningC9C], 22090));
    const std::int32_t soundEvent = action->frameEvent;
    const std::int32_t soundX = action->x;
    static_cast<void>(audio::PlaySe_52C450(soundEvent - 10000, soundX));
    static_cast<void>(AdvanceBattleEffect_439470(action));
    if (payload->castSkill103C == 95 && payload->effects1C[payload->effectRunningC9C].position != -1) {
        action->frameRemain = 0;
        action->frameIndex = 0;
    }
    action->frameEvent = 0;
}

// BTE01：播放/可能推进后一次事件持样，每轮新图/record投射，末声/清事件。
// 原函数：0x004070E0。
void UpdateBattleAction70_4070E0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 5;
    action->framePlayingRate = 100;
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position != -1) {
        TurnBattleToward_406190(action, world::detail::gBattleAllEntries59E980[position], 1, 0.0F);
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
    const std::int32_t event = action->frameEvent;
    if (event >= 10000 && event < 20000) {
        do {
            const std::int32_t image = payload->castSkillLevel1044 < 12 ? 110016 : 111055;
            static_cast<void>(NewBattleSpiracleShot_418B70(action, &payload->effects1C[payload->effectRunningC9C], image));
        } while (AdvanceBattleEffect_439470(action) != 1);
        static_cast<void>(audio::PlaySe_52C450(254, action->x));
        action->frameEvent = 0;
    }
}

// 原函数：0x004071C0。
void UpdateBattleAction249_4071C0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 5;
    action->framePlayingRate = 100;
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position != -1) {
        TurnBattleToward_406190(action, world::detail::gBattleAllEntries59E980[position], 1, 0.0F);
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
    if (payload->effects1C[payload->effectRunningC9C].position == -1) {
        action->frameEvent = 0;
        return;
    }
    const std::int32_t event = action->frameEvent;
    if (event < 10000 || event >= 20000) {
        return;
    }
    do {
        if (payload->castSkillLevel1044 >= 12) {
            static_cast<void>(NewBattleBlastWave_419180(action, &payload->effects1C[payload->effectRunningC9C], 111030));
        } else {
            const std::int32_t skill = payload->castSkill103C;
            if (skill == 2005) {
                static_cast<void>(NewBattleBlastWave_419180(action, &payload->effects1C[payload->effectRunningC9C], 104222));
            } else if (skill == 4003) {
                static_cast<void>(NewBattleBlastWave_419180(action, &payload->effects1C[payload->effectRunningC9C], 104238));
            }
        }
    } while (AdvanceBattleEffect_439470(action) != 1);
    static_cast<void>(audio::PlaySe_52C450(268, action->x));
    action->frameEvent = 0;
}

// 原[4072E0,408169)：动作16投射命中与退场；完整流程见doc/cg2d_battle_projectile_hit_spec.md。
void UpdateBattleAction16_4072E0(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.battleActor;
    if (payload == nullptr) {
        return;
    }
    const std::int32_t initialIndex = payload->effectRunningC9C;
    if (initialIndex < 0 || initialIndex >= 200) {
        return;
    }
    if ((*reinterpret_cast<const unsigned char*>(
            &payload->effects1C[initialIndex].flags) & 8u) == 0) {
        TurnBattleToward_406190(action, gBattleAllEntries59E980[payload->effects1C[initialIndex].position], 0, 0.0F);
    }
    const std::int32_t earlyIndex = payload->effectRunningC9C;
    if ((payload->effects1C[earlyIndex].flags & 0x20000000u) != 0
        && GetBattleDistance_406300(action, gBattleAllEntries59E980[payload->effects1C[earlyIndex].position]) < 70
        && action->frameEvent == 0) {
        YobiAction* const target = gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position];
        if (target == nullptr) {
            return;
        }
        auto* const targetPayload = target->payload.battleActor;
        static_cast<void>(MarkDeath(action));
        action->phase = 17;
        const std::int32_t mode = payload->attackModeFF4;
        if (mode == 2) {
            YobiAction* const effect = NewBattleRandomEffect_419FC0(action, 110469, 0);
            if (effect != nullptr) {
                effect->y -= 103;
            }
        } else if (mode == 4) {
            const float x = action->offset40;
            const float y = action->offset44;
            const float speed = action->value54;
            action->value54 = 20.0F;
            MoveBattleByAngle_405EA0(action);
            YobiAction* const effect = NewBattleRandomEffect_419FC0(action, 110469, 0);
            if (effect != nullptr) {
                effect->x = static_cast<std::int32_t>(action->offset40);
                effect->y = static_cast<std::int32_t>(action->offset44) - 90;
            }
            action->offset40 = x;
            action->offset44 = y;
            action->value54 = speed;
        } else {
            YobiAction* const effect = NewBattleRandomEffect_419FC0(action, 110473, 0);
            if (effect != nullptr) {
                effect->y -= 68;
            }
        }
        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110474, 0));
        if (target->phase != 0) {
            AdvanceBattleCastState_439340(target);
            targetPayload->valueFCC = 0;
            targetPayload->value1024 = 0;
        }
        AdvanceBattleCastState_439340(target);
        return;
    }
    if (GetBattleDistance_406300(action,
            gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 20
        && action->frameEvent == 0) {
        const std::int32_t index = payload->effectRunningC9C;
        const std::int32_t position = payload->effects1C[index].position;
        YobiAction* const target = gBattleAllEntries59E980[position];
        if (target == nullptr) {
            return;
        }
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t image = -1;
        std::int32_t secondaryImage = -1;
        std::int32_t direction = 0;
        const std::uint32_t initialFlags = payload->effects1C[index].flags;
        if ((initialFlags & 4u) != 0) {
            image = 110000;
        } else if ((initialFlags & 0x800u) != 0) {
            image = 110099;
        } else if ((initialFlags & 0x400u) != 0) {
            image = FindBattleAppendEffectTechnique_43BEA0(8, position) < 12 ? 110098 : 111048;
        } else {
            const std::int32_t skill = payload->castSkill103C;
            switch (skill) {
            case 0: case 2007: case 2008: case 2021:
                image = payload->castSkillLevel1044 < 12 ? 110133 : 111029;
                break;
            case 1: image = payload->castSkillLevel1044 < 12 ? 110134 : 111047; break;
            case 3: image = payload->castSkillLevel1044 < 12 ? 110135 : 111046; break;
            case 5: image = payload->castSkillLevel1044 < 12 ? 110139 : 111052; break;
            case 6: image = payload->castSkillLevel1044 < 12 ? 110137 : 111050; break;
            case 81: image = 110174; break;
            case 84: image = 110175; break;
            case 85: image = 110159; break;
            case 87: image = 110160; break;
            case 110: image = 110074; break;
            case 257: {
                const std::int32_t level = payload->castSkillLevel1044;
                secondaryImage = 110476;
                if (level >= 12) {
                    secondaryImage = 111051;
                } else if (level >= 9) {
                    image = 110014;
                }
                break;
            }
            case 258: secondaryImage = 110477; break;
            case 259:
                image = platform::RandomRange_46FAF0(0, 2) + 110001;
                secondaryImage = 110470;
                break;
            case 260: image = payload->castSkillLevel1044 < 12 ? 110473 : 111054; break;
            case 262:
                if (payload->castSkillLevel1044 < 12) {
                    image = 110473;
                    direction = 1;
                } else {
                    image = 111049;
                }
                break;
            default:
                if ((skill >= 75 && skill <= 80) || (skill >= 2096 && skill <= 2101)) {
                    image = 110074;
                } else {
                    image = platform::RandomRange_46FAF0(0, 2) + 110001;
                }
                break;
            }
        }
        TurnBattleToward_406190(target, action, 1, 0.0F);
        target->battleOpponent10 = gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        if (payload->castSkillLevel1044 >= 9 && payload->castSkill103C == 259) {
            static_cast<void>(NewBattleTiledEffect_41E9D0(0, 15));
        }
        const std::int32_t appliedIndex = payload->effectRunningC9C;
        const std::uint32_t flags = payload->effects1C[appliedIndex].flags;
        if ((flags & 8u) != 0) {
            const std::int32_t skill = payload->castSkill103C;
            if (skill == 95 || skill == 4002 || skill == 3038) {
                if (target->phase != 0) {
                    AdvanceBattleCastState_439340(target);
                    targetPayload->valueFCC = 0;
                    targetPayload->value1024 = 0;
                }
                AdvanceBattleCastState_439340(target);
            } else if (payload->attackModeFF4 == 4 && target->phase != 0) {
                AdvanceBattleCastState_439340(target);
                targetPayload->valueFCC = 0;
                targetPayload->value1024 = 0;
                AdvanceBattleCastState_439340(target);
            } else {
                target->phase = 31;
            }
            goto finishProjectileHit;
        }
        if ((flags & 0x100u) != 0) {
            const std::int32_t skill = payload->castSkill103C;
            if (skill == 95 || skill == 4002 || skill == 3038) {
                if (target->phase != 0) {
                    AdvanceBattleCastState_439340(target);
                    targetPayload->valueFCC = 0;
                    targetPayload->value1024 = 0;
                }
                AdvanceBattleCastState_439340(target);
            } else {
                if (payload->attackModeFF4 == 4 && target->phase != 0) {
                    AdvanceBattleCastState_439340(target);
                    targetPayload->valueFCC = 0;
                    targetPayload->value1024 = 0;
                }
                targetPayload->flash1004 = 1;
                static_cast<void>(audio::PlaySe_52C450(210, action->x));
                target->phase = 31;
            }
            goto finishProjectileHit;
        }
        {
            if ((flags & 0x7E000u) == 0 && payload->effects1C[appliedIndex].position3 != 0) {
                if ((flags & 2u) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                if (image >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, image, direction));
                }
                if (secondaryImage >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, secondaryImage, 0));
                }
                NewBattleRebirthEffects_4208F0(gBattleAllEntries59E980[payload->battlePosition18], target);
                const std::int32_t mode = payload->attackModeFF4;
                static_cast<void>(audio::PlaySe_52C450(mode == 2 ? 141 : (mode == 4 ? 142 : 255), action->x));
                const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                const std::int32_t amount = effect.position3;
                (gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                if ((gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position]->readiness < 0)) {
                    std::optional<platform::LegacyCryptInt> temporary;
                    temporary.emplace(static_cast<std::int32_t>(0));
                    const std::int32_t copyEffectIndex = payload->effectRunningC9C;
                    const std::uint32_t zeroEncoded = temporary->encoded;
                    YobiAction* const copyTarget = gBattleAllEntries59E980[
                        payload->effects1C[copyEffectIndex].position];
                    copyTarget->readiness.encoded = zeroEncoded;
                    copyTarget->readiness.mask = temporary->mask;
                    copyTarget->readiness.readCount = temporary->readCount;
                    temporary.reset();
                }
                if ((*reinterpret_cast<const unsigned char*>(
                        &payload->effects1C[payload->effectRunningC9C].flags) & 0x20u) != 0) {
                    targetPayload->flagsFDC |= 2u;
                    gYobiUpdateGateMode = 1;
                    targetPayload->aliveFF8 = 1;
                }
            }
            const std::uint32_t hitFlags = payload->effects1C[payload->effectRunningC9C].flags;
            if ((hitFlags & 0x48000u) != 0) {
                const std::int32_t skill = payload->castSkill103C;
                if (skill == 95 || skill == 4002 || skill == 3038) {
                    if (target->phase != 0) {
                        AdvanceBattleCastState_439340(target);
                        targetPayload->valueFCC = 0;
                        targetPayload->value1024 = 0;
                    }
                } else {
                    if (payload->attackModeFF4 == 4 && target->phase != 0) {
                        AdvanceBattleCastState_439340(target);
                        targetPayload->valueFCC = 0;
                        targetPayload->value1024 = 0;
                    }
                    const std::int32_t currentSkill = payload->castSkill103C;
                    if ((currentSkill == 110 || currentSkill == 3019)
                        && (payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                        static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                    }
                }
                AdvanceBattleCastState_439340(target);
            } else if ((hitFlags & 0x24000u) != 0) {
                const std::int32_t skill = payload->castSkill103C;
                if (skill == 95 || skill == 4002 || skill == 3038) {
                    if (target->phase != 0) {
                        AdvanceBattleCastState_439340(target);
                        targetPayload->valueFCC = 0;
                        targetPayload->value1024 = 0;
                    }
                } else {
                    if (payload->attackModeFF4 == 4 && target->phase != 0) {
                        AdvanceBattleCastState_439340(target);
                        targetPayload->valueFCC = 0;
                        targetPayload->value1024 = 0;
                    }
                    const std::int32_t currentSkill = payload->castSkill103C;
                    if ((currentSkill == 110 || currentSkill == 3019)
                        && (payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                        static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                    }
                }
                AdvanceBattleCastState_439340(target);
                const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                if ((effect.flags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                } else {
                    const std::int32_t amount = effect.position3;
                    (gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const std::int32_t numberIndex = payload->effectRunningC9C;
                    const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                    const std::int32_t y = target->y - 80;
                    const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                    static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 5, 2, numberFlags));
                    if ((*reinterpret_cast<const unsigned char*>(
                            &payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        gBattleAllEntries59E980[payload->battlePosition18], target));
                }
            } else if ((hitFlags & 0x12000u) != 0) {
                const std::int32_t skill = payload->castSkill103C;
                if (skill == 95 || skill == 4002 || skill == 3038) {
                    if (target->phase != 0) {
                        AdvanceBattleCastState_439340(target);
                        targetPayload->valueFCC = 0;
                        targetPayload->value1024 = 0;
                    }
                } else {
                    if (payload->attackModeFF4 == 4 && target->phase != 0) {
                        AdvanceBattleCastState_439340(target);
                        targetPayload->valueFCC = 0;
                        targetPayload->value1024 = 0;
                    }
                    const std::int32_t currentSkill = payload->castSkill103C;
                    if ((currentSkill == 110 || currentSkill == 3019)
                        && (payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                        static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                    }
                }
                AdvanceBattleCastState_439340(target);
            } else {
                if ((hitFlags & 0x100000u) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->effect1054));
                    const std::uint32_t targetFlags = targetPayload->flagsFDC;
                    targetPayload->effect1054 = nullptr;
                    targetPayload->flagsFDC = targetFlags & ~0x20u;
                }
                const std::int32_t skill = payload->castSkill103C;
                if (skill == 95 || skill == 4002 || skill == 3038) {
                    if ((payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                        static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                    }
                    const double backwardAngle = target->angle50 + 180.0;
                    target->angle50 = static_cast<float>(backwardAngle);
                    if (backwardAngle >= 360.0) {
                        target->angle50 = static_cast<float>(backwardAngle - 360.0);
                    }
                    target->value54 = 8.0F;
                    MoveBattleByAngle_405EA0(target);
                    const double forwardAngle = target->angle50 + 180.0;
                    target->angle50 = static_cast<float>(forwardAngle);
                    if (forwardAngle >= 360.0) {
                        target->angle50 = static_cast<float>(forwardAngle - 360.0);
                    }
                    if (target->phase != 0) {
                        AdvanceBattleCastState_439340(target);
                        targetPayload->valueFCC = 0;
                        targetPayload->value1024 = 0;
                    }
                } else {
                    if (payload->attackModeFF4 == 4 && target->phase != 0) {
                        AdvanceBattleCastState_439340(target);
                        targetPayload->valueFCC = 0;
                        targetPayload->value1024 = 0;
                    }
                    const std::int32_t currentSkill = payload->castSkill103C;
                    if ((currentSkill == 110 || currentSkill == 3019)
                        && (payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                        static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                    }
                }
                AdvanceBattleCastState_439340(target);
                const std::int32_t numberIndex = payload->effectRunningC9C;
                const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                if ((numberFlags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        gBattleAllEntries59E980[payload->effects1C[numberIndex].position], 22459, 0, 0));
                } else {
                    const std::int32_t y = target->y - 80;
                    const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                    static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 6, 2, numberFlags));
                    if ((*reinterpret_cast<const unsigned char*>(
                            &payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        gBattleAllEntries59E980[payload->battlePosition18], target));
                }
            }
            if (payload->castSkill103C == 259
                && (payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
            }
            if (payload->castSkill103C == 3029
                && (payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                static_cast<void>(NewBattleDebuffEffect_41F660(target, 110079));
            }
        }
    finishProjectileHit:
        const std::int32_t finishIndex = payload->effectRunningC9C;
        if ((*reinterpret_cast<const unsigned char*>(
                &payload->effects1C[finishIndex].flags) & 8u) == 0
            && payload->effects1C[finishIndex].position3 != 0) {
            const std::int32_t duration = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
            const std::int32_t skill = payload->castSkill103C;
            payload->valueFD0 = duration;
            if (skill == 267 || skill == 3029) {
                payload->valueFD0 = 2;
            }
        }
        action->frameEvent = 1;
    }
    const std::int32_t delay = payload->valueFD0;
    if (delay > 0) {
        payload->valueFD0 = delay - 1;
        const std::int32_t image = action->imageNo;
        if (image == 110016 || image == 111055) {
            static_cast<void>(PlayFrame_517DA0(action, 0));
        }
    }
    if (payload->valueFD0 == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
        MoveBattleByAngle_405EA0(action);
        if (action->frameEvent == 1) {
            const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
            if ((flags & 0x108u) == 0) {
                static_cast<void>(MarkDeath(action));
            }
            action->phase = 17;
        }
    }
}

// BPEX01：移动后的坐标超出原矩形才标死；公共尾及最终摘链/释放见doc/cg2d_battle_projectile_exit_spec.md。
// 原函数：0x004093B0。
void UpdateBattleAction17_4093B0(YobiAction* const action)
{
    MoveBattleByAngle_405EA0(action);
    if (!(action->offset40 >= -200.0F) || action->offset40 > 840.0F
        || !(action->offset44 >= -200.0F) || action->offset44 > 680.0F) {
        static_cast<void>(MarkDeath(action));
    }
}

// B20H01：低BYTE标志及HP归零的取样/交替发布见doc/cg2d_battle_near_hit_spec.md。
// 原函数：0x00408410。
void UpdateBattleAction20_408410(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    const std::int32_t initialIndex = payload->effectRunningC9C;
    const std::uint8_t entryFlags = *reinterpret_cast<const std::uint8_t*>(
        &payload->effects1C[initialIndex].flags);
    if ((entryFlags & 8u) == 0) {
        TurnBattleToward_406190(action, gBattleAllEntries59E980[payload->effects1C[initialIndex].position], 1, 0.0F);
    }
    if (GetBattleDistance_406300(action,
            gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 70) {
        const std::int32_t index = payload->effectRunningC9C;
        const std::int32_t position = payload->effects1C[index].position;
        const std::uint32_t initialFlags = payload->effects1C[index].flags;
        YobiAction* const target = gBattleAllEntries59E980[position];
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t image;
        if ((initialFlags & 4u) != 0) {
            image = 110000;
        } else if ((initialFlags & 0x400u) != 0) {
            image = FindBattleAppendEffectTechnique_43BEA0(8, position) < 12 ? 110098 : 111048;
        } else {
            image = platform::RandomRange_46FAF0(0, 2) + 110001;
        }
        TurnBattleToward_406190(target, action, 1, 0.0F);
        target->battleOpponent10 = gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        const std::int32_t appliedIndex = payload->effectRunningC9C;
        const std::uint32_t flags = payload->effects1C[appliedIndex].flags;
        if ((flags & 8u) != 0) {
            target->phase = 31;
        } else if ((flags & 0x100u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(210, action->x));
            targetPayload->flash1004 = 1;
            target->phase = 31;
        } else {
            if (payload->effects1C[appliedIndex].position3 != 0) {
                if ((flags & 2u) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                if (image >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, image, 0));
                }
                NewBattleRebirthEffects_4208F0(gBattleAllEntries59E980[payload->battlePosition18], target);
                const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                if ((effect.flags & 0x10000000u) == 0) {
                    const std::int32_t amount = effect.position3;
                    (gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                    if ((gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position]->readiness < 0)) {
                        std::optional<platform::LegacyCryptInt> temporary;
                        temporary.emplace(static_cast<std::int32_t>(0));
                        const std::int32_t copyIndex = payload->effectRunningC9C;
                        const std::uint32_t encoded = temporary->encoded;
                        const std::int32_t copyPosition = payload->effects1C[copyIndex].position;
                        YobiAction* const copyTarget = gBattleAllEntries59E980[copyPosition];
                        copyTarget->readiness.encoded = encoded;
                        const std::uint32_t mask = temporary->mask;
                        copyTarget->readiness.mask = mask;
                        const std::uint8_t readCount = temporary->readCount;
                        copyTarget->readiness.readCount = readCount;
                        temporary.reset();
                    }
                    const std::uint8_t activationFlags = *reinterpret_cast<const std::uint8_t*>(
                        &payload->effects1C[payload->effectRunningC9C].flags);
                    if ((activationFlags & 0x20u) != 0) {
                        targetPayload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        targetPayload->aliveFF8 = 1;
                    }
                }
            }
            AdvanceBattleCastState_439340(target);
            const std::int32_t numberIndex = payload->effectRunningC9C;
            const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
            if ((numberFlags & 0x10000000u) != 0) {
                static_cast<void>(NewBattleFloatingIcon_41F4C0(
                    gBattleAllEntries59E980[payload->effects1C[numberIndex].position], 22459, 0, 0));
            } else {
                const std::int32_t y = target->y - 80;
                const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 6, 2, numberFlags));
                const std::uint8_t iconFlags = *reinterpret_cast<const std::uint8_t*>(
                    &payload->effects1C[payload->effectRunningC9C].flags);
                if ((iconFlags & 2u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                }
                static_cast<void>(NewBattleAttributeEffects_4209C0(
                    gBattleAllEntries59E980[payload->battlePosition18], target));
            }
            static_cast<void>(MarkDeath(action));
        }
        action->phase = 17;
    }
    MoveBattleByAngle_405EA0(action);
}

// B22L01：两次Y-X取样、距离门禁和Move/PlayFrame返回见回旋命中SPEC。
// 原函数：0x00408750。
std::int32_t UpdateBattleAction21_408750(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const float turnY = static_cast<float>(payload->y04);
    const float turnX = static_cast<float>(payload->x00);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, 0, 10.0F);
    NormalizeBattleAngle_405E60(action);
    const float targetY = static_cast<float>(payload->y04);
    const float targetX = static_cast<float>(payload->x00);
    if (GetBattleDistanceToPosition_4062A0(action, targetX, targetY) < 20) {
        action->phase = 22;
    }
    MoveBattleByAngle_405EA0(action);
    return PlayFrame_517DA0(action, 0);
}

// B22H01：回旋命中独立HP/数显与延迟、低BYTE及取样阶段见doc/cg2d_battle_boomerang_hit_spec.md。
// 原函数：0x004087C0。
std::int32_t UpdateBattleAction22_4087C0(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    const std::int32_t initialFrameEvent = action->frameEvent;
    auto* const payload = action->payload.battleActor;
    if (initialFrameEvent == 0) {
        if (action->field1D0 == 0) {
            action->field1D0 = static_cast<std::int32_t>(GetBattleAngleDifference_406060(action,
                gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]));
        }
        if (GetBattleDistance_406300(action,
                gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 60) {
            const float step = static_cast<float>(static_cast<double>(action->effectAcceleration48)
                + 0.3F);
            action->effectAcceleration48 = step;
            TurnBattleToward_406190(action,
                gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 0, step);
        }
        const std::int32_t turnIndex = payload->effectRunningC9C;
        const float step = action->effectAcceleration48;
        TurnBattleToward_406190(action,
            gBattleAllEntries59E980[payload->effects1C[turnIndex].position], 0, step);
        NormalizeBattleAngle_405E60(action);
        YobiAction* const other = action->battleOpponent10;
        if (other->phase != 15) {
            TurnBattleToward_406190(other, action, 1, 0.0F);
        }
        if (action->frameEvent == 0
            && GetBattleDistance_406300(action,
                gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 20) {
            std::int32_t image = -1;
            std::int32_t secondaryImage = -1;
            std::int32_t direction = 0;
            YobiAction* const target = gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position];
            auto* const targetPayload = target->payload.battleActor;
            action->effectAcceleration48 = 10.0F;
            const std::int32_t imageIndex = payload->effectRunningC9C;
            const std::uint32_t imageFlags = payload->effects1C[imageIndex].flags;
            if ((imageFlags & 4u) != 0) {
                image = 110000;
            } else if ((imageFlags & 0x400u) != 0) {
                image = FindBattleAppendEffectTechnique_43BEA0(8, payload->effects1C[imageIndex].position) < 12
                    ? 110098 : 111048;
            } else {
                const std::int32_t skill = payload->castSkill103C;
                switch (skill) {
                case 0: case 2007: case 2008: case 2021:
                    image = payload->castSkillLevel1044 < 12 ? 110133 : 111029;
                    break;
                case 1: image = payload->castSkillLevel1044 < 12 ? 110134 : 111047; break;
                case 3: image = payload->castSkillLevel1044 < 12 ? 110135 : 111046; break;
                case 5: image = payload->castSkillLevel1044 < 12 ? 110139 : 111052; break;
                case 6: image = payload->castSkillLevel1044 < 12 ? 110137 : 111050; break;
                case 3019: case 3038: image = 110074; break;
                case 81: image = 110174; break;
                case 84: image = 110175; break;
                case 85: image = 110159; break;
                case 87: image = 110160; break;
                case 257: {
                    const std::int32_t level = payload->castSkillLevel1044;
                    secondaryImage = 110476;
                    if (level >= 12) {
                        secondaryImage = 111051;
                    } else if (level >= 9) {
                        image = 110014;
                    }
                    break;
                }
                case 258: secondaryImage = 110477; break;
                case 259:
                    image = platform::RandomRange_46FAF0(0, 2) + 110001;
                    secondaryImage = 110470;
                    break;
                case 260: image = payload->castSkillLevel1044 < 12 ? 110473 : 111054; break;
                case 262:
                    if (payload->castSkillLevel1044 < 12) {
                        image = 110473;
                        direction = 1;
                    } else {
                        image = 111049;
                    }
                    break;
                case 266: {
                    const std::int32_t level = payload->castSkillLevel1044;
                    secondaryImage = 110476;
                    if (level >= 12) {
                        secondaryImage = 111028;
                    } else if (level >= 9) {
                        image = 110014;
                    }
                    break;
                }
                case 2022:
                    image = kBattleBoomerangHitImages54D43C[GetBattleValueBand_43BF00(payload->castSkillLevel1044 + 1) - 1];
                    break;
                default:
                    if ((skill >= 75 && skill <= 80) || (skill >= 2096 && skill <= 2101)) {
                        image = 110074;
                    } else {
                        image = platform::RandomRange_46FAF0(0, 2) + 110001;
                    }
                    break;
                }
            }
            TurnBattleToward_406190(target, action, 1, 0.0F);
            target->battleOpponent10 = gBattleAllEntries59E980[payload->battlePosition18];
            targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
                payload->effects1C[payload->effectRunningC9C].flags);
            ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
            if (payload->castSkillLevel1044 >= 9 && payload->castSkill103C == 259) {
                static_cast<void>(NewBattleTiledEffect_41E9D0(0, 15));
            }
            const std::int32_t appliedIndex = payload->effectRunningC9C;
            const std::uint32_t flags = payload->effects1C[appliedIndex].flags;
            if ((flags & 8u) != 0) {
                target->phase = 31;
            } else if ((flags & 0x100u) != 0) {
                static_cast<void>(audio::PlaySe_52C450(210, action->x));
                targetPayload->flash1004 = 1;
                target->phase = 31;
            } else {
                if ((flags & 0xE000u) == 0 && payload->effects1C[appliedIndex].position3 != 0) {
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                    }
                    if (image >= 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, image, direction));
                    }
                    if (secondaryImage >= 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, secondaryImage, 0));
                    }
                    NewBattleRebirthEffects_4208F0(gBattleAllEntries59E980[payload->battlePosition18], target);
                    static_cast<void>(audio::PlaySe_52C450(143, action->x));
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    const std::int32_t amount = effect.position3;
                    (gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                    if ((gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position]->readiness < 0)) {
                        std::optional<platform::LegacyCryptInt> temporary;
                        temporary.emplace(static_cast<std::int32_t>(0));
                        const std::int32_t copyIndex = payload->effectRunningC9C;
                        const std::uint32_t encoded = temporary->encoded;
                        const std::int32_t copyPosition = payload->effects1C[copyIndex].position;
                        YobiAction* const copyTarget = gBattleAllEntries59E980[copyPosition];
                        copyTarget->readiness.encoded = encoded;
                        const std::uint32_t mask = temporary->mask;
                        copyTarget->readiness.mask = mask;
                        const std::uint8_t readCount = temporary->readCount;
                        copyTarget->readiness.readCount = readCount;
                        temporary.reset();
                    }
                    const std::uint8_t activationFlags = *reinterpret_cast<const std::uint8_t*>(
                        &payload->effects1C[payload->effectRunningC9C].flags);
                    if ((activationFlags & 0x20u) != 0) {
                        targetPayload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        targetPayload->aliveFF8 = 1;
                    }
                }
                const std::uint32_t hitFlags = payload->effects1C[payload->effectRunningC9C].flags;
                if ((hitFlags & 0x8000u) != 0) {
                    AdvanceBattleCastState_439340(target);
                } else if ((hitFlags & 0x4000u) != 0) {
                    AdvanceBattleCastState_439340(target);
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    if ((effect.flags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                    } else {
                        const std::int32_t amount = effect.position3;
                        (gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                        YobiAction* const compareTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        if ((compareTarget->readiness > compareTarget->maxHp)) {
                            YobiAction* const copyTarget = gBattleAllEntries59E980[
                                payload->effects1C[payload->effectRunningC9C].position];
                            copyTarget->readiness = copyTarget->maxHp;
                        }
                        const std::int32_t numberIndex = payload->effectRunningC9C;
                        const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                        const std::int32_t y = target->y - 80;
                        const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                        static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 5, 2, numberFlags));
                        const std::uint8_t restoreIconFlags = *reinterpret_cast<const std::uint8_t*>(
                            &payload->effects1C[payload->effectRunningC9C].flags);
                        if ((restoreIconFlags & 2u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(
                            gBattleAllEntries59E980[payload->battlePosition18], target));
                    }
                } else if ((hitFlags & 0x2000u) != 0) {
                    AdvanceBattleCastState_439340(target);
                } else {
                    if ((hitFlags & 0x100000u) != 0) {
                        static_cast<void>(MarkDeath(targetPayload->effect1054));
                        const std::uint32_t targetFlags = targetPayload->flagsFDC;
                        targetPayload->effect1054 = nullptr;
                        targetPayload->flagsFDC = targetFlags & ~0x20u;
                    }
                    AdvanceBattleCastState_439340(target);
                    const std::int32_t numberIndex = payload->effectRunningC9C;
                    const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                    if ((numberFlags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[payload->effects1C[numberIndex].position], 22459, 0, 0));
                    } else {
                        const std::int32_t y = target->y - 80;
                        const std::int32_t displayed = payload->effects1C[numberIndex].position3;
                        static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 6, 2, numberFlags));
                        const std::uint8_t damageIconFlags = *reinterpret_cast<const std::uint8_t*>(
                            &payload->effects1C[payload->effectRunningC9C].flags);
                        if ((damageIconFlags & 2u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(
                            gBattleAllEntries59E980[payload->battlePosition18], target));
                    }
                }
                if (payload->castSkill103C == 259
                    && (payload->effects1C[payload->effectRunningC9C].flags & 0x80000u) != 0) {
                    static_cast<void>(NewBattleDebuffEffect_41F660(target, 110075));
                }
            }
            const std::int32_t finishIndex = payload->effectRunningC9C;
            const std::uint8_t finishFlags = *reinterpret_cast<const std::uint8_t*>(
                &payload->effects1C[finishIndex].flags);
            if ((finishFlags & 8u) == 0 && payload->effects1C[finishIndex].position3 != 0) {
                payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u) / 4;
            }
            if (AdvanceBattleEffect_439470(action) == 1) {
                action->frameEvent = 1;
            }
        }
    }
    const std::int32_t delay = payload->valueFD0;
    if (delay > 0) {
        payload->valueFD0 = delay - 1;
    }
    std::int32_t result = payload->valueFD0;
    if (result == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
        MoveBattleByAngle_405EA0(action);
        result = action->frameEvent;
        if (result == 1) {
            action->phase = 23;
        }
    }
    return result;
}

// B22L01：固定caster/父载荷、266列表和互斥标死后的移动播放见回旋命中SPEC。
// 原函数：0x004090F0。
std::int32_t UpdateBattleAction23_4090F0(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    std::int32_t turnSign = action->field1D0;
    YobiAction* const caster = payload->caster1070;
    if (turnSign == 0) {
        turnSign = static_cast<std::int32_t>(GetBattleAngleDifference_406060(action, action->battleOpponent10));
        action->field1D0 = turnSign;
    }
    if (turnSign <= 0) {
        const float difference = static_cast<float>(GetBattleAngleDifference_406060(action, action->battleOpponent10));
        if (-action->effectAcceleration48 > difference
            || !(static_cast<double>(action->effectAcceleration48)
                * 0.2F >= difference)) {
            action->angle50 = static_cast<float>(static_cast<double>(action->angle50) - action->effectAcceleration48);
        } else {
            action->angle50 = static_cast<float>(static_cast<double>(difference) + action->angle50);
        }
    } else {
        const double rawDifference = GetBattleAngleDifference_406060(action, action->battleOpponent10);
        const float difference = static_cast<float>(rawDifference);
        const double step = rawDifference > action->effectAcceleration48
                || static_cast<double>(action->effectAcceleration48)
                    * -0.2F > difference
            ? action->effectAcceleration48 : difference;
        action->angle50 = static_cast<float>(step + action->angle50);
    }
    NormalizeBattleAngle_405E60(action);
    if (GetBattleDistance_406300(action, action->battleOpponent10) < 100) {
        action->effectAcceleration48 = static_cast<float>(static_cast<double>(action->effectAcceleration48)
            + 0.3F);
    }
    if (GetBattleDistance_406300(action, action->battleOpponent10) < 60) {
        YobiAction* const parent = action->battleOpponent10;
        action->effectAcceleration48 = 180.0F;
        if (parent->phase == 15) {
            AdvanceBattleCastState_439340(parent);
        }
        YobiAction* const currentParent = action->battleOpponent10;
        if (currentParent->phase != 24) {
            currentParent->characterAction = 12;
            static_cast<void>(PlayFrame_517DA0(action->battleOpponent10, 0));
            AdvanceBattleCastState_439340(action->battleOpponent10);
            if (payload->castSkill103C == 266) {
                auto* const parentPayload = action->battleOpponent10->payload.battleActor;
                const std::int32_t index = parentPayload->effectRunningC9C;
                parentPayload->valueFF0 = 0;
                if ((parentPayload->effects1C[index].flags & 0x200u) != 0) {
                    static_cast<void>(AdvanceBattleEffect_439470(action->battleOpponent10));
                    AdvanceBattleCastState_439340(action->battleOpponent10);
                    AppendBattleEffect_439400(action, {parentPayload->effects1C[parentPayload->effectRunningC9C].position, 0, 0, 0});
                    if (gBattleAllEntries59E980[parentPayload->effects1C[parentPayload->effectRunningC9C].position]->phase != 0) {
                        AppendBattleCast_4392A0(caster, action, 25);
                    }
                    AppendBattleCast_4392A0(caster, action, 73);
                    AppendBattleCast_4392A0(caster, action, 25);
                    AppendBattleCast_4392A0(caster, action, 222);
                    action->effectAcceleration48 = 10.0F;
                    AdvanceBattleCastState_439340(action);
                }
            }
        }
        if (GetBattleDistance_406300(action, action->battleOpponent10) < 20) {
            if (payload->castSkill103C == 266) {
                auto* const parentPayload = action->battleOpponent10->payload.battleActor;
                const std::int32_t index = parentPayload->castPlayIndexFC4;
                parentPayload->valueFF0 = 0;
                if (parentPayload->castListCA4[index + 1] == 221) {
                    AdvanceBattleCastState_439340(action->battleOpponent10);
                }
            }
            static_cast<void>(MarkDeath(action));
            static_cast<void>(audio::PlaySe_52C450(216, action->x));
        }
    }
    MoveBattleByAngle_405EA0(action);
    return PlayFrame_517DA0(action, 0);
}

// BPAW01：先发布新FF0，严格>20才先归零再推进，不播放。
// 原函数：0x00409380。
void UpdateBattleAction24_409380(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t count = payload->valueFF0 + 1;
    payload->valueFF0 = count;
    if (count > 20) {
        payload->valueFF0 = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BPAW01：X发布后fresh查目标/Y发布；等待先归零动作再读低BYTE播放门禁。
// 原函数：0x00406BA0。
void UpdateBattleAction25_406BA0(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position == -1) {
        AdvanceBattleCastState_439340(action);
        return;
    }
    YobiAction* const target = gBattleAllEntries59E980[position];
    if (target == nullptr) {
        AdvanceBattleCastState_439340(action);
        return;
    }
    if (payload->battlePosition18 == position || target->phase == 0) {
        auto* const targetPayload = target->payload.battleActor;
        targetPayload->x10 = target->x;
        YobiAction* const currentTarget = gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        auto* const currentPayload = currentTarget->payload.battleActor;
        currentPayload->y14 = currentTarget->y;
        AdvanceBattleCastState_439340(action);
        return;
    }
    action->characterAction = 0;
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
}

// BSEX01：动作8/rate100，PlayFrame(1)严格==1才推进，不增计数或移动。
// 原函数：0x0040B410。
void UpdateBattleAction32_40B410(YobiAction* const action)
{
    action->characterAction = 8;
    action->framePlayingRate = 100;
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// BSEX01：动作9/rate100，PlayFrame(1)严格==1才推进。
// 原函数：0x0040B450。
void UpdateBattleAction33_40B450(YobiAction* const action)
{
    action->characterAction = 9;
    action->framePlayingRate = 100;
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// BSEX01：音效后fresh BYTE/40次散射；单次半宽/原反射与直到非零播放保持。
// 原函数：0x0040B5E0。
std::int32_t UpdateBattleAction34_40B5E0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if ((static_cast<std::uint16_t>(payload->effectFlagsFE4) & 0xC04) != 0) {
        action->characterAction = 9;
    }
    action->framePlayingRate = 100;
    const std::int32_t count = payload->valueFCC + 1;
    payload->valueFCC = count;
    const std::int32_t duration = 14 * static_cast<std::int32_t>(((payload->flagsFDC >> 1) & 1u) + 1u);
    if (count < duration) {
        const double angle = action->angle50 + 180.0;
        action->value54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        if ((static_cast<std::uint8_t>(payload->flagsFDC) & 2u) != 0) {
            gYobiUpdateGateMode = 1;
            payload->aliveFF8 = 1;
        }
        MoveBattleByAngle_405EA0(action);
    } else {
        if (count == duration) {
            static_cast<void>(audio::PlaySe_52C450(207, action->x));
            if ((static_cast<std::uint8_t>(payload->flagsFDC) & 2u) != 0) {
                if (CountBattleActiveEntries_406360() <= 1) {
                    gYobiUpdateGateMode = 0;
                }
                const std::uint32_t state = payload->stateFE0;
                payload->aliveFF8 = 0;
                payload->stateFE0 = state | 2u;
            }
            for (std::int32_t remaining = 40; remaining != 0; --remaining) {
                static_cast<void>(NewBattleScatterEffect_41E510(action, 110004));
            }
            payload->valueFF0 = 0;
            payload->valueFD0 = 0;
            action->value54 = 15.0F;
        }
        MoveBattleByAngle_405EA0(action);
        const std::int32_t direction = action->direction + 1;
        action->direction = direction;
        if (direction >= 8) {
            action->direction = direction - 8;
        }
        if (payload->valueFF0 >= 3) {
            if (!(action->offset40 >= -200.0F) || action->offset40 > 840.0F
                || !(action->offset44 >= -200.0F) || action->offset44 > 680.0F) {
                AdvanceBattleCastState_439340(action);
                payload->valueFD0 = 0;
            }
        } else {
            static_cast<void>(render::LoadGraphic_522570(action->graphicIndex));
            const double halfWidth = render::ReadGraphicDxWidth(action->graphicIndex) * 0.5;
            if (halfWidth > action->offset40 && payload->valueFD0 != 1) {
                const double delta = action->angle50 - 90.0;
                action->angle50 = static_cast<float>(action->angle50 - (delta + delta));
                ++payload->valueFF0;
                static_cast<void>(audio::PlaySe_52C450(208, action->x));
                payload->valueFD0 = 1;
            } else if (action->offset40 > static_cast<float>(platform::gWindowWidth) - halfWidth && payload->valueFD0 != 2) {
                const double delta = action->angle50 - 270.0;
                action->angle50 = static_cast<float>(action->angle50 - (delta + delta));
                ++payload->valueFF0;
                static_cast<void>(audio::PlaySe_52C450(208, action->x));
                payload->valueFD0 = 2;
            } else if (static_cast<double>(-action->graphicOffsetY) > action->offset44 && payload->valueFD0 != 3) {
                const double delta = action->angle50 - 180.0;
                action->angle50 = static_cast<float>(action->angle50 - (delta + delta));
                ++payload->valueFF0;
                static_cast<void>(audio::PlaySe_52C450(208, action->x));
                payload->valueFD0 = 3;
            } else if (action->offset44 > static_cast<float>(platform::gWindowHeight) && payload->valueFD0 != 4) {
                const double angle = action->angle50;
                action->angle50 = static_cast<float>(angle - (angle + angle));
                ++payload->valueFF0;
                static_cast<void>(audio::PlaySe_52C450(208, action->x));
                payload->valueFD0 = 4;
            }
        }
    }
    std::int32_t result;
    do {
        result = PlayFrame_517DA0(action, 1);
    } while (result == 0);
    return result;
}

// BSEX01：入口持样BYTE边界/40次散射；晚期每帧尾迹，创建后重取周期。
// 原函数：0x0040B930。
std::int32_t UpdateBattleAction35_40B930(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if ((static_cast<std::uint16_t>(payload->effectFlagsFE4) & 0xC04) != 0) {
        action->characterAction = 9;
    }
    action->framePlayingRate = 100;
    const std::int32_t previous = payload->valueFCC;
    const std::uint32_t flags = payload->flagsFDC;
    const std::int32_t count = previous + 1;
    payload->valueFCC = count;
    const std::int32_t duration = 14 * static_cast<std::int32_t>(((flags >> 1) & 1u) + 1u);
    if (count < duration) {
        const double angle = action->angle50 + 180.0;
        action->value54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        if ((static_cast<std::uint8_t>(payload->flagsFDC) & 2u) != 0) {
            gYobiUpdateGateMode = 1;
            payload->aliveFF8 = 1;
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count == duration) {
        if ((static_cast<std::uint8_t>(flags) & 2u) != 0) {
            if (CountBattleActiveEntries_406360() <= 1) {
                gYobiUpdateGateMode = 0;
            }
            const std::uint32_t state = payload->stateFE0;
            payload->aliveFF8 = 0;
            payload->stateFE0 = state | 2u;
        }
        for (std::int32_t remaining = 40; remaining != 0; --remaining) {
            static_cast<void>(NewBattleScatterEffect_41E510(action, 110004));
        }
        payload->valueFF0 = 0;
    }
    static_cast<void>(NewBattlePositionEffect_41E680(action, 110005));
    const std::int32_t previousCycle = payload->valueFF0;
    payload->valueFF0 = previousCycle + 1;
    if (previousCycle >= 2) {
        payload->valueFF0 = 0;
    }
    action->value54 = 15.0F;
    MoveBattleByAngle_405EA0(action);
    if (!(action->offset40 >= -200.0F) || action->offset40 > 840.0F
        || !(action->offset44 >= -200.0F) || action->offset44 > 680.0F) {
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, 1);
}

// BSEX01：完整FDC后低BYTE门禁；条件播放后fresh FF0，新值>=30归零再推进。
// 原函数：0x0040BAD0。
void UpdateBattleAction36_40BAD0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    const std::int32_t count = payload->valueFF0 + 1;
    payload->valueFF0 = count;
    if (count >= 30) {
        payload->valueFF0 = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BWT01：完整FDC后低BYTE；条件播放后fresh FF0，新值>=60归零/推进。
// 原函数：0x0040BB20。
void UpdateBattleAction37_40BB20(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    const std::int32_t count = payload->valueFF0 + 1;
    payload->valueFF0 = count;
    if (count >= 60) {
        payload->valueFF0 = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BWT01：完整FDC后低BYTE；条件播放后fresh FF0，新值>=120归零/推进。
// 原函数：0x0040BB70。
void UpdateBattleAction38_40BB70(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    const std::int32_t count = payload->valueFF0 + 1;
    payload->valueFF0 = count;
    if (count >= 120) {
        payload->valueFF0 = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BWT01：原声前X持样/声后重取、间隔36短路、两次方向和连续三播放保持。
// 原函数：0x0040BBC0。
std::int32_t UpdateBattleAction39_40BBC0(YobiAction* const action)
{
    if (action->nameX == 0) {
        const std::int32_t x = action->x;
        action->characterAction = 8;
        static_cast<void>(audio::PlaySe_52C450(163, x));
        static_cast<void>(audio::PlaySe_52C450(171, action->x));
    }
    const std::int32_t previousCount = action->nameX;
    const std::int32_t interval = action->nameY;
    const std::int32_t count = previousCount + 1;
    action->nameX = count;
    if (interval == 36 || count % (36 - interval) == 0) {
        if (action->characterAction == 9) {
            const std::int32_t x = action->x;
            action->characterAction = 8;
            static_cast<void>(audio::PlaySe_52C450(163, x));
            static_cast<void>(audio::PlaySe_52C450(171, action->x));
            const std::int32_t firstDirection = action->direction + 1;
            action->direction = firstDirection;
            if (firstDirection >= 8) {
                action->direction = firstDirection - 8;
            }
            const std::int32_t secondDirection = action->direction + 1;
            action->direction = secondDirection;
            if (secondDirection >= 8) {
                action->direction = secondDirection - 8;
            }
        } else {
            const std::int32_t x = action->x;
            action->characterAction = 9;
            static_cast<void>(audio::PlaySe_52C450(163, x));
            static_cast<void>(audio::PlaySe_52C450(174, action->x));
        }
        const std::int32_t cycle = action->nameY + 1;
        action->nameY = cycle;
        if (cycle > 9) {
            action->nameX = 0;
            action->nameY = 0;
            action->nameValueC0 = 0;
            action->characterAction = 9;
            Sub_406260(action);
            AdvanceBattleCastState_439340(action);
        }
    }
    static_cast<void>(PlayFrame_517DA0(action, 1));
    static_cast<void>(PlayFrame_517DA0(action, 1));
    return PlayFrame_517DA0(action, 1);
}

// BWT01：方向恢复后推进，原无播放/计数或动作发布。
// 原函数：0x0040BD40。
void UpdateBattleAction40_40BD40(YobiAction* const action)
{
    Sub_406260(action);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00417B80。
void UpdateBattleAction241_417B80(YobiAction* const action)
{
    const std::int32_t event = action->frameEvent;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 6;
    action->framePlayingRate = 100;
    if (event == 0) {
        if (PlayFrame_517DA0(action, 1) == 1) {
            static_cast<void>(AdvanceBattleEffect_439470(action));
            AdvanceBattleCastState_439340(action);
            payload->valueFD0 = 0;
        }
        return;
    }
    const std::int32_t delay = payload->valueFF0;
    if (delay > 0) {
        payload->valueFF0 = delay - 1;
        return;
    }
    const auto* const effect = &payload->effects1C[payload->effectRunningC9C];
    if (effect->position == -1) {
        payload->valueFF0 = 0;
        action->frameEvent = 0;
        return;
    }
    const std::int32_t skill = payload->castSkill103C;
    if (skill == 267) {
        const std::int32_t image = payload->castSkillLevel1044 < 12 ? 114177 : 114196;
        static_cast<void>(NewBattleArcShot_419DA0(action, effect, image, payload->image102C, 0));
    } else if (skill == 3029) {
        const std::int32_t image = payload->castSkillLevel1044 < 12 ? 114263 : 114264;
        static_cast<void>(NewBattleArcShot_419DA0(action, effect, image, payload->image102C, 0));
    }
    static_cast<void>(AdvanceBattleEffect_439470(action));
    payload->valueFF0 = 6;
    const std::int32_t soundEvent = action->frameEvent;
    const std::int32_t soundX = action->x;
    static_cast<void>(audio::PlaySe_52C450(soundEvent - 10000, soundX));
}

// 原函数：0x00417CC0。
void UpdateBattleAction242_417CC0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    static_cast<void>(PlayFrame_517DA0(action, 0));
    for (;;) {
        const auto* const effect = &payload->effects1C[payload->effectRunningC9C];
        if (effect->position == -1) {
            break;
        }
        const std::int32_t skill = payload->castSkill103C;
        if (skill == 267) {
            const std::int32_t image = payload->castSkillLevel1044 < 12 ? 114177 : 114196;
            static_cast<void>(NewBattleArcShot_419DA0(action, effect, image, payload->image102C, 0));
        } else if (skill == 3029) {
            const std::int32_t image = payload->castSkillLevel1044 < 12 ? 114263 : 114264;
            static_cast<void>(NewBattleArcShot_419DA0(action, effect, image, payload->image102C, 0));
        }
        static_cast<void>(AdvanceBattleEffect_439470(action));
    }
    static_cast<void>(AdvanceBattleEffect_439470(action));
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00416220。
std::int32_t UpdateBattleAction232_416220(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 3;
    action->value54 = 10.0F;
    action->framePlayingRate = 100;
    const std::int32_t turnPosition = payload->effects1C[payload->effectRunningC9C].position;
    TurnBattleToward_406190(action, gBattleAllEntries59E980[turnPosition], 1, 0.0F);
    MoveBattleByAngle_405EA0(action);
    const std::int32_t distancePosition = payload->effects1C[payload->effectRunningC9C].position;
    if (GetBattleDistance_406300(action, gBattleAllEntries59E980[distancePosition]) < 20) {
        const std::int32_t index = payload->effectRunningC9C;
        YobiAction* const target = gBattleAllEntries59E980[payload->effects1C[index].position];
        const std::uint32_t flags = payload->effects1C[index].flags;
        auto* const targetPayload = target->payload.battleActor;
        TurnBattleToward_406190(target, action, 1, 0.0F);
        targetPayload->castSkill103C = -1;
        target->battleOpponent10 = gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        if ((flags & 8u) != 0) {
            target->phase = 31;
        } else if ((flags & 0x100u) != 0) {
            if (targetPayload->flash1004 == 0) {
                static_cast<void>(audio::PlaySe_52C450(210, action->x));
                targetPayload->flash1004 = 1;
                target->phase = 31;
            }
        } else {
            if ((flags & 0xC000u) == 0
                && payload->effects1C[payload->effectRunningC9C].position3 != 0) {
                target->offset40 = static_cast<float>(targetPayload->x10);
                target->offset44 = static_cast<float>(targetPayload->y14);
                if ((flags & 0x2000u) != 0) {
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(action, 110008, 0));
                    }
                    const std::int32_t imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                    NewBattleRebirthEffects_4208F0(action, target);
                    const std::int32_t skill = payload->castSkill103C;
                    payload->effectFlagsFE4 = 0x2000;
                    if (skill != 404 && skill != 406 && (flags & 0x20u) != 0) {
                        payload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        payload->aliveFF8 = 1;
                    }
                } else {
                    if ((flags & 0x100000u) != 0) {
                        static_cast<void>(MarkDeath(targetPayload->effect1054));
                        const std::uint32_t targetFlags = targetPayload->flagsFDC;
                        targetPayload->effect1054 = nullptr;
                        targetPayload->flagsFDC = targetFlags & ~0x20u;
                    }
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                    }
                    const std::int32_t imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                    NewBattleRebirthEffects_4208F0(action, target);
                    if ((flags & 0x20u) != 0) {
                        targetPayload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        targetPayload->aliveFF8 = 1;
                    }
                }
            }
            if ((flags & 0x8000u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else if ((flags & 0x4000u) != 0) {
                AdvanceBattleCastState_439340(target);
                if ((flags & 0x10000000u) != 0) {
                    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(gBattleAllEntries59E980[position], 22459, 0, 0));
                } else {
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    const std::int32_t amount = effect.position3;
                    (gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const std::int32_t recoveryDisplayIndex = payload->effectRunningC9C;
                    const std::int32_t y = target->y - 80;
                    const std::int32_t displayed = payload->effects1C[recoveryDisplayIndex].position3;
                    static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 5, 2, flags));
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                }
            } else if ((flags & 0x2000u) != 0) {
                AdvanceBattleCastState_439340(target);
                if ((flags & 0x10000000u) != 0) {
                    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(gBattleAllEntries59E980[position], 22459, 0, 0));
                } else {
                    (action->readiness -= static_cast<std::int32_t>(payload->effects1C[payload->effectRunningC9C].position3));
                    if ((action->readiness < 0)) {
                        // 原416655..416671先齐读，再按mask、encoded、readCount发布。
                        // 现由整数赋值统一载荷发布：一次RNG，保留目标token与reserved。
                        action->readiness = std::int32_t{0};
                    }
                    const std::int32_t reflectionDisplayIndex = payload->effectRunningC9C;
                    const std::int32_t y = action->y - 80;
                    const std::int32_t displayed = payload->effects1C[reflectionDisplayIndex].position3;
                    static_cast<void>(NewBattleNumber_41F1A0(action->x, y, displayed, 6, 2, flags));
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(action, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                }
            } else {
                if ((flags & 0x10000000u) != 0) {
                    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(gBattleAllEntries59E980[position], 22459, 0, 0));
                } else {
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    const std::int32_t amount = effect.position3;
                    if (amount != 0) {
                        (gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                        YobiAction* const compareTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        if ((compareTarget->readiness < 0)) {
                            std::optional<platform::LegacyCryptInt> temporary;
                            temporary.emplace(static_cast<std::int32_t>(0));
                            // 原416753..416781先取位置/encoded，再借目标；后两字段交替读写。
                            const std::int32_t copyIndex = payload->effectRunningC9C;
                            const std::int32_t copyPosition = payload->effects1C[copyIndex].position;
                            const std::uint32_t encoded = temporary->encoded;
                            YobiAction* const copyTarget = gBattleAllEntries59E980[copyPosition];
                            copyTarget->readiness.encoded = encoded;
                            copyTarget->readiness.mask = temporary->mask;
                            copyTarget->readiness.readCount = temporary->readCount;
                            temporary.reset();
                        }
                        const std::int32_t damageDisplayIndex = payload->effectRunningC9C;
                        const std::int32_t y = target->y - 80;
                        const std::int32_t displayed = payload->effects1C[damageDisplayIndex].position3;
                        static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 6, 2, flags));
                        if ((flags & 2u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                    } else {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[effect.position], 22331, 0, 0));
                    }
                }
                AdvanceBattleCastState_439340(target);
            }
        }
        if ((flags & 8u) == 0 && payload->effects1C[payload->effectRunningC9C].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
        }
        AdvanceBattleCastState_439340(action);
        static_cast<void>(AdvanceBattleEffect_439470(action));
    }
    const std::int32_t imageNo = (platform::Rand16() & 1) != 0 ? 110006 : 110004;
    static_cast<void>(NewBattlePositionEffect_41E680(action, imageNo));
    return PlayFrame_517DA0(action, 0);
}

// 原函数：0x00416860。
std::int32_t UpdateBattleAction233_416860(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if ((payload->effectFlagsFE4 & 0xC04) != 0) {
        action->characterAction = 9;
    }
    action->framePlayingRate = 50;
    const std::uint32_t initialFlags = payload->flagsFDC;
    const std::int32_t previousCount = payload->valueFCC;
    const std::int32_t initialBit = static_cast<std::int32_t>((initialFlags >> 1) & 1u);
    const std::int32_t count = previousCount + 1;
    payload->valueFCC = count;
    if (count < 14 * (initialBit + 1)) {
        const double angle = action->angle50 + 180.0;
        action->value54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        const std::uint8_t turnFlags = *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
        if ((turnFlags & 2u) != 0) {
            gYobiUpdateGateMode = 1;
            payload->aliveFF8 = 1;
        }
        return PlayFrame_517DA0(action, 1);
    }
    if (count < 14 * initialBit + 34) {
        const std::int32_t direction = action->direction + 1;
        action->direction = direction;
        if (direction >= 8) {
            action->direction = direction - 8;
        }
        const std::int32_t threshold = 14 * (static_cast<std::int32_t>((payload->flagsFDC >> 1) & 1u) + 1);
        if (payload->valueFCC == threshold) {
            action->value54 = 5.0F;
            const std::uint8_t thresholdFlags = *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
            if ((thresholdFlags & 2u) != 0) {
                if (CountBattleActiveEntries_406360() <= 1) {
                    gYobiUpdateGateMode = 0;
                }
                payload->aliveFF8 = 0;
            }
            payload->valueFF0 = 0;
        }
        if (payload->valueFF0 == 0) {
            static_cast<void>(NewBattlePositionEffect_41E680(action, 110006));
        }
        const std::int32_t previous = payload->valueFF0;
        payload->valueFF0 = previous + 1;
        if (previous >= 2) {
            payload->valueFF0 = 0;
        }
        return PlayFrame_517DA0(action, 1);
    }
    if (count >= 14 * initialBit + 54) {
        payload->valueFCC = 0;
        if ((initialFlags & 2u) == 0) {
            action->value54 = 6.0F;
            action->framePlayingRate = 100;
            AdvanceBattleCastState_439340(action);
            if (action->phase == 11) {
                action->value54 = 4.0F;
                action->framePlayingRate = 100;
            }
        } else if ((payload->effectFlagsFE4 & 0x3010) != 0) {
            action->value54 = 6.0F;
            action->framePlayingRate = 100;
            AdvanceBattleCastState_439340(action);
        } else {
            action->value54 = 1.0F;
            action->framePlayingRate = 250;
            AdvanceBattleCastState_439340(action);
        }
    }
    return PlayFrame_517DA0(action, 1);
}

// 原函数：0x00416AB0。
std::int32_t UpdateBattleAction234_416AB0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    action->framePlayingRate = 100;
    const std::int32_t count = payload->valueFCC + 1;
    payload->valueFCC = count;
    if (count == 1) {
        static_cast<void>(audio::PlaySe_52C450(207, action->x));
        const std::uint8_t initialFlags = *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
        if ((initialFlags & 2u) != 0) {
            if (CountBattleActiveEntries_406360() <= 1) {
                gYobiUpdateGateMode = 0;
            }
            const std::uint32_t state = payload->stateFE0;
            payload->aliveFF8 = 0;
            payload->stateFE0 = state | 2u;
        }
        for (std::int32_t remaining = 40; remaining != 0; --remaining) {
            static_cast<void>(NewBattleScatterEffect_41E510(action, 110004));
        }
        payload->valueFF0 = 0;
        payload->valueFD0 = 0;
        action->value54 = 15.0F;
        const std::uint8_t angleTimer = *reinterpret_cast<const std::uint8_t*>(&payload->valueFCC);
        if ((angleTimer & 1u) == 0) {
            action->angle50 = static_cast<float>(action->angle50 + 180.0);
        }
        const double angle = action->angle50;
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
    }
    MoveBattleByAngle_405EA0(action);
    const std::int32_t direction = action->direction + 1;
    action->direction = direction;
    if (direction >= 8) {
        action->direction = direction - 8;
    }
    if (payload->valueFF0 >= 3) {
        if (!(action->offset40 >= -200.0F) || action->offset40 > 840.0F
            || !(action->offset44 >= -200.0F) || action->offset44 > 680.0F) {
            AdvanceBattleCastState_439340(action);
            payload->valueFD0 = 0;
        }
    } else {
        static_cast<void>(render::LoadGraphic_522570(action->graphicIndex));
        const double halfWidth = render::ReadGraphicDxWidth(action->graphicIndex) * 0.5;
        if (halfWidth > action->offset40 && payload->valueFD0 != 1) {
            const double delta = action->angle50 - 90.0;
            action->angle50 = static_cast<float>(action->angle50 - (delta + delta));
            ++payload->valueFF0;
            static_cast<void>(audio::PlaySe_52C450(208, action->x));
            payload->valueFD0 = 1;
        } else if (action->offset40 > static_cast<float>(platform::gWindowWidth) - halfWidth && payload->valueFD0 != 2) {
            const double delta = action->angle50 - 270.0;
            action->angle50 = static_cast<float>(action->angle50 - (delta + delta));
            ++payload->valueFF0;
            static_cast<void>(audio::PlaySe_52C450(208, action->x));
            payload->valueFD0 = 2;
        } else if (static_cast<double>(-action->graphicOffsetY) > action->offset44 && payload->valueFD0 != 3) {
            const double delta = action->angle50 - 180.0;
            action->angle50 = static_cast<float>(action->angle50 - (delta + delta));
            ++payload->valueFF0;
            static_cast<void>(audio::PlaySe_52C450(208, action->x));
            payload->valueFD0 = 3;
        } else if (action->offset44 > static_cast<float>(platform::gWindowHeight) && payload->valueFD0 != 4) {
            const double angle = action->angle50;
            const double doubledAngle = angle + angle;
            const double currentAngle = action->angle50;
            action->angle50 = static_cast<float>(currentAngle - doubledAngle);
            ++payload->valueFF0;
            static_cast<void>(audio::PlaySe_52C450(208, action->x));
            payload->valueFD0 = 4;
        }
    }
    std::int32_t result;
    do {
        result = PlayFrame_517DA0(action, 1);
    } while (result == 0);
    return result;
}

// 原函数：0x00416DD0。
std::int32_t UpdateBattleAction237_416DD0(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 3;
    action->value54 = 10.0F;
    action->framePlayingRate = 100;
    bool applyEffect = true;
    if (payload->valueFF0 < 16) {
        MoveBattleByAngle_405EA0(action);
        if (payload->valueFF0 < 16) {
            const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
            applyEffect = GetBattleDistance_406300(action, gBattleAllEntries59E980[position]) < 20;
        }
    }
    if (applyEffect) {
        const std::int32_t index = payload->effectRunningC9C;
        YobiAction* const target = gBattleAllEntries59E980[payload->effects1C[index].position];
        const std::uint32_t flags = payload->effects1C[index].flags;
        auto* const targetPayload = target->payload.battleActor;
        TurnBattleToward_406190(target, action, 1, 0.0F);
        targetPayload->castSkill103C = -1;
        target->battleOpponent10 = gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        if ((flags & 8u) != 0) {
            target->phase = 31;
        } else if ((flags & 0x100u) != 0) {
            if (targetPayload->flash1004 == 0) {
                static_cast<void>(audio::PlaySe_52C450(210, action->x));
                targetPayload->flash1004 = 1;
                target->phase = 31;
            }
        } else {
            if ((flags & 0xC000u) == 0
                && payload->effects1C[payload->effectRunningC9C].position3 != 0) {
                target->offset40 = static_cast<float>(targetPayload->x10);
                target->offset44 = static_cast<float>(targetPayload->y14);
                if ((flags & 0x2000u) != 0) {
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(action, 110008, 0));
                    }
                    const std::int32_t imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                    NewBattleRebirthEffects_4208F0(action, target);
                    payload->effectFlagsFE4 = 0x2000;
                    if ((flags & 0x20u) != 0) {
                        payload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        payload->aliveFF8 = 1;
                    }
                } else {
                    if ((flags & 0x100000u) != 0) {
                        static_cast<void>(MarkDeath(targetPayload->effect1054));
                        const std::uint32_t targetFlags = targetPayload->flagsFDC;
                        targetPayload->effect1054 = nullptr;
                        targetPayload->flagsFDC = targetFlags & ~0x20u;
                    }
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                    }
                    const std::int32_t imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                    NewBattleRebirthEffects_4208F0(action, target);
                    if ((flags & 0x20u) != 0) {
                        targetPayload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        targetPayload->aliveFF8 = 1;
                    }
                }
            }
            if ((flags & 0x8000u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else if ((flags & 0x4000u) != 0) {
                AdvanceBattleCastState_439340(target);
                if ((flags & 0x10000000u) != 0) {
                    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(gBattleAllEntries59E980[position], 22459, 0, 0));
                } else {
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    const std::int32_t amount = effect.position3;
                    (gBattleAllEntries59E980[effect.position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const std::int32_t recoveryDisplayIndex = payload->effectRunningC9C;
                    const std::int32_t y = target->y - 80;
                    const std::int32_t displayed = payload->effects1C[recoveryDisplayIndex].position3;
                    static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 5, 2, flags));
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                }
            } else if ((flags & 0x2000u) != 0) {
                AdvanceBattleCastState_439340(action);
                AdvanceBattleCastState_439340(target);
                if ((flags & 0x10000000u) != 0) {
                    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(gBattleAllEntries59E980[position], 22459, 0, 0));
                } else {
                    (action->readiness -= static_cast<std::int32_t>(payload->effects1C[payload->effectRunningC9C].position3));
                    if ((action->readiness < 0)) {
                        // 原4171EA..417206先齐读encoded、mask、readCount，再写源HP。
                        // 现由整数赋值统一载荷发布：一次RNG，保留目标token与reserved。
                        action->readiness = std::int32_t{0};
                    }
                    const std::int32_t reflectionDisplayIndex = payload->effectRunningC9C;
                    const std::int32_t y = action->y - 80;
                    const std::int32_t displayed = payload->effects1C[reflectionDisplayIndex].position3;
                    static_cast<void>(NewBattleNumber_41F1A0(action->x, y, displayed, 6, 2, flags));
                    if ((flags & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(action, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                }
            } else {
                if ((flags & 0x10000000u) != 0) {
                    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(gBattleAllEntries59E980[position], 22459, 0, 0));
                } else {
                    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
                    const std::int32_t amount = effect.position3;
                    if (amount != 0) {
                        (gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                        YobiAction* const compareTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        if ((compareTarget->readiness < 0)) {
                            std::optional<platform::LegacyCryptInt> temporary;
                            temporary.emplace(static_cast<std::int32_t>(0));
                            // 原4172E8..417316先取索引/encoded，再取位置/目标；后两字段交替。
                            const std::int32_t copyIndex = payload->effectRunningC9C;
                            const std::uint32_t encoded = temporary->encoded;
                            const std::int32_t copyPosition = payload->effects1C[copyIndex].position;
                            YobiAction* const copyTarget = gBattleAllEntries59E980[copyPosition];
                            copyTarget->readiness.encoded = encoded;
                            copyTarget->readiness.mask = temporary->mask;
                            copyTarget->readiness.readCount = temporary->readCount;
                            temporary.reset();
                        }
                        const std::int32_t damageDisplayIndex = payload->effectRunningC9C;
                        const std::int32_t y = target->y - 80;
                        const std::int32_t displayed = payload->effects1C[damageDisplayIndex].position3;
                        static_cast<void>(NewBattleNumber_41F1A0(target->x, y, displayed, 6, 2, flags));
                        if ((flags & 2u) != 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                        }
                        static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                    } else {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[effect.position], 22331, 0, 0));
                    }
                }
                AdvanceBattleCastState_439340(target);
            }
        }
        if ((flags & 8u) == 0 && payload->effects1C[payload->effectRunningC9C].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
        }
        if ((flags & 0x2000u) == 0 && payload->valueFF0 < 16) {
            action->phase = 5;
        }
        static_cast<void>(AdvanceBattleEffect_439470(action));
    }
    const std::int32_t previous = payload->valueFF0;
    payload->valueFF0 = previous + 1;
    if (previous >= 16) {
        AdvanceBattleCastState_439340(action);
        payload->valueFF0 = 0;
    }
    return PlayFrame_517DA0(action, 0);
}

// 原函数：0x00417410。
void UpdateBattleActions238_239_417410(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    const BattleEntryEffectRecord& initial = payload->effects1C[payload->effectRunningC9C];
    if ((initial.flags & 0x20000000u) != 0
        && GetBattleDistance_406300(action, gBattleAllEntries59E980[initial.position]) < 70
        && action->frameEvent == 0) {
        YobiAction* const target = gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        static_cast<void>(MarkDeath(action));
        action->phase = 17;
        YobiAction* const effect = NewBattleRandomEffect_419FC0(action, 110473, 0);
        if (effect != nullptr) {
            effect->y -= 68;
        }
        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110474, 0));
        AdvanceBattleCastState_439340(target);
        return;
    }
    const std::int32_t distancePosition = payload->effects1C[payload->effectRunningC9C].position;
    if (GetBattleDistance_406300(action, gBattleAllEntries59E980[distancePosition]) < 20
        && action->frameEvent == 0) {
        const BattleEntryEffectRecord& current = payload->effects1C[payload->effectRunningC9C];
        const std::int32_t position = current.position;
        YobiAction* const target = gBattleAllEntries59E980[position];
        const std::uint32_t initialFlags = current.flags;
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t imageNo = -1;
        std::int32_t secondImageNo = -1;
        if ((initialFlags & 4u) != 0) {
            imageNo = 110000;
        } else if ((initialFlags & 0x400u) != 0) {
            imageNo = FindBattleAppendEffectTechnique_43BEA0(8, position) < 12 ? 110098 : 111048;
        } else {
            const std::int32_t skill = payload->castSkill103C;
            if (skill == 267 || skill == 3029) {
                secondImageNo = 110477;
            } else {
                imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
            }
        }
        TurnBattleToward_406190(target, action, 1, 0.0F);
        target->battleOpponent10 = gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        const std::int32_t hitIndex = payload->effectRunningC9C;
        const std::uint32_t hitFlags = payload->effects1C[hitIndex].flags;
        if ((hitFlags & 8u) != 0) {
            target->phase = 31;
        } else if ((hitFlags & 0x100u) != 0) {
            targetPayload->flash1004 = 1;
            static_cast<void>(audio::PlaySe_52C450(210, action->x));
            target->phase = 31;
        } else {
            if ((hitFlags & 0xE000u) == 0 && payload->effects1C[hitIndex].position3 != 0) {
                if ((hitFlags & 2u) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                if (imageNo >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                }
                if (secondImageNo >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, secondImageNo, 0));
                }
                NewBattleRebirthEffects_4208F0(gBattleAllEntries59E980[payload->battlePosition18], target);
                static_cast<void>(audio::PlaySe_52C450(255, action->x));
                const BattleEntryEffectRecord& damage = payload->effects1C[payload->effectRunningC9C];
                const std::int32_t amount = damage.position3;
                (gBattleAllEntries59E980[damage.position]->readiness -= static_cast<std::int32_t>(amount));
                YobiAction* const compareTarget = gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                if ((compareTarget->readiness < 0)) {
                    std::optional<platform::LegacyCryptInt> temporary;
                    temporary.emplace(static_cast<std::int32_t>(0));
                    // 原4176DA..41770C先取索引/encoded，再借共享目标；后两字段交替。
                    const std::int32_t copyIndex = payload->effectRunningC9C;
                    const std::uint32_t encoded = temporary->encoded;
                    const std::int32_t copyPosition = payload->effects1C[copyIndex].position;
                    YobiAction* const copyTarget = gBattleAllEntries59E980[copyPosition];
                    copyTarget->readiness.encoded = encoded;
                    copyTarget->readiness.mask = temporary->mask;
                    copyTarget->readiness.readCount = temporary->readCount;
                    temporary.reset();
                }
                const std::uint8_t activationFlags = *reinterpret_cast<const std::uint8_t*>(
                    &payload->effects1C[payload->effectRunningC9C].flags);
                if ((activationFlags & 0x20u) != 0) {
                    targetPayload->flagsFDC |= 2u;
                    gYobiUpdateGateMode = 1;
                    targetPayload->aliveFF8 = 1;
                }
            }
            const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
            bool showAttributes = false;
            if ((flags & 0x8000u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else if ((flags & 0x4000u) != 0) {
                AdvanceBattleCastState_439340(target);
                const std::int32_t healIndex = payload->effectRunningC9C;
                if ((payload->effects1C[healIndex].flags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        gBattleAllEntries59E980[payload->effects1C[healIndex].position], 22459, 0, 0));
                } else {
                    const std::int32_t amount = payload->effects1C[healIndex].position3;
                    (gBattleAllEntries59E980[payload->effects1C[healIndex].position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const BattleEntryEffectRecord& display = payload->effects1C[payload->effectRunningC9C];
                    const std::uint32_t displayFlags = display.flags;
                    const std::int32_t displayed = display.position3;
                    const std::int32_t y = target->y - 80;
                    const std::int32_t x = target->x;
                    static_cast<void>(NewBattleNumber_41F1A0(x, y, displayed, 5, 2, displayFlags));
                    showAttributes = true;
                }
            } else if ((flags & 0x2000u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else {
                if ((flags & 0x100000u) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->effect1054));
                    const std::uint32_t targetFlags = targetPayload->flagsFDC;
                    targetPayload->effect1054 = nullptr;
                    targetPayload->flagsFDC = targetFlags & ~0x20u;
                }
                AdvanceBattleCastState_439340(target);
                const std::int32_t displayIndex = payload->effectRunningC9C;
                const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                if ((displayFlags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        gBattleAllEntries59E980[payload->effects1C[displayIndex].position], 22459, 0, 0));
                } else {
                    const std::int32_t y = target->y - 80;
                    const std::int32_t displayed = payload->effects1C[displayIndex].position3;
                    const std::int32_t x = target->x;
                    static_cast<void>(NewBattleNumber_41F1A0(x, y, displayed, 6, 2, displayFlags));
                    showAttributes = true;
                }
            }
            if (showAttributes) {
                const std::uint8_t attributeFlags = *reinterpret_cast<const std::uint8_t*>(
                    &payload->effects1C[payload->effectRunningC9C].flags);
                if ((attributeFlags & 2u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                }
                static_cast<void>(NewBattleAttributeEffects_4209C0(
                    gBattleAllEntries59E980[payload->battlePosition18], target));
            }
        }
        const std::int32_t tailIndex = payload->effectRunningC9C;
        const std::uint8_t tailFlags =
            *reinterpret_cast<const std::uint8_t*>(&payload->effects1C[tailIndex].flags);
        if ((tailFlags & 8u) == 0
            && payload->effects1C[tailIndex].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
        }
        action->frameEvent = 1;
    }
    const std::int32_t remaining = payload->valueFD0;
    if (remaining > 0) {
        payload->valueFD0 = remaining - 1;
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    // 与计时递减独立：归零当帧还会再次播放，且播放可能改写帧事件。
    if (payload->valueFD0 == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
        const std::int32_t phase = action->phase;
        const double speed = action->value54;
        action->value54 = static_cast<float>(phase == 238 ? speed - 0.5 : speed + 0.5);
        if (!(action->value54 > 1.0F)) {
            const float boundarySpeed = action->value54;
            const bool reverseSpeed = !(boundarySpeed >= 0.0F);
            action->phase = 239;
            if (reverseSpeed) {
                action->value54 = -action->value54;
            }
        }
        MoveBattleByAngle_405EA0(action);
        if (action->frameEvent == 1) {
            const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
            if ((flags & 8u) == 0 && (flags & 0x100u) == 0) {
                static_cast<void>(MarkDeath(action));
            }
            action->phase = 17;
        } else {
            const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
            TurnBattleToward_406190(action, gBattleAllEntries59E980[position], 1, 6.0F);
        }
    }
}

// 原函数：0x00417AC0。
void UpdateBattleAction240_417AC0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 6;
    action->framePlayingRate = 100;
    if (payload->value1024 == 0) {
        action->battleCastEffect10 = NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 34, 0, 1);
    }
    static_cast<void>(PlayFrame_517DA0(action, 1));
    const std::int32_t event = action->frameEvent;
    if (event >= 10000 && event < 20000) {
        if (action->battleCastEffect10 != nullptr) {
            static_cast<void>(MarkDeath(action->battleCastEffect10));
            action->battleCastEffect10 = nullptr;
        }
        payload->value1024 = 0;
        action->phase = 241;
        payload->valueFF0 = 0;
    }
    if (action->battleCastEffect10 != nullptr) {
        const std::int32_t skill = payload->castSkill103C;
        if (skill == 267 || skill == 3029) {
            static_cast<void>(DrawBattleTiledEffect_41E890(5));
        }
    }
}

// 原函数：0x00417D80。
void UpdateBattleAction243_417D80(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    const BattleEntryEffectRecord& initial = payload->effects1C[payload->effectRunningC9C];
    const std::uint8_t turnFlags = *reinterpret_cast<const std::uint8_t*>(&initial.flags);
    if ((turnFlags & 8u) == 0) {
        TurnBattleToward_406190(action, gBattleAllEntries59E980[initial.position], 0, 0.0F);
    }
    const std::int32_t distancePosition = payload->effects1C[payload->effectRunningC9C].position;
    if (GetBattleDistance_406300(action, gBattleAllEntries59E980[distancePosition]) < 20
        && action->frameEvent == 0) {
        const BattleEntryEffectRecord& current = payload->effects1C[payload->effectRunningC9C];
        const std::int32_t position = current.position;
        const std::uint32_t initialFlags = current.flags;
        YobiAction* const target = gBattleAllEntries59E980[position];
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t imageNo = -1;
        std::int32_t secondImageNo = -1;
        std::int32_t imageMode = 0;
        if ((initialFlags & 4u) != 0) {
            imageNo = 110000;
        } else if ((initialFlags & 0x800u) != 0) {
            imageNo = 110099;
        } else if ((initialFlags & 0x400u) != 0) {
            imageNo = FindBattleAppendEffectTechnique_43BEA0(8, position) < 12 ? 110098 : 111048;
        } else {
            const std::int32_t skill = payload->castSkill103C;
            switch (skill) {
            case 0:
            case 2007:
            case 2008:
            case 2021:
                imageNo = payload->castSkillLevel1044 < 12 ? 110133 : 111029;
                break;
            case 1: imageNo = payload->castSkillLevel1044 < 12 ? 110134 : 111047; break;
            case 3: imageNo = payload->castSkillLevel1044 < 12 ? 110135 : 111046; break;
            case 5: imageNo = payload->castSkillLevel1044 < 12 ? 110139 : 111052; break;
            case 6: imageNo = payload->castSkillLevel1044 < 12 ? 110137 : 111050; break;
            case 81: imageNo = 110174; break;
            case 84: imageNo = 110175; break;
            case 85: imageNo = 110159; break;
            case 87: imageNo = 110160; break;
            case 110: imageNo = 110074; break;
            case 257: {
                const std::int32_t technique = payload->castSkillLevel1044;
                secondImageNo = 110476;
                if (technique >= 12) {
                    secondImageNo = 111051;
                } else if (technique >= 9) {
                    imageNo = 110014;
                }
                break;
            }
            case 258: secondImageNo = 110477; break;
            case 259:
                imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                secondImageNo = 110470;
                break;
            case 260: imageNo = payload->castSkillLevel1044 < 12 ? 110473 : 111054; break;
            case 262:
                if (payload->castSkillLevel1044 < 12) {
                    imageNo = 110473;
                    imageMode = 1;
                } else {
                    imageNo = 111049;
                    imageMode = 0;
                }
                break;
            default:
                if ((skill >= 75 && skill <= 80) || (skill >= 2096 && skill <= 2101)) {
                    imageNo = 110074;
                } else {
                    imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                }
                break;
            }
        }
        TurnBattleToward_406190(target, action, 1, 0.0F);
        target->battleOpponent10 = gBattleAllEntries59E980[payload->battlePosition18];
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        if (payload->castSkillLevel1044 >= 9 && payload->castSkill103C == 259) {
            static_cast<void>(NewBattleTiledEffect_41E9D0(0, 15));
        }
        const std::int32_t hitIndex = payload->effectRunningC9C;
        const std::uint32_t hitFlags = payload->effects1C[hitIndex].flags;
        if ((hitFlags & 8u) != 0) {
            target->phase = 31;
        } else if ((hitFlags & 0x100u) != 0) {
            targetPayload->flash1004 = 1;
            static_cast<void>(audio::PlaySe_52C450(210, action->x));
            target->phase = 31;
        } else {
            if ((hitFlags & 0x7E000u) == 0 && payload->effects1C[hitIndex].position3 != 0) {
                if ((hitFlags & 2u) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                if (imageNo >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, imageMode));
                }
                if (secondImageNo >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, secondImageNo, 0));
                }
                NewBattleRebirthEffects_4208F0(gBattleAllEntries59E980[payload->battlePosition18], target);
                static_cast<void>(audio::PlaySe_52C450(255, action->x));
            }
            const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
            bool showAttributes = false;
            if ((flags & 0x48000u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else if ((flags & 0x24000u) != 0) {
                AdvanceBattleCastState_439340(target);
                const std::int32_t healIndex = payload->effectRunningC9C;
                if ((payload->effects1C[healIndex].flags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        gBattleAllEntries59E980[payload->effects1C[healIndex].position], 22459, 0, 0));
                } else {
                    const std::int32_t amount = payload->effects1C[healIndex].position3;
                    (gBattleAllEntries59E980[payload->effects1C[healIndex].position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const BattleEntryEffectRecord& display = payload->effects1C[payload->effectRunningC9C];
                    const std::uint32_t displayFlags = display.flags;
                    const std::int32_t y = target->y - 80;
                    const std::int32_t displayed = display.position3;
                    const std::int32_t x = target->x;
                    static_cast<void>(NewBattleNumber_41F1A0(x, y, displayed, 5, 2, displayFlags));
                    showAttributes = true;
                }
            } else if ((flags & 0x12000u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else {
                if ((flags & 0x100000u) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->effect1054));
                    const std::uint32_t targetFlags = targetPayload->flagsFDC;
                    targetPayload->effect1054 = nullptr;
                    targetPayload->flagsFDC = targetFlags & ~0x20u;
                }
                AdvanceBattleCastState_439340(target);
                const std::int32_t displayIndex = payload->effectRunningC9C;
                if ((payload->effects1C[displayIndex].flags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        gBattleAllEntries59E980[payload->effects1C[displayIndex].position], 22459, 0, 0));
                }
                showAttributes = true;
            }
            if (showAttributes) {
                const std::uint8_t attributeFlags = *reinterpret_cast<const std::uint8_t*>(
                    &payload->effects1C[payload->effectRunningC9C].flags);
                if ((attributeFlags & 2u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                }
                static_cast<void>(NewBattleAttributeEffects_4209C0(
                    gBattleAllEntries59E980[payload->battlePosition18], target));
            }
        }
        const std::int32_t tailIndex = payload->effectRunningC9C;
        const std::uint8_t tailFlags =
            *reinterpret_cast<const std::uint8_t*>(&payload->effects1C[tailIndex].flags);
        if ((tailFlags & 8u) == 0
            && payload->effects1C[tailIndex].position3 != 0) {
            const std::int32_t duration = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
            const std::int32_t skill = payload->castSkill103C;
            payload->valueFD0 = duration;
            if (skill == 267 || skill == 3029) {
                payload->valueFD0 = 2;
            }
        }
        action->frameEvent = 1;
    }
    const std::int32_t remaining = payload->valueFD0;
    if (remaining > 0) {
        payload->valueFD0 = remaining - 1;
        const std::int32_t imageNo = action->imageNo;
        if (imageNo == 110016 || imageNo == 111055) {
            static_cast<void>(PlayFrame_517DA0(action, 0));
        }
    }
    if (payload->valueFD0 == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
        MoveBattleByAngle_405EA0(action);
        if (action->frameEvent == 1) {
            const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
            if ((flags & 8u) == 0 && (flags & 0x100u) == 0) {
                static_cast<void>(MarkDeath(action));
            }
            action->phase = 17;
        }
    }
}

// 原函数：0x00418550。
void UpdateBattleAction244_418550(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    payload->flagsFDC |= 2u;
    gYobiUpdateGateMode = 1;
    payload->aliveFF8 = 1;
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00418580。
void UpdateBattleAction245_418580(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
    const std::int32_t lifetime = effect.position3;
    const std::int32_t type = effect.position;
    static_cast<void>(NewBattleTiledEffect_41E9D0(type, lifetime));
    static_cast<void>(AdvanceBattleEffect_439470(action));
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x004185C0。
void UpdateBattleAction246_4185C0(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const std::int32_t value = payload->effects1C[payload->effectRunningC9C].position3;
    action->value54 = static_cast<float>(static_cast<double>(value) * 0.01F);
    static_cast<void>(AdvanceBattleEffect_439470(action));
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x004185F0。
void UpdateBattleAction247_4185F0(YobiAction* const action)
{
    const float x = action->offset40;
    auto* const payload = action->payload.battleActor;
    payload->x10 = static_cast<std::int32_t>(x);
    payload->y14 = static_cast<std::int32_t>(action->offset44);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00418620。
std::int32_t UpdateBattleAction248_418620(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    action->characterAction = 1;
    action->value54 = 8.0F;
    action->framePlayingRate = 100;
    const float turnY = static_cast<float>(payload->y14);
    const float turnX = static_cast<float>(payload->x10);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->y14);
    const float distanceX = static_cast<float>(payload->x10);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->x10);
        action->offset44 = static_cast<float>(payload->y14);
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, 0);
}

// 原函数：0x004186B0。
void UpdateBattleAction255_4186B0(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    const std::int32_t event = action->frameEvent;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 5;
    action->framePlayingRate = 100;
    if (event >= 10000 && event < 20000) {
        for (;;) {
            const std::int32_t index = payload->effectRunningC9C;
            const BattleEntryEffectRecord& effect = payload->effects1C[index];
            const std::int32_t position = effect.position;
            if (position == -1) {
                break;
            }
            YobiAction* const target = gBattleAllEntries59E980[position];
            if (target == nullptr) {
                break;
            }
            auto* const targetPayload = target->payload.battleActor;
            if (index == 0) {
                const std::int32_t x = position < 10 ? 500 : 150;
                const std::int32_t y = position < 10 ? 450 : 50;
                static_cast<void>(NewBattleEffect_41CD00(action, x, {.offsetY = y}, 47, 0, 1));
            }
            const std::uint32_t initialFlags = effect.flags;
            std::int32_t imageNo;
            if ((initialFlags & 4u) != 0) {
                imageNo = 110000;
            } else if ((initialFlags & 0x400u) != 0) {
                imageNo = FindBattleAppendEffectTechnique_43BEA0(8, effect.position) < 12 ? 110098 : 111048;
            } else if ((initialFlags & 0x200u) != 0) {
                imageNo = 110135;
            } else if (payload->effectRunningC9C == 0) {
                imageNo = 110135;
            } else {
                imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
            }
            targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(effect.flags);
            ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
            const std::uint32_t hitFlags = effect.flags;
            if ((hitFlags & 8u) != 0) {
                target->phase = 31;
            } else if ((hitFlags & 0x100u) != 0) {
                targetPayload->flash1004 = 1;
                static_cast<void>(audio::PlaySe_52C450(210, action->x));
                target->phase = 31;
            } else {
                if ((hitFlags & 0xE000u) == 0 && effect.position3 != 0) {
                    if ((hitFlags & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                    }
                    if (imageNo >= 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                    }
                    NewBattleRebirthEffects_4208F0(action, target);
                    static_cast<void>(audio::PlaySe_52C450(134, action->x));
                    const std::int32_t damagePosition = effect.position;
                    const std::int32_t amount = effect.position3;
                    (gBattleAllEntries59E980[damagePosition]->readiness -= static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = gBattleAllEntries59E980[effect.position];
                    if ((compareTarget->readiness < 0)) {
                        std::optional<platform::LegacyCryptInt> temporary;
                        temporary.emplace(static_cast<std::int32_t>(0));
                        const std::int32_t copyPosition = effect.position;
                        const std::uint32_t encoded = temporary->encoded;
                        YobiAction* const copyTarget = gBattleAllEntries59E980[copyPosition];
                        copyTarget->readiness.encoded = encoded;
                        copyTarget->readiness.mask = temporary->mask;
                        copyTarget->readiness.readCount = temporary->readCount;
                        temporary.reset();
                    }
                    const std::uint8_t activationFlags =
                        *reinterpret_cast<const std::uint8_t*>(&effect.flags);
                    if ((activationFlags & 0x20u) != 0) {
                        targetPayload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        targetPayload->aliveFF8 = 1;
                    }
                }
                const std::uint32_t flags = effect.flags;
                bool showAttributes = false;
                if ((flags & 0x8000u) != 0) {
                    AdvanceBattleCastState_439340(target);
                } else if ((flags & 0x4000u) != 0) {
                    AdvanceBattleCastState_439340(target);
                    if ((effect.flags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[effect.position], 22459, 0, 0));
                    } else {
                        YobiAction* const healTarget = gBattleAllEntries59E980[effect.position];
                        const std::int32_t amount = healTarget->counter1DC;
                        (healTarget->readiness += static_cast<std::int32_t>(amount));
                        YobiAction* const compareTarget = gBattleAllEntries59E980[effect.position];
                        if ((compareTarget->readiness > compareTarget->maxHp)) {
                            YobiAction* const copyTarget = gBattleAllEntries59E980[effect.position];
                            copyTarget->readiness = copyTarget->maxHp;
                        }
                        const std::uint32_t displayFlags = effect.flags;
                        const std::int32_t y = target->y - 80;
                        const std::int32_t displayed = effect.position3;
                        const std::int32_t x = target->x;
                        static_cast<void>(NewBattleNumber_41F1A0(x, y, displayed, 5, 2, displayFlags));
                        showAttributes = true;
                    }
                } else if ((flags & 0x2000u) != 0) {
                    AdvanceBattleCastState_439340(target);
                } else {
                    if ((flags & 0x100000u) != 0) {
                        static_cast<void>(MarkDeath(targetPayload->effect1054));
                        const std::uint32_t targetFlags = targetPayload->flagsFDC;
                        targetPayload->effect1054 = nullptr;
                        targetPayload->flagsFDC = targetFlags & ~0x20u;
                    }
                    AdvanceBattleCastState_439340(target);
                    const std::uint32_t displayFlags = effect.flags;
                    if ((displayFlags & 0x10000000u) != 0) {
                        // 普通免疫图标取当前C9C；本轮其余操作仍使用固定effect。
                        const std::int32_t currentPosition = payload->effects1C[payload->effectRunningC9C].position;
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            gBattleAllEntries59E980[currentPosition], 22459, 0, 0));
                    } else {
                        const std::int32_t y = target->y - 80;
                        const std::int32_t displayed = effect.position3;
                        const std::int32_t x = target->x;
                        static_cast<void>(NewBattleNumber_41F1A0(x, y, displayed, 6, 2, displayFlags));
                        showAttributes = true;
                    }
                }
                if (showAttributes) {
                    const std::uint8_t attributeFlags =
                        *reinterpret_cast<const std::uint8_t*>(&effect.flags);
                    if ((attributeFlags & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                }
            }
            const std::uint8_t tailFlags = *reinterpret_cast<const std::uint8_t*>(&effect.flags);
            if ((tailFlags & 8u) == 0 && effect.position3 != 0) {
                payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
            }
            if (AdvanceBattleEffect_439470(action) == 1) {
                break;
            }
        }
        static_cast<void>(audio::PlaySe_52C450(268, action->x));
        action->value54 = 8.0F;
        action->framePlayingRate = 100;
        payload->valueFF0 = 0;
        action->frameEvent = 0;
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x00418B50。
void UpdateBattleAction256_418B50(YobiAction* const)
{
    world::detail::gValue5ABB1C = 0;
    world::detail::gValue5ABB20 = 1;
}

} // namespace detail

} // namespace cg::yobi
