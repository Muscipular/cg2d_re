#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原32B FILE；三个地址字段拓宽，其余W32字段和内部字符槽保持。
struct LegacyFile final {
    char* current;                     // 原+00。
    std::int32_t remaining;            // 原+04。
    char* base;                        // 原+08。
    std::uint32_t flags;               // 原+0C。
    std::int32_t descriptor;           // 原+10。
    std::int32_t characterBuffer;      // 原+14；失败缓冲借其前2字节。
    std::int32_t bufferSize;           // 原+18。
    char* temporaryName;               // 原+1C。
};
static_assert(offsetof(LegacyFile, current) == 0);
static_assert(offsetof(LegacyFile, remaining) == 8);
static_assert(offsetof(LegacyFile, base) == 16);
static_assert(offsetof(LegacyFile, flags) == 24);
static_assert(offsetof(LegacyFile, descriptor) == 28);
static_assert(offsetof(LegacyFile, characterBuffer) == 32);
static_assert(offsetof(LegacyFile, bufferSize) == 36);
static_assert(offsetof(LegacyFile, temporaryName) == 40);
static_assert(sizeof(LegacyFile) == 48);

extern char gLegacyStandardInputBuffer1086AE0[4096];
extern LegacyFile gLegacyStaticFiles58F8B8[20];
extern LegacyFile** gLegacyStreamTable1086AC4;
extern std::int32_t gLegacyStreamLimit1087AE0;
extern std::uint32_t gLegacyStreamAllocationCount108603C;

// 原CRT553174初始化；必须已有底层描述符生产，不主动调用或借宿主FILE。
void InitializeLegacyStdio_53E9A5();
LegacyFile* AcquireLegacyStream_53F848();
char* AllocateLegacyStreamBuffer_544815(LegacyFile* stream);
void FreeLegacyStreamBuffer_53F38F(LegacyFile* stream);
std::int32_t FlushLegacyStreamBuffer_53BB3D(LegacyFile* stream);
std::int32_t FlushLegacyStream_53BB02(LegacyFile* stream);
std::int32_t CloseLegacyStream_53B1D0(LegacyFile* stream);
std::int32_t FlushLegacyStreams_53BBA2(std::int32_t mode);
std::int32_t FlushAllLegacyStreams_53BB99();
std::int32_t CloseAllLegacyStreams_5446AD();

} // namespace cg::platform
