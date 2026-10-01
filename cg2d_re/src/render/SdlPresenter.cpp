#include "cg/render/SdlPresenter.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/SurfaceState.hpp"

#include "GraphicDxCacheState.hpp"
#include "LegacyTextureDrawing.hpp"

#include <SDL3/SDL.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <cstddef>
#include <cstring>
#include <exception>
#include <limits>
#include <new>
#include <utility>

#if !defined(SDL_PLATFORM_WIN32)
#error "SdlPresenter 仅支持 Win32 窗口。"
#endif

namespace cg::render {

static_assert(sizeof(std::uintptr_t) == 8, "SdlPresenter 要求 64 位原生窗口引用。");
static_assert(sizeof(std::uint32_t) == 4, "SdlPresenter 要求每像素 4 字节。");
static_assert(kCpuSurfacePitchBytes <= static_cast<std::size_t>((std::numeric_limits<int>::max)()));

namespace {

SdlPresenter gRendererPresenter;

} // namespace

std::uint32_t gFrameParity628FC8 = 0;

struct SdlPresenter::Impl {
    // 只记录本次 SDL_InitSubSystem 成功获得的 VIDEO 引用。
    bool videoInitialized {false};
    // 创建窗口期间的临时属性组；成功包装后立即释放。
    SDL_PropertiesID windowProperties {};
    // 外部窗口的 SDL wrapper，本类不拥有对应的原生窗口。
    SDL_Window* window {};
    SDL_Renderer* renderer {};
    SDL_Texture* texture {};
    // 最终帧的固定尺寸，与外部窗口实际输出尺寸相互独立。
    int width {};
    int height {};

    ~Impl()
    {
        if (texture != nullptr) {
            SDL_DestroyTexture(texture);
        }
        if (renderer != nullptr) {
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
    pending->window = SDL_CreateWindowWithProperties(pending->windowProperties);
    if (pending->window == nullptr) {
        return FailSdl("SDL_CreateWindowWithProperties");
    }
    SDL_DestroyProperties(std::exchange(pending->windowProperties, 0));
    if ((SDL_GetWindowFlags(pending->window) & SDL_WINDOW_EXTERNAL) == 0) {
        return Fail("Create", "SDL 未将窗口标记为外部窗口。");
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
    if (!SDL_SetRenderDrawColor(pending->renderer, 0, 0, 0, 255)) {
        return FailSdl("SDL_SetRenderDrawColor");
    }

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

    if (!SDL_UpdateTexture(impl_->texture, nullptr, pixels.data(), pitchBytes)) {
        return FailSdl("SDL_UpdateTexture");
    }
    if (!SDL_RenderClear(impl_->renderer)) {
        return FailSdl("SDL_RenderClear");
    }
    if (!SDL_RenderTexture(impl_->renderer, impl_->texture, nullptr, nullptr)) {
        return FailSdl("SDL_RenderTexture");
    }
    if (!SDL_RenderPresent(impl_->renderer)) {
        return FailSdl("SDL_RenderPresent");
    }
    lastError_[0] = '\0';
    return true;
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
        platform::gLegacyWindow, kCpuSurfaceWidth, kCpuSurfaceHeight, nullptr);
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

void Flip_44C560()
{
    if (!gRendererPresenter.Present(
            ReadCpuBackPixels(), static_cast<int>(kCpuSurfacePitchBytes))) {
        platform::gRendererLost = 1;
    }
    gFrameParity628FC8 = gFrameParity628FC8 == 0 ? 1u : 0u;
}

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

// 原 44C6B0：DX 合作级别探测在 SDL 后端映射为可尝试重建的门禁。
int TestCooperativeLevel_44C6B0()
{
    return SDL_IsMainThread()
        && GetWindowThreadProcessId(
               reinterpret_cast<HWND>(platform::gLegacyWindow), nullptr)
            == GetCurrentThreadId();
}

} // namespace cg::render
