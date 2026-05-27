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

[[nodiscard]] bool HasExplicitStringTag(const YAML::Node& field)
{
    return field.Tag() == "!" || field.Tag() == "tag:yaml.org,2002:str";
}

[[nodiscard]] bool LooksLikeProjectPath(const std::string& value)
{
    return value.find('\\') != std::string::npos || value.find('/') != std::string::npos;
}

[[nodiscard]] bool IsStringField(const YAML::Node& field)
{
    return field.IsScalar() && HasExplicitStringTag(field);
}

[[nodiscard]] bool IsPathField(const YAML::Node& field)
{
    if (!field.IsScalar()) {
        return false;
    }
    if (HasExplicitStringTag(field)) {
        return true;
    }

    // yaml-cpp 将未加引号数字和未加引号路径都标为 "?"，这里只保守放行项目 yml 实际使用的路径形态。
    return field.Tag() == "?" && LooksLikeProjectPath(field.Scalar());
}

[[nodiscard]] bool ReadOptionalPathField(
    const YAML::Node& item,
    const char* yamlField,
    const std::filesystem::path& ymlPath,
    std::size_t index,
    std::filesystem::path& output,
    ResourceError& error)
{
    const YAML::Node field = item[yamlField];
    if (!field) {
        output.clear();
        return true;
    }
    if (!IsPathField(field)) {
        error = MakeConfigError(
            ymlPath,
            "BinLoader[" + std::to_string(index) + "] 字段 " + yamlField + " 必须是 string");
        return false;
    }

    const std::string value = field.as<std::string>();
    output = value.empty() ? std::filesystem::path {} : std::filesystem::path { value };
    return true;
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
            if (!name) {
                result.error = MakeConfigError(ymlPath, "BinLoader[" + std::to_string(index) + "] 缺少 Name");
                result.binSets.clear();
                return result;
            }
            if (!IsStringField(name)) {
                result.error = MakeConfigError(ymlPath, "BinLoader[" + std::to_string(index) + "] 字段 Name 必须是 string");
                result.binSets.clear();
                return result;
            }

            BinSetDesc desc;
            desc.name = name.as<std::string>();
            if (!ReadOptionalPathField(item, "Graphic", ymlPath, index, desc.graphicDataPath, result.error)
                || !ReadOptionalPathField(item, "GraphicInfo", ymlPath, index, desc.graphicInfoPath, result.error)
                || !ReadOptionalPathField(item, "Anime", ymlPath, index, desc.animeDataPath, result.error)
                || !ReadOptionalPathField(item, "AnimeInfo", ymlPath, index, desc.animeInfoPath, result.error)
                || !ReadOptionalPathField(item, "Coordinate", ymlPath, index, desc.coordinateDataPath, result.error)
                || !ReadOptionalPathField(
                    item,
                    "CoordinateInfo",
                    ymlPath,
                    index,
                    desc.coordinateInfoPath,
                    result.error)) {
                result.binSets.clear();
                return result;
            }
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
