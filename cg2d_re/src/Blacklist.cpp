#include "cg/Blacklist.hpp"
#include "cg/Input.hpp"
#include "platform/LegacyFormat.hpp"
#include "platform/LegacyStdio.hpp"

#include <bit>
#include <cstring>

namespace cg::blacklist {

namespace {

bool EqualPairedBytes(
    const volatile std::uint8_t* left,
    const volatile std::uint8_t* right)
{
    for (;;) {
        const std::uint8_t firstLeft = left[0];
        const std::uint8_t firstRight = right[0];
        if (firstLeft != firstRight) {
            return false;
        }
        if (firstLeft == 0) {
            return true;
        }

        const std::uint8_t secondLeft = left[1];
        const std::uint8_t secondRight = right[1];
        if (secondLeft != secondRight) {
            return false;
        }
        left += 2;
        right += 2;
        if (secondLeft == 0) {
            return true;
        }
    }
}

// 原函数：0x004011A0。
std::int8_t PersistBlacklistFile_4011A0(const std::int32_t fileId)
{
    PathBuffer localPath {};
    platform::FormatLegacyString_53B304(localPath.data(), "data\\blacklist_%d.txt", static_cast<int>(fileId));

    if (std::strcmp(gPath.data(), localPath.data()) != 0) {
        return 0;
    }

    platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1(gPath.data(), "w");
    for (const Record& record : gRecords) {
        if (record[0] == 0) {
            break;
        }

        platform::WriteLegacyStreamString_53B356(reinterpret_cast<const char*>(record.data()), file);
        platform::WriteLegacyStreamString_53B356("\n", file);
    }

    platform::CloseLegacyStream_53B1D0(file);
    return std::int8_t {1};
}

} // namespace

RecordTable gRecords {}; // 原全局：0x00592FA8。
PathBuffer gPath {}; // 原全局：0x0059364C。
std::int32_t gCursor = 0; // 原全局：0x0059384C。
std::int32_t gCurrentFileId = -1; // 原全局：0x005531A0。

static_assert(sizeof(Record) == 17);
static_assert(sizeof(RecordTable) == 0x6A4);
static_assert(sizeof(PathBuffer) == 0x200);

// 原函数：0x00401170。
char* BlacklistRecordAt_401170(const std::int32_t index) noexcept
{
    return reinterpret_cast<char*>(
        gRecords[static_cast<std::size_t>(index)].data());
}

// 原函数：0x00401030。
std::int8_t LoadBlacklistFile(const std::int32_t fileId)
{
    std::memset(gPath.data(), 0, sizeof(gPath));
    std::memset(gRecords.data(), 0, sizeof(gRecords));
    platform::FormatLegacyString_53B304(gPath.data(), "data\\blacklist_%d.txt", static_cast<int>(fileId));

    platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1(gPath.data(), "rb+");
    if (file != nullptr) {
        platform::RewindLegacyStream_53B27D(file);
        if (std::feof(file) == 0) {
            for (;;) {
                platform::ReadLegacyStreamLine_53B226(
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
        platform::CloseLegacyStream_53B1D0(file);
    }

    gCurrentFileId = fileId;
    return std::int8_t {1};
}

// 原函数：0x00401000。
std::int8_t PersistAndReset_401000(const std::int32_t fileId)
{
    if (PersistBlacklistFile_4011A0(fileId) == 0) {
        return 0;
    }

    gCurrentFileId = -1;
    gCursor = 0;
    return 1;
}

// 原 sub_401190@0x401190：直接返回黑名单记录游标的完整有符号 32 位值。
std::int32_t GetBlacklistCount_401190()
{
    return gCursor;
}

// 原函数：0x004013D0。
std::uint8_t ContainsBlacklistRecord_4013D0(const char* const name)
{
    const auto* const inputBytes =
        reinterpret_cast<const volatile std::uint8_t*>(name);
    for (const Record& record : gRecords) {
        if (EqualPairedBytes(
                inputBytes,
                reinterpret_cast<const volatile std::uint8_t*>(
                    input::LegacyText593850()))) {
            continue;
        }
        if (EqualPairedBytes(record.data(), inputBytes)) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x00401270。
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
        const std::size_t wordCopyByteCount = byteCount & ~std::size_t {3};
        for (std::size_t i = 0; i < wordCopyByteCount; ++i) {
            destination[i] = static_cast<std::uint8_t>(name[i]);
        }

        // 原4012BF在四字节分组复制后取游标，余数字节复制后才发布。
        const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(gCursor);
        const std::int32_t nextCursor =
            std::bit_cast<std::int32_t>(cursorBits + std::uint32_t {1});
        for (std::size_t i = wordCopyByteCount; i < byteCount; ++i) {
            destination[i] = static_cast<std::uint8_t>(name[i]);
        }
        gCursor = nextCursor;
        break;
    }

    const std::int32_t fileId = gCurrentFileId;
    if (fileId >= 0) {
        PersistBlacklistFile_4011A0(fileId);
    }
    return 1;
}

// 原函数：0x004012F0。
std::int8_t RemoveBlacklistRecord_4012F0(const char* const name)
{
    const auto* const inputBytes =
        reinterpret_cast<const volatile std::uint8_t*>(name);
    std::size_t i = 0;
    for (; i < kRecordCount; ++i) {
        if (EqualPairedBytes(gRecords[i].data(), inputBytes)) {
            break;
        }
    }

    if (i >= kRecordCount) {
        return 0;
    }

    const std::int32_t oldCursor = gCursor;
    while (static_cast<std::int32_t>(i) != oldCursor) {
        // 原记录表尾的下一槽起点是共享路径起点。
        const std::uint8_t* const source = i + 1 == kRecordCount
            ? reinterpret_cast<const std::uint8_t*>(gPath.data())
            : gRecords[i + 1].data();
        std::size_t byteCount = 0;
        do {
            ++byteCount;
        } while (source[byteCount - 1] != 0);

        std::uint8_t* const destination = gRecords[i].data();
        for (std::size_t byteIndex = 0; byteIndex < byteCount; ++byteIndex) {
            destination[byteIndex] = source[byteIndex];
        }

        ++i;
        if (i >= kRecordCount) {
            return 0;
        }
    }

    const std::uint32_t cursorBits = std::bit_cast<std::uint32_t>(oldCursor);
    gCursor = std::bit_cast<std::int32_t>(cursorBits - std::uint32_t {1});
    std::memset(gRecords[i].data(), 0, kRecordSize);

    const std::int32_t fileId = gCurrentFileId;
    if (fileId >= 0) {
        PersistBlacklistFile_4011A0(fileId);
    }
    return 1;
}

// 原函数：0x00401470。
std::int8_t ClearBlacklistRecords_401470()
{
    if (gRecords[0][0] == 0 || gCursor == 0) {
        return 0;
    }
    std::memset(gRecords.data(), 0, sizeof(gRecords));
    gCursor = 0;
    const std::int32_t fileId = gCurrentFileId;
    if (fileId >= 0) {
        static_cast<void>(PersistBlacklistFile_4011A0(fileId));
    }
    return 1;
}

} // namespace cg::blacklist
