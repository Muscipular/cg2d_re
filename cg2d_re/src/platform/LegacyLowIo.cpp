#include "LegacyLowIo.hpp"

#include <io.h>
#include <fcntl.h>

namespace cg::platform {

std::int32_t OpenLegacyDescriptor_53D435(const char* const filename,
    const std::uint32_t flags, const std::uint32_t permissions)
{
    return ::_open(filename, static_cast<int>(flags), static_cast<int>(permissions));
}

std::int32_t GetLegacyDescriptorLength_53D3BD(const std::int32_t descriptor)
{
    return static_cast<std::int32_t>(::_filelength(descriptor));
}

std::int32_t CloseLegacyDescriptor_53D30A(const std::int32_t descriptor)
{
    return ::_close(descriptor);
}

} // namespace cg::platform
