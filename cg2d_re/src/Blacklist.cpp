#include "cg/Blacklist.hpp"

#include <bit>
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

std::int8_t LoadBlacklistFile(const std::int32_t fileId)
{
    std::memset(gPath.data(), 0, sizeof(gPath));
    std::memset(gRecords.data(), 0, sizeof(gRecords));
    std::sprintf(gPath.data(), "data\\blacklist_%d.txt", static_cast<int>(fileId));

    std::FILE* const file = std::fopen(gPath.data(), "rb+");
    if (file != nullptr) {
        std::rewind(file);
        if (std::feof(file) == 0) {
            for (;;) {
                std::fgets(
                    reinterpret_cast<char*>(
                        gRecords[static_cast<std::size_t>(gCursor)].data()),
                    static_cast<int>(kRecordSize),
                    file);

                const std::int32_t cursor = gCursor;
                Record& record = gRecords[static_cast<std::size_t>(cursor)];
                if (record[0] == 0
                    || record[0] == '\n'
                    || (record[0] == '\r' && record[1] == '\n')) {
                    const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(cursor);
                    gCursor = std::bit_cast<std::int32_t>(cursorBits - std::uint32_t {1});
                    std::memset(record.data(), 0, sizeof(record));
                } else {
                    for (std::size_t i = 0; i < kRecordSize; ++i) {
                        if (record[i] == 0) {
                            break;
                        }
                        if (record[i] == '\r' && record[i + 1] == '\n') {
                            record[i] = 0;
                            record[i + 1] = 0;
                            break;
                        }
                        if (record[i] == '\n') {
                            record[i] = 0;
                        }
                    }

                    const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(cursor);
                    gCursor = std::bit_cast<std::int32_t>(cursorBits + std::uint32_t {1});
                    if (gCursor == static_cast<std::int32_t>(kRecordCount)) {
                        break;
                    }
                }

                if (std::feof(file) != 0) {
                    break;
                }
            }
        }
        std::fclose(file);
    }

    gCurrentFileId = fileId;
    return std::int8_t {1};
}

std::int8_t PersistAndReset_401000(const std::int32_t fileId)
{
    if (PersistBlacklistFile_4011A0(fileId) == 0) {
        return 0;
    }

    gCurrentFileId = -1;
    gCursor = 0;
    return 1;
}

std::int8_t AddBlacklistRecord_401270(const char* const name)
{
    if (gCursor == static_cast<std::int32_t>(kRecordCount)) {
        return 0;
    }

    for (Record& record : gRecords) {
        if (record[0] != 0) {
            continue;
        }

        std::size_t byteCount = 0;
        do {
            ++byteCount;
        } while (name[byteCount - 1] != '\0');

        std::uint8_t* const destination = record.data();
        for (std::size_t i = 0; i < byteCount; ++i) {
            destination[i] = static_cast<std::uint8_t>(name[i]);
        }

        const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(gCursor);
        gCursor = std::bit_cast<std::int32_t>(cursorBits + std::uint32_t {1});
        break;
    }

    const std::int32_t fileId = gCurrentFileId;
    if (fileId >= 0) {
        PersistBlacklistFile_4011A0(fileId);
    }
    return 1;
}

} // namespace cg::blacklist
