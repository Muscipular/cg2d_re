#include "cg/Blacklist.hpp"

namespace cg::blacklist {

// sub_4011A0@0x4011A0 尚未还原；保留真实外部依赖，不提供桩。
std::int8_t PersistBlacklistFile_4011A0(std::int32_t fileId);

RecordTable gRecords {};
PathBuffer gPath {};
std::int32_t gCursor = 0;
std::int32_t gCurrentFileId = -1;

static_assert(sizeof(Record) == 17);
static_assert(sizeof(RecordTable) == 0x6A4);
static_assert(sizeof(PathBuffer) == 0x200);

std::int8_t PersistAndReset_401000(const std::int32_t fileId)
{
    if (PersistBlacklistFile_4011A0(fileId) == 0) {
        return 0;
    }

    gCurrentFileId = -1;
    gCursor = 0;
    return 1;
}

} // namespace cg::blacklist
