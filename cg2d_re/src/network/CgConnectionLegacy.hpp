#pragma once

#include "cg/network/NetworkRuntime.hpp"

namespace cg::network::detail {

// CTO01：仅旧协议函数使用的内部桥，不返回Impl或整个状态的可变引用。
// 输出/格式化缓冲仍按原同步借用合同使用，新业务应采用CgConnection公开接口。
struct ConnectionLegacyAccess final {
    static char* OutputBuffer(CgConnection& owner) noexcept;
    static std::uint32_t BufferCapacity(CgConnection& owner) noexcept;
    static std::int32_t AllocateProtocolBuffers_516DA0(CgConnection& owner, const std::uint32_t bufferLength);
    static std::uint8_t* FormatProtocolInteger_517670(CgConnection& owner, const std::int32_t value);
    static std::uint8_t* FormatProtocolIntegerToken_517120(CgConnection& owner, const std::int32_t value);
    static std::uint8_t* FormatProtocolTextToken_517160(CgConnection& owner, const std::wstring_view text);
    static std::uint8_t* EscapeProtocolText_5171F0(CgConnection& owner, const std::wstring_view text);
    static std::uint8_t* UnescapeProtocolText_5172D0(CgConnection& owner, const char* const text);
    static std::uint8_t* DecodeOptionalProtocolText_5171B0(CgConnection& owner, const char* const text);
    static std::int32_t SendProtocolBuffer_517480(CgConnection& owner, const std::uintptr_t socket, char* const buffer);
    static std::int32_t SendConnectionProbe_512C10(CgConnection& owner, const std::uintptr_t socket, const std::wstring_view text);
    static std::int32_t AppendReceiveQueue_5081E0(CgConnection& owner, const void* const source, const std::uint32_t size);
    static std::int32_t ConsumeReceiveQueue_508230(CgConnection& owner, const std::int32_t size);
    static std::uint16_t ReadProtocolModeDA3B22(CgConnection& owner) noexcept;
    static void WriteProtocolModeDA3B22(CgConnection& owner, const std::uint16_t value) noexcept;
    static void SplitReceivedBuffer_5173B0(CgConnection& owner, char* const buffer);
    static std::int32_t NetworkDispatch_512E40(CgConnection& owner, const std::uintptr_t socket, const void* const received);
    static void HandleServerAddressChangeReply_50E6E0(CgConnection& owner, const std::uintptr_t, const char* const text);
    static std::int32_t HandleVersionUpgradeReply_50E780(CgConnection& owner, const std::uintptr_t, const std::int32_t value);
    static std::int32_t AllocateNetworkWorkspace_516B60(CgConnection& owner, const NetworkCallback callback, const std::uint32_t bufferLength, const std::uintptr_t);
};

} // namespace cg::network::detail
