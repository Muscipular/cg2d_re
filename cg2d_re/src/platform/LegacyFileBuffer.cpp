#include "LegacyFileBuffer.hpp"

#include "LegacyHeap.hpp"

namespace cg::platform {

void InitializeLegacyNarrowFileBuffer_548EFC(
    LegacyNarrowFileBuffer* const storage, LegacyFile* const file, const std::int32_t mode)
{
    storage->modeOneFlag = mode == 1 ? 1 : 0;
    if (mode == 0) {
        LegacyLocaleImplementation** const locale = &storage->privateLocale;
        DestroyLegacyLocale_54867D(locale);
        if (locale != nullptr) {
            ConstructLegacyLocale_54865E(locale);
        }
        storage->scratchString = nullptr;
    }
    ResetLegacyStreamBuffer_549EDE(&storage->base.buffer);
    if (file != nullptr && storage->modeOneFlag == 0) {
        // 原六间接槽共用FILE实际三字段，两侧均须保留别名。
        storage->base.buffer.readCurrentSlot = &file->current;
        storage->base.buffer.readBaseSlot = &file->base;
        storage->base.buffer.writeBaseSlot = &file->base;
        storage->base.buffer.writeCurrentSlot = &file->current;
        storage->base.buffer.readRemainingSlot = &file->remaining;
        storage->base.buffer.writeRemainingSlot = &file->remaining;
    }
    storage->file = file;
    storage->state40Bits = gLegacyNarrowFileState1087B00;
    const std::uint32_t secondState = gLegacyNarrowFileState1087B00;
    storage->state38Bits = 0;
    storage->state3CBits = secondState;
}

void InitializeLegacyWideFileBuffer_549E88(
    LegacyWideFileBuffer* const storage, LegacyFile* const file, const std::int32_t mode)
{
    storage->modeOneFlag = mode == 1 ? 1 : 0;
    if (mode == 0) {
        LegacyLocaleImplementation** const locale = &storage->privateLocale;
        DestroyLegacyLocale_54867D(locale);
        if (locale != nullptr) {
            ConstructLegacyLocale_54865E(locale);
        }
        storage->scratchString = nullptr;
    }
    ResetLegacyStreamBuffer_549EDE(&storage->base.buffer);
    // 原宽入口不把游标/字符余量槽绑定FILE字节字段。
    storage->file = file;
    storage->state40Bits = gLegacyWideFileState1087AF8;
    const std::uint32_t secondState = gLegacyWideFileState1087AF8;
    storage->state38Bits = 0;
    storage->state3CBits = secondState;
}

LegacyNarrowFileBuffer* CloseLegacyNarrowFileBuffer_548ED5(
    LegacyNarrowFileBuffer* const storage)
{
    LegacyFile* const file = storage->file;
    if (file == nullptr || CloseLegacyStream_53B1D0(file) != 0) {
        return nullptr;
    }
    InitializeLegacyNarrowFileBuffer_548EFC(storage, nullptr, 2);
    return storage;
}

LegacyWideFileBuffer* CloseLegacyWideFileBuffer_549E61(
    LegacyWideFileBuffer* const storage)
{
    LegacyFile* const file = storage->file;
    if (file == nullptr || CloseLegacyStream_53B1D0(file) != 0) {
        return nullptr;
    }
    InitializeLegacyWideFileBuffer_549E88(storage, nullptr, 2);
    return storage;
}

LegacyNarrowFileBuffer* ConstructLegacyNarrowFileBuffer_5485FE(
    LegacyNarrowFileBuffer* const storage, LegacyFile* const file)
{
    ConstructLegacyLocale_54865E(&storage->base.localeImplementation);
    try {
        storage->base.buffer.virtualTable = &gLegacyNarrowStreamBufferMethods55000C;
        ResetLegacyStreamBuffer_549EDE(&storage->base.buffer);
    } catch (...) {
        // 原state0只清+34，尚未完成基类，不额外重置基类表。
        DestroyLegacyLocale_54867D(&storage->base.localeImplementation);
        throw;
    }
    try {
        ConstructLegacyLocale_54865E(&storage->privateLocale);
        try {
            storage->base.buffer.virtualTable = &gLegacyNarrowFileBufferMethods54FFD4;
            InitializeLegacyNarrowFileBuffer_548EFC(storage, file, 0);
        } catch (...) {
            // 原state2→1，私有locale先清；原mode0重构失败不补回滚。
            DestroyLegacyLocale_54867D(&storage->privateLocale);
            throw;
        }
    } catch (...) {
        // 原state1 prev=-1，只调完整基类析构，不再重复state0清理。
        DestroyLegacyNarrowStreamBuffer_5486B1(&storage->base);
        throw;
    }
    return storage;
}

LegacyWideFileBuffer* ConstructLegacyWideFileBuffer_54970A(
    LegacyWideFileBuffer* const storage, LegacyFile* const file)
{
    ConstructLegacyLocale_54865E(&storage->base.localeImplementation);
    try {
        storage->base.buffer.virtualTable = &gLegacyWideStreamBufferMethods5500A8;
        ResetLegacyStreamBuffer_549EDE(&storage->base.buffer);
    } catch (...) {
        DestroyLegacyLocale_54867D(&storage->base.localeImplementation);
        throw;
    }
    try {
        ConstructLegacyLocale_54865E(&storage->privateLocale);
        try {
            storage->base.buffer.virtualTable = &gLegacyWideFileBufferMethods550070;
            InitializeLegacyWideFileBuffer_549E88(storage, file, 0);
        } catch (...) {
            DestroyLegacyLocale_54867D(&storage->privateLocale);
            throw;
        }
    } catch (...) {
        DestroyLegacyWideStreamBuffer_54976A(&storage->base);
        throw;
    }
    return storage;
}

void DestroyLegacyNarrowFileBuffer_5487DE(LegacyNarrowFileBuffer* const storage)
{
    storage->base.buffer.virtualTable = &gLegacyNarrowFileBufferMethods54FFD4;
    try {
        try {
            if (storage->modeOneFlag != 0) {
                CloseLegacyNarrowFileBuffer_548ED5(storage);
            }
            LegacyNarrowString* const scratch = storage->scratchString;
            if (scratch != nullptr) {
                TidyLegacyNarrowString_548E29(scratch, true);
                FreeLegacyHeapMemory_53C0A9(scratch);
            }
            DestroyLegacyLocale_54867D(&storage->privateLocale);
            storage->base.buffer.virtualTable = &gLegacyNarrowStreamBufferMethods55000C;
            DestroyLegacyLocale_54867D(&storage->base.localeImplementation);
        } catch (...) {
            // 原正常私有/基类locale清理前未降state1，任一失败仍先清私有locale。
            DestroyLegacyLocale_54867D(&storage->privateLocale);
            throw;
        }
    } catch (...) {
        DestroyLegacyNarrowStreamBuffer_5486B1(&storage->base);
        throw;
    }
    // 原不清+44、不再关闭FILE或释放整个filebuf对象。
}

void DestroyLegacyWideFileBuffer_549935(LegacyWideFileBuffer* const storage)
{
    storage->base.buffer.virtualTable = &gLegacyWideFileBufferMethods550070;
    try {
        try {
            if (storage->modeOneFlag != 0) {
                CloseLegacyWideFileBuffer_549E61(storage);
            }
            // 原宽filebuf的+44也是真实窄字符串，不能改宽字符串域。
            LegacyNarrowString* const scratch = storage->scratchString;
            if (scratch != nullptr) {
                TidyLegacyNarrowString_548E29(scratch, true);
                FreeLegacyHeapMemory_53C0A9(scratch);
            }
            DestroyLegacyLocale_54867D(&storage->privateLocale);
            storage->base.buffer.virtualTable = &gLegacyWideStreamBufferMethods5500A8;
            DestroyLegacyLocale_54867D(&storage->base.localeImplementation);
        } catch (...) {
            DestroyLegacyLocale_54867D(&storage->privateLocale);
            throw;
        }
    } catch (...) {
        DestroyLegacyWideStreamBuffer_54976A(&storage->base);
        throw;
    }
}

LegacyNarrowFileBuffer* DeleteLegacyNarrowFileBuffer_548D4D(
    LegacyNarrowFileBuffer* const storage, const std::uint32_t flags)
{
    DestroyLegacyNarrowFileBuffer_5487DE(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

LegacyWideFileBuffer* DeleteLegacyWideFileBuffer_549DA2(
    LegacyWideFileBuffer* const storage, const std::uint32_t flags)
{
    DestroyLegacyWideFileBuffer_549935(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

} // namespace cg::platform
