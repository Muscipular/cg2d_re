#pragma once

#include <cstdint>

namespace cg::network {

using NetworkCallback = std::int32_t (*)(
    std::uintptr_t socket,
    const void* data,
    std::uint32_t size);

// 原 sub_4037D0@0x4037D0：恢复协议常量并清零相关短状态。
std::int32_t ResetProtocolState_4037D0();

// 原 sub_516DA0@[0x516DA0,0x517038)：分配并清零九项协议缓冲。
std::int32_t AllocateProtocolBuffers_516DA0(std::uint32_t bufferLength);

// 原 sub_517080@[0x517080,0x5170C1)：按有符号 W32 上限复制协议字节并补 NUL。
void CopyProtocolBytes_517080(
    std::uint8_t* destination,
    const std::uint8_t* source,
    std::int32_t capacity);

// 原 sub_517670@[0x517670,0x51768A)：格式化有符号 W32 到共享协议缓冲。
std::uint8_t* FormatProtocolInteger_517670(std::int32_t value);

// 原 sub_517690@[0x517690,0x5176AC)：复制协议字节并返回原目标指针。
std::uint8_t* CopyProtocolBytesAndReturn_517690(
    std::uint8_t* destination,
    std::int32_t capacity,
    const std::uint8_t* source);

// 原 sub_5170D0@[0x5170D0,0x51711C)：在有符号 W32 容量内追加协议文本。
std::int32_t AppendProtocolText_5170D0(
    char* destination,
    const char* source,
    std::int32_t capacity);

// 原 sub_517120@[0x517120,0x517160)：格式化协议整数并追加 ASCII 空格。
std::uint8_t* FormatProtocolIntegerToken_517120(std::int32_t value);

// 原 sub_517160@[0x517160,0x517186)：转义协议文本并追加 ASCII 空格。
std::uint8_t* FormatProtocolTextToken_517160(const char* text);

// 原 sub_517190@[0x517190,0x5171A3)：空指针返回 0，否则委派协议整数解析。
std::int32_t ParseProtocolInteger_517190(const char* text);

// 原 sub_5171F0@[0x5171F0,0x5172CE)：向共享缓冲逐字节转义协议文本。
std::uint8_t* EscapeProtocolText_5171F0(const char* text);

// 原 sub_5172D0@[0x5172D0,0x5173A6)：向共享缓冲逐字节反转义协议文本。
std::uint8_t* UnescapeProtocolText_5172D0(const char* text);

// 原 sub_517480@[0x517480,0x517502)：记录、观察并发送协议行缓冲。
std::int32_t SendProtocolBuffer_517480(
    std::uintptr_t socket,
    char* buffer);

// 原 Report_512CA0@[0x512CA0,0x512CE8)：构造并发送 PGXE 上报协议行。
std::int32_t Report_512CA0(
    std::uintptr_t socket,
    std::int32_t value);

// 原 sub_512C10@[0x512C10,0x512C58)：构造 Or 文本命令并透传发送返回。
std::int32_t Sub_512C10(std::uintptr_t socket, const char* text);

// 原 sub_516B60@0x516B60：安装回调并分配 18 项网络工作区。
std::int32_t AllocateNetworkWorkspace_516B60(
    NetworkCallback callback,
    std::uint32_t bufferLength,
    std::uintptr_t unusedSocket);

// 原 sub_508140@0x508140：启动 Winsock 并创建网络运行期工作区。
std::int32_t InitializeNetworkRuntime_508140();

// 只读原 W8 0xD9F2C8 活动门禁；状态仍由网络运行期唯一持有。
std::uint8_t ReadNetworkRuntimeActive() noexcept;

// 原 0x593850 首字节；供原生窗口枚举回调等逐次读取，不暴露整段文本地址。
std::uint8_t ReadProtocolByte593850() noexcept;

// 原 0xD9F2D0/0xDA3B22 的字段级访问；不公开网络运行期完整状态。
void WriteValueD9F2D0(std::uint32_t value) noexcept;
std::uint16_t ReadProtocolModeDA3B22() noexcept;
void WriteProtocolModeDA3B22(std::uint16_t value) noexcept;

// 原 sub_5081A0@0x5081A0：关闭 Winsock 并释放网络运行期工作区。
void ShutdownNetworkRuntime_5081A0();

} // namespace cg::network
