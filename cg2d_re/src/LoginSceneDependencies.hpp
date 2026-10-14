#pragma once

#include "LoginMessageDependencies.hpp"

#include "cg/LegacyAudio.hpp"

#include <cstdint>

namespace cg::login::detail {

// 登录场景控制器的真实直接目标与共享接口；提供者分属音频、UI及持久化模块。
// 原函数：0x0052BE60。
std::int32_t LoadAndPlayBGM(std::uint32_t bgmId);
// 原函数：0x0045F100。
std::int32_t Sub_45F100(const void* uiRecord);
using cg::audio::Sub_52B990;
// 原函数：0x0045EFD0。
void Sub_45EFD0(
    const void* uiRecord,
    std::uint8_t layer,
    std::int32_t overlayMode,
    std::int32_t overlayImageNo,
    std::int32_t style);
using cg::render::Sub_45F090;

// 原 sub_520200@0x520200：共享当前记录持久化核心的登录侧薄包装。
std::int32_t Sub_520200();

} // namespace cg::login::detail
