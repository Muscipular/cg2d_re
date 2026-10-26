#include "battle_message.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_scene.hpp"
#include "battle_skill.hpp"
#include "battle_skill_effect.hpp"
#include "battle_status.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/world/SceneState.hpp"

#include "animation.hpp"
#include "battle_actor.hpp"
#include "battle_attachment.hpp"
#include "battle_display.hpp"
#include "battle_effect.hpp"
#include "battle_impact.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"
#include "../world/ChatHistory.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>

namespace cg::yobi {

namespace detail {

// YMN17：DSK完整字段等值选择追加技能；未闭合服务端生产，不与BM位混用。
enum BattleAppendSkillSelector : std::uint32_t {
    BattleAppendSkillSelector_SpecialGuard = 0x400u,
    BattleAppendSkillSelector_CrossCounter = 0x1000u,
};


// Transition／Status／POS／ITM／GMN／RID两阶段门禁；不解释MON、CHT、CMB或SKL。
enum BattleMQueuedPhase : std::int32_t {
    BattleMQueuedPhase_ReadAndQueue = 0,
    BattleMQueuedPhase_WaitForTurnReady = 1,
};

// MON子阶段；2等待旧配对槽不忙后继续解析，完成显式置1。
enum BattleMMonsterPhase : std::int32_t {
    BattleMMonsterPhase_ReadActor = 0,
    BattleMMonsterPhase_WaitForActors = 1,
    BattleMMonsterPhase_WaitForDefenderNotBusy = 2,
};

// CHT子阶段；显示消息后逐次减少原计时值。
enum BattleMChatPhase : std::int32_t {
    BattleMChatPhase_ReadAndShow = 0,
    BattleMChatPhase_CountDown = 1,
};

// GMN仅按退场／入口选择命名；未知非零模式保持原创建路径。
enum BattleMGmnMode : std::int32_t {
    BattleMGmnMode_RemoveActorAndRevealTeam = 0,
    BattleMGmnMode_EnterWithEntrance2 = 1,
    BattleMGmnMode_EnterWithRandomEntranceAndEmitter = 2,
};


// 原427470的PSN内部模式；具体状态名未证，只按资源变化及表现命名。
enum BattleMStatusValueMode : std::int32_t {
    BattleMStatusValueMode_DecreaseHpWithHit = 0, // 减HP并播放受击动画。
    BattleMStatusValueMode_IncreaseHpWithRecoveryEffect = 1, // 状态HP恢复效果及资源变更等待。
    BattleMStatusValueMode_DecreaseFp = 2, // 减FP并等待。
    BattleMStatusValueMode_DecreaseHpAndWait = 3, // 减HP，按死亡标记选择等待路径。
    BattleMStatusValueMode_DecreaseHpWithImage104216AndWait = 4, // 在模式3公共路径中追加图104216。
};

// 原ITM的0／6／7；预扫描与执行共用，结果字段及恢复效果字段另有解释域。
enum BattleMItemMode : std::int32_t {
    BattleMItemMode_RecoverTargetHp = 0, // 按目标列表恢复HP。
    BattleMItemMode_SealCard = 6, // 封印卡三阶段及成功／失败表现。
    BattleMItemMode_PlayNodAndWait = 7, // 音效438、点头及等待；物品身份未证。
};

// 原42782E四项分派；仅解释CMB的packetSubState58，原递增及计数门槛保持。
enum BattleMComboPhase : std::int32_t {
    BattleMComboPhase_ReadAndQueue = 0, // 读取消息并建立合击动作队列。
    BattleMComboPhase_WaitForEntryCompletions = 1, // 等待完成计数达到参与者数。
    BattleMComboPhase_AdvanceEntriesAndShowSequence = 2, // 推进参与者，按原条件显示序列。
    BattleMComboPhase_WaitForTurnReady = 3, // 等待回合就绪并结束事件。
};

// 原427470独占的同源W32；活动路径先从载荷恢复，失活消息先解析位置，不依赖初值。
std::int32_t gBattleMCasterPosition5AAA64 = 0; // 原全局：0x005AAA64。

std::uint32_t gBattleMResultFlags5A4A20 = 0; // 原全局：0x005A4A20。

std::int32_t gBattleMValue5ABA7C = 0; // 原全局：0x005ABA7C；与载荷value70共用角色位置及累计技能重复标记，不能统一命名为位置，保留原名。

static std::int32_t gBattleMDefenderPosition5A4A24 = 0; // 原全局：0x005A4A24。

std::int32_t gBattleMTargetPosition5AAA40 = 0; // 原全局：0x005AAA40。

std::int32_t gBattleMSkill5AAA78 = 0; // 原全局：0x005AAA78。

std::int32_t gBattleMTechnique5A4A38 = 0; // 原全局：0x005A4A38。

static std::int32_t gBattleMItemMode59FA10 = 0; // 原全局：0x0059FA10。

std::int32_t gBattleMSkill98Mode5A4A34 = 0; // 原全局：0x005A4A34。

std::int32_t gBattleMFeedbackSourcePosition5ABA90 = 0; // 原全局：0x005ABA90。

std::uint32_t gBattleMFlags5ABA84 = 0; // 原全局：0x005ABA84。

std::int32_t gBattleMFeedbackEffectImage5A4A28 = 0; // 原全局：0x005A4A28。

std::int32_t gBattleMMessageValue59FA14 = 0; // 原全局：0x0059FA14。

static std::int32_t gBattleMStatusMode5ABA88 = 0; // 原全局：0x005ABA88。 PSN先写后读，不属于13项回存。

std::int32_t gBattleMSkillLevel5A4A2C = 0; // 原全局：0x005A4A2C。 SKL状态0先写后读，不属于13项快照。

std::int32_t gBattleMSkillLevelIndex5A3A18 = 0; // 原全局：0x005A3A18。 与88/8C独立的归一化等级索引。

enum class BattleMTransitionHeader : std::int32_t {
    Escape = BattleMHeader_ESC, Clear = BattleMHeader_CLR, Support = BattleMHeader_SUP, PetNotice = BattleMHeader_PNO, PetLeave = BattleMHeader_PLV, PetDown = BattleMHeader_PDN,
};

// 439B30的SKL字段布局；同布局合并不改变技能集合、RID尾扫描或特殊字段。
enum class BattlePreviewSkillLayout {
    None, Attack, Mode73, Damage, ValuesRide, PairRide, ValuesOnly, PairOnly,
    SourceValue, NamedTargets, Mode98, OptionalPair, DirectValues, Team,
    DirectRecords, RideRecords, EffectRecords,
};

static BattlePreviewSkillLayout GetBattlePreviewSkillLayout(const std::int32_t skill)
{
    switch (skill) {
    case BattleSkill_ContinuationAttack: case BattleSkill_Parameter_DoubleEdgedBlade:
    case BattleSkill_Parameter_AllOrNothing: case BattleSkill_GuardBreak: case BattleSkill_Forcecut: case BattleSkill_Steal:
    case BattleSkill_StatusAttackPoison: case BattleSkill_StatusAttackSleep: case BattleSkill_StatusAttackStone:
    case BattleSkill_StatusAttackInebriety: case BattleSkill_StatusAttackConfusion:
    case BattleSkill_StatusAttackForget: case BattleSkill_BloodAttack:
    case BattleSkill_EnergyDrain: case BattleSkill_EquipBreakAttack: case BattleSkill_GoldAttack: case BattleSkill_PanicAttack: case BattleSkill_RandomShot: case BattleSkill_Assassin:
    case BattleSkill_Ranbu: case 110: case 113:
    case BattleSkill_DelayAttack257: case BattleSkill_DelayAttack258: case BattleSkill_StatusAttackPoisonStrike:
    case BattleSkill_Wattack: case BattleSkill_FirstAttack: case BattleSkill_Madmax:
    case BattleSkill_Recklessness: case BattleSkill_Taunt: case BattleSkill_ShortStrike: case BattleSkill_ShadowHand:
    case BattleSkill_MercyStrike: case BattleSkill_KarmaCycle:
    case BattleSkill_StatusAttackPoison2096: case BattleSkill_StatusAttackSleep2097:
    case BattleSkill_StatusAttackStone2098: case BattleSkill_StatusAttackInebriety2099:
    case BattleSkill_StatusAttackConfusion2100: case BattleSkill_StatusAttackForget2101:
    case 3019: case 3038: case BattleSkill_SwallowSlash: case BattleSkill_BladeSoul: case 4002:
        return BattlePreviewSkillLayout::Attack;
    case BattleSkill_NormalAttack:
        return BattlePreviewSkillLayout::Mode73;
    case BattleSkill_SpiracleShot: case BattleSkill_AttackMagicEarth: case BattleSkill_AttackMagicWater:
    case BattleSkill_AttackMagicFire: case BattleSkill_AttackMagicWind: case BattleSkill_AttackMagicEarthStrong:
    case BattleSkill_AttackMagicWaterStrong: case BattleSkill_AttackMagicFireStrong:
    case BattleSkill_AttackMagicWindStrong: case BattleSkill_AttackMagicEarthSuper:
    case BattleSkill_AttackMagicWaterSuper: case BattleSkill_AttackMagicFireSuper:
    case BattleSkill_AttackMagicWindSuper: case BattleSkill_Dorain: case BattleSkill_Bomb: case BattleSkill_EarthQuake:
    case BattleSkill_MagicMissile: case BattleSkill_BlastWave: case 2055: case 2056: case 2057:
    case 2068: case 2069: case 2070: case 2071: case 2072: case 2073:
    case 2074: case 2075: case 2076: case 2077: case 2078: case 2079:
    case BattleSkill_ConfusionShockwave: case 3037: case BattleSkill_WindRendingStrike:
        return BattlePreviewSkillLayout::Damage;
    case BattleSkill_StatusChangePoison: case BattleSkill_StatusChangeSleep: case BattleSkill_StatusChangeStone:
    case BattleSkill_StatusChangeInebriety: case BattleSkill_StatusChangeConfusion:
    case BattleSkill_StatusChangeForget: case BattleSkill_StatusChangePoisonStrong:
    case BattleSkill_StatusChangeSleepStrong:
    case BattleSkill_StatusChangeStoneStrong: case BattleSkill_StatusChangeInebrietyStrong:
    case BattleSkill_StatusChangeConfusionStrong: case BattleSkill_StatusChangeForgetStrong:
    case BattleSkill_StatusChangePoisonSuper: case BattleSkill_StatusChangeSleepSuper:
    case BattleSkill_StatusChangeStoneSuper: case BattleSkill_StatusChangeInebrietySuper:
    case BattleSkill_StatusChangeConfusionSuper: case BattleSkill_StatusChangeForgetSuper:
    case BattleSkill_ReverseType: case BattleSkill_ReflectionPhysics: case BattleSkill_ReflectionMagic:
    case BattleSkill_AbsorbPhysics: case BattleSkill_AbsorbMagic: case BattleSkill_IneffectivePhysics:
    case BattleSkill_IneffectiveMagic:
    case BattleSkill_LpRecovery: case BattleSkill_LpRecoveryStrong: case BattleSkill_LpRecoverySuper:
    case BattleSkill_StatusRecover: case BattleSkill_Revive: case BattleSkill_Death:
    case BattleSkill_Concentration: case BattleSkill_ElementalAmplification: case BattleSkill_Blessing:
    case BattleSkill_DefensiveBarrier: case BattleSkill_VoodooCurse: case BattleSkill_BattleCommand:
    case BattleSkill_BideTime:
    case BattleSkill_SoloDance: case BattleSkill_Adrenaline: case 2050: case 2051: case 2052:
    case 2058: case 2059: case 2060: case 2061: case 2062: case 2063:
    case 2064: case 2065: case 2066: case 2067:
    case 2089: case 2090: case 2091: case 2092: case 2093: case 2094: case 2095:
    case 3002: case 3003: case 3004: case 3005: case 3006: case 3007:
    case 3008: case 3009: case 3010: case 3011: case 3012: case 3013:
    case 3014: case 3015: case 3016:
        return BattlePreviewSkillLayout::ValuesRide;
    case BattleSkill_HealMagic: case BattleSkill_HealMagicStrong: case BattleSkill_HealMagicSuper: case BattleSkill_Sacrifice:
    case BattleSkill_EmergencyTransfusion:
        return BattlePreviewSkillLayout::PairRide;
    case BattleSkill_DefUp: case BattleSkill_DefDown: case BattleSkill_AtkUp: case BattleSkill_AtkDown:
    case BattleSkill_AglUp: case BattleSkill_AglDown:
    case BattleSkill_DetectEnemy: case 109: case BattleSkill_UltimateAttack: case BattleSkill_RecoveryUp: case 117: case 3001:
        return BattlePreviewSkillLayout::ValuesOnly;
    case 108:
        return BattlePreviewSkillLayout::PairOnly;
    case BattleSkill_Consentration: case BattleSkill_TreatTypeEarth: case BattleSkill_TreatTypeWater:
    case BattleSkill_TreatTypeFire: case BattleSkill_TreatTypeWind: case BattleSkill_TreatTypeSilence:
        return BattlePreviewSkillLayout::SourceValue;
    case BattleSkill_Summon:
        return BattlePreviewSkillLayout::NamedTargets;
    case BattleSkill_Dance:
        return BattlePreviewSkillLayout::Mode98;
    case BattleSkill_Copy:
        return BattlePreviewSkillLayout::OptionalPair;
    case BattleSkill_MonWind: case BattleSkill_Acid:
        return BattlePreviewSkillLayout::DirectValues;
    case BattleSkill_Barrier:
        return BattlePreviewSkillLayout::Team;
    case BattleSkill_LvSet:
        return BattlePreviewSkillLayout::DirectRecords;
    case BattleSkill_RiderBreakAttack: case BattleSkill_RiderBreakMind:
        return BattlePreviewSkillLayout::RideRecords;
    case BattleSkill_AttackAll: case 111: case BattleSkill_ThrowItem:
        return BattlePreviewSkillLayout::EffectRecords;
    default:
        return BattlePreviewSkillLayout::None;
    }
}

// 原函数：0x00439070。
std::int32_t GetBattleMHeader_439070(char* const destination)
{
    std::int32_t offset = cg::world::gBattleMScanOffset5A8A3C;
    if (cg::world::gBattleMText5AAA7C[offset] == '\0' || cg::world::gBattleMText5AAA7C[offset + 1] == '\0') {
        return 1;
    }
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(cg::world::gBattleMText5AAA7C[offset]));
        if (value == 0) {
            cg::world::gBattleMScanOffset5A8A3C = offset;
            return 1;
        }
        if (value >= 'A' && value <= 'Z') {
            break;
        }
        ++offset;
    }
    cg::world::gBattleMScanOffset5A8A3C = offset;
    std::strncpy(destination, cg::world::gBattleMText5AAA7C.data() + offset, static_cast<std::uint32_t>(3));
    destination[3] = '\0';
    cg::world::gBattleMScanOffset5A8A3C += 4;
    return 0;
}

// 原函数：0x00438F70。
std::int32_t TakeBattleMInteger_438F70(YobiAction* const action)
{
    auto* const payload = action->payload.battleScene;
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const std::int32_t offset = payload->packetOffset64;
        const auto value = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(world::gBattleMText5AAA7C[offset]));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            const std::int32_t next = offset + 1;
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            payload->packetOffset64 = next;
        } else if (hasDigit || value == '|') {
            ++payload->packetOffset64;
            break;
        } else {
            payload->packetOffset64 = offset + 1;
        }
    }
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x00438FE0。
void TakeBattleMString_438FE0(YobiAction* const action, char* const destination, const std::int32_t capacity)
{
    auto* const payload = action->payload.battleScene;
    std::int32_t written = 0;
    for (;;) {
        const std::int32_t offset = payload->packetOffset64;
        const char byte = world::gBattleMText5AAA7C[offset];
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(byte));
        if (byte == '\0') {
            break;
        }
        if (byte == '|') {
            ++payload->packetOffset64;
            break;
        }
        if (written >= capacity - 1) {
            if (value < 0) {
                payload->packetOffset64 = offset + 1;
            }
            ++payload->packetOffset64;
            continue;
        }
        const bool hasTrail = value < 0;
        destination[written] = byte;
        if (hasTrail) {
            ++written;
            if (written >= capacity - 1) {
                // 原439034..43903C用同一局部游标连续发布两次。
                const std::int32_t next = payload->packetOffset64 + 1;
                payload->packetOffset64 = next;
                payload->packetOffset64 = next + 1;
                continue;
            }
            const std::int32_t next = payload->packetOffset64 + 1;
            payload->packetOffset64 = next;
            destination[written] = world::gBattleMText5AAA7C[next];
        }
        const std::int32_t currentOffset = payload->packetOffset64;
        ++written;
        payload->packetOffset64 = currentOffset + 1;
    }
    destination[written] = '\0';
    input::UnescapeLegacyTextAndNotify_52D2F0(destination);
}

// 原函数：0x00439600。
std::int32_t TakeBattleMPreviewInteger_439600()
{
    std::int32_t offset = cg::world::gBattleMPreviewOffset5AAA60;
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(cg::world::gBattleMText5AAA7C[offset]));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            ++offset;
        } else if (hasDigit || value == '|') {
            cg::world::gBattleMPreviewOffset5AAA60 = offset;
            ++offset;
            break;
        } else {
            ++offset;
        }
    }
    cg::world::gBattleMPreviewOffset5AAA60 = offset;
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x00439670。
void TakeBattleMPreviewString_439670(char* const destination, const std::int32_t capacity)
{
    std::int32_t written = 0;
    for (;;) {
        const char byte = cg::world::gBattleMText5AAA7C[cg::world::gBattleMPreviewOffset5AAA60];
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(byte));
        if (byte == '\0') {
            break;
        }
        if (byte == '|') {
            ++cg::world::gBattleMPreviewOffset5AAA60;
            break;
        }
        if (written >= capacity - 1) {
            if (value < 0) {
                ++cg::world::gBattleMPreviewOffset5AAA60;
            }
            ++cg::world::gBattleMPreviewOffset5AAA60;
            continue;
        }
        const bool hasTrail = value < 0;
        destination[written] = byte;
        if (hasTrail) {
            const std::int32_t offset = cg::world::gBattleMPreviewOffset5AAA60;
            ++written;
            if (written >= capacity - 1) {
                cg::world::gBattleMPreviewOffset5AAA60 = offset + 1;
                ++cg::world::gBattleMPreviewOffset5AAA60;
                continue;
            }
            // 原4396AA先读取第二字节，再发布游标；不同于显式游标版本。
            const char nextByte = cg::world::gBattleMText5AAA7C[offset + 1];
            cg::world::gBattleMPreviewOffset5AAA60 = offset + 1;
            destination[written] = nextByte;
        }
        const std::int32_t currentOffset = cg::world::gBattleMPreviewOffset5AAA60;
        ++written;
        cg::world::gBattleMPreviewOffset5AAA60 = currentOffset + 1;
    }
    destination[written] = '\0';
    input::UnescapeLegacyTextAndNotify_52D2F0(destination);
}

// 原函数：0x00439700。
std::int32_t GetBattleMPreviewHeader_439700(char* const destination)
{
    std::int32_t offset = cg::world::gBattleMPreviewOffset5AAA60;
    if (cg::world::gBattleMText5AAA7C[offset] == '\0' || cg::world::gBattleMText5AAA7C[offset + 1] == '\0') {
        return 1;
    }
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(cg::world::gBattleMText5AAA7C[offset]));
        if (value == 0) {
            cg::world::gBattleMPreviewOffset5AAA60 = offset;
            return 1;
        }
        if (value >= 'A' && value <= 'Z') {
            break;
        }
        ++offset;
    }
    cg::world::gBattleMPreviewOffset5AAA60 = offset;
    std::strncpy(destination, cg::world::gBattleMText5AAA7C.data() + offset, static_cast<std::uint32_t>(3));
    destination[3] = '\0';
    cg::world::gBattleMPreviewOffset5AAA60 += 4;
    return 0;
}

// 原函数：0x0043B940。
std::int32_t TakeBattleMIntegerAt_43B940(std::int32_t* const offset)
{
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const std::int32_t position = *offset;
        const auto value = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(world::gBattleMText5AAA7C[position]));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            const std::int32_t next = position + 1;
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            *offset = next;
        } else if (hasDigit || value == '|') {
            ++*offset;
            break;
        } else {
            *offset = position + 1;
        }
    }
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x0043B9B0。
std::int32_t FindBattleMTarget_43B9B0(
    YobiAction* const scene, const std::int32_t position, std::int32_t skip)
{
    const auto* const payload = static_cast<const BattleScenePayload*>(scene->payload.battleScene);
    std::int32_t offset = payload->packetOffset64;
    while (skip > 0) {
        static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        --skip;
    }
    for (;;) {
        const std::int32_t first = GetBattlePosition_4237E0(TakeBattleMIntegerAt_43B940(&offset));
        if (first == 255) {
            return -1;
        }
        const std::int32_t second = GetBattlePosition_4237E0(TakeBattleMIntegerAt_43B940(&offset));
        const std::uint32_t flags = static_cast<std::uint32_t>(TakeBattleMIntegerAt_43B940(&offset));
        if ((flags & BattleResultMask_CounterOrCrossCounter) == 0) {
            return -1;
        }
        if (first == position) {
            return second;
        }
        static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        if ((flags & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
            for (std::int32_t index = 0; index < 2; ++index) {
                static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
                static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
                static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
            }
        }
    }
}

// 原函数：0x0043B8C0。
void TakeBattleMStringAt_43B8C0(
    std::int32_t* const offset, char* const destination, const std::int32_t capacity)
{
    std::int32_t written = 0;
    for (;;) {
        const std::int32_t position = *offset;
        const char byte = world::gBattleMText5AAA7C[position];
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(byte));
        if (byte == '\0') {
            break;
        }
        if (byte == '|') {
            ++*offset;
            break;
        }
        if (written >= capacity - 1) {
            if (value < 0) {
                *offset = position + 1;
            }
            ++*offset;
            continue;
        }
        const bool hasTrail = value < 0;
        destination[written] = byte;
        if (hasTrail) {
            ++written;
            if (written >= capacity - 1) {
                const std::int32_t next = *offset + 1;
                *offset = next;
                *offset = next + 1;
                continue;
            }
            const std::int32_t next = *offset + 1;
            *offset = next;
            destination[written] = world::gBattleMText5AAA7C[next];
        }
        const std::int32_t currentOffset = *offset;
        ++written;
        *offset = currentOffset + 1;
    }
    destination[written] = '\0';
    input::UnescapeLegacyTextAndNotify_52D2F0(destination);
}

// 原函数：0x00439770。
void ScanBattleMRideParticipants_439770(std::uint8_t* const participants)
{
    using world::gBattleMPreviewOffset5AAA60;
    std::array<char, 4> header;
    std::array<char, 32> text;
    for (;;) {
        const std::int32_t savedOffset = cg::world::gBattleMPreviewOffset5AAA60;
        static_cast<void>(GetBattleMPreviewHeader_439700(header.data()));
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind != BattleMHeader_RID) {
            cg::world::gBattleMPreviewOffset5AAA60 = savedOffset;
            return;
        }
        std::int32_t offset = cg::world::gBattleMPreviewOffset5AAA60;
        if (TakeBattleMIntegerAt_43B940(&offset) != 0) {
            cg::world::gBattleMPreviewOffset5AAA60 = savedOffset;
            return;
        }
        const std::int32_t first = TakeBattleMIntegerAt_43B940(&offset);
        const std::int32_t second = TakeBattleMIntegerAt_43B940(&offset);
        static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        TakeBattleMStringAt_43B8C0(&offset, text.data(), 29);
        static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        for (std::int32_t i = 0; i < 2; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        }
        for (std::int32_t i = 0; i < 5; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        }
        const auto firstFlags = static_cast<std::uint8_t>(TakeBattleMIntegerAt_43B940(&offset));
        TakeBattleMStringAt_43B8C0(&offset, text.data(), 29);
        for (std::int32_t i = 0; i < 6; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&offset));
        }
        const auto secondFlags = static_cast<std::uint8_t>(TakeBattleMIntegerAt_43B940(&offset));
        if ((firstFlags & 2u) == 0 && (secondFlags & 2u) == 0) {
            cg::world::gBattleMPreviewOffset5AAA60 = savedOffset;
            return;
        }
        ++participants[first];
        ++participants[second];
        cg::world::gBattleMPreviewOffset5AAA60 = offset;
    }
}

// 原函数：0x0043BF30。
void TakeBattleMAuxGraphics_43BF30(
    YobiAction* const action, const std::int32_t mode,
    std::int32_t* const firstImage, std::int32_t* const secondImage,
    std::int32_t* const firstImages, std::int32_t* const secondImages, std::int32_t* const values)
{
    if (mode == BattleMAuxGraphicsReadMode_SkipPreviewFields) {
        static_cast<void>(TakeBattleMPreviewInteger_439600());
        static_cast<void>(TakeBattleMPreviewInteger_439600());
        for (std::int32_t index = 0; index < 2; ++index) {
            static_cast<void>(TakeBattleMPreviewInteger_439600());
            static_cast<void>(TakeBattleMPreviewInteger_439600());
            static_cast<void>(TakeBattleMPreviewInteger_439600());
        }
    } else if (mode == BattleMAuxGraphicsReadMode_ReadObjectFields) {
        *firstImage = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(action));
        *secondImage = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(action));
        for (std::int32_t index = 0; index < 2; ++index) {
            firstImages[index] = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(action));
            secondImages[index] = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(action));
            values[index] = TakeBattleMInteger_438F70(action);
        }
    }
}

// 原函数：0x00439920。
std::int32_t ScanBattleMAttackParticipants_439920(
    const std::int32_t skill, std::uint8_t* const participants)
{
    const std::int32_t first = GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
    if (first == 255) {
        return first;
    }
    ++participants[first];
    ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
    std::int32_t flags = TakeBattleMPreviewInteger_439600();
    static_cast<void>(TakeBattleMPreviewInteger_439600());
    std::int32_t currentSkill = skill;
    if (currentSkill == BattleSkill_BloodAttack) {
        static_cast<void>(TakeBattleMPreviewInteger_439600());
    }
    if ((flags & static_cast<std::int32_t>(BattleResultFlag_HasAuxiliaryGraphics)) != 0) {
        TakeBattleMAuxGraphics_43BF30(nullptr, BattleMAuxGraphicsReadMode_SkipPreviewFields, nullptr, nullptr, nullptr, nullptr, nullptr);
    }
    for (;;) {
        std::int32_t extraPosition = -1;
        std::int32_t extraFlags = 0;
        if (currentSkill == BattleSkill_Wattack && (flags & static_cast<std::int32_t>(BattleResultFlag_Success)) != 0) {
            extraPosition = GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
            if (extraPosition != 255) {
                ++participants[extraPosition];
                extraFlags = TakeBattleMPreviewInteger_439600();
                static_cast<void>(TakeBattleMPreviewInteger_439600());
                if ((extraFlags & static_cast<std::int32_t>(BattleResultFlag_HasAuxiliaryGraphics)) != 0) {
                    TakeBattleMAuxGraphics_43BF30(nullptr, BattleMAuxGraphicsReadMode_SkipPreviewFields, nullptr, nullptr, nullptr, nullptr, nullptr);
                }
            }
        }
        if ((flags & static_cast<std::int32_t>(BattleResultFlag_Bodyguard)) != 0) {
            ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
        }
        if (currentSkill == BattleSkill_Wattack) {
            if ((flags & static_cast<std::int32_t>(BattleResultFlag_Success)) != 0 && (extraFlags & static_cast<std::int32_t>(BattleResultFlag_Bodyguard)) != 0) {
                ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
            }
        } else if (currentSkill == BattleSkill_PanicAttack) {
            static_cast<void>(TakeBattleMPreviewInteger_439600());
        } else if ((currentSkill == BattleSkill_Steal || currentSkill == BattleSkill_ShadowHand) && (flags & static_cast<std::int32_t>(BattleResultFlag_Normal)) != 0) {
            extraPosition = GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
            ++participants[extraPosition];
            const std::int32_t extraAttackFlags = TakeBattleMPreviewInteger_439600();
            static_cast<void>(TakeBattleMPreviewInteger_439600());
            if ((extraAttackFlags & static_cast<std::int32_t>(BattleResultFlag_HasAuxiliaryGraphics)) != 0) {
                TakeBattleMAuxGraphics_43BF30(nullptr, BattleMAuxGraphicsReadMode_SkipPreviewFields, nullptr, nullptr, nullptr, nullptr, nullptr);
            }
            if ((extraAttackFlags & static_cast<std::int32_t>(BattleResultFlag_Bodyguard)) != 0) {
                ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
            }
        }
        if (extraPosition == 255) {
            return first;
        }
        const std::int32_t next = GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
        if (next == 255) {
            return first;
        }
        ++participants[next];
        ++participants[GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600())];
        flags = TakeBattleMPreviewInteger_439600();
        static_cast<void>(TakeBattleMPreviewInteger_439600());
        if ((flags & static_cast<std::int32_t>(BattleResultFlag_HasAuxiliaryGraphics)) != 0) {
            TakeBattleMAuxGraphics_43BF30(nullptr, BattleMAuxGraphicsReadMode_SkipPreviewFields, nullptr, nullptr, nullptr, nullptr, nullptr);
        }
        currentSkill = -1;
    }
}

// 原函数：0x00439B30。
void ScanBattleMParticipants_439B30(
    const std::int32_t kind, std::uint8_t* const participants,
    std::int32_t* const source, std::int32_t* const flag)
{
    std::int32_t position = -1;
    *flag = 0;
    // 原439B55..439B63：清零22槽参与计数。
    std::memset(participants, 0, 22);
    std::array<char, 32> text;
    std::array<char, 256> message;
    const auto skip = [](const std::int32_t count) {
        for (std::int32_t i = 0; i < count; ++i) {
            static_cast<void>(TakeBattleMPreviewInteger_439600());
        }
    };
    const auto readPosition = [] {
        return GetBattlePosition_4237E0(TakeBattleMPreviewInteger_439600());
    };
    const auto readSource = [&] {
        position = readPosition();
        ++participants[position];
    };
    const auto scanTargets = [&](const std::int32_t values, const bool auxiliaryGraphics) {
        for (;;) {
            const std::int32_t target = readPosition();
            if (target == 255) {
                return;
            }
            ++participants[target];
            if (values != 0) {
                const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
                skip(values - 1);
                if (auxiliaryGraphics && (targetFlags & static_cast<std::int32_t>(BattleResultFlag_HasAuxiliaryGraphics)) != 0) {
                    TakeBattleMAuxGraphics_43BF30(nullptr, BattleMAuxGraphicsReadMode_SkipPreviewFields, nullptr, nullptr, nullptr, nullptr, nullptr);
                }
            }
        }
    };
    switch (kind) {
    case BattleMHeader_SKL: { // SKL
        const std::int32_t technique = TakeBattleMPreviewInteger_439600();
        skip(1);
        std::int32_t skill = TakeBattleMPreviewInteger_439600();
        skip(2);
        if (technique < 205000) {
            skill = technique / 100;
        }
        const BattlePreviewSkillLayout layout = GetBattlePreviewSkillLayout(skill);
        bool followRide = false;
        switch (layout) {
        case BattlePreviewSkillLayout::Attack:
        case BattlePreviewSkillLayout::Mode73: {
            const std::int32_t mode = TakeBattleMPreviewInteger_439600();
            if (mode == BattleAttackMode_Melee || skill == BattleSkill_Steal || skill == BattleSkill_ShadowHand || skill == 113) {
                position = ScanBattleMAttackParticipants_439920(skill, participants);
            } else if (mode > BattleAttackMode_Melee && mode <= BattleAttackMode_Bow) {
                readSource();
                for (;;) {
                    const std::int32_t target = readPosition();
                    if (target == 255) {
                        break;
                    }
                    std::uint8_t targetCount = participants[target];
                    ++targetCount;
                    const bool sourceTarget = layout == BattlePreviewSkillLayout::Attack
                        && target == position;
                    participants[target] = targetCount;
                    const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
                    skip(1);
                    if ((targetFlags & static_cast<std::int32_t>(BattleResultFlag_HasAuxiliaryGraphics)) != 0) {
                        TakeBattleMAuxGraphics_43BF30(nullptr, BattleMAuxGraphicsReadMode_SkipPreviewFields, nullptr, nullptr, nullptr, nullptr, nullptr);
                    }
                    if (sourceTarget && (targetFlags & static_cast<std::int32_t>(BattleResultFlag_Bodyguard)) != 0) {
                        ++participants[readPosition()];
                    }
                }
            }
            followRide = true;
            break;
        }
        case BattlePreviewSkillLayout::Damage:
        case BattlePreviewSkillLayout::ValuesRide:
        case BattlePreviewSkillLayout::PairRide:
        case BattlePreviewSkillLayout::ValuesOnly:
        case BattlePreviewSkillLayout::PairOnly:
            skip(1);
            readSource();
            if (layout == BattlePreviewSkillLayout::Damage) {
                scanTargets(2, true);
            } else if (layout == BattlePreviewSkillLayout::PairRide
                || layout == BattlePreviewSkillLayout::PairOnly || skill == BattleSkill_Revive) {
                scanTargets(2, false);
            } else {
                scanTargets(1, false);
            }
            followRide = layout == BattlePreviewSkillLayout::Damage
                || layout == BattlePreviewSkillLayout::ValuesRide
                || layout == BattlePreviewSkillLayout::PairRide;
            break;
        case BattlePreviewSkillLayout::SourceValue:
            skip(1);
            readSource();
            skip(1);
            break;
        case BattlePreviewSkillLayout::NamedTargets:
            skip(1);
            readSource();
            for (;;) {
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                ++participants[target];
                TakeBattleMPreviewString_439670(text.data(), 29);
                skip(7);
            }
            break;
        case BattlePreviewSkillLayout::Mode98: {
            skip(1);
            const std::int32_t mode = TakeBattleMPreviewInteger_439600();
            readSource();
            followRide = true;
            switch (mode) {
            case BattleDanceMode_Poison: case BattleDanceMode_Sleep: case BattleDanceMode_Stone: case BattleDanceMode_Inebriety: case BattleDanceMode_Confusion: case BattleDanceMode_Forget:
            case BattleDanceMode_InstantDeath: case BattleDanceMode_RecoverDebuff: case BattleDanceMode_Revive: case BattleDanceMode_ReadTargetFlags:
                scanTargets(mode == BattleDanceMode_Revive ? 2 : 1, false);
                break;
            case BattleDanceMode_ApplyDamageUpAndAttackAnimation: case BattleDanceMode_ShrinkAndRemoveTargets:
                scanTargets(0, false);
                break;
            case BattleDanceMode_SwapActorAndPetPositions:
                for (;;) {
                    const std::int32_t target = readPosition();
                    if (target == 255) {
                        break;
                    }
                    ++participants[target];
                    ++participants[cg::world::GetBattlePetPosition_405B40(target)];
                }
                break;
            case BattleDanceMode_SelectedActorSpinAttackAndExit: case BattleDanceMode_SelectedActorHealAndExit:
                position = readPosition();
                if (position == 255) {
                    followRide = false;
                    break;
                }
                ++participants[position];
                scanTargets(2, mode == BattleDanceMode_SelectedActorSpinAttackAndExit);
                break;
            case BattleDanceMode_SetHpOrFpPairs:
                for (;;) {
                    skip(1);
                    position = TakeBattleMPreviewInteger_439600();
                    if (position == 255) {
                        break;
                    }
                    ++participants[position];
                    skip(1);
                    const std::int32_t target = readPosition();
                    if (target == 255) {
                        break;
                    }
                    ++participants[target];
                }
                break;
            case BattleDanceMode_DecreaseHpTargets: case BattleDanceMode_DecreaseFpTargets:
                scanTargets(2, false);
                break;
            case BattleDanceMode_SetHpFpOne:
                scanTargets(1, false);
                break;
            default:
                break;
            }
            break;
        }
        case BattlePreviewSkillLayout::OptionalPair: {
            readSource();
            const std::int32_t target = readPosition();
            skip(2);
            if (target != -1) {
                ++participants[target];
            }
            break;
        }
        case BattlePreviewSkillLayout::DirectValues:
            readSource();
            scanTargets(1, false);
            break;
        case BattlePreviewSkillLayout::Team: {
            skip(1);
            readSource();
            const std::int32_t first = TakeBattleMPreviewInteger_439600() == 0 ? 0 : 10;
            for (std::int32_t index = first; index < first + 10; ++index) {
                ++participants[index];
            }
            break;
        }
        case BattlePreviewSkillLayout::DirectRecords:
            readSource();
            for (;;) {
                skip(1);
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                ++participants[target];
                skip(7);
            }
            break;
        case BattlePreviewSkillLayout::RideRecords:
            skip(1);
            readSource();
            for (;;) {
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                if (target == position) {
                    ++participants[readPosition()];
                    skip(1);
                    TakeBattleMPreviewString_439670(text.data(), 29);
                    skip(8);
                    TakeBattleMPreviewString_439670(text.data(), 29);
                    skip(7);
                } else {
                    ++participants[target];
                    const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
                    skip(1);
                    if ((targetFlags & static_cast<std::int32_t>(BattleResultFlag_HasAuxiliaryGraphics)) != 0) {
                        TakeBattleMAuxGraphics_43BF30(nullptr, BattleMAuxGraphicsReadMode_SkipPreviewFields, nullptr, nullptr, nullptr, nullptr, nullptr);
                    }
                }
            }
            followRide = true;
            break;
        case BattlePreviewSkillLayout::EffectRecords:
            skip(1);
            readSource();
            for (;;) {
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                ++participants[target];
                const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
                skip(1);
                auto& pair = cg::world::gBattleEffectPairs5A8A40[target];
                const bool namedEffect = skill == BattleSkill_ThrowItem;
                pair[0] = 0;
                pair[1] = 0;
                if (namedEffect) {
                    pair[0] = TakeBattleMPreviewInteger_439600();
                    pair[1] = TakeBattleMPreviewInteger_439600();
                    TakeBattleMPreviewString_439670(text.data(), 29);
                    const std::int32_t value = TakeBattleMPreviewInteger_439600();
                    for (auto& record : cg::world::gBattleNamedEffects5ABCE0) {
                        if (record.name.empty()) {
                            record.name = cg::text::FromGbkWide(text.data());
                            record.value = value;
                            break;
                        }
                        if (cg::text::FromGbkWide(text.data()) == record.name) {
                            record.value = value;
                            break;
                        }
                    }
                }
                if ((targetFlags & static_cast<std::int32_t>(BattleResultFlag_HasAuxiliaryGraphics)) != 0) {
                    TakeBattleMAuxGraphics_43BF30(nullptr, BattleMAuxGraphicsReadMode_SkipPreviewFields, nullptr, nullptr, nullptr, nullptr, nullptr);
                }
            }
            followRide = true;
            break;
        case BattlePreviewSkillLayout::None:
            break;
        }
        if (followRide) {
            ScanBattleMRideParticipants_439770(participants);
        }
        *flag = 1;
        *source = position;
        return;
    }
    case BattleMHeader_ESC: case BattleMHeader_BST: // ESC/BST
        readSource();
        skip(1);
        *flag = 1;
        break;
    case BattleMHeader_CLR: case BattleMHeader_SUP: // CLR/SUP
        for (;;) {
            position = readPosition();
            if (position == 255) {
                break;
            }
            ++participants[position];
        }
        *flag = 1;
        break;
    case BattleMHeader_POS: { // POS
        position = readPosition();
        const std::int32_t pet = cg::world::GetBattlePetPosition_405B40(position);
        ++participants[position];
        ++participants[pet];
        *flag = 1;
        break;
    }
    case BattleMHeader_MON: // MON
        readSource();
        ++participants[cg::world::GetBattlePetPosition_405B40(position)];
        if (TakeBattleMPreviewInteger_439600() != 0) {
            skip(1);
            TakeBattleMPreviewString_439670(text.data(), 29);
            skip(6);
        }
        skip(1);
        *flag = 1;
        break;
    case BattleMHeader_EQU: // EQU
        readSource();
        skip(2);
        *flag = 1;
        break;
    case BattleMHeader_ITM: { // ITM
        const std::int32_t mode = TakeBattleMPreviewInteger_439600();
        if (mode == BattleMItemMode_RecoverTargetHp) {
            readSource();
            for (;;) {
                skip(1);
                const std::int32_t target = readPosition();
                if (target == 255) {
                    break;
                }
                ++participants[target];
            }
        } else if (mode == BattleMItemMode_SealCard) {
            readSource();
            ++participants[readPosition()];
            skip(1);
        } else if (mode == BattleMItemMode_PlayNodAndWait) {
            readSource();
        }
        *flag = 1;
        break;
    }
    case BattleMHeader_CMB: // CMB，输出循环终止位置FF，不是首参与者。
        ++participants[readPosition()];
        scanTargets(2, true);
        ScanBattleMRideParticipants_439770(participants);
        position = 255;
        *flag = 1;
        break;
    case BattleMHeader_PSN: // PSN
        skip(2);
        readSource();
        skip(2);
        ScanBattleMRideParticipants_439770(participants);
        *flag = 1;
        break;
    case BattleMHeader_ANR: case BattleMHeader_ACT: // ANR/ACT
        readSource();
        *flag = 0;
        break;
    case BattleMHeader_PUE: // PUE
        readSource();
        skip(2);
        *flag = 0;
        break;
    case BattleMHeader_CFP: // CFP
        readSource();
        skip(1);
        *flag = 0;
        break;
    case BattleMHeader_CHT: // CHT
        position = readPosition();
        if (static_cast<std::uint32_t>(position) <= 21u) {
            ++participants[position];
        }
        skip(3);
        TakeBattleMPreviewString_439670(message.data(), 256);
        *flag = 0;
        break;
    case BattleMHeader_PNO: case BattleMHeader_PLV: case BattleMHeader_PDN: // PNO/PLV/PDN
        readSource();
        *flag = 1;
        break;
    case BattleMHeader_CBC: // CBC
        readSource();
        skip(7);
        *flag = 0;
        break;
    case BattleMHeader_REB: { // REB
        readSource();
        const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
        skip(1);
        if ((targetFlags & static_cast<std::int32_t>(BattleResultFlag_Success)) != 0) {
            skip(1);
        }
        *flag = 1;
        break;
    }
    case BattleMHeader_GMN: // GMN，首位置只参与计数，gate0仍输出-1。
        ++participants[readPosition()];
        if (TakeBattleMPreviewInteger_439600() != BattleMGmnMode_RemoveActorAndRevealTeam) {
            position = readPosition();
            skip(2);
            TakeBattleMPreviewString_439670(text.data(), 29);
            skip(7);
        }
        for (std::int32_t index = 0; index < 22; ++index) {
            ++participants[index];
        }
        *flag = 1;
        break;
    case BattleMHeader_RID: { // RID，此处零模式19整数，与439770试读20整数不同。
        const std::int32_t mode = TakeBattleMPreviewInteger_439600();
        position = readPosition();
        const std::int32_t target = readPosition();
        ++participants[position];
        ++participants[target];
        const std::int32_t targetFlags = TakeBattleMPreviewInteger_439600();
        if (mode != 0) {
            if ((targetFlags & static_cast<std::int32_t>(BattleResultFlag_Success)) != 0) {
                TakeBattleMPreviewString_439670(text.data(), 29);
                skip(8);
            }
        } else {
            TakeBattleMPreviewString_439670(text.data(), 29);
            skip(8);
            TakeBattleMPreviewString_439670(text.data(), 29);
            skip(7);
        }
        *flag = 1;
        break;
    }
    case BattleMHeader_DSK: // DSK
        skip(3);
        [[fallthrough]];
    case BattleMHeader_END: // END
        for (std::int32_t index = 0; index < 22; ++index) {
            ++participants[index];
        }
        *flag = 0;
        break;
    default:
        break;
    }
    *source = position;
}

// 原函数：0x0043B290。
std::int32_t CheckBattleMParticipants_43B290(
    std::int32_t* const source, const std::int32_t previousSource,
    std::uint32_t* const mask, std::int32_t* const flag)
{
    cg::world::gBattleMPreviewOffset5AAA60 = cg::world::gBattleMScanOffset5A8A3C;
    std::array<char, 4> header;
    std::array<std::uint8_t, 22> participants;
    if (GetBattleMPreviewHeader_439700(header.data()) == 1) {
        return 1;
    }
    std::int32_t kind = 0;
    while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
        && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
        ++kind;
    }
    if (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())) {
        ScanBattleMParticipants_439B30(static_cast<std::int32_t>(kind), participants.data(), source, flag);
        for (std::int32_t index = 0; index < 22; ++index) {
            const std::uint8_t count = participants[index];
            if (count == 0) {
                continue;
            }
            if (!(previousSource == *source && index == previousSource)
                && cg::world::gBattlePreviewParticipants5AAA48[index] != 0) {
                return 0;
            }
            cg::world::gBattlePreviewParticipants5AAA48[index] = count;
        }
    }
    // 原未知头路径不初始化局部计数；不补零或新增拒绝分支。
    for (std::int32_t index = 0; index < 22; ++index) {
        if (participants[index] != 0) {
            const std::uint32_t currentMask = *mask;
            const std::uint32_t bit = 1u << index;
            *mask = currentMask | bit;
        }
    }
    return 1;
}

// 原函数：0x0043B860。
std::int32_t PeekBattleMInteger_43B860(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleScenePayload*>(action->payload.battleScene);
    const char* cursor = world::gBattleMText5AAA7C.data() + payload->packetOffset64;
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*cursor));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            ++cursor;
        } else if (hasDigit || value == '|') {
            break;
        } else {
            ++cursor;
        }
    }
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x00423960。
std::int32_t GetBattleMObjectHeader_423960(YobiAction* const action, char* const destination)
{
    using world::gBattleMText5AAA7C;
    auto* const payload = action->payload.battleScene;
    const std::int32_t initialOffset = payload->packetOffset64;
    if (cg::world::gBattleMText5AAA7C[initialOffset] == '\0' || cg::world::gBattleMText5AAA7C[initialOffset + 1] == '\0') {
        return 1;
    }
    for (;;) {
        const std::int32_t offset = payload->packetOffset64;
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(cg::world::gBattleMText5AAA7C[offset]));
        if (value == 0) {
            return 1;
        }
        if (value >= 'A' && value <= 'Z') {
            break;
        }
        payload->packetOffset64 = offset + 1;
    }
    std::strncpy(destination, cg::world::gBattleMText5AAA7C.data() + payload->packetOffset64, static_cast<std::uint32_t>(3));
    destination[3] = '\0';
    payload->packetOffset64 += 4;
    return 0;
}

// 427750..42780B；两局部量在98之后、A4之前读取，不发布为另一组全局。
static void RestoreBattleMExecutionState(
    const BattleScenePayload* const payload, std::uint32_t& flags9C, std::int32_t& feedbackAlternateImageA0)
{
    gBattleMCasterPosition5AAA64 = payload->casterPosition6C;
    gBattleMValue5ABA7C = payload->value70;
    gBattleMDefenderPosition5A4A24 = payload->defenderPosition74;
    gBattleMTargetPosition5AAA40 = payload->targetPosition78;
    gBattleMSkill5AAA78 = payload->skill7C;
    gBattleMTechnique5A4A38 = payload->technique80;
    gBattleMItemMode59FA10 = payload->itemMode84;
    gBattleMSkill98Mode5A4A34 = payload->skill98Mode88;
    gBattleMFeedbackSourcePosition5ABA90 = payload->feedbackSourcePosition8C;
    gBattleMResultFlags5A4A20 = payload->resultFlags90;
    gBattleMFlags5ABA84 = payload->flags94;
    gBattleMFeedbackEffectImage5A4A28 = payload->feedbackEffectImage98;
    flags9C = payload->flags9C;
    feedbackAlternateImageA0 = payload->feedbackAlternateImageA0;
    gBattleMMessageValue59FA14 = payload->messageValueA4;
}

// 437A29..437ABA；即使消息已推进或对象已标死，活动路径仍回存13项，不写9C/A0。
static void SaveBattleMExecutionState(BattleScenePayload* const payload)
{
    payload->casterPosition6C = gBattleMCasterPosition5AAA64;
    payload->value70 = gBattleMValue5ABA7C;
    payload->defenderPosition74 = gBattleMDefenderPosition5A4A24;
    payload->targetPosition78 = gBattleMTargetPosition5AAA40;
    payload->skill7C = gBattleMSkill5AAA78;
    payload->technique80 = gBattleMTechnique5A4A38;
    payload->itemMode84 = gBattleMItemMode59FA10;
    payload->skill98Mode88 = gBattleMSkill98Mode5A4A34;
    payload->feedbackSourcePosition8C = gBattleMFeedbackSourcePosition5ABA90;
    payload->resultFlags90 = gBattleMResultFlags5A4A20;
    payload->flags94 = gBattleMFlags5ABA84;
    payload->feedbackEffectImage98 = gBattleMFeedbackEffectImage5A4A28;
    payload->messageValueA4 = gBattleMMessageValue59FA14;
}

// 427500..427732失活位路径；由427470入口的PrepareBattleMExecution调用，不单独绑定回调。
static void DrainInactiveBattleMEvent(YobiAction* const scene, BattleScenePayload* const payload)
{
    std::array<char, 256> initialMessage;
    std::array<char, 256> queuedMessage;
    std::array<char, 4> header;
    if (scene->phase == BattleMHeader_CHT && payload->packetSubState58 == 0) {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const std::int32_t color = TakeBattleMInteger_438F70(scene);
        const std::int32_t font = TakeBattleMInteger_438F70(scene);
        payload->messageValueA4 = TakeBattleMInteger_438F70(scene);
        TakeBattleMString_438FE0(scene, initialMessage.data(), 256);
        if (platform::gChatNotificationSoundEnabled_554644 != 0) {
            static_cast<void>(audio::PlaySe_52C450(74, 320));
        }
        cg::world::AppendChatMessage_44B050(cg::text::FromGbkWide(initialMessage.data()), color, font);
    }
    if (scene->phase == BattleMHeader_BST && payload->packetSubState58 == 0) {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const std::int32_t seconds = TakeBattleMInteger_438F70(scene);
        if (gBattleMCasterPosition5AAA64 == cg::world::gBattleCurrentPosition5ABCA4) {
            cg::world::ConsumeBattlePlayerTimer(seconds);
        }
    }
    --payload->remainingMessages5C;
    while (payload->remainingMessages5C > 0 && GetBattleMObjectHeader_423960(scene, header.data()) == 0) {
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind == static_cast<std::int32_t>(kBattleMHeaders553E94.size())) {
            continue;
        }
        const std::int32_t remaining = payload->remainingMessages5C - 1;
        const bool isChat = kind == BattleMHeader_CHT;
        payload->remainingMessages5C = remaining;
        if (isChat) {
            gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            const std::int32_t color = TakeBattleMInteger_438F70(scene);
            const std::int32_t font = TakeBattleMInteger_438F70(scene);
            payload->messageValueA4 = TakeBattleMInteger_438F70(scene);
            TakeBattleMString_438FE0(scene, queuedMessage.data(), 256);
            if (platform::gChatNotificationSoundEnabled_554644 != 0) {
                static_cast<void>(audio::PlaySe_52C450(74, 320));
            }
            cg::world::AppendChatMessage_44B050(cg::text::FromGbkWide(queuedMessage.data()), color, font);
        } else if (kind == BattleMHeader_BST) {
            gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            const std::int32_t seconds = TakeBattleMInteger_438F70(scene);
            if (gBattleMCasterPosition5AAA64 == cg::world::gBattleCurrentPosition5ABCA4) {
                cg::world::ConsumeBattlePlayerTimer(seconds);
            }
        }
    }
    FinishBattleEvent_43B6C0(scene);
}

// 4274DF..427500/427732..42780B；false时父入口直接返回，不执行活动路径的公共回存。
static bool PrepareBattleMExecution(
    YobiAction* const scene, BattleScenePayload* const payload,
    std::uint32_t& flags9C, std::int32_t& feedbackAlternateImageA0)
{
    const std::uint32_t eventBit = 1u << payload->eventSlot60;
    if ((world::gBattleActiveEventMask5AAA74 & eventBit) == 0) {
        DrainInactiveBattleMEvent(scene, payload);
        return false;
    }
    const std::int32_t delay = payload->groupBase68;
    if (delay >= 0) {
        const std::int32_t nextDelay = delay - 1;
        const bool groupDelayFinished = nextDelay == 0;
        payload->groupBase68 = nextDelay;
        if (groupDelayFinished) {
            --gBattlePendingGroupCount5ABB24;
        }
        return false;
    }
    RestoreBattleMExecutionState(payload, flags9C, feedbackAlternateImageA0);
    return true;
}

// 427470的六类完整消息段；入口载荷固定，返回后仍须由总入口执行437A29回存。
static void ProcessBattleMTransitionHeader(
    YobiAction* const scene, BattleScenePayload* const payload, const BattleMTransitionHeader kind)
{
    const std::int32_t subState = payload->packetSubState58;
    if (subState != BattleMQueuedPhase_ReadAndQueue) {
        if (subState == BattleMQueuedPhase_WaitForTurnReady
            && CheckBattleTurnReady_4394A0(scene, kind == BattleMTransitionHeader::Clear ? BattleTurnReadyPolicy_CheckAllSelectedEntries : BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            if (kind == BattleMTransitionHeader::PetNotice || kind == BattleMTransitionHeader::PetLeave) {
                FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            }
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }

    switch (kind) {
    case BattleMTransitionHeader::Escape: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        gBattleMResultFlags5A4A20 = resultFlags;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[casterPosition], BattleAction_ShowReactionIcon);
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22320;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PrepareEscape);
        const std::uint32_t result = gBattleMResultFlags5A4A20;
        if (result == 0) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayDeathAnimation);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_Wait60Ticks);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_FaceFormation);
        } else if (result == 1 || result == 2) {
            if (result == 2) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_HandleDeathAndContinue);
            }
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->soundId1018 = 211;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_DismissPlayerPet);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_MoveToExitPoint);
            const std::int32_t currentPosition = gBattleMCasterPosition5AAA64;
            const bool playerPosition = cg::world::gBattleCurrentPosition5ABCA4 == currentPosition;
            const std::int32_t castState = playerPosition ? BattleAction_RequestBattleExit : BattleAction_RemoveFromBattle;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[currentPosition], castState);
        }
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    }
    case BattleMTransitionHeader::Clear:
    case BattleMTransitionHeader::Support:
        for (;;) {
            const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            const bool endPosition = position == 255;
            gBattleMCasterPosition5AAA64 = position;
            if (endPosition) {
                break;
            }
            YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
            if (actor == nullptr) {
                continue;
            }
            AppendBattleCast_4392A0(scene, actor, BattleAction_ShowReactionIcon);
            if (kind == BattleMTransitionHeader::Clear) {
                cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->reactionImage1010 = 22320;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PrepareEscape);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_MoveToExitPoint);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_RemoveFromBattle);
            } else {
                cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->reactionImage1010 = 22323;
                cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->soundId1018 = 206;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
                YobiAction* const current = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
                if ((*reinterpret_cast<const unsigned char*>(
                        &current->payload.battleActor->flagsFDC) & BattleActorFlag_Dead) == 0) {
                    AppendBattleCast_4392A0(scene, current, BattleAction_PlayJoyAnimationAndResetFrames);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_SetIdlePose);
                }
            }
            MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        }
        break;
    case BattleMTransitionHeader::PetDown: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        actor->payload.battleActor->soundId1018 = 205;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ClearDamageUp);
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    }
    case BattleMTransitionHeader::PetNotice: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        actor->payload.battleActor->reactionImage1010 = 22334;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ShowReactionIcon);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_OscillateFacing);
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    }
    case BattleMTransitionHeader::PetLeave: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        static_cast<void>(platform::SeedRandom_53B4BF(static_cast<std::uint32_t>(position)));
        const std::int32_t xIndex = platform::RandomRange_46FAF0(0, 19);
        // 原X先借角色、读坐标，再借载荷；Y先借角色及载荷，再读坐标。
        const std::int32_t xPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const xActor = cg::world::gBattleAllEntries59E980[xPosition];
        const std::int32_t x = cg::world::gBattleGraphicPoints553758[xIndex][0];
        xActor->payload.battleActor->x08 = x;
        const std::int32_t yIndex = platform::RandomRange_46FAF0(0, 19);
        const std::int32_t yPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const yActor = cg::world::gBattleAllEntries59E980[yPosition];
        auto* const yPayload = yActor->payload.battleActor;
        const std::int32_t y = cg::world::gBattleGraphicPoints553758[yIndex][1];
        yPayload->y0C = y;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PrepareEscape);
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22324;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ShowReactionIcon);
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->soundId1018 = 211;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_MoveToExitPoint);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_RemoveFromBattle);
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        break;
    }
    }
    ++payload->packetSubState58;
}

// 六类活动数据消息；由427470总入口分派，父入口负责恢复及回存，不单独绑定回调。
static void ProcessBattleMDataHeader(
    YobiAction* const scene, BattleScenePayload* const payload, const BattleMDataHeader kind)
{
    switch (kind) {
    case BattleMDataHeader::Action: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor != nullptr) {
            static_cast<void>(NewBattleAttachedEffect_41FBE0(actor, BattleAttachedEffectMode_DisplayImage22327));
        }
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    case BattleMDataHeader::ConsumeFp: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (actorPresent) {
            const std::int32_t amount = TakeBattleMInteger_438F70(scene);
            (cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barValue1B0 -= static_cast<std::int32_t>(amount));
            if ((cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barValue1B0 < 0)) {
                cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barValue1B0 = std::int32_t{0};
            }
        }
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    case BattleMDataHeader::Chat: {
        const std::int32_t subState = payload->packetSubState58;
        if (subState == BattleMChatPhase_ReadAndShow) {
            gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
            const std::int32_t color = TakeBattleMInteger_438F70(scene);
            const std::int32_t font = TakeBattleMInteger_438F70(scene);
            const std::int32_t duration = TakeBattleMInteger_438F70(scene);
            payload->messageValueA4 = duration;
            gBattleMMessageValue59FA14 = duration;
            std::array<char, 256> message;
            TakeBattleMString_438FE0(scene, message.data(), 256);
            if (platform::gChatNotificationSoundEnabled_554644 != 0) {
                static_cast<void>(audio::PlaySe_52C450(74, 320));
            }
            cg::world::AppendChatMessage_44B050(cg::text::FromGbkWide(message.data()), color, font);
            const std::int32_t position = gBattleMCasterPosition5AAA64;
            if (position < 20) {
                YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
                if (actor != nullptr) {
                    static_cast<void>(render::LoadGraphic_522570(actor->graphicIndex));
                    const std::int32_t currentPosition = gBattleMCasterPosition5AAA64;
                    const std::int32_t lifetime = gBattleMMessageValue59FA14;
                    YobiAction* const current = cg::world::gBattleAllEntries59E980[currentPosition];
                    const std::int32_t offsetY = -render::ReadGraphicDxHeightFromField(current->graphicIndex);
                    static_cast<void>(NewBattleHitEffect_41F5B0(current, -30, offsetY, lifetime, 22335));
                }
            }
            ++payload->packetSubState58;
        } else if (subState == BattleMChatPhase_CountDown) {
            const std::int32_t remaining = payload->messageValueA4 - 1;
            payload->messageValueA4 = remaining;
            gBattleMMessageValue59FA14 = remaining;
            if (payload->messageValueA4 <= 0) {
                FinishBattleEvent_43B6C0(scene);
            }
        }
        return;
    }
    case BattleMDataHeader::ChangeBody: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::int32_t image = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->imageNo = image;
        {
            const std::int32_t value = TakeBattleMInteger_438F70(scene);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->readiness = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(scene);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->maxHp = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(scene);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barValue1B0 = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(scene);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->barMaximum1C0 = static_cast<std::int32_t>(value);
        }
        const std::int32_t riderValue = TakeBattleMInteger_438F70(scene);
        for (std::int32_t index = 0; index < 2; ++index) {
            const std::int32_t gear = TakeBattleMInteger_438F70(scene);
            const std::int32_t firstImage = TakeBattleMInteger_438F70(scene);
            const std::int32_t secondImage = TakeBattleMInteger_438F70(scene);
            SetBattleGear_422C20(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], gear, index);
            SetBattleAuxGear_422C90(
                cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], firstImage, secondImage, index);
        }
        YobiAction* const current = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (current->payload.battleActor->rider13C8 != nullptr && riderValue != 0) {
            static_cast<void>(SetBattleRider_4222B0(current, current->imageNo, riderValue));
        }
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    case BattleMDataHeader::AppendSkill: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t technique = TakeBattleMInteger_438F70(scene);
        const std::uint32_t flags = gBattleMResultFlags5A4A20;
        const bool appendEight = flags == BattleAppendSkillSelector_SpecialGuard;
        gBattleMTechnique5A4A38 = technique;
        if (appendEight) {
            AddBattleAppendEffect_43BE00(BattleSkill_SpecialGuard, technique, gBattleMCasterPosition5AAA64);
        } else if (flags == BattleAppendSkillSelector_CrossCounter) {
            AddBattleAppendEffect_43BE00(BattleSkill_CrossCounter, technique, gBattleMCasterPosition5AAA64);
        }
        if (CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }
    case BattleMDataHeader::End:
        if (CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            FinishBattleEvent_43B6C0(scene);
            world::gCurrentSceneState = 7;
        }
        return;
    }
}

// 六类装备/状态消息；固定载荷与当前槽分别取样，已由427470调用并回存，完整战斗另验。
static void ProcessBattleMStatusHeader(
    YobiAction* const scene, BattleScenePayload* const payload, const BattleMStatusHeader kind)
{
    const std::int32_t subState = payload->packetSubState58;
    if (subState != BattleMQueuedPhase_ReadAndQueue) {
        if ((kind == BattleMStatusHeader::QueuedValue || subState == BattleMQueuedPhase_WaitForTurnReady)
            && CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }

    switch (kind) {
    case BattleMStatusHeader::Equipment: {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::int32_t image = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->image1014 = image;
        const std::int32_t baseImage = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->baseImage13AC = baseImage;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ShowReactionIcon);
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22321;
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->soundId1018 = 209;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayConfiguredImage);
        break;
    }
    case BattleMStatusHeader::StatusValue: {
        gBattleMStatusMode5ABA88 = TakeBattleMInteger_438F70(scene);
        gBattleMTechnique5A4A38 = TakeBattleMInteger_438F70(scene);
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t value = TakeBattleMInteger_438F70(scene);
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const skillActor = cg::world::gBattleAllEntries59E980[casterPosition];
        const std::int32_t skillLevel = gBattleMTechnique5A4A38;
        skillActor->payload.battleActor->castSkillLevel1044 = skillLevel;
        switch (gBattleMStatusMode5ABA88) {
        case BattleMStatusValueMode_DecreaseHpWithHit:
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->soundId1018 = 273;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ApplyResourceChange);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayHitAnimation);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1030 = value;
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->numberDisplayType1034 = BattleNumber_DecreaseHp;
            if ((*reinterpret_cast<const unsigned char*>(&gBattleMResultFlags5A4A20) & BattleResultFlag_Death) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_HandleDeath);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_MarkDead);
            }
            break;
        case BattleMStatusValueMode_IncreaseHpWithRecoveryEffect:
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayStatusHpRecoveryEffect);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ApplyResourceChangeAndWait);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1030 = value;
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->numberDisplayType1034 = BattleNumber_IncreaseHp;
            break;
        case BattleMStatusValueMode_DecreaseFp:
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->soundId1018 = 274;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ApplyResourceChangeAndWait);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1030 = value;
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->numberDisplayType1034 = BattleNumber_DecreaseFp;
            break;
        case BattleMStatusValueMode_DecreaseHpAndWait:
        case BattleMStatusValueMode_DecreaseHpWithImage104216AndWait:
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->soundId1018 = 273;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
            if (gBattleMStatusMode5ABA88 == BattleMStatusValueMode_DecreaseHpWithImage104216AndWait) {
                cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->image102C = 104216;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayConfiguredEffect42);
            }
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ApplyResourceChange);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1030 = value;
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->numberDisplayType1034 = BattleNumber_DecreaseHp;
            if ((*reinterpret_cast<const unsigned char*>(&gBattleMResultFlags5A4A20) & BattleResultFlag_Death) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_MarkDead);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_HandleDeath);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_Wait120Ticks);
            } else {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_Wait30Ticks);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BattleMStatusHeader::Debuff: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        auto* const actorPayload = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor;
        const std::int32_t state = TakeBattleMInteger_438F70(scene);
        const std::int32_t type = TakeBattleMInteger_438F70(scene);
        YobiAction* const current = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (current == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        if (actorPayload->currentDebuffState_13CC != state || actorPayload->currentDebuffType_13D0 != type) {
            if (state == CG_BATTLE_STATUS_MODE_RESET) {
                AppendBattleCast_4392A0(scene, current, BattleAction_ClearDebuff);
                actorPayload->statusEffectType13DC = CG_BATTLE_STATUS_TYPE_CLEAR_REQUEST;
                actorPayload->statusEffectMode13D8 = CG_BATTLE_STATUS_MODE_RESET;
            } else if (state == CG_BATTLE_STATUS_MODE_DEBUFF || state == CG_BATTLE_STATUS_MODE_EXTENDED) {
                AppendBattleCast_4392A0(scene, current, BattleAction_ClearDebuff);
                actorPayload->statusEffectType13DC = type;
                actorPayload->statusEffectMode13D8 = state;
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ApplyDebuff);
            }
        }
        break;
    }
    case BattleMStatusHeader::Power: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        auto* const actorPayload = actor->payload.battleActor;
        const std::int32_t mode = TakeBattleMInteger_438F70(scene) <= 0 ? CG_BATTLE_STATUS_MODE_PARAMETER : CG_BATTLE_STATUS_MODE_EXTENDED;
        const std::int32_t type = TakeBattleMInteger_438F70(scene);
        if (actorPayload->statusEffectMode13D8 != mode || actorPayload->statusEffectType13DC != type) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ClearStatusEffect);
            actorPayload->statusEffectType13DC = type;
            actorPayload->statusEffectMode13D8 = mode;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ApplyStatusEffect);
        }
        break;
    }
    case BattleMStatusHeader::Rebirth: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (actor == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        AppendBattleCast_4392A0(scene, actor, BattleAction_FaceFormation);
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        const std::int32_t image = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        if ((gBattleMResultFlags5A4A20 & BattleResultFlag_Success) != 0) {
            gBattleMTargetPosition5AAA40 = TakeBattleMInteger_438F70(scene);
        }
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        const std::int32_t effectPosition = gBattleMTargetPosition5AAA40;
        const BattleEntryEffectRecord effect {effectPosition, effectFlags, 0, image};
        AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], effect);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_TransitionRebirthForm);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_SetIdlePose);
        break;
    }
    case BattleMStatusHeader::QueuedValue: {
        gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(PeekBattleMInteger_43B860(scene));
        YobiAction* const preview = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
        if (preview != nullptr
            && (preview->payload.battleActor->busyFD8 != 0 || preview->phase != BattleAction_Idle)) {
            return;
        }
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::int32_t value = TakeBattleMInteger_438F70(scene);
        AppendBattleQueuedTarget_43B7F0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], value);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ConsumeQueuedTime);
        break;
    }
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    if (kind == BattleMStatusHeader::StatusValue) {
        ParseBattleMRide_4239D0(scene);
    }
    ++payload->packetSubState58;
}

// 原427470的POS=4；固定旧槽地址，保留非对称附件重绑及空槽后的子状态递增。
static void ProcessBattleMPositionHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    const std::int32_t subState = payload->packetSubState58;
    if (subState != BattleMQueuedPhase_ReadAndQueue) {
        if (subState == BattleMQueuedPhase_WaitForTurnReady && CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            const std::int32_t otherPosition = cg::world::GetBattlePetPosition_405B40(gBattleMCasterPosition5AAA64);
            FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            YobiAction* const other = cg::world::gBattleAllEntries59E980[otherPosition];
            if (other != nullptr && other->characterAction != CharacterAnimation_Down && other->phase != BattleAction_BounceOutAfterKnockout) {
                FaceBattleActorTowardFormationPosition_406260(other);
            }
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }

    const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const bool unavailable = position == 255;
    gBattleMCasterPosition5AAA64 = position;
    if (unavailable) {
        return;
    }
    if (cg::world::gBattleAllEntries59E980[position] == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        ++payload->packetSubState58;
        return;
    }
    const std::int32_t otherPosition = cg::world::GetBattlePetPosition_405B40(position);
    const std::int32_t initialPosition = gBattleMCasterPosition5AAA64;
    const bool movePlayer = cg::world::gBattleCurrentPosition5ABCA4 == initialPosition;
    YobiAction* const moving = cg::world::gBattleAllEntries59E980[initialPosition];
    YobiAction** const oldSlot = &cg::world::gBattleAllEntries59E980[initialPosition];
    const std::int32_t oldPosition = initialPosition;
    if (movePlayer) {
        cg::world::gBattleCurrentPosition5ABCA4 = otherPosition;
    }
    {
        const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
        auto* const movingPayload = moving->payload.battleActor;
        movingPayload->x00 = cg::world::gBattleGraphicPoints553758[pointIndex][2];
    }
    {
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[casterPosition];
        auto* const actorPayload = actor->payload.battleActor;
        actorPayload->y04 = cg::world::gBattleGraphicPoints553758[pointIndex][3];
    }
    {
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[casterPosition];
        auto* const actorPayload = actor->payload.battleActor;
        actorPayload->x08 = cg::world::gBattleGraphicPoints553758[pointIndex][0];
    }
    {
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[otherPosition];
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[casterPosition];
        auto* const actorPayload = actor->payload.battleActor;
        actorPayload->y0C = cg::world::gBattleGraphicPoints553758[pointIndex][1];
    }
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->battlePosition18 = otherPosition;
    YobiAction* const initialOther = cg::world::gBattleAllEntries59E980[otherPosition];
    if (initialOther != nullptr) {
        auto* const initialOtherPayload = initialOther->payload.battleActor;
        const std::int32_t casterPosition = gBattleMCasterPosition5AAA64;
        initialOtherPayload->battlePosition18 = casterPosition;
        YobiAction* const other = cg::world::gBattleAllEntries59E980[otherPosition];
        if (other != nullptr && other->phase != BattleAction_BounceOutAfterKnockout) {
            {
                const std::int32_t sampledCasterPosition = gBattleMCasterPosition5AAA64;
                auto* const otherPayload = other->payload.battleActor;
                const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[sampledCasterPosition];
                otherPayload->x00 = cg::world::gBattleGraphicPoints553758[pointIndex][2];
            }
            {
                const std::int32_t sampledCasterPosition = gBattleMCasterPosition5AAA64;
                YobiAction* const actor = cg::world::gBattleAllEntries59E980[otherPosition];
                const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[sampledCasterPosition];
                auto* const actorPayload = actor->payload.battleActor;
                actorPayload->y04 = cg::world::gBattleGraphicPoints553758[pointIndex][3];
            }
            {
                const std::int32_t sampledCasterPosition = gBattleMCasterPosition5AAA64;
                YobiAction* const actor = cg::world::gBattleAllEntries59E980[otherPosition];
                const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[sampledCasterPosition];
                auto* const actorPayload = actor->payload.battleActor;
                actorPayload->x08 = cg::world::gBattleGraphicPoints553758[pointIndex][0];
            }
            {
                const std::int32_t sampledCasterPosition = gBattleMCasterPosition5AAA64;
                YobiAction* const actor = cg::world::gBattleAllEntries59E980[otherPosition];
                const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[sampledCasterPosition];
                auto* const actorPayload = actor->payload.battleActor;
                actorPayload->y0C = cg::world::gBattleGraphicPoints553758[pointIndex][1];
            }
        }
    }
    const std::int32_t sourcePosition = gBattleMCasterPosition5AAA64;
    YobiAction* const replacement = cg::world::gBattleAllEntries59E980[otherPosition];
    gBattleMCasterPosition5AAA64 = otherPosition;
    cg::world::gBattleAllEntries59E980[sourcePosition] = replacement;
    cg::world::gBattleAllEntries59E980[otherPosition] = moving;
    if (moving != nullptr) {
        YobiAction* const rider = moving->payload.battleActor->rider13C8;
        if (rider != nullptr) {
            rider->battleAttachmentParent10 = moving;
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->rider13C8->value1D4 = gBattleMCasterPosition5AAA64;
        }
    }
    YobiAction* const oldActor = *oldSlot;
    if (oldActor != nullptr) {
        YobiAction* const rider = oldActor->payload.battleActor->rider13C8;
        if (rider != nullptr) {
            rider->battleAttachmentParent10 = oldActor;
            (*oldSlot)->payload.battleActor->rider13C8->value1D4 = oldPosition;
        }
        // 原4286E8在旧槽非空门禁内重绑新槽两附件，后面仍重复同两次写入。
        for (std::size_t index = 0; index < 2; ++index) {
            YobiAction* const actor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
            YobiAction* const gear = actor->payload.battleActor->auxiliaryGear13B8[index];
            if (gear != nullptr) {
                gear->battleAttachmentParent10 = actor;
                cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->auxiliaryGear13B8[index]->value1D4 = gBattleMCasterPosition5AAA64;
            }
        }
    }
    YobiAction* const newActor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
    if (newActor != nullptr) {
        YobiAction* const gear = newActor->payload.battleActor->gear13B4;
        if (gear != nullptr) {
            gear->battleAttachmentParent10 = newActor;
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->gear13B4->value1D4 = gBattleMCasterPosition5AAA64;
        }
        for (std::size_t index = 0; index < 2; ++index) {
            YobiAction* const actor = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
            YobiAction* const auxiliary = actor->payload.battleActor->auxiliaryGear13B8[index];
            if (auxiliary != nullptr) {
                auxiliary->battleAttachmentParent10 = actor;
                cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                    ->auxiliaryGear13B8[index]->value1D4 = gBattleMCasterPosition5AAA64;
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
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
        ->soundId1018 = 210;
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ReturnToFormation);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->motionMagnitude54 = 3.0F;
    cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->framePlayingRate = 100;
    YobiAction* const other = *oldSlot;
    if (other != nullptr && other->phase != BattleAction_BounceOutAfterKnockout) {
        AppendBattleCast_4392A0(scene, other, BattleAction_ReturnToFormation);
        MoveBattleCastState_439380(*oldSlot);
        (*oldSlot)->motionMagnitude54 = 3.0F;
        (*oldSlot)->framePlayingRate = 100;
    }
    ++payload->packetSubState58;
}

// 原427470的MON=5；子状态2沿恢复目标重试，成功解析期间固定目标对象及载荷。
static void ProcessBattleMMonsterHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    std::int32_t defenderPosition = gBattleMDefenderPosition5A4A24;
    const std::int32_t subState = payload->packetSubState58;
    if (subState == BattleMMonsterPhase_WaitForActors) {
        if (CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            YobiAction* const defender = cg::world::gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24];
            if (defender == nullptr || defender->payload.battleActor->busyFD8 == 0) {
                FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
                if (cg::world::gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24] != nullptr) {
                    const std::int32_t petPosition = cg::world::GetBattlePetPosition_405B40(gBattleMCasterPosition5AAA64);
                    FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[petPosition]);
                }
                FinishBattleEvent_43B6C0(scene);
            }
        }
        return;
    }
    if (subState == BattleMMonsterPhase_ReadActor) {
        const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[position];
        const bool actorPresent = actor != nullptr;
        gBattleMCasterPosition5AAA64 = position;
        if (!actorPresent) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        defenderPosition = cg::world::GetBattlePetPosition_405B40(position);
        gBattleMDefenderPosition5A4A24 = defenderPosition;
    } else if (subState != BattleMMonsterPhase_WaitForDefenderNotBusy) {
        return;
    }
    YobiAction* const pending = cg::world::gBattleAllEntries59E980[defenderPosition];
    if (pending != nullptr && pending->payload.battleActor->busyFD8 == 1) {
        payload->packetSubState58 = BattleMMonsterPhase_WaitForDefenderNotBusy;
        return;
    }
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_FaceTargetFormationPoint);
    const std::int32_t targetCasterPosition = gBattleMCasterPosition5AAA64;
    YobiAction* const targetCaster = cg::world::gBattleAllEntries59E980[targetCasterPosition];
    const std::int32_t targetPosition = gBattleMDefenderPosition5A4A24;
    targetCaster->payload.battleActor->targetPosition100C = targetPosition;
    gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
    const std::int32_t value = TakeBattleMInteger_438F70(scene);
    std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
    const bool playerEffect = cg::world::gBattleCurrentPosition5ABCA4 == effectCasterPosition;
    if (playerEffect && (gBattleMResultFlags5A4A20 == 0 || value != -1)) {
        AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[effectCasterPosition], -2, 0, 0);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_UpdatePetSnapshot);
        effectCasterPosition = gBattleMCasterPosition5AAA64;
    }
    const std::int32_t effectDefenderPosition = gBattleMDefenderPosition5A4A24;
    YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
    AppendBattleEffectList_4393A0(effectCaster, effectDefenderPosition, 0, 0);
    if (gBattleMResultFlags5A4A20 == 0) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ShowReactionIcon);
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22339;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_TriggerTargetsOnFrameEvent);
        std::int32_t failedDefenderPosition = gBattleMDefenderPosition5A4A24;
        YobiAction* const defender = cg::world::gBattleAllEntries59E980[failedDefenderPosition];
        if (defender == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        if ((*reinterpret_cast<const unsigned char*>(
                &defender->payload.battleActor->flagsFDC) & BattleActorFlag_Dead) != 0) {
            ResetBattleCastList_4391E0(failedDefenderPosition);
            failedDefenderPosition = gBattleMDefenderPosition5A4A24;
        }
        cg::world::gBattleAllEntries59E980[failedDefenderPosition]->payload.battleActor
            ->soundId1018 = 205;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], BattleAction_PlaySound1018);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], BattleAction_ShrinkOut);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], BattleAction_RemoveFromBattle);
    } else {
        const std::int32_t monsterPosition = gBattleMDefenderPosition5A4A24;
        YobiAction** const defenderSlot = &cg::world::gBattleAllEntries59E980[monsterPosition];
        if (*defenderSlot == nullptr) {
            *defenderSlot = NewBattleActor_438290(monsterPosition);
        }
        YobiAction* const defender = *defenderSlot;
        auto* const defenderPayload = defender->payload.battleActor;
        {
            char nameBytes[29]{};
            TakeBattleMString_438FE0(scene, nameBytes, 29);
            static_cast<void>(input::UnescapeLegacyText_52D150(nameBytes));
            defender->nameC4 = cg::text::FromGbkWide(nameBytes);
        }
        defender->imageNo = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        defender->field1D0 = TakeBattleMInteger_438F70(scene);
        {
            defender->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            defender->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            defender->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        {
            defender->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
        }
        defenderPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        defenderPayload->stateFE0 = 0;
        defender->fields288[3] = 0;
        defender->fields288[2] = 0;
        defender->fields288[1] = 0;
        defender->fields288[0] = 0;
        if ((defenderPayload->flagsFDC & BattleActorFlag_Earth) != 0) {
            defender->fields288[0] = 1;
        }
        if ((defenderPayload->flagsFDC & BattleActorFlag_Water) != 0) {
            defender->fields288[1] = 1;
        }
        if ((defenderPayload->flagsFDC & BattleActorFlag_Fire) != 0) {
            defender->fields288[2] = 1;
        }
        if ((defenderPayload->flagsFDC & BattleActorFlag_Wind) != 0) {
            defender->fields288[3] = 1;
        }
        defenderPayload->castPlayIndexFC4 = 0;
        defenderPayload->castListIndexFC8 = BattleCastQueueIndex_BeforeFirst;
        defenderPayload->castListCA4[0] = BattleAction_Idle;
        defenderPayload->castListCA4[1] = BattleCastQueueValue_End;
        defender->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[gBattleMDefenderPosition5A4A24]][2]);
        defender->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[gBattleMDefenderPosition5A4A24]][3]);
        defender->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[gBattleMDefenderPosition5A4A24]][2];
        defender->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[gBattleMDefenderPosition5A4A24]][3];
        defender->scaleX = 0.0F;
        defender->scaleY = 0.0F;
        defenderPayload->battlePosition18 = gBattleMDefenderPosition5A4A24;
        UpdateBattleAlbumFlag_4238F0(defender);
        defender->phase = BattleAction_Idle;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ShowReactionIcon);
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->reactionImage1010 = 22338;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_TriggerTargetsOnFrameEvent);
        cg::world::gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24]->payload.battleActor
            ->soundId1018 = 204;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], BattleAction_PlaySound1018);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMDefenderPosition5A4A24], BattleAction_GrowIn);
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    payload->packetSubState58 = BattleMMonsterPhase_WaitForActors;
}

// 原427470的ITM=7；三模式保留不同的目标结束路径，返回后仍由总入口回存。
static void ProcessBattleMItemHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    const std::int32_t subState = payload->packetSubState58;
    if (subState != BattleMQueuedPhase_ReadAndQueue) {
        const std::int32_t mode = gBattleMItemMode59FA10;
        if (subState == BattleMQueuedPhase_WaitForTurnReady && (mode == BattleMItemMode_RecoverTargetHp || mode == BattleMItemMode_SealCard || mode == BattleMItemMode_PlayNodAndWait)
            && CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }
    const std::int32_t mode = TakeBattleMInteger_438F70(scene);
    gBattleMItemMode59FA10 = mode;
    if (mode != BattleMItemMode_RecoverTargetHp && mode != BattleMItemMode_SealCard && mode != BattleMItemMode_PlayNodAndWait) {
        return;
    }
    const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMCasterPosition5AAA64 = casterPosition;
    YobiAction* const actor = cg::world::gBattleAllEntries59E980[casterPosition];
    if (actor == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    if (mode == BattleMItemMode_PlayNodAndWait) {
        actor->payload.battleActor->soundId1018 = 438;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlaySound1018);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayNodAnimationAndResetFrames);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_Wait120Ticks);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_Wait60Ticks);
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        ++payload->packetSubState58;
        return;
    }
    AppendBattleCast_4392A0(scene, actor, BattleAction_WaitTargetReady);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_FaceFormation);
    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayItemUseEffect);
    if (mode == BattleMItemMode_SealCard) {
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_AdvanceLinkedCast);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_Idle);
        const std::int32_t targetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool targetUnavailable = targetPosition == 255;
        gBattleMTargetPosition5AAA40 = targetPosition;
        if (targetUnavailable) {
            return;
        }
        if (cg::world::gBattleAllEntries59E980[targetPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            return;
        }
        const std::int32_t value = TakeBattleMInteger_438F70(scene);
        const std::int32_t valueTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMMessageValue59FA14 = value;
        cg::world::gBattleAllEntries59E980[valueTargetPosition]->payload.battleActor
            ->value1030 = value;
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, 0, 0);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_Idle);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_StartSealCardEffect);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_PulseScaleWithSealEffect);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_WaitSealCardEffectEnd);
        const std::int32_t reverseTargetPosition = gBattleMTargetPosition5AAA40;
        const std::int32_t reverseCasterPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const reverseTarget = cg::world::gBattleAllEntries59E980[reverseTargetPosition];
        AppendBattleEffectList_4393A0(reverseTarget, reverseCasterPosition, 0, 0);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_AdvanceLinkedCast);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_AdvanceEffectAndCast);
        if (gBattleMMessageValue59FA14 == 1) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ShowReactionIcon);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->reactionImage1010 = 28880;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayJoyAnimationAndResetFrames);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayJoyAnimationAndResetFrames);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayJoyAnimationAndResetFrames);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_RemoveFromBattle);
        } else {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ShowReactionIcon);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->reactionImage1010 = 22333;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayAngryAnimationAndResetFrames);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayAngryAnimationAndResetFrames);
        }
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_AdvanceEffectAndCast);
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        ++payload->packetSubState58;
        return;
    }

    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_StartEffectTargets);
    const std::int32_t value = TakeBattleMInteger_438F70(scene);
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
        const std::int32_t parsedMessageValue = TakeBattleMInteger_438F70(scene);
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        gBattleMMessageValue59FA14 = parsedMessageValue;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffectList_4393A0(effectCaster, effectTargetPosition, 0, 0);
        const std::int32_t skillTargetPosition = gBattleMTargetPosition5AAA40;
        const bool secondKind = value == 7;
        YobiAction* const skillTarget = cg::world::gBattleAllEntries59E980[skillTargetPosition];
        skillTarget->payload.battleActor->castSkillLevel1044 = 0;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], secondKind ? BattleAction_PlayAmountTieredRecoveryEffect : BattleAction_PlayHealEffect);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_ApplyResourceChangeAndWait);
        const std::int32_t messageTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const messageTarget = cg::world::gBattleAllEntries59E980[messageTargetPosition];
        const std::int32_t currentMessageValue = gBattleMMessageValue59FA14;
        messageTarget->payload.battleActor->value1030 = currentMessageValue;
        cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40]->payload.battleActor
            ->numberDisplayType1034 = BattleNumber_IncreaseHp;
    }
    AppendBattleEffectList_4393A0(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], -1, 0, 0);
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原427470的CMB=8；总入口phase8调用，原局部图号跨本次循环保留，处理四个子状态。
static void ProcessBattleMComboHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    switch (payload->packetSubState58) {
    case BattleMComboPhase_ReadAndQueue:
        break;
    case BattleMComboPhase_WaitForEntryCompletions:
        if (payload->completedEntryCount00 >= payload->entryCount04) {
            payload->completedEntryCount00 = 0;
            ++payload->packetSubState58;
        }
        return;
    case BattleMComboPhase_AdvanceEntriesAndShowSequence: {
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[payload->entryPositions08[payload->completedEntryCount00]];
        if (actor->phase == BattleAction_Idle) {
            AdvanceBattleCastState_439340(actor);
        }
        const std::int32_t count = payload->entryCount04;
        if (payload->completedEntryCount00 >= count - 1) {
            if ((gBattleMResultFlags5A4A20 & BattleResultMask_GuardOrSpecialGuard) == 0 && count >= 2) {
                const std::int32_t sequenceValue = gBattleMValue5ABA7C;
                const std::uint8_t sequenceCount =
                    *reinterpret_cast<const unsigned char*>(&payload->entryCount04);
                NewBattleSequence_420100(std::bit_cast<std::int8_t>(sequenceCount), sequenceValue);
            }
            ++payload->packetSubState58;
        }
        return;
    }
    case BattleMComboPhase_WaitForTurnReady:
        if (CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            YobiAction* const target = cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40];
            if (target != nullptr && (*reinterpret_cast<const unsigned char*>(
                    &target->payload.battleActor->flagsFDC) & BattleActorFlag_Dead) == 0) {
                FaceBattleActorTowardFormationPosition_406260(target);
            }
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    default:
        return;
    }

    std::int32_t firstImage = 0;
    std::int32_t secondImage = 0;
    std::array<std::int32_t, 2> firstImages {};
    std::array<std::int32_t, 2> secondImages {};
    std::array<std::int32_t, 2> values {};
    std::int32_t amount; // 原a4仅在每个非空施法者解析时写入，不补空列表默认值。
    payload->completedEntryCount00 = 0;
    payload->entryCount04 = 0;
    gBattleMMessageValue59FA14 = 0;
    const std::int32_t initialTargetPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    gBattleMTargetPosition5AAA40 = initialTargetPosition;
    YobiAction* const initialTarget = cg::world::gBattleAllEntries59E980[initialTargetPosition];
    if (initialTarget == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        return;
    }
    AppendBattleCast_4392A0(scene, initialTarget, BattleAction_Idle);
    for (;;) {
        const std::int32_t casterPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
        const bool casterUnavailable = casterPosition == 255;
        gBattleMCasterPosition5AAA64 = casterPosition;
        if (casterUnavailable) {
            break;
        }
        if (cg::world::gBattleAllEntries59E980[casterPosition] == nullptr) {
            FinishBattleEvent_43B6C0(scene);
            break;
        }
        gBattleMResultFlags5A4A20 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
        amount = TakeBattleMInteger_438F70(scene);
        if ((gBattleMResultFlags5A4A20 & BattleResultFlag_HasAuxiliaryGraphics) != 0) {
            TakeBattleMAuxGraphics_43BF30(
                scene, BattleMAuxGraphicsReadMode_ReadObjectFields, &firstImage, &secondImage, firstImages.data(), secondImages.data(), values.data());
        }
        cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
            ->castSkill103C = -1;
        const std::uint32_t effectFlags = gBattleMResultFlags5A4A20;
        std::int32_t effectAmount = amount;
        if ((effectFlags & BattleResultMask_PhysicalReaction) == 0) {
            const std::int32_t accumulatedValue = gBattleMMessageValue59FA14 + amount;
            gBattleMMessageValue59FA14 = accumulatedValue;
            effectAmount = accumulatedValue;
        }
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const std::int32_t effectTargetPosition = gBattleMTargetPosition5AAA40;
        YobiAction* const effectCaster = cg::world::gBattleAllEntries59E980[effectCasterPosition];
        AppendBattleEffect_439400(effectCaster, {effectTargetPosition, effectFlags, effectAmount, firstImage});
        const std::int32_t entryIndex = payload->entryCount04;
        const std::int32_t entryCasterPosition = gBattleMCasterPosition5AAA64;
        payload->entryPositions08[entryIndex] = entryCasterPosition;
        ++payload->entryCount04;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_WaitTargetReady);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayBeginRunFacingEffectTarget);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_MoveTowardTargetOrFormation);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayEndRunAndRestoreFacing);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_NotifyCasterEntryComplete);
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_Idle);
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
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PerformMeleeAttack);
        std::uint32_t flags = gBattleMResultFlags5A4A20;
        if ((flags & BattleResultMask_PhysicalReaction) == 0) {
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->value1038 = BattleComboFrameHitGate_ForceHitPath;
            flags = gBattleMResultFlags5A4A20;
        }
        if ((flags & BattleResultFlag_BounceOut) != 0) {
            if ((flags & BattleResultMask_NormalOrCritical) != 0 && (flags & BattleResultFlag_ReflectPhysical) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_BounceOutAfterKnockout);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_StartPhysicalReflectEffect);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_Idle);
                flags = gBattleMResultFlags5A4A20;
                if ((flags & BattleResultFlag_ReflectPhysical) != 0) {
                    const std::int32_t playerCasterPosition = gBattleMCasterPosition5AAA64;
                    if (cg::world::gBattleCurrentPosition5ABCA4 == playerCasterPosition) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerCasterPosition], BattleAction_RequestBattleExit);
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerCasterPosition], BattleAction_DismissPlayerPet);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_RemoveFromBattle);
                    }
                    flags = gBattleMResultFlags5A4A20;
                }
            }
        } else if (amount != 0 && (flags & BattleResultMask_NormalOrCritical) != 0) {
            if ((flags & BattleResultFlag_NullifyPhysical) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_StartPhysicalNullifyEffect);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_Idle);
                flags = gBattleMResultFlags5A4A20;
            } else if ((flags & BattleResultFlag_AbsorbPhysical) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_StartPhysicalAbsorbEffect);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_Idle);
                flags = gBattleMResultFlags5A4A20;
            } else if ((flags & BattleResultFlag_ReflectPhysical) != 0) {
                const bool secondReaction = (flags & BattleResultFlag_Critical) != 0;
                const std::int32_t reactionCasterPosition = gBattleMCasterPosition5AAA64;
                const std::int32_t reactionKind = secondReaction ? BattleAction_ReactToCriticalHit : BattleAction_ReactToHit;
                YobiAction* const reactionCaster = cg::world::gBattleAllEntries59E980[reactionCasterPosition];
                AppendBattleCast_4392A0(scene, reactionCaster, reactionKind);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_StartPhysicalReflectEffect);
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_Idle);
                flags = gBattleMResultFlags5A4A20;
            }
        }
        if ((flags & BattleResultMask_DeathOrPhysicalReflect) == BattleResultMask_DeathOrPhysicalReflect) {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ReturnToFormation);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_HandleDeath);
        } else {
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayBeginRunFacingEffectTarget);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ReturnToFormation);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayEndRunAndRestoreFacing);
        }
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
        gBattleMValue5ABA7C = gBattleMCasterPosition5AAA64;
    }

    cg::world::gBattleAllEntries59E980[gBattleMValue5ABA7C]->payload.battleActor->value1038 = BattleComboFrameHitGate_UseFrameEvent;
    std::uint32_t flags = gBattleMResultFlags5A4A20;
    if ((flags & BattleResultMask_PhysicalReaction) != 0) {
        YobiAction* const target = cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40];
        AppendBattleCastAt_4392F0(scene, target, BattleCastQueueValue_End,
            target->payload.battleActor->castListIndexFC8 - 1);
    } else {
        if ((flags & BattleResultFlag_BounceOut) != 0) {
            if ((flags & BattleResultMask_NormalOrCritical) != 0) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_BounceOutAfterKnockout);
                if ((gBattleMResultFlags5A4A20 & BattleResultFlag_ReflectPhysical) != 0) {
                    const std::int32_t playerCasterPosition = gBattleMCasterPosition5AAA64;
                    if (cg::world::gBattleCurrentPosition5ABCA4 == playerCasterPosition) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerCasterPosition], BattleAction_RequestBattleExit);
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerCasterPosition], BattleAction_DismissPlayerPet);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_RemoveFromBattle);
                    }
                } else {
                    const std::int32_t playerTargetPosition = gBattleMTargetPosition5AAA40;
                    if (cg::world::gBattleCurrentPosition5ABCA4 == playerTargetPosition) {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], BattleAction_RequestBattleExit);
                    } else {
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[playerTargetPosition], BattleAction_DismissPlayerPet);
                        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMTargetPosition5AAA40], BattleAction_RemoveFromBattle);
                    }
                }
                flags = gBattleMResultFlags5A4A20;
            }
        } else if (amount != 0 && (flags & BattleResultMask_NormalOrCritical) != 0) {
            const std::int32_t reactionKind = (flags & BattleResultFlag_Critical) != 0 ? BattleAction_ReactToCriticalHit : BattleAction_ReactToHit;
            const std::int32_t reactionTargetPosition = gBattleMTargetPosition5AAA40;
            YobiAction* const reactionTarget = cg::world::gBattleAllEntries59E980[reactionTargetPosition];
            AppendBattleCast_4392A0(scene, reactionTarget, reactionKind);
            flags = gBattleMResultFlags5A4A20;
        }
        std::int32_t followupTargetPosition;
        if ((amount != 0 || (flags & BattleResultFlag_Avoidance) != 0) && (flags & BattleResultFlag_AbsorbPhysical) == 0) {
            followupTargetPosition = gBattleMTargetPosition5AAA40;
            const std::int32_t lastCasterPosition = gBattleMValue5ABA7C;
            const bool samePosition = followupTargetPosition == lastCasterPosition;
            if ((flags & BattleResultFlag_ReflectPhysical) != 0 ? samePosition : !samePosition) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[followupTargetPosition], BattleAction_ReturnToFormation);
                followupTargetPosition = gBattleMTargetPosition5AAA40;
                flags = gBattleMResultFlags5A4A20;
            }
        } else {
            followupTargetPosition = gBattleMTargetPosition5AAA40;
        }
        if ((flags & BattleResultFlag_Death) != 0) {
            const bool sameSide = (flags & BattleResultFlag_ReflectPhysical) != 0;
            const std::int32_t lastCasterPosition = gBattleMValue5ABA7C;
            const bool samePosition = followupTargetPosition == lastCasterPosition;
            if (sameSide ? samePosition : !samePosition) {
                AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[followupTargetPosition], BattleAction_HandleDeath);
            }
        }
    }
    ParseBattleMRide_4239D0(scene);
    ++payload->packetSubState58;
}

// 原427470的GMN=21；模式0仅退场，非零模式保留固定新对象与当前槽的区别。
static void ProcessBattleMGmnHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    const std::int32_t subState = payload->packetSubState58;
    if (subState != BattleMQueuedPhase_ReadAndQueue) {
        if (subState != BattleMQueuedPhase_WaitForTurnReady) {
            return;
        }
        YobiAction* const first = cg::world::gBattleAllEntries59E980[20];
        if (first != nullptr && first->payload.battleActor->busyFD8 == 1) {
            return;
        }
        YobiAction* const second = cg::world::gBattleAllEntries59E980[21];
        if (second != nullptr && second->payload.battleActor->busyFD8 == 1) {
            return;
        }
        if (CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }

    const std::int32_t position = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t mode = TakeBattleMInteger_438F70(scene);
    if (mode == BattleMGmnMode_RemoveActorAndRevealTeam) {
        YobiAction** const slot = &cg::world::gBattleAllEntries59E980[position];
        const std::int32_t palette = static_cast<std::int32_t>(((*slot)->mapX1E0));
        AppendBattleEffectList_4393A0(*slot, palette, 0, 40);
        AppendBattleCast_4392A0(scene, *slot, BattleAction_SelectPalette);
        AppendBattleCast_4392A0(scene, *slot, BattleAction_EndBackgroundFade);
        AppendBattleCast_4392A0(scene, *slot, BattleAction_FadeOutStep3);
        AppendBattleCast_4392A0(scene, *slot, BattleAction_ClearColorOverrides);
        AppendBattleCast_4392A0(scene, *slot, BattleAction_RemoveFromBattle);
        const std::int32_t begin = position == 21 ? 10 : 0;
        const std::int32_t end = position == 21 ? 20 : 10;
        for (std::int32_t index = begin; index < end; ++index) {
            YobiAction** const memberSlot = &cg::world::gBattleAllEntries59E980[index];
            YobiAction* const member = *memberSlot;
            if (member != nullptr) {
                AppendBattleCast_4392A0(scene, member, BattleAction_FadeIn);
                AppendBattleCast_4392A0(scene, *memberSlot, BattleAction_ClearColorOverrides);
                MoveBattleCastState_439380(*memberSlot);
            }
        }
        MoveBattleCastState_439380(*slot);
        ++payload->packetSubState58;
        return;
    }

    std::int8_t entrance = 0;
    if (mode == BattleMGmnMode_EnterWithEntrance2) {
        entrance = 2;
    } else if (mode == BattleMGmnMode_EnterWithRandomEntranceAndEmitter) {
        entrance = static_cast<std::int8_t>((platform::Rand16() & 1) + 1);
        static_cast<void>(NewBattleScreenEmitter_421700(150));
    }
    gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t soundId1018 = TakeBattleMInteger_438F70(scene);
    std::int32_t effectValue = TakeBattleMInteger_438F70(scene);
    if (effectValue == 0) {
        effectValue = -1;
    }
    YobiAction** const slot = &cg::world::gBattleAllEntries59E980[position];
    YobiAction* const previous = *slot;
    if (previous != nullptr) {
        static_cast<void>(MarkDeath(previous));
    }
    YobiAction* const created = NewBattleActor_438290(position);
    *slot = created;
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
    actorPayload->castPlayIndexFC4 = 0;
    actorPayload->castListIndexFC8 = BattleCastQueueIndex_BeforeFirst;
    actorPayload->castListCA4[0] = BattleAction_Idle;
    actorPayload->castListCA4[1] = BattleCastQueueValue_End;
    actorPayload->effectRunningC9C = 0;
    actorPayload->effectCountCA0 = 0;
    actorPayload->effects1C[0].position = -1;
    actorPayload->effects1C[1].position = -1;
    created->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][2]);
    created->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][3]);
    created->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][2];
    created->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][3];
    actorPayload->battlePosition18 = position;
    created->phase = BattleAction_Idle;
    created->rebirthValue1 = 4;
    created->rebirthValue2 = 4;
    {
        created->mapX1E0 = static_cast<std::int32_t>(render::gActivePalette);
    }
    static_cast<void>(PlayFrame_517DA0(*slot, AnimationPlayback_Loop));
    static_cast<void>(NewBattleText_4217F0(4, created->nameC4.data(), 0, 100));
    (*slot)->payload.battleActor->soundId1018 = soundId1018;
    AppendBattleCast_4392A0(scene, *slot, BattleAction_PlaySound1018);
    AppendBattleEffect_439400(*slot, {13, 0, 40, effectValue});
    AppendBattleCast_4392A0(scene, *slot, BattleAction_StartBackgroundFade);
    AppendBattleCast_4392A0(scene, *slot, BattleAction_SelectPalette);
    AppendBattleCast_4392A0(scene, *slot, BattleAction_Idle);
    AppendBattleCast_4392A0(scene, *slot, BattleAction_Wait60Ticks);
    AppendBattleCast_4392A0(scene, *slot, BattleAction_EnableAlphaOverride);
    if (entrance == 0) {
        created->scaleX = 0.0F;
        created->scaleY = 0.0F;
        AppendBattleCast_4392A0(scene, *slot, BattleAction_GrowIn);
    } else if (entrance == 1) {
        SetYobiActionField30F(*slot, 0);
        (*slot)->flags |= YobiActionFlag_Hidden;
        FaceBattleActorTowardFormationPosition_406260(*slot);
        AppendBattleCast_4392A0(scene, *slot, BattleAction_FadeIn);
    } else if (entrance == 2) {
        {
            const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[position];
            YobiAction* const actor = *slot;
            actor->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[pointIndex][0]);
        }
        {
            const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[position];
            YobiAction* const actor = *slot;
            actor->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[pointIndex][1]);
        }
        {
            const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[position];
            YobiAction* const actor = *slot;
            auto* const currentPayload = actor->payload.battleActor;
            currentPayload->moveTargetXFE8 = cg::world::gBattleGraphicPoints553758[pointIndex][2];
        }
        {
            const std::int32_t pointIndex = cg::world::gBattlePointIndices5536FC[position];
            YobiAction* const actor = *slot;
            auto* const currentPayload = actor->payload.battleActor;
            currentPayload->moveTargetYFEC = cg::world::gBattleGraphicPoints553758[pointIndex][3];
        }
        YobiAction* const xActor = *slot;
        xActor->x = static_cast<std::int32_t>(static_cast<std::int64_t>(xActor->offset40));
        YobiAction* const yActor = *slot;
        yActor->y = static_cast<std::int32_t>(static_cast<std::int64_t>(yActor->offset44));
        FaceBattleActorTowardFormationPosition_406260(*slot);
        AppendBattleCast_4392A0(scene, *slot, BattleAction_MoveToTargetPosition);
        AppendBattleCast_4392A0(scene, *slot, BattleAction_PlayEndRunAndRestoreFacing);
    }

    const std::int32_t begin = position == 21 ? 10 : 0;
    const std::int32_t end = position == 21 ? 20 : 10;
    std::int32_t linkedPosition = gBattleMCasterPosition5AAA64;
    if (position == 20 || position == 21) {
        for (std::int32_t index = begin; index < end; ++index) {
            if (cg::world::gBattleAllEntries59E980[index] != nullptr) {
                linkedPosition = index;
                break;
            }
        }
    }
    for (std::int32_t index = begin; index < end; ++index) {
        YobiAction** const memberSlot = &cg::world::gBattleAllEntries59E980[index];
        YobiAction* const member = *memberSlot;
        if (member == nullptr) {
            continue;
        }
        AppendBattleCast_4392A0(scene, member, BattleAction_Wait60Ticks);
        AppendBattleCast_4392A0(scene, *memberSlot, BattleAction_EnableAlphaOverride);
        YobiAction* const colored = *memberSlot;
        colored->color2 = std::bit_cast<std::int32_t>(colored->drawMode.color);
        YobiAction* const flagActor = *memberSlot;
        const bool linkedMember = index == linkedPosition;
        SetYobiActionField30F(flagActor, 255);
        if (linkedMember) {
            AppendBattleEffect_439400(*memberSlot,
                {position, static_cast<std::uint32_t>(static_cast<std::int32_t>(entrance)), 0, effectValue});
            AppendBattleCast_4392A0(scene, *memberSlot, BattleAction_AdvanceLinkedCast);
            AppendBattleCast_4392A0(scene, *memberSlot, BattleAction_AdvanceEffectAndCast);
        }
        const bool casterMember = index == gBattleMCasterPosition5AAA64;
        YobiAction* const castMember = *memberSlot;
        AppendBattleCast_4392A0(scene, castMember, casterMember ? BattleAction_FadeOutStep2 : BattleAction_FadeOutStep3);
        if (index == linkedPosition) {
            AppendBattleCast_4392A0(scene, *memberSlot, BattleAction_Idle);
        }
        MoveBattleCastState_439380(*memberSlot);
    }
    AppendBattleEffect_439400(*slot, {linkedPosition, 0, 0, 0});
    AppendBattleCast_4392A0(scene, *slot, BattleAction_WaitTargetReady);
    AppendBattleCast_4392A0(scene, *slot, BattleAction_AdvanceLinkedCast);
    AppendBattleCast_4392A0(scene, *slot, BattleAction_AdvanceEffectAndCast);
    MoveBattleCastState_439380(*slot);
    gBattleMValue5ABA7C = gBattleMCasterPosition5AAA64;
    ++payload->packetSubState58;
}

// 原427470的RID=22；独立于4239D0预读路径，固定旧载荷并保留两名新角色各自坐标。
static void ProcessBattleMRideHeader(
    YobiAction* const scene, BattleScenePayload* const payload)
{
    const std::int32_t subState = payload->packetSubState58;
    if (subState != BattleMQueuedPhase_ReadAndQueue) {
        if (subState == BattleMQueuedPhase_WaitForTurnReady && CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_AllowInactiveSkip) == 1) {
            FinishBattleEvent_43B6C0(scene);
        }
        return;
    }
    const std::int32_t mode = TakeBattleMInteger_438F70(scene);
    gBattleMCasterPosition5AAA64 = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    const std::int32_t secondPosition = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(scene));
    YobiAction* const existing = cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64];
    if (existing == nullptr) {
        FinishBattleEvent_43B6C0(scene);
        ++payload->packetSubState58;
        return;
    }
    auto* const existingPayload = existing->payload.battleActor;
    const std::uint32_t resultFlags = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
    gBattleMResultFlags5A4A20 = resultFlags;
    if (mode != 0) {
        if ((resultFlags & BattleResultFlag_Success) != 0) {
            const std::int32_t firstPosition = gBattleMCasterPosition5AAA64;
            if (cg::world::gBattleAllEntries59E980[secondPosition] == nullptr) {
                FinishBattleEvent_43B6C0(scene);
                ++payload->packetSubState58;
                return;
            }
            YobiAction* const carrierAction = NewYobiAction_404AE0(2, sizeof(BattleRideCarrierPayload), YobiPayloadKind::BattleRideCarrier);
            auto* const carrier = carrierAction->payload.battleRideCarrier;
            {
                char nameBytes[29]{};
                TakeBattleMString_438FE0(scene, nameBytes, 29);
                carrierAction->nameC4 = cg::text::FromGbkWide(nameBytes);
            }
            carrierAction->imageNo = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
            carrier->gearImage0C = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
            carrierAction->field1D0 = TakeBattleMInteger_438F70(scene);
            {
                carrierAction->readiness = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                carrierAction->maxHp = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                carrierAction->barValue1B0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            {
                carrierAction->barMaximum1C0 = static_cast<std::int32_t>(TakeBattleMInteger_438F70(scene));
            }
            carrier->flags08 = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(scene));
            carrierAction->offset40 = static_cast<float>(
                cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][2]);
            carrierAction->offset44 = static_cast<float>(
                cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][3]);
            existingPayload->effect1060 = carrierAction;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_TransitionRideState);
            const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
            const BattleEntryEffectRecord effectRecord {firstPosition, 1u, effectCasterPosition, secondPosition};
            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[effectCasterPosition], effectRecord);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_FaceFormation);
        } else {
            const std::int32_t firstPosition = gBattleMCasterPosition5AAA64;
            YobiAction* const effect = NewYobiAction_404AE0(2, 0);
            existingPayload->effect1060 = effect;
            effect->offset40 = static_cast<float>(
                cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][2]);
            const std::int32_t pointIndexY = cg::world::gBattlePointIndices5536FC[firstPosition];
            YobiAction* const effectY = existingPayload->effect1060;
            effectY->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[pointIndexY][3]);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_FailToMountAndReturn);
            const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
            const BattleEntryEffectRecord effectRecord {firstPosition, 1u, effectCasterPosition, secondPosition};
            AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[effectCasterPosition], effectRecord);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_ShowReactionIcon);
            cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]->payload.battleActor
                ->reactionImage1010 = 22331;
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayAngryAnimationAndResetFrames);
            AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_PlayAngryAnimationAndResetFrames);
            YobiAction* const second = cg::world::gBattleAllEntries59E980[secondPosition];
            if (second != nullptr) {
                auto* const secondPayload = second->payload.battleActor;
                if ((*reinterpret_cast<const unsigned char*>(&secondPayload->flagsFDC) & BattleActorFlag_Dead) == 0) {
                    secondPayload->reactionImage1010 = 22334;
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondPosition], BattleAction_ShowReactionIcon);
                    AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[secondPosition], BattleAction_OscillateFacing);
                    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[secondPosition]);
                }
            }
        }
    } else {
        YobiAction* const carrierAction = NewYobiAction_404AE0(2, sizeof(BattleRideCarrierPayload), YobiPayloadKind::BattleRideCarrier);
        auto* const carrier = carrierAction->payload.battleRideCarrier;
        existingPayload->effect1060 = carrierAction;
        const std::int32_t firstPosition = gBattleMCasterPosition5AAA64;
        YobiAction* const first = NewBattleActor_438290(firstPosition);
        auto* const firstPayload = first->payload.battleActor;
        {
            char nameBytes[29]{};
            TakeBattleMString_438FE0(scene, nameBytes, 29);
            first->nameC4 = cg::text::FromGbkWide(nameBytes);
        }
        first->imageNo = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
        carrier->gearImage0C = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(scene));
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
        firstPayload->castPlayIndexFC4 = 0;
        firstPayload->castListIndexFC8 = BattleCastQueueIndex_BeforeFirst;
        firstPayload->castListCA4[0] = BattleAction_Idle;
        firstPayload->castListCA4[1] = BattleCastQueueValue_End;
        first->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][2]);
        first->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][3]);
        first->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][2];
        first->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[firstPosition]][3];
        first->scaleX = 1.0F;
        first->scaleY = 1.0F;
        firstPayload->battlePosition18 = firstPosition;
        UpdateBattleAlbumFlag_4238F0(first);
        first->phase = BattleAction_Idle;
        first->flags |= YobiActionFlag_Hidden;
        first->callback = nullptr;
        carrier->first00 = first;
        existingPayload->rider13C8->nameC4 = first->nameC4;
        existingPayload->rider13C8->field1D0 = first->field1D0;
        existingPayload->rider13C8->readiness = first->readiness;
        existingPayload->rider13C8->maxHp = first->maxHp;
        existingPayload->rider13C8->barValue1B0 = first->barValue1B0;
        existingPayload->rider13C8->barMaximum1C0 = first->barMaximum1C0;
        const std::int32_t casterPositionX = gBattleMCasterPosition5AAA64;
        YobiAction* const riderX = existingPayload->rider13C8;
        YobiAction* const sourceX = cg::world::gBattleAllEntries59E980[casterPositionX];
        riderX->offset40 = static_cast<float>(static_cast<double>(sourceX->frameX) + sourceX->offset40);
        const std::int32_t casterPositionY = gBattleMCasterPosition5AAA64;
        YobiAction* const riderY = existingPayload->rider13C8;
        YobiAction* const sourceY = cg::world::gBattleAllEntries59E980[casterPositionY];
        riderY->offset44 = sourceY->offset44;

        YobiAction* const second = NewBattleActor_438290(secondPosition);
        auto* const secondPayload = second->payload.battleActor;
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
        secondPayload->castPlayIndexFC4 = 0;
        secondPayload->castListIndexFC8 = BattleCastQueueIndex_BeforeFirst;
        secondPayload->castListCA4[0] = BattleAction_Idle;
        secondPayload->castListCA4[1] = BattleCastQueueValue_End;
        second->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][2]);
        second->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][3]);
        second->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][2];
        second->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[secondPosition]][3];
        second->scaleX = 1.0F;
        second->scaleY = 1.0F;
        secondPayload->battlePosition18 = secondPosition;
        UpdateBattleAlbumFlag_4238F0(second);
        second->phase = BattleAction_Idle;
        second->flags |= YobiActionFlag_Hidden;
        second->callback = nullptr;
        carrier->second04 = second;
        AppendBattleCast_4392A0(scene, cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64], BattleAction_TransitionRideState);
        const std::int32_t effectCasterPosition = gBattleMCasterPosition5AAA64;
        const BattleEntryEffectRecord effectRecord {effectCasterPosition, 0u, firstPosition, secondPosition};
        AppendBattleEffect_439400(cg::world::gBattleAllEntries59E980[effectCasterPosition], effectRecord);
        YobiAction* const firstEntry = carrier->first00;
        if ((*reinterpret_cast<const unsigned char*>(
                &firstEntry->payload.battleActor->flagsFDC) & BattleActorFlag_Dead) != 0) {
            AppendBattleCast_4392A0(scene, firstEntry, BattleAction_HandleDeath);
        }
        YobiAction* const secondEntry = carrier->second04;
        if ((*reinterpret_cast<const unsigned char*>(
                &secondEntry->payload.battleActor->flagsFDC) & BattleActorFlag_Dead) != 0) {
            AppendBattleCast_4392A0(scene, secondEntry, BattleAction_HandleDeath);
        }
    }
    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[gBattleMCasterPosition5AAA64]);
    ++payload->packetSubState58;
}

// 原函数：0x00427470。
void UpdateBattleEvent_427470(YobiAction* const scene)
{
    auto* const payload = scene->payload.battleScene;
    std::uint32_t flags9C;
    std::int32_t feedbackAlternateImageA0;
    if (!PrepareBattleMExecution(scene, payload, flags9C, feedbackAlternateImageA0)) {
        return;
    }
    switch (scene->phase) {
    case BattleMHeader_SKL: {
        // E10／42A768：完成走437ABA出口，处理中仍走437A29公共回存。
        const auto result = ProcessLuaSkillPacket(scene);
        if (result == LuaSkillPacketResult::Finished) return;
        if (result == LuaSkillPacketResult::Native) {
            ProcessBattleMSkillHeader(scene, payload, flags9C, feedbackAlternateImageA0);
        }
        break;
    }
    case BattleMHeader_ESC:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::Escape);
        break;
    case BattleMHeader_CLR:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::Clear);
        break;
    case BattleMHeader_SUP:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::Support);
        break;
    case BattleMHeader_POS:
        ProcessBattleMPositionHeader(scene, payload);
        break;
    case BattleMHeader_MON:
        ProcessBattleMMonsterHeader(scene, payload);
        break;
    case BattleMHeader_EQU:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::Equipment);
        break;
    case BattleMHeader_ITM:
        ProcessBattleMItemHeader(scene, payload);
        break;
    case BattleMHeader_CMB:
        ProcessBattleMComboHeader(scene, payload);
        break;
    case BattleMHeader_PSN:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::StatusValue);
        break;
    case BattleMHeader_ANR:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::Debuff);
        break;
    case BattleMHeader_PUE:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::Power);
        break;
    case BattleMHeader_ACT:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::Action);
        break;
    case BattleMHeader_CFP:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::ConsumeFp);
        break;
    case BattleMHeader_CHT:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::Chat);
        break;
    case BattleMHeader_PNO:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::PetNotice);
        break;
    case BattleMHeader_PLV:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::PetLeave);
        break;
    case BattleMHeader_CBC:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::ChangeBody);
        break;
    case BattleMHeader_PDN:
        ProcessBattleMTransitionHeader(scene, payload, BattleMTransitionHeader::PetDown);
        break;
    case BattleMHeader_REB:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::Rebirth);
        break;
    case BattleMHeader_BST:
        ProcessBattleMStatusHeader(scene, payload, BattleMStatusHeader::QueuedValue);
        break;
    case BattleMHeader_GMN:
        ProcessBattleMGmnHeader(scene, payload);
        break;
    case BattleMHeader_RID:
        ProcessBattleMRideHeader(scene, payload);
        break;
    case BattleMHeader_DSK:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::AppendSkill);
        break;
    case BattleMHeader_END:
        ProcessBattleMDataHeader(scene, payload, BattleMDataHeader::End);
        break;
    default:
        break;
    }
    SaveBattleMExecutionState(payload);
}

// 原函数：0x004239D0。
void ParseBattleMRide_4239D0(YobiAction* const action)
{
    auto* const scene = action->payload.battleScene;
    std::array<char, 4> header;
    std::array<char, 32> previewName;
    for (;;) {
        const std::int32_t initialOffset = scene->packetOffset64;
        static_cast<void>(GetBattleMObjectHeader_423960(action, header.data()));
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind != BattleMHeader_RID) {
            scene->packetOffset64 = initialOffset;
            return;
        }
        std::int32_t previewOffset = scene->packetOffset64;
        if (TakeBattleMIntegerAt_43B940(&previewOffset) != 0) {
            scene->packetOffset64 = initialOffset;
            return;
        }
        for (std::int32_t i = 0; i < 3; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        }
        TakeBattleMStringAt_43B8C0(&previewOffset, previewName.data(), 29);
        // 原预读20个整数，实际消费19个；成功后仍发布预读游标。
        for (std::int32_t i = 0; i < 8; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        }
        const std::int32_t firstPreviewFlags = TakeBattleMIntegerAt_43B940(&previewOffset);
        TakeBattleMStringAt_43B8C0(&previewOffset, previewName.data(), 29);
        for (std::int32_t i = 0; i < 6; ++i) {
            static_cast<void>(TakeBattleMIntegerAt_43B940(&previewOffset));
        }
        const std::int32_t secondPreviewFlags = TakeBattleMIntegerAt_43B940(&previewOffset);
        if ((*reinterpret_cast<const unsigned char*>(&firstPreviewFlags) & 2u) == 0
            && (secondPreviewFlags & 2) == 0) {
            scene->packetOffset64 = initialOffset;
            return;
        }

        static_cast<void>(TakeBattleMInteger_438F70(action));
        const std::int32_t first = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(action));
        const std::int32_t second = GetBattlePosition_4237E0(TakeBattleMInteger_438F70(action));
        YobiAction* const existing = cg::world::gBattleAllEntries59E980[first];
        if (existing == nullptr) {
            FinishBattleEvent_43B6C0(action);
            ++scene->packetSubState58;
            return;
        }
        auto* const existingPayload = existing->payload.battleActor;
        static_cast<void>(TakeBattleMInteger_438F70(action));
        YobiAction* const firstActor = NewBattleActor_438290(first);
        auto* const firstPayload = firstActor->payload.battleActor;
        {
            char nameBytes[29]{};
            TakeBattleMString_438FE0(action, nameBytes, 29);
            firstActor->nameC4 = cg::text::FromGbkWide(nameBytes);
        }
        firstActor->imageNo = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(action));
        const std::int32_t gearImage = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(action));
        firstActor->field1D0 = TakeBattleMInteger_438F70(action);
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            firstActor->readiness = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            firstActor->maxHp = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            firstActor->barValue1B0 = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            firstActor->barMaximum1C0 = static_cast<std::int32_t>(value);
        }
        firstPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(action));
        firstPayload->stateFE0 = 0;
        firstActor->fields288[3] = 0;
        firstActor->fields288[2] = 0;
        firstActor->fields288[1] = 0;
        firstActor->fields288[0] = 0;
        if ((firstPayload->flagsFDC & BattleActorFlag_Earth) != 0) {
            firstActor->fields288[0] = 1;
        }
        if ((firstPayload->flagsFDC & BattleActorFlag_Water) != 0) {
            firstActor->fields288[1] = 1;
        }
        if ((firstPayload->flagsFDC & BattleActorFlag_Fire) != 0) {
            firstActor->fields288[2] = 1;
        }
        if ((firstPayload->flagsFDC & BattleActorFlag_Wind) != 0) {
            firstActor->fields288[3] = 1;
        }
        firstPayload->castPlayIndexFC4 = 0;
        firstPayload->castListIndexFC8 = BattleCastQueueIndex_BeforeFirst;
        firstPayload->castListCA4[0] = BattleAction_Idle;
        firstPayload->castListCA4[1] = BattleCastQueueValue_End;
        firstActor->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[first]][2]);
        firstActor->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[first]][3]);
        firstActor->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[first]][2];
        firstActor->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[first]][3];
        action->scaleX = 1.0F;
        action->scaleY = 1.0F;
        firstPayload->battlePosition18 = first;
        UpdateBattleAlbumFlag_4238F0(firstActor);
        const std::uint32_t firstFlags = firstActor->flags | YobiActionFlag_Hidden;
        firstActor->phase = BattleAction_Idle;
        firstActor->flags = firstFlags;
        firstActor->callback = nullptr;

        existingPayload->rider13C8->nameC4 = firstActor->nameC4;
        existingPayload->rider13C8->field1D0 = firstActor->field1D0;
        existingPayload->rider13C8->readiness = firstActor->readiness;
        existingPayload->rider13C8->maxHp = firstActor->maxHp;
        existingPayload->rider13C8->barValue1B0 = firstActor->barValue1B0;
        existingPayload->rider13C8->barMaximum1C0 = firstActor->barMaximum1C0;
        YobiAction* const riderX = existingPayload->rider13C8;
        YobiAction** const originalSlot = &cg::world::gBattleAllEntries59E980[first];
        YobiAction* const sourceX = *originalSlot;
        riderX->offset40 = static_cast<float>(static_cast<double>(sourceX->frameX) + sourceX->offset40);
        YobiAction* const sourceY = *originalSlot;
        YobiAction* const riderY = existingPayload->rider13C8;
        riderY->offset44 = sourceY->offset44;

        YobiAction* const secondActor = NewBattleActor_438290(second);
        auto* const secondPayload = secondActor->payload.battleActor;
        {
            char nameBytes[29]{};
            TakeBattleMString_438FE0(action, nameBytes, 29);
            secondActor->nameC4 = cg::text::FromGbkWide(nameBytes);
        }
        secondActor->imageNo = character_select::ResolveCharacterImageForVersion(TakeBattleMInteger_438F70(action));
        secondActor->field1D0 = TakeBattleMInteger_438F70(action);
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            secondActor->readiness = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            secondActor->maxHp = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            secondActor->barValue1B0 = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = TakeBattleMInteger_438F70(action);
            secondActor->barMaximum1C0 = static_cast<std::int32_t>(value);
        }
        secondPayload->flagsFDC = static_cast<std::uint32_t>(TakeBattleMInteger_438F70(action));
        secondPayload->stateFE0 = 0;
        secondActor->fields288[3] = 0;
        secondActor->fields288[2] = 0;
        secondActor->fields288[1] = 0;
        secondActor->fields288[0] = 0;
        if ((secondPayload->flagsFDC & BattleActorFlag_Earth) != 0) {
            secondActor->fields288[0] = 1;
        }
        if ((secondPayload->flagsFDC & BattleActorFlag_Water) != 0) {
            secondActor->fields288[1] = 1;
        }
        if ((secondPayload->flagsFDC & BattleActorFlag_Fire) != 0) {
            secondActor->fields288[2] = 1;
        }
        if ((secondPayload->flagsFDC & BattleActorFlag_Wind) != 0) {
            secondActor->fields288[3] = 1;
        }
        secondPayload->castPlayIndexFC4 = 0;
        secondPayload->castListCA4[0] = BattleAction_Idle;
        secondPayload->castListIndexFC8 = BattleCastQueueIndex_BeforeFirst;
        secondPayload->castListCA4[1] = BattleCastQueueValue_End;
        // 第二对象仍从首位置取坐标；位置身份独立写入second。
        secondActor->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[first]][2]);
        secondActor->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[first]][3]);
        secondActor->x = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[first]][2];
        secondActor->y = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[first]][3];
        action->scaleX = 1.0F;
        action->scaleY = 1.0F;
        secondPayload->battlePosition18 = second;
        UpdateBattleAlbumFlag_4238F0(secondActor);
        const std::uint32_t secondFlags = secondActor->flags | YobiActionFlag_Hidden;
        secondActor->phase = BattleAction_Idle;
        secondActor->flags = secondFlags;
        secondActor->callback = nullptr;

        YobiAction* const carrierAction = NewYobiAction_404AE0(2, sizeof(BattleRideCarrierPayload), YobiPayloadKind::BattleRideCarrier);
        existingPayload->effect1060 = carrierAction;
        auto* const carrier = carrierAction->payload.battleRideCarrier;
        carrier->first00 = firstActor;
        carrier->second04 = secondActor;
        carrier->gearImage0C = gearImage;
        AppendBattleCast_4392A0(action, *originalSlot, BattleAction_DismountAndRestoreActors);
        AppendBattleEffect_439400(*originalSlot, {first, 0u, first, second});
        if ((*reinterpret_cast<const unsigned char*>(&firstPayload->flagsFDC) & BattleActorFlag_Dead) != 0) {
            firstActor->motionMagnitude54 = 4.0F;
            AppendBattleCast_4392A0(action, firstActor, BattleAction_ReturnToFormation);
            AppendBattleCast_4392A0(action, firstActor, BattleAction_FaceFormation);
            AppendBattleCast_4392A0(action, firstActor, BattleAction_HandleDeath);
        } else {
            firstActor->motionMagnitude54 = 5.0F;
            AppendBattleCast_4392A0(action, firstActor, BattleAction_PlayBeginRunFacingEffectTarget);
            AppendBattleCast_4392A0(action, firstActor, BattleAction_ReturnToFormation);
            AppendBattleCast_4392A0(action, firstActor, BattleAction_PlayEndRunAndRestoreFacing);
        }
        if ((*reinterpret_cast<const unsigned char*>(&secondPayload->flagsFDC) & BattleActorFlag_Dead) != 0) {
            secondActor->motionMagnitude54 = 4.0F;
            AppendBattleCast_4392A0(action, secondActor, BattleAction_ReturnToFormation);
            AppendBattleCast_4392A0(action, secondActor, BattleAction_FaceFormation);
            AppendBattleCast_4392A0(action, secondActor, BattleAction_HandleDeath);
        } else {
            secondActor->motionMagnitude54 = 5.0F;
            AppendBattleCast_4392A0(action, secondActor, BattleAction_PlayBeginRunFacingEffectTarget);
            AppendBattleCast_4392A0(action, secondActor, BattleAction_ReturnToFormation);
            AppendBattleCast_4392A0(action, secondActor, BattleAction_PlayEndRunAndRestoreFacing);
        }
        scene->packetOffset64 = previewOffset;
    }
}

// 原函数：0x00438450。
std::int32_t TakeBattleCInteger_438450()
{
    std::int32_t offset = cg::world::gBattleCOffset5ABA80;
    std::uint32_t result = 0;
    bool hasDigit = false;
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(cg::world::gBattleCText5A3A20[offset]));
        if (value == 0) {
            break;
        }
        if ((value >= '0' && value <= '9') || (value >= 'A' && value <= 'F')) {
            if (hasDigit) {
                result <<= 4;
            } else {
                hasDigit = true;
            }
            result += static_cast<std::uint32_t>(value - (value < 'A' ? '0' : '7'));
            ++offset;
        } else if (hasDigit || value == '|') {
            cg::world::gBattleCOffset5ABA80 = offset;
            ++offset;
            break;
        } else {
            ++offset;
        }
    }
    cg::world::gBattleCOffset5ABA80 = offset;
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x004384C0。
void TakeBattleCString_4384C0(char* const destination, const std::int32_t capacity)
{
    std::int32_t written = 0;
    for (;;) {
        const char byte = cg::world::gBattleCText5A3A20[cg::world::gBattleCOffset5ABA80];
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(byte));
        if (byte == '\0') {
            break;
        }
        if (byte == '|') {
            ++cg::world::gBattleCOffset5ABA80;
            break;
        }
        if (written >= capacity - 1) {
            if (value < 0) {
                ++cg::world::gBattleCOffset5ABA80;
            }
            ++cg::world::gBattleCOffset5ABA80;
            continue;
        }
        destination[written] = byte;
        if (value < 0) {
            const std::int32_t offset = cg::world::gBattleCOffset5ABA80;
            ++written;
            if (written >= capacity - 1) {
                cg::world::gBattleCOffset5ABA80 = offset + 1;
                ++cg::world::gBattleCOffset5ABA80;
                continue;
            }
            const char nextByte = cg::world::gBattleCText5A3A20[offset + 1];
            cg::world::gBattleCOffset5ABA80 = offset + 1;
            destination[written] = nextByte;
        }
        ++written;
        ++cg::world::gBattleCOffset5ABA80;
    }
    destination[written] = '\0';
    input::UnescapeLegacyTextAndNotify_52D2F0(destination);
}

} // namespace detail

// 原函数：0x00438550。
void ParseBattleC_438550()
{
    std::uint32_t present = 0;
    cg::world::gBattleCOffset5ABA80 = 2;
    const auto fieldFlags = static_cast<std::uint32_t>(detail::TakeBattleCInteger_438450());
    cg::world::gBattleFieldFlags5ABB58 = fieldFlags;
    if ((fieldFlags & detail::BattleFieldFlag_End) != 0) {
        return;
    }
    if ((fieldFlags & detail::BattleFieldFlag_Earth) != 0) {
        static_cast<void>(detail::NewBattleFieldEffect_41FFE0(110092));
    } else if ((fieldFlags & detail::BattleFieldFlag_Water) != 0) {
        static_cast<void>(detail::NewBattleFieldEffect_41FFE0(110089));
    } else if ((fieldFlags & detail::BattleFieldFlag_Fire) != 0) {
        static_cast<void>(detail::NewBattleFieldEffect_41FFE0(110090));
    } else if ((fieldFlags & detail::BattleFieldFlag_Wind) != 0) {
        static_cast<void>(detail::NewBattleFieldEffect_41FFE0(110091));
    } else if ((fieldFlags & detail::BattleFieldFlag_Silence) != 0) {
        static_cast<void>(detail::NewBattleFieldEffect_41FFE0(110155));
    } else {
        static_cast<void>(MarkDeath(cg::world::gBattleSceneAction5ABA94->payload.battleScene->fieldEffectB0));
        cg::world::gBattleSceneAction5ABA94->payload.battleScene->fieldEffectB0 = nullptr;
    }

    while (cg::world::gBattleCText5A3A20[cg::world::gBattleCOffset5ABA80] != '\0') {
        std::int32_t position = detail::TakeBattleCInteger_438450();
        if (position < 0 || position >= 22) {
            position = 0;
        }
        present |= 1u << position;
        YobiAction** const slot = &cg::world::gBattleAllEntries59E980[position];
        if (*slot == nullptr) {
            *slot = detail::NewBattleActor_438290(position);
            detail::ResetBattleCastList_4391E0(position);
        }
        YobiAction* const actor = *slot;
        auto* const payload = actor->payload.battleActor;
        char nameBytes[29]{};
        detail::TakeBattleCString_4384C0(nameBytes, 29);
        // 4384C0内部已反转义；原438673仍独立再次执行。
        input::UnescapeLegacyText_52D150(nameBytes);
        actor->nameC4 = cg::text::FromGbkWide(nameBytes);
        actor->imageNo = character_select::ResolveCharacterImageForVersion(detail::TakeBattleCInteger_438450());
        payload->baseImage13AC = character_select::ResolveCharacterImageForVersion(detail::TakeBattleCInteger_438450());
        actor->field1D0 = detail::TakeBattleCInteger_438450();
        {
            const std::int32_t value = detail::TakeBattleCInteger_438450();
            actor->readiness = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = detail::TakeBattleCInteger_438450();
            actor->maxHp = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = detail::TakeBattleCInteger_438450();
            actor->barValue1B0 = static_cast<std::int32_t>(value);
        }
        {
            const std::int32_t value = detail::TakeBattleCInteger_438450();
            actor->barMaximum1C0 = static_cast<std::int32_t>(value);
        }
        const auto inputFlags = static_cast<std::uint32_t>(detail::TakeBattleCInteger_438450());
        const std::int32_t ridingImage = character_select::ResolveCharacterImageForVersion(detail::TakeBattleCInteger_438450());
        static_cast<void>(detail::SetBattleRider_4222B0(actor, actor->imageNo, ridingImage));
        for (std::int32_t index = 0; index < 2; ++index) {
            const std::int32_t gearImage = character_select::ResolveCharacterImageForVersion(detail::TakeBattleCInteger_438450());
            const std::int32_t auxiliaryImage = character_select::ResolveCharacterImageForVersion(detail::TakeBattleCInteger_438450());
            const std::int32_t auxiliaryType = detail::TakeBattleCInteger_438450();
            detail::SetBattleGear_422C20(actor, gearImage, index);
            detail::SetBattleAuxGear_422C90(actor, auxiliaryImage, auxiliaryType, index);
        }
        const std::int32_t imageNo = actor->imageNo;
        actor->rebirthValue1 = 0;
        if (imageNo >= 110350 && imageNo <= 110373) {
            actor->rebirthValue1 = 4;
            actor->rebirthValue2 = 4;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Appear) != 0) {
            payload->flagsFDC &= ~detail::BattleActorFlag_EntryComplete;
            actor->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][0]);
            actor->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][1]);
        } else {
            const std::uint32_t flags = payload->flagsFDC | detail::BattleActorFlag_EntryComplete;
            payload->flagsFDC = flags;
            if ((flags & detail::BattleActorFlag_Dead) == 0) {
                actor->offset40 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][2]);
                actor->offset44 = static_cast<float>(cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[position]][3]);
                detail::FaceBattleActorTowardFormationPosition_406260(actor);
                if ((inputFlags & detail::BattleCharacterFlag_Dead) != 0) {
                    detail::FaceBattleActorTowardFormationPosition_406260(actor);
                    actor->characterAction = CharacterAnimation_Down;
                    while (detail::PlayFrame_517DA0(actor, AnimationPlayback_StopAtEnd) != AnimationFrameResult_StoppedOrUnavailable) {
                    }
                    static_cast<void>(detail::NewBattleCenteredEffect_41FA00(actor, 110169));
                }
            }
        }
        std::uint32_t deathFlags = payload->flagsFDC;
        std::uint32_t deathState = payload->stateFE0;
        if ((inputFlags & detail::BattleCharacterFlag_Dead) != 0) {
            deathFlags |= detail::BattleActorFlag_Dead;
            deathState |= detail::BattleRuntimeStateFlag_InactiveSkipReady;
        } else {
            deathFlags &= ~detail::BattleActorFlag_Dead;
            deathState &= ~detail::BattleRuntimeStateFlag_InactiveSkipReady;
        }
        payload->flagsFDC = deathFlags;
        std::uint32_t flags = payload->flagsFDC;
        payload->stateFE0 = deathState;
        flags = (inputFlags & detail::BattleCharacterFlag_Player) != 0 ? flags | detail::BattleActorFlag_Player : flags & ~detail::BattleActorFlag_Player;
        payload->flagsFDC = flags;
        flags = (inputFlags & detail::BattleCharacterFlag_Pet) != 0 ? flags | detail::BattleActorFlag_Pet : flags & ~detail::BattleActorFlag_Pet;
        payload->flagsFDC = flags;
        if ((inputFlags & detail::BattleCharacterFlag_Poison) != 0) {
            static_cast<void>(detail::NewBattleDebuffEffect_41F660(actor, detail::BattleStatusGraphic_Poison));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_Poison;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Sleep) != 0) {
            static_cast<void>(detail::NewBattleDebuffEffect_41F660(actor, detail::BattleStatusGraphic_Sleep));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_Sleep;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Stone) != 0) {
            static_cast<void>(detail::NewBattleDebuffEffect_41F660(actor, detail::BattleStatusGraphic_Stone));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_Stone;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Inebriety) != 0) {
            static_cast<void>(detail::NewBattleDebuffEffect_41F660(actor, detail::BattleStatusGraphic_Inebriety));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_Inebriety;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Confusion) != 0) {
            static_cast<void>(detail::NewBattleDebuffEffect_41F660(actor, detail::BattleStatusGraphic_Confusion));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_Confusion;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Forget) != 0) {
            static_cast<void>(detail::NewBattleDebuffEffect_41F660(actor, detail::BattleStatusGraphic_Forget));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_Forget;
        }
        const float scale = (inputFlags & detail::BattleCharacterFlag_DamageUp) != 0 ? 1.3F : 1.0F;
        actor->scaleX = scale;
        actor->scaleY = scale;
        if ((inputFlags & detail::BattleCharacterFlag_TwoActions) != 0) {
            static_cast<void>(detail::NewBattleAttachedEffect_41FBE0(actor, detail::BattleAttachedEffectMode_DisplayImage22326));
        } else {
            static_cast<void>(MarkDeath(actor->payload.battleActor->attachedEffect1058));
            actor->payload.battleActor->attachedEffect1058 = nullptr;
        }
        const std::uint32_t reverseFlags = payload->flagsFDC;
        if ((inputFlags & detail::BattleCharacterFlag_ReverseType) != 0) {
            payload->flagsFDC = reverseFlags | detail::BattleActorFlag_ReverseType;
            static_cast<void>(detail::NewBattleReverseEffect_41F860(actor, 110152));
        } else {
            YobiAction* const effect = payload->attributeReversalEffect105C;
            payload->flagsFDC = reverseFlags & ~detail::BattleActorFlag_ReverseType;
            static_cast<void>(MarkDeath(effect));
            payload->attributeReversalEffect105C = nullptr;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Morph) != 0) {
            payload->flagsFDC |= detail::BattleActorFlag_Morph;
        }
        // 三对状态位沿原清除掩码，不能按输入位对称化。
        if ((inputFlags & detail::BattleCharacterFlag_DefenseUp) != 0) {
            static_cast<void>(detail::NewBattleStatusEffect_41FE00(actor, detail::BattleStatusGraphic_DefenseUp, detail::BattleStatusEffectPlacement_Default));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_DefenseUp;
        }
        if ((inputFlags & detail::BattleCharacterFlag_DefenseDown) != 0) {
            static_cast<void>(detail::NewBattleStatusEffect_41FE00(actor, detail::BattleStatusGraphic_DefenseDown, detail::BattleStatusEffectPlacement_Default));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_DefenseUp;
        }
        if ((inputFlags & detail::BattleCharacterFlag_AttackUp) != 0) {
            static_cast<void>(detail::NewBattleStatusEffect_41FE00(actor, detail::BattleStatusGraphic_AttackUp, detail::BattleStatusEffectPlacement_Default));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_AttackUp;
        }
        if ((inputFlags & detail::BattleCharacterFlag_AttackDown) != 0) {
            static_cast<void>(detail::NewBattleStatusEffect_41FE00(actor, detail::BattleStatusGraphic_AttackDown, detail::BattleStatusEffectPlacement_Default));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_AttackUp;
        }
        if ((inputFlags & detail::BattleCharacterFlag_AgilityUp) != 0) {
            static_cast<void>(detail::NewBattleStatusEffect_41FE00(actor, detail::BattleStatusGraphic_AgilityUp, detail::BattleStatusEffectPlacement_Default));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_AgilityUp;
        }
        if ((inputFlags & detail::BattleCharacterFlag_AgilityDown) != 0) {
            static_cast<void>(detail::NewBattleStatusEffect_41FE00(actor, detail::BattleStatusGraphic_AgilityDown, detail::BattleStatusEffectPlacement_Default));
        } else {
            payload->flagsFDC &= ~detail::BattleActorFlag_AgilityUp;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Rebirth) != 0) {
            if (payload->rebirthEffectType1080 == 0) {
                if ((inputFlags & detail::BattleCharacterFlag_Earth) != 0) {
                    payload->rebirthEffectType1080 = 1;
                }
                if ((inputFlags & detail::BattleCharacterFlag_Water) != 0) {
                    payload->rebirthEffectType1080 = 2;
                }
                if ((inputFlags & detail::BattleCharacterFlag_Fire) != 0) {
                    payload->rebirthEffectType1080 = 3;
                }
                if ((inputFlags & detail::BattleCharacterFlag_Wind) != 0) {
                    payload->rebirthEffectType1080 = 4;
                }
                const std::int8_t type = payload->rebirthEffectType1080;
                if (type >= 1 && type <= 4) {
                    {
                        const std::int32_t image = detail::kRebirthPersistentImages4113EB[type - 1][0];
                        if (image != 0) {
                            YobiAction* const effect = detail::NewBattleOffsetFollowEffect_419CF0(actor, 0, 2, image);
                            payload->rebirthEffect106C = effect;
                            effect->rebirthValue1 = 4;
                            payload->rebirthEffect106C->rebirthValue2 = 4;
                        }
                    }
                    {
                        const std::int32_t image = detail::kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][1];
                        if (image != 0) {
                            YobiAction* const effect = detail::NewBattleOffsetFollowEffect_419CF0(actor, 0, 1, image);
                            payload->rebirthAuxiliaryEffects1074[0] = effect;
                            effect->rebirthValue1 = 4;
                            payload->rebirthAuxiliaryEffects1074[0]->rebirthValue2 = 4;
                        }
                    }
                    {
                        const std::int32_t image = detail::kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][2];
                        if (image != 0) {
                            YobiAction* const effect = detail::NewBattleOffsetFollowEffect_419CF0(actor, 0, -1, image);
                            payload->rebirthAuxiliaryEffects1074[1] = effect;
                            effect->rebirthValue1 = 4;
                            payload->rebirthAuxiliaryEffects1074[1]->rebirthValue2 = 4;
                        }
                    }
                    {
                        const std::int32_t image = detail::kRebirthPersistentImages4113EB[payload->rebirthEffectType1080 - 1][3];
                        if (image != 0) {
                            YobiAction* const effect = detail::NewBattleOffsetFollowEffect_419CF0(actor, 0, -2, image);
                            payload->rebirthAuxiliaryEffects1074[2] = effect;
                            effect->rebirthValue1 = 4;
                            payload->rebirthAuxiliaryEffects1074[2]->rebirthValue2 = 4;
                        }
                    }
                }
            }
            payload->flagsFDC |= detail::BattleActorFlag_Rebirth;
        } else {
            for (std::int32_t index = 0; index < 3; ++index) {
                YobiAction* const effect = payload->rebirthAuxiliaryEffects1074[index];
                if (effect != nullptr) {
                    static_cast<void>(MarkDeath(effect));
                    payload->rebirthAuxiliaryEffects1074[index] = nullptr;
                }
            }
            YobiAction* const effect = payload->rebirthEffect106C;
            if (effect != nullptr) {
                static_cast<void>(MarkDeath(effect));
                payload->rebirthEffect106C = nullptr;
            }
            const std::uint32_t rebirthFlags = payload->flagsFDC & ~detail::BattleActorFlag_Rebirth;
            payload->rebirthEffectType1080 = 0;
            payload->flagsFDC = rebirthFlags;
        }
        actor->fields288[3] = 0;
        actor->fields288[2] = 0;
        actor->fields288[1] = 0;
        actor->fields288[0] = 0;
        if ((inputFlags & detail::BattleCharacterFlag_Earth) != 0) {
            actor->fields288[0] = 1;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Water) != 0) {
            actor->fields288[1] = 1;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Fire) != 0) {
            actor->fields288[2] = 1;
        }
        if ((inputFlags & detail::BattleCharacterFlag_Wind) != 0) {
            actor->fields288[3] = 1;
        }
        UpdateBattleAlbumFlag_4238F0(actor);
        static_cast<void>(detail::PlayFrame_517DA0(actor, AnimationPlayback_Loop));
    }

    for (std::int32_t index = 0; index < 22; ++index) {
        if ((present & (1u << index)) != 0) {
            continue;
        }
        YobiAction** const slot = &cg::world::gBattleAllEntries59E980[index];
        YobiAction* const actor = *slot;
        if (actor == nullptr) {
            continue;
        }
        auto* const payload = actor->payload.battleActor;
        static_cast<void>(MarkDeath(payload->debuffEffectAction_1054));
        static_cast<void>(MarkDeath(payload->attachedEffect1058));
        static_cast<void>(MarkDeath(payload->attributeReversalEffect105C));
        static_cast<void>(MarkDeath(payload->effect1060));
        static_cast<void>(MarkDeath(payload->centeredEffect1064));
        static_cast<void>(MarkDeath(payload->statusEffect1068));
        static_cast<void>(MarkDeath(payload->rebirthEffect106C));
        static_cast<void>(MarkDeath(payload->rebirthAuxiliaryEffects1074[0]));
        static_cast<void>(MarkDeath(payload->rebirthAuxiliaryEffects1074[1]));
        static_cast<void>(MarkDeath(payload->rebirthAuxiliaryEffects1074[2]));
        static_cast<void>(MarkDeath(payload->gear13B4));
        static_cast<void>(MarkDeath(payload->auxiliaryGear13B8[0]));
        static_cast<void>(MarkDeath(payload->auxiliaryGear13B8[1]));
        static_cast<void>(MarkDeath(payload->rider13C8));
        static_cast<void>(MarkDeath(*slot));
        *slot = nullptr;
    }
}

// BEMS01：固定入口payload、各次取包fresh场景指针；仅CHT/BST消费，见doc/cg2d_battle_exit_message_scan_spec.md。
// 原函数：0x0043BAA0。
std::int32_t ConsumeBattleExitMessages_43BAA0()
{
    auto* const payload = cg::world::gBattleSceneAction5ABA94->payload.battleScene;
    std::array<char, 4> header;
    std::array<char, 256> message;
    for (;;) {
        if (detail::GetBattleMHeader_439070(header.data()) == 1) {
            return 1;
        }
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(detail::kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), detail::kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind != detail::BattleMHeader_CHT && kind != detail::BattleMHeader_BST) {
            continue;
        }
        payload->packetOffset64 = cg::world::gBattleMScanOffset5A8A3C;
        if (kind == detail::BattleMHeader_CHT) { // CHT
            static_cast<void>(detail::GetBattlePosition_4237E0(detail::TakeBattleMInteger_438F70(cg::world::gBattleSceneAction5ABA94)));
            const std::int32_t color = detail::TakeBattleMInteger_438F70(cg::world::gBattleSceneAction5ABA94);
            const std::int32_t font = detail::TakeBattleMInteger_438F70(cg::world::gBattleSceneAction5ABA94);
            payload->messageValueA4 = detail::TakeBattleMInteger_438F70(cg::world::gBattleSceneAction5ABA94);
            detail::TakeBattleMString_438FE0(cg::world::gBattleSceneAction5ABA94, message.data(), 256);
            if (platform::gChatNotificationSoundEnabled_554644 != 0) {
                static_cast<void>(audio::PlaySe_52C450(74, 320));
            }
            cg::world::AppendChatMessage_44B050(cg::text::FromGbkWide(message.data()), color, font);
        } else { // BST
            const std::int32_t position = detail::GetBattlePosition_4237E0(
                detail::TakeBattleMInteger_438F70(cg::world::gBattleSceneAction5ABA94));
            const std::int32_t seconds = detail::TakeBattleMInteger_438F70(cg::world::gBattleSceneAction5ABA94);
            if (position == cg::world::gBattleCurrentPosition5ABCA4) {
                cg::world::ConsumeBattlePlayerTimer(seconds);
            }
        }
    }
}

} // namespace cg::yobi
