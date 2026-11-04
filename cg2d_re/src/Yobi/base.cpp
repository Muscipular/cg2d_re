#include "base.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>
#include <memory>

namespace cg::yobi {

YobiAction::YobiAction(RawFields)
    : readiness(platform::LegacyCryptInt::RawPayload{}),
      maxHp(platform::LegacyCryptInt::RawPayload{}),
      value190(platform::LegacyCryptInt::RawPayload{}),
      value1A0(platform::LegacyCryptInt::RawPayload{}),
      barValue1B0(platform::LegacyCryptInt::RawPayload{}),
      barMaximum1C0(platform::LegacyCryptInt::RawPayload{}),
      mapX1E0(platform::LegacyCryptInt::RawPayload{}),
      mapY1F0(platform::LegacyCryptInt::RawPayload{}),
      mapX200(platform::LegacyCryptInt::RawPayload{}),
      mapY210(platform::LegacyCryptInt::RawPayload{})
{
    // 普通字段使用成员初始化，字符串正常构造；不覆盖对象，也不触发CryptInt默认构造。
}

// 原函数：0x0041EB10。
// 原41EB10逐成员复制；三个联合槽按原生宽度复制，CryptInt不改目标token/保留字节。
YobiAction& YobiAction::operator=(const YobiAction& source)
{
    this->prev = source.prev;
    this->next = source.next;
    this->callback = source.callback;
    this->payload = source.payload;
    this->payloadKind = source.payloadKind;
    this->ownerSession = source.ownerSession;
    std::memcpy(&this->completionSlot10, &source.completionSlot10, sizeof(source.completionSlot10));
    this->priority = source.priority;
    this->displayPriority = source.displayPriority;
    this->cacheIndex = source.cacheIndex;
    this->deathFlag = source.deathFlag;
    this->graphicIndex = source.graphicIndex;
    this->flags = source.flags;
    this->counter28 = source.counter28;
    std::memcpy(&this->numberWindowCallback2C, &source.numberWindowCallback2C, sizeof(source.numberWindowCallback2C));
    this->x = source.x;
    this->y = source.y;
    this->deltaX = source.deltaX;
    this->deltaY = source.deltaY;
    std::memcpy(&this->origin_x_offset40, &source.origin_x_offset40, sizeof(source.origin_x_offset40));
    std::memcpy(&this->origin_y_offset44, &source.origin_y_offset44, sizeof(source.origin_y_offset44));
    std::memcpy(&this->effectAcceleration48, &source.effectAcceleration48, sizeof(source.effectAcceleration48));
    this->verticalVelocityBits4C = source.verticalVelocityBits4C;
    std::memcpy(&this->angle50, &source.angle50, sizeof(source.angle50));
    std::memcpy(&this->motionMagnitude54, &source.motionMagnitude54, sizeof(source.motionMagnitude54));
    this->field58 = source.field58;
    std::memcpy(&this->scaleX, &source.scaleX, sizeof(source.scaleX));
    std::memcpy(&this->scaleY, &source.scaleY, sizeof(source.scaleY));
    this->displayName = source.displayName;
    this->nameX = source.nameX;
    this->nameY = source.nameY;
    std::memcpy(&this->nameOffsetAC, &source.nameOffsetAC, sizeof(source.nameOffsetAC));
    std::memcpy(&this->nameOffsetB0, &source.nameOffsetB0, sizeof(source.nameOffsetB0));
    std::memcpy(&this->battleTurnStepB4, &source.battleTurnStepB4, sizeof(source.battleTurnStepB4));
    std::memcpy(&this->numberVelocityB8, &source.numberVelocityB8, sizeof(source.numberVelocityB8));
    this->nameColor = source.nameColor;
    this->nameFontType = source.nameFontType;
    this->nameLayer = source.nameLayer;
    this->nameValueC0 = source.nameValueC0;
    this->nameC4 = source.nameC4;
    this->titleE1 = source.titleE1;
    this->text102 = source.text102;
    this->text123 = source.text123;
    this->text144 = source.text144;
    this->bytes155 = source.bytes155;
    this->readiness = source.readiness;
    this->maxHp = source.maxHp;
    this->value190 = source.value190;
    this->value1A0 = source.value1A0;
    this->barValue1B0 = source.barValue1B0;
    this->barMaximum1C0 = source.barMaximum1C0;
    std::memcpy(&this->windowCompletionSlot1D0, &source.windowCompletionSlot1D0, sizeof(source.windowCompletionSlot1D0));
    this->value1D4 = source.value1D4;
    this->hoverTextColor1D8 = source.hoverTextColor1D8;
    this->counter1DC = source.counter1DC;
    this->mapX1E0 = source.mapX1E0;
    this->mapY1F0 = source.mapY1F0;
    this->mapX200 = source.mapX200;
    this->mapY210 = source.mapY210;
    this->pathX220 = source.pathX220;
    this->pathY248 = source.pathY248;
    this->pathCountOrDrawOrder_270 = source.pathCountOrDrawOrder_270;
    this->movementActive_272 = source.movementActive_272;
    std::memcpy(&this->worldX, &source.worldX, sizeof(source.worldX));
    std::memcpy(&this->worldY, &source.worldY, sizeof(source.worldY));
    this->movementStepXBits_27C = source.movementStepXBits_27C;
    this->movementStepYBits_280 = source.movementStepYBits_280;
    this->walkSpeed = source.walkSpeed;
    this->fields288 = source.fields288;
    this->extraState290 = source.extraState290;
    this->framePlayingRate = source.framePlayingRate;
    this->imageNo = source.imageNo;
    this->lastImageNo = source.lastImageNo;
    this->characterAction = source.characterAction;
    this->lastCharacterAction = source.lastCharacterAction;
    this->direction = source.direction;
    this->lastDirection = source.lastDirection;
    this->frameIndex = source.frameIndex;
    this->frameRemain = source.frameRemain;
    this->graphicOffsetX = source.graphicOffsetX;
    this->graphicOffsetY = source.graphicOffsetY;
    this->frameEvent = source.frameEvent;
    this->hidden = source.hidden;
    this->field2D8 = source.field2D8;
    this->completedLoops = source.completedLoops;
    this->playRevert = source.playRevert;
    this->overheadMarkerMode2E4 = source.overheadMarkerMode2E4;
    this->overheadMarkerImage2E8 = source.overheadMarkerImage2E8;
    this->overheadMarkerRefresh2EC = source.overheadMarkerRefresh2EC;
    this->frameX = source.frameX;
    this->frameY = source.frameY;
    this->drawMode = source.drawMode;
    this->batchType = source.batchType;
    this->mixFlagsOverrideMask = source.mixFlagsOverrideMask;
    this->color2 = source.color2;
    this->mixFlagsOverrideValue = source.mixFlagsOverrideValue;
    this->mapOffsetX = source.mapOffsetX;
    this->mapOffsetY = source.mapOffsetY;
    this->text31C = source.text31C;
    return *this;
}

namespace detail {

static constexpr std::uint32_t kFourPhaseFlag = YobiActionFlag_FourPhaseUpdateGate;

// 原函数：0x00404D10。
static void DeleteYobiAction_404D10(YobiAction* const action)
{
    // 按有效成员转回分配地址，不能借raw读取其它活动指针成员。
    void* payload = nullptr;
    switch (action->payloadKind) {
    case YobiPayloadKind::None:
    case YobiPayloadKind::Raw: payload = action->payload.raw; break;
    case YobiPayloadKind::BattleActor: payload = action->payload.battleActor; break;
    case YobiPayloadKind::BattleScene: payload = action->payload.battleScene; break;
    case YobiPayloadKind::BattleRideCarrier: payload = action->payload.battleRideCarrier; break;
    case YobiPayloadKind::BattleRingEmitter: payload = action->payload.battleRingEmitter; break;
    case YobiPayloadKind::BattleFollowOffset: payload = action->payload.battleFollowOffset; break;
    case YobiPayloadKind::BattleTransitionEffect: payload = action->payload.battleTransitionEffect; break;
    case YobiPayloadKind::BattleText: payload = action->payload.battleText; break;
    case YobiPayloadKind::MapActor: payload = action->payload.mapActor; break;
    case YobiPayloadKind::MapChara: payload = action->payload.mapChara; break;
    case YobiPayloadKind::MapTransition: payload = action->payload.mapTransition; break;
    case YobiPayloadKind::WindowOpening: payload = action->payload.windowOpening; break;
    case YobiPayloadKind::WindowUi: payload = action->payload.windowUi; break;
    }
    if (payload != nullptr) {
        std::free(payload);
    }
    std::destroy_at(action);
    std::free(action);
}

static bool ShouldDeferForFourPhaseGate(const YobiAction* const action)
{
    return (action->flags & kFourPhaseFlag) != 0
        && action->ownerSession->Yobi().gateMode == YobiUpdateGatePolicy_DeferFlaggedUntilPhaseZero
        && action->ownerSession->Yobi().phaseCounter != 0;
}

} // namespace detail

// 原函数：0x00404A40。
YobiAction* InitializeYobiActionList_404A40(session::ClientSession& session)
{
    auto& list = session.Yobi();
    list.start = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));
    YobiAction* const allocatedEnd = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));
    YobiAction* const initialStart = list.start;
    list.end = allocatedEnd;
    std::construct_at(initialStart, YobiAction::RawFields{});
    std::construct_at(allocatedEnd, YobiAction::RawFields{});

    initialStart->prev = nullptr;
    YobiAction* const forwardStart = list.start;
    YobiAction* const forwardEnd = list.end;
    forwardStart->next = forwardEnd;
    list.start->callback = nullptr;
    list.start->priority = 0;
    list.start->graphicIndex = -1;

    YobiAction* const backwardEnd = list.end;
    YobiAction* const backwardStart = list.start;
    backwardEnd->prev = backwardStart;
    list.end->next = nullptr;
    list.end->callback = nullptr;
    YobiAction* const result = list.end;
    result->priority = 0xFF;
    list.end->graphicIndex = -1;
    return result;
}

// 原函数：0x00404AE0。
YobiAction* NewYobiAction_404AE0(
    session::ClientSession& session,
    const std::uint8_t priority,
    const std::uint32_t payloadSize,
    const YobiPayloadKind payloadKind)
{
    auto* const action = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));
    if (action == nullptr) {
        platform::ShowWideMessage(L"GetAction Erorr!!", L"确认", 0);
        platform::ShowWideMessage(L"记忆体容量不足！pAct", L"GetAction Erorr", 0);
        return nullptr;
    }

    std::construct_at(action, YobiAction::RawFields{});
    action->ownerSession = &session;
    if (payloadSize != 0) {
        void* const payload = std::calloc(std::size_t{1}, payloadSize);
        const bool missingPayload = payload == nullptr;
        action->payloadKind = payloadKind;
        switch (payloadKind) {
        case YobiPayloadKind::None:
            action->payloadKind = YobiPayloadKind::Raw;
            [[fallthrough]];
        case YobiPayloadKind::Raw: action->payload.raw = payload; break;
        case YobiPayloadKind::BattleActor: action->payload.battleActor = static_cast<detail::BattleActorPayload*>(payload); break;
        case YobiPayloadKind::BattleScene: action->payload.battleScene = static_cast<detail::BattleScenePayload*>(payload); break;
        case YobiPayloadKind::BattleRideCarrier: action->payload.battleRideCarrier = static_cast<detail::BattleRideCarrierPayload*>(payload); break;
        case YobiPayloadKind::BattleRingEmitter: action->payload.battleRingEmitter = static_cast<detail::BattleRingEmitterPayload*>(payload); break;
        case YobiPayloadKind::BattleFollowOffset: action->payload.battleFollowOffset = static_cast<detail::BattleFollowOffsetPayload*>(payload); break;
        case YobiPayloadKind::BattleTransitionEffect: action->payload.battleTransitionEffect = static_cast<detail::BattleTransitionEffectPayload*>(payload); break;
        case YobiPayloadKind::BattleText: action->payload.battleText = static_cast<detail::BattleTextPayload*>(payload); break;
        case YobiPayloadKind::MapActor: action->payload.mapActor = static_cast<detail::MapActorPayload*>(payload); break;
        case YobiPayloadKind::MapChara: action->payload.mapChara = static_cast<detail::MapCharaPayload*>(payload); break;
        case YobiPayloadKind::MapTransition: action->payload.mapTransition = static_cast<detail::MapTransitionPayload*>(payload); break;
        case YobiPayloadKind::WindowOpening: action->payload.windowOpening = static_cast<detail::WindowOpeningPayload*>(payload); break;
        case YobiPayloadKind::WindowUi: action->payload.windowUi = static_cast<detail::YobiUIData*>(payload); break;
        }
        if (missingPayload) {
            std::destroy_at(action);
            std::free(action);
            platform::ShowWideMessage(L"记忆体容量不足！yobi", L"GetYobi Erorr", 0);
            return nullptr;
        }
    }

    action->callback = nullptr;
    action->priority = priority;
    action->graphicIndex = -1;
    action->cacheIndex = YobiGraphicCacheIndex_NotQueued;
    action->framePlayingRate = 100;
    action->scaleX = 1.0F;
    action->scaleY = 1.0F;
    // 原404BB2–404BE0：显式初始化描述符，颜色为全白，其余清零。
    const std::uint32_t descriptorColor = 0xFFFFFFFFu;
    action->drawMode = {0, 0, 0, 0, descriptorColor, 0, render::GraphicMixFlag_None, 0};
    action->batchType = static_cast<std::int32_t>(YobiColorChannelFlag_None);
    action->color2 = -1;
    action->mixFlagsOverrideMask = render::GraphicMixFlag_None;
    action->mixFlagsOverrideValue = render::GraphicMixFlag_None;
    action->walkSpeed = 100;

    YobiAction* const start = session.Yobi().start;
    YobiAction* const end = session.Yobi().end;
    YobiAction* insertionPoint = start->next;
    YobiAction* const stop = end->next;
    if (insertionPoint != stop) {
        while (insertionPoint->priority <= priority) {
            insertionPoint = insertionPoint->next;
            if (insertionPoint == stop) {
                return action;
            }
        }

        YobiAction* const previous = insertionPoint->prev;
        action->next = insertionPoint;
        action->prev = previous;
        YobiAction* const currentPrevious = insertionPoint->prev;
        currentPrevious->next = action;
        insertionPoint->prev = action;
    }
    return action;
}

// 原函数：0x00404C50。
std::int32_t YobiUpdateAll_404C50(session::ClientSession& session)
{
    YobiAction* current = session.Yobi().start->next;
    while (current != session.Yobi().end) {
        if (current->deathFlag != 0) {
            if (detail::ShouldDeferForFourPhaseGate(current)) {
                current = current->next;
                continue;
            }

            YobiAction* const previous = current->prev;
            YobiAction* const firstNext = current->next;
            previous->next = firstNext;
            YobiAction* const currentNext = current->next;
            YobiAction* const currentPrevious = current->prev;
            currentNext->prev = currentPrevious;
            YobiAction* const next = current->next;
            detail::DeleteYobiAction_404D10(current);
            current = next;
            continue;
        }

        const YobiCallback callback = current->callback;
        if (callback != nullptr && !detail::ShouldDeferForFourPhaseGate(current)) {
            callback(current);
        }
        current = current->next;
    }

    const std::int32_t result = session.Yobi().phaseCounter + 1;
    const bool resetPhase = result >= 4;
    session.Yobi().phaseCounter = result;
    if (resetPhase) {
        session.Yobi().phaseCounter = 0;
    }
    return result;
}

// 原函数：0x00404D00。
YobiAction* MarkDeath(YobiAction* const action)
{
    if (action != nullptr) {
        action->deathFlag = 1;
    }
    return action;
}

// void MarkDeathUnchecked(YobiAction* const action)
// {
//     action->deathFlag = 1;
// }

// 原函数：0x00404D30。
void YobiClearUp_404D30(session::ClientSession& session)
{
    YobiAction* current = session.Yobi().start->next;
    ClearRegisteredMapActions_46DD40(session);
    while (current != session.Yobi().end) {
        current->deathFlag = 1;
        current = current->next;
    }
}

// 原函数：0x00404D60。
void ShutdownYobiActionList_404D60(session::ClientSession& session)
{
    YobiClearUp_404D30(session);
    YobiUpdateAll_404C50(session);
    std::destroy_at(session.Yobi().start);
    std::free(session.Yobi().start);
    std::destroy_at(session.Yobi().end);
    std::free(session.Yobi().end);
    session.Yobi().start = nullptr;
    session.Yobi().end = nullptr;
}

bool IsYobiActionReady(
    YobiAction* const action,
    const std::int32_t threshold)
{
    return (action->readiness >= threshold);
}

bool IsYobiActionStateEqual(
    YobiAction* const action,
    const std::int32_t expected)
{
    return (action->readiness == expected);
}

bool IsYobiActionStateLessOrEqual(
    YobiAction* const action,
    const std::int32_t maximum)
{
    return (action->readiness <= maximum);
}

bool IsYobiActionFpLessThan(YobiAction* const action, const std::int32_t cost)
{
    return (action->barValue1B0 < cost);
}

// 原401F80：cost在左、action FP为右值；右对象只读一次，保留计数/RNG消费。
bool IsCostGreaterThanYobiActionFp(const std::int32_t cost, YobiAction* const action)
{
    return (cost > action->barValue1B0);
}

bool IsYobiActionFpGreaterEqual(YobiAction* const action, const std::int32_t cost)
{
    return (action->barValue1B0 >= cost);
}

std::int32_t ReadYobiActionFpOnce(YobiAction* const action)
{
    return static_cast<std::int32_t>(action->barValue1B0);
}

std::int32_t ReadYobiActionMaxFpOnce(YobiAction* const action)
{
    return static_cast<std::int32_t>(action->barMaximum1C0);
}

std::int32_t ReadYobiActionHpOnce(YobiAction* const action)
{
    return static_cast<std::int32_t>(action->readiness);
}

std::int32_t ReadYobiActionMaxHpOnce(YobiAction* const action)
{
    return static_cast<std::int32_t>(action->maxHp);
}

float GetYobiActionScaleX(const YobiAction* const action) noexcept
{
    return action->scaleX;
}

std::int32_t GetYobiActionField2DC(const YobiAction* const action) noexcept
{
    return std::bit_cast<std::int32_t>(action->completedLoops);
}

float GetYobiActionScaleY(const YobiAction* const action) noexcept
{
    return action->scaleY;
}

std::int32_t ReadYobiGraphicOffsetX(const YobiAction* const action) noexcept
{
    return action->graphicOffsetX;
}

std::int32_t ReadYobiGraphicOffsetY(const YobiAction* const action) noexcept
{
    return action->graphicOffsetY;
}

std::uint32_t ReadYobiActionFlags(const YobiAction* const action) noexcept
{
    return action->flags;
}

void ClearYobiActionCallback(YobiAction* const action)
{
    action->callback = nullptr;
}

const render::GraphicDescriptor16* GetYobiActionDrawMode(
    const YobiAction* const action) noexcept
{
    return &action->drawMode;
}

std::int32_t GetYobiActionField20(YobiAction* const action)
{
    return action->graphicIndex;
}

void AddYobiActionDrawFlags(YobiAction* const action, const std::uint8_t flags) noexcept
{
    action->drawMode.mixFlags |= flags;
}

std::int32_t GetYobiActionField2E4(const YobiAction* const action) noexcept
{
    return action->overheadMarkerMode2E4;
}

const wchar_t* GetYobiName(const YobiAction* const action) noexcept
{
    return action->nameC4.data();
}

const wchar_t* GetYobiTitle(const YobiAction* const action) noexcept
{
    return action->titleE1.data();
}

std::uint8_t GetYobiActionFlagsLowByte(const YobiAction* const action) noexcept
{
    return reinterpret_cast<const std::uint8_t*>(&action->flags)[0];
}

std::int32_t GetYobiActionCacheIndex(const YobiAction* const action) noexcept
{
    return action->cacheIndex;
}

std::int32_t GetYobiActionField2A8(YobiAction* const action)
{
    return action->imageNo;
}

void SetYobiActionField2A8(
    YobiAction* const action,
    const std::int32_t value)
{
    action->imageNo = value;
}

void SetYobiActionField2B0(
    YobiAction* const action,
    const std::int32_t value)
{
    action->characterAction = value;
}

std::int32_t GetYobiActionField2B0(const YobiAction* const action) noexcept
{
    return action->characterAction;
}

void SetYobiActionField15(
    YobiAction* const action,
    const std::uint8_t value)
{
    action->displayPriority = value;
}

std::uint8_t GetYobiActionField15(const YobiAction* const action) noexcept
{
    return action->displayPriority;
}

std::int32_t GetYobiActionField30(YobiAction* const action)
{
    return action->x;
}

void SetYobiActionField30(
    YobiAction* const action,
    const std::int32_t value)
{
    action->x = value;
}

std::int32_t GetYobiActionField34(YobiAction* const action)
{
    return action->y;
}

void SetYobiActionField34(
    YobiAction* const action,
    const std::int32_t value)
{
    action->y = value;
}

void SetYobiActionField2B8(
    YobiAction* const action,
    const std::int32_t value)
{
    action->direction = value;
}

std::int32_t GetYobiActionField2B8(const YobiAction* const action) noexcept
{
    return action->direction;
}

std::uint8_t ReadYobiBatchType(const YobiAction* const action) noexcept
{
    return action->batchType;
}

void SetYobiActionField308(
    YobiAction* const action,
    const std::uint8_t value)
{
    action->batchType = value;
}

void SetYobiActionMixFlagsOverrideMask(YobiAction* const action, const std::uint8_t value)
{
    action->mixFlagsOverrideMask = value;
}

void SetYobiActionMixFlagsOverrideValue(YobiAction* const action, const std::uint8_t value)
{
    action->mixFlagsOverrideValue = value;
}

void SetYobiActionField30C(
    YobiAction* const action,
    const std::int32_t value)
{
    action->color2 = value;
}

std::uint8_t GetYobiActionField30F(YobiAction* const action)
{
    const std::uint32_t bits = std::bit_cast<std::uint32_t>(action->color2);
    return static_cast<std::uint8_t>(bits >> 24U);
}

void SetYobiActionField30F(
    YobiAction* const action,
    const std::uint8_t value)
{
    const std::uint32_t bits =
        std::bit_cast<std::uint32_t>(action->color2);
    const std::uint32_t updated =
        (bits & 0x00FFFFFFU) | (std::uint32_t{value} << 24U);
    action->color2 = std::bit_cast<std::int32_t>(updated);
}

} // namespace cg::yobi
