#pragma once

#include "StartupNative.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原8B低层记录；只拓宽HANDLE，W8标志/暂存字节及原未解释2字节保持。
struct LegacyDescriptorRecord final {
    HANDLE handle;
    std::uint8_t flags;
    std::uint8_t pipeByte;
    std::uint8_t remaining06[2];
};
static_assert(offsetof(LegacyDescriptorRecord, flags) == 8);
static_assert(offsetof(LegacyDescriptorRecord, pipeByte) == 9);
static_assert(offsetof(LegacyDescriptorRecord, remaining06) == 10);
static_assert(sizeof(LegacyDescriptorRecord) == 16);

extern LegacyDescriptorRecord* gLegacyDescriptorBlocks10869C0[64];
extern std::uint32_t gLegacyDescriptorCapacity1086AC0;

// 原53F52D；实际分配/继承/标准句柄生产，原生HANDLE继承块，返回SetHandleCount。
std::uint32_t InitializeLegacyDescriptors_53F52D();
// 原53F493；低W32定位，成功清原EOF位，不预清系统LastError。
std::int32_t SeekLegacyDescriptor_53F493(
    std::int32_t descriptor, std::int32_t offset, std::uint32_t origin);

// 原句柄访问/释放及关闭/提交，不操作宿主CRT描述符表。
HANDLE GetLegacyDescriptorHandle_543897(std::int32_t descriptor);
std::int32_t ReleaseLegacyDescriptorHandle_54381D(std::int32_t descriptor);
std::int32_t CloseLegacyDescriptor_53D30A(std::int32_t descriptor);
std::int32_t CommitLegacyDescriptor_541222(std::int32_t descriptor);

// 原541279；二进制直写或LF扩CRLF，保留短写、错误槽及已生成CR扣除次序。
std::int32_t WriteLegacyDescriptor_541279(
    std::int32_t descriptor, const void* buffer, std::uint32_t count);

} // namespace cg::platform
