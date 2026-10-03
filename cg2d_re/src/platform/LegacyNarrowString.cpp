#include "LegacyNarrowString.hpp"
#include "LegacyHeap.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <bit>
#include <cstring>

namespace cg::platform {

std::uint32_t gLegacyNarrowStringNpos550040 = 0xFFFFFFFFu;

namespace {

// 原53C9CF在旧字符串内部的原堆域；不是六处宿主裸字节适配的分配器。
void* AllocateLegacyNarrowStringBytes_53C9CF(const std::uint32_t byteCount)
{
    return AllocateLegacyHeapWithHandler_53C3BE(byteCount, 1);
}

// 原53B4E7在同一原堆域，只接收上述块的起点，不释放宿主字符串或适配块。
void FreeLegacyNarrowStringBytes_53B4E7(void* const memory)
{
    FreeLegacyHeapMemory_53C0A9(memory);
}

} // namespace

void TidyLegacyNarrowString_548E29(LegacyNarrowString* const storage, const bool release)
{
    if (release && storage->buffer != nullptr) {
        char* const tag = storage->buffer - 1;
        const auto reference = static_cast<std::uint8_t>(*tag);
        if (reference == 0 || reference == 0xFF) {
            FreeLegacyNarrowStringBytes_53B4E7(tag);
        } else {
            *tag = std::bit_cast<char>(static_cast<std::uint8_t>(reference - 1u));
        }
    }
    storage->buffer = nullptr;
    storage->length = 0;
    storage->capacity = 0;
}

bool GrowLegacyNarrowString_548FA0(LegacyNarrowString* const storage,
    const std::uint32_t requested, const bool trim)
{
    if (requested > 0xFFFFFFFDu) {
        ThrowLegacyStringLength_54A67A();
    }
    char* const buffer = storage->buffer;
    if (buffer != nullptr) {
        const auto reference = static_cast<std::uint8_t>(buffer[-1]);
        if (reference != 0 && reference != 0xFF) {
            if (requested == 0) {
                buffer[-1] = std::bit_cast<char>(
                    static_cast<std::uint8_t>(reference - 1u));
                TidyLegacyNarrowString_548E29(storage, false);
                return false;
            }
            CopyLegacyNarrowString_54908A(storage, requested);
            return true;
        }
    }
    if (requested == 0) {
        if (trim) {
            TidyLegacyNarrowString_548E29(storage, true);
        } else if (buffer != nullptr) {
            storage->length = 0;
            buffer[0] = '\0';
        }
        return false;
    }
    if (trim) {
        const std::uint32_t capacity = storage->capacity;
        if (capacity <= 0x1F && capacity >= requested) {
            return true;
        }
        TidyLegacyNarrowString_548E29(storage, true);
    } else if (storage->capacity >= requested) {
        return true;
    }
    CopyLegacyNarrowString_54908A(storage, requested);
    return true;
}

void CopyLegacyNarrowString_54908A(LegacyNarrowString* const storage,
    const std::uint32_t requested)
{
    std::uint32_t capacity = requested | 0x1Fu;
    if (capacity > 0xFFFFFFFDu) {
        capacity = requested;
    }
    char* block;
    try {
        const std::uint32_t bytes = capacity + 2u;
        const std::uint32_t allocation = std::bit_cast<std::int32_t>(bytes) < 0
            ? 0u : bytes;
        block = static_cast<char*>(AllocateLegacyNarrowStringBytes_53C9CF(allocation));
    } catch (...) {
        // 原551408的空type/5490C9处理器：异常才按原请求容量再分配。
        capacity = requested;
        const std::uint32_t bytes = requested + 2u;
        const std::uint32_t allocation = std::bit_cast<std::int32_t>(bytes) < 0
            ? 0u : bytes;
        block = static_cast<char*>(AllocateLegacyNarrowStringBytes_53C9CF(allocation));
    }
    std::uint32_t count = storage->length;
    if (count != 0) {
        if (count > capacity) {
            count = capacity;
        }
        char* const destination = block + 1;
        static_cast<void>(CopyLegacyMemory_541620(destination, storage->buffer, count));
    }
    const std::uint32_t oldLength = storage->length;
    TidyLegacyNarrowString_548E29(storage, true);
    char* const buffer = block + 1;
    storage->buffer = buffer;
    buffer[-1] = '\0';
    storage->capacity = capacity;
    const std::uint32_t length = oldLength <= capacity ? oldLength : capacity;
    char* const current = storage->buffer;
    storage->length = length;
    current[length] = '\0';
}

void FreezeLegacyNarrowString_548F74(LegacyNarrowString* const storage)
{
    char* const buffer = storage->buffer;
    if (buffer != nullptr) {
        const auto reference = static_cast<std::uint8_t>(buffer[-1]);
        if (reference != 0 && reference != 0xFF) {
            static_cast<void>(GrowLegacyNarrowString_548FA0(storage, storage->length, false));
        }
    }
    char* const current = storage->buffer;
    if (current != nullptr) {
        current[-1] = std::bit_cast<char>(std::uint8_t{0xFF});
    }
}

LegacyNarrowString* AssignLegacyNarrowString_549055(LegacyNarrowString* const storage,
    const char* const source, const std::uint32_t count)
{
    if (GrowLegacyNarrowString_548FA0(storage, count, true)) {
        static_cast<void>(CopyLegacyMemory_541620(storage->buffer, source, count));
        char* const buffer = storage->buffer;
        storage->length = count;
        buffer[count] = '\0';
    }
    return storage;
}

void SplitLegacyNarrowString_549025(LegacyNarrowString* const storage)
{
    char* const saved = storage->buffer;
    if (saved != nullptr) {
        const auto reference = static_cast<std::uint8_t>(saved[-1]);
        if (reference != 0 && reference != 0xFF) {
            TidyLegacyNarrowString_548E29(storage, true);
            const std::uint32_t count = StrLen_53F9E0(saved);
            static_cast<void>(AssignLegacyNarrowString_549055(storage, saved, count));
        }
    }
}

LegacyNarrowString* AppendLegacyNarrowString_548D69(LegacyNarrowString* const storage,
    const std::uint32_t count, const std::int8_t character)
{
    if (gLegacyNarrowStringNpos550040 - storage->length <= count) {
        ThrowLegacyStringLength_54A67A();
    }
    if (count != 0) {
        const std::uint32_t length = count + storage->length;
        if (GrowLegacyNarrowString_548FA0(storage, length, false)) {
            char* destination = storage->buffer;
            destination += storage->length;
            static_cast<void>(FillLegacyMemory_5419D0(destination, character, count));
            char* const buffer = storage->buffer;
            storage->length = length;
            buffer[length] = '\0';
        }
    }
    return storage;
}

LegacyNarrowString* EraseLegacyNarrowString_548DC2(LegacyNarrowString* const storage,
    const std::uint32_t position, std::uint32_t count)
{
    if (storage->length < position) {
        ThrowLegacyStringRange_54A899();
    }
    SplitLegacyNarrowString_549025(storage);
    const std::uint32_t suffix = storage->length - position;
    if (suffix < count) {
        count = suffix;
    }
    if (count != 0) {
        char* const destination = storage->buffer + position;
        std::memmove(destination, destination + count, suffix - count);
        const std::uint32_t length = storage->length - count;
        if (GrowLegacyNarrowString_548FA0(storage, length, false)) {
            char* const buffer = storage->buffer;
            storage->length = length;
            buffer[length] = '\0';
        }
    }
    return storage;
}

} // namespace cg::platform
