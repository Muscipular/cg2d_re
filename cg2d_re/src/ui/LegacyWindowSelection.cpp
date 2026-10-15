#include "cg/ui/LegacyWindowSelection.hpp"

#include "../world/WorldUpdateDependencies.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::ui {
namespace {

// 原 [0x0057C368,0x0057CB24)：99 项旧窗口记录，原 x86 步长 0x14。
// 旧handler层已废弃；仅保留仍被调用的选择／对象状态，见LWR01。
struct LegacyWindowRecord final {
    std::uint32_t id; // 原 +0x00：旧窗口编号，与现代 WindowId 独立。
    std::int8_t category; // 原 +0x08：0/1 活动槽，2 为两槽互斥类别。
    std::array<std::uint8_t, 3> reserved09 {}; // 原 +0x09：未定字节。
    login::YobiAction* runtimeAction = nullptr; // 原 +0x0C：503160 维护的窗口对象。
    std::uint8_t dirty = 0; // 原 +0x10：504180 写入，503160 消费后清零。
    std::array<std::uint8_t, 3> reserved11 {}; // 原 +0x11：未定字节。
};

static_assert(offsetof(LegacyWindowRecord, id) == 0);
static_assert(offsetof(LegacyWindowRecord, category) == 4);
static_assert(sizeof(decltype(LegacyWindowRecord::category)) == 1);
static_assert(offsetof(LegacyWindowRecord, reserved09) == 5);
static_assert(offsetof(LegacyWindowRecord, runtimeAction) == 8);
static_assert(offsetof(LegacyWindowRecord, dirty) == 16);
static_assert(offsetof(LegacyWindowRecord, reserved11) == 17);
static_assert(sizeof(LegacyWindowRecord) == 24);

std::array<LegacyWindowRecord, 99> gLegacyWindowRecords57C368 {{ // 原全局：0x0057C368。
    {0, 2},
    {1, 1},
    {2, 0},
    {18, 0},
    {19, 0},
    {14, 1},
    {15, 1},
    {16, 1},
    {20, 0},
    {21, 0},
    {22, 0},
    {25, 0},
    {26, 0},
    {27, 1},
    {29, 0},
    {30, 1},
    {28, 0},
    {31, 1},
    {32, 1},
    {33, 0},
    {34, 2},
    {35, 2},
    {36, 2},
    {37, 0},
    {38, 0},
    {39, 0},
    {40, 2},
    {41, 2},
    {42, 0},
    {43, 0},
    {44, 0},
    {45, 0},
    {46, 0},
    {47, 0},
    {48, 0},
    {49, 0},
    {50, 1},
    {94, 0},
    {93, 0},
    {92, 0},
    {91, 0},
    {90, 0},
    {89, 0},
    {88, 0},
    {87, 0},
    {86, 1},
    {95, 0},
    {96, 0},
    {51, 2},
    {52, 2},
    {23, 0},
    {24, 0},
    {3, 0},
    {4, 0},
    {5, 0},
    {6, 0},
    {7, 0},
    {8, 0},
    {9, 0},
    {10, 0},
    {11, 0},
    {12, 0},
    {17, 1},
    {53, 2},
    {54, 2},
    {55, 2},
    {56, 2},
    {57, 2},
    {62, 2},
    {63, 2},
    {65, 2},
    {64, 2},
    {66, 2},
    {67, 2},
    {68, 2},
    {69, 2},
    {70, 2},
    {71, 2},
    {72, 2},
    {73, 2},
    {58, 2},
    {59, 2},
    {60, 2},
    {61, 2},
    {74, 2},
    {75, 2},
    {76, 2},
    {77, 2},
    {78, 2},
    {79, 1},
    {80, 1},
    {81, 2},
    {82, 2},
    {83, 2},
    {84, 2},
    {85, 2},
    {98, 2},
    {99, 2},
    {100, 2},
}};
std::array<std::uint32_t, 3> gLegacyWindowSelectionCC236C {}; // 原全局：0x00CC236C。
static_assert(sizeof(gLegacyWindowRecords57C368) == 99 * sizeof(LegacyWindowRecord));
static_assert(sizeof(gLegacyWindowSelectionCC236C) == 3 * sizeof(std::uint32_t));

} // namespace

// 原 [0x5044C0,0x5044DF)：顺序扫描完整的 99 项可写表。
std::int32_t __cdecl Sub_5044C0(const std::uint32_t id)
{
    for (std::size_t index = 0; index < gLegacyWindowRecords57C368.size(); ++index) {
        if (gLegacyWindowRecords57C368[index].id == id) {
            return static_cast<std::int32_t>(index);
        }
    }
    return -1;
}

login::YobiAction*& BorrowLegacyPanelAction57C374(const std::int32_t index) noexcept
{
    return gLegacyWindowRecords57C368[index].runtimeAction;
}

std::uint8_t& BorrowLegacyPanelDirty57C378(const std::int32_t index) noexcept
{
    return gLegacyWindowRecords57C368[index].dirty;
}

// 原函数：0x00504460。
std::int32_t Sub_504460(const std::uint32_t id)
{
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return -1;
    }
    if (gLegacyWindowRecords57C368[index].runtimeAction == nullptr) {
        static_cast<void>(Sub_504180(id));
        return 1;
    }
    static_cast<void>(Sub_5042D0(id));
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 0;
}

// 原 [0x504410,0x50445F)：类别 2 读首槽，其他类别以有符号 W8 索引共享槽。
std::int32_t __cdecl Sub_504410(const std::uint32_t id)
{
    if (id == 0 || id >= 100) {
        return 0;
    }
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    const std::int8_t category =
        gLegacyWindowRecords57C368[static_cast<std::size_t>(index)].category;
    if (category == 2) {
        return gLegacyWindowSelectionCC236C[0] == id;
    }
    return gLegacyWindowSelectionCC236C[static_cast<std::size_t>(category)] == id;
}

// 原 [0x504180,0x5042C6)：按表中可写类别发布选择与标记。
std::int32_t __cdecl Sub_504180(const std::uint32_t id)
{
    if (world::gCurrentSceneType == 10
        && (id == 1 || id == 2 || id == 18 || id == 14
            || id == 20 || id == 17)) {
        return 0;
    }
    if (id == 0 || id >= 100) {
        return 0;
    }
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    LegacyWindowRecord& record =
        gLegacyWindowRecords57C368[static_cast<std::size_t>(index)];
    const std::int8_t categoryByte = record.category;
    if (categoryByte != 0 && world::detail::gValueCAFEA0 == 0) {
        world::detail::gValueCAFEA0 =
            static_cast<std::uint32_t>(Sub_504410(1));
    }

    const std::int8_t categoryGate = record.category;
    const std::uint32_t previousSelection = gLegacyWindowSelectionCC236C[0];
    if (categoryGate == 2) {
        if (previousSelection == id) {
            return 0;
        }
        gLegacyWindowSelectionCC236C[0] = id;
        record.dirty = 1;
        gLegacyWindowSelectionCC236C[1] = 0;
        return 1;
    }

    const std::int32_t previousIndex = Sub_5044C0(previousSelection);
    if (previousIndex < 0) {
        return 0;
    }
    if (gLegacyWindowSelectionCC236C[0] != 0
        && gLegacyWindowRecords57C368[static_cast<std::size_t>(previousIndex)].category
            == 2) {
        const std::int8_t category = record.category;
        gLegacyWindowSelectionCC236C[0] = 0;
        gLegacyWindowSelectionCC236C[1] = 0;
        gLegacyWindowSelectionCC236C[static_cast<std::size_t>(category)] = id;
        record.dirty = 1;
        return 1;
    }

    const std::int8_t category = record.category;
    if (gLegacyWindowSelectionCC236C[static_cast<std::size_t>(category)] == id) {
        return 0;
    }
    gLegacyWindowSelectionCC236C[static_cast<std::size_t>(category)] = id;
    record.dirty = 1;
    return 1;
}

// 原 [0x504AF0,0x504B0E)：先采现值，非零时重入 504180 后才清槽。
std::int32_t __cdecl Sub_504AF0()
{
    const std::uint32_t current = world::detail::gValueCAFEA0;
    if (current == 0) {
        return 0;
    }
    const std::int32_t result = Sub_504180(1);
    world::detail::gValueCAFEA0 = 0;
    return result;
}

// 原 [0x5042D0,0x504373)：保留编号 100、类别 2 的不同返回与清理路径。
std::int32_t __cdecl Sub_5042D0(const std::uint32_t id)
{
    const std::uint32_t previousSecond = gLegacyWindowSelectionCC236C[1];
    if (id == 100) {
        gLegacyWindowSelectionCC236C[0] = 0;
        gLegacyWindowSelectionCC236C[1] = 0;
        if (previousSecond != 1 && (platform::gKeyStates[27] & 4u) != 0) {
            static_cast<void>(Sub_504AF0());
        }
        return 1;
    }
    if (id == 0 || id > 100) {
        return 0;
    }
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    const std::int8_t category =
        gLegacyWindowRecords57C368[static_cast<std::size_t>(index)].category;
    if (category == 2 && gLegacyWindowSelectionCC236C[0] == id) {
        gLegacyWindowSelectionCC236C[0] = 0;
        gLegacyWindowSelectionCC236C[1] = 0;
        if (previousSecond != 1) {
            static_cast<void>(Sub_504AF0());
        }
        return 0;
    }
    if (gLegacyWindowSelectionCC236C[static_cast<std::size_t>(category)] != id) {
        return 0;
    }
    gLegacyWindowSelectionCC236C[static_cast<std::size_t>(category)] = 0;
    if (previousSecond != 1 && category != 0) {
        static_cast<void>(Sub_504AF0());
    }
    return 1;
}

// 原 [0x504380,0x5043C6)：分别比较首两槽，不合并两次写入。
std::int32_t __cdecl Sub_504380(const std::uint32_t id)
{
    if (id == 0 || id >= 100) {
        return 0;
    }
    std::int32_t result = 0;
    if (gLegacyWindowSelectionCC236C[0] != id) {
        gLegacyWindowSelectionCC236C[0] = 0;
        result = 1;
    }
    if (gLegacyWindowSelectionCC236C[1] != id) {
        gLegacyWindowSelectionCC236C[1] = 0;
        result = 1;
    }
    return result;
}

// 原 [0x5043D0,0x504404)：索引 2 单独查首两槽，其余读同一连续存储。
std::int32_t __cdecl Sub_5043D0(const std::int32_t index)
{
    if (index == 2) {
        return gLegacyWindowSelectionCC236C[0] != 0
            || gLegacyWindowSelectionCC236C[1] != 0;
    }
    return gLegacyWindowSelectionCC236C[static_cast<std::size_t>(index)] != 0;
}

std::uint32_t ReadLegacyPanelSelectionCC236C() noexcept
{
    return gLegacyWindowSelectionCC236C[0];
}

std::uint32_t ReadLegacyPanelSelectionCC2370() noexcept
{
    return gLegacyWindowSelectionCC236C[1];
}

} // namespace cg::ui
