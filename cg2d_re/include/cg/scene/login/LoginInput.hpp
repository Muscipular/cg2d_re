#pragma once

#include "cg/input/Input.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::session { class ClientSession; }

namespace cg::network {
class CgConnection;
}

namespace cg::login {

// 原登录控件固定 8×W32 布局。
struct LoginSceneControl32 {
    std::int32_t drawX; // 原成员偏移：+0x00。
    std::int32_t drawY; // 原成员偏移：+0x04。
    std::int32_t left; // 原成员偏移：+0x08。
    std::int32_t top; // 原成员偏移：+0x0C。
    std::int32_t width; // 原成员偏移：+0x10。
    std::int32_t height; // 原成员偏移：+0x14。
    std::int32_t normalImageNo; // 原成员偏移：+0x18。
    std::int32_t activeImageNo; // 原成员偏移：+0x1C。
};

static_assert(offsetof(LoginSceneControl32, drawX) == 0x00);
static_assert(offsetof(LoginSceneControl32, drawY) == 0x04);
static_assert(offsetof(LoginSceneControl32, left) == 0x08);
static_assert(offsetof(LoginSceneControl32, top) == 0x0C);
static_assert(offsetof(LoginSceneControl32, width) == 0x10);
static_assert(offsetof(LoginSceneControl32, height) == 0x14);
static_assert(offsetof(LoginSceneControl32, normalImageNo) == 0x18);
static_assert(offsetof(LoginSceneControl32, activeImageNo) == 0x1C);
static_assert(sizeof(LoginSceneControl32) == 0x20);

// 原登录缓存每项51字节（含NUL）；ALM02自动入口沿同一50字节正文上限。
inline constexpr std::size_t LoginInputValueCapacity = 51;

// 登录旧GBK字段按密码、账号成组保存；失败提示并保留当前输入／存档。
bool SaveInputValues(std::wstring_view first, std::wstring_view second);
bool SaveCapability4InputSlot(network::CgConnection& connection);

// 原 sub_5203B0@[0x5203B0,0x5203D6)，写入 0xFF494F 的51字节密码槽。
const wchar_t* WriteInputValueFF494F_5203B0(const wchar_t* source);
// 原 sub_5203E0@[0x5203E0,0x520409)，读取 0xFF494F 的51字节密码槽。
std::wstring& ReadInputValueFF494F_5203E0(std::wstring& output);
// 原 sub_520410@[0x520410,0x520436)，写入 0xFF4982 的51字节账号槽。
const wchar_t* WriteInputValueFF4982_520410(const wchar_t* source);
// 原 sub_520440@[0x520440,0x520469)，读取 0xFF4982 的51字节账号槽。
std::wstring& ReadInputValueFF4982_520440(std::wstring& output);

// 原 sub_5036F0@[0x5036F0,0x50377E)：登录控件命中与鼠标状态位打包。
std::int32_t QueryLoginControlState_5036F0(
    session::ClientSession& session,
    const LoginSceneControl32* control);

// 原 sub_503780@[0x503780,0x5038A9)：登录控件基础图与命中覆盖图绘制。
void DrawLoginControl_503780(
    session::ClientSession& session,
    const LoginSceneControl32* control,
    std::uint8_t layer,
    std::int32_t state,
    std::uint32_t overlayImageNo,
    std::int32_t renderMode);

// 原 sub_45C7C0@[0x45C7C0,0x45C96E)。
void InitializeInputContexts_45C7C0(network::CgConnection& connection);

// 原 sub_45F970@[0x45F970,0x45FA6C)：能力 3 登录输入初始化。
void InitializeCapabilityInput_45F970(network::CgConnection& connection);

// 原 sub_465320@[0x465320,0x465429)：能力 4 登录输入初始化。
void InitializeCapability4Input_465320(network::CgConnection& connection);

// 原 sub_465740@[0x465740,0x465788)：保存能力 4 输入并切换持久槽。
void SaveAndSwitchCapability4InputSlot_465740(network::CgConnection& connection, std::int32_t newSlot);

// 原 sub_465430@[0x465430,0x46573D)：能力 4 持久槽选择器。
std::int32_t ProcessCapability4SavedSlotSelector_465430(network::CgConnection& connection);

// 原 sub_45FA70@[0x45FA70,0x45FE9C)：能力 3 登录输入控制器。
std::int32_t ProcessCapability3Input_45FA70(network::CgConnection& connection, std::int32_t mode);

// 原 sub_464E30@[0x464E30,0x46531E)：能力 4 登录输入控制器。
std::int32_t ProcessCapability4Input_464E30(network::CgConnection& connection);

// 原 sub_45CA20@[0x45CA20,0x45CDF6)：登录输入与两个提交控件控制器。
std::int32_t ProcessLoginInput_45CA20(network::CgConnection& connection, std::int32_t mode);

} // namespace cg::login
