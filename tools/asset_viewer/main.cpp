#include "resource/BinSetConfigLoader.hpp"
#include "resource/Palette.hpp"
#include "resource/ResourceManager.hpp"
#include "platform/Console.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <limits>
#include <optional>
#include <ostream>
#include <streambuf>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace {

constexpr int RequiredArgCount = 3;
constexpr int InitialMaxWindowWidth = 1024;
constexpr int InitialMaxWindowHeight = 768;
constexpr int InitialMinWindowWidth = 320;
constexpr int InitialMinWindowHeight = 240;
constexpr int Rgba8BytesPerPixel = 4;
constexpr int PaletteGridColumns = 16;
constexpr int PaletteGridRows = 16;
constexpr int PaletteCellSize = 24;

struct SearchOptions {
    std::optional<int> binSetIndex;
    std::optional<int> seqNo;
    std::optional<int> mapNo;
    std::optional<std::filesystem::path> exportPngPath;
    bool show = false;
    bool showPalette = false;
};

[[nodiscard]] std::string PathToUtf8(const std::filesystem::path& path)
{
    const std::u8string value = path.u8string();
    std::string text;
    text.reserve(value.size());
    for (const char8_t ch : value) {
        text.push_back(static_cast<char>(ch));
    }
    return text;
}

[[nodiscard]] std::filesystem::path PathFromUtf8(std::string_view text)
{
    std::u8string value;
    value.reserve(text.size());
    for (const char ch : text) {
        value.push_back(static_cast<char8_t>(ch));
    }
    return std::filesystem::path(value);
}

[[nodiscard]] bool ParseInt(std::string_view text, int& value)
{
    if (text.empty()) {
        return false;
    }

    int parsed = 0;
    const char* const begin = text.data();
    const char* const end = begin + text.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc {} || result.ptr != end) {
        return false;
    }

    value = parsed;
    return true;
}

void PrintUsage(std::ostream& output)
{
    output << "用法：cg-asset-viewer <cgmsv.yml> <palette.cgp> [--bin-set <index>] [--seq <seqNo>] [--map-no <mapNo>] [--show] [--show-palette] [--export-png <path>]\n";
}

void PrintResourceError(std::string_view prefix, const cg::resource::ResourceError& error)
{
    std::cerr << "失败：" << prefix;
    if (!error.message.empty()) {
        std::cerr << "：" << error.message;
    }
    if (!error.path.empty()) {
        std::cerr << "，路径：" << PathToUtf8(error.path);
    }
    if (error.resourceId != 0) {
        std::cerr << "，资源编号：" << error.resourceId;
    }
    std::cerr << '\n';
}

void PrintSdlError(std::string_view prefix)
{
    std::cerr << "失败：" << prefix << "：" << SDL_GetError() << '\n';
}

void PrintPaletteError(std::string_view prefix, const cg::resource::PaletteLoadResult& result)
{
    std::cerr << "失败：" << prefix;
    switch (result.error) {
    case cg::resource::PaletteLoadError::FileOpenFailed:
        std::cerr << "：无法打开文件";
        break;
    case cg::resource::PaletteLoadError::UnexpectedEof:
        std::cerr << "：文件长度不足";
        break;
    case cg::resource::PaletteLoadError::None:
    default:
        break;
    }
    if (!result.path.empty()) {
        std::cerr << "，路径：" << PathToUtf8(result.path);
    }
    std::cerr << '\n';
}

[[nodiscard]] bool ReadOptionValue(int argc, char* argv[], int& index, std::string_view option, int& value)
{
    if (index + 1 >= argc) {
        std::cerr << "失败：" << option << " 缺少参数值\n";
        return false;
    }

    ++index;
    if (!ParseInt(std::string_view { argv[index] }, value)) {
        std::cerr << "失败：" << option << " 必须是整数\n";
        return false;
    }
    return true;
}

[[nodiscard]] bool ReadPathOptionValue(int argc, char* argv[], int& index, std::string_view option, std::filesystem::path& value)
{
    if (index + 1 >= argc) {
        std::cerr << "失败：" << option << " 缺少参数值\n";
        return false;
    }

    ++index;
    value = PathFromUtf8(argv[index]);
    return true;
}

[[nodiscard]] bool ParseOptions(int argc, char* argv[], SearchOptions& options)
{
    for (int index = RequiredArgCount; index < argc; ++index) {
        const std::string_view arg { argv[index] };
        int value = 0;

        if (arg == "--bin-set" || arg == "--binSet" || arg == "-b") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            if (value < 0) {
                std::cerr << "失败：binSet index 不能为负数\n";
                return false;
            }
            options.binSetIndex = value;
        } else if (arg == "--seq" || arg == "--seq-no" || arg == "-s") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            options.seqNo = value;
        } else if (arg == "--map-no" || arg == "--mapNo" || arg == "-m") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            options.mapNo = value;
        } else if (arg == "--show") {
            options.show = true;
        } else if (arg == "--show-palette") {
            options.showPalette = true;
        } else if (arg == "--export-png") {
            std::filesystem::path path;
            if (!ReadPathOptionValue(argc, argv, index, arg, path)) {
                return false;
            }
            options.exportPngPath = std::move(path);
        } else {
            std::cerr << "失败：未知参数 " << arg << '\n';
            return false;
        }
    }

    return true;
}

[[nodiscard]] bool HasSearchFilter(const SearchOptions& options) noexcept
{
    return options.binSetIndex.has_value() || options.seqNo.has_value() || options.mapNo.has_value();
}

[[nodiscard]] bool Matches(const cg::resource::GraphicRef& ref, const SearchOptions& options) noexcept
{
    if (options.binSetIndex.has_value() && ref.binSetIndex != *options.binSetIndex) {
        return false;
    }
    if (options.seqNo.has_value() && ref.seqNo != *options.seqNo) {
        return false;
    }
    if (options.mapNo.has_value() && ref.mapNo != *options.mapNo) {
        return false;
    }
    return true;
}

void PrintSearchOptions(const SearchOptions& options)
{
    std::cout << "搜索条件：";
    bool printed = false;
    if (options.binSetIndex.has_value()) {
        std::cout << "binSet=" << *options.binSetIndex;
        printed = true;
    }
    if (options.seqNo.has_value()) {
        std::cout << (printed ? "，" : "") << "seqNo=" << *options.seqNo;
        printed = true;
    }
    if (options.mapNo.has_value()) {
        std::cout << (printed ? "，" : "") << "mapNo=" << *options.mapNo;
    }
    std::cout << '\n';
}

void PrintGraphic(const cg::resource::GraphicRef& ref, const std::vector<cg::resource::BinSetDesc>& binSets)
{
    std::cout << "binSet=" << ref.binSetIndex;
    const cg::resource::BinSetDesc* binSet = nullptr;
    if (ref.binSetIndex >= 0 && static_cast<std::size_t>(ref.binSetIndex) < binSets.size()) {
        binSet = &binSets[static_cast<std::size_t>(ref.binSetIndex)];
        std::cout << '(' << binSet->name << ')';
    }
    std::cout << "，seqNo=" << ref.seqNo
              << "，mapNo=" << ref.mapNo
              << "，尺寸=" << ref.info.w << 'x' << ref.info.h
              << "，数据偏移=" << ref.info.offset
              << "，数据长度=" << ref.info.length
              << "，坐标偏移=(" << ref.info.x << ',' << ref.info.y << ')'
              << "，dataPath=" << (binSet == nullptr ? std::string {} : PathToUtf8(binSet->graphicDataPath)) << '\n';
}

[[nodiscard]] const char* PixelFormatName(cg::render::PixelFormat format) noexcept
{
    switch (format) {
    case cg::render::PixelFormat::Rgba8:
        return "RGBA8";
    case cg::render::PixelFormat::Bgra8:
        return "BGRA8";
    case cg::render::PixelFormat::Unknown:
    default:
        return "Unknown";
    }
}

[[nodiscard]] std::size_t CountAlphaZeroPixels(const cg::resource::DecodedImage& image) noexcept
{
    if (image.pixels.size() < Rgba8BytesPerPixel) {
        return 0;
    }

    std::size_t count = 0;
    for (std::size_t offset = 3; offset < image.pixels.size(); offset += Rgba8BytesPerPixel) {
        if (image.pixels[offset] == 0) {
            ++count;
        }
    }
    return count;
}

[[nodiscard]] std::string BuildGraphicTitle(
    const cg::resource::GraphicRef& ref,
    const cg::resource::DecodedImage& image,
    std::size_t alphaZeroCount)
{
    return "cg-asset-viewer mapNo=" + std::to_string(ref.mapNo)
        + " seqNo=" + std::to_string(ref.seqNo)
        + " size=" + std::to_string(image.width) + "x" + std::to_string(image.height)
        + " offset=(" + std::to_string(image.offsetX) + "," + std::to_string(image.offsetY) + ")"
        + " format=" + PixelFormatName(image.format)
        + " alpha0=" + std::to_string(alphaZeroCount);
}

void PrintDecodedGraphic(
    const cg::resource::GraphicRef& ref,
    const cg::resource::DecodedImage& image,
    std::size_t alphaZeroCount)
{
    const std::size_t pixelCount = image.pixels.size() / Rgba8BytesPerPixel;
    std::cout << "显示 Graphic：binSet=" << ref.binSetIndex
              << "，seqNo=" << ref.seqNo
              << "，mapNo=" << ref.mapNo
              << "，尺寸=" << image.width << 'x' << image.height
              << "，坐标偏移=(" << image.offsetX << ',' << image.offsetY << ')'
              << "，数据偏移=" << ref.info.offset
              << "，数据长度=" << ref.info.length
              << "，像素格式=" << PixelFormatName(image.format)
              << "，透明像素(alpha==0)=" << alphaZeroCount << '/' << pixelCount
              << "，compressed=" << (image.dataMetadata.compressed ? "true" : "false")
              << "，hasEmbeddedPalette=" << (image.dataMetadata.hasEmbeddedPalette ? "true" : "false")
              << "，paletteSize=" << image.dataMetadata.paletteSize
              << "，headerSize=" << image.dataMetadata.headerSize
              << "，zeroTransparent=" << (image.dataMetadata.zeroTransparent ? "true" : "false")
              << '\n';
}

void PrintPaletteSample(const cg::resource::Palette& palette)
{
    constexpr std::uint8_t sampleIndices[] {0, 1, 15, 16, 127, 239, 240, 255};

    std::cout << "默认 palette 色数：" << cg::resource::Palette::ColorCount << '\n';
    for (const std::uint8_t index : sampleIndices) {
        const cg::resource::Rgba8& color = palette[index];
        std::cout << "palette[" << static_cast<int>(index) << "]=("
                  << static_cast<int>(color.r) << ','
                  << static_cast<int>(color.g) << ','
                  << static_cast<int>(color.b) << ','
                  << static_cast<int>(color.a) << ")\n";
    }
}

void PrintStats(
    const cg::resource::ResourceManager& resources,
    const std::vector<cg::resource::BinSetDesc>& binSets,
    const std::filesystem::path& ymlPath,
    const std::filesystem::path& palettePath)
{
    const cg::resource::ResourceStats& stats = resources.Stats();
    std::cout << "配置路径：" << PathToUtf8(ymlPath) << '\n';
    std::cout << "palette 路径：" << PathToUtf8(palettePath) << '\n';
    std::cout << "binSet 数：" << binSets.size() << "，已加载：" << stats.binSetCount << '\n';
    std::cout << "Graphic 数：" << stats.graphicCount << "，Anime 数：" << stats.animeCount << '\n';
    std::cout << "Graphic 覆盖：" << stats.graphicOverrideCount
              << "，Anime 覆盖：" << stats.animeOverrideCount
              << "，警告：" << stats.warningCount
              << "，错误：" << stats.errorCount << '\n';
    for (std::size_t index = 0; index < binSets.size(); ++index) {
        std::cout << "binSet[" << index << "] " << binSets[index].name
                  << "，Graphic=" << resources.Graphics(static_cast<int>(index)).size()
                  << "，dataPath=" << PathToUtf8(binSets[index].graphicDataPath) << '\n';
    }
}

[[nodiscard]] std::vector<cg::resource::GraphicRef> FindSearchResults(
    const cg::resource::ResourceManager& resources,
    const std::vector<cg::resource::BinSetDesc>& binSets,
    const SearchOptions& options)
{
    std::vector<cg::resource::GraphicRef> matches;
    const int firstBinSet = options.binSetIndex.value_or(0);
    const int endBinSet = options.binSetIndex.value_or(static_cast<int>(binSets.size()) - 1);

    for (int binSetIndex = firstBinSet; binSetIndex <= endBinSet; ++binSetIndex) {
        for (const cg::resource::GraphicRef& ref : resources.Graphics(binSetIndex)) {
            if (!Matches(ref, options)) {
                continue;
            }
            matches.push_back(ref);
        }
    }

    return matches;
}

void PrintSearchResults(
    const std::vector<cg::resource::GraphicRef>& matches,
    const std::vector<cg::resource::BinSetDesc>& binSets)
{
    for (const cg::resource::GraphicRef& ref : matches) {
        PrintGraphic(ref, binSets);
    }
}

[[nodiscard]] const cg::resource::GraphicRef* SelectGraphicForAction(
    const std::vector<cg::resource::GraphicRef>& matches,
    const SearchOptions& options,
    std::string_view actionName)
{
    if (matches.empty()) {
        std::cerr << "失败：" << actionName << " 未命中 Graphic\n";
        return nullptr;
    }

    if (matches.size() == 1) {
        return &matches.front();
    }

    if ((options.binSetIndex.has_value() && options.seqNo.has_value()) || options.mapNo.has_value()) {
        std::cout << actionName << "选择：指定条件命中多条，使用第一条\n";
        return &matches.front();
    }

    std::cerr << "失败：" << actionName << " 需要唯一 Graphic，当前命中数量：" << matches.size() << '\n';
    return nullptr;
}

[[nodiscard]] bool IsValidImageBuffer(const cg::resource::DecodedImage& image) noexcept
{
    if (image.width <= 0 || image.height <= 0 || image.pixels.empty()) {
        return false;
    }

    const std::uint64_t width = static_cast<std::uint64_t>(image.width);
    const std::uint64_t height = static_cast<std::uint64_t>(image.height);
    const std::uint64_t expectedSize = width * height * Rgba8BytesPerPixel;
    return expectedSize <= std::numeric_limits<std::size_t>::max()
        && image.pixels.size() == static_cast<std::size_t>(expectedSize);
}

[[nodiscard]] float ComputeFitScale(const cg::resource::DecodedImage& image, int windowWidth, int windowHeight) noexcept
{
    if (image.width <= 0 || image.height <= 0 || windowWidth <= 0 || windowHeight <= 0) {
        return 1.0F;
    }

    const float fitX = static_cast<float>(windowWidth) / static_cast<float>(image.width);
    const float fitY = static_cast<float>(windowHeight) / static_cast<float>(image.height);
    return std::min(1.0F, std::min(fitX, fitY));
}

[[nodiscard]] SDL_FRect ComputeImageDestination(
    const cg::resource::DecodedImage& image,
    int windowWidth,
    int windowHeight) noexcept
{
    const float scale = ComputeFitScale(image, windowWidth, windowHeight);
    const float width = static_cast<float>(image.width) * scale;
    const float height = static_cast<float>(image.height) * scale;
    return SDL_FRect {
        (static_cast<float>(windowWidth) - width) * 0.5F,
        (static_cast<float>(windowHeight) - height) * 0.5F,
        width,
        height,
    };
}

[[nodiscard]] bool RenderGraphicFrame(
    SDL_Window* window,
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    const cg::resource::DecodedImage& image)
{
    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    const SDL_FRect destination = ComputeImageDestination(image, windowWidth, windowHeight);

    if (!SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255) || !SDL_RenderClear(renderer)) {
        return false;
    }
    if (!SDL_RenderTexture(renderer, texture, nullptr, &destination)) {
        return false;
    }
    return SDL_RenderPresent(renderer);
}

[[nodiscard]] bool RenderPaletteFrame(SDL_Renderer* renderer, const cg::resource::Palette& palette)
{
    if (!SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255) || !SDL_RenderClear(renderer)) {
        return false;
    }

    for (std::size_t index = 0; index < cg::resource::Palette::ColorCount; ++index) {
        const cg::resource::Rgba8& color = palette[static_cast<std::uint8_t>(index)];
        if (!SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a)) {
            return false;
        }

        const int x = static_cast<int>(index % PaletteGridColumns) * PaletteCellSize;
        const int y = static_cast<int>(index / PaletteGridColumns) * PaletteCellSize;
        const SDL_FRect rect {
            static_cast<float>(x),
            static_cast<float>(y),
            static_cast<float>(PaletteCellSize),
            static_cast<float>(PaletteCellSize),
        };
        if (!SDL_RenderFillRect(renderer, &rect)) {
            return false;
        }
    }

    return SDL_RenderPresent(renderer);
}

[[nodiscard]] bool ShowPaletteWindow(const cg::resource::Palette& palette)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        PrintSdlError("SDL 初始化失败");
        return false;
    }

    constexpr int windowWidth = PaletteGridColumns * PaletteCellSize;
    constexpr int windowHeight = PaletteGridRows * PaletteCellSize;
    SDL_Window* window = SDL_CreateWindow("cg-asset-viewer default palette 16x16", windowWidth, windowHeight, 0);
    if (window == nullptr) {
        PrintSdlError("创建 palette 窗口失败");
        SDL_Quit();
        return false;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        PrintSdlError("创建 renderer 失败");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    std::cout << "palette 窗口已打开：ESC 或关闭窗口退出\n";
    std::cout.flush();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE || event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    running = false;
                }
                break;
            default:
                break;
            }
        }

        if (!RenderPaletteFrame(renderer, palette)) {
            PrintSdlError("渲染 palette 失败");
            running = false;
        }
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
}

[[nodiscard]] bool ShowGraphicWindow(
    const cg::resource::GraphicRef& ref,
    const cg::resource::DecodedImage& image,
    std::size_t alphaZeroCount)
{
    if (!IsValidImageBuffer(image)) {
        std::cerr << "失败：解码后的 Graphic 图像为空或像素尺寸不匹配\n";
        return false;
    }
    if (image.format != cg::render::PixelFormat::Rgba8) {
        std::cerr << "失败：当前仅支持显示 RGBA8 Graphic\n";
        return false;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        PrintSdlError("SDL 初始化失败");
        return false;
    }

    const std::string title = BuildGraphicTitle(ref, image, alphaZeroCount);
    const int windowWidth = std::clamp(image.width, InitialMinWindowWidth, InitialMaxWindowWidth);
    const int windowHeight = std::clamp(image.height, InitialMinWindowHeight, InitialMaxWindowHeight);
    SDL_Window* window = SDL_CreateWindow(title.c_str(), windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        PrintSdlError("创建窗口失败");
        SDL_Quit();
        return false;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        PrintSdlError("创建 renderer 失败");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STATIC,
        image.width,
        image.height);
    if (texture == nullptr) {
        PrintSdlError("创建 Graphic 纹理失败");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST)
        || !SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND)
        || !SDL_UpdateTexture(texture, nullptr, image.pixels.data(), image.width * Rgba8BytesPerPixel)) {
        PrintSdlError("上传 Graphic 纹理失败");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    std::cout << "窗口已打开：ESC 或关闭窗口退出\n";
    std::cout.flush();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE || event.key.scancode == SDL_SCANCODE_ESCAPE) {
                    running = false;
                }
                break;
            default:
                break;
            }
        }

        if (!RenderGraphicFrame(window, renderer, texture, image)) {
            PrintSdlError("渲染 Graphic 失败");
            running = false;
        }
        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
}

[[nodiscard]] bool ExportGraphicPng(
    const cg::resource::GraphicRef& ref,
    const cg::resource::DecodedImage& image,
    const std::filesystem::path& path,
    std::size_t alphaZeroCount)
{
    if (!IsValidImageBuffer(image)) {
        std::cerr << "失败：解码后的 Graphic 图像为空或像素尺寸不匹配\n";
        return false;
    }
    if (image.format != cg::render::PixelFormat::Rgba8) {
        std::cerr << "失败：当前仅支持导出 RGBA8 Graphic\n";
        return false;
    }
    if (image.width > std::numeric_limits<int>::max() / Rgba8BytesPerPixel) {
        std::cerr << "失败：Graphic 行跨度超出 SDL surface 限制\n";
        return false;
    }

    SDL_Surface* surface = SDL_CreateSurface(image.width, image.height, SDL_PIXELFORMAT_RGBA32);
    if (surface == nullptr) {
        PrintSdlError("创建 PNG surface 失败");
        return false;
    }

    const std::size_t sourcePitch = static_cast<std::size_t>(image.width) * Rgba8BytesPerPixel;
    for (int y = 0; y < image.height; ++y) {
        std::memcpy(
            static_cast<std::uint8_t*>(surface->pixels) + static_cast<std::size_t>(y) * surface->pitch,
            image.pixels.data() + static_cast<std::size_t>(y) * sourcePitch,
            sourcePitch);
    }

    const std::string savePath = PathToUtf8(path);
    const bool saved = IMG_SavePNG(surface, savePath.c_str());
    SDL_DestroySurface(surface);
    if (!saved) {
        PrintSdlError("保存 PNG 失败");
        return false;
    }

    const std::size_t pixelCount = image.pixels.size() / Rgba8BytesPerPixel;
    std::cout << "导出 PNG：保存路径=" << savePath
              << "，binSet=" << ref.binSetIndex
              << "，seqNo=" << ref.seqNo
              << "，mapNo=" << ref.mapNo
              << "，尺寸=" << image.width << 'x' << image.height
              << "，像素格式=RGBA8"
              << "，透明像素(alpha==0)=" << alphaZeroCount << '/' << pixelCount
              << '\n';
    return true;
}

class NullStreamBuffer final : public std::streambuf {
protected:
    int overflow(int ch) override
    {
        return ch;
    }
};

class ScopedStreamRedirect final {
public:
    explicit ScopedStreamRedirect(std::ostream& stream)
        : stream_(stream)
        , oldBuffer_(stream.rdbuf(&sink_))
    {
    }

    ScopedStreamRedirect(const ScopedStreamRedirect&) = delete;
    ScopedStreamRedirect& operator=(const ScopedStreamRedirect&) = delete;

    ~ScopedStreamRedirect()
    {
        stream_.rdbuf(oldBuffer_);
    }

private:
    std::ostream& stream_;
    std::streambuf* oldBuffer_ = nullptr;
    NullStreamBuffer sink_;
};

} // namespace

int main(int argc, char* argv[])
{
    cg::platform::ConfigureConsoleUtf8();
    if (argc == 2 && std::string_view { argv[1] } == "--help") {
        PrintUsage(std::cout);
        return 0;
    }
    if (argc < RequiredArgCount) {
        PrintUsage(std::cerr);
        return 2;
    }

    SearchOptions options;
    if (!ParseOptions(argc, argv, options)) {
        PrintUsage(std::cerr);
        return 2;
    }

    const std::filesystem::path ymlPath = PathFromUtf8(argv[1]);
    const std::filesystem::path palettePath = PathFromUtf8(argv[2]);
    const cg::resource::BinSetConfigLoadResult config = cg::resource::LoadBinSetConfig(ymlPath);
    if (!config.IsOk()) {
        PrintResourceError("yml 加载失败", config.error);
        return 1;
    }

    if (options.binSetIndex.has_value() && static_cast<std::size_t>(*options.binSetIndex) >= config.binSets.size()) {
        std::cerr << "失败：binSet index 超出范围，最大 index=" << (config.binSets.empty() ? 0 : config.binSets.size() - 1) << '\n';
        return 2;
    }

    cg::resource::ResourceManager resources;
    bool binSetsLoaded = false;
    cg::resource::ResourceError binSetError;
    {
        ScopedStreamRedirect suppressInfo(std::cout);
        ScopedStreamRedirect suppressError(std::cerr);
        binSetsLoaded = resources.LoadBinSets(
            config.binSets,
            palettePath,
            cg::resource::ResourceLoadOptions { .buildAutoMapColorCache = false });
        if (!binSetsLoaded) {
            binSetError = resources.LastError();
        }
    }
    if (!binSetsLoaded) {
        PrintResourceError("binSet 加载失败", binSetError);
        return 1;
    }

    PrintStats(resources, config.binSets, ymlPath, palettePath);
    if (options.showPalette) {
        const cg::resource::PaletteLoadResult paletteResult = cg::resource::LoadPaletteFromFile(palettePath);
        if (!paletteResult) {
            PrintPaletteError("palette 加载失败", paletteResult);
            return 1;
        }
        PrintPaletteSample(paletteResult.palette);
        if (!ShowPaletteWindow(paletteResult.palette)) {
            return 1;
        }
    }

    if (!HasSearchFilter(options)) {
        if (options.show || options.exportPngPath.has_value()) {
            std::cerr << "失败：--show 或 --export-png 需要配合 --bin-set、--seq 或 --map-no 搜索条件\n";
            return 2;
        }
        return 0;
    }

    PrintSearchOptions(options);
    const std::vector<cg::resource::GraphicRef> matches = FindSearchResults(resources, config.binSets, options);
    PrintSearchResults(matches, config.binSets);
    std::cout << "命中数量：" << matches.size() << '\n';
    if (!options.show && !options.exportPngPath.has_value()) {
        return 0;
    }

    const cg::resource::GraphicRef* selected = SelectGraphicForAction(
        matches,
        options,
        options.exportPngPath.has_value() ? "--export-png" : "--show");
    if (selected == nullptr) {
        return 2;
    }

    std::optional<cg::resource::DecodedImage> image = resources.DecodeGraphic(*selected);
    if (!image.has_value()) {
        PrintResourceError("Graphic 解码失败", resources.LastError());
        return 1;
    }

    const std::size_t alphaZeroCount = CountAlphaZeroPixels(*image);
    PrintDecodedGraphic(*selected, *image, alphaZeroCount);
    if (options.exportPngPath.has_value() && !ExportGraphicPng(*selected, *image, *options.exportPngPath, alphaZeroCount)) {
        return 1;
    }
    if (!options.show) {
        return 0;
    }
    if (!ShowGraphicWindow(*selected, *image, alphaZeroCount)) {
        return 1;
    }
    return 0;
}
