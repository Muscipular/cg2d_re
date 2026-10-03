#include "LegacyStreamBuffer.hpp"

namespace cg::platform {

// 原5486BF的两个默认字符槽；栈中W32实参没有被读取。
std::int32_t DefaultLegacyNarrowCharacter_5486BF(
    LegacyNarrowStreamBuffer* const storage, const std::uint32_t ignoredStackValue)
{
    (void)storage;
    (void)ignoredStackValue;
    return -1;
}

std::int32_t DefaultLegacyNarrowRead_5486C5(
    LegacyNarrowStreamBuffer* const storage)
{
    (void)storage;
    return -1;
}

// 原宽默认入口只规定AX；U16返回不承诺原EAX高W16。
std::uint16_t DefaultLegacyWideCharacter_549778(
    LegacyWideStreamBuffer* const storage, const std::uint32_t ignoredStackValue)
{
    (void)storage;
    (void)ignoredStackValue;
    return 0xFFFFu;
}

std::uint16_t DefaultLegacyWideRead_549782(
    LegacyWideStreamBuffer* const storage)
{
    (void)storage;
    return 0xFFFFu;
}

// 原54977F由窄宽六个默认槽共用；完整EAX为0，不访问对象。
template <typename Character>
std::int32_t DefaultLegacyStreamZero_54977F(
    LegacyStreamBufferStorage<Character>* const storage)
{
    (void)storage;
    return 0;
}
template std::int32_t DefaultLegacyStreamZero_54977F<char>(
    LegacyNarrowStreamBuffer* storage);
template std::int32_t DefaultLegacyStreamZero_54977F<std::uint16_t>(
    LegacyWideStreamBuffer* storage);

// 原549925只返回this；两个栈参未读取，不实际绑定缓冲或操作FILE。
template <typename Character>
LegacyStreamBufferStorage<Character>* DefaultLegacySetBuffer_549925(
    LegacyStreamBufferStorage<Character>* const storage,
    Character* const ignoredBuffer, const std::uint32_t ignoredCount)
{
    (void)ignoredBuffer;
    (void)ignoredCount;
    return storage;
}
template LegacyNarrowStreamBuffer* DefaultLegacySetBuffer_549925<char>(
    LegacyNarrowStreamBuffer* storage, char* ignoredBuffer, std::uint32_t ignoredCount);
template LegacyWideStreamBuffer* DefaultLegacySetBuffer_549925<std::uint16_t>(
    LegacyWideStreamBuffer* storage, std::uint16_t* ignoredBuffer,
    std::uint32_t ignoredCount);

char* DecrementLegacyReadCursor_548EA5(LegacyNarrowStreamBuffer* const storage)
{
    ++*storage->readRemainingSlot;
    --*storage->readCurrentSlot;
    return *storage->readCurrentSlot;
}

char* IncrementLegacyReadCursor_548EB5(LegacyNarrowStreamBuffer* const storage)
{
    --*storage->readRemainingSlot;
    char** const slot = storage->readCurrentSlot;
    char* const previous = *slot;
    *slot = previous + 1;
    return previous;
}

char* IncrementLegacyWriteCursor_548EC5(LegacyNarrowStreamBuffer* const storage)
{
    --*storage->writeRemainingSlot;
    char** const slot = storage->writeCurrentSlot;
    char* const previous = *slot;
    *slot = previous + 1;
    return previous;
}

std::uint16_t* DecrementLegacyWideReadCursor_549E1C(LegacyWideStreamBuffer* const storage)
{
    ++*storage->readRemainingSlot;
    --*storage->readCurrentSlot;
    return *storage->readCurrentSlot;
}

std::uint16_t* IncrementLegacyWideReadCursor_549E2D(LegacyWideStreamBuffer* const storage)
{
    --*storage->readRemainingSlot;
    std::uint16_t** const slot = storage->readCurrentSlot;
    std::uint16_t* const previous = *slot;
    *slot = previous + 1;
    return previous;
}

std::uint16_t* IncrementLegacyWideWriteCursor_549E51(LegacyWideStreamBuffer* const storage)
{
    --*storage->writeRemainingSlot;
    std::uint16_t** const slot = storage->writeCurrentSlot;
    std::uint16_t* const previous = *slot;
    *slot = previous + 1;
    return previous;
}

void BumpLegacyWideReadCursor_549E08(LegacyWideStreamBuffer* const storage,
    const std::int32_t characterCount)
{
    *storage->readRemainingSlot -= characterCount;
    *storage->readCurrentSlot += characterCount;
}

void BumpLegacyWideWriteCursor_549E3D(LegacyWideStreamBuffer* const storage,
    const std::int32_t characterCount)
{
    *storage->writeRemainingSlot -= characterCount;
    *storage->writeCurrentSlot += characterCount;
}

template <typename Character>
void ResetLegacyStreamBuffer_549EDE(LegacyStreamBufferStorage<Character>* const storage)
{
    storage->readCurrentSlot = &storage->readCurrent;
    storage->writeCurrentSlot = &storage->writeCurrent;
    storage->readRemainingSlot = &storage->readRemaining;
    storage->readBaseSlot = &storage->readBase;
    storage->writeRemainingSlot = &storage->writeRemaining;
    storage->writeBaseSlot = &storage->writeBase;
    *storage->writeBaseSlot = nullptr;
    *storage->writeCurrentSlot = nullptr;
    *storage->writeRemainingSlot = 0;
    *storage->readBaseSlot = nullptr;
    *storage->readCurrentSlot = nullptr;
    *storage->readRemainingSlot = 0;
}

template void ResetLegacyStreamBuffer_549EDE<char>(LegacyNarrowStreamBuffer*);
template void ResetLegacyStreamBuffer_549EDE<std::uint16_t>(LegacyWideStreamBuffer*);

} // namespace cg::platform
