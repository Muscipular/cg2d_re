#pragma once

namespace cg::platform {

// 真实数组二次C++异常消费；由MSVC terminate负责handler/abort与最终终止。
// 原函数：0x00540A5C。
[[noreturn]] void TerminateLegacyRuntime_540A5C();

} // namespace cg::platform
