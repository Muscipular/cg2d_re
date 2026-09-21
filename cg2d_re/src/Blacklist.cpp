#include "cg/Blacklist.hpp"

#include <cstdio>
#include <cstring>

namespace cg::blacklist {

namespace {

std::int8_t PersistBlacklistFile_4011A0(const std::int32_t fileId)
{
    PathBuffer localPath {};
    std::sprintf(localPath.data(), "data\\blacklist_%d.txt", static_cast<int>(fileId));

    if (std::strcmp(gPath.data(), localPath.data()) != 0) {
        return 0;
    }

    std::FILE* const file = std::fopen(gPath.data(), "w");
    for (const Record& record : gRecords) {
        if (record[0] == 0) {
            break;
        }

        std::fputs(reinterpret_cast<const char*>(record.data()), file);
        std::fputs("\n", file);
    }

    std::fclose(file);
    return std::int8_t {1};
}

} // namespace

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
