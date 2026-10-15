#pragma once

#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::ui {

// 原回调的共同 opened 参数；返回承载原 W32 位型或宿主原生地址。
using LegacyWindowHandler = std::uintptr_t (*)(std::int32_t opened);
// 只查表取得处理器；具体调度时机由调用方决定。
LegacyWindowHandler FindLegacyWindowHandler(std::uint32_t id);

// 原 57C368 旧窗口表与 CC236C 三槽的共享选择入口；与现代窗口编号独立。
// 原函数：0x005044C0。
std::int32_t __cdecl Sub_5044C0(std::uint32_t id);
// 原函数：0x00504410。
std::int32_t __cdecl Sub_504410(std::uint32_t id);
// 原函数：0x00504180。
std::int32_t __cdecl Sub_504180(std::uint32_t id);
// 原函数：0x005042D0。
std::int32_t __cdecl Sub_5042D0(std::uint32_t id);
// 原函数：0x00504380。
std::int32_t __cdecl Sub_504380(std::uint32_t id);
// 原函数：0x005043D0。
std::int32_t __cdecl Sub_5043D0(std::int32_t index);
std::uint32_t ReadLegacyPanelSelectionCC236C() noexcept; // 旧面板直接读取首槽编号，不经类别映射。
std::uint32_t ReadLegacyPanelSelectionCC2370() noexcept; // 旧选择31关闭时读取同源第二槽。
// 原函数：0x00504AF0。
std::int32_t __cdecl Sub_504AF0();
// 原函数：0x00504460。
std::int32_t Sub_504460(std::uint32_t id); // 旧面板按同源action槽切换选择，不直接清理对象。
// 沿用57C368唯一表的非拥有运行期槽，不建立UI平行表；索引来自5044C0。
login::YobiAction*& BorrowLegacyPanelAction57C374(std::int32_t index) noexcept;
std::uint8_t& BorrowLegacyPanelDirty57C378(std::int32_t index) noexcept;

} // namespace cg::ui
