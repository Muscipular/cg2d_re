#include "MapLoader.hpp"

#include "BinaryReader.hpp"

#include <array>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cg::resource {
namespace {

// 地图 cell 数上限只作为加载保护，避免异常文件触发超大分配；不是业务地图尺寸规则。
// hook 中 AUT_DATA 示例约 159201 cells，因此这里保守预留到 200000 cells。
constexpr std::size_t kMaxMapCellCount = 200000;

[[nodiscard]] ResourceError MakeMapError(
    ResourceErrorCode code,
    const std::filesystem::path& path,
    std::string message)
{
    return ResourceError {
        code,
        path,
        0,
        std::move(message),
    };
}

[[nodiscard]] std::string MakeMapContextMessage(
    const std::filesystem::path& path,
    std::uint32_t mapId,
    std::uint16_t serverNo,
    std::uint16_t floor,
    std::int32_t width,
    std::int32_t height,
    std::string_view reason)
{
    std::ostringstream message;
    message << reason << " path=" << path.string()
            << " mapId=" << mapId
            << " serverNo=" << serverNo
            << " floor=" << floor
            << " width=" << width
            << " height=" << height;
    return message.str();
}

[[nodiscard]] std::string MakeMapContextMessage(
    const std::filesystem::path& path,
    std::uint32_t mapId,
    std::uint16_t serverNo,
    std::uint16_t floor,
    std::int32_t width,
    std::int32_t height,
    std::size_t cellCount,
    std::string_view reason)
{
    std::ostringstream message;
    message << MakeMapContextMessage(path, mapId, serverNo, floor, width, height, reason)
            << " cellCount=" << cellCount;
    return message.str();
}

[[nodiscard]] bool ReadMapLayer(BinaryReader& reader, std::vector<std::uint16_t>& layer, std::size_t cellCount)
{
    std::vector<std::uint16_t> values(cellCount);
    for (std::size_t index = 0; index < cellCount; ++index) {
        if (!reader.ReadU16(values[index])) {
            return false;
        }
    }

    layer = std::move(values);
    return true;
}

} // namespace

std::filesystem::path BuildMapFloorPath(
    const std::filesystem::path& mapRoot,
    std::uint32_t mapId,
    std::uint16_t serverNo,
    std::uint16_t floor)
{
    const std::string floorFileName = std::to_string(floor) + ".dat";

    if (mapId == 0) {
        return (mapRoot / "map" / "0" / floorFileName).lexically_normal();
    }

    return (mapRoot / "map" / std::to_string(mapId) / std::to_string(serverNo) / floorFileName).lexically_normal();
}

MapLoadResult LoadMapData(
    const std::filesystem::path& path,
    std::uint32_t mapId,
    std::uint16_t serverNo,
    std::uint16_t floor)
{
    MapLoadResult result;

    BinaryReader reader(path);
    if (!reader.IsOpen()) {
        result.error = reader.LastError();
        return result;
    }

    std::array<std::uint8_t, 4> magic {};
    if (!reader.ReadBytes(magic)) {
        result.error = MakeMapError(ResourceErrorCode::FileReadFailed, path, "地图 magic 读取失败");
        return result;
    }
    if (magic != std::array<std::uint8_t, 4> {'M', 'A', 'P', 0x01}) {
        result.error = MakeMapError(ResourceErrorCode::InvalidFormat, path, "地图 magic 不匹配");
        return result;
    }

    std::int32_t reserved0 = 0;
    std::int32_t reserved1 = 0;
    std::int32_t width = 0;
    std::int32_t height = 0;
    if (!reader.ReadI32(reserved0) || !reader.ReadI32(reserved1) || !reader.ReadI32(width) || !reader.ReadI32(height)) {
        result.error = MakeMapError(ResourceErrorCode::FileReadFailed, path, "地图 header 读取失败");
        return result;
    }
    (void)reserved0;
    (void)reserved1;

    if (width <= 0 || height <= 0) {
        result.error = MakeMapError(
            ResourceErrorCode::InvalidFormat,
            path,
            MakeMapContextMessage(path, mapId, serverNo, floor, width, height, "地图尺寸非正"));
        return result;
    }
    if (width > static_cast<std::int32_t>(std::numeric_limits<std::uint16_t>::max())
        || height > static_cast<std::int32_t>(std::numeric_limits<std::uint16_t>::max())) {
        result.error = MakeMapError(
            ResourceErrorCode::InvalidFormat,
            path,
            MakeMapContextMessage(path, mapId, serverNo, floor, width, height, "地图尺寸超出 MapData 字段范围"));
        return result;
    }

    const std::size_t widthCells = static_cast<std::size_t>(width);
    const std::size_t heightCells = static_cast<std::size_t>(height);
    if (widthCells > std::numeric_limits<std::size_t>::max() / heightCells) {
        result.error = MakeMapError(
            ResourceErrorCode::InvalidFormat,
            path,
            MakeMapContextMessage(path, mapId, serverNo, floor, width, height, "地图 cell 数乘法溢出"));
        return result;
    }

    const std::size_t cellCount = widthCells * heightCells;
    if (cellCount > kMaxMapCellCount) {
        result.error = MakeMapError(
            ResourceErrorCode::InvalidFormat,
            path,
            MakeMapContextMessage(path, mapId, serverNo, floor, width, height, cellCount, "地图 cell 数超出加载保护上限"));
        return result;
    }

    const std::uint64_t bytesNeeded = static_cast<std::uint64_t>(cellCount) * 3ULL * sizeof(std::uint16_t);
    if (bytesNeeded > reader.Size() - reader.Position()) {
        result.error = MakeMapError(
            ResourceErrorCode::FileReadFailed,
            path,
            MakeMapContextMessage(path, mapId, serverNo, floor, width, height, cellCount, "地图三层数据不足"));
        return result;
    }

    result.map.mapId = mapId;
    result.map.serverNo = serverNo;
    result.map.floor = floor;
    result.map.width = static_cast<std::uint16_t>(width);
    result.map.height = static_cast<std::uint16_t>(height);

    if (!ReadMapLayer(reader, result.map.tileData, cellCount)
        || !ReadMapLayer(reader, result.map.objData, cellCount)
        || !ReadMapLayer(reader, result.map.hitData, cellCount)) {
        result.map = MapData {};
        result.error = MakeMapError(
            ResourceErrorCode::FileReadFailed,
            path,
            MakeMapContextMessage(path, mapId, serverNo, floor, width, height, cellCount, "地图三层数据读取失败"));
        return result;
    }

    return result;
}

} // namespace cg::resource
