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

// 原54769B/54791B；原W32条件/CW和借用F64结果，x87合同见数学掩码处理SPEC。
std::int32_t HandleLegacyMathException_54769B(
    std::uint32_t conditions, double* result, std::uint32_t control) noexcept;
void SetLegacyRawX87Status_54791B(std::uint32_t status) noexcept;

// 原88B浮点异常参数；三个值单元只保原W32载荷/标志，不默认填未生产字节。
struct alignas(8) LegacyFloatExceptionValue {
    std::uint32_t payload[4];   // 原F64写入低8B；其余载荷沿原生产路径。
    std::uint32_t format;       // 原值单元+16，valid/format位保持。
    std::uint32_t reserved;     // 原值单元+20，当前两生产体不写。
};
struct alignas(8) LegacyFloatExceptionRecord {
    std::uint32_t header;       // 原+0，舍入/精度/操作号的位域载体。
    std::uint32_t causes;       // 原+4，原条件到五低位映射。
    std::uint32_t enables;      // 原+8，原控制字的反掩码映射。
    std::uint32_t status;       // 原+12，等待式原x87状态样本映射。
    LegacyFloatExceptionValue operand1;  // 原+16。
    LegacyFloatExceptionValue operand2;  // 原+40，父仅选择性生产。
    LegacyFloatExceptionValue result;    // 原+64，系统调用后原F64写回。
};

// 原5473E8/5460EB；全部指针只借用，原异常分派合同见数学异常分派SPEC。
void RaiseLegacyFloatingPointException_5473E8(
    LegacyFloatExceptionRecord* record, std::uint32_t* control,
    std::uint32_t conditions, std::uint32_t operationId,
    const double* argument, double* result) noexcept;
std::int32_t ProcessLegacyX87Exception_5460EB(
    std::uint32_t operationId, LegacyMathErrorRecord* record,
    const std::uint16_t* savedControl) noexcept;

// 原543390/5433A7/5434D9的借用输入快照；由调用父生产，数值不扩宽/不默认初始化。
struct LegacyMathErrorInputs {
    std::uint32_t savedControl; // 原4B载体：CW只读W16，范围返回EDX读完整W32；不补高W16。
    std::uint32_t operationId;  // 原EDX完整W32；范围出口以1DH选择双参。
    const char* functionName;   // 原ECX借用名称指针；只扩已证地址。
    double argument1;          // 原两个W32原样复制到数学记录第一F64。
    double argument2;          // 双参入口才复制；单参路径不预填记录第二F64。
};

// ASM私有合同（不声明CPP double返回）：RCX借用LegacyMathErrorInputs，ST(0)为结果。
// Cg2dStartLegacyTwoArgumentErrorX87/Cg2dStartLegacyOneArgumentErrorX87另以EDX传S32类型。
// Cg2dCheckLegacyRangeExitX87仅消费RCX与ST(0)；三入口均在ASM间返回ST(0)，范围出口另还原EDX的完整原CW载体。

// 原53D019普通值入口；全部一般pow分支留在ASM，内部CI/core保连续ST0。
double PowerLegacyFloat_53D019(double base, double exponent) noexcept;
// ASM私有：Cg2dLegacyCIPowerX87以ST0=Y/ST1=X进出；Cg2dLegacyPowerCoreX87以
// RCX借两F64/EAX为已加载Y高W32/ST0=Y调用，返回ST0及原W32 CW载体EDX。
// Cg2dClassifyLegacyPowerExponentX87消费ST0指数/返回CL0或1或2；不声明CPP double ABI。

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

// Win64真实处理/状态C桥；两分解/指数帮助体只在ASM内部返回ST(0)。
extern "C" std::int32_t Cg2dHandleLegacyMathException(
    std::uint32_t conditions, double* result, std::uint32_t control) noexcept;
extern "C" void Cg2dSetLegacyRawX87Status(std::uint32_t status) noexcept;

// Win64真实父/系统调用桥；数值记录仍88B，外部参数槽仅拓宽已证指针。
extern "C" void Cg2dRaiseLegacyFloatingPointException(
    LegacyFloatExceptionRecord* record, std::uint32_t* control,
    std::uint32_t conditions, std::uint32_t operationId,
    const double* argument, double* result) noexcept;
extern "C" std::int32_t Cg2dProcessLegacyX87Exception(
    std::uint32_t operationId, LegacyMathErrorRecord* record,
    const std::uint16_t* savedControl) noexcept;
extern "C" void Cg2dRaiseLegacyFloatExceptionHost(
    std::uint32_t code, std::uintptr_t* arguments) noexcept;
extern "C" std::int32_t Cg2dDefaultLegacyMathError(LegacyMathErrorRecord* record) noexcept;
extern "C" std::int32_t Cg2dSetLegacyMathErrno(std::int32_t type) noexcept;
extern "C" std::uint32_t Cg2dLegacyMathErrorCallbackGate5925B8;

// Win64普通值桥；只借用两个F64输入/结果，外部转换边界见通用幂本体SPEC。
extern "C" void Cg2dComputeLegacyPower(const double* inputs, double* result) noexcept;
extern "C" std::uint32_t Cg2dLegacyPowerFastExitGate1085FBC;

} // namespace detail
} // namespace cg::platform
