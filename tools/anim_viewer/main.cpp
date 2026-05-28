#include "resource/AnimationLoader.hpp"
#include "resource/BinSetConfigLoader.hpp"
#include "resource/ResourceManager.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iomanip>
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
constexpr int Rgba8BytesPerPixel = 4;
constexpr int InitialMinWindowWidth = 320;
constexpr int InitialMinWindowHeight = 240;
constexpr int InitialMaxWindowWidth = 1024;
constexpr int InitialMaxWindowHeight = 768;
constexpr double DefaultFrameDurationMs = 100.0;
constexpr double MinFrameDurationMs = 16.0;
constexpr double MinSpeed = 0.25;
constexpr double MaxSpeed = 4.0;
constexpr double SpeedStep = 0.25;
constexpr float AnchorHalfSize = 8.0F;
constexpr int FallbackBoundsPadding = 16;
constexpr std::size_t MaxWarningDetails = 20;

struct ViewerOptions {
    std::optional<int> sourceAnimeNo;
    std::optional<int> dir;
    std::optional<int> action;
    bool play = false;
};

struct PlaybackFrame {
    cg::resource::AnimeFrame frame;
    std::optional<cg::resource::GraphicRef> graphic;
    std::optional<cg::resource::DecodedImage> image;
    SDL_Texture* texture = nullptr;
};

struct PlaybackBounds {
    int minX = 0;
    int minY = 0;
    int maxX = 0;
    int maxY = 0;
    bool hasValue = false;
};

struct PlaybackTransform {
    float scale = 1.0F;
    float originX = 0.0F;
    float originY = 0.0F;
};

struct PlaybackState {
    std::size_t frameIndex = 0;
    double speed = 1.0;
    bool paused = false;
    bool showOverlay = true;
};

class AnimWarningCollector final {
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
    output << "用法：cg-anim-viewer <cgmsv.yml> <palette.cgp> --anime-no <原始animeNo> [--dir <dir> --action <action>] [--play]\n";
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
            options.sourceAnimeNo = value;
        } else if (arg == "--dir" || arg == "--direction") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            options.dir = value;
        } else if (arg == "--action") {
            if (!ReadOptionValue(argc, argv, index, arg, value)) {
                return false;
            }
            options.action = value;
        } else if (arg == "--play") {
            options.play = true;
        } else {
            std::cerr << "失败：未知参数 " << arg << '\n';
            return false;
        }
    }

    if (!options.sourceAnimeNo.has_value()) {
        std::cerr << "失败：缺少 --anime-no <animeNo>\n";
        return false;
    }
    if (options.dir.has_value() != options.action.has_value()) {
        std::cerr << "失败：--dir 和 --action 必须同时指定\n";
        return false;
    }
    if (options.play && (!options.dir.has_value() || !options.action.has_value())) {
        std::cerr << "失败：--play 需要同时指定 --dir 和 --action\n";
        return false;
    }
    return true;
}

void PrintActionSummary(const cg::resource::AnimeActionHeader& header)
{
    std::cout << "dir=" << static_cast<int>(header.dir)
              << "，action=" << static_cast<int>(header.action)
              << "，duration=" << header.duration
              << "，frameCount=" << header.frameCount
              << "，pal=" << header.pal
              << "，revert=" << header.revert;
}

void PrintAnimation(
    const cg::resource::AnimeRef& ref,
    const cg::resource::AnimationClip& clip)
{
    std::cout << "Anime：animeNo=" << ref.sourceAnimeNo
              << "，index=" << ref.index
              << "，binSetIndex=" << ref.binSetIndex << '\n';
    std::cout << "action 数：" << clip.actions.size()
              << "，总帧数：" << clip.frames.size() << '\n';

    for (std::size_t index = 0; index < clip.actions.size(); ++index) {
        const cg::resource::AnimeActionHeader& header = clip.actions[index].header;
        std::cout << "action[" << index << "] ";
        PrintActionSummary(header);
        std::cout << '\n';
    }
}

void PrintSelectedAnimation(
    const cg::resource::AnimeRef& ref,
    const cg::resource::AnimationClip& clip,
    const cg::resource::AnimeAction& action)
{
    std::cout << "Anime：animeNo=" << ref.sourceAnimeNo
              << "，index=" << ref.index
              << "，binSetIndex=" << ref.binSetIndex << '\n';
    std::cout << "匹配 action：";
    PrintActionSummary(action.header);
    std::cout << '\n';

    const std::span<const cg::resource::AnimeFrame> frames = cg::resource::GetAnimeActionFrames(clip, action);
    for (std::size_t index = 0; index < frames.size(); ++index) {
        const cg::resource::AnimeFrame& frame = frames[index];
        std::cout << "frame index=" << index
                  << "，graphicSeqNo=" << frame.graphicSeqNo
                  << "，x=" << frame.x
                  << "，y=" << frame.y
                  << "，flag=" << frame.flag << '\n';
    }
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
    AnimWarningCollector& warnings,
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
    AnimWarningCollector& warnings,
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

void ScanActionWarnings(
    cg::resource::ResourceManager& resources,
    const cg::resource::AnimationClip& clip,
    const cg::resource::AnimeAction& action,
    std::size_t actionIndex,
    AnimWarningCollector& warnings)
{
    AddActionFrameSpanWarning(warnings, clip, action, actionIndex);
    const std::span<const cg::resource::AnimeFrame> frames = GetAvailableActionFrames(clip, action);
    for (std::size_t index = 0; index < frames.size(); ++index) {
        const std::optional<cg::resource::GraphicRef> graphic = resources.FindGraphicForFrame(clip, frames[index]);
        if (!graphic.has_value()) {
            AddFrameWarning(
                warnings,
                actionIndex,
                index,
                frames[index],
                "Graphic 缺失，frame graphicSeqNo 找不到",
                graphic);
        }
    }
}

void ScanAnimationWarnings(
    cg::resource::ResourceManager& resources,
    const cg::resource::AnimationClip& clip,
    AnimWarningCollector& warnings)
{
    for (std::size_t index = 0; index < clip.actions.size(); ++index) {
        ScanActionWarnings(resources, clip, clip.actions[index], index, warnings);
    }
}

[[nodiscard]] std::size_t FindActionIndex(
    const cg::resource::AnimationClip& clip,
    const cg::resource::AnimeAction& action) noexcept
{
    for (std::size_t index = 0; index < clip.actions.size(); ++index) {
        if (&clip.actions[index] == &action) {
            return index;
        }
    }
    return clip.actions.size();
}

[[nodiscard]] std::vector<PlaybackFrame> DecodePlaybackFrames(
    cg::resource::ResourceManager& resources,
    const cg::resource::AnimationClip& clip,
    const cg::resource::AnimeAction& action,
    std::size_t actionIndex,
    AnimWarningCollector& warnings)
{
    AddActionFrameSpanWarning(warnings, clip, action, actionIndex);
    const std::span<const cg::resource::AnimeFrame> frames = GetAvailableActionFrames(clip, action);
    std::vector<PlaybackFrame> playbackFrames;
    playbackFrames.reserve(frames.size());

    for (std::size_t index = 0; index < frames.size(); ++index) {
        PlaybackFrame playbackFrame;
        playbackFrame.frame = frames[index];
        playbackFrame.graphic = resources.FindGraphicForFrame(clip, playbackFrame.frame);
        if (!playbackFrame.graphic.has_value()) {
            AddFrameWarning(
                warnings,
                actionIndex,
                index,
                playbackFrame.frame,
                "Graphic 缺失，frame graphicSeqNo 找不到",
                playbackFrame.graphic);
            playbackFrames.push_back(std::move(playbackFrame));
            continue;
        }

        playbackFrame.image = resources.DecodeGraphic(*playbackFrame.graphic);
        if (!playbackFrame.image.has_value()) {
            const cg::resource::ResourceError error = resources.LastError();
            AddFrameWarning(
                warnings,
                actionIndex,
                index,
                playbackFrame.frame,
                "Graphic 解码失败",
                playbackFrame.graphic,
                &error);
            playbackFrames.push_back(std::move(playbackFrame));
            continue;
        }
        if (playbackFrame.image->format != cg::render::PixelFormat::Rgba8 || !IsValidImageBuffer(*playbackFrame.image)) {
            AddFrameWarning(
                warnings,
                actionIndex,
                index,
                playbackFrame.frame,
                "异常帧，Graphic 图像为空或不是 RGBA8",
                playbackFrame.graphic);
            playbackFrame.image.reset();
        }
        playbackFrames.push_back(std::move(playbackFrame));
    }

    return playbackFrames;
}

[[nodiscard]] PlaybackBounds ComputePlaybackBounds(const std::vector<PlaybackFrame>& frames)
{
    PlaybackBounds bounds;
    for (const PlaybackFrame& frame : frames) {
        if (!frame.image.has_value()) {
            continue;
        }

        const cg::resource::DecodedImage& image = *frame.image;
        const int left = static_cast<int>(frame.frame.x) + image.offsetX;
        const int top = static_cast<int>(frame.frame.y) + image.offsetY;
        const int right = left + image.width;
        const int bottom = top + image.height;
        if (!bounds.hasValue) {
            bounds.minX = left;
            bounds.minY = top;
            bounds.maxX = right;
            bounds.maxY = bottom;
            bounds.hasValue = true;
            continue;
        }

        bounds.minX = std::min(bounds.minX, left);
        bounds.minY = std::min(bounds.minY, top);
        bounds.maxX = std::max(bounds.maxX, right);
        bounds.maxY = std::max(bounds.maxY, bottom);
    }

    return bounds;
}

[[nodiscard]] PlaybackBounds ComputeFallbackPlaybackBounds(const std::vector<PlaybackFrame>& frames)
{
    PlaybackBounds bounds;
    for (const PlaybackFrame& frame : frames) {
        const int left = static_cast<int>(frame.frame.x) - FallbackBoundsPadding;
        const int top = static_cast<int>(frame.frame.y) - FallbackBoundsPadding;
        const int right = static_cast<int>(frame.frame.x) + FallbackBoundsPadding;
        const int bottom = static_cast<int>(frame.frame.y) + FallbackBoundsPadding;
        if (!bounds.hasValue) {
            bounds.minX = left;
            bounds.minY = top;
            bounds.maxX = right;
            bounds.maxY = bottom;
            bounds.hasValue = true;
            continue;
        }

        bounds.minX = std::min(bounds.minX, left);
        bounds.minY = std::min(bounds.minY, top);
        bounds.maxX = std::max(bounds.maxX, right);
        bounds.maxY = std::max(bounds.maxY, bottom);
    }

    return bounds;
}

[[nodiscard]] double ComputeFrameDurationMs(const cg::resource::AnimeAction& action, std::size_t frameCount) noexcept
{
    if (frameCount == 0 || action.header.duration <= 0) {
        return DefaultFrameDurationMs;
    }

    const double frameDuration = static_cast<double>(action.header.duration) / static_cast<double>(frameCount);
    return std::max(MinFrameDurationMs, frameDuration);
}

[[nodiscard]] std::string FormatSpeed(double speed)
{
    std::ostringstream output;
    output << std::fixed << std::setprecision(2) << speed << 'x';
    return output.str();
}

[[nodiscard]] std::string BuildPlaybackStatus(
    int sourceAnimeNo,
    int dir,
    int action,
    const PlaybackState& state,
    std::size_t frameCount)
{
    return "animeNo=" + std::to_string(sourceAnimeNo)
        + " dir=" + std::to_string(dir)
        + " action=" + std::to_string(action)
        + " frame=" + std::to_string(state.frameIndex + 1) + "/" + std::to_string(frameCount)
        + " speed=" + FormatSpeed(state.speed)
        + " pause=" + (state.paused ? "true" : "false")
        + " overlay=" + (state.showOverlay ? "true" : "false");
}

void UpdatePlaybackTitle(
    SDL_Window* window,
    int sourceAnimeNo,
    int dir,
    int action,
    const PlaybackState& state,
    std::size_t frameCount)
{
    const std::string title = "cg-anim-viewer " + BuildPlaybackStatus(sourceAnimeNo, dir, action, state, frameCount);
    SDL_SetWindowTitle(window, title.c_str());
}

void PrintPlaybackStatus(
    int sourceAnimeNo,
    int dir,
    int action,
    const PlaybackState& state,
    std::size_t frameCount)
{
    std::cout << "播放状态：" << BuildPlaybackStatus(sourceAnimeNo, dir, action, state, frameCount) << '\n';
}

[[nodiscard]] float ComputePlaybackScale(const PlaybackBounds& bounds, int windowWidth, int windowHeight) noexcept
{
    if (!bounds.hasValue || windowWidth <= 0 || windowHeight <= 0) {
        return 1.0F;
    }

    const int contentWidth = std::max(1, bounds.maxX - bounds.minX);
    const int contentHeight = std::max(1, bounds.maxY - bounds.minY);
    const float fitX = static_cast<float>(windowWidth) / static_cast<float>(contentWidth);
    const float fitY = static_cast<float>(windowHeight) / static_cast<float>(contentHeight);
    return std::min(1.0F, std::min(fitX, fitY));
}

[[nodiscard]] PlaybackTransform ComputePlaybackTransform(
    const PlaybackBounds& bounds,
    int windowWidth,
    int windowHeight) noexcept
{
    const float scale = ComputePlaybackScale(bounds, windowWidth, windowHeight);
    const float contentWidth = static_cast<float>(std::max(1, bounds.maxX - bounds.minX)) * scale;
    const float contentHeight = static_cast<float>(std::max(1, bounds.maxY - bounds.minY)) * scale;

    return PlaybackTransform {
        scale,
        (static_cast<float>(windowWidth) - contentWidth) * 0.5F - static_cast<float>(bounds.minX) * scale,
        (static_cast<float>(windowHeight) - contentHeight) * 0.5F - static_cast<float>(bounds.minY) * scale,
    };
}

[[nodiscard]] SDL_FPoint ProjectPlaybackPoint(const PlaybackTransform& transform, int x, int y) noexcept
{
    return SDL_FPoint {
        transform.originX + static_cast<float>(x) * transform.scale,
        transform.originY + static_cast<float>(y) * transform.scale,
    };
}

[[nodiscard]] SDL_FRect ProjectPlaybackRect(
    const PlaybackTransform& transform,
    int x,
    int y,
    int width,
    int height) noexcept
{
    const SDL_FPoint point = ProjectPlaybackPoint(transform, x, y);
    return SDL_FRect {
        point.x,
        point.y,
        static_cast<float>(width) * transform.scale,
        static_cast<float>(height) * transform.scale,
    };
}

[[nodiscard]] SDL_FRect ComputeFrameDestination(
    const PlaybackFrame& frame,
    const PlaybackBounds& bounds,
    int windowWidth,
    int windowHeight) noexcept
{
    const cg::resource::DecodedImage& image = *frame.image;
    const PlaybackTransform transform = ComputePlaybackTransform(bounds, windowWidth, windowHeight);
    const int left = static_cast<int>(frame.frame.x) + image.offsetX;
    const int top = static_cast<int>(frame.frame.y) + image.offsetY;
    return ProjectPlaybackRect(transform, left, top, image.width, image.height);
}

[[nodiscard]] bool CreateFrameTextures(SDL_Renderer* renderer, std::vector<PlaybackFrame>& frames)
{
    for (PlaybackFrame& frame : frames) {
        if (!frame.image.has_value()) {
            continue;
        }

        const cg::resource::DecodedImage& image = *frame.image;
        frame.texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STATIC,
            image.width,
            image.height);
        if (frame.texture == nullptr) {
            PrintSdlError("创建帧纹理失败");
            return false;
        }
        if (!SDL_SetTextureScaleMode(frame.texture, SDL_SCALEMODE_NEAREST)
            || !SDL_SetTextureBlendMode(frame.texture, SDL_BLENDMODE_BLEND)
            || !SDL_UpdateTexture(frame.texture, nullptr, image.pixels.data(), image.width * Rgba8BytesPerPixel)) {
            PrintSdlError("上传帧纹理失败");
            return false;
        }
    }

    return true;
}

void DestroyFrameTextures(std::vector<PlaybackFrame>& frames)
{
    for (PlaybackFrame& frame : frames) {
        if (frame.texture != nullptr) {
            SDL_DestroyTexture(frame.texture);
            frame.texture = nullptr;
        }
    }
}

[[nodiscard]] bool RenderPlaybackOverlay(
    SDL_Renderer* renderer,
    const PlaybackBounds& bounds,
    const PlaybackFrame& frame,
    int windowWidth,
    int windowHeight)
{
    const PlaybackTransform transform = ComputePlaybackTransform(bounds, windowWidth, windowHeight);
    const SDL_FRect actionRect = ProjectPlaybackRect(
        transform,
        bounds.minX,
        bounds.minY,
        bounds.maxX - bounds.minX,
        bounds.maxY - bounds.minY);
    if (!SDL_SetRenderDrawColor(renderer, 64, 176, 255, 255)
        || !SDL_RenderRect(renderer, &actionRect)) {
        return false;
    }

    if (frame.image.has_value()) {
        const cg::resource::DecodedImage& image = *frame.image;
        const SDL_FRect frameRect = ProjectPlaybackRect(
            transform,
            static_cast<int>(frame.frame.x) + image.offsetX,
            static_cast<int>(frame.frame.y) + image.offsetY,
            image.width,
            image.height);
        if (!SDL_SetRenderDrawColor(renderer, 255, 216, 96, 255)
            || !SDL_RenderRect(renderer, &frameRect)) {
            return false;
        }
    }

    const SDL_FPoint anchor = ProjectPlaybackPoint(
        transform,
        static_cast<int>(frame.frame.x),
        static_cast<int>(frame.frame.y));
    return SDL_SetRenderDrawColor(renderer, 255, 96, 96, 255)
        && SDL_RenderLine(renderer, anchor.x - AnchorHalfSize, anchor.y, anchor.x + AnchorHalfSize, anchor.y)
        && SDL_RenderLine(renderer, anchor.x, anchor.y - AnchorHalfSize, anchor.x, anchor.y + AnchorHalfSize);
}

[[nodiscard]] bool RenderPlaybackFrame(
    SDL_Window* window,
    SDL_Renderer* renderer,
    const PlaybackBounds& bounds,
    const PlaybackFrame& frame,
    bool showOverlay)
{
    if (!SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255) || !SDL_RenderClear(renderer)) {
        return false;
    }

    int windowWidth = 0;
    int windowHeight = 0;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    if (frame.texture != nullptr && frame.image.has_value()) {
        const SDL_FRect destination = ComputeFrameDestination(frame, bounds, windowWidth, windowHeight);
        if (!SDL_RenderTexture(renderer, frame.texture, nullptr, &destination)) {
            return false;
        }
    }

    if (showOverlay && !RenderPlaybackOverlay(renderer, bounds, frame, windowWidth, windowHeight)) {
        return false;
    }

    return SDL_RenderPresent(renderer);
}

void AdvancePlaybackFrame(PlaybackState& state, std::size_t frameCount, int step)
{
    if (frameCount == 0) {
        state.frameIndex = 0;
        return;
    }

    const int count = static_cast<int>(frameCount);
    const int current = static_cast<int>(state.frameIndex);
    state.frameIndex = static_cast<std::size_t>((current + step + count) % count);
}

[[nodiscard]] bool IsEscapeKey(const SDL_KeyboardEvent& key) noexcept
{
    return key.key == SDLK_ESCAPE || key.scancode == SDL_SCANCODE_ESCAPE;
}

[[nodiscard]] bool IsSpaceKey(const SDL_KeyboardEvent& key) noexcept
{
    return key.key == SDLK_SPACE || key.scancode == SDL_SCANCODE_SPACE;
}

[[nodiscard]] bool IsLeftKey(const SDL_KeyboardEvent& key) noexcept
{
    return key.key == SDLK_LEFT || key.scancode == SDL_SCANCODE_LEFT;
}

[[nodiscard]] bool IsRightKey(const SDL_KeyboardEvent& key) noexcept
{
    return key.key == SDLK_RIGHT || key.scancode == SDL_SCANCODE_RIGHT;
}

[[nodiscard]] bool IsSpeedUpKey(const SDL_KeyboardEvent& key) noexcept
{
    return key.key == SDLK_PLUS
        || key.key == SDLK_EQUALS
        || key.key == SDLK_KP_PLUS
        || key.scancode == SDL_SCANCODE_EQUALS;
}

[[nodiscard]] bool IsSpeedDownKey(const SDL_KeyboardEvent& key) noexcept
{
    return key.key == SDLK_MINUS
        || key.key == SDLK_KP_MINUS
        || key.scancode == SDL_SCANCODE_MINUS;
}

[[nodiscard]] bool IsOverlayKey(const SDL_KeyboardEvent& key) noexcept
{
    return key.key == SDLK_B || key.scancode == SDL_SCANCODE_B;
}

[[nodiscard]] bool HandlePlaybackKey(
    const SDL_KeyboardEvent& key,
    PlaybackState& state,
    std::size_t frameCount)
{
    if (IsSpaceKey(key)) {
        state.paused = !state.paused;
        return true;
    }
    if (IsLeftKey(key)) {
        state.paused = true;
        AdvancePlaybackFrame(state, frameCount, -1);
        return true;
    }
    if (IsRightKey(key)) {
        state.paused = true;
        AdvancePlaybackFrame(state, frameCount, 1);
        return true;
    }
    if (IsSpeedUpKey(key)) {
        state.speed = std::min(MaxSpeed, state.speed + SpeedStep);
        return true;
    }
    if (IsSpeedDownKey(key)) {
        state.speed = std::max(MinSpeed, state.speed - SpeedStep);
        return true;
    }
    if (IsOverlayKey(key)) {
        state.showOverlay = !state.showOverlay;
        return true;
    }
    return false;
}

[[nodiscard]] bool ShowPlaybackWindow(
    int sourceAnimeNo,
    int dir,
    int action,
    const cg::resource::AnimeAction& animeAction,
    std::vector<PlaybackFrame>& frames)
{
    if (frames.empty()) {
        std::cerr << "失败：匹配 action 没有帧\n";
        return false;
    }

    PlaybackBounds bounds = ComputePlaybackBounds(frames);
    if (!bounds.hasValue) {
        bounds = ComputeFallbackPlaybackBounds(frames);
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        PrintSdlError("SDL 初始化失败");
        return false;
    }

    const int contentWidth = std::max(1, bounds.maxX - bounds.minX);
    const int contentHeight = std::max(1, bounds.maxY - bounds.minY);
    const int windowWidth = std::clamp(contentWidth, InitialMinWindowWidth, InitialMaxWindowWidth);
    const int windowHeight = std::clamp(contentHeight, InitialMinWindowHeight, InitialMaxWindowHeight);
    SDL_Window* window = SDL_CreateWindow("cg-anim-viewer", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        PrintSdlError("创建窗口失败");
        SDL_Quit();
        return false;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        PrintSdlError("创建 renderer 失败");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (!CreateFrameTextures(renderer, frames)) {
        DestroyFrameTextures(frames);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    PlaybackState state;
    const double frameDurationMs = ComputeFrameDurationMs(animeAction, frames.size());
    double elapsedMs = 0.0;
    std::uint64_t lastTick = SDL_GetTicks();
    UpdatePlaybackTitle(window, sourceAnimeNo, dir, action, state, frames.size());
    std::cout << "窗口已打开：space 暂停/播放，左右键逐帧，+/- 调速，B 切换 overlay，ESC 或关闭窗口退出\n";
    PrintPlaybackStatus(sourceAnimeNo, dir, action, state, frames.size());
    std::cout.flush();

    bool running = true;
    while (running) {
        bool titleChanged = false;
        bool statusChanged = false;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (IsEscapeKey(event.key)) {
                    running = false;
                } else if (HandlePlaybackKey(event.key, state, frames.size())) {
                    elapsedMs = 0.0;
                    titleChanged = true;
                    statusChanged = true;
                }
                break;
            default:
                break;
            }
        }

        const std::uint64_t currentTick = SDL_GetTicks();
        const double deltaMs = static_cast<double>(currentTick - lastTick);
        lastTick = currentTick;
        if (!state.paused) {
            elapsedMs += deltaMs * state.speed;
            if (elapsedMs >= frameDurationMs) {
                const int step = static_cast<int>(elapsedMs / frameDurationMs);
                AdvancePlaybackFrame(state, frames.size(), step);
                elapsedMs -= static_cast<double>(step) * frameDurationMs;
                titleChanged = true;
            }
        }

        if (titleChanged) {
            UpdatePlaybackTitle(window, sourceAnimeNo, dir, action, state, frames.size());
        }
        if (statusChanged) {
            PrintPlaybackStatus(sourceAnimeNo, dir, action, state, frames.size());
            std::cout.flush();
        }

        if (!RenderPlaybackFrame(window, renderer, bounds, frames[state.frameIndex], state.showOverlay)) {
            PrintSdlError("渲染动画帧失败");
            running = false;
        }
        SDL_Delay(16);
    }

    DestroyFrameTextures(frames);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
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

    AnimWarningCollector warnings;
    if (options.dir.has_value() && options.action.has_value()) {
        const cg::resource::AnimeAction* const action =
            cg::resource::FindAnimeAction(*clip, *options.action, *options.dir);
        if (action == nullptr) {
            std::cerr << "失败：未找到匹配 action，animeNo=" << *options.sourceAnimeNo
                      << "，dir=" << *options.dir
                      << "，action=" << *options.action << '\n';
            return 1;
        }

        PrintSelectedAnimation(*ref, *clip, *action);
        const std::size_t actionIndex = FindActionIndex(*clip, *action);
        std::vector<PlaybackFrame> playbackFrames =
            DecodePlaybackFrames(resources, *clip, *action, actionIndex, warnings);
        if (options.play) {
            const bool windowShown = ShowPlaybackWindow(*options.sourceAnimeNo, *options.dir, *options.action, *action, playbackFrames);
            warnings.PrintSummary(std::cout);
            if (!windowShown) {
                return 1;
            }
        } else {
            warnings.PrintSummary(std::cout);
        }
        return 0;
    }

    PrintAnimation(*ref, *clip);
    ScanAnimationWarnings(resources, *clip, warnings);
    warnings.PrintSummary(std::cout);
    return 0;
}
