#pragma once

#include <cstdint>
#include <string_view>

namespace cg::network {

// 原 sub_517080@[0x517080,0x5170C1)：按有符号 W32 上限复制协议字节并补 NUL。
void CopyProtocolBytes_517080(
    std::uint8_t* destination,
    const std::uint8_t* source,
    std::int32_t capacity);

// 原 [0x517040,0x517080)：先清输出 W32，再按参数首指针选择复制来源。
void ReadProtocolCommandHeader_517040(
    std::uint32_t* output,
    std::uint8_t* destination,
    std::int32_t capacity,
    char* const* protocolArgs);

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

// 原 sub_517190@[0x517190,0x5171A3)：空指针返回 0，否则委派协议整数解析。
std::int32_t ParseProtocolInteger_517190(const char* text);

// 原 sub_52CEA0@[0x52CEA0,0x52CECB)：跳过高位字节对，定位下一段。
const char* FindNextDelimitedField_52CEA0(
    const char* source, char delimiter);

// 原 sub_52CF30@[0x52CF30,0x52CF98)：复制字段并补 NUL；遇分隔符返回 0。
std::int32_t CopyDelimitedField_52CF30(
    const char* source, char delimiter, std::int32_t capacity,
    char* destination);

// 原 sub_52CED0@[0x52CED0,0x52CF27)：按序号选字段并保留复制返回。
std::int32_t ExtractDelimitedField_52CED0(
    const char* source, char delimiter, std::int32_t ordinal,
    std::int32_t capacity, char* destination);

// 原 sub_52CFA0@[0x52CFA0,0x52CFED)：取至多 127 字节字段，空串返回 -1。
std::int32_t ParseDelimitedInteger_52CFA0(
    const char* source, char delimiter, std::int32_t ordinal);

// 原 sub_52CFF0@[0x52CFF0,0x52D04F)：空字段返回 2 且不写整数输出。
std::int32_t ParseDelimitedIntegerStatus_52CFF0(
    const char* source, char delimiter, std::int32_t ordinal,
    std::int32_t* output);

// 原 base62Decode@[0x52D050,0x52D0C2)：逐字节先乘 62，负号可在任意位置。
std::int32_t Base62Decode_52D050(const char* text);

// 原 sub_52D0D0@[0x52D0D0,0x52D11D)：提取字段后解码 base62。
std::int32_t ParseDelimitedBase62_52D0D0(
    const char* source, char delimiter, std::int32_t ordinal);

} // namespace cg::network
