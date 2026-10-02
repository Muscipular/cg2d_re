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

// x64 安全兼容边界：原尾部在分类选择为 -1 时，以服务器表基址做负索引，
// 实际采样落在 20 个其他宿主对象的字节中。适配器只映射 page 0/1、slot 0..9
// 的这些真实宿主字节，不固定返回，也不建立副本；精确地址见证据文档。
std::uint8_t ReadNegativeSelectionAvailability(
    std::int32_t page,
    std::int32_t slot);

// 原 sub_520200@0x520200：共享当前记录持久化核心的登录侧薄包装。
std::int32_t Sub_520200();

} // namespace cg::login::detail
