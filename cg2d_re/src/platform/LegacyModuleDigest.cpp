#include "cg/platform/LegacyModuleDigest.hpp"

#include "cg/Input.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::platform {
namespace {

struct Md5Context {
    std::array<std::uint32_t, 4> state;
    std::uint32_t bitCountLow;
    std::uint32_t bitCountHigh;
    std::array<std::uint8_t, 64> buffer;
    std::uint32_t bufferSize;
};

constexpr std::array<std::uint32_t, 64> kRoundConstants {
    0xD76AA478u, 0xE8C7B756u, 0x242070DBu, 0xC1BDCEEEu,
    0xF57C0FAFu, 0x4787C62Au, 0xA8304613u, 0xFD469501u,
    0x698098D8u, 0x8B44F7AFu, 0xFFFF5BB1u, 0x895CD7BEu,
    0x6B901122u, 0xFD987193u, 0xA679438Eu, 0x49B40821u,
    0xF61E2562u, 0xC040B340u, 0x265E5A51u, 0xE9B6C7AAu,
    0xD62F105Du, 0x02441453u, 0xD8A1E681u, 0xE7D3FBC8u,
    0x21E1CDE6u, 0xC33707D6u, 0xF4D50D87u, 0x455A14EDu,
    0xA9E3E905u, 0xFCEFA3F8u, 0x676F02D9u, 0x8D2A4C8Au,
    0xFFFA3942u, 0x8771F681u, 0x6D9D6122u, 0xFDE5380Cu,
    0xA4BEEA44u, 0x4BDECFA9u, 0xF6BB4B60u, 0xBEBFBC70u,
    0x289B7EC6u, 0xEAA127FAu, 0xD4EF3085u, 0x04881D05u,
    0xD9D4D039u, 0xE6DB99E5u, 0x1FA27CF8u, 0xC4AC5665u,
    0xF4292244u, 0x432AFF97u, 0xAB9423A7u, 0xFC93A039u,
    0x655B59C3u, 0x8F0CCC92u, 0xFFEFF47Du, 0x85845DD1u,
    0x6FA87E4Fu, 0xFE2CE6E0u, 0xA3014314u, 0x4E0811A1u,
    0xF7537E82u, 0xBD3AF235u, 0x2AD7D2BBu, 0xEB86D391u,
};

constexpr std::array<int, 64> kShiftAmounts {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
};

std::uint32_t LoadLittleEndian32(const std::uint8_t* input)
{
    return static_cast<std::uint32_t>(input[0])
        | (static_cast<std::uint32_t>(input[1]) << 8u)
        | (static_cast<std::uint32_t>(input[2]) << 16u)
        | (static_cast<std::uint32_t>(input[3]) << 24u);
}

void StoreLittleEndian32(std::uint32_t value, std::uint8_t* output)
{
    output[0] = static_cast<std::uint8_t>(value);
    output[1] = static_cast<std::uint8_t>(value >> 8u);
    output[2] = static_cast<std::uint8_t>(value >> 16u);
    output[3] = static_cast<std::uint8_t>(value >> 24u);
}

// 原函数：0x00402B70。
void Initialize(Md5Context& context)
{
    context.state = {
        0x67452301u,
        0xEFCDAB89u,
        0x98BADCFEu,
        0x10325476u,
    };
    context.bitCountLow = 0;
    context.bitCountHigh = 0;
    context.buffer.fill(0);
    context.bufferSize = 0;
}

// 原函数：0x00402D30。
void Transform(Md5Context& context, const std::uint8_t* block)
{
    std::array<std::uint32_t, 16> words {};
    for (std::size_t index = 0; index < words.size(); ++index) {
        words[index] = LoadLittleEndian32(block + index * 4);
    }

    std::uint32_t a = context.state[0];
    std::uint32_t b = context.state[1];
    std::uint32_t c = context.state[2];
    std::uint32_t d = context.state[3];

    for (std::size_t round = 0; round < 64; ++round) {
        std::uint32_t mixed;
        std::size_t wordIndex;
        if (round < 16) {
            mixed = (b & c) | (~b & d);
            wordIndex = round;
        } else if (round < 32) {
            mixed = (d & b) | (~d & c);
            wordIndex = (5 * round + 1) % 16;
        } else if (round < 48) {
            mixed = b ^ c ^ d;
            wordIndex = (3 * round + 5) % 16;
        } else {
            mixed = c ^ (b | ~d);
            wordIndex = (7 * round) % 16;
        }

        const std::uint32_t previousD = d;
        d = c;
        c = b;
        b += std::rotl(
            a + mixed + kRoundConstants[round] + words[wordIndex],
            kShiftAmounts[round]);
        a = previousD;
    }

    context.state[0] += a;
    context.state[1] += b;
    context.state[2] += c;
    context.state[3] += d;
}

// 原函数：0x00402C50。
void Update(Md5Context& context, const std::uint8_t* data, std::uint32_t size)
{
    const std::uint32_t previousLow = context.bitCountLow;
    context.bitCountLow += size << 3u;
    if (context.bitCountLow < previousLow) {
        ++context.bitCountHigh;
    }
    context.bitCountHigh += size >> 29u;

    std::uint32_t offset = 0;
    if (context.bufferSize != 0) {
        const std::uint32_t space = 64u - context.bufferSize;
        if (size < space) {
            std::memcpy(
                context.buffer.data() + context.bufferSize,
                data,
                size);
            context.bufferSize += size;
            return;
        }

        std::memcpy(
            context.buffer.data() + context.bufferSize,
            data,
            space);
        Transform(context, context.buffer.data());
        context.bufferSize = 0;
        offset = space;
    }

    while (size - offset >= 64u) {
        Transform(context, data + offset);
        offset += 64u;
    }

    const std::uint32_t remaining = size - offset;
    if (remaining != 0) {
        std::memcpy(context.buffer.data(), data + offset, remaining);
        context.bufferSize = remaining;
    }
}

// 原函数：0x00402BC0。
// 关联原入口：0x00402BA0（四个 W32 的小端序列化循环）。
void Finalize(Md5Context& context, std::array<std::uint8_t, 16>& digest)
{
    const std::uint32_t bitCountLow = context.bitCountLow;
    const std::uint32_t bitCountHigh = context.bitCountHigh;

    std::array<std::uint8_t, 64> padding {};
    padding[0] = 0x80u;
    const std::uint32_t paddingSize = context.bufferSize < 56u
        ? 56u - context.bufferSize
        : 120u - context.bufferSize;
    Update(context, padding.data(), paddingSize);

    std::array<std::uint8_t, 8> encodedLength {};
    StoreLittleEndian32(bitCountLow, encodedLength.data());
    StoreLittleEndian32(bitCountHigh, encodedLength.data() + 4);
    Update(context, encodedLength.data(), 8u);

    for (std::size_t index = 0; index < context.state.size(); ++index) {
        StoreLittleEndian32(context.state[index], digest.data() + index * 4);
    }
}

// 原函数：0x00403660。
bool ComputeFileDigest(const char* path, std::array<std::uint8_t, 16>& digest)
{
    Md5Context context {};
    Initialize(context);

    const HANDLE file = ::CreateFileA(
        path,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD fileSizeHigh = 0;
    DWORD remaining = ::GetFileSize(file, &fileSizeHigh);
    // 原4036C1..4036DF：首字节借共享文本，余0x807F字节清零；请求长度仍为0x8000。
    std::array<std::uint8_t, 0x8080> buffer;
    buffer[0] = 0;
    std::memset(buffer.data() + 1, 0, buffer.size() - 1);
    DWORD bytesRead = 0; // 原4036E6只在循环前写零，后续ReadFile沿用同一输出槽。
    do {
        static_cast<void>(::ReadFile(
            file,
            buffer.data(),
            0x8000u,
            &bytesRead,
            nullptr));
        Update(context, buffer.data(), bytesRead);
        remaining -= bytesRead;
    } while (remaining != 0);

    Finalize(context, digest);
    static_cast<void>(::CloseHandle(file));
    return true;
}

// 原函数：0x00403750。
std::array<char, 33> EncodeLowerHex(const std::array<std::uint8_t, 16>& digest)
{
    constexpr char kDigits[] = "0123456789abcdef";
    std::array<char, 33> text {};
    for (std::size_t index = 0; index < digest.size(); ++index) {
        text[index * 2] = kDigits[digest[index] >> 4u];
        text[index * 2 + 1] = kDigits[digest[index] & 0x0Fu];
    }
    text[32] = '\0';
    return text;
}

} // namespace

// 原函数：0x005202E0。
char* ComputeExecutableDigest_5202E0(char* output)
{
    if (output == nullptr) {
        return nullptr;
    }

    char path[256] {};
    static_cast<void>(::GetModuleFileNameA(nullptr, path, 0xFFu));

    std::array<std::uint8_t, 16> digest {};
    if (!ComputeFileDigest(path, digest)) {
        std::strcpy(output, "");
        return nullptr;
    }

    const std::array<char, 33> text = EncodeLowerHex(digest);
    std::memcpy(output, text.data(), text.size());
    return output;
}

} // namespace cg::platform
