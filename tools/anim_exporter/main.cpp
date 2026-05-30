#include "resource/AnimationLoader.hpp"
#include "resource/BinSetConfigLoader.hpp"
#include "resource/ResourceManager.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <limits>
#include <optional>
#include <ostream>
#include <sstream>
#include <span>
#include <streambuf>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace {

constexpr int RequiredArgCount = 3;
constexpr int MaxAtlasWidth = 1024;
constexpr int Rgba8BytesPerPixel = 4;
constexpr std::size_t MaxWarningDetails = 20;

struct ExportOptions {
    std::optional<int> sourceAnimeNo;
    std::optional<int> palOverride;
    std::uint32_t bgColor = 0;
    std::filesystem::path outputDir;
    std::string mode;
    bool hasOutputDir = false;
};

struct ActionPaletteState {
    int palIndex = 0;
    std::optional<int> hiddenPaletteMapNo;
    std::optional<cg::resource::Palette> hiddenPalette;
};

struct ExportFrame {
    cg::resource::AnimeFrame frame;
    cg::resource::GraphicRef graphic;
    cg::resource::DecodedImage image;
};

struct DecodedActionFrames {
    std::vector<ExportFrame> frames;
    std::size_t expectedFrameCount = 0;
};

struct AtlasPlacement {
    int x = 0;
    int y = 0;
    const ExportFrame* frame = nullptr;
};

class WarningCollector final {
public:
    void Add(std::string message)
    {
        warnings_.push_back(std::move(message));
    }

    [[nodiscard]] std::size_t Count() const noexcept
    {
        return warnings_.size();
    }

    void PrintSummary(std::ostream& output) const
    {
        output << "warning 摘要：总数=" << Count() << '\n';
        const std::size_t detailCount = std::min(warnings_.size(), MaxWarningDetails);
        for (std::size_t index = 0; index < detailCount; ++index) {
            output << "warning[" << index << "]：" << warnings_[index] << '\n';
        }
        if (warnings_.size() > detailCount) {
            output << "warning 其余 " << (warnings_.size() - detailCount) << " 条已省略\n";
        }
    }

private:
    std::vector<std::string> warnings_;
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

[[nodiscard]] std::filesystem::path BuildHiddenPaletteTablePath(const std::filesystem::path& ymlPath)
{
    return ymlPath.parent_path() / "data" / "CGLoadSpr.dbf";
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

[[nodiscard]] bool ParseBgColor(std::string_view text, std::uint32_t& value)
{
    if (!(text.starts_with("0x") || text.starts_with("0X"))) {
        return false;
    }

    text.remove_prefix(2);
    if (text.size() != 8) {
        return false;
    }

    std::uint32_t parsed = 0;
    const char* const begin = text.data();
    const char* const end = begin + text.size();
    const auto result = std::from_chars(begin, end, parsed, 16);
    if (result.ec != std::errc {} || result.ptr != end) {
        return false;
    }

    value = parsed;
    return true;
}

void PrintUsage(std::ostream& output)
{
    output << "用法：cg-anim-exporter <cgmsv.yml> <palette.cgp> --mode sprite_atlas --anime-no <animeNo> --o <dir> [--pal=<palIndex>] [--bgColor 0xRRGGBBAA]\n";
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

void PrintSdlError(std::string_view prefix)
{
    std::cerr << "失败：" << prefix << "：" << SDL_GetError() << '\n';
}

[[nodiscard]] bool ReadOptionText(int argc, char* argv[], int& index, std::string_view option, std::string_view& value)
{
    if (index + 1 >= argc) {
        std::cerr << "失败：" << option << " 缺少参数值\n";
        return false;
    }

    ++index;
    value = std::string_view { argv[index] };
    if (value.empty()) {
        std::cerr << "失败：" << option << " 参数值不能为空\n";
        return false;
    }
    return true;
}

[[nodiscard]] bool ReadOptionInt(int argc, char* argv[], int& index, std::string_view option, int& value)
{
    std::string_view text;
    if (!ReadOptionText(argc, argv, index, option, text)) {
        return false;
    }
    if (!ParseInt(text, value)) {
        std::cerr << "失败：" << option << " 必须是整数\n";
        return false;
    }
    return true;
}

[[nodiscard]] bool ReadInlineInt(std::string_view text, std::string_view option, int& value)
{
    if (!ParseInt(text, value)) {
        std::cerr << "失败：" << option << " 必须是整数\n";
        return false;
    }
    return true;
}

[[nodiscard]] bool ReadOptionBgColor(int argc, char* argv[], int& index, std::string_view option, std::uint32_t& value)
{
    std::string_view text;
    if (!ReadOptionText(argc, argv, index, option, text)) {
        return false;
    }
    if (!ParseBgColor(text, value)) {
        std::cerr << "失败：" << option << " 必须是 0xRRGGBBAA 格式\n";
        return false;
    }
    return true;
}

[[nodiscard]] bool ReadInlineBgColor(std::string_view text, std::string_view option, std::uint32_t& value)
{
    if (!ParseBgColor(text, value)) {
        std::cerr << "失败：" << option << " 必须是 0xRRGGBBAA 格式\n";
        return false;
    }
    return true;
}

[[nodiscard]] bool ParseOptions(int argc, char* argv[], ExportOptions& options)
{
    for (int index = RequiredArgCount; index < argc; ++index) {
        const std::string_view arg { argv[index] };
        int value = 0;

        if (arg == "--mode") {
            std::string_view mode;
            if (!ReadOptionText(argc, argv, index, arg, mode)) {
                return false;
            }
            options.mode = std::string { mode };
        } else if (arg.starts_with("--mode=")) {
            options.mode = std::string { arg.substr(std::string_view { "--mode=" }.size()) };
            if (options.mode.empty()) {
                std::cerr << "失败：--mode 参数值不能为空\n";
                return false;
            }
        } else if (arg == "--anime-no" || arg == "--animeNo" || arg == "-a") {
            if (!ReadOptionInt(argc, argv, index, arg, value)) {
                return false;
            }
            options.sourceAnimeNo = value;
        } else if (arg.starts_with("--anime-no=")) {
            if (!ReadInlineInt(arg.substr(std::string_view { "--anime-no=" }.size()), "--anime-no", value)) {
                return false;
            }
            options.sourceAnimeNo = value;
        } else if (arg.starts_with("--animeNo=")) {
            if (!ReadInlineInt(arg.substr(std::string_view { "--animeNo=" }.size()), "--animeNo", value)) {
                return false;
            }
            options.sourceAnimeNo = value;
        } else if (arg == "--o" || arg == "-o") {
            std::string_view outputDir;
            if (!ReadOptionText(argc, argv, index, arg, outputDir)) {
                return false;
            }
            options.outputDir = PathFromUtf8(outputDir);
            options.hasOutputDir = true;
        } else if (arg.starts_with("--o=")) {
            const std::string_view outputDir = arg.substr(std::string_view { "--o=" }.size());
            if (outputDir.empty()) {
                std::cerr << "失败：--o 参数值不能为空\n";
                return false;
            }
            options.outputDir = PathFromUtf8(outputDir);
            options.hasOutputDir = true;
        } else if (arg == "--pal") {
            if (!ReadOptionInt(argc, argv, index, arg, value)) {
                return false;
            }
            options.palOverride = value;
        } else if (arg.starts_with("--pal=")) {
            if (!ReadInlineInt(arg.substr(std::string_view { "--pal=" }.size()), "--pal", value)) {
                return false;
            }
            options.palOverride = value;
        } else if (arg == "--bgColor") {
            if (!ReadOptionBgColor(argc, argv, index, arg, options.bgColor)) {
                return false;
            }
        } else if (arg.starts_with("--bgColor=")) {
            if (!ReadInlineBgColor(arg.substr(std::string_view { "--bgColor=" }.size()), "--bgColor", options.bgColor)) {
                return false;
            }
        } else {
            std::cerr << "失败：未知参数 " << arg << '\n';
            return false;
        }
    }

    if (options.mode != "sprite_atlas") {
        std::cerr << "失败：--mode 目前仅支持 sprite_atlas\n";
        return false;
    }
    if (!options.sourceAnimeNo.has_value()) {
        std::cerr << "失败：缺少 --anime-no <animeNo>\n";
        return false;
    }
    if (*options.sourceAnimeNo < 0) {
        std::cerr << "失败：animeNo 不能为负数\n";
        return false;
    }
    if (!options.hasOutputDir) {
        std::cerr << "失败：缺少 --o <dir>\n";
        return false;
    }
    if (options.palOverride.has_value() && *options.palOverride < 0) {
        std::cerr << "失败：pal 不能为负数\n";
        return false;
    }
    return true;
}

[[nodiscard]] bool EnsureOutputDir(const std::filesystem::path& outputDir)
{
    std::error_code error;
    std::filesystem::create_directories(outputDir, error);
    if (error) {
        std::cerr << "失败：创建输出目录失败，路径=" << PathToUtf8(outputDir)
                  << "，原因=" << error.message() << '\n';
        return false;
    }

    const bool isDir = std::filesystem::is_directory(outputDir, error);
    if (error || !isDir) {
        std::cerr << "失败：输出路径不是目录，路径=" << PathToUtf8(outputDir) << '\n';
        return false;
    }
    return true;
}

[[nodiscard]] bool IsValidImageBuffer(const cg::resource::DecodedImage& image) noexcept
{
    if (image.width <= 0 || image.height <= 0 || image.pixels.empty()) {
        return false;
    }

    const std::uint64_t width = static_cast<std::uint64_t>(image.width);
    const std::uint64_t height = static_cast<std::uint64_t>(image.height);
    const std::uint64_t expectedSize = width * height * Rgba8BytesPerPixel;
    return expectedSize <= std::numeric_limits<std::size_t>::max()
        && image.pixels.size() == static_cast<std::size_t>(expectedSize);
}

void AddFrameWarning(
    WarningCollector& warnings,
    std::size_t actionIndex,
    std::size_t frameIndex,
    const cg::resource::AnimeFrame& frame,
    std::string_view message,
    const std::optional<cg::resource::GraphicRef>& graphic,
    const cg::resource::ResourceError* error = nullptr)
{
    std::ostringstream output;
    output << message
           << "：action[" << actionIndex << "]"
           << "，frame index=" << frameIndex
           << "，graphicSeqNo=" << frame.graphicSeqNo;
    if (graphic.has_value()) {
        output << "，binSet=" << graphic->binSetIndex
               << "，seqNo=" << graphic->seqNo;
    }
    if (error != nullptr && !error->message.empty()) {
        output << "，原因=" << error->message;
    }
    if (error != nullptr && !error->path.empty()) {
        output << "，路径=" << PathToUtf8(error->path);
    }
    warnings.Add(output.str());
}

void AddActionFrameSpanWarning(
    WarningCollector& warnings,
    const cg::resource::AnimationClip& clip,
    const cg::resource::AnimeAction& action,
    std::size_t actionIndex)
{
    if (action.header.frameCount <= 0) {
        std::ostringstream output;
        output << "资源缺帧：action[" << actionIndex << "] frameCount=" << action.header.frameCount;
        warnings.Add(output.str());
        return;
    }
    if (action.frameIndex < 0) {
        std::ostringstream output;
        output << "异常帧：action[" << actionIndex << "] frameIndex=" << action.frameIndex;
        warnings.Add(output.str());
        return;
    }

    const std::size_t frameIndex = static_cast<std::size_t>(action.frameIndex);
    const std::size_t expectedCount = static_cast<std::size_t>(action.header.frameCount);
    const std::size_t actualCount = frameIndex <= clip.frames.size()
        ? std::min(expectedCount, clip.frames.size() - frameIndex)
        : 0;
    if (actualCount != expectedCount) {
        std::ostringstream output;
        output << "异常帧：action[" << actionIndex << "] frameCount=" << action.header.frameCount
               << "，实际 frame span=" << actualCount
               << "，frameIndex=" << action.frameIndex
               << "，总帧数=" << clip.frames.size();
        warnings.Add(output.str());
    }
}

[[nodiscard]] std::span<const cg::resource::AnimeFrame> GetAvailableActionFrames(
    const cg::resource::AnimationClip& clip,
    const cg::resource::AnimeAction& action) noexcept
{
    if (action.frameIndex < 0 || action.header.frameCount <= 0) {
        return {};
    }

    const std::size_t frameIndex = static_cast<std::size_t>(action.frameIndex);
    if (frameIndex >= clip.frames.size()) {
        return {};
    }

    const std::size_t expectedCount = static_cast<std::size_t>(action.header.frameCount);
    const std::size_t actualCount = std::min(expectedCount, clip.frames.size() - frameIndex);
    return std::span<const cg::resource::AnimeFrame>(clip.frames.data() + frameIndex, actualCount);
}

[[nodiscard]] ActionPaletteState ResolveActionPalette(
    cg::resource::ResourceManager& resources,
    const cg::resource::AnimeAction& action,
    const std::optional<int>& palOverride,
    WarningCollector& warnings)
{
    ActionPaletteState state;
    state.palIndex = palOverride.value_or(action.header.pal);
    if (state.palIndex <= 0) {
        return state;
    }

    state.hiddenPaletteMapNo = resources.FindHiddenPaletteMapNoByPalIndex(state.palIndex);
    if (!state.hiddenPaletteMapNo.has_value()) {
        warnings.Add("隐藏 palette fallback：pal=" + std::to_string(state.palIndex)
            + " 未命中 queue1 映射，使用默认 palette");
        return state;
    }

    state.hiddenPalette = resources.LoadHiddenPaletteByPalIndex(state.palIndex);
    if (!state.hiddenPalette.has_value()) {
        const cg::resource::ResourceError error = resources.LastError();
        std::string message = "隐藏 palette fallback：pal=" + std::to_string(state.palIndex)
            + "，hiddenMapNo=" + std::to_string(*state.hiddenPaletteMapNo)
            + " 读取失败，使用默认 palette";
        if (!error.message.empty()) {
            message += "，原因=" + error.message;
        }
        warnings.Add(std::move(message));
    }

    return state;
}

[[nodiscard]] cg::resource::Rgba8 ResolvePaletteZeroColor(
    const cg::resource::ResourceManager& resources,
    const ActionPaletteState& paletteState) noexcept
{
    if (paletteState.hiddenPalette.has_value()) {
        return (*paletteState.hiddenPalette)[0];
    }
    return resources.DefaultPalette()[0];
}

[[nodiscard]] DecodedActionFrames DecodeActionFrames(
    cg::resource::ResourceManager& resources,
    const cg::resource::AnimationClip& clip,
    const cg::resource::AnimeAction& action,
    const ActionPaletteState& paletteState,
    std::size_t actionIndex,
    WarningCollector& warnings)
{
    AddActionFrameSpanWarning(warnings, clip, action, actionIndex);
    const std::span<const cg::resource::AnimeFrame> frames = GetAvailableActionFrames(clip, action);

    DecodedActionFrames result;
    result.expectedFrameCount = frames.size();
    result.frames.reserve(frames.size());

    for (std::size_t index = 0; index < frames.size(); ++index) {
        const cg::resource::AnimeFrame& frame = frames[index];
        const std::optional<cg::resource::GraphicRef> graphic = resources.FindGraphicForFrame(clip, frame);
        if (!graphic.has_value()) {
            AddFrameWarning(
                warnings,
                actionIndex,
                index,
                frame,
                "Graphic 缺失，frame graphicSeqNo 找不到",
                graphic);
            continue;
        }

        std::optional<cg::resource::DecodedImage> image;
        if (paletteState.hiddenPalette.has_value()) {
            image = resources.DecodeGraphic(*graphic, *paletteState.hiddenPalette);
        } else {
            image = resources.DecodeGraphic(*graphic);
        }
        if (!image.has_value()) {
            const cg::resource::ResourceError error = resources.LastError();
            AddFrameWarning(
                warnings,
                actionIndex,
                index,
                frame,
                "Graphic 解码失败",
                graphic,
                &error);
            continue;
        }
        if (image->format != cg::render::PixelFormat::Rgba8 || !IsValidImageBuffer(*image)) {
            AddFrameWarning(
                warnings,
                actionIndex,
                index,
                frame,
                "异常帧，Graphic 图像为空或不是 RGBA8",
                graphic);
            continue;
        }

        ExportFrame exportFrame;
        exportFrame.frame = frame;
        exportFrame.graphic = *graphic;
        exportFrame.image = std::move(*image);
        result.frames.push_back(std::move(exportFrame));
    }

    return result;
}

[[nodiscard]] bool BuildAtlasLayout(
    const std::vector<ExportFrame>& frames,
    std::vector<AtlasPlacement>& placements,
    int& atlasWidth,
    int& atlasHeight,
    WarningCollector& warnings,
    std::size_t actionIndex)
{
    placements.clear();
    atlasWidth = 0;
    atlasHeight = 0;

    int x = 0;
    int y = 0;
    int rowHeight = 0;
    int maxUsedWidth = 0;
    for (const ExportFrame& frame : frames) {
        const cg::resource::DecodedImage& image = frame.image;
        if (image.width > MaxAtlasWidth) {
            std::ostringstream output;
            output << "atlas 失败：action[" << actionIndex << "] 单帧宽度超过 1024"
                   << "，seqNo=" << frame.graphic.seqNo
                   << "，尺寸=" << image.width << 'x' << image.height;
            warnings.Add(output.str());
            return false;
        }

        if (x > 0 && x + image.width > MaxAtlasWidth) {
            maxUsedWidth = std::max(maxUsedWidth, x);
            if (y > std::numeric_limits<int>::max() - rowHeight) {
                warnings.Add("atlas 失败：高度整数溢出，action[" + std::to_string(actionIndex) + "]");
                return false;
            }
            y += rowHeight;
            x = 0;
            rowHeight = 0;
        }

        placements.push_back(AtlasPlacement { x, y, &frame });
        x += image.width;
        rowHeight = std::max(rowHeight, image.height);
    }

    if (placements.empty() || rowHeight <= 0) {
        return false;
    }
    maxUsedWidth = std::max(maxUsedWidth, x);
    if (y > std::numeric_limits<int>::max() - rowHeight) {
        warnings.Add("atlas 失败：高度整数溢出，action[" + std::to_string(actionIndex) + "]");
        return false;
    }
    atlasWidth = maxUsedWidth;
    atlasHeight = y + rowHeight;
    return atlasWidth > 0 && atlasHeight > 0;
}

[[nodiscard]] bool SaveAtlasPng(
    const std::vector<ExportFrame>& frames,
    const std::filesystem::path& outputPath,
    std::uint32_t bgColor,
    const cg::resource::Rgba8& sourceBgColor,
    int& atlasWidth,
    int& atlasHeight,
    WarningCollector& warnings,
    std::size_t actionIndex)
{
    std::vector<AtlasPlacement> placements;
    if (!BuildAtlasLayout(frames, placements, atlasWidth, atlasHeight, warnings, actionIndex)) {
        return false;
    }

    SDL_Surface* surface = SDL_CreateSurface(atlasWidth, atlasHeight, SDL_PIXELFORMAT_RGBA32);
    if (surface == nullptr) {
        PrintSdlError("创建 atlas surface 失败");
        return false;
    }

    const std::uint8_t bgR = static_cast<std::uint8_t>((bgColor >> 24U) & 0xFFU);
    const std::uint8_t bgG = static_cast<std::uint8_t>((bgColor >> 16U) & 0xFFU);
    const std::uint8_t bgB = static_cast<std::uint8_t>((bgColor >> 8U) & 0xFFU);
    const std::uint8_t bgA = static_cast<std::uint8_t>(bgColor & 0xFFU);
    const std::uint32_t mappedBgColor = SDL_MapSurfaceRGBA(surface, bgR, bgG, bgB, bgA);
    if (!SDL_FillSurfaceRect(surface, nullptr, mappedBgColor)) {
        PrintSdlError("填充 atlas 背景失败");
        SDL_DestroySurface(surface);
        return false;
    }

    for (const AtlasPlacement& placement : placements) {
        const cg::resource::DecodedImage& image = placement.frame->image;
        const std::size_t sourcePitch = static_cast<std::size_t>(image.width) * Rgba8BytesPerPixel;
        for (int row = 0; row < image.height; ++row) {
            const std::uint8_t* const sourceRow = image.pixels.data() + static_cast<std::size_t>(row) * sourcePitch;
            std::uint8_t* const targetRow = static_cast<std::uint8_t*>(surface->pixels)
                + static_cast<std::size_t>(placement.y + row) * static_cast<std::size_t>(surface->pitch)
                + static_cast<std::size_t>(placement.x) * Rgba8BytesPerPixel;
            for (int column = 0; column < image.width; ++column) {
                const std::size_t offset = static_cast<std::size_t>(column) * Rgba8BytesPerPixel;
                const bool isTransparent = sourceRow[offset + 3] == 0;
                const bool isPaletteBackground = sourceRow[offset] == sourceBgColor.r
                    && sourceRow[offset + 1] == sourceBgColor.g
                    && sourceRow[offset + 2] == sourceBgColor.b;
                if (isTransparent || isPaletteBackground) {
                    continue;
                }
                std::memcpy(targetRow + offset, sourceRow + offset, Rgba8BytesPerPixel);
            }
        }
    }

    const std::string savePath = PathToUtf8(outputPath);
    const bool saved = IMG_SavePNG(surface, savePath.c_str());
    SDL_DestroySurface(surface);
    if (!saved) {
        PrintSdlError("保存 atlas PNG 失败");
        return false;
    }
    return true;
}

[[nodiscard]] std::filesystem::path BuildAtlasOutputPath(
    const std::filesystem::path& outputDir,
    int animeNo,
    const cg::resource::AnimeAction& action)
{
    std::ostringstream filename;
    filename << "anime_" << animeNo
             << "_dir_" << static_cast<int>(action.header.dir)
             << "_action_" << static_cast<int>(action.header.action)
             << ".png";
    return outputDir / filename.str();
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

    ExportOptions options;
    if (!ParseOptions(argc, argv, options)) {
        PrintUsage(std::cerr);
        return 2;
    }
    if (!EnsureOutputDir(options.outputDir)) {
        return 1;
    }

    const std::filesystem::path ymlPath = PathFromUtf8(argv[1]);
    const std::filesystem::path palettePath = PathFromUtf8(argv[2]);
    const std::filesystem::path hiddenPaletteTablePath = BuildHiddenPaletteTablePath(ymlPath);
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
        cg::resource::ResourceLoadOptions loadOptions;
        loadOptions.buildAutoMapColorCache = false;
        loadOptions.hiddenPaletteTablePath = hiddenPaletteTablePath;
        binSetsLoaded = resources.LoadBinSets(config.binSets, palettePath, loadOptions);
        if (!binSetsLoaded) {
            binSetError = resources.LastError();
        }
    }
    if (!binSetsLoaded) {
        PrintResourceError("binSet 加载失败", binSetError);
        return 1;
    }

    std::optional<cg::resource::AnimationClip> clip;
    cg::resource::ResourceError animationError;
    {
        ScopedStreamRedirect suppressInfo(std::cout);
        ScopedStreamRedirect suppressError(std::cerr);
        clip = resources.LoadAnimation(*options.sourceAnimeNo);
        if (!clip.has_value()) {
            animationError = resources.LastError();
        }
    }
    if (!clip.has_value()) {
        PrintResourceError("Anime 加载失败", animationError);
        return 1;
    }

    const std::optional<cg::resource::AnimeRef> ref = resources.FindAnime(*options.sourceAnimeNo);
    if (!ref.has_value()) {
        std::cerr << "失败：Anime 已加载但无法读取原始 animeNo，animeNo=" << *options.sourceAnimeNo << '\n';
        return 1;
    }

    std::cout << "导出模式：sprite_atlas\n";
    std::cout << "Anime：animeNo=" << ref->sourceAnimeNo
              << "，indexNo=" << ref->indexNo
              << "，binSetIndex=" << ref->binSetIndex
              << "，action 数=" << clip->actions.size() << '\n';
    std::cout << "输出目录：" << PathToUtf8(options.outputDir) << '\n';
    if (options.palOverride.has_value()) {
        std::cout << "palette 覆盖：pal=" << *options.palOverride << '\n';
    }

    WarningCollector warnings;
    std::size_t exportedCount = 0;
    std::size_t failedCount = 0;
    for (std::size_t actionIndex = 0; actionIndex < clip->actions.size(); ++actionIndex) {
        const cg::resource::AnimeAction& action = clip->actions[actionIndex];
        const ActionPaletteState paletteState = ResolveActionPalette(resources, action, options.palOverride, warnings);
        const cg::resource::Rgba8 sourceBgColor = ResolvePaletteZeroColor(resources, paletteState);
        const DecodedActionFrames decoded = DecodeActionFrames(resources, *clip, action, paletteState, actionIndex, warnings);
        if (decoded.expectedFrameCount == 0 || decoded.frames.empty()) {
            warnings.Add("atlas 跳过：action[" + std::to_string(actionIndex) + "] 没有可导出的帧");
            ++failedCount;
            continue;
        }
        const std::filesystem::path outputPath =
            BuildAtlasOutputPath(options.outputDir, *options.sourceAnimeNo, action);
        int atlasWidth = 0;
        int atlasHeight = 0;
        if (!SaveAtlasPng(decoded.frames, outputPath, options.bgColor, sourceBgColor, atlasWidth, atlasHeight, warnings, actionIndex)) {
            ++failedCount;
            continue;
        }

        ++exportedCount;
        std::cout << "导出 atlas：路径=" << PathToUtf8(outputPath)
                  << "，dir=" << static_cast<int>(action.header.dir)
                  << "，action=" << static_cast<int>(action.header.action)
                  << "，frameCount=" << decoded.frames.size() << '/' << decoded.expectedFrameCount
                  << "，尺寸=" << atlasWidth << 'x' << atlasHeight
                  << "，pal=" << paletteState.palIndex << '\n';
    }

    warnings.PrintSummary(std::cout);
    std::cout << "导出完成：成功=" << exportedCount << "，失败/跳过=" << failedCount << '\n';
    if (exportedCount == 0 || failedCount > 0) {
        return 1;
    }
    return 0;
}
