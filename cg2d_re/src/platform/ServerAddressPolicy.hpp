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

// 并行查询只发布地址，由主线程原校验点裁决；control唤醒后检查取消及主线程通过结果。
ServerAddressPolicyResult QueryServerAddressPolicy(
    const std::wstring& name, HANDLE control, const std::function<bool()>& consumeControl,
    const std::function<void(std::string)>& publishAddress, const std::function<bool()>& isApproved);

} // namespace cg::platform
