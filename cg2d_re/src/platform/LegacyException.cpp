#include "LegacyException.hpp"

#include "LegacyHeap.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/Input.hpp"

#include <Windows.h>

#include <bit>
#include <type_traits>
#include <cstring>

namespace cg::platform {

// 原58FB74真实已加载初值；独立原槽，不替换SDK的terminate/unexpected处理器。
void (*gLegacyInconsistencyHandler58FB74)() = TerminateLegacyRuntime_540A5C;

namespace {

// 六条已证无虚基复制记录的签名适配；真实成员本体返回不被原捕获构造消费。
void CopyLegacyExceptionForCatch(void* const destination, const void* const source)
{
    static_cast<void>(CopyConstructLegacyException_54B0B6(
        static_cast<LegacyExceptionObject*>(destination),
        static_cast<const LegacyExceptionObject*>(source)));
}

void CopyLegacyRuntimeErrorForCatch(void* const destination, const void* const source)
{
    static_cast<void>(CopyConstructLegacyRuntimeError_54A0E5(
        static_cast<LegacyRuntimeErrorObject*>(destination),
        static_cast<const LegacyRuntimeErrorObject*>(source)));
}

void CopyLegacyStreamFailureForCatch(void* const destination, const void* const source)
{
    static_cast<void>(CopyConstructLegacyStreamFailure_54A1B8(
        static_cast<LegacyStreamFailureObject*>(destination),
        static_cast<const LegacyStreamFailureObject*>(source)));
}

void CopyLegacyLogicErrorForCatch(void* const destination, const void* const source)
{
    static_cast<void>(CopyConstructLegacyLogicError_54A7AE(
        static_cast<LegacyLogicErrorObject*>(destination),
        static_cast<const LegacyLogicErrorObject*>(source)));
}

void CopyLegacyStringLengthErrorForCatch(void* const destination, const void* const source)
{
    static_cast<void>(CopyConstructLegacyStringLengthError_54A881(
        static_cast<LegacyStringLengthErrorObject*>(destination),
        static_cast<const LegacyStringLengthErrorObject*>(source)));
}

void CopyLegacyStringRangeErrorForCatch(void* const destination, const void* const source)
{
    static_cast<void>(CopyConstructLegacyStringRangeError_54A969(
        static_cast<LegacyStringRangeErrorObject*>(destination),
        static_cast<const LegacyStringRangeErrorObject*>(source)));
}

// 原5501C0单槽；显式this按已证前缀使用，不借宿主type_info或RTTI表。
const LegacyExceptionTypeDescriptorMethods gLegacyTypeDescriptorMethods5501C0{
    DeleteLegacyExceptionTypeDescriptor_54AC7B
};

// 原5501C8/5501CC只含删除析构与what；只接已证两槽，不生成宿主异常元数据。
const LegacyExceptionMethodTable gLegacyExceptionMethods5501C8{
    DeleteLegacyException_54B05D,
    GetLegacyExceptionText_54B116
};
// 原5501D0，已加载18B（含NUL）；仅text为空的what路径使用。
const char gLegacyUnknownException5501D0[18] = "Unknown exception";

// 仅显式this的地址适配；对象首成员是实际基类，不分配或自动构析。
LegacyExceptionObject* DeleteLegacyLogicErrorFromBase(
    LegacyExceptionObject* const storage, const std::uint32_t flags)
{
    DeleteLegacyLogicError_54A792(reinterpret_cast<LegacyLogicErrorObject*>(storage), flags);
    return storage;
}
const char* GetLegacyLogicErrorTextFromBase(const LegacyExceptionObject* const storage)
{
    return GetLegacyLogicErrorText_54A062(
        reinterpret_cast<const LegacyLogicErrorObject*>(storage));
}

// 原550184三槽；Raise已接真实复制/抛出依赖，完整SDK类型迁移仍未闭。
const LegacyExceptionRaiseMethodTable gLegacyLogicErrorMethods550184{
    {DeleteLegacyLogicErrorFromBase, GetLegacyLogicErrorTextFromBase},
    RaiseLegacyLogicError_54A738
};

LegacyExceptionObject* DeleteLegacyStringLengthErrorFromBase(
    LegacyExceptionObject* const storage, const std::uint32_t flags)
{
    DeleteLegacyStringLengthError_54A865(reinterpret_cast<LegacyStringLengthErrorObject*>(storage), flags);
    return storage;
}
const LegacyExceptionRaiseMethodTable gLegacyStringLengthErrorMethods550164{
    {DeleteLegacyStringLengthErrorFromBase, GetLegacyLogicErrorTextFromBase},
    RaiseLegacyStringLengthError_54A848
};

LegacyExceptionObject* DeleteLegacyStringRangeErrorFromBase(
    LegacyExceptionObject* const storage, const std::uint32_t flags)
{
    DeleteLegacyStringRangeError_54A94D(reinterpret_cast<LegacyStringRangeErrorObject*>(storage), flags);
    return storage;
}
const LegacyExceptionRaiseMethodTable gLegacyStringRangeErrorMethods550194{
    {DeleteLegacyStringRangeErrorFromBase, GetLegacyLogicErrorTextFromBase},
    RaiseLegacyStringRangeError_54A930
};

LegacyExceptionObject* DeleteLegacyRuntimeErrorFromBase(
    LegacyExceptionObject* const storage, const std::uint32_t flags)
{
    DeleteLegacyRuntimeError_54A0C9(reinterpret_cast<LegacyRuntimeErrorObject*>(storage), flags);
    return storage;
}
const LegacyExceptionRaiseMethodTable gLegacyRuntimeErrorMethods550130{
    {DeleteLegacyRuntimeErrorFromBase, GetLegacyLogicErrorTextFromBase},
    RaiseLegacyRuntimeError_54A06F
};

LegacyExceptionObject* DeleteLegacyStreamFailureFromBase(
    LegacyExceptionObject* const storage, const std::uint32_t flags)
{
    DeleteLegacyStreamFailure_54A19C(reinterpret_cast<LegacyStreamFailureObject*>(storage), flags);
    return storage;
}
} // namespace

const LegacyExceptionRaiseMethodTable gLegacyStreamFailureMethods5500EC{
    {DeleteLegacyStreamFailureFromBase, GetLegacyLogicErrorTextFromBase},
    RaiseLegacyStreamFailure_54A17F
};

// 原六TypeDescriptor已加载内容：同族单槽、缓存0及精确装饰名；不注册宿主RTTI/自动析构。
LegacyExceptionTypeDescriptorStorage<16> gLegacyExceptionType592B28{
    {&gLegacyTypeDescriptorMethods5501C0, nullptr},
    ".?AVexception@@"
};
LegacyExceptionTypeDescriptorStorage<24> gLegacyRuntimeErrorType592B40{
    {&gLegacyTypeDescriptorMethods5501C0, nullptr},
    ".?AVruntime_error@std@@"
};
LegacyExceptionTypeDescriptorStorage<27> gLegacyStreamFailureType592B60{
    {&gLegacyTypeDescriptorMethods5501C0, nullptr},
    ".?AVfailure@ios_base@std@@"
};
LegacyExceptionTypeDescriptorStorage<22> gLegacyLogicErrorType592BD0{
    {&gLegacyTypeDescriptorMethods5501C0, nullptr},
    ".?AVlogic_error@std@@"
};
LegacyExceptionTypeDescriptorStorage<23> gLegacyStringLengthErrorType592BF0{
    {&gLegacyTypeDescriptorMethods5501C0, nullptr},
    ".?AVlength_error@std@@"
};
LegacyExceptionTypeDescriptorStorage<23> gLegacyStringRangeErrorType592C10{
    {&gLegacyTypeDescriptorMethods5501C0, nullptr},
    ".?AVout_of_range@std@@"
};
static_assert(offsetof(decltype(gLegacyExceptionType592B28), prefix) == 0);
static_assert(offsetof(decltype(gLegacyExceptionType592B28), name) == sizeof(LegacyExceptionTypeDescriptorPrefix));
static_assert(std::is_trivial_v<decltype(gLegacyExceptionType592B28)>);
static_assert(offsetof(decltype(gLegacyRuntimeErrorType592B40), prefix) == 0);
static_assert(offsetof(decltype(gLegacyRuntimeErrorType592B40), name) == sizeof(LegacyExceptionTypeDescriptorPrefix));
static_assert(std::is_trivial_v<decltype(gLegacyRuntimeErrorType592B40)>);
static_assert(offsetof(decltype(gLegacyStreamFailureType592B60), prefix) == 0);
static_assert(offsetof(decltype(gLegacyStreamFailureType592B60), name) == sizeof(LegacyExceptionTypeDescriptorPrefix));
static_assert(std::is_trivial_v<decltype(gLegacyStreamFailureType592B60)>);
static_assert(offsetof(decltype(gLegacyLogicErrorType592BD0), prefix) == 0);
static_assert(offsetof(decltype(gLegacyLogicErrorType592BD0), name) == sizeof(LegacyExceptionTypeDescriptorPrefix));
static_assert(std::is_trivial_v<decltype(gLegacyLogicErrorType592BD0)>);
static_assert(offsetof(decltype(gLegacyStringLengthErrorType592BF0), prefix) == 0);
static_assert(offsetof(decltype(gLegacyStringLengthErrorType592BF0), name) == sizeof(LegacyExceptionTypeDescriptorPrefix));
static_assert(std::is_trivial_v<decltype(gLegacyStringLengthErrorType592BF0)>);
static_assert(offsetof(decltype(gLegacyStringRangeErrorType592C10), prefix) == 0);
static_assert(offsetof(decltype(gLegacyStringRangeErrorType592C10), name) == sizeof(LegacyExceptionTypeDescriptorPrefix));
static_assert(std::is_trivial_v<decltype(gLegacyStringRangeErrorType592C10)>);

// 原六CatchableType实际属性/PMD/描述符/复制目标；字节数字保持U32并按当前对象指针拓宽布局计算。
const LegacyExceptionCatchableObjectType gLegacyExceptionCatchable551578{
    {0u, &gLegacyExceptionType592B28.prefix},
    {0, -1, 0},
    static_cast<std::uint32_t>(sizeof(LegacyExceptionObject)),
    CopyLegacyExceptionForCatch
};
const LegacyExceptionCatchableObjectType gLegacyRuntimeErrorCatchable551598{
    {0u, &gLegacyRuntimeErrorType592B40.prefix},
    {0, -1, 0},
    static_cast<std::uint32_t>(sizeof(LegacyRuntimeErrorObject)),
    CopyLegacyRuntimeErrorForCatch
};
const LegacyExceptionCatchableObjectType gLegacyStreamFailureCatchable5515B8{
    {0u, &gLegacyStreamFailureType592B60.prefix},
    {0, -1, 0},
    static_cast<std::uint32_t>(sizeof(LegacyStreamFailureObject)),
    CopyLegacyStreamFailureForCatch
};
const LegacyExceptionCatchableObjectType gLegacyLogicErrorCatchable5517C0{
    {0u, &gLegacyLogicErrorType592BD0.prefix},
    {0, -1, 0},
    static_cast<std::uint32_t>(sizeof(LegacyLogicErrorObject)),
    CopyLegacyLogicErrorForCatch
};
const LegacyExceptionCatchableObjectType gLegacyStringLengthCatchable5517E0{
    {0u, &gLegacyStringLengthErrorType592BF0.prefix},
    {0, -1, 0},
    static_cast<std::uint32_t>(sizeof(LegacyStringLengthErrorObject)),
    CopyLegacyStringLengthErrorForCatch
};
const LegacyExceptionCatchableObjectType gLegacyStringRangeCatchable551908{
    {0u, &gLegacyStringRangeErrorType592C10.prefix},
    {0, -1, 0},
    static_cast<std::uint32_t>(sizeof(LegacyStringRangeErrorObject)),
    CopyLegacyStringRangeErrorForCatch
};

// 原五捕获数组保持派生→基类顺序及同一基类记录共享；不产生SDK ThrowInfo/RVA。
const LegacyExceptionCatchableArrayStorage<3> gLegacyStreamFailureCatchableArray5515D8{
    3,
    {&gLegacyStreamFailureCatchable5515B8, &gLegacyRuntimeErrorCatchable551598, &gLegacyExceptionCatchable551578}
};
const LegacyExceptionCatchableArrayStorage<2> gLegacyRuntimeErrorCatchableArray551648{
    2,
    {&gLegacyRuntimeErrorCatchable551598, &gLegacyExceptionCatchable551578}
};
const LegacyExceptionCatchableArrayStorage<3> gLegacyStringLengthCatchableArray551800{
    3,
    {&gLegacyStringLengthCatchable5517E0, &gLegacyLogicErrorCatchable5517C0, &gLegacyExceptionCatchable551578}
};
const LegacyExceptionCatchableArrayStorage<2> gLegacyLogicErrorCatchableArray551870{
    2,
    {&gLegacyLogicErrorCatchable5517C0, &gLegacyExceptionCatchable551578}
};
const LegacyExceptionCatchableArrayStorage<3> gLegacyStringRangeCatchableArray551928{
    3,
    {&gLegacyStringRangeCatchable551908, &gLegacyLogicErrorCatchable5517C0, &gLegacyExceptionCatchable551578}
};

LegacyExceptionObject* ConstructLegacyExceptionText_54B079(
    LegacyExceptionObject* const storage, const char* const* const sourceText)
{
    storage->methods = &gLegacyExceptionMethods5501C8;
    const std::uint32_t textBytes = StrLen_53F9E0(*sourceText) + 1u;
    // 原53C9CF只传(U32,1)给53C3BE；沿原堆域，不混入宿主裸字节适配。
    char* const allocatedText = static_cast<char*>(
        AllocateLegacyHeapWithHandler_53C3BE(textBytes, 1));
    storage->text = allocatedText;
    if (allocatedText != nullptr) {
        StrCpy_544DB0(allocatedText, *sourceText);
    }
    storage->ownsText = 1;
    return storage;
}

LegacyExceptionObject* CopyConstructLegacyException_54B0B6(
    LegacyExceptionObject* const storage, const LegacyExceptionObject* const source)
{
    storage->methods = &gLegacyExceptionMethods5501C8;
    const std::uint32_t ownsText = source->ownsText;
    storage->ownsText = ownsText;
    if (ownsText != 0) {
        const std::uint32_t textBytes = StrLen_53F9E0(source->text) + 1u;
        char* const allocatedText = static_cast<char*>(
            AllocateLegacyHeapWithHandler_53C3BE(textBytes, 1));
        storage->text = allocatedText;
        if (allocatedText != nullptr) {
            StrCpy_544DB0(allocatedText, source->text);
        }
    } else {
        storage->text = source->text;
    }
    return storage;
}

void DestroyLegacyException_54B100(LegacyExceptionObject* const storage)
{
    const bool ownsText = storage->ownsText != 0;
    storage->methods = &gLegacyExceptionMethods5501C8;
    if (ownsText) {
        // 原53B4E7只转原53C0A9；不清文本槽/标志，不补重复析构保护。
        FreeLegacyHeapMemory_53C0A9(storage->text);
    }
}

LegacyExceptionObject* DeleteLegacyException_54B05D(
    LegacyExceptionObject* const storage, const std::uint32_t flags)
{
    DestroyLegacyException_54B100(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

const char* GetLegacyExceptionText_54B116(const LegacyExceptionObject* const storage)
{
    const char* const text = storage->text;
    return text != nullptr ? text : gLegacyUnknownException5501D0;
}

// 原54A6D4；借实际593850共享文本，不以固定空串代替可写存储。
LegacyLogicErrorObject* ConstructLegacyLogicError_54A6D4(
    LegacyLogicErrorObject* const storage, const LegacyNarrowString* const source)
{
    const char* const baseText = input::LegacyText593850();
    ConstructLegacyExceptionText_54B079(&storage->base, &baseText);
    try {
        storage->message.prefix00[0] = source->prefix00[0];
        TidyLegacyNarrowString_548E29(&storage->message, false);
        AssignLegacyNarrowStringFromString_54A315(
            &storage->message, source, 0, gLegacyNarrowStringNpos550040);
    } catch (...) {
        // 原state0的54C414→54B100，仅清基类，不补成员回滚。
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    storage->base.methods = &gLegacyLogicErrorMethods550184.base;
    return storage;
}

LegacyLogicErrorObject* CopyConstructLegacyLogicError_54A7AE(
    LegacyLogicErrorObject* const storage, const LegacyLogicErrorObject* const source)
{
    CopyConstructLegacyException_54B0B6(&storage->base, &source->base);
    try {
        storage->message.prefix00[0] = source->message.prefix00[0];
        TidyLegacyNarrowString_548E29(&storage->message, false);
        AssignLegacyNarrowStringFromString_54A315(
            &storage->message, &source->message, 0, gLegacyNarrowStringNpos550040);
    } catch (...) {
        // 原state0的54C43C→54B100，基类复制完成后才启用。
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    storage->base.methods = &gLegacyLogicErrorMethods550184.base;
    return storage;
}

void DestroyLegacyLogicError_54A755(LegacyLogicErrorObject* const storage)
{
    storage->base.methods = &gLegacyLogicErrorMethods550184.base;
    try {
        TidyLegacyNarrowString_548E29(&storage->message, true);
    } catch (...) {
        // 原54C428→54B100；成员析构异常仍清基类，正常基类析构在保护区外。
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    DestroyLegacyException_54B100(&storage->base);
}

LegacyLogicErrorObject* DeleteLegacyLogicError_54A792(
    LegacyLogicErrorObject* const storage, const std::uint32_t flags)
{
    DestroyLegacyLogicError_54A755(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

// 原54A062在五个原虚表共用；当前只接实际logic_error，其他owner仍待。
const char* GetLegacyLogicErrorText_54A062(const LegacyLogicErrorObject* const storage)
{
    const char* const text = storage->message.buffer;
    return text != nullptr ? text : gLegacyNarrowStringEmpty55013B + 1;
}

void DestroyLegacyStringLengthError_54A80B(LegacyStringLengthErrorObject* const storage)
{
    // 原析构都先写logic_error表，不能提前改成各自最派生表。
    storage->base.methods = &gLegacyLogicErrorMethods550184.base;
    try {
        TidyLegacyNarrowString_548E29(&storage->message, true);
    } catch (...) {
        // 原54C450唯一state0动作只清基类。
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    DestroyLegacyException_54B100(&storage->base);
}

LegacyStringLengthErrorObject* DeleteLegacyStringLengthError_54A865(
    LegacyStringLengthErrorObject* const storage, const std::uint32_t flags)
{
    DestroyLegacyStringLengthError_54A80B(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

LegacyStringLengthErrorObject* CopyConstructLegacyStringLengthError_54A881(
    LegacyStringLengthErrorObject* const storage, const LegacyStringLengthErrorObject* const source)
{
    CopyConstructLegacyLogicError_54A7AE(storage, source);
    storage->base.methods = &gLegacyStringLengthErrorMethods550164.base;
    return storage;
}

void DestroyLegacyStringRangeError_54A8F3(LegacyStringRangeErrorObject* const storage)
{
    // 原析构都先写logic_error表，不能提前改成各自最派生表。
    storage->base.methods = &gLegacyLogicErrorMethods550184.base;
    try {
        TidyLegacyNarrowString_548E29(&storage->message, true);
    } catch (...) {
        // 原54C478唯一state0动作只清基类。
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    DestroyLegacyException_54B100(&storage->base);
}

LegacyStringRangeErrorObject* DeleteLegacyStringRangeError_54A94D(
    LegacyStringRangeErrorObject* const storage, const std::uint32_t flags)
{
    DestroyLegacyStringRangeError_54A8F3(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

LegacyStringRangeErrorObject* CopyConstructLegacyStringRangeError_54A969(
    LegacyStringRangeErrorObject* const storage, const LegacyStringRangeErrorObject* const source)
{
    CopyConstructLegacyLogicError_54A7AE(storage, source);
    storage->base.methods = &gLegacyStringRangeErrorMethods550194.base;
    return storage;
}

// 原549FFE，与logic_error存储布局相同，实际原入口/方法表保持独立。
LegacyRuntimeErrorObject* ConstructLegacyRuntimeError_549FFE(
    LegacyRuntimeErrorObject* const storage, const LegacyNarrowString* const source)
{
    const char* const baseText = input::LegacyText593850();
    ConstructLegacyExceptionText_54B079(&storage->base, &baseText);
    try {
        storage->message.prefix00[0] = source->prefix00[0];
        TidyLegacyNarrowString_548E29(&storage->message, false);
        AssignLegacyNarrowStringFromString_54A315(
            &storage->message, source, 0, gLegacyNarrowStringNpos550040);
    } catch (...) {
        // 原54C32C唯一state0动作只清已完成的基类。
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    storage->base.methods = &gLegacyRuntimeErrorMethods550130.base;
    return storage;
}

LegacyRuntimeErrorObject* CopyConstructLegacyRuntimeError_54A0E5(
    LegacyRuntimeErrorObject* const storage, const LegacyRuntimeErrorObject* const source)
{
    CopyConstructLegacyException_54B0B6(&storage->base, &source->base);
    try {
        storage->message.prefix00[0] = source->message.prefix00[0];
        TidyLegacyNarrowString_548E29(&storage->message, false);
        AssignLegacyNarrowStringFromString_54A315(
            &storage->message, &source->message, 0, gLegacyNarrowStringNpos550040);
    } catch (...) {
        // 原54C354只清基类，不额外回滚成员。
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    storage->base.methods = &gLegacyRuntimeErrorMethods550130.base;
    return storage;
}

void DestroyLegacyRuntimeError_54A08C(LegacyRuntimeErrorObject* const storage)
{
    storage->base.methods = &gLegacyRuntimeErrorMethods550130.base;
    try {
        TidyLegacyNarrowString_548E29(&storage->message, true);
    } catch (...) {
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    DestroyLegacyException_54B100(&storage->base);
}

LegacyRuntimeErrorObject* DeleteLegacyRuntimeError_54A0C9(
    LegacyRuntimeErrorObject* const storage, const std::uint32_t flags)
{
    DestroyLegacyRuntimeError_54A08C(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

void DestroyLegacyStreamFailure_54A142(LegacyStreamFailureObject* const storage)
{
    // 原failure析构先恢复runtime_error表，不装最派生failure表。
    storage->base.methods = &gLegacyRuntimeErrorMethods550130.base;
    try {
        TidyLegacyNarrowString_548E29(&storage->message, true);
    } catch (...) {
        DestroyLegacyException_54B100(&storage->base);
        throw;
    }
    DestroyLegacyException_54B100(&storage->base);
}

LegacyStreamFailureObject* DeleteLegacyStreamFailure_54A19C(
    LegacyStreamFailureObject* const storage, const std::uint32_t flags)
{
    DestroyLegacyStreamFailure_54A142(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

LegacyStreamFailureObject* CopyConstructLegacyStreamFailure_54A1B8(
    LegacyStreamFailureObject* const storage, const LegacyStreamFailureObject* const source)
{
    CopyConstructLegacyRuntimeError_54A0E5(storage, source);
    storage->base.methods = &gLegacyStreamFailureMethods5500EC.base;
    return storage;
}

void RaiseLegacyLogicError_54A738(LegacyExceptionObject* const storage)
{
    LegacyLogicErrorObject copied;
    CopyConstructLegacyLogicError_54A7AE(&copied, reinterpret_cast<const LegacyLogicErrorObject*>(storage));
    ThrowLegacyCppException_54B123(&copied, &gLegacyLogicErrorThrowInfo551880);
}

void RaiseLegacyStringLengthError_54A848(LegacyExceptionObject* const storage)
{
    LegacyStringLengthErrorObject copied;
    CopyConstructLegacyStringLengthError_54A881(&copied, reinterpret_cast<const LegacyStringLengthErrorObject*>(storage));
    ThrowLegacyCppException_54B123(&copied, &gLegacyStringLengthErrorThrowInfo551810);
}

void RaiseLegacyStringRangeError_54A930(LegacyExceptionObject* const storage)
{
    LegacyStringRangeErrorObject copied;
    CopyConstructLegacyStringRangeError_54A969(&copied, reinterpret_cast<const LegacyStringRangeErrorObject*>(storage));
    ThrowLegacyCppException_54B123(&copied, &gLegacyStringRangeErrorThrowInfo551938);
}

void RaiseLegacyRuntimeError_54A06F(LegacyExceptionObject* const storage)
{
    LegacyRuntimeErrorObject copied;
    CopyConstructLegacyRuntimeError_54A0E5(&copied, reinterpret_cast<const LegacyRuntimeErrorObject*>(storage));
    ThrowLegacyCppException_54B123(&copied, &gLegacyRuntimeErrorThrowInfo551658);
}

void RaiseLegacyStreamFailure_54A17F(LegacyExceptionObject* const storage)
{
    LegacyStreamFailureObject copied;
    CopyConstructLegacyStreamFailure_54A1B8(&copied, reinterpret_cast<const LegacyStreamFailureObject*>(storage));
    ThrowLegacyCppException_54B123(&copied, &gLegacyStreamFailureThrowInfo5515E8);
}

namespace {

// 原两字符串异常消息已加载；保留ASCII/NUL字节，不改宿主异常文本。
const char gLegacyStringTooLongMessage550170[16] = "string too long";
const char gLegacyInvalidStringPositionMessage5501A0[24] = "invalid string position";

} // namespace

void ThrowLegacyStringLength_54A67A()
{
    LegacyNarrowString message;
    // 原EH prolog临时保存返回地址，var_D读其最高U8；两已证原地址该字节均为0。
    message.prefix00[0] = 0u;
    TidyLegacyNarrowString_548E29(&message, false);
    const std::uint32_t messageBytes = StrLen_53F9E0(gLegacyStringTooLongMessage550170);
    AssignLegacyNarrowString_549055(&message, gLegacyStringTooLongMessage550170, messageBytes);

    LegacyStringLengthErrorObject exception;
    try {
        // 原state0从54A6AE开始，assign失败尚未进入临时串保护区。
        ConstructLegacyLogicError_54A6D4(&exception, &message);
        exception.base.methods = &gLegacyStringLengthErrorMethods550164.base;
        ThrowLegacyCppException_54B123(&exception, &gLegacyStringLengthErrorThrowInfo551810);
    } catch (...) {
        // 原54C400通过共享54A30D，只清临时串，不额外析构exception。
        TidyLegacyNarrowString_548E29(&message, true);
        throw;
    }
}

void ThrowLegacyStringRange_54A899()
{
    LegacyNarrowString message;
    // 原EH prolog临时保存返回地址，var_D读其最高U8；两已证原地址该字节均为0。
    message.prefix00[0] = 0u;
    TidyLegacyNarrowString_548E29(&message, false);
    const std::uint32_t messageBytes = StrLen_53F9E0(gLegacyInvalidStringPositionMessage5501A0);
    AssignLegacyNarrowString_549055(&message, gLegacyInvalidStringPositionMessage5501A0, messageBytes);

    LegacyStringRangeErrorObject exception;
    try {
        // 原state0从54A8CD开始，assign失败尚未进入临时串保护区。
        ConstructLegacyLogicError_54A6D4(&exception, &message);
        exception.base.methods = &gLegacyStringRangeErrorMethods550194.base;
        ThrowLegacyCppException_54B123(&exception, &gLegacyStringRangeErrorThrowInfo551938);
    } catch (...) {
        // 原54C464通过共享54A30D，只清临时串，不额外析构exception。
        TidyLegacyNarrowString_548E29(&message, true);
        throw;
    }
}

void* AdjustLegacyExceptionObject_5409E7(
    void* const storage, const LegacyExceptionPmd* const displacement)
{
    auto* const object = static_cast<std::byte*>(storage);
    const std::int32_t memberDisplacement = displacement->memberDisplacement;
    const std::int32_t virtualBasePointerDisplacement = displacement->virtualBasePointerDisplacement;
    auto* adjusted = object + memberDisplacement;
    if (virtualBasePointerDisplacement >= 0) {
        // 原表指针从原object读取，不能从已加memberDisplacement的地址读取。
        const std::byte* virtualBaseTable;
        std::memcpy(&virtualBaseTable, object + virtualBasePointerDisplacement, sizeof(virtualBaseTable));
        const std::int32_t virtualBaseDisplacement = displacement->virtualBaseDisplacement;
        std::int32_t tableDisplacement;
        std::memcpy(&tableDisplacement, virtualBaseTable + virtualBaseDisplacement, sizeof(tableDisplacement));
        // 原两S32位移相加先保留W32位模式，再作为有符号位移作用于已拓宽地址。
        const std::uint32_t extraBits = std::bit_cast<std::uint32_t>(tableDisplacement)
            + std::bit_cast<std::uint32_t>(virtualBasePointerDisplacement);
        adjusted += std::bit_cast<std::int32_t>(extraBits);
    }
    return adjusted;
}

std::int32_t MatchLegacyExceptionType_5404FA(
    const LegacyExceptionHandlerTypePrefix* const handler,
    const LegacyExceptionCatchableTypePrefix* const catchable,
    const LegacyExceptionThrowInfoPrefix* const throwInfo)
{
    const LegacyExceptionTypeDescriptorPrefix* const handlerType = handler->type;
    if (handlerType == nullptr) {
        return 1;
    }
    // 原名称内联在两个地址之后；不从cachedName取名称，也不读取后续未知字段。
    const auto* const handlerName = reinterpret_cast<const char*>(handlerType + 1);
    if (*handlerName == '\0') {
        return 1;
    }

    const LegacyExceptionTypeDescriptorPrefix* const catchableType = catchable->type;
    if (handlerType != catchableType) {
        const auto* const catchableName = reinterpret_cast<const char*>(catchableType + 1);
        if (StrCmp_5450F0(handlerName, catchableName) != 0) {
            return 0;
        }
    }

    // 原这里均为U8读取；各分支按原顺序读取handler属性，不提前缓存。
    if ((*reinterpret_cast<const unsigned char*>(&catchable->properties) & 2u) != 0
        && (*reinterpret_cast<const unsigned char*>(&handler->properties) & 8u) == 0) {
        return 0;
    }
    const std::uint32_t attributes = throwInfo->attributes;
    if ((attributes & 1u) != 0
        && (*reinterpret_cast<const unsigned char*>(&handler->properties) & 1u) == 0) {
        return 0;
    }
    if ((attributes & 2u) != 0
        && (*reinterpret_cast<const unsigned char*>(&handler->properties) & 2u) == 0) {
        return 0;
    }
    return 1;
}

void DestroyLegacyExceptionTypeDescriptor_54AC66(
    LegacyExceptionTypeDescriptorPrefix* const storage)
{
    storage->methods = &gLegacyTypeDescriptorMethods5501C0;
    char* const cachedName = storage->cachedName;
    if (cachedName != nullptr) {
        FreeLegacyHeapMemory_53C0A9(cachedName);
    }
}

LegacyExceptionTypeDescriptorPrefix* DeleteLegacyExceptionTypeDescriptor_54AC7B(
    LegacyExceptionTypeDescriptorPrefix* const storage, const std::uint32_t flags)
{
    DestroyLegacyExceptionTypeDescriptor_54AC66(storage);
    if ((flags & 1u) != 0) {
        // 原53B4E7薄叶同域调用53C0A9；不能释放宿主type_info或其他堆块。
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

[[noreturn]] void ReportLegacyExceptionInconsistency_540AB2()
{
    __try {
        // 原外层finally已经生效后，才采样并调用此共享槽。
        void (*const handler)() = gLegacyInconsistencyHandler58FB74;
        if (handler != nullptr) {
            __try {
                handler();
            } __except (EXCEPTION_EXECUTE_HANDLER) {
                // 原540AEF恒返1，540AF3仅恢复本帧，无额外业务清理。
            }
        }
    } __finally {
        // 原正常尾跳540B03及外层scope0均到真实终止，回调返回/抛出都不能跳过。
        TerminateLegacyRuntime_540A5C();
    }
}

const LegacyExceptionTryBlockMapEntry* GetLegacyTryRange_53B75B(
    const LegacyExceptionFuncInfoPrefix* const information, std::int32_t nesting,
    const std::int32_t state, std::uint32_t* const firstIndex,
    std::uint32_t* const lastIndex)
{
    std::uint32_t index = information->tryBlockCount;
    const LegacyExceptionTryBlockMapEntry* const table = information->tryBlocks;
    std::uint32_t previous = index;
    std::uint32_t rangeEnd = index;
    if (nesting >= 0) {
        do {
            if (index == 0xFFFFFFFFu) {
                ReportLegacyExceptionInconsistency_540AB2();
            }
            --index;
            // 原-1仍先读表前一记录，再判-1；不补哨兵记录或跳过真实读取。
            const LegacyExceptionTryBlockMapEntry* const entry =
                index == 0xFFFFFFFFu ? table - 1 : table + index;
            if ((entry->tryHigh < state && state <= entry->catchHigh)
                || index == 0xFFFFFFFFu) {
                rangeEnd = previous;
                --nesting;
                previous = index;
            }
        } while (nesting >= 0);
    }
    ++index;
    *firstIndex = index;
    *lastIndex = rangeEnd;
    // 原输出可与输入/彼此别名；此时才复读数量，比较仍使用缓存的两个U32。
    if (rangeEnd > information->tryBlockCount || index > rangeEnd) {
        ReportLegacyExceptionInconsistency_540AB2();
    }
    return table + index;
}

std::int32_t ValidateLegacyExceptionRead_54509E(
    const void* const memory, const std::uint32_t byteCount)
{
    return ::IsBadReadPtr(memory, static_cast<UINT_PTR>(byteCount)) == FALSE ? 1 : 0;
}

std::int32_t ValidateLegacyExceptionWrite_5450BA(
    void* const memory, const std::uint32_t byteCount)
{
    return ::IsBadWritePtr(memory, static_cast<UINT_PTR>(byteCount)) == FALSE ? 1 : 0;
}

} // namespace cg::platform
