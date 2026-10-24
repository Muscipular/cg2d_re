#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace cg::platform {

// SDP01平台内部：在调用线程内并行请求、等待三路结果，并立即调用isAccepted校验各路地址。
[[nodiscard]] bool QueryServerAddressPolicy(
    const std::wstring& name, const std::function<bool(std::string_view)>& isAccepted);

} // namespace cg::platform
