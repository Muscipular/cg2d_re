#pragma once

#include "cg/network/CgConnection.hpp"
#include "cg/world/WorldSceneRuntimeState.hpp"

#include <cstdint>
#include <random>

namespace cg::session {

using LegacyRandomEngine =
    std::linear_congruential_engine<std::uint32_t, 0x343FDu, 0x269EC3u, 0>;

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
    [[nodiscard]] LegacyRandomEngine& RandomEngine() noexcept { return randomEngine_; }

private:
    network::CgConnection connection_;
    world::WorldSceneRuntimeState worldScene_;
    LegacyRandomEngine randomEngine_ {1}; // 原 0x58F870，初始种子 1。
};

} // namespace cg::session
