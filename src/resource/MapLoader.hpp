#pragma once

#include "MapData.hpp"
#include "ResourceError.hpp"

#include <cstdint>
#include <filesystem>

namespace cg::resource {

// MapLoadResult 保存 floor.dat 读取结果和失败原因。
struct MapLoadResult {
    MapData map;
    ResourceError error;

    [[nodiscard]] bool IsOk() const noexcept
    {
        return error.IsOk();
    }
};

// BuildMapFloorPath 构造地图 floor.dat 路径；floor 当前不参与文件名规则。
[[nodiscard]] std::filesystem::path BuildMapFloorPath(
    const std::filesystem::path& mapRoot,
    std::uint32_t mapId,
    std::uint16_t serverNo,
    std::uint16_t floor);

[[nodiscard]] MapLoadResult LoadMapData(
    const std::filesystem::path& path,
    std::uint32_t mapId,
    std::uint16_t serverNo,
    std::uint16_t floor);

} // namespace cg::resource
