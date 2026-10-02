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

// 原10862AC默认FILE标志；打开模式c/n只修改本次采样，不回写该全局。
extern std::uint32_t gLegacyDefaultStreamFlags10862AC;

// 原模式解析及两层打开；返回唯一原FILE槽，不能与std::FILE混用。
LegacyFile* OpenLegacyStreamInSlot_53F6D8(const char* filename, const char* mode,
    std::uint32_t shareFlags, LegacyFile* stream);
LegacyFile* OpenSharedLegacyStream_53B2D1(
    const char* filename, const char* mode, std::uint32_t shareFlags);
LegacyFile* OpenLegacyStream_53B2F1(const char* filename, const char* mode);

// 原CRT553174初始化；必须已有底层描述符生产，不主动调用或借宿主FILE。
void InitializeLegacyStdio_53E9A5();
LegacyFile* AcquireLegacyStream_53F848();
char* AllocateLegacyStreamBuffer_544815(LegacyFile* stream);
void FreeLegacyStreamBuffer_53F38F(LegacyFile* stream);
// 原读取本体；读行count<=0/首EOF不写目标，count1不读取流。
std::int32_t FillLegacyStreamBuffer_53F3BA(LegacyFile* stream);
std::int32_t ReadLegacyStreamByte_53C8B3(LegacyFile* stream);
char* ReadLegacyStreamLine_53B226(char* buffer, std::int32_t maxCount, LegacyFile* stream);
// 原批量读取/写入；数量及元素返回为U32，部分元素字节不撤回。
std::uint32_t ReadLegacyStream_53BCCD(void* buffer, std::uint32_t elementSize,
    std::uint32_t elementCount, LegacyFile* stream);
std::uint32_t WriteLegacyStream_53C680(const void* buffer, std::uint32_t elementSize,
    std::uint32_t elementCount, LegacyFile* stream);
// 原缓冲输出/单字节/字符串；只使用LegacyFile，禁止与宿主FILE混用。
std::int32_t FlushAndWriteLegacyStreamByte_53F8C0(std::int32_t character, LegacyFile* stream);
std::int32_t WriteLegacyStreamByte_53CFDC(std::int32_t character, LegacyFile* stream);
std::int32_t WriteLegacyStreamString_53B356(const char* text, LegacyFile* stream);
// 原stdout/stderr临时缓冲缓存；ftbuf拆除FILE引用但不释放缓存。
extern char* gLegacyTemporaryBuffers1086040[2];
std::int32_t BeginLegacyTemporaryBuffer_53EA5E(LegacyFile* stream);
void EndLegacyTemporaryBuffer_53EAEB(std::int32_t started, LegacyFile* stream);
// 原S32位置及缓冲修正；fseek/rewind按原合同忽略前置flush失败。
std::int32_t TellLegacyStream_53CB78(LegacyFile* stream);
std::int32_t SeekLegacyStream_53BC41(
    LegacyFile* stream, std::int32_t offset, std::uint32_t origin);
void RewindLegacyStream_53B27D(LegacyFile* stream);
// 原缓冲设置和低字节回推；借用外部缓冲不转移所有权。
std::int32_t SetLegacyStreamBuffer_54ACF1(
    LegacyFile* stream, char* buffer, std::uint32_t mode, std::uint32_t size);
std::int32_t UnreadLegacyStreamByte_54463F(std::int32_t character, LegacyFile* stream);
// 原fpos数值为8B的低/高W32，非地址；保留逐半发布及读取次序。
struct LegacyFilePosition final {
    std::uint32_t low;
    std::uint32_t high;
};
static_assert(offsetof(LegacyFilePosition, low) == 0);
static_assert(offsetof(LegacyFilePosition, high) == 4);
static_assert(sizeof(LegacyFilePosition) == 8);
static_assert(alignof(LegacyFilePosition) == 4);

// 原64位位置链独立于S32接口；缓冲修正数量仍为W32。
std::int64_t TellLegacyStream64_54B239(LegacyFile* stream);
std::int32_t SeekLegacyStream64_54B3C0(
    LegacyFile* stream, std::int64_t offset, std::uint32_t origin);
std::int32_t GetLegacyStreamPosition_54ACB7(
    LegacyFile* stream, LegacyFilePosition* position);
std::int32_t SetLegacyStreamPosition_54ACD9(
    LegacyFile* stream, const LegacyFilePosition* position);
std::int32_t FlushLegacyStreamBuffer_53BB3D(LegacyFile* stream);
std::int32_t FlushLegacyStream_53BB02(LegacyFile* stream);
std::int32_t CloseLegacyStream_53B1D0(LegacyFile* stream);
std::int32_t FlushLegacyStreams_53BBA2(std::int32_t mode);
std::int32_t FlushAllLegacyStreams_53BB99();
std::int32_t CloseAllLegacyStreams_5446AD();

} // namespace cg::platform
