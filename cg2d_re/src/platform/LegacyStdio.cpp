#include "LegacyStdio.hpp"
#include "LegacyLowIo.hpp"
#include "LegacyRuntimeError.hpp"
#include "cg/platform/LegacyMemory.hpp"

namespace cg::platform {

// 原53BFF8，原calloc依赖尚未实现；不擅自扩大Malloc的std替代范围。
void* CallocateLegacyMemory_53BFF8(std::uint32_t count, std::uint32_t size);

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
        CallocateLegacyMemory_53BFF8(static_cast<std::uint32_t>(limit),
            static_cast<std::uint32_t>(sizeof(LegacyFile*))));
    if (gLegacyStreamTable1086AC4 == nullptr) {
        gLegacyStreamLimit1087AE0 = 20;
        gLegacyStreamTable1086AC4 = static_cast<LegacyFile**>(
            CallocateLegacyMemory_53BFF8(20,
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

} // namespace cg::platform
