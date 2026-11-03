#pragma once

#include "cg/network/CgConnection.hpp"
#include "cg/world/WorldSceneRuntimeState.hpp"

namespace cg::session {

// 一个账号会话的稳定 owner；当前只承载已完成实例化的网络/登录状态。
class ClientSession final {
public:
    ClientSession() : connection_(*this) {}
    ~ClientSession() = default;
    ClientSession(const ClientSession&) = delete;
    ClientSession& operator=(const ClientSession&) = delete;
    ClientSession(ClientSession&&) = delete;
    ClientSession& operator=(ClientSession&&) = delete;

    [[nodiscard]] network::CgConnection& Connection() noexcept { return connection_; }
    [[nodiscard]] world::WorldSceneRuntimeState& WorldScene() noexcept { return worldScene_; }

private:
    network::CgConnection connection_;
    world::WorldSceneRuntimeState worldScene_;
};

} // namespace cg::session
