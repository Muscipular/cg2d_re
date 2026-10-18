#include "cg/platform/platformTool.hpp"
#include "cg/platform/MainLoop.hpp"
#include <cstdlib>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <bcrypt.h>
#include <iphlpapi.h>
#include <ipifcons.h>

#include <algorithm>
#include <array>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstring>
#include <new>
#include <format>
#include <limits>
#include <random>
#include <string>
#include <string_view>

#include "cg/core/vm.h"

namespace cg::platform {

// 原函数：0x00534EF0。
std::int32_t GetMacAddress_534EF0(char* const output)
{
    ULONG size = 0;
    const ULONG firstResult =
        ::GetAdaptersAddresses(AF_INET, 0, nullptr, nullptr, &size);
    char ethernetText[16];
    char wirelessText[16];
    // 原两槽相距16B，各清前13B，不补尾三字节。
    std::memset(ethernetText, 0, 13);
    std::memset(wirelessText, 0, 13);
    if (firstResult != ERROR_BUFFER_OVERFLOW) {
        return -1;
    }

    void* const allocation = std::malloc(static_cast<std::uint32_t>(size));
    auto* const adapters = static_cast<IP_ADAPTER_ADDRESSES*>(allocation);
    const ULONG secondResult =
        ::GetAdaptersAddresses(AF_INET, 0, nullptr, adapters, &size);
    if (secondResult != NO_ERROR) {
        std::free(allocation);
        return -1;
    }

    const IP_ADAPTER_ADDRESSES* selected = adapters;
    ULONG selectedType = 0; // 非空退出循环必经IfType赋值；遍历结束为空时提前返回。
    while (selected != nullptr) {
        if (selected->OperStatus == IfOperStatusUp) {
            // 原534F77每项只取一次类型，资格和格式分支共用此值。
            selectedType = selected->IfType;
            if (selectedType == IF_TYPE_ETHERNET_CSMACD
                || selectedType == IF_TYPE_IEEE80211) {
                break;
            }
        }
        selected = selected->Next;
    }
    if (selected == nullptr) {
        std::free(allocation);
        return -1;
    }

    char* const text = selectedType == IF_TYPE_ETHERNET_CSMACD
        ? ethernetText : wirelessText;
    // 原两支路均按5→0取六U8；只读volatile视图保留取样序，格式仍按0→5。
    const volatile BYTE* const address = selected->PhysicalAddress;
    const std::uint32_t byte5 = address[5];
    const std::uint32_t byte4 = address[4];
    const std::uint32_t byte3 = address[3];
    const std::uint32_t byte2 = address[2];
    const std::uint32_t byte1 = address[1];
    const std::uint32_t byte0 = address[0];
    std::sprintf(
        text,
        "%02x%02x%02x%02x%02x%02x",
        byte0, byte1, byte2, byte3, byte4, byte5);

    // 原端两条写出路径均未释放成功获取的适配器缓冲。
    if (std::strlen(ethernetText) == 12) {
        std::memcpy(output, ethernetText, std::strlen(ethernetText) + 1);
        return 0;
    }
    if (std::strlen(wirelessText) == 12) {
        std::memcpy(output, wirelessText, std::strlen(wirelessText) + 1);
    }
    return 0;
}

// 本端新增：供ApplyServerAddressPolicy使用，原始字节SHA-256转64位小写十六进制；CNG失败返回空串。
static std::string sha256ToString(std::string_view input)
{
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    if (!BCRYPT_SUCCESS(::BCryptOpenAlgorithmProvider(
            &algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0))) {
        return {};
    }

    BCRYPT_HASH_HANDLE hash = nullptr;
    std::array<unsigned char, 32> digest {};
    NTSTATUS status = ::BCryptCreateHash(
        algorithm, &hash, nullptr, 0, nullptr, 0, 0);
    if (BCRYPT_SUCCESS(status)) {
        while (!input.empty()) {
            const ULONG length = static_cast<ULONG>(std::min<std::size_t>(
                input.size(), std::numeric_limits<ULONG>::max()));
            // BCryptHashData的输入参数没有const限定，但接口合同保证不修改输入。
            status = ::BCryptHashData(
                hash, reinterpret_cast<PUCHAR>(const_cast<char*>(input.data())), length, 0);
            if (!BCRYPT_SUCCESS(status)) {
                break;
            }
            input.remove_prefix(length);
        }
        if (BCRYPT_SUCCESS(status)) {
            status = ::BCryptFinishHash(
                hash, digest.data(), static_cast<ULONG>(digest.size()), 0);
        }
        static_cast<void>(::BCryptDestroyHash(hash));
    }
    static_cast<void>(::BCryptCloseAlgorithmProvider(algorithm, 0));
    if (!BCRYPT_SUCCESS(status)) {
        return {};
    }

    constexpr char digits[] = "0123456789abcdef";
    std::string result(digest.size() * 2, '0');
    for (std::size_t index = 0; index < digest.size(); ++index) {
        result[index * 2] = digits[digest[index] >> 4];
        result[index * 2 + 1] = digits[digest[index] & 0x0F];
    }
    return result;
}

// A10：沿参考ResolveDNS查询系统IPv4地址，临时Winsock引用和结果链均在本次调用内释放。
static std::string ResolveServerPolicyAddress(const std::string& hostname)
{
    WSADATA wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return "";
    }

    addrinfo hints {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    if (::getaddrinfo(hostname.c_str(), nullptr, &hints, &result) != 0) {
        static_cast<void>(::WSACleanup());
        return "";
    }

    std::string address;
    char addressText[INET6_ADDRSTRLEN];
    for (const addrinfo* entry = result; entry != nullptr; entry = entry->ai_next) {
        if (entry->ai_family == AF_INET) {
            const auto* ipv4 = reinterpret_cast<const sockaddr_in*>(entry->ai_addr);
            if (::inet_ntop(AF_INET, &ipv4->sin_addr, addressText, sizeof(addressText)) != nullptr) {
                address = addressText;
                break;
            }
        } else if (entry->ai_family == AF_INET6) {
            const auto* ipv6 = reinterpret_cast<const sockaddr_in6*>(entry->ai_addr);
            if (::inet_ntop(AF_INET6, &ipv6->sin6_addr, addressText, sizeof(addressText)) != nullptr) {
                address = addressText;
                break;
            }
        }
    }

    ::freeaddrinfo(result);
    static_cast<void>(::WSACleanup());
    return address;
}

// A10/SAP01：保留地址映射与校验退出；CUN01将使用提示移至人物选择页。
void ApplyServerAddressPolicy(char* const serverAddress, std::uint16_t& port)
{
    VM_HARD_START
#ifdef TAOMOLI
    if (std::strcmp(serverAddress, "taomoli") == 0 && port <= 9)
    {
        std::strcpy(serverAddress, "218.78.128.210");
        port = static_cast<std::uint16_t>(9030 + port);
    }
    else
#endif
    {
        const std::string address = ResolveServerPolicyAddress(
            std::format("{}.{}.re.cg.qoq.im", serverAddress, port));
        if (!address.empty() && sha256ToString(address + "_re.cg.qoq.im") != "5d1e868246a4c66ee2eb1b9c035fda400068865b1d29aef8df4bdbe351e4a199" /*"8.8.8.8_re.cg.qoq.im"*/)
        {
            std::_Exit(std::random_device()());
        }
    }
    VM_HARD_END
}

} // namespace cg::platform
