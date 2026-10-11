#pragma once

#include "StartupNative.hpp"

#include <cstdint>

namespace cg::platform {

// MC01-B：真实MSVC描述符服务，禁止与独立Win32句柄混用。
// 原函数：0x0053D435。
std::int32_t OpenLegacyDescriptor_53D435(const char* const filename,
    const std::uint32_t flags, const std::uint32_t permissions);

// MC01-B：真实MSVC描述符服务，禁止与独立Win32句柄混用。
// 原函数：0x0053D3BD。
std::int32_t GetLegacyDescriptorLength_53D3BD(const std::int32_t descriptor);

// MC01-B：真实MSVC描述符服务，禁止与独立Win32句柄混用。
// 原函数：0x0053D30A。
std::int32_t CloseLegacyDescriptor_53D30A(const std::int32_t descriptor);

} // namespace cg::platform
