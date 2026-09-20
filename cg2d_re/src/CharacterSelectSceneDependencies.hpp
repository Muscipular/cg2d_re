#pragma once

#include "LoginMessageDependencies.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace cg::login::detail {

// Renderer::DrawText2@0x456670 尚未实现的真实目标。
std::int32_t DrawText2(
    std::int16_t x,
    std::int16_t y,
    std::int8_t value2,
    std::int8_t value3,
    std::int8_t value4,
    const char* text,
    std::int8_t value6);

} // namespace cg::login::detail

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

// sub_460FF0@0x460FF0 尚未实现的真实直接目标；不提供空体或固定返回。
std::int32_t Sub_45C970();

std::int32_t Sub_51B3B0(std::uint32_t selectedIndex);
void NullSub_509080();
std::int32_t Sub_520200();

// sub_460D00@0x460D00 与 sub_461B30@0x461B30 共享的真实直接目标。
std::int32_t Sub_51B110(std::uint32_t slot);
std::int32_t Sub_517880(
    login::YobiAction* action,
    std::int32_t value1,
    std::int32_t value2,
    std::uint32_t value3);

void Sub_518BE0();
std::int32_t Sub_519290();
std::int32_t Sub_5194C0();
std::FILE* Sub_519300();
std::FILE* Sub_519550();
std::int32_t Sub_51BF00(std::int32_t imageNo);
std::int32_t Sub_51BAC0(std::int32_t imageNo);
std::int32_t Sub_51BFF0(std::int32_t imageNo);
std::int32_t Sub_51BB80(std::int32_t imageNo);
std::uint32_t Sub_52B990();

std::int32_t Sub_45F100(const CharacterSelectUiRecord* uiRecord);
void Sub_45EFD0(
    const CharacterSelectUiRecord* uiRecord,
    std::int8_t layer,
    std::int32_t value2,
    std::int32_t value3,
    std::int32_t style);
std::int32_t Sub_45F090(
    std::int32_t x,
    std::int32_t y,
    std::int8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    std::int32_t value5);
std::int32_t Sub_512D30(
    std::uintptr_t socket,
    std::int32_t version,
    const char* payload);

// 原 gKeyState.states；本控制器只读取索引 17 的 bit 1。
extern std::uint8_t gKeyStates[];

// 原 0x9A5C10 的容量未决，只保留未定长外部数组声明。
extern char gVersionPayload9A5C10[];

// 原 0x9A4EAC 的动态文本；容量与生产者未决。
extern char gText9A4EAC[];

} // namespace cg::character_select::detail
