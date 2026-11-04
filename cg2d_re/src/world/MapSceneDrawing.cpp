#include "../scene/login/LoginMessageDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"

#include "cg/resource/GraphicInfo.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "BattleState.hpp"
#include "WorldMapState.hpp"

#include <bit>
#include <cstring>
#include <optional>

namespace cg::world::detail {


// 原 46D179：tile 与静态 obj 的描述符写序不同，不统一重建描述符。
static void DrawCutTile(session::ClientSession& session,
    const std::uint16_t tile, const std::int32_t screenX, const std::int32_t screenY,
    render::GraphicDescriptor16& descriptor)
{
    if (tile <= 999 || screenX < -64 || screenX >= platform::gWindowWidth + 32 || screenY < -48 || screenY >= platform::gWindowHeight + 24) {
        return;
    }
    const CutRecord9FB954& cut = session.WorldScene().map.data.cut.records[tile - 1000];
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.clipMode = 1;
    descriptor.mixFlags = cg::render::GraphicMixFlag_None;
    const std::int16_t cutX = cut.value06;
    const auto drawX = static_cast<std::int16_t>(screenX - cut.value0E);
    const auto clipX = static_cast<std::int16_t>(cutX - 320);
    const std::uint16_t palette = 0;
    descriptor.paletteIndex = palette;
    descriptor.sourceX = clipX;
    const auto clipY = static_cast<std::int16_t>(cut.value08 - 240);
    descriptor.sourceY = clipY;
    const std::int16_t width = cut.value0A;
    descriptor.sourceWidth = width;
    const std::int16_t height = cut.value0C;
    descriptor.sourceHeight = height;
    const std::int32_t imageNo = cut.value00;
    const auto drawY = static_cast<std::int16_t>(screenY - cut.value10);
    static_cast<void>(render::DrawGraphic3_520970(
        drawX, drawY, 5, imageNo, 0, 1, 0, &descriptor));
}

// 原 46D207：原始 obj 索引 .cut；注册标志与发布地址的两次索引分别计算。
static void DrawCutObject(
    session::ClientSession& session,
    const std::uint16_t object, const std::int32_t column, const std::int32_t row,
    const std::int32_t screenX, const std::int32_t screenY,
    render::GraphicDescriptor16& descriptor)
{
    if (object <= 999) {
        return;
    }
    const CutRecord9FB954& cut = session.WorldScene().map.data.cut.records[object - 1000];
    const std::int16_t animationFlag = cut.value04;
    if (animationFlag == 1) {
        const std::int32_t startX = session.WorldScene().map.xStart;
        const std::int32_t startY = session.WorldScene().map.yStart;
        const std::int32_t mapX = startX + column;
        const std::int32_t index = 399 * mapX + startY + row;
        if (cg::yobi::MapYobiRegistryFlagAt(session, index) != 0) {
            cg::yobi::YobiAction* const action = cg::yobi::MapYobiRegistryActionAt(session, index);
            cg::yobi::UpdateRegisteredMapGraphicAction_46DD90(action);
            return;
        }
        const std::int32_t mapY = startY + row;
        cg::yobi::MapYobiRegistryFlagAt(session, index) = 1;
        const std::int32_t imageNo = cut.value00;
        cg::yobi::YobiAction* const action = cg::yobi::CreateRegisteredMapGraphicAction_46DE50(session, imageNo, mapX, mapY);
        const std::int32_t publishX = session.WorldScene().map.xStart + column;
        const std::int32_t publishY = session.WorldScene().map.yStart;
        const std::int32_t offsetX = cut.value0E;
        cg::yobi::MapYobiRegistryActionAt(session, 399 * publishX + row + publishY) = action;
        cg::yobi::SetYobiMapOffsetX(action, offsetX);
        const std::int32_t offsetIndexX = session.WorldScene().map.xStart + column;
        const std::int32_t offsetY = cut.value10;
        const std::int32_t offsetIndexY = session.WorldScene().map.yStart;
        cg::yobi::YobiAction* const offsetAction =
            cg::yobi::MapYobiRegistryActionAt(session, 399 * offsetIndexX + row + offsetIndexY);
        cg::yobi::SetYobiMapOffsetY(offsetAction, offsetY);
        return;
    }
    if (animationFlag != 0) {
        return;
    }
    std::int32_t imageNo;
    static_cast<void>(resource::GetGraphicMapNoIndex(cut.value00, &imageNo));
    const std::int16_t cutX = cut.value06;
    const std::int16_t width = cut.value0A;
    const std::int16_t cutY = cut.value08;
    const auto clipX = static_cast<std::int16_t>(cutX - 320);
    descriptor.sourceWidth = width;
    const auto drawY = static_cast<std::int16_t>(screenY - cut.value10);
    descriptor.sourceX = clipX;
    const std::int16_t height = cut.value0C;
    const auto clipY = static_cast<std::int16_t>(cutY - 240);
    descriptor.sourceHeight = height;
    descriptor.sourceY = clipY;
    const auto drawX = static_cast<std::int16_t>(screenX - cut.value0E);
    const std::int32_t mapY = session.WorldScene().map.yStart + row;
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    const double scaledY = mapY * 64.0;
    const std::int32_t mapX = session.WorldScene().map.xStart + column;
    descriptor.clipMode = 1;
    descriptor.mixFlags = cg::render::GraphicMixFlag_None;
    const auto worldY = static_cast<float>(scaledY);
    const std::uint16_t palette = 0;
    descriptor.paletteIndex = palette;
    const auto worldX = static_cast<float>(mapX * 64.0);
    CacheMapGraphicByResourceType_46BE50(
        imageNo, std::bit_cast<std::uint32_t>(worldX), std::bit_cast<std::uint32_t>(worldY),
        drawX, drawY, &descriptor);
}

// 原普通模式逐格先 tile 后 obj；音效标记是否消费由原参数决定。
static void DrawNormalMapCell(session::ClientSession& session,
    const std::int32_t index, const std::int32_t diagonal, const std::int32_t row,
    const std::int32_t screenX, const std::int32_t screenY, const std::int32_t markers)
{
    const std::int32_t tile = ResolveMapGraphicNumber_46CF00(session, session.WorldScene().map.data.tiles[index]);
    if (tile > 999) {
        if (screenX >= -32 && screenX < platform::gWindowWidth + 32 && screenY >= -24 && screenY < platform::gWindowHeight + 24) {
            const std::uint8_t layer =
                (tile >= 5000 && tile <= 5009) || (tile >= 7664 && tile <= 7679) ? 1 : 5;
            static_cast<void>(cg::render::DrawGraphic(screenX, screenY, layer, tile, 0, nullptr));
        }
    } else if (markers != 0) {
        if (tile >= 250 && tile <= 299) {
            static_cast<void>(audio::AccumulateSpatialSoundRequest_52C9E0(tile, screenX, screenY));
        } else if (tile >= 200 && tile <= 249) {
            static_cast<void>(audio::SelectBgmTarget_52C9C0(tile));
            audio::gBgmRequest1050810 = 1;
        }
    }

    const std::int32_t object = ResolveMapGraphicNumber_46CF00(session, session.WorldScene().map.data.objects[index]);
    if (object > 999) {
        std::int32_t imageNo;
        static_cast<void>(resource::GetGraphicMapNoIndex(object, &imageNo));
        std::int16_t offsetX;
        std::int16_t offsetY;
        static_cast<void>(resource::GetGraphicXY(
            std::bit_cast<std::uint32_t>(imageNo), &offsetX, &offsetY));
        std::int16_t width;
        std::int16_t height;
        static_cast<void>(resource::GetGraphicWH(
            std::bit_cast<std::uint32_t>(imageNo), &width, &height));
        // 原全 W32 局部相加后只消费低 S16；不读取/补造未初始化的高半字。
        const auto clipX = static_cast<std::int16_t>(screenX + offsetX);
        const auto clipY = static_cast<std::int16_t>(screenY + offsetY);
        if (clipX < platform::gWindowWidth && width + clipX - 1 >= 0
            && clipY < platform::gWindowHeight && height + clipY - 1 >= 0) {
            const std::int32_t mapY = session.WorldScene().map.yStart + row;
            const double scaledY = mapY * 64.0;
            const std::int32_t mapX = session.WorldScene().map.xStart + diagonal + row;
            const auto worldY = static_cast<float>(scaledY);
            const auto worldX = static_cast<float>(mapX * 64.0);
            CacheMapGraphicByResourceType_46BE50(
                imageNo, std::bit_cast<std::uint32_t>(worldX), std::bit_cast<std::uint32_t>(worldY),
                static_cast<std::int16_t>(screenX), static_cast<std::int16_t>(screenY), nullptr);
        }
    } else if (markers != 0) {
        std::int32_t imageNo;
        static_cast<void>(resource::GetGraphicMapNoIndex(object, &imageNo));
        if (object == 1) {
            const std::int32_t mapY = session.WorldScene().map.yStart + row;
            const double scaledY = mapY * 64.0;
            const std::int32_t mapX = session.WorldScene().map.xStart + diagonal + row;
            const auto worldY = static_cast<float>(scaledY);
            const auto worldX = static_cast<float>(mapX * 64.0);
            static_cast<void>(CachePrimaryMapGraphic_46BD60(
                imageNo, std::bit_cast<std::uint32_t>(worldX), std::bit_cast<std::uint32_t>(worldY),
                static_cast<std::int16_t>(screenX), static_cast<std::int16_t>(screenY), 0, nullptr));
        } else if (object >= 250 && object <= 299) {
            const std::int32_t span = session.WorldScene().map.spanX;
            const std::int32_t rawObject = session.WorldScene().map.data.objects[span * row + diagonal + row];
            static_cast<void>(audio::AccumulateSpatialSoundRequest_52C9E0(rawObject, screenX, screenY));
        } else if (object >= 200 && object <= 249) {
            static_cast<void>(audio::SelectBgmTarget_52C9C0(object));
            audio::gBgmRequest1050810 = 1;
        }
    }
}

// 原函数：0x00469020。
void DrawMapSceneWithMarkers_469020(session::ClientSession& session)
{
    DrawMapScene_46CF60(session, 1);
}

// 原战斗快照；与 renderer 重建不同，queueMode 固定恢复为 1，不更新时间戳。
// 原函数：0x0043CB10。
void CaptureBattleBackgroundSnapshot_43CB10(session::ClientSession& session)
{
    platform::gPendingGraphicCount = 0;
    platform::gPendingGraphicCount2 = 0;
    static_cast<void>(render::ResetGraphicCache_456D30());
    static_cast<void>(ResetMapDrawCacheCounts_46BD40());
    DrawMapScene_46CF60(session, 0);
    const std::int32_t imageNo = session.WorldScene().battle.backgroundImage;
    static_cast<void>(cg::render::DrawBattleBackgroundScaled(200, imageNo));
    render::SortTextures_520790();
    static_cast<void>(render::ClearSurface_4512C0());
    render::gRenderQueueMode103CF10 = 0;
    render::DrawAllQueued_520DD0(session);
    static_cast<void>(render::CopyBackToTemp());
    render::gRenderQueueMode103CF10 = 1;
    platform::gPendingGraphicCount = 0;
    platform::gPendingGraphicCount2 = 0;
    static_cast<void>(render::ResetGraphicCache_456D30());
    session.WorldScene().battle.backgroundSnapshot = session.WorldScene().battle.backgroundImage;
}

} // namespace cg::world::detail

namespace cg::world {

// 原函数：0x0046CF60。
void __cdecl DrawMapScene_46CF60(session::ClientSession& session, const std::int32_t markers)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t mode = session.WorldScene().map.resourceMode;
    const std::uint32_t previousCount = session.WorldScene().map.framesSinceWindowReload;
    audio::gBgmRequest1050810 = 0;
    const bool cutMode = mode == 1;
    session.WorldScene().map.framesSinceWindowReload = previousCount + 1u;
    if ((gPlayerX != gLastMapDrawPlayerX_9EE8D0)
        || (gPlayerY != gLastMapDrawPlayerY_9EE8E0)
        || session.WorldScene().movement.mapWindowReloadPending != 0) {
        const std::int32_t loadYEnd = session.WorldScene().map.yEnd;
        const std::int32_t loadXEnd = session.WorldScene().map.xEnd;
        const std::int32_t loadYStart = session.WorldScene().map.yStart;
        const std::int32_t loadXStart = session.WorldScene().map.xStart;
        const std::int32_t floor = session.WorldScene().map.floor;
        const auto mapId = std::bit_cast<std::int32_t>(session.WorldScene().map.mapId);
        if (detail::ReadMapCacheRectangle_468C40(session,
                mapId, floor, loadXStart, loadYStart, loadXEnd, loadYEnd,
                session.WorldScene().map.data.tiles.data(), session.WorldScene().map.data.objects.data(), session.WorldScene().map.data.hits.data()) < 0) {
            return;
        }
        const std::int32_t walkYEnd = session.WorldScene().map.yEnd;
        const std::int32_t walkXEnd = session.WorldScene().map.xEnd;
        const std::int32_t walkYStart = session.WorldScene().map.yStart;
        const std::int32_t walkXStart = session.WorldScene().map.xStart;
        if (cutMode) {
            static_cast<void>(detail::BuildMapWalkability_46D9C0(session,
                walkXStart, walkYStart, walkXEnd, walkYEnd,
                session.WorldScene().map.data.tiles.data(), session.WorldScene().map.data.objects.data(), session.WorldScene().map.data.hits.data(),
                session.WorldScene().map.data.walkability.data()));
        } else {
            static_cast<void>(detail::BuildGraphicMapWalkability_4693F0(session,
                walkXStart, walkYStart, walkXEnd, walkYEnd,
                session.WorldScene().map.data.tiles.data(), session.WorldScene().map.data.objects.data(), session.WorldScene().map.data.hits.data(),
                session.WorldScene().map.data.walkability.data()));
        }
        if (session.WorldScene().map.waitingForRectangleData != 0) {
            const auto direction = std::bit_cast<std::int16_t>(session.WorldScene().map.requestedRectangleDirection);
            if (detail::FindMissingMapRectanglesAhead_469870(session, direction) == 0) {
                session.WorldScene().map.waitingForRectangleData = 0;
                session.WorldScene().map.autoMapWindowDirty = 1;
            }
        } else {
            session.WorldScene().map.autoMapWindowDirty = 1;
        }
        session.WorldScene().map.framesSinceWindowReload = 0;
        session.WorldScene().movement.mapWindowReloadPending = 0;
    }

    static_cast<void>(detail::InitializeMapProjectionOrigin_468F70(session));
    const std::int32_t xStart = session.WorldScene().map.xStart;
    const std::int32_t yEnd = session.WorldScene().map.yEnd;
    const auto projectionX = std::bit_cast<std::int32_t>(session.WorldScene().map.projectionSourceC);
    std::int32_t baseX = ((xStart + yEnd) * 64 - 64) / 2 + projectionX;
    const std::int32_t rawY = 48 * (yEnd - xStart - 1);
    const auto projectionY = std::bit_cast<std::int32_t>(session.WorldScene().map.projectionSourceD);
    std::int32_t drawY = rawY / 2 + projectionY;
    std::int32_t firstRow = session.WorldScene().map.spanY - 1;
    std::int32_t firstColumn = 0;
    while (firstRow >= 0) {
        std::int32_t drawX = baseX;
        const std::int32_t diagonal = firstColumn - firstRow;
        std::int32_t row = firstRow;
        while (row >= 0) {
            const std::int32_t column = diagonal + row;
            if (column < 0) {
                break;
            }
            const std::int32_t index = session.WorldScene().map.spanX * row + column;
            if (cutMode) {
                detail::DrawCutTile(session, session.WorldScene().map.data.tiles[index], drawX, drawY, descriptor);
                detail::DrawCutObject(session, session.WorldScene().map.data.objects[index], column, row, drawX, drawY, descriptor);
            } else {
                detail::DrawNormalMapCell(session, index, diagonal, row, drawX, drawY, markers);
            }
            --row;
            drawX -= 64;
        }
        if (firstColumn < session.WorldScene().map.spanX - 1) {
            ++firstColumn;
            baseX += 32;
        } else {
            --firstRow;
            baseX -= 32;
        }
        drawY -= 24;
    }

    detail::SortAndMergeMapDrawCache_46C2F0();
    static_cast<void>(detail::QueueMergedMapDrawCache_46C1C0());
    const std::int32_t playerX = static_cast<std::int32_t>(gPlayerX);
    std::optional<platform::LegacyCryptInt> temporary;
    temporary.emplace(static_cast<std::int32_t>(playerX));
    if (cutMode) {
        const std::uint8_t readCount = temporary->readCount;
        const std::uint32_t encoded = temporary->encoded;
        const std::uint32_t mask = temporary->mask;
        gLastMapDrawPlayerX_9EE8D0.readCount = readCount;
        gLastMapDrawPlayerX_9EE8D0.encoded = encoded;
        gLastMapDrawPlayerX_9EE8D0.mask = mask;
    } else {
        const std::uint32_t mask = temporary->mask;
        const std::uint32_t encoded = temporary->encoded;
        const std::uint8_t readCount = temporary->readCount;
        gLastMapDrawPlayerX_9EE8D0.mask = mask;
        gLastMapDrawPlayerX_9EE8D0.encoded = encoded;
        gLastMapDrawPlayerX_9EE8D0.readCount = readCount;
    }
    temporary.reset();
    const std::int32_t playerY = static_cast<std::int32_t>(gPlayerY);
    temporary.emplace(static_cast<std::int32_t>(playerY));
    if (cutMode) {
        const std::uint8_t readCount = temporary->readCount;
        const std::uint32_t encoded = temporary->encoded;
        const std::uint32_t mask = temporary->mask;
        gLastMapDrawPlayerY_9EE8E0.readCount = readCount;
        gLastMapDrawPlayerY_9EE8E0.encoded = encoded;
        gLastMapDrawPlayerY_9EE8E0.mask = mask;
    } else {
        const std::uint32_t mask = temporary->mask;
        const std::uint32_t encoded = temporary->encoded;
        const std::uint8_t readCount = temporary->readCount;
        gLastMapDrawPlayerY_9EE8E0.mask = mask;
        gLastMapDrawPlayerY_9EE8E0.encoded = encoded;
        gLastMapDrawPlayerY_9EE8E0.readCount = readCount;
    }
    temporary.reset();
    audio::SubmitSpatialSoundSources_52CD60();
}

} // namespace cg::world
