#pragma once

#include <string_view>

namespace cg::core {

enum class LogLevel {
    Info,
    Warning,
    Error,
};

void Log(LogLevel level, std::string_view module, std::string_view message);
void LogSdlError(std::string_view module, std::string_view action);

} // namespace cg::core
