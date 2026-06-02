#include "world/SceneGroundAtlasCache.hpp"

#include "render/IRenderer.hpp"
#include "render/TextureCache.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

struct TextureUpload {
    cg::render::TextureHandle texture;
    cg::render::TextureDesc desc;
    int pitch = 0;
    std::vector<std::uint8_t> pixels;
};

class FakeRenderer final : public cg::render::IRenderer {
public:
    [[nodiscard]] bool BeginFrame(cg::render::ClearColor) override
    {
        return true;
    }

    [[nodiscard]] bool EndFrame() override
    {
        return true;
    }

    [[nodiscard]] bool SetLogicalPresentation(const cg::render::LogicalPresentation&) override
    {
        return true;
    }

    void ResetStats() noexcept override {}

    [[nodiscard]] cg::render::RenderStats GetStats() const noexcept override
    {
        return {};
    }

    [[nodiscard]] cg::render::TextureHandle CreateTexture(const cg::render::TextureDesc& desc) override
    {
        const cg::render::TextureHandle texture{nextTextureId_++, 1};
        liveTextures_.emplace(texture.id, desc);
        created.push_back(desc);
        return texture;
    }

    [[nodiscard]] bool UpdateTexture(cg::render::TextureHandle texture, const void* pixels, int pitch) override
    {
        const auto iter = liveTextures_.find(texture.id);
        if (iter == liveTextures_.end() || pixels == nullptr || pitch <= 0) {
            return false;
        }

        TextureUpload upload;
        upload.texture = texture;
        upload.desc = iter->second;
        upload.pitch = pitch;
        upload.pixels.resize(static_cast<std::size_t>(pitch) * static_cast<std::size_t>(upload.desc.height));
        std::memcpy(upload.pixels.data(), pixels, upload.pixels.size());
        uploads.push_back(std::move(upload));
        return true;
    }

    void DestroyTexture(cg::render::TextureHandle texture) noexcept override
    {
        destroyed.push_back(texture);
        liveTextures_.erase(texture.id);
    }

    [[nodiscard]] bool DrawTexture(cg::render::TextureHandle, const cg::render::RectF&, const cg::render::RectF&) override
    {
        return true;
    }

    [[nodiscard]] bool DrawSprites(const cg::render::SpriteCmd*, std::size_t) override
    {
        return true;
    }

    std::vector<cg::render::TextureDesc> created;
    std::vector<TextureUpload> uploads;
    std::vector<cg::render::TextureHandle> destroyed;

private:
    std::uint32_t nextTextureId_ = 1;
    std::unordered_map<std::uint32_t, cg::render::TextureDesc> liveTextures_;
};

class FakeGroundTileImageProvider final : public cg::world::IGroundTileImageProvider {
public:
    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeTile(std::uint32_t imageNo) override
    {
        requests.push_back(imageNo);
        const auto iter = images.find(imageNo);
        if (iter == images.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

    [[nodiscard]] std::size_t RequestCount(std::uint32_t imageNo) const
    {
        return static_cast<std::size_t>(std::count(requests.begin(), requests.end(), imageNo));
    }

    std::unordered_map<std::uint32_t, cg::resource::DecodedImage> images;
    std::vector<std::uint32_t> requests;
};

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

template <typename T>
int ExpectEquals(const T& actual, const T& expected, const char* message)
{
    return Expect(actual == expected, message);
}

[[nodiscard]] std::array<std::uint8_t, 4> MakePixel(std::uint8_t seed, std::size_t index)
{
    const auto value = static_cast<std::uint8_t>(seed + static_cast<std::uint8_t>(index));
    return {value, static_cast<std::uint8_t>(value + 1U), static_cast<std::uint8_t>(value + 2U), 255U};
}

[[nodiscard]] cg::resource::DecodedImage MakeImage(int width, int height, std::uint8_t seed, int offsetX = 0, int offsetY = 0)
{
    cg::resource::DecodedImage image;
    image.width = width;
    image.height = height;
    image.offsetX = offsetX;
    image.offsetY = offsetY;
    image.format = cg::render::PixelFormat::Rgba8;
    image.pixels.reserve(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4U);
    for (std::size_t index = 0; index < static_cast<std::size_t>(width) * static_cast<std::size_t>(height); ++index) {
        const std::array<std::uint8_t, 4> pixel = MakePixel(seed, index);
        image.pixels.insert(image.pixels.end(), pixel.begin(), pixel.end());
    }
    return image;
}

[[nodiscard]] cg::resource::MapData MakeMap(
    std::uint16_t width,
    std::uint16_t height,
    std::vector<std::uint16_t> tileData)
{
    cg::resource::MapData map;
    map.width = width;
    map.height = height;
    map.tileData = std::move(tileData);
    return map;
}

[[nodiscard]] cg::world::MapTileAtlasCacheKey MakeKey(
    std::uint32_t layerMask = cg::world::MapTileAtlasAllLayerMask,
    std::uint64_t mapGeneration = 1,
    std::uint64_t resourceGeneration = 1)
{
    return cg::world::MapTileAtlasCacheKey{
        10,
        2,
        3,
        layerMask,
        mapGeneration,
        resourceGeneration,
    };
}

[[nodiscard]] std::array<std::uint8_t, 4> PixelAt(const TextureUpload& upload, int x, int y)
{
    const std::size_t offset =
        static_cast<std::size_t>(y) * static_cast<std::size_t>(upload.pitch)
        + static_cast<std::size_t>(x) * 4U;
    return {
        upload.pixels[offset],
        upload.pixels[offset + 1U],
        upload.pixels[offset + 2U],
        upload.pixels[offset + 3U],
    };
}

int ExpectRect(
    const cg::render::RectF& rect,
    float x,
    float y,
    float width,
    float height,
    const char* message)
{
    int failures = 0;
    failures += Expect(rect.x == x, message);
    failures += Expect(rect.y == y, message);
    failures += Expect(rect.width == width, message);
    failures += Expect(rect.height == height, message);
    return failures;
}

int TestCacheHitDecodesAndUploadsOnce()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeGroundTileImageProvider provider;
    provider.images.emplace(1000U, MakeImage(2, 2, 10));

    cg::world::SceneGroundAtlasCache cache(textureCache, provider);
    const cg::world::MapTileAtlasCacheKey key = MakeKey();
    const cg::resource::MapData map = MakeMap(2, 1, {1000U, 1000U});

    const cg::world::SceneGroundAtlas* first = cache.GetOrBuild(key, map);
    const cg::world::SceneGroundAtlas* second = cache.GetOrBuild(key, map);

    int failures = 0;
    failures += Expect(first != nullptr, "首次构建应成功");
    failures += Expect(first == second, "相同 key 应直接命中同一 atlas");
    failures += ExpectEquals(provider.RequestCount(1000U), static_cast<std::size_t>(1), "同 imageNo 只解码一次");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(1), "相同 key 只上传一次");
    if (first != nullptr) {
        failures += ExpectEquals(first->entries.size(), static_cast<std::size_t>(2), "重复 tile 应生成两个 entry");
        failures += ExpectEquals(first->atlas.RegionCount(), static_cast<std::size_t>(1), "重复 imageNo 只打包一个 region");
    }
    return failures;
}

int TestGenerationChangeRebuildsAndDestroysOldTexture()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeGroundTileImageProvider provider;
    provider.images.emplace(1000U, MakeImage(1, 1, 20));

    cg::world::SceneGroundAtlasCache cache(textureCache, provider);
    const cg::resource::MapData map = MakeMap(1, 1, {1000U});
    const cg::world::MapTileAtlasCacheKey key = MakeKey(cg::world::MapTileAtlasAllLayerMask, 1, 1);
    cg::world::MapTileAtlasCacheKey mapChanged = key;
    mapChanged.mapGeneration = 2;
    cg::world::MapTileAtlasCacheKey resourceChanged = mapChanged;
    resourceChanged.resourceGeneration = 2;

    const cg::world::SceneGroundAtlas* first = cache.GetOrBuild(key, map);
    const cg::world::SceneGroundAtlas* second = cache.GetOrBuild(mapChanged, map);
    const bool oldMapTextureRemoved = !textureCache.Find(cg::world::SceneGroundAtlasCache::MakeTextureKey(key)).IsValid();
    const bool newMapTextureExists = textureCache.Find(cg::world::SceneGroundAtlasCache::MakeTextureKey(mapChanged)).IsValid();
    const cg::world::SceneGroundAtlas* third = cache.GetOrBuild(resourceChanged, map);
    const bool oldResourceTextureRemoved =
        !textureCache.Find(cg::world::SceneGroundAtlasCache::MakeTextureKey(mapChanged)).IsValid();

    int failures = 0;
    failures += Expect(first != nullptr && second != nullptr && third != nullptr, "generation 变化后应重建成功");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(3), "每次 generation 变化应重新上传");
    failures += ExpectEquals(renderer.destroyed.size(), static_cast<std::size_t>(2), "旧 generation texture 应被销毁");
    failures += Expect(oldMapTextureRemoved, "mapGeneration 变化应移除旧 texture key");
    failures += Expect(newMapTextureExists, "mapGeneration 新 texture 应存在");
    failures += Expect(oldResourceTextureRemoved, "resourceGeneration 变化应清理旧 texture key");
    return failures;
}

int TestObjectLayerIsIgnored()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeGroundTileImageProvider provider;

    cg::world::SceneGroundAtlasCache cache(textureCache, provider);
    cg::resource::MapData map = MakeMap(1, 1, {0U});
    map.objData = {5000U};
    const cg::world::SceneGroundAtlas* atlas = cache.GetOrBuild(MakeKey(), map);

    int failures = 0;
    failures += Expect(atlas != nullptr, "只有 objData 图号时不应失败");
    failures += ExpectEquals(provider.requests.size(), static_cast<std::size_t>(0), "objData 图号不应请求 provider");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(0), "无地板 entry 不应上传 texture");
    if (atlas != nullptr) {
        failures += ExpectEquals(atlas->entries.size(), static_cast<std::size_t>(0), "objData 不应生成地板 entry");
    }
    return failures;
}

int TestHitLayerIsIgnored()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeGroundTileImageProvider provider;
    provider.images.emplace(1000U, MakeImage(1, 1, 35));

    cg::world::SceneGroundAtlasCache cache(textureCache, provider);
    cg::resource::MapData map = MakeMap(1, 1, {0U});
    map.hitData = {1000U};
    const cg::world::SceneGroundAtlas* atlas = cache.GetOrBuild(MakeKey(), map);

    int failures = 0;
    failures += Expect(atlas != nullptr, "只有 hitData 图号时不应失败");
    failures += ExpectEquals(provider.requests.size(), static_cast<std::size_t>(0), "hitData 图号不应请求 provider");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(0), "hitData 不应触发 texture 上传");
    if (atlas != nullptr) {
        failures += ExpectEquals(atlas->entries.size(), static_cast<std::size_t>(0), "hitData 不应生成地板 entry");
    }
    return failures;
}

int TestLayerMaskFiltersSpecialNormalAndAll()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeGroundTileImageProvider provider;
    provider.images.emplace(5000U, MakeImage(1, 1, 30));
    provider.images.emplace(1000U, MakeImage(1, 1, 40));

    cg::world::SceneGroundAtlasCache cache(textureCache, provider);
    const cg::resource::MapData map = MakeMap(2, 1, {5000U, 1000U});
    const cg::world::SceneGroundAtlas* special =
        cache.GetOrBuild(MakeKey(cg::world::MapTileAtlasSpecialLayerMask), map);
    const cg::world::SceneGroundAtlas* normal =
        cache.GetOrBuild(MakeKey(cg::world::MapTileAtlasNormalLayerMask), map);
    const cg::world::SceneGroundAtlas* all =
        cache.GetOrBuild(MakeKey(cg::world::MapTileAtlasAllLayerMask), map);

    int failures = 0;
    failures += Expect(special != nullptr && normal != nullptr && all != nullptr, "layer mask 构建应成功");
    if (special != nullptr) {
        failures += ExpectEquals(special->entries.size(), static_cast<std::size_t>(1), "special mask 只保留特殊层");
        failures += ExpectEquals(special->entries[0].imageNo, 5000U, "special mask 应保留 5000");
        failures += Expect(special->entries[0].layer == cg::world::TileRenderLayer::Special, "5000 应是特殊层");
    }
    if (normal != nullptr) {
        failures += ExpectEquals(normal->entries.size(), static_cast<std::size_t>(1), "normal mask 只保留普通层");
        failures += ExpectEquals(normal->entries[0].imageNo, 1000U, "normal mask 应保留 1000");
        failures += Expect(normal->entries[0].layer == cg::world::TileRenderLayer::Normal, "1000 应是普通层");
    }
    if (all != nullptr) {
        failures += ExpectEquals(all->entries.size(), static_cast<std::size_t>(2), "all mask 应保留两层");
    }
    return failures;
}

int TestPackingUploadSourceAndPixels()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeGroundTileImageProvider provider;
    provider.images.emplace(1000U, MakeImage(2, 2, 50, -1, 2));
    provider.images.emplace(5000U, MakeImage(3, 1, 80, 4, -3));

    cg::world::SceneGroundAtlasCache cache(textureCache, provider);
    const cg::world::SceneGroundAtlas* atlas = cache.GetOrBuild(MakeKey(), MakeMap(2, 1, {1000U, 5000U}));

    int failures = 0;
    failures += Expect(atlas != nullptr, "packing 构建应成功");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(1), "packing 应上传一次");
    if (atlas == nullptr || renderer.uploads.empty()) {
        return failures;
    }

    const TextureUpload& upload = renderer.uploads[0];
    failures += ExpectEquals(upload.desc.width, 2048, "atlas 默认行宽应为 2048");
    failures += ExpectEquals(upload.desc.height, 2, "atlas 高度应为 shelf 最高行高度");
    failures += ExpectEquals(upload.pitch, 2048 * 4, "atlas pitch 应为 width * 4");
    failures += ExpectEquals(atlas->entries.size(), static_cast<std::size_t>(2), "packing 应生成两个 entry");
    failures += ExpectRect(atlas->entries[0].source, 0.0F, 0.0F, 2.0F, 2.0F, "第一个 tile source rect");
    failures += ExpectRect(atlas->entries[1].source, 2.0F, 0.0F, 3.0F, 1.0F, "第二个 tile source rect");
    failures += ExpectEquals(atlas->entries[0].offsetX, -1, "第一个 tile offsetX");
    failures += ExpectEquals(atlas->entries[0].offsetY, 2, "第一个 tile offsetY");
    failures += ExpectEquals(atlas->entries[1].offsetX, 4, "第二个 tile offsetX");
    failures += ExpectEquals(atlas->entries[1].offsetY, -3, "第二个 tile offsetY");
    failures += ExpectEquals(PixelAt(upload, 0, 0), MakePixel(50, 0), "第一个 tile 左上像素应复制");
    failures += ExpectEquals(PixelAt(upload, 1, 1), MakePixel(50, 3), "第一个 tile 第二行像素应复制");
    failures += ExpectEquals(PixelAt(upload, 2, 0), MakePixel(80, 0), "第二个 tile 左上像素应复制");
    failures += ExpectEquals(PixelAt(upload, 4, 0), MakePixel(80, 2), "第二个 tile 右侧像素应复制");
    failures += ExpectEquals(PixelAt(upload, 10, 1), std::array<std::uint8_t, 4>{0U, 0U, 0U, 0U}, "未占用像素应保持透明");
    return failures;
}

int TestMissingImageFailsWithoutCaching()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeGroundTileImageProvider provider;
    cg::world::SceneGroundAtlasCache cache(textureCache, provider);

    const cg::world::MapTileAtlasCacheKey key = MakeKey();
    const cg::resource::MapData map = MakeMap(1, 1, {1000U});
    const cg::world::SceneGroundAtlas* missing = cache.GetOrBuild(key, map);
    provider.images.emplace(1000U, MakeImage(1, 1, 90));
    const cg::world::SceneGroundAtlas* rebuilt = cache.GetOrBuild(key, map);

    int failures = 0;
    failures += Expect(missing == nullptr, "缺图应构建失败");
    failures += Expect(rebuilt != nullptr, "缺图失败不应缓存，补图后应成功");
    failures += ExpectEquals(provider.RequestCount(1000U), static_cast<std::size_t>(2), "失败后再次构建应重新请求图档");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(1), "缺图失败时不应上传");
    return failures;
}

int TestTileDataMustCoverMapSize()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeGroundTileImageProvider provider;
    provider.images.emplace(1000U, MakeImage(1, 1, 100));

    cg::world::SceneGroundAtlasCache cache(textureCache, provider);
    const cg::world::SceneGroundAtlas* atlas = cache.GetOrBuild(MakeKey(), MakeMap(2, 2, {1000U}));

    int failures = 0;
    failures += Expect(atlas == nullptr, "tileData 不覆盖 width*height 应失败");
    failures += ExpectEquals(provider.requests.size(), static_cast<std::size_t>(0), "tileData 尺寸失败不应请求图档");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(0), "tileData 尺寸失败不应上传");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestCacheHitDecodesAndUploadsOnce();
    failures += TestGenerationChangeRebuildsAndDestroysOldTexture();
    failures += TestObjectLayerIsIgnored();
    failures += TestHitLayerIsIgnored();
    failures += TestLayerMaskFiltersSpecialNormalAndAll();
    failures += TestPackingUploadSourceAndPixels();
    failures += TestMissingImageFailsWithoutCaching();
    failures += TestTileDataMustCoverMapSize();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
