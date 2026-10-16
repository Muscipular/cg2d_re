#include "cg/platform/platformTool.hpp"
#include <cstdlib>

#include <winsock2.h>
#include <iphlpapi.h>
#include <ipifcons.h>

#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstring>
#include <new>

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
    ULONG selectedType;
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

} // namespace cg::platform
