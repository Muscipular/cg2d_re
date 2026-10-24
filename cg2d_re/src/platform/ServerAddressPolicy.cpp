#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windns.h>
#include <wininet.h>

#include "ServerAddressPolicy.hpp"
#include "cg/core/vm.h"

#include <array>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>
#include <span>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace cg::platform::policy_detail {

// 三路共用截止时间，不因收到部分响应或开始读取正文而延长。
static constexpr DWORD kQueryTimeoutMs = 10000;
static constexpr std::size_t kMaxDnsMessageBytes = 65535;
enum class Outcome { Pending, Resolved, Failed, TimedOut };

struct QueryState {
    HANDLE event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    Outcome outcome = Outcome::Pending; // 仅本路std线程读写。
    DWORD error = ERROR_SUCCESS;
    std::string address; // 本路首个IPv4结果，由本路线程立即调用固定校验。
    QueryState() { if (!event) { outcome = Outcome::Failed; error = GetLastError(); } }
    ~QueryState() { if (event) CloseHandle(event); }
    void Fail(DWORD value) { error = value; outcome = Outcome::Failed; }
};

struct Records {
    PDNS_RECORD value = nullptr;
    ~Records() { if (value) DnsRecordListFree(value, DnsFreeRecordList); }
};

// 只消费查询名或其CNAME链的Answer首个A记录，不接受附加区的无关地址。
static Outcome ExtractPolicyAddress(const std::wstring& query, const DNS_RECORDW* records, std::string& output)
{
    std::size_t count = 0;
    for (auto* record = records; record; record = record->pNext) ++count;
    const wchar_t* name = query.c_str();
    for (std::size_t hop = 0; hop <= count; ++hop) {
        const wchar_t* canonical = nullptr;
        for (auto* record = records; record; record = record->pNext) {
            if (record->Flags.S.Section != DnsSectionAnswer || !record->pName
                || !DnsNameCompare_W(record->pName, name)) continue;
            if (record->wType == DNS_TYPE_A) {
                IN_ADDR address {};
                address.s_addr = record->Data.A.IpAddress;
                char text[INET_ADDRSTRLEN] {};
                if (!inet_ntop(AF_INET, &address, text, sizeof(text))) return Outcome::Failed;
                output = text;
                return Outcome::Resolved;
            }
            if (record->wType == DNS_TYPE_CNAME) canonical = record->Data.CNAME.pNameHost;
        }
        if (!canonical) return Outcome::Failed;
        name = canonical;
    }
    return Outcome::Failed;
}

// 只解析Question名称以核对响应；资源记录由Windows DNS解析器处理。
static bool ReadQuestionName(std::span<const unsigned char> bytes, std::size_t& offset, std::string& name)
{
    std::size_t cursor = offset;
    bool jumped = false;
    for (std::size_t steps = 0; steps < bytes.size(); ++steps) {
        if (cursor >= bytes.size()) return false;
        const unsigned length = bytes[cursor++];
        if (length == 0) { if (!jumped) offset = cursor; return true; }
        if ((length & 0xC0u) == 0xC0u) {
            if (cursor >= bytes.size()) return false;
            const std::size_t target = ((length & 0x3Fu) << 8) | bytes[cursor++];
            if (!jumped) offset = cursor;
            jumped = true;
            cursor = target;
            continue;
        }
        if (length > 63 || length > bytes.size() - cursor || name.size() + length + 1 > 255) return false;
        name.push_back(static_cast<char>(length)); // 保留标签边界，避免标签内的点与分隔符混淆。
        for (unsigned index = 0; index < length; ++index) {
            const unsigned char value = bytes[cursor++];
            name.push_back(static_cast<char>(value >= 'A' && value <= 'Z' ? value + ('a' - 'A') : value));
        }
    }
    return false; // 压缩指针循环。
}

struct SystemDnsQuery final : QueryState, std::enable_shared_from_this<SystemDnsQuery> {
    std::wstring name;
    DNS_QUERY_REQUEST request {};
    DNS_QUERY_RESULT result {};
    DNS_QUERY_CANCEL cancellation {};
    std::atomic<bool> complete {false};
    bool pending = false;
    // DnsCancelQuery返回不等于回调结束；由最终回调释放自身引用。
    std::shared_ptr<SystemDnsQuery> callbackOwner;

    ~SystemDnsQuery() { if (result.pQueryRecords) DnsRecordListFree(result.pQueryRecords, DnsFreeRecordList); }

    static void WINAPI Completed(void* context, DNS_QUERY_RESULT*) noexcept
    {
        auto* self = static_cast<SystemDnsQuery*>(context);
        auto owner = std::move(self->callbackOwner);
        self->complete.store(true, std::memory_order_release);
        SetEvent(self->event);
    }

    void Start(const std::wstring& query)
    {
        if (outcome != Outcome::Pending) return;
        name = query;
        result.Version = DNS_QUERY_RESULTS_VERSION1;
        request.Version = DNS_QUERY_REQUEST_VERSION1;
        request.QueryName = name.c_str();
        request.QueryType = DNS_TYPE_A;
        request.QueryOptions = DNS_QUERY_STANDARD;
        request.pQueryContext = this;
        request.pQueryCompletionCallback = Completed;
        callbackOwner = shared_from_this();
        const DNS_STATUS status = DnsQueryEx(&request, &result, &cancellation);
        pending = status == DNS_REQUEST_PENDING;
        if (!pending) {
            callbackOwner.reset(); // 同步完成时系统不调用完成回调。
            if (status != ERROR_SUCCESS) result.QueryStatus = status;
            complete.store(true, std::memory_order_release);
            SetEvent(event);
        }
    }

    void Advance()
    {
        if (outcome != Outcome::Pending || !complete.load(std::memory_order_acquire)) return;
        if (result.QueryStatus != ERROR_SUCCESS) { Fail(result.QueryStatus); return; }
        outcome = ExtractPolicyAddress(name, reinterpret_cast<const DNS_RECORDW*>(result.pQueryRecords), address);
        if (outcome == Outcome::Failed) error = DNS_INFO_NO_RECORDS;
    }

    void Cancel() noexcept
    {
        if (pending && !complete.load(std::memory_order_acquire)) DnsCancelQuery(&cancellation);
    }
};

struct DohQuery final : QueryState, std::enable_shared_from_this<DohQuery> {
    HINTERNET session = nullptr, connection = nullptr, request = nullptr;
    std::wstring name;
    std::array<unsigned char, 512> question {};
    DWORD questionSize = 0;
    std::array<unsigned char, 4096> chunk {};
    DWORD bytesRead = 0;
    std::vector<unsigned char> response;
    std::atomic<DWORD> completionError {ERROR_SUCCESS};
    enum class Stage { Sending, Reading } stage = Stage::Sending;
    bool awaiting = false;
    // WinINet句柄关闭的最后通知负责释放；请求正文和读缓冲区因此不悬空。
    std::shared_ptr<DohQuery> callbackOwner;

    ~DohQuery() { Cancel(); }

    static void CALLBACK Status(HINTERNET, DWORD_PTR context, DWORD status, void* info, DWORD length) noexcept
    {
        auto* self = reinterpret_cast<DohQuery*>(context);
        if (!self) return;
        if (status == INTERNET_STATUS_HANDLE_CLOSING) {
            auto owner = std::move(self->callbackOwner);
            return;
        }
        if (status == INTERNET_STATUS_REQUEST_COMPLETE && length >= sizeof(INTERNET_ASYNC_RESULT)) {
            const auto* result = static_cast<const INTERNET_ASYNC_RESULT*>(info);
            self->completionError.store(result->dwError, std::memory_order_release);
            SetEvent(self->event);
        }
    }

    void Cancel() noexcept
    {
        if (request) InternetCloseHandle(std::exchange(request, nullptr));
        if (connection) InternetCloseHandle(std::exchange(connection, nullptr));
        if (session) InternetCloseHandle(std::exchange(session, nullptr));
    }

    void Start(const std::wstring& query, const wchar_t* host)
    {
        if (outcome != Outcome::Pending) return;
        name = query;
        questionSize = static_cast<DWORD>(question.size());
        if (!DnsWriteQuestionToBuffer_W(reinterpret_cast<DNS_MESSAGE_BUFFER*>(question.data()),
                &questionSize, name.c_str(), DNS_TYPE_A, 0, TRUE)) {
            Fail(DNS_ERROR_INVALID_NAME); return;
        }
        session = InternetOpenW(L"cg2d-re-policy", INTERNET_OPEN_TYPE_PRECONFIG,
            nullptr, nullptr, INTERNET_FLAG_ASYNC);
        if (!session) { Fail(GetLastError()); return; }
        DWORD timeout = kQueryTimeoutMs;
        for (const DWORD option : {INTERNET_OPTION_CONNECT_TIMEOUT, INTERNET_OPTION_SEND_TIMEOUT,
                INTERNET_OPTION_RECEIVE_TIMEOUT}) {
            if (!InternetSetOptionW(session, option, &timeout, sizeof(timeout))) {
                Fail(GetLastError()); return;
            }
        }
        connection = InternetConnectW(session, host, INTERNET_DEFAULT_HTTPS_PORT,
            nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
        if (!connection) { Fail(GetLastError()); return; }
        const wchar_t* accept[] {L"application/dns-message", nullptr};
        request = HttpOpenRequestW(connection, L"POST", L"/dns-query", nullptr, nullptr, accept,
            INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE
                | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_UI
                | INTERNET_FLAG_NO_AUTO_REDIRECT, reinterpret_cast<DWORD_PTR>(this));
        if (!request) { Fail(GetLastError()); return; }
        if (InternetSetStatusCallbackW(request, Status) == INTERNET_INVALID_STATUS_CALLBACK) {
            Fail(GetLastError()); return;
        }
        callbackOwner = shared_from_this();
        constexpr wchar_t headers[] = L"Content-Type: application/dns-message\r\nAccept: application/dns-message\r\n";
        if (!HttpSendRequestW(request, headers, static_cast<DWORD>(std::size(headers) - 1),
                question.data(), questionSize)) {
            const DWORD error = GetLastError();
            if (error != ERROR_IO_PENDING) { Fail(error); return; }
            awaiting = true;
        }
    }

    void FinishResponse()
    {
        if (response.size() < sizeof(DNS_HEADER)) { Fail(DNS_ERROR_BAD_PACKET); return; }
        auto* message = reinterpret_cast<DNS_MESSAGE_BUFFER*>(response.data());
        DNS_HEADER header;
        std::memcpy(&header, &message->MessageHead, sizeof(header));
        DNS_BYTE_FLIP_HEADER_COUNTS(&header);
        if (!header.IsResponse || header.Opcode != 0 || header.Truncation || header.Xid != 0
            || header.QuestionCount != 1 || header.ResponseCode != 0) {
            Fail(DNS_ERROR_BAD_PACKET); return;
        }
        std::size_t offset = sizeof(DNS_HEADER), expectedOffset = sizeof(DNS_HEADER);
        std::string receivedName, expectedName;
        if (!ReadQuestionName(response, offset, receivedName)
            || !ReadQuestionName(std::span(question.data(), questionSize), expectedOffset, expectedName)
            || receivedName != expectedName || offset + 4 > response.size()
            || response[offset] != 0 || response[offset + 1] != DNS_TYPE_A
            || response[offset + 2] != 0 || response[offset + 3] != DNS_CLASS_INTERNET) {
            Fail(DNS_ERROR_BAD_PACKET); return;
        }
        // DNS提取API要求仅头部计数转换为主机序，记录区继续保持网络序。
        std::memcpy(&message->MessageHead, &header, sizeof(header));
        Records records;
        const DNS_STATUS status = DnsExtractRecordsFromMessage_W(message,
            static_cast<WORD>(response.size()), &records.value);
        if (status != ERROR_SUCCESS) { Fail(status); return; }
        outcome = ExtractPolicyAddress(name, reinterpret_cast<const DNS_RECORDW*>(records.value), address);
        if (outcome == Outcome::Failed) error = DNS_INFO_NO_RECORDS;
    }

    void Advance()
    {
        if (outcome != Outcome::Pending) return;
        if (awaiting) {
            if (WaitForSingleObject(event, 0) != WAIT_OBJECT_0) return;
            awaiting = false;
            const DWORD error = completionError.load(std::memory_order_acquire);
            if (error != ERROR_SUCCESS) { Fail(error); return; }
            if (stage == Stage::Reading) {
                if (bytesRead == 0) { FinishResponse(); return; }
                if (response.size() + bytesRead > kMaxDnsMessageBytes) { Fail(ERROR_BUFFER_OVERFLOW); return; }
                response.insert(response.end(), chunk.begin(), chunk.begin() + bytesRead);
            }
        }
        if (stage == Stage::Sending) {
            DWORD status = 0, size = sizeof(status);
            if (!HttpQueryInfoW(request, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &size, nullptr)) {
                Fail(GetLastError()); return;
            }
            if (status != HTTP_STATUS_OK) { Fail(status); return; }
            wchar_t contentType[128] {};
            size = sizeof(contentType);
            if (!HttpQueryInfoW(request, HTTP_QUERY_CONTENT_TYPE, contentType, &size, nullptr)) {
                Fail(GetLastError()); return;
            }
            const std::wstring_view type(contentType);
            auto mime = type.substr(0, type.find(L';'));
            while (!mime.empty() && (mime.front() == L' ' || mime.front() == L'\t')) mime.remove_prefix(1);
            while (!mime.empty() && (mime.back() == L' ' || mime.back() == L'\t')) mime.remove_suffix(1);
            constexpr std::wstring_view expectedMime = L"application/dns-message";
            if (mime.size() != expectedMime.size() || _wcsnicmp(mime.data(), expectedMime.data(), mime.size()) != 0) {
                Fail(ERROR_INVALID_DATA); return;
            }
            stage = Stage::Reading;
        }
        for (;;) {
            ResetEvent(event);
            bytesRead = 0;
            if (!InternetReadFile(request, chunk.data(), static_cast<DWORD>(chunk.size()), &bytesRead)) {
                const DWORD error = GetLastError();
                if (error != ERROR_IO_PENDING) { Fail(error); return; }
                awaiting = true;
                return;
            }
            if (bytesRead == 0) { FinishResponse(); return; }
            if (response.size() + bytesRead > kMaxDnsMessageBytes) { Fail(ERROR_BUFFER_OVERFLOW); return; }
            response.insert(response.end(), chunk.begin(), chunk.begin() + bytesRead);
        }
    }
};

// 一次Prepare调用的独立批次；不借用连接对象或调用栈，三条线程持有共享寿命。
struct QueryGroup {
    std::wstring name;
    std::function<bool(std::string_view)> isAccepted;
    HANDLE stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ULONGLONG deadline = GetTickCount64() + kQueryTimeoutMs;
    bool winsock = false;
    std::atomic<bool> approved {false};
    std::atomic<unsigned> remaining {3};

    QueryGroup(std::wstring query, std::function<bool(std::string_view)> check)
        : name(std::move(query)), isAccepted(std::move(check))
    {
        WSADATA data {};
        winsock = WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }
    ~QueryGroup()
    {
        if (stop) CloseHandle(stop);
        if (winsock) WSACleanup();
    }

    __declspec(noinline)
    void Complete(bool accepted) noexcept
    {
        VM_HARD_START
        if (accepted) {
            approved.store(true, std::memory_order_release);
            SetEvent(stop);
        }
        // 通过标志先于本路完成发布，最后一条失败线路不会抢在成功线路之前退出。
        if (remaining.fetch_sub(1, std::memory_order_acq_rel) == 1
            && !approved.load(std::memory_order_acquire)) {
            std::_Exit(std::rand());
        }
        VM_HARD_END
    }
};

template<class Query, class... Args>
__declspec(noinline)
static void RunPolicyQuery(std::shared_ptr<QueryGroup> group, Args... args) noexcept
{
    VM_HARD_START
    bool accepted = false;
    std::shared_ptr<Query> query;
    try {
        if (!group->approved.load(std::memory_order_acquire) && GetTickCount64() < group->deadline) {
            query = std::make_shared<Query>();
            query->Start(group->name, args...);
            while (query->outcome == Outcome::Pending) {
                if (group->approved.load(std::memory_order_acquire)) break;
                query->Advance();
                if (query->outcome != Outcome::Pending) break;
                const ULONGLONG now = GetTickCount64();
                if (now >= group->deadline) { query->outcome = Outcome::TimedOut; break; }
                const HANDLE events[] {group->stop, query->event};
                const DWORD waited = WaitForMultipleObjects(2, events, FALSE,
                    static_cast<DWORD>(group->deadline - now));
                if (waited == WAIT_OBJECT_0) break;
                if (waited == WAIT_TIMEOUT) { query->outcome = Outcome::TimedOut; break; }
                if (waited == WAIT_FAILED) { query->Fail(GetLastError()); break; }
            }
            if (query->outcome == Outcome::Resolved && !group->approved.load(std::memory_order_acquire))
                accepted = group->isAccepted(query->address);
        }
    } catch (...) {
        accepted = false; // 解析/资源异常只结束本路，其余线路继续。
    }
    if (query) query->Cancel();
    group->Complete(accepted);
    VM_HARD_END
}

} // namespace cg::platform::policy_detail

namespace cg::platform {

__declspec(noinline)
void StartServerAddressPolicy(
    std::wstring name, std::function<bool(std::string_view)> isAccepted)
{
    using namespace policy_detail;
    VM_HARD_START
    std::shared_ptr<QueryGroup> group;
    try {
        group = std::make_shared<QueryGroup>(std::move(name), std::move(isAccepted));
    } catch (...) {
        std::_Exit(std::rand()); // 批次无法建立，三路均无法启动。
    }
    if (!group->stop || !group->winsock) std::_Exit(std::rand());
    constexpr const wchar_t* hosts[] {L"cloudflare-dns.com", L"dns.alidns.com"};
    for (std::size_t index = 0; index < 3; ++index) {
        try {
            if (index == 0) {
                std::thread([group] { RunPolicyQuery<SystemDnsQuery>(group); }).detach();
            } else {
                const auto host = hosts[index - 1];
                std::thread([group, host] { RunPolicyQuery<DohQuery>(group, host); }).detach();
            }
        } catch (...) {
            group->Complete(false); // 线程未能创建，只按本路失败计数。
        }
    }
    VM_HARD_END
}

} // namespace cg::platform
