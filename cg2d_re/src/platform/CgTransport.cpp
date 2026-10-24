// GetAddrInfoExW的可取消异步路径要求Windows 8及以上，仅本实现声明SDK版本。
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#include "cg/platform/CgTransport.hpp"
#include "cg/platform/MainLoop.hpp"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>

#include "ServerAddressPolicy.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>

namespace cg::platform {

struct CgTransport::Impl {
    struct Event {
        WSAEVENT value {WSA_INVALID_EVENT};
        ~Event() { Close(); }
        bool Create() { value = WSACreateEvent(); return value != WSA_INVALID_EVENT; }
        void Close() { if (value != WSA_INVALID_EVENT) WSACloseEvent(std::exchange(value, WSA_INVALID_EVENT)); }
    };
    struct Socket {
        SOCKET value {INVALID_SOCKET};
        ~Socket() { if (value != INVALID_SOCKET) closesocket(value); }
    };
    struct Addresses {
        PADDRINFOEXW value = nullptr;
        ~Addresses() { if (value) FreeAddrInfoExW(value); }
    };
    struct Request {
        ConnectionId id;
        std::wstring host;
        std::uint16_t port;
        bool noDelay;
        std::wstring policyQuery;
    };

    bool winsock = false;
    Event control; // 手动复位；锁内复位/发布，避免丢失唤醒。
    std::thread worker;
    mutable std::mutex mutex;
    bool stopping = false, cancelled = false, ioEnabled = false, policyApproved = false;
    std::array<std::string, 3> policyAddresses; // 三路各发布一次，当前待验地址始终位于队首。
    std::size_t policyAddressCount = 0; // 仅在mutex内交接，拒绝一路不丢弃其他已就绪结果。
    std::optional<Request> pending; // 只保留最新一次连接请求，不建立无界任务队列。
    ConnectionSnapshot status;
    std::array<std::uint8_t, kQueueCapacity> sendBytes {}, receiveBytes {};
    std::size_t sendLength = 0, receiveLength = 0;

    ~Impl()
    {
        Stop();
        control.Close();
        if (winsock) WSACleanup();
    }

    void Stop()
    {
        {
            std::lock_guard lock(mutex);
            stopping = true;
            if (control.value != WSA_INVALID_EVENT) WSASetEvent(control.value);
        }
        if (worker.joinable()) worker.join();
    }

    bool Cancelled(ConnectionId id)
    {
        std::lock_guard lock(mutex);
        return stopping || cancelled || status.id != id;
    }

    // 仅工作线程消费control；新连接/取消在复位后写入会再次置位。
    bool ConsumeControl(ConnectionId id)
    {
        std::lock_guard lock(mutex);
        WSAResetEvent(control.value);
        return stopping || cancelled || status.id != id;
    }

    void Publish(ConnectionId id, ConnectionState state, ConnectionPhase phase, int error = 0)
    {
        std::lock_guard lock(mutex);
        if (status.id != id || stopping || cancelled) return;
        status.state = state;
        status.phase = phase;
        status.error = error;
        if (state == ConnectionState::Failed || state == ConnectionState::Closed) sendLength = 0;
    }

    // 返回时OVERLAPPED已完成，结果与取消句柄不会引用失效栈。
    int Resolve(ConnectionId id, const std::wstring& host, const wchar_t* service,
        int family, int flags, Addresses& addresses)
    {
        Event completed;
        if (!completed.Create()) return WSAGetLastError();
        OVERLAPPED overlapped {};
        overlapped.hEvent = completed.value;
        HANDLE cancellation = nullptr;
        ADDRINFOEXW hints {};
        hints.ai_family = family;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = flags;
        int result = GetAddrInfoExW(host.c_str(), service, NS_DNS, nullptr, &hints,
            &addresses.value, nullptr, &overlapped, nullptr, &cancellation);
        if (result != WSA_IO_PENDING) return result;
        const WSAEVENT events[] {control.value, completed.value};
        for (;;) {
            const DWORD waited = WSAWaitForMultipleEvents(2, events, FALSE, WSA_INFINITE, FALSE);
            if (waited == WSA_WAIT_EVENT_0 + 1) return GetAddrInfoExOverlappedResult(&overlapped);
            const int waitError = waited == WSA_WAIT_FAILED ? WSAGetLastError() : 0;
            if (waitError || ConsumeControl(id)) {
                if (cancellation) GetAddrInfoExCancel(&cancellation);
                // 取消返回也不能先销毁异步上下文，完成事件是释放边界。
                WaitForSingleObject(completed.value, INFINITE);
                GetAddrInfoExOverlappedResult(&overlapped);
                return waitError ? waitError : WSA_OPERATION_ABORTED;
            }
        }
    }

    bool RunPolicy(const Request& request)
    {
        if (request.policyQuery.empty()) return true;
        auto result = QueryServerAddressPolicy(request.policyQuery, control.value,
            [this, id = request.id] { return ConsumeControl(id); },
            [this, id = request.id](std::string address) {
                std::lock_guard lock(mutex);
                if (status.id != id || stopping || cancelled || policyApproved) return;
                policyAddresses[policyAddressCount++] = std::move(address);
                if (policyAddressCount == 1) {
                    status.policyAddress = policyAddresses[0];
                    status.state = ConnectionState::PolicyReady;
                    status.phase = ConnectionPhase::Policy;
                }
            },
            [this, id = request.id] {
                std::lock_guard lock(mutex);
                return status.id == id && !stopping && !cancelled && policyApproved;
            });
        if (result.cancelled || Cancelled(request.id)) return false;
        if (result.approved) return true;
        // 查询截止只结束未完成的线路；已就绪结果仍须在原主线程校验点完成交接。
        for (;;) {
            {
                std::lock_guard lock(mutex);
                if (status.id != request.id || stopping || cancelled) return false;
                if (policyApproved) return true;
                if (status.state != ConnectionState::PolicyReady) {
                    status.state = ConnectionState::Failed;
                    status.phase = ConnectionPhase::Policy;
                    status.error = ERROR_ACCESS_DENIED;
                    sendLength = 0;
                    return false;
                }
            }
            const DWORD waited = WSAWaitForMultipleEvents(1, &control.value, FALSE, WSA_INFINITE, FALSE);
            if (waited == WSA_WAIT_FAILED) {
                Publish(request.id, ConnectionState::Failed, ConnectionPhase::Policy, WSAGetLastError());
                return false;
            }
            if (ConsumeControl(request.id)) return false;
        }
    }

    static std::wstring Endpoint(const sockaddr* address, int length)
    {
        wchar_t host[NI_MAXHOST] {}, service[NI_MAXSERV] {};
        if (GetNameInfoW(address, length, host, NI_MAXHOST, service, NI_MAXSERV,
                NI_NUMERICHOST | NI_NUMERICSERV) != 0) return L"";
        return address->sa_family == AF_INET6
            ? L"[" + std::wstring(host) + L"]:" + service
            : std::wstring(host) + L":" + service;
    }

    // 成功只来自connect返回0或FD_CONNECT错误码0，候选事件随Socket独占销毁。
    int Connect(const Request& request, SOCKET socket, WSAEVENT event, const sockaddr* address, int length,
        ConnectionPhase& phase)
    {
        phase = ConnectionPhase::ConnectStart;
        if (::connect(socket, address, length) == 0) return 0;
        const int error = WSAGetLastError();
        if (error != WSAEWOULDBLOCK) return error;
        phase = ConnectionPhase::Connect;
        const WSAEVENT events[] {control.value, event};
        for (;;) {
            const DWORD waited = WSAWaitForMultipleEvents(2, events, FALSE, WSA_INFINITE, FALSE);
            if (waited == WSA_WAIT_FAILED) return WSAGetLastError();
            if (waited == WSA_WAIT_EVENT_0) {
                if (ConsumeControl(request.id)) return WSA_OPERATION_ABORTED;
                continue;
            }
            WSANETWORKEVENTS networkEvents {};
            if (WSAEnumNetworkEvents(socket, event, &networkEvents) != 0) return WSAGetLastError();
            if (networkEvents.lNetworkEvents & FD_CONNECT) return networkEvents.iErrorCode[FD_CONNECT_BIT];
            if (networkEvents.lNetworkEvents & FD_CLOSE) {
                const int closeError = networkEvents.iErrorCode[FD_CLOSE_BIT];
                return closeError ? closeError : WSAECONNRESET;
            }
        }
    }

    void Transfer(const Request& request, SOCKET socket, WSAEVENT event)
    {
        bool peerClosing = false;
        int closeError = 0;
        const WSAEVENT events[] {control.value, event};
        for (;;) {
            if (Cancelled(request.id)) return;
            // 每轮最多消费初始容量；每次系统调用前重查取消/代次/门禁。
            for (std::size_t budget = kQueueCapacity; budget != 0;) {
                std::lock_guard lock(mutex);
                if (stopping || cancelled || status.id != request.id) return;
                if (!ioEnabled || receiveLength == receiveBytes.size()) break;
                const auto capacity = (std::min)(budget, receiveBytes.size() - receiveLength);
                const int count = recv(socket, reinterpret_cast<char*>(receiveBytes.data() + receiveLength),
                    static_cast<int>(capacity), 0);
                if (count > 0) {
                    receiveLength += static_cast<std::size_t>(count);
                    budget -= static_cast<std::size_t>(count);
                    continue;
                }
                const int error = count == 0 ? closeError : WSAGetLastError();
                if (count < 0 && error == WSAEWOULDBLOCK && !peerClosing) break;
                status.error = error == WSAEWOULDBLOCK ? closeError : error;
                status.phase = ConnectionPhase::Receive;
                status.state = status.error ? ConnectionState::Failed : ConnectionState::Closed;
                sendLength = 0;
                return; // 已接收字节留在队列，主线程排空后才消费终止。
            }
            for (std::size_t budget = kQueueCapacity; budget != 0;) {
                std::lock_guard lock(mutex);
                if (stopping || cancelled || status.id != request.id) return;
                if (!ioEnabled || sendLength == 0 || peerClosing) break;
                const int count = send(socket, reinterpret_cast<const char*>(sendBytes.data()),
                    static_cast<int>((std::min)(budget, sendLength)), 0);
                if (count > 0) {
                    const auto sent = static_cast<std::size_t>(count);
                    sendLength -= sent;
                    std::memmove(sendBytes.data(), sendBytes.data() + sent, sendLength);
                    budget -= sent;
                    status.lastSendTick = ReadTickCount64();
                    continue;
                }
                const int error = count == 0 ? WSAECONNABORTED : WSAGetLastError();
                if (error == WSAEWOULDBLOCK) break;
                status.state = ConnectionState::Failed;
                status.phase = ConnectionPhase::Send;
                status.error = error;
                sendLength = 0;
                return;
            }
            const DWORD waited = WSAWaitForMultipleEvents(2, events, FALSE, WSA_INFINITE, FALSE);
            if (waited == WSA_WAIT_FAILED) {
                Publish(request.id, ConnectionState::Failed, ConnectionPhase::Receive, WSAGetLastError());
                return;
            }
            if (waited == WSA_WAIT_EVENT_0) {
                if (ConsumeControl(request.id)) return;
            } else {
                WSANETWORKEVENTS networkEvents {};
                if (WSAEnumNetworkEvents(socket, event, &networkEvents) != 0) {
                    Publish(request.id, ConnectionState::Failed, ConnectionPhase::Receive, WSAGetLastError());
                    return;
                }
                if (networkEvents.lNetworkEvents & FD_CLOSE) {
                    peerClosing = true;
                    closeError = networkEvents.iErrorCode[FD_CLOSE_BIT];
                }
                for (const auto bit : {FD_READ_BIT, FD_WRITE_BIT}) {
                    if ((networkEvents.lNetworkEvents & (1L << bit)) && networkEvents.iErrorCode[bit]) {
                        Publish(request.id, ConnectionState::Failed,
                            bit == FD_READ_BIT ? ConnectionPhase::Receive : ConnectionPhase::Send,
                            networkEvents.iErrorCode[bit]);
                        return;
                    }
                }
            }
        }
    }

    void Session(const Request& request)
    {
        if (Cancelled(request.id)) return;
        if (!RunPolicy(request) || Cancelled(request.id)) return;
        Publish(request.id, ConnectionState::Resolving, ConnectionPhase::Resolve);
        Addresses addresses;
        ADDRINFOEXW numeric {};
        sockaddr_in ipv4 {};
        // 保留旧IPv4缩写路径；IPv6及域名交由宽字符解析器，互不误用。
        const bool ascii = std::all_of(request.host.begin(), request.host.end(), [](wchar_t c) { return c <= 127; });
        std::string bytes;
        if (ascii) {
            bytes.reserve(request.host.size());
            for (const auto character : request.host) bytes.push_back(static_cast<char>(character));
        }
#pragma warning(suppress : 4996)
        ipv4.sin_addr.s_addr = inet_addr(bytes.c_str());
        if (ipv4.sin_addr.s_addr != INADDR_NONE) {
            ipv4.sin_family = AF_INET;
            ipv4.sin_port = htons(request.port);
            numeric.ai_family = AF_INET;
            numeric.ai_socktype = SOCK_STREAM;
            numeric.ai_protocol = IPPROTO_TCP;
            numeric.ai_addr = reinterpret_cast<sockaddr*>(&ipv4);
            numeric.ai_addrlen = sizeof(ipv4);
        } else {
            const bool ipv6Literal = request.host.find(L':') != std::wstring::npos;
            if (ipv6Literal) {
                const auto scope = request.host.find(L'%');
                if (scope != std::wstring::npos && (scope + 1 == request.host.size()
                    || request.host.find_first_not_of(L"0123456789", scope + 1) != std::wstring::npos)) {
                    Publish(request.id, ConnectionState::Failed, ConnectionPhase::Resolve, WSAEINVAL);
                    return;
                }
            }
            const auto service = std::to_wstring(request.port);
            const int error = Resolve(request.id, request.host, service.c_str(), AF_UNSPEC,
                AI_NUMERICSERV | (ipv6Literal ? AI_NUMERICHOST : 0), addresses);
            if (Cancelled(request.id)) return;
            if (error) {
                Publish(request.id, ConnectionState::Failed, ConnectionPhase::Resolve, error);
                return;
            }
        }
        int lastError = WSAEAFNOSUPPORT;
        auto lastPhase = ConnectionPhase::ConnectStart;
        for (auto* entry = numeric.ai_addr ? &numeric : addresses.value; entry; entry = entry->ai_next) {
            if (Cancelled(request.id)) return;
            if (entry->ai_family != AF_INET && entry->ai_family != AF_INET6) continue;
            if (entry->ai_family == AF_INET6) {
                const auto* address = reinterpret_cast<const sockaddr_in6*>(entry->ai_addr);
                if (IN6_IS_ADDR_LINKLOCAL(&address->sin6_addr) && address->sin6_scope_id == 0) {
                    lastError = WSAEINVAL;
                    lastPhase = ConnectionPhase::Resolve;
                    continue;
                }
            }
            {
                auto endpoint = Endpoint(entry->ai_addr, static_cast<int>(entry->ai_addrlen));
                std::lock_guard lock(mutex);
                if (status.id != request.id || cancelled || stopping) return;
                status.state = ConnectionState::Connecting;
                status.phase = ConnectionPhase::Connect;
                status.endpoint = std::move(endpoint);
                ++status.attempt;
            }
            Event event;
            Socket socket; // 先关闭Socket，再销毁与它关联的事件。
            socket.value = ::socket(entry->ai_family, SOCK_STREAM, IPPROTO_TCP);
            if (socket.value == INVALID_SOCKET) {
                lastError = WSAGetLastError(); lastPhase = ConnectionPhase::Setup; continue;
            }
            if (!event.Create() || WSAEventSelect(socket.value, event.value,
                    FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) != 0) {
                Publish(request.id, ConnectionState::Failed, ConnectionPhase::Setup, WSAGetLastError());
                return;
            }
            if (request.noDelay) {
                const int enabled = 1;
                if (setsockopt(socket.value, IPPROTO_TCP, TCP_NODELAY,
                        reinterpret_cast<const char*>(&enabled), sizeof(enabled)) != 0) {
                    Publish(request.id, ConnectionState::Failed, ConnectionPhase::SocketOptions, WSAGetLastError());
                    return;
                }
            }
            lastError = Connect(request, socket.value, event.value, entry->ai_addr,
                static_cast<int>(entry->ai_addrlen), lastPhase);
            if (Cancelled(request.id)) return;
            if (lastError) continue;
            Publish(request.id, ConnectionState::Connected, ConnectionPhase::Connect);
            Transfer(request, socket.value, event.value);
            return; // 已建立的连接断开不再遍历候选，不重复登录。
        }
        Publish(request.id, ConnectionState::Failed, lastPhase, lastError);
    }

    void Run()
    {
        for (;;) {
            std::optional<Request> request;
            {
                std::lock_guard lock(mutex);
                WSAResetEvent(control.value);
                if (stopping) return;
                request.swap(pending);
            }
            if (!request) {
                const DWORD waited = WSAWaitForMultipleEvents(1, &control.value, FALSE, WSA_INFINITE, FALSE);
                if (waited == WSA_WAIT_FAILED) {
                    std::lock_guard lock(mutex);
                    status.state = ConnectionState::Failed;
                    status.phase = ConnectionPhase::Setup;
                    status.error = WSAGetLastError();
                    stopping = true;
                    return;
                }
                continue;
            }
            try { Session(*request); }
            catch (...) { Publish(request->id, ConnectionState::Failed, ConnectionPhase::Setup, WSAENOBUFS); }
            {
                std::lock_guard lock(mutex);
                if (status.id == request->id && cancelled) {
                    status.state = ConnectionState::Closed;
                    status.phase = ConnectionPhase::Cancel;
                    sendLength = receiveLength = 0;
                }
            }
        }
    }
};

CgTransport::CgTransport() noexcept = default;
CgTransport::~CgTransport() = default;

bool CgTransport::Open(int& error)
{
    error = 0;
    if (impl_) return true;
    try {
        auto pending = std::make_unique<Impl>();
        WSADATA data {};
        error = WSAStartup(MAKEWORD(2, 2), &data);
        if (error) return false;
        pending->winsock = true;
        if (!pending->control.Create()) { error = WSAGetLastError(); return false; }
        pending->worker = std::thread([self = pending.get()] { self->Run(); });
        impl_ = std::move(pending);
        return true;
    } catch (...) { error = WSAENOBUFS; return false; }
}

ConnectionId CgTransport::BeginConnect(std::wstring host, std::uint16_t port,
    bool noDelay, std::wstring policyQuery)
{
    if (!impl_ || nextId_ == (std::numeric_limits<ConnectionId>::max)()) return 0;
    std::lock_guard lock(impl_->mutex);
    if (impl_->stopping) return 0;
    const auto id = nextId_++;
    ConnectionSnapshot next;
    next.id = id;
    next.host = host;
    next.state = ConnectionState::Resolving;
    next.phase = policyQuery.empty() ? ConnectionPhase::Resolve : ConnectionPhase::Policy;
    impl_->pending.emplace(Impl::Request {id, std::move(host), port, noDelay, std::move(policyQuery)});
    impl_->status = std::move(next);
    impl_->cancelled = impl_->ioEnabled = impl_->policyApproved = false;
    impl_->policyAddresses = {};
    impl_->policyAddressCount = 0;
    impl_->sendLength = impl_->receiveLength = 0;
    WSASetEvent(impl_->control.value);
    return id;
}

ConnectionSnapshot CgTransport::PollConnection(ConnectionId id) const
{
    if (!impl_) return {};
    std::lock_guard lock(impl_->mutex);
    if (id == 0 || impl_->status.id != id) return {};
    auto result = impl_->status;
    result.receivedBytes = impl_->receiveLength;
    return result;
}

void CgTransport::ContinueAfterPolicy(ConnectionId id, bool approved)
{
    if (!impl_) return;
    std::lock_guard lock(impl_->mutex);
    if (impl_->status.id != id || impl_->status.state != ConnectionState::PolicyReady
        || impl_->stopping || impl_->cancelled) return;
    impl_->policyApproved = approved;
    if (approved) {
        impl_->policyAddressCount = 0;
    } else {
        for (std::size_t index = 1; index < impl_->policyAddressCount; ++index)
            impl_->policyAddresses[index - 1] = std::move(impl_->policyAddresses[index]);
        --impl_->policyAddressCount;
    }
    if (impl_->policyAddressCount != 0) {
        impl_->status.policyAddress = impl_->policyAddresses[0];
    } else {
        impl_->status.policyAddress.clear();
        impl_->status.state = ConnectionState::Resolving;
        impl_->status.phase = approved ? ConnectionPhase::Resolve : ConnectionPhase::Policy;
    }
    WSASetEvent(impl_->control.value);
}

void CgTransport::SetIoEnabled(ConnectionId id, bool enabled)
{
    if (!impl_) return;
    std::lock_guard lock(impl_->mutex);
    if (impl_->status.id != id || impl_->ioEnabled == enabled) return;
    impl_->ioEnabled = enabled;
    WSASetEvent(impl_->control.value);
}

NetworkQueueResult CgTransport::TryQueueSend(ConnectionId id,
    std::span<const std::uint8_t> bytes, bool replace)
{
    if (!impl_) return NetworkQueueResult::Stopping;
    std::lock_guard lock(impl_->mutex);
    if (impl_->stopping || impl_->cancelled) return NetworkQueueResult::Stopping;
    if (id == 0 || impl_->status.id != id) return NetworkQueueResult::StaleSession;
    if (impl_->status.state != ConnectionState::Connected || !impl_->ioEnabled) return NetworkQueueResult::NotConnected;
    const std::size_t previous = replace ? 0 : impl_->sendLength;
    if (bytes.size() > kQueueCapacity - previous) return NetworkQueueResult::QueueFull;
    auto* destination = impl_->sendBytes.data() + previous;
    if (!bytes.empty()) std::memcpy(destination, bytes.data(), bytes.size());
    impl_->sendLength = previous + bytes.size();
    WSASetEvent(impl_->control.value);
    return NetworkQueueResult::Accepted;
}

std::size_t CgTransport::DrainReceived(ConnectionId id, std::span<std::uint8_t> destination)
{
    if (!impl_) return 0;
    std::lock_guard lock(impl_->mutex);
    if (impl_->status.id != id || impl_->cancelled) return 0;
    const auto count = (std::min)(destination.size(), impl_->receiveLength);
    if (count) {
        std::memcpy(destination.data(), impl_->receiveBytes.data(), count);
        impl_->receiveLength -= count;
        std::memmove(impl_->receiveBytes.data(), impl_->receiveBytes.data() + count, impl_->receiveLength);
        WSASetEvent(impl_->control.value);
    }
    return count;
}

void CgTransport::CloseConnection(ConnectionId id)
{
    if (!impl_) return;
    std::lock_guard lock(impl_->mutex);
    if (id == 0 || impl_->status.id != id) return;
    if (impl_->status.state == ConnectionState::Closed || impl_->status.state == ConnectionState::Failed) return;
    impl_->cancelled = true;
    impl_->status.state = ConnectionState::Closing;
    impl_->sendLength = impl_->receiveLength = 0;
    WSASetEvent(impl_->control.value);
}

void CgTransport::Stop() { impl_.reset(); }

} // namespace cg::platform
