#include "LegacyFloatEnvironment.hpp"
#include "LegacyFormat.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyStringCompareState.hpp"

#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <float.h>
#include <intrin.h>
#include <limits>

#pragma intrinsic(__movsb, __movsd)

namespace cg::platform {
namespace {

// ACP01：只复制实际原宽载荷，避免对原未生产标量直接作CPP求值。
std::uint32_t ReadLegacyMathW32(const void* const bytes) noexcept
{
    static_assert(sizeof(unsigned long) == sizeof(std::uint32_t));
    unsigned long value;
    __movsd(&value, reinterpret_cast<unsigned long*>(const_cast<void*>(bytes)), 1);
    return static_cast<std::uint32_t>(value);
}

std::uint8_t ReadLegacyMathByte(const volatile char* const byte) noexcept
{
    unsigned char value;
    __movsb(&value, reinterpret_cast<unsigned char*>(const_cast<char*>(byte)), 1);
    return value;
}

// 原状态/控制低六位到宿主抽象位的映射相同；不携带PC/IC到x64控制接口。
std::uint32_t ReadHostStatusAsLegacy() noexcept
{
    return HardwareLegacyControlWord_544936(_statusfp()) & 0x3Fu;
}

void StoreLegacyExceptionDouble(LegacyFloatExceptionValue* const value, const double input) noexcept
{
    std::memcpy(value->payload, &input, sizeof(input));
}

double ReadLegacyExceptionDouble(const LegacyFloatExceptionValue* const value) noexcept
{
    double result;
    std::memcpy(&result, value->payload, sizeof(result));
    return result;
}

// 原543390/5433A7的值参数承接；单参路径不生产argument2。
double StartLegacyMathError(
    const LegacyMathErrorInputs& inputs, const std::int32_t type,
    const double value, const bool twoArguments) noexcept
{
    LegacyMathErrorRecord record;
    record.type = type;
    if (twoArguments) record.argument2 = inputs.argument2;
    record.result = value;
    record.functionName = inputs.functionName;
    record.argument1 = inputs.argument1;
    const std::uint16_t savedControl = static_cast<std::uint16_t>(inputs.savedControl);
    static_cast<void>(ProcessLegacyX87Exception_5460EB(inputs.operationId, &record, &savedControl));
    if (savedControl != 0x027Fu) detail::Cg2dWriteLegacyX87Control(savedControl);
    return record.result;
}

} // namespace

// 类型明确的宿主数值入口；取代只在ASM内部互调的ST0/FLAGS数学内核。
double ComputeLegacyUnaryMathNative(const LegacyUnaryMathOperation operation, const double value) noexcept
{
    switch (operation) {
    case LegacyUnaryMathOperation::Exp: return std::exp(value);
    case LegacyUnaryMathOperation::Exp2: return std::exp2(value);
    case LegacyUnaryMathOperation::ExpMinusOne: return std::expm1(value);
    case LegacyUnaryMathOperation::NaturalLog: return std::log(value);
    case LegacyUnaryMathOperation::Log10: return std::log10(value);
    case LegacyUnaryMathOperation::Tangent: return std::tan(value);
    }
    __assume(0); // 私有枚举仅由对应数学调用方选取，不新增业务默认值。
}

double ComputeLegacyBinaryMathNative(
    const LegacyBinaryMathOperation operation, const double first, const double second) noexcept
{
    switch (operation) {
    case LegacyBinaryMathOperation::Power: return std::pow(first, second);
    case LegacyBinaryMathOperation::Divide: return first / second;
    case LegacyBinaryMathOperation::ReverseDivide: return second / first;
    case LegacyBinaryMathOperation::Remainder: return std::fmod(first, second);
    case LegacyBinaryMathOperation::NearestRemainder: return std::remainder(first, second);
    case LegacyBinaryMathOperation::Arctangent: return std::atan2(first, second);
    }
    __assume(0);
}

double DecomposeLegacyFloatNative(const double value, std::int32_t* const exponent) noexcept
{
    static_assert(sizeof(int) == sizeof(std::int32_t));
    int hostExponent;
    const double fraction = std::frexp(value, &hostExponent);
    *exponent = std::bit_cast<std::int32_t>(ReadLegacyMathW32(&hostExponent));
    return fraction;
}

double SetLegacyFloatExponentNative(const double value, const std::int32_t exponent) noexcept
{
    std::uint64_t bits = std::bit_cast<std::uint64_t>(value);
    const std::uint32_t highWord = static_cast<std::uint32_t>(bits >> 48) & 0x800Fu;
    const std::uint16_t replacement = static_cast<std::uint16_t>(
        ((static_cast<std::uint32_t>(exponent) + 0x3FEu) << 4) | highWord);
    bits = (bits & 0x0000FFFFFFFFFFFFull) | (static_cast<std::uint64_t>(replacement) << 48);
    return std::bit_cast<double>(bits);
}

// 原Shop调用仍只读一个W32；P07未生产的第二命令不在本步补值。
extern "C" std::int32_t Cg2dReadLegacyShopCommandW32(const void* const bytes) noexcept
{
    return std::bit_cast<std::int32_t>(ReadLegacyMathW32(bytes));
}

namespace detail {

extern "C" std::uint16_t Cg2dReadLegacyX87Control() noexcept
{
    // 宿主double采用53有效位；40h为旧控制字固定保留位，不另造影子状态。
    const std::uint32_t control = _control87(0, 0);
    return static_cast<std::uint16_t>(HardwareLegacyControlWord_544936(control | _PC_53) | 0x40u);
}

extern "C" void Cg2dWriteLegacyX87Control(const std::uint16_t control) noexcept
{
    static_cast<void>(_control87(AbstractLegacyControlWord_5448A4(control), _MCW_EM | _MCW_RC));
}

extern "C" void Cg2dClearLegacyX87Exceptions() noexcept
{
    static_cast<void>(_clearfp());
}

extern "C" std::int32_t Cg2dTestLegacyFdiv() noexcept
{
    const double divisor = std::bit_cast<double>(std::uint64_t{0x4147FFFF80000000});
    const double dividend = std::bit_cast<double>(std::uint64_t{0x4150017EC0000000});
    const double quotient = dividend / divisor;
    const double remainder = dividend - quotient * divisor;
    return remainder > 1.0 ? 1 : 0;
}

extern "C" std::int32_t Cg2dLegacyFloatNonnegative(const double* const value) noexcept
{
    return *value >= 0.0 ? 1 : 0;
}

extern "C" std::int32_t Cg2dClassifyLegacyFloat(const double* const value) noexcept
{
    const std::uint64_t bits = std::bit_cast<std::uint64_t>(*value);
    const bool negative = (bits >> 63) != 0;
    const std::uint64_t exponent = bits & 0x7FF0000000000000ull;
    const std::uint64_t fraction = bits & 0x000FFFFFFFFFFFFFull;
    if (exponent == 0x7FF0000000000000ull) {
        if (fraction == 0) return negative ? 4 : 0x200;
        if ((fraction & 0x0008000000000000ull) == 0) {
            static_cast<void>(std::feraiseexcept(FE_INVALID));
        }
        return 2; // 原受掩码FLD/FSTP将sNaN安静化，分类为quiet NaN。
    }
    if (exponent == 0 && fraction != 0) return negative ? 0x110 : 0x80;
    if (exponent == 0) return negative ? 0x120 : 0x40;
    return negative ? 0x108 : 0x100;
}

extern "C" std::int32_t Cg2dClassifyLegacyInteger(const double* const value) noexcept
{
    const double input = *value;
    if ((Cg2dClassifyLegacyFloat(&input) & 0x90) != 0) return 0;
    const double rounded = std::rint(input);
    // 原JNE只在有序不等时走非整数，不能用NaN != NaN代替。
    if (rounded < input || rounded > input) return 0;
    const double half = input / 2.0;
    const double roundedHalf = std::rint(half);
    return roundedHalf < half || roundedHalf > half ? 1 : 2;
}

extern "C" std::int32_t Cg2dProcessLegacyPowerSpecial(
    const double* const base, const double* const exponent, double* const result) noexcept
{
    const double first = *base;
    const double second = *exponent;
    const std::uint64_t firstBits = std::bit_cast<std::uint64_t>(first);
    const std::uint64_t secondBits = std::bit_cast<std::uint64_t>(second);
    const double magnitude = std::fabs(first);
    const double infinity = std::numeric_limits<double>::infinity();
    if (secondBits == 0x7FF0000000000000ull) {
        if (magnitude > 1.0) { *result = infinity; return 0; }
        if (!(magnitude >= 1.0)) { *result = 0.0; return 0; }
        *result = std::bit_cast<double>(std::uint64_t{0xFFF8000000000000});
        return 1;
    }
    if (secondBits == 0xFFF0000000000000ull) {
        if (magnitude > 1.0) { *result = 0.0; return 0; }
        if (!(magnitude >= 1.0)) { *result = infinity; return 0; }
        *result = std::bit_cast<double>(std::uint64_t{0xFFF8000000000000});
        return 1;
    }
    if (firstBits == 0x7FF0000000000000ull) {
        *result = second > 0.0 ? infinity : (second >= 0.0 ? 1.0 : 0.0);
    } else if (firstBits == 0xFFF0000000000000ull) {
        const std::int32_t integerClass = Cg2dClassifyLegacyInteger(&second);
        if (second > 0.0) *result = integerClass == 1 ? -infinity : infinity;
        else if (second >= 0.0) *result = 1.0;
        else *result = integerClass == 1 ? -0.0 : 0.0;
    }
    return 0; // 原普通值分支不写结果；返回0并不表示已生产输出。
}

extern "C" void Cg2dLoadLegacyPowerControlWord(const std::uint16_t savedControl) noexcept
{
    Cg2dWriteLegacyX87Control(static_cast<std::uint16_t>((savedControl & 0x300u) | 0x7Fu));
}

extern "C" std::int32_t Cg2dReadLegacyRawX87Status() noexcept
{
    return static_cast<std::int16_t>(ReadHostStatusAsLegacy());
}

extern "C" std::int32_t Cg2dReadAndClearLegacyRawX87Status() noexcept
{
    const std::int32_t status = Cg2dReadLegacyRawX87Status();
    static_cast<void>(_clearfp());
    return status;
}

extern "C" std::int32_t Cg2dControlLegacyRawX87(
    const std::uint32_t value, const std::uint32_t mask) noexcept
{
    const std::uint16_t previous = Cg2dReadLegacyX87Control();
    Cg2dWriteLegacyX87Control(static_cast<std::uint16_t>((previous & ~mask) | (value & mask)));
    return static_cast<std::int16_t>(previous);
}

extern "C" void Cg2dSetLegacyRawX87Status(const std::uint32_t status) noexcept
{
    // 保留原五类标志的生产序，由真实宿主fenv承接，无影子状态或异常样例。
    if ((status & 1u) != 0) static_cast<void>(std::feraiseexcept(FE_INVALID));
    if ((status & 8u) != 0) static_cast<void>(std::feraiseexcept(FE_OVERFLOW));
    if ((status & 0x10u) != 0) static_cast<void>(std::feraiseexcept(FE_UNDERFLOW));
    if ((status & 4u) != 0) static_cast<void>(std::feraiseexcept(FE_DIVBYZERO));
    if ((status & 0x20u) != 0) static_cast<void>(std::feraiseexcept(FE_INEXACT));
}

extern "C" std::int32_t Cg2dHandleLegacyMathException(
    const std::uint32_t conditions, double* const result, const std::uint32_t control) noexcept
{
    std::uint32_t remaining = conditions & 0x1Fu;
    if ((conditions & 8u) != 0 && (control & 1u) != 0) {
        Cg2dSetLegacyRawX87Status(1);
        remaining &= ~8u;
    } else if ((conditions & 4u) != 0 && (control & 4u) != 0) {
        Cg2dSetLegacyRawX87Status(4);
        remaining &= ~4u;
    } else if ((conditions & 1u) != 0 && (control & 8u) != 0) {
        Cg2dSetLegacyRawX87Status(8);
        const bool positive = *result > 0.0;
        const double infinity = std::numeric_limits<double>::infinity();
        const double maximum = std::numeric_limits<double>::max();
        switch (control & 0xC00u) {
        case 0: *result = positive ? infinity : -infinity; break;
        case 0x400: *result = positive ? maximum : -infinity; break;
        case 0x800: *result = positive ? infinity : -maximum; break;
        case 0xC00: *result = positive ? maximum : -maximum; break;
        }
        remaining &= ~1u;
    } else if ((conditions & 2u) != 0 && (control & 0x10u) != 0) {
        const double value = *result;
        bool lostPrecision = (conditions & 0x10u) != 0;
        if (value == 0.0 || std::isnan(value)) {
            lostPrecision = true;
        } else {
            // 原分解指数减600h/尾数移位由标准缩放近似承接。
            const double scaled = std::scalbn(value, -1536);
            *result = scaled;
            lostPrecision = lostPrecision || scaled == 0.0 || std::scalbn(scaled, 1536) != value;
        }
        if (lostPrecision) Cg2dSetLegacyRawX87Status(0x10);
        remaining &= ~2u;
    }
    if ((conditions & 0x10u) != 0 && (control & 0x20u) != 0) {
        Cg2dSetLegacyRawX87Status(0x20);
        remaining &= ~0x10u;
    }
    return remaining == 0 ? 1 : 0;
}

extern "C" void Cg2dRaiseLegacyFloatingPointException(
    LegacyFloatExceptionRecord* const record, std::uint32_t* const control,
    const std::uint32_t conditions, const std::uint32_t operationId,
    const double* const argument, double* const result) noexcept
{
    record->causes = 0;
    record->enables = 0;
    record->status = 0;
    std::uint32_t code = conditions;
    if ((conditions & 0x10u) != 0) { code = 0xC000008Fu; record->causes |= 1u; }
    if ((conditions & 2u) != 0) { code = 0xC0000093u; record->causes |= 2u; }
    if ((conditions & 1u) != 0) { code = 0xC0000091u; record->causes |= 4u; }
    if ((conditions & 4u) != 0) { code = 0xC000008Eu; record->causes |= 8u; }
    if ((conditions & 8u) != 0) { code = 0xC0000090u; record->causes |= 0x10u; }
    // 五次原控制W32重读，允许与记录/系统回写共享存储。
    std::uint32_t sampledControl = ReadLegacyMathW32(control);
    record->enables = (record->enables & ~0x10u) | ((~sampledControl & 1u) << 4);
    sampledControl = ReadLegacyMathW32(control);
    record->enables = (record->enables & ~8u) | ((~sampledControl & 4u) << 1);
    sampledControl = ReadLegacyMathW32(control);
    record->enables = (record->enables & ~4u) | ((~sampledControl >> 1) & 4u);
    sampledControl = ReadLegacyMathW32(control);
    record->enables = (record->enables & ~2u) | ((~sampledControl >> 3) & 2u);
    sampledControl = ReadLegacyMathW32(control);
    record->enables = (record->enables & ~1u) | ((~sampledControl >> 5) & 1u);
    const std::uint32_t status = static_cast<std::uint32_t>(Cg2dReadLegacyRawX87Status());
    if ((status & 1u) != 0) record->status |= 0x10u;
    if ((status & 4u) != 0) record->status |= 8u;
    if ((status & 8u) != 0) record->status |= 4u;
    if ((status & 0x10u) != 0) record->status |= 2u;
    if ((status & 0x20u) != 0) record->status |= 1u;
    sampledControl = ReadLegacyMathW32(control);
    record->header = (ReadLegacyMathW32(&record->header) & ~3u) | ((sampledControl & 0xC00u) >> 10);
    switch (ReadLegacyMathW32(control) & 0x300u) {
    case 0: record->header = (ReadLegacyMathW32(&record->header) & 0xFFFFFFEBu) | 8u; break;
    case 0x200: record->header = (ReadLegacyMathW32(&record->header) & 0xFFFFFFE7u) | 4u; break;
    case 0x300: record->header = ReadLegacyMathW32(&record->header) & 0xFFFFFFE3u; break;
    }
    record->header = (ReadLegacyMathW32(&record->header) & 0xFFFE001Fu) | ((operationId & 0xFFFu) << 5);
    record->operand1.format = ReadLegacyMathW32(&record->operand1.format) | 1u;
    record->operand1.format = (ReadLegacyMathW32(&record->operand1.format) & 0xFFFFFFE3u) | 2u;
    StoreLegacyExceptionDouble(&record->operand1, *argument);
    record->result.format = ReadLegacyMathW32(&record->result.format) | 1u;
    record->result.format = (ReadLegacyMathW32(&record->result.format) & 0xFFFFFFE3u) | 2u;
    StoreLegacyExceptionDouble(&record->result, *result);
    static_cast<void>(Cg2dReadAndClearLegacyRawX87Status());
    std::uintptr_t arguments = reinterpret_cast<std::uintptr_t>(record);
    Cg2dRaiseLegacyFloatExceptionHost(code, &arguments);
    if ((record->enables & 0x10u) != 0) *control &= ~1u;
    if ((record->enables & 8u) != 0) *control &= ~4u;
    if ((record->enables & 4u) != 0) *control &= ~8u;
    if ((record->enables & 2u) != 0) *control &= ~0x10u;
    if ((record->enables & 1u) != 0) *control &= ~0x20u;
    const std::uint32_t rounding = ReadLegacyMathW32(&record->header) & 3u;
    *control = (*control & ~0xC00u) | (rounding << 10);
    // 保原共享后缀实际掩码（清RC位）；不按字段名称修正旧PC写回。
    switch ((ReadLegacyMathW32(&record->header) >> 2) & 7u) {
    case 0: *control = (*control & 0xFFFFF3FFu) | 0x300u; break;
    case 1: *control = (*control & 0xFFFFF3FFu) | 0x200u; break;
    case 2: *control &= 0xFFFFF3FFu; break;
    }
    *result = ReadLegacyExceptionDouble(&record->result);
}

extern "C" std::int32_t Cg2dProcessLegacyX87Exception(
    const std::uint32_t operationId, LegacyMathErrorRecord* const record,
    const std::uint16_t* const savedControl) noexcept
{
    std::uint32_t control = *savedControl;
    std::uint32_t conditions = 0;
    switch (record->type) {
    case 1: case 5: conditions = 8; break;
    case 2: conditions = 4; break;
    case 3: conditions = 0x11; break;
    case 4: conditions = 0x12; break;
    case 7: record->type = 1; break;
    case 8: conditions = 0x10; break;
    }
    if (conditions != 0 && Cg2dHandleLegacyMathException(conditions, &record->result, control) == 0) {
        LegacyFloatExceptionRecord exception;
        if (operationId == 0x10u || operationId == 0x16u || operationId == 0x1Du) {
            const std::uint32_t format = ReadLegacyMathW32(&exception.operand2.format);
            StoreLegacyExceptionDouble(&exception.operand2, record->argument2);
            exception.operand2.format = (format & 0xFFFFFFE3u) | 3u;
        } else {
            exception.operand2.format = ReadLegacyMathW32(&exception.operand2.format) & ~1u;
        }
        Cg2dRaiseLegacyFloatingPointException(
            &exception, &control, conditions, operationId, &record->argument1, &record->result);
    }
    static_cast<void>(Cg2dControlLegacyRawX87(control, 0xFFFFu));
    if (record->type != 8 && Cg2dLegacyMathErrorCallbackGate5925B8 == 0) {
        const std::int32_t handled = Cg2dDefaultLegacyMathError(record);
        if (handled != 0) return handled;
    }
    return Cg2dSetLegacyMathErrno(record->type);
}

extern "C" void Cg2dComputeLegacyPower(const double* const inputs, double* const result) noexcept
{
    const std::uint16_t savedControl = Cg2dReadLegacyX87Control();
    Cg2dLoadLegacyPowerControlWord(savedControl);
    const double exponent = inputs[1];
    const double base = inputs[0];
    double value = ComputeLegacyBinaryMathNative(LegacyBinaryMathOperation::Power, base, exponent);
    if (Cg2dLegacyPowerFastExitGate1085FBC == 0) {
        const std::uint32_t status = ReadHostStatusAsLegacy();
        std::int32_t error = 0;
        if (std::isnan(base) || std::isnan(exponent)) {
            const bool signaling = ClassifyLegacySpecialFloat_5461E4(base) == 4 ||
                ClassifyLegacySpecialFloat_5461E4(exponent) == 4;
            error = signaling ? 1 : 7;
        } else if (std::isnan(value)) error = 1;
        else if (base == 0.0 && exponent < 0.0) error = 2;
        else if (std::isfinite(base) && std::isfinite(exponent) && std::isinf(value)) error = 3;
        else if (base != 0.0 && std::isfinite(base) && std::isfinite(exponent) &&
            (value == 0.0 || std::fpclassify(value) == FP_SUBNORMAL)) error = 4;
        else if ((savedControl & 0x20u) == 0 && (status & 0x20u) != 0) error = 8;
        if (error != 0) {
            const LegacyMathErrorInputs errorInputs {savedControl, 0x1Du, "pow", base, exponent};
            // 原范围出口先将下溢结果放大600h再交掩码处理；宿主pow已落F64，
            // 在此保留同一缩放边界，避免对已经是最终结果的非零次正规数再次缩小。
            if (error == 4) value = std::scalbn(value, 1536);
            value = StartLegacyMathError(errorInputs, error, value, true);
        }
    }
    if (savedControl != 0x027Fu) Cg2dWriteLegacyX87Control(savedControl);
    *result = value;
}

extern "C" void Cg2dLegacyNullsub7() noexcept
{
    // 原53DEF3只有RET；无原调用不虚构接线。
}

extern "C" std::uint32_t Cg2dReadLegacyExponentBits(const double* const value) noexcept
{
    const void* const high = reinterpret_cast<const char*>(value) + 4;
    const std::uint32_t exponent = ReadLegacyMathW32(high) & 0x7FF00000u;
    return exponent == 0x7FF00000u ? ReadLegacyMathW32(high) : exponent;
}

extern "C" std::uint8_t Cg2dReadLegacyCountByte(const volatile char* const byte) noexcept
{
    return ReadLegacyMathByte(byte);
}

extern "C" std::uint32_t Cg2dReadLegacyMappedPair(const char* const bytes) noexcept
{
    const std::uint32_t first = ReadLegacyMathByte(bytes);
    const std::uint32_t second = ReadLegacyMathByte(bytes + 1);
    return (first << 8) + second;
}

} // namespace detail
} // namespace cg::platform
