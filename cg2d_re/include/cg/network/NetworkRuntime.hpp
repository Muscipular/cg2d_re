#pragma once

#include <cstdint>

namespace cg::network {

using NetworkCallback = std::int32_t (*)(
    std::uint32_t value,
    const void* data,
    std::uint32_t size);

// 原 sub_4037D0@0x4037D0：恢复协议常量并清零相关短状态。
std::int32_t ResetProtocolState_4037D0();

// 原 sub_516DA0@[0x516DA0,0x517038)：分配并清零九项协议缓冲。
std::int32_t AllocateProtocolBuffers_516DA0(std::uint32_t bufferLength);

// 原 sub_5170D0@[0x5170D0,0x51711C)：在有符号 W32 容量内追加协议文本。
std::int32_t AppendProtocolText_5170D0(
    char* destination,
    const char* source,
    std::int32_t capacity);

// 原 sub_516B60@0x516B60：安装回调并分配 18 项网络工作区。
std::int32_t AllocateNetworkWorkspace_516B60(
    NetworkCallback callback,
    std::uint32_t bufferLength,
    std::uintptr_t unusedSocket);

// 原 sub_508140@0x508140：启动 Winsock 并创建网络运行期工作区。
std::int32_t InitializeNetworkRuntime_508140();

// 原 sub_5081A0@0x5081A0：关闭 Winsock 并释放网络运行期工作区。
void ShutdownNetworkRuntime_5081A0();

} // namespace cg::network
