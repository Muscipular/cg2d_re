#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "battle_scene.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_message.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include "animation.hpp"
#include "map_render.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"
#include "../world/WorldMapState.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::yobi {

namespace detail {

std::int32_t gBattlePendingGroupCount5ABB24 = 0; // 原全局：0x005ABB24。 4383F0/43B360生产组计数，427470延迟减到零时递减。

// 原函数：0x0043B360。
void DispatchBattleEvents_43B360(YobiAction* const scene)
{
    std::int32_t source = -1;
    std::int32_t previousSource = -1;
    const std::int32_t sceneType = world::gCurrentSceneType;
    const std::uint32_t exitRequested = cg::world::gBattleExitRequested_5ABB20;
    if (!(sceneType == 10
            && static_cast<std::uint32_t>(world::gCurrentSceneState) > 5U)
        && exitRequested != 1) {
        return;
    }
    const std::uint32_t activeEvents = cg::world::gBattleActiveEventMask5AAA74;
    if (activeEvents == 0) {
        gBattlePendingGroupCount5ABB24 = 0;
    } else if (cg::world::gBattleEventDispatchBusy_5ABB1C != 0) {
        return;
    }
    const bool exiting = exitRequested == 1;
    cg::world::gBattleEventDispatchBusy_5ABB1C = 1;
    if (exiting) {
        world::gCurrentSceneState = 7;
        return;
    }
    if (cg::world::gBattleTurnMessagesExhausted_5ABB28 == 1) {
        if (activeEvents == 0 && CheckBattleTurnReady_4394A0(scene, BattleTurnReadyPolicy_CheckAllSelectedEntries) != 0) {
            const std::uint32_t turn = cg::world::gBattleTurnSnapshot5ABC08;
            cg::world::gBattleTurnMessagesExhausted_5ABB28 = 0;
            cg::world::gBattleEventDispatchBusy_5ABB1C = 0;
            gBattlePendingGroupCount5ABB24 = 0;
            cg::world::gBattleTurnSnapshot5ABC08 = turn + 1U;
            world::gCurrentSceneState = 2;
        }
        return;
    }

    std::int32_t slot = 0;
    while (slot < static_cast<std::int32_t>(cg::world::gBattleEventActions5ABA9C.size())
        && cg::world::gBattleEventActions5ABA9C[slot] != nullptr) {
        ++slot;
    }
    // 原43B446..43B464：清零22槽共享参与计数。
    cg::world::gBattlePreviewParticipants5AAA48.fill(0);
    for (YobiAction* const event : cg::world::gBattleEventActions5ABA9C) {
        if (event != nullptr) {
            const auto* const payload = static_cast<const BattleScenePayload*>(event->payload.battleScene);
            for (std::int32_t position = 0;
                 position < static_cast<std::int32_t>(cg::world::gBattlePreviewParticipants5AAA48.size()); ++position) {
                if ((payload->entryMaskA8 & (1U << position)) != 0) {
                    ++cg::world::gBattlePreviewParticipants5AAA48[position];
                }
            }
        }
    }

    YobiAction* previousEvent = nullptr;
    // 原var_8仅由工厂写入；previousEvent首次非空前必经一次成功创建。
    YobiAction* lastCreated = nullptr;
    std::int32_t groupBoundary;
    while (slot < static_cast<std::int32_t>(cg::world::gBattleEventActions5ABA9C.size())) {
        const std::int32_t priorSource = previousSource;
        std::uint32_t participants = 0;
        if (CheckBattleMParticipants_43B290(
                &source, priorSource, &participants, &groupBoundary) != 1) {
            return;
        }
        std::array<char, 4> header;
        if (GetBattleMHeader_439070(header.data()) == 1) {
            cg::world::gBattleTurnMessagesExhausted_5ABB28 = 1;
            return;
        }
        if (previousEvent != nullptr && priorSource == source) {
            auto* const payload = previousEvent->payload.battleScene;
            const std::int32_t messages = payload->remainingMessages5C;
            const std::uint32_t eventMask = payload->entryMaskA8;
            payload->remainingMessages5C = messages + 1;
            payload->entryMaskA8 = eventMask | participants;
            cg::world::gBattleMScanOffset5A8A3C = cg::world::gBattleMPreviewOffset5AAA60;
        } else {
            std::int32_t kind = 0;
            while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
                && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
                ++kind;
            }
            if (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())) {
                if (previousEvent != nullptr
                    && (kind == static_cast<std::int32_t>(BattleMDataHeader::Chat)
                        || kind == static_cast<std::int32_t>(BattleMStatusHeader::QueuedValue))) {
                    auto* const payload = previousEvent->payload.battleScene;
                    const std::int32_t messages = payload->remainingMessages5C;
                    const std::uint32_t eventMask = payload->entryMaskA8;
                    payload->remainingMessages5C = messages + 1;
                    const std::uint32_t combinedMask = eventMask | participants;
                    YobiAction* const mergedEvent = lastCreated;
                    payload->entryMaskA8 = combinedMask;
                    cg::world::gBattleMScanOffset5A8A3C = cg::world::gBattleMPreviewOffset5AAA60;
                    previousSource = source;
                    previousEvent = mergedEvent;
                } else {
                    lastCreated = NewBattleEvent_43B610(
                        static_cast<std::int32_t>(kind), static_cast<std::int32_t>(slot),
                        participants, gBattlePendingGroupCount5ABB24);
                    if (lastCreated != nullptr) {
                        cg::world::gBattleActiveEventMask5AAA74 |= 1U << slot;
                        while (slot < static_cast<std::int32_t>(cg::world::gBattleEventActions5ABA9C.size())
                            && cg::world::gBattleEventActions5ABA9C[slot] != nullptr) {
                            ++slot;
                        }
                        lastCreated->payload.battleScene->remainingMessages5C = 1;
                        cg::world::gBattleMScanOffset5A8A3C = cg::world::gBattleMPreviewOffset5AAA60;
                        previousSource = source;
                        previousEvent = lastCreated;
                    }
                }
            }
        }
        if (groupBoundary != 0) {
            ++gBattlePendingGroupCount5ABB24;
        }
    }
}

// 原函数：0x0043B610。
YobiAction* NewBattleEvent_43B610(
    const std::int32_t kind, const std::int32_t slot,
    const std::uint32_t participants, const std::int32_t group)
{
    YobiAction* const scene = NewYobiAction_404AE0(1, sizeof(BattleScenePayload), YobiPayloadKind::BattleScene);
    if (scene == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = scene->flags | YobiActionFlag_Hidden;
    auto* const payload = scene->payload.battleScene;
    scene->flags = flags;
    scene->callback = UpdateBattleEvent_427470;
    scene->imageNo = 0;
    scene->phase = kind;
    payload->packetSubState58 = 0;
    payload->eventSlot60 = slot;
    const std::int32_t offset = cg::world::gBattleMScanOffset5A8A3C;
    payload->entryMaskA8 = participants;
    payload->packetOffset64 = offset;
    for (std::int32_t position = 0; position < 22; ++position) {
        if ((payload->entryMaskA8 & (1U << position)) != 0) {
            InitBattleCastList_4390E0(position);
        }
    }
    cg::world::gBattleEventActions5ABA9C[payload->eventSlot60] = scene;
    payload->waitCountAC = 0;
    payload->groupBase68 = 22 * group;
    return scene;
}

// 原函数：0x004383B0。
YobiAction* NewBattleScene_4383B0()
{
    YobiAction* const scene = NewYobiAction_404AE0(1, sizeof(BattleScenePayload), YobiPayloadKind::BattleScene);
    if (scene == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = scene->flags;
    scene->callback = DispatchBattleEvents_43B360;
    const std::uint32_t updatedFlags = flags | YobiActionFlag_Hidden;
    scene->imageNo = 0;
    scene->flags = updatedFlags;
    scene->payload.battleScene->entryMaskA8 = 0xFFFFFFFFU;
    return scene;
}

// 原函数：0x0043B6C0。
void FinishBattleEvent_43B6C0(YobiAction* const action)
{
    auto* const payload = action->payload.battleScene;
    const std::int32_t remaining = payload->remainingMessages5C - 1;
    payload->remainingMessages5C = remaining;
    if (remaining > 0) {
        for (;;) {
            const std::int32_t offset = payload->packetOffset64;
            const auto value = std::bit_cast<std::int8_t>(
                static_cast<std::uint8_t>(cg::world::gBattleMText5AAA7C[offset]));
            if (value == 0 || (value >= 'A' && value <= 'Z')) {
                break;
            }
            payload->packetOffset64 = offset + 1;
        }
        // 原43B6FD在NUL处也复制并推进，不复用带NUL早退的423960。
        std::array<char, 4> header;
        std::strncpy(header.data(), cg::world::gBattleMText5AAA7C.data() + payload->packetOffset64, static_cast<std::uint32_t>(3));
        const std::int32_t next = payload->packetOffset64 + 4;
        header[3] = '\0';
        payload->packetOffset64 = next;
        std::int32_t kind = 0;
        while (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())
            && const_cast<char*>(std::strstr(header.data(), kBattleMHeaders553E94[kind])) == nullptr) {
            ++kind;
        }
        if (kind < static_cast<std::int32_t>(kBattleMHeaders553E94.size())) {
            action->phase = static_cast<std::int32_t>(kind);
        }
        payload->packetSubState58 = 0;
        payload->groupBase68 = 0;
        for (std::int32_t index = 0; index < 22; ++index) {
            const std::uint32_t eventMask = payload->entryMaskA8;
            const std::uint32_t bit = 1u << index;
            if ((eventMask & bit) != 0) {
                InitBattleCastList_4390E0(index);
            }
        }
        cg::world::gBattleEventDispatchBusy_5ABB1C = 0;
        payload->waitCountAC = 0;
    } else {
        const std::uint32_t eventBit = 1u << payload->eventSlot60;
        const std::uint32_t activeEvents = cg::world::gBattleActiveEventMask5AAA74;
        cg::world::gBattleActiveEventMask5AAA74 = activeEvents & ~eventBit;
        cg::world::gBattleEventActions5ABA9C[payload->eventSlot60] = nullptr;
        for (std::int32_t index = 0; index < 22; ++index) {
            const std::uint32_t eventMask = payload->entryMaskA8;
            const std::uint32_t bit = 1u << index;
            if ((eventMask & bit) != 0) {
                InitBattleCastList_4390E0(index);
            }
        }
        cg::world::gBattleEventDispatchBusy_5ABB1C = 0;
        static_cast<void>(MarkDeath(action));
    }
}

// 原函数：0x0043C0A0。
void DrawBattleEntryHover_43C0A0(const std::int32_t index)
{
    YobiAction* const action = world::gBattleEntries59E980[index];
    if (action == nullptr || (GetYobiActionFlagsLowByte(action) & YobiActionFlag_PendingValue2) == 0) {
        return;
    }
    const std::int32_t graphic = action->graphicIndex;
    const render::GraphicDxCache* const entries = render::gGraphicDxCache103CFA4;
    const std::uint32_t hoverImage = cg::render::gHoverFramePaletteIndex_D69768;
    const std::int32_t y = action->y;
    const std::int32_t offsetX = action->graphicOffsetX;
    const std::int32_t offsetY = action->graphicOffsetY;
    const std::int32_t x = action->x;
    const std::int32_t height = render::ReadGraphicDxHeightFromEntries(entries, graphic);
    const std::int32_t width = render::ReadGraphicDxWidthFromEntries(entries, graphic);
    cg::render::DrawGraphicHover(
        static_cast<std::uint16_t>(x + offsetX - 2),
        static_cast<std::uint16_t>(y + offsetY - 2),
        static_cast<std::uint16_t>(x + offsetX + width + 2),
        static_cast<std::uint16_t>(y + offsetY + height + 2),
        206, hoverImage, 0);
}

} // namespace detail



// 原函数：0x004383F0。
void InitializeBattleScene_4383F0()
{
    cg::world::gBattleSceneAction5ABA94 = detail::NewBattleScene_4383B0();
    cg::world::gBattleEventActions5ABA9C.fill(nullptr);
    cg::world::gBattleAllEntries59E980.fill(nullptr);
    cg::world::gBattleActiveEventMask5AAA74 = 0;
    detail::gBattlePendingGroupCount5ABB24 = 0;
    cg::world::gBattleExitRequested_5ABB20 = 0;
    gYobiScreenOffsetRising = 0;
    gYobiScreenOffsetAmplitude = 0;
    gYobiUpdateGateMode = YobiUpdateGatePolicy_FullRate;
    ui::detail::WindowBattleCommand_SnapshotPetVitals();
}


// 原函数：0x004990A0。
void DrawBattleActorHpBar_4990A0(const std::int32_t index)
{
    YobiAction* const initialAction = world::gBattleEntries59E980[index];
    if (initialAction->callback != nullptr
        && !(initialAction->readiness <= 0)) {
        YobiAction* const action = world::gBattleEntries59E980[index];
        if ((action->flags & YobiActionFlag_HideBattleHpBar) == 0) {
            static_cast<void>(detail::ShowHpBar_43DB60(action, -10, 3, -1));
        }
    }
}


// 原函数：0x0043BFF0。
void EnableCompletedBattleActorIdleAnimation_43BFF0()
{
    if (world::gCurrentSceneType != 10
        || cg::world::gBattleTurnSnapshot5ABC08 != cg::world::gBattleTurn5ABB74) {
        return;
    }
    for (std::size_t index = 0; index < cg::world::gBattleEntries59E980.size(); ++index) {
        YobiAction* const action = cg::world::gBattleEntries59E980[index];
        if (action != nullptr && ((std::bit_cast<std::int32_t>(cg::world::gBattleEndBits5ABBC0) >> index) & 1) != 0) {
            action->flags |= YobiActionFlag_BattleIdleAnimation;
        }
    }
}


// 原函数：0x0043C040。
void DisableBattleActorIdleAnimation_43C040()
{
    for (YobiAction* const action : world::gBattleEntries59E980) {
        if (action != nullptr) {
            action->flags &= ~YobiActionFlag_BattleIdleAnimation;
        }
    }
}


// 原函数：0x0043C120。
std::int32_t GetSelectedBattleEntry_43C120()
{
    const std::uint32_t target = world::gBattleTargetFlags5ABC44;
    const std::int32_t hitIndex = render::gHoveredGraphicPendingIndex;
    static constexpr std::int32_t rows[4][5] {
        {14, 12, 10, 11, 13},
        {19, 17, 15, 16, 18},
        {9, 7, 5, 6, 8},
        {4, 2, 0, 1, 3},
    };
    for (std::int32_t position = 0; position < 20; ++position) {
        const YobiAction* const action = world::gBattleEntries59E980[position];
        if (action == nullptr || action->cacheIndex != hitIndex
            || (GetYobiActionFlagsLowByte(action) & YobiActionFlag_PendingValue2) == 0) {
            continue;
        }
        if ((target & detail::BattleTargetFlag_Single) != 0) {
            return position;
        }
        if ((target & detail::BattleTargetFlag_CrossNeighbors) != 0) {
            std::int32_t row = position;
            std::int32_t column = position;
            for (std::int32_t r = 0; r < 4; ++r) {
                for (std::int32_t c = 0; c < 5; ++c) {
                    if (rows[r][c] == position) {
                        row = r;
                        column = c;
                    }
                }
            }
            // 四行完整覆盖0..19；原排配对0/1、2/3，先纵向再右邻、左邻。
            detail::DrawBattleEntryHover_43C0A0(rows[row ^ 1][column]);
            if (column + 1 <= 4) {
                detail::DrawBattleEntryHover_43C0A0(rows[row][column + 1]);
            }
            if (column - 1 >= 0) {
                detail::DrawBattleEntryHover_43C0A0(rows[row][column - 1]);
            }
            return position + detail::BattleTargetCode_CrossNeighborBase;
        }
        if ((target & detail::BattleTargetFlag_Team) != 0) {
            const std::int32_t first = position < 10 ? 0 : 10;
            for (std::int32_t index = first; index < first + 10; ++index) {
                if (index != position) {
                    detail::DrawBattleEntryHover_43C0A0(index);
                }
            }
            return position < 10 ? detail::BattleTargetCode_Team0 : detail::BattleTargetCode_Team1;
        }
        if ((target & detail::BattleTargetFlag_All) != 0) {
            for (std::int32_t index = 0; index < 20; ++index) {
                if (index != position) {
                    detail::DrawBattleEntryHover_43C0A0(index);
                }
            }
            return detail::BattleTargetCode_All;
        }
    }
    return detail::BattleTargetCode_None;
}


// 原函数：0x0043C910。
std::int32_t ReturnBattleActorsToFormation_43C910()
{
    bool pending = false;
    for (YobiAction* const action : world::gBattleEntries59E980) {
        if (action != nullptr
            && (*reinterpret_cast<const std::uint8_t*>(
                &static_cast<const detail::BattleActorPayload*>(action->payload.battleActor)->flagsFDC) & detail::BattleActorFlag_EntryComplete) == 0) {
            action->phase = detail::BattleAction_EnterFormation;
            pending = true;
        }
    }
    return pending ? 0 : 1;
}


// 原函数：0x0043C9A0。
void OrientEnteringBattleSides_43C9A0()
{
    const auto turnEntries = [](const std::int32_t first, const std::int32_t end) {
        for (std::int32_t index = first; index < end; ++index) {
            YobiAction* const action = cg::world::gBattleEntries59E980[index];
            if (action != nullptr) {
                const auto* const payload = static_cast<const detail::BattleActorPayload*>(action->payload.battleActor);
                const double yEnd = payload->y0C;
                const double yStart = payload->y04;
                const float y = static_cast<float>(yEnd - yStart);
                const double xEnd = payload->x08;
                const double xStart = payload->x00;
                const float x = static_cast<float>(xEnd - xStart);
                const float angle = static_cast<float>(cg::world::GetVectorAngleDegrees_46F970(x, y));
                cg::world::gBattleEntries59E980[index]->angle50 = angle;
                detail::UpdateBattleDirectionFromAngle_405CB0(cg::world::gBattleEntries59E980[index]);
                static_cast<void>(detail::PlayFrame_517DA0(cg::world::gBattleEntries59E980[index], AnimationPlayback_StopAtEnd));
            }
        }
    };
    const std::uint8_t firstFlags =
        *reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC);
    if ((firstFlags & detail::BattlePlayerFlag_PlayerSurprise) != 0) {
        turnEntries(10, 20);
    }
    const std::uint8_t secondFlags =
        *reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC);
    if ((secondFlags & detail::BattlePlayerFlag_EnemySurprise) != 0) {
        turnEntries(0, 10);
    }
}


// 原函数：0x0043CA50。
std::int32_t Sub_43CA50()
{
    std::int32_t count = 0;
    for (const YobiAction* const action : world::gBattleEntries59E980) {
        if (action != nullptr
            && (*reinterpret_cast<const std::uint8_t*>(
                &static_cast<const detail::BattleActorPayload*>(action->payload.battleActor)->flagsFDC) & detail::BattleActorFlag_Player) != 0) {
            ++count;
        }
    }
    return count;
}


// 原函数：0x0043CA80。
void DrawBattleActorNames_43CA80()
{
    std::int8_t color = 0;
    if (platform::gActorOverheadTextMode_5F974C == 0) {
        return;
    }
    for (std::int32_t index = 0; index < 20; ++index) {
        if (cg::world::gBattleEntries59E980[index] == nullptr) {
            continue;
        }
        const std::int32_t position = cg::world::gBattleCurrentPosition5ABCA4;
        if (position < 20) {
            color = index == position || index == cg::world::GetBattlePetPosition_405B40(position) ? 4 : 0;
        }
        const YobiAction* const action = cg::world::gBattleEntries59E980[index];
        const wchar_t* const name = action->nameC4.data();
        const std::int32_t y = action->y - 24;
        const std::int32_t width = cg::render::TextMeasure(name, 1);
        const std::int32_t x = cg::world::gBattleEntries59E980[index]->x - width / 2;
        static_cast<void>(render::DrawText2_456670(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)),
            0, 1, color, name, 0));
    }
}


// 原函数：0x0043C560。
std::int32_t CheckBattleCommand_43C560(
    const std::int32_t position, const std::uint32_t flags)
{
    cg::world::gBattleTargetFlags5ABC44 = flags;
    const std::int32_t pet = cg::world::GetBattlePetPosition_405B40(position);
    if ((flags & detail::BattleTargetFlag_Select) == 0) {
        return 0;
    }
    if ((flags & detail::BattleTargetFlag_OwnPet) != 0) {
        YobiAction* const action = cg::world::gBattleEntries59E980[pet];
        if (action != nullptr
            && (!(action->readiness <= 0)
                || (flags & detail::BattleTargetFlag_AllowDead) != 0)) {
            cg::world::gBattleEntries59E980[pet]->flags |= YobiActionFlag_PendingValue2;
        }
    }
    if ((flags & detail::BattleTargetFlag_Self) != 0) {
        YobiAction* const action = cg::world::gBattleEntries59E980[position];
        if (action != nullptr
            && (!(action->readiness <= 0)
                || (flags & detail::BattleTargetFlag_AllowDead) != 0)) {
            cg::world::gBattleEntries59E980[position]->flags |= YobiActionFlag_PendingValue2;
        }
    }
    if ((flags & detail::BattleTargetMask_OtherAlliesOrPlayers) != 0) {
        const std::int32_t first = position < 10 ? 0 : 10;
        for (std::int32_t slot = first; slot < first + 10; ++slot) {
            if (slot == position || slot == pet) {
                continue;
            }
            YobiAction* const action = cg::world::gBattleEntries59E980[slot];
            if (action != nullptr
                && ((flags & detail::BattleTargetFlag_OtherAllies) != 0
                    || ((flags & detail::BattleTargetFlag_OtherAllyPlayers) != 0
                        && (*reinterpret_cast<const std::uint8_t*>(
                            &static_cast<const detail::BattleActorPayload*>(action->payload.battleActor)->flagsFDC) & detail::BattleActorFlag_Player) != 0))
                && (!(action->readiness <= 0)
                    || (flags & detail::BattleTargetFlag_AllowDead) != 0)) {
                cg::world::gBattleEntries59E980[slot]->flags |= YobiActionFlag_PendingValue2;
            }
        }
    }
    if ((flags & detail::BattleTargetFlag_Opponents) != 0) {
        const std::int32_t first = position < 10 ? 10 : 0;
        for (std::int32_t slot = first; slot < first + 10; ++slot) {
            YobiAction* const action = cg::world::gBattleEntries59E980[slot];
            if (action != nullptr
                && (!(action->readiness <= 0)
                    || (flags & detail::BattleTargetFlag_AllowDead) != 0)) {
                cg::world::gBattleEntries59E980[slot]->flags |= YobiActionFlag_PendingValue2;
            }
        }
    }
    static constexpr std::int32_t rows[4][5] {
        {14, 12, 10, 11, 13},
        {19, 17, 15, 16, 18},
        {9, 7, 5, 6, 8},
        {4, 2, 0, 1, 3},
    };
    if ((flags & detail::BattleTargetFlag_CheckMeleeRows) == 0
        || ((*reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC) & detail::BattlePlayerFlag_DirectWeapon) == 0
            && position == cg::world::gBattleCurrentPosition5ABCA4)) {
        return 1;
    }
    std::int32_t column = position;
    std::int32_t row = position;
    for (std::int32_t r = 0; r < 4; ++r) {
        for (std::int32_t c = 0; c < 5; ++c) {
            if (rows[r][c] == position) {
                column = c;
                row = r;
            }
        }
    }
    if (row == 0) {
        const std::int32_t front = rows[1][column];
        for (std::int32_t c = 0; c < 5; ++c) {
            const YobiAction* const ownFront = cg::world::gBattleEntries59E980[front];
            if (ownFront != nullptr && (*reinterpret_cast<const std::uint8_t*>(&ownFront->flags) & YobiActionFlag_PendingValue2) != 0) {
                const YobiAction* const enemyFront = cg::world::gBattleEntries59E980[rows[2][c]];
                if (enemyFront != nullptr && (*reinterpret_cast<const std::uint8_t*>(&enemyFront->flags) & YobiActionFlag_PendingValue2) != 0) {
                    YobiAction* const enemyBack = cg::world::gBattleEntries59E980[rows[3][c]];
                    if (enemyBack != nullptr) {
                        enemyBack->flags &= ~YobiActionFlag_PendingValue2;
                    }
                }
            }
        }
    } else if (row == 3) {
        const std::int32_t front = rows[2][column];
        for (std::int32_t c = 0; c < 5; ++c) {
            const YobiAction* const ownFront = cg::world::gBattleEntries59E980[front];
            if (ownFront != nullptr && (*reinterpret_cast<const std::uint8_t*>(&ownFront->flags) & YobiActionFlag_PendingValue2) != 0) {
                const YobiAction* const enemyFront = cg::world::gBattleEntries59E980[rows[1][c]];
                if (enemyFront != nullptr && (*reinterpret_cast<const std::uint8_t*>(&enemyFront->flags) & YobiActionFlag_PendingValue2) != 0) {
                    YobiAction* const enemyBack = cg::world::gBattleEntries59E980[rows[0][c]];
                    if (enemyBack != nullptr) {
                        enemyBack->flags &= ~YobiActionFlag_PendingValue2;
                    }
                }
            }
        }
    }
    return 1;
}


// 原函数：0x0043C060。
void FlushBattleEntries_43C060()
{
    auto& entries = world::gBattleEntries59E980;
    for (std::size_t slot = 0; slot < entries.size(); ++slot) {
        YobiAction* const action = entries[slot];
        if (action != nullptr) {
            action->flags &= ~YobiActionFlag_PendingValue2;
            entries[slot]->flags |= YobiActionFlag_PendingValue1;
            entries[slot]->cacheIndex = YobiGraphicCacheIndex_NotQueued;
        }
    }
}

} // namespace cg::yobi
