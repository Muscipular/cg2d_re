#pragma once

#include "cg/Yobi/YobiSessionState.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/script/LuaSessionState.hpp"
#include "cg/ui/UiFrameRuntimeState.hpp"
#include "cg/world/WorldSceneRuntimeState.hpp"

#include <cstdint>
#include <ctime>
#include <random>

namespace cg::session {

using LegacyRandomEngine =
    std::linear_congruential_engine<std::uint32_t, 0x343FDu, 0x269EC3u, 0>;

// 一个账号会话的稳定 owner；尚未隔离的世界/UI/Yobi 状态不得并行调度。
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
    [[nodiscard]] yobi::YobiSessionState& Yobi() noexcept { return yobi_; }
    [[nodiscard]] LegacyRandomEngine& RandomEngine() noexcept { return randomEngine_; }
    [[nodiscard]] script::LuaSessionState& Lua() noexcept { return lua_; }
    [[nodiscard]] ui::UiFrameRuntimeState& UiFrame() noexcept { return uiFrame_; }
    [[nodiscard]] const ui::UiFrameRuntimeState& UiFrame() const noexcept { return uiFrame_; }
    // 原同线程CRT日历别名收敛为会话唯一对象；后续转换仍覆盖同会话的长期借用。
    [[nodiscard]] std::tm* LocalTime(const std::int64_t* timestamp) noexcept
    {
        const std::tm* const value = ::_localtime64(timestamp);
        if (value == nullptr) { return nullptr; }
        localTime_ = *value;
        return &localTime_;
    }
    [[nodiscard]] bool Disconnected() const noexcept { return disconnected_; }
    void MarkDisconnected() noexcept { disconnected_ = true; }
    void MarkConnected() noexcept { disconnected_ = false; }

private:
    network::CgConnection connection_;
    world::WorldSceneRuntimeState worldScene_;
    yobi::YobiSessionState yobi_;
    LegacyRandomEngine randomEngine_ {1}; // 原 0x58F870，初始种子 1。
    script::LuaSessionState lua_;
    ui::UiFrameRuntimeState uiFrame_;
    std::tm localTime_ {};
    bool disconnected_ = false; // 宿主菜单保留断线标识，不参与原连接阶段分支。
};

} // namespace cg::session
