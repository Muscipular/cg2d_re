#include "LegacyFloatEnvironment.hpp"
#include "LegacyFloatFormat.hpp"

#include <bit>
#include <cstddef>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include "cg/platform/LegacyTime.hpp"

namespace cg::platform {

std::int32_t gLegacyFdivBug1085FC0 = 0;

// 原1085FBC为无有效初始字节/无文件偏移的静态W32槽，不能把IDA的FF当原值。
namespace detail {
extern "C" {
std::uint32_t Cg2dLegacyPowerFastExitGate1085FBC;
}
} // namespace detail

// 原5925B8初始字节94260000；仅已证读取决定是否跳过原matherr，未见写入不等于常量。
namespace detail {
extern "C" {
std::uint32_t Cg2dLegacyMathErrorCallbackGate5925B8 = 0x2694u;
}
} // namespace detail

std::uint32_t AbstractLegacyControlWord_5448A4(const std::uint32_t hardware)
{
    std::uint32_t result = 0;
    if ((hardware & 0x01u) != 0) result |= 0x10u;
    if ((hardware & 0x04u) != 0) result |= 0x08u;
    if ((hardware & 0x08u) != 0) result |= 0x04u;
    if ((hardware & 0x10u) != 0) result |= 0x02u;
    if ((hardware & 0x20u) != 0) result |= 0x01u;
    if ((hardware & 0x02u) != 0) result |= 0x80000u;
    switch (hardware & 0xC00u) {
    case 0x400u: result |= 0x100u; break;
    case 0x800u: result |= 0x200u; break;
    case 0xC00u: result |= 0x300u; break;
    }
    switch (hardware & 0x300u) {
    case 0: result |= 0x20000u; break;
    case 0x200u: result |= 0x10000u; break;
    }
    if ((hardware & 0x1000u) != 0) result |= 0x40000u;
    return result;
}

std::uint32_t HardwareLegacyControlWord_544936(const std::uint32_t abstractValue)
{
    std::uint32_t result = 0;
    if ((abstractValue & 0x10u) != 0) result |= 0x01u;
    if ((abstractValue & 0x08u) != 0) result |= 0x04u;
    if ((abstractValue & 0x04u) != 0) result |= 0x08u;
    if ((abstractValue & 0x02u) != 0) result |= 0x10u;
    if ((abstractValue & 0x01u) != 0) result |= 0x20u;
    if ((abstractValue & 0x80000u) != 0) result |= 0x02u;
    switch (abstractValue & 0x300u) {
    case 0x100u: result |= 0x400u; break;
    case 0x200u: result |= 0x800u; break;
    case 0x300u: result |= 0xC00u; break;
    }
    switch (abstractValue & 0x30000u) {
    case 0: result |= 0x300u; break;
    case 0x10000u: result |= 0x200u; break;
    }
    if ((abstractValue & 0x40000u) != 0) result |= 0x1000u;
    return result;
}

std::uint32_t ControlLegacyX87_544859(
    const std::uint32_t value, const std::uint32_t mask)
{
    const std::uint32_t before = AbstractLegacyControlWord_5448A4(
        detail::Cg2dReadLegacyX87Control());
    const std::uint32_t merged = (before & ~mask) | (value & mask);
    const std::uint32_t hardware = HardwareLegacyControlWord_544936(merged);
    detail::Cg2dWriteLegacyX87Control(static_cast<std::uint16_t>(hardware));
    return merged;
}

std::uint32_t ControlLegacyFloatingPoint_54488E(
    const std::uint32_t value, const std::uint32_t mask)
{
    return ControlLegacyX87_544859(value, mask & 0xFFF7FFFFu);
}

void SetLegacyDefaultPrecision_53FD5C()
{
    static_cast<void>(ControlLegacyFloatingPoint_54488E(0x10000u, 0x30000u));
}

std::int32_t TestLegacyFloatingPointDivision_53FD6E()
{
    return detail::Cg2dTestLegacyFdiv();
}

std::int32_t ProbeLegacyFloatingPointDivision_53FDAC()
{
    const HMODULE module = ::GetModuleHandleA("KERNEL32");
    if (module != nullptr) {
        using FeatureQuery = BOOL (WINAPI*)(DWORD);
        const auto query = reinterpret_cast<FeatureQuery>(
            ::GetProcAddress(module, "IsProcessorFeaturePresent"));
        if (query != nullptr) {
            return static_cast<std::int32_t>(query(0));
        }
    }
    return TestLegacyFloatingPointDivision_53FD6E();
}

void InitializeLegacyFloatingPoint_53B447()
{
    InstallLegacyFloatCallbacks_53B45F();
    gLegacyFdivBug1085FC0 = ProbeLegacyFloatingPointDivision_53FDAC();
    SetLegacyDefaultPrecision_53FD5C();
    detail::Cg2dClearLegacyX87Exceptions();
}

// 原5461E4；只按F64存储位返回特殊类型，不进行浮点算术。
std::int32_t ClassifyLegacySpecialFloat_5461E4(const double value) noexcept
{
    enum : std::int32_t {
        kOrdinary = 0,
        kPositiveInfinity = 1,
        kNegativeInfinity = 2,
        kQuietNaN = 3,
        kSignalingNaN = 4,
    };
    const std::uint64_t bits = std::bit_cast<std::uint64_t>(value);
    const std::uint32_t low = static_cast<std::uint32_t>(bits);
    const std::uint32_t high = static_cast<std::uint32_t>(bits >> 32);
    if (high == 0x7FF00000u && low == 0) {
        return kPositiveInfinity;
    }
    if (high == 0xFFF00000u && low == 0) {
        return kNegativeInfinity;
    }
    const std::uint32_t top = (high >> 16) & 0x7FF8u;
    if (top == 0x7FF8u) {
        return kQuietNaN;
    }
    if (top == 0x7FF0u && ((high & 0x7FFFFu) != 0 || low != 0)) {
        return kSignalingNaN;
    }
    return kOrdinary;
}

// 原546311；原x87落存、比较和状态读取留在私有ASM边界。
std::int32_t ClassifyLegacyFloat_546311(const double value) noexcept
{
    return detail::Cg2dClassifyLegacyFloat(&value);
}

// 原5436AC；原两次舍入、F64落存和x87比较顺序见整数奇偶判定SPEC。
std::int32_t ClassifyLegacyInteger_5436AC(const double value) noexcept
{
    return detail::Cg2dClassifyLegacyInteger(&value);
}

// 原54357C；保原S32状态与选择性F64输出，不将状态改为是否处理布尔值。
std::int32_t ProcessLegacyPowerSpecial_54357C(
    const double base, const double exponent, double* const result) noexcept
{
    return detail::Cg2dProcessLegacyPowerSpecial(&base, &exponent, result);
}

// 原543405；按保存控制字的原PC位构造并加载临时x87控制字，恢复由调用父负责。
void LoadLegacyPowerControlWord_543405(const std::uint16_t savedControl) noexcept
{
    detail::Cg2dLoadLegacyPowerControlWord(savedControl);
}

// 原32B数学错误记录只扩已证名称指针；三个F64字段和S32类型不扩宽。
static_assert(offsetof(LegacyMathErrorRecord, type) == 0);
static_assert(offsetof(LegacyMathErrorRecord, functionName) == 8);
static_assert(offsetof(LegacyMathErrorRecord, argument1) == 16);
static_assert(offsetof(LegacyMathErrorRecord, argument2) == 24);
static_assert(offsetof(LegacyMathErrorRecord, result) == 32);
static_assert(sizeof(LegacyMathErrorRecord) == 40);

// 原5478B2；只写原共享错误槽的两个分支，返回原完整S32输入。
std::int32_t SetLegacyMathErrno_5478B2(const std::int32_t type) noexcept
{
    if (type == 1) {
        gLegacyErrno_1085FE0 = 33;
    } else if (type > 1 && type <= 3) {
        gLegacyErrno_1085FE0 = 34;
    }
    return type;
}

// 原5478D8完整XOR EAX,EAX/RET；不访问传入的借用记录。
std::int32_t DefaultLegacyMathError_5478D8(LegacyMathErrorRecord*) noexcept
{
    return 0;
}

// 原5478DB；等待式FSTSW及完整S16有符号扩展留在ASM。
std::int32_t ReadLegacyRawX87Status_5478DB() noexcept
{
    return detail::Cg2dReadLegacyRawX87Status();
}

// 原5478E9；先FNSTSW，再FNCLEX，返回清除前的完整有符号S16。
std::int32_t ReadAndClearLegacyRawX87Status_5478E9() noexcept
{
    return detail::Cg2dReadAndClearLegacyRawX87Status();
}

// 原5478F8；原硬件位合并，返回合并前控制字的S16符号扩展。
std::int32_t ControlLegacyRawX87_5478F8(
    const std::uint32_t value, const std::uint32_t mask) noexcept
{
    return detail::Cg2dControlLegacyRawX87(value, mask);
}

// 原54769B；原条件/控制字决定结果与状态生产，完整分支及依赖留在ASM。
std::int32_t HandleLegacyMathException_54769B(
    const std::uint32_t conditions, double* const result,
    const std::uint32_t control) noexcept
{
    return detail::Cg2dHandleLegacyMathException(conditions, result, control);
}

// 原54791B；只执行原已定义的x87状态生产操作，不借宿主状态接口。
void SetLegacyRawX87Status_54791B(const std::uint32_t status) noexcept
{
    detail::Cg2dSetLegacyRawX87Status(status);
}

// 原88B数值参数记录与三24B值单元；未生产的载荷/标志不默认初始化。
static_assert(offsetof(LegacyFloatExceptionValue, payload) == 0);
static_assert(offsetof(LegacyFloatExceptionValue, format) == 16);
static_assert(offsetof(LegacyFloatExceptionValue, reserved) == 20);
static_assert(sizeof(LegacyFloatExceptionValue) == 24);
static_assert(offsetof(LegacyFloatExceptionRecord, header) == 0);
static_assert(offsetof(LegacyFloatExceptionRecord, causes) == 4);
static_assert(offsetof(LegacyFloatExceptionRecord, enables) == 8);
static_assert(offsetof(LegacyFloatExceptionRecord, status) == 12);
static_assert(offsetof(LegacyFloatExceptionRecord, operand1) == 16);
static_assert(offsetof(LegacyFloatExceptionRecord, operand2) == 40);
static_assert(offsetof(LegacyFloatExceptionRecord, result) == 64);
static_assert(sizeof(LegacyFloatExceptionRecord) == 88);

// 原错误入口值快照；只把已证名称指针承为原生指针，不拓数值或填未生产字段。
static_assert(offsetof(LegacyMathErrorInputs, savedControl) == 0);
static_assert(offsetof(LegacyMathErrorInputs, operationId) == 4);
static_assert(offsetof(LegacyMathErrorInputs, functionName) == 8);
static_assert(offsetof(LegacyMathErrorInputs, argument1) == 16);
static_assert(offsetof(LegacyMathErrorInputs, argument2) == 24);
static_assert(sizeof(LegacyMathErrorInputs) == 32);

// 原53D019普通pow值入口；原算法留ASM，不借宿主数学或填原未生产辅助结果。
double PowerLegacyFloat_53D019(const double base, const double exponent) noexcept
{
    const double inputs[2] = {base, exponent};
    double result;
    detail::Cg2dComputeLegacyPower(inputs, &result);
    return result;
}

// 原帧数值片段保持，只拓描述符指针；conditionSample为原ECX数值的显式承载。
static_assert(offsetof(LegacyMathDispatchContext, savedControl) == 0);
static_assert(offsetof(LegacyMathDispatchContext, workingControl) == 2);
static_assert(offsetof(LegacyMathDispatchContext, status) == 4);
static_assert(offsetof(LegacyMathDispatchContext, scratch80) == 6);
static_assert(offsetof(LegacyMathDispatchContext, descriptor) == 16);
static_assert(offsetof(LegacyMathDispatchContext, errorType) == 24);
static_assert(offsetof(LegacyMathDispatchContext, conditionSample) == 28);
static_assert(sizeof(LegacyMathDispatchContext) == 32);
static_assert(sizeof(LegacyMathDispatchTarget) == 8);
static_assert(offsetof(LegacyUnaryMathDescriptor, targets) == 16);
static_assert(offsetof(LegacyBinaryMathDescriptor, targets) == 16);
static_assert(sizeof(LegacyUnaryMathDescriptor) == 48);
static_assert(sizeof(LegacyBinaryMathDescriptor) == 144);

// 原5473E8；原字段生产、x87落存与系统调用前后读取留在ASM。
void RaiseLegacyFloatingPointException_5473E8(
    LegacyFloatExceptionRecord* const record, std::uint32_t* const control,
    const std::uint32_t conditions, const std::uint32_t operationId,
    const double* const argument, double* const result) noexcept
{
    detail::Cg2dRaiseLegacyFloatingPointException(
        record, control, conditions, operationId, argument, result);
}

// 原5460EB；原保存控制字只读W16，原类型/真实依赖与返回保留。
std::int32_t ProcessLegacyX87Exception_5460EB(
    const std::uint32_t operationId, LegacyMathErrorRecord* const record,
    const std::uint16_t* const savedControl) noexcept
{
    return detail::Cg2dProcessLegacyX87Exception(operationId, record, savedControl);
}

namespace detail {

extern "C" std::int32_t Cg2dClassifyLegacySpecialFloat(
    const double* const value) noexcept
{
    return ClassifyLegacySpecialFloat_5461E4(*value);
}

// 原RaiseException以一个原生宽度参数传递本调用的记录指针槽。
extern "C" void Cg2dRaiseLegacyFloatExceptionHost(
    const std::uint32_t code, std::uintptr_t* const arguments) noexcept
{
    static_assert(sizeof(std::uintptr_t) == sizeof(ULONG_PTR));
    ::RaiseException(code, 0, 1, reinterpret_cast<const ULONG_PTR*>(arguments));
}

// 只把ASM调用接回已有原叶，不增加记录访问或错误槽owner。
extern "C" std::int32_t Cg2dDefaultLegacyMathError(
    LegacyMathErrorRecord* const record) noexcept
{
    return DefaultLegacyMathError_5478D8(record);
}

extern "C" std::int32_t Cg2dSetLegacyMathErrno(const std::int32_t type) noexcept
{
    return SetLegacyMathErrno_5478B2(type);
}

// 原542FE2读取唯一已有1085FC0探测owner；仅做Win64私有值桥，不缓存/改写。
extern "C" std::int32_t Cg2dReadLegacyFdivBug() noexcept
{
    return gLegacyFdivBug1085FC0;
}

} // namespace detail

} // namespace cg::platform
