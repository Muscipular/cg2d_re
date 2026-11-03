#pragma once

#include "cg/session/ClientSession.hpp"

namespace cg::session {

// 进程级宿主；当前只持有可安全启动的首会话，其他 owner 闭合后才开放新增。
class ClientHost final {
public:
    ClientHost() = default;
    ~ClientHost() = default;
    ClientHost(const ClientHost&) = delete;
    ClientHost& operator=(const ClientHost&) = delete;
    ClientHost(ClientHost&&) = delete;
    ClientHost& operator=(ClientHost&&) = delete;

    [[nodiscard]] ClientSession& FirstSession() noexcept { return firstSession_; }

private:
    ClientSession firstSession_;
};

[[nodiscard]] ClientHost& GetClientHost() noexcept;

} // namespace cg::session
