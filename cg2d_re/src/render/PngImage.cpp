#include "cg/render/PngImage.hpp"

#include "PngImageInternal.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/resource/GraphicDecode.hpp"

#include <Windows.h>
#include <wincodec.h>
#include <shlwapi.h>
#include <SDL3/SDL_surface.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <format>
#include <iterator>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace cg::render {
struct PixelImage {
    TextureSurfaceBinding surface;
    std::int32_t width = 0;
    std::int32_t height = 0;
};

// 本机 SDK COM 接口仅在解码边界内持有，所有失败分支按逆序释放。
struct ReleaseCom {
    void operator()(IUnknown* object) const noexcept { object->Release(); }
};
template <typename T>
using ComOwner = std::unique_ptr<T, ReleaseCom>;

class WicContext final {
public:
    WicContext()
    {
        initialized_ = SUCCEEDED(::CoInitialize(nullptr));
        if (initialized_) {
            IWICImagingFactory* factory = nullptr;
            const HRESULT result = ::CoCreateInstance(
                CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
            factory_.reset(factory);
            if (FAILED(result)) {
                factory_.reset();
            }
        }
    }
    ~WicContext()
    {
        factory_.reset();
        if (initialized_) {
            ::CoUninitialize();
        }
    }
    IWICImagingFactory* Get() const noexcept { return factory_.get(); }

private:
    bool initialized_ = false;
    ComOwner<IWICImagingFactory> factory_;
};

struct PngCacheEntry {
    std::wstring key;
    TextureSurfaceBinding surface;
    std::int32_t width = 0;
    std::int32_t height = 0;
    std::uint64_t lastAccess = 0;
};

struct PngRuntime {
    std::list<PngCacheEntry> entries; // 首项最近使用，尾项首先淘汰。
    std::unordered_map<std::wstring, std::list<PngCacheEntry>::iterator> index;
    std::array<PngImageBatch, MAX_QUEUE_SIZE> batches;
    std::uint32_t frame = 0;
    std::uint64_t lastClean = 0;
};

static PngRuntime& GetPngRuntime()
{
    // 首次业务调用后构造；析构先于启动前创建的 TextureSurfaceOwner。
    static PngRuntime runtime;
    return runtime;
}

static std::uint32_t ReadPngUint32Be(const std::uint8_t* bytes) noexcept
{
    return (static_cast<std::uint32_t>(bytes[0]) << 24)
        | (static_cast<std::uint32_t>(bytes[1]) << 16)
        | (static_cast<std::uint32_t>(bytes[2]) << 8)
        | static_cast<std::uint32_t>(bytes[3]);
}

static bool ReadPngImageSize(
    const std::vector<std::uint8_t>& data, std::int32_t& width, std::int32_t& height)
{
    if (data.size() < 24 || !resource::IsPngGraphicData(data.data())
        || std::memcmp(data.data() + 12, "IHDR", 4) != 0) {
        return false;
    }
    const std::uint32_t sourceWidth = ReadPngUint32Be(data.data() + 16);
    const std::uint32_t sourceHeight = ReadPngUint32Be(data.data() + 20);
    if (sourceWidth == 0 || sourceHeight == 0 || sourceWidth > 0x7FFF || sourceHeight > 0x7FFF) {
        return false;
    }
    width = static_cast<std::int32_t>(sourceWidth);
    height = static_cast<std::int32_t>(sourceHeight);
    return true;
}

static bool ClipPngDestination(
    TextureRectangle& destination, TextureRectangle& source, const PngRectangle* clip)
{
    if (clip == nullptr) {
        return true;
    }
    const TextureRectangle previous = destination;
    destination = {
        std::max(previous[0], (*clip)[0]), std::max(previous[1], (*clip)[1]),
        std::min(previous[2], (*clip)[2]), std::min(previous[3], (*clip)[3])
    };
    if (destination[2] <= destination[0] || destination[3] <= destination[1]) {
        return false;
    }
    const std::int32_t sourceWidth = source[2] - source[0];
    const std::int32_t sourceHeight = source[3] - source[1];
    const std::int32_t width = previous[2] - previous[0];
    const std::int32_t height = previous[3] - previous[1];
    source[0] += (destination[0] - previous[0]) * sourceWidth / width;
    source[1] += (destination[1] - previous[1]) * sourceHeight / height;
    source[2] -= (previous[2] - destination[2]) * sourceWidth / width;
    source[3] -= (previous[3] - destination[3]) * sourceHeight / height;
    return source[2] > source[0] && source[3] > source[1];
}

PixelImage* CreatePixelImage(const std::int32_t width, const std::int32_t height)
{
    if (width <= 0 || height <= 0 || width > 0x7FFF || height > 0x7FFF) return nullptr;
    auto image = std::make_unique<PixelImage>();
    TextureSurface* const surface = CreatePlainTextureSurface_44C8B0(
        static_cast<std::int16_t>(width), static_cast<std::int16_t>(height), 0, 0x4000);
    if (!surface) return nullptr;
    image->surface.Bind(surface);
    ReleaseTextureSurface(surface);
    image->width = width;
    image->height = height;
    return image.release();
}

bool UpdatePixelImage(PixelImage* const image, const std::uint32_t* const pixels,
    const std::int32_t width, const std::int32_t height)
{
    if (!image || !pixels || width != image->width || height != image->height) return false;
    SDL_Surface* const output = GetTextureSdlSurface(image->surface.Get());
    if (!output) return false;
    for (int row = 0; row < height; ++row) {
        std::memcpy(static_cast<std::uint8_t*>(output->pixels) + row * output->pitch,
            pixels + row * width, static_cast<std::size_t>(width) * sizeof(*pixels));
    }
    MarkTextureSurfaceModified(image->surface.Get());
    return true;
}

void ReleasePixelImage(PixelImage* const image) { delete image; }

std::int32_t DrawPixelImage(PixelImage* const image,
    const std::int32_t x, const std::int32_t y, const std::int32_t width, const std::int32_t height,
    const std::uint32_t color, const std::uint8_t layer, const PngRectangle* const clipRectangle)
{
    if (!image || !image->surface.Get() || width <= 0 || height <= 0) return -1;
    PngRuntime& runtime = GetPngRuntime();
    if (runtime.frame != platform::gFrameCount) {
        for (auto& batch : runtime.batches) batch.surface.Bind(nullptr);
        runtime.frame = platform::gFrameCount;
    }
    TextureRectangle source {0, 0, image->width, image->height};
    TextureRectangle destination {x, y, x + width, y + height};
    if (!ClipPngDestination(destination, source, clipRectangle)) return -1;
    const auto index = DrawGraphic(0, 0, layer, -2, 0, nullptr);
    if (index < 0) return index;
    auto& batch = runtime.batches[index];
    batch.surface.Bind(image->surface.Get());
    batch.source = source;
    batch.destination = destination;
    batch.color = color;
    auto& record = gLegacyPendingStorage.records[index];
    record.reference.pngImage = &batch;
    record.batchType = GraphicBatch_PngImage;
    return index;
}

std::size_t ReadPngRectangle(
    const std::span<const std::uint8_t> data, const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height, const std::span<std::uint32_t> pixels)
{
    if (data.empty() || pixels.empty() || width <= 0 || height <= 0) {
        return 0;
    }
    const std::size_t requiredSize =
        static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * sizeof(std::uint32_t);
    if (pixels.size_bytes() < requiredSize) {
        return 0;
    }
    thread_local WicContext context;
    IWICImagingFactory* const factory = context.Get();
    if (factory == nullptr) {
        return 0;
    }
    ComOwner<IStream> stream(::SHCreateMemStream(data.data(), static_cast<UINT>(data.size())));
    if (stream == nullptr) {
        return 0;
    }
    IWICBitmapDecoder* decoderPointer = nullptr;
    HRESULT result = factory->CreateDecoderFromStream(
        stream.get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoderPointer);
    ComOwner<IWICBitmapDecoder> decoder(decoderPointer);
    if (FAILED(result)) {
        return 0;
    }
    IWICBitmapFrameDecode* framePointer = nullptr;
    result = decoder->GetFrame(0, &framePointer);
    ComOwner<IWICBitmapFrameDecode> frame(framePointer);
    if (FAILED(result)) {
        return 0;
    }
    IWICFormatConverter* converterPointer = nullptr;
    result = factory->CreateFormatConverter(&converterPointer);
    ComOwner<IWICFormatConverter> converter(converterPointer);
    if (FAILED(result)) {
        return 0;
    }
    result = converter->Initialize(frame.get(), GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
    if (FAILED(result)) {
        return 0;
    }
    UINT imageWidth = 0;
    UINT imageHeight = 0;
    converter->GetSize(&imageWidth, &imageHeight);
    if (x < 0 || y < 0 || x + width > static_cast<std::int32_t>(imageWidth)
        || y + height > static_cast<std::int32_t>(imageHeight)) {
        return 0;
    }
    const WICRect rectangle {x, y, width, height};
    result = converter->CopyPixels(&rectangle, static_cast<UINT>(width * 4),
        static_cast<UINT>(requiredSize), reinterpret_cast<BYTE*>(pixels.data()));
    return SUCCEEDED(result) ? requiredSize : 0;
}

std::int32_t DrawPngImage(
    const wchar_t* const path, const PngRectangle* const sourceRectangle,
    const std::int32_t x, const std::int32_t y, const std::int32_t width, const std::int32_t height,
    const std::uint32_t color, const std::uint8_t layer, const PngRectangle* const clipRectangle)
{
    if (path == nullptr || gRenderer == nullptr || width <= 0 || height <= 0) {
        return -1;
    }
    std::int32_t sourceX = 0;
    std::int32_t sourceY = 0;
    std::int32_t loadWidth = 0;
    std::int32_t loadHeight = 0;
    if (sourceRectangle != nullptr) {
        sourceX = (*sourceRectangle)[0];
        sourceY = (*sourceRectangle)[1];
        loadWidth = (*sourceRectangle)[2] - sourceX;
        loadHeight = (*sourceRectangle)[3] - sourceY;
        if (sourceX < 0 || sourceY < 0 || loadWidth <= 0 || loadHeight <= 0
            || loadWidth > 0x7FFF || loadHeight > 0x7FFF) {
            return -1;
        }
    }
    const std::wstring key = sourceRectangle == nullptr ? std::format(L"{}:full", path)
        : std::format(L"{}:rect:{}:{}:{}:{}", path, sourceX, sourceY, loadWidth, loadHeight);
    PngRuntime& runtime = GetPngRuntime();
    if (runtime.frame != platform::gFrameCount) {
        for (auto& batch : runtime.batches) {
            batch.surface.Bind(nullptr);
        }
        runtime.frame = platform::gFrameCount;
    }
    auto found = runtime.index.find(key);
    if (found == runtime.index.end()) {
        std::FILE* file = nullptr;
        if (::_wfopen_s(&file, path, L"rb") != 0 || file == nullptr) {
            return -1;
        }
        if (::_fseeki64(file, 0, SEEK_END) != 0) {
            std::fclose(file);
            return -1;
        }
        const std::int64_t fileSize = ::_ftelli64(file);
        if (fileSize <= 0) {
            std::fclose(file);
            return -1;
        }
        std::rewind(file);
        std::vector<std::uint8_t> data(static_cast<std::size_t>(fileSize));
        const std::size_t readSize = std::fread(data.data(), 1, data.size(), file);
        std::fclose(file);
        if (readSize != data.size()) {
            return -1;
        }
        if (sourceRectangle == nullptr && !ReadPngImageSize(data, loadWidth, loadHeight)) {
            return -1;
        }
        if (loadWidth <= 0 || loadHeight <= 0 || loadWidth > 0x7FFF || loadHeight > 0x7FFF) {
            return -1;
        }
        std::vector<std::uint32_t> pixels(
            static_cast<std::size_t>(loadWidth) * static_cast<std::size_t>(loadHeight));
        if (ReadPngRectangle(data, sourceX, sourceY, loadWidth, loadHeight, pixels) != pixels.size() * 4) {
            return -1;
        }
        TextureSurface* const surface = CreatePlainTextureSurface_44C8B0(
            static_cast<std::int16_t>(loadWidth), static_cast<std::int16_t>(loadHeight), 0, 0x4000);
        if (surface == nullptr) {
            return -1;
        }
        SDL_Surface* const output = GetTextureSdlSurface(surface);
        for (std::int32_t row = 0; row < loadHeight; ++row) {
            std::memcpy(static_cast<std::uint8_t*>(output->pixels) + row * output->pitch,
                pixels.data() + row * loadWidth, static_cast<std::size_t>(loadWidth) * 4);
        }
        runtime.entries.emplace_front();
        auto entry = runtime.entries.begin();
        entry->key = key;
        entry->surface.Bind(surface);
        ReleaseTextureSurface(surface);
        entry->width = loadWidth;
        entry->height = loadHeight;
        runtime.index.emplace(key, entry);
        if (runtime.index.size() > 2048) {
            const auto last = std::prev(runtime.entries.end());
            runtime.index.erase(last->key);
            runtime.entries.erase(last);
        }
        found = runtime.index.find(key);
    }
    runtime.entries.splice(runtime.entries.begin(), runtime.entries, found->second);
    PngCacheEntry& image = *found->second;
    image.lastAccess = platform::ReadTickCount64();
    if (image.surface.Get() == nullptr) {
        return -1;
    }
    TextureRectangle source {0, 0, image.width, image.height};
    TextureRectangle destination {x, y, x + width, y + height};
    if (!ClipPngDestination(destination, source, clipRectangle)) {
        return -1;
    }
    const std::int32_t index = DrawGraphic(0, 0, layer, -2, 0, nullptr);
    if (index < 0) {
        return index;
    }
    PngImageBatch& batch = runtime.batches[index];
    batch.surface.Bind(image.surface.Get());
    batch.source = source;
    const auto drawX = static_cast<std::int16_t>(destination[0]);
    const auto drawY = static_cast<std::int16_t>(destination[1]);
    const auto drawWidth = static_cast<std::int16_t>(destination[2] - destination[0]);
    const auto drawHeight = static_cast<std::int16_t>(destination[3] - destination[1]);
    batch.destination = {drawX, drawY, drawX + drawWidth, drawY + drawHeight};
    batch.color = color;
    auto& record = gLegacyPendingStorage.records[index];
    record.reference.pngImage = &batch;
    record.batchType = GraphicBatch_PngImage;
    return index;
}

void DrawPngImageBatch(const PngImageBatch& batch)
{
    static_cast<void>(RenderImageTexture(
        batch.surface.Get(), batch.source, batch.destination, GraphicMixFlag_None, batch.color));
}

void CleanupPngImageCache()
{
    PngRuntime& runtime = GetPngRuntime();
    const std::uint64_t now = platform::ReadTickCount64();
    if (runtime.lastClean != 0 && now - runtime.lastClean < 10000u) {
        return;
    }
    runtime.lastClean = now;
    for (auto entry = runtime.entries.begin(); entry != runtime.entries.end();) {
        if (now - entry->lastAccess >= 60000u) {
            runtime.index.erase(entry->key);
            entry = runtime.entries.erase(entry);
        } else {
            ++entry;
        }
    }
}

void ReleasePngImageResources() noexcept
{
    PngRuntime& runtime = GetPngRuntime();
    for (auto& batch : runtime.batches) {
        batch.surface.Bind(nullptr);
    }
    runtime.index.clear();
    runtime.entries.clear();
}

} // namespace cg::render
