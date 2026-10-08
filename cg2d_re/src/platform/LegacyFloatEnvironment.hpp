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

// 原543405；W16保存控制字值，仅消费PC两位并加载原临时控制字。
void LoadLegacyPowerControlWord_543405(std::uint16_t savedControl) noexcept;

// 原32B栈内数学错误记录；只扩名称指针，字段不默认初始化，调用方持有。
struct LegacyMathErrorRecord {
    std::int32_t type;          // 原+0类型，S32。
    const char* functionName;   // 原+4借用原字节名称，x64为+8。
    double argument1;           // 原+8第一F64，x64为+16。
    double argument2;           // 原+16第二F64，x64为+24；单参生产不预填。
    double result;              // 原+24结果F64，x64为+32。
};

// 原5478B2/5478D8；共享旧errno与原不访问记录的零值叶。
std::int32_t SetLegacyMathErrno_5478B2(std::int32_t type) noexcept;
std::int32_t DefaultLegacyMathError_5478D8(LegacyMathErrorRecord* record) noexcept;

// 原5478DB/5478E9/5478F8；硬件完整S16有符号返回，不是抽象控制字接口。
std::int32_t ReadLegacyRawX87Status_5478DB() noexcept;
std::int32_t ReadAndClearLegacyRawX87Status_5478E9() noexcept;
std::int32_t ControlLegacyRawX87_5478F8(
    std::uint32_t value, std::uint32_t mask) noexcept;

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

// Win64控制字值桥；另外两通用帮助体仅在ASM间用x87/寄存器合同调用。
extern "C" void Cg2dLoadLegacyPowerControlWord(std::uint16_t savedControl) noexcept;

// Win64原硬件叶；保FSTSW等待/FNSTSW后FNCLEX与原控制字合并顺序。
extern "C" std::int32_t Cg2dReadLegacyRawX87Status() noexcept;
extern "C" std::int32_t Cg2dReadAndClearLegacyRawX87Status() noexcept;
extern "C" std::int32_t Cg2dControlLegacyRawX87(
    std::uint32_t value, std::uint32_t mask) noexcept;

} // namespace detail
} // namespace cg::platform
