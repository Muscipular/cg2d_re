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

// 原已加载 55DA8C=10000，只有 46CF60 读取/递增/清零。
static std::uint32_t gFramesSinceMapWindowReload_55DA8C = 10000;

// 原 46D179：tile 与静态 obj 的描述符写序不同，不统一重建描述符。
static void DrawCutTile(
    const std::uint16_t tile, const std::int32_t screenX, const std::int32_t screenY,
    render::GraphicDescriptor16& descriptor)
{
    if (tile <= 999 || screenX < -64 || screenX >= platform::gWindowWidth + 32 || screenY < -48 || screenY >= platform::gWindowHeight + 24) {
        return;
    }
    const CutRecord9FB954& cut = gCutData9FB948.records[tile - 1000];
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
    const std::uint16_t object, const std::int32_t column, const std::int32_t row,
    const std::int32_t screenX, const std::int32_t screenY,
    render::GraphicDescriptor16& descriptor)
{
    if (object <= 999) {
        return;
    }
    const CutRecord9FB954& cut = gCutData9FB948.records[object - 1000];
    const std::int16_t animationFlag = cut.value04;
    if (animationFlag == 1) {
        const std::int32_t startX = gMapXStart;
        const std::int32_t startY = gMapYStart;
        const std::int32_t mapX = startX + column;
        const std::int32_t index = 399 * mapX + startY + row;
        if (cg::yobi::MapYobiRegistryFlagAt(index) != 0) {
            cg::yobi::YobiAction* const action = cg::yobi::MapYobiRegistryActionAt(index);
            cg::yobi::UpdateRegisteredMapGraphicAction_46DD90(action);
            return;
        }
        const std::int32_t mapY = startY + row;
        cg::yobi::MapYobiRegistryFlagAt(index) = 1;
        const std::int32_t imageNo = cut.value00;
        cg::yobi::YobiAction* const action = cg::yobi::CreateRegisteredMapGraphicAction_46DE50(imageNo, mapX, mapY);
        const std::int32_t publishX = gMapXStart + column;
        const std::int32_t publishY = gMapYStart;
        const std::int32_t offsetX = cut.value0E;
        cg::yobi::MapYobiRegistryActionAt(399 * publishX + row + publishY) = action;
        cg::yobi::SetYobiMapOffsetX(action, offsetX);
        const std::int32_t offsetIndexX = gMapXStart + column;
        const std::int32_t offsetY = cut.value10;
        const std::int32_t offsetIndexY = gMapYStart;
        cg::yobi::YobiAction* const offsetAction =
            cg::yobi::MapYobiRegistryActionAt(399 * offsetIndexX + row + offsetIndexY);
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
    const std::int32_t mapY = gMapYStart + row;
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    const double scaledY = mapY * 64.0;
    const std::int32_t mapX = gMapXStart + column;
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
static void DrawNormalMapCell(
    const std::int32_t index, const std::int32_t diagonal, const std::int32_t row,
    const std::int32_t screenX, const std::int32_t screenY, const std::int32_t markers)
{
    const std::int32_t tile = ResolveMapGraphicNumber_46CF00(gMapTileArray[index]);
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

    const std::int32_t object = ResolveMapGraphicNumber_46CF00(gMapObjArray[index]);
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
            const std::int32_t mapY = gMapYStart + row;
            const double scaledY = mapY * 64.0;
            const std::int32_t mapX = gMapXStart + diagonal + row;
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
            const std::int32_t mapY = gMapYStart + row;
            const double scaledY = mapY * 64.0;
            const std::int32_t mapX = gMapXStart + diagonal + row;
            const auto worldY = static_cast<float>(scaledY);
            const auto worldX = static_cast<float>(mapX * 64.0);
            static_cast<void>(CachePrimaryMapGraphic_46BD60(
                imageNo, std::bit_cast<std::uint32_t>(worldX), std::bit_cast<std::uint32_t>(worldY),
                static_cast<std::int16_t>(screenX), static_cast<std::int16_t>(screenY), 0, nullptr));
        } else if (object >= 250 && object <= 299) {
            const std::int32_t span = gMapSpanX;
            const std::int32_t rawObject = gMapObjArray[span * row + diagonal + row];
            static_cast<void>(audio::AccumulateSpatialSoundRequest_52C9E0(rawObject, screenX, screenY));
        } else if (object >= 200 && object <= 249) {
            static_cast<void>(audio::SelectBgmTarget_52C9C0(object));
            audio::gBgmRequest1050810 = 1;
        }
    }
}

// 原函数：0x00469020。
void DrawMapSceneWithMarkers_469020()
{
    DrawMapScene_46CF60(1);
}

// 原战斗快照；与 renderer 重建不同，queueMode 固定恢复为 1，不更新时间戳。
// 原函数：0x0043CB10。
void CaptureBattleBackgroundSnapshot_43CB10(session::ClientSession& session)
{
    platform::gPendingGraphicCount = 0;
    platform::gPendingGraphicCount2 = 0;
    static_cast<void>(render::ResetGraphicCache_456D30());
    static_cast<void>(ResetMapDrawCacheCounts_46BD40());
    DrawMapScene_46CF60(0);
    const std::int32_t imageNo = session.WorldScene().battle.backgroundImage;
    static_cast<void>(cg::render::DrawBattleBackgroundScaled(200, imageNo));
    render::SortTextures_520790();
    static_cast<void>(render::ClearSurface_4512C0());
    render::gRenderQueueMode103CF10 = 0;
    render::DrawAllQueued_520DD0();
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
void __cdecl DrawMapScene_46CF60(const std::int32_t markers)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t mode = gMapResourceMode_9DAD68;
    const std::uint32_t previousCount = detail::gFramesSinceMapWindowReload_55DA8C;
    audio::gBgmRequest1050810 = 0;
    const bool cutMode = mode == 1;
    detail::gFramesSinceMapWindowReload_55DA8C = previousCount + 1u;
    if ((gPlayerX != gLastMapDrawPlayerX_9EE8D0)
        || (gPlayerY != gLastMapDrawPlayerY_9EE8E0)
        || detail::gMapWindowReloadPending_9EE910 != 0) {
        const std::int32_t loadYEnd = detail::gMapYEnd;
        const std::int32_t loadXEnd = detail::gMapXEnd;
        const std::int32_t loadYStart = gMapYStart;
        const std::int32_t loadXStart = gMapXStart;
        const std::int32_t floor = gMapFloor;
        const auto mapId = std::bit_cast<std::int32_t>(gCurrentMapId_9A86DC);
        if (detail::ReadMapCacheRectangle_468C40(
                mapId, floor, loadXStart, loadYStart, loadXEnd, loadYEnd,
                gMapTileArray.data(), gMapObjArray.data(), gMapHitArray.data()) < 0) {
            return;
        }
        const std::int32_t walkYEnd = detail::gMapYEnd;
        const std::int32_t walkXEnd = detail::gMapXEnd;
        const std::int32_t walkYStart = gMapYStart;
        const std::int32_t walkXStart = gMapXStart;
        if (cutMode) {
            static_cast<void>(detail::BuildMapWalkability_46D9C0(
                walkXStart, walkYStart, walkXEnd, walkYEnd,
                gMapTileArray.data(), gMapObjArray.data(), gMapHitArray.data(),
                gMapWalkabilityArray.data()));
        } else {
            static_cast<void>(detail::BuildGraphicMapWalkability_4693F0(
                walkXStart, walkYStart, walkXEnd, walkYEnd,
                gMapTileArray.data(), gMapObjArray.data(), gMapHitArray.data(),
                gMapWalkabilityArray.data()));
        }
        if (detail::gWaitingForMapRectangleData_9EE8B8 != 0) {
            const auto direction = std::bit_cast<std::int16_t>(detail::gRequestedMapRectangleDirection_9EE8A4);
            if (detail::FindMissingMapRectanglesAhead_469870(direction) == 0) {
                detail::gWaitingForMapRectangleData_9EE8B8 = 0;
                detail::gAutoMapWindowDirty_55DA88 = 1;
            }
        } else {
            detail::gAutoMapWindowDirty_55DA88 = 1;
        }
        detail::gFramesSinceMapWindowReload_55DA8C = 0;
        detail::gMapWindowReloadPending_9EE910 = 0;
    }

    static_cast<void>(detail::InitializeMapProjectionOrigin_468F70());
    const std::int32_t xStart = gMapXStart;
    const std::int32_t yEnd = detail::gMapYEnd;
    const auto projectionX = std::bit_cast<std::int32_t>(gWorldProjectionSourceC);
    std::int32_t baseX = ((xStart + yEnd) * 64 - 64) / 2 + projectionX;
    const std::int32_t rawY = 48 * (yEnd - xStart - 1);
    const auto projectionY = std::bit_cast<std::int32_t>(gWorldProjectionSourceD);
    std::int32_t drawY = rawY / 2 + projectionY;
    std::int32_t firstRow = gMapSpanY - 1;
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
            const std::int32_t index = gMapSpanX * row + column;
            if (cutMode) {
                detail::DrawCutTile(gMapTileArray[index], drawX, drawY, descriptor);
                detail::DrawCutObject(gMapObjArray[index], column, row, drawX, drawY, descriptor);
            } else {
                detail::DrawNormalMapCell(index, diagonal, row, drawX, drawY, markers);
            }
            --row;
            drawX -= 64;
        }
        if (firstColumn < gMapSpanX - 1) {
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
