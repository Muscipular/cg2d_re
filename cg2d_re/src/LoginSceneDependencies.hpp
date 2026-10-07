#pragma once

#include "LoginMessageDependencies.hpp"

#include "cg/LegacyAudio.hpp"

#include <cstdint>

namespace cg::login::detail {

// 登录场景控制器尚未实现的真实直接目标；不提供空体或固定返回。
std::int32_t LoadAndPlayBGM(std::uint32_t bgmId);
std::int32_t Sub_45F100(const void* uiRecord);
using cg::audio::Sub_52B990;
void Sub_45EFD0(
    const void* uiRecord,
    std::uint8_t layer,
    std::int32_t overlayMode,
    std::int32_t overlayImageNo,
    std::int32_t style);
using cg::render::Sub_45F090;

// BRU01：原负分类尾部的20个真实宿主BYTE，正常page0/1、slot0..9。
// 直接借现owner；固定池链接只读原身份字节投影，不读节点编号代替原地址BYTE。
// 不复制状态、不消费RNG，不改变原先读取后判断分类的调用位置；见登录负分类SPEC。
std::uint8_t ReadNegativeSelectionAvailability(
    std::int32_t page,
    std::int32_t slot);

// 原 sub_520200@0x520200：共享当前记录持久化核心的登录侧薄包装。
std::int32_t Sub_520200();

} // namespace cg::login::detail
