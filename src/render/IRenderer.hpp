#pragma once

#include "Color.hpp"
#include "RenderTypes.hpp"
#include "Texture.hpp"

namespace cg::render {

// IRenderer 定义上层可依赖的最小渲染接口，不暴露具体后端对象。
class IRenderer {
public:
    virtual ~IRenderer() = default;

    IRenderer(const IRenderer&) = delete;
    IRenderer& operator=(const IRenderer&) = delete;

    IRenderer(IRenderer&&) = delete;
    IRenderer& operator=(IRenderer&&) = delete;

    // BeginFrame 开始一帧渲染，并使用 clearColor 清屏。
    [[nodiscard]] virtual bool BeginFrame(ClearColor clearColor) = 0;

    // EndFrame 结束当前帧并提交显示。
    [[nodiscard]] virtual bool EndFrame() = 0;

    // CreateTexture 创建纹理；失败时返回无效 TextureHandle。
    [[nodiscard]] virtual TextureHandle CreateTexture(const TextureDesc& desc) = 0;

    // DestroyTexture 销毁由 CreateTexture 创建的纹理句柄。
    virtual void DestroyTexture(TextureHandle texture) noexcept = 0;

    // DrawTexture 将 source 区域绘制到 destination 区域。
    [[nodiscard]] virtual bool DrawTexture(TextureHandle texture, const RectF& source, const RectF& destination) = 0;

protected:
    IRenderer() = default;
};

} // namespace cg::render
