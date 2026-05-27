#include "MapLoader.hpp"

#include <string>

namespace cg::resource {

std::filesystem::path BuildMapFloorPath(
    const std::filesystem::path& mapRoot,
    std::uint32_t mapId,
    std::uint16_t group,
    std::uint16_t floor)
{
    (void)floor;

    if (mapId == 0) {
        return (mapRoot / "map" / "0" / "floor.dat").lexically_normal();
    }

    return (mapRoot / "map" / std::to_string(mapId) / std::to_string(group) / "floor.dat").lexically_normal();
}

} // namespace cg::resource