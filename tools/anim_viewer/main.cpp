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

namespace {

constexpr int RequiredArgCount = 3;

struct ViewerOptions {
    std::optional<int> animeNo;
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
    output << "用法：cg-anim-viewer <cgmsv.yml> <palette.cgp> --anime-no <animeNo>\n";
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

[[nodiscard]] bool ParseOptions(int argc, char* argv[], ViewerOptions& options)
{
    for (int index = RequiredArgCount; index < argc; ++index) {
        const std::string_view arg { argv[index] };
        int value = 0;

        if (arg == "--anime-no" || arg == "--animeNo" || arg == "-a") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            if (value < 0) {
                std::cerr << "失败：animeNo 不能为负数\n";
                return false;
            }
            options.animeNo = value;
        } else {
            std::cerr << "失败：未知参数 " << arg << '\n';
            return false;
        }
    }

    if (!options.animeNo.has_value()) {
        std::cerr << "失败：缺少 --anime-no <animeNo>\n";
        return false;
    }
    return true;
}

void PrintAnimation(
    const cg::resource::AnimeRef& ref,
    const cg::resource::AnimationClip& clip)
{
    std::cout << "Anime：animeNo=" << ref.animeNo
              << "，sourceAnimeNo=" << ref.sourceAnimeNo
              << "，binSetIndex=" << ref.binSetIndex << '\n';
    std::cout << "action 数：" << clip.actions.size()
              << "，总帧数：" << clip.frames.size() << '\n';

    for (std::size_t index = 0; index < clip.actions.size(); ++index) {
        const cg::resource::AnimeActionHeader& header = clip.actions[index].header;
        std::cout << "action[" << index << "] dir=" << static_cast<int>(header.dir)
                  << "，action=" << static_cast<int>(header.action)
                  << "，duration=" << header.duration
                  << "，frameCount=" << header.frameCount
                  << "，pal=" << header.pal
                  << "，revert=" << header.revert << '\n';
    }
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

    ViewerOptions options;
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

    const cg::resource::ResourceStats& stats = resources.Stats();
    std::cout << "配置路径：" << PathToUtf8(ymlPath) << '\n';
    std::cout << "palette 路径：" << PathToUtf8(palettePath) << '\n';
    std::cout << "binSet 数：" << config.binSets.size()
              << "，已加载：" << stats.binSetCount
              << "，Anime 数：" << stats.animeCount << '\n';

    std::optional<cg::resource::AnimationClip> clip;
    cg::resource::ResourceError animationError;
    {
        ScopedStreamRedirect suppressInfo(std::cout);
        ScopedStreamRedirect suppressError(std::cerr);
        clip = resources.LoadAnimation(*options.animeNo);
        if (!clip.has_value()) {
            animationError = resources.LastError();
        }
    }
    if (!clip.has_value()) {
        PrintResourceError("Anime 加载失败", animationError);
        return 1;
    }

    const std::optional<cg::resource::AnimeRef> ref = resources.FindAnime(*options.animeNo);
    if (!ref.has_value()) {
        std::cerr << "失败：Anime 已加载但无法读取索引，animeNo=" << *options.animeNo << '\n';
        return 1;
    }

    PrintAnimation(*ref, *clip);
    return 0;
}
