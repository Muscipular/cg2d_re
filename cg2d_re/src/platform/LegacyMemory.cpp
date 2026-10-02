#include "cg/platform/LegacyMemory.hpp"

#include <malloc.h>
#include <new.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace cg::platform {

void* AllocateLegacyByteBuffer(const std::uint32_t byteCount)
{
    // 原53C3BE先拒绝该范围，不调用分配器或new-handler。
    if (byteCount > 0xFFFFFFE0u) {
        return nullptr;
    }
    // 原系统堆路径不清零；边界检查使以下U32对齐运算不溢出。
    const std::uint32_t requested = byteCount == 0 ? 1u : byteCount;
    const std::uint32_t capacity = (requested + 15u) & ~15u;
    for (;;) {
        void* const memory = ::HeapAlloc(
            reinterpret_cast<HANDLE>(::_get_heap_handle()), 0, capacity);
        if (memory != nullptr) {
            return memory;
        }

        // 原541A28每次重读处理器，传原请求长度；不吞处理器抛出的异常。
        const _PNH handler = ::_query_new_handler();
        if (handler == nullptr || handler(byteCount) == 0) {
            return nullptr;
        }
    }
}

void FreeLegacyByteBuffer(void* const memory) noexcept
{
    if (memory != nullptr) {
        static_cast<void>(::HeapFree(
            reinterpret_cast<HANDLE>(::_get_heap_handle()), 0, memory));
    }
}

} // namespace cg::platform
