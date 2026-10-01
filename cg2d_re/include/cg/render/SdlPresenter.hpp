#pragma once

#include <cstdint>
#include <memory>
#include <span>

namespace cg::render {

// SDL3 的 32 位最终帧提交边界，不对应原客户端函数或图形合成算法。
// 全部操作及析构仅限主线程；外部窗口必须存活至 Destroy 成功或本对象析构。
// 首次初始化 SDL VIDEO 的线程须由调用者保证为主线程，且为外部窗口所属线程。
// 持有资源时在错误线程析构会终止进程，调用者须在主线程完成销毁。
class SdlPresenter final {
public:
    SdlPresenter() noexcept;
    ~SdlPresenter();

    SdlPresenter(const SdlPresenter&) = delete;
    SdlPresenter& operator=(const SdlPresenter&) = delete;
    SdlPresenter(SdlPresenter&&) = delete;
    SdlPresenter& operator=(SdlPresenter&&) = delete;

    // 包装当前线程创建的有效 Win32 窗口；width/height 是帧尺寸，不修改原生窗口尺寸。
    // driver 必须显式传入：nullptr 交由 SDL 选择，非空字符串原样传给 SDL。
    // 已创建时返回 false 并保留当前实例；其他失败不留下部分创建的资源。
    [[nodiscard]] bool Create(
        std::uintptr_t nativeWindow, int width, int height, const char* driver) noexcept;

    // Init 的合作级别/显示模式边界；真实切换并同步，不改变逻辑帧尺寸。
    // 全屏模式选择失败沿原 SetDisplayMode 忽略返回的合同保留当前选择。
    [[nodiscard]] bool SetFullscreen(bool fullscreen) noexcept;

    // 上传完整、自上而下的 0x00RRGGBB 帧，并以 nearest 覆盖当前输出区域。
    // pitchBytes 须为正、为 4 的倍数且至少 width * 4；允许行间 padding。
    // pixels 须覆盖至最后一行的最后一个像素，不要求末行尾部 padding。
    // 调用期间源数据须有效且不被并发修改，本类不保留 pixels 的引用。
    [[nodiscard]] bool Present(std::span<const std::uint32_t> pixels, int pitchBytes) noexcept;

    // 可重复销毁；不销毁外部原生窗口，不清除 LastError。
    // 有资源时若不在主线程，返回 false 并保留资源，须回到主线程销毁。
    [[nodiscard]] bool Destroy() noexcept;

    // 对象持有的最近错误；Create/Present 成功后清空，销毁后仍可读取。
    // 指针有效至本对象析构，后续操作可改写内容；过长诊断截断至缓冲区容量。
    [[nodiscard]] const char* LastError() const noexcept;

private:
    struct Impl;

    // 仅实现文件持有 SDL 资源；诊断独立保存，避免失败清理覆盖错误。
    std::unique_ptr<Impl> impl_;
    char lastError_[1024] {};

    // 返回 false 并保存阶段及原因；诊断路径不分配内存。
    bool Fail(const char* operation, const char* detail) noexcept;
    bool FailSdl(const char* operation) noexcept;
};

// 原 628FC8：所有绘制消费者共享的完整 W32 帧槽，Flip 每次翻转。
extern std::uint32_t gFrameParity628FC8;

// Renderer::Init/Destroy 的单一 SDL 后端边界；原生窗口由平台持有。
[[nodiscard]] bool CreateRendererPresenter() noexcept;
[[nodiscard]] bool SetRendererPresenterFullscreen(bool fullscreen) noexcept;
[[nodiscard]] bool DestroyRendererPresenter() noexcept;
[[nodiscard]] const char* RendererPresenterLastError() noexcept;

// 原 44C560：将统一 CPU Back 提交到 SDL，失败置 lost，随后无条件翻转帧槽。
void Flip_44C560();

// 原 453E80：原序写入已排好行的 BGR24 BMP；失败保留部分文件，返回 0/1。
std::int32_t SaveScreenshotBitmap_453E80(
    const std::uint8_t* pixels, std::int32_t width, std::int32_t height, const char* path);

// 原 453760：SDL 后端沿原 GDI 路径截取客户区屏幕原点的 640x480，并写原序文件/消息。
void TakeScreenshot_453760();

// 原 503C80：U32 旧 tick+500 门槛；截图返回后再采样并发布，返回实际 tick。
std::uint32_t TakeScreenshotThrottled_503C80();

// 原 44C3C0：节点、缓存面、设备、接口、调色板、Back、SDL 呈现器与 renderer owner。
void RendererDestroy_44C3C0() noexcept;

} // namespace cg::render
