#include "cg/render/GraphicPendingQueue.hpp"
#include "../Yobi/map_render.hpp"
#include "../Yobi/base.hpp"
#include "../Yobi/window.hpp"
#include "../Yobi/map_actor.hpp"
#include "../Yobi/battle_base.hpp"
#include "../Yobi/battle_attachment.hpp"
#include "../Yobi/animation.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "WorldObjectState.hpp"
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
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "ChatHistory.hpp"
#include "PlayerState.hpp"
#include "WorldMapState.hpp"
#include "WorldUpdateDependencies.hpp"

#include <array>
#include <algorithm>
#include <new>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>
#include <string_view>

namespace cg::world::detail {

static void AllocateActorChatText(
    cg::yobi::detail::MapActorPayload* const payload, const std::wstring_view name,
    const std::wstring_view text, const std::int32_t color,
    const std::int32_t font)
{
    std::size_t offset = name.size() + 2;
    if (text.starts_with(L"[GP]")) { offset += 4; }
    else if (text.starts_with(L"[MIC]")) { offset += 5; }
    else if (text.starts_with(L"[家族]")) { offset += 4; }
    const std::wstring contents(text.substr((std::min)(offset, text.size())));
    const auto remaining = static_cast<std::int32_t>(cg::text::ToGbk(contents).size());
    const std::int32_t width = render::GetFontMetrics(font).sWidth * 40;
    std::size_t split = 0;
    while (split < contents.size()) {
        const std::size_t next = cg::text::Next(contents, split);
        if (split != 0 && render::Font::measure(contents.c_str(), font, static_cast<std::int32_t>(next)) > width) { break; }
        split = next;
    }
    auto* const allocation = new (std::nothrow) std::array<std::wstring, 2>{};
    payload->text1C = allocation;
    if (allocation == nullptr) { return; }
    payload->color20 = color;
    payload->textLength24 = remaining; // 保留原阶段后续使用的GBK数量／别名。
    payload->font28 = font;
    (*allocation)[0] = contents.substr(0, split);
    (*allocation)[1] = contents.substr(split);
}

// 原函数：0x00442DC0。
void SetWorldActorChatBubble_442DC0(
    cg::yobi::YobiAction* const action, const std::int32_t duration,
    const std::wstring_view text, const std::int32_t color,
    const std::int32_t font)
{
    if (action == nullptr) {
        return;
    }
    auto* const payload = action->payload.mapActor;
    auto* const flagBytes = reinterpret_cast<unsigned char*>(
        &(*action->ownerSession->WorldScene().objects)[payload->index].flags42);
    flagBytes[1] |= 0x20u;
    const std::uint64_t tick = platform::ReadTickCount64();
    payload->chatDeadline14 = tick + static_cast<std::uint64_t>(duration);
    FaceWorldActorTowardPlayer_43D860(*action->ownerSession, action);
    auto* const previousText = payload->text1C;
    if (previousText != nullptr) {
        delete previousText;
        payload->text1C = nullptr;
    }
    AllocateActorChatText(payload, action->nameC4.data(), text, color, font);
}

// 原函数：0x0051A6A0。
void SetPlayerChatBubble_51A6A0(
    session::ClientSession& session,
    const std::int32_t duration, const std::wstring_view text, const std::int32_t color,
    const std::int32_t font)
{
    cg::yobi::YobiAction* const action = session.WorldScene().player->action;
    if (action == nullptr) {
        return;
    }
    MarkPlayerChatActive(session);
    auto* const payload = action->payload.mapActor;
    const std::uint64_t tick = platform::ReadTickCount64();
    payload->chatDeadline14 = tick + static_cast<std::uint64_t>(duration);
    auto* const previousText = payload->text1C;
    if (previousText != nullptr) {
        delete previousText;
        payload->text1C = nullptr;
    }
    AllocateActorChatText(payload, session.WorldScene().player->action->nameC4.data(), text, color, font);
}

// 原函数：0x00442C00。
void ClearWorldActorLeaderMarker_442C00(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& record = (*action->ownerSession->WorldScene().objects)[payload->index];
    record.marker44 = 0;
    if (action->overheadMarkerMode2E4 != cg::yobi::OverheadMarkerMode_CustomImageKeepMarkerFlag) {
        record.flags42 &= 0xFEFFu;
    }
}

// 原函数：0x00442CA0。
void Sub_442CA0(cg::yobi::YobiAction* const action, const std::int32_t value)
{
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& record = (*action->ownerSession->WorldScene().objects)[payload->index];
    reinterpret_cast<unsigned char*>(&record.flags42)[1] |= 0x40u;
    record.value10C = value;
}

// 原函数：0x00442CD0。
std::uint16_t* Sub_442CD0(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags42;
    flags &= 0xBFFFu;
    return &flags;
}

// 原函数：0x00442D00。
void Sub_442D00(cg::yobi::YobiAction* const action, const std::int32_t value)
{
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& record = (*action->ownerSession->WorldScene().objects)[payload->index];
    reinterpret_cast<unsigned char*>(&record.flags42)[1] |= 0x80u;
    record.value10C = value;
}

// 原函数：0x00442D30。
std::uint16_t* Sub_442D30(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags42;
    flags &= 0x7FFFu;
    return &flags;
}

// 原函数：0x00442D60。
void Sub_442D60(cg::yobi::YobiAction* const action, const std::int32_t value)
{
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& record = (*action->ownerSession->WorldScene().objects)[payload->index];
    reinterpret_cast<unsigned char*>(&record.flags42)[1] |= 4u;
    record.value10C = value;
}

// 原函数：0x00442D90。
std::uint16_t* Sub_442D90(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags42;
    flags &= 0xEBFFu;
    return &flags;
}

// 原函数：0x0043D970。
std::uint16_t* ShowWorldActorPersistentStatusIcon_43D970(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags120;
    *reinterpret_cast<unsigned char*>(&flags) |= 1u;
    return &flags;
}

// 原函数：0x0043D9A0。
std::uint16_t* HideWorldActorPersistentStatusIcon_43D9A0(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags120;
    flags &= 0xFFFEu;
    return &flags;
}

// 原函数：0x0043D9D0。
void SetWorldActorStatusIcon_43D9D0(cg::yobi::YobiAction* const action, std::int32_t value)
{
    if (action == nullptr) {
        return;
    }
    if (value < 0 || value >= GetActorStatusIconCount_4410E0()) {
        value = 0;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& record = (*action->ownerSession->WorldScene().objects)[payload->index];
    *reinterpret_cast<unsigned char*>(&record.flags120) |= 2u;
    record.value124 = value;
}

// 原函数：0x0043DA10。
void ClearWorldActorStatusIcon_43DA10(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& record = (*action->ownerSession->WorldScene().objects)[payload->index];
    record.flags120 &= 0xFFFDu;
    record.value124 = 0;
}

// 原函数：0x0043DA40。
std::uint16_t* ShowWorldActorTimedStatusIcon_43DA40(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags120;
    *reinterpret_cast<unsigned char*>(&flags) |= 0x24u;
    return &flags;
}

// 原函数：0x0043DA70。
std::uint16_t* ShowActorPetPersistentStatusIcon_43DA70(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags120;
    *reinterpret_cast<unsigned char*>(&flags) |= 8u;
    return &flags;
}

// 原函数：0x0043DAA0。
std::uint16_t* HideActorPetPersistentStatusIcon_43DAA0(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags120;
    flags &= 0xFFF7u;
    return &flags;
}

// 原函数：0x0043DAD0。
std::uint16_t* ShowActorPetTimedStatusIcon_43DAD0(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags120;
    *reinterpret_cast<unsigned char*>(&flags) |= 0x50u;
    return &flags;
}

// 原函数：0x00449C90。
std::uint16_t EnableWorldActorModeMarker_449C90(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return 0;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags42;
    std::uint16_t sampledFlags = flags;
    if ((sampledFlags & 0x200u) == 0) {
        sampledFlags |= 0x100u;
        flags = sampledFlags;
    }
    return sampledFlags;
}

// 原函数：0x00449CC0。
void ClearWorldActorModeMarkerUnlessLeader_449CC0(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& record = (*action->ownerSession->WorldScene().objects)[payload->index];
    if (record.marker44 == 0) {
        record.flags42 &= 0xFEFFu;
    }
}

// 原函数：0x00442C40。
std::uint16_t* MarkWorldActorAsPartyMember_442C40(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags42;
    reinterpret_cast<unsigned char*>(&flags)[1] |= 2u;
    return &flags;
}

// 原函数：0x00442C70。
void ClearWorldActorPartyMembership_442C70(cg::yobi::YobiAction* const action)
{
    if (IsObservationSceneTransitionActive_440B50(*action->ownerSession) != 0) {
        return;
    }
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    (*action->ownerSession->WorldScene().objects)[payload->index].flags42 &= 0xFDFFu;
}

// 原函数：0x00442BD0。
void MarkWorldActorAsPartyLeader_442BD0(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& record = (*action->ownerSession->WorldScene().objects)[payload->index];
    reinterpret_cast<unsigned char*>(&record.flags42)[1] |= 1u;
    record.marker44 = 1;
}

// WOP01：类别2记录更新；两坐标mask/encoded/count先取齐，保创建失败与fresh action序。
// 原函数：0x00445D20。
void UpsertGroundItemObject_445D20(
    session::ClientSession& session, const std::int32_t objectIndex, const std::int32_t imageNo,
    const std::int32_t mapX, const std::int32_t mapY,
    const std::int32_t direction, const std::int32_t value48,
    const char* const name, const std::int32_t value50)
{
    std::int32_t index = FindWorldObjectSlot_444FA0(session, objectIndex);
    if (index < 0) {
        index = ReserveWorldObjectSlot_446130(session);
        if (index < 0) {
            return;
        }
        cg::yobi::YobiAction* const action =
            cg::yobi::CreateMapActorAction_441DB0(session, imageNo, mapX, mapY, direction);
        auto& record = (*session.WorldScene().objects)[static_cast<std::size_t>(index)];
        record.action = action;
        if (action == nullptr) {
            return;
        }
        record.used = 2;
        record.action->payload.mapActor->index = index;
        record.category = 2;
        record.objectIndex = objectIndex;
        record.value34 = -1;
    }

    auto& record = (*session.WorldScene().objects)[static_cast<std::size_t>(index)];
    record.imageNo = static_cast<std::uint32_t>(imageNo);
    record.value48 = value48;
    // 原445D20两段均先取mask/encoded/count，再按同序发布；不改共享operator=(int32_t)。
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        target = static_cast<std::int32_t>(value);
    };
    assignCoordinate(record.coordinate0C, mapX);
    assignCoordinate(record.coordinate1C, mapY);
    record.direction = direction;
    if (std::strlen(name) <= 60) {
        record.nameC9 = cg::text::FromGbkWide(name);
    }
    record.flags114 = 0x400u;
    record.value118 = 0;
    record.value50 = value50;
    cg::yobi::YobiAction* const action = record.action;
    if (action == nullptr) {
        return;
    }
    action->imageNo = imageNo;
    if (std::strlen(name) <= 28) {
        record.action->nameC4 = cg::text::FromGbkWide(name);
    } else {
        record.action->nameC4 = L"???";
    }
    record.action->flags |= cg::yobi::YobiActionFlag_GroundItem;
    record.action->hoverTextColor1D8 = value48;
    record.action->payload.mapActor->itemCount18 = value50;
}

// WOP01：类别4记录更新；两坐标encoded/mask/count先取齐，不补类别2专用字段。
// 原函数：0x00445F40。
void UpsertGroundGoldObject_445F40(
    session::ClientSession& session, const std::int32_t objectIndex, const std::int32_t imageNo,
    const std::int32_t mapX, const std::int32_t mapY,
    const std::int32_t direction, const std::int32_t value4C,
    const std::wstring_view name)
{
    std::int32_t index = FindWorldObjectSlot_444FA0(session, objectIndex);
    if (index < 0) {
        index = ReserveWorldObjectSlot_446130(session);
        if (index < 0) {
            return;
        }
        cg::yobi::YobiAction* const action =
            cg::yobi::CreateMapActorAction_441DB0(session, imageNo, mapX, mapY, direction);
        auto& record = (*session.WorldScene().objects)[static_cast<std::size_t>(index)];
        record.action = action;
        if (action == nullptr) {
            return;
        }
        record.used = 2;
        record.action->payload.mapActor->index = index;
        record.category = 4;
        record.objectIndex = objectIndex;
        record.value34 = -1;
    }

    auto& record = (*session.WorldScene().objects)[static_cast<std::size_t>(index)];
    record.imageNo = static_cast<std::uint32_t>(imageNo);
    record.value4C = value4C;
    const auto assignCoordinate = [](
        platform::LegacyCryptInt& target, const std::int32_t value) {
        target = static_cast<std::int32_t>(value);
    };
    assignCoordinate(record.coordinate0C, mapX);
    assignCoordinate(record.coordinate1C, mapY);
    record.direction = direction;
    const std::string storedName = cg::text::ToGbk(name);
    if (storedName.size() <= 60) {
        record.nameC9 = name;
    }
    record.flags114 = 0x800u;
    record.value118 = 0;
    cg::yobi::YobiAction* const action = record.action;
    if (action == nullptr) {
        return;
    }
    action->imageNo = imageNo;
    if (storedName.size() <= 16) {
        record.action->nameC4 = name;
    } else {
        record.action->nameC4 = L"???";
    }
    record.action->flags |= cg::yobi::YobiActionFlag_GroundGold;
}

// 原函数：0x00521F20。
void RenderChara_521F20(session::ClientSession& session)
{
    const std::int16_t initialCount =
        std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
    cg::yobi::YobiAction* const start = session.Yobi().start;
    render::GraphicCache3* order =
        render::gLegacyPendingStorage.cache3.data() + initialCount;
    cg::yobi::YobiAction* action = start->next;
    render::GraphicCacheItem32* record =
        render::gLegacyPendingStorage.records.data() + initialCount;
    cg::yobi::YobiAction* end = session.Yobi().end;
    for (; action != end; action = action->next, end = session.Yobi().end) {
        const std::uint32_t flags = action->flags;
        if (((flags & cg::yobi::YobiActionFlag_SkipDrawWhenParityZero) != 0 && render::gFrameParity628FC8 == 0)
            || ((flags & cg::yobi::YobiActionFlag_SkipDrawWhenParityOne) != 0 && render::gFrameParity628FC8 == 1)) {
            action->cacheIndex = cg::yobi::YobiGraphicCacheIndex_NotQueued;
            continue;
        }

        // 原测内嵌地址；有效action的空名也入队，参数按原读取序取样。
        const std::wstring& name = action->displayName;
        const std::int32_t nameValue = action->nameValueC0;
        const std::int8_t nameFontType = action->nameFontType;
        const std::int8_t nameLayer = action->nameLayer;
        const std::int8_t nameColor = action->nameColor;
        const std::int32_t nameY = action->nameY;
        const std::int32_t nameX = action->nameX;
        static_cast<void>(render::DrawText2_456670(session,
            static_cast<std::int16_t>(nameX),
            static_cast<std::int16_t>(nameY),
            nameLayer,
            nameFontType,
            nameColor,
            name,
            static_cast<std::int8_t>(nameValue)));

        // 原名称调用后重定位实际槽，再独立重读容量判断用的S16计数。
        const std::int16_t relocatedCount =
            std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
        order = render::gLegacyPendingStorage.cache3.data() + relocatedCount;
        record = render::gLegacyPendingStorage.records.data() + relocatedCount;
        const std::int16_t capacityCount =
            std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
        if (capacityCount >= MAX_QUEUE_SIZE) {
            return;
        }
        if (action->graphicIndex == -1 || action->deathFlag == 1) {
            continue;
        }
        std::uint8_t hiddenFlags;
        std::memcpy(&hiddenFlags, &action->flags, sizeof(hiddenFlags));
        if ((hiddenFlags & cg::yobi::YobiActionFlag_Hidden) != 0) {
            continue;
        }

        const std::uint8_t layer = action->displayPriority;
        order->layer = layer;
        order->cacheIndex = std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);

        // 原S32偏移/F32缩放、F64乘积、S64向零转换及低S32承接保持。
        const std::int32_t offsetX = action->graphicOffsetX;
        const float scaleX = action->scaleX;
        const double scaledX =
            static_cast<double>(offsetX) * scaleX;
        const std::int32_t scaledOffsetX =
            static_cast<std::int32_t>(static_cast<std::int64_t>(scaledX));
        const std::int32_t x = action->x;
        record->x = scaledOffsetX + x;
        const std::int32_t offsetY = action->graphicOffsetY;
        const float scaleY = action->scaleY;
        const double scaledY =
            static_cast<double>(offsetY) * scaleY;
        const std::int32_t scaledOffsetY =
            static_cast<std::int32_t>(static_cast<std::int64_t>(scaledY));
        const std::int32_t y = action->y;
        render::GraphicDescriptor16& descriptor = record->descriptor;
        record->y = scaledOffsetY + y;
        const std::int32_t imageNo = action->graphicIndex;
        const render::GraphicDescriptor16& sourceDescriptor = action->drawMode;
        record->imageNo = imageNo;

        descriptor = sourceDescriptor;
        auto* const descriptorColorBytes = reinterpret_cast<unsigned char*>(&descriptor.color);

        const std::uint8_t initialColorMask = action->batchType;
        if (initialColorMask != 0) {
            if ((initialColorMask & cg::yobi::YobiColorChannelFlag_Red) != 0) {
                std::uint8_t red;
                std::memcpy(
                    &red, reinterpret_cast<const std::uint8_t*>(&action->color2) + 2,
                    sizeof(red));
                descriptorColorBytes[2] = red;
            }
            const std::uint8_t greenMask = action->batchType;
            if ((greenMask & cg::yobi::YobiColorChannelFlag_Green) != 0) {
                std::uint8_t green;
                std::memcpy(
                    &green, reinterpret_cast<const std::uint8_t*>(&action->color2) + 1,
                    sizeof(green));
                descriptorColorBytes[1] = green;
            }
            const std::uint8_t blueMask = action->batchType;
            if ((blueMask & cg::yobi::YobiColorChannelFlag_Blue) != 0) {
                std::uint8_t blue;
                std::memcpy(&blue, &action->color2, sizeof(blue));
                descriptorColorBytes[0] = blue;
            }
            const std::uint8_t alphaMask = action->batchType;
            if ((alphaMask & cg::yobi::YobiColorChannelFlag_Alpha) != 0) {
                std::uint8_t alpha;
                std::memcpy(
                    &alpha, reinterpret_cast<const std::uint8_t*>(&action->color2) + 3,
                    sizeof(alpha));
                descriptorColorBytes[3] = alpha;
            }
        }

        const std::uint8_t firstMixMask = action->mixFlagsOverrideMask;
        if (firstMixMask != 0) {
            const std::uint8_t previousMix = descriptor.mixFlags;
            const std::uint8_t clearedMix =
                previousMix & static_cast<std::uint8_t>(~firstMixMask);
            descriptor.mixFlags = clearedMix;
            const std::uint8_t freshMixMask = action->mixFlagsOverrideMask;
            const std::uint8_t replacementMix = action->mixFlagsOverrideValue;
            descriptor.mixFlags = clearedMix | (replacementMix & freshMixMask);
        }
        if ((action->flags & cg::yobi::YobiActionFlag_ForceAlpha80) != 0) {
            descriptorColorBytes[3] = 0x80;
        }

        record->reference.action = action;
        const std::uint32_t publishedFlags = action->flags;
        if ((publishedFlags & cg::yobi::YobiActionFlag_PendingValue2) != 0) {
            record->value4 = 2;
        } else if ((publishedFlags & cg::yobi::YobiActionFlag_PendingValue1) != 0) {
            record->value4 = 1;
        } else {
            record->value4 = 0;
        }
        const std::int16_t actionCacheIndex =
            std::bit_cast<std::int16_t>(platform::gPendingGraphicCount);
        action->cacheIndex = static_cast<std::int32_t>(actionCacheIndex);
        const std::uint8_t clipMode = descriptor.clipMode;
        record->batchType = 0;
        if (clipMode == 1) {
            record->batchType = 1;
        }
        cg::yobi::YobiAction* const queuedAction = record->reference.action;
        std::uint8_t queuedFlags;
        std::memcpy(&queuedFlags, &queuedAction->flags, sizeof(queuedFlags));
        if ((queuedFlags & cg::yobi::YobiActionFlag_PendingBatchFF) != 0) {
            record->batchType = 0xFF;
        }
        const std::int32_t queuedImageNo = record->imageNo;
        if (queuedImageNo >= 3096 && queuedImageNo <= 3105) {
            render::gSpecialGraphicPending = 1;
        }
        if (const auto* const anchor = cg::yobi::detail::GetBattleTitleAnchor(action)) {
            // 此时人物和骑乘均已更新；在最终队列坐标处对齐，避免叠加称号自身偏移。
            if (!cg::yobi::detail::GetYobiTitleTopLeft(anchor, action, true, record->x, record->y)) {
                action->cacheIndex = cg::yobi::YobiGraphicCacheIndex_NotQueued;
                continue;
            }
        }
        ++platform::gPendingGraphicCount;
        ++order;
        ++record;
    }
}

} // namespace cg::world::detail

namespace cg::world {

// 原函数：0x0043D860。
void FaceWorldActorTowardPlayer_43D860(session::ClientSession& session, cg::yobi::YobiAction* const action)
{
    cg::yobi::YobiAction* const player = session.WorldScene().player->action;
    if (player == nullptr || action == nullptr || session.WorldScene().map.resourceMode != 1) {
        return;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    if ((*session.WorldScene().objects)[payload->index].category != 1) {
        return;
    }
    const auto playerX = static_cast<std::int64_t>(player->worldX);
    const auto targetX = static_cast<std::int64_t>(action->worldX);
    const auto playerY = static_cast<std::int64_t>(player->worldY);
    const auto targetY = static_cast<std::int64_t>(action->worldY);
    const auto dx = static_cast<std::int16_t>(
        static_cast<std::uint16_t>(playerX) - static_cast<std::uint16_t>(targetX));
    const auto dy = static_cast<std::int16_t>(
        static_cast<std::uint16_t>(playerY) - static_cast<std::uint16_t>(targetY));
    if (dx < 0) {
        action->direction = dy < 0 ? 7 : (dy == 0 ? 6 : 5);
    } else if (dx > 0) {
        action->direction = dy < 0 ? 1 : (dy == 0 ? 2 : 3);
    } else if (dy < 0) {
        action->direction = cg::yobi::YobiDirection_UpLeft;
    } else if (dy > 0) {
        action->direction = cg::yobi::YobiDirection_DownRight;
    } else {
        // 原43D956读取实参地址低字节；不是旧方向，不伪造x86地址。
        action->direction = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(action)));
    }
}

// 原函数：0x0043DB00。
std::uint16_t* SuspendWorldActorForObservation_43DB00(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags120;
    *reinterpret_cast<unsigned char*>(&flags) |= 0x80u;
    return &flags;
}

// 原函数：0x0043DB30。
std::uint16_t* ResumeWorldActorAfterObservation_43DB30(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return nullptr;
    }
    const auto* const payload = static_cast<const cg::yobi::detail::MapActorPayload*>(action->payload.mapActor);
    auto& flags = (*action->ownerSession->WorldScene().objects)[payload->index].flags120;
    flags &= 0xFF7Fu;
    return &flags;
}

} // namespace cg::world
