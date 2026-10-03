#include "LegacyStreamBuffer.hpp"

namespace cg::platform {

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
