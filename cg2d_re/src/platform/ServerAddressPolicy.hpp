#pragma once

#include <Windows.h>

#include <functional>
#include <string>

namespace cg::platform {

// SDP01：仅平台内部使用；查询结果按值返回，回调不得访问连接或UI对象。
struct ServerAddressPolicyResult {
    bool approved = false;
    bool cancelled = false;
};

// 在传输工作线程并行等待系统DNS及两家DoH；control唤醒后消费当前会话取消状态。
ServerAddressPolicyResult QueryServerAddressPolicy(
    const std::wstring& name, HANDLE control, const std::function<bool()>& consumeControl);

} // namespace cg::platform
