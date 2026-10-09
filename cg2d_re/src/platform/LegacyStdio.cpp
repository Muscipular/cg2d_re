#include "LegacyStdio.hpp"
#include "LegacyHeap.hpp"
#include "LegacyLowIo.hpp"
#include "LegacyRuntimeError.hpp"
#include "LegacyMultibyte.hpp"
#include "LegacyStringCompareState.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <bit>
#include <cstring>
#include <cstdlib>

namespace cg::platform {

char gLegacyStandardInputBuffer1086AE0[4096] {};
LegacyFile gLegacyStaticFiles58F8B8[20] = {
    {gLegacyStandardInputBuffer1086AE0, 0, gLegacyStandardInputBuffer1086AE0,
        0x101, 0, 0, 0x1000, nullptr},
    {nullptr, 0, nullptr, 2, 1, 0, 0, nullptr},
    {nullptr, 0, nullptr, 2, 2, 0, 0, nullptr}
};
LegacyFile** gLegacyStreamTable1086AC4 = nullptr;
std::int32_t gLegacyStreamLimit1087AE0 = 0;
std::uint32_t gLegacyStreamAllocationCount108603C = 0;
std::uint32_t gLegacyDefaultStreamFlags10862AC = 0;
char* gLegacyTemporaryBuffers1086040[2] {};

namespace {

// SCH01仅stdio索引表清零域；元素地址已拓宽，count/乘积/handler仍W32。
LegacyFile** CallocateLegacyStdioTable_53BFF8(
    const std::uint32_t count, const std::uint32_t elementBytes)
{
    std::uint32_t bytes = count * elementBytes;
    if (bytes <= 0xFFFFFFE0u) {
        if (bytes == 0) {
            bytes = 1;
        }
        bytes = (bytes + 15u) & 0xFFFFFFF0u;
    }
    for (;;) {
        LegacyFile** table = nullptr;
        if (bytes <= 0xFFFFFFE0u) {
            table = static_cast<LegacyFile**>(std::calloc(1u, bytes));
        }
        // 保原成功短路及失败后new-mode/真实处理器读取，不造表容量/状态。
        if (table != nullptr || gLegacyHeapNewMode1086150 == 0) {
            return table;
        }
        if (CallLegacyHeapNewHandler_541A28(bytes) == 0) {
            return nullptr;
        }
    }
}

} // namespace

void InitializeLegacyStdio_53E9A5()
{
    std::int32_t limit = gLegacyStreamLimit1087AE0;
    if (limit == 0) {
        limit = 512;
        gLegacyStreamLimit1087AE0 = limit;
    } else if (limit < 20) {
        limit = 20;
        gLegacyStreamLimit1087AE0 = limit;
    }
    gLegacyStreamTable1086AC4 = static_cast<LegacyFile**>(
        CallocateLegacyStdioTable_53BFF8(static_cast<std::uint32_t>(limit),
            static_cast<std::uint32_t>(sizeof(LegacyFile*))));
    if (gLegacyStreamTable1086AC4 == nullptr) {
        gLegacyStreamLimit1087AE0 = 20;
        gLegacyStreamTable1086AC4 = static_cast<LegacyFile**>(
            CallocateLegacyStdioTable_53BFF8(20,
                static_cast<std::uint32_t>(sizeof(LegacyFile*))));
        if (gLegacyStreamTable1086AC4 == nullptr) {
            ExitLegacyRuntimeError_53DEAB(26);
        }
    }
    for (std::int32_t index = 0; index < 20; ++index) {
        gLegacyStreamTable1086AC4[index] = &gLegacyStaticFiles58F8B8[index];
    }
    for (std::int32_t descriptor = 0; descriptor < 3; ++descriptor) {
        const HANDLE handle =
            gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].handle;
        if (handle == INVALID_HANDLE_VALUE || handle == nullptr) {
            gLegacyStaticFiles58F8B8[descriptor].descriptor = -1;
        }
    }
}

LegacyFile* AcquireLegacyStream_53F848()
{
    const std::int32_t limit = gLegacyStreamLimit1087AE0;
    LegacyFile* stream = nullptr;
    if (limit > 0) {
        auto** const table = gLegacyStreamTable1086AC4;
        for (std::int32_t index = 0; index < limit; ++index) {
            if (table[index] == nullptr) {
                auto* const allocated = static_cast<LegacyFile*>(
                    Malloc_53C3AC(static_cast<std::uint32_t>(sizeof(LegacyFile))));
                gLegacyStreamTable1086AC4[index] = allocated;
                stream = gLegacyStreamTable1086AC4[index];
                break;
            }
            if ((table[index]->flags & 0x83u) == 0) {
                stream = table[index];
                break;
            }
        }
    }
    if (stream != nullptr) {
        stream->descriptor = -1;
        stream->remaining = 0;
        stream->flags = 0;
        stream->base = nullptr;
        stream->current = nullptr;
        stream->temporaryName = nullptr;
    }
    return stream;
}

LegacyFile* OpenLegacyStreamInSlot_53F6D8(const char* const filename,
    const char* const mode, const std::uint32_t shareFlags, LegacyFile* const stream)
{
    std::uint32_t streamFlags = gLegacyDefaultStreamFlags10862AC;
    std::uint32_t openFlags;
    std::int32_t commitSeen = 0;
    std::int32_t hintSeen = 0;
    switch (*mode) {
    case 'r': openFlags = 0; streamFlags |= 1u; break;
    case 'w': openFlags = 0x301; streamFlags |= 2u; break;
    case 'a': openFlags = 0x109; streamFlags |= 2u; break;
    default: return nullptr;
    }
    std::int32_t parsing = 1;
    const char* cursor = mode;
    for (;;) {
        const auto value = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*++cursor));
        // 原先读取下一字节，再检查上一次后缀是否令解析停止。
        if (value == 0 || parsing == 0) {
            break;
        }
        switch (value) {
        case '+':
            if ((openFlags & 2u) != 0) {
                parsing = 0;
            } else {
                openFlags &= ~1u;
                streamFlags &= ~3u;
                openFlags |= 2u;
                streamFlags |= 0x80u;
            }
            break;
        case 'b':
        case 't':
            if ((openFlags & 0xC000u) != 0) {
                parsing = 0;
            } else {
                openFlags |= value == 'b' ? 0x8000u : 0x4000u;
            }
            break;
        case 'c':
        case 'n':
            if (commitSeen != 0) {
                parsing = 0;
            } else {
                commitSeen = 1;
                if (value == 'c') {
                    streamFlags |= 0x4000u;
                } else {
                    streamFlags &= ~0x4000u;
                }
            }
            break;
        case 'D':
            if ((openFlags & 0x40u) != 0) {
                parsing = 0;
            } else {
                openFlags |= 0x40u;
            }
            break;
        case 'T':
            if ((openFlags & 0x1000u) != 0) {
                parsing = 0;
            } else {
                openFlags |= 0x1000u;
            }
            break;
        case 'S':
        case 'R':
            if (hintSeen != 0) {
                parsing = 0;
            } else {
                hintSeen = 1;
                openFlags |= value == 'S' ? 0x20u : 0x10u;
            }
            break;
        default:
            parsing = 0;
            break;
        }
    }
    const std::int32_t descriptor =
        OpenSharedLegacyDescriptor_53D44C(filename, openFlags, shareFlags, 0x1A4);
    if (descriptor < 0) {
        return nullptr;
    }
    ++gLegacyStreamAllocationCount108603C;
    stream->flags = streamFlags;
    stream->remaining = 0;
    stream->current = nullptr;
    stream->base = nullptr;
    stream->temporaryName = nullptr;
    stream->descriptor = descriptor;
    return stream;
}

LegacyFile* OpenSharedLegacyStream_53B2D1(const char* const filename,
    const char* const mode, const std::uint32_t shareFlags)
{
    LegacyFile* const stream = AcquireLegacyStream_53F848();
    if (stream == nullptr) {
        return nullptr;
    }
    return OpenLegacyStreamInSlot_53F6D8(filename, mode, shareFlags, stream);
}

LegacyFile* OpenLegacyStream_53B2F1(const char* const filename, const char* const mode)
{
    return OpenSharedLegacyStream_53B2D1(filename, mode, 0x40);
}

char* AllocateLegacyStreamBuffer_544815(LegacyFile* const stream)
{
    ++gLegacyStreamAllocationCount108603C;
    stream->base = static_cast<char*>(Malloc_53C3AC(4096));
    if (stream->base != nullptr) {
        stream->flags |= 8u;
        stream->bufferSize = 4096;
    } else {
        stream->flags |= 4u;
        stream->base = reinterpret_cast<char*>(&stream->characterBuffer);
        stream->bufferSize = 2;
    }
    char* const base = stream->base;
    stream->remaining = 0;
    stream->current = base;
    return base;
}

void FreeLegacyStreamBuffer_53F38F(LegacyFile* const stream)
{
    const std::uint32_t flags = stream->flags;
    if ((flags & 0x83u) != 0 && (flags & 8u) != 0) {
        Free_53C0A9(stream->base);
        stream->flags &= ~0x408u;
        stream->current = nullptr;
        stream->base = nullptr;
        stream->remaining = 0;
    }
}

std::int32_t FillLegacyStreamBuffer_53F3BA(LegacyFile* const stream)
{
    std::uint32_t flags = stream->flags;
    if ((flags & 0x83u) == 0 || (flags & 0x40u) != 0) {
        return -1;
    }
    if ((flags & 2u) != 0) {
        stream->flags = flags | 0x20u;
        return -1;
    }
    flags |= 1u;
    stream->flags = flags;
    if ((flags & 0x10Cu) == 0) {
        static_cast<void>(AllocateLegacyStreamBuffer_544815(stream));
    } else {
        stream->current = stream->base;
    }
    const std::int32_t read = ReadLegacyDescriptor_541426(stream->descriptor,
        stream->base, static_cast<std::uint32_t>(stream->bufferSize));
    stream->remaining = read;
    if (read == 0 || read == -1) {
        stream->flags |= read == 0 ? 0x10u : 0x20u;
        stream->remaining = 0;
        return -1;
    }

    flags = stream->flags;
    if ((flags & 0x82u) == 0) {
        const std::int32_t descriptor = stream->descriptor;
        const auto* const record = descriptor == -1 ? &gLegacyInvalidDescriptor58FB40
            : &gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
        if ((record->flags & 0x82u) == 0x82u) {
            stream->flags = flags | 0x2000u;
        }
    }
    if (stream->bufferSize == 512) {
        const std::uint32_t currentFlags = stream->flags;
        if ((currentFlags & 8u) != 0 && (currentFlags & 0x400u) == 0) {
            stream->bufferSize = 4096;
        }
    }
    char* const current = stream->current;
    stream->remaining = read - 1;
    const auto value = static_cast<std::uint8_t>(*current);
    stream->current = current + 1;
    return value;
}

std::int32_t ReadLegacyStreamByte_53C8B3(LegacyFile* const stream)
{
    --stream->remaining;
    if (stream->remaining < 0) {
        return FillLegacyStreamBuffer_53F3BA(stream);
    }
    char* const current = stream->current;
    const auto value = static_cast<std::uint8_t>(*current);
    stream->current = current + 1;
    return value;
}

char* ReadLegacyStreamLine_53B226(char* const buffer,
    const std::int32_t maxCount, LegacyFile* const stream)
{
    if (maxCount <= 0) {
        return nullptr;
    }
    char* output = buffer;
    std::int32_t remaining = maxCount - 1;
    while (remaining != 0) {
        std::int32_t value;
        --stream->remaining;
        if (stream->remaining < 0) {
            value = FillLegacyStreamBuffer_53F3BA(stream);
        } else {
            char* const current = stream->current;
            value = static_cast<std::uint8_t>(*current);
            stream->current = current + 1;
        }
        if (value == -1) {
            if (output == buffer) {
                return nullptr;
            }
            break;
        }
        *output++ = std::bit_cast<char>(static_cast<std::uint8_t>(value));
        if (static_cast<std::uint8_t>(value) == '\n') {
            break;
        }
        --remaining;
    }
    *output = '\0';
    return buffer;
}

std::int32_t FlushAndWriteLegacyStreamByte_53F8C0(
    const std::int32_t character, LegacyFile* const stream)
{
    std::uint32_t flags = stream->flags;
    if ((flags & 0x82u) == 0 || (flags & 0x40u) != 0) {
        stream->flags = flags | 0x20u;
        return -1;
    }
    // 拒绝路径不消费fd；原格式化临时FILE并未初始化该字段。
    const std::int32_t descriptor = stream->descriptor;
    if ((flags & 1u) != 0) {
        stream->remaining = 0;
        if ((flags & 0x10u) == 0) {
            stream->flags = flags | 0x20u;
            return -1;
        }
        stream->current = stream->base;
        stream->flags = flags & ~1u;
    }
    flags = stream->flags;
    stream->remaining = 0;
    std::int32_t written = 0;
    flags = (flags & ~0x10u) | 2u;
    stream->flags = flags;
    if ((flags & 0x10Cu) == 0) {
        if ((stream != &gLegacyStaticFiles58F8B8[1]
                && stream != &gLegacyStaticFiles58F8B8[2])
            || IsLegacyDescriptorTty_544705(descriptor) == 0) {
            static_cast<void>(AllocateLegacyStreamBuffer_544815(stream));
        }
    }

    std::int32_t expected;
    if ((stream->flags & 0x108u) != 0) {
        char* const base = stream->base;
        expected = static_cast<std::int32_t>(stream->current - base);
        stream->current = base + 1;
        stream->remaining = stream->bufferSize - 1;
        if (expected > 0) {
            written = WriteLegacyDescriptor_541279(descriptor, base,
                static_cast<std::uint32_t>(expected));
        } else {
            const auto* const record = descriptor == -1 ? &gLegacyInvalidDescriptor58FB40
                : &gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
            if ((record->flags & 0x20u) != 0) {
                static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, 0, 2));
            }
        }
        // 原先重读base存新字节，再判断旧缓冲写出是否成功。
        *stream->base = std::bit_cast<char>(static_cast<std::uint8_t>(character));
    } else {
        expected = 1;
        written = WriteLegacyDescriptor_541279(descriptor, &character, 1);
    }
    if (written != expected) {
        stream->flags |= 0x20u;
        return -1;
    }
    return static_cast<std::uint8_t>(character);
}

std::int32_t WriteLegacyStreamByte_53CFDC(
    const std::int32_t character, LegacyFile* const stream)
{
    --stream->remaining;
    if (stream->remaining < 0) {
        return FlushAndWriteLegacyStreamByte_53F8C0(character, stream);
    }
    char* const current = stream->current;
    const auto value = static_cast<std::uint8_t>(character);
    *current = std::bit_cast<char>(value);
    ++stream->current;
    return value;
}

std::uint32_t ReadLegacyStream_53BCCD(void* const buffer,
    const std::uint32_t elementSize, const std::uint32_t elementCount,
    LegacyFile* const stream)
{
    const std::uint32_t total = elementSize * elementCount;
    if (total == 0) {
        return 0;
    }
    auto* output = static_cast<char*>(buffer);
    std::uint32_t remaining = total;
    std::uint32_t blockSize = (stream->flags & 0x10Cu) != 0
        ? static_cast<std::uint32_t>(stream->bufferSize) : 4096u;
    do {
        if ((stream->flags & 0x10Cu) != 0 && stream->remaining != 0) {
            const auto available = static_cast<std::uint32_t>(stream->remaining);
            const std::uint32_t count = remaining < available ? remaining : available;
            static_cast<void>(CopyLegacyMemory_541620(output, stream->current, count));
            remaining -= count;
            stream->remaining = std::bit_cast<std::int32_t>(
                static_cast<std::uint32_t>(stream->remaining) - count);
            stream->current += count;
            output += count;
        } else if (remaining >= blockSize) {
            const std::uint32_t count = blockSize == 0
                ? remaining : remaining - remaining % blockSize;
            const std::int32_t read =
                ReadLegacyDescriptor_541426(stream->descriptor, output, count);
            if (read == 0 || read == -1) {
                stream->flags |= read == 0 ? 0x10u : 0x20u;
                return (total - remaining) / elementSize;
            }
            remaining -= static_cast<std::uint32_t>(read);
            output += read;
        } else {
            const std::int32_t value = FillLegacyStreamBuffer_53F3BA(stream);
            if (value == -1) {
                return (total - remaining) / elementSize;
            }
            *output = std::bit_cast<char>(static_cast<std::uint8_t>(value));
            blockSize = static_cast<std::uint32_t>(stream->bufferSize);
            ++output;
            --remaining;
        }
    } while (remaining != 0);
    return elementCount;
}

std::uint32_t WriteLegacyStream_53C680(const void* const buffer,
    const std::uint32_t elementSize, const std::uint32_t elementCount,
    LegacyFile* const stream)
{
    const std::uint32_t total = elementSize * elementCount;
    if (total == 0) {
        return 0;
    }
    const auto* input = static_cast<const char*>(buffer);
    std::uint32_t remaining = total;
    std::uint32_t blockSize = (stream->flags & 0x10Cu) != 0
        ? static_cast<std::uint32_t>(stream->bufferSize) : 4096u;
    do {
        const std::uint32_t buffered = stream->flags & 0x108u;
        if (buffered != 0 && stream->remaining != 0) {
            const auto available = static_cast<std::uint32_t>(stream->remaining);
            const std::uint32_t count = remaining < available ? remaining : available;
            static_cast<void>(CopyLegacyMemory_541620(stream->current, input, count));
            stream->remaining = std::bit_cast<std::int32_t>(
                static_cast<std::uint32_t>(stream->remaining) - count);
            stream->current += count;
            remaining -= count;
            input += count;
        } else if (remaining >= blockSize) {
            if (buffered != 0 && FlushLegacyStreamBuffer_53BB3D(stream) != 0) {
                return (total - remaining) / elementSize;
            }
            const std::uint32_t count = blockSize == 0
                ? remaining : remaining - remaining % blockSize;
            const std::int32_t written =
                WriteLegacyDescriptor_541279(stream->descriptor, input, count);
            if (written == -1) {
                stream->flags |= 0x20u;
                return (total - remaining) / elementSize;
            }
            input += written;
            remaining -= static_cast<std::uint32_t>(written);
            if (static_cast<std::uint32_t>(written) < count) {
                stream->flags |= 0x20u;
                return (total - remaining) / elementSize;
            }
        } else {
            const auto character =
                std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*input));
            if (FlushAndWriteLegacyStreamByte_53F8C0(character, stream) == -1) {
                return (total - remaining) / elementSize;
            }
            ++input;
            const std::int32_t size = stream->bufferSize;
            --remaining;
            blockSize = size > 0 ? static_cast<std::uint32_t>(size) : 1u;
        }
    } while (remaining != 0);
    return elementCount;
}

std::int32_t BeginLegacyTemporaryBuffer_53EA5E(LegacyFile* const stream)
{
    if (IsLegacyDescriptorTty_544705(stream->descriptor) == 0) {
        return 0;
    }
    std::uint32_t index;
    if (stream == &gLegacyStaticFiles58F8B8[1]) {
        index = 0;
    } else if (stream == &gLegacyStaticFiles58F8B8[2]) {
        index = 1;
    } else {
        return 0;
    }
    ++gLegacyStreamAllocationCount108603C;
    if ((stream->flags & 0x10Cu) != 0) {
        return 0;
    }
    char** const slot = &gLegacyTemporaryBuffers1086040[index];
    if (*slot == nullptr) {
        char* const allocated = static_cast<char*>(Malloc_53C3AC(4096));
        *slot = allocated;
        if (allocated == nullptr) {
            char* const buffer = reinterpret_cast<char*>(&stream->characterBuffer);
            stream->base = buffer;
            stream->current = buffer;
            stream->bufferSize = 2;
            stream->remaining = 2;
            stream->flags |= 0x1102u;
            return 1;
        }
    }
    char* const buffer = *slot;
    stream->bufferSize = 4096;
    stream->base = buffer;
    stream->current = buffer;
    stream->remaining = 4096;
    stream->flags |= 0x1102u;
    return 1;
}

void EndLegacyTemporaryBuffer_53EAEB(
    const std::int32_t started, LegacyFile* const stream)
{
    if ((stream->flags & 0x1000u) != 0) {
        static_cast<void>(FlushLegacyStreamBuffer_53BB3D(stream));
        if (started != 0) {
            stream->flags &= ~0x1100u;
            stream->bufferSize = 0;
            stream->current = nullptr;
            stream->base = nullptr;
        }
    }
}

std::int32_t WriteLegacyStreamString_53B356(
    const char* const text, LegacyFile* const stream)
{
    const auto length = static_cast<std::uint32_t>(StrLen_53F9E0(text));
    const std::int32_t started = BeginLegacyTemporaryBuffer_53EA5E(stream);
    const std::uint32_t written = WriteLegacyStream_53C680(text, 1, length, stream);
    EndLegacyTemporaryBuffer_53EAEB(started, stream);
    return written == length ? 0 : -1;
}

std::int32_t TellLegacyStream_53CB78(LegacyFile* const stream)
{
    const std::int32_t descriptor = stream->descriptor;
    if (stream->remaining < 0) {
        stream->remaining = 0;
    }
    std::int32_t position = SeekLegacyDescriptor_53F493(descriptor, 0, 1);
    if (position < 0) {
        return -1;
    }
    const std::uint32_t flags = stream->flags;
    if ((flags & 0x108u) == 0) {
        return position - stream->remaining;
    }

    char* const current = stream->current;
    char* const base = stream->base;
    std::int32_t consumed = static_cast<std::int32_t>(current - base);
    if ((flags & 3u) != 0) {
        if ((gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags
                & 0x80u) != 0) {
            for (const char* cursor = base;
                 reinterpret_cast<std::uintptr_t>(cursor)
                    < reinterpret_cast<std::uintptr_t>(current); ++cursor) {
                if (*cursor == '\n') {
                    ++consumed;
                }
            }
        }
    } else if ((flags & 0x80u) == 0) {
        LegacyErrno_1085FE0() = 22;
        return -1;
    }
    if (position == 0) {
        return consumed;
    }
    if ((stream->flags & 1u) != 0) {
        const std::int32_t remaining = stream->remaining;
        if (remaining == 0) {
            consumed = 0;
        } else {
            std::int32_t physicalSize =
                static_cast<std::int32_t>(current - base) + remaining;
            // 保存组槽地址；系统定位之后重新读取组指针和该fd的标志。
            auto** const block = &gLegacyDescriptorBlocks10869C0[descriptor >> 5];
            const std::int32_t slot = descriptor & 31;
            if (((*block)[slot].flags & 0x80u) != 0) {
                if (SeekLegacyDescriptor_53F493(descriptor, 0, 2) == position) {
                    const char* cursor = stream->base;
                    const char* const end = cursor + physicalSize;
                    while (reinterpret_cast<std::uintptr_t>(cursor)
                        < reinterpret_cast<std::uintptr_t>(end)) {
                        if (*cursor == '\n') {
                            ++physicalSize;
                        }
                        ++cursor;
                    }
                    if ((stream->flags & 0x2000u) != 0) {
                        ++physicalSize;
                    }
                } else {
                    static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, position, 0));
                    if (static_cast<std::uint32_t>(physicalSize) <= 512u
                        && (stream->flags & 8u) != 0
                        && (stream->flags & 0x400u) == 0) {
                        physicalSize = 512;
                    } else {
                        physicalSize = stream->bufferSize;
                    }
                    if (((*block)[slot].flags & 4u) != 0) {
                        ++physicalSize;
                    }
                }
            }
            position -= physicalSize;
        }
    }
    return consumed + position;
}

std::int32_t SeekLegacyStream_53BC41(LegacyFile* const stream,
    std::int32_t offset, std::uint32_t origin)
{
    std::uint32_t flags = stream->flags;
    if ((flags & 0x83u) == 0 || (origin != 0 && origin != 1 && origin != 2)) {
        LegacyErrno_1085FE0() = 22;
        return -1;
    }
    stream->flags = flags & ~0x10u;
    if (origin == 1) {
        offset += TellLegacyStream_53CB78(stream);
        origin = 0;
    }
    static_cast<void>(FlushLegacyStreamBuffer_53BB3D(stream));
    flags = stream->flags;
    if ((flags & 0x80u) != 0) {
        stream->flags = flags & ~3u;
    } else if ((flags & 1u) != 0 && (flags & 8u) != 0 && (flags & 0x400u) == 0) {
        stream->bufferSize = 512;
    }
    return SeekLegacyDescriptor_53F493(stream->descriptor, offset, origin) == -1 ? -1 : 0;
}

void RewindLegacyStream_53B27D(LegacyFile* const stream)
{
    const std::int32_t descriptor = stream->descriptor;
    static_cast<void>(FlushLegacyStreamBuffer_53BB3D(stream));
    stream->flags &= ~0x30u;
    auto* const record = descriptor == -1 ? &gLegacyInvalidDescriptor58FB40
        : &gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
    record->flags &= static_cast<std::uint8_t>(~2u);
    const std::uint32_t flags = stream->flags;
    if ((flags & 0x80u) != 0) {
        stream->flags = flags & ~3u;
    }
    static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, 0, 0));
}

std::int32_t SetLegacyStreamBuffer_54ACF1(LegacyFile* const stream,
    char* buffer, const std::uint32_t mode, std::uint32_t size)
{
    if (mode != 4 && (size < 2 || size > 0x7FFFFFFFu || (mode != 0 && mode != 0x40))) {
        return -1;
    }
    size &= ~1u;
    static_cast<void>(FlushLegacyStreamBuffer_53BB3D(stream));
    FreeLegacyStreamBuffer_53F38F(stream);
    stream->flags &= 0xFFFFC2F3u;
    const std::uint32_t flags = stream->flags;
    if ((mode & 4u) != 0) {
        stream->flags = flags | 4u;
        buffer = reinterpret_cast<char*>(&stream->characterBuffer);
        size = 2;
    } else if (buffer == nullptr) {
        buffer = static_cast<char*>(Malloc_53C3AC(size));
        if (buffer == nullptr) {
            ++gLegacyStreamAllocationCount108603C;
            return -1;
        }
        stream->flags |= 0x408u;
    } else {
        stream->flags = flags | 0x500u;
    }
    stream->remaining = 0;
    stream->bufferSize = static_cast<std::int32_t>(size);
    stream->base = buffer;
    stream->current = buffer;
    return 0;
}

std::int32_t UnreadLegacyStreamByte_54463F(
    const std::int32_t character, LegacyFile* const stream)
{
    if (character == -1) {
        return -1;
    }
    std::uint32_t flags = stream->flags;
    if ((flags & 1u) == 0 && ((flags & 0x80u) == 0 || (flags & 2u) != 0)) {
        return -1;
    }
    if (stream->base == nullptr) {
        static_cast<void>(AllocateLegacyStreamBuffer_544815(stream));
    }
    char* current = stream->current;
    if (current == stream->base) {
        if (stream->remaining != 0) {
            return -1;
        }
        stream->current = ++current;
    }
    const auto value = static_cast<std::uint8_t>(character);
    if ((stream->flags & 0x40u) != 0) {
        --stream->current;
        current = stream->current;
        if (static_cast<std::uint8_t>(*current) != value) {
            stream->current = current + 1;
            return -1;
        }
    } else {
        --stream->current;
        current = stream->current;
        *current = std::bit_cast<char>(value);
    }
    // 原先采样flags，再加count，最后写回方向/EOF位。
    flags = stream->flags;
    ++stream->remaining;
    stream->flags = (flags & ~0x10u) | 1u;
    return value;
}

std::int64_t TellLegacyStream64_54B239(LegacyFile* const stream)
{
    const std::int32_t descriptor = stream->descriptor;
    if (stream->remaining < 0) {
        stream->remaining = 0;
    }
    std::int64_t position = SeekLegacyDescriptor64_54B658(descriptor, 0, 1);
    if (position < 0) {
        return -1;
    }
    if ((stream->flags & 0x108u) == 0) {
        return position - static_cast<std::int64_t>(stream->remaining);
    }

    char* const current = stream->current;
    char* const base = stream->base;
    std::uint32_t consumed = static_cast<std::uint32_t>(current - base);
    const std::uint32_t flags = stream->flags;
    if ((flags & 3u) != 0) {
        if ((gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags
                & 0x80u) != 0) {
            for (const char* cursor = base;
                 reinterpret_cast<std::uintptr_t>(cursor)
                    < reinterpret_cast<std::uintptr_t>(current); ++cursor) {
                if (*cursor == '\n') {
                    ++consumed;
                }
            }
        }
    } else if ((flags & 0x80u) == 0) {
        LegacyErrno_1085FE0() = 22;
        return -1;
    }
    if (position == 0) {
        return consumed;
    }
    if ((stream->flags & 1u) != 0) {
        const std::int32_t remaining = stream->remaining;
        if (remaining == 0) {
            consumed = 0;
        } else {
            std::uint32_t physicalSize = static_cast<std::uint32_t>(current - base)
                + static_cast<std::uint32_t>(remaining);
            auto** const block = &gLegacyDescriptorBlocks10869C0[descriptor >> 5];
            const std::int32_t slot = descriptor & 31;
            if (((*block)[slot].flags & 0x80u) != 0) {
                if (SeekLegacyDescriptor64_54B658(descriptor, 0, 2) == position) {
                    const char* cursor = stream->base;
                    const char* const end = cursor + physicalSize;
                    while (reinterpret_cast<std::uintptr_t>(cursor)
                        < reinterpret_cast<std::uintptr_t>(end)) {
                        if (*cursor == '\n') {
                            ++physicalSize;
                        }
                        ++cursor;
                    }
                    if ((stream->flags & 0x2000u) != 0) {
                        ++physicalSize;
                    }
                } else {
                    static_cast<void>(SeekLegacyDescriptor64_54B658(descriptor, position, 0));
                    if (physicalSize <= 512u && (stream->flags & 8u) != 0
                        && (stream->flags & 0x400u) == 0) {
                        physicalSize = 512;
                    } else {
                        physicalSize = static_cast<std::uint32_t>(stream->bufferSize);
                    }
                    if (((*block)[slot].flags & 4u) != 0) {
                        ++physicalSize;
                    }
                }
            }
            // 原sbb高W32只扣借位：物理块长零扩展，不能先转S32。
            position -= static_cast<std::int64_t>(physicalSize);
        }
    }
    // 原adc高W32使用0，已消费量也按U32零扩展。
    return static_cast<std::int64_t>(consumed) + position;
}

std::int32_t SeekLegacyStream64_54B3C0(LegacyFile* const stream,
    std::int64_t offset, std::uint32_t origin)
{
    std::uint32_t flags = stream->flags;
    if ((flags & 0x83u) == 0 || (origin != 0 && origin != 1 && origin != 2)) {
        LegacyErrno_1085FE0() = 22;
        return -1;
    }
    stream->flags = flags & ~0x10u;
    if (origin == 1) {
        offset += TellLegacyStream64_54B239(stream);
        origin = 0;
    }
    static_cast<void>(FlushLegacyStreamBuffer_53BB3D(stream));
    flags = stream->flags;
    if ((flags & 0x80u) != 0) {
        stream->flags = flags & ~3u;
    } else if ((flags & 1u) != 0 && (flags & 8u) != 0 && (flags & 0x400u) == 0) {
        stream->bufferSize = 512;
    }
    return SeekLegacyDescriptor64_54B658(stream->descriptor, offset, origin) == -1 ? -1 : 0;
}

std::int32_t GetLegacyStreamPosition_54ACB7(
    LegacyFile* const stream, LegacyFilePosition* const position)
{
    const auto value = static_cast<std::uint64_t>(TellLegacyStream64_54B239(stream));
    position->low = static_cast<std::uint32_t>(value);
    position->high = static_cast<std::uint32_t>(value >> 32);
    // 原写回两半之后才重读并判断，包括查询失败时的位置发布。
    return (position->low & position->high) == 0xFFFFFFFFu ? -1 : 0;
}

std::int32_t SetLegacyStreamPosition_54ACD9(
    LegacyFile* const stream, const LegacyFilePosition* const position)
{
    const std::uint32_t high = position->high;
    const std::uint32_t low = position->low;
    const std::uint64_t value = (static_cast<std::uint64_t>(high) << 32) | low;
    return SeekLegacyStream64_54B3C0(stream, std::bit_cast<std::int64_t>(value), 0);
}

std::uint16_t FlushAndWriteLegacyStreamWide_54B450(
    const std::uint16_t character, LegacyFile* const stream)
{
    std::uint32_t flags = stream->flags;
    const std::int32_t descriptor = stream->descriptor;
    if ((flags & 0x82u) == 0 || (flags & 0x40u) != 0) {
        stream->flags = flags | 0x20u;
        return 0xFFFF;
    }
    if ((flags & 1u) != 0) {
        stream->remaining = 0;
        if ((flags & 0x10u) == 0) {
            stream->flags = flags | 0x20u;
            return 0xFFFF;
        }
        stream->current = stream->base;
        stream->flags = flags & ~1u;
    }
    flags = stream->flags;
    stream->remaining = 0;
    std::int32_t written = 0;
    flags = (flags & ~0x10u) | 2u;
    stream->flags = flags;
    if ((flags & 0x10Cu) == 0) {
        if ((stream != &gLegacyStaticFiles58F8B8[1]
                && stream != &gLegacyStaticFiles58F8B8[2])
            || IsLegacyDescriptorTty_544705(descriptor) == 0) {
            static_cast<void>(AllocateLegacyStreamBuffer_544815(stream));
        }
    }
    std::int32_t expected;
    if ((stream->flags & 0x108u) != 0) {
        char* const base = stream->base;
        expected = static_cast<std::int32_t>(stream->current - base);
        stream->current = base + 2;
        stream->remaining = stream->bufferSize - 2;
        if (expected > 0) {
            written = WriteLegacyDescriptor_541279(descriptor, base,
                static_cast<std::uint32_t>(expected));
        } else {
            const auto* const record = descriptor == -1 ? &gLegacyInvalidDescriptor58FB40
                : &gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
            if ((record->flags & 0x20u) != 0) {
                static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, 0, 2));
            }
        }
        std::memcpy(stream->base, &character, sizeof(character));
    } else {
        expected = 2;
        written = WriteLegacyDescriptor_541279(descriptor, &character, 2);
    }
    if (written != expected) {
        stream->flags |= 0x20u;
        return 0xFFFF;
    }
    return character;
}

std::uint16_t FillLegacyStreamWideBuffer_54B574(LegacyFile* const stream)
{
    std::uint32_t flags = stream->flags;
    if ((flags & 0x83u) == 0 || (flags & 0x40u) != 0) {
        return 0xFFFF;
    }
    if ((flags & 2u) != 0) {
        stream->flags = flags | 0x20u;
        return 0xFFFF;
    }
    flags |= 1u;
    stream->flags = flags;
    if ((flags & 0x10Cu) == 0) {
        static_cast<void>(AllocateLegacyStreamBuffer_544815(stream));
    } else {
        stream->current = stream->base;
    }
    const std::int32_t read = ReadLegacyDescriptor_541426(stream->descriptor,
        stream->base, static_cast<std::uint32_t>(stream->bufferSize));
    stream->remaining = read;
    if (read == 0 || read == 1 || read == -1) {
        stream->flags |= read == 0 ? 0x10u : 0x20u;
        stream->remaining = 0;
        return 0xFFFF;
    }
    flags = stream->flags;
    if ((flags & 0x82u) == 0) {
        const std::int32_t descriptor = stream->descriptor;
        const auto* const record = descriptor == -1 ? &gLegacyInvalidDescriptor58FB40
            : &gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
        if ((record->flags & 0x82u) == 0x82u) {
            stream->flags = flags | 0x2000u;
        }
    }
    if (stream->bufferSize == 512) {
        const std::uint32_t currentFlags = stream->flags;
        if ((currentFlags & 8u) != 0 && (currentFlags & 0x400u) == 0) {
            stream->bufferSize = 4096;
        }
    }
    char* const current = stream->current;
    stream->remaining = read - 2;
    std::uint16_t value;
    std::memcpy(&value, current, sizeof(value));
    stream->current = current + 2;
    return value;
}

std::uint16_t WriteLegacyStreamWide_54AD92(
    const std::uint16_t character, LegacyFile* const stream)
{
    if ((stream->flags & 0x40u) == 0) {
        const std::int32_t descriptor = stream->descriptor;
        const auto* const record = descriptor == -1 ? &gLegacyInvalidDescriptor58FB40
            : &gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
        if ((record->flags & 0x80u) != 0) {
            char bytes[4];
            const std::int32_t count = Wctomb_54472B(bytes, character);
            if (count == -1) {
                LegacyErrno_1085FE0() = 42;
                return 0xFFFF;
            }
            // 原非1成功结果只写前两字节，不按转换返回数量循环。
            const std::int32_t outputCount = count == 1 ? 1 : 2;
            for (std::int32_t index = 0; index < outputCount; ++index) {
                --stream->remaining;
                if (stream->remaining < 0) {
                    const auto value =
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(bytes[index]));
                    if (FlushAndWriteLegacyStreamByte_53F8C0(value, stream) == -1) {
                        return 0xFFFF;
                    }
                } else {
                    char* const current = stream->current;
                    *current = bytes[index];
                    ++stream->current;
                }
            }
            return character;
        }
    }
    stream->remaining -= 2;
    if (stream->remaining < 0) {
        return FlushAndWriteLegacyStreamWide_54B450(character, stream);
    }
    char* const current = stream->current;
    std::memcpy(current, &character, sizeof(character));
    stream->current += 2;
    return character;
}

std::uint16_t ReadLegacyStreamWide_54AF70(LegacyFile* const stream)
{
    if ((stream->flags & 0x40u) == 0) {
        const std::int32_t descriptor = stream->descriptor;
        const auto* const record = descriptor == -1 ? &gLegacyInvalidDescriptor58FB40
            : &gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
        if ((record->flags & 0x80u) != 0) {
            char bytes[4];
            const std::int32_t first = ReadLegacyStreamByte_53C8B3(stream);
            if (first == -1) {
                return 0xFFFF;
            }
            bytes[0] = std::bit_cast<char>(static_cast<std::uint8_t>(first));
            std::uint32_t count = 1;
            if ((gCtypePointer58FC30[static_cast<std::uint8_t>(first)] & 0x8000u) != 0) {
                const std::int32_t second = ReadLegacyStreamByte_53C8B3(stream);
                if (second == -1) {
                    const auto value =
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(bytes[0]));
                    static_cast<void>(UnreadLegacyStreamByte_54463F(value, stream));
                    return 0xFFFF;
                }
                bytes[1] = std::bit_cast<char>(static_cast<std::uint8_t>(second));
                count = 2;
            }
            // 原输出借stream参数槽+2；保留转换未写目标时的那两个表示字节。
            wchar_t converted = static_cast<wchar_t>(
                static_cast<std::uint16_t>(reinterpret_cast<std::uintptr_t>(stream) >> 16));
            if (Mbtowc_544552(&converted, bytes, count) == -1) {
                LegacyErrno_1085FE0() = 42;
                return 0xFFFF;
            }
            return static_cast<std::uint16_t>(converted);
        }
    }
    stream->remaining -= 2;
    if (stream->remaining < 0) {
        return FillLegacyStreamWideBuffer_54B574(stream);
    }
    char* const current = stream->current;
    std::uint16_t value;
    std::memcpy(&value, current, sizeof(value));
    stream->current = current + 2;
    return value;
}

std::uint16_t UnreadLegacyStreamWide_54AE87(
    const std::uint16_t character, LegacyFile* const stream)
{
    if (character == 0xFFFF) {
        return 0xFFFF;
    }
    std::uint32_t flags = stream->flags;
    if ((flags & 1u) == 0 && ((flags & 0x80u) == 0 || (flags & 2u) != 0)) {
        return 0xFFFF;
    }
    if (stream->base == nullptr) {
        static_cast<void>(AllocateLegacyStreamBuffer_544815(stream));
    }
    flags = stream->flags;
    if ((flags & 0x40u) == 0) {
        const std::int32_t descriptor = stream->descriptor;
        const auto* const record = descriptor == -1 ? &gLegacyInvalidDescriptor58FB40
            : &gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
        if ((record->flags & 0x80u) != 0) {
            char bytes[4];
            const std::int32_t count = Wctomb_54472B(bytes, character);
            if (count == -1) {
                LegacyErrno_1085FE0() = 42;
                return 0xFFFF;
            }
            char* const current = stream->current;
            if (current == stream->base) {
                if (stream->remaining != 0) {
                    return 0xFFFF;
                }
                stream->current = current + count;
            }
            if (count != 1) {
                --stream->current;
                *stream->current = bytes[1];
            }
            --stream->current;
            *stream->current = bytes[0];
            stream->remaining += count;
            flags = stream->flags;
            stream->flags = (flags & ~0x10u) | 1u;
            return character;
        }
    }
    char* const current = stream->current;
    if (current == stream->base) {
        if (stream->remaining != 0) {
            return 0xFFFF;
        }
        stream->current = current + 2;
    }
    stream->remaining += 2;
    stream->current -= 2;
    stream->flags = (flags & ~0x10u) | 1u;
    std::memcpy(stream->current, &character, sizeof(character));
    std::uint16_t value;
    std::memcpy(&value, stream->current, sizeof(value));
    return value;
}

void WriteLegacyFormatByte_53F2C6(const std::int32_t character,
    LegacyFile* const stream, std::int32_t* const written)
{
    if (WriteLegacyStreamByte_53CFDC(character, stream) == -1) {
        *written = -1;
    } else {
        ++*written;
    }
}

void RepeatLegacyFormatByte_53F2FB(const std::int32_t character,
    std::int32_t count, LegacyFile* const stream, std::int32_t* const written)
{
    while (count > 0) {
        --count;
        WriteLegacyFormatByte_53F2C6(character, stream, written);
        if (*written == -1) {
            break;
        }
    }
}

void WriteLegacyFormatString_53F32C(const char* source,
    std::int32_t count, LegacyFile* const stream, std::int32_t* const written)
{
    while (count > 0) {
        --count;
        const auto character =
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(*source));
        ++source;
        WriteLegacyFormatByte_53F2C6(character, stream, written);
        if (*written == -1) {
            break;
        }
    }
}

std::int32_t ReadLegacyFormatByte_53E950(LegacyFile* const stream)
{
    return ReadLegacyStreamByte_53C8B3(stream);
}

void UnreadLegacyFormatByte_53E96A(
    const std::int32_t character, LegacyFile* const stream)
{
    if (character != -1) {
        static_cast<void>(UnreadLegacyStreamByte_54463F(character, stream));
    }
}

std::int32_t SkipLegacyFormatSpace_53E981(
    std::int32_t* const consumed, LegacyFile* const stream)
{
    std::int32_t character;
    do {
        ++*consumed;
        character = ReadLegacyFormatByte_53E950(stream);
    } while (ClassifyLegacySpace_53BDEF(character) != 0);
    return character;
}

std::int32_t FlushLegacyStreamBuffer_53BB3D(LegacyFile* const stream)
{
    std::int32_t result = 0;
    const std::uint32_t flags = stream->flags;
    if ((flags & 3u) == 2 && (flags & 0x108u) != 0) {
        char* const base = stream->base;
        const auto count = static_cast<std::int32_t>(stream->current - base);
        if (count > 0) {
            if (WriteLegacyDescriptor_541279(stream->descriptor, base,
                    static_cast<std::uint32_t>(count)) == count) {
                if ((stream->flags & 0x80u) != 0) {
                    stream->flags &= ~2u;
                }
            } else {
                stream->flags |= 0x20u;
                result = -1;
            }
        }
    }
    char* const base = stream->base;
    stream->remaining = 0;
    stream->current = base;
    return result;
}

std::int32_t FlushLegacyStream_53BB02(LegacyFile* const stream)
{
    if (stream == nullptr) {
        return FlushLegacyStreams_53BBA2(0);
    }
    if (FlushLegacyStreamBuffer_53BB3D(stream) != 0) {
        return -1;
    }
    if ((stream->flags & 0x4000u) != 0) {
        return CommitLegacyDescriptor_541222(stream->descriptor) != 0 ? -1 : 0;
    }
    return 0;
}

std::int32_t CloseLegacyStream_53B1D0(LegacyFile* const stream)
{
    std::int32_t result = -1;
    const std::uint32_t flags = stream->flags;
    if ((flags & 0x40u) == 0 && (flags & 0x83u) != 0) {
        result = FlushLegacyStreamBuffer_53BB3D(stream);
        FreeLegacyStreamBuffer_53F38F(stream);
        if (CloseLegacyDescriptor_53D30A(stream->descriptor) < 0) {
            result = -1;
        } else if (stream->temporaryName != nullptr) {
            Free_53C0A9(stream->temporaryName);
            stream->temporaryName = nullptr;
        }
    }
    stream->flags = 0;
    return result;
}

std::int32_t FlushLegacyStreams_53BBA2(const std::int32_t mode)
{
    std::int32_t successes = 0;
    std::int32_t error = 0;
    for (std::int32_t index = 0; index < gLegacyStreamLimit1087AE0; ++index) {
        LegacyFile* const stream = gLegacyStreamTable1086AC4[index];
        if (stream == nullptr) {
            continue;
        }
        const std::uint32_t flags = stream->flags;
        if ((flags & 0x83u) == 0) {
            continue;
        }
        if (mode == 1) {
            if (FlushLegacyStream_53BB02(stream) != -1) {
                ++successes;
            }
        } else if (mode == 0 && (flags & 2u) != 0) {
            if (FlushLegacyStream_53BB02(stream) == -1) {
                error = -1;
            }
        }
    }
    return mode == 1 ? successes : error;
}

std::int32_t FlushAllLegacyStreams_53BB99()
{
    return FlushLegacyStreams_53BBA2(1);
}

std::int32_t CloseAllLegacyStreams_5446AD()
{
    std::int32_t successes = 0;
    for (std::int32_t index = 3; index < gLegacyStreamLimit1087AE0; ++index) {
        LegacyFile* const stream = gLegacyStreamTable1086AC4[index];
        if (stream != nullptr) {
            if ((stream->flags & 0x83u) != 0 && CloseLegacyStream_53B1D0(stream) != -1) {
                ++successes;
            }
            if (index >= 20) {
                Free_53C0A9(gLegacyStreamTable1086AC4[index]);
                gLegacyStreamTable1086AC4[index] = nullptr;
            }
        }
    }
    return successes;
}

// 原549147；源地址/FILE借用，按末端U8回推，数量W32，只有AL返回布尔值。
bool UnreadLegacyStreamBytes_549147(const char& firstByte,
    LegacyFile* const stream, const std::uint32_t byteCount)
{
    std::uint32_t remaining = byteCount;
    const char* cursor = &firstByte + remaining;
    while (remaining != 0) {
        const auto character = static_cast<std::uint8_t>(cursor[-1]);
        --cursor;
        if (UnreadLegacyStreamByte_54463F(character, stream) == -1) {
            break;
        }
        --remaining;
    }
    if (remaining == 0) {
        return true;
    }
    std::uint32_t rollbackCount = 1;
    if (remaining < rollbackCount) {
        rollbackCount -= remaining;
        do {
            static_cast<void>(ReadLegacyStreamByte_53C8B3(stream));
            --rollbackCount;
        } while (rollbackCount != 0);
    }
    return false;
}

} // namespace cg::platform
