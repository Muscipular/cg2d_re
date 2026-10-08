#pragma once

#include <cstdint>

namespace cg::platform {

// 原1085FC0；初始化组合发布系统/回退探测的完整W32返回值。
extern std::int32_t gLegacyFdivBug1085FC0;

// 原546311/5461E4；F64值与S32分类结果，实际x87合同见浮点分类叶SPEC。
std::int32_t ClassifyLegacyFloat_546311(double value) noexcept;
std::int32_t ClassifyLegacySpecialFloat_5461E4(double value) noexcept;

// 原5436AC；F64输入，原分支返回S32的0/1/2，不写浮点控制字。
std::int32_t ClassifyLegacyInteger_5436AC(double value) noexcept;

// 原54357C；F64值参数/借用结果指针，原S32状态与不写出分支见幂特殊值SPEC。
std::int32_t ProcessLegacyPowerSpecial_54357C(
    double base, double exponent, double* result) noexcept;

// 原控制字抽象/硬件表示转换，只消费和生成原已定义位。
std::uint32_t AbstractLegacyControlWord_5448A4(std::uint32_t hardware);
std::uint32_t HardwareLegacyControlWord_544936(std::uint32_t abstractValue);
std::uint32_t ControlLegacyX87_544859(std::uint32_t value, std::uint32_t mask);
std::uint32_t ControlLegacyFloatingPoint_54488E(std::uint32_t value, std::uint32_t mask);
void SetLegacyDefaultPrecision_53FD5C();

// 原系统查询及x87回退；保留系统BOOL的完整W32返回值。
std::int32_t TestLegacyFloatingPointDivision_53FD6E();
std::int32_t ProbeLegacyFloatingPointDivision_53FDAC();
void InitializeLegacyFloatingPoint_53B447();

namespace detail {

// Win64私有叶；不借宿主CRT控制字接口或改变MXCSR。
extern "C" std::uint16_t Cg2dReadLegacyX87Control() noexcept;
extern "C" void Cg2dWriteLegacyX87Control(std::uint16_t control) noexcept;
extern "C" void Cg2dClearLegacyX87Exceptions() noexcept;
extern "C" std::int32_t Cg2dTestLegacyFdiv() noexcept;
extern "C" std::int32_t Cg2dLegacyFloatNonnegative(const double* value) noexcept;
// Win64分类参数桥；只在本调用内借用F64，不持有或修改调用方值。
extern "C" std::int32_t Cg2dClassifyLegacyFloat(const double* value) noexcept;
extern "C" std::int32_t Cg2dClassifyLegacySpecialFloat(const double* value) noexcept;
// Win64整数判定参数桥；内部5462FF舍入叶仅在ASM间用ST(0)传值。
extern "C" std::int32_t Cg2dClassifyLegacyInteger(const double* value) noexcept;
// Win64幂特殊值桥；仅在本调用内借用F64参数与调用方结果槽。
extern "C" std::int32_t Cg2dProcessLegacyPowerSpecial(
    const double* base, const double* exponent, double* result) noexcept;

} // namespace detail
} // namespace cg::platform
