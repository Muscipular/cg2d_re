#pragma once

#include "Palette.hpp"
#include "ResourceError.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace cg::resource {

// AutoMapData 保存 PUK `.aut` cell 级 RGB 数据，不包含 SDL 或 renderer 资源。
struct AutoMapData {
    std::uint16_t floor = 0;
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::vector<Rgba8> cells;

    [[nodiscard]] std::size_t CellCount() const noexcept
    {
        return static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    }

    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return CellCount() == 0 || cells.empty();
    }
};

// AutoMapDataLoadResult 保存 `.aut` 读取结果；失败时调用方可 fallback 到旧 tile/object 路径。
struct AutoMapDataLoadResult {
    AutoMapData data;
    ResourceError error;

    [[nodiscard]] bool IsOk() const noexcept
    {
        return error.IsOk();
    }
};

// BuildAutoMapDataPath 构造 PUK 自动地图路径：`<mapRoot>/bin/PUK2/cutdat/<floor>.aut`。
[[nodiscard]] std::filesystem::path BuildAutoMapDataPath(
    const std::filesystem::path& mapRoot,
    std::uint16_t floor);

// LoadAutoMapData 读取 `.aut` RGB 三字节 cell 数据，并校验大小必须等于 `width * height * 3`。
[[nodiscard]] AutoMapDataLoadResult LoadAutoMapData(
    const std::filesystem::path& path,
    std::uint16_t width,
    std::uint16_t height,
    std::uint16_t floor);

} // namespace cg::resource
