#include "LegacyRuntimeExit.hpp"
#include "LegacyRuntimeError.hpp"
#include "StartupNative.hpp"

#include <cstddef>

namespace cg::platform {

namespace {

// 原5405F5：仅按异常代码筛选，命中原C++代码即进入原终止，不加参数数目/版本检查。
std::int32_t FilterLegacyArrayUnwindException_5405F5(_EXCEPTION_POINTERS* const information)
{
    if (information->ExceptionRecord->ExceptionCode == 0xE06D7363u) {
        TerminateLegacyRuntime_540A5C();
    }
    return 0;
}

} // namespace

void UnwindLegacyArray_53C560(
    void* const onePast, const std::uint32_t elementBytes, const std::int32_t elementCount,
    const LegacyArrayElementCallback destructor)
{
    auto* current = static_cast<std::byte*>(onePast);
    std::int32_t remaining = elementCount;
    __try {
        for (;;) {
            --remaining;
            if (remaining < 0) {
                break;
            }
            current -= elementBytes;
            destructor(current);
        }
    } __except (FilterLegacyArrayUnwindException_5405F5(GetExceptionInformation())) {
        // 原53C5A6仅恢复本帧栈并进入尾声；筛选正常只返回0或终止，无追加业务动作。
    }
}

void ConstructLegacyArray_53C45E(
    void* const storage, const std::uint32_t elementBytes, const std::int32_t elementCount,
    const LegacyArrayElementCallback constructor, const LegacyArrayElementCallback destructor)
{
    auto* current = static_cast<std::byte*>(storage);
    std::int32_t completed = 0;
    std::uint32_t complete = 0;
    __try {
        while (completed < elementCount) {
            constructor(current);
            current += elementBytes;
            ++completed;
        }
        complete = 1;
    } __finally {
        // 原53C4C8共享finally：只在未完成时展开已完成前缀，当前失败元素不计入。
        if (complete == 0) {
            UnwindLegacyArray_53C560(current, elementBytes, completed, destructor);
        }
    }
}

void DestroyLegacyArray_53C4E0(
    void* const storage, const std::uint32_t elementBytes, const std::int32_t elementCount,
    const LegacyArrayElementCallback destructor)
{
    // 原imul只保留W32乘积，随后才作用于地址；数量/步长不按指针扩宽。
    const std::uint32_t endOffset = elementBytes * elementCount;
    auto* current = static_cast<std::byte*>(storage) + endOffset;
    std::int32_t remaining = elementCount;
    std::uint32_t complete = 0;
    __try {
        for (;;) {
            --remaining;
            if (remaining < 0) {
                break;
            }
            current -= elementBytes;
            destructor(current);
        }
        complete = 1;
    } __finally {
        // 原53C548共享finally：失败对象地址和已递减剩余数交展开，不重试失败析构。
        if (complete == 0) {
            UnwindLegacyArray_53C560(
                current, elementBytes, remaining, destructor);
        }
    }
}

} // namespace cg::platform
