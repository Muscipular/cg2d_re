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

// 原句柄访问/释放及关闭/提交，不操作宿主CRT描述符表。
HANDLE GetLegacyDescriptorHandle_543897(std::int32_t descriptor);
std::int32_t ReleaseLegacyDescriptorHandle_54381D(std::int32_t descriptor);
std::int32_t CloseLegacyDescriptor_53D30A(std::int32_t descriptor);
std::int32_t CommitLegacyDescriptor_541222(std::int32_t descriptor);

// 原541279；准确依赖声明，尚未实现，不能调用宿主_write替代。
std::int32_t WriteLegacyDescriptor_541279(
    std::int32_t descriptor, const void* buffer, std::uint32_t count);

} // namespace cg::platform
