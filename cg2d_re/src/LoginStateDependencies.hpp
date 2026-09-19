#pragma once

#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::login::detail {

// sub_45E960@0x45E960 中仍未实现的直接目标；只保留当前 IDA 调用契约，不提供空体。
struct UIWindowInstance;
UIWindowInstance* Sub_532D80();

std::int32_t Sub_534290();
void Sub_45CE00();
std::int32_t Sub_45CE10(const char* text);

// sub_45EA60@0x45EA60 的直接目标。
YobiAction* Sub_5044E0(
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    std::int32_t value4,
    std::int32_t value5);
std::uint32_t Sub_508AF0();
std::int32_t Sub_508B50();
std::int32_t DrawText3(
    std::int16_t x,
    std::int16_t y,
    std::int8_t value2,
    std::int8_t value3,
    const char* text,
    std::int8_t value5);
YobiAction* MarkDeath(YobiAction* action);

// x64 窄边界：封装原 action+0x170 上的 CryptInt::gte(..., 1)。
// 上层只传强类型对象，不依赖或重建 x86 对象字段偏移。
bool IsYobiActionReady(YobiAction* action, std::int32_t threshold);

} // namespace cg::login::detail
