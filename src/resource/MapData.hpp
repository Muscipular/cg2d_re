#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace cg::resource {

// MapData 保存地图三层原始格子数据，不包含加载路径和渲染后端对象。
struct MapData {
    // mapId 语义：0=固定地图，1=迷宫地图，2=玩家出租屋，3=家族地图，5=lua地图。
    std::uint32_t mapId = 0;
    std::uint16_t serverNo = 0;
    std::uint16_t floor = 0;
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::vector<std::uint16_t> tileData;
    std::vector<std::uint16_t> objData;
    std::vector<std::uint16_t> hitData;

    [[nodiscard]] std::size_t CellCount() const noexcept
    {
        return static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    }

    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return CellCount() == 0;
    }
};

} // namespace cg::resource
