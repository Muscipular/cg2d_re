#pragma once

#include "Color.hpp"
#include "Texture.hpp"

#include <memory>
#include <string_view>

namespace cg::render {

class IRenderer;

// TextTexture 保存由字体栅格化生成的纹理和像素尺寸。
struct TextTexture {
    TextureHandle texture = {};
    int width = 0;
    int height = 0;
};

// FontCache 管理字体对象和由文本生成的纹理，不向公开头暴露 SDL 类型。
class FontCache final {
public:
    explicit FontCache(IRenderer& renderer) noexcept;
    ~FontCache();

    FontCache(const FontCache&) = delete;
    FontCache& operator=(const FontCache&) = delete;

    FontCache(FontCache&&) = delete;
    FontCache& operator=(FontCache&&) = delete;

    [[nodiscard]] bool LoadFont(std::string_view key, std::string_view path, float pointSize);
    [[nodiscard]] TextTexture RenderText(std::string_view fontKey, std::string_view text, ColorF color);
    void Clear() noexcept;

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;
};

} // namespace cg::render
