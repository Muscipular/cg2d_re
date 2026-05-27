#pragma once

#include "IRenderer.hpp"

#include <cstdint>
#include <memory>

namespace cg::platform {

class Window;

} // namespace cg::platform

namespace cg::render {

// SdlRendererBackend 使用 SDL 后端实现 IRenderer，不向上层暴露后端对象。
class SdlRendererBackend final : public IRenderer {
public:
    using WindowId = std::uint32_t;

    SdlRendererBackend();
    explicit SdlRendererBackend(WindowId windowId);
    explicit SdlRendererBackend(const platform::Window& window);
    ~SdlRendererBackend() override;

    SdlRendererBackend(const SdlRendererBackend&) = delete;
    SdlRendererBackend& operator=(const SdlRendererBackend&) = delete;

    SdlRendererBackend(SdlRendererBackend&&) = delete;
    SdlRendererBackend& operator=(SdlRendererBackend&&) = delete;

    // Initialize 通过 SDL 窗口 ID 创建渲染后端；失败时返回 false。
    [[nodiscard]] bool Initialize(WindowId windowId) noexcept;

    // Initialize 通过平台窗口创建渲染后端；失败时返回 false。
    [[nodiscard]] bool Initialize(const platform::Window& window) noexcept;

    [[nodiscard]] bool BeginFrame(ClearColor clearColor) override;
    [[nodiscard]] bool EndFrame() override;
    [[nodiscard]] TextureHandle CreateTexture(const TextureDesc& desc) override;
    void DestroyTexture(TextureHandle texture) noexcept override;
    [[nodiscard]] bool DrawTexture(TextureHandle texture, const RectF& source, const RectF& destination) override;

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace cg::render
