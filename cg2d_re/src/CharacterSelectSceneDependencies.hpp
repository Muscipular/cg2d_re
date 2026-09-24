#pragma once

#include "LoginMessageDependencies.hpp"

#include "cg/LegacyAudio.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace cg::character_select::detail {

// 原 UI 记录严格由 7 个 32 位整数组成；接口保留强类型原生指针。
struct CharacterSelectUiRecord {
    std::array<std::int32_t, 7> values;
};

static_assert(sizeof(CharacterSelectUiRecord) == 0x1C);

// sub_462090 的六个定宽坐标字段和一个原生文本指针。
struct CharacterModalUiRecord {
    std::int32_t textX;
    std::int32_t textY;
    std::int32_t left;
    std::int32_t top;
    std::int32_t width;
    std::int32_t height;
    const char* text;
};

static_assert(offsetof(CharacterModalUiRecord, textX) == 0x00);
static_assert(offsetof(CharacterModalUiRecord, textY) == 0x04);
static_assert(offsetof(CharacterModalUiRecord, left) == 0x08);
static_assert(offsetof(CharacterModalUiRecord, top) == 0x0C);
static_assert(offsetof(CharacterModalUiRecord, width) == 0x10);
static_assert(offsetof(CharacterModalUiRecord, height) == 0x14);
static_assert(offsetof(CharacterModalUiRecord, text) == 0x18);
static_assert(sizeof(CharacterModalUiRecord) == 0x20);

// sub_503990@0x503990 与 sub_503A20@0x503A20 的还原入口。
std::int32_t Sub_503990(const CharacterModalUiRecord* uiRecord);
std::int32_t Sub_503A20(
    const CharacterModalUiRecord* uiRecord,
    std::uint8_t renderMode,
    std::int8_t color,
    std::int32_t hoverMode,
    std::uint32_t hoverImageSequence,
    std::uint8_t layer);

using cg::login::Sub_51B3B0;
// 原 sub_520200@0x520200：共享当前记录持久化核心的角色选择侧薄包装。
std::int32_t Sub_520200();

// sub_460D00@0x460D00 与 sub_461B30@0x461B30 共享的真实直接目标。
using cg::login::Sub_51B110;

void Sub_518BE0();
std::int32_t Sub_519290();
std::int32_t Sub_5194C0();
std::FILE* Sub_519300();
std::FILE* Sub_519550();
// 原 [0x51BF00,0x51BFEF)：三段早退后按 signed 商索引 14 项局部图号表。
std::int32_t Sub_51BF00(std::int32_t imageNo);
// 原 [0x51BAC0,0x51BB76)：旧图号逆变换，仅在有效区间分解。
std::int32_t Sub_51BAC0(std::int32_t imageNo);
// 原 [0x51BFF0,0x51C0CA)：14 项类别查找及特殊区间转换。
std::int32_t Sub_51BFF0(std::int32_t imageNo);
// 原 [0x51BB80,0x51BC4E)：旧图号的分段换算，类别 3/4 交换。
std::int32_t Sub_51BB80(std::int32_t imageNo);
using cg::audio::Sub_52B990;

std::int32_t Sub_45F100(const CharacterSelectUiRecord* uiRecord);
void Sub_45EFD0(
    const CharacterSelectUiRecord* uiRecord,
    std::uint8_t layer,
    std::int32_t overlayMode,
    std::int32_t overlayImageNo,
    std::int32_t style);
using cg::render::Sub_45F090;
using cg::network::Sub_512D30;

// 原 0x9A5C10 的容量未决，只保留未定长外部数组声明。
extern char gVersionPayload9A5C10[];

// 原 0x9A4EAC 的动态文本；容量与生产者未决。
extern char gText9A4EAC[];

} // namespace cg::character_select::detail
