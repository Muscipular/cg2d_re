#pragma once

#include <cstdint>

namespace cg::platform {

// MC01-C3/C4：保CommandOptions失败9/9/8；真实_Exit(255)不跑普通atexit。
[[noreturn]] void ExitLegacyRuntimeError_53DEAB(std::int32_t errorCode);
// 真实数组二次C++异常消费；由MSVC terminate负责handler/abort与最终终止。
[[noreturn]] void TerminateLegacyRuntime_540A5C();

} // namespace cg::platform
