#include "world/MapObjectTextureCache.hpp"

#include "render/IRenderer.hpp"
#include "render/TextureCache.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>
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
        if (!createSucceeds) {
            return {};
        }

        const cg::render::TextureHandle texture{nextTextureId_++, 1};
        liveTextures_.emplace(texture.id, desc);
        created.push_back(desc);
        return texture;
    }

    [[nodiscard]] bool UpdateTexture(cg::render::TextureHandle texture, const void* pixels, int pitch) override
    {
        ++updateCalls;
        if (!updateSucceeds) {
            return false;
        }

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

    bool createSucceeds = true;
    bool updateSucceeds = true;
    std::size_t updateCalls = 0;
    std::vector<cg::render::TextureDesc> created;
    std::vector<TextureUpload> uploads;
    std::vector<cg::render::TextureHandle> destroyed;

private:
    std::uint32_t nextTextureId_ = 1;
    std::unordered_map<std::uint32_t, cg::render::TextureDesc> liveTextures_;
};

class FakeObjectImageProvider final : public cg::world::IMapObjectImageProvider {
public:
    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeObject(std::uint32_t imageNo) override
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

[[nodiscard]] cg::resource::DecodedImage MakeImage(
    int width,
    int height,
    std::uint8_t seed,
    int offsetX = 0,
    int offsetY = 0)
{
    cg::resource::DecodedImage image;
    image.width = width;
    image.height = height;
    image.offsetX = offsetX;
    image.offsetY = offsetY;
    image.format = cg::render::PixelFormat::Rgba8;

    std::size_t pixelCount = 0;
    if (width > 0 && height > 0) {
        pixelCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    }

    image.pixels.reserve(pixelCount * 4U);
    for (std::size_t index = 0; index < pixelCount; ++index) {
        const std::array<std::uint8_t, 4> pixel = MakePixel(seed, index);
        image.pixels.insert(image.pixels.end(), pixel.begin(), pixel.end());
    }
    return image;
}

int ExpectRejectedImage(cg::resource::DecodedImage image, const char* message)
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(100U, std::move(image));
    cg::world::MapObjectTextureCache cache(textureCache, provider);

    const cg::world::MapObjectTexture* objectTexture = cache.GetOrLoad(100U, 1U);

    int failures = 0;
    failures += Expect(objectTexture == nullptr, message);
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(0), "非法图片不应进入缓存");
    failures += ExpectEquals(renderer.created.size(), static_cast<std::size_t>(0), "非法图片不应创建纹理");
    failures += ExpectEquals(provider.RequestCount(100U), static_cast<std::size_t>(1), "非法图片应只请求一次 provider");
    failures += Expect(!textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(100U)).IsValid(), "非法图片不应留下 TextureCache key");
    return failures;
}

int TestHitOnlyDecodesAndUploadsOnce()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(1U, MakeImage(2, 3, 10U, -2, 5));
    cg::world::MapObjectTextureCache cache(textureCache, provider);

    const cg::world::MapObjectTexture* first = cache.GetOrLoad(1U, 100U);
    const cg::world::MapObjectTexture* second = cache.GetOrLoad(1U, 200U);

    int failures = 0;
    failures += Expect(first != nullptr && second != nullptr, "命中前后都应返回纹理");
    failures += Expect(first == second, "命中应返回同一缓存对象");
    failures += ExpectEquals(provider.RequestCount(1U), static_cast<std::size_t>(1), "命中不应重复解码");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(1), "命中不应重复上传");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(1), "命中不应增加缓存数量");
    if (first != nullptr) {
        failures += ExpectEquals(first->imageNo, 1U, "imageNo 应透传");
        failures += ExpectEquals(first->width, 2, "width 应透传");
        failures += ExpectEquals(first->height, 3, "height 应透传");
        failures += ExpectEquals(first->offsetX, -2, "offsetX 应透传");
        failures += ExpectEquals(first->offsetY, 5, "offsetY 应透传");
    }
    return failures;
}

int TestCapacityTwoEvictsLeastRecentlyUsed()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(1U, MakeImage(1, 1, 10U));
    provider.images.emplace(2U, MakeImage(1, 1, 20U));
    provider.images.emplace(3U, MakeImage(1, 1, 30U));
    cg::world::MapObjectTextureCache cache(textureCache, provider, 2U);

    const cg::world::MapObjectTexture* first = cache.GetOrLoad(1U, 10U);
    const cg::world::MapObjectTexture* second = cache.GetOrLoad(2U, 20U);
    const bool firstLoaded = first != nullptr;
    const bool secondLoaded = second != nullptr;
    const std::uint32_t secondTextureId = second != nullptr ? second->texture.id : 0U;
    const cg::world::MapObjectTexture* hitFirst = cache.GetOrLoad(1U, 30U);
    const bool hitFirstLoaded = hitFirst != nullptr;
    const cg::world::MapObjectTexture* third = cache.GetOrLoad(3U, 40U);
    const bool thirdLoaded = third != nullptr;

    int failures = 0;
    failures += Expect(firstLoaded && secondLoaded && hitFirstLoaded && thirdLoaded, "1/2/1/3 加载应成功");
    failures += ExpectEquals(provider.RequestCount(1U), static_cast<std::size_t>(1), "刷新 LRU 的命中不应重新解码 1");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(3), "1/2/3 各上传一次");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(2), "容量 2 应只保留两个对象");
    failures += Expect(textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(1U)).IsValid(), "刷新后的 1 应保留");
    failures += Expect(!textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(2U)).IsValid(), "LRU 2 应被淘汰");
    failures += Expect(textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(3U)).IsValid(), "新加载的 3 应保留");
    failures += ExpectEquals(renderer.destroyed.size(), static_cast<std::size_t>(1), "淘汰 2 应销毁一个 texture");
    if (!renderer.destroyed.empty()) {
        failures += ExpectEquals(renderer.destroyed[0].id, secondTextureId, "被销毁的 texture 应对应 imageNo 2");
    }
    return failures;
}

int TestPruneIdleRemovesThirtySecondIdleAndIgnoresClockRollback()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(1U, MakeImage(1, 1, 10U));
    cg::world::MapObjectTextureCache cache(textureCache, provider, 100U, 30000U);

    const cg::world::MapObjectTexture* loaded = cache.GetOrLoad(1U, 1000U);
    const bool loadedOk = loaded != nullptr;
    cache.PruneIdle(999U);
    const bool keptAfterRollback = textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(1U)).IsValid();
    cache.PruneIdle(31000U);

    int failures = 0;
    failures += Expect(loadedOk, "idle 测试加载应成功");
    failures += Expect(keptAfterRollback, "nowMs 小于 lastUsedMs 时不应误删");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(0), "idle 满 30 秒应清退");
    failures += Expect(!textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(1U)).IsValid(), "idle 清退应移除 TextureCache key");
    failures += ExpectEquals(renderer.destroyed.size(), static_cast<std::size_t>(1), "idle 清退应销毁 texture");
    return failures;
}

int TestProviderNulloptDoesNotCache()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    cg::world::MapObjectTextureCache cache(textureCache, provider);

    const cg::world::MapObjectTexture* objectTexture = cache.GetOrLoad(10U, 1U);

    int failures = 0;
    failures += Expect(objectTexture == nullptr, "provider nullopt 应失败");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(0), "provider nullopt 不应缓存");
    failures += ExpectEquals(provider.RequestCount(10U), static_cast<std::size_t>(1), "provider nullopt 应记录一次请求");
    failures += ExpectEquals(renderer.created.size(), static_cast<std::size_t>(0), "provider nullopt 不应创建纹理");
    return failures;
}

int TestUploadFailureDoesNotCache()
{
    FakeRenderer renderer;
    renderer.updateSucceeds = false;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(20U, MakeImage(1, 1, 20U));
    cg::world::MapObjectTextureCache cache(textureCache, provider);

    const cg::world::MapObjectTexture* objectTexture = cache.GetOrLoad(20U, 1U);

    int failures = 0;
    failures += Expect(objectTexture == nullptr, "上传失败应返回空");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(0), "上传失败不应缓存");
    failures += ExpectEquals(provider.RequestCount(20U), static_cast<std::size_t>(1), "上传失败只解码一次");
    failures += ExpectEquals(renderer.updateCalls, static_cast<std::size_t>(1), "上传失败应调用一次 UpdateTexture");
    failures += ExpectEquals(renderer.destroyed.size(), static_cast<std::size_t>(1), "上传失败应销毁临时 texture");
    failures += Expect(!textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(20U)).IsValid(), "上传失败不应留下 TextureCache key");
    return failures;
}

int TestUploadFailureAtCapacityKeepsExistingCache()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(1U, MakeImage(1, 1, 10U));
    provider.images.emplace(2U, MakeImage(1, 1, 20U));
    cg::world::MapObjectTextureCache cache(textureCache, provider, 1U);

    const cg::world::MapObjectTexture* first = cache.GetOrLoad(1U, 1U);
    const std::uint32_t firstTextureId = first != nullptr ? first->texture.id : 0U;
    renderer.updateSucceeds = false;
    const cg::world::MapObjectTexture* failed = cache.GetOrLoad(2U, 2U);
    const cg::world::MapObjectTexture* hitFirst = cache.GetOrLoad(1U, 3U);
    const bool oldTextureDestroyed = std::any_of(
        renderer.destroyed.begin(),
        renderer.destroyed.end(),
        [firstTextureId](cg::render::TextureHandle texture) {
            return texture.id == firstTextureId;
        });

    int failures = 0;
    failures += Expect(first != nullptr, "容量失败测试初始加载应成功");
    failures += Expect(failed == nullptr, "满容量时新图上传失败应返回空");
    failures += Expect(hitFirst != nullptr && hitFirst->texture.id == firstTextureId, "上传失败后旧图应继续命中");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(1), "上传失败不应减少已有缓存");
    failures += ExpectEquals(provider.RequestCount(1U), static_cast<std::size_t>(1), "旧图后续命中不应重新解码");
    failures += ExpectEquals(provider.RequestCount(2U), static_cast<std::size_t>(1), "失败新图只解码一次");
    failures += ExpectEquals(renderer.uploads.size(), static_cast<std::size_t>(1), "旧图后续命中不应重新上传");
    failures += ExpectEquals(renderer.updateCalls, static_cast<std::size_t>(2), "只应有初始上传和失败上传");
    failures += Expect(textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(1U)).IsValid(), "上传失败后旧 texture key 应保留");
    failures += Expect(!textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(2U)).IsValid(), "失败新图不应留下 texture key");
    failures += Expect(!oldTextureDestroyed, "上传失败不应销毁旧 texture");
    return failures;
}

int TestCreateTextureFailureDoesNotCache()
{
    FakeRenderer renderer;
    renderer.createSucceeds = false;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(30U, MakeImage(1, 1, 30U));
    cg::world::MapObjectTextureCache cache(textureCache, provider);

    const cg::world::MapObjectTexture* objectTexture = cache.GetOrLoad(30U, 1U);

    int failures = 0;
    failures += Expect(objectTexture == nullptr, "CreateTexture 无效应返回空");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(0), "CreateTexture 无效不应缓存");
    failures += ExpectEquals(provider.RequestCount(30U), static_cast<std::size_t>(1), "CreateTexture 无效只解码一次");
    failures += ExpectEquals(renderer.created.size(), static_cast<std::size_t>(0), "CreateTexture 无效不应记录成功创建");
    failures += ExpectEquals(renderer.updateCalls, static_cast<std::size_t>(0), "CreateTexture 无效不应调用 UpdateTexture");
    failures += ExpectEquals(renderer.destroyed.size(), static_cast<std::size_t>(0), "CreateTexture 无效不应销毁 texture");
    failures += Expect(!textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(30U)).IsValid(), "CreateTexture 无效不应留下 TextureCache key");
    return failures;
}

int TestInvalidImagesDoNotCache()
{
    int failures = 0;
    failures += ExpectRejectedImage(MakeImage(0, 1, 10U), "宽高非法应失败");

    cg::resource::DecodedImage nonRgba = MakeImage(1, 1, 20U);
    nonRgba.format = cg::render::PixelFormat::Bgra8;
    failures += ExpectRejectedImage(std::move(nonRgba), "非 Rgba8 应失败");

    cg::resource::DecodedImage wrongSize = MakeImage(2, 2, 30U);
    wrongSize.pixels.pop_back();
    failures += ExpectRejectedImage(std::move(wrongSize), "pixel size 错误应失败");
    return failures;
}

int TestCapacityZeroDoesNotDecodeOrCache()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(1U, MakeImage(1, 1, 10U));
    cg::world::MapObjectTextureCache cache(textureCache, provider, 0U);

    const cg::world::MapObjectTexture* objectTexture = cache.GetOrLoad(1U, 1U);

    int failures = 0;
    failures += Expect(objectTexture == nullptr, "capacity=0 应直接失败");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(0), "capacity=0 不应缓存");
    failures += ExpectEquals(provider.requests.size(), static_cast<std::size_t>(0), "capacity=0 不应解码");
    failures += ExpectEquals(renderer.created.size(), static_cast<std::size_t>(0), "capacity=0 不应创建纹理");
    return failures;
}

int TestClearRemovesCachedTextures()
{
    FakeRenderer renderer;
    cg::render::TextureCache textureCache(renderer);
    FakeObjectImageProvider provider;
    provider.images.emplace(1U, MakeImage(1, 1, 10U));
    provider.images.emplace(2U, MakeImage(1, 1, 20U));
    cg::world::MapObjectTextureCache cache(textureCache, provider);

    const cg::world::MapObjectTexture* first = cache.GetOrLoad(1U, 1U);
    const cg::world::MapObjectTexture* second = cache.GetOrLoad(2U, 2U);
    const bool loaded = first != nullptr && second != nullptr;
    cache.Clear();

    int failures = 0;
    failures += Expect(loaded, "Clear 前加载应成功");
    failures += ExpectEquals(cache.Size(), static_cast<std::size_t>(0), "Clear 应清空缓存");
    failures += ExpectEquals(renderer.destroyed.size(), static_cast<std::size_t>(2), "Clear 应销毁全部自有 texture");
    failures += Expect(!textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(1U)).IsValid(), "Clear 应移除 imageNo 1 key");
    failures += Expect(!textureCache.Find(cg::world::MapObjectTextureCache::MakeTextureKey(2U)).IsValid(), "Clear 应移除 imageNo 2 key");
    return failures;
}

int TestMakeTextureKeyIsStable()
{
    const std::string first = cg::world::MapObjectTextureCache::MakeTextureKey(42U);
    const std::string second = cg::world::MapObjectTextureCache::MakeTextureKey(42U);
    const std::string other = cg::world::MapObjectTextureCache::MakeTextureKey(43U);

    int failures = 0;
    failures += ExpectEquals(first, second, "同 imageNo key 应稳定");
    failures += Expect(first != other, "不同 imageNo key 应不同");
    failures += ExpectEquals(first, std::string{"map-object-texture:imageNo=42"}, "key 格式应稳定");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestHitOnlyDecodesAndUploadsOnce();
    failures += TestCapacityTwoEvictsLeastRecentlyUsed();
    failures += TestPruneIdleRemovesThirtySecondIdleAndIgnoresClockRollback();
    failures += TestProviderNulloptDoesNotCache();
    failures += TestUploadFailureDoesNotCache();
    failures += TestUploadFailureAtCapacityKeepsExistingCache();
    failures += TestCreateTextureFailureDoesNotCache();
    failures += TestInvalidImagesDoNotCache();
    failures += TestCapacityZeroDoesNotDecodeOrCache();
    failures += TestClearRemovesCachedTextures();
    failures += TestMakeTextureKeyIsStable();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
