#include "LegacyLowIo.hpp"
#include "LegacyRuntimeError.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyMemory.hpp"

#include <bit>
#include <cstring>

namespace cg::platform {

LegacyDescriptorRecord* gLegacyDescriptorBlocks10869C0[64] {};
std::uint32_t gLegacyDescriptorCapacity1086AC0 = 0;
std::uint32_t gLegacyDefaultOpenMode1086178 = 0;
std::uint32_t gLegacyPermissionMask1085FE8 = 0;
LegacyDescriptorRecord gLegacyInvalidDescriptor58FB40 {
    INVALID_HANDLE_VALUE, 0, '\n', {0, 0}
};

std::int64_t SeekLegacyDescriptor64_54B658(const std::int32_t descriptor,
    const std::int64_t offset, const std::uint32_t origin)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    auto** const block = &gLegacyDescriptorBlocks10869C0[descriptor >> 5];
    const std::int32_t slot = descriptor & 31;
    if (((*block)[slot].flags & 1u) == 0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const auto bits = static_cast<std::uint64_t>(offset);
    const auto low = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(bits));
    LONG high = std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(bits >> 32));
    const HANDLE handle = GetLegacyDescriptorHandle_543897(descriptor);
    if (handle == INVALID_HANDLE_VALUE) {
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const DWORD positionLow = ::SetFilePointer(handle, low, &high, origin);
    if (positionLow == INVALID_SET_FILE_POINTER) {
        const DWORD error = ::GetLastError();
        if (error != 0) {
            static_cast<void>(MapLegacyFileError_542F44(error));
            return -1;
        }
    }
    (*block)[slot].flags &= static_cast<std::uint8_t>(~2u);
    const std::uint64_t position =
        (static_cast<std::uint64_t>(static_cast<std::uint32_t>(high)) << 32)
        | positionLow;
    return std::bit_cast<std::int64_t>(position);
}

std::uint32_t InitializeLegacyDescriptors_53F52D()
{
    constexpr std::uint32_t kBlockBytes =
        32 * static_cast<std::uint32_t>(sizeof(LegacyDescriptorRecord));
    auto* const initial = static_cast<LegacyDescriptorRecord*>(Malloc_53C3AC(kBlockBytes));
    if (initial == nullptr) {
        ExitLegacyRuntimeError_53DEAB(27);
    }
    gLegacyDescriptorBlocks10869C0[0] = initial;
    gLegacyDescriptorCapacity1086AC0 = 32;
    auto* cursor = initial;
    auto* end = initial + 32;
    while (reinterpret_cast<std::uintptr_t>(cursor) < reinterpret_cast<std::uintptr_t>(end)) {
        cursor->flags = 0;
        cursor->handle = INVALID_HANDLE_VALUE;
        cursor->pipeByte = '\n';
        ++cursor;
        end = gLegacyDescriptorBlocks10869C0[0] + 32;
    }

    STARTUPINFOA information;
    ::GetStartupInfoA(&information);
    if (information.cbReserved2 != 0 && information.lpReserved2 != nullptr) {
        std::int32_t inheritedCount;
        std::memcpy(&inheritedCount, information.lpReserved2, sizeof(inheritedCount));
        const auto* inheritedFlags = information.lpReserved2 + sizeof(inheritedCount);
        const auto* inheritedHandles = inheritedFlags + inheritedCount;
        // 句柄区起点使用原始N；只有随后参与继承/扩容的数量裁到2048。
        if (inheritedCount >= 2048) {
            inheritedCount = 2048;
        }
        std::uint32_t group = 1;
        while (std::bit_cast<std::int32_t>(gLegacyDescriptorCapacity1086AC0) < inheritedCount) {
            auto* const block = static_cast<LegacyDescriptorRecord*>(Malloc_53C3AC(kBlockBytes));
            if (block == nullptr) {
                inheritedCount = std::bit_cast<std::int32_t>(gLegacyDescriptorCapacity1086AC0);
                break;
            }
            gLegacyDescriptorCapacity1086AC0 += 32;
            gLegacyDescriptorBlocks10869C0[group] = block;
            cursor = block;
            end = block + 32;
            while (reinterpret_cast<std::uintptr_t>(cursor) < reinterpret_cast<std::uintptr_t>(end)) {
                cursor->flags = 0;
                cursor->handle = INVALID_HANDLE_VALUE;
                cursor->pipeByte = '\n';
                ++cursor;
                end = gLegacyDescriptorBlocks10869C0[group] + 32;
            }
            ++group;
        }
        for (std::int32_t descriptor = 0; descriptor < inheritedCount; ++descriptor) {
            HANDLE handle;
            std::memcpy(&handle, inheritedHandles, sizeof(handle));
            if (handle != INVALID_HANDLE_VALUE) {
                const std::uint8_t flags = *inheritedFlags;
                if ((flags & 1u) != 0
                    && ((flags & 8u) != 0 || ::GetFileType(handle) != 0)) {
                    auto& record =
                        gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
                    // 原API后重新读取继承区句柄/标志，不能用调用前快照替代。
                    std::memcpy(&record.handle, inheritedHandles, sizeof(record.handle));
                    record.flags = *inheritedFlags;
                }
            }
            ++inheritedFlags;
            inheritedHandles += sizeof(HANDLE);
        }
    }

    for (std::int32_t descriptor = 0; descriptor < 3; ++descriptor) {
        auto& record = gLegacyDescriptorBlocks10869C0[0][descriptor];
        if (record.handle != INVALID_HANDLE_VALUE) {
            record.flags |= 0x80u;
            continue;
        }
        record.flags = 0x81;
        const DWORD standardId = descriptor == 0 ? STD_INPUT_HANDLE
            : (descriptor == 1 ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
        const HANDLE handle = ::GetStdHandle(standardId);
        if (handle == INVALID_HANDLE_VALUE) {
            record.flags |= 0x40u;
            continue;
        }
        const DWORD type = ::GetFileType(handle);
        if (type == 0) {
            record.flags |= 0x40u;
            continue;
        }
        record.handle = handle;
        if ((type & 0xFFu) == 2) {
            record.flags |= 0x40u;
        } else if ((type & 0xFFu) == 3) {
            record.flags |= 8u;
        }
    }
    return ::SetHandleCount(gLegacyDescriptorCapacity1086AC0);
}

std::int32_t SeekLegacyDescriptor_53F493(const std::int32_t descriptor,
    const std::int32_t offset, const std::uint32_t origin)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0
        || (gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags & 1u) == 0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const HANDLE handle = GetLegacyDescriptorHandle_543897(descriptor);
    if (handle == INVALID_HANDLE_VALUE) {
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const DWORD position = ::SetFilePointer(handle, offset, nullptr, origin);
    const DWORD error = position == 0xFFFFFFFFu ? ::GetLastError() : 0;
    if (error != 0) {
        static_cast<void>(MapLegacyFileError_542F44(error));
        return -1;
    }
    gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags &= 0xFDu;
    return std::bit_cast<std::int32_t>(position);
}

std::int32_t WriteLegacyDescriptor_541279(const std::int32_t descriptor,
    const void* const buffer, const std::uint32_t count)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const std::int32_t group = descriptor >> 5;
    const std::int32_t slot = descriptor & 31;
    const std::uint8_t flags = gLegacyDescriptorBlocks10869C0[group][slot].flags;
    if ((flags & 1u) == 0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    if (count == 0) {
        return 0;
    }
    if ((flags & 0x20u) != 0) {
        static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, 0, 2));
    }

    std::uint32_t totalWritten = 0;
    std::uint32_t insertedCarriageReturns = 0;
    DWORD error = 0;
    DWORD written;
    const auto* const source = static_cast<const std::uint8_t*>(buffer);
    if ((gLegacyDescriptorBlocks10869C0[group][slot].flags & 0x80u) != 0) {
        const std::uint8_t* cursor = source;
        std::uint8_t expanded[1028];
        do {
            std::uint32_t used = 0;
            while (static_cast<std::uint32_t>(cursor - source) < count) {
                const std::uint8_t byte = *cursor++;
                if (byte == '\n') {
                    ++insertedCarriageReturns;
                    expanded[used++] = '\r';
                }
                expanded[used++] = byte;
                if (used >= 1024) {
                    break;
                }
            }
            if (::WriteFile(gLegacyDescriptorBlocks10869C0[group][slot].handle,
                    expanded, used, &written, nullptr) == 0) {
                error = ::GetLastError();
                break;
            }
            totalWritten += written;
            // 原541363为jl；成功计数按S32判短写，输出块最大1025字节。
            if (std::bit_cast<std::int32_t>(written) < static_cast<std::int32_t>(used)) {
                break;
            }
        } while (static_cast<std::uint32_t>(cursor - source) < count);
    } else {
        if (::WriteFile(gLegacyDescriptorBlocks10869C0[group][slot].handle,
                buffer, count, &written, nullptr) != 0) {
            totalWritten = written;
        } else {
            error = ::GetLastError();
        }
    }
    if (totalWritten != 0) {
        // 原减全部已生成CR，包含失败/短写块；不按实际写出字节重算或钳制。
        return std::bit_cast<std::int32_t>(totalWritten - insertedCarriageReturns);
    }
    if (error != 0) {
        if (error == 5) {
            LegacyErrno_1085FE0() = 9;
            LegacyDosError_1085FE4() = error;
        } else {
            static_cast<void>(MapLegacyFileError_542F44(error));
        }
        return -1;
    }
    if ((gLegacyDescriptorBlocks10869C0[group][slot].flags & 0x40u) != 0
        && source[0] == 0x1A) {
        return 0;
    }
    LegacyErrno_1085FE0() = 28;
    LegacyDosError_1085FE4() = 0;
    return -1;
}

std::int32_t ReadLegacyDescriptor_541426(const std::int32_t descriptor,
    void* const buffer, std::uint32_t count)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const std::int32_t group = descriptor >> 5;
    const std::int32_t slot = descriptor & 31;
    const std::uint8_t initialFlags = gLegacyDescriptorBlocks10869C0[group][slot].flags;
    if ((initialFlags & 1u) == 0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    if (count == 0 || (initialFlags & 2u) != 0) {
        return 0;
    }
    auto* const bytes = static_cast<std::uint8_t*>(buffer);
    auto* destination = bytes;
    std::uint32_t total = 0;
    if ((initialFlags & 0x48u) != 0) {
        const std::uint8_t pending = gLegacyDescriptorBlocks10869C0[group][slot].pipeByte;
        if (pending != '\n') {
            --count;
            *bytes = pending;
            destination = bytes + 1;
            total = 1;
            gLegacyDescriptorBlocks10869C0[group][slot].pipeByte = '\n';
        }
    }
    DWORD read;
    if (::ReadFile(gLegacyDescriptorBlocks10869C0[group][slot].handle,
            destination, count, &read, nullptr) == 0) {
        const DWORD error = ::GetLastError();
        if (error == 5) {
            LegacyErrno_1085FE0() = 9;
            LegacyDosError_1085FE4() = 5;
        } else if (error == 109) {
            return 0;
        } else {
            static_cast<void>(MapLegacyFileError_542F44(error));
        }
        return -1;
    }
    total += read;
    auto& record = gLegacyDescriptorBlocks10869C0[group][slot];
    const std::uint8_t flags = record.flags;
    if ((flags & 0x80u) == 0) {
        return std::bit_cast<std::int32_t>(total);
    }
    record.flags = read != 0 && bytes[0] == '\n'
        ? static_cast<std::uint8_t>(flags | 4u)
        : static_cast<std::uint8_t>(flags & 0xFBu);

    auto* cursor = bytes;
    auto* output = bytes;
    auto* const end = bytes + total;
    while (cursor < end) {
        const std::uint8_t value = *cursor;
        if (value == 0x1A) {
            auto& current = gLegacyDescriptorBlocks10869C0[group][slot];
            if ((current.flags & 0x40u) == 0) {
                current.flags |= 2u;
            }
            break;
        }
        if (value != '\r') {
            *output++ = value;
            ++cursor;
            continue;
        }
        if (cursor < end - 1) {
            if (cursor[1] == '\n') {
                cursor += 2;
                *output++ = '\n';
            } else {
                *output++ = '\r';
                ++cursor;
            }
            continue;
        }

        ++cursor;
        std::uint8_t lookahead;
        const BOOL succeeded = ::ReadFile(gLegacyDescriptorBlocks10869C0[group][slot].handle,
            &lookahead, 1, &read, nullptr);
        // 原复用read槽，不在peek前清零；失败且LastError==0仍继续检查原槽。
        if ((succeeded == 0 && ::GetLastError() != 0) || read == 0) {
            *output++ = '\r';
            continue;
        }
        if ((gLegacyDescriptorBlocks10869C0[group][slot].flags & 0x48u) != 0) {
            const std::uint8_t pending = lookahead;
            if (pending == '\n') {
                *output++ = '\n';
            } else {
                *output++ = '\r';
                gLegacyDescriptorBlocks10869C0[group][slot].pipeByte = pending;
            }
        } else if (output == bytes && lookahead == '\n') {
            *output++ = '\n';
        } else {
            static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, -1, 1));
            if (lookahead != '\n') {
                *output++ = '\r';
            }
        }
    }
    return static_cast<std::int32_t>(output - bytes);
}

std::int32_t IsLegacyDescriptorTty_544705(const std::int32_t descriptor)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0) {
        return 0;
    }
    return gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags & 0x40u;
}

std::int32_t GetLegacyDescriptorLength_53D3BD(const std::int32_t descriptor)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0
        || (gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags & 1u) == 0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const std::int32_t current = SeekLegacyDescriptor_53F493(descriptor, 0, 1);
    if (current == -1) {
        return -1;
    }
    const std::int32_t end = SeekLegacyDescriptor_53F493(descriptor, 0, 2);
    if (current != end) {
        static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, current, 0));
    }
    return end;
}

std::int32_t AllocateLegacyDescriptor_543711()
{
    for (std::int32_t group = 0; group < 64; ++group) {
        auto* const block = gLegacyDescriptorBlocks10869C0[group];
        if (block != nullptr) {
            for (auto* record = block; record < block + 32; ++record) {
                if ((record->flags & 1u) == 0) {
                    record->handle = INVALID_HANDLE_VALUE;
                    return group * 32 + static_cast<std::int32_t>(
                        record - gLegacyDescriptorBlocks10869C0[group]);
                }
            }
            continue;
        }
        auto* const allocated = static_cast<LegacyDescriptorRecord*>(
            Malloc_53C3AC(32 * static_cast<std::uint32_t>(sizeof(LegacyDescriptorRecord))));
        if (allocated == nullptr) {
            return -1;
        }
        gLegacyDescriptorCapacity1086AC0 += 32;
        gLegacyDescriptorBlocks10869C0[group] = allocated;
        auto* cursor = allocated;
        auto* end = allocated + 32;
        while (cursor < end) {
            cursor->flags = 0;
            cursor->handle = INVALID_HANDLE_VALUE;
            cursor->pipeByte = '\n';
            ++cursor;
            end = gLegacyDescriptorBlocks10869C0[group] + 32;
        }
        return group * 32;
    }
    return -1;
}

std::int32_t SetLegacyDescriptorHandle_5437A6(
    const std::int32_t descriptor, const HANDLE handle)
{
    if (static_cast<std::uint32_t>(descriptor) < gLegacyDescriptorCapacity1086AC0
        && gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].handle
            == INVALID_HANDLE_VALUE) {
        if (gLegacyRuntimeApplicationType58F8A4 == 1) {
            switch (descriptor) {
            case 0: static_cast<void>(::SetStdHandle(STD_INPUT_HANDLE, handle)); break;
            case 1: static_cast<void>(::SetStdHandle(STD_OUTPUT_HANDLE, handle)); break;
            case 2: static_cast<void>(::SetStdHandle(STD_ERROR_HANDLE, handle)); break;
            }
        }
        gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].handle = handle;
        return 0;
    }
    LegacyDosError_1085FE4() = 0;
    LegacyErrno_1085FE0() = 9;
    return -1;
}

std::int32_t SetLegacyDescriptorMode_5463A3(
    const std::int32_t descriptor, const std::uint32_t mode)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0
        || (gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags & 1u) == 0) {
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    auto& record = gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
    const std::uint8_t flags = record.flags;
    if (mode == 0x8000u) {
        record.flags = flags & 0x7Fu;
    } else if (mode == 0x4000u) {
        record.flags = flags | 0x80u;
    } else {
        LegacyErrno_1085FE0() = 22;
        return -1;
    }
    return (flags & 0x80u) != 0 ? 0x4000 : 0x8000;
}

std::int32_t ResizeLegacyDescriptor_5438D4(
    const std::int32_t descriptor, const std::int32_t size)
{
    char zeros[4096];
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0
        || (gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags & 1u) == 0) {
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const std::int32_t originalPosition = SeekLegacyDescriptor_53F493(descriptor, 0, 1);
    if (originalPosition == -1) {
        return -1;
    }
    const std::int32_t end = SeekLegacyDescriptor_53F493(descriptor, 0, 2);
    if (end == -1) {
        return -1;
    }
    std::int32_t result = 0;
    std::int32_t remaining = size - end;
    if (remaining > 0) {
        static_cast<void>(FillLegacyMemory_5419D0(
            zeros, 0, static_cast<std::uint32_t>(sizeof(zeros))));
        const std::int32_t previousMode = SetLegacyDescriptorMode_5463A3(descriptor, 0x8000);
        do {
            const auto count = static_cast<std::uint32_t>(remaining < 4096 ? remaining : 4096);
            const std::int32_t written = WriteLegacyDescriptor_541279(descriptor, zeros, count);
            if (written == -1) {
                if (LegacyDosError_1085FE4() == 5) {
                    LegacyErrno_1085FE0() = 13;
                }
                result = -1;
                break;
            }
            remaining -= written;
        } while (remaining > 0);
        static_cast<void>(SetLegacyDescriptorMode_5463A3(
            descriptor, static_cast<std::uint32_t>(previousMode)));
    } else if (remaining < 0) {
        static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, size, 0));
        const HANDLE handle = GetLegacyDescriptorHandle_543897(descriptor);
        if (::SetEndOfFile(handle) == 0) {
            result = -1;
            LegacyErrno_1085FE0() = 13;
            LegacyDosError_1085FE4() = ::GetLastError();
        }
    }
    static_cast<void>(SeekLegacyDescriptor_53F493(descriptor, originalPosition, 0));
    return result;
}

std::int32_t OpenSharedLegacyDescriptor_53D44C(const char* const filename,
    const std::uint32_t openFlags, const std::uint32_t shareFlags,
    const std::uint32_t permissions)
{
    SECURITY_ATTRIBUTES security;
    security.nLength = sizeof(security);
    security.lpSecurityDescriptor = nullptr;
    std::uint8_t descriptorFlags;
    if ((openFlags & 0x80u) != 0) {
        security.bInheritHandle = FALSE;
        descriptorFlags = 0x10;
    } else {
        descriptorFlags = 0;
        security.bInheritHandle = TRUE;
    }
    if ((openFlags & 0x8000u) == 0
        && ((openFlags & 0x4000u) != 0 || gLegacyDefaultOpenMode1086178 != 0x8000u)) {
        descriptorFlags |= 0x80u;
    }
    DWORD access;
    switch (openFlags & 3u) {
    case 0: access = GENERIC_READ; break;
    case 1: access = GENERIC_WRITE; break;
    case 2: access = GENERIC_READ | GENERIC_WRITE; break;
    default:
        LegacyErrno_1085FE0() = 22;
        LegacyDosError_1085FE4() = 0;
        return -1;
    }
    DWORD sharing;
    switch (shareFlags) {
    case 0x10: sharing = 0; break;
    case 0x20: sharing = FILE_SHARE_READ; break;
    case 0x30: sharing = FILE_SHARE_WRITE; break;
    case 0x40: sharing = FILE_SHARE_READ | FILE_SHARE_WRITE; break;
    default:
        LegacyErrno_1085FE0() = 22;
        LegacyDosError_1085FE4() = 0;
        return -1;
    }
    DWORD creation;
    switch (openFlags & 0x700u) {
    case 0:
    case 0x400: creation = OPEN_EXISTING; break;
    case 0x100: creation = OPEN_ALWAYS; break;
    case 0x200:
    case 0x600: creation = TRUNCATE_EXISTING; break;
    case 0x300: creation = CREATE_ALWAYS; break;
    case 0x500:
    case 0x700: creation = CREATE_NEW; break;
    default:
        LegacyErrno_1085FE0() = 22;
        LegacyDosError_1085FE4() = 0;
        return -1;
    }
    DWORD attributes = FILE_ATTRIBUTE_NORMAL;
    if ((openFlags & 0x100u) != 0
        && ((~gLegacyPermissionMask1085FE8 & permissions) & 0x80u) == 0) {
        attributes = FILE_ATTRIBUTE_READONLY;
    }
    if ((openFlags & 0x40u) != 0) {
        attributes |= FILE_FLAG_DELETE_ON_CLOSE;
        access |= DELETE;
    }
    if ((openFlags & 0x1000u) != 0) {
        attributes |= FILE_ATTRIBUTE_TEMPORARY;
    }
    if ((openFlags & 0x20u) != 0) {
        attributes |= FILE_FLAG_SEQUENTIAL_SCAN;
    } else if ((openFlags & 0x10u) != 0) {
        attributes |= FILE_FLAG_RANDOM_ACCESS;
    }
    const std::int32_t descriptor = AllocateLegacyDescriptor_543711();
    if (descriptor == -1) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 24;
        return -1;
    }
    const HANDLE handle = ::CreateFileA(
        filename, access, sharing, &security, creation, attributes, nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        static_cast<void>(MapLegacyFileError_542F44(::GetLastError()));
        return -1;
    }
    const DWORD type = ::GetFileType(handle);
    if (type == 0) {
        static_cast<void>(::CloseHandle(handle));
        static_cast<void>(MapLegacyFileError_542F44(::GetLastError()));
        return -1;
    }
    if (type == FILE_TYPE_CHAR) {
        descriptorFlags |= 0x40u;
    } else if (type == FILE_TYPE_PIPE) {
        descriptorFlags |= 8u;
    }
    static_cast<void>(SetLegacyDescriptorHandle_5437A6(descriptor, handle));
    descriptorFlags |= 1u;
    const std::uint8_t deviceOrPipe = descriptorFlags & 0x48u;
    gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags = descriptorFlags;

    if (deviceOrPipe == 0 && (descriptorFlags & 0x80u) != 0 && (openFlags & 2u) != 0) {
        const std::int32_t lastPosition = SeekLegacyDescriptor_53F493(descriptor, -1, 2);
        if (lastPosition == -1) {
            if (LegacyDosError_1085FE4() != 131) {
                static_cast<void>(CloseLegacyDescriptor_53D30A(descriptor));
                return -1;
            }
        } else {
            std::uint8_t lastByte = 0;
            const std::int32_t read = ReadLegacyDescriptor_541426(descriptor, &lastByte, 1);
            if (read == 0 && lastByte == 0x1A
                && ResizeLegacyDescriptor_5438D4(descriptor, lastPosition) == -1) {
                static_cast<void>(CloseLegacyDescriptor_53D30A(descriptor));
                return -1;
            }
            if (SeekLegacyDescriptor_53F493(descriptor, 0, 0) == -1) {
                static_cast<void>(CloseLegacyDescriptor_53D30A(descriptor));
                return -1;
            }
        }
    }
    if (deviceOrPipe == 0 && (openFlags & 8u) != 0) {
        gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags |= 0x20u;
    }
    return descriptor;
}

std::int32_t OpenLegacyDescriptor_53D435(const char* const filename,
    const std::uint32_t flags, const std::uint32_t permissions)
{
    return OpenSharedLegacyDescriptor_53D44C(filename, flags, 0x40, permissions);
}

HANDLE GetLegacyDescriptorHandle_543897(const std::int32_t descriptor)
{
    if (static_cast<std::uint32_t>(descriptor) < gLegacyDescriptorCapacity1086AC0) {
        auto& record = gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
        if ((record.flags & 1u) != 0) {
            return record.handle;
        }
    }
    LegacyDosError_1085FE4() = 0;
    LegacyErrno_1085FE0() = 9;
    return INVALID_HANDLE_VALUE;
}

std::int32_t ReleaseLegacyDescriptorHandle_54381D(const std::int32_t descriptor)
{
    if (static_cast<std::uint32_t>(descriptor) < gLegacyDescriptorCapacity1086AC0) {
        const auto& record = gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
        if ((record.flags & 1u) != 0 && record.handle != INVALID_HANDLE_VALUE) {
            if (gLegacyRuntimeApplicationType58F8A4 == 1) {
                switch (descriptor) {
                case 0: static_cast<void>(::SetStdHandle(STD_INPUT_HANDLE, nullptr)); break;
                case 1: static_cast<void>(::SetStdHandle(STD_OUTPUT_HANDLE, nullptr)); break;
                case 2: static_cast<void>(::SetStdHandle(STD_ERROR_HANDLE, nullptr)); break;
                }
            }
            // 原543876在系统API之后重新读取组指针。
            gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].handle =
                INVALID_HANDLE_VALUE;
            return 0;
        }
    }
    LegacyDosError_1085FE4() = 0;
    LegacyErrno_1085FE0() = 9;
    return -1;
}

std::int32_t CommitLegacyDescriptor_541222(const std::int32_t descriptor)
{
    if (static_cast<std::uint32_t>(descriptor) < gLegacyDescriptorCapacity1086AC0
        && (gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags & 1u) != 0) {
        const HANDLE handle = GetLegacyDescriptorHandle_543897(descriptor);
        const DWORD error = ::FlushFileBuffers(handle) != 0 ? 0 : ::GetLastError();
        if (error == 0) {
            return 0;
        }
        LegacyDosError_1085FE4() = error;
    }
    // 原非法描述符路径保留此前DOS错误，不与get/free的清零路径合并。
    LegacyErrno_1085FE0() = 9;
    return -1;
}

std::int32_t CloseLegacyDescriptor_53D30A(const std::int32_t descriptor)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0
        || (gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags & 1u) == 0) {
        LegacyDosError_1085FE4() = 0;
        LegacyErrno_1085FE0() = 9;
        return -1;
    }

    DWORD error = 0;
    if (GetLegacyDescriptorHandle_543897(descriptor) != INVALID_HANDLE_VALUE) {
        bool sharedStandardHandle = false;
        if (descriptor == 1 || descriptor == 2) {
            const HANDLE standardError = GetLegacyDescriptorHandle_543897(2);
            const HANDLE standardOutput = GetLegacyDescriptorHandle_543897(1);
            sharedStandardHandle = standardOutput == standardError;
        }
        if (!sharedStandardHandle) {
            const HANDLE handle = GetLegacyDescriptorHandle_543897(descriptor);
            if (::CloseHandle(handle) == 0) {
                error = ::GetLastError();
            }
        }
    }
    static_cast<void>(ReleaseLegacyDescriptorHandle_54381D(descriptor));
    gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags = 0;
    if (error != 0) {
        static_cast<void>(MapLegacyFileError_542F44(error));
        return -1;
    }
    return 0;
}

} // namespace cg::platform
