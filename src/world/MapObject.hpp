#pragma once

#include "world/Transform.hpp"

#include <cstdint>

namespace cg::world {

// MapObject 表示实际地图 obj 对象，保留原始 objectIndex。
struct MapObject {
    int objectIndex = 0;
    TileCoord tile{};
    std::uint32_t imageNo = 0;
};

} // namespace cg::world
