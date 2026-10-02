#include "LegacyLowIo.hpp"
#include "LegacyRuntimeError.hpp"
#include "cg/platform/LegacyTime.hpp"

namespace cg::platform {

LegacyDescriptorRecord* gLegacyDescriptorBlocks10869C0[64] {};
std::uint32_t gLegacyDescriptorCapacity1086AC0 = 0;

HANDLE GetLegacyDescriptorHandle_543897(const std::int32_t descriptor)
{
    if (static_cast<std::uint32_t>(descriptor) < gLegacyDescriptorCapacity1086AC0) {
        auto& record = gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31];
        if ((record.flags & 1u) != 0) {
            return record.handle;
        }
    }
    gLegacyDosError_1085FE4 = 0;
    gLegacyErrno_1085FE0 = 9;
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
    gLegacyDosError_1085FE4 = 0;
    gLegacyErrno_1085FE0 = 9;
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
        gLegacyDosError_1085FE4 = error;
    }
    // 原非法描述符路径保留此前DOS错误，不与get/free的清零路径合并。
    gLegacyErrno_1085FE0 = 9;
    return -1;
}

std::int32_t CloseLegacyDescriptor_53D30A(const std::int32_t descriptor)
{
    if (static_cast<std::uint32_t>(descriptor) >= gLegacyDescriptorCapacity1086AC0
        || (gLegacyDescriptorBlocks10869C0[descriptor >> 5][descriptor & 31].flags & 1u) == 0) {
        gLegacyDosError_1085FE4 = 0;
        gLegacyErrno_1085FE0 = 9;
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
