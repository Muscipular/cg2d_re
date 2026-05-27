#include "FontCache.hpp"

#include "IRenderer.hpp"

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace cg::render {
namespace {

struct FontDeleter {
    void operator()(TTF_Font* font) const noexcept
    {
        if (font != nullptr) {
            TTF_CloseFont(font);
        }
    }
};

struct SurfaceDeleter {
    void operator()(SDL_Surface* surface) const noexcept
    {
        if (surface != nullptr) {
            SDL_DestroySurface(surface);
        }
    }
};

using FontPtr = std::unique_ptr<TTF_Font, FontDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

[[nodiscard]] std::uint8_t ToColorByte(float value) noexcept
{
    const float clamped = std::clamp(value, 0.0F, 1.0F);
    return static_cast<std::uint8_t>(std::lround(clamped * 255.0F));
}

[[nodiscard]] SDL_Color ToSdlColor(ColorF color) noexcept
{
    return SDL_Color{
        ToColorByte(color.r),
        ToColorByte(color.g),
        ToColorByte(color.b),
        ToColorByte(color.a),
    };
}

} // namespace

struct FontCache::Impl {
    explicit Impl(IRenderer& renderer) noexcept
        : renderer(renderer)
    {
    }

    [[nodiscard]] bool LoadFont(std::string_view key, std::string_view path, float pointSize)
    {
        if (key.empty() || path.empty() || pointSize <= 0.0F) {
            return false;
        }

        const std::string fontPath{path};
        FontPtr font{TTF_OpenFont(fontPath.c_str(), pointSize)};
        if (font == nullptr) {
            return false;
        }

        try {
            fonts.insert_or_assign(std::string{key}, std::move(font));
        } catch (...) {
            return false;
        }

        return true;
    }

    [[nodiscard]] TextTexture RenderText(std::string_view fontKey, std::string_view text, ColorF color)
    {
        if (fontKey.empty() || text.empty()) {
            return {};
        }

        const auto fontIter = fonts.find(std::string{fontKey});
        if (fontIter == fonts.end()) {
            return {};
        }

        SurfacePtr textSurface{TTF_RenderText_Blended(
            fontIter->second.get(),
            text.data(),
            text.size(),
            ToSdlColor(color))};
        if (textSurface == nullptr || textSurface->w <= 0 || textSurface->h <= 0) {
            return {};
        }

        SurfacePtr rgbaSurface{SDL_ConvertSurface(textSurface.get(), SDL_PIXELFORMAT_RGBA32)};
        if (rgbaSurface == nullptr || rgbaSurface->pixels == nullptr || rgbaSurface->pitch <= 0) {
            return {};
        }

        const TextureDesc desc{
            rgbaSurface->w,
            rgbaSurface->h,
            PixelFormat::Rgba8,
            TextureScaleMode::Linear,
        };
        const TextureHandle texture = renderer.CreateTexture(desc);
        if (!texture.IsValid()) {
            return {};
        }

        if (!renderer.UpdateTexture(texture, rgbaSurface->pixels, rgbaSurface->pitch)) {
            renderer.DestroyTexture(texture);
            return {};
        }

        try {
            textTextures.push_back(texture);
        } catch (...) {
            renderer.DestroyTexture(texture);
            return {};
        }

        return TextTexture{texture, rgbaSurface->w, rgbaSurface->h};
    }

    void Clear() noexcept
    {
        for (TextureHandle texture : textTextures) {
            renderer.DestroyTexture(texture);
        }

        textTextures.clear();
        fonts.clear();
    }

    IRenderer& renderer;
    std::unordered_map<std::string, FontPtr> fonts;
    std::vector<TextureHandle> textTextures;
};

FontCache::FontCache(IRenderer& renderer) noexcept
    : impl_(std::make_unique<Impl>(renderer))
{
}

FontCache::~FontCache()
{
    Clear();
}

bool FontCache::LoadFont(std::string_view key, std::string_view path, float pointSize)
{
    return impl_->LoadFont(key, path, pointSize);
}

TextTexture FontCache::RenderText(std::string_view fontKey, std::string_view text, ColorF color)
{
    return impl_->RenderText(fontKey, text, color);
}

void FontCache::Clear() noexcept
{
    if (impl_ != nullptr) {
        impl_->Clear();
    }
}

} // namespace cg::render
