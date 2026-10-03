#include "cg/platform/LegacyNetworkAdapter.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "LegacyFormat.hpp"

#include <winsock2.h>
#include <iphlpapi.h>
#include <ipifcons.h>

#include <cstdio>
#include <cstring>
#include <new>

namespace cg::platform {

std::int32_t Sub_534EF0(char* const output)
{
    ULONG size = 0;
    const ULONG firstResult =
        ::GetAdaptersAddresses(AF_INET, 0, nullptr, nullptr, &size);
    char ethernetText[16] {};
    char wirelessText[16] {};
    if (firstResult != ERROR_BUFFER_OVERFLOW) {
        return -1;
    }

    void* const allocation = AllocateLegacyByteBuffer(static_cast<std::uint32_t>(size));
    auto* const adapters = static_cast<IP_ADAPTER_ADDRESSES*>(allocation);
    const ULONG secondResult =
        ::GetAdaptersAddresses(AF_INET, 0, nullptr, adapters, &size);
    if (secondResult != NO_ERROR) {
        FreeLegacyByteBuffer(allocation);
        return -1;
    }

    const IP_ADAPTER_ADDRESSES* selected = adapters;
    while (selected != nullptr) {
        if (selected->OperStatus == IfOperStatusUp
            && (selected->IfType == IF_TYPE_ETHERNET_CSMACD
                || selected->IfType == IF_TYPE_IEEE80211)) {
            break;
        }
        selected = selected->Next;
    }
    if (selected == nullptr) {
        FreeLegacyByteBuffer(allocation);
        return -1;
    }

    char* const text = selected->IfType == IF_TYPE_ETHERNET_CSMACD
        ? ethernetText : wirelessText;
    const BYTE* const address = selected->PhysicalAddress;
    FormatLegacyString_53B304(
        text,
        "%02x%02x%02x%02x%02x%02x",
        address[0], address[1], address[2],
        address[3], address[4], address[5]);

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
