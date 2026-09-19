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

    // 包装外部既有 Win32 窗口；width/height 是帧尺寸，不修改原生窗口尺寸。
    // driver 必须显式传入：nullptr 交由 SDL 选择，非空字符串原样传给 SDL。
    // 已创建时返回 false 并保留当前实例；其他失败不留下部分创建的资源。
    [[nodiscard]] bool Create(
        std::uintptr_t nativeWindow, int width, int height, const char* driver) noexcept;

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

} // namespace cg::render
