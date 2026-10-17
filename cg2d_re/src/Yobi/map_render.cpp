#include "cg/render/GraphicPendingQueue.hpp"
#include "map_render.hpp"
#include "base.hpp"
#include "window.hpp"
#include "map_actor.hpp"
#include "battle_base.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "../world/WorldObjectState.hpp"
#include "cg/world/Blacklist.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/SdlPresenter.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include "animation.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/BattleState.hpp"
#include "../world/ChatHistory.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <new>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>
#include <string_view>

namespace cg::yobi {

namespace detail {

// 原[554204,5542A4)的40项S32初值；4417A0按每25个图号一组读取。
static std::array<std::int32_t, 40> gActorHeightOffsets554204 { // 原全局：0x00554204。
    -78, -88, -88, -81, -88, -90, -99, -88, -88, -88,
    -79, -86, -88, -85, -88, -88, -96, -78, -88, -86,
    -88, -88, -81, -88, -90, -99, -79, -85, -88, -88,
    -96, -99, -88, -88, -88, -88, -88, -88, -88, -88,
};

static_assert(sizeof(gActorHeightOffsets554204) == 0xA0);

// 原5542F4/554338，分别为17项状态图号表和聊天泡开关。
static std::array<std::int32_t, 17> gActorStatusImages5542F4 { // 原全局：0x005542F4。
    0, 0x192BC, 0x1AF41, 0x1AF42, 0x1AF43, 0x1AF44,
    0x1AF45, 0x1AF46, 0x1AF47, 0x1AF48, 0x1AF49, 0x1AF4A,
    0x1AF4B, 0x1AF4C, 0x1AF4D, 0x1AF4E, 0x19708,
};

static std::uint32_t gChatBubbles554338 = 1; // 原全局：0x00554338。

// 原4441A0：一次动画完成时清可选父槽再标死，未完成则走地图入队。
static void Sub_4441A0(YobiAction* const action)
{
    const float worldY = action->worldY;
    const float worldX = action->worldX;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    if (Sub_517880(action, 100, 1, 0) != 0) {
        YobiAction** const slot = action->completionSlot10;
        if (slot != nullptr) {
            *slot = nullptr;
        }
        MarkDeath(action);
        return;
    }
    const render::GraphicDescriptor16* const descriptor = &action->drawMode;
    const std::int32_t offsetY = Sub_5182B0(action, 1);
    const std::int32_t drawY = offsetY + action->y;
    const std::int32_t offsetX = Sub_5182B0(action, 0);
    const std::int32_t x = action->x;
    const std::int32_t graphicIndex = action->graphicIndex;
    const std::int32_t drawX = offsetX + x;
    const auto worldYBits = std::bit_cast<std::uint32_t>(action->worldY);
    const auto worldXBits = std::bit_cast<std::uint32_t>(action->worldX);
    static_cast<void>(world::Sub_46BD60(
        graphicIndex, worldXBits, worldYBits,
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), 0, descriptor));
}

// 原444260：无payload附件，仅借用可选的父引用槽，不改变通用释放逻辑。
YobiAction* Sub_444260(
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t characterAction,
    const std::int32_t direction, const std::uint8_t displayPriority,
    YobiAction** const completionSlot)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->imageNo = imageNo;
    action->characterAction = characterAction;
    action->callback = Sub_4441A0;
    action->direction = direction;
    action->displayPriority = displayPriority;
    action->flags = 0x20u;
    (action->mapX200 = static_cast<std::int32_t>(mapX));
    (action->mapY210 = static_cast<std::int32_t>(mapY));
    (action->mapX1E0 = static_cast<std::int32_t>(mapX));
    (action->mapY1F0 = static_cast<std::int32_t>(mapY));
    const double scaledX = mapX * 64.0;
    action->field27C = 0;
    action->field280 = 0;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    action->completionSlot10 = completionSlot;
    return action;
}

// 原函数：0x004417A0。
std::int32_t Sub_4417A0(const std::int32_t imageNo)
{
    if (imageNo >= 100000 && imageNo <= 100999) {
        return gActorHeightOffsets554204[static_cast<std::size_t>((imageNo - 100000) / 25)];
    }
    return -88;
}

// 原函数：0x00444000。
YobiAction* Sub_444000(
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t characterAction,
    const std::int32_t direction, const std::uint8_t displayPriority)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->imageNo = imageNo;
    action->characterAction = characterAction;
    action->direction = direction;
    action->displayPriority = displayPriority;
    action->flags = 0x20u;
    // 原四次各先读取临时 encoded/mask/count，再按同序发布目标。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        target = static_cast<std::int32_t>(value);
    };
    assignCoordinate(action->mapX200, mapX);
    assignCoordinate(action->mapY210, mapY);
    assignCoordinate(action->mapX1E0, mapX);
    assignCoordinate(action->mapY1F0, mapY);
    const double scaledX = mapX * 64.0;
    action->field27C = 0;
    action->field280 = 0;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    return action;
}

// 原函数：0x004412D0。
void Sub_4412D0(
    YobiAction* const action, [[maybe_unused]] const std::int32_t primaryIndex,
    std::int32_t* const characterAction, std::int32_t* const revert)
{
    auto* const payload = action->payload.mapActor;
    for (std::int32_t slot = 0; slot < 2; ++slot) {
        const std::int32_t kind = slot == 0 ? payload->kind3C : payload->kind40;
        if (kind == 2) {
            *characterAction = 0;
            *revert = 0;
        } else if (kind >= 3 && kind <= 5) {
            switch (action->characterAction) {
            case 1: *characterAction = 1; *revert = 0; break;
            case 2: *characterAction = 2; *revert = 1; break;
            case 3: *characterAction = 3; *revert = 0; break;
            case 4: *characterAction = 4; *revert = 1; break;
            case 5: *characterAction = 5; *revert = 1; break;
            case 6: *characterAction = 6; *revert = 0; break;
            case 8: *characterAction = 8; *revert = 1; break;
            case 9: *characterAction = 9; *revert = 1; break;
            case 10: *characterAction = 10; *revert = 1; break;
            case 0:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
                *characterAction = 0;
                *revert = 0;
                break;
            default:
                *characterAction = 0;
                *revert = 1;
                break;
            }
        } else if (action->characterAction == 10) {
            *characterAction = 10;
            *revert = 1;
        } else {
            *characterAction = 0;
            *revert = 0;
        }
    }
}

// 原函数：0x00441430。
void Sub_441430(
    YobiAction* const action, [[maybe_unused]] const std::int32_t primaryIndex,
    std::int32_t* const x, std::int32_t* const y, std::uint8_t* const before)
{
    auto* const payload = action->payload.mapActor;
    for (std::int32_t slot = 0; slot < 2; ++slot) {
        const std::int32_t kind = slot == 0 ? payload->kind3C : payload->kind40;
        if (kind < 3 || kind > 5) {
            continue;
        }
        switch (action->direction) {
        case 0: *x = 34; *y = -4; break;
        case 1: *x = 36; *y = 16; break;
        case 2: *x = 16; *y = 24; break;
        case 3: *x = -16; *y = 26; break;
        case 4: *x = -34; *y = 8; break;
        case 5: *x = -36; *y = -12; break;
        case 6: *x = -20; *y = -20; break;
        case 7: *x = 16; *y = -26; break;
        default: break;
        }
        switch (action->direction) {
        case 1:
        case 2:
        case 3:
        case 4:
            *before = 1;
            break;
        case 0:
        case 5:
        case 6:
        case 7:
            *before = 0;
            break;
        default: break;
        }
    }
}

// YGOS01：两借用槽/六kind完整消费，1919由CAC07数据关联补正常父合同，正确绘制正文保持。
// 原函数：0x00441570。
void DrawGear_441570(
    YobiAction* const action, const std::int32_t primaryIndex,
    const std::int32_t offsetX, const std::int32_t offsetY,
    const std::uint8_t before)
{
    auto* const payload = action->payload.mapActor;
    for (std::int32_t slot = 0; slot < 2; ++slot) {
        const std::int32_t kind = slot == 0 ? payload->kind3C : payload->kind40;
        YobiAction* const child = slot == 0 ? payload->action34 : payload->action38;
        if (child == nullptr) {
            continue;
        }
        if (kind >= 3 && kind <= 5) {
            const render::GraphicDescriptor16* const descriptor = &child->drawMode;
            const std::int32_t childY = Sub_518300(child, 1, kind);
            const std::int32_t drawY = childY + action->y + offsetY;
            const std::int32_t childX = Sub_518300(child, 0, kind);
            const std::int32_t drawX = childX + action->x + offsetX;
            const std::int32_t graphicIndex = child->graphicIndex;
            world::DrawGearNormal_46C060(
                primaryIndex, before, graphicIndex,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                descriptor, static_cast<std::uint8_t>(kind + 7));
        } else if (kind == 1 || kind == 2 || kind == 6) {
            const render::GraphicDescriptor16* const descriptor = &child->drawMode;
            const std::int32_t childY = Sub_5182B0(child, 1);
            const std::int32_t drawY = childY + action->y;
            const std::int32_t childX = Sub_5182B0(child, 0);
            const std::int32_t drawX = childX + action->x;
            const std::int32_t graphicIndex = child->graphicIndex;
            world::DrawGearNormal_46C060(
                primaryIndex, kind == 1 ? 0 : 1, graphicIndex,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                descriptor, 0);
        }
    }
}

// 原函数：0x00442F70。
void Sub_442F70(YobiAction* const action, const std::int32_t primaryIndex)
{
    if (action == nullptr) {
        return;
    }
    std::int32_t frameX = action->frameX;
    auto* const payload = action->payload.mapActor;
    std::int32_t frameY = action->frameY;
    YobiAction* const initialRide = payload->action44;
    std::int32_t stateIndex = payload->index;
    if (initialRide != nullptr) {
        frameX += initialRide->frameX;
        frameY += initialRide->frameY;
    }

    std::uint16_t flags;
    std::uint16_t state;
    std::int32_t value;
    bool drawBars = false;
    YobiAction* const player = world::gPlayerRuntimeSlots.action;
    if ((player != nullptr && player == action) || stateIndex == -1) {
        flags = static_cast<std::uint16_t>(world::ReadPlayerFlags18A());
        state = world::ReadPlayerActionFlags();
        value = world::gPlayerRuntimeSlots.value190;
        if ((action->flags & 0x80000u) != 0) {
            flags &= 0x8400u;
            state = static_cast<std::uint16_t>(
                ((state & 8u) != 0 ? 1u : 0u)
                | ((state & 0x10u) != 0 ? 4u : 0u)
                | ((state & 0x40u) != 0 ? 0x20u : 0u));
            for (const world::PetRuntimeRecord& pet : world::gPetRuntimeRecords) {
                if (pet.used != 0 && pet.follow6D4 != 0) {
                    action->readiness = pet.values0C[0];
                    action->maxHp = pet.values0C[1];
                    action->barValue1B0 = pet.values0C[2];
                    action->barMaximum1C0 = pet.values0C[3];
                    drawBars = true;
                    break;
                }
            }
        } else {
            world::CopyPlayerBarValues(
                &action->readiness, &action->maxHp, &action->barValue1B0, &action->barMaximum1C0);
            drawBars = true;
        }
    } else {
        const std::uint16_t originalFlags = world::detail::gWorldObjects[stateIndex].flags42;
        state = world::detail::gWorldObjects[stateIndex].flags120;
        value = world::detail::gWorldObjects[stateIndex].value124;
        flags = originalFlags;
        if ((action->flags & 0x80000u) != 0) {
            flags &= 0x8400u;
            state = static_cast<std::uint16_t>(
                ((state & 8u) != 0 ? 1u : 0u)
                | ((state & 0x10u) != 0 ? 4u : 0u)
                | ((state & 0x40u) != 0 ? 0x20u : 0u));
        }
        drawBars = (originalFlags & 0x200u) != 0;
    }
    if (world::gActorBarsEnabled5AC490 == 0) {
        drawBars = false;
    }

    bool createFirst = action->refresh2EC != 0 && (flags & 0x100u) != 0;
    if (!createFirst && (flags & 0x100u) != 0 && payload->action04 == nullptr) {
        createFirst = (flags & 0x400u) == 0
            || (flags & 0x4000u) == 0 || (flags & 0x8000u) != 0;
    }
    if (createFirst) {
        const std::int32_t kind = action->field2E4;
        action->refresh2EC = 0;
        const std::int32_t mapY = action->mapY1F0.value();
        const std::int32_t mapX = action->mapX1E0.value();
        const std::int32_t imageNo = kind == 1 || kind == 2 ? action->imageNo2E8 : 0x192BC;
        payload->action04 = Sub_444000(imageNo, mapX, mapY, 0, 0, 10);
    } else if (((flags & 0x100u) == 0 || (flags & 0x400u) != 0)
               && payload->action04 != nullptr) {
        MarkDeath(payload->action04);
        payload->action04 = nullptr;
    } else if ((flags & 0x100u) != 0) {
        YobiAction* const initialChild = payload->action04;
        if (initialChild != nullptr) {
            const std::int32_t parentX = action->x;
            initialChild->x = parentX;
            YobiAction* const yChild = payload->action04;
            const std::int32_t parentY = action->y;
            yChild->y = parentY;
            YobiAction* const worldXChild = payload->action04;
            const float worldX = action->worldX;
            worldXChild->worldX = worldX;
            YobiAction* const worldYChild = payload->action04;
            const float worldY = action->worldY;
            worldYChild->worldY = worldY;
            YobiAction* const animatedChild = payload->action04;
            Sub_517880(animatedChild, 100, 0, 0);
            const std::int32_t imageNo = action->imageNo;
            const std::int32_t height = Sub_4417A0(imageNo);
            YobiAction* const yOffsetChild = payload->action04;
            const std::int32_t offsetY = Sub_5182B0(yOffsetChild, 1);
            const std::int32_t currentY = action->y;
            const std::int32_t heightOffsetY = height + offsetY;
            YobiAction* const xOffsetChild = payload->action04;
            const std::int32_t drawY = heightOffsetY + currentY;
            const std::int32_t offsetX = Sub_5182B0(xOffsetChild, 0);
            const std::int32_t currentX = action->x;
            YobiAction* const graphicChild = payload->action04;
            const std::int32_t drawX = offsetX + currentX;
            const std::int32_t graphicIndex = graphicChild->graphicIndex;
            world::DrawGearNormal_46C060(
                primaryIndex, 1, graphicIndex,
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)), nullptr, 0);
        }
    }

    if ((flags & 0x2000u) != 0) {
        const std::uint64_t now = platform::ReadTickCount64();
        const std::array<std::wstring, 2>* drawableText = nullptr;
        if (payload->chatDeadline14 <= now) {
            auto* const expiredText = payload->text1C;
            flags &= 0xDFFFu;
            if (expiredText != nullptr) {
                delete expiredText;
                payload->text1C = nullptr;
            }
        } else if (gChatBubbles554338 != 0 && (action->flags & 0x180u) != 0
                   && (drawableText = payload->text1C) != nullptr) {
            // 原var_114与对象索引别名；包括零长度，后面的共享状态清位仍用此值。
            stateIndex = payload->textLength24;
            if (stateIndex != 0) {
                const std::int32_t firstMeasureFont = payload->font28;
                std::int32_t width = cg::render::TextMeasure((*drawableText)[0], firstMeasureFont);
                const std::int32_t height = Sub_4417A0(action->imageNo);
                std::int32_t y = height + action->y - 32;
                const std::int32_t metricFont = payload->font28;
                const std::int32_t bottom =
                    render::gFontMetrics[metricFont].doubleHeight + y + 8;
                if (!(*payload->text1C)[1].empty()) {
                    const std::wstring& secondMeasureText = (*payload->text1C)[1];
                    const std::int32_t secondWidth =
                        cg::render::TextMeasure(secondMeasureText, metricFont);
                    if (width < secondWidth) {
                        width = secondWidth;
                    }
                    const std::wstring& text = (*payload->text1C)[1];
                    const auto color = std::bit_cast<std::int8_t>(
                        *reinterpret_cast<const std::uint8_t*>(&payload->color20));
                    const auto font = std::bit_cast<std::int8_t>(
                        *reinterpret_cast<const std::uint8_t*>(&payload->font28));
                    const std::int32_t drawY = y + 4;
                    std::int16_t actionX;
                    std::memcpy(&actionX, &action->x, sizeof(actionX));
                    const auto x = static_cast<std::int16_t>(
                        static_cast<std::int32_t>(actionX) - width / 2);
                    cg::render::DrawText_456780(x, static_cast<std::int16_t>(drawY), 0, font, color, text, 0, 0);
                    y -= render::gFontMetrics[payload->font28].doubleHeight;
                }
                const std::int32_t halfWidth = width / 2;
                const std::wstring& text = (*payload->text1C)[0];
                const auto color = std::bit_cast<std::int8_t>(
                    *reinterpret_cast<const std::uint8_t*>(&payload->color20));
                const auto font = std::bit_cast<std::int8_t>(
                    *reinterpret_cast<const std::uint8_t*>(&payload->font28));
                std::int16_t actionX;
                std::memcpy(&actionX, &action->x, sizeof(actionX));
                const std::int32_t drawY = y + 4;
                const auto x = static_cast<std::int16_t>(
                    static_cast<std::int32_t>(actionX) - halfWidth);
                cg::render::DrawText_456780(x, static_cast<std::int16_t>(drawY), 0, font, color, text, 0, 0);
                const std::int32_t borderActionX = action->x;
                DrawNineGrid_49BE00(
                    0x3BE84, static_cast<std::int16_t>(borderActionX - halfWidth - 5),
                    static_cast<std::int16_t>(y - 1), static_cast<std::int16_t>(width + 10),
                    static_cast<std::int16_t>(bottom - y + 2), 0xCE, -1,
                    std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
            }
        } else {
            const std::int32_t y = action->y - 84;
            const std::int32_t x = action->x - 20;
            cg::render::DrawGraphic(x, y, 0xCE, 0x573F, 0, nullptr);
        }
    }

    const std::int16_t nameMode = platform::gValue5F974C;
    if (nameMode > 0) {
        const char* text = nullptr;
        std::int8_t color = 0;
        if (action == world::gPlayerRuntimeSlots.action) {
            if (nameMode <= 6) {
                constexpr std::array<std::int8_t, 6> colors {1, 4, 5, 2, 0, 0};
                text = world::GetPlayerDisplayText(nameMode);
                color = colors[nameMode - 1];
            }
        } else {
            const std::uint32_t actionFlags = action->flags;
            if ((actionFlags & 0x100u) != 0 && (actionFlags & 0x80000u) == 0) {
                if (action->imageNo != 0) {
                    switch (nameMode) {
                    case 1: text = action->nameC4.data(); color = 0; break;
                    case 2: text = action->titleE1.data(); color = 1; break;
                    case 3: text = action->text144.data(); color = 4; break;
                    case 4: text = action->text102.data(); color = 5; break;
                    case 5: text = action->text123.data(); color = 2; break;
                    case 6: text = action->text31C.data(); color = 2; break;
                    default: break;
                    }
                }
            } else if ((actionFlags & 0x80200u) != 0) {
                text = (actionFlags & 0x200u) != 0 && std::strlen(action->titleE1.data()) != 0
                    ? action->titleE1.data() : action->nameC4.data();
            }
        }
        if (text != nullptr) {
            std::wstring buffer;
            cg::text::FormatTo(buffer, L"{}", cg::text::FromGbkWide(text).c_str());
            const auto y = static_cast<std::int16_t>(action->y - 98);
            const std::int32_t width = cg::render::TextMeasure(buffer, 1);
            const auto x = static_cast<std::int16_t>(action->x - width / 2);
            cg::render::DrawText_456780(x, y, 0, 1, color, buffer, 0, 0);
        }
    }

    // 三图标的共同定位按每个原取槽点重读，不捕获一份子指针跨越查询调用。
    const auto positionStatus = [&](YobiAction*& slot) {
        const std::int32_t offsetX = Sub_5182B0(slot, 0);
        slot->x = offsetX + action->x;
        const std::int32_t height = Sub_4417A0(action->imageNo);
        const std::int32_t offsetY = Sub_5182B0(slot, 1);
        slot->y = height + offsetY + action->y - 20;
        const std::int32_t worldX = Sub_5182B0(slot, 0);
        slot->worldX = static_cast<float>(static_cast<double>(worldX) + action->worldX);
        const std::int32_t worldY = Sub_5182B0(slot, 1);
        slot->worldY = static_cast<float>(
            static_cast<double>(worldY) + action->worldY + 0.1F);
    };
    const auto drawStatus = [&](YobiAction*& slot) {
        Sub_517880(slot, 100, 0, 0);
        slot->drawMode.mixFlags |= cg::render::GraphicMixFlag_OriginalPalette;
        YobiAction* const child = slot;
        const std::int32_t y = child->y;
        const render::GraphicDescriptor16* const descriptor = &child->drawMode;
        const std::int32_t x = child->x;
        const std::int32_t graphicIndex = child->graphicIndex;
        world::DrawGearNormal_46C060(
            primaryIndex, 1, graphicIndex,
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), descriptor, 0);
    };
    const auto clearActionState = [&](const std::uint16_t bits) {
        YobiAction* const currentPlayer = world::gPlayerRuntimeSlots.action;
        if ((currentPlayer != nullptr && currentPlayer == action) || stateIndex == -1) {
            world::ClearPlayerActionFlags(bits);
        } else {
            world::detail::gWorldObjects[stateIndex].flags120 &=
                static_cast<std::uint16_t>(~bits);
        }
    };

    const std::uint16_t statusMask = flags & 0xC400u;
    if (statusMask != 0 || (state & 2u) != 0 || (state & 0x1000u) != 0) {
        std::int32_t statusImage = 0;
        if ((state & 2u) != 0) {
            if (value < 0 || value >= world::Sub_4410E0()) {
                value = 0;
            }
            statusImage = gActorStatusImages5542F4[value];
        }
        if ((flags & 0x400u) != 0) {
            statusImage = 0x1AF41;
        }
        if ((flags & 0x8000u) != 0) {
            statusImage = 0x1AF42;
        }
        if ((flags & 0x4000u) != 0) {
            statusImage = 0x1AF43;
        }
        if ((state & 0x1000u) != 0) {
            statusImage = 0x1824F;
        }
        if (payload->action4C == nullptr) {
            const std::int32_t mapY = action->mapY1F0.value();
            const std::int32_t mapX = action->mapX1E0.value();
            YobiAction* const child = Sub_444000(statusImage, mapX, mapY, 0, 0, 10);
            payload->action4C = child;
            child->rebirthValue1 = 4;
            payload->action4C->rebirthValue2 = 4;
        }
        if (payload->action4C != nullptr) {
            payload->action4C->imageNo = statusImage;
            positionStatus(payload->action4C);
            if ((state & 1u) != 0 && statusMask == 0) {
                payload->action4C->x -= 17;
            }
            drawStatus(payload->action4C);
        }
    } else if (payload->action4C != nullptr) {
        MarkDeath(payload->action4C);
        payload->action4C = nullptr;
    }

    if ((state & 4u) != 0) {
        if (payload->action4C == nullptr) {
            if (payload->action0C == nullptr) {
                const std::int32_t mapY = action->mapY1F0.value();
                const std::int32_t mapX = action->mapX1E0.value();
                payload->action0C = Sub_444000(0x1AF4D, mapX, mapY, 0, 0, 10);
                if ((state & 0x20u) != 0) {
                    clearActionState((action->flags & 0x80000u) != 0 ? 0x40 : 0x20);
                    static_cast<void>(audio::PlaySe_52C450(0x1B5, action->x));
                }
                payload->action0C->counter1DC = 0;
                payload->action0C->rebirthValue1 = 4;
                payload->action0C->rebirthValue2 = 4;
            }
            if (payload->action0C != nullptr) {
                positionStatus(payload->action0C);
                drawStatus(payload->action0C);
            }
        }
        ++payload->action0C->counter1DC;
        if (payload->action0C->counter1DC > 600) {
            clearActionState((action->flags & 0x80000u) != 0 ? 0x10 : 4);
        }
    } else if (payload->action0C != nullptr) {
        MarkDeath(payload->action0C);
        payload->action0C = nullptr;
    }

    if ((state & 1u) != 0 && statusMask == 0) {
        if (payload->action10 == nullptr) {
            const std::int32_t mapY = action->mapY1F0.value();
            const std::int32_t mapX = action->mapX1E0.value();
            YobiAction* const child = Sub_444000(0x1AF4E, mapX, mapY, 0, 0, 10);
            payload->action10 = child;
            child->rebirthValue1 = 4;
            payload->action10->rebirthValue2 = 4;
        }
        if (payload->action10 != nullptr) {
            positionStatus(payload->action10);
            if (payload->action4C != nullptr) {
                payload->action10->x += 17;
            }
            drawStatus(payload->action10);
        }
    } else if (payload->action10 != nullptr) {
        MarkDeath(payload->action10);
        payload->action10 = nullptr;
    }

    if (payload->action30 != nullptr) {
        payload->action30->characterAction = action->characterAction == 8 ? 3 : 0;
        payload->action30->direction = action->direction;
        if (Sub_517880(payload->action30, 100, 0, 0) == 0) {
            const render::GraphicDescriptor16* const descriptor = &payload->action30->drawMode;
            const std::int32_t offsetY = Sub_5182B0(payload->action30, 1);
            const std::int32_t y = offsetY + action->y + frameY;
            const std::int32_t offsetX = Sub_5182B0(payload->action30, 0);
            const std::int32_t x = offsetX + action->x + frameX;
            const std::int32_t graphicIndex = payload->action30->graphicIndex;
            world::DrawGearNormal_46C060(
                primaryIndex, 1, graphicIndex, static_cast<std::int16_t>(x),
                static_cast<std::int16_t>(y), descriptor, 0);
        }
    }
    if ((payload->action34 != nullptr || payload->action38 != nullptr)
        && payload->action44 == nullptr) {
        std::int32_t characterAction = 0;
        std::int32_t revert = 1;
        std::int32_t x = 0;
        std::int32_t y = 0;
        std::uint8_t before = 1;
        if (payload->action34 != nullptr) {
            payload->action34->direction = action->direction;
        }
        if (payload->action38 != nullptr) {
            payload->action38->direction = action->direction;
        }
        Sub_4412D0(action, primaryIndex, &characterAction, &revert);
        Sub_441430(action, primaryIndex, &x, &y, &before);
        if (payload->kind3C == 6 && payload->action34->characterAction == 10) {
            payload->action34->graphicIndex = -1;
        }
        if (payload->kind40 == 6 && payload->action38->characterAction == 10) {
            payload->action38->graphicIndex = -1;
        }
        DrawGear_441570(action, primaryIndex, x, y, before);
        if (payload->action34 != nullptr) {
            payload->action34->characterAction = characterAction;
        }
        if (payload->action38 != nullptr) {
            payload->action38->characterAction = characterAction;
        }
        PlayFrame_517DA0(payload->action34, revert);
        PlayFrame_517DA0(payload->action38, revert);
    }
    if (drawBars) {
        ShowHpBar_43DB60(action, -10, 3, -1);
    }
}

// 原函数：0x0043DB60。
std::int32_t ShowHpBar_43DB60(
    YobiAction* const action, const std::int16_t offsetX,
    const std::int16_t offsetY, const std::int32_t color)
{
    NineGridDescriptorFields fields {};
    std::int32_t graphicIndex;
    resource::GetGraphicMapNoIndex(0x3BDF0, &graphicIndex);
    std::int16_t graphicX;
    std::int16_t graphicY;
    resource::GetGraphicXY(static_cast<std::uint32_t>(graphicIndex), &graphicX, &graphicY);
    fields.color = color;
    fields.mixFlags = render::GraphicMixFlag_OriginalPalette;
    fields.clip = 1;
    fields.sourceX = -16;
    fields.sourceY = -1;
    fields.height = 2;
    render::GraphicDescriptor16 descriptor = ToGraphicDescriptor(fields);
    if (action->maxHp.value() != 0) {
        const std::int32_t numerator =
            action->readiness.value() * 32;
        const std::int32_t maximum =
            action->maxHp.value();
        fields.width = static_cast<std::int16_t>(numerator / maximum);
        const std::int32_t y = action->y + offsetY - 2;
        const std::int32_t x = action->x + offsetX - 12;
        descriptor = ToGraphicDescriptor(fields);
        static_cast<void>(render::DrawGraphic3_520970(
            x, y, 0xCF, 0x3BDF0, 0, 1, 0, &descriptor));
    }
    if (action->barMaximum1C0.value() != 0) {
        const std::int32_t numerator =
            action->barValue1B0.value() * 32;
        const std::int32_t maximum =
            action->barMaximum1C0.value();
        fields.width = static_cast<std::int16_t>(numerator / maximum);
        const std::int32_t y = action->y + offsetY + 1;
        const std::int32_t x = action->x + offsetX - 12;
        descriptor = ToGraphicDescriptor(fields);
        static_cast<void>(render::DrawGraphic3_520970(
            x, y, 0xCF, 0x3BDEF, 0, 1, 0, &descriptor));
    }
    const std::int32_t y = action->y + offsetY;
    const std::int32_t x = action->x + offsetX;
    fields.clip = 0;
    descriptor = ToGraphicDescriptor(fields);
    return cg::render::DrawGraphic(x, y, 0xCF, 0x3BDEE, 0, &descriptor);
}

} // namespace detail

// 原函数：0x00441130。
YobiAction* NewYobiGearPet_441130(
    const std::int32_t imageNo, const std::int32_t mapX, const std::int32_t mapY)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }
    action->callback = nullptr;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->displayPriority = 10;
    action->flags = 0x34u;
    (action->mapX200 = static_cast<std::int32_t>(mapX));
    (action->mapY210 = static_cast<std::int32_t>(mapY));
    action->field270 = 0;
    (action->mapX1E0 = static_cast<std::int32_t>(mapX));
    (action->mapY1F0 = static_cast<std::int32_t>(mapY));
    const double scaledX = mapX * 64.0;
    action->field27C = 0;
    action->field280 = 0;
    action->phase = 0;
    const auto worldX = static_cast<float>(scaledX);
    action->worldX = worldX;
    const auto worldY = static_cast<float>(mapY * 64.0);
    action->worldY = worldY;
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    action->x = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenX + 0.5));
    action->y = static_cast<std::int32_t>(
        static_cast<std::int64_t>(screenY + 0.5));
    return action;
}

// 原函数：0x004410F0。
YobiAction* NewYobiGear_4410F0(const std::int32_t imageNo)
{
    YobiAction* const action = NewYobiAction_404AE0(2, 0);
    if (action == nullptr) {
        return nullptr;
    }

    action->callback = nullptr;
    action->imageNo = imageNo;
    action->characterAction = 0;
    action->displayPriority = 10;
    action->flags = 0x34;
    return action;
}

// 原函数：0x00507140。
void DescribeHoveredYobi_507140(const render::GraphicCacheItem32* const graphic)
{
    constexpr wchar_t kFull58AD04[] =
        L"等级 {:<3} {:<16}  生命 {:4}/{:<4}  魔力 {:4}/{:<4}";
    constexpr wchar_t kLevel58AD5C[] = L"等级 {:<3} {:<16}";

    YobiAction* action = graphic->reference.action;
    if (action == nullptr || (action->flags & 0x10u) == 0) {
        return;
    }
    const std::int32_t scene = world::gCurrentSceneType;
    std::int32_t companion = action->field1D8;
    cg::ui::detail::gHoverObjectValueD68AC8 = companion;
    if (scene == 9) {
        action = graphic->reference.action;
        if (action->imageNo <= 0) {
            return;
        }
        const std::uint32_t flags = action->flags;
        if ((flags & 0x80u) != 0) {
            cg::ui::detail::gHoverObjectValueD68AC8 = action->field1D8;
            YobiAction* const fixed = graphic->reference.action;
            const std::int32_t maxFp = cg::world::gPlayerRuntimeSlots.maxFp.value();
            const std::int32_t fp = cg::world::gPlayerRuntimeSlots.fp.value();
            const std::int32_t maxHp = cg::world::gPlayerRuntimeSlots.maxHp.value();
            const std::int32_t hp = cg::world::gPlayerRuntimeSlots.hp.value();
            cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, kFull58AD04, fixed->field1D0, cg::text::FromGbkWide(fixed->nameC4.data()).c_str(), hp, maxHp, fp, maxFp);
        } else if ((flags & 0x80000u) != 0) {
            cg::ui::detail::gHoverObjectValueD68AC8 = action->field1D8;
            YobiAction* const fixed = graphic->reference.action;
            // 原额外字符串实参不被本格式消费，不增加显示内容。
            cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, kLevel58AD5C, fixed->field1D0, cg::text::FromGbkWide(fixed->nameC4.data()).c_str(), cg::text::FromGbkWide(fixed->titleE1.data()).c_str());
        } else if ((flags & 0x100u) != 0) {
            cg::ui::detail::gHoverObjectValueD68AC8 = action->field1D8;
            YobiAction* const fixed = graphic->reference.action;
            cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, L"等级 {:<3} {:<16} [{:<16}]" /* 原常量参考：0x0058AD44。 */, fixed->field1D0, cg::text::FromGbkWide(fixed->nameC4.data()).c_str(), cg::text::FromGbkWide(fixed->titleE1.data()).c_str());
            const char* const title = graphic->reference.action->text144.data();
            if (std::strlen(title) != 0) {
                cg::ui::detail::gHoverTitleD68BF4 = std::format(L"{:<16}", cg::text::FromGbkWide(title));
                cg::ui::detail::gHoverTitleValueD69748 = 4;
            }
            const std::uint8_t control = platform::gKeyStates[0x11] & 2u;
            if (control != 0 && (platform::gKeyStates[0x2D] & 4u) != 0) {
                if (cg::world::blacklist::GetBlacklistCount_401190() >= 72) {
                    cg::world::Sub_44B050(L"你的黑名单已满，无法再加入" /* 原常量参考：0x00553294。 */, 4, 3);
                    return;
                }
                const char* const name = graphic->reference.action->nameC4.data();
                if (name == nullptr) {
                    return;
                }
                std::wstring message;
                if (cg::world::blacklist::ContainsBlacklistRecord_4013D0(cg::text::FromGbkWide(name)) == 0) {
                    const std::int8_t added = cg::world::blacklist::AddBlacklistRecord_401270(cg::text::FromGbkWide(graphic->reference.action->nameC4.data()));
                    cg::text::FormatTo(message, added != 0 ? L"已将{}加入黑名单" /* 原常量参考：0x005532C4。 */ : L"{}加入黑名单失败" /* 原常量参考：0x005532B0。 */, cg::text::FromGbkWide(graphic->reference.action->nameC4.data()).c_str());
                } else {
                    cg::text::FormatTo(message, L"{}已在黑名单中" /* 原常量参考：0x00554928。 */, cg::text::FromGbkWide(graphic->reference.action->nameC4.data()).c_str());
                }
                cg::world::Sub_44B050(message, 4, 3);
            } else if (control != 0 && (platform::gKeyStates[0x2E] & 4u) != 0) {
                const char* const name = graphic->reference.action->nameC4.data();
                if (name == nullptr) {
                    return;
                }
                std::wstring message;
                if (cg::world::blacklist::ContainsBlacklistRecord_4013D0(cg::text::FromGbkWide(name)) != 0) {
                    const std::int8_t removed = cg::world::blacklist::RemoveBlacklistRecord_4012F0(cg::text::FromGbkWide(graphic->reference.action->nameC4.data()));
                    cg::text::FormatTo(message, removed != 0 ? L"已将{}从黑名单中移除" /* 原常量参考：0x005532EC。 */ : L"从黑名单移除{}失败" /* 原常量参考：0x005532D8。 */, cg::text::FromGbkWide(graphic->reference.action->nameC4.data()).c_str());
                } else {
                    cg::text::FormatTo(message, L"{}不在黑名单中" /* 原常量参考：0x0058AD34。 */, cg::text::FromGbkWide(graphic->reference.action->nameC4.data()).c_str());
                }
                cg::world::Sub_44B050(message, 4, 3);
            }
        } else if ((flags & 0x200u) != 0) {
            const char* name = action->titleE1.data();
            if (*name == '\0') {
                name = action->nameC4.data();
            }
            cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, kLevel58AD5C, action->field1D0, cg::text::FromGbkWide(name).c_str());
            static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
        } else if ((flags & 0x400u) != 0) {
            cg::ui::detail::gHoverObjectValueD68AC8 = action->field1D8;
            YobiAction* const fixed = graphic->reference.action;
            const std::int32_t count = fixed->payload.mapActor->value18;
            if (count > 0) {
                cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, L"{}({})", cg::text::FromGbkWide(fixed->nameC4.data()).c_str(), count);
            } else {
                cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, L"{}", cg::text::FromGbkWide(fixed->nameC4.data()).c_str());
            }
            static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
        } else if ((flags & 0x800u) != 0) {
            cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, L"{}", cg::text::FromGbkWide(action->nameC4.data()).c_str());
            static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
        } else if ((flags & 0x1000u) != 0) {
            cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, L"{}", cg::text::FromGbkWide(action->nameC4.data()).c_str());
        }
        return;
    }
    if (scene != 10) {
        return;
    }

    const std::int32_t position = cg::world::gBattleCurrentPosition5ABCA4;
    action = graphic->reference.action;
    const auto* const payload = static_cast<const detail::BattleActorPayload*>(action->payload.battleActor);
    if (position < 10) {
        // G02：只对应参考首匹配507AD4，其他位置分支的门禁保持。
        if (payload != nullptr
            && (world::gBattleShowEnemyHpBars || payload->battlePosition18 < 10)) {
            if ((payload->flagsFDC & 0x2000u) != 0) {
                companion = 4;
            }
            cg::ui::detail::gHoverObjectValueD68AC8 = companion;
        } else {
            if ((action->flags & 0x02000000u) == 0) {
                cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, kLevel58AD5C, action->field1D0, cg::text::FromGbkWide(action->nameC4.data()).c_str());
            }
            return;
        }
    } else if (position < 20) {
        if (payload != nullptr && payload->battlePosition18 < 10) {
            if ((payload->flagsFDC & 0x2000u) != 0) {
                companion = 4;
            }
            cg::ui::detail::gHoverObjectValueD68AC8 = companion;
            YobiAction* const fixed = graphic->reference.action;
            cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, kLevel58AD5C, fixed->field1D0, cg::text::FromGbkWide(fixed->nameC4.data()).c_str());
            return;
        }
        if ((action->flags & 0x02000000u) != 0) {
            return;
        }
    } else {
        if (payload != nullptr && payload->battlePosition18 < 10) {
            if ((payload->flagsFDC & 0x2000u) != 0) {
                companion = 4;
            }
            cg::ui::detail::gHoverObjectValueD68AC8 = companion;
        }
        YobiAction* const fixed = graphic->reference.action;
        cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, kLevel58AD5C, fixed->field1D0, cg::text::FromGbkWide(fixed->nameC4.data()).c_str());
        return;
    }

    // 原507AEA/507B8F固定首对象供名称/等级，后续三个CryptInt仍逐次重取对象。
    YobiAction* const fixed = graphic->reference.action;
    const std::int32_t maxFp = fixed->barMaximum1C0.value();
    const std::int32_t fp = graphic->reference.action->barValue1B0.value();
    const std::int32_t maxHp = graphic->reference.action->maxHp.value();
    const std::int32_t hp = graphic->reference.action->readiness.value();
    cg::text::FormatTo(cg::ui::detail::gHoverTextD68AE8, kFull58AD04, fixed->field1D0, cg::text::FromGbkWide(fixed->nameC4.data()).c_str(), hp, maxHp, fp, maxFp);
}

} // namespace cg::yobi
