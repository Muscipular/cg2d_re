#include "TextureCache.hpp"

#include "IRenderer.hpp"

namespace cg::render {

std::size_t TextureCache::StringHash::operator()(std::string_view value) const noexcept
{
    return std::hash<std::string_view>{}(value);
}

std::size_t TextureCache::StringHash::operator()(const std::string& value) const noexcept
{
    return (*this)(std::string_view{value});
}

TextureCache::TextureCache(IRenderer& renderer) noexcept
    : renderer_(renderer)
{
}

TextureCache::~TextureCache()
{
    Clear();
}

TextureHandle TextureCache::LoadRgba8(std::string_view key, int width, int height, const void* pixels, int pitch)
{
    if (const auto iter = textures_.find(key); iter != textures_.end()) {
        return iter->second;
    }

    const TextureDesc desc{width, height, PixelFormat::Rgba8};
    const TextureHandle texture = renderer_.CreateTexture(desc);
    if (!texture.IsValid()) {
        return {};
    }

    if (!renderer_.UpdateTexture(texture, pixels, pitch)) {
        renderer_.DestroyTexture(texture);
        return {};
    }

    try {
        const auto [iter, inserted] = textures_.emplace(std::string{key}, texture);
        if (!inserted) {
            renderer_.DestroyTexture(texture);
            return iter->second;
        }
    } catch (...) {
        renderer_.DestroyTexture(texture);
        throw;
    }

    return texture;
}

TextureHandle TextureCache::PutTexture(std::string_view key, TextureHandle texture)
{
    if (const auto iter = textures_.find(key); iter != textures_.end()) {
        return iter->second;
    }

    if (!texture.IsValid()) {
        return {};
    }

    const auto [iter, inserted] = textures_.emplace(std::string{key}, texture);
    return inserted ? texture : iter->second;
}

TextureHandle TextureCache::Find(std::string_view key) const
{
    const auto iter = textures_.find(key);
    if (iter == textures_.end()) {
        return {};
    }

    return iter->second;
}

bool TextureCache::Remove(std::string_view key)
{
    const auto iter = textures_.find(key);
    if (iter == textures_.end()) {
        return false;
    }

    renderer_.DestroyTexture(iter->second);
    textures_.erase(iter);
    return true;
}

void TextureCache::Clear()
{
    for (const TextureMap::value_type& entry : textures_) {
        renderer_.DestroyTexture(entry.second);
    }

    textures_.clear();
}

} // namespace cg::render
