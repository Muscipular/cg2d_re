#include "cg/platform/LegacyModuleDigest.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <bcrypt.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::platform {
namespace {

// COR09：拥有本次MD5的CNG句柄，必须先销毁hash，再关闭算法提供者。
struct Md5Context final {
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_HASH_HANDLE hash = nullptr;

    Md5Context() = default;
    Md5Context(const Md5Context&) = delete;
    Md5Context& operator=(const Md5Context&) = delete;

    ~Md5Context()
    {
        if (hash != nullptr) {
            static_cast<void>(::BCryptDestroyHash(hash));
        }
        if (algorithm != nullptr) {
            static_cast<void>(::BCryptCloseAlgorithmProvider(algorithm, 0));
        }
    }
};

// 原函数：0x00403660。
bool ComputeFileDigest(const char* path, std::array<std::uint8_t, 16>& digest)
{
    Md5Context context {};
    // 原402B70／402D30：MD5初始化与轮变换由系统提供，非HMAC。
    if (!BCRYPT_SUCCESS(::BCryptOpenAlgorithmProvider(
            &context.algorithm, BCRYPT_MD5_ALGORITHM, nullptr, 0))) {
        return false;
    }
    if (!BCRYPT_SUCCESS(::BCryptCreateHash(
            context.algorithm, &context.hash, nullptr, 0, nullptr, 0, 0))) {
        return false;
    }

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
        // 原402C50：仅处理本轮读取的字节，不改变读缓冲和文件计数。
        if (!BCRYPT_SUCCESS(::BCryptHashData(
                context.hash, buffer.data(), bytesRead, 0))) {
            static_cast<void>(::CloseHandle(file));
            return false;
        }
        remaining -= bytesRead;
    } while (remaining != 0);

    // 原402BC0／402BA0：系统完成MD5填充与16字节输出，不再二次转换。
    const NTSTATUS status = ::BCryptFinishHash(
        context.hash, digest.data(), static_cast<ULONG>(digest.size()), 0);
    static_cast<void>(::CloseHandle(file));
    return BCRYPT_SUCCESS(status);
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
