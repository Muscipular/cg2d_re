#pragma once

#include <cstdint>
#include <filesystem>

namespace cg::resource {

// BuildMapFloorPath 构造地图 floor.dat 路径；floor 当前不参与文件名规则。
[[nodiscard]] std::filesystem::path BuildMapFloorPath(
    const std::filesystem::path& mapRoot,
    std::uint32_t mapId,
    std::uint16_t serverNo,
    std::uint16_t floor);

} // namespace cg::resource
