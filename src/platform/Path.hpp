#pragma once

#include <filesystem>
#include <string_view>

namespace cg::platform {

// DefaultPathOrg/DefaultPathApp 表示 SDL 偏好目录使用的默认组织名和应用名。
inline constexpr std::string_view DefaultPathOrg = "cgmsv";
inline constexpr std::string_view DefaultPathApp = "cg-client";

// BasePath 获取应用资源基准目录；失败时返回空路径。
[[nodiscard]] std::filesystem::path BasePath();

// PrefPath 获取应用可写偏好目录；失败时返回空路径。
[[nodiscard]] std::filesystem::path PrefPath(
    std::string_view org = DefaultPathOrg,
    std::string_view app = DefaultPathApp);

// AssetsPath 获取资源目录，位于 BasePath()/assets；失败时返回空路径。
[[nodiscard]] std::filesystem::path AssetsPath();

// ConfigPath 获取配置目录，位于 PrefPath()/config；失败时返回空路径。
[[nodiscard]] std::filesystem::path ConfigPath();

// SavePath 获取存档目录，位于 PrefPath()/save；失败时返回空路径。
[[nodiscard]] std::filesystem::path SavePath();

} // namespace cg::platform
