#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::input {

struct InputContext;

} // namespace cg::input

namespace cg::render {

// 原 GraphicCache4@[0x62ECB8,0x73ECB8) 的 4096 项文本链记录。
struct TextPendingRecord {
    std::int16_t x;
    std::int16_t y;
    std::uint8_t color;
    std::uint8_t fontType;
    std::uint8_t renderMode;
    std::uint8_t value7;
    std::uint8_t marker;
    char text[255];
    std::int32_t field108;
    std::uint32_t nextToken;
};

static_assert(sizeof(TextPendingRecord) == 0x110);
static_assert(offsetof(TextPendingRecord, x) == 0x00);
static_assert(offsetof(TextPendingRecord, y) == 0x02);
static_assert(offsetof(TextPendingRecord, color) == 0x04);
static_assert(offsetof(TextPendingRecord, fontType) == 0x05);
static_assert(offsetof(TextPendingRecord, renderMode) == 0x06);
static_assert(offsetof(TextPendingRecord, value7) == 0x07);
static_assert(offsetof(TextPendingRecord, marker) == 0x08);
static_assert(offsetof(TextPendingRecord, text) == 0x09);
static_assert(offsetof(TextPendingRecord, field108) == 0x108);
static_assert(offsetof(TextPendingRecord, nextToken) == 0x10C);

// 原 fontLayer@0x558F50 的六项四字节记录；本目标只读取低字节 layer。
struct TextFontLayerRecord {
    std::uint8_t layer;
    std::array<std::uint8_t, 3> initialZeroTail;
};

static_assert(sizeof(TextFontLayerRecord) == 0x04);
static_assert(offsetof(TextFontLayerRecord, layer) == 0x00);

extern std::array<TextPendingRecord, 4096> gTextPendingRecords;
extern std::array<TextFontLayerRecord, 6> gTextFontLayerRecords;

// Renderer::QueueText@[0x521A30,0x521A98)。headToken 为文本记录的一基令牌。
std::int32_t QueueText_521A30(std::uint32_t headToken, std::uint8_t layer);

// Renderer::DrawText2@[0x456670,0x45677B)。
std::int32_t DrawText2_456670(
    std::int16_t x,
    std::int16_t y,
    std::int8_t renderMode,
    std::int8_t fontType,
    std::int8_t color,
    const char* text,
    std::int8_t value7);

// Renderer::DrawText@[0x456780,0x456891)。
std::int32_t DrawText_456780(
    std::int16_t x,
    std::int16_t y,
    std::uint8_t renderMode,
    std::int8_t fontType,
    std::int8_t color,
    const char* text,
    std::int8_t value7,
    std::int8_t marker);

// 原 Renderer::DrawInputText@[0x4568A0,0x456D27)。
void DrawInputText_4568A0(input::InputContext* context);

} // namespace cg::render
