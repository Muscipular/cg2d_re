#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原streambuf已证+00..+33视图：真实指针拓宽，余量S32；不含未决locale/派生字段。
// 不自动初始化/构析/分配，不接宿主streambuf；六间接槽可指向自身或实际外部字段。
template <typename Character>
struct LegacyStreamBufferStorage final {
    const void* virtualTable;             // 原+00；当前只保留地址，不路由未决虚表。
    Character* readBase;                  // 原+04。
    Character* writeBase;                 // 原+08。
    Character** readBaseSlot;             // 原+0C。
    Character** writeBaseSlot;            // 原+10。
    Character* readCurrent;               // 原+14。
    Character* writeCurrent;              // 原+18。
    Character** readCurrentSlot;          // 原+1C。
    Character** writeCurrentSlot;         // 原+20。
    std::int32_t readRemaining;           // 原+24；字符数。
    std::int32_t writeRemaining;          // 原+28；字符数。
    std::int32_t* readRemainingSlot;      // 原+2C。
    std::int32_t* writeRemainingSlot;     // 原+30。
};
using LegacyNarrowStreamBuffer = LegacyStreamBufferStorage<char>;
using LegacyWideStreamBuffer = LegacyStreamBufferStorage<std::uint16_t>;
static_assert(offsetof(LegacyNarrowStreamBuffer, readCurrentSlot) == 56);
static_assert(offsetof(LegacyNarrowStreamBuffer, writeCurrentSlot) == 64);
static_assert(offsetof(LegacyNarrowStreamBuffer, readRemaining) == 72);
static_assert(offsetof(LegacyNarrowStreamBuffer, writeRemaining) == 76);
static_assert(offsetof(LegacyNarrowStreamBuffer, readRemainingSlot) == 80);
static_assert(offsetof(LegacyNarrowStreamBuffer, writeRemainingSlot) == 88);
static_assert(sizeof(LegacyNarrowStreamBuffer) == 96);
static_assert(offsetof(LegacyWideStreamBuffer, readCurrentSlot) == 56);
static_assert(offsetof(LegacyWideStreamBuffer, readRemainingSlot) == 80);
static_assert(sizeof(LegacyWideStreamBuffer) == 96);

// 原Gndec返回移动后的指针，Gninc/Pninc返回移动前指针，不校验余量/空槽。
char* DecrementLegacyReadCursor_548EA5(LegacyNarrowStreamBuffer* storage);
char* IncrementLegacyReadCursor_548EB5(LegacyNarrowStreamBuffer* storage);
char* IncrementLegacyWriteCursor_548EC5(LegacyNarrowStreamBuffer* storage);
std::uint16_t* DecrementLegacyWideReadCursor_549E1C(LegacyWideStreamBuffer* storage);
std::uint16_t* IncrementLegacyWideReadCursor_549E2D(LegacyWideStreamBuffer* storage);
std::uint16_t* IncrementLegacyWideWriteCursor_549E51(LegacyWideStreamBuffer* storage);
// 原549E08/549E3D；余量按S32字符数扣减，U16*按2B字符步长推进。
void BumpLegacyWideReadCursor_549E08(
    LegacyWideStreamBuffer* storage, std::int32_t characterCount);
void BumpLegacyWideWriteCursor_549E3D(
    LegacyWideStreamBuffer* storage, std::int32_t characterCount);

// 原549EDE窄宽共用重绑定/清六值；只实例化已证char/U16，实际owner另行接线。
template <typename Character>
void ResetLegacyStreamBuffer_549EDE(LegacyStreamBufferStorage<Character>* storage);

} // namespace cg::platform
