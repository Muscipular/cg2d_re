#pragma once

#include "LoginMessageDependencies.hpp"

#include <cstdint>

namespace cg::login::detail {

// LoginScene::Process@0x45FEA0 尚未实现的真实直接目标；不提供空体或固定返回。
std::int32_t Sub_450B70(std::int32_t value0, std::int32_t value1);
std::int32_t LoadAndPlayBGM(std::uint32_t bgmId);
std::int32_t Sub_45F100(const void* uiRecord);
std::uint32_t Sub_52B990();
char* Sub_5202E0(char* value);
char* Sub_5203E0(char* value);
char* Sub_520440(char* value);
void Sub_45EFD0(
    const void* uiRecord,
    std::int8_t value1,
    std::int32_t value2,
    std::int32_t value3,
    std::int32_t style);
std::int32_t HasIDirect3D7();
std::int32_t Sub_45F090(
    std::int32_t x,
    std::int32_t y,
    std::int8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    std::int32_t value5);

// x64 安全兼容边界：原尾部在分类选择为 -1 时，以服务器表基址做负索引，
// 实际采样落在 20 个其他宿主对象的字节中。适配器只映射 page 0/1、slot 0..9
// 的这些真实宿主字节，不固定返回，也不建立副本；精确地址见证据文档。
std::uint8_t ReadNegativeSelectionAvailability(
    std::int32_t page,
    std::int32_t slot);

} // namespace cg::login::detail

namespace cg::platform::detail {

// 原 Win32 直接目标的窄声明；公开业务接口不暴露 HWND、WPARAM、LPARAM 或 HINSTANCE。
std::int32_t SetWindowTextA_54D2E0(std::uintptr_t window, const char* text);
std::int32_t PostMessageA_54D2F0(
    std::uintptr_t window,
    std::uint32_t message,
    std::uintptr_t wParam,
    std::intptr_t lParam);
std::intptr_t ShellExecuteA_54D224(
    std::uintptr_t window,
    const char* operation,
    const char* file,
    const char* parameters,
    const char* directory,
    std::int32_t showCommand);

} // namespace cg::platform::detail
