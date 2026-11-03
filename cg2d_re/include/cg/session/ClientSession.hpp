#pragma once

#include "cg/network/CgConnection.hpp"

namespace cg::session {

// 一个账号会话的稳定 owner；当前只承载已完成实例化的网络/登录状态。
class ClientSession final {
public:
    ClientSession() = default;
    ~ClientSession() = default;
    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;
    ClientSession(ClientSession&&) = delete;
    ClientSession& operator=(ClientSession&&) = delete;

    [[nodiscard]] network::CgConnection& Connection() noexcept { return connection_; }

private:
    network::CgConnection connection_;
};

} // namespace cg::session
