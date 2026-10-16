#include "cg/render/SdlPresenter.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/core/m_log.hpp"
#include <ctime>
#include <string>
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SurfaceState.hpp"

#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"
#include "PngImageInternal.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <SDL3/SDL.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception> // 原0x00540A5C：MSVC std::terminate；原数组包装已无消费者。
#include <format>
#include <limits>
#include <new>
#include <utility>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cerrno> // 原CRT接口：0x01085FE0、0x0053C654；原参数／返回合同由直接调用承接。
#include <direct.h>

#if !defined(SDL_PLATFORM_WIN32)
#error "SdlPresenter 仅支持 Win32 窗口。"
#endif

namespace cg::render {

static_assert(sizeof(std::uintptr_t) == 8, "SdlPresenter 要求 64 位原生窗口引用。");
static_assert(sizeof(std::uint32_t) == 4, "SdlPresenter 要求每像素 4 字节。");

namespace {

SdlPresenter gRendererPresenter;

// 原截图专用槽，仅在 453760/503C80 发布，不复用渲染源掩码或其他计时器。
std::uint32_t gScreenshotRedMask62A64C = 0; // 原全局：0x0062A64C。
std::uint32_t gScreenshotGreenMask62A650 = 0; // 原全局：0x0062A650。
std::uint32_t gScreenshotBlueMask62A654 = 0; // 原全局：0x0062A654。
std::uint64_t gScreenshotTickD68A78 = 0; // 原全局：0x00D68A78。

} // namespace

std::uint32_t gFrameParity628FC8 = 0; // 原全局：0x00628FC8。

struct SdlPresenter::Impl {
    // 只记录本次 SDL_InitSubSystem 成功获得的 VIDEO 引用。
    bool videoInitialized {false};
    // 创建窗口期间的临时属性组；成功包装后立即释放。
    SDL_PropertiesID windowProperties {};
    // 外部窗口的 SDL wrapper，本类不拥有对应的原生窗口。
    SDL_Window* window {};
    SDL_Renderer* renderer {};
    SDL_Texture* texture {};
    SDL_Texture* backTarget {}; // GRT01：整帧合成的持久 GPU Back。
    bool cpuModified = true;
    bool gpuModified = false;
    bool gpuDrawing = false;
    // 最终帧的固定尺寸，与外部窗口实际输出尺寸相互独立。
    int width {};
    int height {};
    SDL_WindowID windowId {};
    bool watchingEvents {false};
    // 观察器可能在其他线程执行；只发布失效，不接触资源或上层状态。
    std::atomic<bool> resourcesLost {false};

    static bool SDLCALL ObserveRenderEvent(void* userdata, SDL_Event* event) noexcept
    {
        auto* const state = static_cast<Impl*>(userdata);
        if ((event->type == SDL_EVENT_RENDER_DEVICE_RESET
                || event->type == SDL_EVENT_RENDER_DEVICE_LOST)
            && event->render.windowID == state->windowId) {
            state->resourcesLost.store(true, std::memory_order_relaxed);
        }
        return true;
    }

    ~Impl()
    {
        if (watchingEvents) {
            SDL_RemoveEventWatch(ObserveRenderEvent, this);
        }
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
        }
        if (backTarget != nullptr) {
            SDL_DestroyTexture(backTarget);
        }
        if (renderer != nullptr) {
            ReleaseRendererGpuTextures(renderer);
            SDL_DestroyRenderer(renderer);
        }
        if (window != nullptr) {
            SDL_DestroyWindow(window);
        }
        if (windowProperties != 0) {
            SDL_DestroyProperties(windowProperties);
        }
        if (videoInitialized) {
            SDL_QuitSubSystem(SDL_INIT_VIDEO);
        }
    }
};

SdlPresenter::SdlPresenter() noexcept = default;

SdlPresenter::~SdlPresenter()
{
    // 活跃资源不能在错误线程释放，也不能通过析构静默泄漏或调度窗口事件。
    if (!Destroy()) {
        std::terminate();
    }
}

bool SdlPresenter::Create(
    std::uintptr_t nativeWindow, int width, int height, const char* driver) noexcept
{
    if (impl_ != nullptr) {
        return Fail("Create", "实例已创建，须先调用 Destroy。");
    }
    if (nativeWindow == 0) {
        return Fail("Create", "外部原生窗口引用不能为零。");
    }
    if (GetWindowThreadProcessId(
            reinterpret_cast<HWND>(nativeWindow), nullptr)
        != GetCurrentThreadId()) {
        return Fail("Create", "外部窗口无效或不属于当前线程。");
    }
    if (!SDL_IsMainThread()) {
        return Fail("Create", "必须在 SDL 主线程调用。");
    }
    if (width <= 0 || height <= 0) {
        return Fail("Create", "帧宽度和高度必须为正。");
    }
    constexpr int bytesPerPixel = sizeof(std::uint32_t);
    if (width > (std::numeric_limits<int>::max)() / bytesPerPixel) {
        return Fail("Create", "帧行字节数超出 SDL 的 int pitch 范围。");
    }
    if (driver != nullptr && driver[0] == '\0') {
        return Fail("Create", "driver 不能为空字符串；自动选择须显式传 nullptr。");
    }

    // 完整创建后才发布 impl_；任一返回路径均由临时实例按逆序清理。
    std::unique_ptr<Impl> pending(new (std::nothrow) Impl);
    if (pending == nullptr) {
        return Fail("Create", "分配呈现器状态失败。");
    }
    pending->width = width;
    pending->height = height;
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        return FailSdl("SDL_InitSubSystem(VIDEO)");
    }
    pending->videoInitialized = true;

    // 拒绝 dummy/offscreen 等后端，避免其忽略原生窗口属性而新建窗口。
    const char* videoDriver = SDL_GetCurrentVideoDriver();
    if (videoDriver == nullptr) {
        return Fail("SDL_GetCurrentVideoDriver", "VIDEO 初始化成功后仍没有活动的视频后端。");
    }
    if (std::strcmp(videoDriver, "windows") != 0) {
        return Fail("Create", "当前 SDL VIDEO 后端不支持此 Win32 外部窗口包装。");
    }

    pending->windowProperties = SDL_CreateProperties();
    if (pending->windowProperties == 0) {
        return FailSdl("SDL_CreateProperties");
    }
    if (!SDL_SetPointerProperty(
            pending->windowProperties,
            SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER,
            reinterpret_cast<void*>(nativeWindow))) {
        return FailSdl("SDL_SetPointerProperty(WIN32_HWND)");
    }
    // SDL 3.4.8 的 DX11 透明缓冲使用 DISCARD，避开本机 flip 前台约 30 FPS。
    // XRGB8888、NONE 混合及 Alpha=255 清屏仍保证最终画面不透明，见呈现 SPEC。
    if (driver != nullptr && std::strcmp(driver, "direct3d11") == 0
        && !SDL_SetBooleanProperty(pending->windowProperties,
            SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, true)) {
        return FailSdl("SDL_SetBooleanProperty(TRANSPARENT)");
    }
    pending->window = SDL_CreateWindowWithProperties(pending->windowProperties);
    if (pending->window == nullptr) {
        return FailSdl("SDL_CreateWindowWithProperties");
    }
    SDL_DestroyProperties(std::exchange(pending->windowProperties, 0));
    if ((SDL_GetWindowFlags(pending->window) & SDL_WINDOW_EXTERNAL) == 0) {
        return Fail("Create", "SDL 未将窗口标记为外部窗口。");
    }
    pending->windowId = SDL_GetWindowID(pending->window);
    if (pending->windowId == 0) {
        return FailSdl("SDL_GetWindowID");
    }

    pending->renderer = SDL_CreateRenderer(pending->window, driver);
    if (pending->renderer == nullptr) {
        return FailSdl("SDL_CreateRenderer");
    }
    pending->texture = SDL_CreateTexture(
        pending->renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height);
    if (pending->texture == nullptr) {
        return FailSdl("SDL_CreateTexture(XRGB8888, STREAMING)");
    }
    if (!SDL_SetTextureBlendMode(pending->texture, SDL_BLENDMODE_NONE)) {
        return FailSdl("SDL_SetTextureBlendMode(NONE)");
    }
    if (!SDL_SetTextureScaleMode(pending->texture, SDL_SCALEMODE_NEAREST)) {
        return FailSdl("SDL_SetTextureScaleMode(NEAREST)");
    }
    pending->backTarget = SDL_CreateTexture(
        pending->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if (pending->backTarget == nullptr
        || !SDL_SetTextureBlendMode(pending->backTarget, SDL_BLENDMODE_NONE)
        || !SDL_SetTextureScaleMode(pending->backTarget, SDL_SCALEMODE_NEAREST)) {
        return FailSdl("Create GPU Back");
    }
    if (!SDL_SetRenderDrawColor(pending->renderer, 0, 0, 0, 255)) {
        return FailSdl("SDL_SetRenderDrawColor");
    }
    if (!SDL_AddEventWatch(Impl::ObserveRenderEvent, pending.get())) {
        return FailSdl("SDL_AddEventWatch");
    }
    pending->watchingEvents = true;

    impl_ = std::move(pending);
    lastError_[0] = '\0';
    return true;
}

bool SdlPresenter::SetFullscreen(const bool fullscreen) noexcept
{
    if (!SDL_IsMainThread()) {
        return Fail("SetFullscreen", "必须在 SDL 主线程调用。");
    }
    if (impl_ == nullptr) {
        return Fail("SetFullscreen", "实例尚未创建或已销毁。");
    }
    if (fullscreen) {
        SDL_DisplayMode mode {};
        const SDL_DisplayID display = SDL_GetDisplayForWindow(impl_->window);
        if (SDL_GetClosestFullscreenDisplayMode(
                display, impl_->width, impl_->height, 0.0f, false, &mode)) {
            // 原 44B9EA 不消费 SetDisplayMode 返回；实际 fullscreen 请求仍须成功。
            static_cast<void>(SDL_SetWindowFullscreenMode(impl_->window, &mode));
        }
    }
    if (!SDL_SetWindowFullscreen(impl_->window, fullscreen)) {
        return FailSdl("SDL_SetWindowFullscreen");
    }
    if (!SDL_SyncWindow(impl_->window)) {
        return FailSdl("SDL_SyncWindow");
    }
    lastError_[0] = '\0';
    return true;
}

bool SdlPresenter::Present(std::span<const std::uint32_t> pixels, int pitchBytes) noexcept
{
    if (!SDL_IsMainThread()) {
        return Fail("Present", "必须在 SDL 主线程调用。");
    }
    if (impl_ == nullptr) {
        return Fail("Present", "实例尚未创建或已销毁。");
    }
    if (impl_->resourcesLost.load(std::memory_order_relaxed)) {
        return Fail("Present", "呈现资源已失效，须销毁后重新创建。");
    }
    constexpr int bytesPerPixel = sizeof(std::uint32_t);
    if (pitchBytes <= 0 || pitchBytes % bytesPerPixel != 0) {
        return Fail("Present", "pitchBytes 必须为正且为 4 的倍数。");
    }
    const auto pitchPixels = static_cast<std::size_t>(pitchBytes / bytesPerPixel);
    const auto rowPixels = static_cast<std::size_t>(impl_->width);
    if (pitchPixels < rowPixels) {
        return Fail("Present", "pitchBytes 小于一行有效像素的字节数。");
    }
    if (pixels.data() == nullptr || pixels.size() < rowPixels) {
        return Fail("Present", "pixels 不足以覆盖第一行有效像素。");
    }
    // 等价于 (height - 1) * pitchPixels + rowPixels <= pixels.size()，
    // 先减后除以避免乘加溢出，并允许省略最后一行的尾部 padding。
    const auto precedingRows = static_cast<std::size_t>(impl_->height - 1);
    if (precedingRows > (pixels.size() - rowPixels) / pitchPixels) {
        return Fail("Present", "pixels 未覆盖到最后一行的最后一个有效像素。");
    }

    if (impl_->cpuModified || !impl_->gpuDrawing) {
        if (!SDL_SetRenderTarget(impl_->renderer, impl_->backTarget)
            || !SDL_UpdateTexture(impl_->texture, nullptr, pixels.data(), pitchBytes)
            || !SDL_RenderTexture(impl_->renderer, impl_->texture, nullptr, nullptr)) {
            impl_->resourcesLost.store(true, std::memory_order_relaxed);
            return FailSdl("Upload CPU Back");
        }
        impl_->cpuModified = false;
        impl_->gpuModified = false;
    }
    if (!SDL_SetRenderTarget(impl_->renderer, nullptr)
        || !SDL_SetRenderDrawColor(impl_->renderer, 0, 0, 0, 255)) {
        return FailSdl("Select window target");
    }
    if (!SDL_RenderClear(impl_->renderer)) {
        impl_->resourcesLost.store(true, std::memory_order_relaxed);
        return FailSdl("SDL_RenderClear");
    }
    if (!SDL_RenderTexture(impl_->renderer, impl_->backTarget, nullptr, nullptr)) {
        impl_->resourcesLost.store(true, std::memory_order_relaxed);
        return FailSdl("SDL_RenderTexture");
    }
    if (!SDL_RenderPresent(impl_->renderer)) {
        impl_->resourcesLost.store(true, std::memory_order_relaxed);
        return FailSdl("SDL_RenderPresent");
    }
    if (impl_->resourcesLost.load(std::memory_order_relaxed)) {
        return Fail("Present", "提交期间呈现设备已重置或丢失，须重新创建。");
    }
    lastError_[0] = '\0';
    return true;
}

bool SdlPresenter::PrepareGpuFrame() noexcept
{
    if (impl_ == nullptr || impl_->resourcesLost.load(std::memory_order_relaxed)) {
        return Fail("GPU Back", "呈现资源尚未创建或已经失效。");
    }
    if (!SDL_SetRenderTarget(impl_->renderer, impl_->backTarget)) {
        return FailSdl("SDL_SetRenderTarget(Back)");
    }
    if (impl_->cpuModified) {
        if (!SDL_UpdateTexture(impl_->texture, nullptr, ReadCpuBackPixels().data(),
                static_cast<int>(GetCpuSurfacePitchBytes()))
            || !SDL_RenderTexture(impl_->renderer, impl_->texture, nullptr, nullptr)) {
            return FailSdl("Upload modified CPU Back");
        }
        impl_->cpuModified = false;
        impl_->gpuModified = false;
    }
    impl_->gpuDrawing = true;
    return true;
}

bool SdlPresenter::DrawTexture(
    TextureSurface* const surface, const std::array<float, 4>& source,
    const std::array<float, 4>& destination, const std::uint32_t color,
    const LegacyTextureBlend sourceBlend, const LegacyTextureBlend destinationBlend) noexcept
{
    if (!PrepareGpuFrame()) { return false; }
    SDL_BlendMode blend;
    if (sourceBlend == LegacyTextureBlend::One && destinationBlend == LegacyTextureBlend::Zero) {
        blend = SDL_BLENDMODE_NONE;
    } else if (sourceBlend == LegacyTextureBlend::One && destinationBlend == LegacyTextureBlend::One) {
        blend = SDL_BLENDMODE_ADD_PREMULTIPLIED;
    } else {
        blend = SDL_BLENDMODE_BLEND;
    }
    const SDL_FRect output {std::min(destination[0], destination[2]),
        std::min(destination[1], destination[3]), std::abs(destination[2] - destination[0]),
        std::abs(destination[3] - destination[1])};
    const auto red = static_cast<std::uint8_t>(color >> 16);
    const auto green = static_cast<std::uint8_t>(color >> 8);
    const auto blue = static_cast<std::uint8_t>(color);
    const auto alpha = static_cast<std::uint8_t>(color >> 24);
    bool drawn;
    if (surface == nullptr) {
        drawn = SDL_SetRenderDrawBlendMode(impl_->renderer, blend)
            && SDL_SetRenderDrawColor(impl_->renderer, red, green, blue, alpha)
            && SDL_RenderFillRect(impl_->renderer, &output);
    } else {
        SDL_Texture* const texture = GetTextureGpuTexture(surface, impl_->renderer);
        if (texture == nullptr) { return FailSdl("Upload image texture"); }
        const SDL_FRect input {std::min(source[0], source[2]), std::min(source[1], source[3]),
            std::abs(source[2] - source[0]), std::abs(source[3] - source[1])};
        const bool flipX = (source[2] < source[0]) != (destination[2] < destination[0]);
        const bool flipY = (source[3] < source[1]) != (destination[3] < destination[1]);
        const auto flip = static_cast<SDL_FlipMode>(
            (flipX ? SDL_FLIP_HORIZONTAL : 0) | (flipY ? SDL_FLIP_VERTICAL : 0));
        drawn = SDL_SetTextureColorMod(texture, red, green, blue)
            && SDL_SetTextureAlphaMod(texture, alpha)
            && SDL_SetTextureBlendMode(texture, blend)
            && SDL_RenderTextureRotated(impl_->renderer, texture, &input, &output, 0.0, nullptr, flip);
    }
    if (!drawn) { return FailSdl("Draw GPU rectangle"); }
    impl_->gpuModified = true;
    return true;
}

bool SdlPresenter::DrawTriangles(
    const std::span<const ColorVertex> vertices, const std::array<std::int32_t, 4>& clip) noexcept
{
    if (!PrepareGpuFrame()) { return false; }
    std::vector<SDL_Vertex> native;
    const std::size_t count = vertices.size() / 3 * 3;
    native.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        const auto& vertex = vertices[index];
        const auto color = vertices[index / 3 * 3].color;
        native.push_back({{vertex.x + 0.5f, vertex.y + 0.5f},
            {static_cast<std::uint8_t>(color >> 16) / 255.0f,
             static_cast<std::uint8_t>(color >> 8) / 255.0f,
             static_cast<std::uint8_t>(color) / 255.0f,
             static_cast<std::uint8_t>(color >> 24) / 255.0f}, {0, 0}});
    }
    SDL_Rect previous;
    const bool hadClip = SDL_RenderClipEnabled(impl_->renderer);
    if (!SDL_GetRenderClipRect(impl_->renderer, &previous)) { return FailSdl("Read GPU clip"); }
    const SDL_Rect rectangle {clip[0], clip[1], clip[2] - clip[0], clip[3] - clip[1]};
    const bool drawn = SDL_SetRenderClipRect(impl_->renderer, &rectangle)
        && SDL_SetRenderDrawBlendMode(impl_->renderer, SDL_BLENDMODE_BLEND)
        && SDL_RenderGeometry(impl_->renderer, nullptr, native.data(), static_cast<int>(native.size()), nullptr, 0);
    const bool restored = SDL_SetRenderClipRect(impl_->renderer, hadClip ? &previous : nullptr);
    if (!drawn || !restored) { return FailSdl("Draw GPU triangles"); }
    impl_->gpuModified = true;
    return true;
}

bool SdlPresenter::ClearFrame(const std::uint32_t color) noexcept
{
    if (impl_ == nullptr) { return Fail("Clear GPU Back", "呈现器尚未创建。"); }
    if (!SDL_SetRenderTarget(impl_->renderer, impl_->backTarget)
        || !SDL_SetRenderDrawColor(impl_->renderer, static_cast<std::uint8_t>(color >> 16),
            static_cast<std::uint8_t>(color >> 8), static_cast<std::uint8_t>(color), 255)
        || !SDL_RenderClear(impl_->renderer)) {
        return FailSdl("Clear GPU Back");
    }
    impl_->gpuDrawing = true;
    impl_->cpuModified = false;
    impl_->gpuModified = true;
    return true;
}

bool SdlPresenter::ReadFrame(std::span<std::uint32_t> pixels, const int pitchBytes) noexcept
{
    if (impl_ == nullptr) { return Fail("Read GPU Back", "呈现器尚未创建。"); }
    if (!impl_->gpuModified) { return true; }
    if (!SDL_SetRenderTarget(impl_->renderer, impl_->backTarget)) { return FailSdl("Read GPU target"); }
    std::unique_ptr<SDL_Surface, decltype(&SDL_DestroySurface)> captured {
        SDL_RenderReadPixels(impl_->renderer, nullptr), SDL_DestroySurface
    };
    if (captured == nullptr || !SDL_ConvertPixels(impl_->width, impl_->height,
            captured->format, captured->pixels, captured->pitch,
            SDL_PIXELFORMAT_XRGB8888, pixels.data(), pitchBytes)) {
        return FailSdl("Read GPU Back pixels");
    }
    impl_->gpuModified = false;
    return true;
}

void SdlPresenter::MarkCpuFrameModified() noexcept
{
    if (impl_ != nullptr) {
        impl_->cpuModified = true;
        impl_->gpuModified = false;
    }
}

bool DrawGpuTexture(TextureSurface* const surface, const std::array<float, 4>& source,
    const std::array<float, 4>& destination, const std::uint32_t color,
    const LegacyTextureBlend sourceBlend, const LegacyTextureBlend destinationBlend) noexcept
{
    const bool drawn = gRendererPresenter.DrawTexture(surface, source, destination, color, sourceBlend, destinationBlend);
    if (!drawn) { m_log_i("[render] %s\n", gRendererPresenter.LastError()); }
    return drawn;
}

bool DrawGpuTriangles(const std::span<const ColorVertex> vertices, const TextureRectangle& clip) noexcept
{
    const bool drawn = gRendererPresenter.DrawTriangles(vertices, clip);
    if (!drawn) { m_log_i("[render] %s\n", gRendererPresenter.LastError()); }
    return drawn;
}

bool ClearGpuBack(const std::uint32_t color) noexcept { return gRendererPresenter.ClearFrame(color); }

bool SynchronizeBackToCpu() noexcept
{
    SDL_Surface* const back = GetCpuBackSdlSurface();
    return back != nullptr && gRendererPresenter.ReadFrame(
        {static_cast<std::uint32_t*>(back->pixels), GetCpuSurfacePixelCount()}, back->pitch);
}

void MarkCpuBackModified() noexcept { gRendererPresenter.MarkCpuFrameModified(); }

bool SdlPresenter::IsLost() noexcept
{
    if (impl_ == nullptr) {
        return false;
    }
    if (impl_->resourcesLost.load(std::memory_order_relaxed)) {
        return true;
    }
    int width;
    int height;
    if (!SDL_GetRenderOutputSize(impl_->renderer, &width, &height)) {
        impl_->resourcesLost.store(true, std::memory_order_relaxed);
        static_cast<void>(FailSdl("SDL_GetRenderOutputSize"));
    }
    return impl_->resourcesLost.load(std::memory_order_relaxed);
}

bool SdlPresenter::Destroy() noexcept
{
    if (impl_ == nullptr) {
        return true;
    }
    if (!SDL_IsMainThread()) {
        return Fail("Destroy", "必须在 SDL 主线程释放活跃资源。");
    }
    impl_.reset();
    return true;
}

const char* SdlPresenter::LastError() const noexcept
{
    return lastError_;
}

bool SdlPresenter::Fail(const char* operation, const char* detail) noexcept
{
    // 分段复制保持 UTF-8 字符边界，长 SDL 错误可截断，但保留失败阶段。
    std::size_t used = SDL_utf8strlcpy(lastError_, operation, sizeof(lastError_));
    used += SDL_utf8strlcpy(lastError_ + used, ": ", sizeof(lastError_) - used);
    SDL_utf8strlcpy(lastError_ + used, detail, sizeof(lastError_) - used);
    return false;
}

bool SdlPresenter::FailSdl(const char* operation) noexcept
{
    const char* detail = SDL_GetError();
    return Fail(operation, detail[0] != '\0' ? detail : "SDL 未提供详细错误。");
}

bool CreateRendererPresenter() noexcept
{
    return gRendererPresenter.Create(
        platform::gLegacyWindow, GetCpuSurfaceWidth(), GetCpuSurfaceHeight(), "direct3d11");
}

bool SetRendererPresenterFullscreen(const bool fullscreen) noexcept
{
    return gRendererPresenter.SetFullscreen(fullscreen);
}

bool DestroyRendererPresenter() noexcept
{
    return gRendererPresenter.Destroy();
}

const char* RendererPresenterLastError() noexcept
{
    return gRendererPresenter.LastError();
}

// 原函数：0x0044C560。
void Flip_44C560()
{
    if (!gRendererPresenter.Present(
            ReadCpuBackPixels(), static_cast<int>(GetCpuSurfacePitchBytes()))) {
        platform::gRendererLost = 1;
    }
    gFrameParity628FC8 = gFrameParity628FC8 == 0 ? 1u : 0u;
    CleanupPngImageCache();
}

// 原函数：0x00453E80。
std::int32_t SaveScreenshotBitmap_453E80(
    const std::uint8_t* pixels, std::int32_t width, std::int32_t height, const char* path)
{
    static_assert(sizeof(BITMAPFILEHEADER) == 14);
    static_assert(offsetof(BITMAPFILEHEADER, bfSize) == 2);
    static_assert(offsetof(BITMAPFILEHEADER, bfOffBits) == 10);
    static_assert(sizeof(BITMAPINFOHEADER) == 40);
    static_assert(offsetof(BITMAPINFOHEADER, biHeight) == 8);
    static_assert(offsetof(BITMAPINFOHEADER, biBitCount) == 14);
    static_assert(offsetof(BITMAPINFOHEADER, biSizeImage) == 20);
    const std::int32_t pitch = (width * 3 + 3) & ~3;
    const std::int32_t absoluteHeight = height < 0 ? -height : height;
    BITMAPFILEHEADER fileHeader {};
    fileHeader.bfType = 0x4D42;
    fileHeader.bfSize = static_cast<DWORD>(pitch * absoluteHeight + 54);
    fileHeader.bfOffBits = 54;
    BITMAPINFOHEADER infoHeader {};
    infoHeader.biSize = 40;
    infoHeader.biWidth = width;
    infoHeader.biHeight = height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = BI_RGB;

    const HANDLE file = CreateFileA(
        path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return 0;
    }
    DWORD written;
    if (!WriteFile(file, &fileHeader, 14, &written, nullptr) || written != 14) {
        static_cast<void>(CloseHandle(file));
        return 0;
    }
    if (!WriteFile(file, &infoHeader, 40, &written, nullptr) || written != 40) {
        static_cast<void>(CloseHandle(file));
        return 0;
    }
    const DWORD pixelBytes = fileHeader.bfSize - fileHeader.bfOffBits;
    if (!WriteFile(file, pixels, pixelBytes, &written, nullptr) || written != pixelBytes) {
        static_cast<void>(CloseHandle(file));
        return 0;
    }
    static_cast<void>(CloseHandle(file));
    return 1;
}

// 原函数：0x00453760。
void TakeScreenshot_453760()
{
    POINT origin {0, 0};
    static_cast<void>(ClientToScreen(reinterpret_cast<HWND>(platform::gLegacyWindow), &origin));
    if (::_mkdir("screenshot" /* 原常量参考：0x00556738。 */) != 0
        && errno != 17) {
        return;
    }
    __time64_t time;
    time = ::_time64(nullptr);
    const std::tm* const calendar = ::_localtime64(&time);
    char path[256];
    std::int32_t sequence = 0;
    for (; sequence < 1000; ++sequence) {
        const std::int32_t month = calendar->tm_mon;
        const std::int32_t day = calendar->tm_mday;
        const std::int32_t year = calendar->tm_year;
        std::sprintf(path, "screenshot\\CG%02d%02d%02d_%03d.bmp" /* 原常量参考：0x00556714。 */, year % 100, month + 1, day, sequence);
        std::FILE* const probe = std::fopen(path, "r" /* 原常量参考：0x00554908。 */);
        if (probe == nullptr) {
            break;
        }
        static_cast<void>(std::fclose(probe));
    }
    if (sequence >= 1000) {
        world::detail::Sub_44B050(L"今天已经不能存取更多的画面了。" /* 原常量参考：0x005566F4。 */, 4, 3);
        return;
    }
    if (GetLegacyTextureState().pixelBitCount == 32) {
        const std::uint32_t red = gRendererRedMask;
        const std::uint32_t green = gRendererGreenMask;
        const std::uint32_t blue = gRendererBlueMask;
        gScreenshotRedMask62A64C = red;
        gScreenshotGreenMask62A650 = green;
        gScreenshotBlueMask62A654 = blue;
    }

    // SDL 无桌面 Primary 锁；沿原 4539BC GDI 路径，不读取正在构帧的 CPU Back。
    const HDC desktop = GetDC(nullptr);
    BITMAPINFO bitmapInfo;
    bitmapInfo.bmiHeader = {};
    bitmapInfo.bmiHeader.biSize = 40;
    bitmapInfo.bmiHeader.biWidth = 640;
    bitmapInfo.bmiHeader.biHeight = 480;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 24;
    auto* const pixels = static_cast<std::uint8_t*>(std::malloc(0x12C000));
    const HDC compatible = CreateCompatibleDC(desktop);
    std::wstring message;
    if (compatible == nullptr) {
        message = std::format(L"画面储存失败(G1)。({})" /* 原常量参考：0x00556640。 */, cg::text::FromGbkWide(path));
    } else {
        const HBITMAP bitmap = CreateCompatibleBitmap(desktop, 640, 480);
        if (bitmap == nullptr) {
            message = std::format(L"画面储存失败(G2)。({})" /* 原常量参考：0x0055661C。 */, cg::text::FromGbkWide(path));
        } else {
            const HGDIOBJ previous = SelectObject(compatible, bitmap);
            if (!BitBlt(compatible, 0, 0, 640, 480, desktop, origin.x, origin.y, SRCCOPY)) {
                message = std::format(L"画面储存失败(G3)。({})" /* 原常量参考：0x005565F8。 */, cg::text::FromGbkWide(path));
            } else {
                static_cast<void>(SelectObject(compatible, previous));
                if (GetDIBits(compatible, bitmap, 0, 480, pixels, &bitmapInfo, DIB_RGB_COLORS) == 0) {
                    message = std::format(L"画面储存失败(G4)。({})" /* 原常量参考：0x005565D4。 */, cg::text::FromGbkWide(path));
                } else {
                    static_cast<void>(DeleteObject(bitmap));
                    // 保留原成功支路的释放参数及失败残留，详见截图局部适配裁决。
                    static_cast<void>(ReleaseDC(
                        reinterpret_cast<HWND>(platform::gLegacyWindow), compatible));
                    static_cast<void>(ReleaseDC(
                        reinterpret_cast<HWND>(platform::gLegacyWindow), desktop));
                    if (SaveScreenshotBitmap_453E80(pixels, 640, 480, path) != 0) {
                        message = std::format(L"画面已储存。({})" /* 原常量参考：0x005565BC。 */, cg::text::FromGbkWide(path));
                    } else {
                        message = std::format(L"画面储存失败(7)。({})" /* 原常量参考：0x00556598。 */, cg::text::FromGbkWide(path));
                    }
                }
            }
        }
    }
    world::detail::Sub_44B050(message, 4, 3);
    std::free(pixels);
}

// 原函数：0x00503C80。
std::uint32_t TakeScreenshotThrottled_503C80()
{
    const std::uint64_t now = platform::ReadTickCount64();
    if (gScreenshotTickD68A78 + 500u > now) {
        return static_cast<std::uint32_t>(now);
    }
    TakeScreenshot_453760();
    const std::uint64_t completed = platform::ReadTickCount64();
    gScreenshotTickD68A78 = completed;
    return static_cast<std::uint32_t>(completed);
}

// 原函数：0x0044C3C0。
void RendererDestroy_44C3C0() noexcept
{
    if (!SDL_IsMainThread()) {
        return;
    }
    // 16 位专用 PALDATA 不属于当前 32 位 CPU 后端。
    ReleaseTextureNodeSurfaces_44C408();
    DestroyCpuCacheSurfaces();
    ReleaseRendererTextureDevice();
    ReleaseRendererTextureInterface();
    ReleaseRendererPaletteBinding();
    DestroyCpuBackSurface();
    if (!DestroyRendererPresenter()) {
        return; // 后端销毁失败时保留 owner，供主线程重试销毁。
    }
    ReleaseRendererResources();
}

// 原4536D0：每项检查后继续，节点数量动态重读；CPU锁定不等于lost。
int IsLost_4536D0()
{
    const std::int32_t initialCount = gTextureNodeCount;
    int lost = 0;
    if (initialCount > 0) {
        std::int32_t index = 0;
        TextureNode* node = gTextureNodes.data();
        for (;;) {
            TextureSurface* const surface = node->surface;
            if (surface != nullptr) {
                SDL_Surface* const native = GetTextureSdlSurface(surface);
                if (native == nullptr || native->pixels == nullptr) {
                    lost = 1;
                }
            }
            const std::int32_t currentCount = gTextureNodeCount;
            ++index;
            ++node;
            if (index >= currentCount) {
                break;
            }
        }
    }
    TextureSurface* const temp = cg::render::gCpuTempSurface;
    if (temp != nullptr) {
        SDL_Surface* const native = GetTextureSdlSurface(temp);
        if (native == nullptr || native->pixels == nullptr) {
            lost = 1;
        }
    }
    SDL_Surface* const back = GetCpuBackSdlSurface();
    if (back != nullptr && back->pixels == nullptr) {
        lost = 1;
    }
    if (gRendererPresenter.IsLost()) {
        return 1;
    }
    return lost;
}

// 原 44C6B0：DX 合作级别探测在 SDL 后端映射为可尝试重建的门禁。
int TestCooperativeLevel_44C6B0()
{
    return SDL_IsMainThread()
        && GetWindowThreadProcessId(
               reinterpret_cast<HWND>(platform::gLegacyWindow), nullptr)
            == GetCurrentThreadId();
}

} // namespace cg::render
