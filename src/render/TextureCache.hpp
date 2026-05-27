#pragma once

#include "Texture.hpp"

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace cg::render {

class IRenderer;

// TextureCache 按字符串 key 缓存纹理句柄，并负责释放缓存内纹理。
class TextureCache final {
public:
    explicit TextureCache(IRenderer& renderer) noexcept;
    ~TextureCache();

    TextureCache(const TextureCache&) = delete;
    TextureCache& operator=(const TextureCache&) = delete;

    TextureCache(TextureCache&&) = delete;
    TextureCache& operator=(TextureCache&&) = delete;

    // LoadRgba8 创建并上传 RGBA8 纹理；key 已存在时直接返回旧句柄。
    [[nodiscard]] TextureHandle LoadRgba8(std::string_view key, int width, int height, const void* pixels, int pitch);

    // PutTexture 将外部已创建纹理交给缓存管理；key 已存在时直接返回旧句柄。
    [[nodiscard]] TextureHandle PutTexture(std::string_view key, TextureHandle texture);

    // Find 返回 key 对应纹理；不存在时返回无效句柄。
    [[nodiscard]] TextureHandle Find(std::string_view key) const;

    // Remove 移除并销毁 key 对应纹理；成功移除返回 true。
    [[nodiscard]] bool Remove(std::string_view key);

    // Clear 销毁所有缓存纹理。
    void Clear();

private:
    struct StringHash {
        using is_transparent = void;

        [[nodiscard]] std::size_t operator()(std::string_view value) const noexcept;
        [[nodiscard]] std::size_t operator()(const std::string& value) const noexcept;
    };

    using TextureMap = std::unordered_map<std::string, TextureHandle, StringHash, std::equal_to<>>;

    IRenderer& renderer_;
    TextureMap textures_;
};

} // namespace cg::render
