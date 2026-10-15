#include "cg/ui/LegacyWindowSelection.hpp"

#include "../LegacyPanelRuntime.hpp"
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
// handler 绑定实际处理器；运行对象由 runtimeAction 非拥有借用。
struct LegacyWindowRecord final {
    std::uint32_t id; // 原 +0x00：旧窗口编号，与现代 WindowId 独立。
    LegacyWindowHandler handler; // 原 +0x04：旧窗口处理器，绑定见表内原地址。
    std::int8_t category; // 原 +0x08：0/1 活动槽，2 为两槽互斥类别。
    std::array<std::uint8_t, 3> reserved09 {}; // 原 +0x09：未定字节。
    login::YobiAction* runtimeAction = nullptr; // 原 +0x0C：503160 维护的窗口对象。
    std::uint8_t dirty = 0; // 原 +0x10：504180 写入，503160 消费后清零。
    std::array<std::uint8_t, 3> reserved11 {}; // 原 +0x11：未定字节。
};

static_assert(offsetof(LegacyWindowRecord, id) == 0);
static_assert(offsetof(LegacyWindowRecord, handler) == 8);
static_assert(offsetof(LegacyWindowRecord, category) == 16);
static_assert(sizeof(decltype(LegacyWindowRecord::category)) == 1);
static_assert(offsetof(LegacyWindowRecord, reserved09) == 17);
static_assert(offsetof(LegacyWindowRecord, runtimeAction) == 24);
static_assert(offsetof(LegacyWindowRecord, dirty) == 32);
static_assert(offsetof(LegacyWindowRecord, reserved11) == 33);
static_assert(sizeof(LegacyWindowRecord) == 40);

// 旧处理器分别返回 S32/U32/原生地址：整数保留原低 W32，地址保留宿主全宽。
// 仅适配函数类型，不转换函数指针，不改变处理器调用次数或 opened 实参。
template <auto Handler>
std::uintptr_t InvokeLegacyWindowHandler(const std::int32_t opened)
{
    const auto result = Handler(opened);
    if constexpr (sizeof(result) == sizeof(std::uint32_t)) {
        return static_cast<std::uint32_t>(result);
    } else {
        return result;
    }
}

std::array<LegacyWindowRecord, 99> gLegacyWindowRecords57C368 {{ // 原全局：0x0057C368。
    {0, nullptr, 2},
    {1, InvokeLegacyWindowHandler<login::detail::Sub_5029B0>, 1}, // 原回调：0x005029B0。
    {2, InvokeLegacyWindowHandler<login::detail::Sub_5006F0>, 0}, // 原回调：0x005006F0。
    {18, InvokeLegacyWindowHandler<login::detail::Sub_500370>, 0}, // 原回调：0x00500370。
    {19, InvokeLegacyWindowHandler<login::detail::Sub_4FF650>, 0}, // 原回调：0x004FF650。
    {14, InvokeLegacyWindowHandler<login::detail::Sub_4FAEF0>, 1}, // 原回调：0x004FAEF0。
    {15, InvokeLegacyWindowHandler<login::detail::Sub_4FBB40>, 1}, // 原回调：0x004FBB40。
    {16, InvokeLegacyWindowHandler<login::detail::Sub_4F0F70>, 1}, // 原回调：0x004F0F70。
    {20, InvokeLegacyWindowHandler<login::detail::Sub_4F7150>, 0}, // 原回调：0x004F7150。
    {21, InvokeLegacyWindowHandler<login::detail::Sub_4F83B0>, 0}, // 原回调：0x004F83B0。
    {22, InvokeLegacyWindowHandler<login::detail::Sub_4FA050>, 0}, // 原回调：0x004FA050。
    {25, InvokeLegacyWindowHandler<login::detail::Sub_4F5A60>, 0}, // 原回调：0x004F5A60。
    {26, InvokeLegacyWindowHandler<login::detail::Sub_4F51D0>, 0}, // 原回调：0x004F51D0。
    {27, InvokeLegacyWindowHandler<login::detail::Sub_4F4790>, 1}, // 原回调：0x004F4790。
    {29, InvokeLegacyWindowHandler<login::detail::Sub_505C20>, 0}, // 原回调：0x00505C20。
    {30, InvokeLegacyWindowHandler<login::detail::Sub_505170>, 1}, // 原回调：0x00505170。
    {28, InvokeLegacyWindowHandler<login::detail::Sub_4F3ED0>, 0}, // 原回调：0x004F3ED0。
    {31, InvokeLegacyWindowHandler<login::detail::Sub_4F2A30>, 1}, // 原回调：0x004F2A30。
    {32, InvokeLegacyWindowHandler<login::detail::Sub_4F2130>, 1}, // 原回调：0x004F2130。
    {33, InvokeLegacyWindowHandler<login::detail::Sub_4F1C40>, 0}, // 原回调：0x004F1C40。
    {34, InvokeLegacyWindowHandler<login::detail::Sub_4F0B40>, 2}, // 原回调：0x004F0B40。
    {35, InvokeLegacyWindowHandler<login::detail::Sub_4F0460>, 2}, // 原回调：0x004F0460。
    {36, InvokeLegacyWindowHandler<login::detail::Sub_4EFB70>, 2}, // 原回调：0x004EFB70。
    {37, InvokeLegacyWindowHandler<login::detail::Sub_4EF0F0>, 0}, // 原回调：0x004EF0F0。
    {38, InvokeLegacyWindowHandler<login::detail::Sub_4EE380>, 0}, // 原回调：0x004EE380。
    {39, InvokeLegacyWindowHandler<login::detail::Sub_4EDB20>, 0}, // 原回调：0x004EDB20。
    {40, InvokeLegacyWindowHandler<login::detail::Sub_4ED020>, 2}, // 原回调：0x004ED020。
    {41, InvokeLegacyWindowHandler<login::detail::Sub_4ECE00>, 2}, // 原回调：0x004ECE00。
    {42, InvokeLegacyWindowHandler<login::detail::Sub_4EC240>, 0}, // 原回调：0x004EC240。
    {43, InvokeLegacyWindowHandler<login::detail::Sub_4EBD20>, 0}, // 原回调：0x004EBD20。
    {44, InvokeLegacyWindowHandler<login::detail::Sub_4EBAA0>, 0}, // 原回调：0x004EBAA0。
    {45, InvokeLegacyWindowHandler<login::detail::Sub_4EB890>, 0}, // 原回调：0x004EB890。
    {46, InvokeLegacyWindowHandler<login::detail::Sub_4EB1C0>, 0}, // 原回调：0x004EB1C0。
    {47, InvokeLegacyWindowHandler<login::detail::Sub_4EAAA0>, 0}, // 原回调：0x004EAAA0。
    {48, InvokeLegacyWindowHandler<login::detail::Sub_4EA690>, 0}, // 原回调：0x004EA690。
    {49, InvokeLegacyWindowHandler<login::detail::Sub_4EA1B0>, 0}, // 原回调：0x004EA1B0。
    {50, InvokeLegacyWindowHandler<login::detail::Sub_4E9C80>, 1}, // 原回调：0x004E9C80。
    {94, InvokeLegacyWindowHandler<login::detail::Sub_52FD60>, 0}, // 原回调：0x0052FD60。
    {93, InvokeLegacyWindowHandler<login::detail::Sub_52F910>, 0}, // 原回调：0x0052F910。
    {92, InvokeLegacyWindowHandler<login::detail::Sub_52F670>, 0}, // 原回调：0x0052F670。
    {91, InvokeLegacyWindowHandler<login::detail::Sub_52F460>, 0}, // 原回调：0x0052F460。
    {90, InvokeLegacyWindowHandler<login::detail::Sub_52ED50>, 0}, // 原回调：0x0052ED50。
    {89, InvokeLegacyWindowHandler<login::detail::Sub_52E650>, 0}, // 原回调：0x0052E650。
    {88, InvokeLegacyWindowHandler<login::detail::Sub_52E240>, 0}, // 原回调：0x0052E240。
    {87, InvokeLegacyWindowHandler<login::detail::Sub_52DCE0>, 0}, // 原回调：0x0052DCE0。
    {86, InvokeLegacyWindowHandler<login::detail::Sub_52D7B0>, 1}, // 原回调：0x0052D7B0。
    {95, InvokeLegacyWindowHandler<login::detail::Sub_530BA0>, 0}, // 原回调：0x00530BA0。
    {96, InvokeLegacyWindowHandler<login::detail::Sub_532450>, 0}, // 原回调：0x00532450。
    {51, InvokeLegacyWindowHandler<login::detail::Sub_4E4EA0>, 2}, // 原回调：0x004E4EA0。
    {52, InvokeLegacyWindowHandler<login::detail::Sub_4E5E40>, 2}, // 原回调：0x004E5E40。
    {23, InvokeLegacyWindowHandler<login::detail::Sub_4F6CA0>, 0}, // 原回调：0x004F6CA0。
    {24, InvokeLegacyWindowHandler<login::detail::Sub_4F6220>, 0}, // 原回调：0x004F6220。
    {3, InvokeLegacyWindowHandler<login::detail::Sub_4FC730>, 0}, // 原回调：0x004FC730。
    {4, InvokeLegacyWindowHandler<login::detail::Sub_4FC8F0>, 0}, // 原回调：0x004FC8F0。
    {5, InvokeLegacyWindowHandler<login::detail::Sub_4FD1A0>, 0}, // 原回调：0x004FD1A0。
    {6, InvokeLegacyWindowHandler<login::detail::Sub_4FD6A0>, 0}, // 原回调：0x004FD6A0。
    {7, InvokeLegacyWindowHandler<login::detail::Sub_4FDBD0>, 0}, // 原回调：0x004FDBD0。
    {8, InvokeLegacyWindowHandler<login::detail::Sub_4FE0E0>, 0}, // 原回调：0x004FE0E0。
    {9, InvokeLegacyWindowHandler<login::detail::Sub_4FE860>, 0}, // 原回调：0x004FE860。
    {10, InvokeLegacyWindowHandler<login::detail::Sub_4FEF30>, 0}, // 原回调：0x004FEF30。
    {11, InvokeLegacyWindowHandler<login::detail::Sub_4FF1A0>, 0}, // 原回调：0x004FF1A0。
    {12, InvokeLegacyWindowHandler<login::detail::Sub_4FF4C0>, 0}, // 原回调：0x004FF4C0。
    {17, InvokeLegacyWindowHandler<login::detail::Sub_4FA350>, 1}, // 原回调：0x004FA350。
    {53, InvokeLegacyWindowHandler<login::detail::Sub_4D3210>, 2}, // 原回调：0x004D3210。
    {54, InvokeLegacyWindowHandler<login::detail::Sub_4D3410>, 2}, // 原回调：0x004D3410。
    {55, InvokeLegacyWindowHandler<login::detail::Sub_4D6710>, 2}, // 原回调：0x004D6710。
    {56, InvokeLegacyWindowHandler<login::detail::Sub_4D6A60>, 2}, // 原回调：0x004D6A60。
    {57, InvokeLegacyWindowHandler<login::detail::Sub_4D6EA0>, 2}, // 原回调：0x004D6EA0。
    {62, InvokeLegacyWindowHandler<login::detail::Sub_4D8DA0>, 2}, // 原回调：0x004D8DA0。
    {63, InvokeLegacyWindowHandler<login::detail::Sub_4D9220>, 2}, // 原回调：0x004D9220。
    {65, InvokeLegacyWindowHandler<login::detail::Sub_4DA580>, 2}, // 原回调：0x004DA580。
    {64, InvokeLegacyWindowHandler<login::detail::Sub_4DAA40>, 2}, // 原回调：0x004DAA40。
    {66, InvokeLegacyWindowHandler<login::detail::Sub_4DBD30>, 2}, // 原回调：0x004DBD30。
    {67, InvokeLegacyWindowHandler<login::detail::Sub_4DC290>, 2}, // 原回调：0x004DC290。
    {68, InvokeLegacyWindowHandler<login::detail::Sub_4DDFC0>, 2}, // 原回调：0x004DDFC0。
    {69, InvokeLegacyWindowHandler<login::detail::Sub_4DF300>, 2}, // 原回调：0x004DF300。
    {70, InvokeLegacyWindowHandler<login::detail::Sub_4DC7E0>, 2}, // 原回调：0x004DC7E0。
    {71, InvokeLegacyWindowHandler<login::detail::Sub_4DDB20>, 2}, // 原回调：0x004DDB20。
    {72, InvokeLegacyWindowHandler<login::detail::Sub_4E0380>, 2}, // 原回调：0x004E0380。
    {73, InvokeLegacyWindowHandler<login::detail::Sub_4DF730>, 2}, // 原回调：0x004DF730。
    {58, InvokeLegacyWindowHandler<login::detail::Sub_4D7470>, 2}, // 原回调：0x004D7470。
    {59, InvokeLegacyWindowHandler<login::detail::Sub_4D7C20>, 2}, // 原回调：0x004D7C20。
    {60, InvokeLegacyWindowHandler<login::detail::Sub_4D8230>, 2}, // 原回调：0x004D8230。
    {61, InvokeLegacyWindowHandler<login::detail::Sub_4D88C0>, 2}, // 原回调：0x004D88C0。
    {74, InvokeLegacyWindowHandler<login::detail::Sub_4D4F00>, 2}, // 原回调：0x004D4F00。
    {75, InvokeLegacyWindowHandler<login::detail::Sub_4D36F0>, 2}, // 原回调：0x004D36F0。
    {76, InvokeLegacyWindowHandler<login::detail::Sub_4D4A50>, 2}, // 原回调：0x004D4A50。
    {77, InvokeLegacyWindowHandler<login::detail::Sub_4E4A50>, 2}, // 原回调：0x004E4A50。
    {78, InvokeLegacyWindowHandler<login::detail::Sub_4E4460>, 2}, // 原回调：0x004E4460。
    {79, InvokeLegacyWindowHandler<login::detail::Sub_4E92A0>, 1}, // 原回调：0x004E92A0。
    {80, InvokeLegacyWindowHandler<login::detail::Sub_4E97E0>, 1}, // 原回调：0x004E97E0。
    {81, InvokeLegacyWindowHandler<login::detail::Sub_4F0CD0>, 2}, // 原回调：0x004F0CD0。
    {82, InvokeLegacyWindowHandler<login::detail::Sub_4F0E20>, 2}, // 原回调：0x004F0E20。
    {83, InvokeLegacyWindowHandler<login::detail::Sub_4D5250>, 2}, // 原回调：0x004D5250。
    {84, InvokeLegacyWindowHandler<login::detail::Sub_4D55C0>, 2}, // 原回调：0x004D55C0。
    {85, InvokeLegacyWindowHandler<login::detail::Sub_4D62A0>, 2}, // 原回调：0x004D62A0。
    {98, InvokeLegacyWindowHandler<login::detail::Sub_4021A0>, 2}, // 原回调：0x004021A0。
    {99, InvokeLegacyWindowHandler<login::detail::Sub_4014B0>, 2}, // 原回调：0x004014B0。
    {100, nullptr, 2},
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

// 按旧窗口编号借用已绑定的处理器；0/100 两条原空记录仍返回 nullptr。
LegacyWindowHandler FindLegacyWindowHandler(const std::uint32_t id)
{
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return nullptr;
    }
    return gLegacyWindowRecords57C368[static_cast<std::size_t>(index)].handler;
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
