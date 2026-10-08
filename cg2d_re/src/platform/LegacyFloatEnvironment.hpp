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

// 原543185共享CRT分派的实际帧片段；原数值原宽，只有描述符地址拓为原生指针。
struct LegacyMathDispatchContext {
    std::uint16_t savedControl;   // 原EBP-A4，父保存CW。
    std::uint16_t workingControl; // 原EBP-A2，原分支生成临时CW。
    std::uint16_t status;         // 原EBP-A0，等待式FXAM状态样本。
    std::uint8_t scratch80[10];   // 原EBP-9E，实际T80落存/quiet位读取。
    const std::uint8_t* descriptor; // 原EBP-94借用描述符，不持有。
    std::uint8_t errorType;       // 原EBP-90，原0/1/7错误样本。
    std::uint32_t conditionSample; // 原ECX数值样本，跨原分派/目标的显式承载，不默认初始化。
};
// 只在ASM间调用，RCX借用context、ST0（双参另ST1）传结果；void类型不作CPP FP返回。
using LegacyMathDispatchTarget = void (*)(LegacyMathDispatchContext*) noexcept;
struct LegacyUnaryMathDescriptor {
    std::uint8_t prefix[16];      // 原16B头原样承载，本分派只读+14的控制选择BYTE。
    LegacyMathDispatchTarget targets[4]; // 原四代码指针，只扩已证指针。
};
struct LegacyBinaryMathDescriptor {
    std::uint8_t prefix[16];
    LegacyMathDispatchTarget targets[16]; // 原4×4代码指针，不另造固定目标表。
};
// ASM入口Cg2dDispatchLegacyUnaryMathX87/Cg2dDispatchLegacyBinaryMathX87：
// RCX借context、RDX借对应描述符；真实x87目标均只在ASM间消费/返回ST0。

// 原545360/545B51私有ASM合同，供共享指数链的原FDIV探测分支真实调用。
// Cg2dDivideLegacyExtendedX87以RCX借原44B片段：T80在+0/+12，CW在+24/+28。
// 不填T80尾两字节或CW高W16；核心写原分母T80并返回ST0。
// Cg2dReverseDivideLegacyStackX87只在ASM间以ST0/ST1进出，保原EAX/ECX数值及EDX。
// 不声明CPP double ABI；完整分支和生命周期见共享除法依赖SPEC。

// 原542FB0..543185共享幂/指数/对数目标，RCX借同一LegacyMathDispatchContext。
// Cg2dLegacySharedPowerX87/ExpX87/ZeroPowerX87及对数/特殊值目标只在ASM间传ST0/ST1。
// 原542FEF真实调用Cg2dReverseDivideLegacyStackX87；其探测值来自唯一已有gLegacyFdivBug1085FC0。
// Cg2dLegacyExpMinusOneX87的R11D仅承接原继续内层0/丢内层返回后结束外层1的控制流；
// 不作为原数学状态/成功值，调用父以JRCXZ消费以保持末标志，不模拟原栈地址。
// Cg2dClassifyLegacySharedIntegerX87保持ST0/返回EAX0-1-2；不是消费ST0的幂指数分类叶。
// 三个R7共享区间和七原函数身份一起映射，完整原表生产/其它调用父仍沿真实依赖补齐。

// 原545477完整64项除法分派：Cg2dDispatchLegacyDivisionX87只在ASM间调用，
// EAX原操作低六位/ST栈按原体消费，表项仅拓已证代码地址，不加selector拒绝。
// 原545916/545929的Cg2dDivideLegacyPairX87/Cg2dReverseDivideLegacyPairX87，
// 以及原545B3C的Cg2dDivideLegacyStackX87均真实落存两T80并调用完整除法核心。
// 545B3C只按原W32 EAX保存恢复；RCX的原生寄存器暂存用于借址，不改变原44B数值片段。
// 私有ST0返回不声明CPP double ABI；原INT 6分支保持，完整其它父和机器验收继续。

// 原54593C..545B3C八个内存加载父：F32/F64/S16/S32各正/逆向，私有ASM用ST0进出。
// Cg2dDivideLegacyMemoryF32X87/Cg2dDivideLegacyMemoryF64X87/Cg2dDivideLegacyMemoryS16X87/
// Cg2dDivideLegacyMemoryS32X87，以及Cg2dReverseDivideLegacyMemoryF32X87/
// Cg2dReverseDivideLegacyMemoryF64X87/Cg2dReverseDivideLegacyMemoryS16X87/
// Cg2dReverseDivideLegacyMemoryS32X87：
// RCX借一个原值参数的对应位宽只读内存，数值不扩宽；不是CPP double ABI。
// 原FNSTSW AX/TOP 3800H与浮点指数位分支保持，TOP=0按原序保存/重载额外T80。
// 原545A65是逆向F32父的中间分支，在该完整父中实现，原W32 EAX恢复。
// 借址只是Win64值参数传送边界，不模拟原RET4/8或栈地址，所有输入字段/结果仍原宽。

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

// 仅承接原542FE2对已有1085FC0的W32读取，供ASM真实探测分支使用。
extern "C" std::int32_t Cg2dReadLegacyFdivBug() noexcept;

} // namespace detail
} // namespace cg::platform
