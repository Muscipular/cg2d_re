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

// 原550184三槽；Raise只真实声明，完整抛出/类型迁移仍未闭，不能改空槽。
const LegacyExceptionRaiseMethodTable gLegacyLogicErrorMethods550184{
    {DeleteLegacyLogicErrorFromBase, GetLegacyLogicErrorTextFromBase},
    RaiseLegacyLogicError_54A738
};

} // namespace

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

} // namespace cg::platform
