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

// 原 [0x464290,0x4642B4)：四项 left/top/width/height 转换为鼠标命中矩形。
std::int32_t Sub_464290(const std::int32_t* rectangle);

// 原 [0x462870,0x46287C)：清理 Yobi 后递增共享场景状态。
void Sub_462870();

void Sub_518BE0();
std::int32_t Sub_519290();
std::int32_t Sub_5194C0();
std::FILE* Sub_519300();
std::FILE* Sub_519550();
// 原 [0x51BA10,0x51BAB5)：按原区间顺序返回图号类别 0..5。
std::int32_t Sub_51BA10(std::int32_t imageNo);
// 原 [0x51BF00,0x51BFEF)：三段早退后按 signed 商索引 14 项局部图号表。
std::int32_t Sub_51BF00(std::int32_t imageNo);
// 原 [0x51BAC0,0x51BB76)：旧图号逆变换，仅在有效区间分解。
std::int32_t Sub_51BAC0(std::int32_t imageNo);
// 原 [0x51BD00,0x51BDA8)：按图号区间映射 33100/33101 或 -1。
std::int32_t Sub_51BD00(std::int32_t imageNo);
// 原 [0x51BFF0,0x51C0CA)：14 项类别查找及特殊区间转换。
std::int32_t Sub_51BFF0(std::int32_t imageNo);
// 原 [0x51BB80,0x51BC4E)：旧图号的分段换算，类别 3/4 交换。
std::int32_t Sub_51BB80(std::int32_t imageNo);
// 原 [0x51B7D0,0x51B90F)：图号的有序区间类别判定，供 51B740 消费。
std::int32_t Sub_51B7D0(std::int32_t imageNo);
// 原 [0x51B910,0x51BA08)：图号映射至 100900/100901/100902/110598 或 -1。
std::int32_t Sub_51B910(std::int32_t imageNo);
// 原 [0x4C56E0,0x4C5789)：依序写类别、余数及三层分解，返回首个输出指针。
std::int32_t* Sub_4C56E0(
    std::int32_t imageNo,
    std::int32_t* outBase,
    std::int32_t* outClass,
    std::int32_t* outQuarter,
    std::int32_t* outFifth,
    std::int32_t* outOnes);
// 原 [0x4C5650,0x4C56DA)：按类别组合五项图号分量，其他类别透传首参。
std::int32_t Sub_4C5650(
    std::int32_t base,
    std::int32_t imageClass,
    std::int32_t quarter,
    std::int32_t fifth,
    std::int32_t ones);
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
