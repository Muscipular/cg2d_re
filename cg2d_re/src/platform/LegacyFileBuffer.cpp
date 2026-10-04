#include "LegacyFileBuffer.hpp"

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
        storage->state44Bits = 0;
    }
    ResetLegacyStreamBuffer_549EDE(&storage->buffer);
    if (file != nullptr && storage->modeOneFlag == 0) {
        // 原六间接槽共用FILE实际三字段，两侧均须保留别名。
        storage->buffer.readCurrentSlot = &file->current;
        storage->buffer.readBaseSlot = &file->base;
        storage->buffer.writeBaseSlot = &file->base;
        storage->buffer.writeCurrentSlot = &file->current;
        storage->buffer.readRemainingSlot = &file->remaining;
        storage->buffer.writeRemainingSlot = &file->remaining;
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
        storage->state44Bits = 0;
    }
    ResetLegacyStreamBuffer_549EDE(&storage->buffer);
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

} // namespace cg::platform
