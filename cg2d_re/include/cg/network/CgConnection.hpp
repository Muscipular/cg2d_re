#pragma once

#include "cg/platform/CgTransport.hpp"

#include <functional>
#include <memory>

namespace cg::network {

// 正文/参数借用到同步回调结束；注册表随连接对象保留，重连不清空。
using ProtocolReceiveHandler = std::function<void(const char* header, char** arguments)>;
using ProtocolSendHandler = std::function<void(
    const char* header, const char* data, std::int32_t length)>;

// Pump只报告终止，不在连接对象中切换场景；terminal仅在terminated为true时有效。
struct ConnectionPumpResult {
    std::uint8_t value {};
    bool terminated {};
    platform::ConnectionSnapshot terminal;
};

namespace detail { struct ConnectionLegacyAccess; }

// CTO01：主线程协议与会话owner，组合私有CgTransport；当前业务仅接入一个实例。
class CgConnection final {
public:
    CgConnection();
    ~CgConnection();
    CgConnection(const CgConnection&) = delete;
    CgConnection& operator=(const CgConnection&) = delete;
    CgConnection(CgConnection&&) = delete;
    CgConnection& operator=(CgConnection&&) = delete;

    [[nodiscard]] bool Initialize();
    void Shutdown();
    [[nodiscard]] platform::ConnectionId BeginConnect(std::wstring host, std::uint16_t port, bool noDelay);
    [[nodiscard]] platform::ConnectionSnapshot PollConnection();
    [[nodiscard]] platform::ConnectionId Id() const noexcept;
    [[nodiscard]] bool Active() const noexcept;
    [[nodiscard]] std::int32_t Error() const noexcept;
    void SetIoEnabled(bool enabled);
    // 输入为完整明文消息；不添加LF。expectedId用于拒绝重入后过期的发送。
    [[nodiscard]] std::int32_t SendPacket(platform::ConnectionId expectedId,
        const void* data, std::uint32_t size);
    [[nodiscard]] ConnectionPumpResult Pump();
    void RegisterReceive(const char* header, ProtocolReceiveHandler handler);
    void RegisterSend(const char* header, ProtocolSendHandler handler);
    void Close();
    void StopIo(); // Lua/UI关闭前调用；不释放主线程协议工作区。

private:
    friend struct detail::ConnectionLegacyAccess;
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cg::network
