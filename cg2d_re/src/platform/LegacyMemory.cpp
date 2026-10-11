#include "cg/platform/LegacyMemory.hpp"

#include <cstring>
#include <cstdlib>

#include <malloc.h>

namespace cg::platform {

// MC01-C4：直接查询真实CRT块，容量沿现U32接口消费，不读旧块头。
// 原函数：0x0053FCFB。
std::uint32_t GetLegacyAllocationSize_53FCFB(void* const memory)
{
    return static_cast<std::uint32_t>(::_msize(memory));
}

// 原U32 count/size透传；数量校验、零长度与new-handler采用MSVC合同。
// 原函数：0x0053BFF8。
void* CallocateLegacyMemory_53BFF8(
    const std::uint32_t count, const std::uint32_t size)
{
    return std::calloc(count, size);
}

// 空/零及失败旧块策略由同域MSVC realloc处理，不预对齐或手工重试。
// 原函数：0x0053FA5B。
void* ReallocateLegacyMemory_53FA5B(void* const memory, const std::uint32_t byteCount)
{
    return std::realloc(memory, byteCount);
}

// 原函数：0x00541620。
void* CopyLegacyMemory_541620(void* const destination,
    const void* const source, const std::uint32_t byteCount)
{
    return std::memmove(destination, source, byteCount);
}

// MC01：填充直接交MSVC memset；值、数量和返回沿调用方接口。
// 原函数：0x005419D0。
void* FillLegacyMemory_5419D0(
    void* const destination, const std::int32_t value, std::uint32_t byteCount)
{
    return std::memset(destination, value, byteCount);
}

// MC01：比较直接交MSVC memcmp，消费其实际有符号结果。
// 原函数：0x0054B160。
std::int32_t CompareLegacyMemory_54B160(
    const void* const leftBuffer, const void* const rightBuffer, std::uint32_t byteCount)
{
    return std::memcmp(leftBuffer, rightBuffer, byteCount);
}

void* AllocateLegacyByteBuffer(const std::uint32_t byteCount)
{
    return std::malloc(byteCount);
}

void FreeLegacyByteBuffer(void* const memory) noexcept
{
    std::free(memory);
}

} // namespace cg::platform
