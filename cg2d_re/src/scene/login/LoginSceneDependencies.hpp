#pragma once

#include "LoginSavedState.hpp"

#include "LoginMessageDependencies.hpp"
#include "../../ui/UiControlRecord.hpp"

#include "cg/audio/LegacyAudio.hpp"

#include <cstdint>

namespace cg::login::detail {

// 登录场景控制器的真实直接目标与共享接口；提供者分属音频、UI及持久化模块。
// 原函数：0x0045F100。 共用控件输入位图。
using cg::ui::QueryUiControlInputState_45F100;
using cg::audio::Sub_52B990;
// 原函数：0x0045EFD0。 共用控件底图及悬停边框。
using cg::ui::DrawUiControl_45EFD0;
using cg::render::Sub_45F090;


} // namespace cg::login::detail
