#include "BinaryReader.hpp"

#include <array>
#include <limits>
#include <string>
#include <utility>

namespace cg::resource {
namespace {

[[nodiscard]] bool CanReadBytes(std::uint64_t position, std::uint64_t size, std::size_t count) noexcept
{
    return position <= size && count <= size - position;
}

[[nodiscard]] std::uint16_t MakeU16(const std::array<std::uint8_t, 2>& bytes) noexcept
{
    return static_cast<std::uint16_t>(bytes[0])
        | static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[1]) << 8U);
}

[[nodiscard]] std::uint32_t MakeU32(const std::array<std::uint8_t, 4>& bytes) noexcept
{
    return static_cast<std::uint32_t>(bytes[0])
        | (static_cast<std::uint32_t>(bytes[1]) << 8U)
        | (static_cast<std::uint32_t>(bytes[2]) << 16U)
        | (static_cast<std::uint32_t>(bytes[3]) << 24U);
}

[[nodiscard]] std::int16_t MakeI16(std::uint16_t value) noexcept
{
    if (value <= static_cast<std::uint16_t>(std::numeric_limits<std::int16_t>::max())) {
        return static_cast<std::int16_t>(value);
    }

    return static_cast<std::int16_t>(static_cast<std::int32_t>(value) - 0x10000);
}

[[nodiscard]] std::int32_t MakeI32(std::uint32_t value) noexcept
{
    if (value <= static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())) {
        return static_cast<std::int32_t>(value);
    }

    return static_cast<std::int32_t>(static_cast<std::int64_t>(value) - 0x100000000LL);
}

} // namespace

BinaryReader::BinaryReader(const std::filesystem::path& path)
{
    (void)Open(path);
}

bool BinaryReader::Open(const std::filesystem::path& path)
{
    Close();
    path_ = path;
    file_.open(path_, std::ios::binary);

    if (!file_.is_open()) {
        SetError(ResourceErrorCode::FileOpenFailed, "打开二进制文件失败");
        return false;
    }

    file_.seekg(0, std::ios::end);
    const std::streampos endPosition = file_.tellg();
    if (endPosition < std::streampos{0}) {
        Close();
        SetError(ResourceErrorCode::FileReadFailed, "获取文件大小失败");
        return false;
    }

    size_ = static_cast<std::uint64_t>(endPosition);
    file_.seekg(0, std::ios::beg);
    if (!file_) {
        Close();
        SetError(ResourceErrorCode::FileReadFailed, "定位文件起始位置失败");
        return false;
    }

    ClearError();
    return true;
}

void BinaryReader::Close() noexcept
{
    if (file_.is_open()) {
        file_.close();
    }

    file_.clear();
    size_ = 0;
}

bool BinaryReader::IsOpen() const noexcept
{
    return file_.is_open();
}

const ResourceError& BinaryReader::LastError() const noexcept
{
    return lastError_;
}

std::uint64_t BinaryReader::Position()
{
    if (!IsOpen()) {
        return 0;
    }

    const std::streampos position = file_.tellg();
    if (position < std::streampos{0}) {
        return size_;
    }

    return static_cast<std::uint64_t>(position);
}

std::uint64_t BinaryReader::Size() const noexcept
{
    return size_;
}

bool BinaryReader::Seek(std::uint64_t position)
{
    if (!IsOpen()) {
        SetError(ResourceErrorCode::FileOpenFailed, "文件未打开");
        return false;
    }
    if (position > size_) {
        SetError(ResourceErrorCode::OutOfRange, "定位位置超过文件大小");
        return false;
    }
    if (position > static_cast<std::uint64_t>(std::numeric_limits<std::streamoff>::max())) {
        SetError(ResourceErrorCode::OutOfRange, "定位位置超过流接口限制");
        return false;
    }

    file_.clear();
    file_.seekg(static_cast<std::streamoff>(position), std::ios::beg);
    if (!file_) {
        SetError(ResourceErrorCode::FileReadFailed, "定位文件位置失败");
        return false;
    }

    ClearError();
    return true;
}

bool BinaryReader::ReadU8(std::uint8_t& value)
{
    std::array<std::uint8_t, 1> bytes{};
    if (!ReadBytes(bytes)) {
        return false;
    }

    value = bytes[0];
    return true;
}

bool BinaryReader::ReadI16(std::int16_t& value)
{
    std::uint16_t unsignedValue = 0;
    if (!ReadU16(unsignedValue)) {
        return false;
    }

    value = MakeI16(unsignedValue);
    return true;
}

bool BinaryReader::ReadU16(std::uint16_t& value)
{
    std::array<std::uint8_t, 2> bytes{};
    if (!ReadBytes(bytes)) {
        return false;
    }

    value = MakeU16(bytes);
    return true;
}

bool BinaryReader::ReadI32(std::int32_t& value)
{
    std::uint32_t unsignedValue = 0;
    if (!ReadU32(unsignedValue)) {
        return false;
    }

    value = MakeI32(unsignedValue);
    return true;
}

bool BinaryReader::ReadU32(std::uint32_t& value)
{
    std::array<std::uint8_t, 4> bytes{};
    if (!ReadBytes(bytes)) {
        return false;
    }

    value = MakeU32(bytes);
    return true;
}

bool BinaryReader::ReadBytes(std::span<std::uint8_t> bytes)
{
    if (!IsOpen()) {
        SetError(ResourceErrorCode::FileOpenFailed, "文件未打开");
        return false;
    }
    if (bytes.empty()) {
        ClearError();
        return true;
    }
    if (bytes.size() > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max())) {
        SetError(ResourceErrorCode::OutOfRange, "读取长度超过流接口限制");
        return false;
    }

    const std::uint64_t position = Position();
    if (!CanReadBytes(position, size_, bytes.size())) {
        SetError(ResourceErrorCode::OutOfRange, "读取范围超过文件大小");
        return false;
    }

    std::string buffer(bytes.size(), '\0');
    file_.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    if (!file_ || file_.gcount() != static_cast<std::streamsize>(buffer.size())) {
        SetError(ResourceErrorCode::FileReadFailed, "读取二进制数据失败");
        return false;
    }

    for (std::size_t index = 0; index < bytes.size(); ++index) {
        bytes[index] = static_cast<std::uint8_t>(static_cast<unsigned char>(buffer[index]));
    }

    ClearError();
    return true;
}

bool BinaryReader::ReadBytes(std::vector<std::uint8_t>& bytes, std::size_t count)
{
    if (!IsOpen()) {
        SetError(ResourceErrorCode::FileOpenFailed, "文件未打开");
        return false;
    }
    if (count > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max())) {
        SetError(ResourceErrorCode::OutOfRange, "读取长度超过流接口限制");
        return false;
    }

    const std::uint64_t position = Position();
    if (!CanReadBytes(position, size_, count)) {
        SetError(ResourceErrorCode::OutOfRange, "读取范围超过文件大小");
        return false;
    }

    std::vector<std::uint8_t> buffer(count);
    if (!ReadBytes(std::span<std::uint8_t>{buffer.data(), buffer.size()})) {
        return false;
    }

    bytes = std::move(buffer);
    return true;
}

void BinaryReader::ClearError()
{
    lastError_ = ResourceError{};
    lastError_.path = path_;
}

void BinaryReader::SetError(ResourceErrorCode code, std::string message)
{
    lastError_.code = code;
    lastError_.path = path_;
    lastError_.resourceId = 0;
    lastError_.message = std::move(message);
}

} // namespace cg::resource
