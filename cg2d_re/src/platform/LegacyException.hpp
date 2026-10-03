#pragma once

#include <cstddef>
#include <cstdint>

#include "LegacyNarrowString.hpp"

namespace cg::platform {

struct LegacyExceptionObject;

// 原5501C8的两个真实方法槽；原生显式this，不是宿主C++虚表/RTTI/ThrowInfo。
struct LegacyExceptionMethodTable final {
    LegacyExceptionObject* (*deletingDestructor)(
        LegacyExceptionObject* storage, std::uint32_t flags);
    const char* (*what)(const LegacyExceptionObject* storage);
};

// 原exception已证+00/+04/+08；仅地址拓宽，拥有标志是完整W32，不是U8。
struct LegacyExceptionObject final {
    const LegacyExceptionMethodTable* methods; // 原+00；当前真实两槽原生方法表。
    char* text;                               // 原+04；借用或原堆拥有文本。
    std::uint32_t ownsText;                    // 原+08；保留任意非零位型。
};
static_assert(offsetof(LegacyExceptionObject, text) == 8);
static_assert(offsetof(LegacyExceptionObject, ownsText) == 16);
static_assert(sizeof(LegacyExceptionObject) == 24);
static_assert(sizeof(LegacyExceptionMethodTable) == 16);

// 不自动构析/抛出；实际对象/文本必须沿原堆域与显式生命周期使用。
LegacyExceptionObject* ConstructLegacyExceptionText_54B079(
    LegacyExceptionObject* storage, const char* const* sourceText);
LegacyExceptionObject* CopyConstructLegacyException_54B0B6(
    LegacyExceptionObject* storage, const LegacyExceptionObject* source);
void DestroyLegacyException_54B100(LegacyExceptionObject* storage);
LegacyExceptionObject* DeleteLegacyException_54B05D(
    LegacyExceptionObject* storage, std::uint32_t flags);
const char* GetLegacyExceptionText_54B116(const LegacyExceptionObject* storage);

// 原logic_error在基类两槽后还有Raise槽；不与宿主C++异常元数据互换。
struct LegacyExceptionRaiseMethodTable final {
    LegacyExceptionMethodTable base;
    void (*raise)(LegacyExceptionObject* storage);
};
static_assert(offsetof(LegacyExceptionRaiseMethodTable, base) == 0);
static_assert(offsetof(LegacyExceptionRaiseMethodTable, raise) == 16);
static_assert(sizeof(LegacyExceptionRaiseMethodTable) == 24);

// 原28B（1CH）对象：exception+0C的string；地址拓宽后24B基类+24B成员。
struct LegacyLogicErrorObject final {
    LegacyExceptionObject base; // 原+00；实际首成员，方法槽显式this按此地址适配。
    LegacyNarrowString message;  // 原+0C；只按原写首前缀字节及Tidy/assign。
};
static_assert(offsetof(LegacyLogicErrorObject, base) == 0);
static_assert(offsetof(LegacyLogicErrorObject, message) == 24);
static_assert(offsetof(LegacyLogicErrorObject, message)
    + offsetof(LegacyNarrowString, buffer) == 32);
static_assert(sizeof(LegacyLogicErrorObject) == 48);

LegacyLogicErrorObject* ConstructLegacyLogicError_54A6D4(
    LegacyLogicErrorObject* storage, const LegacyNarrowString* source);
LegacyLogicErrorObject* CopyConstructLegacyLogicError_54A7AE(
    LegacyLogicErrorObject* storage, const LegacyLogicErrorObject* source);
void DestroyLegacyLogicError_54A755(LegacyLogicErrorObject* storage);
LegacyLogicErrorObject* DeleteLegacyLogicError_54A792(
    LegacyLogicErrorObject* storage, std::uint32_t flags);
const char* GetLegacyLogicErrorText_54A062(const LegacyLogicErrorObject* storage);

// 原54A738：实际复制后CxxThrowException入口，仅声明；不补固定异常或返回。
void RaiseLegacyLogicError_54A738(LegacyExceptionObject* storage);

// 只复用已证48B存储视图；原方法表/ThrowInfo身份各自独立，不是C++抛出类型合并。
using LegacyStringLengthErrorObject = LegacyLogicErrorObject;
void DestroyLegacyStringLengthError_54A80B(LegacyStringLengthErrorObject* storage);
LegacyStringLengthErrorObject* DeleteLegacyStringLengthError_54A865(
    LegacyStringLengthErrorObject* storage, std::uint32_t flags);
LegacyStringLengthErrorObject* CopyConstructLegacyStringLengthError_54A881(
    LegacyStringLengthErrorObject* storage, const LegacyStringLengthErrorObject* source);
// 原54A848实际拷贝后抛出；仅真实声明，不补固定异常或空槽。
void RaiseLegacyStringLengthError_54A848(LegacyExceptionObject* storage);

using LegacyStringRangeErrorObject = LegacyLogicErrorObject;
void DestroyLegacyStringRangeError_54A8F3(LegacyStringRangeErrorObject* storage);
LegacyStringRangeErrorObject* DeleteLegacyStringRangeError_54A94D(
    LegacyStringRangeErrorObject* storage, std::uint32_t flags);
LegacyStringRangeErrorObject* CopyConstructLegacyStringRangeError_54A969(
    LegacyStringRangeErrorObject* storage, const LegacyStringRangeErrorObject* source);
// 原54A930实际拷贝后抛出；仅真实声明，不补固定异常或空槽。
void RaiseLegacyStringRangeError_54A930(LegacyExceptionObject* storage);

} // namespace cg::platform
