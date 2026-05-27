#pragma once

#include "ResourceError.hpp"
#include "ResourceTypes.hpp"

#include <filesystem>
#include <vector>

namespace cg::resource {

// BinSetConfigLoadResult 保存 cgmsv.yml 中 BinLoader 数组的最小读取结果。
struct BinSetConfigLoadResult {
    std::vector<BinSetDesc> binSets;
    ResourceError error;

    [[nodiscard]] bool IsOk() const noexcept
    {
        return error.IsOk();
    }
};

// LoadBinSetConfig 读取 cgmsv.yml 顶层 BinLoader 数组，按 yml 原顺序保留 Name。
[[nodiscard]] BinSetConfigLoadResult LoadBinSetConfig(const std::filesystem::path& ymlPath);

} // namespace cg::resource
