#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace cg::platform {

// MC01-B：所有现文件指针直接借用MSVC FILE，禁止读取或复制其私有布局。
using LegacyFile = std::FILE;

// 业务位置仍为两个W32数值；fgetpos/fsetpos以真实MSVC fpos_t逐半适配。
struct LegacyFilePosition final {
    std::uint32_t low;
    std::uint32_t high;
};
static_assert(sizeof(LegacyFilePosition) == 8);
static_assert(offsetof(LegacyFilePosition, high) == 4);

LegacyFile* OpenLegacyStream_53B2F1(const char* const filename, const char* const mode);

LegacyFile* OpenSharedLegacyStream_53B2D1(const char* const filename,
    const char* const mode, const std::uint32_t shareFlags);

std::int32_t ReadLegacyStreamByte_53C8B3(LegacyFile* const stream);

char* ReadLegacyStreamLine_53B226(char* const buffer,
    const std::int32_t maxCount, LegacyFile* const stream);

std::uint32_t ReadLegacyStream_53BCCD(void* const buffer,
    const std::uint32_t elementSize, const std::uint32_t elementCount,
    LegacyFile* const stream);

std::uint32_t WriteLegacyStream_53C680(const void* const buffer,
    const std::uint32_t elementSize, const std::uint32_t elementCount,
    LegacyFile* const stream);

std::int32_t WriteLegacyStreamByte_53CFDC(
    const std::int32_t character, LegacyFile* const stream);

std::int32_t WriteLegacyStreamString_53B356(
    const char* const text, LegacyFile* const stream);

std::int32_t TellLegacyStream_53CB78(LegacyFile* const stream);

std::int32_t SeekLegacyStream_53BC41(LegacyFile* const stream,
    std::int32_t offset, std::uint32_t origin);

void RewindLegacyStream_53B27D(LegacyFile* const stream);

std::int32_t SetLegacyStreamBuffer_54ACF1(LegacyFile* const stream,
    char* buffer, const std::uint32_t mode, std::uint32_t size);

std::int32_t UnreadLegacyStreamByte_54463F(
    const std::int32_t character, LegacyFile* const stream);

std::int64_t TellLegacyStream64_54B239(LegacyFile* const stream);

std::int32_t SeekLegacyStream64_54B3C0(LegacyFile* const stream,
    std::int64_t offset, std::uint32_t origin);

std::int32_t GetLegacyStreamPosition_54ACB7(
    LegacyFile* const stream, LegacyFilePosition* const position);

std::int32_t SetLegacyStreamPosition_54ACD9(
    LegacyFile* const stream, const LegacyFilePosition* const position);

std::uint16_t WriteLegacyStreamWide_54AD92(
    const std::uint16_t character, LegacyFile* const stream);

std::uint16_t ReadLegacyStreamWide_54AF70(LegacyFile* const stream);

std::uint16_t UnreadLegacyStreamWide_54AE87(
    const std::uint16_t character, LegacyFile* const stream);

std::int32_t FlushLegacyStream_53BB02(LegacyFile* const stream);

std::int32_t CloseLegacyStream_53B1D0(LegacyFile* const stream);

std::int32_t FlushAllLegacyStreams_53BB99();

std::int32_t CloseAllLegacyStreams_5446AD();

} // namespace cg::platform
