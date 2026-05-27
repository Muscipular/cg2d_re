#include "BinSetConfigLoader.hpp"

#include <yaml-cpp/yaml.h>

#include <cstddef>
#include <string>
#include <utility>

namespace cg::resource {
namespace {

[[nodiscard]] ResourceError MakeConfigError(const std::filesystem::path& path, std::string message)
{
    return ResourceError {
        ResourceErrorCode::InvalidFormat,
        path,
        0,
        std::move(message),
    };
}

} // namespace

BinSetConfigLoadResult LoadBinSetConfig(const std::filesystem::path& ymlPath)
{
    BinSetConfigLoadResult result;

    try {
        const YAML::Node root = YAML::LoadFile(ymlPath.string());
        const YAML::Node binLoader = root["BinLoader"];
        if (!binLoader) {
            result.error = MakeConfigError(ymlPath, "缺少顶层 BinLoader 节点");
            return result;
        }
        if (!binLoader.IsSequence()) {
            result.error = MakeConfigError(ymlPath, "顶层 BinLoader 必须是 sequence");
            return result;
        }

        result.binSets.reserve(binLoader.size());
        for (std::size_t index = 0; index < binLoader.size(); ++index) {
            const YAML::Node item = binLoader[index];
            if (!item.IsMap()) {
                result.error = MakeConfigError(ymlPath, "BinLoader[" + std::to_string(index) + "] 元素必须是 map");
                result.binSets.clear();
                return result;
            }

            const YAML::Node name = item["Name"];
            if (!name || !name.IsScalar()) {
                result.error = MakeConfigError(ymlPath, "BinLoader[" + std::to_string(index) + "] 缺少 string 类型 Name");
                result.binSets.clear();
                return result;
            }

            BinSetDesc desc;
            desc.name = name.as<std::string>();
            result.binSets.push_back(std::move(desc));
        }
    } catch (const YAML::BadFile&) {
        result.error = ResourceError {
            ResourceErrorCode::FileOpenFailed,
            ymlPath,
            0,
            "无法打开 yml 配置文件",
        };
    } catch (const YAML::Exception& ex) {
        result.error = MakeConfigError(ymlPath, ex.what());
        result.binSets.clear();
    }

    return result;
}

} // namespace cg::resource
