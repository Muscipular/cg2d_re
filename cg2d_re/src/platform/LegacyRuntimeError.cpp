#include "LegacyRuntimeError.hpp"

#include <exception>

namespace cg::platform {

// 实际数组展开的二次C++异常交真实MSVC handler/abort/信号机制。
// 原函数：0x00540A5C。
[[noreturn]] void TerminateLegacyRuntime_540A5C()
{
    std::terminate();
}

} // namespace cg::platform
