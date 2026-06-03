#include "platform/Console.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace cg::platform {

void ConfigureConsoleUtf8() noexcept
{
#ifdef _WIN32
    static_cast<void>(SetConsoleOutputCP(CP_UTF8));
    static_cast<void>(SetConsoleCP(CP_UTF8));
#endif
}

} // namespace cg::platform
