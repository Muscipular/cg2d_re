#include "LegacyRuntimeError.hpp"

#include <cstdlib>
#include <exception>

namespace cg::platform {

// MC01-C3：致命分配/启动失败保255；诊断编号和旧消息/钩子只保历史。
[[noreturn]] void ExitLegacyRuntimeError_53DEAB(const std::int32_t /*errorCode*/)
{
    std::_Exit(255);
}

[[noreturn]] void FastLegacyRuntimeErrorExit_53DED0(const std::int32_t /*errorCode*/)
{
    std::_Exit(255);
}

// 实际数组展开的二次C++异常交真实MSVC handler/abort/信号机制。
[[noreturn]] void TerminateLegacyRuntime_540A5C()
{
    std::terminate();
}

} // namespace cg::platform
