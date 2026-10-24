#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace cg::platform {

// SDP01平台内部：按值持有查询名/回调，启动三路分离线程后立即返回，全部失败才退出进程。
void StartServerAddressPolicy(
    std::wstring name, std::function<bool(std::string_view)> isAccepted);

} // namespace cg::platform
