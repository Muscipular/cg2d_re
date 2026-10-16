#pragma once

#include "cg/render/QueueCapacity.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace cg::input {

class InputContext;

} // namespace cg::input

namespace cg::render {

// 原 GraphicCache4@[0x62ECB8,0x73ECB8) 的 4096 项文本链记录。
struct TextPendingRecord {
    std::int16_t x; // 原成员偏移：+0x00。
    std::int16_t y; // 原成员偏移：+0x02。
    std::uint8_t color; // 原成员偏移：+0x04。
    std::uint8_t fontType; // 原成员偏移：+0x05。
    std::uint8_t renderMode; // 原成员偏移：+0x06。
    std::uint8_t value7; // 原成员偏移：+0x07。
    std::uint8_t marker; // 原成员偏移：+0x08。
    std::wstring text; // 原+0x09的255B文本域；UTL02动态UTF-16，不保留旧字节容量。
    std::int32_t field108; // 原成员偏移：+0x108。
    std::uint32_t nextToken; // 原成员偏移：+0x10C。
};

// 原 fontLayer@0x558F50 的六项四字节记录；本目标只读取低字节 layer。
struct TextFontLayerRecord {
    std::uint8_t layer; // 原成员偏移：+0x00。
    std::array<std::uint8_t, 3> initialZeroTail; // 原成员偏移：+0x01。
};

static_assert(sizeof(TextFontLayerRecord) == 0x04);
static_assert(offsetof(TextFontLayerRecord, layer) == 0x00);

extern std::array<TextPendingRecord, MAX_QUEUE_SIZE> gTextPendingRecords; // 原全局：0x0062ECB8。
extern std::array<TextFontLayerRecord, 6> gTextFontLayerRecords; // 原全局：0x00558F50。

// 原 dword_D68BEC@0xD68BEC；当前悬停的零基文本待提交索引。
extern std::int32_t gHoveredTextPendingIndex;

// 原 456D70：R06按文本链顺序绘制缓存字形及阴影，headToken为一基令牌。
// 使用Font的同一字体度量与唯一CPU Back，不改变队列令牌和文本所有权。
void RenderTextToSurface_456D70(std::uint32_t headToken);

// R06：仅显式调用方解析美元颜色标记和转义，分段沿原DrawText队列提交。
void DrawColoredText(
    std::int16_t x, std::int16_t y, std::uint8_t renderMode,
    std::int8_t fontType, std::int8_t color, std::wstring_view text,
    std::int8_t value7, std::int8_t marker);

// 原 sub_45D7A0@[0x45D7A0,0x45D7CF)。
// 左键按下时返回 values 中首个等于当前悬停文本索引的元素下标，否则返回 -1。
std::int32_t FindClickedTextIndex_45D7A0(
    const std::int32_t* values,
    std::int32_t count);

// Renderer::QueueText@[0x521A30,0x521A98)。headToken 为文本记录的一基令牌。
std::int32_t QueueText_521A30(std::uint32_t headToken, std::uint8_t layer);

// Renderer::DrawText3@[0x456560,0x456668)。
std::int32_t DrawText3_456560(
    std::int16_t x,
    std::int16_t y,
    std::int8_t renderMode,
    std::int8_t color,
    std::wstring_view text,
    std::int8_t value7);

// Renderer::DrawText2@[0x456670,0x45677B)。
std::int32_t DrawText2_456670(
    std::int16_t x,
    std::int16_t y,
    std::int8_t renderMode,
    std::int8_t fontType,
    std::int8_t color,
    std::wstring_view text,
    std::int8_t value7);

// Renderer::DrawText@[0x456780,0x456891)。
std::int32_t DrawText_456780(
    std::int16_t x,
    std::int16_t y,
    std::uint8_t renderMode,
    std::int8_t fontType,
    std::int8_t color,
    std::wstring_view text,
    std::int8_t value7,
    std::int8_t marker);

// 原 Renderer::DrawInputText@[0x4568A0,0x456D27)。
void DrawInputText_4568A0(input::InputContext* context);

} // namespace cg::render
