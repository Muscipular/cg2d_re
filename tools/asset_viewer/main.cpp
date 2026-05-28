#include "resource/BinSetConfigLoader.hpp"
#include "resource/ResourceManager.hpp"

#include <charconv>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <optional>
#include <ostream>
#include <streambuf>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

constexpr int RequiredArgCount = 3;

struct SearchOptions {
    std::optional<int> binSetIndex;
    std::optional<int> seqNo;
    std::optional<int> mapNo;
};

[[nodiscard]] std::string PathToUtf8(const std::filesystem::path& path)
{
    const std::u8string value = path.u8string();
    std::string text;
    text.reserve(value.size());
    for (const char8_t ch : value) {
        text.push_back(static_cast<char>(ch));
    }
    return text;
}

[[nodiscard]] std::filesystem::path PathFromUtf8(std::string_view text)
{
    std::u8string value;
    value.reserve(text.size());
    for (const char ch : text) {
        value.push_back(static_cast<char8_t>(ch));
    }
    return std::filesystem::path(value);
}

[[nodiscard]] bool ParseInt(std::string_view text, int& value)
{
    if (text.empty()) {
        return false;
    }

    int parsed = 0;
    const char* const begin = text.data();
    const char* const end = begin + text.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc {} || result.ptr != end) {
        return false;
    }

    value = parsed;
    return true;
}

void PrintUsage(std::ostream& output)
{
    output << "用法：cg-asset-viewer <cgmsv.yml> <palette.cgp> [--bin-set <index>] [--seq <seqNo>] [--map-no <mapNo>]\n";
}

void PrintResourceError(std::string_view prefix, const cg::resource::ResourceError& error)
{
    std::cerr << "失败：" << prefix;
    if (!error.message.empty()) {
        std::cerr << "：" << error.message;
    }
    if (!error.path.empty()) {
        std::cerr << "，路径：" << PathToUtf8(error.path);
    }
    if (error.resourceId != 0) {
        std::cerr << "，资源编号：" << error.resourceId;
    }
    std::cerr << '\n';
}

[[nodiscard]] bool ReadOptionValue(int argc, char* argv[], int& index, std::string_view option, int& value)
{
    if (index + 1 >= argc) {
        std::cerr << "失败：" << option << " 缺少参数值\n";
        return false;
    }

    ++index;
    if (!ParseInt(std::string_view { argv[index] }, value)) {
        std::cerr << "失败：" << option << " 必须是整数\n";
        return false;
    }
    return true;
}

[[nodiscard]] bool ParseOptions(int argc, char* argv[], SearchOptions& options)
{
    for (int index = RequiredArgCount; index < argc; ++index) {
        const std::string_view arg { argv[index] };
        int value = 0;

        if (arg == "--bin-set" || arg == "--binSet" || arg == "-b") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            if (value < 0) {
                std::cerr << "失败：binSet index 不能为负数\n";
                return false;
            }
            options.binSetIndex = value;
        } else if (arg == "--seq" || arg == "--seq-no" || arg == "-s") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            options.seqNo = value;
        } else if (arg == "--map-no" || arg == "--mapNo" || arg == "-m") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            options.mapNo = value;
        } else {
            std::cerr << "失败：未知参数 " << arg << '\n';
            return false;
        }
    }

    return true;
}

[[nodiscard]] bool HasSearchFilter(const SearchOptions& options) noexcept
{
    return options.binSetIndex.has_value() || options.seqNo.has_value() || options.mapNo.has_value();
}

[[nodiscard]] bool Matches(const cg::resource::GraphicRef& ref, const SearchOptions& options) noexcept
{
    if (options.binSetIndex.has_value() && ref.binSetIndex != *options.binSetIndex) {
        return false;
    }
    if (options.seqNo.has_value() && ref.seqNo != *options.seqNo) {
        return false;
    }
    if (options.mapNo.has_value() && ref.mapNo != *options.mapNo) {
        return false;
    }
    return true;
}

void PrintSearchOptions(const SearchOptions& options)
{
    std::cout << "搜索条件：";
    bool printed = false;
    if (options.binSetIndex.has_value()) {
        std::cout << "binSet=" << *options.binSetIndex;
        printed = true;
    }
    if (options.seqNo.has_value()) {
        std::cout << (printed ? "，" : "") << "seqNo=" << *options.seqNo;
        printed = true;
    }
    if (options.mapNo.has_value()) {
        std::cout << (printed ? "，" : "") << "mapNo=" << *options.mapNo;
    }
    std::cout << '\n';
}

void PrintGraphic(const cg::resource::GraphicRef& ref, const std::vector<cg::resource::BinSetDesc>& binSets)
{
    std::cout << "binSet=" << ref.binSetIndex;
    if (ref.binSetIndex >= 0 && static_cast<std::size_t>(ref.binSetIndex) < binSets.size()) {
        std::cout << '(' << binSets[static_cast<std::size_t>(ref.binSetIndex)].name << ')';
    }
    std::cout << "，seqNo=" << ref.seqNo
              << "，mapNo=" << ref.mapNo
              << "，尺寸=" << ref.info.w << 'x' << ref.info.h
              << "，数据偏移=" << ref.info.offset
              << "，数据长度=" << ref.info.length
              << "，坐标偏移=(" << ref.info.x << ',' << ref.info.y << ')'
              << "，dataPath=" << PathToUtf8(ref.dataPath) << '\n';
}

void PrintStats(
    const cg::resource::ResourceManager& resources,
    const std::vector<cg::resource::BinSetDesc>& binSets,
    const std::filesystem::path& ymlPath,
    const std::filesystem::path& palettePath)
{
    const cg::resource::ResourceStats& stats = resources.Stats();
    std::cout << "配置路径：" << PathToUtf8(ymlPath) << '\n';
    std::cout << "palette 路径：" << PathToUtf8(palettePath) << '\n';
    std::cout << "binSet 数：" << binSets.size() << "，已加载：" << stats.binSetCount << '\n';
    std::cout << "Graphic 数：" << stats.graphicCount << "，Anime 数：" << stats.animeCount << '\n';
    std::cout << "Graphic 覆盖：" << stats.graphicOverrideCount
              << "，Anime 覆盖：" << stats.animeOverrideCount
              << "，警告：" << stats.warningCount
              << "，错误：" << stats.errorCount << '\n';
    for (std::size_t index = 0; index < binSets.size(); ++index) {
        std::cout << "binSet[" << index << "] " << binSets[index].name
                  << "，Graphic=" << resources.Graphics(static_cast<int>(index)).size()
                  << "，dataPath=" << PathToUtf8(binSets[index].graphicDataPath) << '\n';
    }
}

[[nodiscard]] std::size_t PrintSearchResults(
    const cg::resource::ResourceManager& resources,
    const std::vector<cg::resource::BinSetDesc>& binSets,
    const SearchOptions& options)
{
    std::size_t matchCount = 0;
    const int firstBinSet = options.binSetIndex.value_or(0);
    const int endBinSet = options.binSetIndex.value_or(static_cast<int>(binSets.size()) - 1);

    for (int binSetIndex = firstBinSet; binSetIndex <= endBinSet; ++binSetIndex) {
        for (const cg::resource::GraphicRef& ref : resources.Graphics(binSetIndex)) {
            if (!Matches(ref, options)) {
                continue;
            }
            PrintGraphic(ref, binSets);
            ++matchCount;
        }
    }

    return matchCount;
}

class NullStreamBuffer final : public std::streambuf {
protected:
    int overflow(int ch) override
    {
        return ch;
    }
};

class ScopedStreamRedirect final {
public:
    explicit ScopedStreamRedirect(std::ostream& stream)
        : stream_(stream)
        , oldBuffer_(stream.rdbuf(&sink_))
    {
    }

    ScopedStreamRedirect(const ScopedStreamRedirect&) = delete;
    ScopedStreamRedirect& operator=(const ScopedStreamRedirect&) = delete;

    ~ScopedStreamRedirect()
    {
        stream_.rdbuf(oldBuffer_);
    }

private:
    std::ostream& stream_;
    std::streambuf* oldBuffer_ = nullptr;
    NullStreamBuffer sink_;
};

} // namespace

int main(int argc, char* argv[])
{
    if (argc == 2 && std::string_view { argv[1] } == "--help") {
        PrintUsage(std::cout);
        return 0;
    }
    if (argc < RequiredArgCount) {
        PrintUsage(std::cerr);
        return 2;
    }

    SearchOptions options;
    if (!ParseOptions(argc, argv, options)) {
        PrintUsage(std::cerr);
        return 2;
    }

    const std::filesystem::path ymlPath = PathFromUtf8(argv[1]);
    const std::filesystem::path palettePath = PathFromUtf8(argv[2]);
    const cg::resource::BinSetConfigLoadResult config = cg::resource::LoadBinSetConfig(ymlPath);
    if (!config.IsOk()) {
        PrintResourceError("yml 加载失败", config.error);
        return 1;
    }

    if (options.binSetIndex.has_value() && static_cast<std::size_t>(*options.binSetIndex) >= config.binSets.size()) {
        std::cerr << "失败：binSet index 超出范围，最大 index=" << (config.binSets.empty() ? 0 : config.binSets.size() - 1) << '\n';
        return 2;
    }

    cg::resource::ResourceManager resources;
    bool binSetsLoaded = false;
    cg::resource::ResourceError binSetError;
    {
        ScopedStreamRedirect suppressInfo(std::cout);
        ScopedStreamRedirect suppressError(std::cerr);
        binSetsLoaded = resources.LoadBinSets(
            config.binSets,
            palettePath,
            cg::resource::ResourceLoadOptions { .buildAutoMapColorCache = false });
        if (!binSetsLoaded) {
            binSetError = resources.LastError();
        }
    }
    if (!binSetsLoaded) {
        PrintResourceError("binSet 加载失败", binSetError);
        return 1;
    }

    PrintStats(resources, config.binSets, ymlPath, palettePath);
    if (!HasSearchFilter(options)) {
        return 0;
    }

    PrintSearchOptions(options);
    const std::size_t matchCount = PrintSearchResults(resources, config.binSets, options);
    std::cout << "命中数量：" << matchCount << '\n';
    return 0;
}
