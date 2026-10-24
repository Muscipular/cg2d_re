#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>

namespace cg::platform {

using ConnectionId = std::uint64_t; // NTM01：进程内不复用，0表示无会话。
enum class ConnectionState { Idle, Resolving, PolicyReady, Connecting, Connected, Closing, Closed, Failed };
enum class ConnectionPhase { None, Setup, SocketOptions, Policy, Resolve, ConnectStart, Connect, Receive, Send, Cancel };
enum class NetworkQueueResult { Accepted, QueueFull, NotConnected, StaleSession, Stopping };

// 跨线程只发布值；策略结果由主线程裁决，原生地址与句柄不越过平台边界。
struct ConnectionSnapshot {
    ConnectionId id = 0;
    ConnectionState state {ConnectionState::Idle};
    ConnectionPhase phase {ConnectionPhase::None};
    int error = 0;
    std::uint64_t attempt = 0; // 同一域名的候选序号。
    std::uint64_t lastSendTick = 0; // 与ReadTickCount64同源的实际send成功时点。
    std::size_t receivedBytes = 0;
    std::wstring host; // 地址策略映射后的host，用于连接失败提示。
    std::wstring endpoint; // IPv6带端口时包含方括号。
    std::string policyAddress; // 原策略查询首个IPv4结果，失败保持空串。
};

// 单工作线程协调Socket/DNS/DoH；系统异步回调只持有自身查询，主线程API只借短锁访问有界队列。
class CgTransport final {
public:
    static constexpr std::size_t kQueueCapacity = 0x4000;
    CgTransport() noexcept;
    ~CgTransport();
    CgTransport(const CgTransport&) = delete;
    CgTransport& operator=(const CgTransport&) = delete;
    CgTransport(CgTransport&&) = delete;
    CgTransport& operator=(CgTransport&&) = delete;

    [[nodiscard]] bool Open(int& error);
    [[nodiscard]] ConnectionId BeginConnect(std::wstring host, std::uint16_t port,
        bool noDelay, std::wstring policyQuery);
    [[nodiscard]] ConnectionSnapshot PollConnection(ConnectionId id) const;
    // 主线程交回当前地址的校验结果；未通过时继续等待其余查询。
    void ContinueAfterPolicy(ConnectionId id, bool approved);
    void SetIoEnabled(ConnectionId id, bool enabled);
    // 只提交bytes；replace原子替换待发内容，普通追加保持字节顺序，不识别协议命令。
    [[nodiscard]] NetworkQueueResult TryQueueSend(ConnectionId id,
        std::span<const std::uint8_t> bytes, bool replace);
    [[nodiscard]] std::size_t DrainReceived(ConnectionId id, std::span<std::uint8_t> destination);
    void CloseConnection(ConnectionId id);
    void Stop(); // 唤醒、取消、join，然后释放事件及Winsock引用。

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    ConnectionId nextId_ {1};
};

} // namespace cg::platform
