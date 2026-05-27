#include "TextureAtlas.hpp"

namespace cg::render {

std::size_t TextureAtlas::StringHash::operator()(std::string_view value) const noexcept
{
    return std::hash<std::string_view>{}(value);
}

std::size_t TextureAtlas::StringHash::operator()(const std::string& value) const noexcept
{
    return (*this)(std::string_view{value});
}

void TextureAtlas::SetTexture(TextureHandle texture) noexcept
{
    texture_ = texture;
}

TextureHandle TextureAtlas::GetTexture() const noexcept
{
    return texture_;
}

bool TextureAtlas::AddRegion(std::string_view key, const RectF& source)
{
    const auto result = regions_.emplace(std::string{key}, source);
    return result.second;
}

std::optional<RectF> TextureAtlas::FindRegion(std::string_view key) const
{
    const auto iter = regions_.find(key);
    if (iter == regions_.end()) {
        return std::nullopt;
    }

    return iter->second;
}

void TextureAtlas::ClearRegions() noexcept
{
    regions_.clear();
}

std::size_t TextureAtlas::RegionCount() const noexcept
{
    return regions_.size();
}

std::optional<SpriteCmd> TextureAtlas::MakeSprite(
    std::string_view key,
    const RectF& destination,
    std::uint32_t layer,
    std::int32_t sortY,
    std::uint32_t order) const
{
    if (!texture_.IsValid()) {
        return std::nullopt;
    }

    const auto iter = regions_.find(key);
    if (iter == regions_.end()) {
        return std::nullopt;
    }

    SpriteCmd cmd{};
    cmd.texture = texture_;
    cmd.source = iter->second;
    cmd.destination = destination;
    cmd.layer = layer;
    cmd.sortY = sortY;
    cmd.order = order;
    return cmd;
}

} // namespace cg::render
