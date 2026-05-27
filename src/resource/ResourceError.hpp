#pragma once

#include <filesystem>
#include <string>

namespace cg::resource {

// ResourceErrorCode 表示 resource 层可恢复上报的错误类型。
enum class ResourceErrorCode {
    None,
    FileOpenFailed,
    FileReadFailed,
    InvalidFormat,
    OutOfRange,
    MissingResource,
    DecodeFailed,
};

// ResourceError 携带文件路径、资源编号和错误文本，便于定位失败来源。
struct ResourceError {
    ResourceErrorCode code = ResourceErrorCode::None;
    std::filesystem::path path;
    int resourceId = 0;
    std::string message;

    [[nodiscard]] bool IsOk() const noexcept
    {
        return code == ResourceErrorCode::None;
    }
};

} // namespace cg::resource
