#include "LegacyException.hpp"

#include "LegacyHeap.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/Input.hpp"

namespace cg::platform {
namespace {

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

} // namespace cg::platform
