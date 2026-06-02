#include "world/Transform.hpp"

namespace cg::world {

WorldPixelCoord TileToWorldPixel(TileCoord tile) noexcept
{
    return WorldPixelCoord{
        tile.x * 64,
        tile.y * 64,
    };
}

ScreenPixelCoord ProjectTileDeltaToScreen(TileCoord delta, ScreenPixelCoord origin) noexcept
{
    return ScreenPixelCoord{
        origin.x + ((delta.x + delta.y) * 32),
        origin.y + ((delta.y - delta.x) * 24),
    };
}

} // namespace cg::world
