#pragma once

#include "ResourceError.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <string>
#include <vector>

namespace cg::resource {

// BinaryReader 提供显式 little-endian 的二进制读取和边界检查。
class BinaryReader final {
public:
    BinaryReader() = default;
    explicit BinaryReader(const std::filesystem::path& path);

    BinaryReader(const BinaryReader&) = delete;
    BinaryReader& operator=(const BinaryReader&) = delete;

    BinaryReader(BinaryReader&&) = default;
    BinaryReader& operator=(BinaryReader&&) = default;

    [[nodiscard]] bool Open(const std::filesystem::path& path);
    void Close() noexcept;

    [[nodiscard]] bool IsOpen() const noexcept;
    [[nodiscard]] const ResourceError& LastError() const noexcept;
    [[nodiscard]] std::uint64_t Position();
    [[nodiscard]] std::uint64_t Size() const noexcept;

    [[nodiscard]] bool Seek(std::uint64_t position);
    [[nodiscard]] bool ReadU8(std::uint8_t& value);
    [[nodiscard]] bool ReadI16(std::int16_t& value);
    [[nodiscard]] bool ReadU16(std::uint16_t& value);
    [[nodiscard]] bool ReadI32(std::int32_t& value);
    [[nodiscard]] bool ReadU32(std::uint32_t& value);
    [[nodiscard]] bool ReadBytes(std::span<std::uint8_t> bytes);
    [[nodiscard]] bool ReadBytes(std::vector<std::uint8_t>& bytes, std::size_t count);

private:
    void ClearError();
    void SetError(ResourceErrorCode code, std::string message);

    std::ifstream file_;
    std::filesystem::path path_;
    std::uint64_t size_ = 0;
    ResourceError lastError_;
};

} // namespace cg::resource
