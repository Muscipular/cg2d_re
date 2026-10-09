#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::detail {

// 原53C2B4：同址不读写，其他情况按完整对象表示交换，保留未消费字节与原生指针。
template <typename T>
void SwapLegacyRecord_53C2B4(T* const left, T* const right)
{
    static_assert(std::is_trivially_copyable_v<T>);
    if (left == right) {
        return;
    }
    auto* const leftBytes = reinterpret_cast<unsigned char*>(left);
    auto* const rightBytes = reinterpret_cast<unsigned char*>(right);
    for (std::size_t index = 0; index < sizeof(T); ++index) {
        const unsigned char rightByte = rightBytes[index];
        const unsigned char leftByte = leftBytes[index];
        leftBytes[index] = rightByte;
        rightBytes[index] = leftByte;
    }
}

// 原53C112/53C266：当前仅供52FCC0公会索引，非一致比较合同沿MC01-C6待决。
// 动画/渲染已沿MC01-C7采用std::qsort；本组仍保原比较、分区及完整字节交换。
template <typename T>
void SortLegacyRecords_53C112(
    T* const base, const std::uint32_t count,
    std::int32_t (*compare)(const void*, const void*))
{
    if (count < 2) {
        return;
    }
    T* first = base;
    T* last = first + count - 1;
    std::array<T*, 30> firstStack;
    std::array<T*, 30> lastStack;
    std::int32_t stackSize = 0;
    for (;;) {
        const auto length = last - first + 1;
        if (length <= 8) {
            while (last > first) {
                T* maximum = first;
                for (T* item = first + 1; item <= last; ++item) {
                    if (compare(item, maximum) > 0) {
                        maximum = item;
                    }
                }
                SwapLegacyRecord_53C2B4(maximum, last);
                --last;
            }
        } else {
            SwapLegacyRecord_53C2B4(first + length / 2, first);
            T* left = first;
            T* right = last + 1;
            for (;;) {
                do {
                    ++left;
                } while (left <= last && compare(left, first) <= 0);
                do {
                    --right;
                } while (right > first && compare(right, first) >= 0);
                if (right < left) {
                    break;
                }
                SwapLegacyRecord_53C2B4(left, right);
            }
            SwapLegacyRecord_53C2B4(first, right);
            // 原(right-first)字节差减1后比较，整项距离必须用严格大于。
            if (right - first > last - left) {
                if (first + 1 < right) {
                    firstStack[stackSize] = first;
                    lastStack[stackSize] = right - 1;
                    ++stackSize;
                }
                if (left < last) {
                    first = left;
                    continue;
                }
            } else {
                if (left < last) {
                    firstStack[stackSize] = left;
                    lastStack[stackSize] = last;
                    ++stackSize;
                }
                if (first + 1 < right) {
                    last = right - 1;
                    continue;
                }
            }
        }
        if (--stackSize < 0) {
            return;
        }
        first = firstStack[stackSize];
        last = lastStack[stackSize];
    }
}

} // namespace cg::detail
