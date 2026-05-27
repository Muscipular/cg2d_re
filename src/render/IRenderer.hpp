#pragma once

#include "Color.hpp"
#include "RenderStats.hpp"
#include "RenderTypes.hpp"
#include "Texture.hpp"

#include <cstddef>

namespace cg::render {

struct SpriteCmd;

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

    // ResetStats 清空当前帧渲染统计。
    virtual void ResetStats() noexcept = 0;

    // GetStats 返回当前帧已累计的渲染统计。
    [[nodiscard]] virtual RenderStats GetStats() const noexcept = 0;

    // CreateTexture 创建纹理；失败时返回无效 TextureHandle。
    [[nodiscard]] virtual TextureHandle CreateTexture(const TextureDesc& desc) = 0;

    // UpdateTexture 使用内存像素更新整张纹理；pixels 为连续像素数据，pitch 为单行字节数。
    [[nodiscard]] virtual bool UpdateTexture(TextureHandle texture, const void* pixels, int pitch) = 0;

    // DestroyTexture 销毁由 CreateTexture 创建的纹理句柄。
    virtual void DestroyTexture(TextureHandle texture) noexcept = 0;

    // DrawTexture 将 source 区域绘制到 destination 区域。
    [[nodiscard]] virtual bool DrawTexture(TextureHandle texture, const RectF& source, const RectF& destination) = 0;

    // DrawSprites 批量绘制 sprite；后端可对连续相同 texture 的命令合批。
    [[nodiscard]] virtual bool DrawSprites(const SpriteCmd* sprites, std::size_t count) = 0;

protected:
    IRenderer() = default;
};

} // namespace cg::render
