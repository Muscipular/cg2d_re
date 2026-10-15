#pragma once

#include "LoginMessageDependencies.hpp"
#include "../../ui/UiControlRecord.hpp"

#include "cg/LegacyAudio.hpp"

#include <cstdint>

namespace cg::login::detail {

// 登录场景控制器的真实直接目标与共享接口；提供者分属音频、UI及持久化模块。
// 原函数：0x0052BE60。
std::int32_t LoadAndPlayBGM(std::uint32_t bgmId);
// 原函数：0x0045F100。 共用控件输入位图。
using cg::ui::QueryUiControlInputState_45F100;
using cg::audio::Sub_52B990;
// 原函数：0x0045EFD0。 共用控件底图及悬停边框。
using cg::ui::DrawUiControl_45EFD0;
using cg::render::Sub_45F090;

// 原 sub_520200@0x520200：共享当前记录持久化核心的登录侧薄包装。
std::int32_t Sub_520200();

} // namespace cg::login::detail
