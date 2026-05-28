#include "resource/BinSetConfigLoader.hpp"
#include "resource/MapLoader.hpp"
#include "resource/ResourceManager.hpp"

#include <charconv>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <ostream>
#include <streambuf>
#include <string>
#include <string_view>
#include <system_error>

namespace {

constexpr int ExpectedArgCount = 7;

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

template <typename T>
[[nodiscard]] bool ParseUnsigned(std::string_view text, T& value)
{
    static_assert(std::numeric_limits<T>::is_integer);
    static_assert(!std::numeric_limits<T>::is_signed);

    if (text.empty()) {
        return false;
    }

    std::uint64_t parsed = 0;
    const char* const begin = text.data();
    const char* const end = begin + text.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc {} || result.ptr != end || parsed > std::numeric_limits<T>::max()) {
        return false;
    }

    value = static_cast<T>(parsed);
    return true;
}

void PrintUsage()
{
    std::cerr << "用法：cg-map-viewer <cgmsv.yml> <mapRoot> <mapId> <serverNo> <floor> <palette.cgp>\n";
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
    if (argc != ExpectedArgCount) {
        PrintUsage();
        return 2;
    }

    const std::filesystem::path ymlPath = PathFromUtf8(argv[1]);
    const std::filesystem::path mapRoot = PathFromUtf8(argv[2]);
    const std::filesystem::path palettePath = PathFromUtf8(argv[6]);

    std::uint32_t mapId = 0;
    std::uint16_t serverNo = 0;
    std::uint16_t floor = 0;
    if (!ParseUnsigned(std::string_view { argv[3] }, mapId)) {
        std::cerr << "失败：mapId 必须是 uint32 范围内的非负整数\n";
        return 2;
    }
    if (!ParseUnsigned(std::string_view { argv[4] }, serverNo)) {
        std::cerr << "失败：serverNo 必须是 uint16 范围内的非负整数\n";
        return 2;
    }
    if (!ParseUnsigned(std::string_view { argv[5] }, floor)) {
        std::cerr << "失败：floor 必须是 uint16 范围内的非负整数\n";
        return 2;
    }

    const cg::resource::BinSetConfigLoadResult config = cg::resource::LoadBinSetConfig(ymlPath);
    if (!config.IsOk()) {
        PrintResourceError("yml 加载失败", config.error);
        return 1;
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
            cg::resource::ResourceLoadOptions {.buildAutoMapColorCache = false});
        if (!binSetsLoaded) {
            binSetError = resources.LastError();
        }
    }
    if (!binSetsLoaded) {
        PrintResourceError("binSet 加载失败", binSetError);
        return 1;
    }

    const std::filesystem::path mapPath = cg::resource::BuildMapFloorPath(mapRoot, mapId, serverNo, floor);
    const cg::resource::MapLoadResult mapResult = cg::resource::LoadMapData(mapPath, mapId, serverNo, floor);
    if (!mapResult.IsOk()) {
        PrintResourceError("地图加载失败", mapResult.error);
        return 1;
    }

    const cg::resource::ResourceStats& stats = resources.Stats();
    const cg::resource::MapData& map = mapResult.map;
    std::cout << "binSet 数：" << config.binSets.size() << "，已加载：" << stats.binSetCount << '\n';
    std::cout << "地图路径：" << PathToUtf8(mapPath) << '\n';
    std::cout << "地图尺寸：" << map.width << "x" << map.height << '\n';
    std::cout << "tile/obj/hit 数量：" << map.tileData.size() << '/'
              << map.objData.size() << '/' << map.hitData.size() << '\n';
    return 0;
}
