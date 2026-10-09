#include "LegacyStdio.hpp"

#include <bit>
#include <cwchar>

namespace cg::platform {

static_assert(sizeof(std::fpos_t) == sizeof(std::uint64_t));
static_assert(sizeof(long) == sizeof(std::int32_t));
static_assert(sizeof(wchar_t) == sizeof(std::uint16_t));

// 原53B2F1：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
LegacyFile* OpenLegacyStream_53B2F1(const char* const filename, const char* const mode)
{
    return std::fopen(filename, mode);
}

// 原53B2D1：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
LegacyFile* OpenSharedLegacyStream_53B2D1(const char* const filename,
    const char* const mode, const std::uint32_t shareFlags)
{
    return ::_fsopen(filename, mode, static_cast<int>(shareFlags));
}

// 原53C8B3：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t ReadLegacyStreamByte_53C8B3(LegacyFile* const stream)
{
    return std::fgetc(stream);
}

// 原53B226：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
char* ReadLegacyStreamLine_53B226(char* const buffer,
    const std::int32_t maxCount, LegacyFile* const stream)
{
    return std::fgets(buffer, maxCount, stream);
}

// 原53BCCD：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::uint32_t ReadLegacyStream_53BCCD(void* const buffer,
    const std::uint32_t elementSize, const std::uint32_t elementCount,
    LegacyFile* const stream)
{
    return static_cast<std::uint32_t>(std::fread(buffer, elementSize, elementCount, stream));
}

// 原53C680：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::uint32_t WriteLegacyStream_53C680(const void* const buffer,
    const std::uint32_t elementSize, const std::uint32_t elementCount,
    LegacyFile* const stream)
{
    return static_cast<std::uint32_t>(std::fwrite(buffer, elementSize, elementCount, stream));
}

// 原53CFDC：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t WriteLegacyStreamByte_53CFDC(
    const std::int32_t character, LegacyFile* const stream)
{
    return std::fputc(character, stream);
}

// 原53B356：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t WriteLegacyStreamString_53B356(
    const char* const text, LegacyFile* const stream)
{
    return std::fputs(text, stream);
}

// 原53CB78：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t TellLegacyStream_53CB78(LegacyFile* const stream)
{
    return static_cast<std::int32_t>(std::ftell(stream));
}

// 原53BC41：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t SeekLegacyStream_53BC41(LegacyFile* const stream,
    std::int32_t offset, std::uint32_t origin)
{
    return std::fseek(stream, offset, static_cast<int>(origin));
}

// 原53B27D：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
void RewindLegacyStream_53B27D(LegacyFile* const stream)
{
    std::rewind(stream);
}

// 原54ACF1：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t SetLegacyStreamBuffer_54ACF1(LegacyFile* const stream,
    char* buffer, const std::uint32_t mode, std::uint32_t size)
{
    return std::setvbuf(stream, buffer, static_cast<int>(mode), size);
}

// 原54463F：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t UnreadLegacyStreamByte_54463F(
    const std::int32_t character, LegacyFile* const stream)
{
    return std::ungetc(character, stream);
}

// 原54B239：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int64_t TellLegacyStream64_54B239(LegacyFile* const stream)
{
    return ::_ftelli64(stream);
}

// 原54B3C0：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t SeekLegacyStream64_54B3C0(LegacyFile* const stream,
    std::int64_t offset, std::uint32_t origin)
{
    return ::_fseeki64(stream, offset, static_cast<int>(origin));
}

// 原54ACB7：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t GetLegacyStreamPosition_54ACB7(
    LegacyFile* const stream, LegacyFilePosition* const position)
{
    std::fpos_t native;
    const std::int32_t result = std::fgetpos(stream, &native);
    if (result == 0) {
        const auto bits = std::bit_cast<std::uint64_t>(native);
        position->low = static_cast<std::uint32_t>(bits);
        position->high = static_cast<std::uint32_t>(bits >> 32);
    }
    return result;
}

// 原54ACD9：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t SetLegacyStreamPosition_54ACD9(
    LegacyFile* const stream, const LegacyFilePosition* const position)
{
    const std::uint64_t bits = (static_cast<std::uint64_t>(position->high) << 32)
        | position->low;
    const auto native = std::bit_cast<std::fpos_t>(bits);
    return std::fsetpos(stream, &native);
}

// 原54AD92：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::uint16_t WriteLegacyStreamWide_54AD92(
    const std::uint16_t character, LegacyFile* const stream)
{
    return static_cast<std::uint16_t>(std::fputwc(character, stream));
}

// 原54AF70：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::uint16_t ReadLegacyStreamWide_54AF70(LegacyFile* const stream)
{
    return static_cast<std::uint16_t>(std::fgetwc(stream));
}

// 原54AE87：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::uint16_t UnreadLegacyStreamWide_54AE87(
    const std::uint16_t character, LegacyFile* const stream)
{
    return static_cast<std::uint16_t>(std::ungetwc(character, stream));
}

// 原53BB02：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t FlushLegacyStream_53BB02(LegacyFile* const stream)
{
    return std::fflush(stream);
}

// 原53B1D0：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t CloseLegacyStream_53B1D0(LegacyFile* const stream)
{
    return std::fclose(stream);
}

// 原53BB99：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t FlushAllLegacyStreams_53BB99()
{
    return ::_flushall();
}

// 原5446AD：MC01-B实际MSVC服务，定宽参数/结果沿调用方。
std::int32_t CloseAllLegacyStreams_5446AD()
{
    return ::_fcloseall();
}

} // namespace cg::platform
