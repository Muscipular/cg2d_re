#pragma once

#include "RenderTypes.hpp"
#include "SpriteCmd.hpp"
#include "Texture.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace cg::render {

// TextureAtlas 保存一张纹理及其内部 sprite 源区域。
class TextureAtlas final {
public:
    void SetTexture(TextureHandle texture) noexcept;
    [[nodiscard]] TextureHandle GetTexture() const noexcept;

    // AddRegion 新增 key 对应源区域；key 已存在时保留旧值并返回 false。
    [[nodiscard]] bool AddRegion(std::string_view key, const RectF& source);

    // FindRegion 查找 key 对应源区域；不存在时返回 std::nullopt。
    [[nodiscard]] std::optional<RectF> FindRegion(std::string_view key) const;

    void ClearRegions() noexcept;
    [[nodiscard]] std::size_t RegionCount() const noexcept;

    // MakeSprite 由区域 key 创建绘制命令；纹理无效或 key 不存在时返回 std::nullopt。
    [[nodiscard]] std::optional<SpriteCmd> MakeSprite(
        std::string_view key,
        const RectF& destination,
        std::uint32_t layer = 0,
        std::int32_t sortY = 0,
        std::uint32_t order = 0) const;

private:
    struct StringHash {
        using is_transparent = void;

        [[nodiscard]] std::size_t operator()(std::string_view value) const noexcept;
        [[nodiscard]] std::size_t operator()(const std::string& value) const noexcept;
    };

    using RegionMap = std::unordered_map<std::string, RectF, StringHash, std::equal_to<>>;

    TextureHandle texture_ = {};
    RegionMap regions_;
};

} // namespace cg::render
