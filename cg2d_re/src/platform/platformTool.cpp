#include "cg/platform/platformTool.hpp"
#include "cg/text/Encoding.hpp"
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
#include <string>
#include <string_view>

#include "cg/core/vm.h"

namespace cg::platform {

// 原函数：0x00534EF0。
std::int32_t GetMacAddress_534EF0(std::wstring& output)
{
    ULONG size = 0;
    const ULONG firstResult =
        ::GetAdaptersAddresses(AF_INET, 0, nullptr, nullptr, &size);
    wchar_t ethernetText[16]{};
    wchar_t wirelessText[16]{};
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

    wchar_t* const text = selectedType == IF_TYPE_ETHERNET_CSMACD
        ? ethernetText : wirelessText;
    // 原两支路均按5→0取六U8；只读volatile视图保留取样序，格式仍按0→5。
    const volatile BYTE* const address = selected->PhysicalAddress;
    const std::uint32_t byte5 = address[5];
    const std::uint32_t byte4 = address[4];
    const std::uint32_t byte3 = address[3];
    const std::uint32_t byte2 = address[2];
    const std::uint32_t byte1 = address[1];
    const std::uint32_t byte0 = address[0];
    std::swprintf(
        text, 16,
        L"%02x%02x%02x%02x%02x%02x",
        byte0, byte1, byte2, byte3, byte4, byte5);

    // 原端两条写出路径均未释放成功获取的适配器缓冲。
    if (std::wcslen(ethernetText) == 12) {
        output = ethernetText;
        return 0;
    }
    if (std::wcslen(wirelessText) == 12) {
        output = wirelessText;
    }
    return 0;
}

// 本端新增：供IsServerAddressPolicyAccepted使用，原始字节SHA-256转64位小写十六进制；CNG失败返回空串。
static std::string sha256ToString(std::string_view input)
{
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    if (!BCRYPT_SUCCESS(::BCryptOpenAlgorithmProvider(
            &algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0))) {
        return "";
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
        return "";
    }

    constexpr char digits[] = "0123456789abcdef";
    std::string result(digest.size() * 2, '0');
    for (std::size_t index = 0; index < digest.size(); ++index) {
        result[index * 2] = digits[digest[index] >> 4];
        result[index * 2 + 1] = digits[digest[index] & 0x0F];
    }
    return result;
}

// NTM01/A10：只生产策略查询，不在主线程执行同步DNS。
std::wstring PrepareServerAddressPolicy(std::wstring& serverAddress, std::uint16_t& port)
{
    VM_HARD_START
    std::wstring query;
#ifdef TAOMOLI
    if (serverAddress == L"taomoli" && port <= 9)
    {
        serverAddress = L"218.78.128.210";
        port = static_cast<std::uint16_t>(9030 + port);
    }
    else
#endif
    {
        query = std::format(L"{}.{}.re.cg.qoq.im", serverAddress, port);
    }
    VM_HARD_END
    return query;
}

// SDP01：纯结果判定；空结果和计算失败均不通过，查询协调器负责汇总，禁止终止进程。
bool IsServerAddressPolicyAccepted(const std::string_view address)
{
    VM_HARD_START
    const bool accepted = !address.empty() && sha256ToString(std::string(address) + "_re.cg.qoq.im")
        == "5d1e868246a4c66ee2eb1b9c035fda400068865b1d29aef8df4bdbe351e4a199"; // 原校验值保持。
    VM_HARD_END
    return accepted;
}

} // namespace cg::platform
