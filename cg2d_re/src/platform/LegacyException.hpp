#pragma once

#include <cstddef>
#include <cstdint>

#include "LegacyNarrowString.hpp"

namespace cg::platform {

// 原PMD三个连续S32位移；只作为异常对象校正数据，不生成宿主RTTI/ThrowInfo。
struct LegacyExceptionPmd final {
    std::int32_t memberDisplacement; // 原+00，先加到对象地址。
    std::int32_t virtualBasePointerDisplacement; // 原+04，非负才从原对象此处读取虚基表指针。
    std::int32_t virtualBaseDisplacement; // 原+08，虚基表内S32表项的字节位移。
};
static_assert(sizeof(LegacyExceptionPmd) == 12);
static_assert(offsetof(LegacyExceptionPmd, virtualBasePointerDisplacement) == 4);
static_assert(offsetof(LegacyExceptionPmd, virtualBaseDisplacement) == 8);
// 仅显式校正非拥有对象地址；不补PMD值/元数据或启用异常主链。
void* AdjustLegacyExceptionObject_5409E7(void* storage, const LegacyExceptionPmd* displacement);

// 原TypeDescriptor已证两个地址前缀；名称字节紧随前缀，不定义名称容量或宿主RTTI。
struct LegacyExceptionTypeDescriptorMethods;
struct LegacyExceptionTypeDescriptorPrefix final {
    const LegacyExceptionTypeDescriptorMethods* methods; // 原+00方法表地址，本匹配函数不调用。
    char* cachedName; // 原+04缓存地址；54AC66按原堆释放，本匹配函数不读取。
};
static_assert(sizeof(LegacyExceptionTypeDescriptorPrefix) == 16);
static_assert(offsetof(LegacyExceptionTypeDescriptorPrefix, cachedName) == 8);

// 六个已加载TypeDescriptor的数据存储；前缀为首成员，后接精确原ASCII名称/NUL。
// 模板只承载已证字节，不定义原SDK RTTI/ThrowInfo、额外字段或自动构析。
template <std::size_t NameBytes>
struct LegacyExceptionTypeDescriptorStorage final {
    LegacyExceptionTypeDescriptorPrefix prefix; // 供5404FA/54AC66/54AC7B显式接口读取或清理。
    char name[NameBytes]; // 原+08起内联名称，原生+16；不读取cachedName代替名称。
};
extern LegacyExceptionTypeDescriptorStorage<16> gLegacyExceptionType592B28;
extern LegacyExceptionTypeDescriptorStorage<24> gLegacyRuntimeErrorType592B40;
extern LegacyExceptionTypeDescriptorStorage<27> gLegacyStreamFailureType592B60;
extern LegacyExceptionTypeDescriptorStorage<22> gLegacyLogicErrorType592BD0;
extern LegacyExceptionTypeDescriptorStorage<23> gLegacyStringLengthErrorType592BF0;
extern LegacyExceptionTypeDescriptorStorage<23> gLegacyStringRangeErrorType592C10;

// 原5501C0只有删除析构一槽；5501C4是另一类RTTI数据，不并入本表。
struct LegacyExceptionTypeDescriptorMethods final {
    LegacyExceptionTypeDescriptorPrefix* (*deletingDestructor)(
        LegacyExceptionTypeDescriptorPrefix* storage, std::uint32_t flags);
};
static_assert(sizeof(LegacyExceptionTypeDescriptorMethods) == 8);
// 只显式清理原堆缓存/对象；不自动构析或清指针，不生成宿主RTTI元数据。
void DestroyLegacyExceptionTypeDescriptor_54AC66(LegacyExceptionTypeDescriptorPrefix* storage);
LegacyExceptionTypeDescriptorPrefix* DeleteLegacyExceptionTypeDescriptor_54AC7B(
    LegacyExceptionTypeDescriptorPrefix* storage, std::uint32_t flags);

// 原HandlerType/CatchableType只声明匹配所读前缀，不补后续字段、生产或默认值。
struct LegacyExceptionHandlerTypePrefix final {
    std::uint32_t properties; // 原+00；匹配按低U8读取位01/02/08。
    const LegacyExceptionTypeDescriptorPrefix* type; // 原+04；空地址是原直接匹配分支。
};
struct LegacyExceptionCatchableTypePrefix final {
    std::uint32_t properties; // 原+00；匹配只读低U8的位02。
    const LegacyExceptionTypeDescriptorPrefix* type; // 原+04；描述符身份先于名称比较。
};
static_assert(sizeof(LegacyExceptionHandlerTypePrefix) == 16);
static_assert(offsetof(LegacyExceptionHandlerTypePrefix, type) == 8);
static_assert(sizeof(LegacyExceptionCatchableTypePrefix) == 16);
static_assert(offsetof(LegacyExceptionCatchableTypePrefix, type) == 8);

// 原ThrowInfo仅此W32被5404FA读取；完整LegacyCppThrowInfo继续不透明。
struct LegacyExceptionThrowInfoPrefix final {
    std::uint32_t attributes; // 原+00；一次缓存W32后核低位01/02。
};
static_assert(sizeof(LegacyExceptionThrowInfoPrefix) == 4);
// 原5404FA返回完整S32的0/1；只消费真实原生前缀，不启用异常父链或生成SDK元数据。
std::int32_t MatchLegacyExceptionType_5404FA(
    const LegacyExceptionHandlerTypePrefix* handler,
    const LegacyExceptionCatchableTypePrefix* catchable,
    const LegacyExceptionThrowInfoPrefix* throwInfo);

// 原TryBlockMapEntry五字段/20B，仅最后的handler地址拓宽；不补handler本体/布局。
struct LegacyExceptionHandlerTypeEntry;
struct LegacyExceptionTryBlockMapEntry final {
    std::int32_t tryLow; // 原+00；5402AB按S32比较状态下界。
    std::int32_t tryHigh; // 原+04；53B75B先读此S32。
    std::int32_t catchHigh; // 原+08；仅tryHigh<state时读取。
    std::int32_t catchCount; // 原+0C；5402AB按S32判断是否存在catch。
    const LegacyExceptionHandlerTypeEntry* handlers; // 原+10；完整handler布局仍不透明。
};
static_assert(sizeof(LegacyExceptionTryBlockMapEntry) == 24);
static_assert(offsetof(LegacyExceptionTryBlockMapEntry, tryHigh) == 4);
static_assert(offsetof(LegacyExceptionTryBlockMapEntry, catchHigh) == 8);
static_assert(offsetof(LegacyExceptionTryBlockMapEntry, catchCount) == 12);
static_assert(offsetof(LegacyExceptionTryBlockMapEntry, handlers) == 16);

// 原FuncInfo只声明至try表指针的已证前缀；不生成旧帧/完整SDK元数据。
struct LegacyExceptionUnwindMapEntry;
struct LegacyExceptionFuncInfoPrefix final {
    std::uint32_t signature; // 原+00，已加载原值19930520；本范围函数不读取。
    std::int32_t maxState; // 原+04；540557的S32状态上界。
    const LegacyExceptionUnwindMapEntry* unwindMap; // 原+08；本范围函数不读取。
    std::uint32_t tryBlockCount; // 原+0C；首读缓存，输出发布后按原时序复读。
    const LegacyExceptionTryBlockMapEntry* tryBlocks; // 原+10；缓存原生非拥有表地址。
};
static_assert(sizeof(LegacyExceptionFuncInfoPrefix) == 32);
static_assert(offsetof(LegacyExceptionFuncInfoPrefix, maxState) == 4);
static_assert(offsetof(LegacyExceptionFuncInfoPrefix, unwindMap) == 8);
static_assert(offsetof(LegacyExceptionFuncInfoPrefix, tryBlockCount) == 16);
static_assert(offsetof(LegacyExceptionFuncInfoPrefix, tryBlocks) == 24);

// 原58FB74已加载540A5C；只承接原共享槽，不安装宿主异常钩子。
extern void (*gLegacyInconsistencyHandler58FB74)();
[[noreturn]] void ReportLegacyExceptionInconsistency_540AB2();
// nesting/state保持S32，输出保持U32；先发布两输出再复读数量，保留地址别名。
const LegacyExceptionTryBlockMapEntry* GetLegacyTryRange_53B75B(
    const LegacyExceptionFuncInfoPrefix* information, std::int32_t nesting,
    std::int32_t state, std::uint32_t* firstIndex, std::uint32_t* lastIndex);

// 原CRT两验证叶：地址拓宽/长度保持U32，原API非零返回0，零返回1。
// 仅供原异常链调用，不新增业务保护或专项模拟坏指针。
std::int32_t ValidateLegacyExceptionRead_54509E(const void* memory, std::uint32_t byteCount);
std::int32_t ValidateLegacyExceptionWrite_5450BA(void* memory, std::uint32_t byteCount);

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

// 原54A738：已接真实本族复制/原抛出依赖，SDK提供者未闭，不补固定异常。
void RaiseLegacyLogicError_54A738(LegacyExceptionObject* storage);

// 只复用已证48B存储视图；原方法表/ThrowInfo身份各自独立，不是C++抛出类型合并。
using LegacyStringLengthErrorObject = LegacyLogicErrorObject;
void DestroyLegacyStringLengthError_54A80B(LegacyStringLengthErrorObject* storage);
LegacyStringLengthErrorObject* DeleteLegacyStringLengthError_54A865(
    LegacyStringLengthErrorObject* storage, std::uint32_t flags);
LegacyStringLengthErrorObject* CopyConstructLegacyStringLengthError_54A881(
    LegacyStringLengthErrorObject* storage, const LegacyStringLengthErrorObject* source);
// 原54A848实际本族拷贝/ThrowInfo接线已实现；SDK提供者未闭，不补固定异常。
void RaiseLegacyStringLengthError_54A848(LegacyExceptionObject* storage);

using LegacyStringRangeErrorObject = LegacyLogicErrorObject;
void DestroyLegacyStringRangeError_54A8F3(LegacyStringRangeErrorObject* storage);
LegacyStringRangeErrorObject* DeleteLegacyStringRangeError_54A94D(
    LegacyStringRangeErrorObject* storage, std::uint32_t flags);
LegacyStringRangeErrorObject* CopyConstructLegacyStringRangeError_54A969(
    LegacyStringRangeErrorObject* storage, const LegacyStringRangeErrorObject* source);
// 原54A930实际本族拷贝/ThrowInfo接线已实现；SDK提供者未闭，不补固定异常。
void RaiseLegacyStringRangeError_54A930(LegacyExceptionObject* storage);

// 只复用已证48B字段视图；runtime_error/failure的原表、父调用和SDK类型迁移独立。
using LegacyRuntimeErrorObject = LegacyLogicErrorObject;
using LegacyStreamFailureObject = LegacyRuntimeErrorObject;
LegacyRuntimeErrorObject* ConstructLegacyRuntimeError_549FFE(
    LegacyRuntimeErrorObject* storage, const LegacyNarrowString* source);
LegacyRuntimeErrorObject* CopyConstructLegacyRuntimeError_54A0E5(
    LegacyRuntimeErrorObject* storage, const LegacyRuntimeErrorObject* source);
void DestroyLegacyRuntimeError_54A08C(LegacyRuntimeErrorObject* storage);
LegacyRuntimeErrorObject* DeleteLegacyRuntimeError_54A0C9(
    LegacyRuntimeErrorObject* storage, std::uint32_t flags);
void DestroyLegacyStreamFailure_54A142(LegacyStreamFailureObject* storage);
LegacyStreamFailureObject* DeleteLegacyStreamFailure_54A19C(
    LegacyStreamFailureObject* storage, std::uint32_t flags);
LegacyStreamFailureObject* CopyConstructLegacyStreamFailure_54A1B8(
    LegacyStreamFailureObject* storage, const LegacyStreamFailureObject* source);
// 原两复制后抛出入口已接真实本族复制，SDK提供者未闭，不代换宿主异常/x86元数据。
void RaiseLegacyRuntimeError_54A06F(LegacyExceptionObject* storage);
void RaiseLegacyStreamFailure_54A17F(LegacyExceptionObject* storage);

// 原5500EC实际表供clear显式换表；仅导出既有真实定义，槽与旧算法保持。
extern const LegacyExceptionRaiseMethodTable gLegacyStreamFailureMethods5500EC;

// 原5515E8元数据身份仅地址传出；x64布局/生产和SDK类型匹配未闭，不复制原x86内容。
struct LegacyCppThrowInfo;
extern const LegacyCppThrowInfo gLegacyStreamFailureThrowInfo5515E8;
// 原54B123真实两地址依赖仅声明；null/null是原重抛，不补实现、固定异常或正常返回值。
void ThrowLegacyCppException_54B123(void* exceptionObject, const LegacyCppThrowInfo* throwInfo);

// 原其余四类ThrowInfo仅实际身份地址声明；五族布局/生产和SDK类型仍独立未闭。
extern const LegacyCppThrowInfo gLegacyLogicErrorThrowInfo551880;
extern const LegacyCppThrowInfo gLegacyStringLengthErrorThrowInfo551810;
extern const LegacyCppThrowInfo gLegacyStringRangeErrorThrowInfo551938;
extern const LegacyCppThrowInfo gLegacyRuntimeErrorThrowInfo551658;

} // namespace cg::platform
