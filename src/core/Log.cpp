#include "core/Log.hpp"

#include <SDL3/SDL.h>

#include <iostream>
#include <string>

namespace cg::core {
namespace {

const char* ToText(LogLevel level)
{
    switch (level) {
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    }

    return "UNKNOWN";
}

} // namespace

void Log(LogLevel level, std::string_view module, std::string_view message)
{
    auto& stream = level == LogLevel::Error ? std::cerr : std::cout;
    stream << '[' << ToText(level) << "][" << module << "] " << message << '\n';
}

void LogSdlError(std::string_view module, std::string_view action)
{
    std::string message;
    message.reserve(action.size() + 32);
    message.append(action);
    message.append(" 失败：");
    message.append(SDL_GetError());
    Log(LogLevel::Error, module, message);
}

} // namespace cg::core
