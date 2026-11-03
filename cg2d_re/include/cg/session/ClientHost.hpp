#pragma once

#include "cg/session/ClientSession.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <list>
#include <string>
#include <vector>

namespace cg::session {

// 进程级宿主；当前仅调度首会话，其他 owner 闭合后才接入新增/关闭业务。
class ClientHost final {
public:
    struct MenuEntry {
        std::uint64_t id;
        std::wstring label;
        bool selected;
        bool disconnected;
    };

    struct SessionEntry {
        explicit SessionEntry(std::uint64_t value) : id(value) {}
        const std::uint64_t id; // 菜单项与关闭请求的稳定身份，不是账号或角色编号。
        ClientSession session;
    };

    ClientHost()
    {
        sessions_.emplace_back(1);
        current_ = sessions_.begin();
    }
    ~ClientHost() = default;
    ClientHost(const ClientHost&) = delete;
    ClientHost& operator=(const ClientHost&) = delete;
    ClientHost(ClientHost&&) = delete;
    ClientHost& operator=(ClientHost&&) = delete;

    [[nodiscard]] ClientSession& FirstSession() noexcept { return sessions_.front().session; }
    [[nodiscard]] ClientSession* CurrentSession() noexcept
    {
        return current_ == sessions_.end() ? nullptr : &current_->session;
    }
    [[nodiscard]] std::uint64_t CurrentId() const noexcept
    {
        return current_ == sessions_.end() ? 0 : current_->id;
    }
    [[nodiscard]] std::size_t SessionCount() const noexcept { return sessions_.size(); }
    [[nodiscard]] const std::list<SessionEntry>& Sessions() const noexcept { return sessions_; }
    [[nodiscard]] std::vector<MenuEntry> MenuEntries();

    // 只创建稳定对象；初始化成功后调用方再选中，失败不改变原当前会话。
    [[nodiscard]] SessionEntry& AddSession()
    {
        sessions_.emplace_back(nextId_);
        try {
            auto& target = sessions_.back().session.WorldScene().slideTransition.lines;
            const auto extent = sessions_.front().session.WorldScene().slideTransition.lines.speed.size();
            target.speed.resize(extent);
            target.x.resize(extent);
            target.y.resize(extent);
        } catch (...) {
            sessions_.pop_back();
            throw;
        }
        ++nextId_;
        return sessions_.back();
    }

    [[nodiscard]] bool SelectSession(const std::uint64_t id) noexcept
    {
        for (auto it = sessions_.begin(); it != sessions_.end(); ++it) {
            if (it->id == id) {
                current_ = it;
                return true;
            }
        }
        return false;
    }

    // 仅移除已保存、断线且完成会话资源清理的对象；当前项优先切向下一项。
    [[nodiscard]] bool RemoveSession(const std::uint64_t id) noexcept
    {
        for (auto it = sessions_.begin(); it != sessions_.end(); ++it) {
            if (it->id != id) continue;
            if (it == current_) {
                const auto next = std::next(it);
                current_ = next != sessions_.end() ? next
                    : (it == sessions_.begin() ? sessions_.end() : std::prev(it));
            }
            sessions_.erase(it);
            return true;
        }
        return false;
    }

private:
    std::list<SessionEntry> sessions_;
    std::list<SessionEntry>::iterator current_;
    std::uint64_t nextId_ = 2;
};

[[nodiscard]] ClientHost& GetClientHost() noexcept;

} // namespace cg::session
