#include "LegacyFileBuffer.hpp"

#include "LegacyHeap.hpp"
#include "cg/platform/LegacyMemory.hpp"

#include <bit>
#include <cstring>

namespace cg::platform {

// 原PE .data虚拟尾部初始填零；保持两个独立可写种子及原每入口两次读取。
volatile std::uint32_t gLegacyNarrowFileState1087B00 = 0u;
volatile std::uint32_t gLegacyWideFileState1087AF8 = 0u;

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
    storage->codecvtFacet = nullptr;
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
    storage->codecvtFacet = nullptr;
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

std::int32_t PutBackLegacyNarrowFileBuffer_548979(
    LegacyNarrowFileBuffer* const storage, const std::int32_t character)
{
    char* const current = *storage->base.buffer.readCurrentSlot;
    if (current != nullptr && reinterpret_cast<std::uintptr_t>(
            *storage->base.buffer.readBaseSlot) < reinterpret_cast<std::uintptr_t>(current)) {
        if (character == -1 || static_cast<std::uint8_t>(current[-1]) == character) {
            DecrementLegacyReadCursor_548EA5(&storage->base.buffer);
            return character == -1 ? 0 : character;
        }
    }
    LegacyFile* const file = storage->file;
    if (file == nullptr || character == -1) {
        return -1;
    }
    if (storage->codecvtFacet == nullptr) {
        return UnreadLegacyStreamByte_54463F(static_cast<std::uint8_t>(character), file) == -1
            ? -1 : character;
    }
    LegacyNarrowString* const bytes = storage->scratchString;
    const std::uint32_t length = bytes->length;
    if (length == 0) {
        return -1;
    }
    FreezeLegacyNarrowString_548F74(bytes);
    const char* const first = bytes->buffer;
    // 原Freeze后再次取FILE，反向回推正常成功才重取字符串/清串并恢复状态。
    if (!UnreadLegacyStreamBytes_549147(*first, storage->file, length)) {
        return -1;
    }
    const std::uint32_t eraseCount = gLegacyNarrowStringNpos550040;
    LegacyNarrowString* const eraseString = storage->scratchString;
    EraseLegacyNarrowString_548DC2(eraseString, 0, eraseCount);
    storage->state40Bits = storage->state3CBits;
    return character;
}

std::uint16_t PutBackLegacyWideFileBuffer_549AE2(
    LegacyWideFileBuffer* const storage, const std::uint16_t character)
{
    std::uint16_t* const current = *storage->base.buffer.readCurrentSlot;
    if (current != nullptr && reinterpret_cast<std::uintptr_t>(
            *storage->base.buffer.readBaseSlot) < reinterpret_cast<std::uintptr_t>(current)) {
        if (character == 0xFFFFu || current[-1] == character) {
            DecrementLegacyWideReadCursor_549E1C(&storage->base.buffer);
            return character == 0xFFFFu ? 0 : character;
        }
    }
    LegacyFile* const file = storage->file;
    if (file == nullptr || character == 0xFFFFu) {
        return 0xFFFFu;
    }
    if (storage->codecvtFacet == nullptr) {
        return UnreadLegacyStreamWide_54AE87(character, file) == 0xFFFFu
            ? 0xFFFFu : character;
    }
    LegacyNarrowString* const bytes = storage->scratchString;
    const std::uint32_t length = bytes->length;
    if (length == 0) {
        return 0xFFFFu;
    }
    FreezeLegacyNarrowString_548F74(bytes);
    const char* const first = bytes->buffer;
    if (!UnreadLegacyStreamBytes_549147(*first, storage->file, length)) {
        return 0xFFFFu;
    }
    const std::uint32_t eraseCount = gLegacyNarrowStringNpos550040;
    LegacyNarrowString* const eraseString = storage->scratchString;
    EraseLegacyNarrowString_548DC2(eraseString, 0, eraseCount);
    storage->state40Bits = storage->state3CBits;
    return character;
}

std::int32_t PeekLegacyNarrowFileBuffer_548A26(LegacyNarrowFileBuffer* const storage)
{
    char* const current = *storage->base.buffer.readCurrentSlot;
    if (current != nullptr) {
        char* const last = current + *storage->base.buffer.readRemainingSlot;
        if (reinterpret_cast<std::uintptr_t>(current) < reinterpret_cast<std::uintptr_t>(last)) {
            return static_cast<std::uint8_t>(*current);
        }
    }
    const auto* const savedMethods = storage->base.buffer.virtualTable;
    const std::int32_t character = savedMethods->uflow(&storage->base);
    // 原即使EOF也调同一保存表的pbackfail，不重取表或省略回推副作用。
    return savedMethods->pbackfail(&storage->base, character);
}

std::uint16_t PeekLegacyWideFileBuffer_549B99(LegacyWideFileBuffer* const storage)
{
    std::uint16_t* const current = *storage->base.buffer.readCurrentSlot;
    if (current != nullptr) {
        std::uint16_t* const last = current + *storage->base.buffer.readRemainingSlot;
        if (reinterpret_cast<std::uintptr_t>(current) < reinterpret_cast<std::uintptr_t>(last)) {
            return *current;
        }
    }
    const auto* const savedMethods = storage->base.buffer.virtualTable;
    const std::uint16_t character = savedMethods->uflow(&storage->base);
    return savedMethods->pbackfail(&storage->base, character);
}

std::int32_t ReadLegacyNarrowFileBuffer_548A53(LegacyNarrowFileBuffer* const storage)
{
    char* const current = *storage->base.buffer.readCurrentSlot;
    if (current != nullptr) {
        char* const last = current + *storage->base.buffer.readRemainingSlot;
        if (reinterpret_cast<std::uintptr_t>(current) < reinterpret_cast<std::uintptr_t>(last)) {
            return static_cast<std::uint8_t>(
                *IncrementLegacyReadCursor_548EB5(&storage->base.buffer));
        }
    }
    LegacyFile* const file = storage->file;
    if (file == nullptr) {
        return -1;
    }
    if (storage->codecvtFacet == nullptr) {
        const std::int32_t character = ReadLegacyStreamByte_53C8B3(file);
        return character == -1 ? -1 : static_cast<std::uint8_t>(character);
    }
    char output; // 原输出/两next局部不初始化，不新增成功产出或进度检查。
    const char* inputNext;
    char* outputNext;
    const std::uint32_t originalState = storage->state40Bits;
    LegacyNarrowString* const initialBytes = storage->scratchString;
    storage->state3CBits = originalState;
    const std::uint32_t eraseCount = gLegacyNarrowStringNpos550040;
    EraseLegacyNarrowString_548DC2(initialBytes, 0, eraseCount);
    for (;;) {
        const std::int32_t character = ReadLegacyStreamByte_53C8B3(storage->file);
        if (character == -1) {
            return -1;
        }
        AppendLegacyNarrowString_548D69(storage->scratchString, 1,
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(character)));
        LegacyNarrowString* const firstBytes = storage->scratchString;
        storage->state40Bits = storage->state3CBits;
        FreezeLegacyNarrowString_548F74(firstBytes);
        LegacyNarrowString* const secondBytes = storage->scratchString;
        const char* const first = firstBytes->buffer;
        FreezeLegacyNarrowString_548F74(secondBytes);
        const std::uint32_t length = firstBytes->length;
        const char* const input = secondBytes->buffer;
        const char* const last = first != nullptr ? first + length : nullptr;
        LegacyLocaleFacet* const facet = storage->codecvtFacet;
        const auto* const methods = reinterpret_cast<const LegacyNarrowCodecvtInputMethods*>(
            facet->methods);
        const std::int32_t result = methods->input(facet, &storage->state40Bits,
            input, last, &inputNext, &output, &output + 1, &outputNext);
        switch (result) {
        case 0:
            return static_cast<std::uint8_t>(output);
        case 1:
            continue;
        case 3: {
            LegacyNarrowString* const bytes = storage->scratchString;
            if (bytes->length < 1u) {
                continue;
            }
            FreezeLegacyNarrowString_548F74(bytes);
            CopyLegacyMemory_541620(&output, bytes->buffer, 1);
            return static_cast<std::uint8_t>(output);
        }
        default:
            return -1;
        }
    }
}

std::uint16_t ReadLegacyWideFileBuffer_549BC7(LegacyWideFileBuffer* const storage)
{
    std::uint16_t* const current = *storage->base.buffer.readCurrentSlot;
    if (current != nullptr) {
        std::uint16_t* const last = current + *storage->base.buffer.readRemainingSlot;
        if (reinterpret_cast<std::uintptr_t>(current) < reinterpret_cast<std::uintptr_t>(last)) {
            return *IncrementLegacyWideReadCursor_549E2D(&storage->base.buffer);
        }
    }
    LegacyFile* const file = storage->file;
    if (file == nullptr) {
        return 0xFFFFu;
    }
    if (storage->codecvtFacet == nullptr) {
        const std::uint16_t character = ReadLegacyStreamWide_54AF70(file);
        return character == 0xFFFFu ? 0xFFFFu : character;
    }
    std::uint16_t output;
    const char* inputNext;
    std::uint16_t* outputNext;
    const std::uint32_t originalState = storage->state40Bits;
    LegacyNarrowString* const initialBytes = storage->scratchString;
    storage->state3CBits = originalState;
    const std::uint32_t eraseCount = gLegacyNarrowStringNpos550040;
    EraseLegacyNarrowString_548DC2(initialBytes, 0, eraseCount);
    for (;;) {
        // 原宽转换仍读取窄字节并累积窄字符串，不能以fgetwc替代此循环。
        const std::int32_t character = ReadLegacyStreamByte_53C8B3(storage->file);
        if (character == -1) {
            return 0xFFFFu;
        }
        AppendLegacyNarrowString_548D69(storage->scratchString, 1,
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(character)));
        LegacyNarrowString* const firstBytes = storage->scratchString;
        storage->state40Bits = storage->state3CBits;
        FreezeLegacyNarrowString_548F74(firstBytes);
        LegacyNarrowString* const secondBytes = storage->scratchString;
        const char* const first = firstBytes->buffer;
        FreezeLegacyNarrowString_548F74(secondBytes);
        const std::uint32_t length = firstBytes->length;
        const char* const input = secondBytes->buffer;
        const char* const last = first != nullptr ? first + length : nullptr;
        LegacyLocaleFacet* const facet = storage->codecvtFacet;
        const auto* const methods = reinterpret_cast<const LegacyWideCodecvtInputMethods*>(
            facet->methods);
        const std::int32_t result = methods->input(facet, &storage->state40Bits,
            input, last, &inputNext, &output, &output + 1, &outputNext);
        switch (result) {
        case 0:
            return output;
        case 1:
            continue;
        case 3: {
            LegacyNarrowString* const bytes = storage->scratchString;
            if (bytes->length < 2u) {
                continue;
            }
            FreezeLegacyNarrowString_548F74(bytes);
            CopyLegacyMemory_541620(&output, bytes->buffer, 2);
            return output;
        }
        default:
            return 0xFFFFu;
        }
    }
}

std::int32_t WriteLegacyNarrowFileBuffer_548843(
    LegacyNarrowFileBuffer* const storage, const std::int32_t character)
{
    if (character == -1) {
        return 0;
    }
    char* const current = *storage->base.buffer.writeCurrentSlot;
    if (current != nullptr) {
        char* const last = current + *storage->base.buffer.writeRemainingSlot;
        if (reinterpret_cast<std::uintptr_t>(current) < reinterpret_cast<std::uintptr_t>(last)) {
            *IncrementLegacyWriteCursor_548EC5(&storage->base.buffer) =
                std::bit_cast<char>(static_cast<std::uint8_t>(character));
            return character;
        }
    }
    LegacyFile* const file = storage->file;
    if (file == nullptr) {
        return -1;
    }
    if (storage->codecvtFacet == nullptr) {
        // 原movsx bl，低S8符号扩展，不替低U8或把FF当输入EOF。
        const std::int8_t byte = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(character));
        return WriteLegacyStreamByte_53CFDC(byte, file) == -1 ? -1 : character;
    }
    const std::uint32_t eraseCount = gLegacyNarrowStringNpos550040;
    LegacyNarrowString* const initialBytes = storage->scratchString;
    char input = std::bit_cast<char>(static_cast<std::uint8_t>(character));
    EraseLegacyNarrowString_548DC2(initialBytes, 0, eraseCount);
    AppendLegacyNarrowString_548D69(storage->scratchString, 8, 0);
    LegacyNarrowString* const firstBytes = storage->scratchString;
    FreezeLegacyNarrowString_548F74(firstBytes);
    LegacyNarrowString* const secondBytes = storage->scratchString;
    char* const first = firstBytes->buffer;
    FreezeLegacyNarrowString_548F74(secondBytes);
    const std::uint32_t length = firstBytes->length;
    char* const output = secondBytes->buffer;
    char* const last = first != nullptr ? first + length : nullptr;
    const char* inputNext; // 原next局部未预清。
    char* outputNext;
    LegacyLocaleFacet* const facet = storage->codecvtFacet;
    const auto* const methods = reinterpret_cast<const LegacyNarrowCodecvtOutputMethods*>(
        facet->methods);
    const std::int32_t result = methods->output(facet, &storage->state40Bits,
        &input, &input + 1, &inputNext, output, last, &outputNext);
    if (result == 3) {
        const std::int8_t byte = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(input));
        LegacyFile* const directFile = storage->file;
        return WriteLegacyStreamByte_53CFDC(byte, directFile) == -1 ? -1 : character;
    }
    if (result == 1) {
        if (inputNext == &input) {
            return -1;
        }
    } else if (result != 0) {
        return -1;
    }
    LegacyNarrowString* const countBytes = storage->scratchString;
    FreezeLegacyNarrowString_548F74(countBytes);
    const std::uint32_t produced = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(outputNext)
        - reinterpret_cast<std::uintptr_t>(countBytes->buffer));
    LegacyNarrowString* const writeBytes = storage->scratchString;
    FreezeLegacyNarrowString_548F74(writeBytes);
    LegacyFile* const outputFile = storage->file;
    const char* const outputBuffer = writeBytes->buffer;
    return WriteLegacyStream_53C680(outputBuffer, 1, produced, outputFile) == produced
        ? character : -1;
}

std::uint16_t WriteLegacyWideFileBuffer_54999A(
    LegacyWideFileBuffer* const storage, const std::uint16_t character)
{
    if (character == 0xFFFFu) {
        return 0;
    }
    std::uint16_t* const current = *storage->base.buffer.writeCurrentSlot;
    if (current != nullptr) {
        std::uint16_t* const last = current + *storage->base.buffer.writeRemainingSlot;
        if (reinterpret_cast<std::uintptr_t>(current) < reinterpret_cast<std::uintptr_t>(last)) {
            *IncrementLegacyWideWriteCursor_549E51(&storage->base.buffer) = character;
            return character;
        }
    }
    LegacyFile* const file = storage->file;
    if (file == nullptr) {
        return 0xFFFFu;
    }
    if (storage->codecvtFacet == nullptr) {
        return WriteLegacyStreamWide_54AD92(character, file) == 0xFFFFu ? 0xFFFFu : character;
    }
    const std::uint32_t eraseCount = gLegacyNarrowStringNpos550040;
    LegacyNarrowString* const initialBytes = storage->scratchString;
    std::uint16_t input = character;
    EraseLegacyNarrowString_548DC2(initialBytes, 0, eraseCount);
    AppendLegacyNarrowString_548D69(storage->scratchString, 8, 0);
    LegacyNarrowString* const firstBytes = storage->scratchString;
    FreezeLegacyNarrowString_548F74(firstBytes);
    LegacyNarrowString* const secondBytes = storage->scratchString;
    char* const first = firstBytes->buffer;
    FreezeLegacyNarrowString_548F74(secondBytes);
    const std::uint32_t length = firstBytes->length;
    char* const output = secondBytes->buffer;
    char* const last = first != nullptr ? first + length : nullptr;
    const std::uint16_t* inputNext;
    char* outputNext;
    LegacyLocaleFacet* const facet = storage->codecvtFacet;
    const auto* const methods = reinterpret_cast<const LegacyWideCodecvtOutputMethods*>(
        facet->methods);
    const std::int32_t result = methods->output(facet, &storage->state40Bits,
        &input, &input + 1, &inputNext, output, last, &outputNext);
    if (result == 3) {
        // 原先读取当时FILE再取局部字符，保留实际callback修改后的局部值。
        LegacyFile* const directFile = storage->file;
        const std::uint16_t directCharacter = input;
        return WriteLegacyStreamWide_54AD92(directCharacter, directFile) == 0xFFFFu
            ? 0xFFFFu : character;
    }
    if (result == 1) {
        if (inputNext == &input) {
            return 0xFFFFu;
        }
    } else if (result != 0) {
        return 0xFFFFu;
    }
    LegacyNarrowString* const countBytes = storage->scratchString;
    FreezeLegacyNarrowString_548F74(countBytes);
    const std::uint32_t produced = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(outputNext)
        - reinterpret_cast<std::uintptr_t>(countBytes->buffer));
    LegacyNarrowString* const writeBytes = storage->scratchString;
    FreezeLegacyNarrowString_548F74(writeBytes);
    LegacyFile* const outputFile = storage->file;
    const char* const outputBuffer = writeBytes->buffer;
    return WriteLegacyStream_53C680(outputBuffer, 1, produced, outputFile) == produced
        ? character : 0xFFFFu;
}

template <typename Character>
std::int32_t SyncLegacyFileBuffer_549D13(LegacyFileBufferStorage<Character>* const storage)
{
    LegacyFile* const file = storage->file;
    if (file == nullptr) {
        return 0;
    }
    return FlushLegacyStream_53BB02(file) < 0 ? -1 : 0;
}

template std::int32_t SyncLegacyFileBuffer_549D13<char>(LegacyNarrowFileBuffer*);
template std::int32_t SyncLegacyFileBuffer_549D13<std::uint16_t>(LegacyWideFileBuffer*);

template <typename Character>
LegacyStreamPosition* SeekLegacyFileBufferOffset_548B68(
    LegacyFileBufferStorage<Character>* const storage, LegacyStreamPosition* const result,
    const std::int32_t offset, const std::uint32_t origin,
    const std::uint32_t /*modeBits*/)
{
    LegacyFilePosition filePosition; // 原两个W32位置局部，不预清。
    LegacyStreamPosition value;      // 原+04/+14未写，整24B复制，不补值。
    LegacyFile* const file = storage->file;
    if (file != nullptr && SeekLegacyStream_53BC41(file, offset, origin) == 0 &&
        GetLegacyStreamPosition_54ACB7(storage->file, &filePosition) == 0) {
        value.offsetBits = 0;
        value.filePositionLowBits = filePosition.low;
        value.filePositionHighBits = filePosition.high;
        value.stateBits = storage->state40Bits;
    } else {
        const std::uint32_t low = gLegacyDefaultPositionLow5500E0;
        value.offsetBits = 0xFFFFFFFFu;
        value.filePositionLowBits = low;
        value.filePositionHighBits = gLegacyDefaultPositionHigh5500E4;
        value.stateBits = gLegacyDefaultPositionState1087B04;
    }
    std::memcpy(result, &value, sizeof(value));
    return result;
}

template <typename Character>
LegacyStreamPosition* SeekLegacyFileBufferPosition_548BEA(
    LegacyFileBufferStorage<Character>* const storage, LegacyStreamPosition* const result,
    LegacyStreamPosition position, const std::uint32_t /*modeBits*/)
{
    LegacyFilePosition filePosition;
    filePosition.low = position.filePositionLowBits;
    const std::int32_t offset = std::bit_cast<std::int32_t>(position.offsetBits);
    const std::uint32_t high = position.filePositionHighBits;
    LegacyFile* const file = storage->file;
    filePosition.high = high;
    if (file != nullptr && SetLegacyStreamPosition_54ACD9(file, &filePosition) == 0 &&
        (offset == 0 || SeekLegacyStream_53BC41(storage->file, offset, 1) == 0) &&
        GetLegacyStreamPosition_54ACB7(storage->file, &filePosition) == 0) {
        LegacyNarrowString* const bytes = storage->scratchString;
        if (bytes != nullptr) {
            storage->state40Bits = position.stateBits;
            const std::uint32_t eraseCount = gLegacyNarrowStringNpos550040;
            EraseLegacyNarrowString_548DC2(bytes, 0, eraseCount);
        }
        const std::uint32_t low = filePosition.low;
        position.offsetBits = 0;
        position.filePositionLowBits = low;
        position.filePositionHighBits = filePosition.high;
        position.stateBits = storage->state40Bits;
    } else {
        const std::uint32_t low = gLegacyDefaultPositionLow5500E0;
        position.offsetBits = 0xFFFFFFFFu;
        position.filePositionLowBits = low;
        position.filePositionHighBits = gLegacyDefaultPositionHigh5500E4;
        position.stateBits = gLegacyDefaultPositionState1087B04;
    }
    // 只改输入位置四个已证W32，复制实际输入对象表示；不填充两个未写区域。
    std::memcpy(result, &position, sizeof(position));
    return result;
}

template LegacyStreamPosition* SeekLegacyFileBufferOffset_548B68<char>(
    LegacyNarrowFileBuffer*, LegacyStreamPosition*, std::int32_t, std::uint32_t, std::uint32_t);
template LegacyStreamPosition* SeekLegacyFileBufferOffset_548B68<std::uint16_t>(
    LegacyWideFileBuffer*, LegacyStreamPosition*, std::int32_t, std::uint32_t, std::uint32_t);
template LegacyStreamPosition* SeekLegacyFileBufferPosition_548BEA<char>(
    LegacyNarrowFileBuffer*, LegacyStreamPosition*, LegacyStreamPosition, std::uint32_t);
template LegacyStreamPosition* SeekLegacyFileBufferPosition_548BEA<std::uint16_t>(
    LegacyWideFileBuffer*, LegacyStreamPosition*, LegacyStreamPosition, std::uint32_t);

LegacyNarrowStreamBufferObject* SetLegacyNarrowFileBuffer_548CAC(
    LegacyNarrowFileBuffer* const storage, char* const buffer, const std::uint32_t countBits)
{
    LegacyFile* const file = storage->file;
    if (file == nullptr || SetLegacyStreamBuffer_54ACF1(file, buffer, 0, countBits) != 0) {
        return nullptr;
    }
    return &storage->base;
}

LegacyWideStreamBufferObject* SetLegacyWideFileBuffer_549CE5(
    LegacyWideFileBuffer* const storage, std::uint16_t* const buffer,
    const std::uint32_t countBits)
{
    LegacyFile* const file = storage->file;
    if (file == nullptr) {
        return nullptr;
    }
    const std::uint32_t byteCount = countBits + countBits;
    if (SetLegacyStreamBuffer_54ACF1(file, reinterpret_cast<char*>(buffer), 0, byteCount) != 0) {
        return nullptr;
    }
    return &storage->base;
}

namespace {

// 仅派生实际表把真实filebuf.base首成员地址还原为所属对象；不覆盖独立基类存储。
LegacyNarrowStreamBufferObject* NarrowFileDeleteMethod(
    LegacyNarrowStreamBufferObject* const storage, const std::uint32_t flags)
{
    return reinterpret_cast<LegacyNarrowStreamBufferObject*>(DeleteLegacyNarrowFileBuffer_548D4D(
        reinterpret_cast<LegacyNarrowFileBuffer*>(storage), flags));
}

LegacyWideStreamBufferObject* WideFileDeleteMethod(
    LegacyWideStreamBufferObject* const storage, const std::uint32_t flags)
{
    return reinterpret_cast<LegacyWideStreamBufferObject*>(DeleteLegacyWideFileBuffer_549DA2(
        reinterpret_cast<LegacyWideFileBuffer*>(storage), flags));
}

std::int32_t NarrowFileOverflowMethod(
    LegacyNarrowStreamBufferObject* const storage, const std::int32_t character)
{
    return WriteLegacyNarrowFileBuffer_548843(
        reinterpret_cast<LegacyNarrowFileBuffer*>(storage), character);
}

std::uint16_t WideFileOverflowMethod(
    LegacyWideStreamBufferObject* const storage, const std::uint16_t character)
{
    return WriteLegacyWideFileBuffer_54999A(
        reinterpret_cast<LegacyWideFileBuffer*>(storage), character);
}

std::int32_t NarrowFilePutBackMethod(
    LegacyNarrowStreamBufferObject* const storage, const std::int32_t character)
{
    return PutBackLegacyNarrowFileBuffer_548979(
        reinterpret_cast<LegacyNarrowFileBuffer*>(storage), character);
}

std::uint16_t WideFilePutBackMethod(
    LegacyWideStreamBufferObject* const storage, const std::uint16_t character)
{
    return PutBackLegacyWideFileBuffer_549AE2(
        reinterpret_cast<LegacyWideFileBuffer*>(storage), character);
}

std::int32_t NarrowFilePeekMethod(LegacyNarrowStreamBufferObject* const storage)
{
    return PeekLegacyNarrowFileBuffer_548A26(reinterpret_cast<LegacyNarrowFileBuffer*>(storage));
}

std::uint16_t WideFilePeekMethod(LegacyWideStreamBufferObject* const storage)
{
    return PeekLegacyWideFileBuffer_549B99(reinterpret_cast<LegacyWideFileBuffer*>(storage));
}

std::int32_t NarrowFileReadMethod(LegacyNarrowStreamBufferObject* const storage)
{
    return ReadLegacyNarrowFileBuffer_548A53(reinterpret_cast<LegacyNarrowFileBuffer*>(storage));
}

std::uint16_t WideFileReadMethod(LegacyWideStreamBufferObject* const storage)
{
    return ReadLegacyWideFileBuffer_549BC7(reinterpret_cast<LegacyWideFileBuffer*>(storage));
}

template <typename Character>
std::int32_t FileZeroMethod(LegacyStreamBufferObject<Character>* const storage)
{
    return DefaultLegacyStreamZero_54977F(&storage->buffer);
}

template <typename Character>
LegacyStreamPosition* FileSeekOffsetMethod(
    LegacyStreamBufferObject<Character>* const storage, LegacyStreamPosition* const result,
    const std::uint32_t offsetBits, const std::uint32_t originBits,
    const std::uint32_t modeBits)
{
    return SeekLegacyFileBufferOffset_548B68(
        reinterpret_cast<LegacyFileBufferStorage<Character>*>(storage), result,
        std::bit_cast<std::int32_t>(offsetBits), originBits, modeBits);
}

template <typename Character>
LegacyStreamPosition* FileSeekPositionMethod(
    LegacyStreamBufferObject<Character>* const storage, LegacyStreamPosition* const result,
    const LegacyStreamPosition position, const std::uint32_t modeBits)
{
    return SeekLegacyFileBufferPosition_548BEA(
        reinterpret_cast<LegacyFileBufferStorage<Character>*>(storage), result, position, modeBits);
}

LegacyNarrowStreamBufferObject* NarrowFileSetBufferMethod(
    LegacyNarrowStreamBufferObject* const storage, char* const buffer,
    const std::uint32_t countBits)
{
    return SetLegacyNarrowFileBuffer_548CAC(
        reinterpret_cast<LegacyNarrowFileBuffer*>(storage), buffer, countBits);
}

LegacyWideStreamBufferObject* WideFileSetBufferMethod(
    LegacyWideStreamBufferObject* const storage, std::uint16_t* const buffer,
    const std::uint32_t countBits)
{
    return SetLegacyWideFileBuffer_549CE5(
        reinterpret_cast<LegacyWideFileBuffer*>(storage), buffer, countBits);
}

template <typename Character>
std::int32_t FileSyncMethod(LegacyStreamBufferObject<Character>* const storage)
{
    return SyncLegacyFileBuffer_549D13(reinterpret_cast<LegacyFileBufferStorage<Character>*>(storage));
}

template <typename Character>
void FileNoOpMethod(
    LegacyStreamBufferObject<Character>* const storage, const std::uint32_t ignoredStackValue)
{
    DefaultLegacyStreamNoOp_5487D0(&storage->buffer, ignoredStackValue);
}

} // namespace

// 原派生两个13槽常量表；四批量槽直接用实际基类，其他真实目标经地址适配调用。
const LegacyNarrowStreamBufferMethods gLegacyNarrowFileBufferMethods54FFD4{
    NarrowFileDeleteMethod,
    NarrowFileOverflowMethod,
    NarrowFilePutBackMethod,
    FileZeroMethod<char>,
    NarrowFilePeekMethod,
    NarrowFileReadMethod,
    ReadLegacyNarrowStreamBuffer_5486E6,
    WriteLegacyNarrowStreamBuffer_54875A,
    FileSeekOffsetMethod<char>,
    FileSeekPositionMethod<char>,
    NarrowFileSetBufferMethod,
    FileSyncMethod<char>,
    FileNoOpMethod<char>
};
const LegacyWideStreamBufferMethods gLegacyWideFileBufferMethods550070{
    WideFileDeleteMethod,
    WideFileOverflowMethod,
    WideFilePutBackMethod,
    FileZeroMethod<std::uint16_t>,
    WideFilePeekMethod,
    WideFileReadMethod,
    ReadLegacyWideStreamBuffer_5497A8,
    WriteLegacyWideStreamBuffer_54982F,
    FileSeekOffsetMethod<std::uint16_t>,
    FileSeekPositionMethod<std::uint16_t>,
    WideFileSetBufferMethod,
    FileSyncMethod<std::uint16_t>,
    FileNoOpMethod<std::uint16_t>
};

LegacyNarrowFileBuffer* ConstructLegacyNarrowFileBufferLate_5487D3(
    LegacyNarrowFileBuffer* const storage, const std::uint32_t /*ignoredStackWord*/)
{
    storage->base.buffer.virtualTable = &gLegacyNarrowFileBufferMethods54FFD4;
    return storage;
}

LegacyWideFileBuffer* ConstructLegacyWideFileBufferLate_54992A(
    LegacyWideFileBuffer* const storage, const std::uint32_t /*ignoredStackWord*/)
{
    storage->base.buffer.virtualTable = &gLegacyWideFileBufferMethods550070;
    return storage;
}

} // namespace cg::platform
