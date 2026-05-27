#include "platform/Path.hpp"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_stdinc.h>

#include <string>

namespace cg::platform {
namespace {

[[nodiscard]] std::filesystem::path FromUtf8Path(const char* path)
{
    if (path == nullptr || path[0] == '\0') {
        return {};
    }

    return std::filesystem::path(std::u8string(reinterpret_cast<const char8_t*>(path)));
}

[[nodiscard]] std::filesystem::path AppendChildPath(std::filesystem::path parent, const char* child)
{
    if (parent.empty()) {
        return {};
    }

    return parent / child;
}

} // namespace

std::filesystem::path BasePath()
{
    return FromUtf8Path(SDL_GetBasePath());
}

std::filesystem::path PrefPath(std::string_view org, std::string_view app)
{
    const std::string orgName(org);
    const std::string appName(app);

    char* path = SDL_GetPrefPath(orgName.c_str(), appName.c_str());
    const std::filesystem::path result = FromUtf8Path(path);
    SDL_free(path);
    return result;
}

std::filesystem::path AssetsPath()
{
    return AppendChildPath(BasePath(), "assets");
}

std::filesystem::path ConfigPath()
{
    return AppendChildPath(PrefPath(), "config");
}

std::filesystem::path SavePath()
{
    return AppendChildPath(PrefPath(), "save");
}

} // namespace cg::platform
