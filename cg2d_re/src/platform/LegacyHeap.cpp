#include "LegacyHeap.hpp"
#include "cg/platform/LegacyMemory.hpp"

#include <cstdlib>
#include <malloc.h>

namespace cg::platform {

// 沿既有.data虚拟零合同；成功初始化后只借用CRT句柄，不拥有或销毁它。
HANDLE gLegacyHeap10869A4 = nullptr;
std::int32_t gLegacyHeapMode10869A8 = 0;
LegacyHeapNewHandler gLegacyHeapNewHandler108614C = nullptr;
std::int32_t gLegacyHeapNewMode1086150 = 0;

std::int32_t InitializeLegacyHeap_541BB8(std::int32_t /*serialized*/)
{
    // HMU01：同步/堆生命周期归宿主；保两caller的实际返回与错误28消费。
    const HANDLE heap = reinterpret_cast<HANDLE>(::_get_heap_handle());
    gLegacyHeap10869A4 = heap;
    if (heap == nullptr) {
        return 0;
    }
    gLegacyHeapMode10869A8 = SelectLegacyHeapMode_541A70();
    return 1;
}

// 只接受本CRT malloc/calloc/realloc块；真实容量保持原U32返回，不读旧块前缀。
std::uint32_t GetLegacyAllocationSize_53FCFB(void* const memory)
{
    return static_cast<std::uint32_t>(::_msize(memory));
}

std::int32_t CallLegacyHeapNewHandler_541A28(const std::uint32_t byteCount)
{
    const auto handler = gLegacyHeapNewHandler108614C;
    return handler != nullptr && handler(byteCount) != 0 ? 1 : 0;
}

void* AllocateLegacyHeapBlock_53C3EA(std::uint32_t byteCount)
{
    // 原mode3最小1B；mode1/2仍向16对齐，但全部交同一CRT分配器。
    if (gLegacyHeapMode10869A8 == 3) {
        return std::malloc(byteCount == 0 ? 1u : byteCount);
    }
    if (byteCount == 0) {
        byteCount = 1;
    }
    byteCount = (byteCount + 15u) & 0xFFFFFFF0u;
    return std::malloc(byteCount);
}

void* AllocateLegacyHeapWithHandler_53C3BE(
    const std::uint32_t byteCount, const std::int32_t enableHandler)
{
    if (byteCount > 0xFFFFFFE0u) {
        return nullptr;
    }
    for (;;) {
        void* const result = AllocateLegacyHeapBlock_53C3EA(byteCount);
        if (result != nullptr || enableHandler == 0) {
            return result;
        }
        if (CallLegacyHeapNewHandler_541A28(byteCount) == 0) {
            return nullptr;
        }
    }
}

void* AllocateLegacyHeapMemory_53C3AC(const std::uint32_t byteCount)
{
    const std::int32_t mode = gLegacyHeapNewMode1086150;
    return AllocateLegacyHeapWithHandler_53C3BE(byteCount, mode);
}

void FreeLegacyHeapMemory_53C0A9(void* const memory)
{
    if (memory == nullptr) {
        return;
    }
    std::free(memory);
}

void* CallocateLegacyMemory_53BFF8(
    const std::uint32_t count, const std::uint32_t size)
{
    const std::uint32_t original = count * size;
    std::uint32_t bytes = original;
    if (bytes <= 0xFFFFFFE0u) {
        if (bytes == 0) {
            bytes = 1;
        }
        bytes = (bytes + 15u) & 0xFFFFFFF0u;
    }
    for (;;) {
        void* result = nullptr;
        if (bytes <= 0xFFFFFFE0u) {
            result = std::calloc(1u, bytes);
            if (result != nullptr) {
                return result;
            }
        }
        if (gLegacyHeapNewMode1086150 == 0) {
            return result;
        }
        if (CallLegacyHeapNewHandler_541A28(bytes) == 0) {
            return nullptr;
        }
    }
}

void* ReallocateLegacyMemory_53FA5B(void* const memory, std::uint32_t byteCount)
{
    if (memory == nullptr) {
        return AllocateLegacyHeapMemory_53C3AC(byteCount);
    }
    if (byteCount == 0) {
        FreeLegacyHeapMemory_53C0A9(memory);
        return nullptr;
    }
    // 保mode1/2原物理数量与失败handler参数；mode3仍消费未改输入。
    if (gLegacyHeapMode10869A8 != 3 && byteCount <= 0xFFFFFFE0u) {
        byteCount = (byteCount + 15u) & 0xFFFFFFF0u;
    }
    for (;;) {
        void* result = nullptr;
        if (byteCount <= 0xFFFFFFE0u) {
            result = std::realloc(memory, byteCount);
            if (result != nullptr) {
                return result;
            }
        }
        if (gLegacyHeapNewMode1086150 == 0) {
            return result;
        }
        if (CallLegacyHeapNewHandler_541A28(byteCount) == 0) {
            return nullptr;
        }
    }
}

} // namespace cg::platform
