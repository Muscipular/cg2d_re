#include "BigMapSurface.hpp"
#include "../world/WorldMapState.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/render/Palette.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/platform/MainLoop.hpp"

#include <Windows.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <limits>
#include <vector>

namespace cg::script {

// 4.06@413102：640×480局部画面，缩小后拼接。本线使用互不重叠的投影矩形块。
static constexpr int kSourceWidth = 640, kSourceHeight = 480;
// 4.06@413090/43858E：玩家与地砖共同包含额外32像素X锚点。
static constexpr int kMapOriginX = 320 + 32, kMapOriginY = 240;
static constexpr std::uint32_t kCacheVersion = 1;

struct BigMapSurface::State {
    struct ReleaseImage { void operator()(render::PixelImage* image) const { render::ReleasePixelImage(image); } };
    using Image = std::unique_ptr<render::PixelImage, ReleaseImage>;
    // 源矩形已考虑.cut裁剪；位置是未缩小的全图像素坐标。
    struct Sprite {
        int image = -1, x = 0, y = 0, layer = 0;
        render::PngRectangle source{};
    };
    struct Tile {
        std::vector<std::uint32_t> sprites;
        std::vector<std::uint32_t> pixels;
        Image image;
        bool dirty = true, failed = false;
        std::uint64_t retryAt = 0, lastUsed = 0;
    };

    int group = -1, pathGroup = -1, floor = -1, mode = -1;
    int width = 0, height = 0, divisor = 1, columns = 0, rows = 0;
    int tileWidth = 0, tileHeight = 0, pending = 0, failed = 0;
    int paletteVersion = -1, paletteChoice = -1;
    bool loaded = false, changed = false, cacheError = false, diskError = false;
    std::uint64_t nextDiskCheck = 0, nextCacheSave = 0;
    std::filesystem::path mapPath, cachePath;
    std::filesystem::file_time_type diskTime{};
    std::string resourceStamp, paletteStamp, cutStamp;
    std::vector<std::uint16_t> ground, objects, hits;
    std::vector<Sprite> sprites;
    std::vector<Tile> tiles;

    template<class T> static void Append(std::string& data, const T& value) {
        data.append(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static std::string PaletteStamp() {
        std::string result(reinterpret_cast<const char*>(render::gPalette), sizeof(render::gPalette));
        Append(result, render::gActivePalette);
        Append(result, render::gPaletteTone);
        return result;
    }

    static std::string ResourceStamp() {
        std::string result;
        Append(result, resource::gMaxGraphicCount);
        // 只读元数据，不计算文件摘要；顺序保留后组覆盖语义。
        for (std::size_t i = 0; i < resource::gBinLoaderGroupCount; ++i) {
            for (const wchar_t* name : {resource::gGraphicPaths[i], resource::gGraphicInfoPaths[i]}) {
                const std::wstring_view nameText(name ? name : L"");
                result.append(reinterpret_cast<const char*>(nameText.data()), nameText.size() * sizeof(wchar_t));
                Append(result, wchar_t{});
                std::error_code error;
                const std::filesystem::path path(nameText);
                const auto size = std::filesystem::file_size(path, error);
                Append(result, size);
                const auto time = std::filesystem::last_write_time(path, error).time_since_epoch().count();
                Append(result, time);
            }
        }
        return result;
    }

    static const resource::GraphicInfo* Graphic(int number) {
        if (number <= 0 || number >= resource::gBinLoaderGraphicCapacity || !resource::gGraphicMapNoIndex) return nullptr;
        const int index = resource::gGraphicMapNoIndex[number];
        if (!resource::gGraphicInfo || index < 0 || index >= resource::gMaxGraphicCount) return nullptr;
        const auto& info = resource::gGraphicInfo[index];
        return info.mapNo == number && info.w > 0 && info.h > 0 ? &info : nullptr;
    }

    Sprite MakeSprite(std::size_t cell, bool object) const {
        Sprite sprite;
        const int raw = object ? objects[cell] : ground[cell];
        const int number = mode == 1 ? raw : world::ResolveMapGraphicNumber_46CF00(raw);
        if (number <= 999) return sprite; // 标记/音效不是静态地图图形。
        const int gx = static_cast<int>(cell % width), gy = static_cast<int>(cell / width);
        sprite.x = kMapOriginX + 32 * (gx + gy);
        sprite.y = kMapOriginY + 24 * (width + gy - gx);
        if (mode == 1) {
            const auto& cuts = world::detail::gCutData9FB948;
            const int count = cuts.count06 + cuts.count08 + cuts.count0A;
            if (raw - 1000 < 0 || raw - 1000 >= count || raw - 1000 >= 50000) {
                sprite.image = -2; sprite.source = {0, 0, 64, 48}; return sprite;
            }
            const auto& cut = cuts.records[raw - 1000];
            if (object && cut.value04 != 0) return {}; // 不烘焙动画或注册Yobi。
            const auto* graphic = Graphic(cut.value00);
            sprite.image = graphic ? static_cast<int>(graphic->seqNo) : -2;
            sprite.x -= cut.value0E; sprite.y -= cut.value10;
            // DrawGraphic3_520970还会减去GraphicInfo偏移，转换为真实图内坐标。
            const int sourceX = cut.value06 - 320 - (graphic ? graphic->x : 0);
            const int sourceY = cut.value08 - 240 - (graphic ? graphic->y : 0);
            sprite.source = {sourceX, sourceY, sourceX + cut.value0A, sourceY + cut.value0C};
            sprite.layer = object ? 6 : 2;
        } else {
            const auto* graphic = Graphic(number);
            if (!graphic) {
                sprite.image = -2; sprite.source = {0, 0, 64, 48}; return sprite;
            }
            sprite.image = static_cast<int>(graphic->seqNo);
            sprite.x += graphic->x; sprite.y += graphic->y;
            sprite.source = {0, 0, graphic->w, graphic->h};
            const int flag = graphic->flag % 100; // 用户明确保留16位，不截低8位。
            sprite.layer = object ? (flag < 2 ? 6 : 3) : (flag == 1 ? 2 : 0);
        }
        return sprite;
    }

    template<class F> void ForTiles(const Sprite& sprite, F function) {
        if (sprite.image == -1) return;
        const int right = sprite.x + sprite.source[2] - sprite.source[0];
        const int bottom = sprite.y + sprite.source[3] - sprite.source[1];
        if (right <= 0 || bottom <= 0 || right <= sprite.x || bottom <= sprite.y) return;
        const int leftTile = std::max(0, sprite.x / kSourceWidth);
        const int topTile = std::max(0, sprite.y / kSourceHeight);
        const int rightTile = std::min(columns - 1, (right - 1) / kSourceWidth);
        const int bottomTile = std::min(rows - 1, (bottom - 1) / kSourceHeight);
        for (int ty = topTile; ty <= bottomTile; ++ty)
            for (int tx = leftTile; tx <= rightTile; ++tx) function(tiles[ty * columns + tx]);
    }

    void UpdateCell(std::size_t cell) {
        for (int object = 0; object < 2; ++object) {
            const auto id = static_cast<std::uint32_t>(2 * cell + object);
            ForTiles(sprites[id], [id](Tile& tile) {
                std::erase(tile.sprites, id); tile.dirty = true; tile.retryAt = 0;
            });
            sprites[id] = MakeSprite(cell, object != 0);
            ForTiles(sprites[id], [id](Tile& tile) {
                tile.sprites.push_back(id); tile.dirty = true; tile.retryAt = 0;
            });
        }
        changed = true;
    }

    bool ReadMap(bool initial) {
        std::ifstream input(mapPath, std::ios::binary);
        std::array<std::int32_t, 5> header{};
        if (!input.read(reinterpret_cast<char*>(header.data()), sizeof(header)) || header[0] != 0x0150414D
            || header[3] <= 0 || header[4] <= 0) return false;
        const std::int64_t count = static_cast<std::int64_t>(header[3]) * header[4];
        if (count > 10000000 || (!initial && (header[3] != width || header[4] != height))) return false;
        std::array<std::vector<std::uint16_t>, 3> data;
        for (auto& plane : data) {
            plane.resize(static_cast<std::size_t>(count));
            if (!input.read(reinterpret_cast<char*>(plane.data()), count * 2)) return false;
        }
        if (initial) {
            width = header[3]; height = header[4];
            ground = std::move(data[0]); objects = std::move(data[1]); hits = std::move(data[2]);
        } else {
            for (std::size_t i = 0; i < ground.size(); ++i) {
                const bool redraw = ground[i] != data[0][i] || objects[i] != data[1][i];
                changed |= redraw || hits[i] != data[2][i];
                ground[i] = data[0][i]; objects[i] = data[1][i]; hits[i] = data[2][i];
                if (redraw) UpdateCell(i);
            }
        }
        std::error_code error;
        diskTime = std::filesystem::last_write_time(mapPath, error);
        return true;
    }

    void SyncVisible() {
        const int spanX = world::gMapSpanX, spanY = world::gMapSpanY;
        if (spanX <= 0 || spanY <= 0 || static_cast<std::size_t>(spanX) * spanY > world::gMapTileArray.size()) return;
        const int playerX = static_cast<int>(world::gPlayerX), playerY = static_cast<int>(world::gPlayerY);
        for (int sy = 0; sy < spanY; ++sy) {
            const int gy = world::gMapYStart + sy;
            if (gy < 0 || gy >= height || std::abs(gy - playerY) > 25) continue;
            for (int sx = 0; sx < spanX; ++sx) {
                const int gx = world::gMapXStart + sx;
                if (gx < 0 || gx >= width || std::abs(gx - playerX) > 25) continue;
                const auto source = static_cast<std::size_t>(sy) * spanX + sx;
                const auto cell = static_cast<std::size_t>(gy) * width + gx;
                const bool redraw = ground[cell] != world::gMapTileArray[source] || objects[cell] != world::gMapObjArray[source];
                changed |= redraw || hits[cell] != world::gMapHitArray[source];
                ground[cell] = world::gMapTileArray[source]; objects[cell] = world::gMapObjArray[source];
                hits[cell] = world::gMapHitArray[source];
                if (redraw) UpdateCell(cell);
            }
        }
    }

    static std::string CutStamp() {
        const auto& cut = world::detail::gCutData9FB948;
        return {reinterpret_cast<const char*>(&cut), sizeof(cut)};
    }

    std::string CacheIdentity() const { return resourceStamp + paletteStamp + cutStamp; }

    void LoadCache() {
        if (group == 1) return;
        std::ifstream input(cachePath, std::ios::binary);
        if (!input) return;
        const std::string identity = CacheIdentity();
        const std::array<std::uint32_t, 7> expected{0x324D4243, kCacheVersion,
            static_cast<unsigned>(width), static_cast<unsigned>(height), static_cast<unsigned>(divisor),
            static_cast<unsigned>(identity.size()), static_cast<unsigned>(tiles.size())};
        std::array<std::uint32_t, 7> header{};
        if (!input.read(reinterpret_cast<char*>(header.data()), sizeof(header)) || header != expected) return;
        std::string saved(identity.size(), '\0');
        if (!input.read(saved.data(), saved.size()) || saved != identity) return;
        std::vector<std::uint16_t> plane(ground.size());
        for (const auto* actual : {&ground, &objects, &hits}) {
            if (!input.read(reinterpret_cast<char*>(plane.data()), plane.size() * 2) || plane != *actual) return;
        }
        // 完整读取后再发布，截断文件不能留下部分已完成状态。
        std::vector<std::vector<std::uint32_t>> cached(tiles.size());
        for (auto& pixels : cached) {
            std::uint32_t count = 0;
            if (!input.read(reinterpret_cast<char*>(&count), sizeof(count))) return;
            if (count != 0 && count != static_cast<unsigned>(tileWidth * tileHeight)) return;
            pixels.resize(count);
            if (count && !input.read(reinterpret_cast<char*>(pixels.data()), count * 4)) return;
        }
        for (std::size_t i = 0; i < tiles.size(); ++i) {
            if (cached[i].empty()) continue;
            tiles[i].pixels = std::move(cached[i]); tiles[i].dirty = false;
        }
        changed = false;
    }

    void SaveCache() {
        if (!loaded || !changed || group == 1) return;
        std::error_code error;
        std::filesystem::create_directories(cachePath.parent_path(), error);
        if (error) { cacheError = true; return; }
        auto temporary = cachePath; temporary += L".tmp";
        std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
        const auto identity = CacheIdentity();
        const std::array<std::uint32_t, 7> header{0x324D4243, kCacheVersion,
            static_cast<unsigned>(width), static_cast<unsigned>(height), static_cast<unsigned>(divisor),
            static_cast<unsigned>(identity.size()), static_cast<unsigned>(tiles.size())};
        output.write(reinterpret_cast<const char*>(header.data()), sizeof(header));
        output.write(identity.data(), identity.size());
        for (const auto* plane : {&ground, &objects, &hits})
            output.write(reinterpret_cast<const char*>(plane->data()), plane->size() * 2);
        for (const auto& tile : tiles) {
            const auto count = static_cast<std::uint32_t>(tile.dirty ? 0 : tile.pixels.size());
            output.write(reinterpret_cast<const char*>(&count), sizeof(count));
            if (count) output.write(reinterpret_cast<const char*>(tile.pixels.data()), count * 4);
        }
        output.close();
        cacheError = !output || !MoveFileExW(temporary.c_str(), cachePath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
        if (!cacheError) changed = false;
    }

    bool Initialize() {
        mapPath = world::FormatMapDataPath_468520(group, pathGroup, floor);
        if (!ReadMap(true)) return false;
        // 4.06@412B49，维度判断严格按顺序。
        divisor = width <= 64 && height <= 48 ? 4 : width <= 256 && height <= 192 ? 8
            : width <= 300 && height <= 300 ? 10 : 16;
        tileWidth = kSourceWidth / divisor; tileHeight = kSourceHeight / divisor;
        columns = (kSourceWidth + 32 * (width + height) + kSourceWidth - 1) / kSourceWidth;
        rows = (kSourceHeight + 24 * (width + height) + kSourceHeight - 1) / kSourceHeight;
        tiles.resize(static_cast<std::size_t>(columns) * rows);
        sprites.resize(ground.size() * 2);
        for (std::size_t i = 0; i < ground.size(); ++i) UpdateCell(i);
        resourceStamp = ResourceStamp(); paletteStamp = PaletteStamp();
        if (mode == 1) cutStamp = CutStamp();
        paletteVersion = render::gPaletteFrameVersion; paletteChoice = render::gActivePalette;
        cachePath = std::filesystem::path(L"data/cache/bigmap") / std::format(L"{}_{}_{}_{}.bmc", group, pathGroup, floor, mode);
        loaded = true;
        SyncVisible();
        LoadCache();
        return true;
    }

    bool BuildTile(std::size_t index) {
        auto& tile = tiles[index];
        const int originX = static_cast<int>(index % columns) * kSourceWidth;
        const int originY = static_cast<int>(index / columns) * kSourceHeight;
        std::vector<std::uint32_t> full(kSourceWidth * kSourceHeight, 0xFF000000u);
        // 4.06@4377C5..4377F9：两张特殊地图铺14392图的(0,820,320,120)区域。
        // 原n7==7来自4.06观察/转场控制，不与本线场景枚举7混同。
        if (floor == 59529 || floor == 38251) {
            const auto* background = Graphic(14392);
            if (!background) return false;
            for (int py = 0; py < kSourceHeight; py += 120)
                for (int px = 0; px < kSourceWidth; px += 320)
                    if (!render::CompositeGraphicPixels(background->seqNo, {0, 820, 320, 940},
                        px, py, full.data(), kSourceWidth, kSourceHeight)) return false;
        }
        // 4.06@4746C3：层升序、同层逆入队；原逐格从下到上、从右到左入队。
        std::sort(tile.sprites.begin(), tile.sprites.end(), [&](auto first, auto second) {
            const auto& a = sprites[first]; const auto& b = sprites[second];
            if (a.layer != b.layer) return a.layer < b.layer;
            const int ax = (first / 2) % width, ay = (first / 2) / width;
            const int bx = (second / 2) % width, by = (second / 2) / width;
            if (ay - ax != by - bx) return ay - ax < by - bx;
            if (ay + ax != by + bx) return ay + ax < by + bx;
            return first > second;
        });
        for (const auto id : tile.sprites) {
            const auto& sprite = sprites[id];
            if (sprite.image < 0 || !render::CompositeGraphicPixels(sprite.image, sprite.source,
                sprite.x - originX, sprite.y - originY, full.data(), kSourceWidth, kSourceHeight)) return false;
        }
        std::vector<std::uint32_t> reduced(static_cast<std::size_t>(tileWidth) * tileHeight);
        // 4.06缩小使用D3DX_FILTER_BOX=5；整数除数下按完整面积平均，不丢细地砖。
        const unsigned samples = divisor * divisor;
        for (int py = 0; py < tileHeight; ++py) for (int px = 0; px < tileWidth; ++px) {
            unsigned r = 0, g = 0, b = 0;
            for (int dy = 0; dy < divisor; ++dy) for (int dx = 0; dx < divisor; ++dx) {
                const auto pixel = full[(py * divisor + dy) * kSourceWidth + px * divisor + dx];
                r += (pixel >> 16) & 255; g += (pixel >> 8) & 255; b += pixel & 255;
            }
            reduced[py * tileWidth + px] = 0xFF000000u | (((r + samples / 2) / samples) << 16)
                | (((g + samples / 2) / samples) << 8) | ((b + samples / 2) / samples);
        }
        if (!tile.image) tile.image.reset(render::CreatePixelImage(tileWidth, tileHeight));
        if (!tile.image || !render::UpdatePixelImage(tile.image.get(), reduced.data(), tileWidth, tileHeight)) return false;
        tile.pixels = std::move(reduced); tile.dirty = false; tile.failed = false; changed = true;
        return true;
    }
};

BigMapSurface::BigMapSurface() : state_(std::make_unique<State>()) {}
BigMapSurface::~BigMapSurface() { state_->SaveCache(); }

bool BigMapSurface::PrepareMap() {
    const int group = static_cast<int>(world::gCurrentMapId_9A86DC), pathGroup = world::gMapCachePathKey_9E0434;
    const int floor = world::gMapFloor, mode = world::gMapResourceMode_9DAD68 == 1 ? 1 : 0;
    const bool resized = state_->loaded && world::detail::gMapWidth_9FB930 > 0 && world::detail::gMapHeight_C821D4 > 0
        && (state_->width != static_cast<int>(world::detail::gMapWidth_9FB930) || state_->height != static_cast<int>(world::detail::gMapHeight_C821D4));
    if (state_->group != group || state_->pathGroup != pathGroup || state_->floor != floor || state_->mode != mode || resized) {
        state_->SaveCache(); state_ = std::make_unique<State>();
        state_->group = group; state_->pathGroup = pathGroup; state_->floor = floor; state_->mode = mode;
    }
    auto& state = *state_;
    if (!state.loaded && !state.Initialize()) { mapWidth = mapHeight = 0; return false; }
    mapWidth = state.width; mapHeight = state.height;
    const auto now = platform::ReadTickCount64();
    if (now >= state.nextDiskCheck) {
        state.nextDiskCheck = now + 500;
        std::error_code error;
        const auto time = std::filesystem::last_write_time(state.mapPath, error);
        if (!error && time != state.diskTime) state.diskError = !state.ReadMap(false);
        else if (error) state.diskError = true;
        if (mode == 1) {
            const auto cut = State::CutStamp();
            if (cut != state.cutStamp) {
                state.cutStamp = cut;
                for (std::size_t cell = 0; cell < state.ground.size(); ++cell) state.UpdateCell(cell);
            }
        }
    }
    state.SyncVisible();
    if (state.paletteVersion != render::gPaletteFrameVersion || state.paletteChoice != render::gActivePalette) {
        state.paletteVersion = render::gPaletteFrameVersion; state.paletteChoice = render::gActivePalette;
        state.paletteStamp = State::PaletteStamp(); state.changed = true;
        for (auto& tile : state.tiles) { tile.dirty = true; tile.retryAt = 0; }
    }
    if (floor != 59529 && floor != 38251) {
        for (auto& tile : state.tiles) if (tile.dirty && tile.sprites.empty()) {
            // 已知没有任何源图覆盖的块就是地图空白，无需排队生成。
            tile.pixels.clear(); tile.image.reset(); tile.dirty = false; tile.failed = false;
        }
    }
    const double centerX = (kMapOriginX + 32 * (mapX + mapY)) / state.divisor;
    const double centerY = (kMapOriginY + 24 * (state.width + mapY - mapX)) / state.divisor;
    const int left = std::max(0, static_cast<int>(std::floor((centerX - w / 2.0) / state.tileWidth)));
    const int top = std::max(0, static_cast<int>(std::floor((centerY - h / 2.0) / state.tileHeight)));
    const int right = std::min(state.columns - 1, static_cast<int>(std::floor((centerX + w / 2.0) / state.tileWidth)));
    const int bottom = std::min(state.rows - 1, static_cast<int>(std::floor((centerY + h / 2.0) / state.tileHeight)));
    std::size_t best = state.tiles.size(); double distance = std::numeric_limits<double>::max();
    state.pending = state.failed = 0;
    for (int ty = top; ty <= bottom; ++ty) for (int tx = left; tx <= right; ++tx) {
        const auto index = static_cast<std::size_t>(ty) * state.columns + tx;
        auto& tile = state.tiles[index]; tile.lastUsed = now;
        if (tile.failed) ++state.failed;
        if (!tile.dirty) continue;
        ++state.pending;
        const double dx = (tx + 0.5) * state.tileWidth - centerX, dy = (ty + 0.5) * state.tileHeight - centerY;
        const double d = dx * dx + dy * dy;
        if (tile.retryAt <= now && d < distance) { best = index; distance = d; }
    }
    if (best < state.tiles.size()) {
        if (!state.BuildTile(best)) {
            auto& tile = state.tiles[best]; tile.failed = true; tile.retryAt = now + 1000;
        } else --state.pending;
    }
    // 仅释放离屏GPU纹理，已生成像素保留供重开视口和持久化使用。
    for (auto& tile : state.tiles) if (tile.image && now - tile.lastUsed > 3000) tile.image.reset();
    if (!state.pending && now >= state.nextCacheSave) {
        state.SaveCache(); state.nextCacheSave = now + 5000;
    }
    return true;
}

std::pair<int, int> BigMapSurface::MapToScreen(int gx, int gy) const {
    const double scaleX = 32.0 / state_->divisor, scaleY = 24.0 / state_->divisor;
    return {static_cast<int>(std::round(w / 2.0 + scaleX * (gx + gy - mapX - mapY))),
        static_cast<int>(std::round(h / 2.0 + scaleY * (gy - gx - mapY + mapX)))};
}

std::pair<int, int> BigMapSurface::ScreenToMap(int sx, int sy) const {
    const double sum = (sx - w / 2.0) * state_->divisor / 32.0;
    const double difference = (sy - h / 2.0) * state_->divisor / 24.0;
    return {static_cast<int>(std::round(mapX + (sum - difference) / 2)),
        static_cast<int>(std::round(mapY + (sum + difference) / 2))};
}

void BigMapSurface::PanFrom(double startX, double startY, int dx, int dy) {
    const double sum = dx * state_->divisor / 32.0, difference = dy * state_->divisor / 24.0;
    mapX = startX - (sum - difference) / 2; mapY = startY - (sum + difference) / 2;
}

bool BigMapSurface::IsExplored(int gx, int gy) const {
    return gx >= 0 && gy >= 0 && gx < mapWidth && gy < mapHeight
        && (state_->hits[static_cast<std::size_t>(gy) * mapWidth + gx] & 0x4000) != 0;
}

std::wstring BigMapSurface::Status() const {
    if (!state_->loaded) return L"地图数据未就绪";
    std::wstring status;
    if (state_->pending) status = std::format(L"生成中：{}块", state_->pending);
    if (state_->failed) status += std::format(L"  待重试：{}块", state_->failed);
    if (state_->cacheError) status += L"  缓存保存失败";
    if (state_->diskError) status += L"  地图数据更新失败";
    return status;
}

void BigMapSurface::Render(int originX, int originY, std::uint8_t layer, const render::PngRectangle& clip) {
    if (!state_->loaded) return;
    auto& state = *state_;
    const int offsetX = originX + x + static_cast<int>(std::round(w / 2.0 - (kMapOriginX + 32 * (mapX + mapY)) / state.divisor));
    const int offsetY = originY + y + static_cast<int>(std::round(h / 2.0 - (kMapOriginY + 24 * (mapWidth + mapY - mapX)) / state.divisor));
    for (int ty = 0; ty < state.rows; ++ty) for (int tx = 0; tx < state.columns; ++tx) {
        const int dx = offsetX + tx * state.tileWidth, dy = offsetY + ty * state.tileHeight;
        if (dx >= clip[2] || dy >= clip[3] || dx + state.tileWidth <= clip[0] || dy + state.tileHeight <= clip[1]) continue;
        auto& tile = state.tiles[ty * state.columns + tx];
        if (tile.pixels.empty()) continue;
        if (!tile.image) {
            tile.image.reset(render::CreatePixelImage(state.tileWidth, state.tileHeight));
            if (!tile.image || !render::UpdatePixelImage(tile.image.get(), tile.pixels.data(), state.tileWidth, state.tileHeight)) {
                tile.image.reset(); tile.failed = true; tile.dirty = true; continue;
            }
        }
        if (render::DrawPixelImage(tile.image.get(), dx, dy, state.tileWidth, state.tileHeight, 0xFFFFFFFFu, layer, &clip) < 0) {
            tile.failed = true; tile.dirty = true;
        }
    }
}

} // namespace cg::script
