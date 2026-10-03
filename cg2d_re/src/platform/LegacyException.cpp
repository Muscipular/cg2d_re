#include "LegacyException.hpp"

#include "LegacyHeap.hpp"
#include "cg/platform/LegacyTime.hpp"

namespace cg::platform {
namespace {

// 原5501C8/5501CC只含删除析构与what；只接已证两槽，不生成宿主异常元数据。
const LegacyExceptionMethodTable gLegacyExceptionMethods5501C8{
    DeleteLegacyException_54B05D,
    GetLegacyExceptionText_54B116
};
// 原5501D0，已加载18B（含NUL）；仅text为空的what路径使用。
const char gLegacyUnknownException5501D0[18] = "Unknown exception";

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

} // namespace cg::platform
