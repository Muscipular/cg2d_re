#include "base.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/platform/LegacyWindow.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>
#include <memory>

namespace cg::login {

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
    // 原calloc的其余零字节直接落到普通字段，不覆盖已开始寿命的CryptInt。
    auto* const bytes = reinterpret_cast<std::byte*>(this);
    std::memset(bytes, 0, offsetof(YobiAction, readiness));
    constexpr std::size_t middle = offsetof(YobiAction, barMaximum1C0) + sizeof(barMaximum1C0);
    std::memset(bytes + middle, 0, offsetof(YobiAction, mapX1E0) - middle);
    constexpr std::size_t tail = offsetof(YobiAction, mapY210) + sizeof(mapY210);
    std::memset(bytes + tail, 0, sizeof(YobiAction) - tail);
}

namespace detail {

std::int32_t gYobiUpdateGateMode = 0; // 原全局：0x0059E570。

std::int32_t gYobiUpdatePhaseCounter = 0; // 原全局：0x0059E574。

namespace {

constexpr std::uint32_t kFourPhaseFlag = 0x00040000U;

} // namespace

YobiAction* gYobiActionStart = nullptr; // 原全局：0x0059E56C。

YobiAction* gYobiActionEnd = nullptr; // 原全局：0x0059E568。

std::int32_t LowSignedByte(const std::int16_t value)
{
    return static_cast<std::int32_t>(
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value)));
}

namespace {

// 原函数：0x00404D10。
void DeleteYobiAction_404D10(YobiAction* const action)
{
    void* const payload = action->payload;
    if (payload != nullptr) {
        std::free(payload);
    }
    std::destroy_at(action);
    std::free(action);
}

bool ShouldDeferForFourPhaseGate(const YobiAction* const action)
{
    return (action->flags & kFourPhaseFlag) != 0
        && gYobiUpdateGateMode == 1
        && gYobiUpdatePhaseCounter != 0;
}

} // namespace

// 原函数：0x00404A40。
YobiAction* Sub_404A40()
{
    gYobiActionStart = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));
    YobiAction* const allocatedEnd = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));
    YobiAction* const initialStart = gYobiActionStart;
    gYobiActionEnd = allocatedEnd;
    std::construct_at(initialStart, YobiAction::RawFields{});
    std::construct_at(allocatedEnd, YobiAction::RawFields{});

    initialStart->prev = nullptr;
    YobiAction* const forwardStart = gYobiActionStart;
    YobiAction* const forwardEnd = gYobiActionEnd;
    forwardStart->next = forwardEnd;
    gYobiActionStart->callback = nullptr;
    gYobiActionStart->priority = 0;
    gYobiActionStart->graphicIndex = -1;

    YobiAction* const backwardEnd = gYobiActionEnd;
    YobiAction* const backwardStart = gYobiActionStart;
    backwardEnd->prev = backwardStart;
    gYobiActionEnd->next = nullptr;
    gYobiActionEnd->callback = nullptr;
    YobiAction* const result = gYobiActionEnd;
    result->priority = 0xFF;
    gYobiActionEnd->graphicIndex = -1;
    return result;
}

// 原函数：0x00404AE0。
YobiAction* NewYobiAction_404AE0(
    const std::uint8_t priority,
    const std::uint32_t payloadSize)
{
    auto* const action = static_cast<YobiAction*>(
        std::calloc(std::size_t{1}, sizeof(YobiAction)));
    if (action == nullptr) {
        platform::ShowWideMessage(L"GetAction Erorr!!", L"確認", 0);
        platform::ShowWideMessage(L"記憶體容量不足！pAct", L"GetAction Erorr", 0);
        return nullptr;
    }

    std::construct_at(action, YobiAction::RawFields{});
    if (payloadSize != 0) {
        void* const payload = std::calloc(std::size_t{1}, payloadSize);
        const bool missingPayload = payload == nullptr;
        action->payload = payload;
        if (missingPayload) {
            std::destroy_at(action);
            std::free(action);
            platform::ShowWideMessage(L"記憶體容量不足！yobi", L"GetYobi Erorr", 0);
            return nullptr;
        }
    }

    action->callback = nullptr;
    action->priority = priority;
    action->graphicIndex = -1;
    action->cacheIndex = -2;
    action->framePlayingRate = 100;
    action->scaleX = 1.0F;
    action->scaleY = 1.0F;
    // 原404BB2–404BE0：显式初始化描述符，颜色为全白，其余清零。
    const std::uint32_t descriptorColor = 0xFFFFFFFFu;
    action->drawMode = {0, 0, 0, 0, descriptorColor, 0, render::GraphicMixFlag_None, 0};
    action->batchType = 0;
    action->color2 = -1;
    action->rebirthValue1 = 0;
    action->rebirthValue2 = 0;
    action->walkSpeed = 100;

    YobiAction* const start = gYobiActionStart;
    YobiAction* const end = gYobiActionEnd;
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
std::int32_t YobiUpdateAll_404C50()
{
    YobiAction* current = gYobiActionStart->next;
    while (current != gYobiActionEnd) {
        if (current->deathFlag != 0) {
            if (ShouldDeferForFourPhaseGate(current)) {
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
            DeleteYobiAction_404D10(current);
            current = next;
            continue;
        }

        const YobiCallback callback = current->callback;
        if (callback != nullptr && !ShouldDeferForFourPhaseGate(current)) {
            callback(current);
        }
        current = current->next;
    }

    const std::int32_t result = gYobiUpdatePhaseCounter + 1;
    const bool resetPhase = result >= 4;
    gYobiUpdatePhaseCounter = result;
    if (resetPhase) {
        gYobiUpdatePhaseCounter = 0;
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

void MarkDeathUnchecked(YobiAction* const action)
{
    action->deathFlag = 1;
}

// 原函数：0x00404D30。
void YobiClearUp_404D30()
{
    YobiAction* current = gYobiActionStart->next;
    Sub_46DD40();
    while (current != gYobiActionEnd) {
        current->deathFlag = 1;
        current = current->next;
    }
}

// 原函数：0x00404D60。
void Sub_404D60()
{
    YobiClearUp_404D30();
    YobiUpdateAll_404C50();
    std::destroy_at(gYobiActionStart);
    std::free(gYobiActionStart);
    std::destroy_at(gYobiActionEnd);
    std::free(gYobiActionEnd);
}

bool IsYobiActionReady(
    YobiAction* const action,
    const std::int32_t threshold)
{
    return (action->readiness >= static_cast<std::int32_t>(threshold));
}

bool IsYobiActionStateEqual(
    YobiAction* const action,
    const std::int32_t expected)
{
    return (action->readiness == static_cast<std::int32_t>(expected));
}

bool IsYobiActionStateLessOrEqual(
    YobiAction* const action,
    const std::int32_t maximum)
{
    return (action->readiness <= static_cast<std::int32_t>(maximum));
}

bool IsYobiActionFpLessThan(YobiAction* const action, const std::int32_t cost)
{
    return (action->barValue1B0 < static_cast<std::int32_t>(cost));
}

// 原401F80：cost在左、action FP为右值；右对象只读一次，保留计数/RNG消费。
bool IsCostGreaterThanYobiActionFp(const std::int32_t cost, YobiAction* const action)
{
    return (static_cast<std::int32_t>(cost) > action->barValue1B0);
}

bool IsYobiActionFpGreaterEqual(YobiAction* const action, const std::int32_t cost)
{
    return (action->barValue1B0 >= static_cast<std::int32_t>(cost));
}

std::int32_t ReadYobiActionFpOnce(YobiAction* const action)
{
    return action->barValue1B0.value();
}

std::int32_t ReadYobiActionMaxFpOnce(YobiAction* const action)
{
    return action->barMaximum1C0.value();
}

std::int32_t ReadYobiActionHpOnce(YobiAction* const action)
{
    return action->readiness.value();
}

std::int32_t ReadYobiActionMaxHpOnce(YobiAction* const action)
{
    return action->maxHp.value();
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
    return action->field2E4;
}

const char* GetYobiName(const YobiAction* const action) noexcept
{
    return action->nameC4.data();
}

const char* GetYobiTitle(const YobiAction* const action) noexcept
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

void SetYobiActionField309(YobiAction* const action, const std::uint8_t value)
{
    action->rebirthValue1 = value;
}

void SetYobiActionField310(YobiAction* const action, const std::uint8_t value)
{
    action->rebirthValue2 = value;
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

} // namespace detail

} // namespace cg::login
