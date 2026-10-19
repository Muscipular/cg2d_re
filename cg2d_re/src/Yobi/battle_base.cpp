#include "battle_base.hpp"
#include "battle_skill_effect.hpp"
#include "base.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"

#include "battle_attachment.hpp"
#include "battle_display.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"
#include "../world/WorldMapState.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>

namespace cg::yobi {

namespace detail {

// 4390E0/4391E0仅共享此写序；各自队列尾、技能及朝向处理保持独立。
static void ResetBattleCastEffects(BattleActorPayload* const payload)
{
    payload->valueFD4 = 0;
    payload->effectFlagsFE4 = 0;
    payload->valueFF0 = 0;
    payload->aliveFF8 = 0;
    payload->value1024 = 0;
    payload->value1038 = 0;
    payload->valueFCC = 0;
    payload->valueFD0 = 0;
    payload->effectRunningC9C = 0;
    payload->effectCountCA0 = 0;
    payload->effects1C[0].position = -1;
    payload->effects1C[1].position = -1;
    for (BattleEntryEffectRecord& effect : payload->effects1C) {
        effect.position = -1;
    }
}

// 原43BE00的20B节点；仅两条链指针拓宽，43BED0负责按原序释放。
struct BattleAppendEffectNode {
    std::int32_t skill;
    std::int32_t technique;
    std::int32_t position;
    BattleAppendEffectNode* previous;
    BattleAppendEffectNode* next;
};

static BattleAppendEffectNode* gBattleAppendEffects5ABA8C = nullptr; // 原全局：0x005ABA8C。

static std::int32_t gDrawYobiCount59E578 = 0; // 原全局：0x0059E578。

static std::array<YobiAction*, 257> gDrawYobiList59E57C {}; // 原全局：0x0059E57C。 原257地址槽，非拥有T*；不清排序范围外尾部。

// 原函数：0x00404D90。
std::int32_t SelectIntValue_404D90(
    const std::int32_t* const values, const std::int32_t count, const std::int32_t index)
{
    if (count <= 0 || index < 0 || index >= count) {
        return 0;
    }
    return values[index];
}

// 原函数：0x0043B7F0。
void AppendBattleQueuedTarget_43B7F0(YobiAction* const action, const std::int32_t value)
{
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.battleActor;
    payload->queuedTargets1084[payload->queuedTargetCount13A8] = value;
    payload->queuedTargets1084[payload->queuedTargetCount13A8 + 1] = -1;
    ++payload->queuedTargetCount13A8;
}

// B22H01：原五个S32阈值按序计数、1..15外返回-1；完整分档合同见回旋命中SPEC。
// 原函数：0x0043BF00。
std::int32_t GetBattleValueBand_43BF00(const std::int32_t value)
{
    if (value < 1 || value > 15) {
        return -1;
    }
    std::int32_t result = 0;
    for (const std::int32_t threshold : world::gBattleValueThresholds54D93C) {
        if (value < threshold) {
            break;
        }
        ++result;
    }
    return result < 1 ? -1 : result;
}

// 原函数：0x004237E0。
std::int32_t GetBattlePosition_4237E0(const std::int32_t position)
{
    if (position == 50) {
        return 20;
    }
    if (position == 51) {
        return 21;
    }
    return position;
}

// 原43BD00按S32技能号返回AL 0/1；54项集合及生产门禁见追加效果链SPEC。
std::uint8_t IsBattleAppendEffectSkill_43BD00(const std::int32_t skill)
{
    switch (skill) {
    case 8:
    case 11:
    case 2006:
    case 2019:
    case 2027:
        return 1;
    default:
        break;
    }
    return (skill >= 55 && skill <= 60)
        || (skill >= 64 && skill <= 66)
        || (skill >= 2010 && skill <= 2012)
        || (skill >= 2014 && skill <= 2015)
        || (skill >= 2050 && skill <= 2052)
        || (skill >= 2058 && skill <= 2067)
        || (skill >= 2089 && skill <= 2095)
        || (skill >= 3002 && skill <= 3016) ? 1 : 0;
}

// 原函数：0x0043BE00。
void AddBattleAppendEffect_43BE00(
    const std::int32_t skill, const std::int32_t technique, const std::int32_t position)
{
    if (IsBattleAppendEffectSkill_43BD00(skill) == 0) {
        return;
    }
    BattleAppendEffectNode* node = gBattleAppendEffects5ABA8C;
    if (node == nullptr) {
        auto* const created = static_cast<BattleAppendEffectNode*>(std::malloc(sizeof(BattleAppendEffectNode)));
        created->skill = skill;
        created->next = nullptr;
        created->previous = nullptr;
        created->position = position;
        created->technique = technique;
        gBattleAppendEffects5ABA8C = created;
        return;
    }
    for (;;) {
        if (node->position == position && node->skill == skill) {
            node->technique = technique;
            return;
        }
        BattleAppendEffectNode* const previous = node;
        node = node->next;
        if (node == nullptr) {
            auto* const created = static_cast<BattleAppendEffectNode*>(std::malloc(sizeof(BattleAppendEffectNode)));
            created->next = nullptr;
            created->skill = skill;
            created->position = position;
            created->technique = technique;
            created->previous = previous;
            previous->next = created;
            return;
        }
    }
}

// 原函数：0x0043BEA0。
std::int32_t FindBattleAppendEffectTechnique_43BEA0(
    const std::int32_t skill, const std::int32_t position)
{
    const BattleAppendEffectNode* node = gBattleAppendEffects5ABA8C;
    while (node != nullptr) {
        if (node->position == position && node->skill == skill) {
            return node->technique;
        }
        node = node->next;
    }
    return -1;
}

// 原函数：0x00405BD0。
static std::int32_t CompareBattleDrawActions_405BD0(
    YobiAction* const* const left, YobiAction* const* const right)
{
    YobiAction* const first = *left;
    YobiAction* const second = *right;
    const std::uint32_t firstFlags = first->flags;
    std::int32_t firstY = first->y;
    std::int32_t secondY = second->y;
    if ((firstFlags & 0x800000u) != 0) {
        firstY = first->deltaY;
    }
    if ((second->flags & 0x800000u) != 0) {
        secondY = second->deltaY;
    }
    if (firstY > secondY) {
        return 1;
    }
    if (firstY < secondY) {
        return -1;
    }
    const auto firstOrder = std::bit_cast<std::int16_t>(first->pathCountOrDrawOrder_270);
    const auto secondOrder = std::bit_cast<std::int16_t>(second->pathCountOrDrawOrder_270);
    return (firstOrder > secondOrder) - (firstOrder < secondOrder);
}

// 原函数：0x00405CB0。
void UpdateBattleDirectionFromAngle_405CB0(YobiAction* const action)
{
    if (action->angle50 >= 360.0F) {
        action->angle50 = static_cast<float>(action->angle50 - 360.0);
    } else if (action->angle50 < 0.0F) {
        action->angle50 = static_cast<float>(action->angle50 + 360.0);
    }
    if (action->angle50 >= 15.0F && action->angle50 < 60.0F) {
        action->direction = 4;
    }
    if (action->angle50 >= 60.0F && action->angle50 < 120.0F) {
        action->direction = 5;
    }
    if (action->angle50 >= 120.0F && action->angle50 < 165.0F) {
        action->direction = 6;
    }
    if (action->angle50 >= 165.0F && action->angle50 < 195.0F) {
        action->direction = 7;
    }
    if (action->angle50 >= 195.0F && action->angle50 < 240.0F) {
        action->direction = 0;
    }
    if (action->angle50 >= 240.0F && action->angle50 < 300.0F) {
        action->direction = 1;
    }
    if (action->angle50 >= 300.0F && action->angle50 < 345.0F) {
        action->direction = 2;
    }
    if ((action->angle50 >= 345.0F && action->angle50 < 360.0F)
        || (action->angle50 >= 0.0F && action->angle50 < 15.0F)) {
        action->direction = 3;
    }
}

// BSTL01：首角度比较上限、分支前第二角度持样，负分支第三次重取；仅修一圈。
// 原函数：0x00405E60。
void NormalizeBattleAngle_405E60(YobiAction* const action)
{
    const float upperAngle = action->angle50;
    const float currentAngle = action->angle50;
    if (upperAngle >= 360.0F) {
        action->angle50 = static_cast<float>(currentAngle - 360.0);
    } else if (!(currentAngle >= 0.0F)) {
        action->angle50 = static_cast<float>(action->angle50 + 360.0);
    }
}

// 原函数：0x00405EA0。
void MoveBattleByAngle_405EA0(YobiAction* const action)
{
    const double xFactor = world::LookupCosineDegrees_46F950(action->angle50);
    const double xStep = xFactor * action->value54;
    const float yAngle = action->angle50;
    action->offset40 = static_cast<float>(xStep + action->offset40);
    const double yFactor = world::LookupSineDegrees_46F930(yAngle);
    action->offset44 = static_cast<float>(yFactor * action->value54 + action->offset44);
}

// 原函数：0x00405ED0。
std::int8_t GetBattleRelativeDirection_405ED0(
    const float sourceX, const float sourceY, const float targetX, const float targetY,
    const float tolerance)
{
    using world::gBattleGraphicPoints553758;
    const std::int32_t referenceY = cg::world::gBattleGraphicPoints553758[1][3] - cg::world::gBattleGraphicPoints553758[0][3];
    const std::int32_t referenceX = cg::world::gBattleGraphicPoints553758[1][2] - cg::world::gBattleGraphicPoints553758[0][2];
    const float referenceDeltaY = static_cast<float>(referenceY);
    const float referenceDeltaX = static_cast<float>(referenceX);
    const double referenceAngle = world::GetVectorAngleDegrees_46F970(referenceDeltaX, referenceDeltaY);
    float reference = static_cast<float>(referenceAngle);
    if (referenceAngle > 180.0) {
        reference = static_cast<float>(reference - 360.0);
    } else if (!(reference > -180.0F)) {
        reference = static_cast<float>(reference + 360.0);
    }
    const float deltaY = static_cast<float>(static_cast<double>(targetY) - sourceY);
    const float deltaX = static_cast<float>(static_cast<double>(targetX) - sourceX);
    double difference = world::GetVectorAngleDegrees_46F970(deltaX, deltaY);
    if (!(reference >= 0.0F)) {
        difference -= reference + 360.0;
    }
    // 原405F85在负基准分支之后仍再次相减，不合并为通常的单次相对角。
    difference -= reference;
    if (difference > 180.0) {
        difference -= 360.0;
    } else if (!(difference > -180.0)) {
        difference += 360.0;
    }
    const double limit = tolerance;
    if (!(-limit > difference) && !(difference > limit)) {
        return 0;
    }
    if (!(180.0 - limit > difference) && !(difference > limit + 180.0)) {
        return 0;
    }
    if (!(-180.0 - limit > difference) && !(difference > limit - 180.0)) {
        return 0;
    }
    return (!(reference >= 0.0F) != !(difference >= 0.0)) ? 1 : -1;
}

// 原函数：0x00406060。
double GetBattleAngleDifference_406060(YobiAction* const source, YobiAction* const target)
{
    if (source == nullptr || target == nullptr) {
        return 0.0;
    }
    const float deltaY = static_cast<float>(static_cast<double>(target->offset44) - source->offset44);
    const float deltaX = static_cast<float>(static_cast<double>(target->offset40) - source->offset40);
    const double targetAngle = world::GetVectorAngleDegrees_46F970(deltaX, deltaY);
    const double difference = targetAngle - source->angle50;
    if (difference > 180.0) {
        return difference - 360.0;
    }
    if (!(difference > -180.0)) {
        return difference + 360.0;
    }
    return difference;
}

// 原函数：0x004060D0。
void TurnBattleTowardPosition_4060D0(
    YobiAction* const action, const float x, const float y,
    const std::int32_t updateDirection, const float step)
{
    const float deltaY = static_cast<float>(static_cast<double>(y) - action->offset44);
    const float deltaX = static_cast<float>(static_cast<double>(x) - action->offset40);
    if (step == 0.0F || std::isnan(step)) {
        action->angle50 = static_cast<float>(world::GetVectorAngleDegrees_46F970(deltaX, deltaY));
    } else {
        double difference = world::GetVectorAngleDegrees_46F970(deltaX, deltaY) - action->angle50;
        if (difference > 180.0) {
            difference -= 360.0;
        } else if (!(difference > -180.0)) {
            difference += 360.0;
        }
        const std::int32_t magnitude = std::abs(static_cast<std::int32_t>(difference));
        if (static_cast<double>(magnitude) >= step) {
            if (difference < 0.0) {
                action->angle50 = static_cast<float>(static_cast<double>(action->angle50) - step);
            } else {
                action->angle50 = static_cast<float>(static_cast<double>(step) + action->angle50);
            }
        } else {
            action->angle50 = static_cast<float>(difference + action->angle50);
        }
    }
    if (updateDirection == 1) {
        UpdateBattleDirectionFromAngle_405CB0(action);
    }
}

// 原函数：0x00406190。
void TurnBattleToward_406190(
    YobiAction* const action, YobiAction* const target,
    const std::int32_t updateDirection, const float step)
{
    if (action == nullptr || target == nullptr) {
        return;
    }
    const float y = static_cast<float>(static_cast<double>(target->offset44) - action->offset44);
    const float x = static_cast<float>(static_cast<double>(target->offset40) - action->offset40);
    if (step == 0.0F || std::isnan(step)) {
        action->angle50 = static_cast<float>(world::GetVectorAngleDegrees_46F970(x, y));
    } else {
        double difference = world::GetVectorAngleDegrees_46F970(x, y) - action->angle50;
        if (difference > 180.0) {
            difference -= 360.0;
        } else if (!(difference > -180.0)) {
            difference += 360.0;
        }
        const std::int32_t magnitude = std::abs(static_cast<std::int32_t>(difference));
        if (static_cast<double>(magnitude) >= step) {
            if (difference < 0.0) {
                action->angle50 = static_cast<float>(static_cast<double>(action->angle50) - step);
            } else {
                action->angle50 = static_cast<float>(static_cast<double>(step) + action->angle50);
            }
        } else {
            action->angle50 = static_cast<float>(difference + action->angle50);
        }
    }
    if (updateDirection == 1) {
        UpdateBattleDirectionFromAngle_405CB0(action);
    }
}

// BRCP01：先X后Y求差，Y平方/X平方和先存F32；原零门禁与正常S32转换。
// 原函数：0x004062A0。
std::int32_t GetBattleDistanceToPosition_4062A0(YobiAction* const action, const float x, const float y)
{
    if (action == nullptr) {
        return 0;
    }
    const double deltaX = static_cast<double>(x) - action->offset40;
    const double deltaY = static_cast<double>(y) - action->offset44;
    const float squared = static_cast<float>(deltaY * deltaY + deltaX * deltaX);
    if (!(squared > 0.0F)) {
        return 0;
    }
    return static_cast<std::int32_t>(std::sqrt(static_cast<double>(squared)));
}

// 原函数：0x00406300。
std::int32_t GetBattleDistance_406300(YobiAction* const first, YobiAction* const second)
{
    if (first == nullptr || second == nullptr) {
        return 0;
    }
    const double y = static_cast<double>(second->offset44) - first->offset44;
    const double x = static_cast<double>(second->offset40) - first->offset40;
    const float squared = static_cast<float>(x * x + y * y);
    if (!(squared > 0.0F)) {
        return 0;
    }
    return static_cast<std::int32_t>(std::sqrt(static_cast<double>(squared)));
}

// BHRC01：唯一22槽按序fresh T*，非空载荷FF8完整S32严格==1才计数。
// 原函数：0x00406360。
std::int32_t CountBattleActiveEntries_406360()
{
    std::int32_t count = 0;
    for (YobiAction* const action : world::gBattleAllEntries59E980) {
        if (action != nullptr && action->payload.battleActor->aliveFF8 == 1) {
            ++count;
        }
    }
    return count;
}

// 原函数：0x004065A0。
std::int32_t SeedBattleRandom_4065A0(const std::int32_t value)
{
    std::int32_t count = 0;
    for (YobiAction* const action : world::gBattleEntries59E980) {
        if (action != nullptr) {
            ++count;
        }
    }
    const auto length = static_cast<std::int32_t>(std::strlen(world::gBattleMText5AAA7C.data()));
    const std::int32_t seed = length * count * value;
    static_cast<void>(platform::SeedRandom_53B4BF(static_cast<std::uint32_t>(seed)));
    return seed;
}

// 原函数：0x00406260。
void FaceBattleActorTowardFormationPosition_406260(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const float y = static_cast<float>(static_cast<double>(payload->y04) - payload->y0C);
    const float x = static_cast<float>(static_cast<double>(payload->x00) - payload->x08);
    action->angle50 = static_cast<float>(world::GetVectorAngleDegrees_46F970(x, y));
    UpdateBattleDirectionFromAngle_405CB0(action);
}

// 原函数：0x004390E0。
void InitBattleCastList_4390E0(const std::int32_t index)
{
    using world::gBattleAllEntries59E980;
    YobiAction* const action = cg::world::gBattleAllEntries59E980[index];
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.battleActor;
    if ((*reinterpret_cast<const unsigned char*>(&payload->flagsFDC) & 2u) != 0 || action->phase == 13) {
        return;
    }
    action->value54 = 1.0F;
    cg::world::gBattleAllEntries59E980[index]->hidden = 0;
    ResetBattleCastEffects(payload);
    payload->castPlayIndexFC4 = 0;
    payload->castListCA4[0] = 0;
    payload->value13A4 = 0;
    payload->queuedTargetCount13A8 = 0;
    const std::uint8_t flags = *reinterpret_cast<const unsigned char*>(&payload->flagsFDC);
    payload->castListIndexFC8 = -1;
    payload->castListCA4[1] = -1;
    payload->queuedTargets1084[0] = -1;
    payload->queuedTargets1084[1] = -1;
    payload->castSkill103C = -1;
    payload->castSkillLevel1044 = -1;
    payload->skillMeta = {}; // E01／4391B6：每次重新初始化动作清除上次技能。
    if ((flags & 2u) == 0
        && (*reinterpret_cast<const unsigned char*>(&world::gBattlePlayerFlags5ABCAC) & 3u) != 0) {
        FaceBattleActorTowardFormationPosition_406260(cg::world::gBattleAllEntries59E980[index]);
    }
}

// 原函数：0x004391E0。
void ResetBattleCastList_4391E0(const std::int32_t index)
{
    using world::gBattleAllEntries59E980;
    YobiAction* const action = cg::world::gBattleAllEntries59E980[index];
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.battleActor;
    action->value54 = 1.0F;
    cg::world::gBattleAllEntries59E980[index]->hidden = 0;
    ResetBattleCastEffects(payload);
    payload->castListIndexFC8 = -1;
    payload->castListCA4[1] = -1;
    payload->queuedTargets1084[0] = -1;
    payload->queuedTargets1084[1] = -1;
    payload->castPlayIndexFC4 = 0;
    payload->castListCA4[0] = 0;
    payload->value13A4 = 0;
    payload->queuedTargetCount13A8 = 0;
}

// 原函数：0x004392A0。
void AppendBattleCast_4392A0(
    YobiAction* const caster, YobiAction* const actor, const std::int32_t state)
{
    if (actor == nullptr) {
        return;
    }
    auto* const payload = actor->payload.battleActor;
    payload->caster1070 = caster;
    const std::int32_t next = payload->castListIndexFC8 + 1;
    payload->castListIndexFC8 = next;
    payload->castListCA4[next] = state;
    payload->castListCA4[payload->castListIndexFC8 + 1] = -1;
    payload->busyFD8 = 1;
}

// 原函数：0x004392F0。
void AppendBattleCastAt_4392F0(
    YobiAction* const caster, YobiAction* const actor,
    const std::int32_t state, const std::int32_t index)
{
    if (actor == nullptr) {
        return;
    }
    auto* const payload = actor->payload.battleActor;
    payload->caster1070 = caster;
    const std::int32_t next = index + 1;
    payload->castListIndexFC8 = next;
    payload->castListCA4[next] = state;
    payload->castListCA4[payload->castListIndexFC8 + 1] = -1;
    payload->busyFD8 = 1;
}

// 原函数：0x00439340。
void AdvanceBattleCastState_439340(YobiAction* const action)
{
    AdvanceLuaSkillEffect(action); // E10：未受接管的对象直接执行原推进。
}

void AdvanceNativeBattleCastState(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.battleActor;
    const std::int32_t next = payload->castPlayIndexFC4 + 1;
    const bool completed = next >= 200;
    payload->castPlayIndexFC4 = next;
    if (completed) {
        action->phase = 236;
    } else {
        const std::int32_t state = payload->castListCA4[next];
        action->phase = state == -1 ? 236 : state;
    }
}

// 原函数：0x004393A0。
void AppendBattleEffectList_4393A0(
    YobiAction* const action, const std::int32_t position,
    const std::uint32_t flags, const std::int32_t position3)
{
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.battleActor;
    payload->effects1C[payload->effectCountCA0].position = position;
    payload->effects1C[payload->effectCountCA0].flags = flags;
    payload->effects1C[payload->effectCountCA0].position3 = position3;
    payload->effects1C[payload->effectCountCA0 + 1].position = -1;
    ++payload->effectCountCA0;
}

// 原函数：0x00439400。
void AppendBattleEffect_439400(YobiAction* const action, const BattleEntryEffectRecord effect)
{
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.battleActor;
    payload->effects1C[payload->effectCountCA0].position = effect.position;
    payload->effects1C[payload->effectCountCA0].flags = effect.flags;
    payload->effects1C[payload->effectCountCA0].position3 = effect.position3;
    payload->effects1C[payload->effectCountCA0].position4 = effect.position4;
    payload->effects1C[payload->effectCountCA0 + 1].position = -1;
    ++payload->effectCountCA0;
}

// 原函数：0x0043BC10。
void AppendBattleAuxEffects_43BC10(
    YobiAction* const action, const std::int32_t position, const std::uint32_t flags,
    const std::int32_t value3, const std::int32_t value4, const std::int32_t value5)
{
    if ((flags & 0x800000u) == 0) {
        return;
    }
    auto* const payload = action->payload.battleActor;
    const std::int32_t previous = payload->effectCountCA0 - 1;
    AppendBattleEffect_439400(action, {position, flags, value3, value4});
    AppendBattleEffect_439400(action, {position, flags, value5, 0});
    BattleEntryEffectRecord original;
    original.position4 = payload->effects1C[previous].position4;
    original.flags = payload->effects1C[previous].flags;
    original.position3 = payload->effects1C[previous].position3;
    original.position = payload->effects1C[previous].position;
    AppendBattleEffect_439400(action, original);
}

// 原函数：0x0043B830。
std::int32_t AdvanceBattleQueuedTarget_43B830(YobiAction* const action)
{
    if (action == nullptr) {
        return 0;
    }
    auto* const payload = action->payload.battleActor;
    const std::int32_t next = payload->value13A4 + 1;
    payload->value13A4 = next;
    return payload->queuedTargets1084[next] == -1 ? 1 : 0;
}

// 原函数：0x00439470。
std::int32_t AdvanceBattleEffect_439470(YobiAction* const action)
{
    if (action == nullptr) {
        return 0;
    }
    auto* const payload = action->payload.battleActor;
    const std::int32_t next = payload->effectRunningC9C + 1;
    payload->effectRunningC9C = next;
    return payload->effects1C[next].position == -1 ? 1 : 0;
}

// 原函数：0x0043BC90。
std::int32_t TakeBattleAuxEffects_43BC90(
    YobiAction* const action, std::int32_t* const position, std::int32_t* const value3,
    std::int32_t* const value4, std::int32_t* const value5)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    AdvanceBattleEffect_439470(action);
    // 输出可能别名载荷游标，保留每次写入后的独立读取。
    *position = payload->effects1C[payload->effectRunningC9C].position;
    *value3 = payload->effects1C[payload->effectRunningC9C].position3;
    *value4 = payload->effects1C[payload->effectRunningC9C].position4;
    AdvanceBattleEffect_439470(action);
    *value5 = payload->effects1C[payload->effectRunningC9C].position3;
    return AdvanceBattleEffect_439470(action);
}

// 原函数：0x00406390。
void ApplyBattleEffectFlags_406390(const std::int32_t sourcePosition, YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const std::int32_t index = payload->effectRunningC9C;
    const std::uint32_t flags = payload->effects1C[index].flags;
    const std::int32_t position = payload->effects1C[index].position;
    const std::int32_t recipient = (flags & 0x2000u) != 0 ? sourcePosition : position;
    if ((flags & 0x200000u) != 0) {
        static_cast<void>(NewBattleFloatingIcon_41F4C0(cg::world::gBattleAllEntries59E980[recipient], 22329, 0, 0));
    }
    if ((flags & 0x400000u) != 0) {
        static_cast<void>(NewBattleFloatingIcon_41F4C0(cg::world::gBattleAllEntries59E980[recipient], 22330, 0, 0));
    }
    if ((flags & 0x800000u) != 0) {
        std::int32_t effectPosition;
        std::int32_t imageNo;
        std::int32_t ridingImageNo;
        std::int32_t gearImageNo;
        static_cast<void>(TakeBattleAuxEffects_43BC90(
            action, &effectPosition, &imageNo, &ridingImageNo, &gearImageNo));
        const std::int32_t auxiliaryRecipient = (flags & 0x2000u) != 0 ? sourcePosition : effectPosition;
        static_cast<void>(NewBattleFloatingIcon_41F4C0(
            cg::world::gBattleAllEntries59E980[auxiliaryRecipient], 22328, 0, 0));
        static_cast<void>(audio::PlaySe_52C450(214, cg::world::gBattleAllEntries59E980[auxiliaryRecipient]->x));
        const std::int32_t riderSecondImage = ridingImageNo;
        const std::int32_t riderFirstImage = imageNo;
        YobiAction* const riderTarget = cg::world::gBattleAllEntries59E980[auxiliaryRecipient];
        static_cast<void>(SetBattleRider_4222B0(riderTarget, riderFirstImage, riderSecondImage));
        const std::int32_t gearImage = gearImageNo;
        YobiAction* const gearTarget = cg::world::gBattleAllEntries59E980[auxiliaryRecipient];
        SetBattleGear_422BC0(gearTarget, gearImage);
    }
    // 附加读取已经推进游标，末两块仍消费入口快照，不刷新flags或position。
    if ((flags & 0x1000000u) != 0) {
        static_cast<void>(NewBattleFloatingIcon_41F4C0(cg::world::gBattleAllEntries59E980[recipient], 22330, 0, 0));
    }
    if ((flags & 0x2000000u) != 0) {
        static_cast<void>(NewBattleFloatingIcon_41F4C0(cg::world::gBattleAllEntries59E980[recipient], 22328, 0, 0));
        static_cast<void>(audio::PlaySe_52C450(214, cg::world::gBattleAllEntries59E980[recipient]->x));
    }
}

// 原函数：0x00416DB0。
void ApplyBattleCastEffects_416DB0(YobiAction* const action)
{
    const std::int32_t position = static_cast<const BattleActorPayload*>(action->payload.battleActor)->battlePosition18;
    ApplyBattleEffectFlags_406390(position, action);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x0040CAC0。
void AdvanceBattleLinkedCast_40CAC0(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    YobiAction* const target = world::gBattleAllEntries59E980[position];
    AdvanceBattleCastState_439340(target);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x0040CAF0。
void AdvanceBattleEffectAndCast_40CAF0(YobiAction* const action)
{
    AdvanceBattleEffect_439470(action);
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00439380。
void MoveBattleCastState_439380(YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    BindBattleSkillMeta(action); // E01／E10：绑定完成后再发布当前动作。
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    action->phase = payload->castListCA4[payload->castPlayIndexFC4];
}

// 原函数：0x004394A0。
std::int32_t CheckBattleTurnReady_4394A0(
    YobiAction* const parent, const std::int32_t enable)
{
    auto* const payload = parent->payload.battleScene;
    for (std::size_t index = 0; index < cg::world::gBattleAllEntries59E980.size(); ++index) {
        YobiAction* const actor = cg::world::gBattleAllEntries59E980[index];
        if (actor == nullptr || (payload->entryMaskA8 & (1u << index)) == 0) {
            continue;
        }
        const std::int32_t parentPhase = parent->phase;
        const auto* const actorPayload = static_cast<const BattleActorPayload*>(actor->payload.battleActor);
        if (parentPhase != 24 && enable == 0
            && (*reinterpret_cast<const unsigned char*>(&actorPayload->stateFE0) & 2u) != 0
            && actorPayload->aliveFF8 == 0 && actor->phase != 27) {
            continue;
        }
        if (actorPayload->busyFD8 == 1) {
            const std::int32_t count = payload->waitCountAC + 1;
            payload->waitCountAC = count;
            if (count > 1200) {
                for (std::size_t slot = 0; slot < cg::world::gBattleAllEntries59E980.size(); ++slot) {
                    if (cg::world::gBattleAllEntries59E980[slot] != nullptr
                        && (payload->entryMaskA8 & (1u << slot)) != 0) {
                        ResetBattleCastList_4391E0(static_cast<std::int32_t>(slot));
                        AppendBattleCast_4392A0(parent, cg::world::gBattleAllEntries59E980[slot], 11);
                        AppendBattleCast_4392A0(parent, cg::world::gBattleAllEntries59E980[slot], 40);
                        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[slot]);
                        payload->waitCountAC = 0;
                    }
                }
            }
            return 0;
        }
        if (parentPhase != 24) {
            const auto& point = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[index]];
            const double expectedX = point[2];
            const double actualX = actor->offset40;
            bool displaced = expectedX < actualX || expectedX > actualX;
            if (!displaced) {
                const double expectedY = point[3];
                const double actualY = actor->offset44;
                displaced = expectedY < actualY || expectedY > actualY;
            }
            if (displaced) {
                AppendBattleCast_4392A0(parent, cg::world::gBattleAllEntries59E980[index], 11);
                AppendBattleCast_4392A0(parent, cg::world::gBattleAllEntries59E980[index], 40);
                MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[index]);
                return 0;
            }
        }
    }
    return 1;
}

} // namespace detail

// 原函数：0x0043BED0。
void ClearBattleAppendEffects_43BED0()
{
    detail::BattleAppendEffectNode* node = detail::gBattleAppendEffects5ABA8C;
    while (node != nullptr) {
        detail::BattleAppendEffectNode* const current = node;
        detail::gBattleAppendEffects5ABA8C = current;
        node = current->next;
        std::free(current);
    }
    detail::gBattleAppendEffects5ABA8C = nullptr;
}

// 原函数：0x00405C30。
std::int32_t SortBattleActions_405C30()
{
    YobiAction* const start = detail::gYobiActionStart;
    YobiAction* const end = detail::gYobiActionEnd;
    std::uint32_t count = 0;
    YobiAction* current = start->next;
    detail::gDrawYobiCount59E578 = 0;
    while (current != end) {
        if ((current->flags & 0x20000u) != 0) {
            detail::gDrawYobiList59E57C[count] = current;
            ++count;
            detail::gDrawYobiCount59E578 = static_cast<std::int32_t>(count);
        }
        current = current->next;
    }
    std::sort(detail::gDrawYobiList59E57C.begin(), detail::gDrawYobiList59E57C.begin() + count,
        [](YobiAction* const left, YobiAction* const right) {
            return detail::CompareBattleDrawActions_405BD0(&left, &right) < 0;
        });
    // AT01：先将每个主称号移到本人物/骑乘/辅助挂件之后，再分配原有层号。
    // 其他对象相互次序保持，不把称号提至场景固定高层。
    for (std::uint32_t titleIndex = 0; titleIndex < count; ++titleIndex) {
        YobiAction* const title = detail::gDrawYobiList59E57C[titleIndex];
        if (detail::GetBattleTitleAnchor(title) == nullptr) {
            continue;
        }
        const YobiAction* const parent = title->battleAttachmentParent10;
        std::uint32_t last = titleIndex;
        for (std::uint32_t sibling = titleIndex + 1; sibling < count; ++sibling) {
            const YobiAction* const candidate = detail::gDrawYobiList59E57C[sibling];
            if (candidate == parent || (candidate->callback == detail::UpdateBattleAttachment_421F90
                && candidate->battleAttachmentParent10 == parent)) {
                last = sibling;
            }
        }
        if (last != titleIndex) {
            std::rotate(detail::gDrawYobiList59E57C.begin() + titleIndex,
                detail::gDrawYobiList59E57C.begin() + titleIndex + 1,
                detail::gDrawYobiList59E57C.begin() + last + 1);
            --titleIndex; // 重查移到当前位置的对象，避免漏过另一人物的称号。
        }
    }
    std::int32_t index = 0;
    if (detail::gDrawYobiCount59E578 > 0) {
        do {
            YobiAction* const action = detail::gDrawYobiList59E57C[index];
            const auto priority = static_cast<std::uint8_t>(index + 9);
            ++index;
            action->displayPriority = priority;
        } while (index < detail::gDrawYobiCount59E578);
        for (std::int32_t gearIndex = 0; gearIndex < detail::gDrawYobiCount59E578; ++gearIndex) {
            detail::ResolveBattleGearLayer406(detail::gDrawYobiList59E57C[gearIndex]);
        }
    }
    return index;
}

YobiAction* const* FindBattleRebirthSlot(const YobiAction* const action) noexcept
{
    const auto* const payload = static_cast<const detail::BattleActorPayload*>(action->payload.battleActor);
    return payload != nullptr ? &payload->rebirthEffect106C : nullptr;
}

YobiAction* const* FindBattleRiderSlot(const YobiAction* const action) noexcept
{
    const auto* const payload = static_cast<const detail::BattleActorPayload*>(action->payload.battleActor);
    return payload != nullptr ? &payload->rider13C8 : nullptr;
}

} // namespace cg::yobi
