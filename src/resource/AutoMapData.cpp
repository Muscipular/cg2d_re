#include "AutoMapData.hpp"

#include "BinaryReader.hpp"

#include <limits>
#include <sstream>
#include <string>
#include <utility>

namespace cg::resource {
namespace {

constexpr std::uint64_t AutoMapBytesPerCell = 3;

[[nodiscard]] ResourceError MakeAutoMapDataError(
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

[[nodiscard]] std::string MakeSizeMismatchMessage(
    std::uint16_t floor,
    std::uint16_t width,
    std::uint16_t height,
    std::uint64_t expectedSize,
    std::uint64_t actualSize)
{
    std::ostringstream message;
    message << "AUT 大小不匹配 floor=" << floor
            << " width=" << width
            << " height=" << height
            << " expected=" << expectedSize
            << " actual=" << actualSize;
    return message.str();
}

[[nodiscard]] bool ComputeExpectedSize(
    std::uint16_t width,
    std::uint16_t height,
    std::uint64_t& expectedSize) noexcept
{
    const std::uint64_t cellCount = static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height);
    if (cellCount > std::numeric_limits<std::uint64_t>::max() / AutoMapBytesPerCell) {
        return false;
    }

    expectedSize = cellCount * AutoMapBytesPerCell;
    return true;
}

} // namespace

std::filesystem::path BuildAutoMapDataPath(
    const std::filesystem::path& mapRoot,
    std::uint16_t floor)
{
    const std::string fileName = std::to_string(floor) + ".aut";
    return (mapRoot / "bin" / "PUK2" / "cutdat" / fileName).lexically_normal();
}

AutoMapDataLoadResult LoadAutoMapData(
    const std::filesystem::path& path,
    std::uint16_t width,
    std::uint16_t height,
    std::uint16_t floor)
{
    AutoMapDataLoadResult result;
    result.error.path = path;

    std::uint64_t expectedSize = 0;
    if (!ComputeExpectedSize(width, height, expectedSize)) {
        result.error = MakeAutoMapDataError(ResourceErrorCode::OutOfRange, path, "AUT 期望大小计算溢出");
        return result;
    }
    if (expectedSize > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
        result.error = MakeAutoMapDataError(ResourceErrorCode::OutOfRange, path, "AUT 期望大小超过内存寻址范围");
        return result;
    }

    BinaryReader reader(path);
    if (!reader.IsOpen()) {
        result.error = reader.LastError();
        return result;
    }
    if (reader.Size() != expectedSize) {
        result.error = MakeAutoMapDataError(
            ResourceErrorCode::InvalidFormat,
            path,
            MakeSizeMismatchMessage(floor, width, height, expectedSize, reader.Size()));
        return result;
    }

    std::vector<std::uint8_t> bytes;
    if (!reader.ReadBytes(bytes, static_cast<std::size_t>(expectedSize))) {
        result.error = reader.LastError();
        return result;
    }

    const std::size_t cellCount = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    result.data.floor = floor;
    result.data.width = width;
    result.data.height = height;
    result.data.cells.resize(cellCount);
    for (std::size_t index = 0; index < cellCount; ++index) {
        const std::size_t offset = index * static_cast<std::size_t>(AutoMapBytesPerCell);
        result.data.cells[index] = Rgba8 {
            bytes[offset],
            bytes[offset + 1U],
            bytes[offset + 2U],
            255,
        };
    }

    return result;
}

} // namespace cg::resource
