#include "LegacyStreamBuffer.hpp"

#include "LegacyHeap.hpp"
#include "cg/platform/LegacyMemory.hpp"

#include <bit>
#include <cstring>

namespace cg::platform {

volatile std::uint32_t gLegacyDefaultPositionLow5500E0 = 0;
volatile std::uint32_t gLegacyDefaultPositionHigh5500E4 = 0;
// 原PE .data虚拟尾部载入填零；只定义真实可写状态，不改变原默认位置未写区。
volatile std::uint32_t gLegacyDefaultPositionState1087B04 = 0u;


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

template <typename Character>
LegacyStreamPosition* DefaultLegacySeekOffset_5498B7(
    LegacyStreamBufferStorage<Character>* const storage, LegacyStreamPosition* const result,
    const std::uint32_t ignoredOffset, const std::uint32_t ignoredOrigin,
    const std::uint32_t ignoredMode)
{
    LegacyStreamPosition position; // 原两处未写区域不值初始化/补零。
    const std::uint32_t low = gLegacyDefaultPositionLow5500E0;
    position.offsetBits = 0xFFFFFFFFu;
    position.filePositionLowBits = low;
    position.filePositionHighBits = gLegacyDefaultPositionHigh5500E4;
    position.stateBits = gLegacyDefaultPositionState1087B04;
    // 原REP六W32；标准对象表示复制允许未初始化填充，不读取其为数值。
    std::memcpy(result, &position, sizeof(position));
    return result;
}

template <typename Character>
LegacyStreamPosition* DefaultLegacySeekPosition_5498EE(
    LegacyStreamBufferStorage<Character>* const storage, LegacyStreamPosition* const result,
    const LegacyStreamPosition ignoredPosition, const std::uint32_t ignoredMode)
{
    LegacyStreamPosition position;
    const std::uint32_t low = gLegacyDefaultPositionLow5500E0;
    position.offsetBits = 0xFFFFFFFFu;
    position.filePositionLowBits = low;
    position.filePositionHighBits = gLegacyDefaultPositionHigh5500E4;
    position.stateBits = gLegacyDefaultPositionState1087B04;
    std::memcpy(result, &position, sizeof(position));
    return result;
}

template <typename Character>
void DefaultLegacyStreamNoOp_5487D0(
    LegacyStreamBufferStorage<Character>* const storage, const std::uint32_t ignoredStackValue)
{
    // 原唯一ret4，无字段写入/返回值合同；不是未知入口的空占位。
}

template LegacyStreamPosition* DefaultLegacySeekOffset_5498B7<char>(
    LegacyNarrowStreamBuffer*, LegacyStreamPosition*, std::uint32_t, std::uint32_t, std::uint32_t);
template LegacyStreamPosition* DefaultLegacySeekOffset_5498B7<std::uint16_t>(
    LegacyWideStreamBuffer*, LegacyStreamPosition*, std::uint32_t, std::uint32_t, std::uint32_t);
template LegacyStreamPosition* DefaultLegacySeekPosition_5498EE<char>(
    LegacyNarrowStreamBuffer*, LegacyStreamPosition*, LegacyStreamPosition, std::uint32_t);
template LegacyStreamPosition* DefaultLegacySeekPosition_5498EE<std::uint16_t>(
    LegacyWideStreamBuffer*, LegacyStreamPosition*, LegacyStreamPosition, std::uint32_t);
template void DefaultLegacyStreamNoOp_5487D0<char>(
    LegacyNarrowStreamBuffer*, std::uint32_t);
template void DefaultLegacyStreamNoOp_5487D0<std::uint16_t>(
    LegacyWideStreamBuffer*, std::uint32_t);

void DestroyLegacyNarrowStreamBuffer_5486B1(LegacyNarrowStreamBufferObject* const storage)
{
    storage->buffer.virtualTable = &gLegacyNarrowStreamBufferMethods55000C;
    DestroyLegacyLocale_54867D(&storage->localeImplementation);
}

void DestroyLegacyWideStreamBuffer_54976A(LegacyWideStreamBufferObject* const storage)
{
    storage->buffer.virtualTable = &gLegacyWideStreamBufferMethods5500A8;
    DestroyLegacyLocale_54867D(&storage->localeImplementation);
}

LegacyNarrowStreamBufferObject* DeleteLegacyNarrowStreamBuffer_548D31(
    LegacyNarrowStreamBufferObject* const storage, const std::uint32_t flags)
{
    DestroyLegacyNarrowStreamBuffer_5486B1(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

LegacyWideStreamBufferObject* DeleteLegacyWideStreamBuffer_549D86(
    LegacyWideStreamBufferObject* const storage, const std::uint32_t flags)
{
    DestroyLegacyWideStreamBuffer_54976A(storage);
    if ((flags & 1u) != 0) {
        FreeLegacyHeapMemory_53C0A9(storage);
    }
    return storage;
}

std::int32_t ConsumeLegacyNarrowStreamBuffer_5486C9(
    LegacyNarrowStreamBufferObject* const storage)
{
    const auto* const methods = storage->buffer.virtualTable;
    if (methods->underflow(storage) == -1) {
        return -1;
    }
    char* const previous = IncrementLegacyReadCursor_548EB5(&storage->buffer);
    return static_cast<std::uint8_t>(*previous);
}

std::uint16_t ConsumeLegacyWideStreamBuffer_549787(
    LegacyWideStreamBufferObject* const storage)
{
    const auto* const methods = storage->buffer.virtualTable;
    if (methods->underflow(storage) == 0xFFFFu) {
        return 0xFFFFu;
    }
    std::uint16_t* const previous = IncrementLegacyWideReadCursor_549E2D(&storage->buffer);
    return *previous;
}

std::int32_t ReadLegacyNarrowStreamBuffer_5486E6(
    LegacyNarrowStreamBufferObject* const storage, char* const destination,
    std::int32_t requested)
{
    std::int32_t completed = 0;
    if (requested <= 0) {
        return completed;
    }
    char* output = destination;
    do {
        char* const current = *storage->buffer.readCurrentSlot;
        std::int32_t available = 0;
        if (current != nullptr) {
            available = *storage->buffer.readRemainingSlot;
        }
        if (current != nullptr && available > 0) {
            const std::int32_t copied = requested < available ? requested : available;
            CopyLegacyMemory_541620(output, current, static_cast<std::uint32_t>(copied));
            // 原复制后先重新读取余量槽地址，再更新完成/请求、余量和游标。
            std::int32_t* const remainingSlot = storage->buffer.readRemainingSlot;
            completed += copied;
            requested -= copied;
            *remainingSlot -= copied;
            char** const currentSlot = storage->buffer.readCurrentSlot;
            output += copied;
            *currentSlot += copied;
        } else {
            const auto* const methods = storage->buffer.virtualTable;
            const std::int32_t character = methods->uflow(storage);
            if (character == -1) {
                break;
            }
            *output = std::bit_cast<char>(static_cast<std::uint8_t>(character));
            ++output;
            ++completed;
            --requested;
        }
    } while (requested > 0);
    return completed;
}

std::int32_t WriteLegacyNarrowStreamBuffer_54875A(
    LegacyNarrowStreamBufferObject* const storage, const char* const source,
    std::int32_t requested)
{
    std::int32_t completed = 0;
    if (requested <= 0) {
        return completed;
    }
    const char* input = source;
    do {
        char* const current = *storage->buffer.writeCurrentSlot;
        std::int32_t available = 0;
        if (current != nullptr) {
            available = *storage->buffer.writeRemainingSlot;
        }
        if (current != nullptr && available > 0) {
            const std::int32_t copied = requested < available ? requested : available;
            CopyLegacyMemory_541620(current, input, static_cast<std::uint32_t>(copied));
            std::int32_t* const remainingSlot = storage->buffer.writeRemainingSlot;
            completed += copied;
            requested -= copied;
            *remainingSlot -= copied;
            char** const currentSlot = storage->buffer.writeCurrentSlot;
            input += copied;
            *currentSlot += copied;
        } else {
            // 原先从源U8零扩展，再读取当时虚表；不能先缓存整个循环的表。
            const std::uint8_t character = static_cast<std::uint8_t>(*input);
            const auto* const methods = storage->buffer.virtualTable;
            if (methods->overflow(storage, character) == -1) {
                break;
            }
            ++input;
            ++completed;
            --requested;
        }
    } while (requested > 0);
    return completed;
}

std::int32_t ReadLegacyWideStreamBuffer_5497A8(
    LegacyWideStreamBufferObject* const storage, std::uint16_t* const destination,
    std::int32_t requested)
{
    std::int32_t completed = 0;
    if (requested <= 0) {
        return completed;
    }
    std::uint16_t* output = destination;
    do {
        std::uint16_t* const current = *storage->buffer.readCurrentSlot;
        std::int32_t available = 0;
        if (current != nullptr) {
            // 原shl1/sar1的S32位型折返，不替换为原余量或容量钳制。
            available = std::bit_cast<std::int32_t>(
                std::bit_cast<std::uint32_t>(*storage->buffer.readRemainingSlot) << 1) >> 1;
        }
        if (current != nullptr && available > 0) {
            const std::int32_t copied = requested < available ? requested : available;
            std::uint16_t* targetCursor = output;
            const std::uint16_t* sourceCursor = current;
            for (std::int32_t left = copied; left > 0; --left) {
                const std::uint16_t character = *sourceCursor;
                *targetCursor = character;
                ++targetCursor;
                ++sourceCursor;
            }
            completed += copied;
            requested -= copied;
            output += copied;
            BumpLegacyWideReadCursor_549E08(&storage->buffer, copied);
        } else {
            const auto* const methods = storage->buffer.virtualTable;
            const std::uint16_t character = methods->uflow(storage);
            if (character == 0xFFFFu) {
                break;
            }
            *output = character;
            ++output;
            ++completed;
            --requested;
        }
    } while (requested > 0);
    return completed;
}

std::int32_t WriteLegacyWideStreamBuffer_54982F(
    LegacyWideStreamBufferObject* const storage, const std::uint16_t* const source,
    std::int32_t requested)
{
    std::int32_t completed = 0;
    if (requested <= 0) {
        return completed;
    }
    const std::uint16_t* input = source;
    do {
        std::uint16_t* const current = *storage->buffer.writeCurrentSlot;
        std::int32_t available = 0;
        if (current != nullptr) {
            available = std::bit_cast<std::int32_t>(
                std::bit_cast<std::uint32_t>(*storage->buffer.writeRemainingSlot) << 1) >> 1;
        }
        if (current != nullptr && available > 0) {
            const std::int32_t copied = requested < available ? requested : available;
            std::uint16_t* targetCursor = current;
            const std::uint16_t* sourceCursor = input;
            for (std::int32_t left = copied; left > 0; --left) {
                const std::uint16_t character = *sourceCursor;
                *targetCursor = character;
                ++targetCursor;
                ++sourceCursor;
            }
            completed += copied;
            requested -= copied;
            input += copied;
            BumpLegacyWideWriteCursor_549E3D(&storage->buffer, copied);
        } else {
            const std::uint16_t character = *input;
            const auto* const methods = storage->buffer.virtualTable;
            if (methods->overflow(storage, character) == 0xFFFFu) {
                break;
            }
            ++input;
            ++completed;
            --requested;
        }
    } while (requested > 0);
    return completed;
}

namespace {

// 默认原入口的this为96B缓冲首成员；此处仅投影实际104B基类，不复制叶行为。
std::int32_t NarrowCharacterMethod(
    LegacyNarrowStreamBufferObject* const storage, const std::int32_t character)
{
    return DefaultLegacyNarrowCharacter_5486BF(
        &storage->buffer, std::bit_cast<std::uint32_t>(character));
}

std::uint16_t WideCharacterMethod(
    LegacyWideStreamBufferObject* const storage, const std::uint16_t character)
{
    return DefaultLegacyWideCharacter_549778(&storage->buffer, character);
}

std::int32_t NarrowReadMethod(LegacyNarrowStreamBufferObject* const storage)
{
    return DefaultLegacyNarrowRead_5486C5(&storage->buffer);
}

std::uint16_t WideReadMethod(LegacyWideStreamBufferObject* const storage)
{
    return DefaultLegacyWideRead_549782(&storage->buffer);
}

template <typename Character>
std::int32_t ZeroMethod(LegacyStreamBufferObject<Character>* const storage)
{
    return DefaultLegacyStreamZero_54977F(&storage->buffer);
}

template <typename Character>
LegacyStreamPosition* SeekOffsetMethod(
    LegacyStreamBufferObject<Character>* const storage, LegacyStreamPosition* const result,
    const std::uint32_t offsetBits, const std::uint32_t originBits,
    const std::uint32_t modeBits)
{
    return DefaultLegacySeekOffset_5498B7(
        &storage->buffer, result, offsetBits, originBits, modeBits);
}

template <typename Character>
LegacyStreamPosition* SeekPositionMethod(
    LegacyStreamBufferObject<Character>* const storage, LegacyStreamPosition* const result,
    const LegacyStreamPosition position, const std::uint32_t modeBits)
{
    return DefaultLegacySeekPosition_5498EE(&storage->buffer, result, position, modeBits);
}

template <typename Character>
LegacyStreamBufferObject<Character>* SetBufferMethod(
    LegacyStreamBufferObject<Character>* const storage, Character* const buffer,
    const std::uint32_t countBits)
{
    // 真实返回缓冲首成员地址，与所属基类对象地址相同；不解引用/SDK互转。
    return reinterpret_cast<LegacyStreamBufferObject<Character>*>(
        DefaultLegacySetBuffer_549925(&storage->buffer, buffer, countBits));
}

template <typename Character>
void NoOpMethod(
    LegacyStreamBufferObject<Character>* const storage, const std::uint32_t ignoredStackValue)
{
    DefaultLegacyStreamNoOp_5487D0(&storage->buffer, ignoredStackValue);
}

} // namespace

// 原基类两个13槽常量表；仅指针拓宽/真实函数地址，初始化不新增SDK对象或退出登记。
const LegacyNarrowStreamBufferMethods gLegacyNarrowStreamBufferMethods55000C{
    DeleteLegacyNarrowStreamBuffer_548D31,
    NarrowCharacterMethod,
    NarrowCharacterMethod,
    ZeroMethod<char>,
    NarrowReadMethod,
    ConsumeLegacyNarrowStreamBuffer_5486C9,
    ReadLegacyNarrowStreamBuffer_5486E6,
    WriteLegacyNarrowStreamBuffer_54875A,
    SeekOffsetMethod<char>,
    SeekPositionMethod<char>,
    SetBufferMethod<char>,
    ZeroMethod<char>,
    NoOpMethod<char>
};
const LegacyWideStreamBufferMethods gLegacyWideStreamBufferMethods5500A8{
    DeleteLegacyWideStreamBuffer_549D86,
    WideCharacterMethod,
    WideCharacterMethod,
    ZeroMethod<std::uint16_t>,
    WideReadMethod,
    ConsumeLegacyWideStreamBuffer_549787,
    ReadLegacyWideStreamBuffer_5497A8,
    WriteLegacyWideStreamBuffer_54982F,
    SeekOffsetMethod<std::uint16_t>,
    SeekPositionMethod<std::uint16_t>,
    SetBufferMethod<std::uint16_t>,
    ZeroMethod<std::uint16_t>,
    NoOpMethod<std::uint16_t>
};

} // namespace cg::platform
