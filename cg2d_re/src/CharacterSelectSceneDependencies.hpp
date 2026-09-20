#pragma once

#include "LoginMessageDependencies.hpp"

#include <array>
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

// sub_460FF0@0x460FF0 尚未实现的真实直接目标；不提供空体或固定返回。
void Sub_45DD50();
std::int32_t Sub_45DD60();
void Sub_45D7D0();
std::int32_t Sub_45D7E0();
std::int32_t Sub_45C970();
std::int32_t Sub_45E0C0(std::int32_t x, std::int32_t y);

// IDA 将返回值误标为 YobiAction*；调用方实际只比较 0、1、2。
std::int32_t Sub_45E090(
    std::int32_t x,
    std::int32_t y,
    const char* text,
    std::int8_t value3);
std::int32_t Sub_45DEE0(
    std::int32_t x,
    std::int32_t y,
    const char* text,
    std::int8_t value3,
    std::int32_t width,
    std::int32_t height);

void Sub_45DBF0();
std::int32_t Sub_45DC00();
std::int32_t Sub_51B3B0(std::uint32_t selectedIndex);
void NullSub_509080();
std::int32_t Sub_520200();
std::int32_t Sub_462090(std::int32_t x, std::int32_t y);

// sub_460D00@0x460D00 与 sub_461B30@0x461B30 共享的真实直接目标。
std::int32_t Sub_51B110(std::uint32_t slot);
std::int32_t Sub_517880(
    login::YobiAction* action,
    std::int32_t value1,
    std::int32_t value2,
    std::uint32_t value3);

// YobiAction::new@0x404AE0；按原 W8/W32 无符号参数承接 x64 对象。
login::YobiAction* NewYobiAction_404AE0(
    std::uint8_t priority,
    std::uint32_t yobiSize);

// x64 返回适配：承接原 0x460D42 将 x86 指针位模式留在 EAX 的路径。
std::int32_t GetYobiActionLegacyPointerBits32(login::YobiAction* action);

// x64 窄布局适配：上层只传强类型对象，不重建或硬编码 x86 对象布局。
std::int32_t GetYobiActionField2A8(login::YobiAction* action);
void SetYobiActionField2A8(
    login::YobiAction* action,
    std::int32_t value);
void SetYobiActionField2B0(
    login::YobiAction* action,
    std::int32_t value);
void SetYobiActionField15(
    login::YobiAction* action,
    std::uint8_t value);
void SetYobiActionField30(
    login::YobiAction* action,
    std::int32_t value);
void SetYobiActionField34(
    login::YobiAction* action,
    std::int32_t value);
void SetYobiActionField2B8(
    login::YobiAction* action,
    std::int32_t value);

void Sub_518BE0();
std::int32_t Sub_519290();
std::int32_t Sub_5194C0();
std::FILE* Sub_519300();
std::FILE* Sub_519550();
void YobiClearUp_404D30();

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
