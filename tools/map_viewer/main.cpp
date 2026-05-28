#include "resource/AutoMapRenderer.hpp"
#include "resource/BinSetConfigLoader.hpp"
#include "resource/MapLoader.hpp"
#include "resource/ResourceManager.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <ostream>
#include <streambuf>
#include <string>
#include <string_view>
#include <system_error>

namespace {

constexpr int ExpectedArgCount = 7;
constexpr int InitialWindowWidth = 1024;
constexpr int InitialWindowHeight = 768;
constexpr float MinZoom = 0.25F;
constexpr float MaxZoom = 16.0F;
constexpr float ZoomStep = 1.15F;
constexpr float PanPixelsPerFrame = 16.0F;
constexpr int Rgba8BytesPerPixel = 4;

struct ViewState {
    float offsetX = 0.0F;
    float offsetY = 0.0F;
    float scale = 1.0F;
};

struct MapCoord {
    int x = 0;
    int y = 0;
};

[[nodiscard]] std::string BuildLayerStateText(const cg::resource::AutoMapRenderOptions& options)
{
    return "tile=" + std::string(options.drawTiles ? "on" : "off")
        + " object=" + std::string(options.drawObjects ? "on" : "off")
        + " collision=" + std::string(options.drawCollision ? "on" : "off");
}

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

template <typename T>
[[nodiscard]] bool ParseUnsigned(std::string_view text, T& value)
{
    static_assert(std::numeric_limits<T>::is_integer);
    static_assert(!std::numeric_limits<T>::is_signed);

    if (text.empty()) {
        return false;
    }

    std::uint64_t parsed = 0;
    const char* const begin = text.data();
    const char* const end = begin + text.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc {} || result.ptr != end || parsed > std::numeric_limits<T>::max()) {
        return false;
    }

    value = static_cast<T>(parsed);
    return true;
}

void PrintUsage()
{
    std::cerr << "用法：cg-map-viewer <cgmsv.yml> <mapRoot> <mapId> <serverNo> <floor> <palette.cgp>\n";
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

[[nodiscard]] float ClampZoom(float scale) noexcept
{
    return std::clamp(scale, MinZoom, MaxZoom);
}

[[nodiscard]] float ComputeInitialScale(const cg::resource::AutoMapImage& image, int windowWidth, int windowHeight) noexcept
{
    if (image.width <= 0 || image.height <= 0 || windowWidth <= 0 || windowHeight <= 0) {
        return 1.0F;
    }

    const float fitX = static_cast<float>(windowWidth) * 0.9F / static_cast<float>(image.width);
    const float fitY = static_cast<float>(windowHeight) * 0.9F / static_cast<float>(image.height);
    return ClampZoom(std::min(1.0F, std::min(fitX, fitY)));
}

[[nodiscard]] ViewState MakeInitialView(const cg::resource::AutoMapImage& image, int windowWidth, int windowHeight) noexcept
{
    ViewState view;
    view.scale = ComputeInitialScale(image, windowWidth, windowHeight);
    view.offsetX = (static_cast<float>(windowWidth) - static_cast<float>(image.width) * view.scale) * 0.5F;
    view.offsetY = (static_cast<float>(windowHeight) - static_cast<float>(image.height) * view.scale) * 0.5F;
    return view;
}

void ZoomAt(ViewState& view, float mouseX, float mouseY, float wheelY) noexcept
{
    if (wheelY == 0.0F) {
        return;
    }

    const float oldScale = view.scale;
    const float newScale = ClampZoom(view.scale * static_cast<float>(std::pow(ZoomStep, wheelY)));
    if (newScale == oldScale) {
        return;
    }

    const float imageX = (mouseX - view.offsetX) / oldScale;
    const float imageY = (mouseY - view.offsetY) / oldScale;
    view.scale = newScale;
    view.offsetX = mouseX - imageX * newScale;
    view.offsetY = mouseY - imageY * newScale;
}

void PanByKeyboard(ViewState& view) noexcept
{
    const bool* const keys = SDL_GetKeyboardState(nullptr);
    if (keys == nullptr) {
        return;
    }

    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {
        view.offsetX += PanPixelsPerFrame;
    }
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
        view.offsetX -= PanPixelsPerFrame;
    }
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
        view.offsetY += PanPixelsPerFrame;
    }
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
        view.offsetY -= PanPixelsPerFrame;
    }
}

[[nodiscard]] MapCoord ScreenToMapCoord(
    const ViewState& view,
    const cg::resource::MapData& map,
    float screenX,
    float screenY) noexcept
{
    const double autoX = (static_cast<double>(screenX) - view.offsetX) / view.scale;
    const double autoY = (static_cast<double>(screenY) - view.offsetY) / view.scale;
    const double sum = (autoX - 1.0) * 0.5;
    const double diff = autoY - static_cast<double>(map.width) - 1.0;
    return MapCoord {
        static_cast<int>(std::lround((sum - diff) * 0.5)),
        static_cast<int>(std::lround((sum + diff) * 0.5)),
    };
}

[[nodiscard]] std::string BuildWindowTitle(
    const ViewState& view,
    const cg::resource::MapData& map,
    const cg::resource::AutoMapRenderOptions& options,
    float mouseX,
    float mouseY)
{
    const MapCoord coord = ScreenToMapCoord(view, map, mouseX, mouseY);
    return "cg-map-viewer map=(" + std::to_string(coord.x) + "," + std::to_string(coord.y) + ") zoom="
        + std::to_string(view.scale) + " layers[" + BuildLayerStateText(options) + "]";
}

void UpdateWindowTitle(
    SDL_Window* window,
    const ViewState& view,
    const cg::resource::MapData& map,
    const cg::resource::AutoMapRenderOptions& options,
    float mouseX,
    float mouseY)
{
    const std::string title = BuildWindowTitle(view, map, options, mouseX, mouseY);
    SDL_SetWindowTitle(window, title.c_str());
}

void PrintLayerState(const cg::resource::AutoMapRenderOptions& options)
{
    std::cout << "图层状态：" << BuildLayerStateText(options) << '\n';
    std::cout.flush();
}

[[nodiscard]] bool UploadAutoMapTexture(SDL_Texture* texture, const cg::resource::AutoMapImage& image)
{
    return SDL_UpdateTexture(texture, nullptr, image.pixels.data(), image.width * Rgba8BytesPerPixel);
}

[[nodiscard]] bool RebuildAutoMapTexture(
    SDL_Texture* texture,
    cg::resource::AutoMapImage& image,
    const cg::resource::MapData& map,
    const cg::resource::ResourceManager& resourceManager,
    const cg::resource::Palette& palette,
    const cg::resource::AutoMapRenderOptions& options)
{
    image = cg::resource::RenderAutoMapRgba8(map, resourceManager, palette, options);
    if (image.width <= 0 || image.height <= 0 || image.pixels.empty()) {
        std::cerr << "失败：自动地图图像为空\n";
        return false;
    }
    if (!UploadAutoMapTexture(texture, image)) {
        PrintSdlError("上传自动地图纹理失败");
        return false;
    }
    return true;
}

[[nodiscard]] bool ToggleLayerOption(SDL_Keycode key, cg::resource::AutoMapRenderOptions& options) noexcept
{
    switch (key) {
    case SDLK_1:
        options.drawTiles = !options.drawTiles;
        return true;
    case SDLK_2:
        options.drawObjects = !options.drawObjects;
        return true;
    case SDLK_3:
        options.drawCollision = !options.drawCollision;
        return true;
    default:
        return false;
    }
}

[[nodiscard]] bool RenderFrame(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    const cg::resource::AutoMapImage& image,
    const ViewState& view)
{
    if (!SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255) || !SDL_RenderClear(renderer)) {
        return false;
    }

    const SDL_FRect destination {
        view.offsetX,
        view.offsetY,
        static_cast<float>(image.width) * view.scale,
        static_cast<float>(image.height) * view.scale,
    };
    if (!SDL_RenderTexture(renderer, texture, nullptr, &destination)) {
        return false;
    }

    return SDL_RenderPresent(renderer);
}

[[nodiscard]] bool ShowAutoMapWindow(
    const cg::resource::MapData& map,
    const cg::resource::ResourceManager& resourceManager,
    const cg::resource::Palette& palette,
    cg::resource::AutoMapImage image,
    cg::resource::AutoMapRenderOptions options)
{
    if (image.width <= 0 || image.height <= 0 || image.pixels.empty()) {
        std::cerr << "失败：自动地图图像为空\n";
        return false;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        PrintSdlError("SDL 初始化失败");
        return false;
    }

    SDL_Window* window = SDL_CreateWindow(
        "cg-map-viewer",
        InitialWindowWidth,
        InitialWindowHeight,
        SDL_WINDOW_RESIZABLE);
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
        PrintSdlError("创建自动地图纹理失败");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST)
        || !SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND)
        || !UploadAutoMapTexture(texture, image)) {
        PrintSdlError("上传自动地图纹理失败");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    ViewState view = MakeInitialView(image, windowWidth, windowHeight);
    float mouseX = static_cast<float>(windowWidth) * 0.5F;
    float mouseY = static_cast<float>(windowHeight) * 0.5F;
    UpdateWindowTitle(window, view, map, options, mouseX, mouseY);

    std::cout << "自动地图尺寸：" << image.width << "x" << image.height << '\n';
    PrintLayerState(options);
    std::cout << "窗口已打开：ESC 退出，WASD/方向键平移，鼠标滚轮缩放，1/2/3 切换 tile/object/collision\n";
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
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                } else if (!event.key.repeat && ToggleLayerOption(event.key.key, options)) {
                    if (!RebuildAutoMapTexture(texture, image, map, resourceManager, palette, options)) {
                        running = false;
                        break;
                    }
                    PrintLayerState(options);
                    UpdateWindowTitle(window, view, map, options, mouseX, mouseY);
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                UpdateWindowTitle(window, view, map, options, mouseX, mouseY);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                mouseX = event.wheel.mouse_x;
                mouseY = event.wheel.mouse_y;
                ZoomAt(view, mouseX, mouseY, event.wheel.y);
                UpdateWindowTitle(window, view, map, options, mouseX, mouseY);
                break;
            default:
                break;
            }
        }

        PanByKeyboard(view);
        UpdateWindowTitle(window, view, map, options, mouseX, mouseY);
        if (!RenderFrame(renderer, texture, image, view)) {
            PrintSdlError("渲染自动地图失败");
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
    if (argc != ExpectedArgCount) {
        PrintUsage();
        return 2;
    }

    const std::filesystem::path ymlPath = PathFromUtf8(argv[1]);
    const std::filesystem::path mapRoot = PathFromUtf8(argv[2]);
    const std::filesystem::path palettePath = PathFromUtf8(argv[6]);

    std::uint32_t mapId = 0;
    std::uint16_t serverNo = 0;
    std::uint16_t floor = 0;
    if (!ParseUnsigned(std::string_view { argv[3] }, mapId)) {
        std::cerr << "失败：mapId 必须是 uint32 范围内的非负整数\n";
        return 2;
    }
    if (!ParseUnsigned(std::string_view { argv[4] }, serverNo)) {
        std::cerr << "失败：serverNo 必须是 uint16 范围内的非负整数\n";
        return 2;
    }
    if (!ParseUnsigned(std::string_view { argv[5] }, floor)) {
        std::cerr << "失败：floor 必须是 uint16 范围内的非负整数\n";
        return 2;
    }

    const cg::resource::BinSetConfigLoadResult config = cg::resource::LoadBinSetConfig(ymlPath);
    if (!config.IsOk()) {
        PrintResourceError("yml 加载失败", config.error);
        return 1;
    }

    cg::resource::ResourceManager resources;
    bool binSetsLoaded = false;
    cg::resource::ResourceError binSetError;
    {
        ScopedStreamRedirect suppressInfo(std::cout);
        ScopedStreamRedirect suppressError(std::cerr);
        binSetsLoaded = resources.LoadBinSets(
            config.binSets,
            palettePath);
        if (!binSetsLoaded) {
            binSetError = resources.LastError();
        }
    }
    if (!binSetsLoaded) {
        PrintResourceError("binSet 加载失败", binSetError);
        return 1;
    }

    const std::filesystem::path mapPath = cg::resource::BuildMapFloorPath(mapRoot, mapId, serverNo, floor);
    const cg::resource::MapLoadResult mapResult = cg::resource::LoadMapData(mapPath, mapId, serverNo, floor);
    if (!mapResult.IsOk()) {
        PrintResourceError("地图加载失败", mapResult.error);
        return 1;
    }

    const cg::resource::ResourceStats& stats = resources.Stats();
    const cg::resource::MapData& map = mapResult.map;
    std::cout << "binSet 数：" << config.binSets.size() << "，已加载：" << stats.binSetCount << '\n';
    std::cout << "palette 路径：" << PathToUtf8(palettePath) << '\n';
    std::cout << "地图路径：" << PathToUtf8(mapPath) << '\n';
    std::cout << "地图尺寸：" << map.width << "x" << map.height << '\n';
    std::cout << "tile/obj/hit 数量：" << map.tileData.size() << '/'
              << map.objData.size() << '/' << map.hitData.size() << '\n';

    const cg::resource::AutoMapRenderOptions options;
    const cg::resource::AutoMapImage image = cg::resource::RenderAutoMapRgba8(
        map,
        resources,
        resources.DefaultPalette(),
        options);
    return ShowAutoMapWindow(map, resources, resources.DefaultPalette(), image, options) ? 0 : 1;
}
