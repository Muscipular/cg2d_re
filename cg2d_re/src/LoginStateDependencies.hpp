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

// sub_45D990@0x45D990 的直接目标。
std::int32_t TextMeasure(const char* text, std::int32_t fontType);
std::int32_t Sub_50FC30();
std::int32_t Sub_50FE30();
std::int32_t Sub_50FF10();
std::int32_t Sub_50FF70();
std::int32_t Sub_510120();
void Sub_5081A0();
std::int8_t Sub_401000(std::int32_t value);
std::int32_t Sub_508140();
std::int32_t Sub_508350();
std::int32_t Sub_508360();
std::int8_t Sub_401030(std::int32_t value);
std::int32_t DrawText(
    std::int16_t x,
    std::int16_t y,
    std::uint8_t value2,
    std::int8_t value3,
    std::int8_t value4,
    const char* text,
    std::int8_t value6,
    std::int8_t value7);

// sub_45D990@0x45D990 与 sub_45EA60@0x45EA60 的共享直接目标。
YobiAction* Sub_5044E0(
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    std::int32_t value4,
    std::int32_t value5);
YobiAction* MarkDeath(YobiAction* action);

// sub_45EA60@0x45EA60 的绘制目标。
std::int32_t DrawText3(
    std::int16_t x,
    std::int16_t y,
    std::int8_t value2,
    std::int8_t value3,
    const char* text,
    std::int8_t value5);

// x64 窄边界：封装原相关函数各自 action+0x170 上的 CryptInt::gte(..., 1)。
// 上层只传强类型对象，不依赖或重建 x86 对象字段偏移。
bool IsYobiActionReady(YobiAction* action, std::int32_t threshold);

// x64 窄边界：封装原 action+0x170 上的 CryptInt::eq(..., expected)。
// 与上述 gte readiness 判定语义不同，只作严格相等比较。
bool IsYobiActionStateEqual(YobiAction* action, std::int32_t expected);

// x64 窄边界：封装原 action+0x170 上的 CryptInt::Lte(..., maximum)。
// 与 gte readiness 和严格相等判定分别保留原比较语义。
bool IsYobiActionStateLessOrEqual(
    YobiAction* action,
    std::int32_t maximum);

} // namespace cg::login::detail
